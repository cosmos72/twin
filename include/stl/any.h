/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_ANY_H
#define TWIN_STL_ANY_H

#include "stl/macros.h" // NOTHROW

#include <cstddef> // size_t
#include <cstring> // memcpy
#include <cassert>

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

    static void func() NOTHROW {
    }

    TypeTag func_;

  public:
    Type() NOTHROW : func_(func) {
    }

    operator TypeTag() const NOTHROW {
      return func_;
    }
  };

public:
  Any() NOTHROW : type_(NULL) {
  }

  template <class T> explicit Any(const T &value) NOTHROW {
    set<T>(value);
  }

  // Any(const Any &other) = default;
  // ~Any() = default;

  Any &operator=(const Any &other) NOTHROW {
    copy(other);
    return *this;
  }

  template <class T> Any &operator=(const T &value) NOTHROW {
    set<T>(value);
    return *this;
  }

  TypeTag type() const NOTHROW {
    return type_;
  }

  template <class T> bool is() const NOTHROW {
    return type_ == Type<T>();
  }

  template <class T> bool get(T &result) const NOTHROW {
    if (is<T>()) {
      memcpy(reinterpret_cast<void *>(&result), raw_, sizeof(T));
      return true;
    }
    return false;
  }

  template <class T> T &value() NOTHROW {
    assert(is<T>());
    return *reinterpret_cast<T *>(raw_);
  }

  template <class T> const T &value() const NOTHROW {
    assert(is<T>());
    return *reinterpret_cast<const T *>(raw_);
  }

private:
  unsigned char raw_[2 * sizeof(size_t)];
  TypeTag type_;

  template <class T> void set(const T &value) NOTHROW {
    std::memcpy(raw_, reinterpret_cast<const void *>(&value), sizeof(T) * Type<T>::check);
    std::memset(raw_ + sizeof(T), '\0', sizeof(raw_) - sizeof(T));
    type_ = Type<T>();
  }

  void copy(const Any &other) NOTHROW {
    std::memcpy(raw_, other.raw_, sizeof(raw_));
    type_ = other.type_;
  }
};

#endif /* TWIN_STL_ANY_H */
