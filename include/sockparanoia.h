/*
 *  sockparanoia.h  --  check libTw functions prototypes from sockproto.h
 *			against twin.h or against libTw.h include files.
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#if defined(_LIB_TW_H)
# define CONST		TW_CONST
# define PREFIX_VOID	tdisplay
# define PREFIX		tdisplay,
#elif defined(_TWIN_H)
# define PREFIX_VOID	void
# define PREFIX
#else
# error you must #include at least one of "twin.h" or "libTw.h"
#endif

#define Act(fn,funct,name)

#define A(n)

#define DECLv(n,arg)	void
#define DECL_(n,arg)	arg A(n)
#define DECLx(n,arg)	t##arg A(n)
#define DECLV(n,arg)	CONST arg *A(n)
#define DECLW(n,arg)	CONST arg *A(n)
#define D(n,arg,f)	DECL##f(n,arg)
#define D_0(arg,f)	DECL##f(0,arg)

#define NAME(funct,name) Tw_##funct##name

#define V(len)		V
#define iV(len)		len
#define W(len)		W
#define iW(len)		len


#define PROTO0(ret0,f0, funct,name,fn) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX_VOID ) \
Act(fn,funct,name);

#define PROTO0SyncSocket PROTO0

#define PROTO1(ret0,f0, funct,name,fn, arg1,f1) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX \
  D(1,arg1,f1) \
) Act(fn,funct,name);

#define PROTO2(ret0,f0, funct,name,fn, arg1,f1, arg2,f2) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX \
  D(1,arg1,f1), D(2,arg2,f2) \
) Act(fn,funct,name);

#define PROTO2FindFunction PROTO2

#define PROTO3(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX \
  D(1,arg1,f1), D(2,arg2,f2), D(3,arg3,f3) \
) Act(fn,funct,name);

#define PROTO4(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX \
  D(1,arg1,f1), D(2,arg2,f2), D(3,arg3,f3), D(4,arg4,f4) \
) Act(fn,funct,name);

#define PROTO5(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX \
  D(1,arg1,f1), D(2,arg2,f2), D(3,arg3,f3), D(4,arg4,f4), D(5,arg5,f5) \
) Act(fn,funct,name);

#define PROTO5CreateMsgPort PROTO5

#define PROTO6(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX \
  D(1,arg1,f1), D(2,arg2,f2), D(3,arg3,f3), D(4,arg4,f4), D(5,arg5,f5), \
  D(6,arg6,f6) \
) Act(fn,funct,name);

#define PROTO7(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX \
  D(1,arg1,f1), D(2,arg2,f2), D(3,arg3,f3), D(4,arg4,f4), D(5,arg5,f5), \
  D(6,arg6,f6), D(7,arg7,f7) \
) Act(fn,funct,name);

#define PROTO8(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7, arg8,f8) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX \
  D(1,arg1,f1), D(2,arg2,f2), D(3,arg3,f3), D(4,arg4,f4), D(5,arg5,f5), \
  D(6,arg6,f6), D(7,arg7,f7), D(8,arg8,f8) \
) Act(fn,funct,name);

#define PROTO11(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	   arg6,f6, arg7,f7, arg8,f8, arg9,f9, arg10,f10, arg11,f11) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX \
  D(1 ,arg1 ,f1 ), D(2 ,arg2 ,f2 ), D(3 ,arg3 ,f3 ), D(4 ,arg4 ,f4 ), D(5 ,arg5 ,f5 ), \
  D(6 ,arg6 ,f6 ), D(7 ,arg7 ,f7 ), D(8 ,arg8 ,f8 ), D(9 ,arg9 ,f9 ), D(10,arg10,f10), \
  D(11,arg11,f11) \
) Act(fn,funct,name);

#define PROTO19(ret0,f0, funct,name,fn,   arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	   arg6,f6, arg7,f7, arg8,f8, arg9,f9, arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
	   arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18, arg19,f19) \
D_0(ret0,f0) NAME(funct, name) ( PREFIX \
  D(1 ,arg1 ,f1 ), D(2 ,arg2 ,f2 ), D(3 ,arg3 ,f3 ), D(4 ,arg4 ,f4 ), D(5 ,arg5 ,f5 ), \
  D(6 ,arg6 ,f6 ), D(7 ,arg7 ,f7 ), D(8 ,arg8 ,f8 ), D(9 ,arg9 ,f9 ), D(10,arg10,f10), \
  D(11,arg11,f11), D(12,arg12,f12), D(13,arg13,f13), D(14,arg14,f14), D(15,arg15,f15), \
  D(16,arg16,f16), D(17,arg17,f17), D(18,arg18,f18), D(19,arg19,f19) \
) Act(fn,funct,name);

#include "sockproto.h"

#undef PREFIX_VOID
#undef PREFIX
#undef Act
#undef A
#undef DECLv
#undef DECL_
#undef DECLx
#undef DECLV
#undef DECLW
#undef D
#undef D_0
#undef NAME
#undef V
#undef W
#undef PROTO0
#undef PROTO0SyncSocket
#undef PROTO1
#undef PROTO2
#undef PROTO2FindFunction
#undef PROTO3
#undef PROTO4
#undef PROTO5
#undef PROTO6
#undef PROTO7
#undef PROTO8
#undef PROTO11
#undef PROTO19

