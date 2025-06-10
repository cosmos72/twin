/*
 *  dm.c  --  twin version of 'xdm'
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>

#include <Tw/Tw.h>
#include <Tw/Twkeys.h>
#include <Tw/Twerrno.h>
#include <Tutf/Tutf.h>

#include "version.h"

#ifdef TW_HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef TW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef TW_HAVE_CRYPT_H
#include <crypt.h>
#endif
#ifdef TW_HAVE_SHADOW_H
#include <shadow.h>
#endif
#ifdef TW_HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#ifdef TW_HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef TW_HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif

#if !defined(TW_HAVE_WAIT3) && !defined(TW_HAVE_WAIT4)
#error both wait3() and wait4() missing! Cannot compile twdm.
#endif

#ifdef BINDIR
#define BINDIR_PREFIX BINDIR "/"
#else
#define BINDIR_PREFIX "../server/"
#endif

#define DM_GADGET_LOGIN 1
#define DM_GADGET_CLEAR 2
#define DM_GADGET_CONSOLE 3

#define DM_AUTO 0
#define DM_ATTACH 1
#define DM_DISPLAY 2

static tmsgport DM_MsgPort;
static tmenu DM_Menu;
static tscreen DM_Screen;
static twindow DM_Window, DM_user, DM_pass;
static const char *DM_Display;
static byte DM_Kill, quiet, oneshot, logged_in;

static TW_VOLATILE pid_t ServerPid = (pid_t)-1, AttachPid = (pid_t)-1;

static char *const *Args;
static const char *hw_name = "--hw=tty", *title, *TwEnvRC = NULL;
static byte use_twdisplay = DM_AUTO, Privileges = TW_PRIV_NONE;

typedef struct s_data {
  char txt[30];
  byte len;
  byte x;
} *data;

static struct s_data user, pass;

static void Usage(void) {
  fprintf(stderr, "Usage: twdm [OPTIONS]\n"
                  "Currently known options: \n"
                  " -h, --help              display this help and exit\n"
                  " -V, --version           output version information and exit\n"
                  " -k, --kill              kill twin server upon display detach\n"
                  " -q, --quiet             quiet; suppress diagnostic messages\n"
                  " -1, --one-shot          start at most one session, then exit\n"
                  " --attach                use \"twattach\" to start display\n"
                  " --display               use \"twdisplay\" to start display\n"
                  "                             (default unless --hw=tty)\n"
                  " --envrc                 tell twin to run twenvrc.sh to get environment\n"
                  " --suidroot              tell twin to keep suid root privileges\n"
                  " --sgidtty               tell twin to keep sgid tty privileges\n"
                  " --title=<title>         set window title\n"
                  " --hw=<arg>              set display hw to use (default: --hw=tty)\n");
}

static void ShowVersion(void) {
  fprintf(stdout, "twdm " TWIN_VERSION_STR "\n");
}

static void ParseArgs(void) {
  const char *s;
  while ((s = *Args)) {
    if (!strcmp(s, "-h") || !Tw_option_strcmp(s, "-help")) {
      Usage();
      exit(0);
    } else if (!strcmp(s, "-V") || !Tw_option_strcmp(s, "-version")) {
      ShowVersion();
      exit(0);
    } else if (!strcmp(s, "-k") || !Tw_option_strcmp(s, "-kill")) {
      DM_Kill = ttrue;
    } else if (!strcmp(s, "-q") || !Tw_option_strcmp(s, "-quiet")) {
      quiet = ttrue;
    } else if (!strcmp(s, "-1") || !Tw_option_strcmp(s, "-one-shot")) {
      oneshot = ttrue;
    } else if (!Tw_option_strcmp(s, "-attach")) {
      use_twdisplay = DM_ATTACH;
    } else if (!Tw_option_strcmp(s, "-display")) {
      use_twdisplay = DM_DISPLAY;
    } else if (!Tw_option_strcmp(s, "-envrc")) {
      TwEnvRC = "--envrc";
    } else if (!Tw_option_strcmp(s, "-suidroot")) {
      Privileges = TW_PRIV_SUIDROOT;
    } else if (!Tw_option_strcmp(s, "-sgidtty")) {
      Privileges = TW_PRIV_SGIDTTY;
    } else if (!Tw_option_strncmp(s, "-title=", 7)) {
      title = s + 7;
    } else if (!Tw_option_strncmp(s, "-hw=", 4)) {
      hw_name = s;
    } else if (!strcmp(s, "--")) {
      /* '--' means end of options */
      break;
    } else {
      fprintf(stderr,
              "twdm: unknown option: `%s'\n"
              "\ttry `twdm --help' for usage summary.\n",
              s);
      exit(1);
    }
    Args++;
  }
}

static void SetHOME(void) {
#if defined(TW_HAVE_SETENV) || defined(TW_HAVE_PUTENV)
  char *HOME = getenv("HOME");
  struct stat buf;
  if ((!HOME || !strcmp(HOME, "/")) &&
#ifdef TW_HAVE_LSTAT
      lstat("/root", &buf) >= 0 &&
#else
      stat("/root", &buf) >= 0 &&
#endif
      S_ISDIR(buf.st_mode)) {

#ifdef TW_HAVE_SETENV
    setenv("HOME", "/root", 1);
#else
    putenv("HOME=/root");
#endif
  }
#endif /* defined(TW_HAVE_SETENV) || defined(TW_HAVE_PUTENV) */
}

static byte InitServer(void) {
  int i, fd[2];
  static char buff[80];

  if (pipe(fd) >= 0)
    switch (fork()) {
    case 0:
      /* child */
      if (fd[1] != 1) {
        close(1);
        dup2(fd[1], 1);
        close(fd[1]);
      }
      if (fd[0] != 1)
        close(fd[0]);

      execl(BINDIR_PREFIX "twin", "twin", "--secure", "--nohw", TwEnvRC, NULL);
      execlp("twin", "twin", "--secure", "--nohw", TwEnvRC, NULL);
      fprintf(stderr, "twdm: exec(twin) failed: %s\n", strerror(errno));
      exit(1);
      return tfalse;
    case (pid_t)-1:
      /* error */
      close(fd[0]);
      close(fd[1]);
      fprintf(stderr, "twdm: fork() failed: %s\n", strerror(errno));
      return tfalse;
    default:
      /* parent */
      close(fd[1]);
      do {
        i = read(fd[0], buff, 79);
      } while (i < 0 && errno == EINTR);
      close(fd[0]);
      if (i > 33 && !memcmp(buff, "twin: starting in background as :", 33)) {
        while (buff[--i] == '\n')
          ;
        buff[++i] = '\0';
        DM_Display = buff + (i = 32);
        while (buff[i] && buff[i] != ' ')
          i++;
        if (buff[i]) {
          buff[i++] = '\0';
          if (!strncmp(buff + i, "(pid ", 5))
            ServerPid = atoi(buff + i + 5);
        }
        return ttrue;
      }

      if (i <= 0)
        fprintf(stderr, "twdm: read() from twin failed: %s\n", strerror(errno));
      else
        fprintf(stderr, "twdm: error starting twin:\n\t%.*s\n", i, buff);

      return tfalse;
    }
  fprintf(stderr, "twdm: pipe() failed: %s\n", strerror(errno));
  return tfalse;
}

static void shortsleep(void) {
  struct timeval tv = {0, 100000};
  select(0, NULL, NULL, NULL, &tv);
}

static byte InitAttach(void) {
  char *attach, *path_attach;
  char buff[] = "--twin@:\0\0\0";

  if (use_twdisplay == DM_ATTACH ||
      (use_twdisplay == DM_AUTO && !Tw_option_strcmp(hw_name, "-hw=tty"))) {

    attach = "twattach";
    path_attach = BINDIR_PREFIX "twattach";
  } else {
    attach = "twdisplay";
    path_attach = BINDIR_PREFIX "twdisplay";
  }

  strncpy(buff + 7, DM_Display, 4);

  /* sleep a little to let server start up */
  shortsleep();

  switch ((AttachPid = fork())) {
  case 0:
    /* child */
    execl(path_attach, attach, "--quiet", buff, hw_name, NULL);
    execlp(attach, attach, "--quiet", buff, hw_name, NULL);
    fprintf(stderr, "twdm: exec(%s) failed: %s\n", attach, strerror(errno));
    exit(1);
    return tfalse;
  case (pid_t)-1:
    /* error */
    fprintf(stderr, "twdm: fork() failed: %s\n", strerror(errno));
    return tfalse;
  default:
    /* parent */
    break;
  }

  /* sleep a little to let twdisplay start up */
  shortsleep();

  return ttrue;
}

TW_DECL_MAGIC(DM_magic);

static byte InitClient(void) {
  dat X, Y;
  char *tbuf;

  if (!TwCheckMagic(DM_magic) || !TwOpen(DM_Display))
    return tfalse;

  DM_Screen = TwFirstScreen();
  X = TwGetDisplayWidth();
  Y = TwGetDisplayHeight();

  if (!title) {
    struct utsname buf;
    if (uname(&buf) >= 0 && (tbuf = (char *)TwAllocMem(12 + strlen(buf.nodename)))) {
      sprintf(tbuf, "Welcome at %s", buf.nodename);
      title = tbuf;
    } else
      title = "Login Manager";
  }

  if ((DM_MsgPort = TwCreateMsgPort(2, "DM")) &&
      (DM_Menu =
           TwCreateMenu(TCOL(tblack, twhite), TCOL(tblack, tgreen), TCOL(tBLACK, twhite),
                        TCOL(tBLACK, tblack), TCOL(tred, twhite), TCOL(tred, tgreen), (byte)0)) &&
      TwItem4MenuCommon(DM_Menu) &&
      (TwInfo4Menu(DM_Menu, TW_ROW_ACTIVE, 61,
                   " Twin Display Manager. Enter user name and password to login.", NULL),
       ttrue) &&

      (DM_Window =
           TwCreateWindow(strlen(title), title, NULL, DM_Menu, TCOL(twhite, tblue), TW_NOCURSOR,
                          TW_WINDOW_DRAG | TW_WINDOW_WANT_KEYS, TW_WINDOWFL_USEROWS, 40, 15, 0)) &&
      (DM_user = TwCreateWindow(
           6, "Login:", NULL, DM_Menu, TCOL(tblack, tcyan), TW_LINECURSOR, TW_WINDOW_WANT_KEYS,
           TW_WINDOWFL_CURSOR_ON | TW_WINDOWFL_USEROWS | TW_WINDOWFL_ROWS_DEFCOL, 30, 1, 0)) &&
      (DM_pass = TwCreateWindow(
           9, "Password:", NULL, DM_Menu, TCOL(tblack, tcyan), TW_LINECURSOR, TW_WINDOW_WANT_KEYS,
           TW_WINDOWFL_CURSOR_ON | TW_WINDOWFL_USEROWS | TW_WINDOWFL_ROWS_DEFCOL, 30, 1, 0)) &&

      TwCreateButtonGadget(DM_Window, 7, 1, " Login ", TW_GADGETFL_USETEXT, DM_GADGET_LOGIN,
                           TCOL(twhite, tblue), TCOL(tWHITE, tgreen), TCOL(tBLACK, tgreen), 5,
                           12) &&
      TwCreateButtonGadget(DM_Window, 7, 1, " Clear ", TW_GADGETFL_USETEXT, DM_GADGET_CLEAR,
                           TCOL(twhite, tblue), TCOL(tWHITE, tgreen), TCOL(tBLACK, tgreen), 16,
                           12) &&
      TwCreateButtonGadget(DM_Window, 7, 1, " Leave ", TW_GADGETFL_USETEXT, DM_GADGET_CONSOLE,
                           TCOL(twhite, tblue), TCOL(tWHITE, tgreen), TCOL(tBLACK, tgreen), 27, 12)

  ) {
    TwSetColTextWindow(DM_Window, TCOL(tRED, tblue));
    TwGotoXYWindow(DM_Window, 7, 8);
    TwWriteCharsetWindow(DM_Window, 26, "L O G I N   F A I L E D  !");

    TwSetXYWindow(DM_Window, (X - 40) / 2, (Y - 15) / 2);
    TwSetXYWindow(DM_user, 4, 2);
    TwSetXYWindow(DM_pass, 4, 7);

    TwMapWindow(DM_Window, DM_Screen);
    TwMapWindow(DM_user, DM_Window);
    TwMapWindow(DM_pass, DM_Window);

    TwFocusSubWidget(DM_user);

    return ttrue;
  }
  return tfalse;
}

static void ClearKey(void) {
  TwGotoXYWindow(DM_user, 0, 0);
  memset(user.txt, ' ', user.len);
  TwWriteCharsetWindow(DM_user, user.len, user.txt);
  TwGotoXYWindow(DM_user, user.len = user.x = 0, 0);
  user.txt[0] = '\0';

  TwGotoXYWindow(DM_pass, 0, 0);
  memset(pass.txt, ' ', pass.len);
  TwWriteCharsetWindow(DM_pass, pass.len, pass.txt);
  TwGotoXYWindow(DM_pass, pass.len = pass.x = 0, 0);
  pass.txt[0] = '\0';
}

static void kill_server(void) {
  if (ServerPid != (pid_t)-1) {
    kill(ServerPid, SIGTERM);
    kill(ServerPid, SIGCONT);
  }
}

static void kill_attach(void) {
  if (AttachPid != (pid_t)-1) {
    kill(AttachPid, SIGTERM);
    kill(AttachPid, SIGCONT);
  }
}

static void Login(void) {
  struct passwd *p;
  char *pw_passwd = NULL, *c;
  byte ok = tfalse;

  if ((p = getpwnam(user.txt))) {
#ifdef TW_HAVE_GETSPNAM
    struct spwd *s = getspnam(user.txt);

    if (s)
      pw_passwd = s->sp_pwdp;
    else
#endif
      pw_passwd = p->pw_passwd;
#if defined(TW_HAVE_CRYPT) || defined(TW_HAVE_CRYPT_CRYPT)
    if (pw_passwd && (((c = crypt(pass.txt, pw_passwd)) && !strcmp(c, pw_passwd)) || !*pw_passwd))
      ok = ttrue;
#endif

    if (ok && TwSetServerUid(p->pw_uid, Privileges)) {
      /* all ok */
      ClearKey();
      TwDeleteMsgPort(DM_MsgPort);
      logged_in = ttrue;
      return;
    }
  }
  /* something is wrong... */
  TwUnMapWindow(DM_pass);
  TwFocusSubWidget(DM_user);
  ClearKey();
  TwFlush();
  sleep(3);
  TwMapWindow(DM_pass, DM_Window);
  /* we slept. swallow all events ready to be received */
  while (TwPeekMsg())
    TwReadMsg(tfalse);
}

static void Logout(void) {
  TwClose();
  if (DM_Kill || logged_in == tfalse)
    kill_server();
  else
    kill_attach();
  shortsleep();
  logged_in = tfalse;
}

static void quit(void) {
  kill_server();
  shortsleep();
  exit(0);
}

static void DelKey(twindow W, data u) {
  if (u->x < u->len) {
    if (u->x + 1 < u->len)
      memmove(u->txt + u->x, u->txt + u->x + 1, u->len - u->x - 1);
    u->txt[--u->len] = '\0';

    if (W == DM_user) {
      TwGotoXYWindow(W, 0, 0);
      TwWriteCharsetWindow(W, u->len, u->txt);
    } else
      TwGotoXYWindow(W, u->len, 0);
    TwWriteCharsetWindow(W, 1, " ");
    TwGotoXYWindow(W, u->x, 0);
  }
}

static void BackKey(twindow W, data u) {
  if (u->x) {
    u->x--;
    DelKey(W, u);
  }
}

static void LeftKey(twindow W, data u) {
  if (u->x)
    TwGotoXYWindow(W, --u->x, 0);
}

static void RightKey(twindow W, data u) {
  if (u->x < u->len)
    TwGotoXYWindow(W, ++u->x, 0);
}

static void HomeKey(twindow W, data u) {
  if (u->x)
    TwGotoXYWindow(W, u->x = 0, 0);
}

static void EndKey(twindow W, data u) {
  if (u->x < u->len)
    TwGotoXYWindow(W, u->x = u->len, 0);
}

static void WriteUtf8Key(twindow W, data u, udat len, char *seq) {
  char *_txt;
  byte _len, x;

  _txt = u->txt;
  _len = u->len;
  x = u->x;

  if (x <= 29) {
    if (x + len > 29)
      len = 29 - x;
    TwCopyMem(seq, _txt + x, len);
    if (x + len > _len)
      _len = x + len;
    _txt[_len] = '\0';
    u->x += len;
    u->len = _len;
    if (W == DM_pass)
      memset(seq, '*', len);
    TwWriteUtf8Window(W, len, seq);
  }
}

static void HandleKey(tevent_keyboard E) {
  data u;

  if (E->W == DM_Window) {
    TwFocusSubWidget(E->W = DM_user);
  }
  if (E->W == DM_user)
    u = &user;
  else
    u = &pass;

  switch (E->Code) {
  case TW_Linefeed:
  case TW_Return:
    if (E->W == DM_pass)
      Login();
    else
      TwFocusSubWidget(DM_pass);
    break;
  case TW_Tab:
    TwFocusSubWidget(E->W == DM_pass ? DM_user : DM_pass);
    break;
  case TW_BackSpace:
    BackKey(E->W, u);
    break;
  case TW_Delete:
  case TW_KP_Delete:
    DelKey(E->W, u);
    break;
  case TW_Left:
  case TW_KP_Left:
    LeftKey(E->W, u);
    break;
  case TW_Right:
  case TW_KP_Right:
    RightKey(E->W, u);
    break;
  case TW_Home:
  case TW_KP_Home:
    HomeKey(E->W, u);
    break;
  case TW_End:
  case TW_KP_End:
    EndKey(E->W, u);
    break;
  default:
    if (E->Code >= 32 && E->Code < 256 && E->SeqLen)
      WriteUtf8Key(E->W, u, E->SeqLen, E->AsciiSeq);
    break;
  }
}

static void HandleGadget(tevent_gadget E) {
  switch (E->Code) {
  case DM_GADGET_LOGIN:
    Login();
    break;
  case DM_GADGET_CLEAR:
    ClearKey();
    break;
  case DM_GADGET_CONSOLE:
    quit();
    break;
  default:
    break;
  }
}

static void SignalPanic(int n) {
  signal(n, SIG_DFL);
  quit();
  TW_RETFROMSIGNAL(0);
}

static void SignalChild(int n) {
  pid_t pid;
  int status;

  signal(SIGCHLD, SignalChild);

  while ((pid = Tw_wait3(&status, WNOHANG, (struct rusage *)0)) != 0 && pid != (pid_t)-1) {
    if (WIFEXITED(status) || WIFSIGNALED(status)) {
      if (pid == AttachPid)
        AttachPid = (pid_t)-1;
    }
  }
  TW_RETFROMSIGNAL(0);
}

static void InitSignals(void) {
  signal(SIGWINCH, SIG_IGN);
  signal(SIGCHLD, SignalChild);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGIO, SIG_IGN);
  signal(SIGHUP, SIG_IGN);
#ifndef TW_DONT_TRAP_SIGNALS
  signal(SIGINT, SignalPanic);
  signal(SIGQUIT, SignalPanic);
  signal(SIGILL, SignalPanic);
  signal(SIGABRT, SignalPanic);
  signal(SIGBUS, SignalPanic);
  signal(SIGFPE, SignalPanic);
  signal(SIGSEGV, SignalPanic);
  signal(SIGTERM, SignalPanic);
  signal(SIGXCPU, SignalPanic);
  signal(SIGXFSZ, SignalPanic);
#ifdef SIGPWR
  signal(SIGPWR, SignalPanic);
#endif
#endif
}

int main(int argc, char *argv[]) {
  fd_set fset;
  int fd;
  tmsg msg;
  uldat err;

  Args = argv + 1;

  ParseArgs();
  SetHOME();

  if (getuid() != 0) {
    fprintf(stderr, "Only root wants to run twdm\n");
    return 1;
  }

  InitSignals();

  if (quiet && (err = open("/dev/null", O_RDWR)) != (uldat)-1 && err != 2) {
    dup2(err, 2);
    close(err);
  }

  while (InitServer() && InitAttach() && InitClient()) {

    fd = TwConnectionFd();
    FD_ZERO(&fset);

    /* wait for user to login */
    while (AttachPid != (pid_t)-1 && ServerPid != (pid_t)-1 && logged_in == tfalse &&
           !TwInPanic()) {

      while ((msg = TwReadMsg(tfalse)))
        switch (msg->Type) {
        case TW_MSG_WIDGET_KEY:
          HandleKey(&msg->Event.EventKeyboard);
          break;
        case TW_MSG_WIDGET_GADGET:
          HandleGadget(&msg->Event.EventGadget);
          break;
        case TW_MSG_SELECTION:
          /*
           * send msg->Event.EventSelection.W as ReqPrivate field,
           * so that we will get it back in TW_MSG_SELECTIONNOTIFY message
           * without having to store it manually
           */
          TwRequestSelection(TwGetOwnerSelection(), msg->Event.EventSelection.W);
          break;
        case TW_MSG_SELECTIONNOTIFY: {
          tevent_selectionnotify E = &msg->Event.EventSelectionNotify;

          /* react as for keypresses */
          if (E->Magic == TW_SEL_UTF8MAGIC) {
            WriteUtf8Key(E->ReqPrivate, E->ReqPrivate == DM_user ? &user : &pass, E->Len,
                         (char *)E->Data);
          }
        } break;
        default:
          break;
        }

      /*
       * sleep until a message or a signal arrives.
       * using TwReadMsg(ttrue) would *NOT* immediately return
       * after a signal is received.
       */
      FD_SET(fd, &fset);
      select(fd + 1, &fset, NULL, NULL, NULL);
    }

    /* wait for user to logout */
    while (AttachPid != (pid_t)-1 && ServerPid != (pid_t)-1 && logged_in == ttrue && !TwInPanic()) {

      while ((msg = TwReadMsg(tfalse)))
        ;

      /* sleep until a message or a signal arrives (as above) */
      FD_SET(fd, &fset);
      select(fd + 1, &fset, NULL, NULL, NULL);
    }
    Logout();
    if (oneshot) {
      break;
    }
  }

  if ((err = TwErrno))
    fprintf(stderr, "%s: libtw error: %s%s\n", argv[0], TwStrError(err),
            TwStrErrorDetail(err, TwErrnoDetail));

  quit();

  /*NOTREACHED*/
  return 0;
}
