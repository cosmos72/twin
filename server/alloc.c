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
#include "sizes.h"

#include <asm/page.h>
#include <sys/types.h>
#include <sys/mman.h>

#if BLOCK_SIZE <= _MAXUDAT
   typedef udat delta;
#  define SDELTA _SIZEOFUDAT
#else
# if BLOCK_SIZE <= _MAXULDAT
   typedef uldat delta;
#  define SDELTA _SIZEOFULDAT
# else
#   error BLOCK_SIZE too big or not defined!
# endif
#endif

#define BLOCK_MASK		(~(size_t)(BLOCK_SIZE-1))
#define BLOCK_BASE(addr)	((addr)&BLOCK_MASK)
#define BLOCK_ALIGN(addr)	(((addr)+BLOCK_SIZE-1)&BLOCK_MASK)

#define getcore(size) (void *)mmap(0, (size), PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0)
#define dropcore(addr, size) munmap((void *)(addr), (size))
#define NOCORE ((void *)-1) /* returned when getcore() fails */

typedef struct block {
    struct block *Prev, *Next;
    union {
	struct {
	    delta Kind, Max, Bottom, Top;
	} S;
	struct {
	    delta Kind;
	    size_t Len;
	} B;
    } SB;
    void *V;
} block;

#define Kind	SB.S.Kind
#define Max	SB.S.Max
#define Bottom	SB.S.Bottom
#define Top	SB.S.Top
#define Len	SB.B.Len

typedef struct parent {
    block *First, *Last;
} parent;

#define ASSERT_BLOCK_DATA ((size_t)&(((block *)0)->V))

#if defined(__i386__) && BLOCK_SIZE == 4096 && _SIZEOFVOIDP == 4 && SDELTA == 2

/*
 * a somewhat optimized version... but it aligns data at 8 bytes,
 * not at 16 like the general version below (not a problem on Intel)
 */
#define BLOCK_DATA 16
#define KINDS 17
static delta  Sizes[KINDS+1] = {   1,   2,  4,  8, 16, 24,48,96,184,288,448,576,808,1016,1352,2032,4079,4096};
static delta Counts[KINDS+1] = {3626,1920,989,502,253,169,84,42, 22, 14,  9,  7,  5,   4,   3,   2,   1,   1};
static parent Heads[KINDS];

byte InitAlloc(void) {
    return BLOCK_DATA == ASSERT_BLOCK_DATA;
}

#else

#define _ALIGN		15
#define _ALIGN1		16
#define SAFEKINDS	20 + BLOCK_SIZE/4096
static delta  Sizes[SAFEKINDS];
static delta Counts[SAFEKINDS];
static parent Heads[SAFEKINDS];

static delta KINDS;
static delta BLOCK_DATA;

static delta BuildBlock(delta s, delta size) {
    delta n, d;
    size_t nfrac[4];
    
    if (s > _ALIGN1 * 4) {
	n = (size * 8) / (s * 8 + 1); /* n * s bytes used for data, n bits for bitmap */
	if (n > 8) {
	    nfrac[0] = (size * 8) % ((s-_ALIGN1) * 8 + 1); /* nfrac wasted bits */
	    nfrac[0] *= BLOCK_SIZE / (s-_ALIGN1);
	    d = 0;
	    nfrac[1] = (size * 8) % (s * 8 + 1);
	    nfrac[1] *= BLOCK_SIZE / s;
	    if (nfrac[1] < nfrac[d]) d = 1;
	    nfrac[2] = (size * 8) % ((s+_ALIGN1) * 8 + 1);
	    nfrac[2] *= BLOCK_SIZE / (s+_ALIGN1);
	    if (nfrac[2] < nfrac[d]) d = 2;
	    
	    if (s > BLOCK_DATA * 8) {
		nfrac[3] = (size * 8) % ((s+2*_ALIGN1) * 8 + 1);
		nfrac[3] *= BLOCK_SIZE / (s+2*_ALIGN1);
		if (nfrac[3] < nfrac[d]) d = 3;
	    }
	    s += (d-1)*_ALIGN1;
	} else {
	    s = ((size * 8) / n - 1) / 8;
	    s &= ~_ALIGN;
	}
    }
    
    Sizes[KINDS] = s;
    Counts[KINDS] = n = (size * 8) / (s * 8 + 1);
    if (++KINDS >= BLOCK_SIZE/128)
	return FALSE;
    
    if (s * s < size * 2)
	s <<= 1;
    else if (n > 7) {
	s += s >> 1;
	s &= ~_ALIGN;
    } else if (n > 1) {
	s = ((size * 8) / (n-1) - 1) / 8;
	s &= ~_ALIGN;
    } else
	s = BLOCK_SIZE;
    return s;
}

byte InitAlloc(void) {
    delta s, bsize;

    BLOCK_DATA = (ASSERT_BLOCK_DATA + _ALIGN) & ~(size_t)_ALIGN;
    bsize = BLOCK_SIZE - BLOCK_DATA;
    
    s = 1;
    while (s < bsize)
	s = BuildBlock(s, bsize);
    Sizes[KINDS] = BLOCK_SIZE;
    Counts[KINDS] = 1;
    return TRUE;
}

#endif

#define B_DATA(base, i)		((void *)((char *)(base) + BLOCK_DATA + Sizes[(base)->Kind] * (i)))
#define B_INDEX(base, mem)	(((char *)(mem) - (char *)(base) - BLOCK_DATA) / Sizes[(base)->Kind])
#define B_BITMAP(base)		((char *)(base) + BLOCK_SIZE - ((base)->Max + 7) / 8)
#define B_GETBIT(bitmap, i)	(((bitmap)[(i)/8] >> ((i)&7)) & 1)
#define B_SETBIT(bitmap, i)	((bitmap)[(i)/8] |= 1 << ((i)&7))
#define B_CLRBIT(bitmap, i)	((bitmap)[(i)/8] &= ~(1 << ((i)&7)))



INLINE delta SearchKind(delta size) {
    delta low = 0, up = KINDS-1, test;
    while (up - low > 1) {
	test = (up + low) / 2;
	if (Sizes[test] > size)
	    up = test;
	else if (Sizes[test] < size)
	    low = test;
	else
	    return test;
    }
    return up;
}

static void InsertB(block *B, parent *P) {
    B->Prev = (block *)0;
    if ((B->Next = P->First))
	P->First->Prev = B;
    P->First = B;
    if (!(P->Last))
	P->Last = B;
}

static void InsertLastB(block *B, parent *P) {
    B->Next = (block *)0;
    if ((B->Prev = P->Last))
	P->Last->Next = B;
    P->Last = B;
    if (!(P->First))
	P->First = B;
}

INLINE void RemoveB(block *B, parent *P) {
    if (B->Prev)
	B->Prev->Next=B->Next;
    else
	P->First=B->Next;
    
    if (B->Next)
	B->Next->Prev=B->Prev;
    else
	P->Last=B->Prev;
    
    B->Prev=B->Next=(block *)0;
}

static delta CFirst, CLast, CSize;
#define MAX_CACHE 32
static block *Cache[MAX_CACHE];

static void *GetBs(size_t len) {
    block *B;
    if (len == BLOCK_SIZE && CSize) {
	B = Cache[CFirst];
	CFirst++;
	CSize--;
	return B;
    }
    if ((B = getcore(len)) != NOCORE)
	return B;
    return (void *)0;
}

INLINE delta SearchB(block *B, delta low, delta up) {
    delta test;
    while (up - low > 1) {
	test = (up + low) / 2;
	if (Cache[test] > B)
	    up = test;
	else if (Cache[test] < B)
	    low = test;
	else
	    return test;
    }
    return up;
}

static void DropBs(block *B, size_t len) {
    delta pos, d;
    while (len >= BLOCK_SIZE) {
	if (!CSize) {
	    CSize++;
	    CFirst = CLast = MAX_CACHE/2;
	    Cache[CFirst] = B;
	} else if (CSize == MAX_CACHE) {
	    if (B > Cache[CLast]) {
		dropcore(B, len);
		return;
	    } else {
		dropcore(Cache[CLast], BLOCK_SIZE);
		if (B < Cache[CFirst])
		    pos = CFirst;
		else
		    pos = SearchB(B, CFirst, CLast-1);
		MoveMem(Cache+pos, Cache+pos+1, (CLast-pos) * sizeof(block *));
		Cache[pos] = B;
	    }
	} else { /* if (CSize < MAX_CACHE) */
	    if (B > Cache[CLast])
		pos = CLast + 1;
	    else if (B < Cache[CFirst])
		pos = CFirst;
	    else
		pos = SearchB(B, CFirst, CLast);
	    
	    if (pos <= (CFirst + CLast) / 2 && CFirst == 0)
		d = MAX_CACHE/2 - CSize/2; /* always d >= 1 */
	    else if (pos > (CFirst + CLast) / 2 && CLast == MAX_CACHE - 1)
		d = (MAX_CACHE - CSize)/2; /* always d + CSize < MAX_CACHE */
	    else
		d = CFirst;
	    
	    if (d != CFirst) {
		MoveMem(Cache + CFirst, Cache + d, CSize * sizeof(block *));
		/* it is possible that d - CFirst < 0, but
		 * x += -n works even if x is unsigned since it is
		 * (mod MAXDELTA+1) unsigned arithmetic
		 */
		pos += d - CFirst;
		CLast += d - CFirst;
		CFirst += d - CFirst;
	    }
	    if (pos <= (CFirst + CLast) / 2) {
		MoveMem(Cache + CFirst, Cache + CFirst - 1, (pos - CFirst) * sizeof(block *));
		CFirst--;
		pos--;
	    } else {
		MoveMem(Cache + pos, Cache + pos + 1, (CLast + 1 - pos) * sizeof(block *));
		CLast++;
	    }
	    Cache[pos] = B;
	    CSize++;
	}
	B = (block *)((char *)B + BLOCK_SIZE);
	len -= BLOCK_SIZE;
    }
}

INLINE void *CreateB(delta kind) {
    block *B;
    if ((B = GetBs(BLOCK_SIZE))) {
	B->Kind = kind;
	B->Max = Counts[kind];
	B->Top = B->Bottom = 0;
	InsertB(B, &Heads[kind]);
    }
    return B;
}

INLINE void DeleteB(block *B) {
    RemoveB(B, &Heads[B->Kind]);
    DropBs(B, BLOCK_SIZE);
}

void *AllocMem(size_t len) {
    if (len > 0 && len <= Sizes[KINDS-1]) {
	byte *bitmap;
	delta i = SearchKind(len), j;
	block *B = Heads[i].First;
	
	if ((B && B->Bottom < B->Max) || (B = CreateB(i))) {
	    i = B->Bottom;
	    bitmap = B_BITMAP(B);
	    B_SETBIT(bitmap, i);
	
	    if (B->Top <= i)
		B->Top = i + 1;
	    for (j = B->Bottom + 1; j < B->Top; j++)
		if (!B_GETBIT(bitmap, j))
		    break;
	    if ((B->Bottom = j) == B->Max) {
		/* it's full, move to list end */
		RemoveB(B, &Heads[B->Kind]);
		InsertLastB(B, &Heads[B->Kind]);
	    }
	    return B_DATA(B, i);
	}
    } else if (len > Sizes[KINDS-1]) {
	block *B;
	len = BLOCK_ALIGN(len + BLOCK_DATA);
	if ((B = GetBs(len))) {
	    B->Prev = B->Next = (void *)0;
	    B->Kind = KINDS;
	    B->Len = len;
	    return B_DATA(B, 0);
	}
    }
    return (void *)0;
}

void FreeMem(void *Mem) {
    byte *bitmap;
    block *B;
    delta i;
    
    if ((B = (block *)BLOCK_BASE((size_t)Mem)) && B->Kind <= KINDS &&
	((i = B_INDEX(B, Mem)), Mem == B_DATA(B, i))) {
	
	if (B->Kind < KINDS) {
	    if (i < B->Top && ((bitmap = B_BITMAP(B)), B_GETBIT(bitmap, i))) {
		B_CLRBIT(bitmap, i);
		if (B->Bottom > i)
		    B->Bottom = i;
		for (i = B->Top - 1; i > B->Bottom; i--)
		    if (B_GETBIT(bitmap, i))
			break;
		B->Top = (i == B->Bottom) ? i : i + 1;
		
		if (B->Top == 0)
		    DeleteB(B);
		else if (B->Top == B->Max - 1) {
		    /* it's no longer full, move to list start */
		    RemoveB(B, &Heads[B->Kind]);
		    InsertB(B, &Heads[B->Kind]);
		}
	    }
	} else if (B->Kind == KINDS) {
	    if (Mem == B_DATA(B, 0)) {
		DropBs(B, B->Len);
	    }
	}
    }
}

void *ReAllocMem(void *Mem, uldat newSize) {
    void *newMem = (void *)0;
    byte *bitmap;
    block *A, *B;
    delta i;
    size_t oldSize;
    
    if (!newSize) {
	if (Mem)
	    FreeMem(Mem);
	return newMem;
    }
    if (!Mem)
	return AllocMem(newSize);
    
    if ((B = (block *)BLOCK_BASE((size_t)Mem)) && B->Kind <= KINDS &&
	((i = B_INDEX(B, Mem)), Mem == B_DATA(B, i))) {
	/*
	 * return Mem if it still fits in the original location,
	 * do a AllocMem() + CopyMem() + FreeMem() cycle otherwise
	 */
	if (B->Kind < KINDS) {
	    if (i < B->Max && ((bitmap = B_BITMAP(B)), B_GETBIT(bitmap, i))) {
		if (newSize <= (oldSize = Sizes[B->Kind]))
		    return Mem;
		if ((newMem = AllocMem(newSize))) {
		    CopyMem(Mem, newMem, oldSize);
		    FreeMem(Mem);
		    return newMem;
		}
	    }
	} else if (B->Kind == KINDS) {
	    if (Mem == B_DATA(B, 0)) {
		newSize = BLOCK_ALIGN(newSize + BLOCK_DATA);
		if (newSize == (oldSize = B->Len))
		    return Mem;
		if (newSize < oldSize) {
		    B->Len = newSize;
		    DropBs((block *)((char *)B + newSize), oldSize - newSize);
		    return Mem;
		}
		if ((A = GetBs(newSize))) {
		    if ((char *)B + oldSize == (char *)A) {
			/* this is sheer luck ! */
			DropBs((block *)((char *)A + newSize - oldSize), oldSize);
			B->Len = newSize;
			return Mem;
		    }
		    CopyMem(B, A, oldSize);
		    A->Len = newSize;
		    DropBs(B, oldSize);
		    return B_DATA(A, 0);
		}
	    }
	}
    }
    return newMem;
}
