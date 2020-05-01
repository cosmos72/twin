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

#ifndef _TWIN_MUTEX_H
#define _TWIN_MUTEX_H

#include "obj/fwd.h"

struct s_mutex {
  uldat Id;
  fn_mutex Fn;
  mutex Prev, Next; /* in the same All */
  all All;
  /* mutex */
  mutex O_Prev, O_Next; /* owned by the same MsgPort */
  msgport Owner;
  byte Perm, NameLen;
  char *Name;
};
struct s_fn_mutex {
  uldat Magic, Size, Used;
  mutex (*Create)(fn_mutex, msgport Owner, byte NameLen, const char *Name, byte Perm);
  void (*Insert)(mutex, all, mutex Prev, mutex Next);
  void (*Remove)(mutex);
  void (*Delete)(mutex);
  void (*ChangeField)(mutex, udat field, uldat CLEARMask, uldat XORMask);
  /* mutex */
  fn_obj Fn_Obj;
  void (*Own)(mutex, msgport);
  void (*DisOwn)(mutex);
};
#define PERM_NONE ((byte)0)
#define PERM_READ ((byte)1)
#define PERM_WRITE ((byte)2)

#endif /* _TWIN_MUTEX_H */
