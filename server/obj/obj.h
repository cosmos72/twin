/*
 *  obj.h  --  declare server class Sobj
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_OBJ_H
#define TWIN_OBJ_H

#include <Tw/autoconf.h> /* for TW_HAVE_* macros */
#include "compiler.h"
#include "obj/fwd.h"
#include "obj/id.h"

#include <Tw/datatypes.h>

typedef class Sobj *Tobj;

class Sobj {
protected:
  Sobj() {
  }
  virtual ~Sobj() {
  }

public:
  uldat Id;

  static Tobj Create();
  Tobj Init(e_id class_id);

  virtual void Delete();

  // default implementation does nothing
  virtual void Remove() {
  }

  // default implementation does nothing
  virtual void ChangeField(udat field, uldat clear_mask, uldat xor_mask) {
    (void)field;
    (void)clear_mask;
    (void)xor_mask;
  }
};

class SobjEntry : public Sobj {
public:
  TobjEntry Prev, Next, Parent;
};

inline uldat Obj2Id(Tobj o) {
  return o ? o->Id : NOID;
}

#endif /* TWIN_OBJ_H */
