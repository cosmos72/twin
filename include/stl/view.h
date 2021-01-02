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

#include <stddef.h> // size_t
#include <assert.h>

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

  View() : data_(NULL), size_(0) {
  }
  template <size_t N> View(const T (&addr)[N]) : data_(addr), size_(N - 1) {
  }
  View(const T *addr, size_t n) : data_(addr), size_(n) {
  }

  View(const Span<T> &other) {
    ref(other);
  }
  View(const Vector<T> &other) {
    ref(other);
  }

  // View(const View&) = default;
  // ~View() = default;
  // operator=(const View&) = default;

  View &operator=(const Span<T> &other) {
    ref(other);
    return *this;
  }

  View &operator=(const Vector<T> &other) {
    ref(other);
    return *this;
  }

  size_t capacity() const {
    return size_;
  }

  size_t size() const {
    return size_;
  }

  const T *data() const {
    return data_;
  }

  const T &operator[](size_t index) const {
    assert(index < size_);
    return data_[index];
  }

  explicit operator bool() const {
    return data_ != nullptr;
  }

  template <class VEC> bool operator==(const VEC &other) const {
    return mem::equalvec(*this, other);
  }

  void ref(const T *addr, size_t n) {
    data_ = addr;
    size_ = n;
  }
  void ref(const View<T> &other) {
    data_ = other.data_;
    size_ = other.size_;
  }
  void ref(const Span<T> &other);
  void ref(const Vector<T> &other);

  const T *begin() const {
    return data_;
  }

  const T *end() const {
    assert(data_ || !size_);
    return data_ + size_;
  }

  View view(size_t start, size_t end) const {
    assert(start <= end);
    assert(end <= size_);
    return View(data_ + start, end - start);
  }

  void swap(View &other) {
    View temp = *this;
    *this = other;
    other = temp;
  }
};

template <class T> void swap(View<T> &left, View<T> &right) {
  left.swap(right);
}

#endif /* TWIN_STL_VIEW_H */
