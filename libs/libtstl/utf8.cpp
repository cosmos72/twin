/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/utf8.h"
#include "stl/chars.h"
#include <Tw/autoconf.h> /* for TW_HAVE* macros */

#ifdef TW_HAVE_ARPA_INET_H
#include <arpa/inet.h> // htonl()
#endif

// convert Unicode codepoint 'rune' from UTF-32 to UTF-8
Utf8::seq Utf8::from_rune(trune rune) {
  // replacement character is \uFFFD
  const seq replacement = {"\xEF\xBF\xBD"};
  seq x = {};
  if (rune <= 0x7F) {
    x.b[0] = (char)rune;
  } else if (rune <= 0x07FF) {
    x.b[0] = 0xC0 | (rune >> 6);
    x.b[1] = 0x80 | (rune & 0x3F);
  } else if (rune <= 0xFFFF) {
    if (rune == 0xFFFE || rune == 0xFFFF || (rune >= 0xD800 && rune <= 0xDFFF)) {
      x.val = replacement.val;
    } else {
      x.b[0] = 0xE0 | (rune >> 12);
      x.b[1] = 0x80 | ((rune >> 6) & 0x3F);
      x.b[2] = 0x80 | (rune & 0x3F);
    }
  } else if (rune <= 0x10FFFF) {
    x.b[0] = 0xF0 | (rune >> 18);
    x.b[1] = 0x80 | ((rune >> 12) & 0x3F);
    x.b[2] = 0x80 | ((rune >> 6) & 0x3F);
    x.b[3] = 0x80 | (rune & 0x3F);
  } else {
    x.val = replacement.val;
  }
  return x;
}

trune Utf8::to_rune(seq x) {
  trune rune;
  if (x.b[1] == 0) {
    rune = x.b[0];
  } else if (x.b[2] == 0) {
    rune = ((trune)(x.b[0] & 0x1F) << 6) | (x.b[1] & 0x3F);
  } else if (x.b[3] == 0) {
    rune = ((trune)(x.b[0] & 0xF) << 12) | ((trune)(x.b[1] & 0x3F) << 6) | (x.b[2] & 0x3F);
  } else {
    rune = ((trune)(x.b[0] & 0x7) << 18) | ((trune)(x.b[1] & 0x3F) << 12) |
           ((trune)(x.b[2] & 0x3F) << 6) | (x.b[3] & 0x3F);
  }
  return rune;
}

size_t Utf8::to_size(seq x) {
  size_t n;
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

Chars Utf8::chars() const {
  return Chars(u.b, to_size(u));
}

// parse UTF-8 sequence from chars
bool Utf8::parse(Chars chars, Chars *remaining) {
  const char *src = chars.data();
  const size_t srcn = chars.size();
  // replacement character is \uFFFD
  const seq replacement = {"\xEF\xBF\xBD"};
  seq x = {};
  uint8_t len = 0;
  bool ok = true;
  do {
    if (srcn == 0) {
      ok = false;
      break;
    }
    // copy first byte
    char ch = x.b[0] = src[0];
    len = 1;
    if ((ch & 0xC0) == 0xC0) {
      const uint8_t max = srcn <= 4 ? srcn : 4;
      // copy up to three continuation bytes
      for (; len < max; len++) {
        if (((ch = src[len]) & 0xC0) != 0x80) {
          break;
        }
        x.b[len] = ch;
      }
    }
    ok = valid(x);
  } while (false);

  if (remaining) {
    *remaining = chars.view(len, srcn);
  }
  u.val = ok ? x.val : replacement.val;
  return ok;
}

bool Utf8::valid(seq x) {
  /*
   * the simplest and safest way to validate an UTF8 sequence
   * is converting it to UTF-32 and back to UTF-8,
   * then compare the two UTF-8 sequences.
   *
   * It catches:
   * 1. overlong UTF8 sequences
   * 2. UTF8 sequences that decode to invalid codepoints [0xDC00,0xDFFF]
   * 3. UTF8 sequences that decode to invalid codepoints [0xFFFE,0xFFFF]
   * 4. UTF8 sequences that decode to invalid codepoints > 0x10FFFF
   */
  const trune rune = to_rune(x);
  const seq y = from_rune(rune);
  return x.val == y.val;
}

bool Utf8::less(seq x, seq y) {
  return htonl(x.val) < htonl(y.val);
}
