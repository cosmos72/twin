/*
 *  sockparanoia.c  --  check libTw functions prototypes against twin
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"


#include "builtin.h"
#include "common.h"
#include "data.h"
#include "dl.h"
#include "draw.h"
#include "extensions.h"
#include "hotkey.h"
#include "hw.h"
#include "hw_multi.h"
#include "hw_private.h"
#include "main.h"
#include "md5.h"
#include "methods.h"
#include "pty.h"
#include "rctypes.h"
#include "rcproto.h"
#include "rcparse_tab.h"
#include "wm.h"
#include "rcrun.h"
#include "remote.h"
#include "resize.h"
#include "scroller.h"
#include "socket.h"
#include "term.h"
#include "tty.h"
#include "util.h"
#include "version.h"

#include "libTw.h"

#define Act0(funct,name) (funct##name)
#define Act1(funct,name) Do(funct,name)
#define Act2(funct,name) Do(funct,name)

#define A(n)

#define DECLv(n,arg)	void
#define DECL_(n,arg)	arg A(n)
#define DECLx(n,arg)	arg *A(n)
#define DECLV(n,arg)	CONST arg *A(n)
#define DECLW(n,arg)	CONST arg *A(n)
#define D(n,arg,f)	DECL##f(n,arg)
#define D_0(arg,f)	DECL##f(0,arg)

#define NAME(funct,name) (* paranoia_##funct##name)


#define V(len)		V
#define iV(len)		len
#define W(len)		W
#define iW(len)		len


#define PROTO0(ret0,f0, funct,name,fn) \
D_0(ret0,f0) NAME(funct, name) (void) = Act##fn(funct,name);

#define PROTO1(ret0,f0, funct,name,fn, arg1,f1) \
D_0(ret0,f0) NAME(funct, name) ( \
  D(1,arg1,f1) \
) = Act##fn(funct,name);

#define PROTO2(ret0,f0, funct,name,fn, arg1,f1, arg2,f2) \
D_0(ret0,f0) NAME(funct, name) ( \
  D(1,arg1,f1), D(2,arg2,f2) \
) = Act##fn(funct,name);

#define PROTO2FindFunction PROTO2

#define PROTO3(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3) \
D_0(ret0,f0) NAME(funct, name) ( \
  D(1,arg1,f1), D(2,arg2,f2), D(3,arg3,f3) \
) = Act##fn(funct,name);

#define PROTO4(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4) \
D_0(ret0,f0) NAME(funct, name) ( \
  D(1,arg1,f1), D(2,arg2,f2), D(3,arg3,f3), D(4,arg4,f4) \
) = Act##fn(funct,name);

#define PROTO5(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5) \
D_0(ret0,f0) NAME(funct, name) ( \
  D(1,arg1,f1), D(2,arg2,f2), D(3,arg3,f3), D(4,arg4,f4), D(5,arg5,f5) \
) = Act##fn(funct,name);

#define PROTO5CreateMsgPort PROTO5

#define PROTO6(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6) \

#define PROTO7(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7) \

#define PROTO8(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7, arg8,f8) \

#define PROTO11(ret0,f0, funct,name,fn,arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
	   arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11) \

#define PROTO19(ret0,f0, funct,name,fn,   arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
	   arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
	   arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18, arg19,f19) \

#include "sockproto.h"
