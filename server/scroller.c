/*
 *  scroller.c  --  manage autorepeat of virtual screen and window scrolling
 *
 *  Copyright (C) 1993-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "data.h"
#include "methods.h"

#include "hw.h"
#include "resize.h"
#include "util.h"

msgport *Scroller_MsgPort;
static void ScrollerH(msgport *MsgPort);

/*
 * just dummy msgs that we receive when we are called
 * to start/stop scrolling immediately
 */
msg *Do_Scroll, *Dont_Scroll;

byte InitScroller(void) {
    if ((Scroller_MsgPort=Do(Create,MsgPort)
	 (FnMsgPort, 8, "Scroller", (time_t)0, 401 MilliSECs, (byte)0, ScrollerH)) &&
	(Do_Scroll=Do(Create,Msg)(FnMsg, 0, 0)) &&
	(Dont_Scroll=Do(Create,Msg)(FnMsg, 0, 0))) {

	return TRUE;
    }
    return FALSE;
}

INLINE void ScrollerDeactivate(void) {
    Scroller_MsgPort->PauseDuration.Fraction = 333 MilliSECs + 1;
    Scroller_MsgPort->WakeUp = 0;
}

INLINE void ScrollerAutoRepeat(void) {
    if (Scroller_MsgPort->PauseDuration.Fraction > 333 MilliSECs)
	Scroller_MsgPort->PauseDuration.Fraction = 333 MilliSECs;
    else
	Scroller_MsgPort->PauseDuration.Fraction = 30 MilliSECs;
}

INLINE void ScrollerDelayRepeat(void) {
    Scroller_MsgPort->PauseDuration.Fraction = 333 MilliSECs + 1;
}

static void ScrollerH(msgport *MsgPort) {
    msg *Msg, *saveMsg;
    mouse_state *Mouse;
    screen *Screen;
    udat CallKey, Funct;
    uldat Attrib;
    dat Limit;
    dat Mouse_delta_x, Mouse_delta_y;
    byte State, FlagsMove, FlagDeskScroll, FlagWinScroll, WinScrolled = FALSE;
    window *FocusWindow;
    
    while ((Msg=Scroller_MsgPort->FirstMsg)) {
	//		Beep();
	Remove(Msg);
	if (Msg == Do_Scroll || Msg == Dont_Scroll)
	    saveMsg = Msg;
	else
	    Delete(Msg);
    }

    if (saveMsg == Dont_Scroll || !All->MouseHW) {
	ScrollerDeactivate();
	return;
    }
    
    Mouse = &All->MouseHW->MouseState;

    if ((FocusWindow=All->FirstScreen->FocusWindow)) {
	Attrib=FocusWindow->Attrib;
	FlagWinScroll = (XAND(Attrib, WINDOW_X_BAR | X_BAR_SELECT)
			 || XAND(Attrib, WINDOW_Y_BAR | Y_BAR_SELECT)) && !(Attrib & TAB_SELECT);
    } else
	FlagWinScroll = FALSE;
    
    FlagDeskScroll=!(All->FullShiftFlags & FULL_SCROLL_LOCK_ACTIVE) && Mouse->keys;

    FlagsMove=All->FlagsMove;
    if (FlagsMove & GLMOVE_1stSCREEN)
	State=STATE_SCREEN;
    else if (FlagsMove & GLMOVE_1stMENU)
	State=STATE_MENU;
    else if (FlagsMove & GLMOVE_ANY_1stWIN)
	State=STATE_WINDOW;
    else
	State=STATE_DEFAULT;
    
    if (State!=STATE_WINDOW && State!=STATE_DEFAULT) {
	ScrollerDeactivate();
	return;
    }
    
    if (State==STATE_WINDOW && All->FlagsMove & GLMOVE_SCROLL_1stWIN && FlagWinScroll) {
	if ((WinScrolled = ExecScrollFocusWindow()))
	    ScrollerAutoRepeat();
    }
    
    
    Mouse_delta_x=-Mouse->delta_x;
    Mouse_delta_y=-Mouse->delta_y;
    FlagDeskScroll &= Mouse_delta_x || Mouse_delta_y;
    
    if (!FlagDeskScroll) {
	if (!WinScrolled)
	    ScrollerDelayRepeat();
	return;
    }
    
    CallKey = DRAG_MOUSE | Mouse->keys;
    
    /*
     Funziona solo se GlobalKeyCodes e GlobalMouseCodes
     hanno formato normale (non compresso)
     */
    
    if (All->GlobalMouseCodes[State][0])           /*Dati Compressi : non implementato */
	Funct=(udat)0;
    else
	Funct=FnM(All->GlobalMouseCodes[State],CallKey);
    
    if (FlagDeskScroll && (Funct==STDEF_MOUSE_DRAG_SOME || Funct==STDEF_MOUSE_DRAG_NULL)) {
	Screen=All->FirstScreen;

	Limit=All->SetUp->MaxMouseSnap;
	if (Scroller_MsgPort->PauseDuration.Fraction > 333 MilliSECs) {
	    Mouse_delta_x = Sign(Mouse_delta_x);
	    Mouse_delta_y = Sign(Mouse_delta_y);
	}
		
	DragFirstScreen(Mouse_delta_x*Limit, Mouse_delta_y*Limit);
	
	if (!WinScrolled)
	    ScrollerAutoRepeat();

	if (!StdAddEventMouse(MSG_MOUSE, All->FullShiftFlags, CallKey, Mouse->x, Mouse->y))
	    Error(NOMEMORY);
    } else {
	if (!WinScrolled)
	    ScrollerDelayRepeat();
    }
}

