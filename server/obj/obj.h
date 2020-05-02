/*
 *  obj.h  --  declare server class s_obj
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_OBJ_H
#define _TWIN_OBJ_H

#include "twautoconf.h" /* for TW_HAVE_* macros */
#include "compiler.h"
#include "obj/fwd.h"

#include <Tw/datatypes.h>

typedef struct s_obj *obj;
typedef struct s_fn_obj *fn_obj;
typedef struct s_obj_parent *obj_parent;

struct s_fn_obj {
  uldat Magic;
  void (*Insert)(obj self, obj parent, obj prev, obj next);
  void (*Remove)(obj self);
  void (*Delete)(obj self);
  void (*ChangeField)(obj self, udat field, uldat clear_mask, uldat xor_mask);
};

struct s_obj_parent {
  obj First, Last;
};

struct s_obj {
  uldat Id;
  fn_obj Fn;
  obj Prev, Next, Parent;

  uldat Magic() const {
    return Fn->Magic;
  }
  static obj Create();
  obj Init();
  void Insert(obj parent, obj prev, obj next) {
    Fn->Insert(this, parent, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  void ChangeField(udat field, uldat clear_mask, uldat xor_mask) {
    Fn->ChangeField(this, field, clear_mask, xor_mask);
  }
};

#endif /* _TWIN_OBJ_H */
