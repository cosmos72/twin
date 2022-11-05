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
  void (*Insert)(Trow self, Tobj parent, Trow prev, Trow next);
  /* Trow */
  TobjFn Fn_Obj;
  byte (*SetText)(Trow self, uldat len, const char *text, byte defaultcol);
  byte (*SetTRune)(Trow self, uldat len, const trune *runes, byte defaultcol);
  void (*Raise)(Trow self);
  void (*Lower)(Trow self);
};

struct Srow : public Sobj {
  TrowFn Fn;
  Trow Prev, Next;
  Tobj Parent;

  /* Trow */
  udat Code;
  byte Flags;
  uldat Len, MaxLen;
  uldat Gap, LenGap;
  trune *Text;
  tcolor *ColText;

  static Trow Create(udat code, byte flags);
  Trow Init(udat code, byte flags);

  /* Tobj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(Twindow w, Trow prev, Trow next);
  virtual void Remove() OVERRIDE;
  virtual void Delete() OVERRIDE;

  /* Trow */
  Twindow Window() const; // cast this->Parent to Twindow

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
