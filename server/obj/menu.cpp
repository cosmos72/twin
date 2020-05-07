/*
 *  menu.cpp  --  define methods of server class s_menu
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
#include "fn.h"      // Fn_menu
#include "id.h"      // AssignId()
#include "methods.h" // InsertLast()
#include "obj/menu.h"
#include "obj/msgport.h"

#include <new>

menu s_menu::Create(msgport owner, tcolor colitem, tcolor colselect, tcolor coldisabled,
                    tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut,
                    byte flagdefcolinfo) {
  menu m = NULL;
  if (owner) {
    void *addr = AllocMem0(sizeof(s_menu));
    if (addr) {
      m = new (addr) s_menu();
      m->Fn = Fn_menu;
      if (!m->Init(owner, colitem, colselect, coldisabled, colselectdisabled, colshtcut,
                   colselshtcut, flagdefcolinfo)) {
        m->Delete();
        m = NULL;
      }
    }
  }
  return m;
}

menu s_menu::Init(msgport owner, tcolor colitem, tcolor colselect, tcolor coldisabled,
                  tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut,
                  byte flagdefcolinfo) {
  if (!owner || !((obj)this)->Init()) {
    return NULL;
  }
  this->ColItem = colitem;
  this->ColSelect = colselect;
  this->ColDisabled = coldisabled;
  this->ColSelectDisabled = colselectdisabled;
  this->ColShtCut = colshtcut;
  this->ColSelShtCut = colselshtcut;
  // this->FirstI = this->LastI = this->SelectI = NULL;
  this->CommonItems = tfalse;
  this->FlagDefColInfo = flagdefcolinfo;
  // this->Info = NULL;
  InsertLast(Menu, this, owner);
  return this;
}
