/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_ARRAY_H
#define TWIN_STL_ARRAY_H

#include "stl/span.h"
#include "stl/mem.h"

#include <cstring> // memset()

// dynamically resizeable array of arbitrary type T, with two constraints:
// 1. T must have trivial copy constructor and destructor
// 2. zero-initializing T must be equivalent to T default constructor
template <class T> class Array : protected Span<T> {
private:
  typedef Span<T> Base;

  // do not implement. reason: any allocation failure would not be visible
  Array<T> &operator=(const Array<T> &other); // = delete;

protected:
  using Base::data_;
  using Base::size_;
  size_t cap_;

  bool init(size_t n) {
    data_ = mem::alloc<T>(n);
    if (n && !data_) {
      // mem::alloc() failed
      data_ = nullptr;
      cap_ = size_ = 0;
      return false;
    }
    cap_ = size_ = n;
    return true;
  }

  void destroy() {
    if (data_ != nullptr) {
      mem::free(data());
    }
  }

  bool ensure_capacity(size_t n) {
    return cap_ >= n || reserve(n >= cap_ * 2 ? n : cap_ * 2);
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

  Array() : Base(), cap_(0) {
  }
  Array(const T *addr, size_t n) : Base(), cap_(0) {
    dup(addr, n);
  }
  // all one-argument constructors are explicit because they allocate,
  // thus they mail fail => we require users to explicitly invoke them.
  explicit Array(size_t n) : Base(), cap_(0) {
    init(n);
  }
  template <size_t N> explicit Array(const T (&addr)[N]) : Base(), cap_(0) {
    dup(addr, N - 1);
  }
  explicit Array(const View<T> &other) : Base(), cap_(0) {
    dup(other.data(), other.size());
  }
  explicit Array(const Span<T> &other) : Base(), cap_(0) {
    dup(other.data(), other.size());
  }
  explicit Array(const Array &other) : Base(), cap_(0) {
    dup(other.data(), other.size());
  }
  ~Array() {
    destroy();
  }

  template <class VEC> bool operator==(const VEC &other) {
    return mem::equalvec(*this, other);
  }

  bool fail() const {
    return data_ == nullptr;
  }
  size_t capacity() const {
    return cap_;
  }
  using Base::data;
  using Base::size;
  using Base::operator[];
  using Base::operator bool;
  using Base::operator==;
  using Base::begin;
  using Base::copy;
  using Base::end;
  using Base::span;
  using Base::view;

  bool dup(const T *addr, size_t n) {
    if (!ensure_capacity(n)) {
      return false;
    }
    memcpy(data(), addr, n * sizeof(T));
    size_ = n;
    return true;
  }
  bool dup(const View<T> &other) {
    return dup(other.data(), other.size());
  }
  bool dup(const Span<T> &other) {
    return dup(other.data(), other.size());
  }
  bool dup(const Array &other) {
    return dup(other.data(), other.size());
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
          data_ = nullptr;
        }
        return false;
      }
      data_ = newdata;
      cap_ = newcap;
    }
    return true;
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

  void swap(Array &other) {
    struct Ref {
      const T *data_;
      size_t size_, cap_;
    };
    Ref temp;
    mem::rawcopy(*this, temp);
    mem::rawcopy(other, *this);
    mem::rawcopy(temp, other);
  }
};

template <class T> void View<T>::ref(const Array<T> &other) {
  data_ = other.data();
  size_ = other.size();
}
template <class T> void Span<T>::ref(Array<T> &other) {
  data_ = other.data();
  size_ = other.size();
}

template <class T> void swap(Array<T> &left, Array<T> &right) {
  left.swap(right);
}

#endif /* TWIN_STL_ARRAY_H */
