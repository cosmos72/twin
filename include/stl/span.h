/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_STL_SPAN_H
#define _TWIN_STL_SPAN_H

#include "stl/view.h"

template <class T> class Span : private View<T> {
private:
  typedef View<T> Base;

protected:
  STL_USING Base::data_;
  STL_USING Base::size_;

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
  template <size_t N> Span(T (&addr)[N]) : Base(addr, N) {
  }
  Span(T *addr, size_t n) : Base(addr, n) {
  }

  Span(Array<T> &other) : Base(other) {
  }

  // Span(const Span&) = default;
  // ~Span() = default;
  // operator=(const Span&) = default;

  Span &operator=(Array<T> &other) {
    ref(other);
    return *this;
  }

  STL_USING Base::capacity;
  STL_USING Base::size;

  STL_USING Base::data;
  T *data() {
    return const_cast<T *>(data_);
  }

  STL_USING Base::operator[];
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
  void ref(Array<T> &other);

  STL_USING Base::begin;
  T *begin() {
    return data();
  }

  STL_USING Base::end;
  T *end() {
    assert(data_ || !size_);
    return data() + size_;
  }

  void swap(Span &other) {
    Span temp = *this;
    *this = other;
    other = temp;
  }
};

typedef Span<char> CharSpan;

template <class T> void View<T>::ref(const Span<T> &other) {
  data_ = other.data();
  size_ = other.size();
}

template <class T> void swap(Span<T> &left, Span<T> &right) {
  left.swap(right);
}

#endif /* _TWIN_STL_SPAN_H */
