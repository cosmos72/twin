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

#include "alloc.h"   // AllocMem0()
#include "obj/all.h" // extern All
#include "obj/mutex.h"
#include "methods.h" // Act(), InsertLast()

#include <new>
#include <cstring> // memcmp()

mutex s_mutex::Create(msgport owner, byte namelen, const char *name, byte perm) {
  byte mask = PERM_WRITE;
  mutex curr, x = NULL, old = NULL;

  if (!perm || !owner)
    return x;

  curr = ::All->FirstMutex;
  while (curr && mask) {
    if (namelen == curr->NameLen && !memcmp(name, curr->Name, namelen)) {
      if (curr->Owner == owner) {
        old = curr;
        continue;
      }
      mask &= curr->Perm & PERM_WRITE ? PERM_NONE : curr->Perm & PERM_READ ? PERM_READ : ~(byte)0;
    }
    curr = curr->Next;
  }

  if (mask >= perm) {
    if (old) {
      old->Perm = perm;
      x = old;
    } else {
      void *addr = AllocMem0(sizeof(s_mutex));
      if (addr) {
        x = new (addr) s_mutex();
        x->Fn = Fn_mutex;
        if (!x->Init(owner, namelen, name, perm)) {
          x->Delete();
          x = NULL;
        }
      }
    }
  }
  return x;
}

mutex s_mutex::Init(msgport owner, byte namelen, const char *name, byte perm) {
  if (!((obj)this)->Init()) {
    return NULL;
  }
  this->Perm = perm;
  InsertLast(Mutex, this, ::All);
  // this->Owner = NULL;
  this->Own(owner);
  return this;
}
