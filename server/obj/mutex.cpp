/*
 *  row.cpp  --  define methods of server class Srow
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
#include "twin.h"    // IS_ALL(), IS_MUTEX()

#include <new>
#include <cstring> // memcmp()

Tmutex Smutex::Create(Tmsgport owner, byte namelen, const char *name, byte perm) {
  byte mask = PERM_WRITE;
  Tmutex curr, x = NULL, old = NULL;

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
      void *addr = AllocMem0(sizeof(Smutex));
      if (addr) {
        x = new (addr) Smutex();
        x->Fn = Fn_Tobj;
        if (!x->Init(owner, namelen, name, perm)) {
          x->Delete();
          x = NULL;
        }
      }
    }
  }
  return x;
}

Tmutex Smutex::Init(Tmsgport owner, byte namelen, const char *name, byte perm) {
  if (!Sobj::Init(Tmutex_class_id)) {
    return NULL;
  }
  Perm = perm;
  InsertLast(Mutex, this, ::All);
  // Owner = NULL;
  Own(owner);
  return this;
}

void Smutex::Delete() {
  DisOwn();
  Remove();
  Sobj::Delete();
}

void Smutex::Insert(Tall parent, Tmutex prev, Tmutex next) {
  if (parent && !All) {
    InsertT(this, &All->FirstMutex, prev, next, NULL);
    All = parent;
  }
}

void Smutex::Remove() {
  if (All) {
    RemoveT(this, &All->FirstMutex, NULL);
    All = (Tall)0;
  }
}

void Smutex::Own(Tmsgport parent) {
  if (parent && !Owner) {
    if ((O_Prev = parent->LastMutex))
      parent->LastMutex->O_Next = this;
    else
      parent->FirstMutex = this;

    O_Next = (Tmutex)0;
    parent->LastMutex = this;

    Owner = parent;
  }
}

void Smutex::DisOwn() {
  Tmsgport parent = Owner;
  if (parent) {
    if (O_Prev)
      O_Prev->O_Next = O_Next;
    else if (parent->FirstMutex == this)
      parent->FirstMutex = O_Next;

    if (O_Next)
      O_Next->O_Prev = O_Prev;
    else if (parent->LastMutex == this)
      parent->LastMutex = O_Prev;

    O_Prev = O_Next = (Tmutex)0;

    Owner = (Tmsgport)0;
  }
}
