/* Copyright (C) 2022 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_TYPE_TRAITS_H
#define TWIN_STL_TYPE_TRAITS_H

/* replacement for std::conditional that does not require C++11 */

template <bool CONDITION, class TYPE_IF_TRUE, class TYPE_IF_FALSE> struct conditional {
  typedef TYPE_IF_TRUE type;
};

template <class TYPE_IF_TRUE, class TYPE_IF_FALSE>
struct conditional<false, TYPE_IF_TRUE, TYPE_IF_FALSE> {
  typedef TYPE_IF_FALSE type;
};

#endif /* TWIN_STL_TYPE_TRAITS_H */
