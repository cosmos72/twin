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
      if (!r->Init(code, flags)) {
        r->Delete();
        r = NULL;
      }
    }
  }
  return r;
}

Trow Srow::Init(udat code, byte flags) {
  if (code < COD_RESERVED && ((Tobj)this)->Init()) {
    this->Code = code;
    this->Flags = flags;
    // this->Gap = this->LenGap = this->Len = this->MaxLen = 0;
    // this->Text = NULL;
    // this->ColText = NULL;
    return this;
  }
  return NULL;
}

Twindow Srow::Window() const {
  Tobj parent = Parent;
  return parent && IS_WINDOW(parent) ? (Twindow)parent : (Twindow)0;
}

void Srow::Insert(Twindow w, Trow prev, Trow next) {
  Fn->Insert(this, w, prev, next);
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
