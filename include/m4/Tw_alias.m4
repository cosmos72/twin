




dnl This file is `m4/Twalias.m4' : m4 macros to autogenerate alias`'TARGET`'_m4.h
dnl Tell the user about this.
/* This file was automatically generated with `m4' from `m4/Twalias.m4', do not edit! */

/*
 *  alias`'TARGET`'_m4.h  --  handy aliases for libTw function prototypes
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

divert(-1)

define(`Twlist')

define(`Twappend',  `define(`Twlist', Twlist() el($1))')

Twappend(Obj)
define(`methodsObj', `
    field(Delete)`'dnl
')

Twappend(Row)
define(`methodsRow', `extends(Obj, $2)`'dnl
    field(Lower)`'dnl
    field(Raise)`'dnl
    field(CirculateChildren)`'dnl
    alias(Circulate$2`'s, CirculateChildren)`'dnl
    field(RestackChildren)`'dnl
    alias(Restack$2`'s, RestackChildren)`'dnl
    alias($2`'4Menu, Create4Menu)`'dnl
')

Twappend(MenuItem)
define(`methodsMenuItem', `extends(Row, $2)`'dnl
    alias(Item4Menu, Create4Menu)`'dnl
    alias(Item4MenuCommon, Create4MenuCommon)`'dnl
')

Twappend(Menu)
define(`methodsMenu', `extends(Obj, $2)`'dnl
    alias(Info4$2, SetInfo)`'dnl
')

Twappend(MsgPort)
define(`methodsMsgPort', `extends(Obj, $2)`'dnl
')

Twappend(Widget)
define(`methodsWidget', `extends(Obj, $2)`'dnl
    field(CirculateChildren)`'dnl
    alias(Circulate$2`'s, CirculateChildren)`'dnl
    field(Draw)`'dnl
    field(Draw2)`'dnl
    field(DrawText)`'dnl
    field(DrawHWFont)`'dnl
    field(DrawHWAttr)`'dnl
    field(Owner)`'dnl
    field(O_Prev)`'dnl
    field(O_Next)`'dnl
    field(Lower)`'dnl
    field(Raise)`'dnl
    field(Map)`'dnl
    field(RecursiveDelete)`'dnl
    field(Resize)`'dnl
    field(RestackChildren)`'dnl
    alias(Restack$2`'s, RestackChildren)`'dnl
    field(Scroll)`'dnl
    field(SetXY)`'dnl
    field(UnMap)`'dnl
')

Twappend(Gadget)
define(`methodsGadget', `extends(Widget, $2)`'dnl
    field(Group)`'dnl
    field(G_Prev)`'dnl
    field(G_Next)`'dnl
')

Twappend(Window)
define(`methodsWindow', `extends(Widget, $2)`'dnl
    alias(Win4Menu, Create4Menu)`'dnl
')

Twappend(Screen)
define(`methodsScreen', `extends(Widget, $2)`'dnl
')

undefine(`Twappend')

define(`field')
define(`alias')


divert

