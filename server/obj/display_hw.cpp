/*
 *  display_hw.cpp  --  define methods of server class s_display_hw
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
#include "obj/display_hw.h"
#include "alloc.h"   // AllocMem0(), CloneStrL()
#include "methods.h" // InsertLast()
#include "twin.h"    // NOSLOT

#include <new>

display_hw s_display_hw::Create(uldat namelen, const char *name) {
  display_hw d = NULL;
  void *addr = AllocMem0(sizeof(s_display_hw));
  if (addr) {
    d = new (addr) s_display_hw();
    d->Fn = Fn_display_hw;
    if (!d->Init(namelen, name)) {
      d->Delete();
      d = NULL;
    }
  }
  return d;
}

display_hw s_display_hw::Init(uldat namelen, const char *name) {
  if (!((obj)this)->Init()) {
    return NULL;
  }
  if (!this->Name.format(Chars(name, namelen))) {
    return NULL;
  }
  this->Module = NULL;
  this->Quitted = ttrue;
  this->AttachSlot = NOSLOT;
  /*
   * ->Quitted will be set to tfalse only
   * after this->InitHW() has succeeded
   */
  InsertLast(DisplayHW, this, ::All);
  return this;
}
