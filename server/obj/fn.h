/*
 *  fn.h  --  declare server class s_fn
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_FN_H
#define _TWIN_FN_H

#include "obj/fwd.h"

/* in the same order as the #defines above ! */
struct s_fn_struct {
  fn_obj Obj;
  fn_widget Widget;
  fn_gadget Gadget;
  fn_window Window;
  fn_screen Screen;
  fn_group Group;
  fn_row Row;
  fn_menuitem Menuitem;
  fn_menu Menu;
  fn_msgport Msgport;
  fn_mutex Mutex;
  fn_msg Msg;
  fn_module Module;
  fn_extension Extension;
  fn_display_hw DisplayHW;
  fn_obj All;
};

extern fn_struct FnStruct;
#define FnWidget (FnStruct.Widget)
#define FnGadget (FnStruct.Gadget)
#define FnWindow (FnStruct.Window)
#define FnScreen (FnStruct.Screen)
#define FnGroup (FnStruct.Group)
#define FnRow (FnStruct.Row)
#define FnMenuItem (FnStruct.Menuitem)
#define FnMenu (FnStruct.Menu)
#define FnMsgPort (FnStruct.Msgport)
#define FnMutex (FnStruct.Mutex)
#define FnMsg (FnStruct.Msg)
#define FnModule (FnStruct.Module)
#define FnExtension (FnStruct.Extension)
#define FnDisplayHW (FnStruct.DisplayHW)

#endif /* _TWIN_FN_H */
