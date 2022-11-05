/*
 *  group.cpp  --  define methods of server class Sgroup
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
#include "fn.h"      // Fn_Tgroup
#include "methods.h" // InsertLast()
#include "obj/group.h"
#include "obj/msgport.h"

#include <new>

Tgroup Sgroup::Create(Tmsgport owner) {
  Tgroup g = NULL;
  if (owner) {
    void *addr = AllocMem0(sizeof(Sgroup));
    if (addr) {
      g = new (addr) Sgroup();
      g->Fn = Fn_Tgroup;
      if (!g->Init(owner)) {
        g->Delete();
        g = NULL;
      }
    }
  }
  return g;
}

Tgroup Sgroup::Init(Tmsgport owner) {
  if (!owner || !((Tobj)this)->Init()) {
    return NULL;
  }
  // this->FirstG = this->LastG = this->SelectG = NULL;
  // this->MsgPort = NULL;

  InsertLast(Group, this, owner);
  return this;
}

void Sgroup::Remove() {
  if (MsgPort) {
    RemoveGeneric((TobjEntry)this, (TobjList)&MsgPort->FirstGroup, NULL);
    MsgPort = NULL;
  }
}

void Sgroup::Delete() {
  Remove();
  while (FirstG) {
    RemoveGadget(FirstG);
  }
  Sobj::Delete();
}
