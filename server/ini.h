/* Copyright (C) 2026 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_INI_H
#define TWIN_INI_H

#include "stl/string.h"
#include "stl/chars.h"

enum IniToken { INI_SECTION, INI_KEY_VALUE, INI_EOF, INI_ERROR };

class Ini {
public:
  explicit Ini(String &path);
  ~Ini();

  bool is_open() const {
    return file != NULL;
  }

  IniToken next();

  Chars section() const {
    return sect;
  }
  Chars key() const {
    return k;
  }
  Chars value() const {
    return v;
  }
  int linenum() const {
    return lineno;
  }

private:
  void *file;
  String buf;
  Chars sect, k, v;
  int lineno;
};

#endif /* TWIN_INI_H */
