/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_STRING_H
#define TWIN_STL_STRING_H

#include "stl/vector.h"
#include "stl/utf8.h"

class String : public Vector<char> {
private:
  typedef char T;
  typedef Vector<T> Base;

  // do not implement. reason: any allocation failure would not be visible
  String &operator=(const String &other); // = delete;

public:
  String() : Base() {
  }
  String(const T *addr, size_t n) : Base(addr, n) {
  }
  // all one-argument constructors are explicit because they allocate,
  // thus they mail fail => we require users to explicitly invoke them.
  explicit String(size_t n) : Base(n) {
  }
  template <size_t N> explicit String(const T (&cstr)[N]) : Base(cstr, N - 1) {
  }
  explicit String(const View<T> &other) : Base(other) {
  }
  explicit String(const Span<T> &other) : Base(other) {
  }
  explicit String(const Vector<T> &other) : Base(other) {
  }
  String(const String &other) : Base(other) {
  }
  // ~String() = default;

  using Base::append;

  // add final '\0' but do not count it in size()
  // return true if successful, false if out of memory
  bool make_c_str();

  bool append(Utf8 seq) {
    return append(seq.data(), seq.size());
  }

  // convert UTF-32 runes to UTF-8 and append them to this string
  bool append(View<trune> runes);

  bool operator+=(char ch) {
    return append(ch);
  }

  bool operator+=(Utf8 seq) {
    return append(seq);
  }

  bool operator+=(Chars other) {
    return append(other);
  }

  // convert UTF-32 runes to UTF-8 and append them to this string
  bool operator+=(View<trune> runes) {
    return append(runes);
  }
};

inline void swap(String &left, String &right) {
  // invoke Vector<char>::swap()
  left.swap(right);
}

#endif /* TWIN_STL_STRING_H */
