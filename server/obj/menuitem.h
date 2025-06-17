/*
 *  menuitem.h  --  declare server class Smenuitem
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_MENUITEM_H
#define TWIN_MENUITEM_H

#include "obj/row.h"

class Smenuitem : public Srow {
public:
  /* Tmenuitem */
  Twindow Window;
  dat Left, ShortCut;
  ldat WCurY;

private:
  Tmenuitem Init(Tobj parent, Twindow window, udat code, byte flags, dat left, ldat len,
                 dat shortcut, const char *name);

public:
  static Tmenuitem Create(Tobj Parent, Twindow window, udat code, byte flags, dat left, ldat len,
                          dat shortcut, const char *name);
  static Tmenuitem Create4Menu(Tobj parent, Twindow window, udat code, byte flags, ldat len,
                               const char *name);
  static uldat Create4MenuCommon(Tmenu menu);

  /* Tobj */
  virtual void Delete() OVERRIDE;
  virtual void Remove() OVERRIDE;

  /* Tmenuitem */
  void Insert(Tobj parent, Tmenuitem prev, Tmenuitem next);

  Tmenuitem Prev() const; // cast this->Prev to Tmenuitem
  Tmenuitem Next() const; // cast this->Next to Tmenuitem
};

/* Some common Tmenuitem codes: */
#define COD_CANCEL 0xF700
#define COD_OK 0xF701
#define COD_HELP 0xF702
#define COD_OPEN 0xF703
#define COD_DELETE 0xF704
#define COD_NEW 0xF705

/* don't use codes above or equal to this one! */
#define COD_RESERVED 0xF800

#endif /* TWIN_MENUITEM_H */
