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
#include <Tw/autoconf.h>

#include <cerrno> // errno

#if defined(CONF__DLOPEN) && defined(TW_HAVE_DLFCN_H)
#include <dlfcn.h> // dlerror()
#endif

#ifdef TW_HAVE_STRING_H
#include <string.h>
#else
char *strerror(int errnum);
#endif

errnum Err;
Chars Errstr;

bool Error(errnum err) NOTHROW {
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
