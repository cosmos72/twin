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

typedef struct Sobj *Tobj;
typedef struct SobjFn *TobjFn;
typedef struct SobjEntry *TobjEntry;
typedef struct SobjList *TobjList;

struct SobjFn {
  uldat Magic;
  void (*Insert)(Tobj self, Tobj parent, Tobj prev, Tobj next);
  void (*Remove)(Tobj self);
  void (*Delete)(Tobj self);
};

struct Sobj {
protected:
  Sobj() {
  }
  virtual ~Sobj() {
  }

public:
  uldat Id;
  //  TobjFn Fn;

  static Tobj Create();
  Tobj Init();

#if 0
  virtual e_id Magic() const = 0;

  void Insert(Tobj parent, Tobj prev, Tobj next) {
    ((TobjEntry)this)->Fn->Insert(this, parent, prev, next);
  }
  void Remove() {
    ((TobjEntry)this)->Fn->Remove(this);
  }
#endif // 0
  void Delete();

  // default implementation does nothing
  virtual void ChangeField(udat /*field*/, uldat /*clear_mask*/, uldat /*xor_mask*/) {
  }
};

inline uldat Obj2Id(Tobj o) {
  return o ? o->Id : NOID;
}

struct SobjEntry : public Sobj {
  TobjFn Fn;
  TobjEntry Prev, Next, Parent;
};

struct SobjList {
  TobjEntry First, Last;
};

#endif /* TWIN_OBJ_H */
