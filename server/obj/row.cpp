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
#include "fn.h"           // Fn_row
#include "obj/menuitem.h" // COD_RESERVED
#include "obj/row.h"

#include <new>

row Srow::Create(udat code, byte flags) {
  row r = NULL;
  if (code < COD_RESERVED) {
    void *addr = AllocMem0(sizeof(Srow));
    if (addr) {
      r = new (addr) Srow();
      r->Fn = Fn_row;
      if (!r->Init(code, flags)) {
        r->Delete();
        r = NULL;
      }
    }
  }
  return r;
}

row Srow::Init(udat code, byte flags) {
  if (code < COD_RESERVED && ((obj)this)->Init()) {
    this->Code = code;
    this->Flags = flags;
    // this->Gap = this->LenGap = this->Len = this->MaxLen = 0;
    // this->Text = NULL;
    // this->ColText = NULL;
    return this;
  }
  return NULL;
}
