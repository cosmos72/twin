/*
 *  sockproto.h  --  macroized prototypes for libTw functions.
 *                   used both on client and server side to implement
 *                   function calls <-> socket data stream
 *                   conversion.
 *                   You can also see this as a custom version of
 *                   `remote procedure calling'.
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TW_SOCKPROTO_H
#define _TW_SOCKPROTO_H

/*

 PROTO<arg #>(<rettype>,<retflag>, <action>,<object>,<self>, <arg1type>,<arg1flag>, ...)

 <arg #> : the number of arguments the function wants

 <flag> : v = void
	  _ = scalar (any integer)
	  x = pointer
	  V(len) = vector of <len> elements
		<len> can be an expression, and may access the other arguments
		of the function as A(n) where <n> is the progressive number
		of the argument: A(1) is the first arg, A(2) the second, ...
	  W(len) = vector of <len> elements, with double-checking/autodetect on len
		so that you can safely pass NULL instead of the vector.

 Function calls wait until server has processed the command and returned the result
 if their return value is not void.
 
 <action> : the function name (Create, Map, Delete, ...)
 <object> : the object it acts upon (Gadget, Window, ...)
 
 <self> : 0 if the server implementation does not need the Fn##object method pointer
	: 1 if the function in the server needs the Fn##object method pointer
	: 2 if the Fn##object method pointer is extracted from the first argument
	

*/

#define OK_MAGIC	((uldat)0x3E4B4F3Cul)

#define FIND_MAGIC	((uldat)0x646E6946ul)
#define MSG_MAGIC	((uldat)0x2167734dul)

PROTO2FindFunction(uldat,_, Find,Function,0, byte,_, byte,V(A(1)))

PROTO0(byte,_,  Sync,Socket,0)

PROTO0(byte,_, Can,Compress,0)
PROTO1(byte,_, Do,Compress,0, byte,_)

PROTO0(void,v, NeedResize,Display,0)

PROTO3(void,v, Attach,HW,0, uldat,_, byte,V(A(1)), byte,_)
PROTO2(byte,_, Detach,HW,0, uldat,_, byte,V(A(1)))

PROTO1(void,v,  Set,FontTranslation,0, byte,V(0x80))

PROTO19(gadget,x, Create,Gadget,1,
	window,x,hwcol,_,hwcol,_,hwcol,_,hwcol,_,
	udat,_,  udat,_, dat,_, dat,_, dat,_, dat,_,
	byte,W(A(9)*A(10)),  byte,W(A(9)*A(10)),  byte,W(A(9)*A(10)),  byte,W(A(9)*A(10)),
	hwcol,W(A(9)*A(10)), hwcol,W(A(9)*A(10)), hwcol,W(A(9)*A(10)), hwcol,W(A(9)*A(10)))

PROTO1(void,v,    Delete,Gadget,0, gadget,x)

PROTO11(gadget,x, CreateButton,Gadget,1, window,x, hwcol,_, hwcol,_, hwcol,_,
	udat,_, udat,_, dat,_, dat,_, dat,_, dat,_, byte,V(A(9)*A(10)))

PROTO5(void,v,   Create4Menu,Row,1, window,x, udat,_, byte,_, ldat,_, byte,V(A(4)))

PROTO11(window,x,     Create,Window,1, dat,_, byte,V(A(1)), hwcol,W(A(1)), menu,x,
	hwcol,_, uldat,_, uldat,_, byte,_, dat,_, dat,_, dat,_)
PROTO1(void,v,        Delete,Window,0, window,x)
PROTO1(window,x, Create4Menu,Window,1, menu,x)
PROTO2(void,v,           Map,Window,2, window,x, widget,x)
PROTO1(void,v,         UnMap,Window,2, window,x)
PROTO3(void,v,    WriteAscii,Window,2, window,x, ldat,_, byte,V(A(2)))
PROTO5(void,v,   WriteHWAttr,Window,2, window,x, dat,_, dat,_, ldat,_, hwattr,V(A(4)))
PROTO3(void,v,      WriteRow,Window,2, window,x, ldat,_, byte,V(A(2)))
PROTO3(void,v,        GotoXY,Window,2, window,x, ldat,_, ldat,_)
PROTO2(void,v,    SetColText,Window,2, window,x, hwcol,_)
PROTO11(void,v, SetColors,Window,2, window,x, udat,_, hwcol,_, hwcol,_,
	hwcol,_, hwcol,_, hwcol,_, hwcol,_, hwcol,_, hwcol,_, hwcol,_)
PROTO8(void,v,     Configure,Window,2, window,x, byte,_, dat,_, dat,_, dat,_, dat,_, dat,_, dat,_)
PROTO3(void,v,     Resize,Window,0, window,x, dat,_, dat,_)

PROTO3(widget,x,SearchWidget,Widget,2, widget,x, dat,_, dat,_)

PROTO5(menuitem,x, Create4Menu,MenuItem,1, menu,x, window,x, byte,_, dat,_, byte,V(A(4)))
PROTO1(uldat,_,    Create4MenuCommon,MenuItem,1, menu,x)
PROTO1(void,v,	        Delete,MenuItem,0, menuitem,x)

PROTO8(menu,x, Create,Menu,1, msgport,x, hwcol,_, hwcol,_, hwcol,_, hwcol,_, hwcol,_,
       hwcol,_, byte,_)
PROTO5(void,v, SetInfo,Menu,2, menu,x, byte,_, ldat,_, byte,V(A(3)), hwcol,W(A(3)))
PROTO1(void,v,  Delete,Menu,0, menu,x)

PROTO5(msgport,x,Create,MsgPort,0, byte,_, byte,V(A(1)), time_t,_, frac_t,_, byte,_)
PROTO1(void,v,   Delete,MsgPort,0, msgport,x)

PROTO4(void,v,  BgImage,Screen,2, screen,x, dat,_, dat,_, hwattr,V(A(2)*A(3)))

PROTO0(screen  ,x, First,Screen,  0)
PROTO1(widget  ,x, First,Widget,  0, widget, x)
PROTO0(msgport ,x, First,MsgPort, 0)
PROTO1(menu    ,x, First,Menu,    0, msgport,x)
PROTO1(menuitem,x, First,MenuItem,0, menu,   x)

PROTO1(obj,x,   Prev,Obj,0, obj,x)
PROTO1(obj,x,   Next,Obj,0, obj,x)
PROTO1(obj,x, Parent,Obj,0, obj,x)

PROTO0(dat,_, GetDisplay,Width,0)
PROTO0(dat,_, GetDisplay,Height,0)

PROTO3(byte,_, SendTo,MsgPort,0, msgport,x, udat,_, byte,V(A(2)))
PROTO3(void,v, BlindSendTo,MsgPort,0, msgport,x, udat,_, byte,V(A(2)))

PROTO0(obj, x, GetOwner,Selection,0)
PROTO3(void,v, SetOwner,Selection,0, msgport,x, time_t,_, frac_t,_)
PROTO2(void,v,  Request,Selection,0, obj,x, uldat,_)
PROTO6(void,v,   Notify,Selection,0, obj,x, uldat,_, uldat,_, byte,V(TW_MAX_MIMELEN), uldat,_, byte,V(A(5)))

#endif /* _TW_SOCKPROTO_H */

