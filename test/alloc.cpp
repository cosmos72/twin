/*
 *  alloc.c  --  wrappers around malloc() / realloc() / free()
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

#include <string.h> // memcpy(), memset()

void *AllocMem(size_t bytes) {
  return bytes ? malloc(bytes) : 0;
}

void *ReAllocMem(void *addr, size_t bytes) {
  void *ret;
  if (addr) {
    if (bytes) {
      /* cannot use AllocMem() + memcpy() here: we don't know addr current size */
      ret = realloc(addr, bytes);
    } else {
      FreeMem(addr);
      ret = NULL;
    }
  } else
    ret = AllocMem(bytes);
  return ret;
}

void FreeMem(void *addr) {
  if (addr) {
    free(addr);
  }
}

void *AllocMem0(size_t len) {
  return len ? calloc(1, len) : NULL;
}

void *ReAllocMem0(void *addr, size_t old_len, size_t new_len) {
  void *ret;
  if (addr) {
    if (new_len) {
      if ((ret = realloc(addr, new_len))) {
        if (new_len > old_len)
          memset((char *)ret + old_len, '\0', new_len - old_len);
      } else if ((ret = AllocMem0(new_len))) {
        memcpy(ret, addr, old_len < new_len ? old_len : new_len);
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
