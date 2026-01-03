/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_METHODS_H
#define TWIN_METHODS_H

#include "obj/window.h" // tpos

byte FindInfo(Tmenu Menu, dat i);

void IncMouseMotionN(void);
void DecMouseMotionN(void);

#define DeleteList(First)                                                                          \
  do {                                                                                             \
    while (First) {                                                                                \
      First->Delete();                                                                             \
    }                                                                                              \
  } while (0)

#define InsertOnly(list, obj, parent) (obj)->Insert((parent), NULL, NULL)

#define InsertFirst(list, obj, parent) (obj)->Insert((parent), NULL, (parent)->list.First)

#define InsertMiddle(list, obj, parent, prev_obj, next_obj)                                        \
  (obj)->Insert((parent), (prev_obj), (next_obj))

#define InsertLast(list, obj, parent) (obj)->Insert((parent), (parent)->list.Last, NULL)

#define MoveFirst(list, parent, obj) ((obj)->Remove(), InsertFirst(list, obj, parent))

#define MoveLast(list, parent, obj) ((obj)->Remove(), InsertLast(list, obj, parent))

#define SendMsg(msgport, whichmsg) InsertLast(Msgs, (whichmsg), (msgport))

#define Row4Menu(w, code, flags, len, name)                                                        \
  ((Trow)Smenuitem::Create4Menu((Tobj)(w), (Twindow)0, (code), (flags), (len), (name)))

void *OverrideMth(void **where, void *OldMethod, void *NewMethod);

#define OverrideMethod(objtype, command, ExpectedMethod, NewMethod)                                \
  OverrideMth((void **)&(Fn_T##objtype->command), (void *)ExpectedMethod, (void *)NewMethod)

#endif /* TWIN_METHODS_H */
