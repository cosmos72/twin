/* Copyright (C) 2000-2022 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_MACROS_H
#define TWIN_STL_MACROS_H

#if defined(__cpp_constexpr) && __cpp_constexpr >= 200704L
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif

#if defined(__cplusplus) && __cplusplus >= 201100
#define OVERRIDE override
#define NOTHROW noexcept
#else
#define OVERRIDE
#define NOTHROW throw()
#endif

#endif /* TWIN_STL_MACROS_H */
