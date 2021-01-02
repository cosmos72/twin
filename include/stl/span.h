/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_SPAN_H
#define TWIN_STL_SPAN_H

#include "stl/view.h"

/** read-write span of T[] */
template <class T> class Span : protected View<T> {
private:
  typedef View<T> Base;

protected:
  using Base::data_;
  using Base::size_;

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

  Span() : Base() {
  }
  template <size_t N> Span(T (&addr)[N]) : Base(addr, N - 1) {
  }
  Span(T *addr, size_t n) : Base(addr, n) {
  }

  Span(Vector<T> &other) : Base(other) {
  }

  // Span(const Span&) = default;
  // ~Span() = default;
  // operator=(const Span&) = default;

  Span &operator=(Vector<T> &other) {
    ref(other);
    return *this;
  }

  using Base::operator==;
  using Base::operator bool;
  using Base::capacity;
  using Base::size;

  using Base::data;
  T *data() {
    return const_cast<T *>(data_);
  }

  using Base::operator[];
  T &operator[](size_t index) {
    assert(index < size_);
    return data()[index];
  }

  void ref(T *addr, size_t n) {
    data_ = addr;
    size_ = n;
  }
  void ref(Span &other) {
    data_ = other.data_;
    size_ = other.size_;
  }
  void ref(Vector<T> &other);

  using Base::begin;
  T *begin() {
    return data();
  }

  using Base::end;
  T *end() {
    assert(data_ || !size_);
    return data() + size_;
  }

  using Base::view;

  Span<T> span(size_t start, size_t end) {
    assert(start <= end);
    assert(end <= size());
    return Span<T>(data() + start, end - start);
  }

  void copy(View<T> src) {
    assert(src.size() == size());
    mem::copyvec(src, *this);
  }

  void swap(Span &other) {
    Span temp = *this;
    *this = other;
    other = temp;
  }
};

template <class T> void View<T>::ref(const Span<T> &other) {
  data_ = other.data();
  size_ = other.size();
}

template <class T> void swap(Span<T> &left, Span<T> &right) {
  left.swap(right);
}

#endif /* TWIN_STL_SPAN_H */
