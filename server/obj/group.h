/*
 *  group.h  --  declare server class s_group
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_GROUP_H
#define _TWIN_GROUP_H

#include "obj/fwd.h"
#include <Tw/datatypes.h>

/* ggroup -- gadget group */

struct s_fn_group {
  uldat Magic;
  void (*Insert)(ggroup, msgport MsgPort, ggroup Prev, ggroup Next);
  void (*Remove)(ggroup);
  void (*Delete)(ggroup);
  void (*ChangeField)(ggroup, udat field, uldat CLEARMask, uldat XORMask);
  /* ggroup */
  fn_obj Fn_Obj; /* backup of overloaded functions */
  void (*InsertGadget)(ggroup, gadget);
  void (*RemoveGadget)(ggroup, gadget);
  gadget (*GetSelectedGadget)(ggroup);
  void (*SetSelectedGadget)(ggroup, gadget);
};

struct s_group {
  uldat Id;
  fn_group Fn;
  ggroup Prev, Next; /* list in the same msgport */
  msgport MsgPort;
  /* ggroup */
  gadget FirstG, LastG; /* list in this ggroup */
  gadget SelectG;

  static ggroup Create(msgport Parent);
  ggroup Init(msgport Parent);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
};

#endif /* _TWIN_GROUP_H */
