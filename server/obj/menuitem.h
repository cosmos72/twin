/*
 *  menuitem.h  --  declare server class s_menuitem
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_MENUITEM_H
#define _TWIN_MENUITEM_H

#include "obj/row.h"

struct s_menuitem {
  uldat Id;
  fn_menuitem Fn;
  menuitem Prev, Next;
  obj Parent;
  /* row */
  udat Code;
  byte Flags;
  uldat Len, MaxLen;
  uldat Gap, LenGap;
  trune *Text;
  tcolor *ColText;
  /* menuitem */
  window Window;
  dat Left, ShortCut;
  ldat WCurY;
};

struct s_fn_menuitem {
  uldat Magic, Size;
  menuitem (*Create)(fn_menuitem, obj Parent, window Window, udat Code, byte Flags, dat Left,
                     ldat Len, dat ShortCut, const char *Name);
  void (*Insert)(menuitem, obj, menuitem Prev, menuitem Next);
  void (*Remove)(menuitem);
  void (*Delete)(menuitem);
  void (*ChangeField)(menuitem, udat field, uldat CLEARMask, uldat XORMask);
  /* row */
  fn_obj Fn_Obj;
  byte (*SetText)(row, uldat Len, const char *Text, byte DefaultCol);
  byte (*SetTRune)(row, uldat Len, const trune *TRune, byte DefaultCol);
  /* menuitem */
  void (*Raise)(menuitem);
  void (*Lower)(menuitem);
  fn_row Fn_Row;
  menuitem (*Create4Menu)(fn_menuitem, obj Parent, window Window, udat Code, byte Flags, ldat Len,
                          const char *Name);
  uldat (*Create4MenuCommon)(fn_menuitem, menu);
  /* for compatibility this must return a non-zero value. */
};

/* Some common menuitem codes: */
#define COD_CANCEL 0xF700
#define COD_OK 0xF701
#define COD_HELP 0xF702
#define COD_OPEN 0xF703
#define COD_DELETE 0xF704
#define COD_NEW 0xF705

/* don't use codes above or equal to this one! */
#define COD_RESERVED 0xF800

#endif /* _TWIN_MENUITEM_H */
