/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/array.h"

template <class T> void test_view() {
  enum { n = 3 };
  T data[n] = {1, 2, 3};
  View<T> w;

  assert(w.size() == 0);
  assert(w.data() == NULL);

  w.ref(data, n);
  assert(w.size() == n);
  assert(w.data() == data);

#ifdef STL_CXX11
  {
    T sum1 = 0;
    for (T e : w) {
      sum1 += e;
    }
    assert(sum1 == n * (n + 1) / 2);
  }
#endif
  {
    T sum2 = 0;
    typename View<T>::const_iterator iter = w.begin(), end = w.end();
    for (; iter != end; ++iter) {
      sum2 += *iter;
    }
    assert(sum2 == n * (n + 1) / 2);
  }

  for (size_t i = 0; i < n; i++) {
    assert(w[i] == data[i]);
    data[i] = T(-data[i]);
    assert(w[i] == data[i]);
  }
}

template <class T> void test_span() {
  enum { n = 3 };
  T data[n] = {1, 2, 3};
  Span<T> s;
  View<T> w;

  assert(s.size() == 0);
  assert(s.data() == NULL);

  s.ref(data, n);
  assert(s.size() == n);
  assert(s.data() == data);

  w.ref(s);
  assert(w.size() == n);
  assert(w.data() == data);

  for (size_t i = 0; i < n; i++) {
    assert(s[i] == data[i]);
    assert(s[i] == w[i]);
    s[i] = T(-s[i]);
    assert(s[i] == data[i]);
    assert(s[i] == w[i]);
  }
}

template <class T> void test_array() {
  enum { n = 3 };
  T data[n] = {1, 2, 3};
  Array<T> a;
  Span<T> s;
  View<T> w;

  assert(a.size() == 0);
  assert(a.data() == NULL);

  assert(a.copy(data, n));
  assert(a.size() == n);
  assert(a.data() != data);

  s.ref(a);
  assert(s.size() == n);
  assert(s.data() == a.data());

  w.ref(a);
  assert(w.size() == n);
  assert(w.data() == a.data());

  for (size_t i = 0; i < n; i++) {
    assert(a[i] == data[i]);
    assert(a[i] == s[i]);
    assert(a[i] == w[i]);
    a[i] = T(-a[i]);
    assert(a[i] == T(-data[i]));
    assert(a[i] == s[i]);
    assert(a[i] == w[i]);
  }

  assert(a.resize(1 << 20));
  assert(!a.resize(size_t(-1) / 2 / sizeof(T)));
}

template <class T> void test() {
  test_view<T>();
  test_span<T>();
  test_array<T>();
}

int main() {
  test<unsigned char>();
  test<short>();
  test<int>();
  test<long>();
  test<float>();
  return 0;
}
