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
#include "methods.h" // InsertLast(), RemoveGeneric()
#include "twin.h"    // IS_ALL(), IS_MODULE()

#include <new>

Tmodule Smodule::Create(Chars name) {
  Tmodule m = NULL;
  if (name) {
    void *addr = AllocMem0(sizeof(Smodule));
    if (addr) {
      m = new (addr) Smodule();
      m->Fn = Fn_Tmodule;
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
  InsertLast(Module, this, ::All);
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
    InsertGeneric((TobjEntry)this, (TobjList)&parent->FirstModule, //
                  (TobjEntry)prev, (TobjEntry)next, NULL);
    All = parent;
  }
}

void Smodule::Remove() {
  if (All) {
    RemoveGeneric((TobjEntry)this, (TobjList)&All->FirstModule, NULL);
    All = (Tall)0;
  }
}
