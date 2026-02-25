/* Copyright (C) 2022 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/chars.h"

#include <cstring> // memchr(), strlen()

Chars Chars::from_c(const char *c_str) NOTHROW {
  return Chars(c_str, c_str ? strlen(c_str) : 0);
}

Chars Chars::from_c_maxlen(const char c_str[], size_t max_len) NOTHROW {
  const char *end = (const char *)std::memchr(c_str, '\0', max_len);
  const size_t len = end ? size_t(c_str - end) : max_len;
  return Chars(c_str, len);
}

size_t Chars::find0(Chars str, Chars substr) NOTHROW {
  size_t n1 = str.size();
  size_t n2 = substr.size();
  if (n2 == 0) {
    return 0;
  } else if (n1 < n2) {
    return size_t(-1);
  }
  const char *s1 = str.data();
  const char *s2 = substr.data();
  if (s1 <= s2 && s1 + n2 > s2) {
    /* s2 points inside s1, and there's no space for an earlier substring == s1 */
    const size_t pos = s2 - s1;
    if (n1 >= n2 + pos) {
      return pos;
    }
  }
  const char *pos;
  const char ch0 = s2[0];
  while (n1 >= n2 && (pos = (const char *)memchr(s1, ch0, n1 - n2 + 1)) != NULL) {
    if (n2 == 1 || !std::memcmp(pos, s2, n2)) {
      return pos - str.data();
    }
    n1 -= (pos + 1) - s1;
    s1 = pos + 1;
  }
  return size_t(-1);
}

bool Chars::starts_with0(Chars str, Chars substr) NOTHROW {
  size_t n1 = str.size();
  size_t n2 = substr.size();
  if (n2 == 0) {
    return true;
  } else if (n1 < n2) {
    return false;
  }
  const char *s1 = str.data();
  const char *s2 = substr.data();
  return s1 == s2 || !std::memcmp(s1, s2, n2);
}

bool Chars::ends_with0(Chars str, Chars substr) NOTHROW {
  size_t n1 = str.size();
  size_t n2 = substr.size();
  if (n2 == 0) {
    return true;
  } else if (n1 < n2) {
    return false;
  }
  const char *s1 = str.data();
  const char *s2 = substr.data();
  return (s1 == s2 && n1 == n2) || !std::memcmp(s1 + (n1 - n2), s2, n2);
}

Chars Chars::trim0(Chars str) NOTHROW {
  size_t i, j;
  char ch;
  for (i = 0; i < str.size() && (ch = str[i]) >= '\0' && ch <= ' '; i++) {
  }
  for (j = str.size(); j > i && (ch = str[j - 1]) >= '\0' && ch <= ' '; j--) {
  }
  return str.view(i, j);
}
