/*
 *  TT.h  --  internal version of public header for libTT
 *
 *  Note: you cannot include both this TT.h and <TT/TT.h>, as they conflict!
 *
 *
 *  Copyright (C) 2001,2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TT_H
#define _TT_H

/* include our internal copy of TTtypes.h instead of the public one */
#include "TTtypes.h"


/* this is a libTT internal value. Do not use it in clients! */
#define TT_MAX_ERROR	256


#define MAXID		(TW_MAXTOPAQUE >> 8)
#define magic_shift	(TW_SIZEOFTOPAQUE * 8 - 8) /* keep 8 bits available for magic */
#define magic_mask	MAXID

#define SMALLBUFF	TW_SMALLBUFF
#define BIGBUFF		TW_BIGBUFF

#define NOID		((opaque)0)
#define BADID		TW_MAXTOPAQUE
#define NOFD		((int)-1)
#define NOSLOT		TW_MAXTOPAQUE

#define TT_NOID		NOID
#define TT_BADID	BADID
#define TT_NOFD		NOFD
#define TT_NOSLOT	NOSLOT

#define TT_MIN2(a,b) ((a)<(b) ? (a) : (b))
#define TT_MAX2(a,b) ((a)>(b) ? (a) : (b))

#define TT_DECL_MAGIC(id) TW_DECL_MAGIC(id)

extern void *(*TTAllocMem)(size_t);
extern void *(*TTReAllocMem)(void *, size_t);
extern void  (*TTFreeMem)(void *);
void *TTCloneMem(TT_CONST void *S, size_t len);
ttbyte *TTCloneStr(TT_CONST ttbyte *S);
void TTConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *));


ttbyte TTCheckMagic(TT_CONST ttbyte id[]);

ttbyte TTInPanic(void);
ttuint TTLibraryVersion(void);

ttbyte TTOpen(TT_CONST ttbyte *target, ...);
void TTClose(void);
void TTCloseQuickNDirty(void);
int TTConnectionFd(void);
ttbyte TTFlush(void);
ttbyte TTTimidFlush(void);
ttbyte TTSync(void);

ttbyte TTMainLoop(void);
void TTExitMainLoop(void);

tt_obj TTNew(tt_fn FN);
void  TTDel(tt_obj o);

ttbyte TTInstanceOf(tt_fn FN, tt_obj o);
tt_fn TTClassOf(tt_obj o);

TT_CONST ttbyte *TTClassNameOf(tt_obj o);

TT_CONST ttbyte *TTGetName_ttfn(tt_fn fn);
tt_fn TTGetSuper_ttfn(tt_fn fn);

TT_FN_ATTR_CONST ttuint TTGetValueId(TT_CONST ttbyte *);
TT_FN_ATTR_CONST TT_CONST ttbyte *TTGetValueName(ttuint);

extern void *(*TTAllocMem)(size_t);
extern void *(*TTReAllocMem)(void *, size_t);
extern void  (*TTFreeMem)(void *);
extern void *TTCloneMem(TT_CONST void *, size_t);
extern ttbyte *TTCloneStr(TT_CONST ttbyte *);

#define TTCopyMem(From, To, Size)	memcpy(To, From, Size)
#define TTMoveMem(From, To, Size)	memmove(To, From, Size)
#define TTWriteMem(Mem, Char, Size)	memset(Mem, Char, Size)
#define TTCmpMem(m1, m2, Size)		memcmp(m1, m2, Size)

#define TTLenStr(S) strlen(S)
#define TTCmpStr(S1, S2) strcmp(S1, S2)
#define TTCopyStr(From,To) strcpy(To, From)




#endif /* _TT_H */
