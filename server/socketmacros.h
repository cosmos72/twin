#define __CAT(a,b) a##b
#define CAT(a,b) __CAT(a,b)

#define ORDER(arg)	(arg##_magic >> magic_shift)

#define ID(s,o)		(s=o, s ? &(s->Id) : &noId)

#define A(n)		a##n
#define AL(n)		len##n

#define FN0(name)
#define FN1(name)	Fn##name,
#define FN2(name)

#define Act0(funct,name) ACT0(funct,name)
#define Act1(funct,name) Do(funct,name)
#define Act2(funct,name) Act(funct,A(1))

#define DECLv(n,arg)
#define DECL_(n,arg)	arg A(n);
#define DECLx(n,arg)	uldat AL(n); arg A(n);
#define DECLV(n,arg)	uldat AL(n); CONST arg *A(n);
#define DECLW(n,arg)	uldat AL(n); CONST arg *A(n);
#define D(n,arg,f)	DECL##f(n,arg)

#define DECL0v(n,arg)
#define DECL0_(n,arg)	arg A(n);
#define DECL0x(n,arg)	arg A(n);
#define DECL0V(n,arg)	CONST arg *A(n);
#define DECL0W(n,arg)	CONST arg *A(n);
#define D_0(arg,f)	uldat fail = 1; DECL0##f(0,arg)

#define RETv(ret,call)	call;
#define RET_(ret,call)	REPLY(OK_MAGIC, ret, (A(0) = call, &A(0)));
#define RETx(ret,call)	REPLY(OK_MAGIC, uldat, ID(A(0), call));

#define FAILv
#define FAIL_	REPLY(fail, uldat, &fail);
#define FAILx	FAIL_

#define RET0(ret,f,call) if (s == end) { RET##f(ret,call) return; } else fail = -1;
#define RET1(ret,f,call) if (s == end) { RET##f(ret,call) return; } else fail = -1;
#define RET2(ret,f,call) if (s == end && A(1)) { RET##f(ret,call) return; } else \
				if (A(1)) fail = 1; \
				else fail = -1;

#define NAME(funct,name)	static void CAT(NAME_PREFIX,funct##name)(void)

#define V(len)		V
#define iV(len)		len
#define W(len)		W
#define iW(len)		len

#define Left(size)	(s + (size) <= end)

#define PARSE_(n,arg,len,pass) if (Left(SIZEOF(arg))) { \
				    fail++; \
				    POP(s,arg,A(n)); \
				    pass \
				}
#define PARSEx(n,arg,len,pass) if (Left(SIZEOF(uldat))) { \
				    fail++; \
				    POP(s,uldat,AL(n)); \
				    A(n) = (arg)Id2Obj(ORDER(arg), AL(n)); \
				    pass \
				}
#define PARSEV(n,arg,len,pass) if (Left(AL(n) = (len) * SIZEOF(arg))) { \
				    fail++; \
				    POPADDR(s, arg, AL(n), A(n)); \
				    pass \
				}

#define PARSEW(n,arg,len,pass) if (Left(SIZEOF(uldat)) && (POP(s,uldat,AL(n)), Left(AL(n)))) { \
				    fail++; \
				    POPADDR(s, arg, AL(n), A(n)); \
				    if (AL(n) != (len) * SIZEOF(arg)) \
					A(n) = NULL; \
				    pass \
				}

#define P(n,arg,f,len,pass)	PARSE##f(n,arg,len,pass)



#define PROTO0(ret0,f0, funct,name,fn) \
NAME(funct, name) \
{ D_0(ret0,f0) \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name))) \
  FAIL##f0 \
}

#define PROTO0SyncSocket PROTO0

#define PROTO1(ret0,f0, funct,name,fn, arg1,f1) \
NAME(funct, name) \
{ D_0(ret0,f0) \
  D(1,arg1,f1) \
  P(1,arg1,f1,i##f1, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1)))) \
  FAIL##f0 \
}

#define PROTO2(ret0,f0, funct,name,fn, arg1,f1, arg2,f2) \
NAME(funct, name) \
{ D_0(ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1), A(2))))) \
  FAIL##f0 \
}

#define PROTO2FindFunction PROTO2

#define PROTO3(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3) \
NAME(funct, name) \
{ D_0(ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1), A(2), A(3)))))) \
  FAIL##f0 \
}

#define PROTO4(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4) \
NAME(funct, name) \
{ D_0(ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1), A(2), A(3), A(4))))))) \
  FAIL##f0 \
}

#define PROTO5(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5) \
NAME(funct, name) \
{ D_0(ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  D(5,arg5,f5) \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, \
  P(5,arg5,f5,i##f5, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1), A(2), A(3), A(4), A(5)))))))) \
  FAIL##f0 \
}

#define PROTO6(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6) \
NAME(funct, name) \
{ D_0(ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  D(5,arg5,f5) D(6,arg6,f6) \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, \
  P(5,arg5,f5,i##f5, P(6,arg6,f6,i##f6, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1), A(2), A(3), A(4), A(5), A(6))))))))) \
  FAIL##f0 \
}

#define PROTO7(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7) \
NAME(funct, name) \
{ D_0(ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  D(5,arg5,f5) D(6,arg6,f6) D(7,arg7,f7) \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, \
  P(5,arg5,f5,i##f5, P(6,arg6,f6,i##f6, P(7,arg7,f7,i##f7, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1), A(2), A(3), A(4), A(5), A(6), A(7)))))))))) \
  FAIL##f0 \
}

#define PROTO8(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7, arg8,f8) \
NAME(funct, name) \
{ D_0(ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  D(5,arg5,f5) D(6,arg6,f6) D(7,arg7,f7) D(8,arg8,f8) \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, \
  P(5,arg5,f5,i##f5, P(6,arg6,f6,i##f6, P(7,arg7,f7,i##f7, P(8,arg8,f8,i##f8, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8))))))))))) \
  FAIL##f0 \
}

#define PROTO11(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	   arg6,f6, arg7,f7, arg8,f8, arg9,f9, arg10,f10, arg11,f11) \
NAME(funct, name) \
{ D_0( ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4, f4) \
  D(5,arg5,f5) D(6,arg6,f6) D(7,arg7,f7) D(8,arg8,f8) \
  D(9,arg9, f9) D(10,arg10,f10) D(11,arg11,f11) \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, \
  P(5,arg5,f5,i##f5, P(6,arg6,f6,i##f6, P(7,arg7,f7,i##f7, P(8,arg8,f8,i##f8, \
  P(9,arg9,f9,i##f9, P(10,arg10,f10,i##f10, P(11,arg11,f11,i##f11, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8), A(9), A(10), \
      A(11)))))))))))))) \
  FAIL##f0 \
}

#define PROTO19(ret0,f0, funct,name,fn,   arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	   arg6,f6, arg7,f7, arg8,f8, arg9,f9, arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
	   arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18, arg19,f19) \
NAME(funct, name) \
{ D_0( ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4, f4) \
  D(5,arg5,f5) D(6,arg6,f6) D(7,arg7,f7) D(8,arg8,f8) \
  D(9,arg9, f9) D(10,arg10,f10) D(11,arg11,f11) D(12,arg12,f12) \
  D(13,arg13,f13) D(14,arg14,f14) D(15,arg15,f15) D(16,arg16,f16) \
  D(17,arg17,f17) D(18,arg18,f18) D(19,arg19,f19) \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, \
  P(5,arg5,f5,i##f5, P(6,arg6,f6,i##f6, P(7,arg7,f7,i##f7, P(8,arg8,f8,i##f8, \
  P(9,arg9,f9,i##f9, P(10,arg10,f10,i##f10, P(11,arg11,f11,i##f11, P(12,arg12,f12,i##f12, \
  P(13,arg13,f13,i##f13, P(14,arg14,f14,i##f14, P(15,arg15,f15,i##f15, P(16,arg16,f16,i##f16, \
  P(17,arg17,f17,i##f17, P(18,arg18,f18,i##f18, P(19,arg19,f19,i##f19, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8), A(9), A(10), \
      A(11), A(12), A(13), A(14), A(15), A(16), A(17), A(18), A(19) \
     ))))))))))))))))))))) \
  FAIL##f0 \
}



#define FN_DECL	\
    sock_fn CAT(NAME_PREFIX,F) [] =
    
#define FN_ENDDECL \
    /* handy aliases (also for compatibility) */ \
    { 0, "DeleteWidget", CAT(NAME_PREFIX,DeleteObj) }, \
    { 0, "DeleteGadget", CAT(NAME_PREFIX,DeleteObj) }, \
    { 0, "MapGadget",    CAT(NAME_PREFIX,MapWidget) }, \
    { 0, "UnMapGadget",  CAT(NAME_PREFIX,UnMapWidget) }, \
    { 0, "RecursiveDeleteGadget", CAT(NAME_PREFIX,RecursiveDeleteWidget) }, \
    { 0, "DeleteWindow", CAT(NAME_PREFIX,DeleteObj) }, \
    { 0, "MapWindow",    CAT(NAME_PREFIX,MapWidget) }, \
    { 0, "UnMapWindow",  CAT(NAME_PREFIX,UnMapWidget) }, \
    { 0, "RecursiveDeleteWindow", CAT(NAME_PREFIX,RecursiveDeleteWidget) }, \
    { 0, "DeleteMenuItem", CAT(NAME_PREFIX,DeleteObj) }, \
    { 0, "DeleteMenu",   CAT(NAME_PREFIX,DeleteObj) }, \
    { 0, "DeleteMsgPort", CAT(NAME_PREFIX,DeleteObj) }, \
    { 0, NULL, (void (*)(void))0 }

