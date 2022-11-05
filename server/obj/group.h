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

/* Tgroup -- group of Tgadget:s */

struct SgroupFn {
  uldat Magic;
  void (*Insert)(Tgroup, Tmsgport MsgPort, Tgroup Prev, Tgroup Next);
  /* Tgroup */
  TobjFn Fn_Obj; /* backup of overloaded functions */
  void (*InsertGadget)(Tgroup, Tgadget);
  void (*RemoveGadget)(Tgroup, Tgadget);
  Tgadget (*GetSelectedGadget)(Tgroup);
  void (*SetSelectedGadget)(Tgroup, Tgadget);
};

struct Sgroup : public Sobj {
  TgroupFn Fn;
  Tgroup Prev, Next; /* list in the same Tmsgport */
  Tmsgport MsgPort;
  /* Tgroup */
  Tgadget FirstG, LastG; /* list in this Tgroup */
  Tgadget SelectG;

  static Tgroup Create(Tmsgport Parent);
  Tgroup Init(Tmsgport Parent);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }

  void Insert(Tmsgport owner, Tgroup prev, Tgroup next) {
    Fn->Insert(this, owner, prev, next);
  }
  virtual void Remove() OVERRIDE;
  virtual void Delete() OVERRIDE;

  /* group */
  void InsertGadget(Tgadget g) {
    Fn->InsertGadget(this, g);
  }
  void RemoveGadget(Tgadget g) {
    Fn->RemoveGadget(this, g);
  }
  Tgadget GetSelectedGadget() {
    return Fn->GetSelectedGadget(this);
  }
  void SetSelectedGadget(Tgadget g) {
    Fn->SetSelectedGadget(this, g);
  }
};

#endif /* TWIN_GROUP_H */
