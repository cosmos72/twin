
#ifndef _TT_MEM_H
#define _TT_MEM_H

#include <TT/TTtypes.h>



/*
 * TT<op>Mem functions
 */


/** pointer to custom malloc() function; use TTConfigAllocMem() to change it */
extern void *(*TTAllocMem)(size_t);

/** pointer to custom realloc() function; use TTConfigAllocMem() to change it */
extern void *(*TTReallocMem)(void *, size_t);

/** pointer to custom free() function; use TTConfigAllocMem() to change it */
extern void  (*TTFreeMem)(void *);

/** change TTAllocMem(), TTReAllocMem(), TTFreeMem() function pointers */
void TTConfigAllocMem(void *(*my_malloc)(size_t),
		      void *(*my_realloc)(void *, size_t),
		      void  (*my_free)(void *));


/** wrapper around memcpy() */
#define TTCopyMem(From, To, Size)	memcpy(To, From, Size)
/** wrapper around memmove() */
#define TTMoveMem(From, To, Size)	memmove(To, From, Size)
/** wrapper around memset() */
#define TTWriteMem			memset
/** wrapper around memcmp() */
#define TTCmpMem			memcmp

/** custom malloc()+memcpy() function */
void *TTCloneMem(TT_CONST void *M, ttopaque len);





/*
 * TT<op>Str* functions
 */


/** wrapper around strlen() */
ttopaque TTLenStr(TT_CONST ttbyte *s);
/** 1+strlen(s) */
ttopaque TTLenStr1(TT_CONST ttbyte *s);

/** wrapper around strcpy() */
#define TTCopyStr(From,To)		strcpy(To, From)

/** wrapper around memcpy */
#define TTCopyStrL			TTCopyMem


#if TT_SIZEOF_ttfont == 1
# define TTCopyStr2Font			TTCopyStr
# define TTCopyStrL2Font		TTCopyStrL
#else
/** copy from ttbyte[] to ttfont[] */
ttfont *TTCopyStr2Font(TT_CONST ttbyte *s, ttfont *dest);

/** copy from ttbyte[] to ttfont[] */
ttfont *TTCopyStrL2Font(TT_CONST ttbyte *s, ttfont *dest, ttopaque len);
#endif

/** copy from ttbyte[] to ttattr[] */
ttattr *TTCopyStr2Attr(TT_CONST ttbyte *s, ttattr *dest, ttcol c);

/** copy from ttbyte[] to ttattr[] */
ttattr *TTCopyStrL2Attr(TT_CONST ttbyte *s, ttattr *dest, ttopaque len, ttcol c);


/** custom strdup() function */
#define TTCloneStrL			TTCloneMem
ttbyte *TTCloneStr(TT_CONST ttbyte *S);


#if TT_SIZEOF_ttfont == 1
# define TTCloneStrL2Font		TTCloneStrL
# define TTCloneStr2Font  		TTCloneStr
#else
ttfont *TTCloneStrL2Font(TT_CONST ttbyte *s, ttopaque len);
ttfont *TTCloneStr2Font(TT_CONST ttbyte *s);
#endif

ttattr *TTCloneStrL2Attr(TT_CONST ttbyte *s, ttopaque len, ttcol c);
ttattr *TTCloneStr2Attr(TT_CONST ttbyte *s, ttcol c);



/** wrapper around strcmp() */
#define TTCmpStr			strcmp



/*
 * TT<op>Font* functions
 */

#if TT_SIZEOF_ttfont == 1
# define TTLenFont			TTLenStr
# define TTLenFont1			TTLenStr1
# define TTCopyFont			TTCopyStr
# define TTCopyFontL			TTCopyStrL
# define TTCopyFont2Attr		TTCopyStr2Attr
# define TTCopyFontL2Attr		TTCopyStrL2Attr
# define TTCloneFont			TTCloneStr
# define TTCloneFontL			TTCloneStrL
# define TTCloneFont2Attr		TTCloneStr2Attr
# define TTCloneFontL2Attr		TTCloneStrL2Attr
#else
ttopaque TTLenFont(TT_CONST ttfont *f);
ttopaque TTLenFont1(TT_CONST ttfont *f);
ttfont * TTCopyFont(TT_CONST ttfont *f, ttfont *dest);
# define TTCopyFontL(f, dest, len)	TTCopyMem((f), (dest), (len)*TT_SIZEOF_ttfont)
ttattr * TTCopyFont2Attr(TT_CONST ttfont *f, ttattr *dest, ttcol c);
ttattr * TTCopyFontL2Attr(TT_CONST ttfont *f, ttattr *dest, ttopaque len, ttcol c);
ttfont * TTCloneFont(TT_CONST ttfont *f);
# define TTCloneFontL(f, len)		TTCloneMem((f), (len)*TT_SIZEOF_ttfont)
ttattr * TTCloneFont2Attr(TT_CONST ttfont *f, ttcol c);
ttattr * TTCloneFontL2Attr(TT_CONST ttfont *f, ttopaque len, ttcol c);
#endif

# define TTCloneAttr(a, len)		TTCloneMem((a), (len)*TT_SIZEOF_ttattr)

#define TTAttr(col, font)	HWATTR(col, font)
#define TTGetCol_ttattr(attr)	HWCOL(attr)
#define TTGetFont_ttattr(attr)	HWFONT(attr)

#define TTCol(fg, bg)		COL(fg, bg)
#define TTGetFg_ttcol(col)	COLFG(col)
#define TTGetBg_ttcol(col)	COLBG(col)

#endif /* _TT_MEM_H */
