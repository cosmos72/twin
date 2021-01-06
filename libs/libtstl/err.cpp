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
#include "twautoconf.h"

#include <errno.h>

#ifdef TW_HAVE_STRING_H
#include <string.h>
#else
char *strerror(int errnum);
#endif

errnum Err;
const char *Errstr;

bool Error(errnum err) {
  switch ((Err = err)) {
  case NOMEMORY:
    Errstr = "Out of memory!";
    break;
  case NOTABLES:
    Errstr = "Internal tables full!";
    break;
  case SYSERROR:
    Errstr = strerror(errno);
    break;
  default:
    break;
  }
  return false;
}
