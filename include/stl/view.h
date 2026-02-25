/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_VIEW_H
#define TWIN_STL_VIEW_H

#include "stl/fwd.h"
#include "stl/mem.h"

#include <cstddef> // size_t
#include <cassert>

/** read-only view of T[] */
template <class T> class View {

protected:
  const T *data_;
  size_t size_;

public:
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef const T value_type;
  typedef const T &reference;
  typedef const T *pointer;
  typedef const T *iterator;
  typedef const T &const_reference;
  typedef const T *const_pointer;
  typedef const T *const_iterator;

  CONSTEXPR View() NOTHROW : data_(NULL), size_(0) {
  }
  template <size_t N> CONSTEXPR View(const T (&addr)[N]) NOTHROW : data_(addr), size_(N - 1) {
  }
  CONSTEXPR View(const T *addr, size_t n) NOTHROW : data_(addr), size_(n) {
  }

  View(const Span<T> &other) NOTHROW {
    ref(other);
  }
  View(const Vector<T> &other) NOTHROW {
    ref(other);
  }

  // View(const View&) = default;
  // ~View() = default;
  // operator=(const View&) = default;

  View &operator=(const Span<T> &other) NOTHROW {
    ref(other);
    return *this;
  }

  View &operator=(const Vector<T> &other) NOTHROW {
    ref(other);
    return *this;
  }

  size_t capacity() const NOTHROW {
    return size_;
  }

  size_t size() const NOTHROW {
    return size_;
  }

  const T *data() const NOTHROW {
    return data_;
  }

  const T &operator[](size_t index) const NOTHROW {
    assert(index < size_);
    return data_[index];
  }

  /*explicit*/ operator bool() const NOTHROW {
    return size_ != 0;
  }

  bool empty() const NOTHROW {
    return size_ == 0;
  }

  template <class VEC> bool operator==(const VEC &other) const NOTHROW {
    return mem::equalvec(*this, other);
  }
  template <class VEC> bool operator!=(const VEC &other) const NOTHROW {
    return !mem::equalvec(*this, other);
  }

  template <size_t N> void ref(const T (&addr)[N]) NOTHROW {
    data_ = addr;
    size_ = N - 1;
  }
  void ref(const T *addr, size_t n) NOTHROW {
    data_ = addr;
    size_ = n;
  }
  void ref(const View<T> other) NOTHROW {
    data_ = other.data_;
    size_ = other.size_;
  }
  void ref(const Span<T> &other) NOTHROW;   // defined in stl/span.h
  void ref(const Vector<T> &other) NOTHROW; // defined in stl/vector.h

  const T *begin() const NOTHROW {
    return data_;
  }

  const T *end() const NOTHROW {
    assert(data_ || !size_);
    return data_ + size_;
  }

  /* remove the first element */
  void pop_front() NOTHROW {
    if (size_) {
      ++data_;
      --size_;
    }
  }

  /* remove the last element */
  void pop_back() NOTHROW {
    if (size_) {
      --size_;
    }
  }

  /* return a subview of this view */
  View view(size_t start, size_t end) const NOTHROW {
    assert(start <= end);
    assert(end <= size_);
    return View(data_ + start, end - start);
  }

  void swap(View &other) NOTHROW {
    View temp = *this;
    *this = other;
    other = temp;
  }
};

template <class T> void swap(View<T> &left, View<T> &right) NOTHROW {
  left.swap(right);
}

#endif /* TWIN_STL_VIEW_H */
