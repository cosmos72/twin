/*
 *  menu.cpp  --  define methods of server class Smenu
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
#include "fn.h"      // Fn_Tmenu
#include "hw.h"      // QueuedDrawArea2FullScreen
#include "methods.h" // InsertLast()
#include "twin.h"    // IS_SCREEN(), IS_WINDOW()
#include "obj/menu.h"
#include "obj/msgport.h"

#include <new>

Tmenu Smenu::Create(Tmsgport owner, tcolor colitem, tcolor colselect, tcolor coldisabled,
                    tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut,
                    byte flagdefcolinfo) {
  Tmenu m = NULL;
  if (owner) {
    void *addr = AllocMem0(sizeof(Smenu));
    if (addr) {
      m = new (addr) Smenu();
      m->Fn = Fn_Tmenu;
      if (!m->Init(owner, colitem, colselect, coldisabled, colselectdisabled, colshtcut,
                   colselshtcut, flagdefcolinfo)) {
        m->Delete();
        m = NULL;
      }
    }
  }
  return m;
}

Tmenu Smenu::Init(Tmsgport owner, tcolor colitem, tcolor colselect, tcolor coldisabled,
                  tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut,
                  byte flagdefcolinfo) {
  if (!owner || !Sobj::Init()) {
    return NULL;
  }
  ColItem = colitem;
  ColSelect = colselect;
  ColDisabled = coldisabled;
  ColSelectDisabled = colselectdisabled;
  ColShtCut = colshtcut;
  ColSelShtCut = colselshtcut;
  // FirstI = LastI = SelectI = NULL;
  CommonItems = tfalse;
  FlagDefColInfo = flagdefcolinfo;
  // Info = NULL;
  InsertLast(Menu, this, owner);
  return this;
}

void Smenu::Remove() {
  if (MsgPort) {
    RemoveGeneric((TobjEntry)this, (TobjList)&MsgPort->FirstMenu, NULL);
    MsgPort = (Tmsgport)0;
  }
}

void Smenu::Delete() {
  uldat count = 30;

  Tmsgport msgport = MsgPort;
  Twidget w, WNext;

  /*
   * warning:
   * the only way to get the list of windows that
   * are using this Tmenu is to scan the whole MsgPort.
   * We must UnMap() all those windows
   * as a Twindow without Tmenu cannot be displayed.
   *
   * optimization: if we are going to UnMap() a lot of windows,
   * we set QueuedDrawArea2FullScreen = true, so that the UnMap()
   * calls do not have to redraw every time.
   */
  if (!QueuedDrawArea2FullScreen) {
    for (w = msgport->FirstW; w && count; w = WNext) {
      WNext = w->O_Next;
      if (IS_WINDOW(w) && ((Twindow)w)->Menu == this) {
        if (w->Parent && IS_SCREEN(w->Parent)) {
          count--;
        }
      }
    }
    if (!count) {
      QueuedDrawArea2FullScreen = true;
    }
  }
  for (w = msgport->FirstW; w; w = WNext) {
    WNext = w->O_Next;
    if (IS_WINDOW(w) && ((Twindow)w)->Menu == this) {
      if (w->Parent && IS_SCREEN(w->Parent)) {
        w->UnMap();
        ((Twindow)w)->Menu = NULL;
      }
    }
  }
  Remove();
  DeleteList(FirstI);
  if (Info) {
    Info->Delete();
  }
  Sobj::Delete();
}
