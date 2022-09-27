/*
 *  obj.cpp  --  define methods of server class s_obj
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
#include "fn.h"    // Fn_obj
#include "id.h"    // AssignId()
#include "obj/obj.h"

#include <new>

obj s_obj::Create() {
  obj_entry o = NULL;
  void *addr = AllocMem0(sizeof(s_obj_entry));
  if (addr) {
    o = new (addr) s_obj_entry();
    o->Fn = Fn_obj;
    if (!o->Init()) {
      o->Delete();
      o = NULL;
    }
  }
  return o;
}

obj s_obj::Init() {
  if (AssignId(e_id(((obj_entry)this)->Fn->Magic), this)) {
    return this;
  }
  return NULL;
}

void s_obj::Delete() {
  ((obj_entry)this)->Fn->Delete(this);
}
void s_obj::ChangeField(udat field, uldat clear_mask, uldat xor_mask) {
  ((obj_entry)this)->Fn->ChangeField(this, field, clear_mask, xor_mask);
}
