/*
 *  hw.c  --  common functions to use HW/hw_* displays
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

/*
 * this is a very sensible part of code, as it must
 * correctly link both against twin and against twdisplay,
 * picking the correct versions of functions like
 * FlushHW(), PanicHW(), AllHWCanDragAreaNow(), DragAreaHW(), etc.
 *
 * This for example rules out calling methods like Delete()
 * or referencing the variable All.
 */

#include "twin.h"

#ifdef TW_HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef TW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef TW_HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#ifdef TW_HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef TW_HAVE_SYS_TTYDEFAULTS_H
#include <sys/ttydefaults.h>
#else
#include "my_ttydefaults.h"
#endif

#include "tty_ioctl.h"

#include "alloc.h"
#include "algo.h"
#include "common.h"
#include "hw.h"

Tdisplay HW, DisplayHWCTTY;

tcell *Video, *OldVideo;

bool NeedOldVideo, CanDragArea;
bool ExpensiveFlushVideo, ValidOldVideo;
byte NeedHW;

dat (*ChangedVideo)[2][2];
bool ChangedVideoFlag, ChangedVideoFlagAgain;
bool QueuedDrawArea2FullScreen;

dat DisplayWidth, DisplayHeight;

udat CursorX, CursorY;
uldat CursorType;

struct termios ttysave;

VOLATILE bool GotSignals;
static VOLATILE bool GotSignalWinch;
static VOLATILE bool GotSignalChild;
static VOLATILE bool GotSignalHangup;

static void SignalWinch(int n) NOTHROW {
  (void)n;
  GotSignals = GotSignalWinch = true;
  signal(SIGWINCH, SignalWinch);
}

static void HandleSignalWinch(void) {
  GotSignalWinch = false;
  if (DisplayHWCTTY && DisplayHWCTTY != HWCTTY_DETACHED && DisplayHWCTTY->DisplayIsCTTY) {
    ResizeDisplayPrefer(DisplayHWCTTY);
  }
}

static void SignalChild(int n) NOTHROW {
  (void)n;
  GotSignals = GotSignalChild = true;
  signal(SIGCHLD, SignalChild);
}

static void HandleSignalChild(void) {
  pid_t pid;
  int status;
  GotSignalChild = false;
  while ((pid = Tw_wait3(&status, WNOHANG, (struct rusage *)0)) != 0 && pid != (pid_t)-1) {
    if (WIFEXITED(status) || WIFSIGNALED(status)) {
      RemotePidIsDead(pid);
    }
  }
}

/*
 * got a SIGHUP. shutdown the display on controlling tty, if any
 */
static void SignalHangup(int n) {
  (void)n;
  GotSignals = GotSignalHangup = true;
  signal(SIGHUP, SignalHangup);
}

static void HandleSignalHangup(void) {
  GotSignalHangup = false;
  if (DisplayHWCTTY && DisplayHWCTTY != HWCTTY_DETACHED && DisplayHWCTTY->DisplayIsCTTY) {

    DisplayHWCTTY->NeedHW |= NeedPanicHW, NeedHW |= NeedPanicHW;
  }
}

void HandleSignals(void) {
  GotSignals = tfalse;
  if (GotSignalWinch) {
    HandleSignalWinch();
  }
  if (GotSignalChild) {
    HandleSignalChild();
  }
  if (GotSignalHangup) {
    HandleSignalHangup();
  }
}

#ifndef TW_DONT_TRAP_SIGNALS
static void SignalFatal(int n) {
  sigset_t s, t;

  signal(n, SIG_DFL);

  sigemptyset(&s);
  sigaddset(&s, n);
  sigprocmask(SIG_BLOCK, &s, &t);

  Quit(-n);

  kill(getpid(), n);
}
#endif

static const int signals_ignore[] = {
    SIGUSR1,   SIGUSR2, SIGPIPE, SIGALRM,
#ifdef SIGIO
    SIGIO,
#endif
#if defined(SIGPOLL) && (!defined(SIGIO) || SIGPOLL != SIGIO)
    SIGPOLL,
#endif
#if defined(SIGLOST) && (!defined(SIGIO) || SIGLOST != SIGIO) &&                                   \
    (!defined(SIGPWR) || SIGLOST != SIGPWR)
    SIGLOST,
#endif
#ifdef SIGPROF
    SIGPROF,
#endif
#ifdef SIGSYS
    SIGSYS,
#endif
#if defined(SIGUNUSED) && (!defined(SIGSYS) || SIGUNUSED != SIGSYS)
    SIGUNUSED,
#endif
#ifdef SIGTRAP
    SIGTRAP,
#endif
#ifdef SIGURG
    SIGURG,
#endif
#ifdef SIGVTALRM
    SIGVTALRM,
#endif
};

static const int signals_fatal[] = {
    SIGINT,    SIGQUIT, SIGILL, SIGABRT, SIGBUS, SIGFPE, SIGSEGV, SIGTERM,
#ifdef SIGXPCU
    SIGXCPU,
#endif
#ifdef SIGXFSZ
    SIGXFSZ,
#endif
#ifdef SIGPWR
    SIGPWR,
#endif
#ifdef SIGEMT
    SIGEMT,
#endif
#if defined(SIGSTKFLT) && (!defined(SIGEMT) || SIGSTKFLT != SIGEMT)
    SIGSTKFLT,
#endif
};

bool InitSignals(void) NOTHROW {
  uldat i;
  signal(SIGWINCH, SignalWinch);
  signal(SIGCHLD, SignalChild);
  signal(SIGHUP, SignalHangup);
  for (i = 0; i < N_OF(signals_ignore); i++) {
    signal(signals_ignore[i], SIG_IGN);
  }
  for (i = 0; i < N_OF(signals_fatal); i++) {
    signal(signals_fatal[i], SignalFatal);
  }
  return true;
}

void QuitSignals(void) NOTHROW {
  uldat i;
  signal(SIGWINCH, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  for (i = 0; i < N_OF(signals_ignore); i++) {
    signal(signals_ignore[i], SIG_IGN);
  }
  for (i = 0; i < N_OF(signals_fatal); i++) {
    signal(signals_fatal[i], SIG_DFL);
  }
}

void AllDefaultSignals(void) NOTHROW {
  uldat i;
  signal(SIGWINCH, SIG_DFL);
  signal(SIGCHLD, SIG_DFL);
  signal(SIGHUP, SIG_DFL);
  for (i = 0; i < N_OF(signals_ignore); i++) {
    signal(signals_ignore[i], SIG_DFL);
  }
  for (i = 0; i < N_OF(signals_fatal); i++) {
    signal(signals_fatal[i], SIG_DFL);
  }
}

void MoveToXY(dat x, dat y) NOTHROW {
  CursorX = x;
  CursorY = y;
}

void SetCursorType(uldat type) NOTHROW {
  if ((type & 0xF) == 0) {
    type |= LINECURSOR;
  } else if ((type & 0xF) > SOLIDCURSOR) {
    type = (type & ~(uldat)0xF) | SOLIDCURSOR;
  }
  CursorType = type;
}

void NeedRedrawVideo(Tdisplay hw, dat Left, dat Up, dat Right, dat Down) NOTHROW {
  if (hw->RedrawVideo) {
    hw->RedrawLeft = Min2(hw->RedrawLeft, Left);
    hw->RedrawUp = Min2(hw->RedrawUp, Up);
    hw->RedrawRight = Max2(hw->RedrawRight, Right);
    hw->RedrawDown = Max2(hw->RedrawDown, Down);
  } else {
    hw->RedrawVideo = true;
    hw->RedrawLeft = Left;
    hw->RedrawUp = Up;
    hw->RedrawRight = Right;
    hw->RedrawDown = Down;
  }
}

/*
 * The following functions are quite os-independent,
 * but they are part of the functionality implemented by hw.c
 * so they are included here.
 *
 * Also, some implementations might hw accel some of these...
 */

/*
 * for better cleannes, DirtyVideo()
 * should be used *before* actually touching Video[]
 */
void DirtyVideo(dat Xstart, dat Ystart, dat Xend, dat Yend) NOTHROW {
  dat s0, s1, e0, e1, len, min;
  byte i;

  if (QueuedDrawArea2FullScreen || Xstart > Xend || Xstart >= DisplayWidth || Xend < 0 ||
      Ystart > Yend || Ystart >= DisplayHeight || Yend < 0)
    return;

  Xstart = Max2(Xstart, 0);
  Ystart = Max2(Ystart, 0);
  Xend = Min2(Xend, DisplayWidth - 1);
  Yend = Min2(Yend, DisplayHeight - 1);

  ChangedVideoFlag = ChangedVideoFlagAgain = ttrue;

  for (; Ystart <= Yend; Ystart++) {
    s0 = ChangedVideo[Ystart][0][0];
    e0 = ChangedVideo[Ystart][0][1];
    s1 = ChangedVideo[Ystart][1][0];
    e1 = ChangedVideo[Ystart][1][1];

    /* decide how to rearrange the slots to include Xstart..Xend */

    if (s0 == -1) {
      /* trivial ! */
      ChangedVideo[Ystart][0][0] = Xstart;
      ChangedVideo[Ystart][0][1] = Xend;
      continue;
    }

    if ((s0 != -1 && Xstart >= s0 && Xend <= e0) || (s1 != -1 && Xstart >= s1 && Xend <= e1))
      /* nothing to do :) */
      continue;

    if (s1 == -1) {
      /*
       * this is quite simple.
       * if s0..e0 and Xstart..Xend intersect or touch, merge.
       * else just put them in s0,e0 and s1,e1 :
       * there is no point in forcing a merge now if it costs cells,
       * we'll merge (if needed) when a third dirty segment appears.
       */
      if (Xstart <= e0 + 1 && Xend + 1 >= s0) {
        ChangedVideo[Ystart][0][0] = Min2(s0, Xstart);
        ChangedVideo[Ystart][0][1] = Max2(e0, Xend);
      } else if (Xstart < s0) {
        ChangedVideo[Ystart][0][0] = Xstart;
        ChangedVideo[Ystart][0][1] = Xend;
        ChangedVideo[Ystart][1][0] = s0;
        ChangedVideo[Ystart][1][1] = e0;
      } else {
        ChangedVideo[Ystart][1][0] = Xstart;
        ChangedVideo[Ystart][1][1] = Xend;
      }
      continue;
    }

    /*
     * now the hairy thing... there are 5 possible actions:
     *
     * 0. put Xstart..Xend in slot 0 and merge s0..e0 with s1..e1
     * 1. merge Xstart..Xend to s0..e0
     * 2. merge all together, freeing slot 1
     * 3. merge Xstart..Xend to s1..e1
     * 4. put Xstart..Xend in slot 1 and merge s1..e1 with s0..e0
     *    4. is the same as 0., except for slot inversion.
     *
     * do it the brutal way: calculate the 5 cases and choose
     * the one resulting in less dirty cells.
     *
     * we do no checks about the relative ordering of s0..e0, s1..e1, Xstart..Xend
     * and use Min2(sA, sB)..Max2(eA, eB) for merging.
     * the less dirty pattern automatically selects the best solution
     * which is non intersecting, and in the 0.,4. cases we choose manually
     * to preserve ordering between slots.
     *
     * all lenghts are decreased by 2 to avoid always doing (...)+2
     */

    /*0,4*/ min = Xend - Xstart + e1 - s0;
    i = 0;
    /* 1 */ len = Max2(Xend, e0) - Min2(Xstart, s0) + e1 - e0;
    if (len < min)
      min = len, i = 1;
    /* 2 */ len = Max2(Xend, e1) - Min2(Xstart, s0) - 1;
    if (len <= min)
      min = len, i = 2; /* prefer this if equal */
    /* 3 */ len = e0 - s0 + Max2(Xend, e1) - Min2(Xstart, s1);
    if (len < min)
      min = len, i = 3;

    switch (i) {
    case 0:
      i = Xstart > s0;
      ChangedVideo[Ystart][!i][0] = s0;
      ChangedVideo[Ystart][!i][1] = e1;
      ChangedVideo[Ystart][i][0] = Xstart;
      ChangedVideo[Ystart][i][1] = Xend;
      break;
    case 1:
      ChangedVideo[Ystart][0][0] = Min2(Xstart, s0);
      ChangedVideo[Ystart][0][1] = Max2(Xend, e0);
      break;
    case 2:
      ChangedVideo[Ystart][0][0] = Min2(Xstart, s0);
      ChangedVideo[Ystart][0][1] = Max2(Xend, e1);
      ChangedVideo[Ystart][1][0] = -1;
      break;
    case 3:
      ChangedVideo[Ystart][1][0] = Min2(Xstart, s1);
      ChangedVideo[Ystart][1][1] = Max2(Xend, e1);
      break;
    default:
      break;
    }
  }
}

static void Video2OldVideo(dat Xstart, dat Ystart, dat Xend, dat Yend) NOTHROW {
  tcell *src, *dst;
  uldat xc, yc;

  if (Xstart > Xend || Xstart >= DisplayWidth || Xend < 0 || Ystart > Yend ||
      Ystart >= DisplayHeight || Yend < 0)
    return;
  Xstart = Max2(Xstart, 0);
  Ystart = Max2(Ystart, 0);
  Xend = Min2(Xend, DisplayWidth - 1);
  Yend = Min2(Yend, DisplayHeight - 1);

  yc = Yend - Ystart + 1;
  xc = sizeof(tcell) * (uldat)(Xend - Xstart + 1);
  src = Video + Xstart + Ystart * (ldat)DisplayWidth;
  dst = OldVideo + Xstart + Ystart * (ldat)DisplayWidth;

  while (yc--) {
    CopyMem(src, dst, xc);
    src += DisplayWidth;
    dst += DisplayWidth;
  }
}

/* An important Video function: copy a rectangle. It must be _*FAST*_ !! */
void DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  dat DstRgt = DstLeft + (Rgt - Left), DstDwn = DstUp + (Dwn - Up);
  ldat len, count;
  tcell *src = Video, *dst = Video;
  byte Accel;

  if (QueuedDrawArea2FullScreen)
    return;

  count = Dwn - Up + 1;
  len = (ldat)(Rgt - Left + 1) * sizeof(tcell);

  /* if HW can do the scroll, use it instead of redrawing */

  /* HACK : for consistency problems, we actually drag only if all HW can drag */

  Accel = AllHWCanDragAreaNow(Left, Up, Rgt, Dwn, DstLeft, DstUp);

  if (Accel) {
    FlushHW();
    DragAreaHW(Left, Up, Rgt, Dwn, DstLeft, DstUp);
  } else
    DirtyVideo(DstLeft, DstUp, DstRgt, DstDwn);

  /* do the drag inside Video[] */

  if (DstUp <= Up) {
    src += Left + Up * (ldat)DisplayWidth;
    dst += DstLeft + DstUp * (ldat)DisplayWidth;
    if (DstUp != Up) {
      /* copy forward */
      while (count--) {
        CopyMem(src, dst, len);
        dst += DisplayWidth;
        src += DisplayWidth;
      }
    } else if (Left != DstLeft) {
      /* the tricky case: DstUp == Up */
      /* copy forward, but with memmove() */
      while (count--) {
        MoveMem(src, dst, len);
        dst += DisplayWidth;
        src += DisplayWidth;
      }
    }
  } else if (DstUp > Up) {
    /* copy backward */
    src += Left + Dwn * (ldat)DisplayWidth;
    dst += DstLeft + DstDwn * (ldat)DisplayWidth;
    while (count--) {
      CopyMem(src, dst, len);
      dst -= DisplayWidth;
      src -= DisplayWidth;
    }
  }

  if (Accel && NeedOldVideo)
    Video2OldVideo(DstLeft, DstUp, DstRgt, DstDwn);
}

bool InitTtysave(void) NOTHROW {
  int fd = open("/dev/tty", O_RDWR | O_NOCTTY);
  InitTtyStruct(fd, ttysave);
  if (fd >= 0) {
    close(fd);
  }
  return true;
}

void InitTtyStruct(int fd, termios &ttyb) NOTHROW {

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
}
