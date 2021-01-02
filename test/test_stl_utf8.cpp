/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/utf8.h"
#include "stl/string.h"

void test_stl_utf8() {
  for (trune rune = 0; rune <= 0x110000; ++rune) {
    const Utf8 x(rune);

    assert(x.size() == x.chars().size());

    if ((rune >= 0xD800 && rune <= 0xDFFF) || rune == 0xFFFE || rune == 0xFFFF || rune > 0x10FFFF) {
      // invalid codepoint, encoded to 'replacement character'
      assert(x.rune() == 0xFFFD);
      assert(x.size() == 3);
      continue;
    }
    assert(x.rune() == rune);
    if (rune <= 0x7F) {
      assert(x.size() == 1);
    } else if (rune <= 0x7FF) {
      assert(x.size() == 2);
    } else if (rune <= 0xFFFF) {
      assert(x.size() == 3);
    } else {
      assert(x.size() == 4);
    }
    Utf8 y;
    assert(y.parse(x.chars()));
    assert(x == y);
  }
}
