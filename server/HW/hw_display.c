/*
 *  hw_display.c  --  functions to let twin use `twdisplay' as display
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
#include "methods.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"

struct display_data {
    msgport *display, *Helper;
    byte CanDragNow, CanResize;
};

#define displaydata	((struct display_data *)HW->Private)
#define display		(displaydata->display)
#define Helper		(displaydata->Helper)
#define CanDragNow	(displaydata->CanDragNow)
#define CanResize	(displaydata->CanResize)

#define XY ((udat  *)HW->gotoxybuf)
#define TT ((uldat *)HW->cursorbuf)

static msg *Msg;
static event_display *ev;
static uldat Used;

INLINE void display_CreateMsg(udat Code, udat Len) {
    Msg->Event.EventDisplay.Code = Code;
    Msg->Event.EventDisplay.Len  = Len;
}

static void display_MoveToXY(udat x, udat y) {
    XY[2] = x;
    XY[3] = y;
}

static void display_SetCursorType(uldat CursorType) {
    TT[1] = CursorType;
}

static void display_Beep(void) {
    display_CreateMsg(DPY_Beep, 0);
    SocketSendMsg(display, Msg);
    setFlush();
}

static void display_Configure(udat resource, byte todefault, udat value) {
    display_CreateMsg(DPY_Configure, 0);
	
    ev->X = resource;
    if (todefault) {
	if (resource == HW_BELLPITCH || resource == HW_BELLDURATION)
	    ev->Y = MAXUDAT;
	else
	    ev->Y = 0;
    } else
	ev->Y = value;
    SocketSendMsg(display, Msg);
    setFlush();
}

static void display_KeyboardEvent(int fd, display_hw *hw) {
    msg *hMsg;
    event_any *Event;
    dat x, y, dx, dy;
    SaveHW;
    SetHW(hw);
    
    while ((hMsg = Helper->FirstMsg)) {
	
	Remove(hMsg);
	Event = &hMsg->Event;
	
	switch (hMsg->Type) {
	  case MSG_WINDOW_KEY:
	    KeyboardEventCommon(Event->EventKeyboard.Code, Event->EventKeyboard.SeqLen, Event->EventKeyboard.AsciiSeq);
	    break;
	  case MSG_WINDOW_MOUSE:
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
	  case MSG_WINDOW_CHANGE:
	    if (HW->X != Event->EventWindow.XWidth - 2 ||
		HW->Y != Event->EventWindow.YWidth - 2) {
		
		HW->X = Event->EventWindow.XWidth - 2;
		HW->Y = Event->EventWindow.YWidth - 2;
		ResizeDisplayPrefer(HW);
	    }
	    break;
	  case MSG_WINDOW_GADGET:
	    if (!Event->EventGadget.Code)
		/* 0 == Close Code */
		HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	  default:
	    break;
	}
	
	Delete(hMsg);
    }
    RestoreHW;
}

static void display_HelperH(msgport *Port) {
    display_KeyboardEvent(NOFD, Port->AttachHW);
}


INLINE void display_DrawHWAttr(dat x, dat y, udat buflen, hwattr *buf) {
    display_CreateMsg(DPY_DrawHWAttr, buflen * sizeof(hwattr));
    ev->X = x;
    ev->Y = y;
    ev->Data = (byte *)buf;
    SocketSendMsg(display, Msg);
}
    
INLINE void display_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV;
    uldat buflen = 0;
    hwattr *buf;
    dat xbegin = x, ybegin = y;
    
    V = Video + x + y * ScreenWidth;
    oV = OldVideo + x + y * ScreenWidth;
    
    for (; len; x++, V++, oV++, len--) {
	if (buflen && ValidOldVideo && *V == *oV) {
	    display_DrawHWAttr(xbegin, ybegin, buflen, buf);
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
	display_DrawHWAttr(xbegin, ybegin, buflen, buf);
}

INLINE void real_GotoXY(udat x, udat y) {
    display_CreateMsg(DPY_MoveToXY, 0);
    ev->X = x;
    ev->Y = y;
    SocketSendMsg(display, Msg);
    setFlush();
}

INLINE void real_SetCursorType(uldat type) {
    display_CreateMsg(DPY_MoveToXY, sizeof(uldat));
    ev->Data = (byte *)&type;
    SocketSendMsg(display, Msg);
    setFlush();
}

static void display_FlushVideo(void) {
    dat start, end;
    udat i;
    
    /* first burst all changes */
    if (ChangedVideoFlag) {
	for (i=0; i<ScreenHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    end   = ChangedVideo[i>>1][i&1][1];
	    
	    if (start != -1)
		display_Mogrify(start, i>>1, end-start+1);
	}
	setFlush();
    }
    
    /* update the cursor */
    if ((XY[0] != XY[2] || XY[1] != XY[3]) && TT[1] != NOCURSOR)
	real_GotoXY(XY[2], XY[3]);

    if (TT[0] != TT[1])
	real_SetCursorType(TT[1]);
    
    HW->ChangedMouseFlag = FALSE;
    
    TT[0] = TT[1];
    XY[0] = XY[2];
    XY[1] = XY[3];
}

static void display_FlushHW(void) {
    display_CreateMsg(DPY_FlushHW, 0);
    SocketSendMsg(display, Msg);
    if (RemoteFlush(HW->attach))
	clrFlush();
}

static void display_DetectSize(udat *x, udat *y) {
    *x = HW->X;
    *y = HW->Y;
}

static void display_CheckResize(udat *x, udat *y) {
    if (!CanResize) {
	*x = Min2(*x, HW->X);
	*y = Min2(*y, HW->Y);
    }
}

static void display_Resize(udat x, udat y) {
    if (x != HW->X || y != HW->Y) {
	display_CreateMsg(DPY_Resize, 0);
	ev->X = x;
	ev->Y = y;

	if (CanResize) {
	    HW->X = x;
	    HW->Y = y;
	}
	SocketSendMsg(display, Msg);
	setFlush();
    }
}
    
static byte display_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return CanDragNow && (Rgt-Left+1) * (Dwn-Up+1) > 20;
}

static void display_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    udat data[4] = {Rgt, Dwn, DstLeft, DstUp};
    
    display_FlushVideo(); /* a *must* before any direct operation */
    
    display_CreateMsg(DPY_DragArea, 4*sizeof(dat));

    ev->X = Left;
    ev->Y = Up;
    ev->Data = (byte *)data;

    SocketSendMsg(display, Msg);
    setFlush();
}

static void display_QuitHW(void) {
    Helper->AttachHW = (display_hw *)0; /* to avoid infinite loop */
    Delete(Helper);

    if (!--Used && Msg)
	Delete(Msg), Msg = (msg *)0;
    
    /* the rest is done by Act(Quit,HW)(HW) by the upper layers, */
    /* including KillSlot(HW->attach); */
    
    HW->KeyboardEvent = (void *)NoOp;
    HW->QuitHW = NoOp;
}

byte display_InitHW(void) {
    byte *arg = HW->Name;
    msgport *Port;
    
    if (arg && HW->NameLen > 4) {
	arg += 4;
	if (strncmp(arg, "display", 7))
	    return FALSE; /* user said "use <arg> as display, not twdisplay" */
	arg += 7;
    } else
	arg = NULL;
    
    if (HW->attach == NOSLOT) {
	/*
	 * we can't start unless we have a connected twdisplay...
	 */
	fputs("      display_InitHW() failed: not connected to twdisplay.\n"
	      "      (you cannot use -hw=display from command line or twattach)\n", stderr);
	return FALSE;
    }

    if (!(Port = RemoteGetMsgPort(HW->attach))) {
	fputs("      display_InitHW() failed: twdisplay did not create a MsgPort.\n", stderr);
	return FALSE;
    }

    if (!(HW->Private = (struct display_data *)AllocMem(sizeof(struct display_data)))
	|| !(Helper = Do(Create,MsgPort)
	     (FnMsgPort, 16, "twdisplay Helper", (time_t)0, (frac_t)0, 0, display_HelperH))
	|| (!Msg && !(Msg = Do(Create,Msg)(FnMsg, MSG_DISPLAY, sizeof(event_display))))) {

	if (HW->Private) {
	    if (Helper)
		Delete(Helper);
	    FreeMem(HW->Private);
	}
	fputs("      display_InitHW(): Out of memory!\n", stderr);
	return FALSE;
    }

    ev = &Msg->Event.EventDisplay;
    display = Port;
    Helper->AttachHW = HW;
	
    HW->mouse_slot = NOSLOT;
    HW->keyboard_slot = NOSLOT;

    HW->FlushVideo = display_FlushVideo;
    HW->FlushHW = display_FlushHW;

    HW->KeyboardEvent = (void *)NoOp; /* we must go through display_HelperH */
    HW->MouseEvent = (void *)NoOp; /* mouse events handled by display_KeyboardEvent */
	    
    HW->MoveToXY = display_MoveToXY;
    HW->SetCursorType = display_SetCursorType;
    XY[0] = XY[1] = XY[2] = XY[3] = 0;
    TT[0] = TT[1] = -1; /* force updating cursor */

    HW->ShowMouse = NoOp;
    HW->HideMouse = NoOp;

    CanResize = arg && strstr(arg, ",resize");
    HW->DetectSize  = display_DetectSize;
    HW->CheckResize = display_CheckResize;
    HW->Resize      = display_Resize;

    HW->ExportClipBoard = NoOp;
    HW->ImportClipBoard = (void *)NoOp;
    HW->PrivateClipBoard = NULL;
    
    if (arg && strstr(arg, ",drag")) {
	HW->CanDragArea = display_CanDragArea;
	HW->DragArea = display_DragArea;
    } else
	HW->CanDragArea = NULL;
    CanDragNow = !!HW->CanDragArea;

    HW->Beep = display_Beep;
    HW->Configure = display_Configure;
    HW->SetPalette = (void *)NoOp; /* TODO */
    HW->ResetPalette = NoOp;
	    
    HW->QuitHW = display_QuitHW;
    HW->QuitKeyboard = NoOp;
    HW->QuitMouse = NoOp;
    HW->QuitVideo = NoOp;

    HW->DisplayIsCTTY = FALSE;
    HW->SoftMouse = FALSE;

    HW->NeedOldVideo = TRUE;
    HW->ExpensiveFlushVideo = arg && strstr(arg, ",slow");
    HW->NeedHW = NEEDPersistentSlot;
    HW->merge_Threshold = 0;

    display_CreateMsg(DPY_Helper, sizeof(Helper->Id));
    ev->Data = (byte *)&Helper->Id;
    SocketSendMsg(display, Msg);

    HW->X = HW->Y = 0;
    display_Resize(ScreenWidth, ScreenHeight);
    HW->X = ScreenWidth;
    HW->Y = ScreenHeight;
    
    setFlush();
    
    Used++;

    return TRUE;
}

#ifdef MODULE

byte InitModule(module *Module) {
    Module->Private = display_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module *Module) {
}

#endif /* MODULE */
