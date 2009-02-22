/*  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
#ifndef _TWIN_UNALIGNED_H
#define _TWIN_UNALIGNED_H

#include <Tw/prefix.h>

#define PushV(s,len,vec)	(Tw(CopyMem)(vec, s, len), (s) += (len))
#define PopV(s,len,vec)		(Tw(CopyMem)(s, vec, len), (s) += (len))
#define PopAddr(s,type,len,ptr) ((ptr) = (len) ? (type *)(s) : (type *)0, (s) += (len))


#ifndef TW_CAN_UNALIGNED
# if defined(__i386__) || defined(__x86_64__)
#  define TW_CAN_UNALIGNED 1
# else /* !__i386__ */
#  warning not ix86 arch, using memcpy
#  define TW_CAN_UNALIGNED 0
# endif /* __i386__ */
#endif /* TW_CAN_UNALIGNED */



#if TW_CAN_UNALIGNED == 1

# define Push(s,type,val)	(*(type *)(s) = (val), (s) += sizeof(type))
# define Pop(s,type,lval)	((lval) = *(type TW(CONST) *)(s), (s) += sizeof(type))

#else /* TW_CAN_UNALIGNED == 0 */

# define Push(s,type,val)	do { type __tmp = (val); PushV(s,sizeof(type),&__tmp); } while(0)
# define Pop(s,type,lval)	PopV(s,sizeof(type),&(lval))

#endif /* TW_CAN_UNALIGNED == 1 */


#endif /* _TWIN_UNALIGNED_H */
