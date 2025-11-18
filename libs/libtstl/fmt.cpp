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
// Fmt<Void>

to_chars_result Fmt<Void>::write_to(Span<char> dst) const NOTHROW {
  (void)dst;
  return to_chars_result(0, SUCCESS);
}

size_t Fmt<Void>::size() const NOTHROW {
  return 0;
}

// =============================================================================
// Fmt<long>

to_chars_result Fmt<long>::write_to(Span<char> dst) const NOTHROW {
  return to_chars(dst, val_, base_);
}

size_t Fmt<long>::size() const NOTHROW {
  return to_chars_len(val_, base_);
}

// =============================================================================
// Fmt<unsigned long>

to_chars_result Fmt<unsigned long>::write_to(Span<char> dst) const NOTHROW {
  return to_chars(dst, val_, base_);
}

size_t Fmt<unsigned long>::size() const NOTHROW {
  return to_chars_len(val_, base_);
}

// =============================================================================
// Fmt<View<char> >
to_chars_result Fmt<View<char> /**/>::write_to(Span<char> dst) const NOTHROW {
  return to_chars(dst, val_);
}

size_t Fmt<View<char> /**/>::size() const NOTHROW {
  return val_.size();
}
