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

struct SmenuitemFn {
  uldat Magic;
  void (*Insert)(Tmenuitem, obj, Tmenuitem Prev, Tmenuitem Next);
  void (*Remove)(Tmenuitem);
  void (*Delete)(Tmenuitem);
  void (*ChangeField)(Tmenuitem, udat field, uldat CLEARMask, uldat XORMask);
  /* Trow */
  TobjFn Fn_Obj;
  byte (*SetText)(Trow, uldat Len, const char *Text, byte DefaultCol);
  byte (*SetTRune)(Trow, uldat Len, const trune *TRune, byte DefaultCol);
  /* Tmenuitem */
  void (*Raise)(Tmenuitem);
  void (*Lower)(Tmenuitem);
  TrowFn Fn_Row;
  Tmenuitem (*Create4Menu)(obj Parent, Twindow Window, udat Code, byte Flags, ldat Len,
                           const char *Name);
  uldat (*Create4MenuCommon)(Tmenu);
  /* for compatibility this must return a non-zero value. */
};

struct Smenuitem : public Sobj {
  TmenuitemFn Fn;
  Tmenuitem Prev, Next;
  obj Parent;
  /* Trow */
  udat Code;
  byte Flags;
  uldat Len, MaxLen;
  uldat Gap, LenGap;
  trune *Text;
  tcolor *ColText;
  /* Tmenuitem */
  Twindow Window;
  dat Left, ShortCut;
  ldat WCurY;

  static Tmenuitem Create(obj Parent, Twindow Window, udat Code, byte Flags, dat Left, ldat Len,
                          dat ShortCut, const char *Name);
  Tmenuitem Init(obj Parent, Twindow Window, udat Code, byte Flags, dat Left, ldat Len,
                 dat ShortCut, const char *Name);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(obj parent, Tmenuitem prev, Tmenuitem next) {
    Fn->Insert(this, parent, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
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
