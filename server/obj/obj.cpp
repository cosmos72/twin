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

#include "alloc.h" // AllocMem0(), FreeMem()
#include "id.h"    // AssignId()

obj s_obj::Create(fn_obj Fn) {
  obj o;

  if ((o = (obj)AllocMem0(Fn->Size, 1))) {
    if (AssignId(Fn, o)) {
      o->Fn = Fn;
      o->Prev = o->Next = NULL;
      o->Parent = NULL;
    } else {
      FreeMem(o);
      o = NULL;
    }
  }
  return o;
}
