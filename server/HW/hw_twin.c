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


struct tw_data {
    tdisplay Td;
    twindow Twin;
    tmsgport Tmsgport;
};

#define twdata		((struct tw_data *)HW->Private)
#define Td		(twdata->Td)
#define Twin 		(twdata->Twin)
#define Tmsgport	(twdata->Tmsgport)

#define XY ((udat  *)HW->gotoxybuf)
#define TT ((uldat *)HW->cursorbuf)


static void TW_MoveToXY(udat x, udat y) {
    XY[2] = x;
    XY[3] = y;
}

static void TW_SetCursorType(uldat CursorType) {
    TT[1] = CursorType;
}

static void TW_Beep(void) {
    Tw_WriteAsciiWindow(Td, Twin, 1, "\007");
    setFlush();
}

static void TW_Configure(udat resource, byte todefault, udat value) {
    switch (resource) {
      case HW_KBDAPPLIC:
	Tw_WriteAsciiWindow(Td, Twin, 2, todefault || !value ? "\033>" : "\033=");
	setFlush();
	break;
      case HW_ALTCURSKEYS:
	Tw_WriteAsciiWindow(Td, Twin, 5, todefault || !value ? "\033[?1l" : "\033[?1h");
	setFlush();
	break;
      case HW_BELLPITCH:
	if (todefault)
	    Tw_WriteAsciiWindow(Td, Twin, 5, "\033[10]");
	else {
	    byte buf[10];
	    sprintf(buf, "\033[10;%.3hd]", value);
	    Tw_WriteAsciiWindow(Td, Twin, strlen(buf), buf);
	}
	setFlush();
	break;
      case HW_BELLDURATION:
	if (todefault)
	    Tw_WriteAsciiWindow(Td, Twin, 5, "\033[11]");
	else {
	    byte buf[10];
	    sprintf(buf, "\033[11;%.3hd]", value);
	    Tw_WriteAsciiWindow(Td, Twin, strlen(buf), buf);
	}
	setFlush();
	break;
      default:
	break;
    }
}

static void TW_KeyboardEvent(int fd, display_hw *hw) {
    tmsg Msg;
    tevent_any Event;
    dat x, y, dx, dy;
    SaveHW;
    SetHW(hw);
    
    while ((Msg = Tw_ReadMsg(Td, FALSE))) {
	
	Event = &Msg->Event;
	
	if (Event->EventCommon.Window == Twin) switch (Msg->Type) {
	  case TW_MSG_WINDOW_KEY:
	    KeyboardEventCommon(Event->EventKeyboard.Code, Event->EventKeyboard.SeqLen, Event->EventKeyboard.AsciiSeq);
	    break;
	  case TW_MSG_WINDOW_MOUSE:
	    x = Event->EventMouse.X;
	    y = Event->EventMouse.Y;
	    dx = x == 0 ? -1 : x == ScreenWidth - 1 ? 1 : 0;
	    dy = y == 0 ? -1 : y == ScreenHeight - 1 ? 1 : 0;
	    if (dx || dy || x != HW->MouseState.x || y != HW->MouseState.y ||
		(Event->EventMouse.Code & HOLD_ANY) != HW->MouseState.keys)
		
		MouseEventCommon(x, y, dx, dy, Event->EventMouse.Code);
	    HW->MouseState.x = x;
	    HW->MouseState.y = y;
	    HW->MouseState.keys = Event->EventMouse.Code & HOLD_ANY;
	    break;
	  case TW_MSG_WINDOW_CHANGE:
	    if (HW->X != Event->EventWindow.XWidth - 2 ||
		HW->Y != Event->EventWindow.YWidth - 2) {
		
		HW->X = Event->EventWindow.XWidth - 2;
		HW->Y = Event->EventWindow.YWidth - 2;
		ResizeDisplayPrefer(HW);
	    }
	    break;
	  case TW_MSG_WINDOW_GADGET:
	    if (!Event->EventGadget.Code)
		/* 0 == Close Code */
		HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	  default:
	    break;
	}
    }
    if (Tw_InPanic(Td))
	HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
    RestoreHW;
}

INLINE void TW_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV;
    uldat buflen = 0;
    hwattr *buf;
    dat xbegin = x, ybegin = y;
    
    V = Video + x + y * ScreenWidth;
    oV = OldVideo + x + y * ScreenWidth;
    
    for (; len; x++, V++, oV++, len--) {
	if (buflen && ValidOldVideo && *V == *oV) {
	    Tw_WriteHWAttrWindow(Td, Twin, xbegin, ybegin, buflen, buf);
	    buflen = 0;
	}
	if (!ValidOldVideo || *V != *oV) {
	    if (!buflen++) {
		xbegin = x;
		ybegin = y;
		buf = V;
	    }
	}
    }
    if (buflen)
	Tw_WriteHWAttrWindow(Td, Twin, xbegin, ybegin, buflen, buf);
}

static void TW_FlushVideo(void) {
    dat start, end;
    udat i;
    
    
    /* first burst all changes */
    if (ChangedVideoFlag) {
	for (i=0; i<ScreenHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    end   = ChangedVideo[i>>1][i&1][1];
	    
	    if (start != -1)
		TW_Mogrify(start, i>>1, end-start+1);
	}
	setFlush();
    }
    
    /* update the cursor */
    if ((XY[0] != XY[2] || XY[1] != XY[3]) && TT[1] != NOCURSOR) {
	Tw_GotoXYWindow(Td, Twin, XY[2], XY[3]);
	setFlush();
    }
    if (TT[0] != TT[1]) {
	/* Tw_SetCursorWindow(Twin, TT[1]); */
	byte buff[16];
	sprintf(buff, "\033[?%d;%d;%dc",
		(int)(TT[1] & 0xFF),
		(int)((TT[1] >> 8) & 0xFF),
		(int)((TT[1] >> 16) & 0xFF));
	Tw_WriteAsciiWindow(Td, Twin, strlen(buff), buff);
	setFlush();
    }
    
    HW->ChangedMouseFlag = FALSE;
    
    TT[0] = TT[1];
    XY[0] = XY[2];
    XY[1] = XY[3];
}

static void TW_FlushHW(void) {
    byte ret = Tw_TimidFlush(Td);
    if (ret == FALSE)
	HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
    else if (ret == TRUE) {
	if (HW->NeedHW & NEEDFromPreviousFlushHW) {
	    HW->NeedHW &= ~NEEDFromPreviousFlushHW;
	    RemoteCouldWrite(HW->keyboard_slot);
	}
	clrFlush();
    }
    else { /* ret == TRUE+TRUE */
	if (!(HW->NeedHW & NEEDFromPreviousFlushHW)) {
	    HW->NeedHW |= NEEDFromPreviousFlushHW;
	    RemoteCouldntWrite(HW->keyboard_slot);
	}
    }
}

static void TW_DetectSize(udat *x, udat *y) {
    *x = HW->X;
    *y = HW->Y;
}

static void TW_CheckResize(udat *x, udat *y) {
    /* always ok */
}

static void TW_Resize(udat x, udat y) {
    if (x != HW->X || y != HW->Y) {
	Tw_ResizeWindow(Td, Twin, (HW->X = x) + 2, (HW->Y = y) + 2);
	setFlush();
    }
}
    
#if 0
static byte TW_canDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return (Rgt-Left+1) * (Dwn-Up+1) > 20;
}

static void TW_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    TW_FlushVideo(); /* a *must* before any direct libTw operation */
    
    Tw_DragAreaWindow(Td, Twin, Twin, Left, Up, Rgt, Dwn, DstLeft, DstUp);
    setFlush();
}
#endif

static void TW_QuitHW(void) {
    /* not necessary, and Tmsgport, Twin could be undefined */
    /*
    Tw_UnMapWindow(Td, Twin);
    Tw_DeleteWindow(Td, Twin);
    Tw_DeleteMsgPort(Td, Tmsgport);
     */
    Tw_Close(Td);
    
    UnRegisterRemote(HW->keyboard_slot);
    HW->keyboard_slot = NOSLOT;
    
    HW->KeyboardEvent = (void *)NoOp;
    
    HW->QuitHW = NoOp;
}

byte TW_InitHW(void) {
    byte *arg = HW->Name;
    byte name[] = " twin :??? on twin ";
    hwcol namecol[] = " twin :??? on twin ";
    uldat len;
    tmenu Tmenu;
    tscreen Tscreen;
    
    if (arg && HW->NameLen > 4) {
	arg += 4;
	if (strncmp(arg, "twin", 4))
	    return FALSE; /* user said "use <arg> as display, not libTw" */
	
	if (*(arg+=4) == '@')
	    ++arg; /* use specified TWDISPLAY */
	else
	    arg = NULL; /* use default TWDISPLAY */
    } else
	arg = NULL;
    
    if (!arg && !(arg = origTWDisplay)) {
	/*
	 * we can't call Tw_Open(NULL) since getenv("TWDISPLAY")
	 * returns OUR socket... and using ourself as display isn't
	 * exactly a bright idea.
	 */
	fputs("      TW_InitHW() failed: TWDISPLAY is not set\n", stderr);
	return FALSE;
    }

    if (!(HW->Private = (struct tw_data *)AllocMem(sizeof(struct tw_data)))) {
	fputs("      TW_InitHW(): Out of memory!\n", stderr);
	return FALSE;
    }

    if ((Td = Tw_Open(arg)) &&
	
	/*
	 * check if the server supports the functions we need and store their IDs
	 * to avoid deadlocking later when we need them.
	 */
	Tw_FindFunctions(Td, Tw_MapWindow, Tw_WriteAsciiWindow, Tw_WriteHWAttrWindow,
			 Tw_GotoXYWindow, Tw_ResizeWindow, /* Tw_DragAreaWindow, */ NULL) &&

	(Tscreen = Tw_FirstScreen(Td)) &&
	(Tmsgport = Tw_CreateMsgPort
	 (Td, 12, "Twin on Twin", (uldat)0, (udat)0, (byte)0)) &&
	(Tmenu = Tw_CreateMenu
	 (Td, Tmsgport,
	  COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0)) &&
	Tw_Item4MenuCommon(Td, Tmenu)) do {
	    
	    Tw_Info4Menu(Td, Tmenu, TW_ROW_ACTIVE, (uldat)14, " Twin on Twin ", "ptppppppptpppp");
	      
	    strcpy(name+6, TWDisplay);
	    strcat(name+6, " on twin ");
	    len = strlen(name);
	    memset(namecol, '\x9F', len);
	    
	    Twin = Tw_CreateWindow
		(Td, strlen(name), name, namecol, Tmenu, COL(WHITE,BLACK), LINECURSOR,
		 WINDOW_WANT_KEYS|WINDOW_WANT_MOUSE|WINDOW_WANT_CHANGE|WINDOW_DRAG|WINDOW_RESIZE|WINDOW_CLOSE,
		 WINFL_USECONTENTS|WINFL_CURSOR_ON,
		 2 + (HW->X = ScreenWidth), 2 + (HW->Y = ScreenHeight), (uldat)0);

	    if (!Twin)
		break;
	    
	    Tw_SetColorsWindow(Td, Twin, 0x1FF, COL(HIGH|YELLOW,CYAN), COL(HIGH|GREEN,HIGH|BLUE),
			      COL(WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE),
			      COL(WHITE,BLACK), COL(WHITE,HIGH|BLACK), COL(HIGH|BLACK,BLACK), COL(BLACK,HIGH|BLACK));
	    Tw_MapWindow(Td, Twin, Tscreen);
	    
	    Tw_Flush(Td);
	    /*
	     * NOT Tw_Sync() as it might deadlock when
	     * twin:x displays on twin:y which displays on twin:x
	     */
	    
	    HW->mouse_slot = NOSLOT;
	    HW->keyboard_slot = RegisterRemote(Tw_ConnectionFd(Td), HW,
					       (void (*)(int fd, void *))TW_KeyboardEvent);
	    if (HW->keyboard_slot == NOSLOT)
		break;

	    HW->FlushVideo = TW_FlushVideo;
	    HW->FlushHW = TW_FlushHW;

	    HW->KeyboardEvent = TW_KeyboardEvent;
	    HW->MouseEvent = (void *)NoOp; /* mouse events handled by TW_KeyboardEvent */
	    
	    HW->MoveToXY = TW_MoveToXY;
	    HW->SetCursorType = TW_SetCursorType;
	    XY[0] = XY[1] = XY[2] = XY[3] = 0;
	    TT[0] = TT[1] = -1; /* force updating cursor */

	    HW->ShowMouse = NoOp;
	    HW->HideMouse = NoOp;
	    
	    HW->DetectSize  = TW_DetectSize;
	    HW->CheckResize = TW_CheckResize;
	    HW->Resize      = TW_Resize;

	    HW->ExportClipBoard = NoOp;
	    HW->ImportClipBoard = (void *)NoOp;
	    HW->PrivateClipBoard = NULL;
	    
	    HW->CanDragArea = NULL;

	    HW->Beep = TW_Beep;
	    HW->Configure = TW_Configure;
	    HW->SetPalette = (void *)NoOp;
	    HW->ResetPalette = NoOp;
	    
	    HW->QuitHW = TW_QuitHW;
	    HW->QuitKeyboard = NoOp;
	    HW->QuitMouse = NoOp;
	    HW->QuitVideo = NoOp;

	    HW->DisplayIsCTTY = FALSE;
	    HW->SoftMouse = FALSE; /* mouse pointer handled by X11 server */

	    HW->NeedOldVideo = TRUE;
	    HW->ExpensiveFlushVideo = FALSE;
	    HW->NeedHW = 0;
	    HW->merge_Threshold = 0;

	    return TRUE;
	    
	} while (0); else {
	    /* TwErrno(NULL) is valid... used when Tw_Open fails */
	    if ((len = Tw_Errno(Td)))
		fprintf(stderr, "      TW_InitHW() failed: %s\n", Tw_StrError(Td, len));
	    else
		fputs("      TW_InitHW() failed.\n", stderr);
	}
    
    if (Td && Tw_ConnectionFd(Td) >= 0)
	TW_QuitHW();

    return FALSE;
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
