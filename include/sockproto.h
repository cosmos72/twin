/*
 *  sockproto.h  --  macroized prototypes for libTw functions.
 *                   used both on client and server side to implement
 *                   function calls <-> socket data stream
 *                   conversion.
 *                   You can also see this as a custom version of
 *                   `remote procedure calling'.
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
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

 PROTO<arg #>(<rettype>,<retflag>, <action>,<object>,<self>,<slot>, <arg1type>,<arg1flag>, ...)

 <arg #> : the number of arguments the function wants

 <flag> : v = void
	  _ = scalar (any integer)
	  x = pointer
	  V(len) = vector of <len> elements
		<len> can be an expression, and may access the other arguments
		of the function as A(n) where <n> is the progressive number
		of the argument: A(1) is the first arg, A(2) the second, ...

 Function calls wait until server has processed the command if and only if
 ther return value is not void.
 
 <action> : the function name (Create, Map, Delete, ...)
 <object> : the object it acts upon (Gadget, Window, ...)
 
 <self> : 1 if the function implementation in the server needs the Fn##object method pointer
	: 2 if the Fn##object method pointer is extracted from the first argument
	: 0 otherwise

*/

#define OK_MAGIC	((uldat)0x3E4B4F3Cul)

#define FIND_MAGIC	((uldat)0x646E6946ul)
#define MSG_MAGIC	((uldat)0x2167734dul)

PROTO2FindFunction(uldat,_, Find,Function,0, byte,_, byte,V(A(1)))

PROTO0Abs(byte,_,  Sync,Socket,0)

PROTO0Abs(byte,_, Can,Compress,0)
PROTO1Abs(byte,_, Do,Compress,0, byte,_)

PROTO0Abs(void,v, NeedResize,Display,0)

PROTO3Abs(void,v, Attach,HW,0, byte,_, byte,V(A(1)), byte,_)
PROTO0Abs(void,v, Detach,HW,0)

PROTO20(gadget,x, Create,Gadget,1,
	window,x,hwcol,_,hwcol,_,hwcol,_,hwcol,_,
	udat,_,   udat,_, udat,_, udat,_, udat,_, udat,_, byte,_,
	byte,V(A(9)*A(10)),  byte,V(A(9)*A(10)),  byte,V(A(9)*A(10)),  byte,V(A(9)*A(10)),
	hwcol,V(A(9)*A(10)), hwcol,V(A(9)*A(10)), hwcol,V(A(9)*A(10)), hwcol,V(A(9)*A(10)))

PROTO2(void,v,      Copy,Gadget,2, gadget,x, gadget,x)
PROTO1(void,v,    Delete,Gadget,2, gadget,x)

PROTO5(void,v,   Create4Menu,Row,1, window,x, udat,_, byte,_, uldat,_, byte,V(A(4)))

PROTO11(window,x,     Create,Window,1, udat,_, byte,V(A(1)), hwcol,V(A(1)), menu,x,
	hwcol,_, uldat,_, uldat,_, byte,_, udat,_, udat,_, udat,_)
PROTO1(void,v,        Delete,Window,2, window,x)
PROTO1(window,x, Create4Menu,Window,1, menu,x)
PROTO2(void,v,           Map,Window,2, window,x, screen,x)
PROTO1(void,v,         UnMap,Window,2, window,x)
PROTO3(void,v,    WriteAscii,Window,2, window,x, uldat,_, byte,V(A(2)))
PROTO5(void,v,   WriteHWAttr,Window,2, window,x, udat,_, udat,_, uldat,_, hwattr,V(A(4)))
PROTO3(void,v,      WriteRow,Window,2, window,x, uldat,_, byte,V(A(2)))
PROTO3(void,v,        GotoXY,Window,2, window,x, uldat,_, uldat,_)
PROTO2(void,v,    SetColText,Window,2, window,x, hwcol,_)
PROTO11(void,v, SetColors,Window,2, window,x, udat,_, hwcol,_, hwcol,_,
	hwcol,_, hwcol,_, hwcol,_, hwcol,_, hwcol,_, hwcol,_, hwcol,_)
PROTO8(void,v,     Configure,Window,2, window,x, byte,_, dat,_, udat,_, udat,_, udat,_, udat,_, udat,_)

PROTO3(gadget,x,SearchGadget,Window,2, window,x, dat,_, dat,_)

PROTO5(menuitem,x, Create4Menu,MenuItem,1, menu,x, window,x, byte,_, udat,_, byte,V(A(4)))
PROTO1(menuitem,x, Create4MenuCommon,MenuItem,1, menu,x)
PROTO1(void,v,	        Delete,MenuItem,2, menuitem,x)

PROTO8(menu,x, Create,Menu,1, msgport,x, hwcol,_, hwcol,_, hwcol,_, hwcol,_, hwcol,_,
       hwcol,_, byte,_)
PROTO5(void,v, SetInfo,Menu,2, menu,x, byte,_, uldat,_, byte,V(A(3)), hwcol,V(A(3)))
PROTO1(void,v,  Delete,Menu,2, menu,x)

PROTO5CreateMsgPort(msgport,x,Create,MsgPort,1, byte,_, byte,V(A(1)), time_t,_, frac_t,_, byte,_)
PROTO1(void,v,   Delete,MsgPort,2, msgport,x)

PROTO4(void,v,  BgImage,Screen,2, screen,x, udat,_, udat,_, hwattr,V(A(2)*A(3)))

PROTO0Abs(screen  ,x, First,Screen,  0)
PROTO1Abs(window  ,x, First,Window,  0, screen, x)
PROTO1Abs(gadget  ,x, First,Gadget,  0, window, x)
PROTO0Abs(msgport ,x, First,MsgPort, 0)
PROTO1Abs(menu    ,x, First,Menu,    0, msgport,x)
PROTO1Abs(menuitem,x, First,MenuItem,0, menu,   x)

PROTO1Abs(obj,x,   Prev,Obj,0, obj,x)
PROTO1Abs(obj,x,   Next,Obj,0, obj,x)
PROTO1Abs(obj,x, Parent,Obj,0, obj,x)

#endif /* _TW_SOCKPROTO_H */

