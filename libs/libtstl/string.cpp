/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/string.h"

bool String::make_c_str() {
  if (cap_ > size_) {
    data()[size_] = '\0';
    return true;
  }
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
  // n+1 to make space for final '\0'
  if (!resize0(n + 1, false)) {
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
  // store final '\0' but do not count it in size()
  data()[n] = '\0';
  size_ = n;
  return true;
}
