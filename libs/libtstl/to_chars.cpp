/*
 *  to_chars.cpp  --  functions to convert arbitrary types to string
 *
 *  Copyright (C) 2022 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "stl/to_chars.h"
#include "stl/chars.h"

#include <cassert>
#include <cstring> // memcpy()
#include <climits> // CHAR_BIT

/** convert a small number (<= 36) to a single printed digit */
static char to_char(unsigned val) {
  if (val < 10) {
    return '0' + val;
  } else {
    return 'a' + (val - 10);
  }
}

to_chars_result to_chars(Span<char> dst, long val, unsigned base) {
  if (val >= 0) {
    return to_chars(dst, (unsigned long)val, base);
  } else if (dst.size() < 2) {
    return to_chars_result(0, NOTABLES);
  }
  dst[0] = '-';
  to_chars_result result = to_chars(dst.span(1, dst.size()), (unsigned long)-val, base);
  return to_chars_result(result.written + 1, result.err);
}

to_chars_result to_chars(Span<char> dst, unsigned long val, unsigned base) {
  if (base < 2 || base > 36) {
    return to_chars_result(0, INVALERROR);
  }
  if (!dst) {
    return to_chars_result(0, NOTABLES);
  } else if (val == 0) {
    dst[0] = '0';
    return to_chars_result(1, SUCCESS);
  }
  char buf[sizeof(unsigned long) * CHAR_BIT];
  char *addr = buf + sizeof(buf);
  while (addr > buf && val != 0) {
    *--addr = to_char(val % base);
    val /= base;
  }
  const size_t src_n = buf + sizeof(buf) - addr;
  const size_t dst_n = dst.size();
  const size_t n = src_n <= dst_n ? src_n : dst_n;
  std::memcpy(dst.data(), addr, n);
  return to_chars_result(n, val == 0 && dst_n >= src_n ? SUCCESS : NOTABLES);
}

to_chars_result to_chars(Span<char> dst, View<char> src) {
  const size_t dst_n = dst.size();
  const size_t src_n = src.size();
  const size_t n = src_n <= dst_n ? src_n : dst_n;
  if (n) {
    std::memmove(dst.data(), src.data(), n);
  }
  return to_chars_result(n, dst_n >= src_n ? SUCCESS : NOTABLES);
}

////////////////////////////////////////////////////////////////////////////////

size_t to_chars_len(long val, unsigned base) {
  if (val >= 0) {
    return to_chars_len((unsigned long)val, base);
  } else {
    return 1 + to_chars_len((unsigned long)-val, base);
  }
}

size_t to_chars_len(unsigned long val, unsigned base) {
  if (base < 2 || base > 36) {
    return 0;
  } else if (val == 0) {
    return 1;
  }
  size_t len = 0;
  while (val != 0) {
    val /= base;
    len++;
  }
  return len;
}
