/*
 *  row.cpp  --  define methods of server class Srow
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "alloc.h"        // AllocMem0()
#include "builtin.h"      // ColorFill()
#include "draw.h"         // DrawAreaWidget()
#include "fn.h"           // Fn_Tobj
#include "methods.h"      // RemoveT()
#include "obj/menuitem.h" // COD_RESERVED
#include "obj/row.h"
#include "obj/window.h"
#include "resize.h" // EnsureLenRow(), ResizeRelWindow()
#include "twin.h"   // IS_WINDOW()

#include <new>
#include <Tutf/Tutf.h> // Tutf_CP437_to_UTF_32[]

Trow Srow::Create(udat code, byte flags) {
  Trow r = NULL;
  if (code < COD_RESERVED) {
    void *addr = AllocMem0(sizeof(Srow));
    if (addr) {
      r = new (addr) Srow();
      r->Fn = Fn_Tobj;
      if (!r->Init(code, flags, Trow_class_id)) {
        r->Delete();
        r = NULL;
      }
    }
  }
  return r;
}

Trow Srow::Init(udat code, byte flags, e_id class_id) {
  if (code >= COD_RESERVED || !Sobj::Init(class_id)) {
    return NULL;
  }
  Code = code;
  Flags = flags;
  // Gap = LenGap = Len = MaxLen = 0;
  // Text = NULL;
  // ColText = NULL;
  return this;
}

Twindow Srow::Window() const {
  Tobj parent = Parent;
  return parent && IS_WINDOW(parent) ? (Twindow)parent : (Twindow)0;
}

void Srow::Insert(Twindow parent, Trow prev, Trow next) {
  if (parent && W_USE(parent, USEROWS) && !Parent) {
    InsertT(this, &parent->USE.R.FirstRow, prev, next, &parent->HLogic);
    Parent = parent;
    parent->USE.R.NumRowOne = parent->USE.R.NumRowSplit = (ldat)0;
  }
}

void Srow::Remove() {
  Twindow w = Window();
  if (w && W_USE(w, USEROWS)) {
    w->USE.R.NumRowOne = w->USE.R.NumRowSplit = (ldat)0;
    RemoveT(this, &w->USE.R.FirstRow, &w->HLogic);
  }
  Parent = NULL;
}

void Srow::Delete() {
  Twindow w = Window();

  Remove();
  if (Text)
    FreeMem(Text);
  if (ColText)
    FreeMem(ColText);

  Sobj::Delete();

  if (w && w->Parent && (w->Flags & WINDOWFL_MENU))
    ResizeRelWindow(w, 0, -1);
}

bool Srow::SetText(uldat len, const char *text, bool default_color) {
  Trow row = this;
  if (not EnsureLenRow(row, len, default_color)) {
    return false;
  } else if (len) {
    trune *row_text = row->Text;
    ldat i = len;
    while (i-- > 0) {
      *row_text++ = Tutf_CP437_to_UTF_32[(byte)*text++];
    }
    if (!(row->Flags & ROW_DEFCOL) && !default_color) {
      ColorFill(row->ColText, len, TCOL(twhite, tblack));
    }
  }
  row->Len = len;
  row->Gap = row->LenGap = 0;
  return true;
}

bool Srow::SetTRune(uldat len, const trune *runes, bool default_color) {
  Trow row = this;
  if (not EnsureLenRow(row, len, default_color)) {
    return false;
  } else if (len) {
    CopyMem(runes, row->Text, len * sizeof(trune));
    if (!(row->Flags & ROW_DEFCOL) && !default_color) {
      tcolor col = TCOL(twhite, tblack);
      uldat i;
      for (i = 0; i < len; i++) {
        row->ColText[i] = col;
      }
    }
  }
  row->Len = len;
  row->Gap = row->LenGap = 0;
  return true;
}

void Srow::Raise() {
  Trow row = this;
  Tobj parent;
  if (!row || !(parent = row->Parent)) {
    return;

  } else if (IS_MENU(parent) && IS_MENUITEM(row)) {
    Tmenu menu = (Tmenu)parent;
    if (row != menu->FirstI) {
      row->Remove();
      ((Tmenuitem)row)->Insert(menu, (Tmenuitem)0, menu->FirstI);
      SyncMenu(menu);
    }
  } else if (IS_WINDOW(parent) && W_USE((Twindow)parent, USEROWS)) {
    Twindow window = (Twindow)parent;
    if (row != window->USE.R.FirstRow) {
      row->Remove();
      row->Insert(window, (Trow)0, window->USE.R.FirstRow);
      DrawAreaWidget(window);
    }
  }
}

void Srow::Lower() {
  Trow row = this;
  Tobj parent;
  if (!row || !(parent = row->Parent)) {
    return;
  } else if (IS_MENU(parent) && IS_MENUITEM(row)) {
    Tmenu menu = (Tmenu)parent;
    if (row != menu->LastI) {
      row->Remove();
      ((Tmenuitem)row)->Insert(menu, menu->LastI, (Tmenuitem)0);
      SyncMenu(menu);
    }
  } else if (IS_WINDOW(parent) && W_USE((Twindow)parent, USEROWS)) {
    Twindow window = (Twindow)parent;
    if (row != window->USE.R.LastRow) {
      row->Remove();
      row->Insert(window, window->USE.R.LastRow, (Trow)0);
      DrawAreaWidget(window);
    }
  }
}
