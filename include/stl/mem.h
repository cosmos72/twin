/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_MEM_H
#define TWIN_STL_MEM_H

#include "stl/alloc.h"

#include <cstring> // memcmp(), memcpy()

namespace mem {

template <class T> inline T *alloc(size_t n) NOTHROW {
  return reinterpret_cast<T *>(AllocMem(n * sizeof(T)));
}
template <class T> inline T *realloc(T *addr, size_t /*old_n*/, size_t new_n) NOTHROW {
  return reinterpret_cast<T *>(ReAllocMem(addr, new_n * sizeof(T)));
}
template <class T> inline void free(T *addr) NOTHROW {
  FreeMem(addr);
}

/* zero-initialize returned memory */
template <class T> inline T *alloc0(size_t n) NOTHROW {
  return reinterpret_cast<T *>(AllocMem0(n * sizeof(T)));
}

/* if new_n > old_n, zero-initialize additional memory */
template <class T> inline T *realloc0(T *addr, size_t old_n, size_t new_n) NOTHROW {
  return reinterpret_cast<T *>(ReAllocMem0(addr, old_n * sizeof(T), new_n * sizeof(T)));
}

// may throw if T constructor or assignment operator throws
template <class T> void rawswap(T &a, T &b) {
  const T temp(a);
  a = b;
  b = temp;
}

template <class T1, class T2>
inline bool equalvec(const T1 *left, size_t left_n, const T2 *right, size_t right_n) NOTHROW {
  typedef char sizeof_T1_equals_sizeof_T2[sizeof(T1) == sizeof(T2) ? sizeof(T2) : -1];

  const void *left_v = static_cast<const void *>(left);
  const void *right_v = static_cast<const void *>(right);

  return sizeof(T1) == sizeof(T2) && left_n == right_n &&
         (left_v == right_v || left_n == 0 ||
          !std::memcmp(left_v, right_v, left_n * sizeof(sizeof_T1_equals_sizeof_T2)));
}

template <class Vec1, class Vec2>
inline bool equalvec(const Vec1 &left, const Vec2 &right) NOTHROW {
  return equalvec(left.data(), left.size(), right.data(), right.size());
}

template <class T1, class T2>
void copyvec(const T1 *src, size_t src_n, T2 *dst, size_t dst_n) NOTHROW {
  typedef char sizeof_T1_equals_sizeof_T2[sizeof(T1) == sizeof(T2) ? sizeof(T2) : -1];
  size_t n = src_n < dst_n ? src_n : dst_n;
  if (n) {
    std::memmove(static_cast<void *>(dst), static_cast<const void *>(src),
                 n * sizeof(sizeof_T1_equals_sizeof_T2) / sizeof(char));
  }
}

template <class Vec1, class Vec2> inline void copyvec(const Vec1 &src, Vec2 &dst) NOTHROW {
  copyvec(src.data(), src.size(), dst.data(), dst.size());
}

} // namespace mem

#endif /* TWIN_STL_ALLOC_H */
