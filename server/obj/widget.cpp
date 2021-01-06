/*
 *  widget.cpp  --  define methods of server class s_widget
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/widget.h"
#include "alloc.h" // AllocMem0()
#include "fn.h"    // Fn_widget
#include "id.h"    // AssignId()

#include <new>
#include <string.h> // memset()

widget s_widget::Create(msgport owner, dat xwidth, dat ywidth, uldat attr, uldat flags, dat left,
                        dat up, tcell fill) {
  widget w = NULL;
  if (owner) {
    void *addr = AllocMem0(sizeof(s_widget));
    if (addr) {
      w = new (addr) s_widget();
      w->Fn = Fn_widget;
      if (!w->Init(owner, xwidth, ywidth, attr, flags, left, up, fill)) {
        w->Delete();
        w = NULL;
      }
    }
  }
  return w;
}

widget s_widget::Init(msgport owner, dat xwidth, dat ywidth, uldat attr, uldat flags, dat left,
                      dat up, tcell fill) {
  if (owner && ((obj)this)->Init()) {
    Left = left;
    Up = up;
    XWidth = xwidth;
    YWidth = ywidth;
    Attr = attr;
    Flags = flags;
    USE_Fill = fill;
    /*
    if (w_USE(this, USEEXPOSE))
      memset(&USE.E, '\0', sizeof(USE.E));
    */
    Own(owner);
    return this;
  }
  return NULL;
}
