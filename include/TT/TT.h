/*
 *  TT.h  --  header for libTT
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

#include <Tw/Twtypes.h>
#include <Tw/Tw.h>

typedef void (*ttfn_repaint)(void *,dat,dat,dat,dat);
    
#include <TT/compiler.h>
#include <TT/version.h>
#include <TT/TTflags.h>
#include <TT/TTdefsm4.h>

#define TT_MIN2(a,b) ((a)<(b) ? (a) : (b))

#define TT_DECL_MAGIC(id) \
	static TT_CONST byte id[10+sizeof(uldat)] = { \
	    10+sizeof(uldat), \
	    sizeof(byte), \
	    sizeof(udat), \
	    sizeof(uldat), \
	    sizeof(hwcol), \
	    sizeof(time_t), \
	    sizeof(frac_t), \
	    sizeof(hwfont), \
	    sizeof(hwattr), \
	    0 \
	}


byte TTCheckMagic(TT_CONST byte id[]);

byte TTInPanic(void);
uldat TTLibraryVersion(void);

byte TTOpen(TT_CONST byte *target, ...);
void TTClose(void);
int TTConnectionFd(void);
byte TTFlush(void);
byte TTTimidFlush(void);
byte TTSync(void);

byte TTMainLoop(void);

ttobj TTNew(ttfn_ttobj FN);
void  TTDel(ttobj o);

byte TTInstanceOf(ttfn_ttobj FN, ttobj o);
ttfn_ttobj TTClassOf(ttobj o);

#define TT_NEW(obj)		((obj)TTNew((ttfn_ttobj)TT_CAT(TTFN_,obj)))
#define TT_DEL(o)		TTDel((ttobj)(o))

#define TT_INSTANCEOF(obj, o)	TTInstanceOf((ttfn_ttobj)TT_CAT(TTFN_,obj), (ttobj)o)
#define TT_CLASSOF(o)		((void *)TTClassOf((ttobj)o))

#define TTAllocMem TwAllocMem
#define TTReAllocMem TwReAllocMem
#define TTFreeMem  TwFreeMem
#define TTCloneMem TwCloneMem
#define TTWriteMem TwWriteMem
#define TTCopyMem  TwCopyMem
#define TTCmpMem   TwCmpMem

#define TTCloneStr TwCloneStr
#define TTLenStr   TwLenStr
#define TTCmpStr   TwCmpStr
#define TTCopyStr  TwCopyStr

ttnative TTCreate_ttnative(uldat id);
ttlabel TTCreate_ttlabel(byte TT_CONST *text);
ttanybutton TTCreate_ttanybutton(hwfont TT_CONST * text, dat width, dat height, dat pitch);
ttbutton TTCreate_ttbutton(hwfont TT_CONST * text, dat width, dat height, dat pitch);
ttapplication TTCreate_ttapplication(byte TT_CONST *name);


#endif /* _TT_H */
