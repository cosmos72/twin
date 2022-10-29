/*
 *  Tmsgport.cpp  --  define methods of server class Smsgport
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/msgport.h"

#include "alloc.h"   // AllocMem0(), CloneStrL()
#include "methods.h" // InsertMiddle()
#include "obj/all.h" // extern All
#include "util.h"    // SumTime()
#include "twin.h"    // NOFD, NOPID, NOSLOT

#include <new>

Tmsgport Smsgport::Create(byte namelen, const char *name, tany pausesec, tany pausefraction,
                          byte wakeup, void (*handler)(Tmsgport)) {
  Tmsgport p = NULL;
  if (name) {
    void *addr = AllocMem0(sizeof(Smsgport));
    if (addr) {
      p = new (addr) Smsgport();
      p->Fn = Fn_Tmsgport;
      if (!p->Init(namelen, name, pausesec, pausefraction, wakeup, handler)) {
        p->Delete();
        p = NULL;
      }
    }
  }
  return p;
}

Tmsgport Smsgport::Init(byte namelen, const char *name, tany pausesec, tany pausefraction,
                        byte wakeup, void (*handler)(Tmsgport)) {

  if (!handler || !((Tobj)this)->Init()) {
    return NULL;
  }
  if (name && !(this->Name = CloneStrL(name, namelen))) {
    return NULL;
  }
  this->WakeUp = wakeup;
  this->NameLen = namelen;
  this->Handler = handler;
  // this->ShutDownHook = (void (*)(Tmsgport))0;
  this->PauseDuration.Seconds = pausesec;
  this->PauseDuration.Fraction = pausefraction;
  (void)SumTime(&this->CallTime, &::All->Now, &this->PauseDuration);
  this->RemoteData.Fd = NOFD;
  this->RemoteData.ChildPid = NOPID;
  this->RemoteData.FdSlot = NOSLOT;
  /*
  this->FirstMsg = this->LastMsg = NULL;
  this->FirstMenu = this->LastMenu = NULL;
  this->FirstW = this->LastW = NULL;
  this->FirstGroup = this->LastGroup = NULL;
  this->FirstMutex = this->LastMutex = NULL;
  this->CountE = this->SizeE = (uldat)0;
  this->Es = NULL;
  this->AttachHW = NULL;
  */
  InsertMiddle(MsgPort, this, ::All, WakeUp ? (Tmsgport)0 : ::All->LastMsgPort,
               WakeUp ? ::All->FirstMsgPort : (Tmsgport)0);
  SortMsgPortByCallTime(this);
  return this;
}
