/*
 *  attach.c  --  connect to a running twin and tell it to attach/detach
 *                from a given display
 *
 *  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>
#include "version.h"

static char *MYname;

static void Usage(byte detach) {
  fprintf(stderr,
          "Usage: %s [OPTIONS] %s\n"
          "Currently known options: \n"
          "  -h, --help               display this help and exit\n"
          "  -V, --version            output version information and exit\n"
          "  -a                       attach display%s\n"
          "  -d                       detach display%s\n"
          "  -s, --share              allow multiple simultaneous displays (default)\n"
          "  -x, --excl               request exclusive display: detach all others\n"
          "  -v, --verbose            verbose output (default)\n"
          "  -q, --quiet              quiet: don't report messages from twin server\n"
          "  -f, --force              force running even with wrong protocol version\n"
          "  --twin@<TWDISPLAY>       specify server to contact (default is $TWDISPLAY)\n"
          "  --hw=<display>[,options] start the given display driver\n"
          "Currently known display drivers: \n"
          "\tX[@<XDISPLAY>]\n"
          "\txft[@<XDISPLAY>]\n"
          "\ttwin[@<TWDISPLAY>]\n"
          "\ttty[@<tty device>]\n",
          MYname, detach ? "" : "--hw=<display> [...]", detach ? "" : " (default)",
          detach ? " (default)" : "");
}

static TW_VOLATILE byte gotSignals, gotSignalWinch, gotSignalPanic;

static void SignalWinch(int n) {
  signal(SIGWINCH, SignalWinch);
  gotSignals = gotSignalWinch = ttrue;
  TW_RETFROMSIGNAL(0);
}

static void SignalPanic(int n) {
  signal(n, SIG_IGN);
  gotSignals = gotSignalPanic = ttrue;
  TW_RETFROMSIGNAL(0);
}

static void ShowVersion(void) {
  printf("%s " TWIN_VERSION_STR " with socket protocol " TW_PROTOCOL_VERSION_STR "\n", MYname);
}

static byte VersionsMatch(byte force) {
  uldat cv = TW_PROTOCOL_VERSION, lv = TwLibraryVersion(), sv = TwServerVersion();

  if (lv != sv || lv != cv) {
    fprintf(stderr,
            "%s: %s: socket protocol version mismatch!%s\n"
            "          client is %d.%d.%d, library is %d.%d.%d, server is %d.%d.%d\n",
            MYname, (force ? "warning" : "fatal"), (force ? " (ignored)" : ""),
            (int)TW_VER_MAJOR(cv), (int)TW_VER_MINOR(cv), (int)TW_VER_PATCH(cv),
            (int)TW_VER_MAJOR(lv), (int)TW_VER_MINOR(lv), (int)TW_VER_PATCH(lv),
            (int)TW_VER_MAJOR(sv), (int)TW_VER_MINOR(sv), (int)TW_VER_PATCH(sv));
    return tfalse;
  }
  return ttrue;
}

static void InitSignals(void) {
  signal(SIGWINCH, SignalWinch);
  signal(SIGCHLD, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGIO, SIG_IGN);
#ifndef TW_DONT_TRAP_SIGNALS
  signal(SIGHUP, SignalPanic);
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

static char *fix_tty(char *arg, byte is_ourtty[1], byte err[1]) {
  char *target = NULL;
  char *opts = arg + 7;
  char *comma = strchr(opts, ',');
  TW_CONST char *tty = ttyname(0);
  byte is_servtty = 0;
  if (!tty) {
    fprintf(stderr, "%s: ttyname() failed, cannot find controlling tty!\n", MYname);
    *err = 1;
    return NULL;
  }
  if (comma) {
    *comma = '\0';
  }

  if (!opts[0]) {
    *is_ourtty = 1; /* attach twin to our tty */
  } else if (opts[0] == '@' && opts[1]) {
    if (opts[1] == '-') {
      is_servtty = 1; /* tell twin to attach to its own tty */
    } else if (!strcmp(opts + 1, tty)) {
      *is_ourtty = 1; /* attach twin to our tty */
    }
  } else {
    fprintf(stderr, "%s: malformed display hw `%s'\n", MYname, arg);
    *err = 1;
    return NULL;
  }

  if (comma)
    *comma = ',';
  else
    comma = "";

  if (*is_ourtty) {
    TW_CONST char *term = getenv("TERM");
    if (!term) {
      term = "";
    }
    target = (char *)malloc(strlen(tty) + 9 + strlen(comma) + (*term ? 6 + strlen(term) : 0));
    if (target) {
      sprintf(target, "-hw=tty@%s%s%s%s", tty, (*term ? ",TERM=" : ""), term, comma);
    }
  } else if (is_servtty) {
    target = malloc(8 + strlen(comma));
    if (target) {
      sprintf(target, "-hw=tty%s", comma);
    }
  } else {
    target = strdup(arg);
  }
  if (!target) {
    fprintf(stderr, "%s: out of memory!\n", MYname);
    *err = 1;
  }
  return target;
}

TW_DECL_MAGIC(attach_magic);

int main(int argc, char *argv[]) {
  char *dpy = NULL, *target = NULL, *arg;
  uldat chunk;
  byte detach = 0, redirect, force = 0, flags = TW_ATTACH_HW_REDIRECT;
  byte is_ourtty = 0;

  TwMergeHyphensArgv(argc, argv);

  MYname = argv[0];

  if (strstr(argv[0], "detach")) {
    detach = 1;
  }

  while ((arg = *++argv) != NULL) {
    if (!strcmp(arg, "-V") || !strcmp(arg, "-version")) {
      ShowVersion();
      return 0;
    } else if (!strcmp(arg, "-h") || !strcmp(arg, "-help")) {
      Usage(detach);
      return 0;
    } else if (!strcmp(arg, "-x") || !strcmp(arg, "-excl"))
      flags |= TW_ATTACH_HW_EXCLUSIVE;
    else if (!strcmp(arg, "-s") || !strcmp(arg, "-share"))
      flags &= ~TW_ATTACH_HW_EXCLUSIVE;
    else if (!strcmp(arg, "-a"))
      detach = 0;
    else if (!strcmp(arg, "-d"))
      detach = 1;
    else if (!strcmp(arg, "-v") || !strcmp(arg, "-verbose"))
      flags |= TW_ATTACH_HW_REDIRECT;
    else if (!strcmp(arg, "-q") || !strcmp(arg, "-quiet"))
      flags &= ~TW_ATTACH_HW_REDIRECT;
    else if (!strcmp(arg, "-f") || !strcmp(arg, "-force"))
      force = 1;
    else if (!strncmp(arg, "-twin@", 6))
      dpy = arg + 6;
    else if (!strncmp(arg, "-hw=", 4)) {
      if (!strncmp(arg + 4, "tty", 3)) {
        byte err = 0;
        target = fix_tty(arg, &is_ourtty, &err);
        if (err != 0 || !target) {
          return 1;
        }
      } else if (arg[4]) {
        target = strdup(arg);
      } else {
        Usage(detach);
        return 1;
      }
    } else {
      fprintf(stderr,
              "%s: argument `%s' not recognized\n"
              "\ttry `%s --help' for usage summary.\n",
              MYname, arg, MYname);
      return 1;
    }
  }

  if (detach == 0 && !target) {
    Usage(detach);
    return 1;
  }

  redirect = flags & TW_ATTACH_HW_REDIRECT;

  InitSignals();

  if (TwCheckMagic(attach_magic) && TwOpen(dpy) && TwCreateMsgPort(8, "twattach"))
    do {
      byte ret = 0;

      if (!VersionsMatch(force)) {
        if (!force) {
          fprintf(stderr, "%s: Aborting. Use option `--force' to ignore versions check.\n", MYname);
          TwClose();
          return 1;
        }
      }

      if (detach) {
        return !TwDetachHW(target ? strlen(target) : 0, target);
      }

      TwAttachHW(target ? strlen(target) : 0, target, flags);
      TwFlush();
      free(target);

      if (redirect)
        fprintf(stderr, "reported messages...\n");

      for (;;) {
        TW_CONST char *reply = TwAttachGetReply(&chunk);
        if (reply <= (char *)2) {
          ret = (byte)(size_t)reply;
          break;
        } else if (reply == (char *)-1) {
          break; /* libtw panic */
        }

        fprintf(stderr, "%.*s", (int)chunk, reply);
      }
      fflush(stderr);

      if (TwInPanic())
        break;

      if (is_ourtty) {
        fputs("\033[2J", stdout);
        fflush(stdout);
      }

      /*
       * twin waits this before grabbing the display...
       * so we can fflush(stdout) to show all messages
       * *BEFORE* twin draws on (eventually) the same tty
       */
      TwAttachConfirm();

      if (ret == 2) {
        /*
         * twin told us to stay and sit on the display
         * until it is quitted.
         */
        int fd = TwConnectionFd();
        fd_set fds;
        FD_ZERO(&fds);

        while (!gotSignalPanic && !TwInPanic()) {
          while (TwReadMsg(tfalse))
            ;
          FD_SET(fd, &fds);
          select(fd + 1, &fds, NULL, NULL, NULL);
          if (gotSignalWinch)
            TwNeedResizeDisplay(), TwFlush(), gotSignalWinch = tfalse;
        }
      } else if (redirect) {
        if (ret)
          fprintf(stderr, "... ok, twin successfully attached.\n");
        else
          fprintf(stderr, "... ach, twin failed to attach.\n");
      }
      return !ret;
    } while (0);

  chunk = TwErrno;
  fprintf(stderr, "%s: libtw error: %s%s\n", MYname, TwStrError(chunk),
          TwStrErrorDetail(chunk, TwErrnoDetail));
  return 1;
}
