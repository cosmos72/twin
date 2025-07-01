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

#define Do(command, objtype) (S##objtype::command)

#define SKIP_FIRST_ARG(arg, ...) (__VA_ARGS__)
#define Act(command, obj) (obj)->command SKIP_FIRST_ARG

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
