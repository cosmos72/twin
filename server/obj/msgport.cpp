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
#include "hw.h"      // QueuedDrawArea2FullScreen
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

  if (!handler || !Sobj::Init(Tmsgport_class_id)) {
    return NULL;
  }
  if (name && !(Name = CloneStrL(name, namelen))) {
    return NULL;
  }
  NameLen = namelen;
  WakeUp = wakeup;
  Handler = handler;
  // ShutDownHook = (void (*)(Tmsgport))0;
  PauseDuration.Seconds = pausesec;
  PauseDuration.Fraction = pausefraction;
  (void)SumTime(&CallTime, &::All->Now, &PauseDuration);
  RemoteData.Fd = NOFD;
  RemoteData.ChildPid = NOPID;
  RemoteData.FdSlot = NOSLOT;
  /*
  Msgs.First = Msgs.Last = NULL;
  Menus.First = Menus.Last = NULL;
  Widgets.First = Widgets.Last = NULL;
  Groups.First = Groups.Last = NULL;
  Mutexes.First = Mutexes.Last = NULL;
  CountE = SizeE = (uldat)0;
  Es = NULL;
  AttachHW = NULL;
  */
  InsertMiddle(MsgPorts, this, ::All, WakeUp ? (Tmsgport)0 : ::All->MsgPorts.Last,
               WakeUp ? ::All->MsgPorts.First : (Tmsgport)0);
  SortMsgPortByCallTime(this);
  return this;
}

void Smsgport::Delete() {
  Twidget w;
  uldat count = 20;

  /*
   * optimization: if we are going to UnMap() a lot of windows,
   * we set QueuedDrawArea2FullScreen = true, so that the UnMap()
   * calls do not have to redraw every time.
   */
  for (w = Widgets.First; w && count; w = w->O_Next) {
    if (IS_WINDOW(w) && w->Parent && IS_SCREEN(w->Parent)) {
      count--;
    }
  }
  if (!count) {
    QueuedDrawArea2FullScreen = true;
  }
  if (ShutDownHook) {
    ShutDownHook(this);
  }
  /*
   * must delete the Menus first, as widgets also contain Tmenuitem's windows,
   * which can only be deleted after deleting the corresponding Tmenuitem.
   */
  DeleteList(Msgs.First);
  DeleteList(Menus.First);
  DeleteList(Widgets.First);
  DeleteList(Groups.First);
  DeleteList(Mutexes.First);

  Remove();
  if (Name) {
    FreeMem(Name);
  }
  Sobj::Delete();
}

void Smsgport::Insert(Tall parent, Tmsgport prev, Tmsgport next) {
  if (parent && !All) {
    parent->MsgPorts.Insert(this, prev, next);
    All = parent;
  }
}

void Smsgport::Remove() {
  if (::All->RunMsgPort == this) {
    ::All->RunMsgPort = Next;
  }
  if (All) {
    All->MsgPorts.Remove(this);
    All = (Tall)0;
  }
}
