/*
 *  hw.c  --  skeleton and common functions for all HW/hw_* display implementations
 *
 *  Copyright (C) 1993-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>

#include <termios.h>
#include <sys/ttydefaults.h>
#ifndef VDISABLE
# ifdef _POSIX_VDISABLE
#  define VDISABLE	_POSIX_VDISABLE
# else
#  define VDISABLE	255
# endif
#endif

#include "twin.h"
#include "data.h"
#include "main.h"
#include "methods.h"
#include "draw.h"
#include "remote.h"

#include "resize.h"
#include "wm.h"
#include "hw.h"
#include "hw_private.h"
#include "util.h"
#include "socket.h"

#ifdef CONF_MODULES
# include "dl.h"
#endif

#include "libTwkeys.h"
#include "hotkey.h"

/* common data */

display_hw *HW;

hwattr *Video, *OldVideo;

byte ValidOldVideo;

dat (*ChangedVideo)[2][2];
byte ChangedVideoFlag;
byte ChangedVideoFlagAgain;
byte ChangedMouseFlag;

static udat savedScreenWidth, savedScreenHeight;
udat ScreenWidth = 100, ScreenHeight = 30;


FILE *errFILE;

dat Last_x, Last_y;


static dat AccelVideo[4] = { MAXDAT, MAXDAT, MINDAT, MINDAT };
byte   StrategyFlag;
frac_t StrategyDelay = (frac_t)0;

struct termios ttysave;

/* common functions */


void saveDisplaySize(void) {
    savedScreenWidth = ScreenWidth;
    savedScreenHeight = ScreenHeight;
}











#include "HW/hw_null.h"

#ifdef CONF_HW_X11
# include "HW/hw_X11.h"
#endif

#ifdef CONF_HW_TWIN
# include "HW/hw_twin.h"
#endif

#ifdef CONF_HW_TTY
# include "HW/hw_tty.h"
#endif

#ifdef CONF_HW_GGI
# include "HW/hw_ggi.h"
#endif









#ifndef DONT_TRAP_SIGNALS
static void SignalPanic(int n) {
    sigset_t s, t;

    signal(n, SIG_DFL);
    
    sigemptyset(&s);
    sigaddset(&s, n);
    sigprocmask(SIG_BLOCK, &s, &t);
    
    Quit(-n);
    
    kill(getpid(), n);
}
#endif

static void SignalChild(int n) {
    while (wait4((pid_t)-1, (int *)0, WNOHANG, (struct rusage *)0) > 0)
	;
    signal(SIGCHLD, SignalChild);
}

void InitSignals(void) {
    signal(SIGCHLD, SignalChild);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGIO,   SIG_IGN);
#ifndef DONT_TRAP_SIGNALS
    signal(SIGHUP,  SignalPanic);
    signal(SIGINT,  SignalPanic);
    signal(SIGQUIT, SignalPanic);
    signal(SIGILL,  SignalPanic);
    signal(SIGABRT, SignalPanic);
    signal(SIGBUS,  SignalPanic);
    signal(SIGFPE,  SignalPanic);
    signal(SIGSEGV, SignalPanic);
    signal(SIGTERM, SignalPanic);
    signal(SIGXCPU, SignalPanic);
    signal(SIGXFSZ, SignalPanic);
    signal(SIGPWR,  SignalPanic);
#endif
}

void DevNullStderr(void) {
    int fd;
    
    if ((fd = open("/dev/null", O_RDWR)) >= 0) {
	close(2);
	if (dup2(fd, 2) < 0)
	    dup2(1, 2);
	close(fd);
    }
}

static byte check4(byte *s, byte *arg) {
    if (arg && strncmp(s, arg, strlen(s))) {
	fprintf(stderr, "twin: `-hw=%s' given, skipping `-hw=%s' display driver.\n",
		arg, s);
	return FALSE;
    } else if (arg)
	fprintf(stderr, "twin: trying given `-hw=%s' display driver.\n", s);
    else
	fprintf(stderr, "twin: autoprobing `-hw=%s' display driver.\n", s);
    return TRUE;
}

#ifdef CONF_MODULES
static module *ModuleHW;

display_hw *module_InitHW(byte *arg) {
    byte *name, *tmp;
    uldat len;
    display_hw *(*InitD)(byte *);
    display_hw *hw = (display_hw *)0;
    
    if (!arg)
	return hw;
    
    len = strlen(arg);
    name = memchr(arg, '@', len);
    tmp = memchr(arg, ',', len);
    if (tmp && (!name || tmp < name))
	name = tmp;
    if (name)
	len = name - arg;
    
    if ((name = AllocMem(len + 10))) {
	sprintf(name, "HW/hw_%.*s.so", (int)len, arg);
			
	ModuleHW = DlLoadAny(len+9, name);
	
	FreeMem(name);
	    
	if (ModuleHW) {
	    fprintf(errFILE, "twin: starting display driver module `HW/hw_%.*s.so'...\n", (int)len, arg);
	    
	    if ((InitD = ModuleHW->Private) && (hw = InitD(arg))) {
		fprintf(errFILE, "twin: ...module `HW/hw_%.*s.so' successfully started.\n", (int)len, arg);
		return hw;
	    }
	    Delete(ModuleHW);
	}
    }

    if (ModuleHW) {
	fprintf(errFILE, "twin: ...module `HW/hw_%.*s.so' failed to start.\n", (int)len, arg);
	ModuleHW = (module *)0;
    } else
	fprintf(errFILE, "twin: unable to load display driver module `HW/hw_%.*s.so' :\n"
		"      %s\n", (int)len, arg, ErrStr);
    
    return hw;
}
#endif /* CONF_MODULES */

display_hw *warn_NoHW(char *arg, uldat tried) {
#ifdef CONF_MODULES
    if (!tried && !arg)
	    fputs("twin: no display driver compiled into twin.\n"
		  "      please run as `twin -hw=<display>'\n", errFILE);
#endif
    
    fprintf(errFILE, "\ntwin: \033[1m  ALL  DISPLAY  DRIVERS  FAILED%s%s%c\033[0m\n",
	    arg ? "  for  `-hw=" : "", arg ? arg : "", arg ? '\'' : '.');

    /*
     * we may try forking in the background with null display HW here...
     * but we must consider that if InitHW gets called by `twattach',
     * we *already* fallback to null display HW... so maybe it's not so useful.
     */
    return (display_hw *)0;
}

/*
 * InitHW runs InitXXX() functions, starting from best setup
 * and falling back in case some of them fails.
 */
byte InitHW(byte *arg) {
    uldat tried = 0;
    display_hw *hw;
    byte success;
    
    if (arg && !strncmp(arg, "-hw=", 4))
	arg += 4;
    else
	arg = NULL;

    if (!errFILE)
	errFILE = stderr;
    
    success =
#ifdef CONF_HW_X11
	(check4("X", arg) && (tried++, hw = X11_InitHW(arg))) ||
#endif
#ifdef CONF_HW_TWIN
	(check4("twin", arg) && (tried++, hw = TW_InitHW(arg))) ||
#endif
#ifdef CONF_HW_TTY
	(check4("tty", arg) && (tried++, hw = tty_InitHW(arg))) ||
#endif
#ifdef CONF_HW_GGI
	(check4("ggi", arg) && (tried++, hw = GGI_InitHW(arg))) ||
#endif
#ifdef CONF_MODULES
	(hw = module_InitHW(arg)) ||
#endif
	(hw = warn_NoHW(arg, tried));
    
    fflush(errFILE);
    fflush(stdout);
    
    if (success) {
	if (arg) arg -= 4;
	if (arg != origHW) {
	    if (origHW) free(origHW);
	    origHW = arg ? (byte *)strdup(arg) : arg;
	}
	HW = hw;
	ResizeDisplay();
	DrawArea(FULLSCREEN);
	UpdateCursor();
    }
    return success;
}

void QuitHW(void) {
    if (HW)
	HW->QuitHW();
#ifdef CONF_MODULES
    if (ModuleHW)
	Delete(ModuleHW), ModuleHW = (module *)0;
#endif
}

byte doAttachHW(byte len, byte *arg, byte tostderr) {
    byte buff[SMALLBUFF];
    byte ret = FALSE;
    
    if (HW->QuitHW == NoOp) {
	if (len) {
	    len = Min2(len, SMALLBUFF-1);
	    CopyMem(arg, buff, len);
	    buff[len] = '\0';
	    arg = buff;
	} else
	    arg = NULL;
	
	if (savedScreenWidth)
	    ScreenWidth = savedScreenWidth, savedScreenWidth = 0;
	if (savedScreenHeight)
	    ScreenHeight = savedScreenHeight, savedScreenHeight = 0;
	
	errFILE = tostderr ? stderr : stdout;
	ret = InitHW(arg);
	errFILE = stderr;
    }
    return ret;
}


/* cannot fail */
void doDetachHW(void) {
    static byte NeverDetached = TRUE;

    /* try to fire up the Socket Server ... */
#if defined (CONF_MODULES) && !defined(CONF_SOCKET)
    (void)DlLoad(SocketSo);
#endif
    
    QuitHW();

    if (NeverDetached) {
	/*
	 * we fork in the background
	 * so that the shell we were started from
	 * can realize we have finished with the tty
	 */
	switch (fork()) {
	  case 0: /* child: continue */
	    NeverDetached = FALSE;
	    setsid();
	    break;
	  case -1:
	    break;
	  default: /* parent: exit */
	    exit(0);
	}
    }
    
    HW = null_InitHW(NULL); /* cannot fail */

    if (All->attach != NOSLOT) {
	KillSlot(All->attach);
	All->attach = NOSLOT;
    }
}


/*
 * The following functions are quite os-independent,
 * but they are part of the functionality implemented by hw.c
 * so they are included here.
 * 
 * Also, some implementations might hw accel some of these...
 */


byte StdAddEventMouse(udat CodeMsg, udat ShiftFlags, udat Code, dat MouseX, dat MouseY) {
    msg *Msg;
    event_mouse *Event;
    
    if ((Code & ANY_ACTION_MOUSE)==DRAG_MOUSE
	&& (Msg = WM_MsgPort->LastMsg)
	&& Msg->Type==MSG_MOUSE
	&& (Event=&Msg->Event.EventMouse)
	&& Event->FullShiftFlags==ShiftFlags
	&& Event->Code==Code) {
	Event->X=MouseX;
	Event->Y=MouseY;
	return TRUE;
    }
    if ((Msg=Do(Create,Msg)(FnMsg, CodeMsg, sizeof(event_mouse)))) {
	Event=&Msg->Event.EventMouse;
	Event->FullShiftFlags=ShiftFlags;
	Event->Code=Code;
	Event->X=MouseX;
	Event->Y=MouseY;
	SendMsg(WM_MsgPort, Msg);
	return TRUE;
    }
    return FALSE;
}

void SendHotKey(window *Window) {
    event_keyboard *Event;
    msg *Msg;
    byte Len, *Seq = NULL;

    
    /* the following is REALLY heavy on the compiler... but it should optimize drastically. */
	
#define IS(key, len, seq) if (TW_##key == HOT_KEY) Len = len, Seq = seq;
#  include "hw_keys.h"
#undef IS
	
    if (!Seq || !*Seq)
	return;
	
    if ((Msg=Do(Create,Msg)(FnMsg, MSG_WINDOW_KEY, Len + sizeof(event_keyboard)))) {
	Event = &Msg->Event.EventKeyboard;

	Event->Window = Window;
	Event->FullShiftFlags = All->FullShiftFlags;
	Event->Code = HOT_KEY;
	Event->SeqLen = Len;
	CopyMem(Seq, Event->AsciiSeq, Len);
	Event->AsciiSeq[Len] = '\0'; /* terminate string with \0 */
	SendMsg(Window->Menu->MsgPort, Msg);
    }
}

void KeyboardEventCommon(udat Code, udat Len, byte *Seq) {
    event_keyboard *Event;
    msg *Msg;

    if ((Msg=Do(Create,Msg)(FnMsg, MSG_KEY, Len + sizeof(event_keyboard)))) {
	Event = &Msg->Event.EventKeyboard;
	Event->FullShiftFlags = All->FullShiftFlags;
	    
	Event->Code = Code;
	Event->SeqLen = Len;
	CopyMem(Seq, Event->AsciiSeq, Len);
	Event->AsciiSeq[Len] = '\0'; /* terminate string with \0 */
	SendMsg(WM_MsgPort, Msg);
    }
}

void MouseEventCommon(dat x, dat y, dat dx, dat dy, udat IdButtons) {
    dat prev_x, prev_y;
    udat Buttons, OldButtons, Overload;
    mouse_state *OldState;
    udat result;

    OldState=All->MouseState;
    OldButtons=OldState->keys;
    prev_x = OldState->x;
    prev_y = OldState->y;

    x = Max2(x, 0); x = Min2(x, ScreenWidth - 1);
    OldState->delta_x = x == 0 ? Min2(dx, 0) : x == ScreenWidth - 1 ? Max2(dx, 0) : 0;

    y = Max2(y, 0); y = Min2(y, ScreenHeight - 1);
    OldState->delta_y = y == 0 ? Min2(dy, 0) : y == ScreenHeight - 1 ? Max2(dy, 0) : 0;
	
    if (x != prev_x || y != prev_y)
	ChangedMouseFlag = TRUE;

    OldState->x = x;
    OldState->y = y;
    
    Buttons=(byte)0;
    Overload=All->MouseOverload;
    
    if (IdButtons & HOLD_LEFT)
	Buttons |= HOLD << (Overload>>LEFT & OV_ANY);
    if (IdButtons & HOLD_MIDDLE)
	Buttons |= HOLD << (Overload>>MIDDLE & OV_ANY);
    if (IdButtons & HOLD_RIGHT)
	Buttons |= HOLD << (Overload>>RIGHT & OV_ANY);
    
    OldState->keys=Buttons;
    
    if (Buttons != OldButtons || (OldButtons && (x != prev_x || y != prev_y))) {
	
	if (OldButtons && (x != prev_x || y != prev_y)) {
	    if (!StdAddEventMouse(MSG_MOUSE, All->FullShiftFlags, DRAG_MOUSE | OldButtons, x, y))
		Error(NOMEMORY);
	}
	if ((Buttons & HOLD_LEFT) != (OldButtons & HOLD_LEFT)) {
	    result = (Buttons & HOLD_LEFT ? DOWN_LEFT : RELEASE_LEFT) | (OldButtons &= ~HOLD_LEFT);
	    OldButtons |= Buttons & HOLD_LEFT;
	    if (!StdAddEventMouse(MSG_MOUSE, All->FullShiftFlags, result, x, y))
		Error(NOMEMORY);
	}
	if ((Buttons & HOLD_MIDDLE) != (OldButtons & HOLD_MIDDLE)) {
	    result = (Buttons & HOLD_MIDDLE ? DOWN_MIDDLE : RELEASE_MIDDLE) | (OldButtons &= ~HOLD_MIDDLE);
	    OldButtons |= Buttons & HOLD_MIDDLE;
	    if (!StdAddEventMouse(MSG_MOUSE, All->FullShiftFlags, result, x, y))
		Error(NOMEMORY);
	}
	if ((Buttons & HOLD_RIGHT) != (OldButtons & HOLD_RIGHT)) {
	    result = (Buttons & HOLD_RIGHT ? DOWN_RIGHT : RELEASE_RIGHT) | (OldButtons &= ~HOLD_RIGHT);
	    OldButtons |= Buttons & HOLD_RIGHT;
	    if (!StdAddEventMouse(MSG_MOUSE, All->FullShiftFlags, result, x, y))
		Error(NOMEMORY);
	}
    }
}

#if 0 /* not used for now */
byte GetShortShiftFlags(void) {
    
    udat FullShiftFlags=All->FullShiftFlags;
    
    return FullShiftFlags & FULL_LEFT_CTRL_PRESSED
	? LEFT_CTRL
	:	FullShiftFlags & FULL_RIGHT_CTRL_PRESSED
	? RIGHT_CTRL
	: FullShiftFlags & FULL_LEFT_ALT_PRESSED
	? LEFT_ALT
	: FullShiftFlags & FULL_RIGHT_ALT_PRESSED
	? RIGHT_ALT
	: (
	   FullShiftFlags & FULL_LEFT_SHIFT_PRESSED
	   ? LEFT_SHIFT
	   : FullShiftFlags & FULL_RIGHT_SHIFT_PRESSED
	   ? RIGHT_SHIFT
	   : (byte)0
	   )
	|
	(FullShiftFlags & FULL_CAPS_LOCK_ACTIVE ? CAPS_LOCK : (byte)0)
	|
	(FullShiftFlags & FULL_NUM_LOCK_ACTIVE ? NUM_LOCK : (byte)0)
	;
}
#endif /* 0 */

/* VideoFlipMouse is quite os-independent ;) */
void VideoFlipMouse(void) {
    uldat pos = (Last_x = All->MouseState->x) + (Last_y = All->MouseState->y) * ScreenWidth;
    hwattr h = Video[pos];
    hwcol c = ~HWCOL(h) ^ COL(HIGH,HIGH);

    Video[pos] = HWATTR( c, HWFONT(h) );
}

void SignalWinch(int n) {
    All->NeedHW |= NEEDResizeDisplay;
}

void ResizeDisplay(void) {
    screen *fScreen = All->FirstScreen;

    HW->DetectSize(&ScreenWidth, &ScreenHeight);
    
    if (!(Video = (hwattr *)realloc(Video, ScreenWidth*ScreenHeight*sizeof(hwattr))) ||
	(HW->NeedOldVideo && !(OldVideo = (hwattr *)realloc(OldVideo, ScreenWidth*ScreenHeight*sizeof(hwattr)))) ||
	!(ChangedVideo = (dat (*)[2][2])ReAllocMem(ChangedVideo, ScreenHeight*sizeof(dat)*4))) {
	
	fprintf(stderr, "twin: out of memory!\n");
	exit(1);
    }
    WriteMem(ChangedVideo, 0xff, ScreenHeight*sizeof(dat)*4);
    ValidOldVideo = FALSE;
		     
    while (fScreen) {
	fScreen->ScreenHeight = ScreenHeight;
        fScreen->ScreenWidth  = ScreenWidth;
	fScreen = fScreen->Next;
    }
    All->NeedHW &= ~NEEDResizeDisplay;
    
    signal(SIGWINCH, SignalWinch);
}

void FillVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, hwattr Attrib) {
    hwattr *pos;
    udat _xc, xc, yc, delta;
    
    if (Xstart > Xend || Xstart >= ScreenWidth || Xend < 0 ||
	Ystart > Yend || Ystart >= ScreenHeight || Yend < 0)
	return;
    Xstart = Max2(Xstart, 0);
    Ystart = Max2(Ystart, 0);
    Xend = Min2(Xend, ScreenWidth-1);
    Yend = Min2(Yend, ScreenHeight-1);
    DirtyVideo(Xstart, Ystart, Xend, Yend);

    yc = Yend - Ystart + 1;
    _xc = Xend - Xstart + 1;
    delta = ScreenWidth - _xc;
    pos = Video + Xstart + Ystart * ScreenWidth;
    
    while (yc--) {
	xc = _xc;
	while (xc--)
	    *pos++ = Attrib;
	pos += delta;
    }
}

void FillOldVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, hwattr Attrib) {
    hwattr *pos;
    udat _xc, xc, yc, delta;
    
    if (Xstart > Xend || Xstart >= ScreenWidth || Xend < 0 ||
	Ystart > Yend || Ystart >= ScreenHeight || Yend < 0)
	return;
    Xstart = Max2(Xstart, 0);
    Ystart = Max2(Ystart, 0);
    Xend = Min2(Xend, ScreenWidth-1);
    Yend = Min2(Yend, ScreenHeight-1);

    yc = Yend - Ystart + 1;
    _xc = Xend - Xstart + 1;
    delta = ScreenWidth - _xc;
    pos = OldVideo + Xstart + Ystart * ScreenWidth;
    
    while (yc--) {
	xc = _xc;
	while (xc--)
	    *pos++ = Attrib;
	pos += delta;
    }
}

static void Video2OldVideo(dat Xstart, dat Ystart, dat Xend, dat Yend) {
    hwattr *src, *dst;
    uldat xc, yc;
    
    if (Xstart > Xend || Xstart >= ScreenWidth || Xend < 0 ||
	Ystart > Yend || Ystart >= ScreenHeight || Yend < 0)
	return;
    Xstart = Max2(Xstart, 0);
    Ystart = Max2(Ystart, 0);
    Xend = Min2(Xend, ScreenWidth-1);
    Yend = Min2(Yend, ScreenHeight-1);

    yc = Yend - Ystart + 1;
    xc = sizeof(hwattr) * (Xend - Xstart + 1);
    src = Video + Xstart + Ystart * ScreenWidth;
    dst = OldVideo + Xstart + Ystart * ScreenWidth;
    
    while (yc--) {
	CopyMem(src, dst, xc);
	src += ScreenWidth;
	dst += ScreenWidth;
    }
}

/*
 * for better cleannes, DirtyVideo()
 * should be used *before* actually touching Video[]
 */
void DirtyVideo(dat Xstart, dat Ystart, dat Xend, dat Yend) {
    dat s0, s1, e0, e1, len, min;
    byte i;
    
    if (Xstart > Xend || Xstart >= ScreenWidth || Xend < 0 ||
	Ystart > Yend || Ystart >= ScreenHeight || Yend < 0)
	return;
    Xstart = Max2(Xstart, 0);
    Ystart = Max2(Ystart, 0);
    Xend = Min2(Xend, ScreenWidth-1);
    Yend = Min2(Yend, ScreenHeight-1);

    ChangedVideoFlag = ChangedVideoFlagAgain = TRUE;
    
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

	if ((s0 != -1 && Xstart >= s0 && Xend <= e0) ||
	    (s1 != -1 && Xstart >= s1 && Xend <= e1))
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
	    if (Xstart <= e0+1 && Xend+1 >= s0) {
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
	
	/*0,4*/ min = Xend-Xstart + e1-s0;			i = 0;
	/* 1 */ len = Max2(Xend,e0)-Min2(Xstart,s0) + e1-e0;	if (len <  min) min = len, i = 1;
	/* 2 */	len = Max2(Xend,e1)-Min2(Xstart,s0) - 1;	if (len <= min) min = len, i = 2; /* prefer this if equal */
	/* 3 */	len = e0-s0 + Max2(Xend,e1)-Min2(Xstart,s1);	if (len <  min) min = len, i = 3;
	
	switch (i) {
	  case 0:
	    i = Xstart > s0;
	    ChangedVideo[Ystart][!i][0] = s0;
	    ChangedVideo[Ystart][!i][1] = e1;
	    ChangedVideo[Ystart][ i][0] = Xstart;
	    ChangedVideo[Ystart][ i][1] = Xend;
	    break;
	  case 1:
	    ChangedVideo[Ystart][0][0] = Min2(Xstart,s0);
	    ChangedVideo[Ystart][0][1] = Max2(Xend,e0);
	    break;
	  case 2:
	    ChangedVideo[Ystart][0][0] = Min2(Xstart,s0);
	    ChangedVideo[Ystart][0][1] = Max2(Xend,e1);
	    ChangedVideo[Ystart][1][0] = -1;
	    break;
	  case 3:
	    ChangedVideo[Ystart][1][0] = Min2(Xstart,s1);
	    ChangedVideo[Ystart][1][1] = Max2(Xend,e1);
	    break;
	  default:
	    break;
	}
    }
}

INLINE uldat Plain_countDirtyVideo(dat X1, dat Y1, dat X2, dat Y2) {
    uldat t = 0;
    dat a, b;
    
    for (; Y1 <= Y2; Y1++) {
	a = ChangedVideo[Y1][0][0];
	b = ChangedVideo[Y1][0][1];
	if (a != -1 && a <= X2 && b >= X1)
	    t += Min2(b, X2) - Max2(a, X1) + 1;
	if (a != -1) {
	    a = ChangedVideo[Y1][1][0];
	    b = ChangedVideo[Y1][1][0];
	    if (a != -1 && a <= X2 && b >= X1)
		t += Min2(b, X2) - Max2(a, X1) + 1;
	}
    }
    return t;
}

void StrategyReset(void) {
    AccelVideo[0] = AccelVideo[1] = MAXDAT;
    AccelVideo[2] = AccelVideo[3] = MINDAT;
    StrategyFlag = HW_UNSET;
}

byte Strategy4Video(dat Xstart, dat Ystart, dat Xend, dat Yend) {
    uldat Varea = 0, XYarea_2 = (Xend-Xstart+1)*(Yend-Ystart+1)/2;
    dat x1, y1, x2, y2;
    
    if (StrategyFlag != HW_UNSET && StrategyFlag != HW_ACCEL)
	return HW_BUFFER;

    /* find the intersection between the current area and AccelVideo[] */
    if (Xstart < AccelVideo[2] && Ystart < AccelVideo[3] &&
	Xend >= AccelVideo[0] && Yend >= AccelVideo[1]) {
	
	x1 = Max2(Xstart, AccelVideo[0]);
	y1 = Max2(Ystart, AccelVideo[1]);
	x2 = Min2(Xend,   AccelVideo[2]);
	y2 = Min2(Yend,   AccelVideo[3]);
	
	Varea = (uldat)(x2-x1+1)*(y2-y1+1);
    }
    if (Varea < XYarea_2)
	Varea += Plain_countDirtyVideo(Xstart, Ystart, Xend, Yend);
    
    if (Varea < XYarea_2) {
	AccelVideo[0] = Min2(Xstart, AccelVideo[0]);
	AccelVideo[1] = Min2(Ystart, AccelVideo[1]);
	AccelVideo[2] = Max2(Xend,   AccelVideo[2]);
	AccelVideo[3] = Max2(Yend,   AccelVideo[3]);
	return StrategyFlag = HW_ACCEL;
    }
    return StrategyFlag = HW_BUFFER;
}

/* An important Video function: copy a rectangle. It must be _*FAST*_ !! */
void DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    dat DstRgt = DstLeft + (Rgt - Left), DstDwn = DstUp + (Dwn - Up);
    ldat len, count;
    hwattr *src = Video, *dst = Video;
    byte Accel = FALSE;
    
    count = Dwn - Up + 1;
    len   = (Rgt-Left+1) * sizeof(hwattr);

    /* if HW can do the scroll, use it instead of redrawing */
    if (HW->canDragArea && HW->canDragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp) &&
	Strategy4Video(DstLeft, DstUp, DstRgt, DstDwn) == HW_ACCEL) {
	
	Accel = TRUE;
	HW->DragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp);
    } else
	DirtyVideo(DstLeft, DstUp, DstRgt, DstDwn);


    /* do the drag inside Video[] */
    
    if (DstUp <= Up) {
	src +=    Left +    Up * ScreenWidth;
	dst += DstLeft + DstUp * ScreenWidth;
	if (DstUp != Up)
	    /* copy forward */
	    while (count--) {
		CopyMem(src, dst, len);
		dst += ScreenWidth;
		src += ScreenWidth;
	    }
	else if (Left != DstLeft)
	    /* the tricky case: DstUp == Up */
	    /* copy forward, but with memmove() */
	    while (count--) {
		MoveMem(src, dst, len);
		dst += ScreenWidth;
		src += ScreenWidth;
	    }
    } else if (DstUp > Up) {
	/* copy backward */
	src +=    Left +    Dwn * ScreenWidth;
	dst += DstLeft + DstDwn * ScreenWidth;
	while (count--) {
	    CopyMem(src, dst, len);
	    dst -= ScreenWidth;
	    src -= ScreenWidth;
	}
    }

    if (Accel && HW->NeedOldVideo)
	Video2OldVideo(DstLeft, DstUp, DstRgt, DstDwn);
}

void InitTtysave(void) {
    ioctl(0, TCGETS, &ttysave);
    
    /* return error - use system defaults */
    ttysave.c_cc [VINTR]	= CINTR;
    ttysave.c_cc [VQUIT]	= CQUIT;
    ttysave.c_cc [VERASE]	= CERASE;
    ttysave.c_cc [VKILL]	= CKILL;
    ttysave.c_cc [VSTART]	= CSTART;
    ttysave.c_cc [VSTOP]	= CSTOP;
    ttysave.c_cc [VSUSP]	= CSUSP;
#ifdef VDSUSP
    ttysave.c_cc [VDSUSP]	= CDSUSP;
#endif
#ifdef VREPRINT
    ttysave.c_cc [VREPRINT]	= CRPRNT;
#endif
#ifdef VDISCRD
    ttysave.c_cc [VDISCRD]	= CFLUSH;
#endif
#ifdef VWERSE
    ttysave.c_cc [VWERSE]	= CWERASE;
#endif
#ifdef VLNEXT
    ttysave.c_cc [VLNEXT]	= CLNEXT;
#endif
	
    ttysave.c_cc [VEOF]	= CEOF;
    ttysave.c_cc [VEOL]	= VDISABLE;
#ifdef VEOL2
    ttysave.c_cc [VEOL2]	= VDISABLE;
#endif
#ifdef VSWTC
    ttysave.c_cc [VSWTC]	= VDISABLE;
#endif
#ifdef VSWTCH
    ttysave.c_cc [VSWTCH]	= VDISABLE;
#endif
    ttysave.c_cc [VMIN]	= 1;
    ttysave.c_cc [VTIME]	= 0;
	
    /* input modes */
    ttysave.c_iflag = (BRKINT | IGNPAR | ICRNL | IXON
#ifdef IMAXBEL
		       | IMAXBEL
#endif
		       );
	
    /* output modes */
    ttysave.c_oflag = (OPOST | ONLCR);
    
    /* control modes */
    ttysave.c_cflag = (CS8 | CREAD);
	
    /* line discipline modes */
    ttysave.c_lflag = (ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK
#ifdef ECHOKE
		       | ECHOKE
#endif
		       );
}
