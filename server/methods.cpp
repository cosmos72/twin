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

static SobjFn _FnObj = {};

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

static SwidgetFn _FnWidget = {
    TtyKbdFocus,
};

/* Tgadget */

static SwidgetFn _FnGadget = {
    TtyKbdFocus,
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

static SwindowFn _FnWindow = {
    /* Twidget */
    TtyKbdFocus,
    /* Twindow */
    FakeWriteCharset,
    FakeWriteUtf8,
    FakeWriteTRune,
    FakeWriteTCell,
    FakeFindBorderWindow,
};

/* Tscreen */

static SwidgetFn _FnScreen = {
    (Twidget(*)(Twidget))NoOp, /* KbdFocus */
};

/* Tgroup */

/* Trow */

byte FindInfo(Tmenu Menu, dat i) {
  Trow Info;

  if (Menu && (Info = Menu->Info) && Info->Len > (udat)i)
    return ttrue;
  return tfalse;
}

/* Tmenu */
/* Tmsg */
/* Tmsgport */
/* Tmutex */

/* Tmodule */

static SmoduleFn _FnModule = {
    /* Tmodule */
    DlOpen,
    DlClose,
};

/* Tdisplay */

SstructFn FnStruct = {
    &_FnObj, &_FnWidget, &_FnGadget, &_FnWindow, &_FnScreen, &_FnModule,
};
