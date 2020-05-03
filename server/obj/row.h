/*
 *  group.h  --  declare server class s_group
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_ROW_H
#define _TWIN_ROW_H

#include "obj/obj.h"

/* row */

struct s_fn_row {
  uldat Magic;
  void (*Insert)(row, window, row prev, row next);
  void (*Remove)(row);
  void (*Delete)(row);
  void (*ChangeField)(row, udat field, uldat clearmask, uldat xormask);
  /* row */
  fn_obj Fn_Obj;
  byte (*SetText)(row, uldat len, const char *text, byte defaultcol);
  byte (*SetTRune)(row, uldat len, const trune *runes, byte defaultcol);
  void (*Raise)(row);
  void (*Lower)(row);
};

struct s_row : public s_obj {
  fn_row Fn;
  row Prev, Next;
  window Window;
  /* row */
  udat Code;
  byte Flags;
  uldat Len, MaxLen;
  uldat Gap, LenGap;
  trune *Text;
  tcolor *ColText;

  static row Create(udat code, byte flags);
  row Init(udat code, byte flags);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(window w, row prev, row next) {
    Fn->Insert(this, w, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  /* row */
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

#endif /* _TWIN_ROW_H */
