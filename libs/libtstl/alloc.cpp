/*
 *  alloc.cpp  --  wrappers around malloc() / realloc() / free()
 *
 *  Copyright (C) 1999-2020 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "stl/alloc.h"
#include "stl/err.h"

#include <stdlib.h>
#include <cstring>

void *AllocMem(size_t len) NOTHROW {
  void *ret = NULL;
  if (len && !(ret = malloc(len))) /* malloc(0) is not portable */
    Error(NOMEMORY);
  return ret;
}

void *ReAllocMem(void *addr, size_t len) NOTHROW {
  void *ret;
  if (addr) {
    if (len) { /* realloc(addr, 0) is not portable */
      ret = realloc(addr, len);
      /* cannot use AllocMem() + CopyMem() here: we don't know mem current size */
    } else {
      FreeMem(addr);
      ret = NULL;
    }
  } else
    ret = AllocMem(len);
  return ret;
}

void FreeMem(void *addr) NOTHROW {
  if (addr) {
    free(addr);
  }
}

void *AllocMem0(size_t len) NOTHROW {
  void *ret = NULL;
  if (len && !(ret = calloc(1, len)))
    Error(NOMEMORY);
  return ret;
}

void *ReAllocMem0(void *addr, size_t old_len, size_t new_len) NOTHROW {
  void *ret;
  if (addr) {
    if (new_len) {
      if ((ret = realloc(addr, new_len))) {
        if (new_len > old_len)
          memset((char *)ret + old_len, '\0', new_len - old_len);
      } else if ((ret = AllocMem0(new_len))) {
        size_t min_len = old_len < new_len ? old_len : new_len;
        memcpy(ret, addr, min_len);
        FreeMem(addr);
      }
    } else {
      FreeMem(addr);
      ret = NULL;
    }
  } else
    ret = AllocMem0(new_len);
  return ret;
}
