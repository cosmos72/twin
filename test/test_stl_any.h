/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/any.h"

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

void test_any_long(long n, Any &out) {
  out = n;
}
