/*
 *  wm.c  --  builtin window manager for twin
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
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
#include "builtin.h"
#include "data.h"
#include "draw.h"
#include "extreg.h"
#include "log.h"
#include "main.h"
#include "methods.h"
#include "obj/id.h" // NOID, Id2Obj()
#include "resize.h"
#include "util.h"
#include "scroller.h"
#include "wm.h"

#include "hw.h"
#include "hw_multi.h"
#include "common.h"

#include <Tw/Twkeys.h> /* for TW_* key defines */
#include <Tutf/Tutf.h> /* for UCS-2 to charset conversions */

#include "rctypes.h"
#include "rcrun.h"

static void DetailCtx(wm_ctx *C);

byte ClickWindowPos = TW_MAXBYTE;

static Tmsgport WM_MsgPort;
static Tmsgport MapQueue;

#define XBarSize (Window->XWidth - (udat)5)
#define YBarSize (Window->YWidth - (udat)4)

static udat TabStart(Twindow Window, sbyte isX) {
  ldat NumLogicMax;
  udat ret;

  if (isX) {
    NumLogicMax = Max2(Window->WLogic, Window->XLogic + Window->XWidth - 2);
    ret = Window->XLogic * (ldat)XBarSize / NumLogicMax;
  } else {
    NumLogicMax = Max2(Window->HLogic, Window->YLogic + (ldat)Window->YWidth - 2);
    ret = Window->YLogic * (ldat)YBarSize / NumLogicMax;
  }
  return ret;
}

static udat TabLen(Twindow Window, sbyte isX) {
  ldat NumLogicMax;
  udat ret;

  if (isX) {
    NumLogicMax = Max2(Window->WLogic, Window->XLogic + Window->XWidth - 2);
    ret = ((Window->XWidth - 2) * (ldat)XBarSize + NumLogicMax - 1) / NumLogicMax;
  } else {
    NumLogicMax = Max2(Window->HLogic, Window->YLogic + Window->YWidth - 2);
    ret = ((Window->YWidth - 2) * (ldat)YBarSize + NumLogicMax - 1) / NumLogicMax;
  }
  return ret ? ret : 1;
}

/* this returns -1 before the tab, 0 on the tab, 1 after */
inline sbyte IsTabPosition(Twindow Window, udat pos, sbyte isX) {
  udat start;
  return pos >= (start = TabStart(Window, isX)) ? pos - start < TabLen(Window, isX) ? 0 : 1 : -1;
}

tpos WMFindBorderWindow(Twindow w, dat u, dat v, byte border, tcell *ptr_cell) {
  const trune *borderRunes;
  trune rune = 0;
  ldat k;
  tcell extra_u;
  tcolor color;
  dat rev_u, rev_v;
  dat XWidth, YWidth;
  tpos found = POS_SIDE_UP;
  sbyte Back_Fwd, i;

  if (!w) {
    return found;
  }
  XWidth = w->XWidth;
  YWidth = w->YWidth;
  rev_u = XWidth - u - 1;
  rev_v = YWidth - v - 1;

  if (u < 0 || v < 0 || rev_u < 0 || rev_v < 0) {
    return POS_ROOT;
  }
  if ((w->Flags & WINDOWFL_BORDERLESS) || (u && v && rev_u && rev_v)) {
    return POS_INSIDE;
  }

  bool flDrag = false, flResize = false, flScroll = false;
  const bool flFocus = (Twidget)w == All->Screens.First->FocusW();

  if (flFocus) {
    switch (All->State & state_any) {
    case state_drag:
      flDrag = true;
      break;
    case state_resize:
      flResize = true;
      break;
    case state_scroll:
      flScroll = true;
      break;
    default:
      break;
    }
  }

  uldat attr = w->Attr;
  byte horiz = extra_u = u ? rev_u ? 1 : 2 : 0;
  byte vert = v ? rev_v ? 1 : 2 : 0;

  const bool canClose = (attr & WINDOW_CLOSE) != 0;
  const bool canResize = (attr & WINDOW_RESIZE) != 0;
  const bool haveBarX = (attr & WINDOW_X_BAR) != 0;
  const bool haveBarY = (attr & WINDOW_Y_BAR) != 0;
  const bool isMenuWin = (w->Flags & WINDOWFL_MENU) != 0;

  if (!(borderRunes = w->BorderPattern[border]) &&
      !(borderRunes = RCFindBorderPattern(w, border))) {
    borderRunes = w->BorderPattern[border] = StdBorder[border];
  }
  const bool flNested = !(w->Parent && IS_SCREEN(w->Parent));

  if (!flNested) {
    switch (vert) {
    case 0:

#define is_u(pos)                                                                                  \
  ((pos) >= 0   ? u == (udat)(pos) || u == (udat)(pos) + (udat)1                                   \
   : (pos) < -1 ? rev_u + (udat)1 == (udat) - (pos) || rev_u + (udat)2 == (udat) - (pos)           \
                : 0)
#define delta_u(pos) ((pos) >= 0 ? u - (udat)(pos) : (udat) - (pos) - rev_u - (udat)1)

      i = BUTTON_MAX;
      if (canClose && All->ButtonVec[0].exists && is_u(All->ButtonVec[0].pos)) {
        i = 0;
      } else if (!isMenuWin) {
        for (i = 1; i < BUTTON_MAX; i++) {
          if (All->ButtonVec[i].exists && is_u(All->ButtonVec[i].pos)) {
            break;
          }
        }
      }
      if (i < BUTTON_MAX) {
        rune = All->ButtonVec[i].shape[extra_u = delta_u(All->ButtonVec[i].pos)];
        found = (tpos)i;
      } else if (w->NameLen) {

        k = 2 * (ldat)u - ((ldat)XWidth - (ldat)w->NameLen - (ldat)3);
        if (k > 0)
          k /= 2;
        if (k > 0 && k <= w->NameLen) {
          rune = Tutf_CP437_to_UTF_32[(byte)w->Name[--k]];
          found = POS_TITLE;
        } else if (k == 0 || k == w->NameLen + 1) {
          rune = ' ';
          found = POS_TITLE;
        } else
          rune = borderRunes[horiz];
      } else
        rune = borderRunes[horiz];
      break;

#undef is_u
#undef delta_u

    case 1:
      if (horiz == 0) {
        rune = borderRunes[(extra_u = vert) * 3];
        found = POS_SIDE_LEFT;
      } else if (horiz == 2) {
        if (haveBarY) {
          if (rev_v < (udat)3) {
            rune = ScrollBarY[extra_u = (udat)3 - rev_v];
            found = rev_v == (udat)2 ? POS_Y_ARROW_BACK : POS_Y_ARROW_FWD;
          } else if (!(Back_Fwd = IsTabPosition(w, v - (udat)1, tfalse))) {
            rune = TabY;
            found = POS_Y_TAB;
          } else {
            rune = ScrollBarY[0];
            found = Back_Fwd > (sbyte)0 ? POS_Y_BAR_FWD : POS_Y_BAR_BACK;
          }
        } else {
          rune = borderRunes[(extra_u = vert) * 3 + 2];
          found = POS_SIDE_RIGHT;
        }
      }
      break;
    case 2:
      found = POS_SIDE_DOWN;

      if (rev_u < (udat)2) {
        if (canResize) {
          rune = GadgetResize[extra_u = (udat)1 - rev_u];
          found = POS_BUTTON_RESIZE;
        } else
          rune = borderRunes[6 + (extra_u = (udat)2 - rev_u)];
      } else if (!haveBarX || !horiz) {
        rune = borderRunes[6 + horiz];
      } else if (rev_u < (udat)4) {
        rune = ScrollBarX[extra_u = (udat)4 - rev_u];
        found = rev_u == (udat)3 ? POS_X_ARROW_BACK : POS_X_ARROW_FWD;
      } else if (!(Back_Fwd = IsTabPosition(w, u - (udat)1, ttrue))) {
        rune = TabX;
        found = POS_X_TAB;
        extra_u = 0;
      } else {
        rune = ScrollBarX[0];
        found = Back_Fwd > (sbyte)0 ? POS_X_BAR_FWD : POS_X_BAR_BACK;
        extra_u = 0;
      }
      break;
    default:
      break;
    }
  } else {
    switch (vert) {
    case 0:
      if (w->NameLen) {
        k = 2 * (ldat)u - ((ldat)XWidth - (ldat)w->NameLen - (ldat)3);
        if (k > 0) {
          k /= 2;
        }
        if (k > 0 && k <= w->NameLen) {
          rune = Tutf_CP437_to_UTF_32[(byte)w->Name[--k]];
          found = POS_TITLE;
        } else if (k == 0 || k == w->NameLen + 1) {
          rune = ' ';
          found = POS_TITLE;
        } else {
          rune = borderRunes[horiz];
        }
      } else {
        rune = borderRunes[horiz];
      }
      break;

    default:
      rune = borderRunes[vert * 3 + horiz];
      found = vert == 1 ? (horiz ? POS_SIDE_RIGHT : POS_SIDE_LEFT) : POS_SIDE_DOWN;
      break;
    }
  }
  if (!ptr_cell)
    return found;

  if (flDrag && found >= POS_TITLE && found <= POS_SIDE_DOWN) {
    if (found == POS_TITLE && w->ColName && k >= 0 && k < w->NameLen) {
      color = w->ColName[k];
    } else {
      color = w->ColGadgets;
    }
  } else
    switch (found) {
    case POS_TITLE:
      if (w->ColName && k >= 0 && k < w->NameLen) {
        color = w->ColName[k];
      } else {
        color = w->ColBorder;
      }
      break;
    case POS_SIDE_LEFT:
    case POS_SIDE_UP:
    case POS_SIDE_RIGHT:
    case POS_SIDE_DOWN:
      color = w->ColBorder;
      break;
    case POS_BUTTON_RESIZE:
      color = w->ColGadgets;
      if (flResize) {
        color = TCOL(TCOLBG(color), TCOLFG(color));
      }
      break;
    case POS_X_ARROW_BACK:
    case POS_X_ARROW_FWD:
    case POS_Y_ARROW_FWD:
    case POS_Y_ARROW_BACK:
      color = w->ColArrows;
      break;
    case POS_X_TAB:
    case POS_Y_TAB:
      color = w->ColTabs;
      break;
    case POS_X_BAR_BACK:
    case POS_X_BAR_FWD:
    case POS_Y_BAR_BACK:
    case POS_Y_BAR_FWD:
      color = w->ColBars;
      break;
    default:
      if (found >= BUTTON_MAX) {
        color = w->ColBorder;
      } else {
        color = w->ColGadgets;
        if (flDrag) {
          color = TCOL(TCOLBG(color), TCOLFG(color));
        }
      }
      break;
    }
  if (flScroll && found >= POS_X_BAR_BACK && found <= POS_Y_ARROW_FWD) {
    color ^= TCOL(thigh, thigh);
  } else if (found < BUTTON_MAX && (w->State & WINDOW_GADGET_PRESSED) &&
             (w->State & (BUTTON_FIRST_SELECT << found))) {

    color = TCOL(TCOLBG(color), TCOLFG(color));
  } else if (found >= POS_TITLE && found <= POS_SIDE_DOWN && !flFocus && !flNested && !isMenuWin) {
    // remove high background color from non-focused window
    color &= ~TCOL(0, thigh);
  }

  *ptr_cell = TCELL(color, rune);

  return found;
}

static void SmartPlace(Twidget w, Tscreen screen);

void Check4Resize(Twindow w) {
  Tmsg msg;
  event_any *Event;
  byte HasBorder;

  if (!w)
    return;

  HasBorder = !(w->Flags & WINDOWFL_BORDERLESS) * 2;

  if (w->Attr & WINDOW_WANT_CHANGES &&
      (!W_USE(w, USECONTENTS) || w->XWidth != w->USE.C.TtyData->SizeX + HasBorder ||
       w->YWidth != w->USE.C.TtyData->SizeY + HasBorder)) {

    if ((msg = Smsg::Create(msg_widget_change, 0))) {
      Event = &msg->Event;
      Event->EventWidget.W = (Twidget)w;
      Event->EventWidget.Code = MSG_WIDGET_RESIZE;
      Event->EventWidget.XWidth = w->XWidth - HasBorder;
      Event->EventWidget.YWidth = w->YWidth - HasBorder;
      SendMsg(w->Owner, msg);
    }
  }
  if (W_USE(w, USECONTENTS))
    CheckResizeWindowContents(w);
}

void AskCloseWidget(Twidget w) {
  Tmsg msg;

  if (w && (!IS_WINDOW(w) || (w->Attr & WINDOW_CLOSE))) {

    if ((msg = Smsg::Create(msg_widget_gadget, 0))) {
      msg->Event.EventGadget.W = w;
      msg->Event.EventGadget.Code = (udat)0; // COD_CLOSE
      SendMsg(w->Owner, msg);
    }
  }
}

void MaximizeWindow(Twindow w, byte full_screen) {
  Tscreen screen;

  if (w && IS_WINDOW(w) && (w->Attr & WINDOW_RESIZE) && (screen = (Tscreen)w->Parent) &&
      IS_SCREEN(screen)) {

    if (full_screen) {
      if (screen->YLogic == TW_MINDAT)
        screen->YLogic++;
      w->Left = screen->XLogic - 1;
      w->Up = screen->YLogic;
      w->XWidth = All->DisplayWidth + 2;
      w->YWidth = All->DisplayHeight + 1 - screen->Up;
    } else {
      if (screen->YLogic == TW_MAXDAT)
        screen->YLogic--;
      w->Left = screen->XLogic;
      w->Up = screen->YLogic + 1;
      w->XWidth = All->DisplayWidth;
      w->YWidth = All->DisplayHeight - 1 - screen->Up;
    }
    QueuedDrawArea2FullScreen = true;
    Check4Resize(w);
  }
}

void ShowWinList(wm_ctx *C) {
  if (!C->Screen)
    C->Screen = All->Screens.First;
  if (WinList->Parent)
    WinList->UnMap();
  if (C->ByMouse) {
    WinList->Left = C->i - 5;
    WinList->Up = C->j - C->Screen->Up;
  } else {
    WinList->Left = 0;
    WinList->Up = TW_MAXDAT;
  }
  WinList->Map((Twidget)C->Screen);
}

static void RecursiveFocusWidget(Twidget w) {
  Twidget P;
  w->SelectW = NULL;
  while ((P = w->Parent)) {
    P->SelectW = w;
    if (IS_SCREEN(P))
      break;
    w = P;
  }
  if (ContainsCursor((Twidget)WindowParent(w)))
    UpdateCursor();
}

static Twidget RecursiveFindFocusWidget(Twidget w) {
  if (w)
    while (w->SelectW)
      w = w->SelectW;
  return w;
}

static void CleanupLastW(Twidget last_w, udat LastKeys, byte LastInside) {
  Tmsg NewMsg;
  event_any *Event;
  udat i;

  if (last_w) {
    if (LastInside) {
      if ((NewMsg = Smsg::Create(msg_widget_mouse, 0))) {
        Event = &NewMsg->Event;
        Event->EventMouse.W = last_w;
        Event->EventMouse.ShiftFlags = (udat)0;
        Event->EventMouse.Code = MOVE_MOUSE | (LastKeys & HOLD_ANY);
        Event->EventMouse.X = TW_MINDAT;
        Event->EventMouse.Y = TW_MINDAT;
        SendMsg(last_w->Owner, NewMsg);
      }
    }
    while (LastKeys & HOLD_ANY) {
      if ((NewMsg = Smsg::Create(msg_widget_mouse, 0))) {
        Event = &NewMsg->Event;
        Event->EventMouse.W = last_w;
        Event->EventMouse.ShiftFlags = (udat)0;

        i = HOLD_N(LastKeys & HOLD_ANY);

        Event->EventMouse.Code = RELEASE_N(i) | (LastKeys &= ~HOLD_CODE(i));
        Event->EventMouse.X = TW_MINDAT;
        Event->EventMouse.Y = TW_MINDAT;
        SendMsg(last_w->Owner, NewMsg);
      } else
        LastKeys = 0;
    }
  } else
    LastKeys = 0;
}

static void HandleHilightAndSelection(Twidget W, udat Code, dat X, dat Y, byte Inside) {
  udat _Code;
  byte IsHoldButtonSelection = (Code & HOLD_ANY) == All->SetUp->ButtonSelection;

  if (isSINGLE_PRESS(Code) && HOLD_CODE(PRESS_N(Code)) == All->SetUp->ButtonSelection)
    StartHilight((Twindow)W, (ldat)X + W->XLogic, (ldat)Y + W->YLogic);
  else if (IsHoldButtonSelection && isMOVE(Code))
    ExtendHilight((Twindow)W, (ldat)X + W->XLogic, (ldat)Y + W->YLogic);
  else if (isSINGLE_RELEASE(Code)) {

    _Code = HOLD_CODE(RELEASE_N(Code));

    if (_Code == All->SetUp->ButtonSelection && IS_WINDOW(W))
      SetSelectionFromWindow((Twindow)W);
    else if (_Code == All->SetUp->ButtonPaste && Inside) {
      // send Selection Paste Tmsg
      Tmsg NewMsg;

      // store selection owner
      SelectionImport();

      if ((NewMsg = Smsg::Create(msg_selection, 0))) {
        event_any *event = &NewMsg->Event;
        event->EventSelection.W = W;
        event->EventSelection.Code = 0;
        event->EventSelection.pad = (udat)0;
        event->EventSelection.X = X;
        event->EventSelection.Y = Y;
        SendMsg(W->Owner, NewMsg);
      }
    }
  }
}

static byte CheckForwardMsg(wm_ctx *C, Tmsg msg, byte WasUsed) {
  static uldat LastWId = NOID;
  static byte LastInside = tfalse;
  static udat LastKeys = 0;
  Twidget last_w, w, P;
  event_any *event;
  udat Code;
  dat X, Y;
  byte Inside, inUse = tfalse;

  if (msg->Type != msg_key && msg->Type != msg_mouse)
    return inUse;

  last_w = (Twidget)Id2Obj(Twidget_class_byte, LastWId);

  w = All->Screens.First->FocusW();

  if ((All->State & state_any) == state_menu) {
    if (!w)
      // the menu is being used, but no menu windows opened yet. continue.
      w = (Twidget)All->Screens.First->MenuWindow;
    else
      // the menu is being used. leave last_w. */
      w = NULL;
  } else {
    if (All->Screens.First->ClickWindow && w != (Twidget)All->Screens.First->ClickWindow) {
      // cannot send messages to focused window while user clicked on another window
      w = NULL;
    }
  }

  if (w)
    w = RecursiveFindFocusWidget(w);
  else
    return inUse;

  event = &msg->Event;
  if (msg->Type == msg_key) {
    if (!WasUsed && All->State == state_default) {

      // back up to first parent that WANT_KEYS or has AUTO_KEYS
      while (w && !(w->Attr & WIDGET_WANT_KEYS ||
                    (IS_WINDOW(w) && ((Twindow)w)->Attr & WINDOW_AUTO_KEYS)))
        w = w->Parent;
      if (w) {
        if (w->Attr & WIDGET_WANT_KEYS) {
          msg->Type = msg_widget_key;
          event->EventKeyboard.W = (Twidget)w;
          SendMsg(w->Owner, msg);
          return ttrue;
        } else if (IS_WINDOW(w) && ((Twindow)w)->Attr & WINDOW_AUTO_KEYS)
          FallBackKeyAction((Twindow)w, &event->EventKeyboard);
      }
    }
    return inUse;
  }

  if (w != last_w && (LastKeys || LastInside)) {
    // try to leave last_w with a clean status
    CleanupLastW(last_w, LastKeys, LastInside);

    last_w = w;
    LastKeys = 0;
    LastInside = tfalse;
  }

  if (!w->Owner || (IS_WINDOW(w) && ((Twindow)w)->Flags & WINDOWFL_MENU))
    return inUse;

  C->Code = Code = event->EventMouse.Code;
  C->i = X = event->EventMouse.X;
  C->j = Y = event->EventMouse.Y;
  Inside = tfalse;

  if (LastKeys) {
    TranslateCoordsWidget(NULL, w, &X, &Y, &Inside);
    if (C->Pos == POS_ROOT || C->Pos == POS_MENU)
      Inside = tfalse;

  } else {

    C->W = NonScreenParent(w);
    C->Screen = (Tscreen)C->W->Parent;

    if (C->Screen && IS_SCREEN(C->Screen)) {

      DetailCtx(C);

      if (w == C->W) {
        X -= C->Left + !(IS_WINDOW(w) && w->Flags & WINDOWFL_BORDERLESS);
        Y -= C->Up + !(IS_WINDOW(w) && w->Flags & WINDOWFL_BORDERLESS);
        Inside = C->Pos == POS_INSIDE;
      } else {
        TranslateCoordsWidget(NULL, w, &X, &Y, &Inside);
        if (C->DW != w || C->Pos == POS_ROOT || C->Pos == POS_MENU)
          /* w may be obscured by something else */
          Inside = tfalse;
        while (Inside == ttrue + ttrue && (P = w->Parent)) {
          /* on the border, must report to the parent */
          w = P;
          X = C->i;
          Y = C->j;
          TranslateCoordsWidget(NULL, w, &X, &Y, &Inside);
        }
      }
    }
    if (Inside == ttrue + ttrue)
      Inside = tfalse;
  }

  /* manage window hilight and Selection */
  if (IS_WINDOW(w) && Inside && Code && !LastKeys && (All->State & state_any) == state_default &&
      !(w->Attr & WIDGET_WANT_MOUSE)) {

    HandleHilightAndSelection(w, Code, X, Y, Inside);

    LastWId = w->Id;

    return inUse;
  }
  /* finished with Selection stuff */

  /* forward the message */
  if (Inside || LastKeys || LastInside) {
    if (Code == MOVE_MOUSE ? (Inside || LastInside) && (w->Attr & WIDGET_WANT_MOUSE_MOTION)
                           : (Inside || LastKeys) && (w->Attr & WIDGET_WANT_MOUSE)) {

      if (Code == MOVE_MOUSE && !Inside)
        X = Y = TW_MINDAT;

      msg->Type = msg_widget_mouse;
      event->EventMouse.W = (Twidget)w;
      event->EventMouse.X = X;
      event->EventMouse.Y = Y;
      SendMsg(w->Owner, msg);

      LastInside = (w->Attr & WIDGET_WANT_MOUSE_MOTION) ? Inside : 0;
      LastKeys = (w->Attr & WIDGET_WANT_MOUSE) ? Code & HOLD_ANY : 0;
      if (isPRESS(Code))
        LastKeys |= HOLD_CODE(PRESS_N(Code));

      inUse = ttrue;
    }
  }

  LastWId = w->Id;

  return inUse;
}

static ldat DragPosition[2];

static void InitCtx(const Tmsg msg, wm_ctx *C) {

  C->Code = msg->Event.EventCommon.Code;
  C->ShiftFlags = msg->Event.EventKeyboard.ShiftFlags; /* ok for mouse too */

  if ((C->ByMouse = (msg->Type == msg_mouse))) {
    C->i = msg->Event.EventMouse.X;
    C->j = msg->Event.EventMouse.Y;

    if ((C->Screen = Sscreen::Find(C->j)) && C->Screen == All->Screens.First &&
        C->Screen->Up < C->j) {
      C->W = C->Screen->FindWidgetAt(C->i, C->j - C->Screen->Up);
    } else
      C->W = NULL;
  } else {
    C->Screen = All->Screens.First;
    C->W = C->Screen->FocusW();
  }
}

static void DetailCtx(wm_ctx *C) {
  if (C->W) {
    /* ensure IS_SCREEN(C->W->Parent) is true. */
    Twidget parent = C->W->Parent;
    if (!parent || !IS_SCREEN(parent)) {
      log(ERROR) << "twin: wm.c: DetailCtx(): internal error: C->W is a subwidget!\n";
      return;
    }
    C->Screen = (Tscreen)parent;
  }
  C->DW = NULL;
  if (C->Screen) {
    C->Menu = C->Screen->FindMenu();
  }
  if (C->ByMouse) {
    C->Pos = POS_ROOT;

    if (C->W) {
      ldat HasBorder = IS_WINDOW(C->W) && !(C->W->Flags & WINDOWFL_BORDERLESS);

      C->Up = (ldat)C->W->Up - C->Screen->YLogic + (ldat)C->Screen->Up;
      C->Left = (ldat)C->W->Left - C->Screen->XLogic;
      C->Rgt = C->Left + (ldat)C->W->XWidth - 1;
      C->Dwn = C->Up + (C->W->Attr & WINDOW_ROLLED_UP ? 0 : (ldat)C->W->YWidth - (ldat)1);

      if (C->i >= C->Left + HasBorder && C->i <= C->Rgt - HasBorder && C->j >= C->Up + HasBorder &&
          C->j <= C->Dwn - HasBorder) {

        C->Pos = POS_INSIDE;
        C->DW = RecursiveFindWidgetAt((Twidget)C->W, C->i - C->Left, C->j - C->Up);
      } else if (HasBorder) {
        /*
         * (i,j) _may_ be outside the window... see ContinueMenu()
         * and CheckForwardMsg()
         */
        if (IS_WINDOW(C->W) &&
            (C->i == C->Left || C->i == C->Rgt || C->j == C->Up || C->j == C->Dwn))
          C->Pos = ((Twindow)C->W)->FindBorder(C->i - C->Left, C->j - C->Up, 0, NULL);
      }
    }
    /*
     * allow code above to calculate C->Left, C->Up
     * (used for Interactive Drag/Resize/Scroll)
     * but return the correct C->Pos :
     */
    if (C->Screen && C->j <= C->Screen->Up) {
      C->Pos = POS_ROOT;
      if (C->j == C->Screen->Up) {
        if (C->i > All->DisplayWidth - (dat)3)
          C->Pos = POS_BUTTON_SCREEN;
        else {
          C->Pos = POS_MENU;
          C->W = C->Screen->FocusW();
          C->Item = C->Menu->FindItem(C->i);
        }
      }
    }
  }
}

inline void Fill4RC_VM(wm_ctx *C, Twidget W, udat Type, byte Pos, udat Code) {
  C->W = W;
  C->Type = Type;
  C->Pos = Pos;
  C->Code = Code;
}

void FocusCtx(wm_ctx *C) {
  if (C->W) {
    C->Screen = ScreenParent(C->W);
  }
  if (C->Screen && C->Screen != All->Screens.First) {
    C->Screen->Focus();
  } else {
    C->Screen = All->Screens.First;
  }
  if (C->W && C->W != C->Screen->FocusW()) {
    C->W->Focus();
  } else {
    C->W = C->Screen->FocusW();
  }
}

static byte ActivateScreen(wm_ctx *C) {
  if (C->Screen && C->Screen != All->Screens.First) {
    C->Screen->Focus();
  }
  C->Screen = All->Screens.First;
  All->State = state_screen | (C->ByMouse ? state_fl_bymouse : 0);
  C->Screen->DrawMenu(0, TW_MAXDAT);
  return ttrue;
}

/* this is mouse-only */
static void ContinueScreen(wm_ctx *C) {
  ResizeFirstScreen(C->j - All->Screens.First->Up);
}

static void ReleaseScreen(wm_ctx *C) {
  All->State = state_default;
  All->Screens.First->DrawMenu(0, TW_MAXDAT);
}

/* this is mouse-only */
static byte ActivateScreenButton(wm_ctx *C) {
  if (C->Screen == All->Screens.First && (All->State & state_any) == state_default) {
    All->State = state_button_screen | state_fl_bymouse;
    C->Screen->Flags |= SCREENFL_BACK_SELECT | SCREENFL_BACK_PRESSED;
    C->Screen->DrawMenu(All->DisplayWidth - (dat)2, All->DisplayWidth - (dat)1);
    return ttrue;
  }
  return tfalse;
}

#if 0
/* this is mouse-only */
static void ContinueScreenButton(wm_ctx *C) {
    udat temp = C->Screen->State;

    DetailCtx(C);

    if (C->Pos == POS_BUTTON_SCREEN) {
        C->Screen->State |= GADGET_PRESSED;
    } else {
        C->Screen->State &= ~GADGET_PRESSED;
    }
    if (temp != C->Screen->State) {
        C->Screen->DrawMenu( All->DisplayWidth-(dat)2, All->DisplayWidth-(dat)1);
    }
}
#endif

/* this is mouse-only */
static void ReleaseScreenButton(wm_ctx *C) {

  All->State = state_default;
  C->Screen->Flags &= ~(SCREENFL_BACK_SELECT | SCREENFL_BACK_PRESSED);

  if (C->Screen != All->Screens.Last && (DetailCtx(C), C->Pos == POS_BUTTON_SCREEN)) {
    MoveLast(Screens, All, C->Screen);
    DrawArea2(NULL, NULL, NULL, 0, Min2(C->Screen->Up, All->Screens.First->Up), TW_MAXDAT,
              TW_MAXDAT, tfalse);
    UpdateCursor();
  } else {
    C->Screen->DrawMenu(All->DisplayWidth - (dat)2, All->DisplayWidth - (dat)1);
  }
}

static byte ActivateMenu(wm_ctx *C) {
  if (C->Screen && C->Screen != All->Screens.First) {
    C->Screen->Focus();
  }
  C->Screen = All->Screens.First;
  C->W = C->Screen->FocusW();
  C->Menu = C->Screen->FindMenu();

  if (C->ByMouse) {
    if (C->j == C->Screen->Up) {
      C->Item = C->Menu->FindItem(C->i);
    } else {
      C->Item = (Tmenuitem)0;
    }
  } else {
    if (!(C->Item = C->Menu->GetSelectedItem()) && !(C->Item = C->Menu->Items.First)) {
      C->Item = All->CommonMenu->Items.First;
    }
  }
  C->Screen->ActivateMenu(C->Item, C->ByMouse);

  return ttrue;
}

/* this is mouse-only */
static void ContinueMenu(wm_ctx *C) {
  Twindow w;
  uldat y;

  DetailCtx(C);

  if (C->Pos == POS_MENU && C->Item && C->Item != C->Menu->GetSelectedItem()) {
    SetMenuState(C->Item, ttrue);
  } else if (C->Pos == POS_INSIDE && (w = (Twindow)C->W) && (w->Flags & WINDOWFL_MENU)) {
    y = (ldat)C->j - C->Up - (ldat)1 + w->YLogic;
    if ((C->Item = (Tmenuitem)w->FindRow(y))) {
      SetMenuState(C->Item, ttrue);
      return;
    }
  } else if ((w = (Twindow)All->Screens.First->FocusW()) && (w->Flags & WINDOWFL_MENU) &&
             (C->Item = (Tmenuitem)w->FindRow(w->CurY)) && IS_MENUITEM(C->Item) &&
             !C->Item->Window) {

    if ((w = (Twindow)C->Item->Parent) && IS_WINDOW(w)) {
      SetMenuState(C->Item = w->MenuItem, ttrue);
    } else {
      SetMenuState(C->Item = C->Menu->GetSelectedItem(), ttrue);
    }
  }
}

static void ReleaseMenu(wm_ctx *C) {
  Twindow MW = All->Screens.First->MenuWindow;
  Twindow FW = (Twindow)All->Screens.First->FocusW();
  Tmenu menu;
  Tmenuitem item;
  Trow Row;
  Tmsg msg;
  event_menu *event;
  udat Code;

  if (FW && IS_WINDOW(FW) && FW->CurY < TW_MAXLDAT && (menu = FW->Menu) &&
      (item = menu->GetSelectedItem()) && (item->Flags & ROW_ACTIVE) &&
      (Row = FW->FindRow(FW->CurY)) && (Row->Flags & ROW_ACTIVE) && Row->Code)

    Code = Row->Code;
  else
    Code = (udat)0;

  /* disable the menu BEFORE processing the code! */
  CloseMenu();

  if (Code >= COD_RESERVED) {
    /* handle COD_RESERVED codes internally */
    Fill4RC_VM(C, (Twidget)MW, msg_menu_row, POS_MENU, Row->Code);
    (void)RC_VMQueue(C);
  } else if (Code) {
    if ((msg = Smsg::Create(msg_menu_row, 0))) {
      event = &msg->Event.EventMenu;
      event->W = MW;
      event->Code = Code;
      event->Menu = menu;
      event->Row = Row;
      if (MW)
        SendMsg(MW->Owner, msg);
      else
        SendMsg(menu->MsgPort, msg);
    }
  }
}

static void ShowResize(Twindow w) {
  static char buf[40];
  dat x = w->XWidth;
  dat y = w->YWidth;

  if (!(w->Flags & WINDOWFL_BORDERLESS)) {
    x -= 2, y -= 2;
  }
  snprintf(buf, sizeof(buf), "%hdx%hd", x, y);
  All->BuiltinRow->SetText(strlen(buf), buf, 0);
  All->Screens.First->DrawMenu(All->DisplayWidth - 20, All->DisplayWidth - 10);
}

static void HideResize(void) {
  All->BuiltinRow->SetText(0, NULL, 0);
  All->Screens.First->DrawMenu(All->DisplayWidth - 20, All->DisplayWidth - 10);
}

static byte ActivateDrag(wm_ctx *C) {
  if (C->Screen == All->Screens.First && C->W && IS_WINDOW(C->W) && C->W->Attr & WINDOW_DRAG) {

    All->Screens.First->ClickWindow = (Twindow)C->W;
    All->State = state_drag;
    if (C->ByMouse) {
      All->State |= state_fl_bymouse;

      DetailCtx(C);
      DragPosition[0] = (ldat)C->i - C->Left;
      DragPosition[1] = (ldat)C->j - C->Up;
    }
    DrawBorderWindow((Twindow)C->W, BORDER_ANY);
    return ttrue;
  }
  return tfalse;
}

static byte ActivateResize(wm_ctx *C) {
  if (C->Screen == All->Screens.First && C->W && IS_WINDOW(C->W) && C->W->Attr & WINDOW_RESIZE) {

    All->Screens.First->ClickWindow = (Twindow)C->W;
    All->State = state_resize;
    if (C->ByMouse) {
      All->State |= state_fl_bymouse;

      DetailCtx(C);
      DragPosition[0] = (ldat)C->i - C->Rgt;
      DragPosition[1] = (ldat)C->j - C->Dwn;
    }
    DrawBorderWindow((Twindow)C->W, BORDER_ANY);
    ShowResize((Twindow)C->W);
    return ttrue;
  }
  return tfalse;
}

static byte ActivateScroll(wm_ctx *C) {
  Twindow w = (Twindow)C->W;

  if (C->Screen == All->Screens.First && w && IS_WINDOW(w) &&
      C->W->Attr & (WINDOW_X_BAR | WINDOW_Y_BAR)) {

    /*
     * paranoia: SneakSetupMouse() mail call us even for a mouse event
     * OUTSIDE or INSIDE C->W
     */
    if (C->ByMouse) {
      DetailCtx(C);
      if ((ldat)C->j == C->Dwn) {
        if (C->Pos == POS_X_ARROW_BACK)
          w->State |= X_BAR_SELECT | ARROW_BACK_SELECT;
        else if (C->Pos == POS_X_ARROW_FWD)
          w->State |= X_BAR_SELECT | ARROW_FWD_SELECT;
        else if (C->Pos == POS_X_BAR_BACK)
          w->State |= X_BAR_SELECT | PAGE_BACK_SELECT;
        else if (C->Pos == POS_X_BAR_FWD)
          w->State |= X_BAR_SELECT | PAGE_FWD_SELECT;
        else {
          w->State |= X_BAR_SELECT | TAB_SELECT;
          DragPosition[0] = (ldat)C->i - C->Left - 1 - TabStart(w, (sbyte)1);
          DragPosition[1] = 0;
        }
      } else if ((ldat)C->i == C->Rgt) {
        if (C->Pos == POS_Y_ARROW_BACK)
          w->State |= Y_BAR_SELECT | ARROW_BACK_SELECT;
        else if (C->Pos == POS_Y_ARROW_FWD)
          w->State |= Y_BAR_SELECT | ARROW_FWD_SELECT;
        else if (C->Pos == POS_Y_BAR_BACK)
          w->State |= Y_BAR_SELECT | PAGE_BACK_SELECT;
        else if (C->Pos == POS_Y_BAR_FWD)
          w->State |= Y_BAR_SELECT | PAGE_FWD_SELECT;
        else {
          w->State |= Y_BAR_SELECT | TAB_SELECT;
          DragPosition[0] = 0;
          DragPosition[1] = (ldat)C->j - C->Up - 1 - TabStart(w, (sbyte)0);
        }
      }
      if (w->State & SCROLL_ANY_SELECT)
        All->State = state_scroll | state_fl_bymouse;
    } else
      All->State = state_scroll;

    if ((All->State & state_any) == state_scroll && IS_WINDOW(w)) {
      All->Screens.First->ClickWindow = w;
      DrawBorderWindow(w, BORDER_ANY);
      return ttrue;
    }
  }
  return tfalse;
}

/* this is mouse only */
static void ContinueDrag(wm_ctx *C) {
  if ((C->W = (Twidget)All->Screens.First->ClickWindow)) {
    DetailCtx(C);
    if (C->W == All->Screens.First->Widgets.First)
      DragFirstWindow(C->i - C->Left - DragPosition[0],
                      Max2(C->j, All->Screens.First->Up + 1) - C->Up - DragPosition[1]);
    else
      DragWindow((Twindow)C->W, C->i - C->Left - DragPosition[0],
                 Max2(C->j, All->Screens.First->Up + 1) - C->Up - DragPosition[1]);
  }
}

/* this is mouse only */
static void ContinueResize(wm_ctx *C) {

  if ((C->W = (Twidget)All->Screens.First->ClickWindow)) {
    DetailCtx(C);
    if (C->W == All->Screens.First->Widgets.First)
      ResizeRelFirstWindow(C->i - C->Rgt - DragPosition[0],
                           Max2(C->j, All->Screens.First->Up + 1) - C->Dwn - DragPosition[1]);
    else
      ResizeRelWindow((Twindow)C->W, C->i - C->Rgt - DragPosition[0],
                      Max2(C->j, All->Screens.First->Up + 1) - C->Dwn - DragPosition[1]);
    ShowResize((Twindow)C->W);
  }
}

/* this is mouse only */
static void ContinueScroll(wm_ctx *C) {
  Twindow w;
  uldat NumLogicMax;
  ldat i;

  if ((C->W = (Twidget)(w = All->Screens.First->ClickWindow))) {
    DetailCtx(C);

    if (w->State & X_BAR_SELECT) {
      NumLogicMax = Max2(w->WLogic, w->XLogic + (ldat)w->XWidth - 2);
      i = C->i;
      if (i + 4 > C->Rgt + DragPosition[0])
        i = C->Rgt + DragPosition[0] - 4;
      ScrollWindow(w,
                   (i - C->Left - 1 - DragPosition[0] - TabStart(w, (sbyte)1)) *
                       (NumLogicMax / (w->XWidth - 5)),
                   0);

    } else if (w->State & Y_BAR_SELECT) {
      NumLogicMax = Max2(w->HLogic, w->YLogic + (ldat)w->YWidth - 2);
      i = Max2(C->j, All->Screens.First->Up + 1);
      if (i + 3 > C->Dwn + DragPosition[1])
        i = C->Dwn + DragPosition[1] - 3;
      ScrollWindow(w, 0,
                   (i - C->Up - 1 - DragPosition[1] - TabStart(w, (sbyte)0)) *
                       (NumLogicMax / (w->YWidth - (udat)4)));
    }
  }
}

static void ReleaseDragResizeScroll(const wm_ctx *C) {
  Twindow FW = All->Screens.First->ClickWindow;
  udat wasResize;

  wasResize = (All->State & state_any) == state_resize;
  All->State = state_default;

  if (FW) {
    FW->State &= ~(BUTTON_ANY_SELECT | SCROLL_ANY_SELECT | XY_BAR_SELECT);
    DrawBorderWindow(FW, BORDER_ANY);

    if (wasResize) {
      Check4Resize(FW);
      HideResize();
    }
  }
}

/* this is mouse only */
static byte ActivateButton(wm_ctx *C) {
  if (C->W && IS_WINDOW(C->W)) {
    All->State = C->Pos | state_fl_bymouse;
    ((Twindow)C->W)->State |= (BUTTON_FIRST_SELECT << C->Pos) | WINDOW_GADGET_PRESSED;

    C->Type = msg_mouse;
    (void)RC_VMQueue(C);

    DrawBorderWindow((Twindow)C->W, BORDER_UP);
    return ttrue;
  }
  return tfalse;
}

#if 0
/* this is mouse only */
/*
 * this would release window border buttons when moving away from them...
 * I prefer them to stay pressed, so this is disabled
 */
static void ContinueButton(wm_ctx *C) {
    Twindow FW = All->Screens.First->ClickWindow;
    uldat ltemp;
    byte found = tfalse;

    if (!FW)
        return;

    if (FW == C->W && (ltemp = FW->State) & BUTTON_ANY_SELECT) {
        DetailCtx(C);
        if (C->Pos < BUTTON_MAX &&
            (ltemp & BUTTON_ANY_SELECT) == (BUTTON_FIRST_SELECT << C->Pos))

            found = ttrue;
    }

    if (found)
        FW->State |= GADGET_PRESSED;
    else
        FW->State &= ~GADGET_PRESSED;
    if (ltemp != FW->State)
        DrawBorderWindow(FW, BORDER_UP);
}
#endif

/* this is mouse only */
static void ReleaseButton(wm_ctx *C) {
  Twindow FW = All->Screens.First->ClickWindow;

  All->State = state_default;
  if (FW) {
    if (FW == (Twindow)C->W && FW->State & BUTTON_ANY_SELECT) {
      DetailCtx(C);

      if (C->Pos < BUTTON_MAX &&
          (FW->State & BUTTON_ANY_SELECT) == (BUTTON_FIRST_SELECT << C->Pos)) {

        C->W = (Twidget)FW;
        C->Type = msg_mouse;
        (void)RC_VMQueue(C);
      }
    }
    FW->State &= ~(BUTTON_ANY_SELECT | WINDOW_GADGET_PRESSED);
    DrawBorderWindow(FW, BORDER_UP);
  }
}

/* this is mouse only */
static byte ActivateGadget(wm_ctx *C) {
  Tgadget G = (Tgadget)C->DW;
  All->State = state_gadget | state_fl_bymouse;
  if (!G->Group &&
      (G->Flags & (GADGETFL_TOGGLE | GADGETFL_PRESSED)) == (GADGETFL_TOGGLE | GADGETFL_PRESSED))
    UnPressGadget(G, ttrue);
  else
    PressGadget(G);
  if (!(G->Flags & GADGETFL_TOGGLE))
    RecursiveFocusWidget((Twidget)G);
  return ttrue;
}

/* this is mouse only */
static void ContinueGadget(wm_ctx *C) {
  Twindow FW = All->Screens.First->ClickWindow;
  Tgadget FG;
  udat temp;

  if (FW && (FG = (Tgadget)RecursiveFindFocusWidget((Twidget)FW))) {
    temp = FG->Flags;

    if (!(temp & GADGETFL_TOGGLE)) {
      if (FW == (Twindow)C->W && FG && (DetailCtx(C), (Twidget)FG == C->DW))
        FG->Flags |= GADGETFL_PRESSED;
      else
        FG->Flags &= ~GADGETFL_PRESSED;

      if (temp != FG->Flags) {
        if ((Twidget)FW == All->Screens.First->Widgets.First)
          DrawUnobscuredWidget((Twidget)FG, 0, 0, TW_MAXDAT, TW_MAXDAT, tfalse);
        else
          DrawAreaWidget((Twidget)FG);
      }
    }
  }
}

/* this is mouse only */
static void ReleaseGadget(wm_ctx *C) {
  Twindow FW = All->Screens.First->ClickWindow;
  Tgadget FG;

  All->State = state_default;
  if (!FW)
    return;

  DetailCtx(C);

  FG = (Tgadget)RecursiveFindFocusWidget((Twidget)FW);

  if (!FG || !IS_GADGET(FG) || FG->Flags & GADGETFL_TOGGLE)
    return;

  UnPressGadget(FG, (Twidget)FW == C->W && FG && (Twidget)FG == C->DW);
  /* FW->SelectW=NULL; */
}

/* the only Activate*() that make sense from within RC_VM() */
byte ActivateCtx(wm_ctx *C, byte State) {
  switch (State) {
  case state_resize:
    return ActivateResize(C);
  case state_drag:
    return ActivateDrag(C);
  case state_scroll:
    return ActivateScroll(C);
  case state_menu:
    return ActivateMenu(C);
  case state_screen:
    return ActivateScreen(C);
  default:
    break;
  }
  return tfalse;
}

/* force returning to state_default. used before RCReload() */
void ForceRelease(const wm_ctx *C) {
  switch (All->State & state_any) {
  case state_resize:
  case state_drag:
  case state_scroll:
    ReleaseDragResizeScroll(C);
    break;
  case state_gadget: {
    Twindow FW;
    Tgadget FG;

    if ((FW = All->Screens.First->ClickWindow) &&
        (FG = (Tgadget)RecursiveFindFocusWidget((Twidget)FW)) && IS_GADGET(FG) &&
        !(FG->Flags & GADGETFL_TOGGLE))

      UnPressGadget(FG, tfalse);
  } break;
  case state_menu:
    CloseMenu();
    break;
  case state_screen:
    break;
  case state_button_screen:
    All->Screens.First->Flags &= ~(SCREENFL_BACK_SELECT | SCREENFL_BACK_PRESSED);
    break;
  default:
    if ((All->State & state_any) < BUTTON_MAX) {
      Twindow FW;
      if ((FW = All->Screens.First->ClickWindow))
        FW->State &= ~(BUTTON_ANY_SELECT | WINDOW_GADGET_PRESSED);
    }
    break;
  }
  All->State = state_default;
}

/*
 * these must be handled manually as we want the gadgets/buttons
 * to immediately change color when selected
 *
 * this is mouse only
 */
static byte ActivateMouseState(wm_ctx *C) {
  byte used = tfalse;
  byte isPressButtonSelection;

  if (isSINGLE_PRESS(C->Code)) {
    isPressButtonSelection = HOLD_CODE(PRESS_N(C->Code)) == All->SetUp->ButtonSelection;

    switch (C->Pos) {
    case POS_BUTTON_SCREEN:
      if (isPressButtonSelection)
        used = ttrue, ActivateScreenButton(C);
      break;
    case POS_INSIDE:
      if (isPressButtonSelection && C->DW && IS_GADGET(C->DW) &&
          !(((Tgadget)C->DW)->Flags & GADGETFL_DISABLED))
        used = ttrue, ActivateGadget(C);
      break;
    default:
      if (C->Pos < BUTTON_MAX)
        used = ttrue, ActivateButton(C);
      break;
    }
  }
  return used;
}

/*
 * check if the mouse (C->i, C->j) is in a position suitable for the State.
 * setup DragPosition[] as if the current State was initiated with the mouse,
 * or return tfalse if (C->i, C->j) is in a non-appropriate position.
 */
static byte SneakSetupMouse(wm_ctx *C) {
  /* State was set with keyboard */
  Twindow w = All->Screens.First->ClickWindow;
  byte ok = ttrue;

  switch (All->State & state_any) {
  case state_resize:
    DetailCtx(C);
    if (IS_WINDOW(w) && w == (Twindow)C->W) {
      DragPosition[0] = (ldat)C->i - C->Rgt;
      DragPosition[1] = (ldat)C->j - C->Dwn;
    } else
      ok = tfalse;
    break;
  case state_drag:
    DetailCtx(C);
    if (IS_WINDOW(w) && w == (Twindow)C->W) {
      DragPosition[0] = (ldat)C->i - C->Left;
      DragPosition[1] = (ldat)C->j - C->Up;
    } else
      ok = tfalse;
    break;
  case state_scroll:
    /*
     * this is complex... we must setup w->State with what you clicked upon.
     * do the dirty way and reuse the function ActivateScroll().
     * since it returns (void), check for (w->State & SCROLL_ANY_SELECT).
     */
    if (IS_WINDOW(w) && w == (Twindow)C->W) {
      ActivateScroll(C);
      ok = !!(w->State & SCROLL_ANY_SELECT);
    } else
      ok = tfalse;
    break;
  case state_gadget:
  case state_menu:
  case state_screen:
  case state_button_screen:
  case state_root:
  case state_default:
  default:
    break;
  }
  return ok;
}

/* handle mouse during various STATE_* */
/* this is mouse only */
static void ContinueReleaseMouseState(wm_ctx *C, byte State) {
  if (isSINGLE_RELEASE(C->Code)) {
    switch (State) {
    case state_resize:
    case state_drag:
    case state_scroll:
      ReleaseDragResizeScroll(C);
      break;
    case state_gadget:
      ReleaseGadget(C);
      break;
    case state_menu:
      ReleaseMenu(C);
      break;
    case state_screen:
      ReleaseScreen(C);
      break;
    case state_button_screen:
      ReleaseScreenButton(C);
      break;
    case state_root:
    case state_default:
    default:
      if (State < BUTTON_MAX)
        ReleaseButton(C);
      else
        /* paranoid... */
        All->State = state_default;
      break;
    }
  } else if (isSINGLE_PRESS(C->Code) || isSINGLE_MOVE(C->Code) || isRELEASE(C->Code)) {
    switch (State) {
    case state_resize:
      ContinueResize(C);
      break;
    case state_drag:
      ContinueDrag(C);
      break;
    case state_scroll:
      ContinueScroll(C);
      break;
    case state_gadget:
      ContinueGadget(C);
      break;
    case state_menu:
      ContinueMenu(C);
      break;
    case state_screen:
      ContinueScreen(C);
      break;
    /*case state_button_screen: ContinueScreenButton(C); break; */
    case state_root:
    case state_default:
      break;
    default:
      /* if (State < BUTTON_MAX)
       *  ContinueButton(C); */
      break;
    }
  }
}

static Tmenuitem PrevItem(Tmenuitem item, Tmenu menu) {
  Tmenuitem prev;

  if (!(prev = item->PrevItem())) {
    if (item->Parent == menu) {
      if (menu->CommonItems && All->CommonMenu) {
        prev = All->CommonMenu->Items.Last;
      }
    } else {
      prev = menu->Items.Last;
    }
  }
  return prev ? prev : item;
}

static Tmenuitem NextItem(Tmenuitem item, Tmenu menu) {
  Tmenuitem next;

  if (!(next = item->NextItem())) {
    if (item->Parent == (Tobj)menu) {
      if (menu->CommonItems && All->CommonMenu) {
        next = All->CommonMenu->Items.First;
      }
    } else {
      next = menu->Items.First;
    }
  }
  return next ? next : item;
}

static void EnterItem(Tmenuitem item) {
  Twindow w = item->Window;
  if (!(item = (Tmenuitem)w->FindRow(w->CurY))) {
    item = (Tmenuitem)w->FindRow(0);
  }
  if (item && IS_MENUITEM(item)) {
    SetMenuState(item, tfalse);
  }
}

/* handle keyboard during various STATE_* */
/* this is keyboard only */
static byte ActivateKeyState(wm_ctx *C, byte State) {
  Twindow w = (Twindow)All->Screens.First->FocusW();
  ldat numrow;
  dat XDelta = 0, YDelta = 0, depth;
  udat Key = C->Code;
  byte used = tfalse;
  Tmenuitem M;

  if (!w || !IS_WINDOW(w))
    return used;

  switch (Key) {
  case TW_Right:
    XDelta = 1;
    break;
  case TW_Left:
    XDelta = -1;
    break;
  case TW_Down:
    YDelta = 1;
    break;
  case TW_Up:
    YDelta = -1;
    break;
  default:
    break;
  }

  switch (State & state_any) {
  case state_drag:
  case state_resize:
    if (Key == TW_Escape || Key == TW_Return) {
      used = ttrue, ReleaseDragResizeScroll(C);
    } else if (State == state_resize && (w->Attr & WINDOW_RESIZE)) {
      used = ttrue;
      ResizeRelWindow(w, XDelta, YDelta);
      ShowResize(w);
    } else if (State == state_drag && (w->Attr & WINDOW_DRAG)) {
      used = ttrue, DragWindow(w, XDelta, YDelta);
    }
    break;
  case state_scroll:
    if (Key == TW_Escape || Key == TW_Return) {
      ReleaseDragResizeScroll(C);
      used = ttrue;
      break;
    }
    if (w->Attr & (WINDOW_X_BAR | WINDOW_Y_BAR)) {
      switch (Key) {
      case TW_Insert:
        XDelta = (w->XWidth - 3);
        break;
      case TW_Delete:
        XDelta = -(w->XWidth - 3);
        break;
      case TW_Next:
        YDelta = (w->YWidth - 3);
        break;
      case TW_Prior:
        YDelta = -(w->YWidth - 3);
        break;
      default:
        break;
      }
      if (!(w->Attr & WINDOW_X_BAR)) {
        XDelta = 0;
      }
      if (!(w->Attr & WINDOW_Y_BAR)) {
        YDelta = 0;
      }
      if (XDelta || YDelta) {
        used = ttrue, ScrollWindow(w, XDelta, YDelta);
      }
    }
    break;
  case state_menu:
    C->Menu = C->Screen->FindMenu();
    C->Item = C->Menu->GetSelectedItem();
    M = C->Menu->RecursiveGetSelectedItem(&depth);
    switch (Key) {
    case TW_Escape:
      if (depth <= 1) {
        CloseMenu();
      } else {
        SetMenuState(w->MenuItem, tfalse);
      }
      used = ttrue;
      break;
    case TW_Return:
      if (M && IS_MENUITEM(M) && M->Window) {
        EnterItem(M);
      } else {
        ReleaseMenu(C);
      }
      used = ttrue;
      break;
    case TW_Left:
      if (depth > 0 && (All->SetUp->Flags & setup_menu_relax) && (w = (Twindow)M->Parent) &&
          IS_WINDOW(w) && (M = w->MenuItem) && (w = (Twindow)M->Parent) && IS_WINDOW(w)) {
        SetMenuState(M, tfalse);
      } else if (depth <= 1) {
        SetMenuState(C->Item = PrevItem(C->Item, C->Menu), tfalse);
      }
      used = ttrue;
      break;
    case TW_Right:
      if (depth > 0 && (All->SetUp->Flags & setup_menu_relax) && M->Window)
        EnterItem(M);
      else if (depth <= 1)
        SetMenuState(C->Item = NextItem(C->Item, C->Menu), tfalse);
      used = ttrue;
      break;
    case TW_Up:
      if (!w->HLogic || (All->State & state_fl_bymouse)) {
        break;
      }
      numrow = w->CurY;
      if (numrow && numrow < w->HLogic) {
        numrow--;
      } else {
        numrow = w->HLogic - 1;
      }
      M = (Tmenuitem)w->FindRow(numrow);
      if (M && IS_MENUITEM(M)) {
        SetMenuState(M, tfalse);
      }
      used = ttrue;
      break;
    case TW_Down:
      if (!w->HLogic || (All->State & state_fl_bymouse)) {
        break;
      }
      numrow = w->CurY;
      if (numrow < w->HLogic - 1) {
        numrow++;
      } else {
        numrow = 0;
      }
      M = (Tmenuitem)w->FindRow(numrow);
      if (M && IS_MENUITEM(M)) {
        SetMenuState(M, tfalse);
      }
      used = ttrue;
      break;
    default:
      break;
    }
    break;
  case state_screen:
    if (Key == TW_Escape || Key == TW_Return)
      used = ttrue, ReleaseScreen(C);
    else if (YDelta)
      used = ttrue, ResizeFirstScreen(-YDelta);
    break;
  case state_default:
    /* ? is this needed ? */
    DragFirstScreen(XDelta, YDelta);
    break;
  default:
    break;
  }
  return used;
}

/*
 * automatically recursively focus a subwidget on mouse motion
 * if it and all its non-Tscreen parents have WIDGET_AUTO_FOCUS flag set
 */
static void TryAutoFocus(wm_ctx *C) {
  Twidget w, deepW, oldW, focusW = All->Screens.First->FocusW();

  if (!focusW)
    focusW = (Twidget)All->Screens.First->MenuWindow;

  if (!focusW)
    return;

  oldW = RecursiveFindFocusWidget((Twidget)All->Screens.First);

  if ((w = C->W) && w == focusW && (deepW = C->DW) && deepW != oldW) {

    /*
     * must have AUTO_FOCUS to be autofocused...
     * and focusing top-level widgets is handled elsewhere
     */
    if (!(deepW->Attr & WIDGET_AUTO_FOCUS) || (w == deepW && w->Parent->SelectW != w))
      return;

    focusW = deepW;

    /* climb through all AUTO_FOCUS widgets */
    while (deepW != w) {
      if (!(deepW = deepW->Parent))
        return;
      if (!(deepW->Attr & WIDGET_AUTO_FOCUS))
        break;
    }
    /* climb through all already focused widgets */
    while (deepW != w && deepW->Parent && deepW->Parent->SelectW == deepW) {
      deepW = deepW->Parent;
    }
    if (deepW == w) {
      RecursiveFocusWidget(focusW);
#ifdef DEBUG_WM
      log(INFO) << "autofocus: 0x" << hex(oldW ? oldW->Id : NOID) << " -> 0x" << hex(focusW->Id)
                << "\n";
#endif
    }
  }
}

/* the Window Manager built into Twin */
static void WManagerH(Tmsgport MsgPort) {
  static wm_ctx _C;
  wm_ctx *C = &_C;
  Tmsg msg;
  Twidget w;
  byte used = tfalse;

  while ((msg = WM_MsgPort->Msgs.First)) {

    msg->Remove();

    if (msg->Type == msg_map) {
      SendMsg(MapQueue, msg);
      continue;
    } else if (msg->Type != msg_mouse && msg->Type != msg_key) {
      if (msg->Type == msg_control) {
        /*
         * for now, don't allow starting a different WM (MSG_CONTROL_RESTART)
         * but just restart this one (MSG_CONTROL_OPEN)
         */
        if ((msg->Event.EventControl.Code == MSG_CONTROL_RESTART ||
             msg->Event.EventControl.Code == MSG_CONTROL_OPEN)) {

          Fill4RC_VM(C, NULL, msg_control, POS_ROOT, MSG_CONTROL_OPEN);
          (void)RC_VMQueue(C);
        }
      }
      msg->Delete();
      continue;
    }

    InitCtx(msg, C);

    if (All->State == state_default) {
      if (C->ByMouse && isSINGLE_PRESS(C->Code)) {
        if (C->Screen && C->Screen != All->Screens.First) {
          C->Screen->Focus();
          InitCtx(msg, C);
        }
        if (HOLD_CODE(PRESS_N(C->Code)) == All->SetUp->ButtonSelection) {
          if (C->W && C->W != C->Screen->FocusW()) {
            C->W->Focus();
          }
          DetailCtx(C);
          w = RecursiveFindFocusWidget((Twidget)All->Screens.First);
          if (C->DW && C->DW != w) {
            RecursiveFocusWidget(C->DW);
          }
          All->State |= state_fl_bymouse;
        } else {
          DetailCtx(C);
        }
        /* mouse action, setup ClickWindow */
        ClickWindowPos = C->Pos;
        All->Screens.First->ClickWindow = (Twindow)C->W;

        used = ActivateMouseState(C);
      } else {
        used = tfalse, DetailCtx(C);

        if (C->ByMouse) {
          if (!isPRESS(C->Code) && !(C->Code & HOLD_ANY)) {
            TryAutoFocus(C);
          }
        } else {
          /* for keyboard actions, ClickWindow == FocusW */
          All->Screens.First->ClickWindow = (Twindow)C->W;
        }
      }

      if (!used) {
        C->Type = C->ByMouse ? msg_mouse : msg_key;
        used = RC_VMQueue(C);
      }

    } else if (C->ByMouse) {

      /*
       * if you use the mouse during a keyboard-activated STATE_xxx,
       * you will activate the state_fl_bymouse flag.
       * This means that the mouse "steals" to the keyboard
       * the action you were doing (resize, scroll, ...)
       * and the keyboard becomes non-functional until
       * you return to state_default.
       *
       * Since the WM does not know which mouse button is used to
       * activate the various STATE_xxx (it's specified in the RC Virtual Machine)
       * this also means that you can Drag,Resize,Scroll,Activate Menu or screen
       * with the "wrong" mouse button with this trick:
       * first you enter the State STATE_xxx using the Menu, then you
       * click in an appropriate place with whatever mouse button you like.
       *
       * clicking in a non-appropriate place (e.g. Menu during a Window Scroll)
       * just forces a return to state_default.
       */
      if (!(All->State & state_fl_bymouse)) {
        if (SneakSetupMouse(C)) {
          /* ok, mouse is in a meaningful position */
          All->State |= state_fl_bymouse;
        } else {
          /* clicked in a strange place? return to state_default */
          ForceRelease(C);
        }
      }
      if (All->State & state_fl_bymouse) {
        ContinueReleaseMouseState(C, All->State & state_any);
      }
    } else /* (!C->ByMouse) */ {
      used = ActivateKeyState(C, All->State);
    }

    /* cleanup ClickWindow if not needed anymore */
    if ((All->State & state_any) == state_default &&
        (!C->ByMouse || !isPRESS(C->Code) || !(C->Code & HOLD_ANY))) {

      ClickWindowPos = TW_MAXBYTE;
      All->Screens.First->ClickWindow = NULL;
    }

    /* must we send the event to the focused window too ? */
    if (C->ByMouse || (All->State == state_default && !used)) {

      if (CheckForwardMsg(C, msg, used)) {
        /* don't msg->Delete() ! */
        continue;
      }
    }

    msg->Delete();
  }

  if (All->State == state_default) {
    for (used = 30, msg = MapQueue->Msgs.First; msg && used; msg = msg->Next, used--)
      ;
    if (!used)
      QueuedDrawArea2FullScreen = true;
    while ((msg = MapQueue->Msgs.First)) {
      C->W = msg->Event.EventMap.W;
      SmartPlace((Twidget)C->W, msg->Event.EventMap.Screen);

      Fill4RC_VM(C, C->W, msg_map, POS_ROOT, 0);
      (void)RC_VMQueue(C);

      msg->Delete();
    }
  }

  if (All->MouseDisplay && All->MouseDisplay->MouseState.keys &&
      Scroller_MsgPort->WakeUp != TIMER_ALWAYS) {
    extern Tmsg Do_Scroll;
    Scroller_MsgPort->WakeUp = TIMER_ALWAYS;
    SendMsg(Scroller_MsgPort, Do_Scroll);
  } else if ((!All->MouseDisplay || !All->MouseDisplay->MouseState.keys) &&
             Scroller_MsgPort->WakeUp == TIMER_ALWAYS) {
    extern Tmsg Dont_Scroll;
    SendMsg(Scroller_MsgPort, Dont_Scroll);
  }

  if (RC_VM(&WM_MsgPort->PauseDuration)) {
    /* sleep specified amount of time */
    WM_MsgPort->WakeUp |= TIMER_ONCE;
  }
}

static dat XWidth, YWidth;

static byte doSmartPlace(Twidget w, dat x[2], dat y[2]) {
  dat wleft, wright, tryx[2];
  dat wup, wdown, tryy[2];
  byte ok = tfalse;

  if (XWidth > x[1] - x[0] + 1 || YWidth > y[1] - y[0] + 1)
    return tfalse;

  if (!w)
    return ttrue;

  wright = (wleft = w->Left) + w->XWidth;
  wdown = (wup = w->Up) + (IS_WINDOW(w) && (((Twindow)w)->Attr & WINDOW_ROLLED_UP) ? 1 : w->YWidth);
  w = w->Next;

  if (x[0] >= wright || x[1] < wleft || y[0] >= wdown || y[1] < wup)
    return w ? doSmartPlace(w, x, y) : ttrue;

  if (y[0] < wup) {
    tryx[0] = x[0];
    tryx[1] = x[1];
    tryy[0] = y[0];
    tryy[1] = wup - 1;
    ok = doSmartPlace(w, tryx, tryy);
  }
  if (!ok && x[0] < wleft) {
    tryx[0] = x[0];
    tryx[1] = wleft - 1;
    tryy[0] = y[0];
    tryy[1] = y[1];
    ok = doSmartPlace(w, tryx, tryy);
  }
  if (!ok && x[1] >= wright) {
    tryx[0] = wright;
    tryx[1] = x[1];
    tryy[0] = y[0];
    tryy[1] = y[1];
    ok = doSmartPlace(w, tryx, tryy);
  }
  if (!ok && y[1] >= wdown) {
    tryx[0] = x[0];
    tryx[1] = x[1];
    tryy[0] = wdown;
    tryy[1] = y[1];
    ok = doSmartPlace(w, tryx, tryy);
  }
  if (ok) {
    x[0] = tryx[0];
    x[1] = tryx[1];
    y[0] = tryy[0];
    y[1] = tryy[1];
  }
  return ok;
}

#define MAXLRAND48 0x80000000l

static void SmartPlace(Twidget w, Tscreen screen) {
  dat X[2];
  dat Y[2];

  if (!w || w->Parent)
    return;

  if (w->Up == TW_MAXDAT) {
    X[1] = (X[0] = screen->XLogic) + All->DisplayWidth - 1;
    Y[1] = (Y[0] = screen->YLogic + 1) + All->DisplayHeight - screen->Up - 2;

    XWidth = w->XWidth;
    YWidth = IS_WINDOW(w) && w->Attr & WINDOW_ROLLED_UP ? 1 : w->YWidth;

    if (!doSmartPlace(screen->Widgets.First, X, Y)) {
      /* can't be smart... be random */
      if (XWidth <= X[1] - X[0])
        X[0] += lrand48() / (MAXLRAND48 / (X[1] - X[0] + 2 - XWidth));
      if (YWidth <= Y[1] - Y[0])
        Y[0] += lrand48() / (MAXLRAND48 / (Y[1] - Y[0] + 2 - YWidth));
    }
    if (XWidth > X[1] - X[0] + 1 && X[0] > TW_MINDAT)
      X[0]--;
    if (YWidth > Y[1] - Y[0] + 1 && Y[0] > TW_MINDAT)
      Y[0]--;

    w->Left = X[0] - screen->XLogic;
    w->Up = Y[0] - screen->YLogic;
  }
  w->MapTopReal(screen);
}

bool InitWM(void) {
  byte logged = false;

  srand48(time(NULL));
  if ((WM_MsgPort = Smsgport::Create(2, "WM", 0, 0, 0, WManagerH)) &&
      /* this will later be sent to rcrun.c, it forces loading ~/.config/twin/twinrc */
      SendControlMsg(WM_MsgPort, MSG_CONTROL_OPEN, 0, NULL)) {

    if (RegisterExt(WM, MsgPort, WM_MsgPort)) {

      if ((MapQueue = Smsgport::Create(11, "WM MapQueue", 0, 0, 0, (void (*)(Tmsgport))NoOp))) {

        MapQueue->Remove();

        if (InitRC()) {
          return true;
        } else {
          log(ERROR) << "twin: RC: " << Errstr << "\n";
          logged = true;
        }
      }
      UnRegisterExt(WM, MsgPort, WM_MsgPort);
    } else {
      log(ERROR) << "twin: WM: RegisterExt(WM,MsgPort) failed! Another WM is running?\n";
      logged = true;
    }
  }
  if (WM_MsgPort) {
    WM_MsgPort->Delete();
  }
  if (!logged) {
    log(ERROR) << "twin: WM: " << Errstr << "\n";
  }
  return false;
}

#if 0  // unused
void QuitWM(void) {
  QuitRC();
  UnRegisterExt(WM, MsgPort, WM_MsgPort);
  WM_MsgPort->Delete();
  MapQueue->Delete();
}
#endif // 0
