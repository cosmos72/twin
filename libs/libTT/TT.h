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
#include <TT/TTmem.h>


#define TT_DECL_MAGIC(id) TW_DECL_MAGIC(id)

ttbyte TTCheckMagic(TT_CONST ttbyte id[]);

ttbyte TTInPanic(void);
ttuint TTLibraryVersion(void);

ttbyte TTOpen(TT_CONST ttbyte *target);
void TTClose(void);
void TTCloseQuickNDirty(void);
int TTConnectionFd(void);
ttbyte TTFlush(void);
ttbyte TTTimidFlush(void);
ttbyte TTSync(void);

ttbyte TTMainLoop(void);
ttbyte TTMainLoopOnce(ttbyte wait);
void TTExitMainLoop(void);

tt_obj TTNew(tt_obj Class);
void  TTDel(tt_obj o);

tt_obj TTGetFirst(tt_obj Class);
tt_obj TTGetLast(tt_obj Class);

ttopaque TTSizeOf(tt_obj o);
tt_obj TTClassOf(tt_obj o);
tt_obj TTSuperClassOf(tt_obj o);
TT_CONST ttbyte *TTClassNameOf(tt_obj o);
ttbyte TTInstanceOf(tt_obj Class, tt_obj o);

tt_obj TTGetInstalled_tteventmask(void);
tt_obj TTGetDefault_tteventmask(void);
void TTSetInstalled_tteventmask(tt_obj installed_event_mask);
void TTSetDefault_tteventmask(tt_obj default_event_mask);

#endif /* _TT_H */
