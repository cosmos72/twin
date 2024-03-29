/*
 *  err.cpp  --  wrappers around errno / strerror()
 *
 *  Copyright (C) 1999-2020 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "stl/err.h"
#include "stl/chars.h"
#include "twconfig.h" /* for CONF_* macros */

#include <Tw/autoconf.h> /* for TW_HAVE_* macros */

#include <cerrno> // errno

#if defined(CONF__DLOPEN) && defined(TW_HAVE_DLFCN_H)
#include <dlfcn.h> // dlerror()
#endif

#ifdef TW_HAVE_STRING_H
#include <cstring>
#else
extern "C" char *strerror(int error_no);
#endif

e_errnum Err;
Chars Errstr;

bool Error(e_errnum err) NOTHROW {
  switch ((Err = err)) {
  case NOMEMORY:
    Errstr = Chars("Out of memory!");
    break;
  case NOTABLES:
    Errstr = Chars("Internal tables full!");
    break;
  case DLERROR:
#if defined(CONF__DLOPEN) && defined(TW_HAVE_DLFCN_H)
    Errstr = Chars::from_c(dlerror());
#else
    Errstr = Chars("Error loading dynamic library");
#endif
    break;
  case SYSERROR:
    Errstr = Chars::from_c(strerror(errno));
    break;
  case INVALERROR:
    Errstr = Chars("Invalid value");
    break;
  default:
    break;
  }
  return false;
}
