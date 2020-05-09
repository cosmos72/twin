/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_STL_ARRAY_H
#define _TWIN_STL_ARRAY_H

#include "stl/span.h"
#include "stl/mem.h"

#include <string.h> // memcpy(), memset()

template <class T> class Array : private Span<T> {
private:
  typedef Span<T> Base;

  // do not implement. reason: any allocation failure would not be visible
  Array<T> &operator=(const Array<T> &other);

protected:
  STL_USING Base::data_;
  STL_USING Base::size_;
  size_t cap_;

  bool init(size_t n) {
    data_ = mem::alloc<T>(n);
    if (n && !data_) {
      // mem::alloc() failed
      data_ = (T *)(size_t)-1;
      cap_ = size_ = 0;
      return false;
    }
    cap_ = size_ = n;
    return true;
  }

  void destroy() {
    if (data_ != (T *)(size_t)-1) {
      mem::free(data());
    }
  }

  bool ensure_capacity(size_t n) {
    return cap_ >= n || reserve(n >= cap_ * 2 ? n : cap_ * 2);
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
  explicit Array(size_t n) : Base() {
    init(n);
  }
  Array(const T *mem, size_t n) : Base() {
    dup(mem, n);
  }
  explicit Array(const View<T> &other) : Base() {
    dup(other.data(), other.size());
  }
  explicit Array(const Span<T> &other) : Base() {
    dup(other.data(), other.size());
  }
  Array(const Array<T> &other) : Base() {
    dup(other.data(), other.size());
  }
  ~Array() {
    destroy();
  }

  bool fail() const {
    return data_ == (T *)(size_t)-1;
  }
  size_t capacity() const {
    return cap_;
  }
  STL_USING Base::size;
  STL_USING Base::data;
  STL_USING Base::operator[];
  STL_USING Base::begin;
  STL_USING Base::end;

  bool dup(const T *mem, size_t n) {
    if (!ensure_capacity(n)) {
      return false;
    }
    memcpy(data(), mem, n * sizeof(T));
    size_ = n;
    return true;
  }
  bool dup(const View<T> &other) {
    return dup(other.data(), other.size());
  }
  bool dup(const Span<T> &other) {
    return dup(other.data(), other.size());
  }
  bool dup(const Array<T> &other) {
    return dup(other.data(), other.size());
  }

  void clear() {
    (void)resize(0);
  }

  bool resize(size_t n) {
    if (!ensure_capacity(n)) {
      return false;
    }
    if (n > size_) {
      memset(data() + size_, '\0', (n - size_) * sizeof(T));
    }
    size_ = n;
    return true;
  }

  bool reserve(size_t newcap) {
    if (newcap > cap_) {
      T *olddata = fail() ? (T *)0 : data();
      T *newdata = mem::realloc(olddata, cap_, newcap);
      if (!newdata) {
        if (cap_ == 0) {
          data_ = (T *)(size_t)-1;
        }
        return false;
      }
      data_ = newdata;
      cap_ = newcap;
    }
    return true;
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

#endif /* _TWIN_STL_ARRAY_H */
