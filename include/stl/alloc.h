/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_ALLOC_H
#define TWIN_STL_ALLOC_H

#include <stddef.h> // size_t

#include "stl/macros.h" // NOTHROW

void *AllocMem(size_t len) NOTHROW;               // wrapper for malloc()
void *ReAllocMem(void *addr, size_t len) NOTHROW; // wrapper for realloc()
void FreeMem(void *addr) NOTHROW;                 // wrapper for free()

void *AllocMem0(size_t len) NOTHROW; // wrapper for calloc()
// wrapper for realloc() + memset()
void *ReAllocMem0(void *mem, size_t old_len, size_t new_len) NOTHROW;

#endif /* TWIN_STL_ALLOC_H */
