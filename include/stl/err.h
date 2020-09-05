/*
 *  alloc.c  --  wrappers around malloc() / realloc() / free()
 *
 *  Copyright (C) 1999-2000,2016 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef TWIN_STL_ERR_H
#define TWIN_STL_ERR_H

typedef enum e_errnum : unsigned char {
  SUCCESS = 0,
  NOMEMORY = 1,
  NOTABLES = 2,
  DLERROR = 3,
  SYSERROR = 4,
} errnum;

extern errnum Err;
extern const char *Errstr;
bool Error(errnum err); // always returns false

#endif /* TWIN_STL_ERR_H */
