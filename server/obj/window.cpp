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
#include "draw.h"    // ContainsCursor(), DrawAreaWidget(), DrawAreaWindow2()
#include "fn.h"      // Fn_Twindow
#include "methods.h" // IncMouseMotionN(), DecMouseMotionN()
#include "resize.h"  // UpdateCursor(), RollUpWindow()
#include "twin.h"    // NOFD, NOPID, NOSLOT

#include <Tutf/Tutf.h>      // Tutf_CP437_to_UTF_32[]
#include <Tw/Twstat_defs.h> // TWS_window_*

#include <new>
#include <cstring> // memset()

static bool InitTtyDataWindow(Twindow w, dat scrollbacklines);

Twindow Swindow::Create(Tmsgport owner, dat titlelen, const char *title, const tcolor *coltitle,
                        Tmenu m, tcolor coltext, uldat cursortype, uldat attr, uldat flags,
                        dat xwidth, dat ywidth, dat scrollbacklines) {

  Twindow w = NULL;
  if (owner) {
    void *addr = AllocMem0(sizeof(Swindow));
    if (addr) {
      w = new (addr) Swindow();
      w->Fn = (TwidgetFn)Fn_Twindow;
      if (!w->Init(owner, titlelen, title, coltitle, m, coltext, cursortype, attr, flags, xwidth,
                   ywidth, scrollbacklines)) {
        w->Delete();
        w = NULL;
      }
    }
  }
  return w;
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

/* ttydata */

static bool InitTtyDataWindow(Twindow w, dat scrollbacklines) {
  ttydata *Data = w->USE.C.TtyData;
  ldat count = w->WLogic * w->HLogic;
  tcell *p = w->USE.C.Contents, h;

  if (!Data && !(w->USE.C.TtyData = Data = (ttydata *)AllocMem(sizeof(ttydata))))
    return false;

  if (!p && !(w->USE.C.Contents = p = (tcell *)AllocMem(count * sizeof(tcell))))
    return false;

  h = TCELL(TCOL(twhite, tblack), ' ');
  while (count--)
    *p++ = h;

  /*
   * this is a superset of reset_tty(),
   * but we don't want to call it from here
   */
  Data->State = ESnormal;
  Data->Flags = TTY_AUTOWRAP;
  Data->Effects = 0;
  w->YLogic = w->CurY = Data->ScrollBack = scrollbacklines;
  w->USE.C.HSplit = 0;
  Data->SizeX = w->WLogic;
  Data->SizeY = w->HLogic - scrollbacklines;
  Data->Top = 0;
  Data->Bottom = Data->SizeY;
  Data->saveX = Data->X = w->CurX = 0;
  Data->saveY = Data->Y = 0;
  Data->Pos = Data->Start = w->USE.C.Contents + Data->ScrollBack * w->WLogic;
  Data->Split = w->USE.C.Contents + w->WLogic * w->HLogic;

  w->CursorType = LINECURSOR;
  /* respect the WINDOWFL_CURSOR_ON set by the client and don't force it on */
#if 0
  w->Flags |= WINDOWFL_CURSOR_ON;
#endif
  w->ColText = Data->Color = Data->DefColor = Data->saveColor = TCOL(twhite, tblack);
  Data->Underline = TCOL(thigh | twhite, tblack);
  Data->HalfInten = TCOL(thigh | tblack, tblack);
  Data->TabStop[0] = 0x01010100;
  Data->TabStop[1] = Data->TabStop[2] = Data->TabStop[3] = Data->TabStop[4] = 0x01010101;
  Data->nPar = 0;

  Data->G = Data->saveG = 0;
  /* default to latin1 charset */
  Data->currG = Data->G0 = Data->saveG0 = LATIN1_MAP;
  Data->G1 = Data->saveG1 = VT100GR_MAP;

  Data->utf8 = Data->utf8_count = Data->utf8_char = 0;
  Data->InvCharset = Tutf_UTF_32_to_ISO_8859_1;
  Data->newLen = Data->newMax = 0;
  Data->newName = NULL;

  return true;
}

void Swindow::Delete() {
  UnMap();
  if (Name)
    FreeMem(Name);
  if (ColName)
    FreeMem(ColName);
  if (W_USE(this, USECONTENTS)) {
    if (USE.C.TtyData)
      FreeMem(USE.C.TtyData);
    if (USE.C.Contents)
      FreeMem(USE.C.Contents);
  } else if (W_USE(this, USEROWS)) {
    DeleteList(USE.R.FirstRow);
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
    DrawAreaWindow2(this);
  }
  Left = x;
  Up = y;
  if (parent) {
    if (IS_SCREEN(parent)) {
      Left += parent->XLogic;
      Up += parent->YLogic;
    }
    Insert(parent, prev, next);
    DrawAreaWindow2(this);
  }
}
