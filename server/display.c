/*
 *  display.c  -- connect to a running twin and register as a display
 *
 *  Copyright (C) 2000-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

/*
 * this is basically a chopped down version of twin server,
 * with the minimum required features to startup a display driver,
 * plus libTw code to talk to twin, register on it as a special display,
 * and forward messages and commands between the display driver and twin.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>

#include "twin.h"
#include "hw.h"
#include "hw_private.h"
#include "common.h"
#include "dl_helper.h"
#include "fdlist.h"
#include "version.h"

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>



#ifndef PKG_LIBDIR
# warning PKG_LIBDIR is not #defined, assuming "/usr/local/lib/twin"
# define PKG_LIBDIR "/usr/local/lib/twin"
#endif


static CONST byte * CONST modules_prefix = PKG_LIBDIR "/" DL_PREFIX;

static CONST byte *MYname;

static dat TryDisplayWidth, TryDisplayHeight;
static byte ValidVideo;

CONST byte *TWDisplay, *origTWDisplay, *origTERM;

byte nullMIME[TW_MAX_MIMELEN];

udat ErrNo;
byte CONST * ErrStr;

byte CONST * HOME;

#define L 0x55
#define M 0xAA
#define H 0xFF

palette Palette[MAXCOL+1] = {
    /* the default colour table, for VGA+ colour systems */
    {0,0,0}, {0,0,M}, {0,M,0}, {0,M,M}, {M,0,0}, {M,0,M}, {M,M,0}, {M,M,M},
    {L,L,L}, {L,L,H}, {L,H,L}, {L,H,H}, {H,L,L}, {H,L,H}, {H,H,L}, {H,H,H}};

palette defaultPalette[MAXCOL+1] = {
    /* the default colour table, for VGA+ colour systems */
    {0,0,0}, {0,0,M}, {0,M,0}, {0,M,M}, {M,0,0}, {M,0,M}, {M,M,0}, {M,M,M},
    {L,L,L}, {L,L,H}, {L,H,L}, {L,H,H}, {H,L,L}, {H,L,H}, {H,H,L}, {H,H,H}};

#undef H
#undef M
#undef L

static fdlist FdList[5];
static uldat FdSize = 5, FdTop, FdBottom, FdWQueued;
#define LS      FdList[Slot]
#define ls      FdList[slot]

static fd_set save_rfds, save_wfds;
static int max_fds;

static tmsgport TMsgPort = NOID, THelper = NOID;
static byte MouseMotionN; /* non-zero to report also mouse motion events */

int (*OverrideSelect)(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) = select;

/*
 * first, utility functions
 */

void Quit(int status);

void NoOp(void) {
}
byte AlwaysFalse(void) {
    return tfalse;
}
byte AlwaysTrue(void) {
    return ttrue;
}
void *AlwaysNull(void) {
    return NULL;
}

void GainPrivileges(void) {
}
void RemotePidIsDead(pid_t pid) {
}

byte Error(udat Code_Error) {
    switch ((ErrNo = Code_Error)) {
      case NOMEMORY:
	ErrStr = "Out of memory!";
	break;
      case NOTABLES:
	ErrStr = "Internal tables full!";
	break;
      case SYSCALLERROR:
	ErrStr = strerror(errno);
	break;
      default:
	break;
    }
    return tfalse;
}

int printk(CONST byte *format, ...) {
    int i = 0;
#ifdef TW_HAVE_VPRINTF
    va_list ap;
    va_start(ap, format);
    i = vfprintf(stderr, (CONST char *)format, ap);
    va_end(ap);
#endif
    return i;
}

int flushk(void) {
    return fflush(stderr);
}

static void OutOfMemory(void) {
    printk("twdisplay: Out of memory!\n");
}

INLINE uldat FdListGet(void) {
    if (FdBottom < FdSize)
	return FdBottom;
    return NOSLOT;
}

uldat RegisterRemote(int Fd, obj HandlerData, void *HandlerIO) {
    uldat Slot, j;
    
    if ((Slot = FdListGet()) == NOSLOT)
	return Slot;
    
    WriteMem(&LS, 0, sizeof(fdlist));
    
    LS.Fd = Fd;
    LS.pairSlot = NOSLOT;
    if ((LS.HandlerData = HandlerData))
	LS.HandlerIO.D = HandlerIO;
    else
	LS.HandlerIO.S = HandlerIO;
    LS.extern_couldntwrite = tfalse;
    
    if (FdTop <= Slot)
	FdTop = Slot + 1;
    for (j = FdBottom + 1; j < FdTop; j++)
	if (FdList[j].Fd == NOFD)
	    break;
    FdBottom = j;

    if (Fd >= 0) {
	FD_SET(Fd, &save_rfds);
	if (max_fds < Fd)
	    max_fds = Fd;
    }
    return Slot;
}

void UnRegisterRemote(uldat Slot) {
    /* not needed, we are going to quit anyway if this gets called */
}

void RemoteCouldntWrite(uldat Slot) {
    if (Slot == NOSLOT || Slot >= FdTop || LS.Fd == NOFD)
	return;
    if (LS.extern_couldntwrite == tfalse) {
	LS.extern_couldntwrite = ttrue;
	FdWQueued++;
    }
    FD_SET(LS.Fd, &save_wfds);
}

void RemoteCouldWrite(uldat Slot) {
    if (Slot == NOSLOT || Slot >= FdTop || LS.Fd == NOFD)
	return;
    if (LS.extern_couldntwrite == ttrue) {
	LS.extern_couldntwrite = tfalse;
	FdWQueued--;
    }
    FD_CLR(LS.Fd, &save_wfds);
}

static void RemoteEvent(int FdCount, fd_set *FdSet) {
    uldat Slot;
    int fd;
    for (Slot=0; Slot<FdTop && FdCount; Slot++) {
	if ((fd = LS.Fd) >= 0) {
	    if (FD_ISSET(fd, FdSet)) {
		FdCount--;
		if (LS.HandlerData)
		    LS.HandlerIO.D (fd, LS.HandlerData);
		else
		    LS.HandlerIO.S (fd, Slot);
	    }
	}
    }
}


static struct s_fn_module _FnModule = {
    module_magic, (uldat)sizeof(struct s_module), (uldat)1,
	(void *)NoOp, /* CreateModule */
	(void *)NoOp, /* InsertModule */
	(void *)NoOp, /* RemoveModule */
	(void *)NoOp, /* DeleteModule */
	NULL,	      /* Fn_Obj */
	(void *)NoOp, /* DlOpen	      */
	(void *)NoOp  /* DlClose      */
};

static struct s_module _Module = {
    module_magic,
	&_FnModule,
};



static module DlLoadAny(uldat len, byte *name) {
    module Module = &_Module;
    byte (*init_func)(module);
    byte *path;

    if (!dlinit_once()) {
        return (module)0;
    }

    if ((Module->Name = CloneStrL(name, len)) &&
	(path = AllocMem(len + strlen(modules_prefix) + strlen(DL_SUFFIX) + 1))) {
	
	sprintf(path, "%s%.*s%s", modules_prefix, (int)len, name, DL_SUFFIX);
	Module->Handle = (void *)dlopen(path);
        FreeMem(path);

        if (Module->Handle) {
	    /*
	     * Module MUST have a InitModule function, as it needs to set
	     * Module->Private to its xxx_InitHW() startup code.
	     */
	    if ((init_func = (byte (*)(module)) dlsym((dlhandle)Module->Handle, "InitModule"))) {
                if (init_func(Module)) {
                    return Module;
                } else if (ErrStr == NULL || *ErrStr == '\0') {
                    Error(DLERROR);
                    ErrStr = "InitModule() failed";
                }
            } else {
                Error(DLERROR);
                ErrStr = "InitModule() not found in module";
            }
        } else {
            Error(DLERROR);
	    ErrStr = dlerror();
	}
    } else {
	ErrStr = "Out of memory!";
    }
    return (module)0;
}

static byte module_InitHW(byte *arg, uldat len) {
    byte *name, *tmp;
    byte *(*InitD)(void);
    module Module = NULL;

    if (!arg || len <= 4)
	return tfalse;
    
    arg += 4; len -= 4; /* skip "-hw=" */
    
    name = memchr(arg, '@', len);
    tmp = memchr(arg, ',', len);
    if (tmp && (!name || tmp < name))
	name = tmp;
    if (name)
	len = name - arg;

    if (len == 1 && *arg == 'X')
        len = 3, arg = "X11";
    
    if ((name = AllocMem(len + 4))) {
	sprintf(name, "hw_%.*s", (int)len, arg);
			
	Module = DlLoadAny(len + 3, name);
	
	if (Module) {
	    printk("twdisplay: starting display driver module `%."STR(TW_SMALLBUFF)"s'...\n", name);
	    
	    if ((InitD = Module->Private) && InitD()) {
		printk("twdisplay: ...module `%."STR(TW_SMALLBUFF)"s' successfully started.\n", name);
		HW->Module = Module; Module->Used++;
		
		FreeMem(name);
		return ttrue;
	    }
	    /*Delete(Module);*/
	}
    } else
	ErrStr = "Out of memory!";
    
    if (Module) {
	printk("twdisplay: ...module `%."STR(TW_SMALLBUFF)"s' failed to start.\n", name ? name : (byte *)"(NULL)");
    } else
	printk("twdisplay: unable to load display driver module `%."STR(TW_SMALLBUFF)"s' :\n"
	       "      %."STR(TW_SMALLBUFF)"s\n", name ? name : (byte *)"(NULL)", ErrStr);

    if (name)
    	FreeMem(name);

    return tfalse;
}

static display_hw CreateDisplayHW(uldat len, CONST byte *name);
static byte InitDisplayHW(display_hw);
static void QuitDisplayHW(display_hw);

static struct s_fn_display_hw _FnDisplayHW = {
    display_hw_magic, (uldat)sizeof(struct s_display_hw), (uldat)1,
	(void *)NoOp, /* CreateDisplayHW */
	(void *)NoOp, /* InsertDisplayHW */
	(void *)NoOp, /* RemoveDisplayHW */
	(void *)NoOp, /* DeleteDisplayHW */
	(void *)NoOp, /* ChangeFieldDisplayHW */
	NULL,	      /* Fn_Obj */
	InitDisplayHW,
	QuitDisplayHW
};

static struct s_display_hw _HW = {
    display_hw_magic,
	&_FnDisplayHW,
};


void warn_NoHW(uldat len, CONST char *arg, uldat tried) {
    printk("twdisplay: All display drivers failed");
    if (arg)
        printk(" for `%.*s\'", (int)Min2(TW_SMALLBUFF, len), arg);
    else
        printk(".");
    printk("\n");
}

static void UpdateFlagsHW(void) {
    if (!HW->Quitted) {
	NeedOldVideo = HW->FlagsHW & FlHWNeedOldVideo;
	ExpensiveFlushVideo = HW->FlagsHW & FlHWExpensiveFlushVideo;
	CanDragArea = !!HW->CanDragArea;
    }
}


/*
 * InitDisplayHW runs HW specific InitXXX() functions, starting from best setup
 * and falling back in case some of them fails.
 */
static byte InitDisplayHW(display_hw D_HW) {
    byte *arg = D_HW->Name;
    uldat tried = 0;
    byte success;

    SaveHW;
    SetHW(D_HW);

    D_HW->DisplayIsCTTY = D_HW->NeedHW = D_HW->FlagsHW = tfalse;
    
    if (arg && !strncmp(arg, "-hw=", 4))
	arg += 4;
    else
	arg = NULL;

#define TRY4(hw) (tried++, module_InitHW(hw, LenStr(hw)))

    if (!arg || !*arg) {
        success =
	TRY4("-hw=gfx") ||
	TRY4("-hw=X11") ||
	TRY4("-hw=twin") ||
#if 0 /* cannot use `--hw=display' inside twdisplay! */
	TRY4("-hw=display") ||
#endif
	TRY4("-hw=tty") ||
	TRY4("-hw=ggi");
    } else {
	success = module_InitHW(D_HW->Name, D_HW->NameLen);
    }
    if (success) {
        D_HW->Quitted = tfalse;
	if (!DisplayHWCTTY && D_HW->DisplayIsCTTY)
	    DisplayHWCTTY = D_HW;
	UpdateFlagsHW();
    }
    else
        warn_NoHW(D_HW->NameLen, HW->Name, tried);
    
    RestoreHW;

    return success;
}

static void QuitDisplayHW(display_hw D_HW) {
    if (D_HW && D_HW->QuitHW)
	HW = D_HW, D_HW->QuitHW();
}


static display_hw CreateDisplayHW(uldat NameLen, CONST byte *Name) {
    byte *newName = NULL;
    
    if (Name && (newName = CloneStrL(Name, NameLen))) {	
	HW = &_HW;
	HW->NameLen = NameLen;
	HW->Name = newName;
	HW->Module = NULL;
	HW->Quitted = ttrue;
	HW->AttachSlot = NOSLOT;
	/*
	 * ->Quitted will be set to tfalse only
	 * after DisplayHW->InitHW() has succeeded.
	 */
	return HW;
    }
    if (newName)
	FreeMem(newName);
    return (display_hw)0;
}

static byte IsValidHW(uldat len, CONST byte *arg) {
    uldat i;
    byte b;
    if (len >= 4 && !CmpMem(arg, "-hw=", 4))
        arg += 4, len -=4;
    
    for (i = 0; i < len; i++) {
        b = arg[i];
        if (b == '@' || b == ',')
            /* the rest are options - validated by each display HW */
            break;
        if ((b < '0' || b > '9') && (b < 'A' || b > 'Z') && (b < 'a' || b > 'z') && b != '_') {
            printk("twdisplay: invalid non-alphanumeric character `%c' in display HW name: `%.*s'\n", (int)b, Min2((int)len,TW_SMALLBUFF), arg);
            return tfalse;
        }
    }
    return ttrue;
}

static display_hw AttachDisplayHW(uldat len, CONST byte *arg, uldat slot, byte flags) {
    if ((len && len <= 4) || CmpMem("-hw=", arg, Min2(len,4))) {
	printk("twdisplay: specified `%.*s\' is not `--hw=<display>\'\n",
		(int)len, arg);
	return (display_hw)0;
    }
    
    if (IsValidHW(len, arg) && CreateDisplayHW(len, arg)) {
	HW->AttachSlot = slot;
	if (InitDisplayHW(HW))
	    return HW;
    }
    return (display_hw)0;
}


#if 0
/* not needed, server-side hw_display already does optimization for us */
INLINE void OptimizeChangedVideo(void) {
    uldat _start, start, _end, end;
    ldat i;
    
    for (i=0; i<(ldat)DisplayHeight*2; i++) {
	start = (uldat)ChangedVideo[i>>1][!(i&1)][0];
	    
	if (start != (uldat)-1) {
	    
	    start += (i>>1) * (ldat)DisplayWidth;
	    _start = start;

	    _end = end = (uldat)ChangedVideo[i>>1][!(i&1)][1] + (i>>1) * (ldat)DisplayWidth;
		
	    while (start <= end && Video[start] == OldVideo[start])
		start++;
	    while (start <= end && Video[end] == OldVideo[end])
		end--;

	    if (start > end) {
		if (i&1) {
		    /*
		     * this is the first area, to make it empty
		     * copy the second on this.
		     */
		    if (ChangedVideo[i>>1][1][0] != -1) {
			ChangedVideo[i>>1][0][0] = ChangedVideo[i>>1][1][0];
			ChangedVideo[i>>1][0][1] = ChangedVideo[i>>1][1][1];
			ChangedVideo[i>>1][1][0] = -1;
		    } else
			ChangedVideo[i>>1][0][0] = -1;
		} else
		    ChangedVideo[i>>1][1][0] = -1;
		continue;
	    } else if (start > _start || end < _end) {
		ChangedVideo[i>>1][!(i&1)][0] += start - _start;
		ChangedVideo[i>>1][!(i&1)][1] -= _end - end;
	    }
	}
    }
}
#else
# define OptimizeChangedVideo() do { } while(0)
#endif

INLINE void SyncOldVideo(void) {
    uldat start, len;
    ldat i;

    if (ChangedVideoFlag) {
	for (i=0; i<(ldat)DisplayHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    
	    if (start != -1) {
		len = ChangedVideo[i>>1][i&1][1] + 1 - start;
		start += (i>>1) * (ldat)DisplayWidth;
		
		ChangedVideo[i>>1][i&1][0] = -1;
	    
		CopyMem(Video + start, OldVideo + start, len * sizeof(hwattr));
	    }
	}
    }
}

void FlushHW(void) {
    
    if (!ValidVideo)
	return;
    
    if (QueuedDrawArea2FullScreen) {
	QueuedDrawArea2FullScreen = tfalse;
	DirtyVideo(0, 0, DisplayWidth - 1, DisplayHeight - 1);
	ValidOldVideo = tfalse;
    } else if (HW->RedrawVideo) {
	DirtyVideo(HW->RedrawLeft, HW->RedrawUp, HW->RedrawRight, HW->RedrawDown);
	ValidOldVideo = tfalse;
    } else if (NeedOldVideo && ValidOldVideo)
	OptimizeChangedVideo();
    
    HW->FlushVideo();
    
    HW->RedrawVideo = tfalse;
    
    if (HW->NeedHW & NEEDFlushHW)
	HW->FlushHW();

    if (NeedHW & NEEDFlushStdout)
	fflush(stdout), NeedHW &= ~NEEDFlushStdout;
    
    SyncOldVideo();

    ChangedVideoFlag = tfalse;
    ValidOldVideo = ttrue;
}


void ResizeDisplayPrefer(display_hw D_HW) {
    SaveHW;
    SetHW(D_HW);
    D_HW->DetectSize(&TryDisplayWidth, &TryDisplayHeight);
    NeedHW |= NEEDResizeDisplay;
    RestoreHW;
}

static byte ReAllocVideo(dat Width, dat Height) {
    byte change = DisplayWidth != Width || DisplayHeight != Height;
    
    if (!NeedOldVideo && OldVideo) {
	FreeMem(OldVideo);
	OldVideo = NULL;
    } else if ((NeedOldVideo && !OldVideo) || change) {
	if (!(OldVideo = (hwattr *)ReAllocMem(OldVideo, (ldat)Width*Height*sizeof(hwattr))) && Width && Height) {
	    OutOfMemory();
	    Quit(1);
	}
	ValidOldVideo = tfalse;
    }
    
    if (!Video || change) {
	DisplayWidth = Width;
	DisplayHeight = Height;
	
	if ((!(Video = (hwattr *)ReAllocMem(Video, (ldat)DisplayWidth*DisplayHeight*sizeof(hwattr))) ||
	     !(ChangedVideo = (dat (*)[2][2])ReAllocMem(ChangedVideo, (ldat)DisplayHeight*sizeof(dat)*4))) &&
	     DisplayWidth && DisplayHeight) {
	    
	    OutOfMemory();
	    Quit(1);
	}
	ValidVideo = tfalse;
	WriteMem(ChangedVideo, 0xff, (ldat)DisplayHeight*sizeof(dat)*4);
    
    }
    return change;
}


/*
 * return ttrue if DisplayWidth or DisplayHeight were changed
 */
static byte ResizeDisplay(void) {
    byte change = tfalse;
    tmsg Tmsg;
    
    if (!TryDisplayWidth || !TryDisplayHeight)
	HW->DetectSize(&TryDisplayWidth, &TryDisplayHeight);
    
    HW->CheckResize(&TryDisplayWidth, &TryDisplayHeight);
    HW->Resize(TryDisplayWidth, TryDisplayHeight);

    change = ReAllocVideo(TryDisplayWidth, TryDisplayHeight);
    
    NeedHW &= ~NEEDResizeDisplay;

    TryDisplayWidth = TryDisplayHeight = 0;
    
    if (change && (Tmsg = TwCreateMsg(TW_MSG_DISPLAY, TW_SIZEOF_TEVENT_DISPLAY))) {
	Tmsg->Event.EventDisplay.Code = TW_DPY_Resize;
	Tmsg->Event.EventDisplay.Len  = 0;
	Tmsg->Event.EventDisplay.X    = DisplayWidth;
	Tmsg->Event.EventDisplay.Y    = DisplayHeight;
	TwBlindSendMsg(THelper, Tmsg);
    }
    return change;
}

byte AllHWCanDragAreaNow(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return (CanDragArea && HW->CanDragArea &&
	    HW->CanDragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp));
}

void DragAreaHW(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    HW->DragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp);
}

    
void SetPaletteHW(udat N, udat R, udat G, udat B) {
    if (N <= MAXCOL) {
	palette c;
	c.Red = R;
	c.Green = G;
	c.Blue = B;
	if (CmpMem(&Palette[N], &c, sizeof(palette))) {
	    Palette[N] = c;
	    HW->SetPalette(N, R, G, B);
	}
    }
}

void ResetPaletteHW(void) {
    HW->ResetPalette();
}

static void HandleMsg(tmsg Msg) {
    tevent_display EventD;
    
    switch (Msg->Type) {
      case TW_MSG_SELECTION:
	/* should never happen */
	printk("\ntwdisplay: HandleMsg(): unexpected Selection Message from twin!\n");
	break;
      case TW_MSG_SELECTIONREQUEST:
#if 0
	printk("twdisplay: Selection Request from 0x%08x, owner is underlying HW\n", Msg->Event.EventSelectionRequest.Requestor);
#endif
	/* request selection from underlying HW */
	
	/*
	 * Just like in TwinSelectionGetOwner() : normally Requestor
	 * is a meaningful pointer; here it is just a libTw Id.
	 * Cast it to (obj) as expected by HW displays...
	 * we will cast it back when needed
	 */
	HW->HWSelectionRequest((obj)(topaque)Msg->Event.EventSelectionRequest.Requestor,
			       Msg->Event.EventSelectionRequest.ReqPrivate);
	break;
      case TW_MSG_SELECTIONNOTIFY:
#if 0
	printk("twdisplay: Selection Notify to underlying HW\n");
#endif
	/* notify selection to underlying HW */
	HW->HWSelectionNotify(Msg->Event.EventSelectionNotify.ReqPrivate,
			      Msg->Event.EventSelectionNotify.Magic,
			      Msg->Event.EventSelectionNotify.MIME,
			      Msg->Event.EventSelectionNotify.Len,
			      Msg->Event.EventSelectionNotify.Data);
	break;
      case TW_MSG_DISPLAY:
	EventD = &Msg->Event.EventDisplay;
	switch (EventD->Code) {
	  case TW_DPY_DrawHWAttr:
	    if (EventD->Len /= sizeof(hwattr)) {
		if (EventD->X + EventD->Len > DisplayWidth || EventD->Y >= DisplayHeight) {
		    /*
		     * in a perfect world this should not happen, 
		     * but with our asyncronous display size negotiation,
		     * it actually can.
		     */

		    if (EventD->Y < DisplayHeight && EventD->X < DisplayWidth)
			EventD->Len = DisplayWidth - EventD->X;
		    else
			break;
		}
		DirtyVideo(EventD->X, EventD->Y, EventD->X + EventD->Len - 1, EventD->Y);
		CopyMem(EventD->Data, &Video[EventD->X + EventD->Y * (ldat)DisplayWidth], (uldat)EventD->Len * sizeof(hwattr));
	    }
	    break;
	  case TW_DPY_FlushHW:
	    ValidVideo = ttrue;
	    FlushHW();
	    break;
	  case TW_DPY_SetCursorType:
	    if (EventD->Len == sizeof(uldat))
		SetCursorType(*(uldat *)EventD->Data);
	    break;
	  case TW_DPY_MoveToXY:
	    MoveToXY(EventD->X, EventD->Y);
	    break;
	  case TW_DPY_Resize:
	    /*
	     * twin told us the new display size.
	     * don't detect/check anything, just apply it
	     * (if it is meaningful and different from current)
	     */
	    if (EventD->X != DisplayWidth || EventD->Y != DisplayHeight) {
		HW->Resize(EventD->X, EventD->Y);
		ReAllocVideo(EventD->X, EventD->Y);
	    }
	    break;
	  case TW_DPY_SelectionExport:
	    NeedHW |= NEEDSelectionExport;
	    break;
	  case TW_DPY_DragArea:
#define c ((udat *)EventD->Data)
	    if (EventD->Len == 4*sizeof(udat))
		DragArea(EventD->X, EventD->Y, c[0], c[1], c[2], c[3]);
#undef c
	    break;
	  case TW_DPY_Beep:
	    HW->Beep();
	    break;
	  case TW_DPY_Configure:
	    if (EventD->X == HW_MOUSEMOTIONEVENTS)
		MouseMotionN = EventD->Y > 0;
	    HW->Configure(EventD->X, EventD->Y == -1, EventD->Y);
	    break;
	  case TW_DPY_SetPalette:
#define c ((udat *)EventD->Data)
	    if (EventD->Len == 3*sizeof(udat))
		HW->SetPalette(EventD->X, c[0], c[1], c[2]);
#undef c
	    break;
	  case TW_DPY_ResetPalette:
	    HW->ResetPalette();
	    break;
	  case TW_DPY_Helper:
	    THelper = *(uldat *)EventD->Data;
	    break;
	  case TW_DPY_Quit:
	    Quit(0);
	    break;
	  default:
	    break;
	}
    }
}

static void SocketIO(void) {
    tmsg Msg;
    while ((Msg = TwReadMsg(tfalse)))
	HandleMsg(Msg);
}

void SelectionExport(void) {
    HW->HWSelectionExport();
    NeedHW &= ~NEEDSelectionExport;
}

/* HW back-end function: get selection owner */

/*
 * In the same function in twin server, this returns a meaningful pointer.
 * Here, it returns just an Id coming from libTw.
 * Cheat and cast to to (obj), since the underlying display HW code
 * treats it as opaque. We will cast it back to (uldat) when needed.
 */
obj TwinSelectionGetOwner(void) {
    return (obj)(topaque)TwGetOwnerSelection();
}

/* HW back-end function: set selection owner */
void TwinSelectionSetOwner(obj Owner, tany Time, tany Frac) {
    tmsg Msg;

    if ((Msg=TwCreateMsg(TW_MSG_SELECTIONCLEAR, sizeof(tevent_common)))) {
	TwBlindSendMsg(THelper, Msg);
    }
}

/* HW back-end function: notify selection */
void TwinSelectionNotify(obj Requestor, uldat ReqPrivate, uldat Magic, CONST byte MIME[MAX_MIMELEN],
			    uldat Len, CONST byte *Data) {
    if (!MIME)
	MIME = nullMIME;
#if 0
    printk("twdisplay: Selection Notify to 0x%08x\n", (uldat)Requestor);
#endif
    /* cast back Requestor from fake (obj) to its original (uldat) */
    TwNotifySelection((topaque)Requestor, ReqPrivate, Magic, MIME, Len, Data);
}

/* HW back-end function: request selection */
void TwinSelectionRequest(obj Requestor, uldat ReqPrivate, obj Owner) {
#if 0
    printk("twdisplay: Selection Request from 0x%08x, Owner is 0x%08x\n", (uldat)Requestor, (uldat)Owner);
#endif
    /* cast back Owner from the fake (obj) to (uldat) */
    TwRequestSelection((topaque)Owner, ReqPrivate);
}

static byte StdAddMouseEvent(udat Code, dat MouseX, dat MouseY) {
    tevent_mouse Event;
    tmsg Msg;

    if (HW->FlagsHW & FlHWNoInput)
	return ttrue;

    if ((Msg=TwCreateMsg(TW_MSG_WIDGET_MOUSE, sizeof(event_mouse)))) {
	Event = &Msg->Event.EventMouse;

	Event->Code = Code;
	Event->ShiftFlags = (udat)0;
	Event->X = MouseX;
	Event->Y = MouseY;
	
	TwBlindSendMsg(THelper, Msg);
	return ttrue;
    }
    return tfalse;
}

byte MouseEventCommon(dat x, dat y, dat dx, dat dy, udat Buttons) {
    dat prev_x, prev_y;
    udat OldButtons, i;
    mouse_state *OldState;
    udat result;
    byte ret = ttrue;
     
    OldState=&HW->MouseState;
    OldButtons=OldState->keys;
    prev_x = OldState->x;
    prev_y = OldState->y;

    x = Max2(x, 0); x = Min2(x, DisplayWidth - 1);
    OldState->delta_x = x == 0 ? Min2(dx, 0) : x == DisplayWidth - 1 ? Max2(dx, 0) : 0;

    y = Max2(y, 0); y = Min2(y, DisplayHeight - 1);
    OldState->delta_y = y == 0 ? Min2(dy, 0) : y == DisplayHeight - 1 ? Max2(dy, 0) : 0;
	
    if (x != prev_x || y != prev_y)
	HW->FlagsHW |= FlHWChangedMouseFlag;

    OldState->x = x;
    OldState->y = y;
    
    OldState->keys=Buttons;
    
    if (Buttons != OldButtons || ((MouseMotionN || OldButtons) && (x != prev_x || y != prev_y))) {
	
	if ((MouseMotionN || OldButtons) && (x != prev_x || y != prev_y)) {
	    ret = StdAddMouseEvent(MOVE_MOUSE | OldButtons, x, y);
	}
	for (i = 0; i < BUTTON_N_MAX && ret; i++) {
	    if ((Buttons & HOLD_CODE(i)) != (OldButtons & HOLD_CODE(i))) {
		result = (Buttons & HOLD_CODE(i) ? PRESS_CODE(i) : RELEASE_CODE(i)) | (OldButtons &= ~HOLD_CODE(i));
		OldButtons |= Buttons & HOLD_CODE(i);
		ret = StdAddMouseEvent(result, x, y);
	    }
	}
    }
    return ret;
}

byte KeyboardEventCommon(udat Code, udat ShiftFlags, udat Len, CONST byte *Seq) {
    tevent_keyboard Event;
    tmsg Msg;

    if (HW->FlagsHW & FlHWNoInput)
	return ttrue;

    if ((Msg=TwCreateMsg(TW_MSG_WIDGET_KEY, Len + sizeof(event_keyboard)))) {
	Event = &Msg->Event.EventKeyboard;
	    
	Event->Code = Code;
	Event->ShiftFlags = ShiftFlags;
	Event->SeqLen = Len;
	CopyMem(Seq, Event->AsciiSeq, Len);
	Event->AsciiSeq[Len] = '\0'; /* terminate string with \0 */
	
	TwBlindSendMsg(THelper, Msg);
	return ttrue;
    }
    return tfalse;
}

static void MainLoop(int Fd) {
    struct timeval sel_timeout;
    fd_set read_fds, write_fds, *pwrite_fds;
    uldat err, detail;
    int sys_errno, num_fds;
    
    for (;;) {
	if (GotSignals)
	    HandleSignals();
	
	if (NeedHW & NEEDResizeDisplay)
	    ResizeDisplay();
	
	if (NeedHW & NEEDSelectionExport)
	    SelectionExport();

	/*
	 * don't FlushHW() unconditionalluy here,
	 * as it gets called syncronously
	 * with the main twin from HandleEvent()...
	 * just update what is needed.
	 */
	if (HW->RedrawVideo)
	    FlushHW();
	else {
	    HW->UpdateMouseAndCursor();

	    if (HW->NeedHW & NEEDFlushHW)
		HW->FlushHW();
	    if (NeedHW & NEEDFlushStdout)
		fflush(stdout), NeedHW &= ~NEEDFlushStdout;
	}
	
	read_fds = save_rfds;
	
	if (!FdWQueued)
	    pwrite_fds = NULL;
	else {
	    write_fds = save_wfds;
	    pwrite_fds = &write_fds;
	}
	
	if (!TwFlush())
	    break;
	
        /*
	 * messages can arrive during Tw* function calls,
	 * so the FD_ISSET() test alone does not suffice.
	 * also use an inactivity timeout: 2min...
	 */
	sel_timeout.tv_sec  = TwPendingMsg() ? 0 : 120;
	sel_timeout.tv_usec = 0;

	if (NeedHW & NEEDPanicHW)
	    break;

	num_fds = OverrideSelect(max_fds+1, &read_fds, pwrite_fds, NULL, &sel_timeout);

	if (num_fds < 0 && errno != EINTR)
	    /* ach, problem. */
	    break;

        if (num_fds == 0 && !TwPendingMsg()) {
	    /*
	     * no activity during timeout delay && no pending messages..
	     * just ping twin to keep connection active..
	     * however, if width <= 0 : means we lost connection, so exit..
	     */
	    if (TwGetDisplayWidth() <= 0) {
		QuitDisplayHW(HW);
		printk( "twdisplay: lost connection to TWIN.. \n" );
 		exit(1);
	    }
	}
       
	if ((num_fds > 0 && FD_ISSET(Fd, &read_fds)) || TwPendingMsg()) {
	    /*
	     * messages can arrive during Tw* function calls,
	     * so the FD_ISSET() test alone does not suffice.
	     */
	    if (FD_ISSET(Fd, &read_fds)) {
		num_fds--;
		FD_CLR(Fd, &read_fds);
	    }
	    SocketIO();
	}
	if (num_fds > 0)
	    RemoteEvent(num_fds, &read_fds);
    }

    if (NeedHW & NEEDPanicHW)
	Quit(0);
    
    if (num_fds < 0 && errno != EINTR) {
	QuitDisplayHW(HW);
	printk("twdisplay: select(): %."STR(TW_SMALLBUFF)"s\n", strerror(errno));
	exit(1);
    }
    if (TwInPanic()) {
	err = TwErrno;
	detail = TwErrnoDetail;
    	QuitDisplayHW(HW);
	printk("%."STR(TW_SMALLBUFF)"s: libTw error: %."STR(TW_SMALLBUFF)"s%."STR(TW_SMALLBUFF)"s\n", MYname,
		TwStrError(err), TwStrErrorDetail(err, detail));
	exit(1);
    }
    err = TwErrno;
    detail = TwErrnoDetail;
    sys_errno = errno;
    QuitDisplayHW(HW);
    printk("%."STR(TW_SMALLBUFF)"s: shouldn't happen! Please report:\n"
	    "\tlibTw TwErrno: %d(%d),\t%."STR(TW_SMALLBUFF)"s%."STR(TW_SMALLBUFF)"s\n"
	    "\tsystem  errno: %d,\t%."STR(TW_SMALLBUFF)"s\n", MYname,
	    err, detail, TwStrError(err), TwStrErrorDetail(err, detail),
	    sys_errno, strerror(sys_errno));
    exit(1);
}

dat GetDisplayWidth(void) {
    return DisplayWidth;
}
dat GetDisplayHeight(void) {
    return DisplayHeight;
}

static void Usage(void) {
    fputs("Usage: twdisplay [OPTIONS]\n"
	  "Currently known options: \n"
	  " -h, --help               display this help and exit\n"
	  " -V, --version            output version information and exit\n"
	  " -s, --share              allow multiple simultaneous displays (default)\n"
	  " -x, --excl               request exclusive display - detach all others\n"
	  " -v, --verbose            verbose output (default)\n"
	  " -q, --quiet              quiet - don't report messages from twin server\n"
	  " -f, --force              force running even with wrong protocol version\n"
	  " --twin@<TWDISPLAY>       specify server to contact instead of $TWDISPLAY\n"
	  " --hw=<display>[,options] start the given display (only one --hw=... allowed)\n"
	  "                          (default: autoprobe all displays until one succeeds)\n"
	  "Currently known display drivers: \n"
	  "\tgfx[@<XDISPLAY>]\n"
	  "\tX[@<XDISPLAY>]\n"
	  "\ttwin[@<TWDISPLAY>]\n"
	  "\ttty[@<tty device>]\n"
	  "\tggi[@<ggi display>]\n", stdout);
}

static void TryUsage(CONST char *opt) {
    if (opt)
	fprintf(stdout, "twdisplay: unknown option `%."STR(TW_SMALLBUFF)"s'\n", opt);
    fputs("           try `twdisplay --help' for usage summary.\n", stdout);
}

static void ShowVersion(void) {
    fputs("twdisplay " TWIN_VERSION_STR " with socket protocol "
	  TW_PROTOCOL_VERSION_STR "\n", stdout);
}

static byte VersionsMatch(byte force) {
    uldat cv = TW_PROTOCOL_VERSION, lv = TwLibraryVersion(), sv = TwServerVersion();
	
    if (lv != sv || lv != cv) {
	printk("twdisplay: %."STR(TW_SMALLBUFF)"s: socket protocol version mismatch!%."STR(TW_SMALLBUFF)"s\n"
		"           client is %d.%d.%d, library is %d.%d.%d, server is %d.%d.%d\n",
		(force ? "warning" : "fatal"), (force ? " (ignored)" : ""),
		TW_VER_MAJOR(cv), TW_VER_MINOR(cv), TW_VER_PATCH(cv),
		TW_VER_MAJOR(lv), TW_VER_MINOR(lv), TW_VER_PATCH(lv),
		TW_VER_MAJOR(sv), TW_VER_MINOR(sv), TW_VER_PATCH(sv));
	return tfalse;
    }
    return ttrue;
}

static void MergeHyphensArgv(int argc, char **argv) {
    char *S;
    while (argc) {
	if ((S = *argv) && S[0] == '-' && S[1] == '-' && S[2] && S[3])
	    (*argv)++;
	argv++, argc--;
    }
}

TW_DECL_MAGIC(display_magic);

int main(int argc, char *argv[]) {
    byte flags = TW_ATTACH_HW_REDIRECT, force = 0;
    byte *dpy = NULL, *arg = NULL, *tty = ttyname(0);
    byte ret = 0, ourtty = 0;
    byte *s;
    TW_CONST byte *buff;
    uldat chunk;
    int Fd;
    
    MergeHyphensArgv(argc, argv);
    
    MYname = argv[0];
    
    while (*++argv) {
	if (!strcmp(*argv, "-V") || !strcmp(*argv, "-version")) {
	    ShowVersion();
	    return 0;
	} else if (!strcmp(*argv, "-h") || !strcmp(*argv, "-help")) {
	    Usage();
	    return 0;
	} else if (!strcmp(*argv, "-x") || !strcmp(*argv, "-excl"))
	    flags |= TW_ATTACH_HW_EXCLUSIVE;
	else if (!strcmp(*argv, "-s") || !strcmp(*argv, "-share"))
	    flags &= ~TW_ATTACH_HW_EXCLUSIVE;
	else if (!strcmp(*argv, "-v") || !strcmp(*argv, "-verbose"))
	    flags |= TW_ATTACH_HW_REDIRECT;
	else if (!strcmp(*argv, "-q") || !strcmp(*argv, "-quiet"))
	    flags &= ~TW_ATTACH_HW_REDIRECT;
	else if (!strcmp(*argv, "-f") || !strcmp(*argv, "-force"))
	    force = 1;
	else if (!strncmp(*argv, "-twin@", 6))
	    dpy = *argv + 6;
	else if (!strncmp(*argv, "-hw=", 4)) {
	    if (!strncmp(*argv+4, "display", 7)) {
		printk("%."STR(TW_SMALLBUFF)"s: argument `--hw=display' is for internal use only.\n", MYname);
		TryUsage(NULL);
		return 1;
	    }
	    if (!strncmp(*argv+4, "tty", 3)) {
		buff = *argv + 7;
		s = strchr(buff, ',');
		if (s) *s = '\0';
		
		if (!*buff)
		    /* attach twin to our tty */
		    ourtty = 1;
		else if (*buff == '@' && buff[1]) {
		    if (buff[1] == '-') {
			/*
			 * using server controlling tty makes no sense for twdisplay
			 */
			printk("%."STR(TW_SMALLBUFF)"s: `%."STR(TW_SMALLBUFF)"s' makes sense only with twattach.\n", MYname, *argv);
			return 1;
		    } else if (tty) {
			if (!strcmp(buff+1, tty))
			    /* attach twin to our tty */
			    ourtty = 1;
		    } else {
			printk("%."STR(TW_SMALLBUFF)"s: ttyname() failed: cannot find controlling tty!\n", MYname);
			return 1;
		    }
		} else {
		    printk("%."STR(TW_SMALLBUFF)"s: malformed display hw `%."STR(TW_SMALLBUFF)"s'\n", MYname, *argv);
		    return 1;
		}

		if (s) *s = ',';
		else s = "";
		
		if (ourtty) {
		    buff = getenv("TERM");
		    if (!buff) buff = "";
		    
		    arg = malloc(strlen(tty) + 9 + strlen(s) + (buff ? 6 + strlen(buff) : 0));
		    
		    sprintf(arg, "-hw=tty%s%s%s", (buff ? (byte *)",TERM=" : buff), buff, s);
		} else
		    arg = *argv;
	    } else if ((*argv)[4])
		arg = *argv;
	    else {
		TryUsage(*argv);
		return 1;
	    }
	} else {
	    TryUsage(*argv);
	    return 1;
	}
    }
    
    if (!arg) {
	/* if user did not specify any `--hw=<dpy>', autoprobe */
	arg = "";
    }

#ifdef CONF__ALLOC
    /* do this as soon as possible */
    if (!InitAlloc()) {
	fputs("twin: InitAlloc() failed: internal error!\n", stderr);
	return 1;
    }
#endif
    
    origTWDisplay = CloneStr(getenv("TWDISPLAY"));
    TWDisplay = dpy ? CloneStr(dpy) : origTWDisplay;
    origTERM = CloneStr(getenv("TERM"));
    HOME = CloneStr(getenv("HOME"));

    if (!strcmp(origTWDisplay, TWDisplay)) {
        if (!force) {
            printk("twdisplay: Aborting. Use option `-f' to open a display inside itself.\n");
            TwClose();
            return 1;
        }
    }
    
    InitSignals();
    InitTtysave();
    
#ifdef CONF__ALLOC
    TwConfigMalloc(AllocMem, ReAllocMem, FreeMem);
#endif

    if (TwCheckMagic(display_magic) && TwOpen(TWDisplay)) do {
	byte *buf;
	
	if (!VersionsMatch(force)) {
	    if (!force) {
		printk("twdisplay: Aborting. Use option `-f' to ignore versions check.\n");
		TwClose();
		return 1;
	    }
	}

	if (RegisterRemote(Fd = TwConnectionFd(), NULL, NoOp) == NOSLOT) {
	    TwClose();
	    OutOfMemory();
	    return 1;
	}
	    
	if (!(TMsgPort = TwCreateMsgPort(9, "twdisplay")))
	    break;

	DisplayWidth = TryDisplayWidth = TwGetDisplayWidth();
	DisplayHeight = TryDisplayHeight = TwGetDisplayHeight();

	if (!(HW = AttachDisplayHW(strlen(arg), arg, NOSLOT, 0))) {
	    TwClose();
	    return 1;
	}
	if (!(buf = AllocMem(HW->NameLen + 80))) {
	    QuitDisplayHW(HW);
	    TwClose();
	    OutOfMemory();
	    return 1;
	}
	
	sprintf(buf, "-hw=display@(%.*s),x=%d,y=%d%s%s%s", (int)HW->NameLen, HW->Name,
		(int)HW->X, (int)HW->Y, HW->CanResize ? ",resize" : "",
		/* CanDragArea */ ttrue ? ",drag" : "", ExpensiveFlushVideo ? ",slow" : "");
	
	TwAttachHW(strlen(buf), buf, flags);
	TwFlush();
	
	flags &= TW_ATTACH_HW_REDIRECT;
	
	if (flags)
	    printk("messages reported by twin server...\n");
	
	for (;;) {
	    buff = TwAttachGetReply(&chunk);
	    if (buff <= (byte *)2) {
		ret = (byte)(size_t)buff;
		break;
	    } else if (buff == (byte *)-1)
		/* libTw panic */
		break;

	    printk("  %.*s", (int)chunk, buff);
	}
	flushk();
	
	if (TwInPanic())
	    break;

	if (ourtty) {
	    fputs("\033[2J", stdout);
	    fflush(stdout);
	}

	
	/*
	 * twin waits this before grabbing the display...
	 * so we can fflush(stdout) to show all messages
	 * *BEFORE* twin draws on (eventually) the same tty
	 */
	TwAttachConfirm();

	ResizeDisplay();
	
	if (flags && !ourtty) {
	    if (ret)
		printk("... ok, twin successfully attached.\n");
	    else
		printk("... ach, twin failed to attach.\n");
	    flushk();
	}
	if (ret == 2)
	    /*
	     * twin told us to stay and sit on the display
	     * until it is quitted.
	     */
	    MainLoop(Fd);
	else if (ret)
	    printk("%."STR(TW_SMALLBUFF)"s: twin said we can quit... strange!\n", MYname);
	
	Quit(!ret);
    } while (0);
    
    printk("%."STR(TW_SMALLBUFF)"s: libTw error: %."STR(TW_SMALLBUFF)"s%."STR(TW_SMALLBUFF)"s\n", MYname,
	    TwStrError(TwErrno), TwStrErrorDetail(TwErrno, TwErrnoDetail));
    return 1;
}

void Quit(int status) {
    QuitDisplayHW(HW);
    if (status < 0)
	return; /* give control back to signal handler */
    exit(status);
}
