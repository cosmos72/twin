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

struct SstructFn {
  TobjFn obj;
  TwidgetFn widget;
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
  TdisplayFn display;
  TobjFn all;
};

extern TstructFn FnStruct;
#define Fn_Tobj (FnStruct.obj)
#define Fn_Twidget (FnStruct.widget)
#define Fn_Tgadget (FnStruct.gadget)
#define Fn_Twindow (FnStruct.window)
#define Fn_Tscreen (FnStruct.screen)
#define Fn_Tgroup (FnStruct.group)
#define Fn_Trow (FnStruct.row)
#define Fn_Tmenuitem (FnStruct.menuitem)
#define Fn_Tmenu (FnStruct.menu)
#define Fn_Tmsgport (FnStruct.msgport)
#define Fn_Tmutex (FnStruct.mutex)
#define Fn_Tmsg (FnStruct.msg)
#define Fn_Tmodule (FnStruct.module)
#define Fn_Tdisplay (FnStruct.display)

#endif /* TWIN_FN_H */
