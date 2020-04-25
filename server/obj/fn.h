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
struct s_fn {
  fn_obj f_obj;
  fn_widget f_widget;
  fn_gadget f_gadget;
  fn_window f_window;
  fn_screen f_screen;
  fn_group f_group;
  fn_row f_row;
  fn_menuitem f_menuitem;
  fn_menu f_menu;
  fn_msgport f_msgport;
  fn_mutex f_mutex;
  fn_msg f_msg;
  fn_module f_module;
  fn_extension f_extension;
  fn_display_hw f_display_hw;
  fn_obj f_all;
};

#endif /* _TWIN_FN_H */
