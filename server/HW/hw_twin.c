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
#include "common.h"

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

struct sel_req {
    uldat Requestor;
    uldat ReqPrivate;
};

static void TW_SelectionRequest_up(uldat Requestor, uldat ReqPrivate);
static void TW_SelectionNotify_up(uldat ReqPrivate, uldat Magic, byte MIME[MAX_MIMELEN],
				  uldat Len, byte *Data);

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

static void TW_HandleMsg(tmsg Msg) {
    tevent_any Event;
    dat x, y, dx, dy;

    Event = &Msg->Event;

    switch (Msg->Type) {
      case TW_MSG_SELECTIONCLEAR:
	HW->HWSelectionPrivate = NULL; /* selection now owned by some other libTw client */
	TwinSelectionSetOwner((obj *)HW, SEL_CURRENTTIME, SEL_CURRENTTIME);
	return;
      case TW_MSG_SELECTIONREQUEST:
	TW_SelectionRequest_up(Event->EventSelectionRequest.Requestor, Event->EventSelectionRequest.ReqPrivate);
	return;
      case TW_MSG_SELECTIONNOTIFY:
	TW_SelectionNotify_up(Event->EventSelectionNotify.ReqPrivate, Event->EventSelectionNotify.Magic,
			      Event->EventSelectionNotify.MIME, Event->EventSelectionNotify.Len,
			      Event->EventSelectionNotify.Data);
	return;
      default:
	break;
    }

    if (Event->EventCommon.Window == Twin) {
	switch (Msg->Type) {
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
	    break;
	  default:
	    break;
	}
    }
    
}

static void TW_KeyboardEvent(int fd, display_hw *hw) {
    tmsg Msg;
    SaveHW;
    SetHW(hw);
    
    while ((Msg = Tw_ReadMsg(Td, FALSE)))
	TW_HandleMsg(Msg);
	
	
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
    if (CursorType != NOCURSOR && (CursorX != HW->XY[0] || CursorY != HW->XY[1])) {
	Tw_GotoXYWindow(Td, Twin, HW->XY[0] = CursorX, HW->XY[1] = CursorY);
	setFlush();
    }
    if (CursorType != HW->TT) {
	/* Tw_SetCursorWindow(Twin, CursorType); */
	byte buff[16];
	sprintf(buff, "\033[?%d;%d;%dc",
		(int)(CursorType & 0xFF),
		(int)((CursorType >> 8) & 0xFF),
		(int)((CursorType >> 16) & 0xFF));
	Tw_WriteAsciiWindow(Td, Twin, strlen(buff), buff);
	HW->TT = CursorType;
	setFlush();
    }
    
    HW->FlagsHW &= ~FlHWChangedMouseFlag;
    HW->RedrawVideo = FALSE;
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
    Tw_DragAreaWindow(Td, Twin, Twin, Left, Up, Rgt, Dwn, DstLeft, DstUp);
    setFlush();
}
#endif

/*
 * import Selection from libTw
 */
static byte TW_SelectionImport_TW(void) {
    return !HW->HWSelectionPrivate;
}

/*
 * export our Selection to libTw
 */
static void TW_SelectionExport_TW(void) {
    if (!HW->HWSelectionPrivate) {
	HW->HWSelectionPrivate = (void *)Tmsgport;
	Tw_SetOwnerSelection(Td, Tmsgport, SEL_CURRENTTIME, SEL_CURRENTTIME);
	setFlush();
    }
}

/*
 * request Selection from libTw
 */
static void TW_SelectionRequest_TW(obj *Requestor, uldat ReqPrivate) {
    if (!HW->HWSelectionPrivate) {
	struct sel_req *SelReq = AllocMem(sizeof(struct sel_req));
	/*
	 * instead of storing (Requestor, ReqPrivate) in some static buffer,
	 * we exploit the ReqPrivate field of libTw Selection Request/Notify
	 */
	if (SelReq) {
	    SelReq->Requestor = (uldat)Requestor;
	    SelReq->ReqPrivate = ReqPrivate;
	    Tw_RequestSelection(Td, Tw_GetOwnerSelection(Td), (uldat)SelReq);
	}
    }
    /* else race! someone else became Selection owner in the meanwhile... */
}

/*
 * request twin Selection from upper layer
 */
static void TW_SelectionRequest_up(uldat Requestor, uldat ReqPrivate) {
    struct sel_req *SelReq = AllocMem(sizeof(struct sel_req));
    /*
     * instead of storing (Requestor, ReqPrivate) in some static buffer,
     * we exploit the ReqPrivate field of libTw Selection Request/Notify
     */
    if (SelReq) {
	SelReq->Requestor = Requestor;
	SelReq->ReqPrivate = ReqPrivate;
	TwinSelectionRequest((obj *)HW, (uldat)SelReq, TwinSelectionGetOwner());
    }
    /* we will get a HW->HWSelectionNotify (i.e. TW_SelectionNotify_TW) call */
}

/*
 * notify our Selection to libTw
 */
static void TW_SelectionNotify_TW(uldat ReqPrivate, uldat Magic, byte MIME[MAX_MIMELEN],
				  uldat Len, byte *Data) {
    struct sel_req *SelReq = (void *)ReqPrivate;

    if (SelReq) {
        Tw_NotifySelection(Td, SelReq->Requestor, SelReq->ReqPrivate, Magic, MIME, Len, Data);
	FreeMem(SelReq);
    }
}

/*
 * notify the libTw Selection to twin upper layer
 */
static void TW_SelectionNotify_up(uldat ReqPrivate, uldat Magic, byte MIME[MAX_MIMELEN],
				  uldat Len, byte *Data) {
    struct sel_req *SelReq = (void *)ReqPrivate;

    if (SelReq) {
        TwinSelectionNotify((void *)SelReq->Requestor, SelReq->ReqPrivate, Magic, MIME, Len, Data);
	FreeMem(SelReq);
    }
}

    
static void TW_QuitHW(void) {
    /* not necessary, and Tmsgport, Twin could be undefined */
    /*
     * Tw_UnMapWindow(Td, Twin);
     * Tw_DeleteWindow(Td, Twin);
     * Tw_DeleteMsgPort(Td, Tmsgport);
     */
    Tw_Close(Td);
    
    UnRegisterRemote(HW->keyboard_slot);
    HW->keyboard_slot = NOSLOT;
    
    HW->KeyboardEvent = (void *)NoOp;
    
    HW->QuitHW = NoOp;
}

byte TW_InitHW(void) {
    byte *arg = HW->Name, *opt = NULL;
    byte name[] = " twin :??? on twin ";
    hwcol namecol[] = " twin :??? on twin ";
    uldat len;
    tmenu Tmenu;
    tscreen Tscreen;
    
    if (arg && HW->NameLen > 4) {
	arg += 4;
	if (strncmp(arg, "twin", 4))
	    return FALSE; /* user said "use <arg> as display, not libTw" */

	arg += 4;
	
	if ((opt = strstr(arg, ",noinput"))) {
	    *opt = '\0';
	    HW->FlagsHW |= FlHWNoInput;
	}
	
	if (*arg == '@')
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
	if (opt) *opt = ',';
	return FALSE;
    }

    if (!(HW->Private = (struct tw_data *)AllocMem(sizeof(struct tw_data)))) {
	fputs("      TW_InitHW(): Out of memory!\n", stderr);
	if (opt) *opt = ',';
	return FALSE;
    }

#ifdef CONF__ALLOC
    Tw_ConfigMalloc(AllocMem, ReAllocMem, FreeMem);
#endif
	
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
		 2 + (HW->X = GetDisplayWidth()), 2 + (HW->Y = GetDisplayHeight()), (uldat)0);

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
	    
	    HW->XY[0] = HW->XY[1] = 0;
	    HW->TT = (uldat)-1; /* force updating cursor */

	    HW->ShowMouse = NoOp;
	    HW->HideMouse = NoOp;
	    HW->UpdateMouseAndCursor = NoOp;
	    HW->MouseState.x = HW->MouseState.y = HW->MouseState.keys = 0;
	    
	    HW->DetectSize  = TW_DetectSize;
	    HW->CheckResize = TW_CheckResize;
	    HW->Resize      = TW_Resize;
	    
	    HW->HWSelectionImport  = TW_SelectionImport_TW;
	    HW->HWSelectionExport  = TW_SelectionExport_TW;
	    HW->HWSelectionRequest = TW_SelectionRequest_TW;
	    HW->HWSelectionNotify  = TW_SelectionNotify_TW;
	    HW->HWSelectionPrivate = NULL;
	    
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
	    HW->FlagsHW &= ~FlHWSoftMouse; /* mouse pointer handled by X11 server */

	    HW->FlagsHW |= FlHWNeedOldVideo;
	    HW->FlagsHW &= ~FlHWExpensiveFlushVideo;
	    HW->NeedHW = 0;
	    HW->CanResize = TRUE;
	    HW->merge_Threshold = 0;

	    /*
	     * we must draw everything on our new shiny window
	     * without forcing all other displays
	     * to redraw everything too.
	     */
	    NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);
	    
	    if (opt) *opt = ',';
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

    if (opt) *opt = ',';
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
