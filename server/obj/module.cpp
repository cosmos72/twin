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

#include <new>

module s_module::Create(uldat namelen, const char *name) {
  module m = NULL;
  if (name) {
    void *addr = AllocMem0(sizeof(s_module));
    if (addr) {
      m = new (addr) s_module();
      m->Fn = Fn_module;
      if (!m->Init(namelen, name)) {
        m->Delete();
        m = NULL;
      }
    }
  }
  return m;
}

module s_module::Init(uldat namelen, const char *name) {
  if (!name || !((obj)this)->Init()) {
    return NULL;
  }
  if (!(this->Name = CloneStrL(name, namelen))) {
    return NULL;
  }
  this->NameLen = namelen;
  this->Used = 0;
  // this->Handle = NULL;
  // this->DoInit = NULL;
  InsertLast(Module, this, ::All);
  return this;
}
