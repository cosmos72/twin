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
#include <TT/defines.h>


/** check some magic data to ensure client and library have compatible types */
ttbyte TTCheckMagic(TT_CONST ttbyte id[]);

/** return 1 if a fatal error occurred (panic) */
ttbyte TTInPanic(void);

/** get full library version number */
ttuint TTLibraryVersion(void);

typedef struct s_ttclasses *ttclasses;
typedef struct s_tthw * tthw;
/** register a new libTT display target */
ttbyte TTRegisterTarget(TT_CONST byte *name, ttclasses (*InitHW)(tthw *HW));

/** initialize libTT opening given display target (got from $TTDISPLAY if not specified) */
ttbyte TTOpen(TT_CONST ttbyte *target);

/** reinitialize libTT to use newly given display target, WITHOUT deleting libTT objects */
ttbyte TTReopen(TT_CONST ttbyte *target);

/** shutdown libTT; not necessary if you are going to exit() */
void TTClose(void);

/** quickly shutdown libTT, does not generate `delete' events for objects being deleted; not necessary if you are going to exit() */
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

/** create a new object of class `Class'; `Class' should be one of the `TTClass_*' classes */
ttobj TTNew(ttclass Class);

/** delete given object */
void  TTDel(ttobj o);


/** get the first existing object of class `Class'; `Class' should be one of the `TTClass_*' classes */
ttobj TTGetFirst(ttclass Class);

/** get the last existing object of class `Class'; `Class' should be one of the `TTClass_*' classes */
ttobj TTGetLast(ttclass Class);


/** get size needed to store object `o' (i.e. sizeof(ttobj), NOT sizeof(*ttobj)) */
TT_FN_ATTR_CONST ttuint  TTSizeOf(ttobj o);

/** return the class (runtime type) of `o' (will be one of the `TTClass_*' types) */
TT_FN_ATTR_CONST ttclass TTClassOf(ttobj o);

/** return TT_TRUE if `o' can be cast to class represented by `Class' */
TT_FN_ATTR_CONST ttbyte  TTInstanceOf(ttclass Class, ttobj o);

/** return the name of the class of `o' (i.e "ttbutton", "ttwindow", ...) */
TT_FN_ATTR_CONST TT_CONST TT_ARG_ARRAY_Z ttbyte *TTClassNameOf(ttobj o);



/** create a vector with given values */
ttvector TTCreateL_ttvector(ttopaque value_n, ...);


/** get the current mask of blocked events */
tteventmask TTGetInstalled_tteventmask(void);
/** get the default mask of blocked events to be used while listeners are being called */
tteventmask TTGetDefault_tteventmask(void);
/** set the current mask of blocked events */
void TTSetInstalled_tteventmask(tteventmask installed_event_mask);
/** set the default mask of blocked events to be used while listeners are being called */
void TTSetDefault_tteventmask(tteventmask default_event_mask);



#endif /* _TT_H */
