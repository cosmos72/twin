/*
 *  menu.h  --  declare server class Smenu
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_MENU_H
#define TWIN_MENU_H

#include "obj/obj.h"

struct SmenuFn {
  uldat Magic;
  void (*Insert)(menu, msgport, menu Prev, menu Next);
  void (*Remove)(menu);
  void (*Delete)(menu);
  void (*ChangeField)(menu, udat field, uldat CLEARMask, uldat XORMask);
  /* menu */
  TobjFn Fn_Obj;
  row (*SetInfo)(menu, byte Flags, ldat Len, const char *Text, const tcolor *ColText);
  menuitem (*FindItem)(menu, dat i);
  menuitem (*GetSelectedItem)(menu);
  menuitem (*RecursiveGetSelectedItem)(menu, dat *depth);
  void (*SetSelectedItem)(menu, menuitem);
};

struct Smenu : public Sobj {
  TmenuFn Fn;
  menu Prev, Next; /* in the same msgport */
  msgport MsgPort;
  /* menu */
  tcolor ColItem, ColSelect, ColDisabled, ColSelectDisabled, ColShtCut, ColSelShtCut;
  byte CommonItems;
  byte FlagDefColInfo;
  row Info;
  menuitem FirstI, LastI, SelectI;

  static menu Create(msgport port, tcolor colitem, tcolor colselect, tcolor coldisabled,
                     tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut,
                     byte flagdefcolinfo);
  menu Init(msgport port, tcolor colitem, tcolor colselect, tcolor coldisabled,
            tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut, byte flagdefcolinfo);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(msgport owner, menu prev, menu next) {
    Fn->Insert(this, owner, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  /* menu */
  row SetInfo(byte flags, ldat len, const char *text, const tcolor *coltext) {
    return Fn->SetInfo(this, flags, len, text, coltext);
  }
  menuitem FindItem(dat i) {
    return Fn->FindItem(this, i);
  }
  menuitem GetSelectedItem() {
    return Fn->GetSelectedItem(this);
  }
  menuitem RecursiveGetSelectedItem(dat *depth) {
    return Fn->RecursiveGetSelectedItem(this, depth);
  }
  void SetSelectedItem(menuitem item) {
    Fn->SetSelectedItem(this, item);
  }
};

#endif /* TWIN_MENU_H */
