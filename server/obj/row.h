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

struct s_row {
  uldat Id;
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
};

struct s_fn_row {
  uldat Magic, Size;
  row (*Create)(fn_row, udat Code, byte Flags);
  void (*Insert)(row, window, row Prev, row Next);
  void (*Remove)(row);
  void (*Delete)(row);
  void (*ChangeField)(row, udat field, uldat CLEARMask, uldat XORMask);
  /* row */
  fn_obj Fn_Obj;
  byte (*SetText)(row, uldat Len, const char *Text, byte DefaultCol);
  byte (*SetTRune)(row, uldat Len, const trune *TRune, byte DefaultCol);
  void (*Raise)(row);
  void (*Lower)(row);
};

/*Flags : */
#define ROW_INACTIVE ((byte)0x00)
#define ROW_ACTIVE ((byte)0x01)
#define ROW_IGNORE ((byte)0x02)
#define ROW_DEFCOL ((byte)0x04)

#endif /* _TWIN_ROW_H */
