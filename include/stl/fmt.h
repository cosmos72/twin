/* Copyright (C) 2022 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_FMT_H
#define TWIN_STL_FMT_H

#include "stl/to_chars.h"
#include "stl/chars.h"
#include "stl/void.h"
#include "stl/type_traits.h"

#include <stddef.h>

/** Helper class to convert arbitrary types to string */
class FmtBase {
public:
  /** convert this FmtBase to string */
  virtual to_chars_result write_to(Span<char> dst) const = 0;

  /** return number of chars needed to convert this FmtBase to string */
  virtual size_t size() const = 0;
};

/**
 * Converts Void to empty string.
 */
template <> class Fmt<Void> : public FmtBase {
public:
  explicit Fmt(Void unused) {
  }

  to_chars_result write_to(Span<char> dst) const OVERRIDE;
  size_t size() const OVERRIDE;
};

/**
 * Converts signed long to string.
 */
template <> class Fmt<long> : public FmtBase {
public:
  explicit Fmt(long val, unsigned base = 10) : val_(val), base_(base) {
  }

  to_chars_result write_to(Span<char> dst) const OVERRIDE;
  size_t size() const OVERRIDE;

private:
  long val_;
  unsigned base_;
};

/**
 * Converts unsigned long to string.
 */
template <> class Fmt<unsigned long> : public FmtBase {
public:
  explicit Fmt(unsigned long val, unsigned base = 10) : val_(val), base_(base) {
  }

  to_chars_result write_to(Span<char> dst) const OVERRIDE;
  size_t size() const OVERRIDE;

private:
  unsigned long val_;
  unsigned base_;
};

/**
 * Copies chars to string.
 */
template <> class Fmt<View<char> /**/> : public FmtBase {
public:
  explicit Fmt(View<char> val) : val_(val) {
  }

  to_chars_result write_to(Span<char> dst) const OVERRIDE;
  size_t size() const OVERRIDE;

private:
  View<char> val_;
};

inline Fmt<long> fmt(signed char val, unsigned base = 10) {
  return Fmt<long>(val, base);
}
inline Fmt<long> fmt(short val, unsigned base = 10) {
  return Fmt<long>(val, base);
}
inline Fmt<long> fmt(int val, unsigned base = 10) {
  return Fmt<long>(val, base);
}
inline Fmt<long> fmt(long val, unsigned base = 10) {
  return Fmt<long>(val, base);
}

inline Fmt<unsigned long> fmt(unsigned char val, unsigned base = 10) {
  return Fmt<unsigned long>(val, base);
}
inline Fmt<unsigned long> fmt(unsigned short val, unsigned base = 10) {
  return Fmt<unsigned long>(val, base);
}
inline Fmt<unsigned long> fmt(unsigned int val, unsigned base = 10) {
  return Fmt<unsigned long>(val, base);
}
inline Fmt<unsigned long> fmt(unsigned long val, unsigned base = 10) {
  return Fmt<unsigned long>(val, base);
}

template <class T>
inline Fmt<typename conditional<(T(-1) < T(0)), long, unsigned long>::type> hex(T val) {
  return fmt(val, 16);
}

inline Fmt<Void> fmt(Void val) {
  return Fmt<Void>(val);
}

inline Fmt<View<char> /**/> fmt(View<char> val) {
  return Fmt<View<char> /**/>(val);
}

template <size_t N> Fmt<View<char> /**/> fmt(const char (&addr)[N]) {
  return Fmt<View<char> /**/>(View<char>(addr, N - 1));
}

template <class T> Fmt<T> fmt(Fmt<T> val) {
  return val;
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct CountFmtArgs {
  enum {
    value = !IsVoid<T9>::value   ? 9
            : !IsVoid<T8>::value ? 8
            : !IsVoid<T7>::value ? 7
            : !IsVoid<T6>::value ? 6
            : !IsVoid<T5>::value ? 5
            : !IsVoid<T4>::value ? 4
            : !IsVoid<T3>::value ? 3
            : !IsVoid<T2>::value ? 2
            : !IsVoid<T1>::value ? 1
                                 : 0
  };
};

#endif /* TWIN_STL_FMT_H */
