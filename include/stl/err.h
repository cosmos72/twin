/*
 *  err.h  --  utilities to store/retrieve global error code and message
 *
 *  Copyright (C) 1999-2000,2016,2022 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef TWIN_STL_ERR_H
#define TWIN_STL_ERR_H

#include "stl/fwd.h"

// keep the name 'errnum_e' as it's part of the ABI:
// C++ mangled name for function 'bool Error(e_errnum err)' below contains it
typedef enum errnum_e /*: unsigned*/ {
  SUCCESS = 0,
  NOMEMORY = 1,   //< memory allocation failed
  NOTABLES = 2,   //< internal tables/buffers full
  DLERROR = 3,    //< error loading shared library at runtime
  SYSERROR = 4,   //< libc/OS error, see errno.
  INVALERROR = 5, //< invalid value passed to function/method
} e_errnum;

extern e_errnum Err;
extern Chars Errstr;
bool Error(e_errnum err) NOTHROW; // always returns false

#endif /* TWIN_STL_ERR_H */
