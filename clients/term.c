/*
 *  tterm.c  --  create and manage multiple terminal emulator windows on twin
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>

#include <Tw/Twautoconf.h>

#ifdef TW_HAVE_SYS_RESOURCE_H
# include <sys/resource.h>
#endif

#ifdef TW_HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#ifdef TW_HAVE_TERMIOS_H
# include <termios.h>
#else
# ifdef TW_HAVE_TERMIO_H
#  include <termio.h>
# endif
#endif

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
#endif

#include "version.h"

#include "pty.h"

#define COD_QUIT      (udat)1
#define COD_ASKCLOSE  (udat)2
#define COD_SPAWN     (udat)3

static tscreen Term_Screen;
static tmsgport Term_MsgPort;
static tmenu Term_Menu;
static uldat WinN;

fd_set save_rfds;
uldat max_fds;
int twin_fd;

static void CloseTerm(uldat Slot);


/* 5. remote fds handling */
typedef struct {
    int Fd;
    twindow Window;
    pid_t Pid;
} fdlist;

static fdlist *FdList;
static uldat FdSize, FdTop, FdBottom;
#define LS	FdList[Slot]


/* functions */

static uldat FdListGrow(void) {
    uldat oldsize, size;
    fdlist *newFdList;
    
    if ((oldsize = FdSize) == MAXULDAT)
	return TW_NOSLOT;
    
    if ((size = oldsize < 64 ? 96 : oldsize + (oldsize>>1)) < oldsize)
	size = MAXULDAT;
    
    if (!(newFdList = (fdlist *)TwReAllocMem(FdList, size*sizeof(fdlist))))
	return TW_NOSLOT;
    
    for (FdSize = oldsize+1; FdSize<size; FdSize++)
	newFdList[FdSize].Fd = TW_NOFD;
    
    FdList = newFdList;
    
    return oldsize;
}

TW_INLINE uldat FdListGet(void) {
    if (FdBottom < FdSize)
	return FdBottom;
    return FdListGrow();
}

static uldat Slot_Window(twindow Window) {
    uldat Slot;
    for (Slot = 0; Slot < FdTop; Slot++) {
	if (LS.Fd != -1 && LS.Window == Window)
	    return Slot;
    }
    return TW_NOSLOT;
}

TW_INLINE int Fd_Slot(uldat Slot) {
    if (Slot < FdTop)
	return LS.Fd;
    return TW_NOFD;
}

static uldat RegisterRemote(int Fd, twindow Window, pid_t Pid) {
    uldat Slot, j;
    
    if ((Slot = FdListGet()) == TW_NOSLOT)
	return Slot;
    
    LS.Fd = Fd;
    LS.Window = Window;
    LS.Pid = Pid;
    
    if (FdTop <= Slot)
	FdTop = Slot + 1;
    for (j = FdBottom; j < FdTop; j++)
	if (FdList[j].Fd == TW_NOFD)
	    break;
    FdBottom = j;

    FD_SET(Fd, &save_rfds);
    if (max_fds < Fd)
	max_fds = Fd;
    
    return Slot;
}

/* UnRegister a Fd and related stuff given a slot number */
static void UnRegisterRemote(uldat Slot) {
    int i;
    uldat j;
    
    if (Slot < FdTop && LS.Fd != TW_NOFD) {
	FD_CLR(LS.Fd, &save_rfds);
	LS.Fd = TW_NOFD;
	
	for (i=max_fds; i>=0; i--)
	    if (FD_ISSET(i, &save_rfds))
		break;
	max_fds = i;
	
	if (FdBottom > Slot)
	    FdBottom = Slot;
	for (j = FdTop; j > FdBottom; j--)
	    if (FdList[j].Fd != TW_NOFD)
		break;
	FdTop = (j == FdBottom) ? j : j + 1;
    }
}

void Resize(uldat Slot, dat X, dat Y) {
    struct winsize wsiz;
    if (Slot < FdTop && LS.Fd != TW_NOFD) {
	wsiz.ws_col = X;
	wsiz.ws_row = Y;
	wsiz.ws_xpixel = 0;
	wsiz.ws_ypixel = 0;
    
	if (ioctl(LS.Fd, TIOCSWINSZ, &wsiz) == 0)
	    kill(-LS.Pid, SIGWINCH);
    }
}


static byte **TokenizeStringVec(uldat len, byte *s) {
    byte **cmd = NULL, *buf, c;
    uldat save_len, save_n, n = 0;
    
    /* skip initial spaces */
    while (len && ((c = *s) == '\0' || c == ' ')) {
	len--, s++;
    }
    save_len = len;
    
    if (len && (buf = TwAllocMem(len + 1))) {
	TwCopyMem(s, buf, len);
	buf[len] = '\0';
	
	/* how many args? */
	while (len) {
	    len--, c = *s++;
	    if (c && c != ' ') {
		n++;
		while (len && (c = *s) && c != ' ') {
		    len--, s++;
		}
	    }
	}
	if ((cmd = TwAllocMem((n + 1) * sizeof(byte *)))) {
	    save_n = n;
	    n = 0;
	    len = save_len;
	    s = buf;
	    
	    /* put args in cmd[] */
	    while (len) {
		len--, c = *s++;
		if (c && c != ' ') {
		    cmd[n++] = s - 1;
		    while (len && (c = *s) && c != ' ') {
			len--, s++;
		    }
		    *s = '\0'; /* safe, we did a malloc(len+1) */
		}
	    }
	    cmd[n] = NULL; /* safe, we did a malloc(n+1) */
	}
    }
    return cmd;
}

static void FreeStringVec(byte **cmd) {
    TwFreeMem(cmd[0]);
    TwFreeMem(cmd);
}

static byte **default_args;
static byte *default_title = "Twin Term";

static twindow newTermWindow(byte *title) {
    twindow Window = TwCreateWindow
	(TwLenStr(title), title, NULL,
	 Term_Menu, COL(WHITE,BLACK), TW_LINECURSOR,
	 TW_WINDOW_WANT_KEYS|TW_WINDOW_WANT_CHANGES|TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_Y_BAR|TW_WINDOW_CLOSE,
	 TW_WINDOWFL_CURSOR_ON|TW_WINDOWFL_USECONTENTS,
	 80, 25, 200);

    if (Window != TW_NOID) {
	TwSetColorsWindow
	    (Window, 0x1FF, COL(HIGH|YELLOW,CYAN), COL(HIGH|GREEN,HIGH|BLUE),
	     COL(WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE),
	     COL(WHITE,BLACK), COL(HIGH|BLACK,HIGH|WHITE), COL(HIGH|BLACK,BLACK), COL(BLACK,HIGH|BLACK));
	
	TwConfigureWindow(Window, (1<<2)|(1<<3), 0, 0, 5, 1, 0, 0);
    }
    return Window;
}

static byte OpenTerm(TW_CONST byte *arg0, byte * TW_CONST *argv) {
    twindow Window;
    int Fd;
    pid_t Pid;
    uldat Slot;
    byte *title;
    
    /* if {arg0, argv} is {NULL, ...} or {"", ... } then start user's shell */
    if (arg0 && *arg0 && argv && argv[0]) {
	if ((title = strrchr(argv[0], '/')))
	    title++;
	else
	    title = argv[0];
    } else {
	arg0 = default_args[0];
	argv = default_args+1;
	
	title = default_title;
    }
    
    if ((Window = newTermWindow(title))) {
	if ((Fd = Spawn(Window, &Pid, 80, 25, arg0, argv)) != TW_NOFD) {
	    if ((Slot = RegisterRemote(Fd, Window, Pid)) != TW_NOSLOT) {
		TwMapWindow(Window, Term_Screen);
		WinN++;
		return !TwInPanic();
	    }
	    close(Fd);
	}
	TwDeleteWindow(Window);
    }
    return FALSE;
}

static void CloseTerm(uldat Slot) {
    close(LS.Fd);
    TwDeleteWindow(LS.Window);
    UnRegisterRemote(Slot);
    WinN--;
}

/*
 * it is not safe to call libTw functions from within signal handlers
 * (expecially if you compiled libTw as thread-safe) so
 * just set a flag in the handler and react to it syncronously
 */
static volatile byte ReceivedSignalChild;

static TW_RETSIGTYPE SignalChild(int n) {
    ReceivedSignalChild = TRUE;
    signal(SIGCHLD, SignalChild);
    TW_RETFROMSIGNAL(0);
}

static void RemotePidIsDead(pid_t pid) {
    uldat Slot;
    
    for (Slot=0; Slot<FdTop; Slot++) {
	if (LS.Fd != TW_NOFD && LS.Pid == pid) {
	    CloseTerm(Slot);
	    return;
	}
    }
}

static void HandleSignalChild(void) {
    pid_t pid;
    int status;
    while ((pid = wait3(&status, WNOHANG, (struct rusage *)0)) != 0 && pid != (pid_t)-1) {
	if (WIFEXITED(status) || WIFSIGNALED(status))
	    RemotePidIsDead(pid);
    }
    ReceivedSignalChild = FALSE;
}

static byte Add_Spawn_Row4Menu(twindow Window) {
    uldat len = strlen(default_title);
    byte *name, ret;
    
    if (strcmp(default_title, "Twin Term") &&
	(name = TwAllocMem(len + 6))) {
	
	TwCopyMem(" New ", name, 5);	
	TwCopyMem(default_title, name + 5, len);
	name[len+5] = ' ';

	ret = TwRow4Menu(Window, COD_SPAWN, TW_ROW_ACTIVE, len+6, name);
	TwFreeMem(name);
    } else
	ret = TwRow4Menu(Window, COD_SPAWN, TW_ROW_ACTIVE, 10, " New Term ");
    return ret;
}

TW_DECL_MAGIC(term_magic);

static byte InitTerm(void) {
    twindow Window;
    uldat err;
    
    signal(SIGCHLD, SignalChild);

#if defined(TW_HAVE_PUTENV)
    putenv("TERM=linux");
#elif defined(TW_HAVE_SETENV)
    setenv("TERM","linux",1);
#endif

    if (TwCheckMagic(term_magic) && TwOpen(NULL) &&
	(Term_MsgPort=TwCreateMsgPort(6, "twterm")) &&
	(Term_Menu=TwCreateMenu
	 (COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0)) &&
	(TwInfo4Menu(Term_Menu, TW_ROW_ACTIVE, 18, " Remote Twin Term ", "ptpppppptpppptpppp"), TRUE) &&
	(Window=TwWin4Menu(Term_Menu)) &&
	Add_Spawn_Row4Menu(Window) &&
	TwRow4Menu(Window, COD_QUIT,  FALSE, 6, " Exit ") &&
	TwItem4Menu(Term_Menu, Window, TRUE, 6, " File ") &&
	TwItem4MenuCommon(Term_Menu) &&
	(Term_Screen = TwFirstScreen()) &&
	(OpenTerm(NULL, NULL)))
	 
	return TRUE;

    TwClose();
    
    if ((err = TwErrno))
	fprintf(stderr, "twterm: libTw error: %s%s\n",
		TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));

    return FALSE;
}

static void TwinTermH(void) {
    tmsg Msg;
    tevent_any Event;
    udat Code/*, Repeat*/;
    twindow Win;
    uldat Slot;
    int Fd;
    
    while ((Msg=TwReadMsg(FALSE))) {
	
	Event=&Msg->Event;
	Win = Event->EventCommon.W;
	Slot = Slot_Window(Win);
	Fd = Fd_Slot(Slot);
	
	if (Msg->Type==TW_MSG_WIDGET_KEY) {
	    /* send keypresses */
	    write(Fd, Event->EventKeyboard.AsciiSeq, Event->EventKeyboard.SeqLen);
	} else if (Msg->Type==TW_MSG_SELECTION) {
	    /*
	     * send Msg->Event.EventCommon.Window as ReqPrivate field,
	     * so that we will get it back in TW_MSG_SELECTIONNOTIFY message
	     * without having to store it manually
	     */
	    TwRequestSelection(TwGetOwnerSelection(), Win);
	    
	} else if (Msg->Type==TW_MSG_SELECTIONNOTIFY) {

	    Win = Event->EventSelectionNotify.ReqPrivate;
	    Slot = Slot_Window(Win);
	    Fd = Fd_Slot(Slot);

	    /* react as for keypresses */
#ifdef CONF__UNICODE
	    if (Event->EventSelectionNotify.Magic == TW_SEL_HWFONTMAGIC) {
		byte *Dst = Event->EventSelectionNotify.Data;
		hwfont *Src = (hwfont *)Dst;
		uldat n = Event->EventSelectionNotify.Len / sizeof(hwfont);

		/* FIXME: this is rough */
		while (n--)
		    *Dst++ = Tutf_UTF_16_to_CP437(*Src++);
		
		write(Fd, Event->EventSelectionNotify.Data, Event->EventSelectionNotify.Len / sizeof(hwfont));
	    } else
#endif
		write(Fd, Event->EventSelectionNotify.Data, Event->EventSelectionNotify.Len);

	} else if (Msg->Type==TW_MSG_WIDGET_MOUSE) {
	    fprintf(stderr, "twterm: unexpected Mouse event message!\n");
	    
	} else if (Msg->Type==TW_MSG_WIDGET_GADGET) {
	    if (Event->EventGadget.Code == 0 /* Close Code */ )
		CloseTerm(Slot);
	} else if (Msg->Type==TW_MSG_MENU_ROW) {
	    if (Event->EventMenu.Menu==Term_Menu) {
		Code=Event->EventMenu.Code;
		switch (Code) {
		  case COD_SPAWN:
		    OpenTerm(NULL, NULL);
		    break;
		  case COD_ASKCLOSE:
		    CloseTerm(Slot);
		    break;
		  default:
		    break;
		}
	    }
	} else if (Msg->Type==TW_MSG_WIDGET_CHANGE) {
	    Resize(Slot, Event->EventWidget.XWidth, Event->EventWidget.YWidth);
	    
	} else if (Msg->Type==TW_MSG_USER_CONTROL) {
	    if (Event->EventControl.Code == TW_MSG_CONTROL_OPEN) {
		byte **cmd = TokenizeStringVec(Event->EventControl.Len, Event->EventControl.Data);
		if (cmd) {
		    OpenTerm(cmd[0], cmd);
		    FreeStringVec(cmd);
		}
	    }
	}
    }
}

static void TwinTermIO(int Slot) {
    static byte buf[TW_BIGBUFF];
    ldat got = 0, chunk = 0;
    
    do {
	/*
	 * TW_BIGBUFF - 1 to avoid silly windows...
	 * linux ttys buffer up to 4095 bytes.
	 */
	chunk = read(LS.Fd, buf + got, TW_BIGBUFF - 1 - got);
    } while (chunk > 0 && (got += chunk) < TW_BIGBUFF - 1);
    
    if (got)
	TwWriteAsciiWindow(LS.Window, got, buf);
    else if (chunk == -1 && errno != EINTR && errno != EWOULDBLOCK)
	/* something bad happened to our child :( */
	CloseTerm(Slot);
}

static void Usage(char *name) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n"
	    "Currently known options: \n"
	    " -h, --help              display this help and exit\n"
	    " -V, --version           output version information and exit\n"
	    " -t <title>              set window title\n"
	    " -e <command>            run <command> instead of user's shell\n"
	    "                         (must be last option)\n", name);
}

static void ShowVersion(void) {
    fputs("twterm " TWIN_VERSION_STR "\n", stdout);
}

int main(int argc, char *argv[]) {
    fd_set fds;
    int num_fds;
    uldat Slot, err;
    struct timeval zero = {0, 0}, *pt;
    byte *t, *name = argv[0], *shell[3];

    FD_ZERO(&save_rfds);

    argv++, argc--;
    
    TwMergeHyphensArgv(argc, argv);
    
    while (argc) {
	if (!strcmp(*argv, "-h") || !strcmp(*argv, "-help")) {
	    Usage(name);
	    return 0;
	} else if (!strcmp(*argv, "-V") || !strcmp(*argv, "-version")) {
	    ShowVersion();
	    return 0;
	} else if (argc > 1 && !strcmp(*argv, "-t")) {
	    default_title = *++argv;
	    argc--;
	} else if (argc > 1 && !strcmp(*argv, "-e")) {
	    default_args = (byte **)argv;
	    default_args[0] = default_args[1];
	    break;
	} else {
	    fprintf(stderr, "%s: argument `%s' not recognized\n"
		    "\ttry `%s --help' for usage summary.\n", name, *argv, name);
	    return 1;
	}
	argv++;
	argc--;
    }
    
    if (!default_args) {
	if ((shell[0] = getenv("SHELL")) &&
	    (shell[0] = strdup(shell[0])) &&
	    (shell[1] = (t = strrchr(shell[0], '/'))
	     ? strdup(t) : strdup(shell[0]))) {
	    
	    if (shell[1][0] == '/')
		shell[1][0] = '-';
	    shell[2] = NULL;
	    default_args = shell;
	} else
	    return 1;
    }

    if (!InitTerm())
	return 0;

    twin_fd = TwConnectionFd();
    FD_SET(twin_fd, &save_rfds);
    if (max_fds < twin_fd)
	max_fds = twin_fd;

    while (WinN) {
	/* bail out if something goes *really* wrong */
	if (!TwFlush())
	    break;

	if (TwPendingMsg())
	    /* some Msg is available, don't sleep */
	    pt = &zero;
	else
	    pt = NULL;
	
	fds = save_rfds;

	do {
	    num_fds = select(max_fds+1, &fds, NULL, NULL, pt);
	    
	    if (ReceivedSignalChild)
		HandleSignalChild();
	} while (num_fds < 0 && errno == EINTR);

	if (num_fds < 0 && errno != EINTR) {
	    /* panic! */
	    TwClose();
	    return 1;
	}

	/*
	 * We cannot rely on FD_ISSET(twin_fd, &fds) alone to call TwinTermH(),
	 * as Msgs may have been already received through the socket
	 * and sitting in the local queue
	 */
	if ((num_fds > 0 && FD_ISSET(twin_fd, &fds)) || TwPendingMsg()) {
	    if (FD_ISSET(twin_fd, &fds))
		num_fds--;
	    TwinTermH();
	}
	
	for (Slot = 0; num_fds > 0 && Slot < FdTop; Slot++) {
	    if (LS.Fd != TW_NOFD && FD_ISSET(LS.Fd, &fds))
		TwinTermIO(Slot), num_fds--;
	}
    }
    if ((err = TwErrno)) {
	fprintf(stderr, "twterm: libTw error: %s%s\n",
		TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
	return 1;
    }
    TwClose();
    return 0;
}
