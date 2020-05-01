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
#include <string.h> // memset()

widget s_widget::Create(fn_widget Fn, msgport Owner, dat XWidth, dat YWidth, uldat Attrib,
                        uldat Flags, dat Left, dat Up, tcell Fill) {
  widget W = (widget)0;

  if (Owner && (W = (widget)Fn->Fn_Obj->Create((fn_obj)Fn))) {
    W->FirstW = W->LastW = W->SelectW = NULL;
    W->Left = Left;
    W->Up = Up;
    W->XWidth = XWidth;
    W->YWidth = YWidth;
    W->Attrib = Attrib;
    W->Flags = Flags;
    W->XLogic = W->YLogic = 0;
    W->O_Prev = W->O_Next = NULL;
    W->Owner = NULL;

    W->ShutDownHook = NULL;
    W->Hook = NULL;
    W->WhereHook = NULL;
    W->MapUnMapHook = NULL;
    W->MapQueueMsg = NULL;

    W->USE_Fill = Fill;
    if (w_USE(W, USEEXPOSE))
      memset(&W->USE.E, '\0', sizeof(W->USE.E));

    W->Own(Owner);
  }
  return W;
}
