/*
 *  methods.c  --  functions for OO-style calls on twin data structures
 *                 (windows, menus, ...)
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
#include "main.h"
#include "builtin.h"
#include "methods.h"
#include "obj/id.h"
#include "data.h"
#include "extreg.h"

#include "dl.h"
#include "draw.h"
#include "printk.h"
#include "resize.h"
#include "util.h"
#include "hw.h"
#include "hw_multi.h"

#include <Tw/Tw.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>
#include <Tutf/Tutf.h>
#include <Tutf/Tutf_defs.h>

#ifdef CONF_EXT
#include "extensions/ext_query.h"
#endif

/* some object-oriented ones not included in fn_obj */

void *OverrideMth(void **where, void *OrigMth, void *NewMth) {
  if (*where == OrigMth) {
    *where = NewMth;
    return NewMth;
  }
  return NULL;
}

#if 0 /* not used */
INLINE void DeletePartialList(obj Obj) {
  obj Next;
  while (Obj) {
    Next = Obj->Next;
    Obj->Delete();
    Obj = Next;
  }
}
#endif

INLINE void InsertGeneric(obj Obj, obj_parent Parent, obj Prev, obj Next, ldat *ObjCount) {
  if (Obj->Prev || Obj->Next)
    return;

  if (Prev)
    Next = Prev->Next;
  else if (Next)
    Prev = Next->Prev;

  if ((Obj->Prev = Prev))
    Prev->Next = Obj;
  else
    Parent->First = Obj;

  if ((Obj->Next = Next))
    Next->Prev = Obj;
  else
    Parent->Last = Obj;

  if (ObjCount)
    (*ObjCount)++;
}

INLINE void RemoveGeneric(obj Obj, obj_parent Parent, ldat *ObjCount) {
  if (Obj->Prev)
    Obj->Prev->Next = Obj->Next;
  else if (Parent->First == Obj)
    Parent->First = Obj->Next;

  if (Obj->Next)
    Obj->Next->Prev = Obj->Prev;
  else if (Parent->Last == Obj)
    Parent->Last = Obj->Prev;

  Obj->Prev = Obj->Next = (obj)0;
  if (ObjCount)
    (*ObjCount)--;
}

/* fn_obj and others fn_XXX functions */

/* obj */

static void InsertObj(obj Obj, obj Parent, obj Prev, obj Next) {
  printk("twin: internal error: pure virtual function InsertObj() called!\n");
#if 0
    if (!Obj->Parent && Parent) {
	InsertGeneric(Obj, Parent, Prev, Next, (ldat *)0);
	Obj->Parent = (obj)Parent;
    }
#endif
}

static void RemoveObj(obj Obj) {
  printk("twin: internal error: pure virtual function RemoveObj() called!\n");
#if 0
    if (Obj->Parent) {
	RemoveGeneric(Obj, (obj_parent)Obj->Parent, (ldat *)0);
	Obj->Parent = (obj)0;
    }
#endif
}

static void DeleteObj(obj Obj) {
  /* not a good idea to RemoveObj(Obj) here */
  DropId(Obj);
  FreeMem(Obj);
}

static struct s_fn_obj _FnObj = {
    obj_magic, sizeof(struct s_obj),
    InsertObj, RemoveObj,
    DeleteObj, (void (*)(obj, udat, uldat, uldat))NoOp,
};

/* widget */

static void InsertWidget(widget W, widget Parent, widget Prev, widget Next) {
  if (Parent)
    /*
     * don't check W->Parent here, as Raise() and Lower() call
     * RemoveWidget() then InsertWidget() but RemoveWidget() does not reset W->Parent
     */
    InsertGeneric((obj)W, (obj_parent)&Parent->FirstW, (obj)Prev, (obj)Next, (ldat *)0);
}

static void RemoveWidget(widget W) {
  if (W->Parent)
    RemoveGeneric((obj)W, (obj_parent)&W->Parent->FirstW, (ldat *)0);
}

static void DeleteWidget(widget W) {
  fn_obj Fn_Obj = W->Fn->Fn_Obj;

  Act(UnMap, W)(W);
  if (W->Hook)
    Act(RemoveHook, W)(W, W->Hook, W->WhereHook);
  if (W->ShutDownHook)
    W->ShutDownHook(W);
  Act(DisOwn, W)(W);
  while (W->FirstW)
    Act(UnMap, W->FirstW)(W->FirstW);

  Fn_Obj->Delete((obj)W);
}

static void SetFillWidget(widget W, tcell Fill) {
  if (W->USE_Fill != Fill) {
    W->USE_Fill = Fill;
    if (W->Parent)
      DrawAreaWidget(W);
  }
}

static void ChangeFieldWidget(widget W, udat field, uldat CLEARMask, uldat XORMask) {
  uldat i;

  if (W)
    switch (field) {
    case TWS_widget_Left:
    case TWS_widget_Up:
    case TWS_widget_Width:
    case TWS_widget_Height:
      break;
    case TWS_widget_USE_Fill:
      i = (tcell)((W->USE_Fill & ~CLEARMask) ^ XORMask);
      SetFillWidget(W, i);
      break;
    case TWS_widget_XLogic:
    case TWS_widget_YLogic:
      break;
    default:
      W->Fn->Fn_Obj->ChangeField((obj)W, field, CLEARMask, XORMask);
      break;
    }
}

widget FocusWidget(widget W) {
  widget oldW;
  if (W)
    oldW = Act(KbdFocus, W)(W);
  else
    oldW = Do(KbdFocus, widget)(W);

  if (W != oldW && (!W || W->Parent == (widget)All->FirstScreen)) {
    if (W && IS_WINDOW(W))
      DrawBorderWindow((window)W, BORDER_ANY);
    if (oldW && IS_WINDOW(oldW))
      DrawBorderWindow((window)oldW, BORDER_ANY);
    if ((W && IS_WINDOW(W)) || (oldW && IS_WINDOW(oldW))) {
      UpdateCursor();
      if (!W || !IS_WINDOW(W) || !(((window)W)->Flags & WINDOWFL_MENU))
        Act(DrawMenu, All->FirstScreen)(All->FirstScreen, 0, TW_MAXDAT);
    }
  }
  return oldW;
}

#define TtyKbdFocus FakeKbdFocus
widget FakeKbdFocus(widget W) {
  widget oldW;
  widget P;
  screen Screen = W && (P = W->Parent) && IS_SCREEN(P) ? (screen)P : All->FirstScreen;

  if (Screen) {
    oldW = Screen->FocusW;
    Screen->FocusW = W;
  } else
    oldW = (widget)0;

  return oldW;
}

static gadget FindGadgetByCode(widget Parent, udat Code) {
  widget W;

  for (W = Parent->FirstW; W; W = W->Next) {
    if (IS_GADGET(W) && ((gadget)W)->Code == Code)
      return (gadget)W;
  }
  return (gadget)0;
}

static void IncMouseMotionN(void) {
  if (!All->MouseMotionN++)
    EnableMouseMotionEvents(ttrue);
}

static void DecMouseMotionN(void) {
  if (All->MouseMotionN && !--All->MouseMotionN)
    EnableMouseMotionEvents(tfalse);
}

static void MapWidget(widget W, widget Parent) {
  msg Msg;

  if (W && !W->Parent && !W->MapQueueMsg && Parent) {
    if (IS_SCREEN(Parent)) {
      if (Ext(WM, MsgPort) && (Msg = New(msg)(Fn_msg, msg_map, 0))) {
        Msg->Event.EventMap.W = W;
        Msg->Event.EventMap.Code = 0;
        Msg->Event.EventMap.Screen = (screen)Parent;
        W->MapQueueMsg = Msg;
        SendMsg(Ext(WM, MsgPort), Msg);
      } else
        Act(MapTopReal, W)(W, (screen)Parent);
    } else if (IS_WIDGET(Parent)) {
      if (W->Up == TW_MAXDAT) {
        W->Left = Parent->XLogic;
        W->Up = Parent->YLogic;
      }
      InsertFirst(W, W, Parent);
      W->Parent = Parent;

      DrawAreaWidget(W);

      if (W->Attrib & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        IncMouseMotionN();

      if (W->MapUnMapHook)
        W->MapUnMapHook(W);
    }
  }
}

static void MapTopRealWidget(widget W, screen Screen) {
  widget OldW;

  if (Screen && !W->Parent && (!IS_WINDOW(W) || ((window)W)->Menu)) {
    if (W->MapQueueMsg)
      /*
       * let the upper layer do this:
       * W->MapQueueMsg->Delete();
       */
      W->MapQueueMsg = (msg)0;

    if (W->Up == TW_MAXDAT) {
      W->Left = Screen->XLogic;
      W->Up = Max2(Screen->YLimit + 1, 0) + Screen->YLogic;
    } else {
      W->Left += Screen->XLogic;
      W->Up += Screen->YLogic;
    }

    InsertFirst(W, W, (widget)Screen);
    W->Parent = (widget)Screen;

    /* top-level widgets must be visible */
    W->Flags &= ~WINDOWFL_NOTVISIBLE;

    if (W->Attrib & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
      IncMouseMotionN();

    if (Screen == All->FirstScreen) {
      OldW = Act(KbdFocus, W)(W);
      if (OldW && IS_WINDOW(OldW))
        DrawBorderWindow((window)OldW, BORDER_ANY);
      UpdateCursor();
    }
    if (IS_WINDOW(W))
      DrawAreaWindow2((window)W);
    else
      DrawAreaWidget(W);
    if (!(W->Flags & WINDOWFL_MENU))
      Act(DrawMenu, Screen)(Screen, 0, TW_MAXDAT);

    if (W->MapUnMapHook)
      W->MapUnMapHook(W);

    if (Screen->FnHookW)
      Screen->FnHookW(Screen->HookW);
  }
}

static void UnMapWidget(widget W) {
  widget Parent;
  window Next;
  screen Screen;
  byte wasFocus;

  if (W && (Parent = W->Parent)) {
    if (IS_SCREEN(Parent)) {
      if ((Screen = (screen)Parent) == All->FirstScreen && W == (widget)Screen->MenuWindow) {
        /*
         * ! DANGER !
         * Trying to UnMap() the menu owner.
         * shutdown the menu first!
         */
        CloseMenu();
      }

      if (W->Attrib & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        DecMouseMotionN();

      if (Screen->ClickWindow == (window)W)
        Screen->ClickWindow = NULL;

      if ((wasFocus = W == Screen->FocusW)) {
        if (W->Flags & WINDOWFL_MENU)
          Next = Screen->MenuWindow;
        else {
          if ((widget)W == Screen->FirstW)
            Next = (window)W->Next;
          else
            Next = (window)Screen->FirstW;

          while (Next && !IS_WINDOW(Next))
            Next = (window)Next->Next;
        }
      }

      W->Remove();
      if (IS_WINDOW(W))
        DrawAreaWindow2((window)W);
      else
        DrawAreaWidget(W);

      if (IS_SCREEN(Parent)) {
        W->Left = 0;
        W->Up = TW_MAXDAT;
      }
      W->Parent = (widget)0;

      if (wasFocus) {
        if (Screen == All->FirstScreen) {
          /*
           * in case the user was dragging this window...
           */
          if ((All->State & state_any) < state_menu)
            All->State &= ~state_any;

          if (Next) {
            (void)Act(KbdFocus, Next)(Next);
            DrawBorderWindow(Next, BORDER_ANY);
          } else
            Do(KbdFocus, window)((window)0);
          if (!(W->Flags & WINDOWFL_MENU))
            Act(DrawMenu, Screen)(Screen, 0, TW_MAXDAT);
          UpdateCursor();
        } else
          Screen->FocusW = (widget)Next;
      }

      if (W->MapUnMapHook)
        W->MapUnMapHook(W);

      if (Screen->FnHookW)
        Screen->FnHookW(Screen->HookW);

    } else {
      /* UnMap() a sub-window */
      if (W == Parent->SelectW)
        Parent->SelectW = (widget)0;

      W->Remove();
      DrawAreaWidget(W);
      W->Parent = (widget)0;

      if (W->Attrib & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        DecMouseMotionN();

      if (W->MapUnMapHook)
        W->MapUnMapHook(W);
    }
  } else if (W->MapQueueMsg) {
    /* the window was still waiting to be mapped! */
    W->MapQueueMsg->Delete();
    W->MapQueueMsg = (msg)0;
  }
}

static void RaiseW(widget W) {
  RaiseWidget(W, tfalse);
}

static void LowerW(widget W) {
  LowerWidget(W, tfalse);
}

static void SetXYWidget(widget W, dat X, dat Y) {
  widget Prev, Next;

  if (W->Parent) {
    Prev = W->Prev;
    Next = W->Next;
    W->Remove();
    DrawAreaWidget(W);
  }
  W->Left = X;
  W->Up = Y;
  if (W->Parent) {
    InsertMiddle(W, W, W->Parent, Prev, Next);
    DrawAreaWidget(W);
  }
}

static void OwnWidget(widget Widget, msgport Owner) {
  if (!Widget->Owner) {
    if ((Widget->O_Next = Owner->FirstW))
      Owner->FirstW->O_Prev = Widget;
    else
      Owner->LastW = Widget;

    Widget->O_Prev = (widget)0;
    Owner->FirstW = Widget;
    Widget->Owner = Owner;
  }
}

static void DisOwnWidget(widget W) {
  msgport Owner;
  if ((Owner = W->Owner)) {
    if (W->O_Prev)
      W->O_Prev->O_Next = W->O_Next;
    else if (Owner->FirstW == W)
      Owner->FirstW = W->O_Next;

    if (W->O_Next)
      W->O_Next->O_Prev = W->O_Prev;
    else if (Owner->LastW == W)
      Owner->LastW = W->O_Prev;

    W->O_Prev = W->O_Next = (widget)0;
    W->Owner = (msgport)0;
  }
}

static void RecursiveDeleteWidget(widget W, msgport maybeOwner) {
  while (W->FirstW)
    Act(RecursiveDelete, W->FirstW)(W->FirstW, maybeOwner);

  if (W->Owner == maybeOwner)
    W->Delete();
  else
    W->UnMap();
}

static byte InstallHookWidget(widget W, fn_hook Hook, fn_hook *WhereHook) {
  if (W && !W->Hook && !W->WhereHook && Hook && WhereHook && !WhereHook[0] && !WhereHook[1]) {

    W->Hook = WhereHook[0] = Hook;
    W->WhereHook = WhereHook;
    WhereHook[1] = (fn_hook)W;
    return ttrue;
  }
  return tfalse;
}

static void RemoveHookWidget(widget W, fn_hook Hook, fn_hook *WhereHook) {
  if (W && Hook && W->Hook == Hook && WhereHook && W->WhereHook == WhereHook &&
      WhereHook[0] == Hook && WhereHook[1] == (void *)W) {

    W->Hook = *WhereHook = (fn_hook)0;
    W->WhereHook = (fn_hook *)0;
    WhereHook[1] = (fn_hook)0;
  }
}

static struct s_fn_widget _FnWidget = {
    widget_magic,
    sizeof(struct s_widget),
    InsertWidget,
    RemoveWidget,
    DeleteWidget,
    ChangeFieldWidget,
    &_FnObj,
    DrawSelfWidget, /* exported by draw.c */
    FindWidgetAt,   /* exported by draw.c */
    FindGadgetByCode,
    SetXYWidget,
    SetFillWidget,
    FocusWidget,
    TtyKbdFocus,
    MapWidget,
    UnMapWidget,
    MapTopRealWidget,
    RaiseW,
    LowerW,
    OwnWidget,
    DisOwnWidget,
    RecursiveDeleteWidget,
    ExposeWidget2, /* exported by resize.c */
    InstallHookWidget,
    RemoveHookWidget,
};

/* gadget */

static void DeleteGadget(gadget G) {
  byte i;

  Act(UnMap, G)(G);
  if (G_USE(G, USETEXT)) {
    for (i = 0; i < 4; i++) {
      if (G->USE.T.Text[i])
        FreeMem(G->USE.T.Text[i]);
      if (G->USE.T.Color[i])
        FreeMem(G->USE.T.Color[i]);
    }
  }
  G->Fn->Fn_Widget->Delete((widget)G);
}

static void ChangeFieldGadget(gadget G, udat field, uldat CLEARMask, uldat XORMask) {
  uldat i, mask;

  if (G)
    switch (field) {
    case TWS_gadget_ColText:
    case TWS_gadget_ColSelect:
    case TWS_gadget_ColDisabled:
    case TWS_gadget_ColSelectDisabled:
    case TWS_gadget_Code:
      break;
    case TWS_gadget_Flags:
      mask = GADGETFL_DISABLED | GADGETFL_TEXT_DEFCOL | GADGETFL_PRESSED | GADGETFL_TOGGLE;
      CLEARMask &= mask;
      XORMask &= mask;
      i = (G->Flags & ~CLEARMask) ^ XORMask;
      if ((i & mask) != (G->Flags & mask)) {
        if ((i & GADGETFL_PRESSED) != (G->Flags & GADGETFL_PRESSED)) {
          if (i & GADGETFL_PRESSED)
            PressGadget(G);
          else
            UnPressGadget(G, ttrue);
        }
        mask = GADGETFL_DISABLED | GADGETFL_TEXT_DEFCOL;
        if ((i & mask) != (G->Flags & mask)) {
          G->Flags = i;
          DrawAreaWidget((widget)G);
        } else
          G->Flags = i;
      }
      break;
    default:
      G->Fn->Fn_Widget->ChangeField((widget)G, field, CLEARMask, XORMask);
      break;
    }
}

static gadget CreateEmptyButton(fn_gadget Fn_Gadget, msgport Owner, dat XWidth, dat YWidth,
                                tcolor BgCol) {
  gadget G;
  ldat Size;
  byte i;
  dat j, k;
#define _FULL T_UTF_32_FULL_BLOCK
#define _LOWER T_UTF_32_LOWER_HALF_BLOCK
#define _UPPER T_UTF_32_UPPER_HALF_BLOCK

  if ((G = (gadget)New(widget)((fn_widget)Fn_Gadget, Owner, ++XWidth, ++YWidth, 0,
                               GADGETFL_USETEXT | GADGETFL_BUTTON, 0, 0, (tcell)0))) {

    Size = (ldat)XWidth * YWidth;

    for (i = 0; i < 4; i++)
      G->USE.T.Text[i] = NULL, G->USE.T.Color[i] = NULL;

    for (i = 0; i < 4; i++)
      if (!(G->USE.T.Text[i] = (trune *)AllocMem(Size * sizeof(trune))) ||
          !(G->USE.T.Color[i] = (tcolor *)AllocMem(Size * sizeof(tcolor)))) {

        Act(Delete, G)(G);
        return (gadget)0;
      }

    Size = (ldat)--XWidth * --YWidth;
    BgCol &= TCOL(0, tmaxcol);

    for (i = 0; i < 4; i++) {
      for (j = k = (dat)0; j < YWidth; j++, k += XWidth + 1) {
        G->USE.T.Text[i][k + (i & 1 ? 0 : XWidth)] = i & 1 ? ' ' : k ? _FULL : _LOWER;
        G->USE.T.Color[i][k + (i & 1 ? 0 : XWidth)] = BgCol;
      }
      G->USE.T.Text[i][k] = ' ';
      for (j = (dat)0; j < XWidth; j++)
        G->USE.T.Text[i][k + 1 + j] = i & 1 ? ' ' : _UPPER;
#if TW_SIZEOF_TCOLOR == 1
      memset((void *)(G->USE.T.Color[i] + k), BgCol, XWidth + 1);
#else
      for (j = (dat)0; j <= XWidth; j++)
        G->USE.T.Color[i][k + j] = BgCol;
#endif
    }

    G->G_Prev = G->G_Next = (gadget)0;
    G->Group = (ggroup)0;
  }
  return G;
#undef _FULL
#undef _UPPER
#undef _LOWER
}

byte FillButton(gadget G, widget Parent, udat Code, dat Left, dat Up, udat Flags, const char *Text,
                tcolor Color, tcolor ColorDisabled) {
  dat i, j, XWidth, YWidth;
  const char *T;

  if (Code >= COD_RESERVED)
    return tfalse;

  G->Code = Code;
  G->Left = Left;
  G->Up = Up;
  G->Flags = (Flags & ~GADGETFL_USEANY) | GADGETFL_USETEXT | GADGETFL_BUTTON;
  XWidth = G->XWidth;
  YWidth = G->YWidth;

  T = Text;
  for (j = (dat)0; j < (YWidth - (dat)1) * XWidth; j += XWidth) {
    for (i = (dat)0; i < XWidth - (dat)1; i++) {
      G->USE.T.Text[0][i + j] = G->USE.T.Text[1][i + j + 1] = G->USE.T.Text[2][i + j] =
          G->USE.T.Text[3][i + j + 1] = Tutf_CP437_to_UTF_32[(byte) * (T++)];

      G->USE.T.Color[0][i + j] = G->USE.T.Color[1][i + j + 1] = Color;
      G->USE.T.Color[2][i + j] = G->USE.T.Color[3][i + j + 1] = ColorDisabled;
    }
  }
  if (Parent)
    Act(Map, G)(G, Parent);

  return ttrue;
}

static gadget CreateButton(fn_gadget Fn_Gadget, widget Parent, dat XWidth, dat YWidth,
                           const char *Text, uldat Flags, udat Code, tcolor BgCol, tcolor Col,
                           tcolor ColDisabled, dat Left, dat Up) {
  gadget G;
  if (Parent &&
      (G = Fn_Gadget->CreateEmptyButton(Fn_Gadget, Parent->Owner, XWidth, YWidth, BgCol))) {
    if (Act(FillButton, G)(G, Parent, Code, Left, Up, Flags, Text, Col, ColDisabled))
      return G;
    Act(Delete, G)(G);
    G = (gadget)0;
  }
  return G;
}

static struct s_fn_gadget _FnGadget = {
    gadget_magic,                                           //
    sizeof(struct s_gadget),                                //
    (void (*)(gadget, widget, widget, widget))InsertWidget, //
    (void (*)(gadget))RemoveWidget, DeleteGadget, ChangeFieldGadget,
    /* widget */
    &_FnObj, DrawSelfGadget,                    /* exported by draw.c */
    (widget (*)(gadget, dat, dat))FindWidgetAt, /* exported by draw.c */
    (gadget (*)(gadget, udat))FindGadgetByCode, (void (*)(gadget, dat, dat))SetXYWidget,
    (void (*)(gadget, tcell))SetFillWidget, (widget(*)(gadget))FocusWidget,
    (widget(*)(gadget))TtyKbdFocus, (void (*)(gadget, widget))MapWidget,
    (void (*)(gadget))UnMapWidget, (void (*)(gadget, screen))MapTopRealWidget,
    (void (*)(gadget))RaiseW, (void (*)(gadget))LowerW, (void (*)(gadget, msgport))OwnWidget,
    (void (*)(gadget))DisOwnWidget, (void (*)(gadget, msgport))RecursiveDeleteWidget,
    (void (*)(gadget, dat, dat, dat, dat, const char *, const trune *,
              const tcell *))ExposeWidget2, /* exported by resize.c */
    (byte (*)(gadget, fn_hook, void (**)(widget)))InstallHookWidget,
    (void (*)(gadget, fn_hook, void (**)(widget)))RemoveHookWidget,
    /* gadget */
    &_FnWidget, CreateEmptyButton, FillButton, CreateButton,
    WriteTextsGadget,  /* exported by resize.c */
    WriteTRunesGadget, /* exported by resize.c */
};

/* window */

static void DeleteWindow(window W) {
  Act(UnMap, W)(W);
  if (W->Name)
    FreeMem(W->Name);
  if (W->ColName)
    FreeMem(W->ColName);
  if (W_USE(W, USECONTENTS)) {
    if (W->USE.C.TtyData)
      FreeMem(W->USE.C.TtyData);
    if (W->USE.C.Contents)
      FreeMem(W->USE.C.Contents);
  } else if (W_USE(W, USEROWS))
    DeleteList(W->USE.R.FirstRow);

  W->Fn->Fn_Widget->Delete((widget)W);
}

static void ChangeFieldWindow(window W, udat field, uldat CLEARMask, uldat XORMask) {
  uldat i, mask;

  if (W)
    switch (field) {
    case TWS_window_CurX:
    case TWS_window_CurY:
    case TWS_window_XstSel:
    case TWS_window_YstSel:
    case TWS_window_XendSel:
    case TWS_window_YendSel:
      /* FIXME: finish this */
      break;
    case TWS_window_ColGadgets:
    case TWS_window_ColArrows:
    case TWS_window_ColBars:
    case TWS_window_ColTabs:
    case TWS_window_ColBorder:
    case TWS_window_ColText:
    case TWS_window_ColSelect:
    case TWS_window_ColDisabled:
    case TWS_window_ColSelectDisabled: {
      tcolor *C = NULL;
      switch (field) {
      case TWS_window_ColGadgets:
        C = &W->ColGadgets;
        break;
      case TWS_window_ColArrows:
        C = &W->ColArrows;
        break;
      case TWS_window_ColBars:
        C = &W->ColBars;
        break;
      case TWS_window_ColTabs:
        C = &W->ColTabs;
        break;
      case TWS_window_ColBorder:
        C = &W->ColBorder;
        break;
      case TWS_window_ColText:
        C = &W->ColText;
        break;
      case TWS_window_ColSelect:
        C = &W->ColSelect;
        break;
      case TWS_window_ColDisabled:
        C = &W->ColDisabled;
        break;
      case TWS_window_ColSelectDisabled:
        C = &W->ColSelectDisabled;
        break;
      default:
        break;
      }
      i = (*C & ~CLEARMask) ^ XORMask;
      if (i != *C) {
        *C = i;
        /* FIXME: this is an overkill */
        DrawAreaWidget((widget)W);
      }
    } break;
    case TWS_window_Flags:
      mask = WINDOWFL_CURSOR_ON;
      CLEARMask &= mask;
      XORMask &= mask;
      i = (W->Flags & ~CLEARMask) ^ XORMask;
      if ((i & mask) != (W->Flags & mask)) {
        W->Flags = i;
        if (ContainsCursor((widget)W))
          UpdateCursor();
      }
      break;
    case TWS_window_Attrib:
      mask = WINDOW_WANT_KEYS | WINDOW_WANT_MOUSE | WINDOW_WANT_CHANGES | WINDOW_AUTO_FOCUS |
             WINDOW_DRAG | WINDOW_RESIZE | WINDOW_CLOSE | WINDOW_ROLLED_UP | WINDOW_X_BAR |
             WINDOW_Y_BAR | WINDOW_AUTO_KEYS | WINDOW_WANT_MOUSE_MOTION;
      CLEARMask &= mask;
      XORMask &= mask;
      i = (W->Attrib & ~CLEARMask) ^ XORMask;
      if ((i & mask) != (W->Attrib & mask)) {
        if ((i & WINDOW_ROLLED_UP) != (W->Attrib & WINDOW_ROLLED_UP))
          RollUpWindow(W, !!(i & WINDOW_ROLLED_UP));
        if ((i & (WINDOW_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS)) !=
                (W->Attrib & (WINDOW_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS)) &&
            W->Parent) {
          if (i & (WINDOW_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
            IncMouseMotionN();
          else
            DecMouseMotionN();
        }
        mask = WINDOW_RESIZE | WINDOW_CLOSE | WINDOW_X_BAR | WINDOW_Y_BAR;
        if ((i & mask) != (W->Attrib & mask) && W->Parent) {
          W->Attrib = i;
          DrawBorderWindow(W, BORDER_ANY);
        } else
          W->Attrib = i;
      }
      break;
    case TWS_window_State:
    case TWS_window_CursorType:
    case TWS_window_MinXWidth:
    case TWS_window_MinYWidth:
    case TWS_window_MaxXWidth:
    case TWS_window_MaxYWidth:
    case TWS_window_WLogic:
    case TWS_window_HLogic:
      break;
    default:
      W->Fn->Fn_Widget->ChangeField((widget)W, field, CLEARMask, XORMask);
      break;
    }
}

static void SetTitleWindow(window W, dat titlelen, char *title) {
  widget P;

  if (W->Name)
    FreeMem(W->Name);

  W->NameLen = titlelen;
  W->Name = title;

#if 1
  /*
   * do not allow changing window borders just because
   * some untrusted application set a new title
   */
  DrawBorderWindow(W, BORDER_UP);
#else
  /* user may have title-dependent borders in ~/.twinrc, honour them: */
  Win->BorderPattern[0] = Win->BorderPattern[1] = NULL;
  DrawBorderWindow(W, BORDER_ANY);
#endif

  if ((P = W->Parent) && IS_SCREEN(P)) {
    /* need to update window list with new name ? */
    if (((screen)P)->FnHookW)
      ((screen)P)->FnHookW(((screen)P)->HookW);
  }
}

static void SetColTextWindow(window W, tcolor ColText) {
  W->ColText = ColText;
}

static void SetColorsWindow(window W, udat Bitmap, tcolor ColGadgets, tcolor ColArrows,
                            tcolor ColBars, tcolor ColTabs, tcolor ColBorder, tcolor ColText,
                            tcolor ColSelect, tcolor ColDisabled, tcolor ColSelectDisabled) {
  if (Bitmap & 1)
    W->ColGadgets = ColGadgets;
  if (Bitmap & 2)
    W->ColArrows = ColArrows;
  if (Bitmap & 4)
    W->ColBars = ColBars;
  if (Bitmap & 8)
    W->ColTabs = ColTabs;
  if (Bitmap & 0x10)
    W->ColBorder = ColBorder;
  if (Bitmap & 0x20) {
    W->ColText = ColText;
    if (W_USE(W, USECONTENTS))
      W->USE.C.TtyData->Color = ColText;
  }
  if (Bitmap & 0x40)
    W->ColSelect = ColSelect;
  if (Bitmap & 0x80)
    W->ColDisabled = ColDisabled;
  if (Bitmap & 0x100)
    W->ColSelectDisabled = ColSelectDisabled;
  if (W->Parent)
    DrawBorderWindow(W, BORDER_ANY);
}

static void SetXYWindow(window W, dat X, dat Y) {
  widget Prev, Next;

  if (W->Parent) {
    Prev = W->Prev;
    Next = W->Next;
    W->Remove();
    DrawAreaWindow2(W);
  }
  W->Left = X;
  W->Up = Y;
  if (W->Parent) {
    if (IS_SCREEN(W->Parent)) {
      W->Left += W->Parent->XLogic;
      W->Up += W->Parent->YLogic;
    }
    InsertMiddle(W, W, W->Parent, Prev, Next);
    DrawAreaWindow2(W);
  }
}

static void ConfigureWindow(window W, byte Bitmap, dat Left, dat Up, dat MinXWidth, dat MinYWidth,
                            dat MaxXWidth, dat MaxYWidth) {
  widget Prev, Next;
  dat HasBorder = 2 * !(W->Flags & WINDOWFL_BORDERLESS);

  if (W->Parent) {
    Prev = W->Prev;
    Next = W->Next;
    W->Remove();
    DrawAreaWindow2(W);
  }

  if (Bitmap & 1) {
    W->Left = Left;
    if (W->Parent && IS_SCREEN(W->Parent))
      W->Left += W->Parent->XLogic;
  }
  if (Bitmap & 2) {
    W->Up = Up;
    if (W->Parent && IS_SCREEN(W->Parent))
      W->Up += W->Parent->YLogic;
  }

  if (Bitmap & 4) {
    if (MinXWidth <= TW_MAXDAT - HasBorder)
      MinXWidth = Max2(MinXWidth, MinXWidth + HasBorder);
    W->MinXWidth = MinXWidth;
    W->XWidth = Max2(MinXWidth, W->XWidth);
  }
  if (Bitmap & 8) {
    if (MinYWidth <= TW_MAXDAT - HasBorder)
      MinYWidth = Max2(MinYWidth, MinYWidth + HasBorder);
    W->MinYWidth = MinYWidth;
    W->YWidth = Max2(MinYWidth, W->YWidth);
  }
  if (Bitmap & 0x10) {
    if (MaxXWidth <= TW_MAXDAT - HasBorder)
      MaxXWidth = Max2(MaxXWidth, MaxXWidth + HasBorder);
    W->MaxXWidth = Max2(W->MinXWidth, MaxXWidth);
    W->XWidth = Min2(MaxXWidth, W->XWidth);
  }
  if (Bitmap & 0x20) {
    if (MaxYWidth <= TW_MAXDAT - HasBorder)
      MaxYWidth = Max2(MaxYWidth, MaxYWidth + HasBorder);
    W->MaxYWidth = Max2(W->MinYWidth, MaxYWidth);
    W->YWidth = Min2(MaxYWidth, W->YWidth);
  }
  if (W->Parent) {
    InsertMiddle(W, W, W->Parent, Prev, Next);
    DrawAreaWindow2(W);
  }
}

static void GotoXYWindow(window Window, ldat X, ldat Y) {
  if (W_USE(Window, USECONTENTS)) {
    ttydata *TT = Window->USE.C.TtyData;

    X = Max2(X, 0);
    Y = Max2(Y, 0);
    if (X >= TT->SizeX)
      X = TT->SizeX - 1;
    if (Y >= TT->SizeY)
      Y = TT->SizeY - 1;
    TT->X = X;
    TT->Y = Y;
    TT->Pos = TT->Start + X + (ldat)Y * TT->SizeX;
    if (TT->Pos >= TT->Split)
      TT->Pos -= TT->Split - Window->USE.C.Contents;
    Y += Window->HLogic - TT->SizeY;
  }
  Window->CurX = X;
  Window->CurY = Y;
  if (ContainsCursor((widget)Window))
    UpdateCursor();
}

window Create4MenuWindow(fn_window Fn_Window, menu Menu) {
  window Window = (window)0;
  if (Menu && (Window = New(window)(Fn_Window, Menu->MsgPort, 0, NULL, (tcolor *)0, Menu,
                                    TCOL(tblack, twhite), NOCURSOR, WINDOW_AUTO_KEYS,
                                    WINDOWFL_MENU | WINDOWFL_USEROWS | WINDOWFL_ROWS_DEFCOL |
                                        WINDOWFL_ROWS_SELCURRENT,
                                    MIN_XWIN, MIN_YWIN, 0))) {

    Act(SetColors, Window)(Window, 0x1FF, TCOL(0, 0), TCOL(0, 0), TCOL(0, 0), TCOL(0, 0),
                           TCOL(thigh | twhite, twhite), TCOL(tblack, twhite), TCOL(tblack, tgreen),
                           TCOL(thigh | tblack, twhite), TCOL(thigh | tblack, tblack));
    Act(Configure, Window)(Window, 0x3F, 0, 1, MIN_XWIN, MIN_YWIN, TW_MAXDAT, TW_MAXDAT);
  }
  return Window;
}

byte FakeWriteAscii(window Window, uldat Len, const char *Ascii) {
  if (DlLoad(TermSo) && Window->Fn->TtyWriteAscii != FakeWriteAscii)
    return Act(TtyWriteAscii, Window)(Window, Len, Ascii);
  return tfalse;
}

byte FakeWriteString(window Window, uldat Len, const char *String) {
  if (DlLoad(TermSo) && Window->Fn->TtyWriteString != FakeWriteString)
    return Act(TtyWriteString, Window)(Window, Len, String);
  return tfalse;
}

byte FakeWriteTRune(window Window, uldat Len, const trune *TRune) {
  if (DlLoad(TermSo) && Window->Fn->TtyWriteTRune != FakeWriteTRune)
    return Act(TtyWriteTRune, Window)(Window, Len, TRune);
  return tfalse;
}

byte FakeWriteTCell(window Window, dat x, dat y, uldat Len, const tcell *Attr) {
  if (DlLoad(TermSo) && Window->Fn->TtyWriteTCell != FakeWriteTCell)
    return Act(TtyWriteTCell, Window)(Window, x, y, Len, Attr);
  return tfalse;
}

window FakeOpenTerm(const char *arg0, const char *const *argv) {
  if (DlLoad(TermSo) && Ext(Term, Open) != FakeOpenTerm)
    return Ext(Term, Open)(arg0, argv);
  return NULL;
}

tpos FakeFindBorderWindow(window W, dat u, dat v, byte Border, tcell *PtrAttr) {
  byte Horiz, Vert;

  Horiz = u ? u + 1 == W->XWidth ? (byte)2 : (byte)1 : (byte)0;
  Vert = v ? v + 1 == W->YWidth ? (byte)2 : (byte)1 : (byte)0;

  if (*PtrAttr)
    *PtrAttr = TCELL(W->ColBorder, StdBorder[Border][Vert * 3 + Horiz]);

  return v ? POS_ROOT : POS_TITLE;
}

static row FindRow(window Window, ldat Row) {
  row CurrRow, ElPossib[4];
  byte Index;
  ldat k, ElNumRows[4], ElDist[4];

  ElPossib[0] = Window->USE.R.RowOne;
  ElPossib[1] = Window->USE.R.RowSplit;
  ElPossib[2] = Window->USE.R.FirstRow;
  ElPossib[3] = Window->USE.R.LastRow;
  ElNumRows[0] = Window->USE.R.NumRowOne;
  ElNumRows[1] = Window->USE.R.NumRowSplit;
  ElNumRows[2] = (ldat)0;
  ElNumRows[3] = Window->HLogic - (ldat)1;
  ElDist[0] = (ElPossib[0] && ElNumRows[0] ? Abs(ElNumRows[0] - Row) : TW_MAXLDAT);
  ElDist[1] = (ElPossib[1] && ElNumRows[1] ? Abs(ElNumRows[1] - Row) : TW_MAXLDAT);
  ElDist[2] = Row;
  ElDist[3] = Abs(ElNumRows[3] - Row);

  Index = Minimum((byte)4, ElDist);
  CurrRow = ElPossib[Index];
  k = ElNumRows[Index];

  if (CurrRow) {
    if (k < Row)
      while (k < Row && (CurrRow = CurrRow->Next))
        k++;
    else if (k > Row)
      while (k > Row && (CurrRow = CurrRow->Prev))
        k--;
  }
  if (CurrRow && IS_MENUITEM(CurrRow))
    ((menuitem)CurrRow)->WCurY = Row;
  return CurrRow;
}

static row FindRowByCode(window Window, udat Code, ldat *NumRow) {
  row Row;
  ldat Num = (ldat)0;

  if ((Row = Window->USE.R.FirstRow))
    while (Row && Row->Code != Code) {
      Row = Row->Next;
      Num++;
    }
  if (Row && NumRow)
    *NumRow = Num;

  return Row;
}

static struct s_fn_window _FnWindow = {
    window_magic,
    sizeof(struct s_window),
    (void (*)(window, widget, widget, widget))InsertWidget,
    (void (*)(window))RemoveWidget,
    DeleteWindow,
    ChangeFieldWindow,
    /* widget */
    &_FnObj,
    DrawSelfWindow,
    (widget(*)(window, dat, dat))FindWidgetAt,
    (gadget(*)(window, udat))FindGadgetByCode,
    SetXYWindow,
    (void (*)(window, tcell))SetFillWidget,
    (widget(*)(window))FocusWidget,
    (widget(*)(window))TtyKbdFocus,
    (void (*)(window, widget))MapWidget,
    (void (*)(window))UnMapWidget,
    (void (*)(window, screen))MapTopRealWidget,
    (void (*)(window))RaiseW,
    (void (*)(window))LowerW,
    (void (*)(window, msgport))OwnWidget,
    (void (*)(window))DisOwnWidget,
    (void (*)(window, msgport))RecursiveDeleteWidget,
    (void (*)(window, dat, dat, dat, dat, const char *, const trune *,
              const tcell *))ExposeWindow2, /* exported by resize.c */
    (byte (*)(window, fn_hook, void (**)(widget)))InstallHookWidget,
    (void (*)(window, fn_hook, void (**)(widget)))RemoveHookWidget,
    /* window */
    &_FnWidget,
    FakeWriteAscii,
    FakeWriteString,
    FakeWriteTRune,
    FakeWriteTCell,
    RowWriteAscii, /* exported by resize.c */
    RowWriteAscii,
    RowWriteTRune,
    (byte(*)(window, dat, dat, uldat, const tcell *))AlwaysFalse,

    GotoXYWindow,
    SetTitleWindow,
    SetColTextWindow,
    SetColorsWindow,
    ConfigureWindow,
    Create4MenuWindow,
    FakeFindBorderWindow,
    FindRow,
    FindRowByCode,
};

/* screen */

static screen CreateSimpleScreen(fn_screen Fn_Screen, dat NameLen, const char *Name, tcell Bg) {
  return New(screen)(Fn_Screen, NameLen, Name, 1, 1, &Bg);
}

static void BgImageScreen(screen Screen, dat BgWidth, dat BgHeight, const tcell *Bg) {
  size_t size;

  if (Screen && S_USE(Screen, USEBG) && Bg && (size = (size_t)BgWidth * BgHeight * sizeof(tcell)) &&
      (Screen->USE.B.Bg = (tcell *)ReAllocMem(Screen->USE.B.Bg, size))) {

    Screen->USE.B.BgWidth = BgWidth;
    Screen->USE.B.BgHeight = BgHeight;
    CopyMem(Bg, Screen->USE.B.Bg, size);
    DrawArea2((screen)0, (widget)0, (widget)0, 0, Screen->YLimit + 1, TW_MAXDAT, TW_MAXDAT, tfalse);
  }
}

static void InsertScreen(screen Screen, all Parent, screen Prev, screen Next) {
  if (!Screen->All && Parent) {
    InsertGeneric((obj)Screen, (obj_parent)&Parent->FirstScreen, (obj)Prev, (obj)Next, (ldat *)0);
    Screen->All = Parent;
  }
}

static void RemoveScreen(screen Screen) {
  if (Screen->All) {
    RemoveGeneric((obj)Screen, (obj_parent)&Screen->All->FirstScreen, (ldat *)0);
    Screen->All = (all)0;
  }
}

static void DeleteScreen(screen Screen) {
  while (Screen->FirstW)
    Act(UnMap, Screen->FirstW)(Screen->FirstW);

  Screen->Remove();

  if (S_USE(Screen, USEBG) && Screen->USE.B.Bg) {
    FreeMem(Screen->USE.B.Bg);
    Screen->USE.B.Bg = NULL;
  }

  Screen->Fn->Fn_Widget->Delete((widget)Screen);
}

static void ChangeFieldScreen(screen S, udat field, uldat CLEARMask, uldat XORMask) {
  if (S)
    switch (field) {
    default:
      S->Fn->Fn_Widget->ChangeField((widget)S, field, CLEARMask, XORMask);
      break;
    }
}

static void SetXYScreen(screen Screen, dat X, dat Y) {
  if (Screen == All->FirstScreen) {
    Y = Max2(Y, -1);
    Y = Min2(Y, All->DisplayHeight - 1);
    ResizeFirstScreen(Y - Screen->YLimit);
  }
}

static menu FindMenuScreen(screen Screen) {
  if (Screen) {
    if (Screen->MenuWindow && IS_WINDOW(Screen->MenuWindow))
      /* menu activated from Screen->MenuWindow, return its menu */
      return Screen->MenuWindow->Menu;

    /* no window activated the menu... either the menu is inactive
     * or it is activated from the builtin menu */

    if (Screen->FocusW && IS_WINDOW(Screen->FocusW) &&
        ((window)Screen->FocusW)->Menu != All->CommonMenu)
      /* menu inactive... return the focus window's one */
      return ((window)Screen->FocusW)->Menu;

    /* last case: menu activated from builtin menu */
    return All->BuiltinMenu;
  }
  return (menu)0;
}

static screen FindScreen(dat j) {
  screen CurrScreen;
  byte VirtScrFound = tfalse;

  CurrScreen = All->FirstScreen;
  while (CurrScreen && !(VirtScrFound = (j >= (dat)CurrScreen->YLimit)))
    CurrScreen = CurrScreen->Next;

  if (VirtScrFound)
    return CurrScreen;

  return (screen)0;
}

static widget FocusScreen(screen tScreen) {
  screen Screen = All->FirstScreen;
  if (tScreen && Screen != tScreen) {
    MoveFirst(Screen, All, tScreen);
    DrawArea2((screen)0, (widget)0, (widget)0, 0, Min2(Screen->YLimit, tScreen->YLimit), TW_MAXDAT,
              TW_MAXDAT, tfalse);
    UpdateCursor();
  }
  return (widget)Screen;
}

static void ActivateMenuScreen(screen Screen, menuitem Item, byte ByMouse) {

  if ((All->State & state_any) != state_default)
    return;

  if (Screen && Screen != All->FirstScreen)
    Act(Focus, Screen)(Screen);

  SetMenuState(Item, ByMouse);
}

static void DeActivateMenuScreen(screen Screen) {
  if (Screen == All->FirstScreen && (All->State & state_any) == state_menu)
    CloseMenu();
}

static struct s_fn_screen _FnScreen = {
    screen_magic,
    sizeof(struct s_screen),
    InsertScreen,
    RemoveScreen,
    DeleteScreen,
    ChangeFieldScreen,
    /* widget */
    &_FnObj,
    DrawSelfScreen,
    (widget(*)(screen, dat, dat))FindWidgetAt,
    (gadget(*)(screen, udat))FindGadgetByCode,
    SetXYScreen,
    (void (*)(screen, tcell))SetFillWidget,
    FocusScreen,
    (widget(*)(screen))NoOp,        /* KbdFocusWidget */
    (void (*)(screen, widget))NoOp, /* MapWidget */
    (void (*)(screen))NoOp,         /* UnMapWidget */
    (void (*)(screen, screen))NoOp, /* MapTopRealWidget */
    (void (*)(screen))RaiseW,
    (void (*)(screen))LowerW,
    (void (*)(screen, msgport))OwnWidget,
    (void (*)(screen))DisOwnWidget,
    (void (*)(screen, msgport))RecursiveDeleteWidget,
    (void (*)(screen, dat, dat, dat, dat, const char *, const trune *,
              const tcell *))ExposeWidget2, /* exported by resize.c */
    (byte (*)(screen, fn_hook, void (**)(widget)))InstallHookWidget,
    (void (*)(screen, fn_hook, void (**)(widget)))RemoveHookWidget,
    /* screen */
    &_FnWidget,
    FindMenuScreen,
    FindScreen,
    CreateSimpleScreen,
    BgImageScreen,
    DrawMenuScreen,
    ActivateMenuScreen,
    DeActivateMenuScreen,
};

/* ggroup */

static void InsertGroup(ggroup Group, msgport MsgPort, ggroup Prev, ggroup Next) {
  if (!Group->MsgPort && MsgPort) {
    InsertGeneric((obj)Group, (obj_parent)&MsgPort->FirstGroup, (obj)Prev, (obj)Next, (ldat *)0);
    Group->MsgPort = MsgPort;
  }
}

static void RemoveGroup(ggroup Group) {
  if (Group->MsgPort) {
    RemoveGeneric((obj)Group, (obj_parent)&Group->MsgPort->FirstGroup, (ldat *)0);
    Group->MsgPort = NULL;
  }
}

static void DeleteGroup(ggroup Group) {
  Group->Remove();
  while (Group->FirstG)
    Act(RemoveGadget, Group)(Group, Group->FirstG);

  Group->Fn->Fn_Obj->Delete((obj)Group);
}

static void InsertGadgetGroup(ggroup Group, gadget G) {
  if (G && !G->Group && !G->G_Prev && !G->G_Next) {
    if ((G->G_Next = Group->FirstG))
      Group->FirstG->G_Prev = G;
    else
      Group->LastG = G;

    Group->FirstG = G;
    G->Group = Group;
  }
}

static void RemoveGadgetGroup(ggroup Group, gadget G) {
  if (G && G->Group == Group) {
    if (G->G_Prev)
      G->G_Prev->G_Next = G->G_Next;
    else if (Group->FirstG == G)
      Group->FirstG = G->G_Next;

    if (G->G_Next)
      G->G_Next->G_Prev = G->G_Prev;
    else if (Group->LastG == G)
      Group->LastG = G->G_Prev;

    G->G_Prev = G->G_Next = (gadget)0;
    G->Group = (ggroup)0;
  }
}

static gadget GetSelectedGadget(ggroup Group) {
  return Group->SelectG;
}

static void SetSelectedGadget(ggroup Group, gadget G) {
  if (!G || (G && G->Group == Group)) {
    if (Group->SelectG)
      UnPressGadget(Group->SelectG, ttrue);
    if (G)
      PressGadget(G);
  }
}

static struct s_fn_group _FnGroup = {
    ggroup_magic,      sizeof(struct s_group),
    InsertGroup,       RemoveGroup,
    DeleteGroup,       (void (*)(ggroup, udat, uldat, uldat))NoOp,
    &_FnObj,           InsertGadgetGroup,
    RemoveGadgetGroup, GetSelectedGadget,
    SetSelectedGadget,
};

/* row */

static void InsertRow(row Row, window Parent, row Prev, row Next) {
  if (!Row->Window && Parent && W_USE(Parent, USEROWS)) {
    InsertGeneric((obj)Row, (obj_parent)&Parent->USE.R.FirstRow, (obj)Prev, (obj)Next,
                  &Parent->HLogic);
    Row->Window = Parent;
    Parent->USE.R.NumRowOne = Parent->USE.R.NumRowSplit = (ldat)0;
  }
}

static void RemoveRow(row Row) {
  if (Row->Window && W_USE(Row->Window, USEROWS)) {
    Row->Window->USE.R.NumRowOne = Row->Window->USE.R.NumRowSplit = (ldat)0;
    RemoveGeneric((obj)Row, (obj_parent)&Row->Window->USE.R.FirstRow, &Row->Window->HLogic);
    Row->Window = (window)0;
  }
}

static void DeleteRow(row Row) {
  if (Row) {
    window W = Row->Window;

    Row->Remove();
    if (Row->Text)
      FreeMem(Row->Text);
    if (Row->ColText)
      FreeMem(Row->ColText);

    Row->Fn->Fn_Obj->Delete((obj)Row);

    if (W && W->Parent && (W->Flags & WINDOWFL_MENU))
      ResizeRelWindow(W, 0, -1);
  }
}

static byte SetTextRow(row Row, uldat Len, const char *Text, byte DefaultCol) {
  if (EnsureLenRow(Row, Len, DefaultCol)) {
    if (Len) {

      trune *RowText = Row->Text;
      ldat i = Len;
      while (i-- > 0) {
        *RowText++ = Tutf_CP437_to_UTF_32[(byte)*Text++];
      }
      if (!(Row->Flags & ROW_DEFCOL) && !DefaultCol)
        /* will not work correctly if sizeof(tcolor) != 1 */
        memset(Row->ColText, TCOL(twhite, tblack), Len * sizeof(tcolor));
    }
    Row->Len = Len;
    Row->Gap = Row->LenGap = 0;
    return ttrue;
  }
  return tfalse;
}

static byte SetTRuneRow(row Row, uldat Len, const trune *TRune, byte DefaultCol) {
  if (EnsureLenRow(Row, Len, DefaultCol)) {
    if (Len) {
      CopyMem(TRune, Row->Text, Len * sizeof(trune));
      if (!(Row->Flags & ROW_DEFCOL) && !DefaultCol)
        /* will not work correctly if sizeof(tcolor) != 1 */
        memset(Row->ColText, TCOL(twhite, tblack), Len * sizeof(tcolor));
    }
    Row->Len = Len;
    Row->Gap = Row->LenGap = 0;
    return ttrue;
  }
  return tfalse;
}

/*
 * this is used also for plain rows.
 * be careful to only access fields that even rows have
 */
static void RaiseMenuItem(menuitem M) {
  obj Parent;
  menuitem Next;

  if (M && (Parent = (obj)M->Parent)) {
    if (IS_MENU(Parent))
      Next = ((menu)Parent)->FirstI;
    else if (IS_WINDOW(Parent) && W_USE((window)Parent, USEROWS))
      Next = (menuitem)((window)Parent)->USE.R.FirstRow;
    else
      return;

    Act(Remove, M)(M);
    Act(Insert, M)(M, Parent, (menuitem)0, Next);

    if (IS_MENU(Parent))
      SyncMenu((menu)Parent);
    else
      DrawAreaWidget((widget)Parent);
  }
}

/*
 * this is used also for plain rows.
 * be careful to only access fields that even rows have
 */
static void LowerMenuItem(menuitem M) {
  obj Parent;
  menuitem Prev;

  if (M && (Parent = (obj)M->Parent)) {
    if (IS_MENU(Parent))
      Prev = ((menu)Parent)->LastI;
    else if (IS_WINDOW(Parent) && W_USE((window)Parent, USEROWS))
      Prev = (menuitem)((window)Parent)->USE.R.LastRow;
    else
      return;

    Act(Remove, M)(M);
    Act(Insert, M)(M, Parent, Prev, (menuitem)0);

    if (IS_MENU(Parent))
      SyncMenu((menu)Parent);
    else if (Parent->Parent)
      DrawAreaWidget((widget)Parent);
  }
}

static struct s_fn_row _FnRow = {
    row_magic,
    sizeof(struct s_row),
    InsertRow,
    RemoveRow,
    DeleteRow,
    (void (*)(row, udat, uldat, uldat))NoOp,
    /* row */
    &_FnObj,
    SetTextRow,
    SetTRuneRow,
    (void (*)(row))RaiseMenuItem,
    (void (*)(row))LowerMenuItem,
};

byte FindInfo(menu Menu, dat i) {
  row Info;

  if (Menu && (Info = Menu->Info) && Info->Len > (udat)i)
    return ttrue;
  return tfalse;
}

/* menuitem */

static void InsertMenuItem(menuitem MenuItem, obj Parent, menuitem Prev, menuitem Next) {
  if (!MenuItem->Parent && Parent) {
    if (IS_MENU(Parent)) {
      InsertGeneric((obj)MenuItem, (obj_parent) & ((menu)Parent)->FirstI, (obj)Prev, (obj)Next,
                    (ldat *)0);
      MenuItem->Parent = Parent;
    } else if (IS_WINDOW(Parent)) {
      (MenuItem->Fn->Fn_Row->Insert)((row)MenuItem, (window)Parent, (row)Prev, (row)Next);
    }
  }
}

static void RemoveMenuItem(menuitem MenuItem) {
  if (MenuItem->Parent) {
    if (IS_MENU(MenuItem->Parent)) {
      RemoveGeneric((obj)MenuItem, (obj_parent) & ((menu)MenuItem->Parent)->FirstI, (ldat *)0);
      MenuItem->Parent = (obj)0;
    } else
      (MenuItem->Fn->Fn_Row->Remove)((row)MenuItem);
  }
}

static void DeleteMenuItem(menuitem MenuItem) {
  if (MenuItem) {
    obj Parent = MenuItem->Parent;

    MenuItem->Remove();
    if (IS_MENU(Parent))
      SyncMenu((menu)Parent);

    if (MenuItem->Window)
      MenuItem->Window->Delete();

    MenuItem->Fn->Fn_Row->Delete((row)MenuItem);
  }
}

menuitem Create4MenuMenuItem(fn_menuitem Fn_MenuItem, obj Parent, window Window, udat Code,
                             byte Flags, ldat Len, const char *Name) {
  dat Left, ShortCut;

  if (!Parent)
    return (menuitem)0;

  if (IS_MENU(Parent) && ((menu)Parent)->LastI)
    Left = ((menu)Parent)->LastI->Left + ((menu)Parent)->LastI->Len;
  else
    Left = (dat)1;

  ShortCut = (dat)0;
  while (ShortCut < Len && Name[ShortCut] == ' ')
    ShortCut++;

  if (Window)
    Window->Left = Left;

  return New(menuitem)(Fn_MenuItem, Parent, Window, Code, Flags, Left, Len, ShortCut, Name);
}

/* this returns non-zero for compatibility */
static uldat Create4MenuCommonMenuItem(fn_menuitem Fn_MenuItem, menu Menu) {
  if (Menu) {
    Menu->CommonItems = ttrue;
    SyncMenu(Menu);
    return (uldat)1;
  }
  return (uldat)0;
}

static struct s_fn_menuitem _FnMenuItem = {
    menuitem_magic,
    sizeof(struct s_menuitem),
    InsertMenuItem,
    RemoveMenuItem,
    DeleteMenuItem,
    (void (*)(menuitem, udat, uldat, uldat))NoOp,
    &_FnObj,
    SetTextRow,
    SetTRuneRow,
    RaiseMenuItem,
    LowerMenuItem,
    &_FnRow,
    Create4MenuMenuItem,
    Create4MenuCommonMenuItem,
};

/* menu */

static void InsertMenu(menu Menu, msgport MsgPort, menu Prev, menu Next) {
  if (!Menu->MsgPort && MsgPort) {
    InsertGeneric((obj)Menu, (obj_parent)&MsgPort->FirstMenu, (obj)Prev, (obj)Next, (ldat *)0);
    Menu->MsgPort = MsgPort;
  }
}

static void RemoveMenu(menu Menu) {
  if (Menu->MsgPort) {
    RemoveGeneric((obj)Menu, (obj_parent)&Menu->MsgPort->FirstMenu, (ldat *)0);
    Menu->MsgPort = (msgport)0;
  }
}

static void DeleteMenu(menu Menu) {
  uldat count = 30;

  if (Menu) {
    msgport MsgPort = Menu->MsgPort;
    widget W, WNext;

    /*
     * warning:
     * the only way to get the list of windows that
     * are using this menu is to scan the whole MsgPort.
     * We must UnMap() all those windows
     * as a window without menu cannot be displayed.
     *
     * optimization: if we are going to UnMap() a lot of windows,
     * we set QueuedDrawArea2FullScreen = ttrue, so that the UnMap()
     * calls do not have to redraw every time.
     */
    if (!QueuedDrawArea2FullScreen) {
      for (W = MsgPort->FirstW; W && count; W = WNext) {
        WNext = W->O_Next;
        if (IS_WINDOW(W) && ((window)W)->Menu == Menu) {
          if (W->Parent && IS_SCREEN(W->Parent)) {
            count--;
          }
        }
      }
      if (!count)
        QueuedDrawArea2FullScreen = ttrue;
    }
    for (W = MsgPort->FirstW; W; W = WNext) {
      WNext = W->O_Next;
      if (IS_WINDOW(W) && ((window)W)->Menu == Menu) {
        if (W->Parent && IS_SCREEN(W->Parent)) {
          Act(UnMap, W)(W);
          ((window)W)->Menu = (menu)0;
        }
      }
    }
    Menu->Remove();
    DeleteList(Menu->FirstI);
    if (Menu->Info)
      Menu->Info->Delete();

    Menu->Fn->Fn_Obj->Delete((obj)Menu);
  }
}

static row SetInfoMenu(menu Menu, byte Flags, ldat Len, const char *Text, const tcolor *ColText) {
  row Row;
  if ((Row = New(row)(Fn_row, 0, Flags))) {
    if ((!Text || (Row->Text = CloneStr2TRune(Text, Len))) &&
        (!ColText || (Row->ColText = (tcolor *)CloneMem(ColText, Len * sizeof(tcolor))))) {
      Row->Len = Row->MaxLen = Len;
      if (Menu->Info)
        Menu->Info->Delete();
      return Menu->Info = Row;
    }
    Row->Delete();
    Row = (row)0;
  }
  return Row;
}

static menuitem FindItem(menu Menu, dat i) {
  menuitem Item = (menuitem)0;

  if (Menu) {
    for (Item = Menu->FirstI; Item; Item = Item->Next) {
      if (i >= Item->Left && (uldat)(i - Item->Left) < Item->Len)
        break;
    }

    if (!Item && Menu->CommonItems && All->CommonMenu) {

      Item = Menu->LastI;

      if (!Item || (i >= Item->Left && (uldat)(i - Item->Left) >= Item->Len)) {
        /* search in All->CommonMenu */
        if (Item)
          i -= Item->Left + (dat)Item->Len;
        for (Item = All->CommonMenu->FirstI; Item; Item = Item->Next) {
          if (i >= Item->Left && (uldat)(i - Item->Left) < Item->Len)
            break;
        }
      } else
        Item = (menuitem)0;
    }
  }
  return Item;
}

static menuitem GetSelectedItem(menu Menu) {
  if (Menu) {
    if (Menu->SelectI)
      return Menu->SelectI;
    if (Menu->CommonItems && All->CommonMenu)
      return All->CommonMenu->SelectI;
  }
  return (menuitem)0;
}

static menuitem RecursiveGetSelectedItem(menu Menu, dat *depth) {
  menuitem _I = Act(GetSelectedItem, Menu)(Menu), I = (menuitem)0;
  window W = (window)0, FW = (window)All->FirstScreen->FocusW;
  dat d = -1;

  while (_I && IS_MENUITEM(_I)) {
    I = _I;
    d++;
    if (W == FW)
      break;
    if ((W = I->Window) && W->Parent)
      _I = (menuitem)Act(FindRow, W)(W, W->CurY);
    else
      break;
  }
  if (depth)
    *depth = d > 0 ? d : 0;
  return I;
}

static void SetSelectedItem(menu Menu, menuitem Item) {
  if (Menu) {
    if (Item) {
      if (Item->Parent == (obj)Menu) {
        Menu->SelectI = Item;
        if (Menu->CommonItems && All->CommonMenu)
          All->CommonMenu->SelectI = (menuitem)0;
      } else if (Menu->CommonItems && Item->Parent == (obj)All->CommonMenu) {
        Menu->SelectI = (menuitem)0;
        All->CommonMenu->SelectI = Item;
      }
      /* else Item is not a meaningful one! */
    } else {
      Menu->SelectI = Item;
      if (Menu->CommonItems && All->CommonMenu)
        All->CommonMenu->SelectI = Item;
    }

    Act(DrawMenu, All->FirstScreen)(All->FirstScreen, 0, TW_MAXDAT);
  }
}

static struct s_fn_menu _FnMenu = {
    menu_magic,
    sizeof(struct s_menu),
    InsertMenu,
    RemoveMenu,
    DeleteMenu,
    (void (*)(menu, udat, uldat, uldat))NoOp,
    /* menu */
    &_FnObj,
    SetInfoMenu,
    FindItem,
    GetSelectedItem,
    RecursiveGetSelectedItem,
    SetSelectedItem,
};

/* msg */

static void InsertMsg(msg Msg, msgport Parent, msg Prev, msg Next) {
  if (!Msg->MsgPort && Parent) {
    /* if adding the first msg, move the msgport to the head
     * of msgport list, so that the scheduler will run it */
    if (!Parent->FirstMsg && Parent->All)
      MoveFirst(MsgPort, All, Parent);

    InsertGeneric((obj)Msg, (obj_parent)&Parent->FirstMsg, (obj)Prev, (obj)Next, (ldat *)0);
    Msg->MsgPort = Parent;
  }
}

static void RemoveMsg(msg Msg) {
  if (Msg->MsgPort) {
    RemoveGeneric((obj)Msg, (obj_parent)&Msg->MsgPort->FirstMsg, (ldat *)0);
    Msg->MsgPort = (msgport)0;
  }
}

static void DeleteMsg(msg Msg) {
  if (Msg) {
    Msg->Remove();
    Msg->Fn->Fn_Obj->Delete((obj)Msg);
  }
}

static struct s_fn_msg _FnMsg = {
    msg_magic,
    sizeof(struct s_msg),
    InsertMsg,
    RemoveMsg,
    DeleteMsg,
    (void (*)(msg, udat, uldat, uldat))NoOp,
    /* msg */
    &_FnObj,
};

/* msgport */

static void InsertMsgPort(msgport MsgPort, all Parent, msgport Prev, msgport Next) {
  if (!MsgPort->All && Parent) {
    InsertGeneric((obj)MsgPort, (obj_parent)&Parent->FirstMsgPort, (obj)Prev, (obj)Next, (ldat *)0);
    MsgPort->All = Parent;
  }
}

static void RemoveMsgPort(msgport MsgPort) {
  if (MsgPort->All) {
    if (All->RunMsgPort == MsgPort)
      All->RunMsgPort = MsgPort->Next;
    RemoveGeneric((obj)MsgPort, (obj_parent)&MsgPort->All->FirstMsgPort, (ldat *)0);
    MsgPort->All = (all)0;
  }
}

static void DeleteMsgPort(msgport MsgPort) {
  uldat count = 30;
  widget W;
  extension *Es;

  if (MsgPort) {
    /*
     * optimization: if we are going to UnMap() a lot of windows,
     * we set QueuedDrawArea2FullScreen = ttrue, so that the UnMap()
     * calls do not have to redraw every time.
     */
    for (W = MsgPort->FirstW; W && count; W = W->O_Next) {
      if (IS_WINDOW(W) && W->Parent && IS_SCREEN(W->Parent)) {
        count--;
      }
    }
    if (!count)
      QueuedDrawArea2FullScreen = ttrue;

    if (MsgPort->ShutDownHook)
      MsgPort->ShutDownHook(MsgPort);

    /*
     * must delete the Menus first, as among widgets there are also
     * menuitem windows, which cannot be deleted before deleting
     * the corresponding menuitem.
     */
    DeleteList(MsgPort->FirstMsg);
    DeleteList(MsgPort->FirstMenu);
    DeleteList(MsgPort->FirstW);
    DeleteList(MsgPort->FirstGroup);
    DeleteList(MsgPort->FirstMutex);

    for (count = MsgPort->CountE, Es = MsgPort->Es; count; count--, Es++)
      Act(UnuseExtension, MsgPort)(MsgPort, *Es);

    MsgPort->Remove();
    if (MsgPort->Name)
      FreeMem(MsgPort->Name);

    MsgPort->Fn->Fn_Obj->Delete((obj)MsgPort);
  }
}

static byte GrowExtensionMsgPort(msgport M) {
  uldat oldsize, size;
  extension *newEs;

  oldsize = M->SizeE;

  size = oldsize < 2 ? 4 : oldsize + (oldsize >> 1);
  if (size > MAXID)
    size = MAXID;

  if (!(newEs = (extension *)ReAllocMem0(M->Es, sizeof(extension), oldsize, size)))
    return tfalse;

  M->Es = newEs;
  return ttrue;
}

static void UseExtensionMsgPort(msgport M, extension E) {
  uldat count;
  extension *Es;

  if (M && E) {

    for (count = M->CountE, Es = M->Es; count; count--, Es++) {
      if (*Es == E)
        /* already in use */
        return;
    }

    if (M->CountE >= M->SizeE && !GrowExtensionMsgPort(M))
      return;

    M->Es[M->CountE++] = E;
    E->Used++;
  }
}

static void UnuseExtensionMsgPort(msgport M, extension E) {
  uldat count;
  extension *Es;

  if (M && E)
    for (count = M->CountE, Es = M->Es; count; count--, Es++) {
      if (*Es == E) {
        if (count > 1)
          *Es = Es[count - 1];
        M->CountE--;

        if (!--E->Used)
          E->Delete();
      }
    }
}

static struct s_fn_msgport _FnMsgPort = {
    msgport_magic,
    sizeof(struct s_msgport),
    InsertMsgPort,
    RemoveMsgPort,
    DeleteMsgPort,
    (void (*)(msgport, udat, uldat, uldat))NoOp,
    /* msgport */
    &_FnObj,
    UseExtensionMsgPort,
    UnuseExtensionMsgPort,
};

/* mutex */

static void InsertMutex(mutex Mutex, all Parent, mutex Prev, mutex Next) {
  if (!Mutex->All && Parent) {
    InsertGeneric((obj)Mutex, (obj_parent)&Mutex->All->FirstMutex, (obj)Prev, (obj)Next, (ldat *)0);
    Mutex->All = Parent;
  }
}

static void RemoveMutex(mutex Mutex) {
  if (Mutex->All) {
    RemoveGeneric((obj)Mutex, (obj_parent)&Mutex->All->FirstMutex, (ldat *)0);
    Mutex->All = (all)0;
  }
}

static void DeleteMutex(mutex Mutex) {
  Act(DisOwn, Mutex)(Mutex);
  Mutex->Remove();
  Mutex->Fn->Fn_Obj->Delete((obj)Mutex);
}

static void OwnMutex(mutex Mutex, msgport Parent) {
  if (!Mutex->Owner && Parent) {

    if ((Mutex->O_Prev = Parent->LastMutex))
      Parent->LastMutex->O_Next = Mutex;
    else
      Parent->FirstMutex = Mutex;

    Mutex->O_Next = (mutex)0;
    Parent->LastMutex = Mutex;

    Mutex->Owner = Parent;
  }
}

static void DisOwnMutex(mutex Mutex) {
  msgport Parent;
  if ((Parent = Mutex->Owner)) {
    if (Mutex->O_Prev)
      Mutex->O_Prev->O_Next = Mutex->O_Next;
    else if (Parent->FirstMutex == Mutex)
      Parent->FirstMutex = Mutex->O_Next;

    if (Mutex->O_Next)
      Mutex->O_Next->O_Prev = Mutex->O_Prev;
    else if (Parent->LastMutex == Mutex)
      Parent->LastMutex = Mutex->O_Prev;

    Mutex->O_Prev = Mutex->O_Next = (mutex)0;

    Mutex->Owner = (msgport)0;
  }
}

static struct s_fn_mutex _FnMutex = {
    mutex_magic,
    sizeof(struct s_mutex),
    InsertMutex,
    RemoveMutex,
    DeleteMutex,
    (void (*)(mutex, udat, uldat, uldat))NoOp,
    /* mutex */
    &_FnObj,
    OwnMutex,
    DisOwnMutex,
};

/* module */

static void InsertModule(module Module, all Parent, module Prev, module Next) {
  if (!Module->All && Parent) {
    InsertGeneric((obj)Module, (obj_parent)&Parent->FirstModule, (obj)Prev, (obj)Next, (ldat *)0);
    Module->All = Parent;
  }
}

static void RemoveModule(module Module) {
  if (Module->All) {
    RemoveGeneric((obj)Module, (obj_parent)&Module->All->FirstModule, (ldat *)0);
    Module->All = (all)0;
  }
}

static void DeleteModule(module Module) {
  if (!Module->Used) {

    Act(DlClose, Module)(Module);
    Module->Remove();
    if (Module->Name)
      FreeMem(Module->Name);

    Module->Fn->Fn_Obj->Delete((obj)Module);
  }
}

static struct s_fn_module _FnModule = {
    module_magic,
    sizeof(struct s_module),
    InsertModule,
    RemoveModule,
    DeleteModule,
    (void (*)(module, udat, uldat, uldat))NoOp,
    /* module */
    &_FnObj,
    DlOpen,
    DlClose,
};

/* extension */

static void DeleteExtension(extension E) {
  fn_module Fn_Module;
  msgport M;
  extension *Es;
  uldat i;

  /* search for MsgPorts using this extension */
  for (M = All->FirstMsgPort; M; M = M->Next) {
    for (i = M->CountE, Es = M->Es; i; i--, Es++) {
      if (E == *Es) {
        Act(UnuseExtension, M)(M, E);
        break;
      }
    }
  }

  /* used if the extension is NOT DlOpen()ed */
  if (E->Quit)
    E->Quit(E);

  E->Fn->Fn_Module->Delete((module)E);
}

static struct s_fn_extension _FnExtension = {
    extension_magic, sizeof(struct s_extension),
    (void (*)(extension, all, extension, extension))InsertModule, //
    (void (*)(extension))RemoveModule, DeleteExtension,
    (void (*)(extension, udat, uldat, uldat))NoOp,
    /* module */
    &_FnObj, (byte(*)(extension))DlOpen, (void (*)(extension))DlClose, &_FnModule,
#ifdef CONF_EXT
    QueryExtension, /* exported by extensions/ext_query.c */
#else
    (extension(*)(byte, const char *))AlwaysNull, /* Query */
#endif
};

/* display_hw */

static void InsertDisplayHW(display_hw DisplayHW, all Parent, display_hw Prev, display_hw Next) {
  if (!DisplayHW->All && Parent) {
    InsertGeneric((obj)DisplayHW, (obj_parent)&Parent->FirstDisplayHW, (obj)Prev, (obj)Next,
                  (ldat *)0);
    DisplayHW->All = Parent;
#if 0
	/*
	 * here we would call uninitialized DisplayHW routines like MoveToXY,
	 * put this after DisplayHW->InitHW()
	 */
	if (All->FnHookDisplayHW)
	    All->FnHookDisplayHW(All->HookDisplayHW);
#endif
  }
}

static void RemoveDisplayHW(display_hw DisplayHW) {
  if (DisplayHW->All) {
    RemoveGeneric((obj)DisplayHW, (obj_parent)&DisplayHW->All->FirstDisplayHW, (ldat *)0);
    DisplayHW->All = (all)0;

    if (All->FnHookDisplayHW)
      All->FnHookDisplayHW(All->HookDisplayHW);
  }
}

static void DeleteDisplayHW(display_hw DisplayHW) {
  byte isCTTY = DisplayHW->DisplayIsCTTY && DisplayHW == DisplayHWCTTY;
  byte Quitted = DisplayHW->Quitted;

  if (!Quitted)
    Act(Quit, DisplayHW)(DisplayHW);

  /* avoid getting stale pointers */
  if (All->MouseHW == DisplayHW)
    All->MouseHW = NULL;
  if (All->ExclusiveHW == DisplayHW)
    All->ExclusiveHW = NULL;

  DisplayHW->Remove();
  if (DisplayHW->NameLen && DisplayHW->Name)
    FreeMem(DisplayHW->Name);

  DisplayHW->Fn->Fn_Obj->Delete((obj)DisplayHW);

  if (!Quitted) {
    if (!All->FirstDisplayHW || isCTTY)
      RunNoHW(tfalse);
    else if (All->FirstDisplayHW && ResizeDisplay()) {
      QueuedDrawArea2FullScreen = ttrue;
    }
  }
}

static struct s_fn_display_hw _FnDisplayHW = {
    display_hw_magic,
    sizeof(struct s_display_hw),
    InsertDisplayHW,
    RemoveDisplayHW,
    DeleteDisplayHW,
    (void (*)(display_hw, udat, uldat, uldat))NoOp,
    /* display_hw */
    &_FnObj,
    InitDisplayHW,
    QuitDisplayHW,
};

fn_struct FnStruct = {
    &_FnObj,   &_FnWidget, &_FnGadget,   &_FnWindow,    &_FnScreen,
    &_FnGroup, &_FnRow,    &_FnMenuItem, &_FnMenu,      &_FnMsgPort,
    &_FnMutex, &_FnMsg,    &_FnModule,   &_FnExtension, &_FnDisplayHW,
};
