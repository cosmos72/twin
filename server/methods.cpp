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

#include "dl.h"
#include "draw.h"
#include "extreg.h"
#include "log.h"
#include "resize.h"
#include "util.h"
#include "hw.h"
#include "hw_multi.h"

#include <new>
#include <Tw/Tw.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>
#include <Tutf/Tutf.h>
#include <Tutf/Tutf_defs.h>

/* some object-oriented ones not included in TobjFn */

void *OverrideMth(void **where, void *OrigMth, void *NewMth) {
  if (*where == OrigMth) {
    *where = NewMth;
    return NewMth;
  }
  return NULL;
}

#if 0 /* not used */
inline void DeletePartialList(Tobj Obj) {
  Tobj Next;
  while (Obj) {
    Next = Obj->Next;
    Obj->Delete();
    Obj = Next;
  }
}
#endif

inline void InsertGeneric(TobjEntry Obj, TobjList Parent, TobjEntry Prev, TobjEntry Next,
                          ldat *ObjCount) {
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

inline void RemoveGeneric(TobjEntry Obj, TobjList Parent, ldat *ObjCount) {
  if (Obj->Prev)
    Obj->Prev->Next = Obj->Next;
  else if (Parent->First == Obj)
    Parent->First = Obj->Next;

  if (Obj->Next)
    Obj->Next->Prev = Obj->Prev;
  else if (Parent->Last == Obj)
    Parent->Last = Obj->Prev;

  Obj->Prev = Obj->Next = NULL;
  if (ObjCount)
    (*ObjCount)--;
}

/* TobjFn and others fn_XXX functions */

/* Tobj */

static void InsertObj(Tobj Obj, Tobj Parent, Tobj Prev, Tobj Next) {
  log(ERROR) << "twin: internal error: pure virtual function InsertObj() called!\n";
}

static void RemoveObj(Tobj Obj) {
  log(ERROR) << "twin: internal error: pure virtual function RemoveObj() called!\n";
}

static void DeleteObj(Tobj Obj) {
  /* not a good idea to RemoveObj(Obj) here */
  DropId(Obj);
  FreeMem(Obj);
}

static struct SobjFn _FnObj = {
    obj_magic, InsertObj, RemoveObj, DeleteObj, (void (*)(Tobj, udat, uldat, uldat))NoOp,
};

/* Twidget */

static void InsertWidget(Twidget W, Twidget Parent, Twidget Prev, Twidget Next) {
  if (Parent) {
    /*
     * don't check W->Parent here, as Raise() and Lower() call
     * RemoveWidget() then InsertWidget() but RemoveWidget() does not reset W->Parent
     */
    InsertGeneric((TobjEntry)W, (TobjList)&Parent->FirstW, (TobjEntry)Prev, (TobjEntry)Next, NULL);
  }
}

static void RemoveWidget(Twidget W) {
  if (W->Parent) {
    RemoveGeneric((TobjEntry)W, (TobjList)&W->Parent->FirstW, NULL);
  }
}

static void DeleteWidget(Twidget W) {
  W->UnMap();
  if (W->Hook) {
    W->RemoveHook(W->Hook, W->WhereHook);
  }
  if (W->ShutDownHook) {
    W->ShutDownHook(W);
  }
  W->DisOwn();
  while (W->FirstW) {
    W->FirstW->UnMap();
  }
  DeleteObj((Tobj)W);
}

static void SetFillWidget(Twidget W, tcell Fill) {
  if (W->USE_Fill != Fill) {
    W->USE_Fill = Fill;
    if (W->Parent)
      DrawAreaWidget(W);
  }
}

static void ChangeFieldWidget(Twidget W, udat field, uldat CLEARMask, uldat XORMask) {
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
      W->Fn->Fn_Obj->ChangeField((Tobj)W, field, CLEARMask, XORMask);
      break;
    }
}

Twidget FocusWidget(Twidget W) {
  Twidget oldW;
  if (W)
    oldW = W->KbdFocus();
  else
    oldW = Do(KbdFocus, widget)(NULL);

  if (W != oldW && (!W || W->Parent == (Twidget)All->FirstScreen)) {
    if (W && IS_WINDOW(W))
      DrawBorderWindow((Twindow)W, BORDER_ANY);
    if (oldW && IS_WINDOW(oldW))
      DrawBorderWindow((Twindow)oldW, BORDER_ANY);
    if ((W && IS_WINDOW(W)) || (oldW && IS_WINDOW(oldW))) {
      UpdateCursor();
      if (!W || !IS_WINDOW(W) || !(((Twindow)W)->Flags & WINDOWFL_MENU))
        All->FirstScreen->DrawMenu(0, TW_MAXDAT);
    }
  }
  return oldW;
}

#define TtyKbdFocus FakeKbdFocus
Twidget FakeKbdFocus(Twidget W) {
  Twidget oldW;
  Twidget P;
  screen Screen = W && (P = W->Parent) && IS_SCREEN(P) ? (screen)P : All->FirstScreen;

  if (Screen) {
    oldW = Screen->FocusW;
    Screen->FocusW = W;
  } else
    oldW = (Twidget)0;

  return oldW;
}

static Tgadget FindGadgetByCode(Twidget Parent, udat Code) {
  Twidget W;

  for (W = Parent->FirstW; W; W = W->Next) {
    if (IS_GADGET(W) && ((Tgadget)W)->Code == Code)
      return (Tgadget)W;
  }
  return (Tgadget)0;
}

static void IncMouseMotionN(void) {
  if (!All->MouseMotionN++)
    EnableMouseMotionEvents(ttrue);
}

static void DecMouseMotionN(void) {
  if (All->MouseMotionN && !--All->MouseMotionN)
    EnableMouseMotionEvents(tfalse);
}

static void MapWidget(Twidget W, Twidget Parent) {
  Tmsg msg;

  if (W && !W->Parent && !W->MapQueueMsg && Parent) {
    if (IS_SCREEN(Parent)) {
      if (Ext(WM, MsgPort) && (msg = New(msg)(msg_map, 0))) {
        msg->Event.EventMap.W = W;
        msg->Event.EventMap.Code = 0;
        msg->Event.EventMap.Screen = (screen)Parent;
        W->MapQueueMsg = msg;
        SendMsg(Ext(WM, MsgPort), msg);
      } else
        W->MapTopReal((screen)Parent);
    } else if (IS_WIDGET(Parent)) {
      if (W->Up == TW_MAXDAT) {
        W->Left = Parent->XLogic;
        W->Up = Parent->YLogic;
      }
      InsertFirst(W, W, Parent);
      W->Parent = Parent;

      DrawAreaWidget(W);

      if (W->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        IncMouseMotionN();

      if (W->MapUnMapHook)
        W->MapUnMapHook(W);
    }
  }
}

static void MapTopRealWidget(Twidget W, screen Screen) {
  Twidget OldW;

  if (Screen && !W->Parent && (!IS_WINDOW(W) || ((Twindow)W)->Menu)) {
    if (W->MapQueueMsg)
      /*
       * let the upper layer do this:
       * W->MapQueueMsg->Delete();
       */
      W->MapQueueMsg = (Tmsg)0;

    if (W->Up == TW_MAXDAT) {
      W->Left = Screen->XLogic;
      W->Up = Max2(Screen->YLimit + 1, 0) + Screen->YLogic;
    } else {
      W->Left += Screen->XLogic;
      W->Up += Screen->YLogic;
    }

    InsertFirst(W, W, (Twidget)Screen);
    W->Parent = (Twidget)Screen;

    /* top-level widgets must be visible */
    W->Flags &= ~WINDOWFL_NOTVISIBLE;

    if (W->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
      IncMouseMotionN();

    if (Screen == All->FirstScreen) {
      OldW = W->KbdFocus();
      if (OldW && IS_WINDOW(OldW))
        DrawBorderWindow((Twindow)OldW, BORDER_ANY);
      UpdateCursor();
    }
    if (IS_WINDOW(W))
      DrawAreaWindow2((Twindow)W);
    else
      DrawAreaWidget(W);
    if (!(W->Flags & WINDOWFL_MENU))
      Screen->DrawMenu(0, TW_MAXDAT);

    if (W->MapUnMapHook)
      W->MapUnMapHook(W);

    if (Screen->FnHookW)
      Screen->FnHookW(Screen->HookW);
  }
}

static void UnMapWidget(Twidget W) {
  Twidget Parent;
  Twindow Next;
  screen Screen;
  byte wasFocus;

  if (W && (Parent = W->Parent)) {
    if (IS_SCREEN(Parent)) {
      if ((Screen = (screen)Parent) == All->FirstScreen && W == (Twidget)Screen->MenuWindow) {
        /*
         * ! DANGER !
         * Trying to UnMap() the Tmenu owner.
         * shutdown the Tmenu first!
         */
        CloseMenu();
      }

      if (W->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        DecMouseMotionN();

      if (Screen->ClickWindow == (Twindow)W)
        Screen->ClickWindow = NULL;

      if ((wasFocus = W == Screen->FocusW)) {
        if (W->Flags & WINDOWFL_MENU)
          Next = Screen->MenuWindow;
        else {
          if ((Twidget)W == Screen->FirstW)
            Next = (Twindow)W->Next;
          else
            Next = (Twindow)Screen->FirstW;

          while (Next && !IS_WINDOW(Next))
            Next = (Twindow)Next->Next;
        }
      }

      W->Remove();
      if (IS_WINDOW(W))
        DrawAreaWindow2((Twindow)W);
      else
        DrawAreaWidget(W);

      if (IS_SCREEN(Parent)) {
        W->Left = 0;
        W->Up = TW_MAXDAT;
      }
      W->Parent = (Twidget)0;

      if (wasFocus) {
        if (Screen == All->FirstScreen) {
          /*
           * in case the user was dragging this Twindow...
           */
          if ((All->State & state_any) < state_menu)
            All->State &= ~state_any;

          if (Next) {
            (void)Next->KbdFocus();
            DrawBorderWindow(Next, BORDER_ANY);
          } else
            Do(KbdFocus, window)(NULL);
          if (!(W->Flags & WINDOWFL_MENU))
            Screen->DrawMenu(0, TW_MAXDAT);
          UpdateCursor();
        } else
          Screen->FocusW = (Twidget)Next;
      }

      if (W->MapUnMapHook)
        W->MapUnMapHook(W);

      if (Screen->FnHookW)
        Screen->FnHookW(Screen->HookW);

    } else {
      /* UnMap() a sub-window */
      if (W == Parent->SelectW)
        Parent->SelectW = (Twidget)0;

      W->Remove();
      DrawAreaWidget(W);
      W->Parent = (Twidget)0;

      if (W->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        DecMouseMotionN();

      if (W->MapUnMapHook)
        W->MapUnMapHook(W);
    }
  } else if (W->MapQueueMsg) {
    /* the window was still waiting to be mapped! */
    W->MapQueueMsg->Delete();
    W->MapQueueMsg = (Tmsg)0;
  }
}

static void RaiseW(Twidget W) {
  RaiseWidget(W, tfalse);
}

static void LowerW(Twidget W) {
  LowerWidget(W, tfalse);
}

static void SetXYWidget(Twidget W, dat X, dat Y) {
  Twidget Prev, Next;

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

static void OwnWidget(Twidget Widget, Tmsgport Owner) {
  if (!Widget->Owner) {
    if ((Widget->O_Next = Owner->FirstW))
      Owner->FirstW->O_Prev = Widget;
    else
      Owner->LastW = Widget;

    Widget->O_Prev = (Twidget)0;
    Owner->FirstW = Widget;
    Widget->Owner = Owner;
  }
}

static void DisOwnWidget(Twidget W) {
  Tmsgport Owner;
  if ((Owner = W->Owner)) {
    if (W->O_Prev)
      W->O_Prev->O_Next = W->O_Next;
    else if (Owner->FirstW == W)
      Owner->FirstW = W->O_Next;

    if (W->O_Next)
      W->O_Next->O_Prev = W->O_Prev;
    else if (Owner->LastW == W)
      Owner->LastW = W->O_Prev;

    W->O_Prev = W->O_Next = (Twidget)0;
    W->Owner = (Tmsgport)0;
  }
}

static void RecursiveDeleteWidget(Twidget W, Tmsgport maybeOwner) {
  while (W->FirstW)
    W->FirstW->RecursiveDelete(maybeOwner);

  if (W->Owner == maybeOwner)
    W->Delete();
  else
    W->UnMap();
}

static byte InstallHookWidget(Twidget W, HookFn Hook, HookFn *WhereHook) {
  if (W && !W->Hook && !W->WhereHook && Hook && WhereHook && !WhereHook[0] && !WhereHook[1]) {

    W->Hook = WhereHook[0] = Hook;
    W->WhereHook = WhereHook;
    WhereHook[1] = (HookFn)W;
    return ttrue;
  }
  return tfalse;
}

static void RemoveHookWidget(Twidget W, HookFn Hook, HookFn *WhereHook) {
  if (W && Hook && W->Hook == Hook && WhereHook && W->WhereHook == WhereHook &&
      WhereHook[0] == Hook && WhereHook[1] == (void *)W) {

    W->Hook = *WhereHook = (HookFn)0;
    W->WhereHook = (HookFn *)0;
    WhereHook[1] = (HookFn)0;
  }
}

static struct SwidgetFn _FnWidget = {
    widget_magic,      InsertWidget,     RemoveWidget,          DeleteWidget,  ChangeFieldWidget,
    &_FnObj,           DrawSelfWidget, /* exported by draw.c */
    FindWidgetAt,                      /* exported by draw.c */
    FindGadgetByCode,  SetXYWidget,      SetFillWidget,         FocusWidget,   TtyKbdFocus,
    MapWidget,         UnMapWidget,      MapTopRealWidget,      RaiseW,        LowerW,
    OwnWidget,         DisOwnWidget,     RecursiveDeleteWidget, ExposeWidget2, /* exported by
                                                                                  resize.c */
    InstallHookWidget, RemoveHookWidget,
};

/* Tgadget */

static void DeleteGadget(Tgadget G) {
  byte i;

  G->UnMap();
  if (G_USE(G, USETEXT)) {
    for (i = 0; i < 4; i++) {
      if (G->USE.T.Text[i])
        FreeMem(G->USE.T.Text[i]);
      if (G->USE.T.Color[i])
        FreeMem(G->USE.T.Color[i]);
    }
  }
  if (G->Group) {
    G->Group->RemoveGadget(G);
  }

  DeleteWidget((Twidget)G);
}

static void ChangeFieldGadget(Tgadget G, udat field, uldat CLEARMask, uldat XORMask) {
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
          DrawAreaWidget((Twidget)G);
        } else
          G->Flags = i;
      }
      break;
    default:
      G->Fn->Fn_Widget->ChangeField((Twidget)G, field, CLEARMask, XORMask);
      break;
    }
}

static Tgadget CreateEmptyButton(Tmsgport Owner, dat XWidth, dat YWidth, tcolor BgCol) {
  Tgadget G = NULL;
  ldat Size;
  byte i;
  dat j, k;
#define _FULL T_UTF_32_FULL_BLOCK
#define _LOWER T_UTF_32_LOWER_HALF_BLOCK
#define _UPPER T_UTF_32_UPPER_HALF_BLOCK
  void *addr = AllocMem0(sizeof(Sgadget));
  if (addr) {
    G = new (addr) Sgadget();
    G->Fn = Fn_Tgadget;
    if (!((Twidget)G)
             ->Init(Owner, ++XWidth, ++YWidth, 0, GADGETFL_USETEXT | GADGETFL_BUTTON, 0, 0,
                    (tcell)0)) {
      G->Delete();
      return NULL;
    }

    Size = (ldat)XWidth * YWidth;

    for (i = 0; i < 4; i++)
      G->USE.T.Text[i] = NULL, G->USE.T.Color[i] = NULL;

    for (i = 0; i < 4; i++)
      if (!(G->USE.T.Text[i] = (trune *)AllocMem(Size * sizeof(trune))) ||
          !(G->USE.T.Color[i] = (tcolor *)AllocMem(Size * sizeof(tcolor)))) {

        G->Delete();
        return NULL;
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

    G->G_Prev = G->G_Next = (Tgadget)0;
    G->Group = (Tgroup)0;
  }
  return G;
#undef _FULL
#undef _UPPER
#undef _LOWER
}

byte FillButton(Tgadget G, Twidget Parent, udat Code, dat Left, dat Up, udat Flags,
                const char *Text, tcolor Color, tcolor ColorDisabled) {
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
    G->Map(Parent);

  return ttrue;
}

static Tgadget CreateButton(Twidget Parent, dat XWidth, dat YWidth, const char *Text, uldat Flags,
                            udat Code, tcolor BgCol, tcolor Col, tcolor ColDisabled, dat Left,
                            dat Up) {
  Tgadget G;
  if (Parent && (G = CreateEmptyButton(Parent->Owner, XWidth, YWidth, BgCol))) {
    if (G->FillButton(Parent, Code, Left, Up, Flags, Text, Col, ColDisabled))
      return G;
    G->Delete();
    G = NULL;
  }
  return G;
}

static struct SgadgetFn _FnGadget = {
    gadget_magic,                                               //
    (void (*)(Tgadget, Twidget, Twidget, Twidget))InsertWidget, //
    (void (*)(Tgadget))RemoveWidget, DeleteGadget, ChangeFieldGadget,
    /* Twidget */
    &_FnObj, DrawSelfGadget,                      /* exported by draw.c */
    (Twidget (*)(Tgadget, dat, dat))FindWidgetAt, /* exported by draw.c */
    (Tgadget (*)(Tgadget, udat))FindGadgetByCode, (void (*)(Tgadget, dat, dat))SetXYWidget,
    (void (*)(Tgadget, tcell))SetFillWidget, (Twidget(*)(Tgadget))FocusWidget,
    (Twidget(*)(Tgadget))TtyKbdFocus, (void (*)(Tgadget, Twidget))MapWidget,
    (void (*)(Tgadget))UnMapWidget, (void (*)(Tgadget, screen))MapTopRealWidget,
    (void (*)(Tgadget))RaiseW, (void (*)(Tgadget))LowerW, (void (*)(Tgadget, Tmsgport))OwnWidget,
    (void (*)(Tgadget))DisOwnWidget, (void (*)(Tgadget, Tmsgport))RecursiveDeleteWidget,
    (void (*)(Tgadget, dat, dat, dat, dat, const char *, const trune *,
              const tcell *))ExposeWidget2, /* exported by resize.c */
    (byte (*)(Tgadget, HookFn, void (**)(Twidget)))InstallHookWidget,
    (void (*)(Tgadget, HookFn, void (**)(Twidget)))RemoveHookWidget,
    /* Tgadget */
    &_FnWidget, CreateEmptyButton, FillButton, CreateButton,
    WriteTextsGadget,  /* exported by resize.c */
    WriteTRunesGadget, /* exported by resize.c */
};

/* Twindow */

static void DeleteWindow(Twindow W) {
  W->UnMap();
  if (W->Name)
    FreeMem(W->Name);
  if (W->ColName)
    FreeMem(W->ColName);
  if (W_USE(W, USECONTENTS)) {
    if (W->USE.C.TtyData)
      FreeMem(W->USE.C.TtyData);
    if (W->USE.C.Contents)
      FreeMem(W->USE.C.Contents);
  } else if (W_USE(W, USEROWS)) {
    DeleteList(W->USE.R.FirstRow);
  }
  DeleteWidget((Twidget)W);
}

static void ChangeFieldWindow(Twindow W, udat field, uldat CLEARMask, uldat XORMask) {
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
        DrawAreaWidget((Twidget)W);
      }
    } break;
    case TWS_window_Flags:
      mask = WINDOWFL_CURSOR_ON;
      CLEARMask &= mask;
      XORMask &= mask;
      i = (W->Flags & ~CLEARMask) ^ XORMask;
      if ((i & mask) != (W->Flags & mask)) {
        W->Flags = i;
        if (ContainsCursor((Twidget)W))
          UpdateCursor();
      }
      break;
    case TWS_window_Attr:
      mask = WINDOW_WANT_KEYS | WINDOW_WANT_MOUSE | WINDOW_WANT_CHANGES | WINDOW_AUTO_FOCUS |
             WINDOW_DRAG | WINDOW_RESIZE | WINDOW_CLOSE | WINDOW_ROLLED_UP | WINDOW_X_BAR |
             WINDOW_Y_BAR | WINDOW_AUTO_KEYS | WINDOW_WANT_MOUSE_MOTION;
      CLEARMask &= mask;
      XORMask &= mask;
      i = (W->Attr & ~CLEARMask) ^ XORMask;
      if ((i & mask) != (W->Attr & mask)) {
        if ((i & WINDOW_ROLLED_UP) != (W->Attr & WINDOW_ROLLED_UP))
          RollUpWindow(W, !!(i & WINDOW_ROLLED_UP));
        if ((i & (WINDOW_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS)) !=
                (W->Attr & (WINDOW_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS)) &&
            W->Parent) {
          if (i & (WINDOW_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
            IncMouseMotionN();
          else
            DecMouseMotionN();
        }
        mask = WINDOW_RESIZE | WINDOW_CLOSE | WINDOW_X_BAR | WINDOW_Y_BAR;
        if ((i & mask) != (W->Attr & mask) && W->Parent) {
          W->Attr = i;
          DrawBorderWindow(W, BORDER_ANY);
        } else
          W->Attr = i;
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
      W->Fn->Fn_Widget->ChangeField((Twidget)W, field, CLEARMask, XORMask);
      break;
    }
}

static void SetTitleWindow(Twindow W, dat titlelen, char *title) {
  Twidget P;

  if (W->Name)
    FreeMem(W->Name);

  W->NameLen = titlelen;
  W->Name = title;

#if 1
  /*
   * do not allow changing Twindow borders just because
   * some untrusted application set a new title
   */
  DrawBorderWindow(W, BORDER_UP);
#else
  /* user may have title-dependent borders in ~/.config/twin/twinrc, honour them: */
  Win->BorderPattern[0] = Win->BorderPattern[1] = NULL;
  DrawBorderWindow(W, BORDER_ANY);
#endif

  if ((P = W->Parent) && IS_SCREEN(P)) {
    /* need to update Twindow list with new name ? */
    if (((screen)P)->FnHookW)
      ((screen)P)->FnHookW(((screen)P)->HookW);
  }
}

static void SetColTextWindow(Twindow W, tcolor ColText) {
  W->ColText = ColText;
}

static void SetColorsWindow(Twindow W, udat Bitmap, tcolor ColGadgets, tcolor ColArrows,
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

static void SetXYWindow(Twindow W, dat X, dat Y) {
  Twidget Prev, Next;

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

static void ConfigureWindow(Twindow W, byte Bitmap, dat Left, dat Up, dat MinXWidth, dat MinYWidth,
                            dat MaxXWidth, dat MaxYWidth) {
  Twidget Prev, Next;
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

static void GotoXYWindow(Twindow Window, ldat X, ldat Y) {
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
  if (ContainsCursor((Twidget)Window))
    UpdateCursor();
}

Twindow Create4MenuWindow(Tmenu Menu) {
  Twindow Window = (Twindow)0;
  if (Menu && (Window = New(window)(Menu->MsgPort, 0, NULL, (tcolor *)0, Menu, TCOL(tblack, twhite),
                                    NOCURSOR, WINDOW_AUTO_KEYS,
                                    WINDOWFL_MENU | WINDOWFL_USEROWS | WINDOWFL_ROWS_DEFCOL |
                                        WINDOWFL_ROWS_SELCURRENT,
                                    MIN_XWIN, MIN_YWIN, 0))) {

    Window->SetColors(0x1FF, TCOL(0, 0), TCOL(0, 0), TCOL(0, 0), TCOL(0, 0),
                      TCOL(thigh | twhite, twhite), TCOL(tblack, twhite), TCOL(tblack, tgreen),
                      TCOL(thigh | tblack, twhite), TCOL(thigh | tblack, tblack));
    Window->Configure(0x3F, 0, 1, MIN_XWIN, MIN_YWIN, TW_MAXDAT, TW_MAXDAT);
  }
  return Window;
}

byte FakeWriteCharset(Twindow Window, uldat Len, const char *charset_bytes) {
  if (DlLoad(TermSo) && Window->Fn->TtyWriteCharset != FakeWriteCharset)
    return Window->TtyWriteCharset(Len, charset_bytes);
  return tfalse;
}

byte FakeWriteUtf8(Twindow Window, uldat Len, const char *utf8_bytes) {
  if (DlLoad(TermSo) && Window->Fn->TtyWriteUtf8 != FakeWriteUtf8)
    return Window->TtyWriteUtf8(Len, utf8_bytes);
  return tfalse;
}

byte FakeWriteTRune(Twindow Window, uldat Len, const trune *runes) {
  if (DlLoad(TermSo) && Window->Fn->TtyWriteTRune != FakeWriteTRune)
    return Window->TtyWriteTRune(Len, runes);
  return tfalse;
}

byte FakeWriteTCell(Twindow Window, dat x, dat y, uldat Len, const tcell *cells) {
  if (DlLoad(TermSo) && Window->Fn->TtyWriteTCell != FakeWriteTCell)
    return Window->TtyWriteTCell(x, y, Len, cells);
  return tfalse;
}

Twindow FakeOpenTerm(const char *arg0, const char *const *argv) {
  if (DlLoad(TermSo) && Ext(Term, Open) != FakeOpenTerm)
    return Ext(Term, Open)(arg0, argv);
  return NULL;
}

tpos FakeFindBorderWindow(Twindow W, dat u, dat v, byte Border, tcell *PtrAttr) {
  byte Horiz, Vert;

  Horiz = u ? u + 1 == W->XWidth ? (byte)2 : (byte)1 : (byte)0;
  Vert = v ? v + 1 == W->YWidth ? (byte)2 : (byte)1 : (byte)0;

  if (*PtrAttr)
    *PtrAttr = TCELL(W->ColBorder, StdBorder[Border][Vert * 3 + Horiz]);

  return v ? POS_ROOT : POS_TITLE;
}

static Trow FindRow(Twindow Window, ldat Row) {
  Trow CurrRow, ElPossib[4];
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
    ((Tmenuitem)CurrRow)->WCurY = Row;
  return CurrRow;
}

static Trow FindRowByCode(Twindow Window, udat Code, ldat *NumRow) {
  Trow Row;
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

static struct SwindowFn _FnWindow = {
    window_magic,
    (void (*)(Twindow, Twidget, Twidget, Twidget))InsertWidget,
    (void (*)(Twindow))RemoveWidget,
    DeleteWindow,
    ChangeFieldWindow,
    /* Twidget */
    &_FnObj,
    DrawSelfWindow,
    (Twidget(*)(Twindow, dat, dat))FindWidgetAt,
    (Tgadget(*)(Twindow, udat))FindGadgetByCode,
    SetXYWindow,
    (void (*)(Twindow, tcell))SetFillWidget,
    (Twidget(*)(Twindow))FocusWidget,
    (Twidget(*)(Twindow))TtyKbdFocus,
    (void (*)(Twindow, Twidget))MapWidget,
    (void (*)(Twindow))UnMapWidget,
    (void (*)(Twindow, screen))MapTopRealWidget,
    (void (*)(Twindow))RaiseW,
    (void (*)(Twindow))LowerW,
    (void (*)(Twindow, Tmsgport))OwnWidget,
    (void (*)(Twindow))DisOwnWidget,
    (void (*)(Twindow, Tmsgport))RecursiveDeleteWidget,
    (void (*)(Twindow, dat, dat, dat, dat, const char *, const trune *,
              const tcell *))ExposeWindow2, /* exported by resize.c */
    (byte (*)(Twindow, HookFn, void (**)(Twidget)))InstallHookWidget,
    (void (*)(Twindow, HookFn, void (**)(Twidget)))RemoveHookWidget,
    /* Twindow */
    &_FnWidget,
    FakeWriteCharset,
    FakeWriteUtf8,
    FakeWriteTRune,
    FakeWriteTCell,
    RowWriteCharset, /* exported by resize.c */
    RowWriteUtf8,
    RowWriteTRune,
    (byte(*)(Twindow, dat, dat, uldat, const tcell *))AlwaysFalse,

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

static screen CreateSimpleScreen(dat NameLen, const char *Name, tcell Bg) {
  return New(screen)(NameLen, Name, 1, 1, &Bg);
}

static void BgImageScreen(screen Screen, dat BgWidth, dat BgHeight, const tcell *Bg) {
  size_t size;

  if (Screen && S_USE(Screen, USEBG) && Bg && (size = (size_t)BgWidth * BgHeight * sizeof(tcell)) &&
      (Screen->USE.B.Bg = (tcell *)ReAllocMem(Screen->USE.B.Bg, size))) {

    Screen->USE.B.BgWidth = BgWidth;
    Screen->USE.B.BgHeight = BgHeight;
    CopyMem(Bg, Screen->USE.B.Bg, size);
    DrawArea2((screen)0, (Twidget)0, (Twidget)0, 0, Screen->YLimit + 1, TW_MAXDAT, TW_MAXDAT,
              tfalse);
  }
}

static void InsertScreen(screen Screen, all Parent, screen Prev, screen Next) {
  if (!Screen->All && Parent) {
    InsertGeneric((TobjEntry)Screen, (TobjList)&Parent->FirstScreen, (TobjEntry)Prev,
                  (TobjEntry)Next, NULL);
    Screen->All = Parent;
  }
}

static void RemoveScreen(screen Screen) {
  if (Screen->All) {
    RemoveGeneric((TobjEntry)Screen, (TobjList)&Screen->All->FirstScreen, NULL);
    Screen->All = (all)0;
  }
}

static void DeleteScreen(screen Screen) {
  while (Screen->FirstW)
    Screen->FirstW->UnMap();

  Screen->Remove();

  if (S_USE(Screen, USEBG) && Screen->USE.B.Bg) {
    FreeMem(Screen->USE.B.Bg);
    Screen->USE.B.Bg = NULL;
  }
  DeleteWidget((Twidget)Screen);
}

static void ChangeFieldScreen(screen S, udat field, uldat CLEARMask, uldat XORMask) {
  if (S)
    switch (field) {
    default:
      S->Fn->Fn_Widget->ChangeField((Twidget)S, field, CLEARMask, XORMask);
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

static Tmenu FindMenuScreen(screen Screen) {
  if (Screen) {
    if (Screen->MenuWindow && IS_WINDOW(Screen->MenuWindow))
      /* Tmenu activated from Screen->MenuWindow, return its Tmenu */
      return Screen->MenuWindow->Menu;

    /* no Twindow activated the Tmenu... either the Tmenu is inactive
     * or it is activated from the builtin Tmenu */

    if (Screen->FocusW && IS_WINDOW(Screen->FocusW) &&
        ((Twindow)Screen->FocusW)->Menu != All->CommonMenu)
      /* Tmenu inactive... return the focus Twindow's one */
      return ((Twindow)Screen->FocusW)->Menu;

    /* last case: Tmenu activated from builtin Tmenu */
    return All->BuiltinMenu;
  }
  return (Tmenu)0;
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

static Twidget FocusScreen(screen tScreen) {
  screen Screen = All->FirstScreen;
  if (tScreen && Screen != tScreen) {
    MoveFirst(Screen, All, tScreen);
    DrawArea2((screen)0, (Twidget)0, (Twidget)0, 0, Min2(Screen->YLimit, tScreen->YLimit),
              TW_MAXDAT, TW_MAXDAT, tfalse);
    UpdateCursor();
  }
  return (Twidget)Screen;
}

static void ActivateMenuScreen(screen Screen, Tmenuitem Item, byte ByMouse) {

  if ((All->State & state_any) != state_default)
    return;

  if (Screen && Screen != All->FirstScreen)
    Screen->Focus();

  SetMenuState(Item, ByMouse);
}

static void DeActivateMenuScreen(screen Screen) {
  if (Screen == All->FirstScreen && (All->State & state_any) == state_menu)
    CloseMenu();
}

static struct SscreenFn _FnScreen = {
    screen_magic,
    InsertScreen,
    RemoveScreen,
    DeleteScreen,
    ChangeFieldScreen,
    /* Twidget */
    &_FnObj,
    DrawSelfScreen,
    (Twidget(*)(screen, dat, dat))FindWidgetAt,
    (Tgadget(*)(screen, udat))FindGadgetByCode,
    SetXYScreen,
    (void (*)(screen, tcell))SetFillWidget,
    FocusScreen,
    (Twidget(*)(screen))NoOp,        /* KbdFocusWidget */
    (void (*)(screen, Twidget))NoOp, /* MapWidget */
    (void (*)(screen))NoOp,          /* UnMapWidget */
    (void (*)(screen, screen))NoOp,  /* MapTopRealWidget */
    (void (*)(screen))RaiseW,
    (void (*)(screen))LowerW,
    (void (*)(screen, Tmsgport))OwnWidget,
    (void (*)(screen))DisOwnWidget,
    (void (*)(screen, Tmsgport))RecursiveDeleteWidget,
    (void (*)(screen, dat, dat, dat, dat, const char *, const trune *,
              const tcell *))ExposeWidget2, /* exported by resize.c */
    (byte (*)(screen, HookFn, void (**)(Twidget)))InstallHookWidget,
    (void (*)(screen, HookFn, void (**)(Twidget)))RemoveHookWidget,
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

/* Tgroup */

static void InsertGroup(Tgroup group, Tmsgport MsgPort, Tgroup Prev, Tgroup Next) {
  if (!group->MsgPort && MsgPort) {
    InsertGeneric((TobjEntry)group, (TobjList)&MsgPort->FirstGroup, (TobjEntry)Prev,
                  (TobjEntry)Next, NULL);
    group->MsgPort = MsgPort;
  }
}

static void RemoveGroup(Tgroup group) {
  if (group->MsgPort) {
    RemoveGeneric((TobjEntry)group, (TobjList)&group->MsgPort->FirstGroup, NULL);
    group->MsgPort = NULL;
  }
}

static void DeleteGroup(Tgroup group) {
  group->Remove();
  while (group->FirstG)
    group->RemoveGadget(group->FirstG);

  DeleteObj((Tobj)group);
}

static void InsertGadgetGroup(Tgroup group, Tgadget G) {
  if (G && !G->Group && !G->G_Prev && !G->G_Next) {
    if ((G->G_Next = group->FirstG))
      group->FirstG->G_Prev = G;
    else
      group->LastG = G;

    group->FirstG = G;
    G->Group = group;
  }
}

static void RemoveGadgetGroup(Tgroup group, Tgadget G) {
  if (G && G->Group == group) {
    if (G->G_Prev)
      G->G_Prev->G_Next = G->G_Next;
    else if (group->FirstG == G)
      group->FirstG = G->G_Next;

    if (G->G_Next)
      G->G_Next->G_Prev = G->G_Prev;
    else if (group->LastG == G)
      group->LastG = G->G_Prev;

    G->G_Prev = G->G_Next = (Tgadget)0;
    G->Group = (Tgroup)0;
  }
}

static Tgadget GetSelectedGadget(Tgroup group) {
  return group->SelectG;
}

static void SetSelectedGadget(Tgroup group, Tgadget G) {
  if (!G || (G && G->Group == group)) {
    if (group->SelectG)
      UnPressGadget(group->SelectG, ttrue);
    if (G)
      PressGadget(G);
  }
}

static struct SgroupFn _FnGroup = {
    ggroup_magic,
    InsertGroup,
    RemoveGroup,
    DeleteGroup,
    (void (*)(Tgroup, udat, uldat, uldat))NoOp,
    &_FnObj,
    InsertGadgetGroup,
    RemoveGadgetGroup,
    GetSelectedGadget,
    SetSelectedGadget,
};

/* Trow */

static void InsertRow(Trow Row, Twindow Parent, Trow Prev, Trow Next) {
  if (!Row->Window && Parent && W_USE(Parent, USEROWS)) {
    InsertGeneric((TobjEntry)Row, (TobjList)&Parent->USE.R.FirstRow, (TobjEntry)Prev,
                  (TobjEntry)Next, &Parent->HLogic);
    Row->Window = Parent;
    Parent->USE.R.NumRowOne = Parent->USE.R.NumRowSplit = (ldat)0;
  }
}

static void RemoveRow(Trow Row) {
  if (Row->Window && W_USE(Row->Window, USEROWS)) {
    Row->Window->USE.R.NumRowOne = Row->Window->USE.R.NumRowSplit = (ldat)0;
    RemoveGeneric((TobjEntry)Row, (TobjList)&Row->Window->USE.R.FirstRow, &Row->Window->HLogic);
    Row->Window = NULL;
  }
}

static void DeleteRow(Trow Row) {
  if (Row) {
    Twindow W = Row->Window;

    Row->Remove();
    if (Row->Text)
      FreeMem(Row->Text);
    if (Row->ColText)
      FreeMem(Row->ColText);

    DeleteObj((Tobj)Row);

    if (W && W->Parent && (W->Flags & WINDOWFL_MENU))
      ResizeRelWindow(W, 0, -1);
  }
}

static byte SetTextRow(Trow Row, uldat Len, const char *Text, byte DefaultCol) {
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

static byte SetTRuneRow(Trow Row, uldat Len, const trune *TRune, byte DefaultCol) {
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
static void RaiseMenuItem(Tmenuitem M) {
  Tobj Parent;
  Tmenuitem Next;

  if (M && (Parent = (Tobj)M->Parent)) {
    if (IS_MENU(Parent))
      Next = ((Tmenu)Parent)->FirstI;
    else if (IS_WINDOW(Parent) && W_USE((Twindow)Parent, USEROWS))
      Next = (Tmenuitem)((Twindow)Parent)->USE.R.FirstRow;
    else
      return;

    M->Remove();
    M->Insert(Parent, (Tmenuitem)0, Next);

    if (IS_MENU(Parent))
      SyncMenu((Tmenu)Parent);
    else
      DrawAreaWidget((Twidget)Parent);
  }
}

/*
 * this is used also for plain rows.
 * be careful to only access fields that even rows have
 */
static void LowerMenuItem(Tmenuitem M) {
  Tobj Parent;
  Tmenuitem Prev;

  if (M && (Parent = (Tobj)M->Parent)) {
    if (IS_MENU(Parent))
      Prev = ((Tmenu)Parent)->LastI;
    else if (IS_WINDOW(Parent) && W_USE((Twindow)Parent, USEROWS))
      Prev = (Tmenuitem)((Twindow)Parent)->USE.R.LastRow;
    else
      return;

    M->Remove();
    M->Insert(Parent, Prev, NULL);

    if (IS_MENU(Parent))
      SyncMenu((Tmenu)Parent);
    else if (((TobjEntry)Parent)->Parent)
      DrawAreaWidget((Twidget)Parent);
  }
}

static struct SrowFn _FnRow = {
    row_magic,
    InsertRow,
    RemoveRow,
    DeleteRow,
    (void (*)(Trow, udat, uldat, uldat))NoOp,
    /* Trow */
    &_FnObj,
    SetTextRow,
    SetTRuneRow,
    (void (*)(Trow))RaiseMenuItem,
    (void (*)(Trow))LowerMenuItem,
};

byte FindInfo(Tmenu Menu, dat i) {
  Trow Info;

  if (Menu && (Info = Menu->Info) && Info->Len > (udat)i)
    return ttrue;
  return tfalse;
}

/* Tmenuitem */

static void InsertMenuItem(Tmenuitem MenuItem, Tobj Parent, Tmenuitem Prev, Tmenuitem Next) {
  if (!MenuItem->Parent && Parent) {
    if (IS_MENU(Parent)) {
      InsertGeneric((TobjEntry)MenuItem, (TobjList) & ((Tmenu)Parent)->FirstI, (TobjEntry)Prev,
                    (TobjEntry)Next, NULL);
      MenuItem->Parent = Parent;
    } else if (IS_WINDOW(Parent)) {
      InsertRow((Trow)MenuItem, (Twindow)Parent, (Trow)Prev, (Trow)Next);
    }
  }
}

static void RemoveMenuItem(Tmenuitem MenuItem) {
  if (MenuItem->Parent) {
    if (IS_MENU(MenuItem->Parent)) {
      RemoveGeneric((TobjEntry)MenuItem, (TobjList) & ((Tmenu)MenuItem->Parent)->FirstI, NULL);
      MenuItem->Parent = (Tobj)0;
    } else {
      RemoveRow((Trow)MenuItem);
    }
  }
}

static void DeleteMenuItem(Tmenuitem MenuItem) {
  if (MenuItem) {
    Tobj Parent = MenuItem->Parent;

    MenuItem->Remove();
    if (IS_MENU(Parent))
      SyncMenu((Tmenu)Parent);

    if (MenuItem->Window)
      MenuItem->Window->Delete();

    DeleteRow((Trow)MenuItem);
  }
}

Tmenuitem Create4MenuMenuItem(Tobj Parent, Twindow Window, udat Code, byte Flags, ldat Len,
                              const char *Name) {
  dat Left, ShortCut;

  if (!Parent)
    return (Tmenuitem)0;

  if (IS_MENU(Parent) && ((Tmenu)Parent)->LastI)
    Left = ((Tmenu)Parent)->LastI->Left + ((Tmenu)Parent)->LastI->Len;
  else
    Left = (dat)1;

  ShortCut = (dat)0;
  while (ShortCut < Len && Name[ShortCut] == ' ')
    ShortCut++;

  if (Window)
    Window->Left = Left;

  return New(menuitem)(Parent, Window, Code, Flags, Left, Len, ShortCut, Name);
}

/* this returns non-zero for compatibility */
static uldat Create4MenuCommonMenuItem(Tmenu Menu) {
  if (Menu) {
    Menu->CommonItems = ttrue;
    SyncMenu(Menu);
    return (uldat)1;
  }
  return (uldat)0;
}

static struct SmenuitemFn _FnMenuItem = {
    menuitem_magic,
    InsertMenuItem,
    RemoveMenuItem,
    DeleteMenuItem,
    (void (*)(Tmenuitem, udat, uldat, uldat))NoOp,
    &_FnObj,
    SetTextRow,
    SetTRuneRow,
    RaiseMenuItem,
    LowerMenuItem,
    &_FnRow,
    Create4MenuMenuItem,
    Create4MenuCommonMenuItem,
};

/* Tmenu */

static void InsertMenu(Tmenu Menu, Tmsgport MsgPort, Tmenu Prev, Tmenu Next) {
  if (!Menu->MsgPort && MsgPort) {
    InsertGeneric((TobjEntry)Menu, (TobjList)&MsgPort->FirstMenu, (TobjEntry)Prev, (TobjEntry)Next,
                  NULL);
    Menu->MsgPort = MsgPort;
  }
}

static void RemoveMenu(Tmenu Menu) {
  if (Menu->MsgPort) {
    RemoveGeneric((TobjEntry)Menu, (TobjList)&Menu->MsgPort->FirstMenu, NULL);
    Menu->MsgPort = (Tmsgport)0;
  }
}

static void DeleteMenu(Tmenu Menu) {
  uldat count = 30;

  if (Menu) {
    Tmsgport MsgPort = Menu->MsgPort;
    Twidget W, WNext;

    /*
     * warning:
     * the only way to get the list of windows that
     * are using this Tmenu is to scan the whole MsgPort.
     * We must UnMap() all those windows
     * as a Twindow without Tmenu cannot be displayed.
     *
     * optimization: if we are going to UnMap() a lot of windows,
     * we set QueuedDrawArea2FullScreen = ttrue, so that the UnMap()
     * calls do not have to redraw every time.
     */
    if (!QueuedDrawArea2FullScreen) {
      for (W = MsgPort->FirstW; W && count; W = WNext) {
        WNext = W->O_Next;
        if (IS_WINDOW(W) && ((Twindow)W)->Menu == Menu) {
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
      if (IS_WINDOW(W) && ((Twindow)W)->Menu == Menu) {
        if (W->Parent && IS_SCREEN(W->Parent)) {
          W->UnMap();
          ((Twindow)W)->Menu = NULL;
        }
      }
    }
    Menu->Remove();
    DeleteList(Menu->FirstI);
    if (Menu->Info)
      Menu->Info->Delete();

    DeleteObj((Tobj)Menu);
  }
}

static Trow SetInfoMenu(Tmenu Menu, byte Flags, ldat Len, const char *Text, const tcolor *ColText) {
  Trow Row;
  if ((Row = New(row)(0, Flags))) {
    if ((!Text || (Row->Text = CloneStr2TRune(Text, Len))) &&
        (!ColText || (Row->ColText = (tcolor *)CloneMem(ColText, Len * sizeof(tcolor))))) {
      Row->Len = Row->MaxLen = Len;
      if (Menu->Info)
        Menu->Info->Delete();
      return Menu->Info = Row;
    }
    Row->Delete();
    Row = (Trow)0;
  }
  return Row;
}

static Tmenuitem FindItem(Tmenu Menu, dat i) {
  Tmenuitem Item = (Tmenuitem)0;

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
        Item = (Tmenuitem)0;
    }
  }
  return Item;
}

static Tmenuitem GetSelectedItem(Tmenu Menu) {
  if (Menu) {
    if (Menu->SelectI)
      return Menu->SelectI;
    if (Menu->CommonItems && All->CommonMenu)
      return All->CommonMenu->SelectI;
  }
  return (Tmenuitem)0;
}

static Tmenuitem RecursiveGetSelectedItem(Tmenu Menu, dat *depth) {
  Tmenuitem I = NULL, _I = Menu->GetSelectedItem();
  Twindow W = (Twindow)0, FW = (Twindow)All->FirstScreen->FocusW;
  dat d = -1;

  while (_I && IS_MENUITEM(_I)) {
    I = _I;
    d++;
    if (W == FW)
      break;
    if ((W = I->Window) && W->Parent)
      _I = (Tmenuitem)W->FindRow(W->CurY);
    else
      break;
  }
  if (depth)
    *depth = d > 0 ? d : 0;
  return I;
}

static void SetSelectedItem(Tmenu Menu, Tmenuitem Item) {
  if (Menu) {
    if (Item) {
      if (Item->Parent == (Tobj)Menu) {
        Menu->SelectI = Item;
        if (Menu->CommonItems && All->CommonMenu)
          All->CommonMenu->SelectI = (Tmenuitem)0;
      } else if (Menu->CommonItems && Item->Parent == (Tobj)All->CommonMenu) {
        Menu->SelectI = (Tmenuitem)0;
        All->CommonMenu->SelectI = Item;
      }
      /* else Item is not a meaningful one! */
    } else {
      Menu->SelectI = Item;
      if (Menu->CommonItems && All->CommonMenu)
        All->CommonMenu->SelectI = Item;
    }

    All->FirstScreen->DrawMenu(0, TW_MAXDAT);
  }
}

static struct SmenuFn _FnMenu = {
    menu_magic,
    InsertMenu,
    RemoveMenu,
    DeleteMenu,
    (void (*)(Tmenu, udat, uldat, uldat))NoOp,
    /* Tmenu */
    &_FnObj,
    SetInfoMenu,
    FindItem,
    GetSelectedItem,
    RecursiveGetSelectedItem,
    SetSelectedItem,
};

/* Tmsg */

static void InsertMsg(Tmsg msg, Tmsgport Parent, Tmsg Prev, Tmsg Next) {
  if (!msg->MsgPort && Parent) {
    /* if adding the first Tmsg, move the Tmsgport to the head
     * of Tmsgport list, so that the scheduler will run it */
    if (!Parent->FirstMsg && Parent->All)
      MoveFirst(MsgPort, All, Parent);

    InsertGeneric((TobjEntry)msg, (TobjList)&Parent->FirstMsg, (TobjEntry)Prev, (TobjEntry)Next,
                  NULL);
    msg->MsgPort = Parent;
  }
}

static void RemoveMsg(Tmsg msg) {
  if (msg->MsgPort) {
    RemoveGeneric((TobjEntry)msg, (TobjList)&msg->MsgPort->FirstMsg, NULL);
    msg->MsgPort = NULL;
  }
}

static void DeleteMsg(Tmsg msg) {
  if (msg) {
    msg->Remove();
    DeleteObj((Tobj)msg);
  }
}

static struct SmsgFn _FnMsg = {
    msg_magic,
    InsertMsg,
    RemoveMsg,
    DeleteMsg,
    (void (*)(Tmsg, udat, uldat, uldat))NoOp,
    /* Tmsg */
    &_FnObj,
};

/* Tmsgport */

static void InsertMsgPort(Tmsgport MsgPort, all Parent, Tmsgport Prev, Tmsgport Next) {
  if (!MsgPort->All && Parent) {
    InsertGeneric((TobjEntry)MsgPort, (TobjList)&Parent->FirstMsgPort, (TobjEntry)Prev,
                  (TobjEntry)Next, NULL);
    MsgPort->All = Parent;
  }
}

static void RemoveMsgPort(Tmsgport MsgPort) {
  if (MsgPort->All) {
    if (All->RunMsgPort == MsgPort)
      All->RunMsgPort = MsgPort->Next;
    RemoveGeneric((TobjEntry)MsgPort, (TobjList)&MsgPort->All->FirstMsgPort, NULL);
    MsgPort->All = (all)0;
  }
}

static void DeleteMsgPort(Tmsgport MsgPort) {
  uldat count = 20;
  Twidget W;

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
     * Tmenuitem windows, which cannot be deleted before deleting
     * the corresponding Tmenuitem.
     */
    DeleteList(MsgPort->FirstMsg);
    DeleteList(MsgPort->FirstMenu);
    DeleteList(MsgPort->FirstW);
    DeleteList(MsgPort->FirstGroup);
    DeleteList(MsgPort->FirstMutex);

    MsgPort->Remove();
    if (MsgPort->Name)
      FreeMem(MsgPort->Name);

    DeleteObj((Tobj)MsgPort);
  }
}

static struct SmsgportFn _FnMsgPort = {
    msgport_magic,
    InsertMsgPort,
    RemoveMsgPort,
    DeleteMsgPort,
    (void (*)(Tmsgport, udat, uldat, uldat))NoOp,
    /* Tmsgport */
    &_FnObj,
};

/* Tmutex */

static void InsertMutex(Tmutex Mutex, all Parent, Tmutex Prev, Tmutex Next) {
  if (!Mutex->All && Parent) {
    InsertGeneric((TobjEntry)Mutex, (TobjList)&Mutex->All->FirstMutex, (TobjEntry)Prev,
                  (TobjEntry)Next, NULL);
    Mutex->All = Parent;
  }
}

static void RemoveMutex(Tmutex Mutex) {
  if (Mutex->All) {
    RemoveGeneric((TobjEntry)Mutex, (TobjList)&Mutex->All->FirstMutex, NULL);
    Mutex->All = (all)0;
  }
}

static void DeleteMutex(Tmutex Mutex) {
  Mutex->DisOwn();
  Mutex->Remove();
  DeleteObj((Tobj)Mutex);
}

static void OwnMutex(Tmutex Mutex, Tmsgport Parent) {
  if (!Mutex->Owner && Parent) {

    if ((Mutex->O_Prev = Parent->LastMutex))
      Parent->LastMutex->O_Next = Mutex;
    else
      Parent->FirstMutex = Mutex;

    Mutex->O_Next = (Tmutex)0;
    Parent->LastMutex = Mutex;

    Mutex->Owner = Parent;
  }
}

static void DisOwnMutex(Tmutex Mutex) {
  Tmsgport Parent;
  if ((Parent = Mutex->Owner)) {
    if (Mutex->O_Prev)
      Mutex->O_Prev->O_Next = Mutex->O_Next;
    else if (Parent->FirstMutex == Mutex)
      Parent->FirstMutex = Mutex->O_Next;

    if (Mutex->O_Next)
      Mutex->O_Next->O_Prev = Mutex->O_Prev;
    else if (Parent->LastMutex == Mutex)
      Parent->LastMutex = Mutex->O_Prev;

    Mutex->O_Prev = Mutex->O_Next = (Tmutex)0;

    Mutex->Owner = (Tmsgport)0;
  }
}

static struct SmutexFn _FnMutex = {
    mutex_magic,
    InsertMutex,
    RemoveMutex,
    DeleteMutex,
    (void (*)(Tmutex, udat, uldat, uldat))NoOp,
    /* Tmutex */
    &_FnObj,
    OwnMutex,
    DisOwnMutex,
};

/* Tmodule */

static void InsertModule(Tmodule Module, all Parent, Tmodule Prev, Tmodule Next) {
  if (!Module->All && Parent) {
    InsertGeneric((TobjEntry)Module, (TobjList)&Parent->FirstModule, (TobjEntry)Prev,
                  (TobjEntry)Next, NULL);
    Module->All = Parent;
  }
}

static void RemoveModule(Tmodule Module) {
  if (Module->All) {
    RemoveGeneric((TobjEntry)Module, (TobjList)&Module->All->FirstModule, NULL);
    Module->All = (all)0;
  }
}

static void DeleteModule(Tmodule Module) {
  if (!Module->Used) {

    Module->DlClose();
    Module->Remove();
    if (Module->Name)
      FreeMem(Module->Name);

    DeleteObj((Tobj)Module);
  }
}

static struct SmoduleFn _FnModule = {
    module_magic,
    InsertModule,
    RemoveModule,
    DeleteModule,
    (void (*)(Tmodule, udat, uldat, uldat))NoOp,
    /* Tmodule */
    &_FnObj,
    DlOpen,
    DlClose,
};

/* Tdisplay */

static void InsertDisplayHW(Tdisplay DisplayHW, all Parent, Tdisplay Prev, Tdisplay Next) {
  if (!DisplayHW->All && Parent) {
    InsertGeneric((TobjEntry)DisplayHW, (TobjList)&Parent->FirstDisplayHW, (TobjEntry)Prev,
                  (TobjEntry)Next, NULL);
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

static void RemoveDisplayHW(Tdisplay DisplayHW) {
  if (DisplayHW->All) {
    RemoveGeneric((TobjEntry)DisplayHW, (TobjList)&DisplayHW->All->FirstDisplayHW, NULL);
    DisplayHW->All = (all)0;

    if (All->FnHookDisplayHW)
      All->FnHookDisplayHW(All->HookDisplayHW);
  }
}

static void DeleteDisplayHW(Tdisplay DisplayHW) {
  byte isCTTY = DisplayHW->DisplayIsCTTY && DisplayHW == DisplayHWCTTY;
  byte Quitted = DisplayHW->Quitted;

  if (!Quitted)
    DisplayHW->DoQuit();

  /* avoid getting stale pointers */
  if (All->MouseHW == DisplayHW)
    All->MouseHW = NULL;
  if (All->ExclusiveHW == DisplayHW)
    All->ExclusiveHW = NULL;

  DisplayHW->Remove();
  String().swap(DisplayHW->Name); // destroy DisplayHW->Name

  DeleteObj((Tobj)DisplayHW);

  if (!Quitted) {
    if (!All->FirstDisplayHW || isCTTY)
      RunNoHW(tfalse);
    else if (All->FirstDisplayHW && ResizeDisplay()) {
      QueuedDrawArea2FullScreen = ttrue;
    }
  }
}

static struct SdisplayFn _FnDisplayHW = {
    display_hw_magic,
    InsertDisplayHW,
    RemoveDisplayHW,
    DeleteDisplayHW,
    (void (*)(Tdisplay, udat, uldat, uldat))NoOp,
    /* Tdisplay */
    &_FnObj,
    InitDisplayHW,
    QuitDisplayHW,
};

TstructFn FnStruct = {
    &_FnObj,      &_FnWidget, &_FnGadget,  &_FnWindow, &_FnScreen, &_FnGroup,  &_FnRow,
    &_FnMenuItem, &_FnMenu,   &_FnMsgPort, &_FnMutex,  &_FnMsg,    &_FnModule, &_FnDisplayHW,
};
