#ifndef _TW_UNALIGNED_H
#define _TW_UNALIGNED_H

#ifdef _LIB_TW_H
# define Tw(arg)	Tw##arg
#else
# define Tw(arg)	arg
#endif

#define PushV(s,len,vec)	(Tw(CopyMem)((vec), (s), (len)), (s) += (len))
#define PopV(s,len,lvec)	(Tw(CopyMem)((s), (lvec), (len)), (s) += (len))
#define PopAddr(s,type,len,ptr)	((ptr) = (len) ? (type *)s : (type *)0, s += (len))


#ifdef __i386__

# define Push(s,type,val)	(*(type *)(s) = (val), ++(type *)(s))
# define Pop(s,type,lval)	((lval) = *(type *)s, ++(type *)s)

#else /* !__i386__ */

# warning not ix86 arch, using memcpy
# define Push(s,type,val)	PushV(s,sizeof(type),&(val))
# define Pop(s,type,lval)	PopV(s,sizeof(type),&(lval))

#endif /* __i386__ */

#endif /* _TW_UNALIGNED_H */
