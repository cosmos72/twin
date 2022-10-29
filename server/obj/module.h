/*
 *  module.h  --  declare server class Smodule
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_MODULE_H
#define TWIN_MODULE_H

#include "obj/fwd.h"
#include "obj/obj.h"
#include <Tw/datatypes.h>

/* Tmodule */

struct SmoduleFn {
  uldat Magic;
  void (*Insert)(Tmodule, all, Tmodule Prev, Tmodule Next);
  void (*Remove)(Tmodule);
  void (*Delete)(Tmodule);
  void (*ChangeField)(Tmodule, udat field, uldat CLEARMask, uldat XORMask);
  /* Tmodule */
  TobjFn Fn_Obj;
  bool (*DlOpen)(Tmodule);
  void (*DlClose)(Tmodule);
};

struct Smodule : public Sobj {
  TmoduleFn Fn;
  Tmodule Prev, Next; /* in the same All */
  all All;
  /* Tmodule */
  uldat NameLen, Used;
  char *Name;
  void *Handle;
  bool (*DoInit)(void);

  static Tmodule Create(uldat namelen, const char *name);
  Tmodule Init(uldat namelen, const char *name);

  /* Tobj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(all a, Tmodule prev, Tmodule next) {
    Fn->Insert(this, a, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  /* Tmodule */
  bool DlOpen() {
    return Fn->DlOpen(this);
  }
  void DlClose() {
    Fn->DlClose(this);
  }
};

#endif /* TWIN_MODULE_H */
