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
#include "fn.h"           // Fn_Trow
#include "methods.h"      // RemoveGeneric()
#include "obj/menuitem.h" // COD_RESERVED
#include "obj/row.h"
#include "obj/window.h"
#include "resize.h" // ResizeRelWindow()
#include "twin.h"   // IS_WINDOW

#include <new>

Trow Srow::Create(udat code, byte flags) {
  Trow r = NULL;
  if (code < COD_RESERVED) {
    void *addr = AllocMem0(sizeof(Srow));
    if (addr) {
      r = new (addr) Srow();
      r->Fn = Fn_Trow;
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
    InsertGeneric((TobjEntry)this, (TobjList)&parent->USE.R.FirstRow, (TobjEntry)prev,
                  (TobjEntry)next, &parent->HLogic);
    Parent = parent;
    parent->USE.R.NumRowOne = parent->USE.R.NumRowSplit = (ldat)0;
  }
}

void Srow::Remove() {
  Twindow w = Window();
  if (w && W_USE(w, USEROWS)) {
    w->USE.R.NumRowOne = w->USE.R.NumRowSplit = (ldat)0;
    RemoveGeneric((TobjEntry)this, (TobjList)&w->USE.R.FirstRow, &w->HLogic);
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
