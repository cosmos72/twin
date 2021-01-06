/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_FIELD_H
#define TWIN_FIELD_H

#include "stl/any.h"
#include "stl_types.h"

#include <Tw/Twstat.h>

// can contain a value of arbitrary type T, with two constraints:
// 1. T has trivial copy constructor and destructor
// 2. sizeof(T) <= 2 * sizeof(size_t)
class Field {
public:
  using Label = uint32_t;
  using TypeId = uint32_t;

  Any any;
  Label label;
  TypeId type_id;

  Field() : any(), label(), type_id(TWS_void) {
  }

  Field(size_t value, Label l, TypeId t) : any(value), label(l), type_id(t) {
  }
  Field(obj o, Label l, TypeId t) : any(o), label(l), type_id(t) {
  }
  Field(CharSpan charspan, Label l, TypeId t) : any(charspan), label(l), type_id(t) {
  }
  Field(ColorSpan colorspan, Label l, TypeId t) : any(colorspan), label(l), type_id(t) {
  }
  Field(RuneSpan runespan, Label l, TypeId t) : any(runespan), label(l), type_id(t) {
  }
  Field(CellSpan cellspan, Label l, TypeId t) : any(cellspan), label(l), type_id(t) {
  }

  // Field(const Field &other) = default;
  // ~Field() = default;

  Field &operator=(const Field &other) {
    any = other.any;
    label = other.label;
    type_id = other.type_id;
    return *this;
  }

  size_t &scalar() {
    return any.value<size_t>();
  }
  obj &obj() {
    return any.value<obj>();
  }
  CharSpan &chars() {
    return any.value<CharSpan>();
  }
  ColorSpan &colors() {
    return any.value<ColorSpan>();
  }
  RuneSpan &runes() {
    return any.value<RuneSpan>();
  }
  CellSpan &cells() {
    return any.value<CellSpan>();
  }
};

#endif /* TWIN_FIELD_H */
