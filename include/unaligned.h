/*  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
#ifndef TWIN_UNALIGNED_H
#define TWIN_UNALIGNED_H

#include <string.h> // memcpy()

#define PopV(s, len, vec) (memcpy(vec, s, len), (s) += (len))
#define PopAddr(s, type, len, ptr) ((ptr) = (len) ? (type *)(s) : (type *)0, (s) += (len))
#define PushV(s, len, vec) (memcpy(s, vec, len), (s) += (len))

#ifdef __cplusplus
template <class T> inline byte *Pop(const byte *src, T &val) {
  memcpy(&val, src, sizeof(T));
  return const_cast<byte *>(src) + sizeof(T);
}
template <class T> inline char *Pop(const char *src, T &val) {
  memcpy(&val, src, sizeof(T));
  return const_cast<char *>(src) + sizeof(T);
}
template <class T> inline byte *Push(byte *dst, T val) {
  memcpy(dst, &val, sizeof(T));
  return dst + sizeof(T);
}
#define Pop(s, type, lval) ((s) = Pop<type>(s, lval))
#define Push(s, type, val) ((s) = Push<type>(s, val))

#else /* not C++ */
#define Pop(s, type, lval) PopV(s, sizeof(type), &(lval))
#define Push(s, type, val)                                                                         \
  do {                                                                                             \
    type __tmp = (val);                                                                            \
    PushV(s, sizeof(type), &__tmp);                                                                \
  } while (0)
#endif

#endif /* TWIN_UNALIGNED_H */
