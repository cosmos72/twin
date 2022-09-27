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
#include "stl/fmt.h"
#include "stl/chars.h"

class String : public Vector<char> {
private:
  typedef char T;
  typedef Vector<T> Base;

  // do not implement. reason: any allocation failure would not be visible
  String &operator=(const String &other); // = delete;

public:
  CONSTEXPR String() : Base() {
  }
  // allocate a copy of addr and store it in this string
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

  /// convert args to string and assign them to this string,
  /// then ensure it is also '\0' terminated.
  /// @return true if successful, false if resizing this string failed
  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8,
            class T9>
  bool format(const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5,
              const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9) {

    return formatv(CountFmtArgs<T1, T2, T3, T4, T5, T6, T7, T8, T9>::value,    //
                   &lvalue(fmt(arg1)), &lvalue(fmt(arg2)), &lvalue(fmt(arg3)), //
                   &lvalue(fmt(arg4)), &lvalue(fmt(arg5)), &lvalue(fmt(arg6)), //
                   &lvalue(fmt(arg7)), &lvalue(fmt(arg8)), &lvalue(fmt(arg9)));
  }
  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  bool format(const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5,
              const T6 &arg6, const T7 &arg7, const T8 &arg8) {
    return format(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, Void());
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  bool format(const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5,
              const T6 &arg6, const T7 &arg7) {
    return format(arg1, arg2, arg3, arg4, arg5, arg6, arg7, Void(), Void());
  }
  template <class T1, class T2, class T3, class T4, class T5, class T6>
  bool format(const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5,
              const T6 &arg6) {
    return format(arg1, arg2, arg3, arg4, arg5, arg6, Void(), Void(), Void());
  }
  template <class T1, class T2, class T3, class T4, class T5>
  bool format(const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5) {
    return format(arg1, arg2, arg3, arg4, arg5, Void(), Void(), Void(), Void());
  }
  template <class T1, class T2, class T3, class T4>
  bool format(const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4) {
    return format(arg1, arg2, arg3, arg4, Void(), Void(), Void(), Void(), Void());
  }
  template <class T1, class T2, class T3>
  bool format(const T1 &arg1, const T2 &arg2, const T3 &arg3) {
    return format(arg1, arg2, arg3, Void(), Void(), Void(), Void(), Void(), Void());
  }
  template <class T1, class T2> bool format(const T1 &arg1, const T2 &arg2) {
    return format(arg1, arg2, Void(), Void(), Void(), Void(), Void(), Void(), Void());
  }
  template <class T1> bool format(const T1 &arg1) {
    return format(arg1, Void(), Void(), Void(), Void(), Void(), Void(), Void(), Void());
  }
  bool format() {
    clear();
    return make_c_str();
  }

  // add final '\0' but do not count it in size()
  // return true if successful, false if out of memory
  bool make_c_str();

  bool contains(Chars key) const {
    return Chars(*this).contains(key);
  }

  size_t find(Chars key) const {
    return Chars(*this).find(key);
  }

  bool starts_with(Chars substr) const {
    return Chars(*this).starts_with(substr);
  }

  using Base::append;

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

private:
  bool formatv(size_t arg_n, /* const FmtBase* */...);
};

inline void swap(String &left, String &right) {
  // invoke Vector<char>::swap()
  left.swap(right);
}

#endif /* TWIN_STL_STRING_H */
