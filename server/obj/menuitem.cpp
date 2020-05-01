/*
 *  row.cpp  --  define methods of server class s_row
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/menuitem.h"
#include "algo.h"         // Max2()
#include "alloc.h"        // FreeMem(), CloneStr2TRune()
#include "resize.h"       // SyncMenu()
#include "twin.h"         // IS_WINDOW()
#include <Tw/datasizes.h> // TW_MAXLDAT

menuitem s_menuitem::Create(fn_menuitem Fn, obj Parent, window Window, udat Code, byte Flags,
                            dat Left, ldat Len, dat ShortCut, const char *Name) {
  menuitem MenuItem = (menuitem)0;
  trune *_Name = NULL;

  if (Parent && (IS_MENU(Parent) || (IS_WINDOW(Parent) && W_USE((window)Parent, USEROWS))) &&
      (!Window || IS_WINDOW(Window)) && Name && (_Name = CloneStr2TRune(Name, Len)) &&
      (MenuItem = (menuitem)s_row::Create((fn_row)Fn, Code, Flags))) {

    MenuItem->Len = Len;
    MenuItem->Text = _Name;
    MenuItem->Window = Window;
    MenuItem->Left = Left;
    MenuItem->ShortCut = ShortCut;
    MenuItem->WCurY = TW_MAXLDAT;

    if (Window)
      Window->MenuItem = MenuItem;

    if (IS_WINDOW(Parent)) {
      Window = (window)Parent;

      if ((ldat)Window->XWidth < (Len = Max2((ldat)10, Len + (ldat)2)))
        Window->XWidth = Len;

      if ((ldat)Window->YWidth < (Len = Min2(TW_MAXDAT, Window->HLogic + (ldat)3)))
        Window->YWidth = Len;

      MenuItem->Insert((obj)Window, (menuitem)Window->USE.R.LastRow, NULL);
    } else {
      MenuItem->Insert(Parent, ((menu)Parent)->LastI, NULL);
      SyncMenu((menu)Parent);
    }
    return MenuItem;
  }
  if (_Name)
    FreeMem(_Name);
  return MenuItem;
}
