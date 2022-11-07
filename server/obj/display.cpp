/*
 *  Tdisplay.cpp  --  define methods of server class Sdisplay
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
#include "obj/display.h"
#include "alloc.h"    // AllocMem0(), CloneStrL()
#include "methods.h"  // InsertGeneric(), InsertLast(), RemoveGeneric()
#include "twin.h"     // NOSLOT
#include "hw.h"       // DisplayHWCTTY
#include "hw_multi.h" // ResizeDisplay(), RunNoHW()

#include <new>

Tdisplay Sdisplay::Create(uldat namelen, const char *name) {
  Tdisplay d = NULL;
  void *addr = AllocMem0(sizeof(Sdisplay));
  if (addr) {
    d = new (addr) Sdisplay();
    d->Fn = Fn_Tdisplay;
    if (!d->Init(namelen, name)) {
      d->Delete();
      d = NULL;
    }
  }
  return d;
}

Tdisplay Sdisplay::Init(uldat namelen, const char *name) {
  if (!Sobj::Init(Tdisplay_class_id) || !Name.format(Chars(name, namelen))) {
    return NULL;
  }
  Module = NULL;
  Quitted = ttrue;
  AttachSlot = NOSLOT;
  /*
   * ->Quitted will be set to tfalse only
   * after InitHW() has succeeded
   */
  InsertLast(Display, this, ::All);
  return this;
}

void Sdisplay::Delete() {
  byte isCTTY = DisplayIsCTTY && this == DisplayHWCTTY;
  byte quitted = Quitted;

  if (!quitted) {
    DoQuit();
  }
  /* avoid getting stale pointers */
  if (::All->MouseDisplay == this)
    ::All->MouseDisplay = NULL;
  if (::All->ExclusiveDisplay == this)
    ::All->ExclusiveDisplay = NULL;

  Remove();
  String().swap(Name); // destroy Name

  Sobj::Delete();

  if (!quitted) {
    if (!::All->FirstDisplay || isCTTY) {
      RunNoHW(tfalse);
    } else if (::All->FirstDisplay && ResizeDisplay()) {
      QueuedDrawArea2FullScreen = true;
    }
  }
}

void Sdisplay::Insert(Tall parent, Tdisplay prev, Tdisplay next) {
  if (parent && !All) {
    InsertGeneric((TobjEntry)this, (TobjList)&parent->FirstDisplay, //
                  (TobjEntry)prev, (TobjEntry)next, NULL);
    All = parent;
#if 0
    /*
     * here we would call uninitialized display routines like MoveToXY,
     * put this after InitHW()
     */
    if (All->HookDisplayFn) {
      All->HookDisplayFn(All->HookDisplay);
    }
#endif
  }
}

void Sdisplay::Remove() {
  if (All) {
    RemoveGeneric((TobjEntry)this, (TobjList)&All->FirstDisplay, NULL);
    All = (Tall)0;

    if (::All->HookDisplayFn)
      ::All->HookDisplayFn(::All->HookDisplay);
  }
}
