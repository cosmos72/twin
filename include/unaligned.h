#ifndef _TWIN_UNALIGNED_H
#define _TWIN_UNALIGNED_H

#include "Tw/prefix.h"

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
# define Push(s,type,val)	do {type __tmp = (val); PushV(s,sizeof(type),&__tmp); } while(0)
# define Pop(s,type,lval)	PopV(s,sizeof(type),&(lval))

#endif /* __i386__ */

#endif /* _TWIN_UNALIGNED_H */
