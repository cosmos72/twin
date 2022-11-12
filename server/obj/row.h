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
  /* Trow */
  TobjFn Fn_Obj;
  bool (*SetText)(Trow self, uldat len, const char *text, bool default_color);
  bool (*SetTRune)(Trow self, uldat len, const trune *runes, bool default_color);
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

protected:
  Trow Init(udat code, byte flags, e_id class_id);

public:
  static Trow Create(udat code, byte flags);

  /* Tobj */
  virtual void Delete() OVERRIDE;
  virtual void Remove() OVERRIDE;

  /* Trow */
  Twindow Window() const; // cast this->Parent to Twindow

  void Insert(Twindow w, Trow prev, Trow next);

  bool SetText(uldat len, const char *text, bool default_color) {
    return Fn->SetText(this, len, text, default_color);
  }
  bool SetTRune(uldat len, const trune *runes, bool default_color) {
    return Fn->SetTRune(this, len, runes, default_color);
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
