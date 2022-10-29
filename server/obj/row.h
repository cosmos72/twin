/*
 *  group.h  --  declare server class Sgroup
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_ROW_H
#define TWIN_ROW_H

#include "obj/obj.h"

/* Trow */

struct SrowFn {
  uldat Magic;
  void (*Insert)(Trow, Twindow, Trow prev, Trow next);
  void (*Remove)(Trow);
  void (*Delete)(Trow);
  void (*ChangeField)(Trow, udat field, uldat clearmask, uldat xormask);
  /* Trow */
  TobjFn Fn_Obj;
  byte (*SetText)(Trow, uldat len, const char *text, byte defaultcol);
  byte (*SetTRune)(Trow, uldat len, const trune *runes, byte defaultcol);
  void (*Raise)(Trow);
  void (*Lower)(Trow);
};

struct Srow : public Sobj {
  TrowFn Fn;
  Trow Prev, Next;
  Twindow Window;
  /* Trow */
  udat Code;
  byte Flags;
  uldat Len, MaxLen;
  uldat Gap, LenGap;
  trune *Text;
  tcolor *ColText;

  static Trow Create(udat code, byte flags);
  Trow Init(udat code, byte flags);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(Twindow w, Trow prev, Trow next) {
    Fn->Insert(this, w, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  /* Trow */
  byte SetText(uldat len, const char *text, byte defaultcol) {
    return Fn->SetText(this, len, text, defaultcol);
  }
  byte SetTRune(uldat len, const trune *runes, byte defaultcol) {
    return Fn->SetTRune(this, len, runes, defaultcol);
  }
  void Raise() {
    Fn->Raise(this);
  }
  void Lower() {
    Fn->Lower(this);
  }
};

/*Flags : */
#define ROW_INACTIVE ((byte)0x00)
#define ROW_ACTIVE ((byte)0x01)
#define ROW_IGNORE ((byte)0x02)
#define ROW_DEFCOL ((byte)0x04)

#endif /* TWIN_ROW_H */
