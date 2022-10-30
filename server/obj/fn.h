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
  TobjFn Obj;
  TwidgetFn Widget;
  TgadgetFn Gadget;
  TwindowFn Window;
  TscreenFn Screen;
  TgroupFn Group;
  TrowFn Row;
  TmenuitemFn Menuitem;
  TmenuFn Menu;
  TmsgportFn Msgport;
  TmutexFn Mutex;
  TmsgFn Msg;
  TmoduleFn Module;
  TdisplayFn Display;
  TobjFn All;
};

extern TstructFn FnStruct;
#define Fn_Tobj (FnStruct.Obj)
#define Fn_Twidget (FnStruct.Widget)
#define Fn_Tgadget (FnStruct.Gadget)
#define Fn_Twindow (FnStruct.Window)
#define Fn_Tscreen (FnStruct.Screen)
#define Fn_Tgroup (FnStruct.Group)
#define Fn_Trow (FnStruct.Row)
#define Fn_Tmenuitem (FnStruct.Menuitem)
#define Fn_Tmenu (FnStruct.Menu)
#define Fn_Tmsgport (FnStruct.Msgport)
#define Fn_Tmutex (FnStruct.Mutex)
#define Fn_Tmsg (FnStruct.Msg)
#define Fn_Tmodule (FnStruct.Module)
#define Fn_Tdisplay (FnStruct.Display)

#endif /* TWIN_FN_H */
