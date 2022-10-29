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
struct SstructFn {
  TobjFn obj;
  TwidgetFn Twidget;
  TgadgetFn gadget;
  TwindowFn window;
  TscreenFn screen;
  TgroupFn group;
  TrowFn row;
  TmenuitemFn menuitem;
  TmenuFn menu;
  TmsgportFn msgport;
  TmutexFn mutex;
  TmsgFn msg;
  TmoduleFn module;
  TdisplayFn Tdisplay;
  TobjFn all;
};

extern TstructFn FnStruct;
#define Fn_obj (FnStruct.obj)
#define Fn_Twidget (FnStruct.Twidget)
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
#define Fn_display_hw (FnStruct.Tdisplay)

#endif /* TWIN_FN_H */
