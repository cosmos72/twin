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
  // g++ 4.9.2 does not fully grok "using Base::Base"
  // => manually define all constructors

  constexpr Chars() : Base() {
  }
  template <size_t N> constexpr Chars(const char (&addr)[N]) : Base(addr, N - 1) {
  }
  constexpr Chars(const char *addr, size_t n) : Base(addr, n) {
  }
  // construct from base class
  constexpr Chars(const View<char> &other) : Base(other) {
  }
  Chars(const Span<char> &other) : Base(other) {
  }
  Chars(const Vector<char> &other) : Base(other) {
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
