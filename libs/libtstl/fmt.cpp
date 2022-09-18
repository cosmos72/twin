/*
 *  fmt.cpp  --  classes to convert arbitrary types to string
 *
 *  Copyright (C) 2022 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "stl/fmt.h"

// =============================================================================
// Fmt<long>

to_chars_result Fmt<long>::write_to(Span<char> dst) const {
  return to_chars(dst, val_, base_);
}

size_t Fmt<long>::size() const {
  if (val_ >= 0) {
    return Fmt<unsigned long>((unsigned long)val_, base_).size();
  } else {
    return 1 + Fmt<unsigned long>((unsigned long)-val_, base_).size();
  }
}

// =============================================================================
// Fmt<unsigned long>

to_chars_result Fmt<unsigned long>::write_to(Span<char> dst) const {
  return to_chars(dst, val_, base_);
}

size_t Fmt<unsigned long>::size() const {
  if (base_ < 2 || base_ > 36) {
    return 0;
  } else if (val_ == 0) {
    return 1;
  }
  unsigned long val = val_;
  const unsigned base = base_;
  unsigned n = 0;
  while (val != 0) {
    val /= base;
    n++;
  }
  return n;
}

// =============================================================================
// Fmt<Chars>
to_chars_result Fmt<Chars>::write_to(Span<char> dst) const {
  return to_chars(dst, val_);
}

size_t Fmt<Chars>::size() const {
  return val_.size();
}
