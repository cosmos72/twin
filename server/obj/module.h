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
  /* Tmodule */
  TobjFn Fn_Obj;
  bool (*DlOpen)(Tmodule);
  void (*DlClose)(Tmodule);
};

struct Smodule : public Sobj {
  TmoduleFn Fn;
  Tmodule Prev, Next; /* in the same All */
  Tall All;
  /* Tmodule */
  uldat NameLen, Used;
  char *Name;
  void *Handle;
  bool (*DoInit)(void);

  static Tmodule Create(uldat namelen, const char *name);
  Tmodule Init(uldat namelen, const char *name);

  /* Tobj */
  virtual void Delete() OVERRIDE;
  virtual void Remove() OVERRIDE;

  /* Tmodule */
  void Insert(Tall parent, Tmodule prev, Tmodule next);

  bool DlOpen() {
    return Fn->DlOpen(this);
  }
  void DlClose() {
    Fn->DlClose(this);
  }
};

#endif /* TWIN_MODULE_H */
