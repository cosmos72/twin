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
#include "fn.h"      // Fn_Tobj
#include "methods.h" // RemoveGeneric()
#include "obj/all.h"
#include "obj/id.h" // AssignId()
#include "obj/msg.h"
#include "obj/msgport.h"
#include "stl/fmt.h"
#include "twin.h" // IS_MSG(), IS_MSGPORT()

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
    m->Fn = Fn_Tobj;
    if (!m->Init(type, (uldat)eventlen)) {
      m->Delete();
      m = NULL;
    }
  }
  return m;
}

Tmsg Smsg::Init(udat type, uldat eventlen) {
  if (AssignId(Tmsg_class_id, this)) {
    // this->Prev = this->Next = NULL;
    // this->MsgPort = NULL;
    this->Type = type;
    this->Len = eventlen;
    return this;
  }
  return NULL;
}

void Smsg::Delete() {
  Remove();
  Sobj::Delete();
}

void Smsg::Insert(Tmsgport parent, Tmsg prev, Tmsg next) {
  if (parent && !MsgPort) {
    /* if adding the first Tmsg, move the Tmsgport to the head
     * of Tmsgport list, so that the scheduler will run it */
    Tall all = parent->All;
    if (all && !parent->FirstMsg) {
      // MoveFirst is a macro, parent->All would be evaluated *after* it's set to NULL
      MoveFirst(MsgPort, all, parent);
    }

    InsertGeneric((TobjEntry)this, (TobjList)&parent->FirstMsg, (TobjEntry)prev, (TobjEntry)next,
                  NULL);
    MsgPort = parent;
  }
}

void Smsg::Remove() {
  if (MsgPort) {
    RemoveGeneric((TobjEntry)this, (TobjList)&MsgPort->FirstMsg, NULL);
    MsgPort = (Tmsgport)0;
  }
}
