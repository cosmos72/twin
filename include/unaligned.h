#ifndef _TW_UNALIGNED_H
#define _TW_UNALIGNED_H

#ifdef _TW_H
# define Tw(arg)	Tw_##arg
# define TW(arg)	TW_##arg
#else
# define Tw(arg)	arg
# define TW(arg)	arg
#endif

#define PushV(s,len,vec)	(Tw(CopyMem)(vec, s, len), (s) += (len))
#define PopV(s,len,lvec)	(Tw(CopyMem)(s, lvec, len), (s) += (len))
#define PopAddr(s,type,len,ptr)	((ptr) = (len) ? (type *)s : (type *)0, s += (len))


#ifdef __i386__

# define TW_CAN_UNALIGNED 1
# define Push(s,type,val)	(*(type *)(s) = (val), ++(type *)(s))
# define Pop(s,type,lval)	((lval) = *(type TW(CONST) *)s, ++(type TW(CONST) *)s)

#else /* !__i386__ */

# warning not ix86 arch, using memcpy
# define TW_CAN_UNALIGNED 0
# define Push(s,type,val)	do {type tmp = (val); PushV(s,sizeof(type),&tmp); } while(0)
# define Pop(s,type,lval)	PopV(s,sizeof(type),&(lval))

#endif /* __i386__ */

#endif /* _TW_UNALIGNED_H */
