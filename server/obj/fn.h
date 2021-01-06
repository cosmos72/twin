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

#ifndef TWIN_FN_H
#define TWIN_FN_H

#include "obj/fwd.h"

/* in the same order as the #defines above ! */
struct s_fn_struct {
  fn_obj obj;
  fn_widget widget;
  fn_gadget gadget;
  fn_window window;
  fn_screen screen;
  fn_group group;
  fn_row row;
  fn_menuitem menuitem;
  fn_menu menu;
  fn_msgport msgport;
  fn_mutex mutex;
  fn_msg msg;
  fn_module module;
  fn_display_hw display_hw;
  fn_obj all;
};

extern fn_struct FnStruct;
#define Fn_obj (FnStruct.obj)
#define Fn_widget (FnStruct.widget)
#define Fn_gadget (FnStruct.gadget)
#define Fn_window (FnStruct.window)
#define Fn_screen (FnStruct.screen)
#define Fn_group (FnStruct.group)
#define Fn_row (FnStruct.row)
#define Fn_menuitem (FnStruct.menuitem)
#define Fn_menu (FnStruct.menu)
#define Fn_msgport (FnStruct.msgport)
#define Fn_mutex (FnStruct.mutex)
#define Fn_msg (FnStruct.msg)
#define Fn_module (FnStruct.module)
#define Fn_display_hw (FnStruct.display_hw)

#endif /* TWIN_FN_H */
