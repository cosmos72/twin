/*
 *  row.cpp  --  define methods of server class Srow
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "algo.h"  // Max2()
#include "alloc.h" // AllocMem0(), CloneStr2TRune()
#include "fn.h"    // Fn_Tmenuitem
#include "obj/menuitem.h"
#include "resize.h" // SyncMenu()
#include "twin.h"   // IS_WINDOW()

#include <new>
#include <Tw/datasizes.h> // TW_MAXLDAT

Tmenuitem Smenuitem::Create(obj parent, Twindow w, udat code, byte flags, dat left, ldat len,
                            dat shortcut, const char *name) {
  Tmenuitem item = NULL;
  if (parent && (IS_MENU(parent) || (IS_WINDOW(parent) && W_USE((Twindow)parent, USEROWS))) &&
      (!w || IS_WINDOW(w)) && name) {

    void *addr = AllocMem0(sizeof(Smenuitem));
    if (addr) {
      item = new (addr) Smenuitem();
      item->Fn = Fn_Tmenuitem;
      if (!item->Init(parent, w, code, flags, left, len, shortcut, name)) {
        item->Delete();
        item = NULL;
      }
    }
  }
  return item;
}

Tmenuitem Smenuitem::Init(obj parent, Twindow w, udat code, byte flags, dat left, ldat len,
                          dat shortcut, const char *name) {

  if (parent && (IS_MENU(parent) || (IS_WINDOW(parent) && W_USE((Twindow)parent, USEROWS))) &&
      (!w || IS_WINDOW(w)) && name && (this->Text = CloneStr2TRune(name, len)) &&
      ((Trow)this)->Init(code, flags)) {

    this->Len = len;
    this->Window = w;
    this->Left = left;
    this->ShortCut = shortcut;
    this->WCurY = TW_MAXLDAT;

    if (w)
      w->MenuItem = this;

    if (IS_WINDOW(parent)) {
      w = (Twindow)parent;

      if ((ldat)w->XWidth < (len = Max2((ldat)10, len + (ldat)2)))
        w->XWidth = len;

      if ((ldat)w->YWidth < (len = Min2(TW_MAXDAT, w->HLogic + (ldat)3)))
        w->YWidth = len;

      this->Insert((obj)w, (Tmenuitem)w->USE.R.LastRow, NULL);
    } else {
      this->Insert(parent, ((Tmenu)parent)->LastI, NULL);
      SyncMenu((Tmenu)parent);
    }
    return this;
  }
  return NULL;
}
