/*
 *  module.cpp  --  define methods of server class s_module
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/all.h" // extern All
#include "obj/module.h"
#include "alloc.h"   // CloneStrL()
#include "methods.h" // InsertLast()

module s_module::Create(fn_module Fn, uldat namelen, const char *name) {
  module Module = NULL;
  char *newName = NULL;

  if (name && (newName = CloneStrL(name, namelen))) {
    if ((Module = (module)s_obj::Create((fn_obj)Fn))) {

      Module->NameLen = namelen;
      Module->Name = newName;
      Module->Used = 0;
      Module->Handle = NULL;
      Module->Init = NULL;

      InsertLast(Module, Module, ::All);
      return Module;
    }
    FreeMem(newName);
  }
  return Module;
}
