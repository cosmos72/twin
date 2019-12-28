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

struct s_obj {
  uldat Id;
  fn_obj Fn;
  obj Prev, Next, Parent;
};

struct s_obj_parent {
  obj First, Last;
};

struct s_fn_obj {
  uldat Magic, Size, Used;
  obj (*Create)(fn_obj);
  void (*Insert)(obj Obj, obj, obj Prev, obj Next);
  void (*Remove)(obj);
  void (*Delete)(obj);
  void (*ChangeField)(obj, udat field, uldat CLEARMask, uldat XORMask);
};

#endif /* _TWIN_OBJ_H */
