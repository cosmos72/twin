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
                    (tcell)0, Tgadget_class_id)) {
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
    /* Twidget */
    &_FnObj,                           //
    (Twidget (*)(Tgadget))TtyKbdFocus, //
    /* Tgadget */
    &_FnWidget, CreateEmptyButton, FillButton, CreateButton,
    WriteTextsGadget,  /* exported by resize.c */
    WriteTRunesGadget, /* exported by resize.c */
};

/* Twindow */

static void SetTitleWindow(Twindow w, dat titlelen, char *title) {
  Twidget parent;

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

  if ((parent = w->Parent) && IS_SCREEN(parent)) {
    /* need to update Twindow list with new name ? */
    ((Tscreen)parent)->HookMap();
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

static void ConfigureWindow(Twindow w, byte Bitmap, dat Left, dat Up, dat MinXWidth, dat MinYWidth,
                            dat MaxXWidth, dat MaxYWidth) {
  Twidget prev, next;
  dat HasBorder = 2 * !(w->Flags & WINDOWFL_BORDERLESS);

  if (w->Parent) {
    prev = w->Prev;
    next = w->Next;
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
    InsertMiddle(w, w, w->Parent, prev, next);
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

static Trow FindRow(Twindow window, ldat row_i) {
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
  ElDist[0] = (ElPossib[0] && ElNumRows[0] ? Abs(ElNumRows[0] - row_i) : TW_MAXLDAT);
  ElDist[1] = (ElPossib[1] && ElNumRows[1] ? Abs(ElNumRows[1] - row_i) : TW_MAXLDAT);
  ElDist[2] = row_i;
  ElDist[3] = Abs(ElNumRows[3] - row_i);

  Index = Minimum((byte)4, ElDist);
  CurrRow = ElPossib[Index];
  k = ElNumRows[Index];

  if (CurrRow) {
    if (k < row_i)
      while (k < row_i && (CurrRow = CurrRow->Next))
        k++;
    else if (k > row_i)
      while (k > row_i && (CurrRow = CurrRow->Prev))
        k--;
  }
  if (CurrRow && IS_MENUITEM(CurrRow))
    ((Tmenuitem)CurrRow)->WCurY = row_i;
  return CurrRow;
}

static Trow FindRowByCode(Twindow window, udat Code, ldat *row_i) {
  Trow row;
  ldat i = 0;

  if ((row = window->USE.R.FirstRow))
    while (row && row->Code != Code) {
      row = row->Next;
      i++;
    }
  if (row && row_i)
    *row_i = i;

  return row;
}

static struct SwindowFn _FnWindow = {
    /* Twidget */
    &_FnObj,
    (Twidget(*)(Twindow))TtyKbdFocus,
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
    FindScreen,
    CreateSimpleScreen,
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

static byte SetTextRow(Trow row, uldat Len, const char *Text, byte DefaultCol) {
  if (EnsureLenRow(row, Len, DefaultCol)) {
    if (Len) {

      trune *RowText = row->Text;
      ldat i = Len;
      while (i-- > 0) {
        *RowText++ = Tutf_CP437_to_UTF_32[(byte)*Text++];
      }
      if (!(row->Flags & ROW_DEFCOL) && !DefaultCol)
        /* will not work correctly if sizeof(tcolor) != 1 */
        memset(row->ColText, TCOL(twhite, tblack), Len * sizeof(tcolor));
    }
    row->Len = Len;
    row->Gap = row->LenGap = 0;
    return ttrue;
  }
  return tfalse;
}

static byte SetTRuneRow(Trow row, uldat Len, const trune *TRune, byte DefaultCol) {
  if (EnsureLenRow(row, Len, DefaultCol)) {
    if (Len) {
      CopyMem(TRune, row->Text, Len * sizeof(trune));
      if (!(row->Flags & ROW_DEFCOL) && !DefaultCol)
        /* will not work correctly if sizeof(tcolor) != 1 */
        memset(row->ColText, TCOL(twhite, tblack), Len * sizeof(tcolor));
    }
    row->Len = Len;
    row->Gap = row->LenGap = 0;
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
  Tmenuitem next;

  if (M && (Parent = (Tobj)M->Parent)) {
    if (IS_MENU(Parent))
      next = ((Tmenu)Parent)->FirstI;
    else if (IS_WINDOW(Parent) && W_USE((Twindow)Parent, USEROWS))
      next = (Tmenuitem)((Twindow)Parent)->USE.R.FirstRow;
    else
      return;

    M->Remove();
    M->Insert(Parent, (Tmenuitem)0, next);

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
  Tmenuitem prev;

  if (M && (Parent = (Tobj)M->Parent)) {
    if (IS_MENU(Parent))
      prev = ((Tmenu)Parent)->LastI;
    else if (IS_WINDOW(Parent) && W_USE((Twindow)Parent, USEROWS))
      prev = (Tmenuitem)((Twindow)Parent)->USE.R.LastRow;
    else
      return;

    M->Remove();
    M->Insert(Parent, prev, NULL);

    if (IS_MENU(Parent))
      SyncMenu((Tmenu)Parent);
    else if (((TobjEntry)Parent)->Parent)
      DrawAreaWidget((Twidget)Parent);
  }
}

static struct SrowFn _FnRow = {
    /* Trow */
    &_FnObj, SetTextRow, SetTRuneRow, (void (*)(Trow))RaiseMenuItem, (void (*)(Trow))LowerMenuItem,
};

byte FindInfo(Tmenu Menu, dat i) {
  Trow Info;

  if (Menu && (Info = Menu->Info) && Info->Len > (udat)i)
    return ttrue;
  return tfalse;
}

/* Tmenuitem */

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
    &_FnObj,       SetTextRow, SetTRuneRow,         RaiseMenuItem,
    LowerMenuItem, &_FnRow,    Create4MenuMenuItem, Create4MenuCommonMenuItem,
};

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

TstructFn FnStruct = {
    &_FnObj,      &_FnWidget, &_FnGadget,  &_FnWindow, &_FnScreen, &_FnGroup,  &_FnRow,
    &_FnMenuItem, &_FnMenu,   &_FnMsgPort, &_FnMutex,  &_FnMsg,    &_FnModule, &_FnDisplay,
};
