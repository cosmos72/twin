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
#include "methods.h" // MoveFirst()
#include "resize.h"  // SyncMenu()
#include "twin.h"    // IS_WINDOW()

#include <new>
#include <Tw/datasizes.h> // TW_MAXLDAT

Tmenuitem Smenuitem::Create(Tobj parent, Twindow w, udat code, byte flags, dat left, ldat len,
                            dat shortcut, const char *name) {
  Tmenuitem item = NULL;
  if (parent && (IS_MENU(parent) || (IS_WINDOW(parent) && W_USE((Twindow)parent, USEROWS))) &&
      (!w || IS_WINDOW(w)) && name) {

    void *addr = AllocMem0(sizeof(Smenuitem));
    if (addr) {
      item = new (addr) Smenuitem();
      item->Fn = Fn_Tobj;
      if (!item->Init(parent, w, code, flags, left, len, shortcut, name)) {
        item->Delete();
        item = NULL;
      }
    }
  }
  return item;
}

Tmenuitem Smenuitem::Init(Tobj parent, Twindow w, udat code, byte flags, dat left, ldat len,
                          dat shortcut, const char *name) {

  if (parent && (IS_MENU(parent) || (IS_WINDOW(parent) && W_USE((Twindow)parent, USEROWS))) &&
      (!w || IS_WINDOW(w)) && name && (Text = CloneStr2TRune(name, len)) &&
      Srow::Init(code, flags, Tmenuitem_class_id)) {

    Len = len;
    Window = w;
    Left = left;
    ShortCut = shortcut;
    WCurY = TW_MAXLDAT;

    if (w)
      w->MenuItem = this;

    if (IS_WINDOW(parent)) {
      w = (Twindow)parent;

      if ((ldat)w->XWidth < (len = Max2((ldat)10, len + (ldat)2)))
        w->XWidth = len;

      if ((ldat)w->YWidth < (len = Min2(TW_MAXDAT, w->HLogic + (ldat)3)))
        w->YWidth = len;

      Insert((Tobj)w, (Tmenuitem)w->USE.R.Rows.Last, NULL);
    } else {
      Insert(parent, ((Tmenu)parent)->Items.Last, NULL);
      SyncMenu((Tmenu)parent);
    }
    return this;
  }
  return NULL;
}

void Smenuitem::Delete() {
  Tobj parent = Parent;
  Remove();
  if (IS_MENU(parent)) {
    SyncMenu((Tmenu)parent);
  }
  if (Window) {
    Window->Delete();
    Window = (Twindow)0;
  }
  Srow::Delete();
}

Tmenuitem Smenuitem::PrevItem() const {
  Trow prev = Srow::Prev;
  return prev && IS_MENUITEM(prev) ? (Tmenuitem)prev : (Tmenuitem)0;
}

Tmenuitem Smenuitem::NextItem() const {
  Trow next = Srow::Next;
  return next && IS_MENUITEM(next) ? (Tmenuitem)next : (Tmenuitem)0;
}

void Smenuitem::Insert(Tobj parent, Tmenuitem prev, Tmenuitem next) {
  if (parent && !Parent) {
    if (IS_MENU(parent)) {
      ((Tmenu)parent)->Items.Insert(this, prev, next);
      Parent = parent;
    } else if (IS_WINDOW(parent)) {
      // call superclass implementation
      Srow::Insert((Twindow)parent, prev, next);
    }
  }
}

void Smenuitem::Remove() {
  if (Parent) {
    if (IS_MENU(Parent)) {
      ((Tmenu)Parent)->Items.Remove(this);
      Parent = (Tobj)0;
    } else {
      Srow::Remove();
    }
  }
}

Tmenuitem Smenuitem::Create4Menu(Tobj parent, Twindow window, udat code, byte flags, ldat len,
                                 const char *name) {

  if (!parent) {
    return (Tmenuitem)0;
  }
  dat left;
  if (IS_MENU(parent) && ((Tmenu)parent)->Items.Last)
    left = ((Tmenu)parent)->Items.Last->Left + ((Tmenu)parent)->Items.Last->Len;
  else
    left = (dat)1;

  dat shortcut = 0;
  while (shortcut < len && name[shortcut] == ' ')
    shortcut++;

  if (window) {
    window->Left = left;
  }
  return Smenuitem::Create(parent, window, code, flags, left, len, shortcut, name);
}

uldat Smenuitem::Create4MenuCommon(Tmenu menu) {
  if (!menu) {
    return 0;
  }
  menu->CommonItems = ttrue;
  SyncMenu(menu);
  return 1;
}
