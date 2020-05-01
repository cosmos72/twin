/*
 *  widget.cpp  --  define methods of server class s_widget
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/widget.h"
#include "obj/fn.h" // FnStruct

widget s_widget::Create(msgport owner, dat xwidth, dat ywidth, uldat attrib, uldat flags, dat left,
                        dat up, tcell use_fill) {
  return FnStruct.widget->Create(FnStruct.widget, owner, xwidth, ywidth, attrib, flags, left, up,
                                 use_fill);
}
