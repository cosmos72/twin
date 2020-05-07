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
#ifndef _TWIN_ALLOC_H
#define _TWIN_ALLOC_H

#include "stl/alloc.h"

#include <Tw/datatypes.h>
#include <string.h> // memcpy(), memmove()

extern udat Errno;
extern const char *Errstr;
byte Error(udat Errno);

/* memory allocation. these function call Error(NOMEMORY) on failure */

void *AllocMem(size_t len);               /* wrapper for malloc()  */
void *ReAllocMem(void *addr, size_t len); /* wrapper for realloc() */
void FreeMem(void *addr);                 /* wrapper for free()    */

void *AllocMem0(size_t len);                                  /* wrapper for calloc() */
void *ReAllocMem0(void *mem, size_t old_len, size_t new_len); /* wrapper for realloc() + memset() */

/* inline/define stuff: */

#define CopyMem(From, To, len) memcpy(To, From, len)
#define MoveMem(From, To, len) memmove(To, From, len)

void *CloneMem(const void *From, uldat len);
char *CloneStr(const char *s);
char *CloneStrL(const char *s, uldat len);
char **CloneStrList(char **s);
trune *CloneStr2TRune(const char *s, uldat len);

#endif /* _TWIN_ALLOC_H */
