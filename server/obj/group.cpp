/*
 *  group.cpp  --  define methods of server class s_group
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "alloc.h"   // AllocMem0()
#include "fn.h"      // Fn_group
#include "id.h"      // AssignId()
#include "methods.h" // InsertLast()
#include "obj/group.h"
#include "obj/msgport.h"

#include <new>

ggroup s_group::Create(msgport owner) {
  ggroup g = NULL;
  if (owner) {
    void *addr = AllocMem0(sizeof(s_group));
    if (addr) {
      g = new (addr) s_group();
      g->Fn = Fn_group;
      if (!g->Init(owner)) {
        g->Delete();
        g = NULL;
      }
    }
  }
  return g;
}

ggroup s_group::Init(msgport owner) {
  if (!owner || !((obj)this)->Init()) {
    return NULL;
  }
  // this->FirstG = this->LastG = this->SelectG = NULL;
  // this->MsgPort = NULL;

  InsertLast(Group, this, owner);
  return this;
}
