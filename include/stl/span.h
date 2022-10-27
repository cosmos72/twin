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
template <class T> class Span : public View<T> {
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

  CONSTEXPR Span() NOTHROW : Base() {
  }
  template <size_t N> CONSTEXPR Span(T (&addr)[N]) NOTHROW : Base(addr, N - 1) {
  }
  CONSTEXPR Span(T *addr, size_t n) NOTHROW : Base(addr, n) {
  }

  Span(Vector<T> &other) NOTHROW : Base(other) {
  }

  // Span(const Span&) = default;
  // ~Span() = default;
  // operator=(const Span&) = default;

  Span &operator=(Vector<T> &other) NOTHROW {
    ref(other);
    return *this;
  }

  using Base::data;

  T *data() NOTHROW {
    return const_cast<T *>(data_);
  }

  using Base::operator[];

  T &operator[](size_t index) NOTHROW {
    assert(index < size_);
    return data()[index];
  }

  void ref(T *addr, size_t n) NOTHROW {
    data_ = addr;
    size_ = n;
  }
  void ref(Span other) NOTHROW {
    data_ = other.data_;
    size_ = other.size_;
  }
  void ref(Vector<T> &other) NOTHROW;

  using Base::begin;

  T *begin() NOTHROW {
    return data();
  }

  using Base::end;

  T *end() NOTHROW {
    assert(data_ || !size_);
    return data() + size_;
  }

  Span<T> span(size_t start, size_t end) NOTHROW {
    assert(start <= end);
    assert(end <= size_);
    return Span<T>(data() + start, end - start);
  }

  void copy(View<T> src) NOTHROW {
    assert(src.size() == size_);
    mem::copyvec(src, *this);
  }

  void swap(Span &other) NOTHROW {
    Span temp = *this;
    *this = other;
    other = temp;
  }
};

template <class T> void View<T>::ref(const Span<T> &other) NOTHROW {
  data_ = other.data();
  size_ = other.size();
}

template <class T> void swap(Span<T> &left, Span<T> &right) NOTHROW {
  left.swap(right);
}

typedef Span<char> CharSpan;

#endif /* TWIN_STL_SPAN_H */
