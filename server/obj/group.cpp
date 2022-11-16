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
#include "fn.h"      // Fn_Tobj
#include "methods.h" // InsertLast()
#include "obj/group.h"
#include "obj/msgport.h"
#include "twin.h" // IS_MSGPORT(), IS_GROUP()

#include <new>

Tgroup Sgroup::Create(Tmsgport owner) {
  Tgroup g = NULL;
  if (owner) {
    void *addr = AllocMem0(sizeof(Sgroup));
    if (addr) {
      g = new (addr) Sgroup();
      g->Fn = Fn_Tobj;
      if (!g->Init(owner)) {
        g->Delete();
        g = NULL;
      }
    }
  }
  return g;
}

Tgroup Sgroup::Init(Tmsgport owner) {
  if (!owner || !Sobj::Init(Tgroup_class_id)) {
    return NULL;
  }
  InsertLast(Group, this, owner);
  return this;
}

void Sgroup::Insert(Tmsgport parent, Tgroup prev, Tgroup next) {
  if (parent && !MsgPort) {
    InsertGeneric((TobjEntry)this, (TobjList)&parent->FirstGroup, //
                  (TobjEntry)prev, (TobjEntry)next, NULL);
    MsgPort = parent;
  }
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

void Sgroup::InsertGadget(Tgadget g) {
  Tgroup group = this;
  if (g && !g->Group && !g->G_Prev && !g->G_Next) {
    if ((g->G_Next = group->FirstG))
      group->FirstG->G_Prev = g;
    else
      group->LastG = g;

    group->FirstG = g;
    g->Group = group;
  }
}

void Sgroup::RemoveGadget(Tgadget g) {
  Tgroup group = this;
  if (g && g->Group == group) {
    if (g->G_Prev)
      g->G_Prev->G_Next = g->G_Next;
    else if (group->FirstG == g)
      group->FirstG = g->G_Next;

    if (g->G_Next)
      g->G_Next->G_Prev = g->G_Prev;
    else if (group->LastG == g)
      group->LastG = g->G_Prev;

    g->G_Prev = g->G_Next = (Tgadget)0;
    g->Group = (Tgroup)0;
  }
}

Tgadget Sgroup::GetSelectedGadget() const {
  return SelectG;
}

void Sgroup::SetSelectedGadget(Tgadget g) {
  Tgroup group = this;
  if (!g || (g && g->Group == group)) {
    if (group->SelectG)
      UnPressGadget(group->SelectG, ttrue);
    if (g)
      PressGadget(g);
  }
}
