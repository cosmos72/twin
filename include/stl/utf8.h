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

typedef uint32_t trune;
template <class T> class View;

class Utf8 {
private:
  union seq {
    char b[4];
    uint32_t val;
  } u;

  static bool valid(seq x);
  static trune to_rune(seq x);
  static size_t to_size(seq x);
  static seq to_utf8(trune rune);
  static bool less(seq x, seq y);

  explicit Utf8(seq x) {
    u.val = x.val;
  }

public:
  Utf8() {
    u.val = 0;
  }

  Utf8(const Utf8 &other) {
    u.val = other.u.val;
  }

  // convert Unicode codepoint 'rune' from UTF-32 to UTF-8
  explicit Utf8(trune rune) {
    u.val = to_utf8(rune).val;
  }

  // parse UTF-8 sequence from chars
  bool parse(View<char> chars, View<char> *remaining = NULL);

  trune rune() const {
    return to_rune(u);
  }

  //! @return length of UTF-8 byte sequence
  size_t size() const {
    return to_size(u);
  }

  //! @return pointer to UTF-8 byte sequence
  const char *data() const {
    return u.b;
  }

  //! @return view on UTF-8 byte sequence
  View<char> chars() const;

  Utf8 &operator=(const Utf8 &other) {
    u.val = other.u.val;
    return *this;
  }

  bool operator==(const Utf8 &other) const {
    return u.val == other.u.val;
  }

  bool operator<(const Utf8 &other) const {
     return less(u, other.u);
  }

};

#endif /* TWIN_STL_UTF8_H */
