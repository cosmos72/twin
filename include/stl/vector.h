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

  bool init(size_t n) {
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

  void destroy() {
    if (data_ != NULL) {
      mem::free(data());
    }
  }

  bool ensure_capacity(size_t n) {
    if (cap_ >= n) {
      return true;
    }
    const size_t cap2 = cap_ >= 10 ? cap_ * 2 : 10;
    return reserve(n >= cap2 ? n : cap2);
  }

  bool resize0(size_t n, bool zerofill) {
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

  CONSTEXPR Vector() : Base(), cap_(0) {
  }
  // allocate a copy of addr and store it in this string
  Vector(const T *addr, size_t n) : Base(), cap_(0) {
    assign(addr, n);
  }
  // all one-argument constructors are explicit because they allocate,
  // thus they mail fail => we require users to explicitly invoke them.
  explicit Vector(size_t n) : Base(), cap_(0) {
    init(n);
  }
  template <size_t N> explicit Vector(const T (&addr)[N]) : Base(), cap_(0) {
    assign(addr, N - 1);
  }
  explicit Vector(const View<T> &other) : Base(), cap_(0) {
    assign(other.data(), other.size());
  }
  explicit Vector(const Span<T> &other) : Base(), cap_(0) {
    assign(other.data(), other.size());
  }
  explicit Vector(const Vector &other) : Base(), cap_(0) {
    assign(other.data(), other.size());
  }
  ~Vector() {
    destroy();
  }

  template <class VEC> bool operator==(const VEC &other) {
    return mem::equalvec(*this, other);
  }

  bool fail() const {
    return data_ == NULL;
  }
  size_t capacity() const {
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

  bool assign(const T *addr, size_t n) {
    if (!ensure_capacity(n)) {
      return false;
    }
    memcpy(data(), addr, n * sizeof(T));
    size_ = n;
    return true;
  }
  bool assign(const View<T> other) {
    return assign(other.data(), other.size());
  }
  bool assign(const Span<T> other) {
    return assign(other.data(), other.size());
  }
  bool assign(const Vector &other) {
    return assign(other.data(), other.size());
  }

  void clear() {
    (void)resize(0);
  }

  bool resize(size_t n) {
    return resize0(n, true);
  }

  bool reserve(size_t newcap) {
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

  bool append(const T &src) {
    return append(View<T>(&src, 1));
  }

  bool append(const T *src, const size_t n) {
    return append(View<T>(src, n));
  }

  bool append(View<T> src) {
    const size_t oldn = size_;
    const size_t srcn = src.size();
    if (!resize0(oldn + srcn, false)) {
      return false;
    }
    span(oldn, size()).copy(src);
    return true;
  }

  void swap(Vector &other) {
    mem::rawswap(data_, other.data_);
    mem::rawswap(size_, other.size_);
    mem::rawswap(cap_, other.cap_);
  }
};

template <class T> void View<T>::ref(const Vector<T> &other) {
  data_ = other.data();
  size_ = other.size();
}

template <class T> void Span<T>::ref(Vector<T> &other) {
  data_ = other.data();
  size_ = other.size();
}

template <class T> void swap(Vector<T> &left, Vector<T> &right) {
  left.swap(right);
}

typedef Vector<char> CharVec;

#endif /* TWIN_STL_VECTOR_H */
