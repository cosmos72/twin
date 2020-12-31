/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/utf8.h"

utf8::utf8(trune rune) : val_() {
#if TW_IS_LITTLE_ENDIAN
  if (rune <= 0x7F) {
    val_ = rune;
  } else if (rune <= 0x07FF) {
    val_ = 0x80C0 | (rune >> 6) | ((rune & 0x3F) << 8);
  } else if (rune <= 0xFFFF) {
    val_ = 0x8080E0 | (rune >> 12) | ((rune << 2) & 0x3F00) | ((rune & 0x3F) << 16);
  } else if (rune <= 0x10FFFF) {
    val_ = 0x808080F0 | (rune >> 18) | ((rune >> 4) & 0x3F00) | ((rune & 0xFC0) << 10) |
           ((rune & 0x3F) << 24);
  } else {
    val_ = 0xBFBFED; // encode replacement character 0xFFFD
  }
#else // TW_IS_BIG_ENDIAN
  if (rune <= 0x7F) {
    val_ = rune;
  } else if (rune <= 0x07FF) {
    val_ = 0xC080 | ((rune << 2) & 0x1F00) | (rune & 0x3F);
  } else if (rune <= 0xFFFF) {
    val_ = 0xE08080 | ((rune << 4) & 0xF0000) | ((rune << 2) & 0x3F00) | (rune & 0x3F);
  } else if (rune <= 0x10FFFF) {
    val_ = 0xF0808080 | ((rune << 6) & 0x7000000) | ((rune << 4) | 0x3F0000) |
           ((rune << 2) & 0x3F00) | (rune & 0x3F);
  } else {
    val_ = 0xEDBFBF; // encode replacement character 0xFFFD
  }
#endif
}

trune utf8::rune() const {
  trune rune;
#if TW_IS_LITTLE_ENDIAN
  if ((val_ & 0xFF000000) != 0) {
    rune = ((val_ & 0x7) << 18) | ((val_ & 0x3F00) << 4) | ((val_ >> 10) & 0xFC0) |
           ((val_ >> 24) & 0x3F);
  } else if ((val_ & 0xFF0000) != 0) {
    rune = ((val_ & 0x3F) << 12) | ((val_ & 0x3F00) >> 2) | ((val_ >> 16) & 0x3F);
  } else if ((val_ & 0xFF00) != 0) {
    rune = ((val_ & 0x3F) << 6) | ((val_ >> 8) & 0x3F);
  } else {
    rune = val_;
  }
#else // TW_IS_BIG_ENDIAN
#endif
  return rune;
}

size_t utf8::size() const {
  size_t n;
  if ((val_ & 0xFF000000) != 0) {
    n = 4;
  } else if ((val_ & 0xFF0000) != 0) {
    n = 3;
  } else if ((val_ & 0xFF00) != 0) {
    n = 2;
  } else {
    n = 1;
  }
  return n;
}
