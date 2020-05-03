/*
 *  extension.cpp  --  define methods of server class s_extension
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "alloc.h"
#include "fn.h"
#include "obj/extension.h"

#include <new>

extension s_extension::Create(uldat namelen, const char *name) {
  extension e = NULL;
  void *addr = AllocMem0(sizeof(s_extension), 1);
  if (addr) {
    e = new (addr) s_extension();
    e->Fn = Fn_extension;
    if (!e->Init(namelen, name)) {
      e->Delete();
      e = NULL;
    }
  }
  return e;
}

extension s_extension::Init(uldat namelen, const char *name) {
  return (extension)((module)this)->Init(namelen, name);
}
