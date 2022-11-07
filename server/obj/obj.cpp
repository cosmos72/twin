/*
 *  obj.cpp  --  define methods of server class Sobj
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "alloc.h" // AllocMem0()
#include "fn.h"    // Fn_Tobj
#include "id.h"    // AssignId()
#include "obj/obj.h"

#include <new>

Tobj Sobj::Create() {
  TobjEntry o = NULL;
  void *addr = AllocMem0(sizeof(SobjEntry));
  if (addr) {
    o = new (addr) SobjEntry();
    o->Fn = Fn_Tobj;
    if (!o->Init(Tobj_class_id)) {
      o->Delete();
      o = NULL;
    }
  }
  return o;
}

Tobj Sobj::Init(e_id class_id) {
  if (AssignId(class_id, this)) {
    return this;
  }
  return NULL;
}

void Sobj::Delete() {
  /* not a good idea to Remove() here: too late */
  DropId(this);
  FreeMem(this);
}
