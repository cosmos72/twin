/*
 *  TTmem.c  --  memory and string functions for libTT
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

#include <Tw/Tw_defs.h>

#include "TT.h"

#include "mutex.h"

#include "TTextern.h"

#ifdef CONF__UNICODE
# define Str2Unicode(c) Tutf_CP437_to_UTF_16[c]
#else
# define Str2Unicode(c) (c)
#endif

/*
 * TT<op>Mem functions
 */

#ifdef CONF_SOCKET_PTHREADS
TH_R_MUTEX_HELPER_DEFS(static);
#endif

void *(*TTAllocMem)(ttopaque) = malloc;
void *(*TTReallocMem)(void *, ttopaque) = realloc;
void  (*TTFreeMem)(void *) = free;

/**
 * creates a copy of a chunk of memory
 */
void *TTCloneMem(TT_CONST void *S, ttopaque len) {
    void *T;
    if (S && (T = TTAllocMem(len)))
	return TTCopyMem(S, T, len);
    return NULL;
}

/**
 * sets the function to call to allocate/realloc/free memory;
 * can be called only if no connections to server are open.
 */
void TTConfigAllocMem(void *(*my_malloc)(ttopaque),
		      void *(*my_realloc)(void *, ttopaque),
		      void  (*my_free)(void *)) {
    
    /* do not mess with TTD.refcount */
    th_r_mutex_lock(mutex);
    if (!TTD.OpenFlag) {
	if (my_malloc && my_realloc && my_free) {
	    TTAllocMem = my_malloc;
	    TTReallocMem = my_realloc;
	    TTFreeMem = my_free;
	} else {
	    TTAllocMem = malloc;
	    TTReallocMem = realloc;
	    TTFreeMem = free;
	}
    }
    th_r_mutex_unlock(mutex);
}


/*
 * TT<op>Str* functions
 */

ttopaque TTLenStr(TT_CONST ttbyte *s) {
    return s ? strlen(s) : 0;
}
ttopaque TTLenStr1(TT_CONST ttbyte *s) {
    return s ? 1 + strlen(s) : 0;
}
#define TTLenStr1(s) ((s) ? 1 + strlen(s) : 0)


#if TT_SIZEOF_ttfont != 1
ttfont *TTCopyStrL2Font(TT_CONST ttbyte *s, ttfont *dest, ttopaque len) {
    ttfont *tmp = dest;
    while (len--)
	*tmp++ = Str2Unicode(*s++);
    return dest;
}

ttfont *TTCopyStr2Font(TT_CONST ttbyte *s, ttfont *dest) {
    return TTCopyStrL2Font(s, dest, TTLenStr1(s));
}
#endif


ttattr *TTCopyStrL2Attr(TT_CONST ttbyte *s, ttattr *dest, ttopaque len, ttcol c) {
    ttfont f;
    ttattr *tmp = dest;
    
    while (len--) {
	f = Str2Unicode(*s++);
	*tmp++ = TTAttr(c, f);
    }
    return dest;
}

ttattr *TTCopyStr2Attr(TT_CONST ttbyte *s, ttattr *dest, ttcol c) {
    return TTCopyStrL2Attr(s, dest, TTLenStr1(s), c);
}

ttbyte *TTCloneStr(TT_CONST ttbyte *s) {
    if (s)
	return TTCloneMem(s, TTLenStr1(s));
    return NULL;
}

#if TT_SIZEOF_ttfont != 1
ttfont *TTCloneStrL2Font(TT_CONST ttbyte *s, ttopaque len) {
    ttfont *dest;
    
    if (s && (dest = TTAllocMem(len * sizeof(ttfont))))
	return TTCopyStrL2Font(s, dest, len);
    return NULL;
}
ttfont *TTCloneStr2Font(TT_CONST ttbyte *s) {
    return TTCloneStrL2Font(s, TTLenStr1(s));
}
#endif

ttattr *TTCloneStrL2Attr(TT_CONST ttbyte *s, ttopaque len, ttcol c) {
    ttattr *attr;
    
    if (len && (attr = (ttattr *)TTAllocMem(len * sizeof(ttfont))))
	return TTCopyStrL2Attr(s, attr, len, c);
    return NULL;
}

ttattr *TTCloneStr2Attr(TT_CONST ttbyte *s, ttcol c) {
    return TTCloneStrL2Attr(s, TTLenStr1(s), c);
}


/*
 * TT<op>Font* functions
 */

#if TT_SIZEOF_ttfont != 1
ttopaque TTLenFont(TT_CONST ttfont *f) {
    ttopaque len = 0;
    if (f)
	while (*f++)
	    len++;
    return len;
}
ttopaque TTLenFont1(TT_CONST ttfont *f) {
    return f ? 1 + TTLenFont(f) : 0;
}
#define TTLenFont1(f) ((f) ? 1 + TTLenFont(f) : 0)


ttfont *TTCopyFont(TT_CONST ttfont *f, ttfont *dest) {
    return TTCopyMem(f, dest, TTLenFont1(f));
}
ttattr *TTCopyFontL2Attr(TT_CONST ttfont *f, ttattr *dest, ttopaque len, ttcol c) {
    ttattr *tmp = dest;
    ttfont a;
    while (len--) {
	a = *f++;
	*tmp++ = TTAttr(c, a);
    }
    return dest;
}
ttattr *TTCopyFont2Attr(TT_CONST ttfont *f, ttattr *dest, ttcol c) {
    return TTCopyFontL2Attr(f, dest, TTLenFont1(f), c);
}

ttfont *TTCloneFont(TT_CONST ttfont *s) {
    return TTCloneFontL(s, TTLenFont1(s));
}
ttattr *TTCloneFontL2Attr(TT_CONST ttfont *f, ttopaque len, ttcol c) {
    ttattr *dest;
    if (f && (dest = TTAllocMem(len * sizeof(ttattr))))
	return TTCopyFontL2Attr(f, dest, len, c);
    return NULL;
}
ttattr *TTCloneFont2Attr(TT_CONST ttfont *f, ttcol c) {
    return TTCloneFontL2Attr(f, TTLenFont1(f), c);
}
#endif
