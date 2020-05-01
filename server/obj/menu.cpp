/*
 *  menu.cpp  --  define methods of server class s_menu
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/menu.h"
#include "obj/msgport.h"
#include "methods.h" // InsertLast()

menu s_menu::Create(fn_menu Fn, msgport port, tcolor colitem, tcolor colselect, tcolor coldisabled,
                    tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut,
                    byte flagdefcolinfo) {
  menu Menu = NULL;
  if (port && (Menu = (menu)s_obj::Create((fn_obj)Fn))) {
    Menu->ColItem = colitem;
    Menu->ColSelect = colselect;
    Menu->ColDisabled = coldisabled;
    Menu->ColSelectDisabled = colselectdisabled;
    Menu->ColShtCut = colshtcut;
    Menu->ColSelShtCut = colselshtcut;
    Menu->FirstI = Menu->LastI = Menu->SelectI = NULL;
    Menu->CommonItems = tfalse;
    Menu->FlagDefColInfo = flagdefcolinfo;
    Menu->Info = NULL;
    InsertLast(Menu, Menu, port);
  }
  return Menu;
}
