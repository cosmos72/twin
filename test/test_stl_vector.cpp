/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/any.h"
#include "stl/string.h"

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

  View<T> w2;
  swap(w, w2);
  assert(w.size() == 0);
  assert(w.data() == NULL);
  assert(w2.size() == n);
  assert(w2.data() == data);
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

  Span<T> s2;
  swap(s, s2);
  assert(s.size() == 0);
  assert(s.data() == NULL);
  assert(s2.size() == n);
  assert(s2.data() == data);
}

template <class T> void test_vector() {
  enum { n = 3 };
  T data[n] = {1, 2, 3};
  Vector<T> a;
  Span<T> s;
  View<T> w;

  assert(a.size() == 0);
  assert(a.data() == NULL);

  assert(a.assign(data, n));
  assert(a.size() == n);
  assert(a.data());
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

  enum { large = 1 << 20 };
  assert(a.resize(large));
  assert(a.data());
  assert(a.size() == large);
  assert(!a.resize(size_t(-1) / 2 / sizeof(T)));
  assert(a.data());
  assert(a.size() == large);

  Vector<T> a2;
  a.swap(a2);
  assert(a.data() == NULL);
  assert(a.size() == 0);
  assert(a2.data());
  assert(a2.size() == large);
}

void test_string() {
  typedef char T;
  enum { n = 3 };
  T data[n + 1] = "abc";
  String st(data);

  assert(st.size() == n);
  assert(st.data());
  assert(st.data() != data);

  Chars w = st;
  assert(w.size() == n);
  assert(w.data() == st.data());

  Span<char> s = st;
  assert(s.size() == n);
  assert(s.data() == st.data());

  Vector<char> a;
  st.swap(a);
  assert(st.data() == NULL);
  assert(st.size() == 0);
  assert(a.data());
  assert(a.size() == n);
}

template <class T> void test_any() {
  T value(17), copy(0);
  Any any;
  any = value;

  assert(any.is<T>());
  assert(any.type() == Any::Type<T>());
  assert(any.value<T>() == value);
  assert(&any.value<T>() != &value);

  assert(value != copy);
  assert(any.get<T>(copy));
  assert(value == copy);
}

void swap_string(String &left, String &right) {
  left.swap(right);
}

template <class T> void test_T() {
  test_view<T>();
  test_span<T>();
  test_vector<T>();
  test_any<T>();
}

void test_stl_vector() {
  test_T<signed char>();
  test_T<unsigned char>();
  test_T<short>();
  test_T<int>();
  test_T<long>();
  test_T<float>();
  test_string();
}
