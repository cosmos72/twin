/*
 *  screen.cpp  --  define methods of server class Swidget
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/screen.h"

#include "alloc.h"   // AllocMem0(), AllocMem(), CloneStrL(), CopyMem()
#include "builtin.h" // Builtin_MsgPort
#include "fn.h"      // Fn_Tscreen
#include "methods.h" // RemoveGeneric()
#include "obj/all.h"
#include "twin.h" // IS_ALL(), IS_SCREEN()

#include <new>
#include <Tw/datasizes.h> // TW_MAXDAT

Tscreen Sscreen::Create(dat namelen, const char *name, dat bgwidth, dat bgheight, const tcell *bg) {
  Tscreen screen = NULL;
  if (bgwidth && bgheight) {
    void *addr = AllocMem0(sizeof(Sscreen));
    if (addr) {
      screen = new (addr) Sscreen();
      screen->Fn = (TwidgetFn)Fn_Tscreen;
      if (!screen->Init(namelen, name, bgwidth, bgheight, bg)) {
        screen->Delete();
        screen = NULL;
      }
    }
  }
  return screen;
}

Tscreen Sscreen::Init(dat namelen, const char *name, dat bgwidth, dat bgheight, const tcell *bg) {
  size_t size = (size_t)bgwidth * bgheight * sizeof(tcell);

  if (!size || !Swidget::Init(Builtin_MsgPort, TW_MAXDAT, TW_MAXDAT, 0, SCREENFL_USEBG, 0, 0, bg[0],
                              Tscreen_class_id)) {
    return NULL;
  }
  if (name && !(this->Name = CloneStrL(name, namelen))) {
    return NULL;
  }
  if (!(this->USE.B.Bg = (tcell *)AllocMem(size))) {
    return NULL;
  }
  this->NameLen = namelen;
  // this->MenuWindow = this->ClickWindow = NULL;
  // this->HookMap.clear();
  this->USE.B.BgWidth = bgwidth;
  this->USE.B.BgHeight = bgheight;
  CopyMem(bg, this->USE.B.Bg, size);
  this->All = NULL;
  return this;
}

void Sscreen::Delete() {
  while (FirstW) {
    FirstW->UnMap();
  }
  Remove();
  if (S_USE(this, USEBG) && USE.B.Bg) {
    FreeMem(USE.B.Bg);
    USE.B.Bg = NULL;
  }
  Swidget::Delete();
}

void Sscreen::InsertWidget(Tobj parent, Twidget prev, Twidget next) {
  if ((parent && IS_ALL(parent)) && (!prev || IS_SCREEN(prev)) && (!next || IS_SCREEN(next))) {
    Insert((Tall)parent, (Tscreen)prev, (Tscreen)next);
  }
}

void Sscreen::Insert(Tall parent, Tscreen prev, Tscreen next) {
  if (parent && !All) {
    InsertGeneric((TobjEntry)this, (TobjList)&parent->FirstScreen, (TobjEntry)prev, (TobjEntry)next,
                  NULL);
    All = parent;
  }
}

void Sscreen::Remove() {
  if (All) {
    RemoveGeneric((TobjEntry)this, (TobjList)&All->FirstScreen, NULL);
    All = (Tall)0;
  }
}

void Sscreen::ChangeField(udat field, uldat clear_mask, uldat xor_mask) {
  Tscreen screen = this;
  if (screen)
    switch (field) {
    default:
      Swidget::ChangeField(field, clear_mask, xor_mask);
      break;
    }
}
