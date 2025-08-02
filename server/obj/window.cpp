/*
 *  Twidget.cpp  --  define methods of server class Swindow
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/window.h"

#include "algo.h"    // Max2()
#include "alloc.h"   // AllocMem0(), CloneStrL()
#include "data.h"    // DEFAULT_Col*
#include "draw.h"    // ContainsCursor(), DrawAreaWidget(), DrawAreaWindow()
#include "methods.h" // IncMouseMotionN(), DecMouseMotionN()
#include "resize.h"  // UpdateCursor(), RollUpWindow()
#include "twin.h"    // NOFD, NOPID, NOSLOT
#include "util.h"    // Minimum()

#include <Tutf/Tutf.h>      // Tutf_CP437_to_UTF_32[]
#include <Tw/Twstat_defs.h> // TWS_window_*

#include <new>
#include <cstring> // memset()

static bool InitTtyDataWindow(Twindow window, dat scrollbacklines);

Twindow Swindow::Create(Tmsgport owner, dat titlelen, const char *title, const tcolor *coltitle,
                        Tmenu m, tcolor coltext, uldat cursortype, uldat attr, uldat flags,
                        dat xwidth, dat ywidth, dat scrollbacklines) {

  Twindow window = (Twindow)0;
  if (owner) {
    void *addr = AllocMem0(sizeof(Swindow));
    if (addr) {
      window = new (addr) Swindow();
      if (!window->Init(owner, titlelen, title, coltitle, m, coltext, cursortype, attr, flags,
                        xwidth, ywidth, scrollbacklines)) {
        window->Delete();
        window = NULL;
      }
    }
  }
  return window;
}

Twindow Swindow::Create4Menu(Tmenu menu) {
  Twindow window = (Twindow)0;
  if (menu && (window = Swindow::Create(menu->MsgPort, 0, NULL, (tcolor *)0, menu,
                                        TCOL(tblack, twhite), NOCURSOR, WINDOW_AUTO_KEYS,
                                        WINDOWFL_MENU | WINDOWFL_USEROWS | WINDOWFL_ROWS_DEFCOL |
                                            WINDOWFL_ROWS_SELCURRENT,
                                        MIN_XWIN, MIN_YWIN, 0))) {

    window->SetColors(0x1FF, TCOL(0, 0), TCOL(0, 0), TCOL(0, 0), TCOL(0, 0), TCOL(tWHITE, twhite),
                      TCOL(tblack, twhite), TCOL(tblack, tgreen), TCOL(tBLACK, twhite),
                      TCOL(tBLACK, tblack));
    window->Configure(0x3F, 0, 1, MIN_XWIN, MIN_YWIN, TW_MAXDAT, TW_MAXDAT);
  }
  return window;
}

Twindow Swindow::Init(Tmsgport owner, dat titlelen, const char *title, const tcolor *coltitle,
                      Tmenu m, tcolor coltext, uldat cursortype, uldat attr, uldat flags,
                      dat xwidth, dat ywidth, dat scrollbacklines) {

  byte hasborder = 2 * !(flags & WINDOWFL_BORDERLESS);
  /* overflow safety */
  if (hasborder) {
    if ((dat)(xwidth + hasborder) > 0)
      xwidth += hasborder;
    if ((dat)(ywidth + hasborder) > 0)
      ywidth += hasborder;
  }
  if (!Swidget::Init(owner, xwidth, ywidth, attr, flags, 0, TW_MAXDAT, TCELL(coltext, ' '),
                     Twindow_class_id)) {
    return NULL;
  }
  if (title && !(Name = CloneStrL(title, titlelen))) {
    return NULL;
  }
  if (coltitle && !(ColName = (tcolor *)CloneMem(coltitle, titlelen * sizeof(tcolor)))) {
    return NULL;
  }

  Menu = m;
  // MenuItem = NULL;
  NameLen = titlelen;
  //  BorderPattern[0] = BorderPattern[1] = NULL;
  RemoteData.Fd = NOFD;
  RemoteData.ChildPid = NOPID;
  RemoteData.FdSlot = NOSLOT;
  // CurX = CurY = 0;
  // XstSel = YstSel = XendSel = YendSel = 0;
  ColGadgets = DEFAULT_ColGadgets;
  ColArrows = DEFAULT_ColArrows;
  ColBars = DEFAULT_ColBars;
  ColTabs = DEFAULT_ColTabs;
  ColBorder = DEFAULT_ColBorder;
  ColText = coltext;
  ColSelect = TCOL(TCOLBG(coltext), TCOLFG(coltext));
  ColDisabled = DEFAULT_ColDisabled;
  ColSelectDisabled = DEFAULT_ColSelectDisabled;
  /* sanity: */
  if (Flags & WINDOWFL_BORDERLESS)
    Attr &= ~WINDOW_ROLLED_UP;
  if (Attr & WINDOW_WANT_KEYS)
    Attr &= ~WINDOW_AUTO_KEYS;

  // State = (uldat)0;
  CursorType = cursortype;

  MinXWidth = MIN_XWIN;
  MinYWidth = MIN_YWIN;
  XWidth = xwidth = Max2(MIN_XWIN, xwidth);
  YWidth = ywidth = Max2(MIN_YWIN, ywidth);
  MaxXWidth = TW_MAXDAT;
  MaxYWidth = TW_MAXDAT;

  Charset = Tutf_CP437_to_UTF_32;
  // memset(&USE, '\0', sizeof(USE));

  if (W_USE(this, USECONTENTS)) {
    if (TW_MAXDAT - scrollbacklines < ywidth - hasborder)
      scrollbacklines = TW_MAXDAT - YWidth + hasborder;
    CurY = YLogic = scrollbacklines;
    WLogic = xwidth - hasborder;
    HLogic = scrollbacklines + ywidth - hasborder;
    if (!InitTtyDataWindow(this, scrollbacklines)) {
      return NULL;
    }
  } else if (W_USE(this, USEROWS)) {
    WLogic = 1024; /* just an arbitrary value... */
    HLogic = 0;    /* no rows */
  } else {
    WLogic = xwidth - hasborder;
    HLogic = scrollbacklines + ywidth - hasborder;
  }
  return this;
}

/* tty_data */

static bool InitTtyDataWindow(Twindow window, dat scrollbacklines) {
  tty_data *Data = window->USE.C.TtyData;
  ldat count = window->WLogic * window->HLogic;
  tcell *p = window->USE.C.Contents, h;

  if (!Data && !(window->USE.C.TtyData = Data = (tty_data *)AllocMem(sizeof(tty_data))))
    return false;

  if (!p && !(window->USE.C.Contents = p = (tcell *)AllocMem(count * sizeof(tcell)))) {
    FreeMem(Data);
    return false;
  }

  new (&Data->newName) String();

  h = TCELL(TCOL(twhite, tblack), ' ');
  while (count--) {
    *p++ = h;
  }
  /*
   * this is a superset of reset_tty(),
   * but we don't want to call it from here
   */
  Data->State = ESnormal;
  Data->Flags = TTY_AUTOWRAP;
  Data->Effects = 0;
  window->YLogic = window->CurY = Data->ScrollBack = scrollbacklines;
  window->USE.C.HSplit = 0;
  Data->SizeX = window->WLogic;
  Data->SizeY = window->HLogic - scrollbacklines;
  Data->Top = 0;
  Data->Bottom = Data->SizeY;
  Data->saveX = Data->X = window->CurX = 0;
  Data->saveY = Data->Y = 0;
  Data->Pos = Data->Start = window->USE.C.Contents + Data->ScrollBack * window->WLogic;
  Data->Split = window->USE.C.Contents + window->WLogic * window->HLogic;

  window->CursorType = LINECURSOR;
  /* respect the WINDOWFL_CURSOR_ON set by the client and don't force it on */
#if 0
  window->Flags |= WINDOWFL_CURSOR_ON;
#endif
  window->ColText = Data->Color = Data->DefColor = Data->saveColor = TCOL(twhite, tblack);
  Data->Underline = TCOL(tWHITE, tblack);
  Data->HalfInten = TCOL(tBLACK, tblack);
  Data->TabStop[0] = 0x01010100;
  Data->TabStop[1] = Data->TabStop[2] = Data->TabStop[3] = Data->TabStop[4] = 0x01010101;
  Data->nPar = 0;

  /* default to latin1 charset */
  std::memset(Data->Gv, LATIN1_MAP, sizeof(Data->Gv));
  std::memset(Data->saveGv, LATIN1_MAP, sizeof(Data->saveGv));
  Data->Gv[1] = Data->saveGv[1] = VT100GR_MAP;
  Data->Gi = Data->saveGi = 0;

  /* default to UTF-8 mode */
  Data->utf8 = 1;
  Data->utf8_count = Data->utf8_char = 0;
  window->Charset = Tutf_ISO_8859_1_to_UTF_32;
  Data->InvCharset = Tutf_UTF_32_to_ISO_8859_1;

  return true;
}

void Swindow::Delete() {
  UnMap();
  if (Name) {
    FreeMem(Name);
  }
  if (ColName) {
    FreeMem(ColName);
  }
  if (W_USE(this, USECONTENTS)) {
    tty_data *tty = USE.C.TtyData;
    if (tty) {
      tty->newName.~String();
      FreeMem(tty);
    }
    if (USE.C.Contents) {
      FreeMem(USE.C.Contents);
    }
  } else if (W_USE(this, USEROWS)) {
    DeleteList(USE.R.Rows.First);
  }
  Swidget::Delete();
}

void Swindow::ChangeField(udat field, uldat clear_mask, uldat xor_mask) {
  uldat old, curr, mask;

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
      C = &ColGadgets;
      break;
    case TWS_window_ColArrows:
      C = &ColArrows;
      break;
    case TWS_window_ColBars:
      C = &ColBars;
      break;
    case TWS_window_ColTabs:
      C = &ColTabs;
      break;
    case TWS_window_ColBorder:
      C = &ColBorder;
      break;
    case TWS_window_ColText:
      C = &ColText;
      break;
    case TWS_window_ColSelect:
      C = &ColSelect;
      break;
    case TWS_window_ColDisabled:
      C = &ColDisabled;
      break;
    case TWS_window_ColSelectDisabled:
      C = &ColSelectDisabled;
      break;
    default:
      break;
    }
    curr = (*C & ~clear_mask) ^ xor_mask;
    if (*C != curr) {
      *C = curr;
      /* FIXME: this is an overkill */
      DrawAreaWidget(this);
    }
    break;
  }
  case TWS_window_Flags:
    mask = WINDOWFL_CURSOR_ON;
    old = Flags & mask;
    curr = ((old & ~clear_mask) ^ xor_mask) & mask;
    if (curr != old) {
      Flags = curr | (Flags & ~mask);
      if (ContainsCursor(this)) {
        UpdateCursor();
      }
    }
    break;
  case TWS_window_Attr:
    mask = WINDOW_WANT_KEYS | WINDOW_WANT_MOUSE | WINDOW_WANT_CHANGES | WINDOW_AUTO_FOCUS |
           WINDOW_DRAG | WINDOW_RESIZE | WINDOW_CLOSE | WINDOW_ROLLED_UP | WINDOW_X_BAR |
           WINDOW_Y_BAR | WINDOW_AUTO_KEYS | WINDOW_WANT_MOUSE_MOTION;
    old = Attr & mask;
    curr = ((old & ~clear_mask) ^ xor_mask) & mask;
    if (curr != old) {
      if ((curr & WINDOW_ROLLED_UP) != (old & WINDOW_ROLLED_UP)) {
        RollUpWindow(this, !!(curr & WINDOW_ROLLED_UP));
      }
      uldat lmask = WINDOW_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS;
      if ((curr & lmask) != (old & lmask) && Parent) {
        if (curr & mask)
          IncMouseMotionN();
        else
          DecMouseMotionN();
      }
      lmask = WINDOW_RESIZE | WINDOW_CLOSE | WINDOW_X_BAR | WINDOW_Y_BAR;
      const bool drawborder = (curr & lmask) != (old & lmask) && Parent;
      Attr = curr | (Attr & ~mask);
      if (drawborder) {
        DrawBorderWindow(this, BORDER_ANY);
      }
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
    Swidget::ChangeField(field, clear_mask, xor_mask);
    break;
  }
}

void Swindow::SetXY(dat x, dat y) {
  Twidget parent = Parent, prev = NULL, next = NULL;
  if (parent) {
    prev = Prev;
    next = Next;
    Remove();
    DrawAreaWindow(this);
  }
  Left = x;
  Up = y;
  if (parent) {
    if (IS_SCREEN(parent)) {
      Left += parent->XLogic;
      Up += parent->YLogic;
    }
    Insert(parent, prev, next);
    DrawAreaWindow(this);
  }
}

void Swindow::Expose(dat xwidth, dat ywidth, dat left, dat up, dat pitch, const char *ascii,
                     const trune *runes, const tcell *cells) {
  ExposeWindow2(this, xwidth, ywidth, left, up, pitch, ascii, runes, cells);
}

void Swindow::GotoXY(ldat x, ldat y) {
  if (W_USE(this, USECONTENTS)) {
    tty_data *tt = USE.C.TtyData;

    x = Max2(x, 0);
    y = Max2(y, 0);
    if (x >= tt->SizeX)
      x = tt->SizeX - 1;
    if (y >= tt->SizeY)
      y = tt->SizeY - 1;
    tt->X = x;
    tt->Y = y;
    tt->Pos = tt->Start + x + (ldat)y * tt->SizeX;
    if (tt->Pos >= tt->Split)
      tt->Pos -= tt->Split - USE.C.Contents;
    y += HLogic - tt->SizeY;
  }
  CurX = x;
  CurY = y;
  if (ContainsCursor(this)) {
    UpdateCursor();
  }
}

void Swindow::SetTitle(dat titlelen, char *title) {
  Twidget parent;

  if (Name)
    FreeMem(Name);

  NameLen = titlelen;
  Name = title;

#if 1
  /*
   * do not allow changing Twindow borders just because
   * some untrusted application set a new title
   */
  DrawBorderWindow(this, BORDER_UP);
#else
  /* user may have title-dependent borders in ~/.config/twin/twinrc, honour them: */
  Win->BorderPattern[0] = Win->BorderPattern[1] = NULL;
  DrawBorderWindow(this, BORDER_ANY);
#endif

  if ((parent = Parent) && IS_SCREEN(parent)) {
    /* need to update Twindow list with new name ? */
    ((Tscreen)parent)->HookMap();
  }
}

void Swindow::SetColText(tcolor color) {
  ColText = color;
}

void Swindow::SetColors(udat bitmap, tcolor colgadgets, tcolor colarrows, tcolor colbars,
                        tcolor coltabs, tcolor colborder, tcolor coltext, tcolor colselect,
                        tcolor coldisabled, tcolor colselectdisabled) {
  if (bitmap & 1)
    ColGadgets = colgadgets;
  if (bitmap & 2)
    ColArrows = colarrows;
  if (bitmap & 4)
    ColBars = colbars;
  if (bitmap & 8)
    ColTabs = coltabs;
  if (bitmap & 0x10)
    ColBorder = colborder;
  if (bitmap & 0x20) {
    ColText = coltext;
    if (W_USE(this, USECONTENTS))
      USE.C.TtyData->Color = coltext;
  }
  if (bitmap & 0x40)
    ColSelect = colselect;
  if (bitmap & 0x80)
    ColDisabled = coldisabled;
  if (bitmap & 0x100)
    ColSelectDisabled = colselectdisabled;
  if (Parent)
    DrawBorderWindow(this, BORDER_ANY);
}

void Swindow::Configure(byte bitmap, dat left, dat up, //
                        dat minxwidth, dat minywidth,  //
                        dat maxxwidth, dat maxywidth) {
  Twidget prev = NULL, next = NULL;
  dat hasBorder = 2 * !(Flags & WINDOWFL_BORDERLESS);

  if (Parent) {
    prev = Prev;
    next = Next;
    Remove();
    DrawAreaWindow(this);
  }

  if (bitmap & 1) {
    Left = left;
    if (Parent && IS_SCREEN(Parent))
      Left += Parent->XLogic;
  }
  if (bitmap & 2) {
    Up = up;
    if (Parent && IS_SCREEN(Parent))
      Up += Parent->YLogic;
  }

  if (bitmap & 4) {
    if (minxwidth <= TW_MAXDAT - hasBorder)
      minxwidth = Max2(minxwidth, minxwidth + hasBorder);
    MinXWidth = minxwidth;
    XWidth = Max2(minxwidth, XWidth);
  }
  if (bitmap & 8) {
    if (minywidth <= TW_MAXDAT - hasBorder)
      minywidth = Max2(minywidth, minywidth + hasBorder);
    MinYWidth = minywidth;
    YWidth = Max2(minywidth, YWidth);
  }
  if (bitmap & 0x10) {
    if (maxxwidth <= TW_MAXDAT - hasBorder)
      maxxwidth = Max2(maxxwidth, maxxwidth + hasBorder);
    MaxXWidth = Max2(MinXWidth, maxxwidth);
    XWidth = Min2(maxxwidth, XWidth);
  }
  if (bitmap & 0x20) {
    if (maxywidth <= TW_MAXDAT - hasBorder)
      maxywidth = Max2(maxywidth, maxywidth + hasBorder);
    MaxYWidth = Max2(MinYWidth, maxywidth);
    YWidth = Min2(maxywidth, YWidth);
  }
  if (Parent) {
    InsertMiddle(Widgets, this, Parent, prev, next);
    DrawAreaWindow(this);
  }
}

Trow Swindow::FindRow(ldat row_i) const {
  Trow row, el_possible[4];
  byte i;
  ldat k, el_row_n[4], el_distance[4];

  el_possible[0] = USE.R.RowOne;
  el_possible[1] = USE.R.RowSplit;
  el_possible[2] = USE.R.Rows.First;
  el_possible[3] = USE.R.Rows.Last;
  el_row_n[0] = USE.R.NumRowOne;
  el_row_n[1] = USE.R.NumRowSplit;
  el_row_n[2] = (ldat)0;
  el_row_n[3] = HLogic - (ldat)1;
  el_distance[0] = (el_possible[0] && el_row_n[0] ? Abs(el_row_n[0] - row_i) : TW_MAXLDAT);
  el_distance[1] = (el_possible[1] && el_row_n[1] ? Abs(el_row_n[1] - row_i) : TW_MAXLDAT);
  el_distance[2] = row_i;
  el_distance[3] = Abs(el_row_n[3] - row_i);

  i = Minimum((byte)4, el_distance);
  row = el_possible[i];
  k = el_row_n[i];

  if (row) {
    if (k < row_i)
      while (k < row_i && (row = row->Next))
        k++;
    else if (k > row_i)
      while (k > row_i && (row = row->Prev))
        k--;
  }
  if (row && IS_MENUITEM(row))
    ((Tmenuitem)row)->WCurY = row_i;
  return row;
}

Trow Swindow::FindRowByCode(udat Code, ldat *row_i) const {
  Trow row;
  ldat i = 0;

  if ((row = USE.R.Rows.First))
    while (row && row->Code != Code) {
      row = row->Next;
      i++;
    }
  if (row && row_i) {
    *row_i = i;
  }
  return row;
}
