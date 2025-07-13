/* Copyright (C) 2000-2019 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_ALGO_H
#define TWIN_ALGO_H

#include "stl/macros.h" // NOTHROW

#include <Tw/datatypes.h>

#include <stddef.h>

inline ldat Abs(ldat x) NOTHROW {
  return x >= 0 ? x : -x;
}
inline dat Sign(ldat x) NOTHROW {
  return x > 0 ? 1 : x < 0 ? -1 : 0;
}
inline ldat Min2(ldat x, ldat y) NOTHROW {
  return x < y ? x : y;
}
inline ldat Max2(ldat x, ldat y) NOTHROW {
  return x > y ? x : y;
}
inline ldat Max3(ldat x, ldat y, ldat z) NOTHROW {
  return Max2(Max2(x, y), z);
}
inline ldat Clamp(ldat min_x, ldat x, ldat max_x) NOTHROW {
  return x < min_x ? min_x : x > max_x ? max_x : x;
}

inline uldat Min2u(uldat x, uldat y) NOTHROW {
  return x < y ? x : y;
}

#endif /* TWIN_ALGO_H */
