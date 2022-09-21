/*
 *  module.h  --  declare server class s_module
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_MODULE_H
#define TWIN_MODULE_H

#include "obj/fwd.h"
#include "obj/obj.h"
#include <Tw/datatypes.h>

/* module */

struct s_fn_module {
  e_id Magic;
  void (*Insert)(module, all, module Prev, module Next);
  void (*Remove)(module);
  void (*Delete)(module);
  void (*ChangeField)(module, udat field, uldat CLEARMask, uldat XORMask);
  /* module */
  fn_obj Fn_Obj;
  bool (*DlOpen)(module);
  void (*DlClose)(module);
};

struct s_module : public s_obj {
  fn_module Fn;
  module Prev, Next; /* in the same All */
  all All;
  /* module */
  uldat NameLen, Used;
  char *Name;
  void *Handle;
  bool (*DoInit)(void);

  static module Create(uldat namelen, const char *name);
  module Init(uldat namelen, const char *name);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(all a, module prev, module next) {
    Fn->Insert(this, a, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  /* module */
  bool DlOpen() {
    return Fn->DlOpen(this);
  }
  void DlClose() {
    Fn->DlClose(this);
  }
};

#endif /* TWIN_MODULE_H */
