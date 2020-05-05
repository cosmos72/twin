/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_METHODS_H
#define _TWIN_METHODS_H

#include "obj/fn.h"
#include "obj/window.h" // tpos

byte FindInfo(menu Menu, dat i);
window FakeOpenTerm(const char *arg0, const char *const *argv);
widget FakeKbdFocus(widget W);
byte FakeWriteAscii(window Window, uldat Len, const char *Ascii);
byte FakeWriteString(window Window, uldat Len, const char *String);
byte FakeWriteTRune(window Window, uldat Len, const trune *TRune);
byte FakeWriteTCell(window Window, dat x, dat y, uldat Len, const tcell *Attr);
tpos FakeFindBorderWindow(window W, dat u, dat v, byte Border, tcell *PtrAttr);

#define Do(Command, obj_type) (Fn_##obj_type->Command)

#define New(obj_type) s_##obj_type::Create

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

#define SendMsg(MsgPort, WhichMsg) (InsertLast(Msg, WhichMsg, (MsgPort)))

#define Info4Menu(Menu, Flags, Len, Text, ColText) (Menu)->SetInfo(Flags, Len, Text, ColText)

#define Win4Menu(Menu) Do(Create4Menu, window)(Menu)

#define Row4Menu(Window, Code, Flags, Len, Name)                                                   \
  (row) Do(Create4Menu, menuitem)((obj)(Window), (window)0, (Code), (Flags), (Len), (Name))

#define Item4Menu(Menu, Window, Flags, Len, Name)                                                  \
  Do(Create4Menu, menuitem)((obj)(Menu), (Window), (udat)0, (Flags), (Len), (Name))

#define Item4MenuCommon(Menu) Do(Create4MenuCommon, menuitem)(Menu)

void *OverrideMth(void **where, void *OldMethod, void *NewMethod);

#define OverrideMethod(obj_type, Command, ExpectedMethod, NewMethod)                               \
  OverrideMth((void **)&(Fn_##obj_type->Command), (void *)ExpectedMethod, (void *)NewMethod)

#endif /* _TWIN_METHODS_H */
