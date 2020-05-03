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

#include "obj/obj.h"

#include "alloc.h" // AllocMem0()
#include "fn.h"    // Fn_obj
#include "id.h"    // AssignId()

obj s_obj::Create() {
  obj o = (obj)AllocMem0(sizeof(s_obj), 1);
  obj_entry e = (obj_entry)o;
  if (e) {
    e->Fn = Fn_obj;
    if (!o->Init()) {
      Fn_obj->Delete(o);
      o = NULL;
    }
  }
  return o;
}

obj s_obj::Init() {
  if (AssignId(((obj_entry)this)->Fn, this)) {
    return this;
  }
  return NULL;
}

s_obj::s_obj() {
}
