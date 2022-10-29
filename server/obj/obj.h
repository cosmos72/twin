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

typedef struct Sobj *obj;
typedef struct SobjFn *TobjFn;
typedef struct s_obj_entry *obj_entry;
typedef struct s_obj_list *obj_list;

struct SobjFn {
  uldat Magic;
  void (*Insert)(obj self, obj parent, obj prev, obj next);
  void (*Remove)(obj self);
  void (*Delete)(obj self);
  void (*ChangeField)(obj self, udat field, uldat clear_mask, uldat xor_mask);
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

  static obj Create();
  obj Init();

#if 0
  virtual e_id Magic() const = 0;

  void Insert(obj parent, obj prev, obj next) {
    ((obj_entry)this)->Fn->Insert(this, parent, prev, next);
  }
  void Remove() {
    ((obj_entry)this)->Fn->Remove(this);
  }
#endif // 0
  void Delete();
  void ChangeField(udat field, uldat clear_mask, uldat xor_mask);
};

inline uldat Obj2Id(obj o) {
  return o ? o->Id : NOID;
}

struct s_obj_entry : public Sobj {
  TobjFn Fn;
  obj_entry Prev, Next, Parent;
};

struct s_obj_list {
  obj_entry First, Last;
};

#endif /* TWIN_OBJ_H */
