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

#ifndef _TWIN_EXTENSION_H
#define _TWIN_EXTENSION_H

#include "obj/module.h"

struct s_extension {
  uldat Id;
  fn_extension Fn;
  extension Prev, Next; /* in the same All */
  all All;
  /* module */
  uldat NameLen, Used;
  char *Name;
  void *Handle, *Init;
  /* extension */
  tany (*CallB)(extension, topaque len, const byte *data,
                void *return_type); /* call extension-specific functions */
  void (*Quit)(extension);          /* how to quit this extension if it is not dlopen()ed */
};

struct s_fn_extension {
  uldat Magic, Size;
  extension (*Create)(fn_extension, uldat NameLen, const char *Name);
  void (*Insert)(extension, all, extension Prev, extension Next);
  void (*Remove)(extension);
  void (*Delete)(extension);
  void (*ChangeField)(extension, udat field, uldat CLEARMask, uldat XORMask);
  /* module */
  fn_obj Fn_Obj;
  byte (*DlOpen)(extension);
  void (*DlClose)(extension);
  /* extension */
  fn_module Fn_Module;
  extension (*Query)(byte namelen, const char *name);
};

#endif /* _TWIN_EXTENSION_H */
