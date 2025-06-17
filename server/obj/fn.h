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

class SstructFn {
public:
  TobjFn Obj;
  TwidgetFn Widget;
  TwidgetFn Gadget;
  TwindowFn Window;
  TwidgetFn Screen;
  TmoduleFn Module;
};

extern SstructFn FnStruct;
#define Fn_Tobj (FnStruct.Obj)
#define Fn_Twidget (FnStruct.Widget)
#define Fn_Tgadget (FnStruct.Gadget)
#define Fn_Twindow (FnStruct.Window)
#define Fn_Tscreen (FnStruct.Screen)
#define Fn_Tmodule (FnStruct.Module)

#endif /* TWIN_FN_H */
