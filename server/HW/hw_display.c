/*
 *  hw_display.c  --  functions to let twin use `twdisplay' as display
 *
 *  Copyright (C) 2000-2001 by Massimiliano Ghilardi
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
#include "extreg.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"

struct display_data {
    msgport display, Helper;
};

#define displaydata	((struct display_data *)HW->Private)
#define display		(displaydata->display)
#define Helper		(displaydata->Helper)

static msg Msg;
static event_display *ev;
static uldat Used;

INLINE void display_CreateMsg(udat Code, udat Len) {
    Msg->Event.EventDisplay.Code = Code;
    Msg->Event.EventDisplay.Len  = Len;
}

static void display_Beep(void) {
    display_CreateMsg(DPY_Beep, 0);
    Ext(Socket,SendMsg)(display, Msg);
    setFlush();
}

static void display_Configure(udat resource, byte todefault, udat value) {
    display_CreateMsg(DPY_Configure, 0);
	
    ev->X = resource;
    if (todefault)
	ev->Y = -1;
    else
	ev->Y = (dat)value;
    Ext(Socket,SendMsg)(display, Msg);
    setFlush();
}

/* handle messages from twdisplay */
static void display_HandleEvent(display_hw hw) {
    msg hMsg;
    event_any *Event;
    dat x, y, dx, dy;
    udat keys;
    
    SaveHW;
    SetHW(hw);
    
    while ((hMsg = Helper->FirstMsg)) {
	
	Remove(hMsg);
	Event = &hMsg->Event;
	
	switch (hMsg->Type) {
	  case MSG_WIDGET_KEY:
	    KeyboardEventCommon(Event->EventKeyboard.Code, Event->EventKeyboard.ShiftFlags,
				Event->EventKeyboard.SeqLen, Event->EventKeyboard.AsciiSeq);
	    break;
	  case MSG_WIDGET_MOUSE:
	    x = Event->EventMouse.X;
	    y = Event->EventMouse.Y;
	    dx = x == 0 ? -1 : x == DisplayWidth - 1 ? 1 : 0;
	    dy = y == 0 ? -1 : y == DisplayHeight - 1 ? 1 : 0;
	    keys = Event->EventMouse.Code;
	    keys = (keys & HOLD_ANY) | (isPRESS(keys) ? HOLD_CODE(PRESS_N(keys)) : 0);

	    MouseEventCommon(x, y, dx, dy, keys);
	    break;
	  case MSG_WIDGET_GADGET:
	    if (!Event->EventGadget.Code)
		/* 0 == Close Code */
		HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	    break;
	  case MSG_SELECTIONCLEAR:
	    /* selection now owned by some other client on the same display HW as twdisplay */
	    HW->HWSelectionPrivate = (tany)0;
	    /*
	     * DO NOT use (obj)display here instead of (obj)HW, as it is a msgport
	     * and would bypass the OwnerOnce mechanism, often resulting in an infinite loop.
	     */
	    TwinSelectionSetOwner((obj)HW, SEL_CURRENTTIME, SEL_CURRENTTIME);
	    break;
	  case MSG_SELECTIONREQUEST:
	    /*
	     * should never happen, twdisplay uses libTw calls to manage Selection Requests
	     */
	    printk("\ntwin: display_HandleEvent(): unexpected SelectionRequest Message from twdisplay!\n");
#if 0
	    TwinSelectionRequest(Event->EventSelectionRequest.Requestor,
				 Event->EventSelectionRequest.ReqPrivate,
				 TwinSelectionGetOwner());
#endif
	    break;
	  case MSG_SELECTIONNOTIFY:
	    /*
	     * should never happen, twdisplay uses libTw calls to manage Selection Notifies
	     */
	    printk("\ntwin: display_HandleEvent(): unexpected SelectionNotify Message from twdisplay!\n");
#if 0
	    TwinSelectionNotify(dRequestor, dReqPrivate,
				Event->EventSelectionNotify.Magic,
				Event->EventSelectionNotify.MIME,
				Event->EventSelectionNotify.Len,
				Event->EventSelectionNotify.Data);
#endif
	    break;

	  case MSG_DISPLAY:
	    switch (Event->EventDisplay.Code) {
	      case DPY_RedrawVideo:
		/*
		 * Not needed, twdisplay keeps its own copy of Video[]
		 * and never generates DPY_RedrawVideo events
		 */
		printk("\ntwin: display_HandleEvent(): unexpected Display.(DPY_RedrawVideo) Message from twdisplay!\n");
#if 0
		if (Event->EventDisplay.Len == sizeof(dat) * 2)
		    NeedRedrawVideo(Event->EventDisplay.X, Event->EventDisplay.Y,
				    ((udat *)Event->EventDisplay.Data)[0], 
				    ((udat *)Event->EventDisplay.Data)[1]);
		break;
#endif
	      case DPY_Resize:
		if (HW->X != Event->EventDisplay.X ||
		    HW->Y != Event->EventDisplay.Y) {
		
		    HW->X = Event->EventDisplay.X;
		    HW->Y = Event->EventDisplay.Y;
		    ResizeDisplayPrefer(HW);
		}
		break;
	      default:
		break;
	    }
	    break;
	  default:
	    break;
	}
	Delete(hMsg);
    }
    RestoreHW;
}

static void display_HelperH(msgport Port) {
    display_HandleEvent(Port->AttachHW);
}


INLINE void display_DrawHWAttr(dat x, dat y, udat buflen, hwattr *buf) {
    display_CreateMsg(DPY_DrawHWAttr, buflen * sizeof(hwattr));
    ev->X = x;
    ev->Y = y;
    ev->Data = (byte *)buf;
    Ext(Socket,SendMsg)(display, Msg);
}
    
INLINE void display_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV;
    uldat buflen = 0;
    hwattr *buf;
    dat xbegin = x, ybegin = y;
    
    V = Video + x + y * DisplayWidth;
    oV = OldVideo + x + y * DisplayWidth;
    
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

INLINE void display_MoveToXY(udat x, udat y) {
    display_CreateMsg(DPY_MoveToXY, 0);
    ev->X = x;
    ev->Y = y;
    Ext(Socket,SendMsg)(display, Msg);
}

INLINE void display_SetCursorType(uldat type) {
    display_CreateMsg(DPY_SetCursorType, sizeof(uldat));
    ev->Data = (byte *)&type;
    Ext(Socket,SendMsg)(display, Msg);
}

static void display_FlushVideo(void) {
    dat start, end;
    udat i;
    
    /* first burst all changes */
    if (ChangedVideoFlag) {
	for (i=0; i<DisplayHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    end   = ChangedVideo[i>>1][i&1][1];
	    
	    if (start != -1)
		display_Mogrify(start, i>>1, end-start+1);
	}
	setFlush();
    }
    
    /* update the cursor */
    if (!ValidOldVideo || (CursorType != NOCURSOR && (CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
	display_MoveToXY(HW->XY[0] = CursorX, HW->XY[1] = CursorY);
	setFlush();
    }
    if (!ValidOldVideo || CursorType != HW->TT) {
	display_SetCursorType(HW->TT = CursorType);
	setFlush();
    }
    HW->FlagsHW &= ~FlHWChangedMouseFlag;
}

static void display_FlushHW(void) {
    display_CreateMsg(DPY_FlushHW, 0);
    Ext(Socket,SendMsg)(display, Msg);
    if (RemoteFlush(HW->AttachSlot))
	clrFlush();
}

static void display_DetectSize(dat *x, dat *y) {
    *x = HW->X;
    *y = HW->Y;
}

static void display_CheckResize(dat *x, dat *y) {
    if (!HW->CanResize) {
	*x = Min2(*x, HW->X);
	*y = Min2(*y, HW->Y);
    }
}

static void display_Resize(dat x, dat y) {
    /*
     * when !HW->CanResize we send the Resize message even if
     * x == HW->X && y == HW->Y as twdisplay might be using a smaller area
     */
    if (!HW->CanResize || x != HW->X || y != HW->Y) {
	display_CreateMsg(DPY_Resize, 0);
	ev->X = x;
	ev->Y = y;

	if (HW->CanResize) {
	    HW->X = x;
	    HW->Y = y;
	}
	    
	Ext(Socket,SendMsg)(display, Msg);
	setFlush();
    }
}
    
static byte display_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return (Rgt-Left+1) * (Dwn-Up+1) > 20;
}

static void display_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    udat data[4];
    
    data[0] = Rgt;
    data[1] = Dwn;
    data[2] = DstLeft;
    data[3] = DstUp;
    
    display_CreateMsg(DPY_DragArea, 4*sizeof(dat));

    ev->X = Left;
    ev->Y = Up;
    ev->Data = (byte *)data;

    Ext(Socket,SendMsg)(display, Msg);
    setFlush();
}

static void display_SetPalette(udat N, udat R, udat G, udat B) {
    udat data[3];
    data[0] = R;
    data[1] = G;
    data[2] = B;
    
    display_CreateMsg(DPY_SetPalette, 4*sizeof(dat));

    ev->X = N;
    ev->Data = (byte *)data;

    Ext(Socket,SendMsg)(display, Msg);
    setFlush();
}

static void display_ResetPalette(void) {
    display_CreateMsg(DPY_ResetPalette, 0);
    Ext(Socket,SendMsg)(display, Msg);
    setFlush();
}

/*
 * import Selection from twdisplay
 */
static byte display_SelectionImport_display(void) {
    return !HW->HWSelectionPrivate;
}

/*
 * export our Selection to twdisplay
 */
static void display_SelectionExport_display(void) {
    if (!HW->HWSelectionPrivate) {
	HW->HWSelectionPrivate = (tany)display;
	display_CreateMsg(DPY_SelectionExport, 0);
	Ext(Socket,SendMsg)(display, Msg);
	setFlush();
    }
}

/*
 * request Selection from twdisplay
 */
static void display_SelectionRequest_display(obj Requestor, uldat ReqPrivate) {
    if (!HW->HWSelectionPrivate) {
	/*
	 * shortcut: since (display) is a msgport, use fail-safe TwinSelectionRequest()
	 * to send message to twdisplay.
	 */
	TwinSelectionRequest(Requestor, ReqPrivate, (obj)display);
    }
    /* else race! someone else became Selection owner in the meanwhile... */
}

/*
 * notify our Selection to twdisplay
 */
static void display_SelectionNotify_display(uldat ReqPrivate, uldat Magic, CONST byte MIME[MAX_MIMELEN],
					    uldat Len, byte CONST * Data) {
    /*
     * shortcut: since (display) is a msgport, use fail-safe TwinSelectionNotify()
     * to send message to twdisplay.
     */
    TwinSelectionNotify((obj)display, ReqPrivate, Magic, MIME, Len, Data);
}



static void display_QuitHW(void) {
    /* tell twdisplay to cleanly quit */
    display_CreateMsg(DPY_Quit, 4*sizeof(dat));
    Ext(Socket,SendMsg)(display, Msg);
    RemoteFlush(HW->AttachSlot);

    /* then cleanup */
    
    Helper->AttachHW = (display_hw)0; /* to avoid infinite loop */
    Delete(Helper);

    if (!--Used && Msg)
	Delete(Msg), Msg = (msg)0;
    
    /*
     * the rest is done by Act(Quit,HW)(HW) by the upper layers,
     * including KillSlot(HW->AttachSlot) which forces twdisplay
     * to shutdown its display and quit
     */
    
    HW->KeyboardEvent = (void *)NoOp;
    HW->QuitHW = NoOp;
}

static void fix4display(void) {
    byte *arg, *arg17;
    
    if (HW->Name && HW->NameLen > 17 && !CmpMem(HW->Name, "-hw=display@(-hw=", 17) &&
	(arg = memchr(arg17 = HW->Name + 17, ')', HW->NameLen - 17))) {
	
	sprintf(HW->Name, "-display=%.*s", (int)(arg - arg17), arg17);
	HW->NameLen = 9 + (arg - arg17);
    }
}

#ifdef THIS_MODULE
static
#endif
byte display_InitHW(void) {
    byte *s, *arg = HW->Name;
    msgport Port;
    
    if (arg && HW->NameLen > 4) {
	arg += 4;
	if (strncmp(arg, "display", 7))
	    return FALSE; /* user said "use <arg> as display, not twdisplay" */
	arg += 7;
    } else
	arg = NULL;
    
    if (HW->AttachSlot == NOSLOT) {
	/*
	 * we can't start unless we have a connected twdisplay...
	 */
	printk("      display_InitHW() failed: not connected to twdisplay.\n"
	       "      (you cannot use -hw=display from command line or twattach)\n");
	return FALSE;
    }

    if (!(Port = RemoteGetMsgPort(HW->AttachSlot))) {
	printk("      display_InitHW() failed: twdisplay did not create a MsgPort.\n");
	return FALSE;
    }
    
    if (!Ext(Socket,SendMsg)) {
	printk("      display_InitHW() failed: SocketSendMsg() not available.\n"
	       "      (maybe you should load Socket Server module?)\n");
	return FALSE;
    }
    
    if (!(HW->Private = (struct display_data *)AllocMem(sizeof(struct display_data)))
	|| !(Helper = Do(Create,MsgPort)
	     (FnMsgPort, 16, "twdisplay Helper", 0, 0, 0, display_HelperH))
	|| (!Msg && !(Msg = Do(Create,Msg)(FnMsg, MSG_DISPLAY, sizeof(event_display))))) {

	if (HW->Private) {
	    if (Helper)
		Delete(Helper);
	    FreeMem(HW->Private);
	}
	printk("      display_InitHW(): Out of memory!\n");
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
    HW->MouseEvent = (void *)NoOp; /* mouse events handled by display_HelperH */
	    
    HW->XY[0] = HW->XY[1] = 0;
    HW->TT = (uldat)-1; /* force updating cursor */

    HW->ShowMouse = NoOp;
    HW->HideMouse = NoOp;
    HW->UpdateMouseAndCursor = NoOp;
    HW->MouseState.x = HW->MouseState.y = HW->MouseState.keys = 0;
    
    HW->DetectSize  = display_DetectSize;
    HW->CheckResize = display_CheckResize;
    HW->Resize      = display_Resize;

    HW->HWSelectionImport  = display_SelectionImport_display;
    HW->HWSelectionExport  = display_SelectionExport_display;
    HW->HWSelectionRequest = display_SelectionRequest_display;
    HW->HWSelectionNotify  = display_SelectionNotify_display;
    HW->HWSelectionPrivate = (tany)0;
    
    if (arg && strstr(arg, ",drag")) {
	HW->CanDragArea = display_CanDragArea;
	HW->DragArea = display_DragArea;
    } else
	HW->CanDragArea = NULL;

    HW->Beep = display_Beep;
    HW->Configure = display_Configure;
    HW->SetPalette = display_SetPalette;
    HW->ResetPalette = display_ResetPalette;
	    
    HW->QuitHW = display_QuitHW;
    HW->QuitKeyboard = NoOp;
    HW->QuitMouse = NoOp;
    HW->QuitVideo = NoOp;

    HW->DisplayIsCTTY = FALSE;
    HW->FlagsHW &= ~FlHWSoftMouse;

    HW->FlagsHW |= FlHWNeedOldVideo;
    if (arg && strstr(arg, ",slow"))
	HW->FlagsHW |= FlHWExpensiveFlushVideo;
    else
	HW->FlagsHW &= ~FlHWExpensiveFlushVideo;
	
    HW->NeedHW = NEEDPersistentSlot;
    HW->CanResize = arg && strstr(arg, ",resize");
    HW->merge_Threshold = 0;

    display_CreateMsg(DPY_Helper, sizeof(Helper->Id));
    ev->Data = (byte *)&Helper->Id;
    Ext(Socket,SendMsg)(display, Msg);
    /* don't flush now, twdisplay waits for attach messages */

    if (arg && (s = strstr(arg, ",x=")))
	HW->X = atoi(s+3);
    else
	HW->X = GetDisplayWidth();
    if (arg && (s = strstr(arg, ",y=")))
	HW->Y = atoi(s+3);
    else
	HW->Y = GetDisplayHeight();
    
    /*
     * we must draw everything on our new shiny window
     * without forcing all other displays
     * to redraw everything too.
     */
    HW->RedrawVideo = FALSE;
    NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);

    setFlush();
    
    Used++;

    fix4display();
    
    return TRUE;
}

#ifdef THIS_MODULE

byte InitModule(module Module) {
    Module->Private = display_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module Module) {
}

#endif /* THIS_MODULE */
