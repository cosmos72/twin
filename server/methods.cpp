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
  Tobj next;
  while (obj) {
    next = obj->Next;
    obj->Delete();
    obj = next;
  }
}
#endif

void InsertGeneric(TobjEntry obj, TobjList parent, TobjEntry prev, TobjEntry next, ldat *objcount) {
  if (obj->Prev || obj->Next)
    return;

  if (prev)
    next = prev->Next;
  else if (next)
    prev = next->Prev;

  if ((obj->Prev = prev))
    prev->Next = obj;
  else
    parent->First = obj;

  if ((obj->Next = next))
    next->Prev = obj;
  else
    parent->Last = obj;

  if (objcount)
    ++(*objcount);
}

void RemoveGeneric(TobjEntry obj, TobjList parent, ldat *objcount) {
  if (obj->Prev)
    obj->Prev->Next = obj->Next;
  else if (parent->First == obj)
    parent->First = obj->Next;

  if (obj->Next)
    obj->Next->Prev = obj->Prev;
  else if (parent->Last == obj)
    parent->Last = obj->Prev;

  obj->Prev = obj->Next = NULL;
  if (objcount)
    --(*objcount);
}

/* TobjFn and others fn_XXX functions */

/* Tobj */

static struct SobjFn _FnObj = {};

/* Twidget */

void IncMouseMotionN(void) {
  if (!All->MouseMotionN++)
    EnableMouseMotionEvents(ttrue);
}

void DecMouseMotionN(void) {
  if (All->MouseMotionN && !--All->MouseMotionN)
    EnableMouseMotionEvents(tfalse);
}

#define TtyKbdFocus FakeKbdFocus
Twidget FakeKbdFocus(Twidget w) {
  Twidget oldW;
  Twidget parent;
  Tscreen screen =
      w && (parent = w->Parent) && IS_SCREEN(parent) ? (Tscreen)parent : All->FirstScreen;

  if (screen) {
    oldW = screen->FocusW();
    screen->FocusW(w);
  } else
    oldW = (Twidget)0;

  return oldW;
}

static struct SwidgetFn _FnWidget = {
    &_FnObj,
    TtyKbdFocus,
};

/* Tgadget */

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

static struct SgadgetFn _FnGadget = {
    /* Twidget */
    &_FnObj,                           //
    (Twidget (*)(Tgadget))TtyKbdFocus, //
    /* Tgadget */
    &_FnWidget, FillButton, //
    WriteTextsGadget,       /* exported by resize.c */
    WriteTRunesGadget,      /* exported by resize.c */
};

/* Twindow */

bool FakeWriteCharset(Twindow window, uldat Len, const char *charset_bytes) {
  if (DlLoad(TermSo) && window->fn()->TtyWriteCharset != FakeWriteCharset)
    return window->TtyWriteCharset(Len, charset_bytes);
  return false;
}

bool FakeWriteUtf8(Twindow window, uldat Len, const char *utf8_bytes) {
  if (DlLoad(TermSo) && window->fn()->TtyWriteUtf8 != FakeWriteUtf8)
    return window->TtyWriteUtf8(Len, utf8_bytes);
  return false;
}

bool FakeWriteTRune(Twindow window, uldat Len, const trune *runes) {
  if (DlLoad(TermSo) && window->fn()->TtyWriteTRune != FakeWriteTRune)
    return window->TtyWriteTRune(Len, runes);
  return false;
}

bool FakeWriteTCell(Twindow window, dat x, dat y, uldat Len, const tcell *cells) {
  if (DlLoad(TermSo) && window->fn()->TtyWriteTCell != FakeWriteTCell)
    return window->TtyWriteTCell(x, y, Len, cells);
  return false;
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

static struct SwindowFn _FnWindow = {
    /* Twidget */
    &_FnObj,
    (Twidget(*)(Twindow))TtyKbdFocus,
    /* Twindow */
    &_FnWidget,
    &Swindow::Create4Menu,
    FakeWriteCharset,
    FakeWriteUtf8,
    FakeWriteTRune,
    FakeWriteTCell,
    FakeFindBorderWindow,
};

/* Tscreen */

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
    /* Twidget */
    &_FnObj,
    (Twidget(*)(Tscreen))NoOp, /* KbdFocus */
    /* Tscreen */
    &_FnWidget,
    FindMenuScreen,
    BgImageScreen,
    DrawMenuScreen,
    ActivateMenuScreen,
    DeActivateMenuScreen,
};

/* Tgroup */

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
    &_FnObj, InsertGadgetGroup, RemoveGadgetGroup, GetSelectedGadget, SetSelectedGadget,
};

/* Trow */

byte FindInfo(Tmenu Menu, dat i) {
  Trow Info;

  if (Menu && (Info = Menu->Info) && Info->Len > (udat)i)
    return ttrue;
  return tfalse;
}

/* Tmenu */

static Trow SetInfoMenu(Tmenu Menu, byte Flags, ldat Len, const char *Text, const tcolor *ColText) {
  Trow row;
  if ((row = New(row)(0, Flags))) {
    if ((!Text || (row->Text = CloneStr2TRune(Text, Len))) &&
        (!ColText || (row->ColText = (tcolor *)CloneMem(ColText, Len * sizeof(tcolor))))) {
      row->Len = row->MaxLen = Len;
      if (Menu->Info)
        Menu->Info->Delete();
      return Menu->Info = row;
    }
    row->Delete();
    row = (Trow)0;
  }
  return row;
}

static Tmenuitem FindItem(Tmenu Menu, dat i) {
  Tmenuitem item = (Tmenuitem)0;

  if (Menu) {
    for (item = Menu->FirstI; item; item = item->Next()) {
      if (i >= item->Left && (uldat)(i - item->Left) < item->Len)
        break;
    }

    if (!item && Menu->CommonItems && All->CommonMenu) {

      item = Menu->LastI;

      if (!item || (i >= item->Left && (uldat)(i - item->Left) >= item->Len)) {
        /* search in All->CommonMenu */
        if (item)
          i -= item->Left + (dat)item->Len;
        for (item = All->CommonMenu->FirstI; item; item = item->Next()) {
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
    /* Tmenu */
    &_FnObj, SetInfoMenu, FindItem, GetSelectedItem, RecursiveGetSelectedItem, SetSelectedItem,
};

/* Tmsg */

static struct SmsgFn _FnMsg = {
    /* Tmsg */
    &_FnObj,
};

/* Tmsgport */

static struct SmsgportFn _FnMsgPort = {
    /* Tmsgport */
    &_FnObj,
};

/* Tmutex */

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
    /* Tmutex */
    &_FnObj,
    OwnMutex,
    DisOwnMutex,
};

/* Tmodule */

static struct SmoduleFn _FnModule = {
    /* Tmodule */
    &_FnObj,
    DlOpen,
    DlClose,
};

/* Tdisplay */

static struct SdisplayFn _FnDisplay = {
    /* Tdisplay */
    &_FnObj,
    InitDisplay,
    QuitDisplay,
};

SstructFn FnStruct = {
    &_FnObj,  &_FnWidget,  &_FnGadget, &_FnWindow, &_FnScreen, &_FnGroup,
    &_FnMenu, &_FnMsgPort, &_FnMutex,  &_FnMsg,    &_FnModule, &_FnDisplay,
};
