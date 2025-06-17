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

class Smutex : public Sobj {
public:
  TobjFn Fn;
  Tmutex Prev, Next; /* in the same All */
  Tall All;
  /* Tmutex */
  Tmutex O_Prev, O_Next; /* owned by the same MsgPort */
  Tmsgport Owner;
  byte Perm, NameLen;
  char *Name;

private:
  Tmutex Init(Tmsgport owner, byte namelen, const char *name, byte perm);

public:
  static Tmutex Create(Tmsgport owner, byte namelen, const char *name, byte perm);

  /* Tobj */
  virtual void Delete() OVERRIDE;
  virtual void Remove() OVERRIDE;

  /* Tmutex */
  void Insert(Tall a, Tmutex prev, Tmutex next);

  void Own(Tmsgport owner);
  void DisOwn();
};

#define PERM_NONE ((byte)0)
#define PERM_READ ((byte)1)
#define PERM_WRITE ((byte)2)

#endif /* TWIN_MUTEX_H */
