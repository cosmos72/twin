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

#ifndef _TWIN_MODULE_H
#define _TWIN_MODULE_H

#include "obj/fwd.h"

/* module */

struct s_module {
  uldat Id;
  fn_module Fn;
  module Prev, Next; /* in the same All */
  all All;
  /* module */
  uldat NameLen, Used;
  char *Name;
  void *Handle;
  byte (*Init)(void);
};
struct s_fn_module {
  uldat Magic, Size, Used;
  module (*Create)(fn_module, uldat NameLen, CONST char *Name);
  void (*Insert)(module, all, module Prev, module Next);
  void (*Remove)(module);
  void (*Delete)(module);
  void (*ChangeField)(module, udat field, uldat CLEARMask, uldat XORMask);
  /* module */
  fn_obj Fn_Obj;
  byte (*DlOpen)(module);
  void (*DlClose)(module);
};

#endif /* _TWIN_MODULE_H */
