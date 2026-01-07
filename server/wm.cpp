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
#include <Tutf/Tutf.h> /* for UTF-32 <-> charset conversions */

#include "rctypes.h"
#include "rcrun.h"

static void DetailCtx(wm_ctx *c);

byte ClickWindowPos = TW_MAXBYTE;

static Tmsgport WM_MsgPort;
static Tmsgport MapQueue;

static inline ldat XBarSize(Twindow w) {
  return (ldat)w->XWidth - (ldat)5;
}

static inline ldat YBarSize(Twindow w) {
  return (ldat)w->YWidth - (ldat)4;
}

static udat TabStart(Twindow w, sbyte isX) {
  ldat NumLogicMax;
  udat ret;

  if (isX) {
    NumLogicMax = Max2(w->WLogic, w->XLogic + w->XWidth - 2);
    ret = w->XLogic * XBarSize(w) / NumLogicMax;
  } else {
    NumLogicMax = Max2(w->HLogic, w->YLogic + (ldat)w->YWidth - 2);
    ret = w->YLogic * YBarSize(w) / NumLogicMax;
  }
  return ret;
}

static udat TabLen(Twindow w, sbyte isX) {
  ldat NumLogicMax;
  udat ret;

  if (isX) {
    NumLogicMax = Max2(w->WLogic, w->XLogic + w->XWidth - 2);
    ret = ((w->XWidth - 2) * XBarSize(w) + NumLogicMax - 1) / NumLogicMax;
  } else {
    NumLogicMax = Max2(w->HLogic, w->YLogic + w->YWidth - 2);
    ret = ((w->YWidth - 2) * YBarSize(w) + NumLogicMax - 1) / NumLogicMax;
  }
  return ret ? ret : 1;
}

/* this returns -1 before the tab, 0 on the tab, 1 after */
inline sbyte IsTabPosition(Twindow w, udat pos, sbyte isX) {
  const udat start = TabStart(w, isX);
  return pos >= start ? pos - start < TabLen(w, isX) ? 0 : 1 : -1;
}

tpos WMFindBorderWindow(Twindow w, dat u, dat v, byte border, tcell *ptr_cell) {
  const trune *borderRunes;
  size_t namelen;
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

  namelen = w->Name.size();
  bool flDrag = false, flResize = false, flScroll = false;
  const bool flFocus = (w == All->Screens.First->FocusW()) || (w == All->Screens.First->MenuWindow);

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
      } else if (namelen > 0) {
        k = 2 * (ldat)u - ((ldat)XWidth - (ldat)namelen - (ldat)3);
        if (k > 0)
          k /= 2;
        if (k > 0 && k <= (ldat)namelen) {
          rune = Tutf_CP437_to_UTF_32[(byte)w->Name[--k]];
          found = POS_TITLE;
        } else if (k == 0 || k == (ldat)namelen + 1) {
          rune = ' ';
          found = POS_TITLE;
        } else {
          rune = borderRunes[horiz];
        }
      } else {
        rune = borderRunes[horiz];
      }
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
      if (namelen > 0) {
        k = 2 * (ldat)u - ((ldat)XWidth - (ldat)namelen - (ldat)3);
        if (k > 0) {
          k /= 2;
        }
        if (k > 0 && k <= (ldat)namelen) {
          rune = Tutf_CP437_to_UTF_32[(byte)w->Name[--k]];
          found = POS_TITLE;
        } else if (k == 0 || k == (ldat)namelen + 1) {
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
    if (found == POS_TITLE && w->ColName && k >= 0 && k < (ldat)namelen) {
      color = w->ColName[k];
    } else {
      color = w->ColGadgets;
    }
  } else {
    switch (found) {
    case POS_TITLE:
      if (w->ColName && k >= 0 && k < (ldat)namelen) {
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

void ShowWinList(wm_ctx *c) {
  if (!c->Screen) {
    c->Screen = All->Screens.First;
  }
  if (WinList->Parent) {
    WinList->UnMap();
  }
  if (c->ByMouse) {
    WinList->Left = c->i - 5;
    WinList->Up = c->j - c->Screen->Up;
  } else {
    WinList->Left = 0;
    WinList->Up = TW_MAXDAT;
  }
  WinList->Map((Twidget)c->Screen);
}

static void RecursiveFocusWidget(Twidget w) {
  Twidget p;
  w->SelectW = NULL;
  while ((p = w->Parent)) {
    p->SelectW = w;
    if (IS_SCREEN(p)) {
      break;
    }
    w = p;
  }
  if (ContainsCursor(WindowParent(w))) {
    UpdateCursor();
  }
}

static Twidget RecursiveFindFocusWidget(Twidget w) {
  if (w) {
    while (w->SelectW) {
      w = w->SelectW;
    }
  }
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
      } else {
        LastKeys = 0;
      }
    }
  } else {
    LastKeys = 0;
  }
}

static void HandleHilightAndSelection(Twidget W, udat Code, dat X, dat Y, byte Inside) {
  udat _Code;
  byte IsHoldButtonSelection = (Code & HOLD_ANY) == All->SetUp->ButtonSelection;

  if (isSINGLE_PRESS(Code) && HOLD_CODE(PRESS_N(Code)) == All->SetUp->ButtonSelection) {
    StartHilight((Twindow)W, (ldat)X + W->XLogic, (ldat)Y + W->YLogic);
  } else if (IsHoldButtonSelection && isMOVE(Code)) {
    ExtendHilight((Twindow)W, (ldat)X + W->XLogic, (ldat)Y + W->YLogic);
  } else if (isSINGLE_RELEASE(Code)) {

    _Code = HOLD_CODE(RELEASE_N(Code));

    if (_Code == All->SetUp->ButtonSelection && IS_WINDOW(W)) {
      SetSelectionFromWindow((Twindow)W);
    } else if (_Code == All->SetUp->ButtonPaste && Inside) {
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

static bool CheckForwardMsg(wm_ctx *c, Tmsg msg, byte WasUsed) {
  static uldat LastWId = NOID;
  static byte LastInside = tfalse;
  static udat LastKeys = 0;
  Tscreen s;
  Twidget last_w, w, p;
  event_any *event;
  udat Code;
  dat X, Y;
  byte Inside;
  bool inUse = false;

  if (msg->Type != msg_key && msg->Type != msg_mouse) {
    return inUse;
  }
  last_w = (Twidget)Id2Obj(Twidget_class_byte, LastWId);
  s = All->Screens.First;
  w = s->FocusW();

  if ((All->State & state_any) == state_menu) {
    if (!w) {
      // the menu is being used, but no menu window opened yet.
      // continue with most recently focused window (saved in MenuWindow)
      w = s->MenuWindow;
    } else {
      // the menu is being used. leave last_w. */
      w = NULL;
    }
  } else {
    Twidget click_w = s->ClickWindow;
    if (click_w && w != click_w) {
      // cannot send messages to focused window while user clicked on another window
      w = NULL;
    }
  }

  if (w) {
    w = RecursiveFindFocusWidget(w);
  } else {
    return inUse;
  }
  event = &msg->Event;
  if (msg->Type == msg_key) {
    if (!WasUsed && All->State == state_default) {

      // back up to first parent that WANT_KEYS or has AUTO_KEYS
      while (w && !(w->Attr & WIDGET_WANT_KEYS ||
                    (IS_WINDOW(w) && ((Twindow)w)->Attr & WINDOW_AUTO_KEYS))) {
        w = w->Parent;
      }
      if (w) {
        if (w->Attr & WIDGET_WANT_KEYS) {
          msg->Type = msg_widget_key;
          event->EventKeyboard.W = (Twidget)w;
          SendMsg(w->Owner, msg);
          return ttrue;
        } else if (IS_WINDOW(w) && ((Twindow)w)->Attr & WINDOW_AUTO_KEYS) {
          FallBackKeyAction((Twindow)w, &event->EventKeyboard);
        }
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

  if (!w->Owner || (IS_WINDOW(w) && ((Twindow)w)->Flags & WINDOWFL_MENU)) {
    return inUse;
  }
  c->Code = Code = event->EventMouse.Code;
  c->i = X = event->EventMouse.X;
  c->j = Y = event->EventMouse.Y;
  Inside = tfalse;

  if (LastKeys) {
    TranslateCoordsWidget(NULL, w, &X, &Y, &Inside);
    if (c->Pos == POS_ROOT || c->Pos == POS_MENU) {
      Inside = tfalse;
    }
  } else {

    c->W = NonScreenParent(w);
    c->Screen = (Tscreen)c->W->Parent;

    if (c->Screen && IS_SCREEN(c->Screen)) {

      DetailCtx(c);

      if (w == c->W) {
        X -= c->Left + !(IS_WINDOW(w) && w->Flags & WINDOWFL_BORDERLESS);
        Y -= c->Up + !(IS_WINDOW(w) && w->Flags & WINDOWFL_BORDERLESS);
        Inside = c->Pos == POS_INSIDE;
      } else {
        TranslateCoordsWidget(NULL, w, &X, &Y, &Inside);
        if (c->DW != w || c->Pos == POS_ROOT || c->Pos == POS_MENU)
          /* w may be obscured by something else */
          Inside = tfalse;
        while (Inside == ttrue + ttrue && (p = w->Parent)) {
          /* on the border, must report to the parent */
          w = p;
          X = c->i;
          Y = c->j;
          TranslateCoordsWidget(NULL, w, &X, &Y, &Inside);
        }
      }
    }
    if (Inside == ttrue + ttrue) {
      Inside = tfalse;
    }
  }

  /* manage window highlight and Selection */
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

      if (Code == MOVE_MOUSE && !Inside) {
        X = Y = TW_MINDAT;
      }
      msg->Type = msg_widget_mouse;
      event->EventMouse.W = (Twidget)w;
      event->EventMouse.X = X;
      event->EventMouse.Y = Y;
      SendMsg(w->Owner, msg);

      LastInside = (w->Attr & WIDGET_WANT_MOUSE_MOTION) ? Inside : 0;
      LastKeys = (w->Attr & WIDGET_WANT_MOUSE) ? Code & HOLD_ANY : 0;
      if (isPRESS(Code)) {
        LastKeys |= HOLD_CODE(PRESS_N(Code));
      }
      inUse = true;
    }
  }
  LastWId = w->Id;
  return inUse;
}

static ldat DragPosition[2];

static void InitCtx(const Tmsg msg, wm_ctx *c) {

  c->Code = msg->Event.EventCommon.Code;
  c->ShiftFlags = msg->Event.EventKeyboard.ShiftFlags; /* ok for mouse too */

  if ((c->ByMouse = (msg->Type == msg_mouse))) {
    c->i = msg->Event.EventMouse.X;
    c->j = msg->Event.EventMouse.Y;

    if ((c->Screen = Sscreen::Find(c->j)) && c->Screen == All->Screens.First &&
        c->Screen->Up < c->j) {
      c->W = c->Screen->FindWidgetAt(c->i, c->j - c->Screen->Up);
    } else {
      c->W = NULL;
    }
  } else {
    c->Screen = All->Screens.First;
    c->W = c->Screen->FocusW();
  }
}

static void DetailCtx(wm_ctx *c) {
  if (c->W) {
    /* ensure IS_SCREEN(c->W->Parent) is true. */
    Twidget parent = c->W->Parent;
    if (!parent || !IS_SCREEN(parent)) {
      log(ERROR) << "twin: wm.c: DetailCtx(): internal error: c->W is a subwidget!\n";
      return;
    }
    c->Screen = (Tscreen)parent;
  }
  c->DW = NULL;
  if (c->Screen) {
    c->Menu = c->Screen->FindMenu();
  }
  if (c->ByMouse) {
    c->Pos = POS_ROOT;

    if (c->W) {
      ldat HasBorder = IS_WINDOW(c->W) && !(c->W->Flags & WINDOWFL_BORDERLESS);

      c->Up = (ldat)c->W->Up - c->Screen->YLogic + (ldat)c->Screen->Up;
      c->Left = (ldat)c->W->Left - c->Screen->XLogic;
      c->Rgt = c->Left + (ldat)c->W->XWidth - 1;
      c->Dwn = c->Up + (c->W->Attr & WINDOW_ROLLED_UP ? 0 : (ldat)c->W->YWidth - (ldat)1);

      if (c->i >= c->Left + HasBorder && c->i <= c->Rgt - HasBorder && c->j >= c->Up + HasBorder &&
          c->j <= c->Dwn - HasBorder) {

        c->Pos = POS_INSIDE;
        c->DW = RecursiveFindWidgetAt((Twidget)c->W, c->i - c->Left, c->j - c->Up);
      } else if (HasBorder) {
        /*
         * (i,j) _may_ be outside the window... see ContinueMenu()
         * and CheckForwardMsg()
         */
        if (IS_WINDOW(c->W) &&
            (c->i == c->Left || c->i == c->Rgt || c->j == c->Up || c->j == c->Dwn))
          c->Pos = ((Twindow)c->W)->FindBorder(c->i - c->Left, c->j - c->Up, 0, NULL);
      }
    }
    /*
     * allow code above to calculate c->Left, c->Up
     * (used for Interactive Drag/Resize/Scroll)
     * but return the correct c->Pos :
     */
    if (c->Screen && c->j <= c->Screen->Up) {
      c->Pos = POS_ROOT;
      if (c->j == c->Screen->Up) {
        if (c->i > All->DisplayWidth - (dat)3)
          c->Pos = POS_BUTTON_SCREEN;
        else {
          c->Pos = POS_MENU;
          c->W = c->Screen->FocusW();
          c->Item = c->Menu->FindItem(c->i);
        }
      }
    }
  }
}

inline void Fill4RC_VM(wm_ctx *c, Twidget W, udat Type, byte Pos, udat Code) {
  c->W = W;
  c->Type = Type;
  c->Pos = Pos;
  c->Code = Code;
}

void FocusCtx(wm_ctx *c) {
  if (c->W) {
    c->Screen = ScreenParent(c->W);
  }
  if (c->Screen && c->Screen != All->Screens.First) {
    c->Screen->Focus();
  } else {
    c->Screen = All->Screens.First;
  }
  if (c->W && c->W != c->Screen->FocusW()) {
    c->W->Focus();
  } else {
    c->W = c->Screen->FocusW();
  }
}

static byte ActivateScreen(wm_ctx *c) {
  if (c->Screen && c->Screen != All->Screens.First) {
    c->Screen->Focus();
  }
  c->Screen = All->Screens.First;
  All->State = state_screen | (c->ByMouse ? state_fl_bymouse : 0);
  c->Screen->DrawMenu(0, TW_MAXDAT);
  return ttrue;
}

/* this is mouse-only */
static void ContinueScreen(wm_ctx *c) {
  ResizeFirstScreen(c->j - All->Screens.First->Up);
}

static void ReleaseScreen(wm_ctx *c) {
  (void)c;
  All->State = state_default;
  All->Screens.First->DrawMenu(0, TW_MAXDAT);
}

/* this is mouse-only */
static byte ActivateScreenButton(wm_ctx *c) {
  if (c->Screen == All->Screens.First && (All->State & state_any) == state_default) {
    All->State = state_button_screen | state_fl_bymouse;
    c->Screen->Flags |= SCREENFL_BACK_SELECT | SCREENFL_BACK_PRESSED;
    c->Screen->DrawMenu(All->DisplayWidth - (dat)2, All->DisplayWidth - (dat)1);
    return ttrue;
  }
  return tfalse;
}

#if 0
/* this is mouse-only */
static void ContinueScreenButton(wm_ctx *c) {
    udat temp = c->Screen->State;

    DetailCtx(c);

    if (c->Pos == POS_BUTTON_SCREEN) {
        c->Screen->State |= GADGET_PRESSED;
    } else {
        c->Screen->State &= ~GADGET_PRESSED;
    }
    if (temp != c->Screen->State) {
        c->Screen->DrawMenu( All->DisplayWidth-(dat)2, All->DisplayWidth-(dat)1);
    }
}
#endif

/* this is mouse-only */
static void ReleaseScreenButton(wm_ctx *c) {

  All->State = state_default;
  c->Screen->Flags &= ~(SCREENFL_BACK_SELECT | SCREENFL_BACK_PRESSED);

  if (c->Screen != All->Screens.Last && (DetailCtx(c), c->Pos == POS_BUTTON_SCREEN)) {
    MoveLast(Screens, All, c->Screen);
    DrawArea2(NULL, NULL, NULL, 0, Min2(c->Screen->Up, All->Screens.First->Up), TW_MAXDAT,
              TW_MAXDAT, tfalse);
    UpdateCursor();
  } else {
    c->Screen->DrawMenu(All->DisplayWidth - (dat)2, All->DisplayWidth - (dat)1);
  }
}

static bool ActivateMenu(wm_ctx *c) {
  if (c->Screen && c->Screen != All->Screens.First) {
    c->Screen->Focus();
  }
  c->Screen = All->Screens.First;
  c->W = c->Screen->FocusW();
  c->Menu = c->Screen->FindMenu();

  if (c->ByMouse) {
    if (c->j == c->Screen->Up) {
      c->Item = c->Menu->FindItem(c->i);
    } else {
      c->Item = (Tmenuitem)0;
    }
  } else {
    if (!(c->Item = c->Menu->GetSelectedItem()) && !(c->Item = c->Menu->Items.First)) {
      c->Item = All->CommonMenu->Items.First;
    }
  }
  c->Screen->ActivateMenu(c->Item, c->ByMouse);

  return true;
}

/* this is mouse-only */
static void ContinueMenu(wm_ctx *c) {
  Twindow w;
  uldat y;

  DetailCtx(c);

  if (c->Pos == POS_MENU && c->Item && c->Item != c->Menu->GetSelectedItem()) {
    SetMenuState(c->Item, ttrue);
  } else if (c->Pos == POS_INSIDE && (w = (Twindow)c->W) && (w->Flags & WINDOWFL_MENU)) {
    y = (ldat)c->j - c->Up - (ldat)1 + w->YLogic;
    if ((c->Item = (Tmenuitem)w->FindRow(y))) {
      SetMenuState(c->Item, ttrue);
      return;
    }
  } else if ((w = (Twindow)All->Screens.First->FocusW()) && (w->Flags & WINDOWFL_MENU) &&
             (c->Item = (Tmenuitem)w->FindRow(w->CurY)) && IS_MENUITEM(c->Item) &&
             !c->Item->Window) {

    if ((w = (Twindow)c->Item->Parent) && IS_WINDOW(w)) {
      SetMenuState(c->Item = w->MenuItem, ttrue);
    } else {
      SetMenuState(c->Item = c->Menu->GetSelectedItem(), ttrue);
    }
  }
}

static void ReleaseMenu(wm_ctx *c) {
  Tscreen s = All->Screens.First;
  Twindow win_m = s->MenuWindow;
  Twindow win_f = (Twindow)s->FocusW();
  Tmenu menu;
  Tmenuitem item;
  Trow Row;
  Tmsg msg;
  event_menu *event;
  udat Code;

  if (win_f && IS_WINDOW(win_f) && win_f->CurY < TW_MAXLDAT && (menu = win_f->Menu) &&
      (item = menu->GetSelectedItem()) && (item->Flags & ROW_ACTIVE) &&
      (Row = win_f->FindRow(win_f->CurY)) && (Row->Flags & ROW_ACTIVE) && Row->Code) {

    Code = Row->Code;
  } else {
    Code = (udat)0;
  }
  /* disable the menu BEFORE processing the code! */
  CloseMenu();

  if (Code >= COD_RESERVED) {
    Fill4RC_VM(c, win_m, msg_menu_row, POS_MENU, Row->Code);
    (void)RC_VMQueue(c);
    /* handle COD_RESERVED codes internally */
  } else if (Code && (msg = Smsg::Create(msg_menu_row, 0))) {
    event = &msg->Event.EventMenu;
    event->W = win_m;
    event->Code = Code;
    event->Menu = menu;
    event->Row = Row;
    SendMsg(win_m ? win_m->Owner : menu->MsgPort, msg);
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

static bool ActivateDrag(wm_ctx *c) {
  if (c->Screen == All->Screens.First && c->W && IS_WINDOW(c->W) && c->W->Attr & WINDOW_DRAG) {

    All->Screens.First->ClickWindow = (Twindow)c->W;
    All->State = state_drag;
    if (c->ByMouse) {
      All->State |= state_fl_bymouse;

      DetailCtx(c);
      DragPosition[0] = (ldat)c->i - c->Left;
      DragPosition[1] = (ldat)c->j - c->Up;
    }
    DrawBorderWindow((Twindow)c->W, BORDER_ANY);
    return true;
  }
  return false;
}

static bool ActivateResize(wm_ctx *c) {
  if (c->Screen == All->Screens.First && c->W && IS_WINDOW(c->W) && c->W->Attr & WINDOW_RESIZE) {

    All->Screens.First->ClickWindow = (Twindow)c->W;
    All->State = state_resize;
    if (c->ByMouse) {
      All->State |= state_fl_bymouse;

      DetailCtx(c);
      DragPosition[0] = (ldat)c->i - c->Rgt;
      DragPosition[1] = (ldat)c->j - c->Dwn;
    }
    DrawBorderWindow((Twindow)c->W, BORDER_ANY);
    ShowResize((Twindow)c->W);
    return true;
  }
  return false;
}

static bool ActivateScroll(wm_ctx *c) {
  Twindow w = (Twindow)c->W;

  if (c->Screen == All->Screens.First && w && IS_WINDOW(w) &&
      c->W->Attr & (WINDOW_X_BAR | WINDOW_Y_BAR)) {

    /*
     * paranoia: SneakSetupMouse() may call us even for a mouse event
     * OUTSIDE or INSIDE c->W
     */
    if (c->ByMouse) {
      DetailCtx(c);
      if ((ldat)c->j == c->Dwn) {
        if (c->Pos == POS_X_ARROW_BACK) {
          w->State |= X_BAR_SELECT | ARROW_BACK_SELECT;
        } else if (c->Pos == POS_X_ARROW_FWD) {
          w->State |= X_BAR_SELECT | ARROW_FWD_SELECT;
        } else if (c->Pos == POS_X_BAR_BACK) {
          w->State |= X_BAR_SELECT | PAGE_BACK_SELECT;
        } else if (c->Pos == POS_X_BAR_FWD) {
          w->State |= X_BAR_SELECT | PAGE_FWD_SELECT;
        } else {
          w->State |= X_BAR_SELECT | TAB_SELECT;
          DragPosition[0] = (ldat)c->i - c->Left - 1 - TabStart(w, (sbyte)1);
          DragPosition[1] = 0;
        }
      } else if ((ldat)c->i == c->Rgt) {
        if (c->Pos == POS_Y_ARROW_BACK) {
          w->State |= Y_BAR_SELECT | ARROW_BACK_SELECT;
        } else if (c->Pos == POS_Y_ARROW_FWD) {
          w->State |= Y_BAR_SELECT | ARROW_FWD_SELECT;
        } else if (c->Pos == POS_Y_BAR_BACK) {
          w->State |= Y_BAR_SELECT | PAGE_BACK_SELECT;
        } else if (c->Pos == POS_Y_BAR_FWD) {
          w->State |= Y_BAR_SELECT | PAGE_FWD_SELECT;
        } else {
          w->State |= Y_BAR_SELECT | TAB_SELECT;
          DragPosition[0] = 0;
          DragPosition[1] = (ldat)c->j - c->Up - 1 - TabStart(w, (sbyte)0);
        }
      }
      if (w->State & SCROLL_ANY_SELECT) {
        All->State = state_scroll | state_fl_bymouse;
      }
    } else {
      All->State = state_scroll;
    }
    if ((All->State & state_any) == state_scroll && IS_WINDOW(w)) {
      All->Screens.First->ClickWindow = w;
      DrawBorderWindow(w, BORDER_ANY);
      return true;
    }
  }
  return false;
}

/* this is mouse only */
static void ContinueDrag(wm_ctx *c) {
  if ((c->W = (Twidget)All->Screens.First->ClickWindow)) {
    DetailCtx(c);
    if (c->W == All->Screens.First->Widgets.First) {
      DragFirstWindow(c->i - c->Left - DragPosition[0],
                      Max2(c->j, All->Screens.First->Up + 1) - c->Up - DragPosition[1]);
    } else {
      DragWindow((Twindow)c->W, c->i - c->Left - DragPosition[0],
                 Max2(c->j, All->Screens.First->Up + 1) - c->Up - DragPosition[1]);
    }
  }
}

/* this is mouse only */
static void ContinueResize(wm_ctx *c) {

  if ((c->W = (Twidget)All->Screens.First->ClickWindow)) {
    DetailCtx(c);
    if (c->W == All->Screens.First->Widgets.First) {
      ResizeRelFirstWindow(c->i - c->Rgt - DragPosition[0],
                           Max2(c->j, All->Screens.First->Up + 1) - c->Dwn - DragPosition[1]);
    } else {
      ResizeRelWindow((Twindow)c->W, c->i - c->Rgt - DragPosition[0],
                      Max2(c->j, All->Screens.First->Up + 1) - c->Dwn - DragPosition[1]);
    }
    ShowResize((Twindow)c->W);
  }
}

/* this is mouse only */
static void ContinueScroll(wm_ctx *c) {
  Twindow w;
  uldat NumLogicMax;
  ldat i;

  if ((c->W = (Twidget)(w = All->Screens.First->ClickWindow))) {
    DetailCtx(c);

    if (w->State & X_BAR_SELECT) {
      NumLogicMax = Max2(w->WLogic, w->XLogic + (ldat)w->XWidth - 2);
      i = c->i;
      if (i + 4 > c->Rgt + DragPosition[0])
        i = c->Rgt + DragPosition[0] - 4;
      ScrollWindow(w,
                   (i - c->Left - 1 - DragPosition[0] - TabStart(w, (sbyte)1)) *
                       (NumLogicMax / (w->XWidth - 5)),
                   0);

    } else if (w->State & Y_BAR_SELECT) {
      NumLogicMax = Max2(w->HLogic, w->YLogic + (ldat)w->YWidth - 2);
      i = Max2(c->j, All->Screens.First->Up + 1);
      if (i + 3 > c->Dwn + DragPosition[1]) {
        i = c->Dwn + DragPosition[1] - 3;
      }
      ScrollWindow(w, 0,
                   (i - c->Up - 1 - DragPosition[1] - TabStart(w, (sbyte)0)) *
                       (NumLogicMax / (w->YWidth - (udat)4)));
    }
  }
}

static void ReleaseDragResizeScroll(const wm_ctx * /*c*/) {
  Twindow win_f = All->Screens.First->ClickWindow;
  udat wasResize;

  wasResize = (All->State & state_any) == state_resize;
  All->State = state_default;

  if (win_f) {
    win_f->State &= ~(BUTTON_ANY_SELECT | SCROLL_ANY_SELECT | XY_BAR_SELECT);
    DrawBorderWindow(win_f, BORDER_ANY);

    if (wasResize) {
      Check4Resize(win_f);
      HideResize();
    }
  }
}

/* this is mouse only */
static bool ActivateButton(wm_ctx *c) {
  if (c->W && IS_WINDOW(c->W)) {
    All->State = c->Pos | state_fl_bymouse;
    ((Twindow)c->W)->State |= (BUTTON_FIRST_SELECT << c->Pos) | WINDOW_GADGET_PRESSED;

    c->Type = msg_mouse;
    (void)RC_VMQueue(c);

    DrawBorderWindow((Twindow)c->W, BORDER_UP);
    return true;
  }
  return false;
}

#if 0
/* this is mouse only */
/*
 * this would release window border buttons when moving away from them...
 * I prefer them to stay pressed, so this is disabled
 */
static void ContinueButton(wm_ctx *c) {
    Twindow w = All->Screens.First->ClickWindow;
    uldat ltemp;
    byte found = tfalse;

    if (!w)
        return;

    if (w == c->W && (ltemp = w->State) & BUTTON_ANY_SELECT) {
        DetailCtx(c);
        if (c->Pos < BUTTON_MAX &&
            (ltemp & BUTTON_ANY_SELECT) == (BUTTON_FIRST_SELECT << c->Pos))

            found = ttrue;
    }

    if (found)
        w->State |= GADGET_PRESSED;
    else
        w->State &= ~GADGET_PRESSED;
    if (ltemp != w->State)
        DrawBorderWindow(w, BORDER_UP);
}
#endif

/* this is mouse only */
static void ReleaseButton(wm_ctx *c) {
  Twindow w = All->Screens.First->ClickWindow;

  All->State = state_default;
  if (w) {
    if (w == (Twindow)c->W && w->State & BUTTON_ANY_SELECT) {
      DetailCtx(c);

      if (c->Pos < BUTTON_MAX &&
          (w->State & BUTTON_ANY_SELECT) == (BUTTON_FIRST_SELECT << c->Pos)) {

        c->W = (Twidget)w;
        c->Type = msg_mouse;
        (void)RC_VMQueue(c);
      }
    }
    w->State &= ~(BUTTON_ANY_SELECT | WINDOW_GADGET_PRESSED);
    DrawBorderWindow(w, BORDER_UP);
  }
}

/* this is mouse only */
static byte ActivateGadget(wm_ctx *c) {
  Tgadget G = (Tgadget)c->DW;
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
static void ContinueGadget(wm_ctx *c) {
  Twindow w = All->Screens.First->ClickWindow;
  Tgadget fg;
  udat temp;

  if (w && (fg = (Tgadget)RecursiveFindFocusWidget((Twidget)w))) {
    temp = fg->Flags;

    if (!(temp & GADGETFL_TOGGLE)) {
      if (w == (Twindow)c->W && fg && (DetailCtx(c), (Twidget)fg == c->DW))
        fg->Flags |= GADGETFL_PRESSED;
      else
        fg->Flags &= ~GADGETFL_PRESSED;

      if (temp != fg->Flags) {
        if ((Twidget)w == All->Screens.First->Widgets.First)
          DrawUnobscuredWidget((Twidget)fg, 0, 0, TW_MAXDAT, TW_MAXDAT, tfalse);
        else
          DrawAreaWidget((Twidget)fg);
      }
    }
  }
}

/* this is mouse only */
static void ReleaseGadget(wm_ctx *c) {
  Twindow w = All->Screens.First->ClickWindow;
  Tgadget fg;

  All->State = state_default;
  if (!w)
    return;

  DetailCtx(c);

  fg = (Tgadget)RecursiveFindFocusWidget((Twidget)w);

  if (!fg || !IS_GADGET(fg) || fg->Flags & GADGETFL_TOGGLE)
    return;

  UnPressGadget(fg, (Twidget)w == c->W && fg && (Twidget)fg == c->DW);
  /* w->SelectW=NULL; */
}

/* the only Activate*() that make sense from within RC_VM() */
bool ActivateCtx(wm_ctx *c, byte State) {
  switch (State) {
  case state_resize:
    return ActivateResize(c);
  case state_drag:
    return ActivateDrag(c);
  case state_scroll:
    return ActivateScroll(c);
  case state_menu:
    return ActivateMenu(c);
  case state_screen:
    return ActivateScreen(c);
  default:
    break;
  }
  return false;
}

/* force returning to state_default. used before RCReload() */
void ForceRelease(const wm_ctx *c) {
  switch (All->State & state_any) {
  case state_resize:
  case state_drag:
  case state_scroll:
    ReleaseDragResizeScroll(c);
    break;
  case state_gadget: {
    Twindow fw;
    Tgadget fg;

    if ((fw = All->Screens.First->ClickWindow) &&
        (fg = (Tgadget)RecursiveFindFocusWidget((Twidget)fw)) && IS_GADGET(fg) &&
        !(fg->Flags & GADGETFL_TOGGLE))

      UnPressGadget(fg, tfalse);
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
      Twindow fw;
      if ((fw = All->Screens.First->ClickWindow))
        fw->State &= ~(BUTTON_ANY_SELECT | WINDOW_GADGET_PRESSED);
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
static bool ActivateMouseState(wm_ctx *c) {
  bool used = false;

  if (isSINGLE_PRESS(c->Code)) {
    bool isPressButtonSelection = HOLD_CODE(PRESS_N(c->Code)) == All->SetUp->ButtonSelection;

    switch (c->Pos) {
    case POS_BUTTON_SCREEN:
      if (isPressButtonSelection) {
        used = true;
        ActivateScreenButton(c);
      }
      break;
    case POS_INSIDE:
      if (isPressButtonSelection && c->DW && IS_GADGET(c->DW) &&
          !(((Tgadget)c->DW)->Flags & GADGETFL_DISABLED)) {
        used = true;
        ActivateGadget(c);
      }
      break;
    default:
      if (c->Pos < BUTTON_MAX) {
        used = true;
        ActivateButton(c);
      }
      break;
    }
  }
  return used;
}

/*
 * check if the mouse (c->i, c->j) is in a position suitable for the State.
 * setup DragPosition[] as if the current State was initiated with the mouse,
 * or return false if (c->i, c->j) is in a non-appropriate position.
 */
static bool SneakSetupMouse(wm_ctx *c) {
  /* State was set with keyboard */
  Twindow w = All->Screens.First->ClickWindow;
  bool ok = true;

  switch (All->State & state_any) {
  case state_resize:
    DetailCtx(c);
    if (IS_WINDOW(w) && w == (Twindow)c->W) {
      DragPosition[0] = (ldat)c->i - c->Rgt;
      DragPosition[1] = (ldat)c->j - c->Dwn;
    } else {
      ok = false;
    }
    break;
  case state_drag:
    DetailCtx(c);
    if (IS_WINDOW(w) && w == (Twindow)c->W) {
      DragPosition[0] = (ldat)c->i - c->Left;
      DragPosition[1] = (ldat)c->j - c->Up;
    } else {
      ok = false;
    }
    break;
  case state_scroll:
    /*
     * this is complex... we must setup w->State with what you clicked upon.
     * do the dirty way and reuse the function ActivateScroll().
     * since it returns (void), check for (w->State & SCROLL_ANY_SELECT).
     */
    if (IS_WINDOW(w) && w == (Twindow)c->W) {
      ActivateScroll(c);
      ok = (w->State & SCROLL_ANY_SELECT) != 0;
    } else {
      ok = false;
    }
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
static void ContinueReleaseMouseState(wm_ctx *c, byte State) {
  if (isSINGLE_RELEASE(c->Code)) {
    switch (State) {
    case state_resize:
    case state_drag:
    case state_scroll:
      ReleaseDragResizeScroll(c);
      break;
    case state_gadget:
      ReleaseGadget(c);
      break;
    case state_menu:
      ReleaseMenu(c);
      break;
    case state_screen:
      ReleaseScreen(c);
      break;
    case state_button_screen:
      ReleaseScreenButton(c);
      break;
    case state_root:
    case state_default:
    default:
      if (State < BUTTON_MAX)
        ReleaseButton(c);
      else
        /* paranoid... */
        All->State = state_default;
      break;
    }
  } else if (isSINGLE_PRESS(c->Code) || isSINGLE_MOVE(c->Code) || isRELEASE(c->Code)) {
    switch (State) {
    case state_resize:
      ContinueResize(c);
      break;
    case state_drag:
      ContinueDrag(c);
      break;
    case state_scroll:
      ContinueScroll(c);
      break;
    case state_gadget:
      ContinueGadget(c);
      break;
    case state_menu:
      ContinueMenu(c);
      break;
    case state_screen:
      ContinueScreen(c);
      break;
    /*case state_button_screen: ContinueScreenButton(c); break; */
    case state_root:
    case state_default:
      break;
    default:
      /* if (State < BUTTON_MAX)
       *  ContinueButton(c); */
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
static bool ActivateKeyState(wm_ctx *c, byte State) {
  Twindow w = (Twindow)All->Screens.First->FocusW();
  Tmenuitem M;
  ldat numrow;
  dat XDelta = 0, YDelta = 0, depth;
  udat Key = c->Code;
  bool used = false;

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
      used = true;
      ReleaseDragResizeScroll(c);
    } else if (State == state_resize && (w->Attr & WINDOW_RESIZE)) {
      used = true;
      ResizeRelWindow(w, XDelta, YDelta);
      ShowResize(w);
    } else if (State == state_drag && (w->Attr & WINDOW_DRAG)) {
      used = true;
      DragWindow(w, XDelta, YDelta);
    }
    break;
  case state_scroll:
    if (Key == TW_Escape || Key == TW_Return) {
      ReleaseDragResizeScroll(c);
      used = true;
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
        used = true;
        ScrollWindow(w, XDelta, YDelta);
      }
    }
    break;
  case state_menu:
    c->Menu = c->Screen->FindMenu();
    c->Item = c->Menu->GetSelectedItem();
    M = c->Menu->RecursiveGetSelectedItem(&depth);
    switch (Key) {
    case TW_Escape:
      if (depth <= 1) {
        CloseMenu();
      } else {
        SetMenuState(w->MenuItem, tfalse);
      }
      used = true;
      break;
    case TW_Return:
      if (M && IS_MENUITEM(M) && M->Window) {
        EnterItem(M);
      } else {
        ReleaseMenu(c);
      }
      used = true;
      break;
    case TW_Left:
      if (depth > 0 && (All->SetUp->Flags & setup_menu_relax) && (w = (Twindow)M->Parent) &&
          IS_WINDOW(w) && (M = w->MenuItem) && (w = (Twindow)M->Parent) && IS_WINDOW(w)) {
        SetMenuState(M, tfalse);
      } else if (depth <= 1) {
        SetMenuState(c->Item = PrevItem(c->Item, c->Menu), tfalse);
      }
      used = true;
      break;
    case TW_Right:
      if (depth > 0 && (All->SetUp->Flags & setup_menu_relax) && M->Window)
        EnterItem(M);
      else if (depth <= 1)
        SetMenuState(c->Item = NextItem(c->Item, c->Menu), tfalse);
      used = true;
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
      used = true;
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
      used = true;
      break;
    default:
      break;
    }
    break;
  case state_screen:
    if (Key == TW_Escape || Key == TW_Return) {
      used = true;
      ReleaseScreen(c);
    } else if (YDelta) {
      used = true;
      ResizeFirstScreen(-YDelta);
    }
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
static void TryAutoFocus(wm_ctx *c) {
  Twidget w, deepW, oldW, focusW = All->Screens.First->FocusW();

  if (!focusW)
    focusW = (Twidget)All->Screens.First->MenuWindow;

  if (!focusW)
    return;

  oldW = RecursiveFindFocusWidget((Twidget)All->Screens.First);

  if ((w = c->W) && w == focusW && (deepW = c->DW) && deepW != oldW) {

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
static void WManagerH(Tmsgport /*msgport*/) {
  static wm_ctx _C;
  wm_ctx *c = &_C;
  Tmsg msg;
  Twidget w;
  bool used = false;

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

          Fill4RC_VM(c, NULL, msg_control, POS_ROOT, MSG_CONTROL_OPEN);
          (void)RC_VMQueue(c);
        }
      }
      msg->Delete();
      continue;
    }

    InitCtx(msg, c);

    if (All->State == state_default) {
      if (c->ByMouse && isSINGLE_PRESS(c->Code)) {
        if (c->Screen && c->Screen != All->Screens.First) {
          c->Screen->Focus();
          InitCtx(msg, c);
        }
        if (HOLD_CODE(PRESS_N(c->Code)) == All->SetUp->ButtonSelection) {
          if (c->W && c->W != c->Screen->FocusW()) {
            c->W->Focus();
          }
          DetailCtx(c);
          w = RecursiveFindFocusWidget((Twidget)All->Screens.First);
          if (c->DW && c->DW != w) {
            RecursiveFocusWidget(c->DW);
          }
          All->State |= state_fl_bymouse;
        } else {
          DetailCtx(c);
        }
        /* mouse action, setup ClickWindow */
        ClickWindowPos = c->Pos;
        All->Screens.First->ClickWindow = (Twindow)c->W;

        used = ActivateMouseState(c);
      } else {
        used = false;
        DetailCtx(c);

        if (c->ByMouse) {
          if (!isPRESS(c->Code) && !(c->Code & HOLD_ANY)) {
            TryAutoFocus(c);
          }
        } else {
          /* for keyboard actions, ClickWindow == FocusW */
          All->Screens.First->ClickWindow = (Twindow)c->W;
        }
      }

      if (!used) {
        c->Type = c->ByMouse ? msg_mouse : msg_key;
        used = RC_VMQueue(c);
      }

    } else if (c->ByMouse) {

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
        if (SneakSetupMouse(c)) {
          /* ok, mouse is in a meaningful position */
          All->State |= state_fl_bymouse;
        } else {
          /* clicked in a strange place? return to state_default */
          ForceRelease(c);
        }
      }
      if (All->State & state_fl_bymouse) {
        ContinueReleaseMouseState(c, All->State & state_any);
      }
    } else /* (!c->ByMouse) */ {
      used = ActivateKeyState(c, All->State);
    }

    /* cleanup ClickWindow if not needed anymore */
    if ((All->State & state_any) == state_default &&
        (!c->ByMouse || !isPRESS(c->Code) || !(c->Code & HOLD_ANY))) {

      ClickWindowPos = TW_MAXBYTE;
      All->Screens.First->ClickWindow = NULL;
    }

    /* must we send the event to the focused window too ? */
    if (c->ByMouse || (All->State == state_default && !used)) {

      if (CheckForwardMsg(c, msg, used)) {
        /* don't msg->Delete() ! */
        continue;
      }
    }

    msg->Delete();
  }

  if (All->State == state_default) {
    unsigned n;
    for (n = 30, msg = MapQueue->Msgs.First; msg && n; msg = msg->Next, n--) {
    }
    if (!n) {
      /* 30 or more windows to be mapped => mark the whole screen as "to be redrawn" */
      QueuedDrawArea2FullScreen = true;
    }
    while ((msg = MapQueue->Msgs.First)) {
      c->W = msg->Event.EventMap.W;
      SmartPlace((Twidget)c->W, msg->Event.EventMap.Screen);

      Fill4RC_VM(c, c->W, msg_map, POS_ROOT, 0);
      (void)RC_VMQueue(c);

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

static bool doSmartPlace(Twidget w, dat x[2], dat y[2]) {
  dat wleft, wright, tryx[2];
  dat wup, wdown, tryy[2];
  bool ok = false;

  if (XWidth > x[1] - x[0] + 1 || YWidth > y[1] - y[0] + 1) {
    return false;
  }
  if (!w) {
    return true;
  }
  wright = (wleft = w->Left) + w->XWidth;
  wdown = (wup = w->Up) + (IS_WINDOW(w) && (((Twindow)w)->Attr & WINDOW_ROLLED_UP) ? 1 : w->YWidth);
  w = w->Next;

  if (x[0] >= wright || x[1] < wleft || y[0] >= wdown || y[1] < wup) {
    return w ? doSmartPlace(w, x, y) : true;
  }
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
