/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_STL_ANY_H
#define _TWIN_STL_ANY_H

#include <stddef.h> // size_t
#include <string.h> // memcpy
#include <assert.h>

// can contain a value of arbitrary type T, with two constraints:
// 1. T has trivial copy constructor and destructor
// 2. sizeof(T) <= 2 * sizeof(size_t)
class Any {
public:
  typedef void (*TypeTag)();

  template <class T> class Type {
    friend class Any;

    typedef char sizeof_T_less_eq_sizeof_Any_data[sizeof(T) <= 2 * sizeof(size_t) ? 1 : -1];
    enum { check = sizeof(sizeof_T_less_eq_sizeof_Any_data) / sizeof(char) };

    static void func() {
    }

    TypeTag func_;

  public:
    Type() : func_(func) {
    }

    operator TypeTag() const {
      return func_;
    }
  };

public:
  Any() : type_(NULL) {
  }

  template <class T> explicit Any(const T &value) {
    set<T>(value);
  }

  Any(const Any &other) {
    copy(other);
  }

  // ~Any() = default;

  Any &operator=(const Any &other) {
    copy(other);
    return *this;
  }

  template <class T> Any &operator=(const T &value) {
    set<T>(value);
    return *this;
  }

  TypeTag type() const {
    return type_;
  }

  template <class T> bool is() const {
    return type_ == Type<T>();
  }

  template <class T> bool get(T &result) const {
    if (is<T>()) {
      memcpy(reinterpret_cast<void *>(&result), raw_, sizeof(T));
      return true;
    }
    return false;
  }

  template <class T> T &value() {
    assert(is<T>());
    return *reinterpret_cast<T *>(raw_);
  }

  template <class T> const T &value() const {
    assert(is<T>());
    return *reinterpret_cast<const T *>(raw_);
  }

private:
  unsigned char raw_[2 * sizeof(size_t)];
  TypeTag type_;

  template <class T> void set(const T &value) {
    memcpy(raw_, reinterpret_cast<const void *>(&value), sizeof(T) * Type<T>::check);
    memset(raw_ + sizeof(T), '\0', sizeof(raw_) - sizeof(T));
    type_ = Type<T>();
  }

  void copy(const Any &other) {
    memcpy(raw_, other.raw_, sizeof(raw_));
    type_ = other.type_;
  }
};

#endif /* _TWIN_STL_ANY_H */
