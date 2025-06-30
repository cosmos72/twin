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

class Smenu : public Sobj {
public:
  TobjFn Fn;
  Tmenu Prev, Next; /* in the same Tmsgport */
  Tmsgport MsgPort;
  /* Tmenu */
  tcolor ColItem, ColSelect, ColDisabled, ColSelectDisabled, ColShtCut, ColSelShtCut;
  byte CommonItems;
  byte FlagDefColInfo;
  Trow Info;
  List<Tmenuitem> Items;
  Tmenuitem SelectI;

  static Tmenu Create(Tmsgport port, tcolor colitem, tcolor colselect, tcolor coldisabled,
                      tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut,
                      byte flagdefcolinfo);
  Tmenu Init(Tmsgport port, tcolor colitem, tcolor colselect, tcolor coldisabled,
             tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut, byte flagdefcolinfo);

  /* obj */
  virtual void Delete() OVERRIDE;
  void Insert(Tmsgport owner, Tmenu prev, Tmenu next);
  virtual void Remove() OVERRIDE;

  /* Tmenu */
  Trow SetInfo(byte flags, ldat len, const char *text, const tcolor *coltext);
  Tmenuitem FindItem(dat i) const;
  Tmenuitem GetSelectedItem() const;
  Tmenuitem RecursiveGetSelectedItem(dat *depth) const;
  void SetSelectedItem(Tmenuitem item);
};

#endif /* TWIN_MENU_H */
