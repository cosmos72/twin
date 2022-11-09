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
#include "stl/chars.h"
#include "stl/err.h"

struct to_chars_result {
  size_t written;
  e_errnum err;

  explicit to_chars_result(size_t written_ = 0, e_errnum err_ = SUCCESS) NOTHROW //
      : written(written_),
        err(err_) {
  }

  friend inline bool operator==(to_chars_result a, to_chars_result b) NOTHROW {
    return a.written == b.written && a.err == b.err;
  }

  friend inline bool operator!=(to_chars_result a, to_chars_result b) NOTHROW {
    return !(a == b);
  }
};

/** convert signed integer to string */
to_chars_result to_chars(Span<char> out, long val, unsigned base = 10) NOTHROW;
inline to_chars_result to_chars(Span<char> out, int val, unsigned base = 10) NOTHROW {
  return to_chars(out, (long)val, base);
}
inline to_chars_result to_chars(Span<char> out, short val, unsigned base = 10) NOTHROW {
  return to_chars(out, (long)val, base);
}
inline to_chars_result to_chars(Span<char> out, signed char val, unsigned base = 10) NOTHROW {
  return to_chars(out, (long)val, base);
}

/** convert unsigned integer to string */
to_chars_result to_chars(Span<char> out, unsigned long val, unsigned base = 10) NOTHROW;
inline to_chars_result to_chars(Span<char> out, unsigned int val, unsigned base = 10) NOTHROW {
  return to_chars(out, (unsigned long)val, base);
}
inline to_chars_result to_chars(Span<char> out, unsigned short val, unsigned base = 10) NOTHROW {
  return to_chars(out, (unsigned long)val, base);
}
inline to_chars_result to_chars(Span<char> out, unsigned char val, unsigned base = 10) NOTHROW {
  return to_chars(out, (unsigned long)val, base);
}

/** copy chars to string */
to_chars_result to_chars(Span<char> out, View<char> val) NOTHROW;

////////////////////////////////////////////////////////////////////////////////

/** return number of characters needed to store conversion of signed integer to string */
size_t to_chars_len(long val, unsigned base = 10) NOTHROW;
inline size_t to_chars_len(int val, unsigned base = 10) NOTHROW {
  return to_chars_len((long)val, base);
}
inline size_t to_chars_len(short val, unsigned base = 10) NOTHROW {
  return to_chars_len((long)val, base);
}
inline size_t to_chars_len(signed char val, unsigned base = 10) NOTHROW {
  return to_chars_len((long)val, base);
}

/** return number of characters needed to store conversion of unsigned integer to string */
size_t to_chars_len(unsigned long val, unsigned base = 10) NOTHROW;
inline size_t to_chars_len(unsigned int val, unsigned base = 10) NOTHROW {
  return to_chars_len((unsigned long)val, base);
}
inline size_t to_chars_len(unsigned short val, unsigned base = 10) NOTHROW {
  return to_chars_len((unsigned long)val, base);
}
inline size_t to_chars_len(unsigned char val, unsigned base = 10) NOTHROW {
  return to_chars_len((unsigned long)val, base);
}

/** return number of characters needed to store conversion of View<char> to string */
inline size_t to_chars_len(View<char> val) NOTHROW {
  return val.size();
}

#endif /* TWIN_STL_TO_CHARS_H */
