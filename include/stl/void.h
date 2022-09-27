/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_VOID_H
#define TWIN_STL_VOID_H

// replacement for void that can be instantiated
struct Void {};

template <class T> struct IsVoid {
  enum { value = false };
};

template <> struct IsVoid<Void> {
  enum { value = true };
};

#endif /* TWIN_STL_VOID_H */
