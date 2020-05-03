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
typedef struct s_obj_entry *obj_entry;
typedef struct s_obj_list *obj_list;

struct s_fn_obj {
  uldat Magic;
  void (*Insert)(obj self, obj parent, obj prev, obj next);
  void (*Remove)(obj self);
  void (*Delete)(obj self);
  void (*ChangeField)(obj self, udat field, uldat clear_mask, uldat xor_mask);
};

struct s_obj {
  uldat Id;
  //  fn_obj Fn;

  static obj Create();
  obj Init();
  s_obj();
  virtual ~s_obj();

#if 0
  uldat Magic() const {
    return ((const s_obj_entry *)this)->Fn->Magic;
  }
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

struct s_obj_entry : public s_obj {
  fn_obj Fn;
  obj_entry Prev, Next, Parent;

  virtual ~s_obj_entry();
};

struct s_obj_list {
  obj_entry First, Last;
};

#endif /* _TWIN_OBJ_H */
