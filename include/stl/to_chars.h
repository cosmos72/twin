/* Copyright (C) 2022 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_TO_CHARS_H
#define TWIN_STL_TO_CHARS_H

#include "stl/span.h"
#include "stl/err.h"

struct to_chars_result {
  size_t written;
  errnum err;

  friend inline bool operator==(to_chars_result a, to_chars_result b) {
    return a.written == b.written && a.err == b.err;
  }

  friend inline bool operator!=(to_chars_result a, to_chars_result b) {
    return !(a == b);
  }
};

/** convert signed integer to string */
to_chars_result to_chars(Span<char> out, long val, unsigned base = 10);
inline to_chars_result to_chars(Span<char> out, int val, unsigned base = 10) {
  return to_chars(out, (long)val, base);
}
inline to_chars_result to_chars(Span<char> out, short val, unsigned base = 10) {
  return to_chars(out, (long)val, base);
}
inline to_chars_result to_chars(Span<char> out, signed char val, unsigned base = 10) {
  return to_chars(out, (long)val, base);
}

/** convert unsigned integer to string */
to_chars_result to_chars(Span<char> out, unsigned long val, unsigned base = 10);
inline to_chars_result to_chars(Span<char> out, unsigned int val, unsigned base = 10) {
  return to_chars(out, (unsigned long)val, base);
}
inline to_chars_result to_chars(Span<char> out, unsigned short val, unsigned base = 10) {
  return to_chars(out, (unsigned long)val, base);
}
inline to_chars_result to_chars(Span<char> out, unsigned char val, unsigned base = 10) {
  return to_chars(out, (unsigned long)val, base);
}

/** copy chars to string */
to_chars_result to_chars(Span<char> out, Chars val);

#endif /* TWIN_STL_TO_CHARS_H */
