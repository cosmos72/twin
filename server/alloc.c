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


#ifdef DEBUG_MALLOC

#include "printk.h"

#ifdef CONF__ALLOC
  /* bounds are calculated runtime */
  byte *S = (byte *)(size_t)-1;
  byte *E = (byte *)0;
#else
# if defined(__linux__) && defined(__i386__)
   /* these are good guesses for Linux/i386 with standard malloc() */
   byte *S = (byte *)0x08000000l;
   byte *E = (byte *)0x09000000l;
# else
#  error malloc() bounds not known for this architecture!
# endif
#endif

void panic_free(void *v) {
    printk("FAIL: %.8X not in %.8X - %.8X\n",
	    (size_t)v, (size_t)S, (size_t)E);
}

#endif /* DEBUG_MALLOC */




#ifdef CONF__ALLOC

/*
 #ifdef __linux__
 # include <asm/page.h>
 #endif
 */
#include <sys/types.h>
#include <sys/mman.h>


#if TW_PAGE_SIZE <= TW_MAXUDAT
   typedef udat delta;
#  define SDELTA TW_SIZEOFUDAT
#else
# if TW_PAGE_SIZE <= TW_MAXULDAT
   typedef uldat delta;
#  define SDELTA TW_SIZEOFULDAT
# else
#   error TW_PAGE_SIZE too big or not defined!
# endif
#endif

#define TW_PAGE_MASK		(~(size_t)(TW_PAGE_SIZE-1))
#define TW_PAGE_BASE(addr)	((addr)&TW_PAGE_MASK)
#define TW_PAGE_ALIGN(addr)	(((addr)+TW_PAGE_SIZE-1)&TW_PAGE_MASK)


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

#define ASSERT_TW_PAGE_DATA ((size_t)&(((block *)0)->V))

#if defined(__i386__) && TW_PAGE_SIZE == 4096 && TW_SIZEOFTOPAQUE == 4 && SDELTA == 2

/*
 * a somewhat optimized version... but it aligns data at 8 bytes,
 * not at 16 like the general version below (not a problem on Intel)
 */
#define TW_PAGE_DATA 16
#define KINDS 18
static delta  Sizes[KINDS+1] = {   1,   2,  4,  8, 16, 32,64,96,144,208,312,448,576,808,1016,1352,2032,4079,4096};
static delta Counts[KINDS+1] = {3626,1920,989,502,253,127,63,42, 28, 19, 13,  9,  7,  5,   4,   3,   2,   1,   1};
static parent Heads[KINDS];

byte InitAlloc(void) {
    return TW_PAGE_DATA == ASSERT_TW_PAGE_DATA;
}

#else

#define _ALIGN		15
#define _ALIGN1		16
#define SAFEKINDS	20 + TW_PAGE_SIZE/4096
static delta  Sizes[SAFEKINDS];
static delta Counts[SAFEKINDS];
static parent Heads[SAFEKINDS];

static delta KINDS;
static delta TW_PAGE_DATA;

static delta BuildBlock(delta s, delta size) {
    delta n, d;
    size_t nfrac[4];
    
    if (s > _ALIGN1 * 4) {
	n = (size * 8) / (s * 8 + 1); /* n * s bytes used for data, n bits for bitmap */
	if (n > 8) {
	    nfrac[0] = (size * 8) % ((s-_ALIGN1) * 8 + 1); /* nfrac wasted bits */
	    nfrac[0] *= TW_PAGE_SIZE / (s-_ALIGN1);
	    d = 0;
	    nfrac[1] = (size * 8) % (s * 8 + 1);
	    nfrac[1] *= TW_PAGE_SIZE / s;
	    if (nfrac[1] < nfrac[d]) d = 1;
	    nfrac[2] = (size * 8) % ((s+_ALIGN1) * 8 + 1);
	    nfrac[2] *= TW_PAGE_SIZE / (s+_ALIGN1);
	    if (nfrac[2] < nfrac[d]) d = 2;
	    
	    if (s > TW_PAGE_DATA * 8) {
		nfrac[3] = (size * 8) % ((s+2*_ALIGN1) * 8 + 1);
		nfrac[3] *= TW_PAGE_SIZE / (s+2*_ALIGN1);
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
    if (++KINDS >= TW_PAGE_SIZE/128)
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
	s = TW_PAGE_SIZE;
    return s;
}

byte InitAlloc(void) {
    delta s, bsize;

    TW_PAGE_DATA = (ASSERT_TW_PAGE_DATA + _ALIGN) & ~(size_t)_ALIGN;
    bsize = TW_PAGE_SIZE - TW_PAGE_DATA;
    
    s = 1;
    while (s < bsize)
	s = BuildBlock(s, bsize);
    Sizes[KINDS] = TW_PAGE_SIZE;
    Counts[KINDS] = 1;
    return TRUE;
}

#endif

#define B_DATA(base, i)		((void *)((byte *)(base) + TW_PAGE_DATA + Sizes[(base)->Kind] * (i)))
#define B_INDEX(base, mem)	(((byte *)(mem) - (byte *)(base) - TW_PAGE_DATA) / Sizes[(base)->Kind])
#define B_BITMAP(base)		((byte *)(base) + TW_PAGE_SIZE - ((base)->Max + 7) / 8)
#define B_GETBIT(bitmap, i)	(((bitmap)[(i)/8] >> ((i)&7)) & 1)
#define B_SETBIT(bitmap, i)	((bitmap)[(i)/8] |= 1 << ((i)&7))
#define B_CLRBIT(bitmap, i)	((bitmap)[(i)/8] &= ~(1 << ((i)&7)))

static byte *Highest;

void *AllocStatHighest(void) {
    return Highest;
}

INLINE void *getcore(size_t size) {
    byte *ret = (byte *)mmap(0, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);

    if (ret != (byte *)-1) { /* returned when mmap() fails */
	if (Highest < ret + size)
	    Highest = ret + size;
	return ret;
    }
    return (void *)0;
}

INLINE void dropcore(void *addr, size_t size) {
    if (Highest <= (byte *)addr + size && Highest > (byte *)addr)
	Highest = addr;
    
    munmap(addr, size);
}
    


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
#define  LOW_CACHE  8
#define  AVG_CACHE 16
#define HIGH_CACHE 24
#define  MAX_CACHE 32
static block *Cache[MAX_CACHE];

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

/* add given blocks to the cache, and if cache overflows
 * return highest-addresses blocks to the OS */
static void DropBs(block *B, size_t len) {
    delta pos, d;
    while (len >= TW_PAGE_SIZE) {
	if (!CSize) {
	    CSize++;
	    CFirst = CLast = MAX_CACHE/2;
	    Cache[CFirst] = B;
	} else if (CSize == MAX_CACHE) {
	    if (B > Cache[CLast]) {
		dropcore(B, len);
		return;
	    } else {
		dropcore(Cache[CLast], TW_PAGE_SIZE);
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
	B = (block *)((byte *)B + TW_PAGE_SIZE);
	len -= TW_PAGE_SIZE;
    }
}


/* try to get blocks from the cache, else ask more blocks from the OS */
static void *GetBs(size_t len) {
    block *B;
    size_t xlen = 0;
    
    /* try to get blocks from the cache */
    if (CSize * TW_PAGE_SIZE >= len) {
	if (len == TW_PAGE_SIZE) {
	    B = Cache[CFirst];
	    CFirst++;
	    CSize--;
	    return B;
	} else {
	    size_t got = TW_PAGE_SIZE;
	    delta i, n;
	    B = Cache[n = CFirst];
	    for (i = CFirst+1; got < len && i <= CLast; i++) {
		if ((byte *)B + got == (byte *)Cache[i])
		    got += TW_PAGE_SIZE;
		else {
		    B = Cache[n = i];
		    got = TW_PAGE_SIZE;
		}
	    }
	    if (got >= len) {
		got /= TW_PAGE_SIZE;
		MoveMem(Cache + CFirst, Cache + CFirst + got, (n - CFirst) * sizeof(block *));
		CFirst += got;
		CSize -= got;
		return B;
	    }
	}
    }

    /* try to ask blocks from the OS in medium-sized chunks */
    if (CSize < LOW_CACHE)
	xlen = TW_PAGE_SIZE * (AVG_CACHE - CSize);
    
    if (!(B = getcore(len + xlen)))
	return B;

    if (xlen)
	DropBs((block *)((byte *)B + len), xlen);
    
    return B;
}

INLINE void *CreateB(delta kind) {
    block *B;
    if ((B = GetBs(TW_PAGE_SIZE))) {
	B->Kind = kind;
	B->Max = Counts[kind];
	B->Top = B->Bottom = 0;
	InsertB(B, &Heads[kind]);
    }
    return B;
}

INLINE void DeleteB(block *B) {
    RemoveB(B, &Heads[B->Kind]);
    DropBs(B, TW_PAGE_SIZE);
}

#ifdef DEBUG_MALLOC
/* this is ok for Linux/i386 */

#define POISON_DATA ((byte *)0xc1234567l)
#define UNPOISON_DATA ((byte *)0xd6543210l)

static void do_poison(byte *v, size_t len, byte *p) {
    /* assume sizeof(void *) is a power of 2 */
    size_t d = len & (sizeof(void *) - 1);
    
    len -= d;
    while (len > 0) {
	CopyMem(&p, v, sizeof(void *));
	p++;
	v += sizeof(void *);
	len -= sizeof(void *);
    }
    CopyMem(&p, v, d);
}

#define POISON(v, len) do_poison(v, len, POISON_DATA)
#define UNPOISON(v, len) do_poison(v, len, UNPOISON_DATA)

#endif

void *AllocMem(size_t len) {
#ifdef DEBUG_MALLOC
    byte *ret;
#endif
    
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
#ifdef DEBUG_MALLOC
	    ret = B_DATA(B, i);
	    if (S > ret) S = ret;
	    if (E < ret) E = ret;
	    UNPOISON(ret, Sizes[B->Kind]);
#endif
	    return B_DATA(B, i);
	}
    } else if (len > Sizes[KINDS-1]) {
	block *B;
	len = TW_PAGE_ALIGN(len + TW_PAGE_DATA);
	if ((B = GetBs(len))) {
	    B->Prev = B->Next = (void *)0;
	    B->Kind = KINDS;
	    B->Len = len;
#ifdef DEBUG_MALLOC
	    ret = B_DATA(B, 0);
	    if (S > ret) S = ret;
	    if (E < ret) E = ret;
	    UNPOISON(ret, B->Len - (ret - (byte *)B));
#endif
	    return B_DATA(B, 0);
	}
    }
    Error(NOMEMORY);
    return (void *)0;
}

void FreeMem(void *Mem) {
    byte *bitmap;
    block *B;
    delta i;

#ifdef DEBUG_MALLOC
    if (FAIL(Mem))
	return;
#endif
    
    if ((B = (block *)TW_PAGE_BASE((size_t)Mem)) && B->Kind <= KINDS &&
	((i = B_INDEX(B, Mem)), Mem == B_DATA(B, i))) {


	if (B->Kind < KINDS) {
	    if (i < B->Top && ((bitmap = B_BITMAP(B)), B_GETBIT(bitmap, i))) {
		
#ifdef DEBUG_MALLOC
		POISON(Mem, Sizes[B->Kind]);
#endif
		
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
		return;
	    }
	} else if (B->Kind == KINDS) {
	    if (Mem == B_DATA(B, 0)) {
#ifdef DEBUG_MALLOC
		POISON(Mem, B->Len - ((byte *)Mem - (byte *)B));
#endif
		DropBs(B, B->Len);
		return;
	    }
	}
    }
#ifdef DEBUG_MALLOC
    printk("FAIL: %.8X is not a valid pointer\n", (size_t)Mem);
#endif    
}

void *ReAllocMem(void *Mem, size_t newSize) {
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
    
    if ((B = (block *)TW_PAGE_BASE((size_t)Mem)) && B->Kind <= KINDS &&
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
		Error(NOMEMORY);
	    }
	} else if (B->Kind == KINDS) {
	    if (Mem == B_DATA(B, 0)) {
		newSize = TW_PAGE_ALIGN(newSize + TW_PAGE_DATA);
		if (newSize == (oldSize = B->Len))
		    return Mem;
		if (newSize < oldSize) {
		    B->Len = newSize;
		    DropBs((block *)((byte *)B + newSize), oldSize - newSize);
		    return Mem;
		}
		if ((A = GetBs(newSize))) {
		    if ((byte *)B + oldSize == (byte *)A) {
			/* this is sheer luck ! */
			DropBs((block *)((byte *)A + newSize - oldSize), oldSize);
			B->Len = newSize;
			return Mem;
		    }
		    CopyMem(B, A, oldSize);
		    A->Len = newSize;
		    DropBs(B, oldSize);
		    return B_DATA(A, 0);
		}
		Error(NOMEMORY);
	    }
	}
    }
    return newMem;
}

#else /* !CONF__ALLOC */

void *AllocMem(size_t Size) {
    byte Error(udat Code_Error);
    void *res;
    if (!(res = malloc(Size)))
	Error(NOMEMORY);
    return res;
}

#endif /* CONF__ALLOC */


