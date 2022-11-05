/*
 *  msg.cpp  --  define methods of server class Smsg
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "alloc.h" // AllocMem()
#include "log.h"
#include "fn.h"      // Fn_Tmsg
#include "methods.h" // RemoveGeneric()
#include "obj/id.h"  // AssignId()
#include "obj/msg.h"
#include "obj/msgport.h"
#include "stl/fmt.h"

#include <new> // placement new

Tmsg Smsg::Create(udat type, size_t eventlen) {

  const size_t headerlen = sizeof(Smsg);
  if (eventlen > ((uldat)-1) - headerlen || eventlen > ((size_t)-1) - headerlen) {
    log(ERROR) << "twin: CreateMsg(): event length too large: " << eventlen << "\n";
    return NULL;
  }
  Tmsg m;
  if ((m = (Tmsg)AllocMem0(headerlen + eventlen))) {
    new (m) Smsg(); // in-place constructor
    m->Fn = Fn_Tmsg;
    if (!m->Init(type, (uldat)eventlen)) {
      m->Delete();
      m = NULL;
    }
  }
  return m;
}

Tmsg Smsg::Init(udat type, uldat eventlen) {
  if (AssignId(e_id(Fn->Magic), (Tobj)this)) {
    // this->Prev = this->Next = NULL;
    // this->MsgPort = NULL;
    this->Type = type;
    this->Len = eventlen;
    return this;
  }
  return NULL;
}

void Smsg::Remove() {
  if (MsgPort) {
    RemoveGeneric((TobjEntry)this, (TobjList)&MsgPort->FirstMsg, NULL);
    MsgPort = (Tmsgport)0;
  }
}

void Smsg::Delete() {
  Remove();
  Sobj::Delete();
}
