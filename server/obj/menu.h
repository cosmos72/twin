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
  void (*Insert)(Tmenu, Tmsgport, Tmenu Prev, Tmenu Next);
  void (*Remove)(Tmenu);
  void (*Delete)(Tmenu);
  /* Tmenu */
  TobjFn Fn_Obj;
  Trow (*SetInfo)(Tmenu, byte Flags, ldat Len, const char *Text, const tcolor *ColText);
  Tmenuitem (*FindItem)(Tmenu, dat i);
  Tmenuitem (*GetSelectedItem)(Tmenu);
  Tmenuitem (*RecursiveGetSelectedItem)(Tmenu, dat *depth);
  void (*SetSelectedItem)(Tmenu, Tmenuitem);
};

struct Smenu : public Sobj {
  TmenuFn Fn;
  Tmenu Prev, Next; /* in the same Tmsgport */
  Tmsgport MsgPort;
  /* Tmenu */
  tcolor ColItem, ColSelect, ColDisabled, ColSelectDisabled, ColShtCut, ColSelShtCut;
  byte CommonItems;
  byte FlagDefColInfo;
  Trow Info;
  Tmenuitem FirstI, LastI, SelectI;

  static Tmenu Create(Tmsgport port, tcolor colitem, tcolor colselect, tcolor coldisabled,
                      tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut,
                      byte flagdefcolinfo);
  Tmenu Init(Tmsgport port, tcolor colitem, tcolor colselect, tcolor coldisabled,
             tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut, byte flagdefcolinfo);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(Tmsgport owner, Tmenu prev, Tmenu next) {
    Fn->Insert(this, owner, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  /* Tmenu */
  Trow SetInfo(byte flags, ldat len, const char *text, const tcolor *coltext) {
    return Fn->SetInfo(this, flags, len, text, coltext);
  }
  Tmenuitem FindItem(dat i) {
    return Fn->FindItem(this, i);
  }
  Tmenuitem GetSelectedItem() {
    return Fn->GetSelectedItem(this);
  }
  Tmenuitem RecursiveGetSelectedItem(dat *depth) {
    return Fn->RecursiveGetSelectedItem(this, depth);
  }
  void SetSelectedItem(Tmenuitem item) {
    Fn->SetSelectedItem(this, item);
  }
};

#endif /* TWIN_MENU_H */
