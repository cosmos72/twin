/*
 *  group.h  --  declare server class Sgroup
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_GROUP_H
#define TWIN_GROUP_H

#include "obj/fwd.h"
#include <Tw/datatypes.h>

/* ggroup -- gadget group */

struct SgroupFn {
  uldat Magic;
  void (*Insert)(ggroup, Tmsgport MsgPort, ggroup Prev, ggroup Next);
  void (*Remove)(ggroup);
  void (*Delete)(ggroup);
  void (*ChangeField)(ggroup, udat field, uldat CLEARMask, uldat XORMask);
  /* ggroup */
  TobjFn Fn_Obj; /* backup of overloaded functions */
  void (*InsertGadget)(ggroup, gadget);
  void (*RemoveGadget)(ggroup, gadget);
  gadget (*GetSelectedGadget)(ggroup);
  void (*SetSelectedGadget)(ggroup, gadget);
};

struct Sgroup : public Sobj {
  TgroupFn Fn;
  ggroup Prev, Next; /* list in the same Tmsgport */
  Tmsgport MsgPort;
  /* ggroup */
  gadget FirstG, LastG; /* list in this ggroup */
  gadget SelectG;

  static ggroup Create(Tmsgport Parent);
  ggroup Init(Tmsgport Parent);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(Tmsgport owner, ggroup prev, ggroup next) {
    Fn->Insert(this, owner, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  /* group */
  void InsertGadget(gadget g) {
    Fn->InsertGadget(this, g);
  }
  void RemoveGadget(gadget g) {
    Fn->RemoveGadget(this, g);
  }
  gadget GetSelectedGadget() {
    return Fn->GetSelectedGadget(this);
  }
  void SetSelectedGadget(gadget g) {
    Fn->SetSelectedGadget(this, g);
  }
};

#endif /* TWIN_GROUP_H */
