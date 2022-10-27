/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_VECTOR_H
#define TWIN_STL_VECTOR_H

#include "stl/span.h"
#include "stl/mem.h"

#include <cstring> // memset()

// dynamically resizeable vector of arbitrary type T, with two constraints:
// 1. T must have trivial copy constructor, destructor and assignment operator
// 2. zero-initializing T must be equivalent to T default constructor
template <class T> class Vector : protected Span<T> {
private:
  typedef Span<T> Base;

  // do not implement. reason: any allocation failure would not be visible
  Vector<T> &operator=(const Vector<T> &other); // = delete;

protected:
  using Base::data_;
  using Base::size_;
  size_t cap_;

  bool init(size_t n) NOTHROW {
    data_ = mem::alloc<T>(n);
    if (n && !data_) {
      // mem::alloc() failed
      data_ = NULL;
      cap_ = size_ = 0;
      return false;
    }
    cap_ = size_ = n;
    return true;
  }

  void destroy() NOTHROW {
    if (data_ != NULL) {
      mem::free(data());
    }
  }

  bool ensure_capacity(size_t n) NOTHROW {
    if (cap_ >= n) {
      return true;
    }
    const size_t cap2 = cap_ >= 10 ? cap_ * 2 : 10;
    return reserve(n >= cap2 ? n : cap2);
  }

  bool resize0(size_t n, bool zerofill) NOTHROW {
    if (!ensure_capacity(n)) {
      return false;
    }
    if (n > size_) {
      memset(data() + size_, '\0', (n - size_) * sizeof(T));
    }
    size_ = n;
    return true;
  }

public:
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef T &reference;
  typedef T *pointer;
  typedef T *iterator;
  typedef const T &const_reference;
  typedef const T *const_pointer;
  typedef const T *const_iterator;

  CONSTEXPR Vector() NOTHROW : Base(), cap_(0) {
  }
  // allocate a copy of addr and store it in this string
  Vector(const T *addr, size_t n) NOTHROW : Base(), cap_(0) {
    assign(addr, n);
  }
  // all one-argument constructors are explicit because they allocate,
  // thus they mail fail => we require users to explicitly invoke them.
  explicit Vector(size_t n) NOTHROW : Base(), cap_(0) {
    init(n);
  }
  template <size_t N> explicit Vector(const T (&addr)[N]) NOTHROW : Base(), cap_(0) {
    assign(addr, N - 1);
  }
  explicit Vector(const View<T> &other) NOTHROW : Base(), cap_(0) {
    assign(other.data(), other.size());
  }
  explicit Vector(const Span<T> &other) NOTHROW : Base(), cap_(0) {
    assign(other.data(), other.size());
  }
  explicit Vector(const Vector &other) NOTHROW : Base(), cap_(0) {
    assign(other.data(), other.size());
  }
  ~Vector() NOTHROW {
    destroy();
  }

  template <class VEC> bool operator==(const VEC &other) NOTHROW {
    return mem::equalvec(*this, other);
  }

  bool fail() const NOTHROW {
    return data_ == NULL;
  }
  size_t capacity() const NOTHROW {
    return cap_;
  }
  using Base::begin;
  using Base::copy;
  using Base::data;
  using Base::empty;
  using Base::end;
  using Base::operator[];
  using Base::operator bool;
  using Base::operator==;
  using Base::operator!=;
  using Base::pop_back;
  using Base::size;
  using Base::span;
  using Base::view;

  bool assign(const T *addr, size_t n) NOTHROW {
    if (!ensure_capacity(n)) {
      return false;
    }
    std::memcpy(data(), addr, n * sizeof(T));
    size_ = n;
    return true;
  }
  bool assign(const View<T> other) NOTHROW {
    return assign(other.data(), other.size());
  }
  bool assign(const Span<T> other) NOTHROW {
    return assign(other.data(), other.size());
  }
  bool assign(const Vector &other) NOTHROW {
    return assign(other.data(), other.size());
  }

  void clear() NOTHROW {
    (void)resize(0);
  }

  bool resize(size_t n) NOTHROW {
    return resize0(n, true);
  }

  bool reserve(size_t newcap) NOTHROW {
    if (newcap > cap_) {
      T *olddata = fail() ? (T *)0 : data();
      T *newdata = mem::realloc(olddata, cap_, newcap);
      if (!newdata) {
        if (cap_ == 0) {
          data_ = NULL;
        }
        return false;
      }
      data_ = newdata;
      cap_ = newcap;
    }
    return true;
  }

  bool append(const T &src) NOTHROW {
    return append(View<T>(&src, 1));
  }

  bool append(const T *src, const size_t n) NOTHROW {
    return append(View<T>(src, n));
  }

  bool append(View<T> src) NOTHROW {
    const size_t oldn = size_;
    const size_t srcn = src.size();
    if (!resize0(oldn + srcn, false)) {
      return false;
    }
    span(oldn, size()).copy(src);
    return true;
  }

  void swap(Vector &other) NOTHROW {
    mem::rawswap(data_, other.data_);
    mem::rawswap(size_, other.size_);
    mem::rawswap(cap_, other.cap_);
  }

  /// \return pointer to internal data and transfer its ownership to the caller.
  /// After this method returns, caller owns the returned data and must invoke FreeMem() on it
  /// when no longer needed.
  /// After this method returns, the vector will be empty.
  T *release() NOTHROW {
    T *ptr = data();
    data_ = NULL;
    size_ = 0;
    cap_ = 0;
    return ptr;
  }
};

template <class T> void View<T>::ref(const Vector<T> &other) NOTHROW {
  data_ = other.data();
  size_ = other.size();
}

template <class T> void Span<T>::ref(Vector<T> &other) NOTHROW {
  data_ = other.data();
  size_ = other.size();
}

template <class T> void swap(Vector<T> &left, Vector<T> &right) NOTHROW {
  left.swap(right);
}

typedef Vector<char> CharVec;

#endif /* TWIN_STL_VECTOR_H */
