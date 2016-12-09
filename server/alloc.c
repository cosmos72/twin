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
            res = realloc(Mem, Size);
            /* cannot use AllocMem() + CopyMem() here: we don't know Mem current size */
	} else {
            FreeMem(Mem);
            res = NULL;
        }
    }
    else
        res = AllocMem(Size);
    return res;
}


void *AllocMem0(size_t ElementSize, size_t Count) {
    void *res = NULL;
    if (ElementSize || Count) {
        if (!(res = calloc(Count, ElementSize)))
            Error(NOMEMORY);
    }
    return res;
}


void *ReAllocMem0(void *Mem, size_t ElementSize, size_t OldCount, size_t NewCount) {
    void *res;
    if (Mem) {
        if (ElementSize && NewCount) {
            if ((res = realloc(Mem, ElementSize * NewCount))) {
                if (NewCount > OldCount)
                    WriteMem((byte *)res + ElementSize * OldCount, '\0', ElementSize * (NewCount - OldCount));
            } else if ((res = AllocMem0(ElementSize, NewCount))) {
                size_t MinCount = OldCount < NewCount ? OldCount : NewCount;
                CopyMem(Mem, res, ElementSize * MinCount);
                FreeMem(Mem);
            }
	} else {
            FreeMem(Mem);
            res = NULL;
        }
    }
    else
        res = AllocMem0(ElementSize, NewCount);
    return res;
}

