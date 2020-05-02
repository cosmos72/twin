/*
 *  msgport.cpp  --  define methods of server class s_msgport
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

msgport s_msgport::Create(byte namelen, const char *name, tany pausesec, tany pausefraction,
                          byte wakeup, void (*handler)(msgport)) {
  msgport p = NULL;
  if (name) {
    p = (msgport)AllocMem0(sizeof(s_msgport), 1);
    if (p) {
      p->Fn = Fn_msgport;
      if (!p->Init(namelen, name, pausesec, pausefraction, wakeup, handler)) {
        p->Delete();
        p = NULL;
      }
    }
  }
  return p;
}

msgport s_msgport::Init(byte namelen, const char *name, tany pausesec, tany pausefraction,
                        byte wakeup, void (*handler)(msgport)) {

  if (!handler || !((obj)this)->Init()) {
    return NULL;
  }
  if (name && !(this->Name = CloneStrL(name, namelen))) {
    return NULL;
  }
  this->WakeUp = wakeup;
  this->NameLen = namelen;
  this->Handler = handler;
  // this->ShutDownHook = (void (*)(msgport))0;
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
  InsertMiddle(MsgPort, this, ::All, WakeUp ? (msgport)0 : ::All->LastMsgPort,
               WakeUp ? ::All->FirstMsgPort : (msgport)0);
  SortMsgPortByCallTime(this);
  return this;
}
