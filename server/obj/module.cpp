/*
 *  module.cpp  --  define methods of server class Smodule
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/all.h" // extern All
#include "obj/module.h"
#include "alloc.h"   // CloneStrL()
#include "twin.h"    // IS_ALL(), IS_MODULE()
#include "methods.h" // InsertLast()

#include <new>

Tmodule Smodule::Create(Chars name) {
  Tmodule m = NULL;
  if (name) {
    void *addr = AllocMem0(sizeof(Smodule));
    if (addr) {
      m = new (addr) Smodule();
      if (!m->Init(name)) {
        m->Delete();
        m = NULL;
      }
    }
  }
  return m;
}

Tmodule Smodule::Init(Chars name) {
  if (!name || !Sobj::Init(Tmodule_class_id) || !Name.format(name)) {
    return NULL;
  }
  this->Used = 0;
  // this->Handle = NULL;
  // this->DoInit = NULL;
  InsertLast(Modules, this, ::All);
  return this;
}

void Smodule::Delete() {
  if (!Used) {
    DlClose();
    Remove();
    String().swap(Name); // destroy Name
    Sobj::Delete();
  }
}

void Smodule::Insert(Tall parent, Tmodule prev, Tmodule next) {
  if (parent && !All) {
    parent->Modules.Insert(this, prev, next);
    All = parent;
  }
}

void Smodule::Remove() {
  if (All) {
    All->Modules.Remove(this);
    All = (Tall)0;
  }
}
