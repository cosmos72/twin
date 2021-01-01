/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/utf8.h"


utf8::utf8(trune rune) {
  // encode replacement character 0xFFFD
  const u4 replacement = { { 0xED, 0xBF, 0xBF, 0 } };
  u4 x = { };
  if (rune <= 0x7F) {
    x.b[0] = (char)rune;
  } else if (rune <= 0x07FF) {
    x.b[0] = 0xC0 | (rune >> 6);
    x.b[1] = 0x80 | (rune & 0x3F);
  } else if (rune <= 0xFFFF) {
    if (rune < 0xD800 || rune > 0xDFFF) {
      x.b[0] = 0xE0 | (rune >> 12);
      x.b[1] = 0x80 | ((rune >> 6) & 0x3F);
      x.b[2] = 0x80 | (rune & 0x3F);
    } else {
      x.val = replacement.val;
    }
  } else if (rune <= 0x10FFFF) {
    x.b[0] = 0xF0 | (rune >> 18);
    x.b[1] = 0x80 | ((rune >> 12) & 0x3F);
    x.b[2] = 0x80 | ((rune >> 6) & 0x3F);
    x.b[3] = 0x80 | (rune & 0x3F);
  } else {
    x.val = replacement.val;
  }
  u.val = x.val;
}

trune utf8::rune() const {
  trune rune;
  u4 x;
  x.val = u.val;
  if (x.b[1] == 0) {
    rune = x.b[0];
  } else if (x.b[2] == 0) {
    rune = ((trune)(x.b[0] & 0x1F) << 6) |
       (x.b[1] & 0x3F);
  } else if (x.b[3] == 0) {
    rune = ((trune)(x.b[0] & 0xF) << 12) |
       ((trune)(x.b[1] & 0x3F) << 6) |
       (x.b[2] & 0x3F);
  } else {
    rune = ((trune)(x.b[0] & 0x7) << 18) |
       ((trune)(x.b[1] & 0x3F) << 12) |
       ((trune)(x.b[2] & 0x3F) << 6) |
       (x.b[3] & 0x3F);
  }
  return rune;
}

size_t utf8::size() const {
  size_t n;
  u4 x;
  x.val = u.val;
  if (x.b[1] == 0) {
    n = 1;
  } else if (x.b[2] == 0) {
    n = 2;
  } else if (x.b[3] == 0) {
    n = 3;
  } else {
    n = 4;
  }
  return n;
}
