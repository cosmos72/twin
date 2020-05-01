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

#include "obj/display_hw.h"
#include "obj/all.h" // extern All
#include "alloc.h"   // CloneStrL()
#include "methods.h" // InsertLast()
#include "twin.h"    // NOSLOT

display_hw s_display_hw::Create(fn_display_hw Fn, uldat namelen, const char *name) {
  display_hw d = NULL;
  char *newName = NULL;

  if (name && (newName = CloneStrL(name, namelen))) {
    if ((d = (display_hw)s_obj::Create((fn_obj)Fn))) {

      d->NameLen = namelen;
      d->Name = newName;
      d->Module = NULL;
      d->Quitted = ttrue;
      d->AttachSlot = NOSLOT;
      /*
       * ->Quitted will be set to tfalse only
       * after d->InitHW() has succeeded
       */
      InsertLast(DisplayHW, d, ::All);
      return d;
    }
    FreeMem(newName);
  }
  return d;
}
