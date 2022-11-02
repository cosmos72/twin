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
inline void DeletePartialList(Tobj obj) {
  Tobj Next;
  while (obj) {
    Next = obj->Next;
    obj->Delete();
    obj = Next;
  }
}
#endif

inline void InsertGeneric(TobjEntry obj, TobjList parent, TobjEntry prev, TobjEntry Next,
                          ldat *ObjCount) {
  if (obj->Prev || obj->Next)
    return;

  if (prev)
    Next = prev->Next;
  else if (Next)
    prev = Next->Prev;

  if ((obj->Prev = prev))
    prev->Next = obj;
  else
    parent->First = obj;

  if ((obj->Next = Next))
    Next->Prev = obj;
  else
    parent->Last = obj;

  if (ObjCount)
    (*ObjCount)++;
}

inline void RemoveGeneric(TobjEntry obj, TobjList parent, ldat *ObjCount) {
  if (obj->Prev)
    obj->Prev->Next = obj->Next;
  else if (parent->First == obj)
    parent->First = obj->Next;

  if (obj->Next)
    obj->Next->Prev = obj->Prev;
  else if (parent->Last == obj)
    parent->Last = obj->Prev;

  obj->Prev = obj->Next = NULL;
  if (ObjCount)
    (*ObjCount)--;
}

/* TobjFn and others fn_XXX functions */

/* Tobj */

static void InsertObj(Tobj obj, Tobj parent, Tobj Prev, Tobj Next) {
  log(ERROR) << "twin: internal error: pure virtual function InsertObj() called!\n";
}

static void RemoveObj(Tobj obj) {
  log(ERROR) << "twin: internal error: pure virtual function RemoveObj() called!\n";
}

static void DeleteObj(Tobj obj) {
  /* not a good idea to RemoveObj(obj) here */
  DropId(obj);
  FreeMem(obj);
}

static struct SobjFn _FnObj = {
    obj_magic,
    InsertObj,
    RemoveObj,
    DeleteObj,
};

/* Twidget */

static void InsertWidget(Twidget w, Twidget parent, Twidget Prev, Twidget Next) {
  if (parent) {
    /*
     * don't check w->Parent here, as Raise() and Lower() call
     * RemoveWidget() then InsertWidget() but RemoveWidget() does not reset w->Parent
     */
    InsertGeneric((TobjEntry)w, (TobjList)&parent->FirstW, (TobjEntry)Prev, (TobjEntry)Next, NULL);
  }
}

static void RemoveWidget(Twidget w) {
  if (w->Parent) {
    RemoveGeneric((TobjEntry)w, (TobjList)&w->Parent->FirstW, NULL);
  }
}

static void DeleteWidget(Twidget w) {
  w->UnMap();
  if (w->Hook) {
    w->RemoveHook(w->Hook, w->WhereHook);
  }
  if (w->ShutDownHook) {
    w->ShutDownHook(w);
  }
  w->DisOwn();
  while (w->FirstW) {
    w->FirstW->UnMap();
  }
  DeleteObj((Tobj)w);
}

void SetFillWidget(Twidget w, tcell Fill) {
  if (w->USE_Fill != Fill) {
    w->USE_Fill = Fill;
    if (w->Parent)
      DrawAreaWidget(w);
  }
}

void IncMouseMotionN(void) {
  if (!All->MouseMotionN++)
    EnableMouseMotionEvents(ttrue);
}

void DecMouseMotionN(void) {
  if (All->MouseMotionN && !--All->MouseMotionN)
    EnableMouseMotionEvents(tfalse);
}

Twidget FocusWidget(Twidget w) {
  Twidget oldW;
  if (w)
    oldW = w->KbdFocus();
  else
    oldW = Do(KbdFocus, widget)(NULL);

  if (w != oldW && (!w || w->Parent == (Twidget)All->FirstScreen)) {
    if (w && IS_WINDOW(w))
      DrawBorderWindow((Twindow)w, BORDER_ANY);
    if (oldW && IS_WINDOW(oldW))
      DrawBorderWindow((Twindow)oldW, BORDER_ANY);
    if ((w && IS_WINDOW(w)) || (oldW && IS_WINDOW(oldW))) {
      UpdateCursor();
      if (!w || !IS_WINDOW(w) || !(((Twindow)w)->Flags & WINDOWFL_MENU))
        All->FirstScreen->DrawMenu(0, TW_MAXDAT);
    }
  }
  return oldW;
}

#define TtyKbdFocus FakeKbdFocus
Twidget FakeKbdFocus(Twidget w) {
  Twidget oldW;
  Twidget P;
  Tscreen screen = w && (P = w->Parent) && IS_SCREEN(P) ? (Tscreen)P : All->FirstScreen;

  if (screen) {
    oldW = screen->FocusW();
    screen->FocusW(w);
  } else
    oldW = (Twidget)0;

  return oldW;
}

static Tgadget FindGadgetByCode(Twidget parent, udat Code) {
  Twidget w;

  for (w = parent->FirstW; w; w = w->Next) {
    if (IS_GADGET(w) && ((Tgadget)w)->Code == Code)
      return (Tgadget)w;
  }
  return (Tgadget)0;
}

static void MapWidget(Twidget w, Twidget parent) {
  Tmsg msg;

  if (w && parent && !w->Parent && !w->MapQueueMsg) {
    if (IS_SCREEN(parent)) {
      if (Ext(WM, MsgPort) && (msg = New(msg)(msg_map, 0))) {
        msg->Event.EventMap.W = w;
        msg->Event.EventMap.Code = 0;
        msg->Event.EventMap.Screen = (Tscreen)parent;
        w->MapQueueMsg = msg;
        SendMsg(Ext(WM, MsgPort), msg);
      } else
        w->MapTopReal((Tscreen)parent);
    } else if (IS_WIDGET(parent)) {
      if (w->Up == TW_MAXDAT) {
        w->Left = parent->XLogic;
        w->Up = parent->YLogic;
      }
      InsertFirst(W, w, parent);
      w->Parent = parent;

      DrawAreaWidget(w);

      if (w->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        IncMouseMotionN();

      if (w->MapUnMapHook)
        w->MapUnMapHook(w);
    }
  }
}

static void MapTopRealWidget(Twidget w, Tscreen screen) {
  Twidget OldW;

  if (screen && !w->Parent && (!IS_WINDOW(w) || ((Twindow)w)->Menu)) {
    if (w->MapQueueMsg)
      /*
       * let the upper layer do this:
       * w->MapQueueMsg->Delete();
       */
      w->MapQueueMsg = (Tmsg)0;

    if (w->Up == TW_MAXDAT) {
      w->Left = screen->XLogic;
      w->Up = Max2(screen->Up + 1, 0) + screen->YLogic;
    } else {
      w->Left += screen->XLogic;
      w->Up += screen->YLogic;
    }

    InsertFirst(W, w, (Twidget)screen);
    w->Parent = (Twidget)screen;

    /* top-level widgets must be visible */
    w->Flags &= ~WINDOWFL_NOTVISIBLE;

    if (w->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
      IncMouseMotionN();

    if (screen == All->FirstScreen) {
      OldW = w->KbdFocus();
      if (OldW && IS_WINDOW(OldW))
        DrawBorderWindow((Twindow)OldW, BORDER_ANY);
      UpdateCursor();
    }
    if (IS_WINDOW(w))
      DrawAreaWindow2((Twindow)w);
    else
      DrawAreaWidget(w);
    if (!(w->Flags & WINDOWFL_MENU))
      screen->DrawMenu(0, TW_MAXDAT);

    if (w->MapUnMapHook)
      w->MapUnMapHook(w);

    if (screen->FnHookW)
      screen->FnHookW(screen->HookW);
  }
}

static void UnMapWidget(Twidget w) {
  Twidget Parent;
  Twindow Next;
  Tscreen screen;
  byte wasFocus;

  if (w && (Parent = w->Parent)) {
    if (IS_SCREEN(Parent)) {
      if ((screen = (Tscreen)Parent) == All->FirstScreen && w == (Twidget)screen->MenuWindow) {
        /*
         * ! DANGER !
         * Trying to UnMap() the Tmenu owner.
         * shutdown the Tmenu first!
         */
        CloseMenu();
      }

      if (w->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        DecMouseMotionN();

      if (screen->ClickWindow == (Twindow)w)
        screen->ClickWindow = NULL;

      if ((wasFocus = w == screen->FocusW())) {
        if (w->Flags & WINDOWFL_MENU)
          Next = screen->MenuWindow;
        else {
          if ((Twidget)w == screen->FirstW)
            Next = (Twindow)w->Next;
          else
            Next = (Twindow)screen->FirstW;

          while (Next && !IS_WINDOW(Next))
            Next = (Twindow)Next->Next;
        }
      }

      w->Remove();
      if (IS_WINDOW(w))
        DrawAreaWindow2((Twindow)w);
      else
        DrawAreaWidget(w);

      if (IS_SCREEN(Parent)) {
        w->Left = 0;
        w->Up = TW_MAXDAT;
      }
      w->Parent = (Twidget)0;

      if (wasFocus) {
        if (screen == All->FirstScreen) {
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
          if (!(w->Flags & WINDOWFL_MENU))
            screen->DrawMenu(0, TW_MAXDAT);
          UpdateCursor();
        } else
          screen->FocusW(Next);
      }

      if (w->MapUnMapHook)
        w->MapUnMapHook(w);

      if (screen->FnHookW)
        screen->FnHookW(screen->HookW);

    } else {
      /* UnMap() a sub-window */
      if (w == Parent->SelectW)
        Parent->SelectW = (Twidget)0;

      w->Remove();
      DrawAreaWidget(w);
      w->Parent = (Twidget)0;

      if (w->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        DecMouseMotionN();

      if (w->MapUnMapHook)
        w->MapUnMapHook(w);
    }
  } else if (w->MapQueueMsg) {
    /* the window was still waiting to be mapped! */
    w->MapQueueMsg->Delete();
    w->MapQueueMsg = (Tmsg)0;
  }
}

static void RaiseW(Twidget w) {
  RaiseWidget(w, tfalse);
}

static void LowerW(Twidget w) {
  LowerWidget(w, tfalse);
}

static void SetXYWidget(Twidget w, dat x, dat y) {
  Twidget Prev, Next;

  if (w->Parent) {
    Prev = w->Prev;
    Next = w->Next;
    w->Remove();
    DrawAreaWidget(w);
  }
  w->Left = x;
  w->Up = y;
  if (w->Parent) {
    InsertMiddle(w, w, w->Parent, Prev, Next);
    DrawAreaWidget(w);
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

static void DisOwnWidget(Twidget w) {
  Tmsgport Owner;
  if ((Owner = w->Owner)) {
    if (w->O_Prev)
      w->O_Prev->O_Next = w->O_Next;
    else if (Owner->FirstW == w)
      Owner->FirstW = w->O_Next;

    if (w->O_Next)
      w->O_Next->O_Prev = w->O_Prev;
    else if (Owner->LastW == w)
      Owner->LastW = w->O_Prev;

    w->O_Prev = w->O_Next = (Twidget)0;
    w->Owner = (Tmsgport)0;
  }
}

static void RecursiveDeleteWidget(Twidget w, Tmsgport maybeOwner) {
  while (w->FirstW)
    w->FirstW->RecursiveDelete(maybeOwner);

  if (w->Owner == maybeOwner)
    w->Delete();
  else
    w->UnMap();
}

static byte InstallHookWidget(Twidget w, HookFn Hook, HookFn *WhereHook) {
  if (w && !w->Hook && !w->WhereHook && Hook && WhereHook && !WhereHook[0] && !WhereHook[1]) {

    w->Hook = WhereHook[0] = Hook;
    w->WhereHook = WhereHook;
    WhereHook[1] = (HookFn)w;
    return ttrue;
  }
  return tfalse;
}

static void RemoveHookWidget(Twidget w, HookFn Hook, HookFn *WhereHook) {
  if (w && Hook && w->Hook == Hook && WhereHook && w->WhereHook == WhereHook &&
      WhereHook[0] == Hook && WhereHook[1] == (void *)w) {

    w->Hook = *WhereHook = (HookFn)0;
    w->WhereHook = (HookFn *)0;
    WhereHook[1] = (HookFn)0;
  }
}

static struct SwidgetFn _FnWidget = {
    widget_magic,      InsertWidget,     RemoveWidget,          DeleteWidget, //
    &_FnObj,           DrawSelfWidget, /* exported by draw.c */
    FindWidgetAt,                      /* exported by draw.c */
    FindGadgetByCode,  SetXYWidget,      SetFillWidget,         FocusWidget,   TtyKbdFocus,
    MapWidget,         UnMapWidget,      MapTopRealWidget,      RaiseW,        LowerW,
    OwnWidget,         DisOwnWidget,     RecursiveDeleteWidget, ExposeWidget2, /* exported by
                                                                                  resize.c */
    InstallHookWidget, RemoveHookWidget,
};

/* Tgadget */

static void DeleteGadget(Tgadget g) {
  byte i;

  g->UnMap();
  if (G_USE(g, USETEXT)) {
    for (i = 0; i < 4; i++) {
      if (g->USE.T.Text[i])
        FreeMem(g->USE.T.Text[i]);
      if (g->USE.T.Color[i])
        FreeMem(g->USE.T.Color[i]);
    }
  }
  if (g->Group) {
    g->Group->RemoveGadget(g);
  }

  DeleteWidget((Twidget)g);
}

static Tgadget CreateEmptyButton(Tmsgport Owner, dat XWidth, dat YWidth, tcolor BgCol) {
  Tgadget g = NULL;
  ldat Size;
  byte i;
  dat j, k;
#define _FULL T_UTF_32_FULL_BLOCK
#define _LOWER T_UTF_32_LOWER_HALF_BLOCK
#define _UPPER T_UTF_32_UPPER_HALF_BLOCK
  void *addr = AllocMem0(sizeof(Sgadget));
  if (addr) {
    g = new (addr) Sgadget();
    g->Fn = (TwidgetFn)Fn_Tgadget;
    if (!((Twidget)g)
             ->Init(Owner, ++XWidth, ++YWidth, 0, GADGETFL_USETEXT | GADGETFL_BUTTON, 0, 0,
                    (tcell)0)) {
      g->Delete();
      return NULL;
    }

    Size = (ldat)XWidth * YWidth;

    for (i = 0; i < 4; i++)
      g->USE.T.Text[i] = NULL, g->USE.T.Color[i] = NULL;

    for (i = 0; i < 4; i++)
      if (!(g->USE.T.Text[i] = (trune *)AllocMem(Size * sizeof(trune))) ||
          !(g->USE.T.Color[i] = (tcolor *)AllocMem(Size * sizeof(tcolor)))) {

        g->Delete();
        return NULL;
      }

    Size = (ldat)--XWidth * --YWidth;
    BgCol &= TCOL(0, tmaxcol);

    for (i = 0; i < 4; i++) {
      for (j = k = (dat)0; j < YWidth; j++, k += XWidth + 1) {
        g->USE.T.Text[i][k + (i & 1 ? 0 : XWidth)] = i & 1 ? ' ' : k ? _FULL : _LOWER;
        g->USE.T.Color[i][k + (i & 1 ? 0 : XWidth)] = BgCol;
      }
      g->USE.T.Text[i][k] = ' ';
      for (j = (dat)0; j < XWidth; j++)
        g->USE.T.Text[i][k + 1 + j] = i & 1 ? ' ' : _UPPER;
#if TW_SIZEOF_TCOLOR == 1
      memset((void *)(g->USE.T.Color[i] + k), BgCol, XWidth + 1);
#else
      for (j = (dat)0; j <= XWidth; j++)
        g->USE.T.Color[i][k + j] = BgCol;
#endif
    }

    g->G_Prev = g->G_Next = (Tgadget)0;
    g->Group = (Tgroup)0;
  }
  return g;
#undef _FULL
#undef _UPPER
#undef _LOWER
}

byte FillButton(Tgadget g, Twidget Parent, udat Code, dat Left, dat Up, udat Flags,
                const char *Text, tcolor Color, tcolor ColorDisabled) {
  dat i, j, XWidth, YWidth;
  const char *T;

  if (Code >= COD_RESERVED)
    return tfalse;

  g->Code = Code;
  g->Left = Left;
  g->Up = Up;
  g->Flags = (Flags & ~GADGETFL_USEANY) | GADGETFL_USETEXT | GADGETFL_BUTTON;
  XWidth = g->XWidth;
  YWidth = g->YWidth;

  T = Text;
  for (j = (dat)0; j < (YWidth - (dat)1) * XWidth; j += XWidth) {
    for (i = (dat)0; i < XWidth - (dat)1; i++) {
      g->USE.T.Text[0][i + j] = g->USE.T.Text[1][i + j + 1] = g->USE.T.Text[2][i + j] =
          g->USE.T.Text[3][i + j + 1] = Tutf_CP437_to_UTF_32[(byte) * (T++)];

      g->USE.T.Color[0][i + j] = g->USE.T.Color[1][i + j + 1] = Color;
      g->USE.T.Color[2][i + j] = g->USE.T.Color[3][i + j + 1] = ColorDisabled;
    }
  }
  if (Parent)
    g->Map(Parent);

  return ttrue;
}

static Tgadget CreateButton(Twidget Parent, dat XWidth, dat YWidth, const char *Text, uldat Flags,
                            udat Code, tcolor BgCol, tcolor Col, tcolor ColDisabled, dat Left,
                            dat Up) {
  Tgadget g;
  if (Parent && (g = CreateEmptyButton(Parent->Owner, XWidth, YWidth, BgCol))) {
    if (g->FillButton(Parent, Code, Left, Up, Flags, Text, Col, ColDisabled))
      return g;
    g->Delete();
    g = NULL;
  }
  return g;
}

static struct SgadgetFn _FnGadget = {
    gadget_magic,                                               //
    (void (*)(Tgadget, Twidget, Twidget, Twidget))InsertWidget, //
    (void (*)(Tgadget))RemoveWidget, DeleteGadget,
    /* Twidget */
    &_FnObj, DrawSelfGadget,                      /* exported by draw.c */
    (Twidget (*)(Tgadget, dat, dat))FindWidgetAt, /* exported by draw.c */
    (Tgadget (*)(Tgadget, udat))FindGadgetByCode, (void (*)(Tgadget, dat, dat))SetXYWidget,
    (void (*)(Tgadget, tcell))SetFillWidget, (Twidget(*)(Tgadget))FocusWidget,
    (Twidget(*)(Tgadget))TtyKbdFocus, (void (*)(Tgadget, Twidget))MapWidget,
    (void (*)(Tgadget))UnMapWidget, (void (*)(Tgadget, Tscreen))MapTopRealWidget,
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

static void DeleteWindow(Twindow w) {
  w->UnMap();
  if (w->Name)
    FreeMem(w->Name);
  if (w->ColName)
    FreeMem(w->ColName);
  if (W_USE(w, USECONTENTS)) {
    if (w->USE.C.TtyData)
      FreeMem(w->USE.C.TtyData);
    if (w->USE.C.Contents)
      FreeMem(w->USE.C.Contents);
  } else if (W_USE(w, USEROWS)) {
    DeleteList(w->USE.R.FirstRow);
  }
  DeleteWidget((Twidget)w);
}

static void SetTitleWindow(Twindow w, dat titlelen, char *title) {
  Twidget P;

  if (w->Name)
    FreeMem(w->Name);

  w->NameLen = titlelen;
  w->Name = title;

#if 1
  /*
   * do not allow changing Twindow borders just because
   * some untrusted application set a new title
   */
  DrawBorderWindow(w, BORDER_UP);
#else
  /* user may have title-dependent borders in ~/.config/twin/twinrc, honour them: */
  Win->BorderPattern[0] = Win->BorderPattern[1] = NULL;
  DrawBorderWindow(w, BORDER_ANY);
#endif

  if ((P = w->Parent) && IS_SCREEN(P)) {
    /* need to update Twindow list with new name ? */
    if (((Tscreen)P)->FnHookW)
      ((Tscreen)P)->FnHookW(((Tscreen)P)->HookW);
  }
}

static void SetColTextWindow(Twindow w, tcolor ColText) {
  w->ColText = ColText;
}

static void SetColorsWindow(Twindow w, udat Bitmap, tcolor ColGadgets, tcolor ColArrows,
                            tcolor ColBars, tcolor ColTabs, tcolor ColBorder, tcolor ColText,
                            tcolor ColSelect, tcolor ColDisabled, tcolor ColSelectDisabled) {
  if (Bitmap & 1)
    w->ColGadgets = ColGadgets;
  if (Bitmap & 2)
    w->ColArrows = ColArrows;
  if (Bitmap & 4)
    w->ColBars = ColBars;
  if (Bitmap & 8)
    w->ColTabs = ColTabs;
  if (Bitmap & 0x10)
    w->ColBorder = ColBorder;
  if (Bitmap & 0x20) {
    w->ColText = ColText;
    if (W_USE(w, USECONTENTS))
      w->USE.C.TtyData->Color = ColText;
  }
  if (Bitmap & 0x40)
    w->ColSelect = ColSelect;
  if (Bitmap & 0x80)
    w->ColDisabled = ColDisabled;
  if (Bitmap & 0x100)
    w->ColSelectDisabled = ColSelectDisabled;
  if (w->Parent)
    DrawBorderWindow(w, BORDER_ANY);
}

static void SetXYWindow(Twindow w, dat x, dat y) {
  Twidget Prev, Next;

  if (w->Parent) {
    Prev = w->Prev;
    Next = w->Next;
    w->Remove();
    DrawAreaWindow2(w);
  }
  w->Left = x;
  w->Up = y;
  if (w->Parent) {
    if (IS_SCREEN(w->Parent)) {
      w->Left += w->Parent->XLogic;
      w->Up += w->Parent->YLogic;
    }
    InsertMiddle(w, w, w->Parent, Prev, Next);
    DrawAreaWindow2(w);
  }
}

static void ConfigureWindow(Twindow w, byte Bitmap, dat Left, dat Up, dat MinXWidth, dat MinYWidth,
                            dat MaxXWidth, dat MaxYWidth) {
  Twidget Prev, Next;
  dat HasBorder = 2 * !(w->Flags & WINDOWFL_BORDERLESS);

  if (w->Parent) {
    Prev = w->Prev;
    Next = w->Next;
    w->Remove();
    DrawAreaWindow2(w);
  }

  if (Bitmap & 1) {
    w->Left = Left;
    if (w->Parent && IS_SCREEN(w->Parent))
      w->Left += w->Parent->XLogic;
  }
  if (Bitmap & 2) {
    w->Up = Up;
    if (w->Parent && IS_SCREEN(w->Parent))
      w->Up += w->Parent->YLogic;
  }

  if (Bitmap & 4) {
    if (MinXWidth <= TW_MAXDAT - HasBorder)
      MinXWidth = Max2(MinXWidth, MinXWidth + HasBorder);
    w->MinXWidth = MinXWidth;
    w->XWidth = Max2(MinXWidth, w->XWidth);
  }
  if (Bitmap & 8) {
    if (MinYWidth <= TW_MAXDAT - HasBorder)
      MinYWidth = Max2(MinYWidth, MinYWidth + HasBorder);
    w->MinYWidth = MinYWidth;
    w->YWidth = Max2(MinYWidth, w->YWidth);
  }
  if (Bitmap & 0x10) {
    if (MaxXWidth <= TW_MAXDAT - HasBorder)
      MaxXWidth = Max2(MaxXWidth, MaxXWidth + HasBorder);
    w->MaxXWidth = Max2(w->MinXWidth, MaxXWidth);
    w->XWidth = Min2(MaxXWidth, w->XWidth);
  }
  if (Bitmap & 0x20) {
    if (MaxYWidth <= TW_MAXDAT - HasBorder)
      MaxYWidth = Max2(MaxYWidth, MaxYWidth + HasBorder);
    w->MaxYWidth = Max2(w->MinYWidth, MaxYWidth);
    w->YWidth = Min2(MaxYWidth, w->YWidth);
  }
  if (w->Parent) {
    InsertMiddle(w, w, w->Parent, Prev, Next);
    DrawAreaWindow2(w);
  }
}

static void GotoXYWindow(Twindow window, ldat x, ldat y) {
  if (W_USE(window, USECONTENTS)) {
    ttydata *TT = window->USE.C.TtyData;

    x = Max2(x, 0);
    y = Max2(y, 0);
    if (x >= TT->SizeX)
      x = TT->SizeX - 1;
    if (y >= TT->SizeY)
      y = TT->SizeY - 1;
    TT->X = x;
    TT->Y = y;
    TT->Pos = TT->Start + x + (ldat)y * TT->SizeX;
    if (TT->Pos >= TT->Split)
      TT->Pos -= TT->Split - window->USE.C.Contents;
    y += window->HLogic - TT->SizeY;
  }
  window->CurX = x;
  window->CurY = y;
  if (ContainsCursor((Twidget)window))
    UpdateCursor();
}

Twindow Create4MenuWindow(Tmenu Menu) {
  Twindow window = (Twindow)0;
  if (Menu && (window = New(window)(Menu->MsgPort, 0, NULL, (tcolor *)0, Menu, TCOL(tblack, twhite),
                                    NOCURSOR, WINDOW_AUTO_KEYS,
                                    WINDOWFL_MENU | WINDOWFL_USEROWS | WINDOWFL_ROWS_DEFCOL |
                                        WINDOWFL_ROWS_SELCURRENT,
                                    MIN_XWIN, MIN_YWIN, 0))) {

    window->SetColors(0x1FF, TCOL(0, 0), TCOL(0, 0), TCOL(0, 0), TCOL(0, 0),
                      TCOL(thigh | twhite, twhite), TCOL(tblack, twhite), TCOL(tblack, tgreen),
                      TCOL(thigh | tblack, twhite), TCOL(thigh | tblack, tblack));
    window->Configure(0x3F, 0, 1, MIN_XWIN, MIN_YWIN, TW_MAXDAT, TW_MAXDAT);
  }
  return window;
}

byte FakeWriteCharset(Twindow window, uldat Len, const char *charset_bytes) {
  if (DlLoad(TermSo) && window->fn()->TtyWriteCharset != FakeWriteCharset)
    return window->TtyWriteCharset(Len, charset_bytes);
  return tfalse;
}

byte FakeWriteUtf8(Twindow window, uldat Len, const char *utf8_bytes) {
  if (DlLoad(TermSo) && window->fn()->TtyWriteUtf8 != FakeWriteUtf8)
    return window->TtyWriteUtf8(Len, utf8_bytes);
  return tfalse;
}

byte FakeWriteTRune(Twindow window, uldat Len, const trune *runes) {
  if (DlLoad(TermSo) && window->fn()->TtyWriteTRune != FakeWriteTRune)
    return window->TtyWriteTRune(Len, runes);
  return tfalse;
}

byte FakeWriteTCell(Twindow window, dat x, dat y, uldat Len, const tcell *cells) {
  if (DlLoad(TermSo) && window->fn()->TtyWriteTCell != FakeWriteTCell)
    return window->TtyWriteTCell(x, y, Len, cells);
  return tfalse;
}

Twindow FakeOpenTerm(const char *arg0, const char *const *argv) {
  if (DlLoad(TermSo) && Ext(Term, Open) != FakeOpenTerm)
    return Ext(Term, Open)(arg0, argv);
  return NULL;
}

tpos FakeFindBorderWindow(Twindow w, dat u, dat v, byte Border, tcell *PtrAttr) {
  byte Horiz, Vert;

  Horiz = u ? u + 1 == w->XWidth ? (byte)2 : (byte)1 : (byte)0;
  Vert = v ? v + 1 == w->YWidth ? (byte)2 : (byte)1 : (byte)0;

  if (*PtrAttr)
    *PtrAttr = TCELL(w->ColBorder, StdBorder[Border][Vert * 3 + Horiz]);

  return v ? POS_ROOT : POS_TITLE;
}

static Trow FindRow(Twindow window, ldat Row) {
  Trow CurrRow, ElPossib[4];
  byte Index;
  ldat k, ElNumRows[4], ElDist[4];

  ElPossib[0] = window->USE.R.RowOne;
  ElPossib[1] = window->USE.R.RowSplit;
  ElPossib[2] = window->USE.R.FirstRow;
  ElPossib[3] = window->USE.R.LastRow;
  ElNumRows[0] = window->USE.R.NumRowOne;
  ElNumRows[1] = window->USE.R.NumRowSplit;
  ElNumRows[2] = (ldat)0;
  ElNumRows[3] = window->HLogic - (ldat)1;
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

static Trow FindRowByCode(Twindow window, udat Code, ldat *NumRow) {
  Trow Row;
  ldat Num = (ldat)0;

  if ((Row = window->USE.R.FirstRow))
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
    (void (*)(Twindow, Tscreen))MapTopRealWidget,
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

/* Tscreen */

static Tscreen CreateSimpleScreen(dat NameLen, const char *Name, tcell Bg) {
  return New(screen)(NameLen, Name, 1, 1, &Bg);
}

static void BgImageScreen(Tscreen screen, dat BgWidth, dat BgHeight, const tcell *Bg) {
  size_t size;

  if (screen && S_USE(screen, USEBG) && Bg && (size = (size_t)BgWidth * BgHeight * sizeof(tcell)) &&
      (screen->USE.B.Bg = (tcell *)ReAllocMem(screen->USE.B.Bg, size))) {

    screen->USE.B.BgWidth = BgWidth;
    screen->USE.B.BgHeight = BgHeight;
    CopyMem(Bg, screen->USE.B.Bg, size);
    DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, 0, screen->Up + 1, TW_MAXDAT, TW_MAXDAT, tfalse);
  }
}

static void InsertScreen(Tscreen screen, Tall Parent, Tscreen Prev, Tscreen Next) {
  if (!screen->All && Parent) {
    InsertGeneric((TobjEntry)screen, (TobjList)&Parent->FirstScreen, (TobjEntry)Prev,
                  (TobjEntry)Next, NULL);
    screen->All = Parent;
  }
}

static void RemoveScreen(Tscreen screen) {
  if (screen->All) {
    RemoveGeneric((TobjEntry)screen, (TobjList)&screen->All->FirstScreen, NULL);
    screen->All = (Tall)0;
  }
}

static void DeleteScreen(Tscreen screen) {
  while (screen->FirstW)
    screen->FirstW->UnMap();

  screen->Remove();

  if (S_USE(screen, USEBG) && screen->USE.B.Bg) {
    FreeMem(screen->USE.B.Bg);
    screen->USE.B.Bg = NULL;
  }
  DeleteWidget((Twidget)screen);
}

static void SetXYScreen(Tscreen screen, dat x, dat y) {
  if (screen == All->FirstScreen) {
    y = Max2(y, -1);
    y = Min2(y, All->DisplayHeight - 1);
    ResizeFirstScreen(y - screen->Up);
  }
}

static Tmenu FindMenuScreen(Tscreen screen) {
  if (screen) {
    if (screen->MenuWindow && IS_WINDOW(screen->MenuWindow))
      /* menu activated from screen->MenuWindow, return its Tmenu */
      return screen->MenuWindow->Menu;

    /* no Twindow activated the Tmenu... either the Tmenu is inactive
     * or it is activated from the builtin Tmenu */

    if (screen->FocusW() && IS_WINDOW(screen->FocusW()) &&
        ((Twindow)screen->FocusW())->Menu != All->CommonMenu)
      /* Tmenu inactive... return the focus Twindow's one */
      return ((Twindow)screen->FocusW())->Menu;

    /* last case: Tmenu activated from builtin Tmenu */
    return All->BuiltinMenu;
  }
  return (Tmenu)0;
}

static Tscreen FindScreen(dat j) {
  Tscreen screen;
  byte VirtScrFound = tfalse;

  screen = All->FirstScreen;
  while (screen && !(VirtScrFound = (j >= (dat)screen->Up)))
    screen = screen->Next();

  if (VirtScrFound)
    return screen;

  return (Tscreen)0;
}

static Twidget FocusScreen(Tscreen screen_tofocus) {
  Tscreen screen = All->FirstScreen;
  if (screen_tofocus && screen != screen_tofocus) {
    MoveFirst(Screen, All, screen_tofocus);
    DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, 0, Min2(screen->Up, screen_tofocus->Up),
              TW_MAXDAT, TW_MAXDAT, tfalse);
    UpdateCursor();
  }
  return (Twidget)screen;
}

static void ActivateMenuScreen(Tscreen screen, Tmenuitem item, bool by_mouse) {

  if ((All->State & state_any) != state_default)
    return;

  if (screen && screen != All->FirstScreen)
    screen->Focus();

  SetMenuState(item, by_mouse);
}

static void DeActivateMenuScreen(Tscreen screen) {
  if (screen == All->FirstScreen && (All->State & state_any) == state_menu)
    CloseMenu();
}

static struct SscreenFn _FnScreen = {
    screen_magic,
    InsertScreen,
    RemoveScreen,
    DeleteScreen,
    /* Twidget */
    &_FnObj,
    DrawSelfScreen,
    (Twidget(*)(Tscreen, dat, dat))FindWidgetAt,
    (Tgadget(*)(Tscreen, udat))FindGadgetByCode,
    SetXYScreen,
    (void (*)(Tscreen, tcell))SetFillWidget,
    FocusScreen,
    (Twidget(*)(Tscreen))NoOp,        /* KbdFocusWidget */
    (void (*)(Tscreen, Twidget))NoOp, /* MapWidget */
    (void (*)(Tscreen))NoOp,          /* UnMapWidget */
    (void (*)(Tscreen, Tscreen))NoOp, /* MapTopRealWidget */
    (void (*)(Tscreen))RaiseW,
    (void (*)(Tscreen))LowerW,
    (void (*)(Tscreen, Tmsgport))OwnWidget,
    (void (*)(Tscreen))DisOwnWidget,
    (void (*)(Tscreen, Tmsgport))RecursiveDeleteWidget,
    (void (*)(Tscreen, dat, dat, dat, dat, const char *, const trune *,
              const tcell *))ExposeWidget2, /* exported by resize.c */
    (byte (*)(Tscreen, HookFn, void (**)(Twidget)))InstallHookWidget,
    (void (*)(Tscreen, HookFn, void (**)(Twidget)))RemoveHookWidget,
    /* Tscreen */
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

static void InsertGadgetGroup(Tgroup group, Tgadget g) {
  if (g && !g->Group && !g->G_Prev && !g->G_Next) {
    if ((g->G_Next = group->FirstG))
      group->FirstG->G_Prev = g;
    else
      group->LastG = g;

    group->FirstG = g;
    g->Group = group;
  }
}

static void RemoveGadgetGroup(Tgroup group, Tgadget g) {
  if (g && g->Group == group) {
    if (g->G_Prev)
      g->G_Prev->G_Next = g->G_Next;
    else if (group->FirstG == g)
      group->FirstG = g->G_Next;

    if (g->G_Next)
      g->G_Next->G_Prev = g->G_Prev;
    else if (group->LastG == g)
      group->LastG = g->G_Prev;

    g->G_Prev = g->G_Next = (Tgadget)0;
    g->Group = (Tgroup)0;
  }
}

static Tgadget GetSelectedGadget(Tgroup group) {
  return group->SelectG;
}

static void SetSelectedGadget(Tgroup group, Tgadget g) {
  if (!g || (g && g->Group == group)) {
    if (group->SelectG)
      UnPressGadget(group->SelectG, ttrue);
    if (g)
      PressGadget(g);
  }
}

static struct SgroupFn _FnGroup = {
    ggroup_magic, InsertGroup,       RemoveGroup,       DeleteGroup, //
    &_FnObj,      InsertGadgetGroup, RemoveGadgetGroup, GetSelectedGadget, SetSelectedGadget,
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
    Twindow w = Row->Window;

    Row->Remove();
    if (Row->Text)
      FreeMem(Row->Text);
    if (Row->ColText)
      FreeMem(Row->ColText);

    DeleteObj((Tobj)Row);

    if (w && w->Parent && (w->Flags & WINDOWFL_MENU))
      ResizeRelWindow(w, 0, -1);
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

Tmenuitem Create4MenuMenuItem(Tobj Parent, Twindow window, udat Code, byte Flags, ldat Len,
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

  if (window)
    window->Left = Left;

  return New(menuitem)(Parent, window, Code, Flags, Left, Len, ShortCut, Name);
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
    menuitem_magic, InsertMenuItem, RemoveMenuItem,      DeleteMenuItem,
    &_FnObj,        SetTextRow,     SetTRuneRow,         RaiseMenuItem,
    LowerMenuItem,  &_FnRow,        Create4MenuMenuItem, Create4MenuCommonMenuItem,
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
    Twidget w, WNext;

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
      for (w = MsgPort->FirstW; w && count; w = WNext) {
        WNext = w->O_Next;
        if (IS_WINDOW(w) && ((Twindow)w)->Menu == Menu) {
          if (w->Parent && IS_SCREEN(w->Parent)) {
            count--;
          }
        }
      }
      if (!count)
        QueuedDrawArea2FullScreen = ttrue;
    }
    for (w = MsgPort->FirstW; w; w = WNext) {
      WNext = w->O_Next;
      if (IS_WINDOW(w) && ((Twindow)w)->Menu == Menu) {
        if (w->Parent && IS_SCREEN(w->Parent)) {
          w->UnMap();
          ((Twindow)w)->Menu = NULL;
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
  Tmenuitem item = (Tmenuitem)0;

  if (Menu) {
    for (item = Menu->FirstI; item; item = item->Next) {
      if (i >= item->Left && (uldat)(i - item->Left) < item->Len)
        break;
    }

    if (!item && Menu->CommonItems && All->CommonMenu) {

      item = Menu->LastI;

      if (!item || (i >= item->Left && (uldat)(i - item->Left) >= item->Len)) {
        /* search in All->CommonMenu */
        if (item)
          i -= item->Left + (dat)item->Len;
        for (item = All->CommonMenu->FirstI; item; item = item->Next) {
          if (i >= item->Left && (uldat)(i - item->Left) < item->Len)
            break;
        }
      } else
        item = (Tmenuitem)0;
    }
  }
  return item;
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
  Twindow w = (Twindow)0, FW = (Twindow)All->FirstScreen->FocusW();
  dat d = -1;

  while (_I && IS_MENUITEM(_I)) {
    I = _I;
    d++;
    if (w == FW)
      break;
    if ((w = I->Window) && w->Parent)
      _I = (Tmenuitem)w->FindRow(w->CurY);
    else
      break;
  }
  if (depth)
    *depth = d > 0 ? d : 0;
  return I;
}

static void SetSelectedItem(Tmenu Menu, Tmenuitem item) {
  if (Menu) {
    if (item) {
      if (item->Parent == (Tobj)Menu) {
        Menu->SelectI = item;
        if (Menu->CommonItems && All->CommonMenu)
          All->CommonMenu->SelectI = (Tmenuitem)0;
      } else if (Menu->CommonItems && item->Parent == (Tobj)All->CommonMenu) {
        Menu->SelectI = (Tmenuitem)0;
        All->CommonMenu->SelectI = item;
      }
      /* else item is not a meaningful one! */
    } else {
      Menu->SelectI = item;
      if (Menu->CommonItems && All->CommonMenu)
        All->CommonMenu->SelectI = item;
    }

    All->FirstScreen->DrawMenu(0, TW_MAXDAT);
  }
}

static struct SmenuFn _FnMenu = {
    menu_magic,
    InsertMenu,
    RemoveMenu,
    DeleteMenu,
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
    /* Tmsg */
    &_FnObj,
};

/* Tmsgport */

static void InsertMsgPort(Tmsgport MsgPort, Tall Parent, Tmsgport Prev, Tmsgport Next) {
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
    MsgPort->All = (Tall)0;
  }
}

static void DeleteMsgPort(Tmsgport MsgPort) {
  uldat count = 20;
  Twidget w;

  if (MsgPort) {
    /*
     * optimization: if we are going to UnMap() a lot of windows,
     * we set QueuedDrawArea2FullScreen = ttrue, so that the UnMap()
     * calls do not have to redraw every time.
     */
    for (w = MsgPort->FirstW; w && count; w = w->O_Next) {
      if (IS_WINDOW(w) && w->Parent && IS_SCREEN(w->Parent)) {
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
    /* Tmsgport */
    &_FnObj,
};

/* Tmutex */

static void InsertMutex(Tmutex Mutex, Tall Parent, Tmutex Prev, Tmutex Next) {
  if (!Mutex->All && Parent) {
    InsertGeneric((TobjEntry)Mutex, (TobjList)&Mutex->All->FirstMutex, (TobjEntry)Prev,
                  (TobjEntry)Next, NULL);
    Mutex->All = Parent;
  }
}

static void RemoveMutex(Tmutex Mutex) {
  if (Mutex->All) {
    RemoveGeneric((TobjEntry)Mutex, (TobjList)&Mutex->All->FirstMutex, NULL);
    Mutex->All = (Tall)0;
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
    /* Tmutex */
    &_FnObj,
    OwnMutex,
    DisOwnMutex,
};

/* Tmodule */

static void InsertModule(Tmodule Module, Tall Parent, Tmodule Prev, Tmodule Next) {
  if (!Module->All && Parent) {
    InsertGeneric((TobjEntry)Module, (TobjList)&Parent->FirstModule, (TobjEntry)Prev,
                  (TobjEntry)Next, NULL);
    Module->All = Parent;
  }
}

static void RemoveModule(Tmodule Module) {
  if (Module->All) {
    RemoveGeneric((TobjEntry)Module, (TobjList)&Module->All->FirstModule, NULL);
    Module->All = (Tall)0;
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
    /* Tmodule */
    &_FnObj,
    DlOpen,
    DlClose,
};

/* Tdisplay */

static void InsertDisplayHW(Tdisplay DisplayHW, Tall Parent, Tdisplay Prev, Tdisplay Next) {
  if (!DisplayHW->All && Parent) {
    InsertGeneric((TobjEntry)DisplayHW, (TobjList)&Parent->FirstDisplay, (TobjEntry)Prev,
                  (TobjEntry)Next, NULL);
    DisplayHW->All = Parent;
#if 0
        /*
         * here we would call uninitialized DisplayHW routines like MoveToXY,
         * put this after DisplayHW->InitHW()
         */
        if (All->HookDisplayFn)
            All->HookDisplayFn(All->HookDisplay);
#endif
  }
}

static void RemoveDisplayHW(Tdisplay DisplayHW) {
  if (DisplayHW->All) {
    RemoveGeneric((TobjEntry)DisplayHW, (TobjList)&DisplayHW->All->FirstDisplay, NULL);
    DisplayHW->All = (Tall)0;

    if (All->HookDisplayFn)
      All->HookDisplayFn(All->HookDisplay);
  }
}

static void DeleteDisplayHW(Tdisplay DisplayHW) {
  byte isCTTY = DisplayHW->DisplayIsCTTY && DisplayHW == DisplayHWCTTY;
  byte Quitted = DisplayHW->Quitted;

  if (!Quitted)
    DisplayHW->DoQuit();

  /* avoid getting stale pointers */
  if (All->MouseDisplay == DisplayHW)
    All->MouseDisplay = NULL;
  if (All->ExclusiveDisplay == DisplayHW)
    All->ExclusiveDisplay = NULL;

  DisplayHW->Remove();
  String().swap(DisplayHW->Name); // destroy DisplayHW->Name

  DeleteObj((Tobj)DisplayHW);

  if (!Quitted) {
    if (!All->FirstDisplay || isCTTY)
      RunNoHW(tfalse);
    else if (All->FirstDisplay && ResizeDisplay()) {
      QueuedDrawArea2FullScreen = ttrue;
    }
  }
}

static struct SdisplayFn _FnDisplay = {
    display_hw_magic,
    InsertDisplayHW,
    RemoveDisplayHW,
    DeleteDisplayHW,
    /* Tdisplay */
    &_FnObj,
    InitDisplayHW,
    QuitDisplayHW,
};

TstructFn FnStruct = {
    &_FnObj,      &_FnWidget, &_FnGadget,  &_FnWindow, &_FnScreen, &_FnGroup,  &_FnRow,
    &_FnMenuItem, &_FnMenu,   &_FnMsgPort, &_FnMutex,  &_FnMsg,    &_FnModule, &_FnDisplay,
};
