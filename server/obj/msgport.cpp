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

#include "alloc.h"   // CloneStrL()
#include "methods.h" // InsertMiddle()
#include "obj/all.h" // extern All
#include "util.h"    // SumTime()
#include "twin.h"    // NOFD, NOPID, NOSLOT

msgport s_msgport::Create(fn_msgport Fn, byte NameLen, const char *Name, tany PauseSec,
                          tany PauseFraction, byte WakeUp, void (*Handler)(msgport)) {

  msgport p = NULL;
  char *_Name;

  if (Handler && (!Name || (_Name = CloneStrL(Name, NameLen))) &&
      (p = (msgport)s_obj::Create((fn_obj)Fn))) {

    p->WakeUp = WakeUp;
    p->NameLen = NameLen;
    p->Name = _Name;
    p->Handler = Handler;
    p->ShutDownHook = (void (*)(msgport))0;
    p->PauseDuration.Seconds = PauseSec;
    p->PauseDuration.Fraction = PauseFraction;
    (void)SumTime(&p->CallTime, &::All->Now, &p->PauseDuration);
    p->RemoteData.Fd = NOFD;
    p->RemoteData.ChildPid = NOPID;
    p->RemoteData.FdSlot = NOSLOT;
    p->FirstMsg = p->LastMsg = NULL;
    p->FirstMenu = p->LastMenu = NULL;
    p->FirstW = p->LastW = NULL;
    p->FirstGroup = p->LastGroup = NULL;
    p->FirstMutex = p->LastMutex = NULL;
    p->CountE = p->SizeE = (uldat)0;
    p->Es = NULL;
    p->AttachHW = NULL;
    InsertMiddle(p, p, ::All, WakeUp ? (msgport)0 : ::All->LastMsgPort,
                 WakeUp ? ::All->FirstMsgPort : (msgport)0);
    SortMsgPortByCallTime(p);
  } else if (NameLen && _Name) {
    FreeMem(_Name);
  }
  return p;
}
