/*
 *  alloc.c  --  a custom version of malloc/realloc/free routines
 *               which exclusively uses mmap/munmap to get/release memory.
 *               this implementation is not particularly smart or optimized,
 *               but has a big advantage for a long-term lived program like twin:
 *               it's extremely clever at giving free()d memory back to the OS.
 *               it uses up to 32 memory pages (4k each on i386) as memory pool
 *               to reduce frequency of mmap/munmap calls.
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "util.h"

void *AllocMem(size_t Size) {
    void *res = NULL;
    if (Size) {
        if (!(res = malloc(Size)))
            Error(NOMEMORY);
    }
    return res;
}

void *ReAllocMem(void *Mem, size_t Size) {
    void *res;
    if (Mem) {
        if (Size) {
            if (!(res = realloc(Mem, Size))) {
                if ((res = AllocMem(Size))) {
                    CopyMem(Mem, res, Size);
                    FreeMem(Mem);
                }
            }
	} else {
            FreeMem(Mem);
            res = NULL;
        }
    }
    else
        res = AllocMem(Size);
    return res;
}

