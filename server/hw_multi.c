/*
 *  hw_multi.c  --  functions to use multiple
 *                  simultaneous HW/hw_* displays
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
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>

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
#include "common.h"
#include "hw_multi.h"
#include "util.h"
#include "socket.h"

#ifdef CONF_MODULES
# include "dl.h"
#endif

#include "libTwkeys.h"
#include "hotkey.h"


/* HW specific headers */

#ifdef CONF_HW_X11
# include "HW/hw_X11.h"
#endif

#ifdef CONF_HW_DISPLAY
# include "HW/hw_display.h"
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








#define forHW for (HW = All->FirstDisplayHW; HW; HW = HW->Next)

#define safeforHW(s_HW) for (HW = All->FirstDisplayHW; HW && (((s_HW) = HW->Next), TRUE); HW = (s_HW))


/* common data */

static dat (*saveChangedVideo)[2][2];

static udat savedScreenWidth, savedScreenHeight;
static udat TryScreenWidth, TryScreenHeight;

static dat AccelVideo[4] = { MAXDAT, MAXDAT, MINDAT, MINDAT };
byte   StrategyFlag;
frac_t StrategyDelay = (frac_t)0;



/* common functions */

void saveDisplaySize(void) {
    savedScreenWidth = ScreenWidth;
    savedScreenHeight = ScreenHeight;
}

udat GetWidthScreen(screen *Screen) {
    return All->FirstDisplayHW ? Screen ? Screen->ScreenWidth : 0 : savedScreenWidth;
}

udat GetHeightScreen(screen *Screen) {
    return All->FirstDisplayHW ? Screen ? Screen->ScreenHeight : 0 : savedScreenHeight;
}

void UpdateFlagsHW(void) {
    NeedOldVideo = ExpensiveFlushVideo = FALSE;
    CanDragArea = TRUE;

    forHW {
	if (!HW->Quitted) {
	    NeedOldVideo |= HW->NeedOldVideo;
	    ExpensiveFlushVideo |= HW->ExpensiveFlushVideo;
	    CanDragArea &= !!HW->CanDragArea;
	}
    }
}

void RunNoHW(void) {
    savedScreenWidth  = ScreenWidth;
    savedScreenHeight = ScreenHeight;

    ResizeDisplay();

    if (!DisplayHWCTTY) {
	/*
	 * we fork in the background
	 * so that the shell we were started from
	 * can realize we have finished with the tty
	 */
	switch (fork()) {
	  case 0: /* child: continue */
	    DisplayHWCTTY = HWCTTY_DETACHED;
	    setsid();
	    break;
	  case -1:
	    break;
	  default: /* parent: exit */
	    exit(0);
	}
    }
    /* try to fire up the Socket Server ... */
#if defined (CONF_MODULES) && !defined(CONF_SOCKET)
    (void)DlLoad(SocketSo);
#endif
}


void warn_NoHW(uldat len, char *arg, uldat tried) {
#ifdef CONF_MODULES
    if (!tried && !arg)
	    fputs("twin: no display driver compiled into twin.\n"
		  "      please run as `twin -hw=<display>'\n", stderr);
    else
#endif
    {
	fputs("twin: All display drivers failed", stderr);
	if (arg)
	    fprintf(stderr, " for `-hw=%.*s\'", (int)len, arg);
	else
	    putc('.', stderr);
	putc('\n', stderr);
    }
}

#ifdef CONF_MODULES

static byte module_InitHW(void) {
    byte *name, *tmp;
    byte *(*InitD)(void);
    byte *arg = HW->Name;
    uldat len = HW->NameLen;
    module *Module;

    if (!arg || len <= 4)
	return FALSE;
    
    arg += 4; len -= 4; /* skip "-hw=" */
    
    name = memchr(arg, '@', len);
    tmp = memchr(arg, ',', len);
    if (tmp && (!name || tmp < name))
	name = tmp;
    if (name)
	len = name - arg;
    
    if ((name = AllocMem(len + 10))) {
	sprintf(name, "HW/hw_%.*s.so", (int)len, arg);
			
	Module = DlLoadAny(len+9, name);
	
	if (Module) {
	    fprintf(stderr, "twin: starting display driver module `HW/hw_%.*s.so'...\n", (int)len, arg);
	    
	    if ((InitD = Module->Private) && InitD()) {
		fprintf(stderr, "twin: ...module `%s' successfully started.\n", name);
		FreeMem(name);
		HW->Module = Module; Module->Used++;
		return TRUE;
	    }
	    Delete(Module);
	}
    }

    if (Module) {
	fprintf(stderr, "twin: ...module `%s' failed to start.\n", name);
    } else
	fprintf(stderr, "twin: unable to load display driver module `%s' :\n"
			"      %s\n", name, ErrStr);
    
    FreeMem(name);
    return FALSE;
}

#endif /* CONF_MODULES */

#if defined(CONF_HW_X11) || defined(CONF_HW_TWIN) || defined(CONF_HW_DISPLAY) || defined(CONF_HW_TTY) || defined(CONF_HW_GGI)
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

static void fix4(byte *s, display_hw *D_HW) {
    uldat len;
    if (!D_HW->NameLen) {
	if (D_HW->Name)
	    FreeMem(D_HW->Name), D_HW->Name = NULL;
	len = strlen(s) + 4;
	if ((D_HW->Name = AllocMem(len + 1))) {
	    sprintf(D_HW->Name, "-hw=%s", s);
	    D_HW->NameLen = len;
	}
    }
}
#endif /* defined(CONF_HW_X11) || defined(CONF_HW_TWIN) || defined(CONF_HW_DISPLAY) || defined(CONF_HW_TTY) || defined(CONF_HW_GGI) */

/*
 * InitDisplayHW runs HW specific InitXXX() functions, starting from best setup
 * and falling back in case some of them fails.
 */
byte InitDisplayHW(display_hw *D_HW) {
    byte *arg = D_HW->Name;
    uldat tried = 0;
    byte success;

    SaveHW;
    SetHW(D_HW);

    D_HW->Quitted = D_HW->DisplayIsCTTY = D_HW->NeedHW = FALSE;
    
    if (arg && !strncmp(arg, "-hw=", 4))
	arg += 4;
    else
	arg = NULL;

    success =
#ifdef CONF_HW_X11
	(check4("X", arg) && (tried++, X11_InitHW()) && (fix4("X", D_HW), TRUE)) ||
#endif
#ifdef CONF_HW_TWIN
	(check4("twin", arg) && (tried++, TW_InitHW()) && (fix4("twin", D_HW), TRUE)) ||
#endif
#ifdef CONF_HW_DISPLAY
	(check4("display", arg) && (tried++, display_InitHW()) && (fix4("display", D_HW), TRUE)) ||
#endif
#ifdef CONF_HW_TTY
	(check4("tty", arg) && (tried++, tty_InitHW()) && (fix4("tty", D_HW), TRUE)) ||
#endif
#ifdef CONF_HW_GGI
	(check4("ggi", arg) && (tried++, GGI_InitHW()) && (fix4("ggi", D_HW), TRUE)) ||
#endif
#ifdef CONF_MODULES
	module_InitHW() ||
#endif
	(warn_NoHW(arg ? D_HW->NameLen - 4 : 0, arg, tried), FALSE);

    if (success) {
	if (!DisplayHWCTTY && D_HW->DisplayIsCTTY)
	    DisplayHWCTTY = D_HW;
	if (All->FnHookDisplayHW)
	    All->FnHookDisplayHW(All->HookDisplayHW);
	UpdateFlagsHW(); /* this garbles HW... not a problem here */
    } else
	D_HW->Quitted = TRUE;
    
    fflush(stderr);
    RestoreHW;

    return success;
}

void QuitDisplayHW(display_hw *D_HW) {
    msgport *MsgPort;
    uldat slot;
    SaveHW;
    
    if (D_HW) {
	if (D_HW->QuitHW)
	    HW = D_HW, D_HW->QuitHW();

	D_HW->Quitted = TRUE;
	
	if ((slot = D_HW->AttachSlot) != NOSLOT) {
	    /* avoid KillSlot <-> DeleteDisplayHW infinite recursion */
	    if ((MsgPort = RemoteGetMsgPort(slot)))
		MsgPort->AttachHW = (display_hw *)0;
	    KillSlot(slot);
	}

#ifdef CONF_MODULES
	if (D_HW->Module) {
	    D_HW->Module->Used--;
	    Delete(D_HW->Module);
	    D_HW->Module = (module *)0;
	}
#endif
	UpdateFlagsHW(); /* this garbles HW... not a problem here */
    }
    RestoreHW;
}

display_hw *AttachDisplayHW(uldat len, byte *arg, uldat slot) {
    display_hw *D_HW;

    if ((len && len <= 4) || CmpMem("-hw=", arg, Min2(len,4))) {
	fprintf(stderr, "twin: specified `%.*s\' is not `-hw=<display>\'\n",
		(int)len, arg);
	fflush(stderr);
	return NULL;
    }
    
    if (!All->FirstDisplayHW) {
	/* was running with no HW, restore ScreenWidth/ScreenHeight */
	if (savedScreenWidth && savedScreenHeight) {
	    TryScreenWidth = savedScreenWidth;
	    TryScreenHeight = savedScreenHeight;
	    savedScreenWidth = savedScreenHeight = 0;
	}
    }
    
    if ((D_HW = Do(Create,DisplayHW)(FnDisplayHW, len, arg))) {
	D_HW->AttachSlot = slot;
	if (Act(Init,D_HW)(D_HW)) {
	    if (ResizeDisplay()) {
		DrawArea(FULLSCREEN);
		UpdateCursor();
	    }
	    return D_HW;
	}
	/* failed, clean up without calling RunNoHW() */
	D_HW->Quitted = TRUE;
	D_HW->AttachSlot = NOSLOT;
	D_HW->QuitHW = NoOp;
	Delete(D_HW);
	D_HW = (display_hw *)0;
    }
    return D_HW;
}


byte DetachDisplayHW(uldat len, byte *arg) {
    byte done = FALSE;
    display_hw *s_HW;
    if (len) {
	safeforHW(s_HW) {
	    if (HW->NameLen == len && !CmpMem(HW->Name, arg, len)) {
		Delete(HW);
		done = TRUE;
		break;
	    }
	}
    } else {
	QuitHW();
	done = TRUE;
    }
    return done;
}

byte InitHW(void) {
    byte **arglist = orig_argv;
    byte *dummy[2] = {"", NULL}, ret = FALSE;

    if (!arglist || !*arglist) {
	/* autoprobe */
	arglist = dummy;
    }
    while (*arglist) {
	ret |= !!AttachDisplayHW(strlen(*arglist), *arglist, NOSLOT);
	arglist++;
    }
    if (!ret)
	fputs("\ntwin:   \033[1mALL  DISPLAY  DRIVERS  FAILED.\033[0m\n", stderr);
    return ret;
}

void QuitHW(void) {
    DeleteList(All->FirstDisplayHW);
}

byte RestartHW(byte verbose) {
    display_hw *s_HW;
    byte ret = FALSE;
    
    if (All->FirstDisplayHW) {
	safeforHW(s_HW) {
	    if (Act(Init,HW)(HW))
		ret = TRUE;
	    else
		Delete(HW);
	}
	if (ret) {
	    ResizeDisplay();
	    DrawArea(FULLSCREEN);
	    UpdateCursor();
	} else {
	    fputs("\ntwin:   \033[1mALL  DISPLAY  DRIVERS  FAILED.\033[0m\n", stderr);
	    fflush(stderr);
	    RunNoHW();
	}
    } else if (verbose) {
	fputs("twin: RestartHW(): All display drivers removed by SuspendHW().\n"
	      "      No display available for restarting, use twattach or twdisplay\n", stderr);
	fflush(stderr);
    }
    return ret;
}

void SuspendHW(byte verbose) {
    display_hw *s_HW;
    safeforHW(s_HW) {
	if (HW->AttachSlot != NOSLOT && HW->NeedHW & NEEDPersistentSlot)
	    /* we will not be able to restart it */
	    Delete(HW);
	else
	    Act(Quit,HW)(HW);
    }
    if (verbose && !All->FirstDisplayHW) {
	fputs("twin: SuspendHW(): All display drivers had to be removed\n"
	      "      since they were attached to clients (twattach/twdisplay).\n"
	      "twin: --- STOPPED ---\n", stderr);
	fflush(stderr);
    }
}

void PanicHW(void) {
    display_hw *s_HW;
    
    if (NeedHW & NEEDPanicHW) {
	safeforHW(s_HW) {
	    if (HW->NeedHW & NEEDPanicHW)
		Delete(HW);
	}
	NeedHW &= ~NEEDPanicHW;
    }
}

void ResizeDisplayPrefer(display_hw *D_HW) {
    SaveHW;
    SetHW(D_HW);
    D_HW->DetectSize(&TryScreenWidth, &TryScreenHeight);
    NeedHW |= NEEDResizeDisplay;
    RestoreHW;
}

/*
 * return TRUE if ScreenWidth or ScreenHeight were changed
 */
byte ResizeDisplay(void) {
    screen *fScreen;
    udat Width, Height;
    byte change = FALSE;
    
    if (All->FirstDisplayHW) {
	
	if (!TryScreenWidth || !TryScreenHeight) {
	    /*
	     * we are trying to come up with a fair display size
	     * and have all HW agree on it.
	     */
	    TryScreenWidth = TryScreenHeight = MAXUDAT;
	    forHW {
		HW->DetectSize(&Width, &Height);
		if (TryScreenWidth  > Width)  TryScreenWidth  = Width;
		if (TryScreenHeight > Height) TryScreenHeight = Height;
	    }
	}
	
	/* now let's check how much we can resize all HW displays... */
	do {
	    Width = TryScreenWidth;
	    Height = TryScreenHeight;
	    forHW {
		HW->CheckResize(&TryScreenWidth, &TryScreenHeight);
	    }
	} while (TryScreenWidth < Width || TryScreenHeight < Height);
	 
	if (!TryScreenWidth || TryScreenWidth == MAXUDAT)
	    TryScreenWidth = ScreenWidth;
	if (!TryScreenHeight || TryScreenHeight == MAXUDAT)
	    TryScreenHeight = ScreenHeight;
	
	/* size seems reasonable, apply it to all HW displays */
	forHW {
	    HW->Resize(TryScreenWidth, TryScreenHeight);
	}
    } else
	TryScreenWidth = TryScreenHeight = 1;

    change = ScreenWidth != TryScreenWidth || ScreenHeight != TryScreenHeight;
    
    if (!NeedOldVideo && OldVideo) {
	FreeMem(OldVideo);
	OldVideo = NULL;
    } else if ((NeedOldVideo && !OldVideo) || change) {
	if (!(OldVideo = (hwattr *)ReAllocMem(OldVideo, TryScreenWidth*TryScreenHeight*sizeof(hwattr)))) {
	    fprintf(stderr, "twin: out of memory!\n");
	    Quit(1);
	}
	ValidOldVideo = FALSE;
    }
    
    if (!Video || change) {
	ScreenWidth = TryScreenWidth;
	ScreenHeight = TryScreenHeight;
	
	if (!(Video = (hwattr *)ReAllocMem(Video, ScreenWidth*ScreenHeight*sizeof(hwattr))) ||
	    !(ChangedVideo = (dat (*)[2][2])ReAllocMem(ChangedVideo, ScreenHeight*sizeof(dat)*4)) ||
	    !(saveChangedVideo = (dat (*)[2][2])ReAllocMem(saveChangedVideo, ScreenHeight*sizeof(dat)*4))) {
	    
	    fprintf(stderr, "twin: out of memory!\n");
	    Quit(1);
	}
	WriteMem(ChangedVideo, 0xff, ScreenHeight*sizeof(dat)*4);
    
	for (fScreen = All->FirstScreen; fScreen; fScreen = fScreen->Next) {
	    fScreen->ScreenHeight = ScreenHeight;
	    fScreen->ScreenWidth  = ScreenWidth;
	}
    }
    NeedHW &= ~NEEDResizeDisplay;

    TryScreenWidth = TryScreenHeight = 0;
    
    return change;
}

void BeepHW(void) {
    NeedHW |= NEEDBeepHW;
}

void ConfigureHW(udat resource, byte todefault, udat value) {
    forHW {
	HW->Configure(resource, todefault, value);
    }
}

void SetPaletteHW(udat N, udat R, udat G, udat B) {
    if (N <= MAXCOL) {
	palette c = {R, G, B};
	if (CmpMem(&Palette[N], &c, sizeof(palette))) {
	    Palette[N] = c;
	    forHW {
		HW->SetPalette(N, R, G, B);
	    }
	}
    }
}

void ResetPaletteHW(void) {
    forHW {
	HW->ResetPalette();
    }
}

void ExportClipBoard(void) {
    forHW {
	if (HW->ExportClipBoard)
	    HW->ExportClipBoard();
    }
    NeedHW &= ~NEEDExportClipBoard;
}

void ImportClipBoard(byte Wait) {
    if ((HW = All->MouseHW)) {
	if (HW->ImportClipBoard)
	    HW->ImportClipBoard(Wait);
    }
}

INLINE void DiscardBlinkVideo(void) {
    int i;
    uldat start, len;
    hwattr *V;
    
    for (i=0; i<ScreenHeight*2; i++) {
	start = (uldat)ChangedVideo[i>>1][i&1][0];

	if (start != -1) {
	    len = (uldat)ChangedVideo[i>>1][i&1][1] + 1 - start;
	    start += (i>>1)*ScreenWidth;

	    for (V = &Video[start]; len; V++, len--)
		*V &= ~HWATTR(COL(0,HIGH), (hwfont)0);
	}
    }
}

INLINE void OptimizeChangedVideo(void) {
    uldat _start, start, _end, end;
    int i;
    
    for (i=0; i<ScreenHeight*2; i++) {
	start = (uldat)ChangedVideo[i>>1][!(i&1)][0];
	    
	if (start != (uldat)-1) {
	    
	    start += (i>>1) * ScreenWidth;
	    _start = start;

	    _end = end = (uldat)ChangedVideo[i>>1][!(i&1)][1] + (i>>1) * ScreenWidth;
		
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

INLINE void SyncOldVideo(void) {
    uldat start, len;
    int i;

    if (ChangedVideoFlag) {
	for (i=0; i<ScreenHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    
	    if (start != -1) {
		len = ChangedVideo[i>>1][i&1][1] + 1 - start;
		start += (i>>1)*ScreenWidth;
		
		ChangedVideo[i>>1][i&1][0] = -1;
	    
		CopyMem(Video + start, OldVideo + start, len * sizeof(hwattr));
	    }
	}
    }
}

#define MaxRecentBeepHW ((byte)30)

void FlushHW(void) {
    static timevalue LastBeep = {(time_t)0, (frac_t)0};
    timevalue tmp = {(time_t)0, 100 MilliSECs};
    byte doBeep = FALSE, saved = FALSE, mangled = FALSE;
    byte saveChangedVideoFlag, saveValidOldVideo;
    /*
     * we can NEVER get (saved == FALSE && mangled == TRUE)
     * as it would mean we have irreversibly lost ChangedVideo[]
     */

    /*
     * displaying on ourselves can cause infine beeping loops...
     * avoid it
     */
    if (NeedHW & NEEDBeepHW) {
	IncrTime(&tmp, &LastBeep);
	if (CmpTime(&All->Now, &tmp) >= 0) {
	    CopyMem(&All->Now, &LastBeep, sizeof(timevalue));
	    doBeep = TRUE;
	}
	NeedHW &= ~NEEDBeepHW;
    }

    if (All->SetUp->Flags & SETUP_NOBLINK)
	DiscardBlinkVideo();

    if (NeedOldVideo && ValidOldVideo)
	OptimizeChangedVideo();
    
    forHW {
	/*
	 * adjust ChangedVideoFlag and ChangedVideo[]
	 * to include HW supplied area HW->Redraw*
	 * 
	 * we do not save/restore ChangedVideoFlagAgain since it's used
	 * to decide buffering strategies, so must reflect whether
	 * some display had to draw something or not.
	 */
	if (mangled) {
	    ValidOldVideo = saveValidOldVideo;
	    ChangedVideoFlag = saveChangedVideoFlag;
	    CopyMem(saveChangedVideo, ChangedVideo, ScreenHeight*sizeof(dat)*4);
	    mangled = FALSE;
	}
	if (HW->RedrawVideo) {
	    if (!saved) {
		saveValidOldVideo = ValidOldVideo;
		saveChangedVideoFlag = ChangedVideoFlag;
		CopyMem(ChangedVideo, saveChangedVideo, ScreenHeight*sizeof(dat)*4);
		saved = TRUE;
	    }
	    DirtyVideo(HW->RedrawLeft, HW->RedrawUp, HW->RedrawRight, HW->RedrawDown);
	    ValidOldVideo = FALSE;
	    /* the OldVideo[] caching would make all this stuff useless otherwise */
	    mangled = TRUE;
	}
	if (doBeep)
	    HW->Beep();

	HW->FlushVideo();

	HW->RedrawVideo = FALSE;
	
	if (HW->NeedHW & NEEDFlushHW)
	    HW->FlushHW();
    }
    if (NeedHW & NEEDFlushStdout)
	fflush(stdout), NeedHW &= ~NEEDFlushStdout;
    
    SyncOldVideo();

    ChangedVideoFlag = FALSE;
    ValidOldVideo = TRUE;
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
	Event->Code = HOT_KEY;
	Event->SeqLen = Len;
	CopyMem(Seq, Event->AsciiSeq, Len);
	Event->AsciiSeq[Len] = '\0'; /* terminate string with \0 */
	SendMsg(Window->Menu->MsgPort, Msg);
    }
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

void RefreshVideo(void) {
    ValidOldVideo = FALSE;
    DirtyVideo(0, 0, ScreenWidth - 1, ScreenHeight - 1);
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

byte AllHWCanDragAreaNow(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    dat DstRgt = DstLeft + (Rgt - Left), DstDwn = DstUp + (Dwn - Up);
    byte Accel;

    if (CanDragArea && Strategy4Video(DstLeft, DstUp, DstRgt, DstDwn) == HW_ACCEL) {
	Accel = TRUE;
	forHW {
	    if (HW->CanDragArea && HW->CanDragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp))
		;
	    else {
		Accel = FALSE;
		break;
	    }
	}
    } else
	Accel = FALSE;

    return Accel;
}

void DragAreaHW(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    forHW {
	HW->DragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp);
    }
}

byte *GetClipData(void) {
    return All->ClipData;
}

uldat GetClipLen(void) {
    return All->ClipLen;
}


byte MouseEventCommon(dat x, dat y, dat dx, dat dy, udat IdButtons) {
    dat prev_x, prev_y;
    udat Buttons, OldButtons, Overload;
    mouse_state *OldState;
    udat result;
    byte ret = TRUE;
    
    OldState=&HW->MouseState;
    OldButtons=OldState->keys;
    prev_x = OldState->x;
    prev_y = OldState->y;

    x = Max2(x, 0); x = Min2(x, ScreenWidth - 1);
    OldState->delta_x = x == 0 ? Min2(dx, 0) : x == ScreenWidth - 1 ? Max2(dx, 0) : 0;

    y = Max2(y, 0); y = Min2(y, ScreenHeight - 1);
    OldState->delta_y = y == 0 ? Min2(dy, 0) : y == ScreenHeight - 1 ? Max2(dy, 0) : 0;
	
    if (x != prev_x || y != prev_y)
	HW->ChangedMouseFlag = TRUE;

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
	    if (!StdAddEventMouse(MSG_MOUSE, DRAG_MOUSE | OldButtons, x, y))
		ret = FALSE;
	}
	if ((Buttons & HOLD_LEFT) != (OldButtons & HOLD_LEFT)) {
	    result = (Buttons & HOLD_LEFT ? DOWN_LEFT : RELEASE_LEFT) | (OldButtons &= ~HOLD_LEFT);
	    OldButtons |= Buttons & HOLD_LEFT;
	    if (!StdAddEventMouse(MSG_MOUSE, result, x, y))
		ret = FALSE;
	}
	if ((Buttons & HOLD_MIDDLE) != (OldButtons & HOLD_MIDDLE)) {
	    result = (Buttons & HOLD_MIDDLE ? DOWN_MIDDLE : RELEASE_MIDDLE) | (OldButtons &= ~HOLD_MIDDLE);
	    OldButtons |= Buttons & HOLD_MIDDLE;
	    if (!StdAddEventMouse(MSG_MOUSE, result, x, y))
		ret = FALSE;
	}
	if ((Buttons & HOLD_RIGHT) != (OldButtons & HOLD_RIGHT)) {
	    result = (Buttons & HOLD_RIGHT ? DOWN_RIGHT : RELEASE_RIGHT) | (OldButtons &= ~HOLD_RIGHT);
	    OldButtons |= Buttons & HOLD_RIGHT;
	    if (!StdAddEventMouse(MSG_MOUSE, result, x, y))
		ret = FALSE;
	}

	/* keep it available */
	All->MouseHW = HW;
    }
    return ret;
}

byte StdAddEventMouse(udat CodeMsg, udat Code, dat MouseX, dat MouseY) {
    msg *Msg;
    event_mouse *Event;
    
    if ((Code & ANY_ACTION_MOUSE)==DRAG_MOUSE
	&& (Msg = WM_MsgPort->LastMsg)
	&& Msg->Type==MSG_MOUSE
	&& (Event=&Msg->Event.EventMouse)
	&& Event->Code==Code) {
	Event->X=MouseX;
	Event->Y=MouseY;
	return TRUE;
    }
    if ((Msg=Do(Create,Msg)(FnMsg, CodeMsg, sizeof(event_mouse)))) {
	Event=&Msg->Event.EventMouse;
	Event->Code=Code;
	Event->ShiftFlags=(udat)0;
	Event->X=MouseX;
	Event->Y=MouseY;
	SendMsg(WM_MsgPort, Msg);
	return TRUE;
    }
    return FALSE;
}

byte KeyboardEventCommon(udat Code, udat Len, byte *Seq) {
    event_keyboard *Event;
    msg *Msg;

    if ((Msg=Do(Create,Msg)(FnMsg, MSG_KEY, Len + sizeof(event_keyboard)))) {
	Event = &Msg->Event.EventKeyboard;
	    
	Event->Code = Code;
	Event->ShiftFlags = (udat)0;
	Event->SeqLen = Len;
	CopyMem(Seq, Event->AsciiSeq, Len);
	Event->AsciiSeq[Len] = '\0'; /* terminate string with \0 */
	SendMsg(WM_MsgPort, Msg);
	return TRUE;
    }
    return FALSE;
}

#ifdef __linux__
# include <linux/kd.h>
#endif

byte InitTransUser(void) {
    byte c;
#ifdef __linux__
    scrnmap_t map[E_TABSZ];
    
    if (ioctl(0, GIO_SCRNMAP, map) == 0) {
	if (sizeof(scrnmap_t) == 1)
	    CopyMem(map+0x80, All->Gtranslations[USER_MAP], 0x80);
	else {
	    for (c = 0; c < 0x80; c++)
		All->Gtranslations[USER_MAP][c] = (byte)map[c | 0x80];
	}
    } else
#endif
    {
	for (c = 0; c < 0x80; c++)
	    All->Gtranslations[USER_MAP][c] = c | 0x80;
    }
    return TRUE;
}

