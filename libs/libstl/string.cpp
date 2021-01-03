/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include "stl/string.h"

bool String::append_runes(View<trune> runes) {
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
