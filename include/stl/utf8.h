/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_UTF8_H
#define TWIN_STL_UTF8_H

#include <Tw/datatypes.h> /* trune */
#include <Tw/datasizes.h> /* TW_IS_LITTLE_ENDIAN */

class utf8 {
private:
  union u4 {
     char b[4];
     uint32_t val;
  } u;
   
public:
  explicit utf8(trune rune);

  trune rune() const;

  size_t size() const;

  const char *data() const {
    return u.b;
  }
};

#endif /* TWIN_STL_UTF8_H */
