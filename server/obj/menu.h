/*
 *  menu.h  --  declare server class s_menu
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_MENU_H
#define _TWIN_MENU_H

#include "obj/obj.h"

struct s_menu {
  uldat Id;
  fn_menu Fn;
  menu Prev, Next; /* in the same msgport */
  msgport MsgPort;
  /* menu */
  tcolor ColItem, ColSelect, ColDisabled, ColSelectDisabled, ColShtCut, ColSelShtCut;
  byte CommonItems;
  byte FlagDefColInfo;
  row Info;
  menuitem FirstI, LastI, SelectI;
};
struct s_fn_menu {
  uldat Magic, Size;
  menu (*Create)(fn_menu, msgport MsgPort, tcolor ColItem, tcolor ColSelect, tcolor ColDisabled,
                 tcolor ColSelectDisabled, tcolor ColShtCut, tcolor ColSelShtCut,
                 byte FlagDefColInfo);
  void (*Insert)(menu, msgport, menu Prev, menu Next);
  void (*Remove)(menu);
  void (*Delete)(menu);
  void (*ChangeField)(menu, udat field, uldat CLEARMask, uldat XORMask);
  /* menu */
  fn_obj Fn_Obj;
  row (*SetInfo)(menu, byte Flags, ldat Len, const char *Text, const tcolor *ColText);
  menuitem (*FindItem)(menu, dat i);
  menuitem (*GetSelectedItem)(menu);
  menuitem (*RecursiveGetSelectedItem)(menu, dat *depth);
  void (*SetSelectedItem)(menu, menuitem);
};

#endif /* _TWIN_MENU_H */
