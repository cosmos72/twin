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

#ifdef _TW_H
#include <Tw/compiler.h>
#else
#include <compiler.h>
#endif

#define PushV(s, len, vec) (Tw(CopyMem)(vec, s, len), (s) += (len))
#define PopV(s, len, vec) (Tw(CopyMem)(s, vec, len), (s) += (len))
#define PopAddr(s, type, len, ptr) ((ptr) = (len) ? (type *)(s) : (type *)0, (s) += (len))

#define Push(s, type, val)                                                                         \
  do {                                                                                             \
    type __tmp = (val);                                                                            \
    PushV(s, sizeof(type), &__tmp);                                                                \
  } while (0)
#define Pop(s, type, lval) PopV(s, sizeof(type), &(lval))

#endif /* _TWIN_UNALIGNED_H */
