/* Copyright (C) 2000-2022 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_FWD_H
#define TWIN_STL_FWD_H

#if defined(__cpp_constexpr) && __cpp_constexpr >= 200704L
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif

class Chars;
template <class T> class Fmt;
template <class T> class Span;
class String;
template <class T> class Vector;
template <class T> class View;

typedef Span<char> CharSpan;

struct to_chars_result;

template <class T> T &lvalue(T &&val) {
  return val;
}

template <class T> const T &lvalue(const T &&val) {
  return val;
}

#endif /* TWIN_STL_FWD_H */
