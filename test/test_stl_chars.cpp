/* Copyright (C) 2022 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/chars.h"

#include <cassert>

void test_stl_chars() {
  Chars foobar("foobar");

  assert(Chars().find(Chars()) == 0);
  assert(Chars().find(Chars("x")) == size_t(-1));

  assert(foobar.find(Chars()) == 0);
  assert(foobar.find(Chars("foo")) == 0);
  assert(foobar.find(Chars("obar")) == 2);
  assert(foobar.find(Chars("oobar")) == 1);
  assert(foobar.find(Chars("oor")) == size_t(-1));
  assert(foobar.find(Chars("bc")) == size_t(-1));
}
