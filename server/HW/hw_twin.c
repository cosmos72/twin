/*
 *  hw_twin.h  --  functions to let twin display on another twin
 *
 *  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"

#include "libTw.h"
#include "libTwkeys.h"



static display_hw twin;


static twindow Twindow = NOID;
static tmsgport Tmsgport = NOID;

static udat Twidth, Theight;

#define XY ((udat  *)All->gotoxybuf)
#define TT ((uldat *)All->cursorbuf)


static void TW_MoveToXY(udat x, udat y) {
    XY[2] = x;
    XY[3] = y;
}

static void TW_SetCursorType(uldat CursorType) {
    if ((CursorType & 0xF) == 0)
	CursorType |= LINECURSOR;
    else if ((CursorType & 0xF) > SOLIDCURSOR)
	CursorType = (CursorType & ~(uldat)0xF) | SOLIDCURSOR;
    TT[1] = CursorType;
}

static void TW_Beep(void) {
    TwWriteAsciiWindow(Twindow, 1, "\007");
    All->NeedFlushHW = TRUE;
}

static void TW_KeyboardEvent(int fd, uldat slot) {
    tmsg Msg;
    tevent_any Event;
    dat x, y, dx, dy;
    
    while ((Msg = TwReadMsg(FALSE))) {
	
	Event = &Msg->Event;
	
	if (Event->EventCommon.Window == Twindow) switch (Msg->Type) {
	  case TW_MSG_WINDOW_KEY:
	    KeyboardEventCommon(Event->EventKeyboard.Code, Event->EventKeyboard.SeqLen, Event->EventKeyboard.AsciiSeq);
	    break;
	  case TW_MSG_WINDOW_MOUSE:
	    x = Event->EventMouse.X;
	    y = Event->EventMouse.Y;
	    dx = x == 0 ? -1 : x == ScreenWidth - 1 ? 1 : 0;
	    dy = y == 0 ? -1 : y == ScreenHeight - 1 ? 1 : 0;
	    if (dx || dy || x != All->MouseState->x || y != All->MouseState->y ||
		(Event->EventMouse.Code & HOLD_ANY) != All->MouseState->keys)
		
		MouseEventCommon(x, y, dx, dy, Event->EventMouse.Code);
	    break;
	  case TW_MSG_WINDOW_CHANGE:
	    if (Twidth != Event->EventWindow.XWidth - 2 ||
		Theight != Event->EventWindow.YWidth - 2) {
		
		Twidth = Event->EventWindow.XWidth - 2;
		Theight = Event->EventWindow.YWidth - 2;
		All->NeedResizeDisplay = TRUE;
	    }
	    break;
	  case TW_MSG_WINDOW_GADGET:
	    if (!Event->EventGadget.Code)
		/* 0 == Close Code */
		Quit(0);
	  default:
	    break;
	}
    }
}

INLINE void TW_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV;
    uldat buflen = 0;
    hwattr *buf;
    udat xbegin = x, ybegin = y;
    
    V = Video + x + y * ScreenWidth;
    oV = OldVideo + x + y * ScreenWidth;
    
    for (; len; x++, V++, oV++, len--) {
	if (buflen && ValidOldVideo && *V == *oV) {
	    TwWriteHWAttrWindow(Twindow, xbegin, ybegin, buflen, buf);
	    buflen = 0;
	}
	if (!ValidOldVideo || *V != *oV) {
	    *oV = *V;
	    if (!buflen++) {
		xbegin = x;
		ybegin = y;
		buf = V;
	    }
	}
    }
    if (buflen)
	TwWriteHWAttrWindow(Twindow, xbegin, ybegin, buflen, buf);
}

static void TW_FlushVideo(void) {
    dat start, end;
    udat i;
    
    
    /* first burst all changes */
    if (ChangedVideoFlag) {
	for (i=0; i<ScreenHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    end   = ChangedVideo[i>>1][i&1][1];
	    ChangedVideo[i>>1][i&1][0] = -1;
	    
	    if (start != -1)
		TW_Mogrify(start, i>>1, end-start+1);
	}
	All->NeedFlushHW = TRUE;
    }
    
    /* update the cursor */
    if (XY[0] != XY[2] || XY[1] != XY[3]) {
	TwGotoXYWindow(Twindow, XY[2], XY[3]);
	All->NeedFlushHW = TRUE;
    }
    if (TT[0] != TT[1]) {
	/* TwSetCursorWindow(Twindow, TT[1]); */
	byte buff[16];
	sprintf(buff, "\033[?%d;%d;%dc",
		(int)(TT[1] & 0xFF),
		(int)((TT[1] >> 8) & 0xFF),
		(int)((TT[1] >> 16) & 0xFF));
	TwWriteAsciiWindow(Twindow, strlen(buff), buff);
	All->NeedFlushHW = TRUE;
    }
    
    ChangedVideoFlag = ChangedMouseFlag = FALSE;
    ValidOldVideo = TRUE;
    
    TT[0] = TT[1];
    XY[0] = XY[2];
    XY[1] = XY[3];
}

static void TW_Flush(void) {
    TwFlush();
    All->NeedFlushHW = FALSE;
}

static void TW_DetectSize(udat *x, udat *y) {
    *x = Twidth;
    *y = Theight;
}

#if 0
static byte TW_canDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return (Rgt-Left+1) * (Dwn-Up+1) > 20;
}

static void TW_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    TW_FlushVideo(); /* a *must* before any direct libTw operation */
    
    TwDragAreaWindow(Twindow, Twindow, Left, Up, Rgt, Dwn, DstLeft, DstUp);
    All->NeedHW_Flush = TRUE;
}
#endif

static void TW_QuitHW(void) {
    TwUnMapWindow(Twindow);
    TwDeleteWindow(Twindow);
    TwClose();
    
    UnRegisterRemote(All->keyboard_slot);
    All->keyboard_slot = NOSLOT;
    
    twin.KeyboardEvent = (void *)NoOp;
    
    twin.QuitHW = NoOp;

}

display_hw *TW_InitHW(byte *arg) {
    byte name[] = " twin :??? on twin ";
    hwcol namecol[] = " twin :??? on twin ";
    uldat len;
    tmenu Tmenu;
    tscreen Tscreen;
    
    if (arg) {
	if (strncmp(arg, "twin", 4))
	    return NULL; /* user said "use <arg> as display, not libTw" */
	else if (*(arg+=4) == '@')
	    ++arg; /* use specified TWDISPLAY */
	else
	    arg = NULL; /* use default TWDISPLAY */
    }
    
    if (!arg && !(arg = origTWDisplay)) {
	/*
	 * we can't call TwOpen(NULL) since getenv("TWDISPLAY")
	 * returns OUR socket... and using ourself as display isn't
	 * exactly a bright idea.
	 */
	fprintf(errFILE, "twin: TW_InitHW() failed: TWDISPLAY is not set\n");
	return NULL;
    }
    
    if (TwOpen(arg) &&
	(Tscreen = TwFirstScreen()) &&
	(Tmsgport = TwCreateMsgPort
	 (12, "Twin on Twin", (uldat)0, (udat)0, (byte)0)) &&
	(Tmenu = TwCreateMenu
	 (Tmsgport,
	  COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0)) &&
	TwItem4MenuCommon(Tmenu)) do {
	      
	    TwInfo4Menu(Tmenu, TW_ROW_ACTIVE, (uldat)14, " Twin on Twin ", "ptppppppptpppp");
	      
	    strcpy(name+6, TWDisplay);
	    strcat(name+6, " on twin ");
	    len = strlen(name);
	    memset(namecol, '\x9F', len);
	    
	    Twindow = TwCreateWindow
		(strlen(name), name, namecol, Tmenu, COL(WHITE,BLACK), LINECURSOR,
		 WINDOW_WANT_KEYS|WINDOW_WANT_MOUSE|WINDOW_WANT_CHANGE|WINDOW_DRAG|WINDOW_RESIZE|WINDOW_CLOSE,
		 WINFL_USECONTENTS|WINFL_CURSOR_ON,
		 2 + (Twidth = ScreenWidth), 2 + (Theight = ScreenHeight), (uldat)0);

	    if (!Twindow)
		break;
	    
	    TwSetColorsWindow(Twindow, 0x1FF, COL(HIGH|YELLOW,CYAN), COL(HIGH|GREEN,HIGH|BLUE),
			      COL(WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE),
			      COL(WHITE,BLACK), COL(WHITE,HIGH|BLACK), COL(HIGH|BLACK,BLACK), COL(BLACK,HIGH|BLACK));
	    TwMapWindow(Twindow, Tscreen);
	    
	    TwSync();
	    
	    All->keyboard_slot = RegisterRemote(TwConnectionFd(), TW_KeyboardEvent);
	    if (All->keyboard_slot == NOSLOT)
		break;
	    twin.KeyboardEvent = TW_KeyboardEvent;
	    twin.QuitKeyboard  = NoOp;
	    
	    twin.SoftMouse = FALSE; /* mouse pointer handled by X11 server */
	    twin.MouseEvent = (void *)NoOp; /* mouse events handled by X11_KeyboardEvent */
	    twin.ShowMouse = NoOp;
	    twin.HideMouse = NoOp;
	    twin.QuitMouse = NoOp;
	    
	    All->mouse_slot = NOSLOT;
	    
	    twin.canDragArea = NULL;
	    twin.DetectSize  = TW_DetectSize;
	    
	    twin.FlushVideo = TW_FlushVideo;
	    twin.QuitVideo = NoOp;
	    twin.FlushHW = TW_Flush;
	    
	    twin.NeedOldVideo = TRUE;
	    twin.merge_Threshold = 0;
	    twin.ExpensiveFlushVideo = FALSE;
	    
	    twin.MoveToXY = TW_MoveToXY;           XY[0] = XY[1] = XY[2] = XY[3] = 0;
	    twin.SetCursorType = TW_SetCursorType; TT[0] = TT[1] = -1; /* force updating cursor */
	    twin.Beep = TW_Beep;
	    
	    twin.QuitHW = TW_QuitHW;
	    
	    InitTtysave();
	    
	    return &twin;
	    
	} while (0); else {
	    if (TwErrno)
		fprintf(errFILE, "twin: TW_InitHW() failed: %s\n", TwStrError(TwErrno));
	    else
		fprintf(errFILE, "twin: TW_InitHW() failed.\n");
	}
    
    if (TwConnectionFd() >= 0)
	TW_QuitHW();
    
    return NULL;
}

#ifdef MODULE

byte InitModule(module *Module) {
    Module->Private = TW_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module *Module) {
}

#endif /* MODULE */
