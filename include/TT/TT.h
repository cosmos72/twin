/*
 *  TT.h  --  public header for libTT
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TT_H
#define _TT_H

#include <TT/TTtypes.h>


#define TT_NOID		((ttopaque)0)
#define TT_BADID	TW_MAXTOPAQUE


#define TT_MIN2(a,b) ((a)<(b) ? (a) : (b))

extern void *(*TTAllocMem)(size_t);
extern void *(*TTReAllocMem)(void *, size_t);
extern void  (*TTFreeMem)(void *);
void *TTCloneMem(TT_CONST void *S, size_t len);
ttbyte *TTCloneStr(TT_CONST ttbyte *S);
void TTConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *));



#define TT_DECL_MAGIC(id) TW_DECL_MAGIC(id)

ttbyte TTCheckMagic(TT_CONST ttbyte id[]);

ttbyte TTInPanic(void);
ttuint TTLibraryVersion(void);

ttbyte TTOpen(TT_CONST ttbyte *target, ...);
ttbyte TTOpenV(ttuint nargs, TT_CONST ttbyte *ap0, va_list ap);
ttbyte TTOpenA(ttuint nargs, TT_CONST ttbyte **args);
void TTClose(void);
void TTCloseQuickNDirty(void);

int TTConnectionFd(void);
ttbyte TTFlush(void);
ttbyte TTTimidFlush(void);
ttbyte TTSync(void);

ttbyte TTMainLoop(void);
void TTExitMainLoop(void);

ttobj TTNew(ttfn FN);
void  TTDel(ttobj o);

ttbyte TTInstanceOf(ttfn FN, ttobj o);
ttfn TTClassOf(ttobj o);

TT_CONST ttbyte *TTClassNameOf(ttobj o);
TT_CONST ttbyte *TTGetName_ttfn(ttfn fn);
ttfn TTGetSuper_ttfn(ttfn fn);

#define TTNEW(type)		((type)TTNew(TT_CAT(TTFN_,type)))
#define TTDEL(o)		TTDel((ttobj)(o))

#define TTINSTANCEOF(type, o)	TTInstanceOf(TT_CAT(TTFN_,type), (ttobj)(o))
#define TTCLASSOF(o)		TTClassOf((ttobj)(o))

TT_FN_ATTR_CONST ttuint TTGetValueId(TT_CONST ttbyte *);
TT_FN_ATTR_CONST TT_CONST ttbyte *TTGetValueName(ttuint);

#define TTCopyMem(From, To, Size)	memcpy(To, From, Size)
#define TTMoveMem(From, To, Size)	memmove(To, From, Size)
#define TTWriteMem			memset
#define TTCmpMem			memcmp

#define TTLenStr			strlen
#define TTCmpStr			strcmp
#define TTCopyStr(From,To)		strcpy(To, From)

ttcallback TTCreateL_ttcallback(ttcomponent o, ttevent ev, ttuint flags, ttuint narg_component, ttuint narg_event, ttuint nargs, ttcallback_fn function, ...);

#endif /* _TT_H */
