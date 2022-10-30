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

#include "alloc.h"   // AllocMem0(), AllocMem(), CloneStrL(), CopyMem()
#include "builtin.h" // Builtin_MsgPort
#include "fn.h"      // Fn_Tscreen
#include "obj/screen.h"

#include <new>
#include <Tw/datasizes.h> // TW_MAXDAT

Tscreen Sscreen::Create(dat namelen, const char *name, dat bgwidth, dat bgheight, const tcell *bg) {
  Tscreen S = NULL;
  if (bgwidth && bgheight) {
    void *addr = AllocMem0(sizeof(Sscreen));
    if (addr) {
      S = new (addr) Sscreen();
      S->Fn = Fn_Tscreen;
      if (!S->Init(namelen, name, bgwidth, bgheight, bg)) {
        S->Delete();
        S = NULL;
      }
    }
  }
  return S;
}

Tscreen Sscreen::Init(dat namelen, const char *name, dat bgwidth, dat bgheight, const tcell *bg) {
  size_t size = (size_t)bgwidth * bgheight * sizeof(tcell);

  if (!size || !((Twidget)this)
                    ->Init(Builtin_MsgPort, TW_MAXDAT, TW_MAXDAT, 0, SCREENFL_USEBG, 0, 0, bg[0])) {
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
  // this->HookW = NULL;
  // this->FnHookW = NULL;
  this->USE.B.BgWidth = bgwidth;
  this->USE.B.BgHeight = bgheight;
  CopyMem(bg, this->USE.B.Bg, size);
  this->All = NULL;
  return this;
}
