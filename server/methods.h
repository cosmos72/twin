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
byte FakeWriteCharset(Twindow window, uldat Len, const char *charset_bytes);
byte FakeWriteUtf8(Twindow window, uldat Len, const char *utf8_bytes);
byte FakeWriteTRune(Twindow window, uldat Len, const trune *runes);
byte FakeWriteTCell(Twindow window, dat x, dat y, uldat Len, const tcell *cells);
tpos FakeFindBorderWindow(Twindow window, dat u, dat v, byte Border, tcell *PtrAttr);

void SetFillWidget(Twidget w, tcell Fill);
void IncMouseMotionN(void);
void DecMouseMotionN(void);

void RemoveGeneric(TobjEntry obj, TobjList parent, ldat *ObjCount);

#define Do(Command, objtype) (Fn_T##objtype->Command)

#define New(objtype) S##objtype::Create

#define SKIP_FIRST_ARG(arg, ...) (__VA_ARGS__)
#define Act(Command, Obj) (Obj)->Command SKIP_FIRST_ARG

#define DeleteList(First)                                                                          \
  do {                                                                                             \
    while (First) {                                                                                \
      First->Delete();                                                                             \
    }                                                                                              \
  } while (0)

#define InsertOnly(ObjName, Obj, Parent) (Obj)->Insert((Parent), NULL, NULL)

#define InsertFirst(ObjName, Obj, Parent) (Obj)->Insert((Parent), NULL, (Parent)->First##ObjName)

#define InsertMiddle(ObjName, Obj, Parent, ObjPrev, ObjNext)                                       \
  (Obj)->Insert((Parent), (ObjPrev), (ObjNext))

#define InsertLast(ObjName, Obj, Parent) (Obj)->Insert((Parent), (Parent)->Last##ObjName, NULL)

#define MoveFirst(ObjName, Parent, Obj) ((Obj)->Remove(), InsertFirst(ObjName, Obj, Parent))

#define MoveLast(ObjName, Parent, Obj) ((Obj)->Remove(), InsertLast(ObjName, Obj, Parent))

#define SendMsg(msgport, whichmsg) (InsertLast(Msg, (whichmsg), (msgport)))

#define Info4Menu(menu, flags, len, text, coltext) (menu)->SetInfo(flags, len, text, coltext)

#define Win4Menu(Menu) Do(Create4Menu, window)(Menu)

#define Row4Menu(w, code, flags, len, name)                                                        \
  (Trow) Do(Create4Menu, menuitem)((Tobj)(w), (Twindow)0, (code), (flags), (len), (name))

#define Item4Menu(menu, w, flags, len, name)                                                       \
  Do(Create4Menu, menuitem)((Tobj)(menu), (w), (udat)0, (flags), (len), (name))

#define Item4MenuCommon(Menu) Do(Create4MenuCommon, menuitem)(Menu)

void *OverrideMth(void **where, void *OldMethod, void *NewMethod);

#define OverrideMethod(objtype, Command, ExpectedMethod, NewMethod)                                \
  OverrideMth((void **)&(Fn_T##objtype->Command), (void *)ExpectedMethod, (void *)NewMethod)

#endif /* TWIN_METHODS_H */
