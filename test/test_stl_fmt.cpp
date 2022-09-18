/* Copyright (C) 2022 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/fmt.h"
#include "stl/string.h"

#include <cassert>

void test_stl_fmt() {
  String out(64);

  assert(to_chars(out, 0) == (to_chars_result{1, SUCCESS}));
  assert(out[0] == '0');

  assert(to_chars(out, 12345) == (to_chars_result{5, SUCCESS}));
  assert(out.span(0, 5) == Chars("12345"));

  assert(to_chars(out, 2147483647l) == (to_chars_result{10, SUCCESS}));
  assert(out.span(0, 10) == Chars("2147483647"));

  assert(to_chars(out, -2147483648l) == (to_chars_result{11, SUCCESS}));
  assert(out.span(0, 11) == Chars("-2147483648"));

  assert(to_chars(out, 2147483647l, 16) == (to_chars_result{8, SUCCESS}));
  assert(out.span(0, 8) == Chars("7fffffff"));

  assert(to_chars(out, -2147483648l, 16) == (to_chars_result{9, SUCCESS}));
  assert(out.span(0, 9) == Chars("-80000000"));
}
