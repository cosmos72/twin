/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_STL_FIELD_H
#define _TWIN_STL_FIELD_H

#include "stl/any.h"

// can contain a value of arbitrary type T, with two constraints:
// 1. T has trivial copy constructor and destructor
// 2. sizeof(T) <= 2 * sizeof(size_t)
class Field : public Any {
public:
  using Label = uint32_t;
  using TypeId = uint32_t;

  Label label;
  TypeId type_id;

  Field() : Any() {
  }

  template <class T> Field(const T &value, Label l, TypeId t) : Any(value), label(l), type_id(t) {
  }

  // Field(const Field &other) = default;
  // ~Field() = default;

  Field &operator=(const Field &other) {
    Any::operator=(other);
    label = other.label;
    type_id = other.type_id;
    return *this;
  }
};

#endif /* _TWIN_STL_FIELD_H */
