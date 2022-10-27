/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/string.h"

#include <cstdarg>

bool String::make_c_str() NOTHROW {
  if (cap_ > size_) {
    data()[size_] = '\0';
    return true;
  }
  return append('\0') && (pop_back(), true);
}

bool String::append(View<trune> runes) NOTHROW {
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

bool String::formatv(size_t arg_n, /* const FmtBase* */...) NOTHROW {
  std::va_list vargs;
  size_t len = 0;

  va_start(vargs, arg_n);
  for (size_t i = 0; i < arg_n; ++i) {
    const FmtBase *fmt = va_arg(vargs, const FmtBase *);
    if (fmt) {
      len += fmt->size();
    }
  }
  va_end(vargs);

  // len+1 to make space for final '\0'
  if (!resize0(len + 1, false)) {
    return false;
  }
  va_start(vargs, arg_n);
  size_t written = 0;
  for (size_t i = 0; i < arg_n; ++i) {
    const FmtBase *fmt = va_arg(vargs, const FmtBase *);
    if (fmt) {
      to_chars_result ret = fmt->write_to(span(written, len));
      if (ret.err != SUCCESS) {
        va_end(vargs);
        return false;
      }
      written += ret.written;
    }
  }
  assert(written == len);
  // store final '\0' but do not count it in size()
  data()[len] = '\0';
  size_ = len;
  va_end(vargs);
  return true;
}
