/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_CHARS_H
#define TWIN_STL_CHARS_H

#include "stl/view.h"

class Chars : public View<char> {
  typedef View<char> Base;

public:
  using Base::Base; // export base class constructors

  // construct from base class
  constexpr Chars(const Base &chars) : Base(chars) {
  }

  // Chars(const Chars&) = default;
  // ~Chars() = default;
  // operator=(const Chars&) = default;

  static Chars from_c(const char *c_str);

  bool contains(Chars substr) const {
    return find0(*this, substr) != size_t(-1);
  }

  /* return position of substring if present in this chars,
   * otherwise return size_t(-1) */
  size_t find(Chars substr) const {
    return find0(*this, substr);
  }

  bool starts_with(Chars substr) const {
    return starts_with0(*this, substr);
  }

  /* return a subview of this chars */
  Chars view(size_t start, size_t end) const {
    return Chars(Base::view(start, end));
  }

private:
  static size_t find0(Chars str, Chars substr);
  static bool starts_with0(Chars str, Chars substr);
};

#endif /* TWIN_STL_CHARS_H */
