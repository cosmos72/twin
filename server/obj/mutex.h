/*
 *  mutex.h  --  declare server class Smutex
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_MUTEX_H
#define TWIN_MUTEX_H

#include "obj/obj.h"

struct SmutexFn {
  uldat Magic;
  void (*Insert)(Tmutex, all, Tmutex Prev, Tmutex Next);
  void (*Remove)(Tmutex);
  void (*Delete)(Tmutex);
  void (*ChangeField)(Tmutex, udat field, uldat CLEARMask, uldat XORMask);
  /* Tmutex */
  TobjFn Fn_Obj;
  void (*Own)(Tmutex, Tmsgport);
  void (*DisOwn)(Tmutex);
};

struct Smutex : public Sobj {
  TmutexFn Fn;
  Tmutex Prev, Next; /* in the same All */
  all All;
  /* Tmutex */
  Tmutex O_Prev, O_Next; /* owned by the same MsgPort */
  Tmsgport Owner;
  byte Perm, NameLen;
  char *Name;

  static Tmutex Create(Tmsgport owner, byte namelen, const char *name, byte perm);
  Tmutex Init(Tmsgport owner, byte namelen, const char *name, byte perm);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(all a, Tmutex prev, Tmutex next) {
    Fn->Insert(this, a, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  /* Tmutex */
  void Own(Tmsgport owner) {
    Fn->Own(this, owner);
  }
  void DisOwn() {
    Fn->DisOwn(this);
  }
};

#define PERM_NONE ((byte)0)
#define PERM_READ ((byte)1)
#define PERM_WRITE ((byte)2)

#endif /* TWIN_MUTEX_H */
