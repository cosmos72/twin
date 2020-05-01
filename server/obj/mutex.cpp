/*
 *  row.cpp  --  define methods of server class s_row
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/mutex.h"
#include "obj/all.h" // extern All
#include "methods.h" // Act(), InsertLast()
#include <string.h>  // memcmp()

mutex s_mutex::Create(fn_mutex Fn, msgport Owner, byte NameLen, const char *Name, byte Perm) {
  byte Possible = PERM_WRITE;
  mutex CurrMutex, NewMutex = (mutex)0, AlreadyMutex = (mutex)0;

  if (!Perm || !Owner)
    return NewMutex;

  CurrMutex = ::All->FirstMutex;
  while (CurrMutex && Possible) {
    if (NameLen == CurrMutex->NameLen && !memcmp(Name, CurrMutex->Name, NameLen)) {
      if (CurrMutex->Owner == Owner) {
        AlreadyMutex = CurrMutex;
        continue;
      }
      Possible &= CurrMutex->Perm & PERM_WRITE ? PERM_NONE
                                               : CurrMutex->Perm & PERM_READ ? PERM_READ : (byte)~0;
    }
    CurrMutex = CurrMutex->Next;
  }

  if (Possible >= Perm) {
    if (AlreadyMutex) {
      AlreadyMutex->Perm = Perm;
      NewMutex = AlreadyMutex;
    } else if ((NewMutex = (mutex)s_obj::Create((fn_obj)Fn))) {
      NewMutex->Perm = Perm;
      InsertLast(Mutex, NewMutex, ::All);
      NewMutex->Owner = (msgport)0;
      Act(Own, NewMutex)(NewMutex, Owner);
    }
  }
  return NewMutex;
}
