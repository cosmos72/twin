/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_UTF8_H
#define TWIN_STL_UTF8_H

#include <stddef.h> // size_t
#include <stdint.h> // uint32_t

#include "stl/macros.h" // NOTHROW

typedef uint32_t trune;
class Chars;

class Utf8 {
private:
  union seq {
    char b[4];
    uint32_t val;
  } u;

  static bool valid(seq x) NOTHROW;
  static trune to_rune(seq x) NOTHROW;
  static size_t to_size(seq x) NOTHROW;
  static seq from_rune(trune rune) NOTHROW;
  static bool less(seq x, seq y) NOTHROW;

  explicit Utf8(seq x) NOTHROW {
    u.val = x.val;
  }

public:
  Utf8() NOTHROW {
    u.val = 0;
  }

  Utf8(const Utf8 &other) NOTHROW {
    u.val = other.u.val;
  }

  // convert Unicode codepoint 'rune' from UTF-32 to UTF-8
  explicit Utf8(trune rune) NOTHROW {
    u.val = from_rune(rune).val;
  }

  // parse UTF-8 sequence from chars
  bool parse(Chars chars, Chars *remaining = NULL) NOTHROW;

  trune rune() const NOTHROW {
    return to_rune(u);
  }

  //! @return length of UTF-8 byte sequence
  size_t size() const NOTHROW {
    return to_size(u);
  }

  //! @return pointer to UTF-8 byte sequence
  const char *data() const NOTHROW {
    return u.b;
  }

  //! @return view on UTF-8 byte sequence
  Chars chars() const NOTHROW;

  Utf8 &operator=(const Utf8 &other) NOTHROW {
    u.val = other.u.val;
    return *this;
  }

  bool operator==(const Utf8 &other) const NOTHROW {
    return u.val == other.u.val;
  }

  bool operator<(const Utf8 &other) const NOTHROW {
    return less(u, other.u);
  }
};

#endif /* TWIN_STL_UTF8_H */
