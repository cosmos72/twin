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

/** value meaning `no object' */
#define TT_NOID		((ttopaque)0)
/** value meaning `invalid object' */
#define TT_BADID	TW_MAXTOPAQUE

/** the obvious meaning */
#define TT_MIN2(a,b) ((a)<(b) ? (a) : (b))
/** the obvious meaning */
#define TT_MAX2(a,b) ((a)>(b) ? (a) : (b))

/** pointer to custom malloc() function; use TTConfigMalloc() to change it */
extern void *(*TTAllocMem)(size_t);
/** pointer to custom realloc() function; use TTConfigMalloc() to change it */
extern void *(*TTReAllocMem)(void *, size_t);
/** pointer to custom free() function; use TTConfigMalloc() to change it */
extern void  (*TTFreeMem)(void *);
/** pointer to custom malloc()+memcpy() function */
void *TTCloneMem(TT_CONST void *S, size_t len);
/** pointer to custom strdup() function */
ttbyte *TTCloneStr(TT_CONST ttbyte *S);
/** change TTAllocMem(), TTReAllocMem(), TTFreeMem() function pointers */
void TTConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *));


/** define some magic data to use with TTCheckMagic() */
#define TT_DECL_MAGIC(id) TW_DECL_MAGIC(id)

/** check some magic data to ensure client and library have compatible types */
ttbyte TTCheckMagic(TT_CONST ttbyte id[]);

/** return 1 if a fatal error occurred (panic) */
ttbyte TTInPanic(void);
/** get full library version number */
ttuint TTLibraryVersion(void);

/** initialize libTT opening given display target (got from $TTDISPLAY if not specified) */
ttbyte TTOpen(TT_CONST ttbyte *target);
/** shutdown libTT */
void TTClose(void);
/** quickly shutdown libTT, does not generate `delete' events for objects deleted */
void TTCloseQuickNDirty(void);

/** get file descriptor used to communicate with target server */
int TTConnectionFd(void);
/** send all buffered data to target server, blocking if needed */
ttbyte TTFlush(void);
/** try to send all buffered data to target server, non-blocking */
ttbyte TTTimidFlush(void);
/** send all buffered data to target server and wait for all replies */
ttbyte TTSync(void);

/** enter libTT main loop; it just waits for events and dispatches them */
ttbyte TTMainLoop(void);
/** run libTT main loop once: wait for events (if `wait' is set) and dispatch available events, then return */
ttbyte TTMainLoopOnce(ttbyte wait);
/** tell libTT main loop to return */
void TTExitMainLoop(void);

/** create a new object; `FN' should be one of the `TTFN_*' types */
ttobj TTNew(ttfn FN);
/** delete given object */
void  TTDel(ttobj o);

/** get internal library size needed to store object `o' */
ttuint TTSizeOf(ttobj o);
/** return 1 if `o' can be cast to type represented by `FN' */
ttbyte TTInstanceOf(ttfn FN, ttobj o);
/** return the type of `o' (will be one of the `TTFN_*' types) */
ttfn   TTClassOf(ttobj o);
/** return the name of the type of `o' (i.e "ttbutton", ...) */
TT_CONST ttbyte *TTClassNameOf(ttobj o);

/** get internal library size needed to store objects of type `FN' */
ttuint TTGetSize_ttfn(ttfn FN);
/** get the parent type of the type `FN' */
ttfn   TTGetSuper_ttfn(ttfn fn);
/** get the name of the type `FN' (i.e "ttbutton", ...) */
TT_CONST ttbyte *TTGetName_ttfn(ttfn fn);

/** create a new object given its type name (i.e TTNEW(ttlabel)) */
#define TTNEW(type)		((type)TTNew(TT_CAT(TTFN_,type)))
/** delete given object */
#define TTDEL(o)		TTDel((ttobj)(o))

/** return 1 if `o' can be cast to type name `t' (i.e TTINSTANCEOF(tttheme, o)) */
#define TTINSTANCEOF(t, o)	TTInstanceOf(TT_CAT(TTFN_,t), (ttobj)(o))
/** return the type of `o' (will be one of the `TTFN_*' types) */
#define TTCLASSOF(o)		TTClassOf((ttobj)(o))

/** return the id to access the field `name' in an object (i.e TTGetValueId("ttwidget_x")) */
TT_FN_ATTR_CONST ttuint TTGetValueId(TT_CONST ttbyte *name);
/** return the name of a field from its `id' - this is the opposite of TTGetValueId() */
TT_FN_ATTR_CONST TT_CONST ttbyte *TTGetValueName(ttuint id);

/** wrapper around memcpy() */
#define TTCopyMem(From, To, Size)	memcpy(To, From, Size)
/** wrapper around memmove() */
#define TTMoveMem(From, To, Size)	memmove(To, From, Size)
/** wrapper around memset() */
#define TTWriteMem			memset
/** wrapper around memcmp() */
#define TTCmpMem			memcmp
/** wrapper around strlen() */
#define TTLenStr			strlen
/** wrapper around strcmp() */
#define TTCmpStr			strcmp
/** wrapper around strcpy() */
#define TTCopyStr(From,To)		strcpy(To, From)

/** create a ttcallback with given fields */
ttcallback TTCreateL_ttcallback(ttcomponent o, ttevent ev, ttuint flags, ttuint narg_component, ttuint narg_event, ttuint nargs, ttcallback_fn function, ...);

#endif /* _TT_H */
