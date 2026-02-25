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

  CONSTEXPR Chars() NOTHROW : Base() {
  }
  template <size_t N> CONSTEXPR Chars(const char (&addr)[N]) NOTHROW : Base(addr, N - 1) {
  }
  CONSTEXPR Chars(const char *addr, size_t n) NOTHROW : Base(addr, n) {
  }
  // construct from base class
  CONSTEXPR Chars(const View<char> &other) NOTHROW : Base(other) {
  }
  Chars(const Span<char> &other) NOTHROW : Base(other) {
  }
  Chars(const Vector<char> &other) NOTHROW : Base(other) {
  }

  // Chars(const Chars&) = default;
  // ~Chars() = default;
  // operator=(const Chars&) = default;

  // create from nul-terminated C array of chars
  static Chars from_c(const char c_str[]) NOTHROW;

  // create from possibly nul-terminated C array of chars, limited to maximum length
  static Chars from_c_maxlen(const char c_str[], size_t max_len) NOTHROW;

  bool contains(Chars substr) const NOTHROW {
    return find0(*this, substr) != size_t(-1);
  }

  /* return position of substring if present in this chars,
   * otherwise return size_t(-1) */
  size_t find(Chars substr) const NOTHROW {
    return find0(*this, substr);
  }

  bool starts_with(Chars substr) const NOTHROW {
    return starts_with0(*this, substr);
  }

  bool ends_with(Chars substr) const NOTHROW {
    return ends_with0(*this, substr);
  }

  /* return a subview of this chars */
  Chars view(size_t start, size_t end) const NOTHROW {
    return Chars(Base::view(start, end));
  }

  /** skip all initial and final bytes in '\0' ... SPACE */
  Chars trim() const NOTHROW {
    return trim0(*this);
  }

private:
  static size_t find0(Chars str, Chars substr) NOTHROW;
  static bool starts_with0(Chars str, Chars substr) NOTHROW;
  static bool ends_with0(Chars str, Chars substr) NOTHROW;
  static Chars trim0(Chars str) NOTHROW;
};

#endif /* TWIN_STL_CHARS_H */
