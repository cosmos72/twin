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
#include "alloc.h"
#include "algo.h"
#include "data.h"
#include "methods.h"
#include "scroller.h"

#include "hw_multi.h"
#include "resize.h"
#include "log.h"
#include "util.h"

/* FIXME: Fix screen scrolling during Interactive Drag/Resize */

Tmsgport Scroller_MsgPort;
static void ScrollerH(Tmsgport MsgPort);

/*
 * just dummy msgs that we receive when we are called
 * to start/stop scrolling immediately
 */
Tmsg Do_Scroll, Dont_Scroll;

byte InitScroller(void) {
  if ((Scroller_MsgPort =
           New(msgport)(16, "builtin scroller", (tany)0, 401 MilliSECs, (byte)0, ScrollerH)) &&
      (Do_Scroll = New(msg)(0, 0)) && (Dont_Scroll = New(msg)(0, 0))) {

    return ttrue;
  }
  log(ERROR) << "twin: Scroller: " << Errstr << "\n";
  return tfalse;
}

inline void ScrollerDeactivate(void) {
  Scroller_MsgPort->PauseDuration.Fraction = 333 MilliSECs + 1;
  Scroller_MsgPort->WakeUp = 0;
}

inline void ScrollerAutoRepeat(void) {
  if (Scroller_MsgPort->PauseDuration.Fraction > 333 MilliSECs)
    Scroller_MsgPort->PauseDuration.Fraction = 333 MilliSECs;
  else
    Scroller_MsgPort->PauseDuration.Fraction = 30 MilliSECs;
}

inline void ScrollerDelayRepeat(void) {
  Scroller_MsgPort->PauseDuration.Fraction = 333 MilliSECs + 1;
}

static void ScrollerH(Tmsgport MsgPort) {
  Tmsg msg, saveMsg;
  mouse_state *Mouse;
  uldat Attr, WState;
  dat Limit;
  dat Mouse_delta_x, Mouse_delta_y;
  byte State, FlagDeskScroll, FlagWinScroll, WinScrolled = tfalse;
  Twindow FocusWindow;

  while ((msg = Scroller_MsgPort->FirstMsg)) {
    msg->Remove();
    if (msg == Do_Scroll || msg == Dont_Scroll)
      saveMsg = msg;
    else
      msg->Delete();
  }

  if (saveMsg == Dont_Scroll || !All->MouseDisplay) {
    ScrollerDeactivate();
    return;
  }

  Mouse = &All->MouseDisplay->MouseState;

  FocusWindow = (Twindow)All->FirstScreen->FocusW();
  if (FocusWindow && !IS_WINDOW(FocusWindow))
    FocusWindow = (Twindow)0;

  if (FocusWindow) {
    Attr = FocusWindow->Attr;
    WState = FocusWindow->State;
    FlagWinScroll = (((Attr & WINDOW_X_BAR) && (WState & X_BAR_SELECT)) ||
                     ((Attr & WINDOW_Y_BAR) && (WState & Y_BAR_SELECT))) &&
                    !(WState & TAB_SELECT);
  } else
    FlagWinScroll = tfalse;

  FlagDeskScroll = (All->SetUp->Flags & setup_screen_scroll)
                   /* only a single button must be held */
                   && (Mouse->keys == HOLD_CODE(HOLD_N(Mouse->keys)));

  State = All->State & state_any;

  if (State != state_default && State != state_scroll && State != state_drag &&
      State != state_resize) {

    ScrollerDeactivate();
    return;
  }

  if (State != state_default && FlagWinScroll) {
    if ((WinScrolled = ExecScrollFocusWindow()))
      ScrollerAutoRepeat();
  }

  Mouse_delta_x = -Mouse->delta_x;
  Mouse_delta_y = -Mouse->delta_y;
  FlagDeskScroll &= Mouse_delta_x || Mouse_delta_y;

  if (!FlagDeskScroll) {
    if (!WinScrolled) {
      ScrollerDelayRepeat();
    }
    return;
  }

  if (FlagDeskScroll) {
    Limit = All->SetUp->MaxMouseSnap;
    if (Scroller_MsgPort->PauseDuration.Fraction > 333 MilliSECs) {
      Mouse_delta_x = Sign(Mouse_delta_x);
      Mouse_delta_y = Sign(Mouse_delta_y);
    }

    DragFirstScreen(Mouse_delta_x * Limit, Mouse_delta_y * Limit);

    if (!WinScrolled)
      ScrollerAutoRepeat();

    StdAddMouseEvent(MOVE_MOUSE | Mouse->keys, Mouse->x, Mouse->y);
  } else {
    if (!WinScrolled)
      ScrollerDelayRepeat();
  }
}
