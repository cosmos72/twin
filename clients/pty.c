/*
 *  parts of code taken from:
 *
 *  rxvt-2.4.7 sources (various authors)
 *  available at ftp://ftp.math.fu-berlin.de/pub/rxvt/
 *  and usual Linux mirrors
 *
 *
 * other parts from Twin autor:
 *
 *  Copyright (C) 1993-1999  Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

/* tell <stdlib.h> to declare grantpt(), unlockpt(), ptsname() */
#define _GNU_SOURCE
#define _XOPEN_SOURCE

#include <Tw/autoconf.h> /* for TW_HAVE_* macros */
#include "twconfig.h"    /* for CONF_* macros */

#ifdef TW_HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef TW_HAVE_GRP_H
#include <grp.h>
#endif
#ifdef TW_HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef TW_HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef TW_HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef TW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if defined(TW_HAVE_TERMIOS_H)
#include <termios.h>
#elif defined(TW_HAVE_TERMIO_H)
#include <termio.h>
#endif

#ifdef TW_HAVE_SYS_TTYDEFAULTS_H
#include <sys/ttydefaults.h>
#else
#include "my_ttydefaults.h"
#endif
#include "tty_ioctl.h"

#include <Tw/Tw.h>

#include "term.h"

/* pseudo-teletype connections handling functions */

/* 0. variables */
static char *ptydev, *ttydev;
static int ptyfd, ttyfd;

#ifdef CONF_TERM_DEVPTS
static void ptyError(const char *d, const char *f, const char *arg) {
  fprintf(stderr, "twterm: %s: %s(\"%s\") failed: %s\n", d ? d : "<NULL>", f ? f : "<NULL>",
          arg ? arg : "<NULL>", strerror(errno));
}

static void getPtyError(const char *f, const char *arg) {
  ptyError("opening pseudo-tty", f, arg);
}
#endif

/* 1. Acquire a pseudo-teletype from the system. */
/*
 * On failure, returns tfalse.
 * On success, fills ttydev and ptydev with the names of the master
 * and slave parts and sets ttyfd and ptyfd to the file descriptors
 */
static byte getPty(void) {
  int fd = -1, sfd = -1;
#ifdef CONF_TERM_DEVPTS

  /* open master pty */
  if (
#ifdef TW_HAVE_GETPT
      (fd = getpt()) >= 0
#else
      (fd = open("/dev/ptmx", O_RDWR | O_NOCTTY)) >= 0
#endif
  ) {

    if (grantpt(fd) == 0) {
      if (unlockpt(fd) == 0) {
        ptydev = ttydev = ptsname(fd);
        if ((sfd = open(ptydev, O_RDWR | O_NOCTTY)) >= 0)
          goto Found;
        else
          getPtyError("slave open", ptydev);
      } else
        getPtyError("unlockpt", "");
    } else
      getPtyError("grantpt", "");

    close(fd);
  } else
    getPtyError(
#ifdef TW_HAVE_GETPT
        "getpt", ""
#else
        "open", "/dev/ptmx"
#endif
    );
#else /* ! CONF_TERM_DEVPTS */
  static char pty_name[] = "/dev/pty??";
  static char tty_name[] = "/dev/tty??";
  int len = strlen(tty_name);
  char *c1, *c2;

  ptydev = pty_name;
  ttydev = tty_name;

#define PTYCHAR1 "pqrstuvwxyzabcde"
#define PTYCHAR2 "0123456789abcdef"
  for (c1 = PTYCHAR1; *c1; c1++) {
    ptydev[len - 2] = ttydev[len - 2] = *c1;
    for (c2 = PTYCHAR2; *c2; c2++) {
      ptydev[len - 1] = ttydev[len - 1] = *c2;
      if ((fd = open(ptydev, O_RDWR | O_NOCTTY)) >= 0) {
        if ((sfd = open(ttydev, O_RDWR | O_NOCTTY)) >= 0)
          /* access(ttydev, R_OK|W_OK) won't do as it checks against REAL uid */
          goto Found;
        close(fd);
      }
    }
  }
  fprintf(stderr, "twterm: failed to get a pty/tty pseudo-tty pair\n");

#endif
  return tfalse;

Found:
  fcntl(fd, F_SETFL, O_NDELAY);
  fcntl(fd, F_SETFD, FD_CLOEXEC);
  ttyfd = sfd;
  ptyfd = fd;
  return ttrue;
}

static gid_t tty_grgid;

gid_t getTtyGrgid(void) {
  struct group *gr;

  if (!tty_grgid) {
    if ((gr = getgrnam("tty")))
      tty_grgid = gr->gr_gid;
    else
      tty_grgid = (gid_t)-1;
  }
  return tty_grgid;
}

/* 2. Fixup permission for pty master/slave pairs and set window size on slave */
static byte fixupPty(dat X, dat Y) {
  struct winsize wsiz;
  uid_t id = getuid();
  gid_t tty_gid = getTtyGrgid();

  wsiz.ws_col = X;
  wsiz.ws_row = Y;
  wsiz.ws_xpixel = 0;
  wsiz.ws_ypixel = 0;
  (void)ioctl(ttyfd, TIOCSWINSZ, &wsiz);

  if (tty_gid != (gid_t)-1 &&
#ifndef CONF_TERM_DEVPTS
      chown(ptydev, id, 0) == 0 && chmod(ptydev, 0600) == 0 &&
#endif
      chown(ttydev, id, tty_gid) == 0 && chmod(ttydev, 0620) == 0)

    return ttrue;
  return tfalse;
}

/* 3. setup sane tty speed, input mode, output mode, conversions and flags */
static byte fixupTty(int fd) {
  struct termios ttyb = {};
  byte ok = tty_getioctl(fd, &ttyb) == 0;

  ttyb.c_cc[VINTR] = CINTR;
  ttyb.c_cc[VQUIT] = CQUIT;
  ttyb.c_cc[VERASE] = CERASE;
  ttyb.c_cc[VKILL] = CKILL;
  ttyb.c_cc[VSTART] = CSTART;
  ttyb.c_cc[VSTOP] = CSTOP;
  ttyb.c_cc[VSUSP] = CSUSP;
#ifdef VDSUSP
  ttyb.c_cc[VDSUSP] = VDISABLE;
#endif
#ifdef VREPRINT
  ttyb.c_cc[VREPRINT] = CRPRNT;
#endif
#ifdef VDISCRD
  ttyb.c_cc[VDISCRD] = CFLUSH;
#endif
#ifdef VWERSE
  ttyb.c_cc[VWERSE] = CWERASE;
#endif
#ifdef VLNEXT
  ttyb.c_cc[VLNEXT] = CLNEXT;
#endif

  ttyb.c_cc[VEOF] = CEOF;
  ttyb.c_cc[VEOL] = VDISABLE;
#ifdef VEOL2
  ttyb.c_cc[VEOL2] = VDISABLE;
#endif
#ifdef VSWTC
  ttyb.c_cc[VSWTC] = VDISABLE;
#endif
#ifdef VSWTCH
  ttyb.c_cc[VSWTCH] = VDISABLE;
#endif
  ttyb.c_cc[VMIN] = 1;
  ttyb.c_cc[VTIME] = 0;

  if (ok) {
    /* tweak as needed */

    /* input modes */
    ttyb.c_iflag |= TW_TTY_IFLAG_ON;
    ttyb.c_iflag &= ~TW_TTY_IFLAG_OFF;

    /* output modes */
    ttyb.c_oflag |= TW_TTY_OFLAG_ON;
    ttyb.c_oflag &= ~TW_TTY_OFLAG_OFF;

    /* control modes */
    ttyb.c_cflag |= TW_TTY_CFLAG_ON;
    ttyb.c_cflag &= ~TW_TTY_CFLAG_OFF;

    /* line discipline modes */
    ttyb.c_lflag |= TW_TTY_LFLAG_ON;
    ttyb.c_lflag &= ~TW_TTY_LFLAG_OFF;

  } else {
    /* full initialization */

    ttyb.c_iflag = TW_TTY_IFLAG_ON;                      /* input modes */
    ttyb.c_oflag = TW_TTY_OFLAG_ON;                      /* output modes */
    ttyb.c_cflag = TW_TTY_CFLAG_ON | TW_TTY_CFLAG_SPEED; /* control modes */
    ttyb.c_lflag = TW_TTY_LFLAG_ON;                      /* line discipline modes */
  }
  return tty_setioctl(fd, &ttyb) == 0 ? ttrue : tfalse;
}

/* 4. close all fds except tty_fd_to_dup: duplicate it on fds 0, 1 and 2 */
void closeAllFds(int tty_fd_to_dup); /* in util.h */

/* 5. Establish fd 0 as controlling teletype for new session and switch to it */
static byte switchToTty(void) {

  pid_t pid = setsid();
  if (pid < 0)
    return tfalse;

#ifdef TIOCSCTTY
  ioctl(0, TIOCSCTTY, 0);
#endif

/* set process group */
#if defined(TW_HAVE_TCSETPGRP)
  tcsetpgrp(0, pid);
#elif defined(TIOCSPGRP)
  ioctl(0, TIOCSPGRP, &pid);
#endif

  return ttrue;
}

/* 5. fork() a program in a pseudo-teletype */
int Spawn(twindow Window, pid_t *ppid, dat X, dat Y, const char *arg0, const char *const *argv) {

  TwGetPrivileges();

  if (!getPty()) {
    TwDropPrivileges();
    return tfalse;
  }
  (void)fixupPty(X, Y);
  (void)fixupTty(ttyfd);

  TwDropPrivileges();

  switch ((*ppid = fork())) {
  case -1:
    /* failed */
    close(ptyfd);
    ptyfd = -1;
    break;
  case 0:
    /* child */
    signal(SIGHUP, SIG_DFL); // restore default SIGHUP behavior
    closeAllFds(ttyfd);
    if (switchToTty()) {
      execvp(arg0, (char *const *)argv);
    }
    exit(1);
    break;
  default:
    /* father */
    break;
  }
  close(ttyfd);

  return ptyfd;
}
