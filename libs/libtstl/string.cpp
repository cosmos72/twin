/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/string.h"

#include <cstring> // strlen()

Chars chars_from_c(const char *c_str) {
  return Chars(c_str, c_str ? strlen(c_str) : 0);
}

bool String::make_c_str() {
  return append('\0') && (pop_back(), true);
}

bool String::append(View<trune> runes) {
  const size_t oldsize = size();
  bool ok = true;
  for (size_t i = 0, n = runes.size(); ok && i < n; i++) {
    ok = append(Utf8(runes[i]));
  }
  if (!ok) {
    resize(oldsize);
  }
  return ok;
}

bool String::formatv(View<const FmtBase *> args) {
  size_t n = 0;
  for (const FmtBase *arg : args) {
    if (arg) {
      n += arg->size();
    }
  }
  if (!resize0(n, false)) {
    return false;
  }
  size_t i = 0;
  for (const FmtBase *arg : args) {
    if (arg) {
      to_chars_result ret = arg->write_to(span(i, n));
      if (ret.err != SUCCESS) {
        return false;
      }
      i += ret.written;
    }
  }
  assert(i == n);
  return true;
}
