/* Copyright (C) 2026 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include "ini.h"

#include <stddef.h> // size_t
#include <string.h> // memchr()
#include <stdio.h>  // fopen(), fgets()

Ini::Ini(String &path) : file(NULL), buf(), lineno(0) {
  if (buf.resize(512) && path.make_c_str()) {
    file = fopen(path.data(), "r");
  }
}

Ini::~Ini() {
  if (file) {
    fclose((FILE *)file);
  }
}

static size_t line_length(Chars buf) {
  const char *end = (const char *)memchr(buf.data(), '\0', buf.size());
  return end ? end - buf.data() : buf.size();
}

IniToken Ini::next() {
  v = k = sect = Chars();
  while (file && fgets(buf.data(), buf.size(), (FILE *)file)) {
    lineno++;
    Chars line = Chars(buf.data(), line_length(buf)).trim();
    if (line.empty() || line[0] == ';' || line[0] == '#') {
      continue;
    }
    if (line[0] == '[') {
      if (line.end()[-1] != ']') {
        return INI_ERROR;
      }
      sect = line.view(1, line.size() - 1);
      return INI_SECTION;
    }
    size_t sep = line.find(Chars("="));
    if (sep == (size_t)-1) {
      return INI_ERROR;
    }
    k = line.view(0, sep).trim();
    v = line.view(sep + 1, line.size()).trim();
    return INI_KEY_VALUE;
  }
  return INI_EOF;
}
