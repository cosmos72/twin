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

#include "obj/fn.h"
#include "obj/window.h" // tpos

byte FindInfo(Tmenu Menu, dat i);
Twindow FakeOpenTerm(const char *arg0, const char *const *argv);
Twidget FakeKbdFocus(Twidget w);
bool FakeWriteCharset(Twindow window, uldat len, const char *charset_bytes);
bool FakeWriteUtf8(Twindow window, uldat len, const char *utf8_bytes);
bool FakeWriteTRune(Twindow window, uldat len, const trune *runes);
bool FakeWriteTCell(Twindow window, dat x, dat y, uldat len, const tcell *cells);
tpos FakeFindBorderWindow(Twindow w, dat u, dat v, byte border, tcell *ptr_cell);

void IncMouseMotionN(void);
void DecMouseMotionN(void);

template <class OBJ, class LIST>
void InsertT(OBJ *obj, LIST *list[2], OBJ *prev, OBJ *next, ldat *objcount) {
  if (obj->Prev || obj->Next) {
    return;
  }
  if (prev) {
    next = (OBJ *)prev->Next;
  } else if (next) {
    prev = (OBJ *)next->Prev;
  }
  if ((obj->Prev = prev) != NULL) {
    prev->Next = obj;
  } else {
    list[0] = obj;
  }
  if ((obj->Next = next) != NULL) {
    next->Prev = obj;
  } else {
    list[1] = obj;
  }
  if (objcount) {
    ++(*objcount);
  }
}

template <class OBJ, class LIST> //
void RemoveT(OBJ *obj, LIST *list[2], ldat *objcount) {
  if (obj->Prev) {
    obj->Prev->Next = obj->Next;
  } else if (list[0] == obj) {
    list[0] = (LIST *)obj->Next;
  }
  if (obj->Next) {
    obj->Next->Prev = obj->Prev;
  } else if (list[1] == obj) {
    list[1] = (LIST *)obj->Prev;
  }
  obj->Prev = obj->Next = NULL;
  if (objcount) {
    --(*objcount);
  }
}

#define Do(command, objtype) (S##objtype::command)

#define New(objtype) S##objtype::Create

#define SKIP_FIRST_ARG(arg, ...) (__VA_ARGS__)
#define Act(command, obj) (obj)->command SKIP_FIRST_ARG

#define DeleteList(First)                                                                          \
  do {                                                                                             \
    while (First) {                                                                                \
      First->Delete();                                                                             \
    }                                                                                              \
  } while (0)

#define InsertOnly(obj_name, obj, parent) (obj)->Insert((parent), NULL, NULL)

#define InsertFirst(obj_name, obj, parent) (obj)->Insert((parent), NULL, (parent)->First##obj_name)

#define InsertMiddle(obj_name, obj, parent, prev_obj, next_obj)                                    \
  (obj)->Insert((parent), (prev_obj), (next_obj))

#define InsertLast(obj_name, obj, parent) (obj)->Insert((parent), (parent)->Last##obj_name, NULL)

#define MoveFirst(obj_name, parent, obj) ((obj)->Remove(), InsertFirst(obj_name, obj, parent))

#define MoveLast(obj_name, parent, obj) ((obj)->Remove(), InsertLast(obj_name, obj, parent))

#define SendMsg(msgport, whichmsg) (InsertLast(Msg, (whichmsg), (msgport)))

#define Info4Menu(menu, flags, len, text, coltext) (menu)->SetInfo(flags, len, text, coltext)

#define Win4Menu(menu) Swindow::Create4Menu(menu)

#define Row4Menu(w, code, flags, len, name)                                                        \
  ((Trow)Smenuitem::Create4Menu((Tobj)(w), (Twindow)0, (code), (flags), (len), (name)))

#define Item4Menu(menu, w, flags, len, name)                                                       \
  Smenuitem::Create4Menu((Tobj)(menu), (w), (udat)0, (flags), (len), (name))

#define Item4MenuCommon(Menu) Smenuitem::Create4MenuCommon(Menu)

void *OverrideMth(void **where, void *OldMethod, void *NewMethod);

#define OverrideMethod(objtype, command, ExpectedMethod, NewMethod)                                \
  OverrideMth((void **)&(Fn_T##objtype->command), (void *)ExpectedMethod, (void *)NewMethod)

#endif /* TWIN_METHODS_H */
