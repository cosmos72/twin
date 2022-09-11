/*
 *  mutex.h  --  declare server class s_mutex
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

struct s_fn_mutex {
  e_id Magic;
  void (*Insert)(mutex, all, mutex Prev, mutex Next);
  void (*Remove)(mutex);
  void (*Delete)(mutex);
  void (*ChangeField)(mutex, udat field, uldat CLEARMask, uldat XORMask);
  /* mutex */
  fn_obj Fn_Obj;
  void (*Own)(mutex, msgport);
  void (*DisOwn)(mutex);
};

struct s_mutex : public s_obj {
  fn_mutex Fn;
  mutex Prev, Next; /* in the same All */
  all All;
  /* mutex */
  mutex O_Prev, O_Next; /* owned by the same MsgPort */
  msgport Owner;
  byte Perm, NameLen;
  char *Name;

  static mutex Create(msgport owner, byte namelen, const char *name, byte perm);
  mutex Init(msgport owner, byte namelen, const char *name, byte perm);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(all a, mutex prev, mutex next) {
    Fn->Insert(this, a, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  /* mutex */
  void Own(msgport owner) {
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
