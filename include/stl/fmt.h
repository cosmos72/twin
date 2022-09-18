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

#include <cstddef>

/** Helper class to convert arbitrary types to string */
class FmtBase {
public:
  /** convert this FmtBase to string */
  virtual to_chars_result write_to(Span<char> dst) const = 0;

  /** return number of chars needed to convert this FmtBase to string */
  virtual size_t size() const = 0;
};

/**
 * Converts signed long to string.
 */
template <> class Fmt<long> : public FmtBase {
public:
  explicit Fmt(long val, unsigned base = 10) : val_(val), base_(base) {
  }

  to_chars_result write_to(Span<char> dst) const override;
  size_t size() const override;

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

  to_chars_result write_to(Span<char> dst) const override;
  size_t size() const override;

private:
  unsigned long val_;
  unsigned base_;
};

/**
 * Copies chars to string.
 */
template <> class Fmt<Chars> : public FmtBase {
public:
  explicit Fmt(Chars val) : val_(val) {
  }

  to_chars_result write_to(Span<char> dst) const override;
  size_t size() const override;

private:
  Chars val_;
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

template <class T> inline Fmt<T> hex(T val) {
  return fmt(val, 16);
}

inline Fmt<Chars> fmt(Chars val) {
  return Fmt<Chars>(val);
}

template <size_t N> Fmt<Chars> fmt(const char (&addr)[N]) {
  return Fmt<Chars>(Chars(addr, N - 1));
}

template <class T> Fmt<T> fmt(Fmt<T> val) {
  return val;
}

#endif /* TWIN_STL_FMT_H */
