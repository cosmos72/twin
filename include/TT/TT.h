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

/** initialize libTT opening given display target (got from $TTDISPLAY if not specified) */
ttbyte TTOpen(TT_CONST ttbyte *target);

/** reinitialize libTT to use newly given display target, WITHOUT deleting libTT objects */
ttbyte TTReopen(TT_CONST ttbyte *target);

/** shutdown libTT; not necessary if you are going to exit(): use TTReopen("null") instead */
void TTClose(void);

/** quickly shutdown libTT, does not generate `delete' events for objects deleted; not necessary if you are going to exit(): use TTReopen("null") instead */
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
TT_FN_ATTR_CONST ttuint TTSizeOf(ttobj o);

/** return 1 if `o' can be cast to type represented by `FN' */
TT_FN_ATTR_CONST ttbyte TTInstanceOf(ttfn FN, ttobj o);

/** return the type of `o' (will be one of the `TTFN_*' types) */
TT_FN_ATTR_CONST ttfn   TTClassOf(ttobj o);

/** return the name of the type of `o' (i.e "ttbutton", ...) */
TT_FN_ATTR_CONST TT_CONST ttbyte *TTClassNameOf(ttobj o);

/** get internal library size needed to store objects of type `FN' */
TT_FN_ATTR_CONST ttuint TTGetSize_ttfn(ttfn FN);

/** get the parent type of the type `FN' */
TT_FN_ATTR_CONST ttfn   TTGetSuper_ttfn(ttfn fn);

/** return 1 if type `t' can be cast to type `FN' */
TT_FN_ATTR_CONST ttbyte TTIsInstance_ttfn(ttfn FN, ttfn t);

/** get the name of the type `FN' (i.e "ttbutton", ...) */
TT_FN_ATTR_CONST TT_CONST ttbyte *TTGetName_ttfn(ttfn FN);



/** return the id to access the field `name' in an object (i.e TTGetFieldId("ttwidget_x")) */
TT_FN_ATTR_CONST ttuint TTGetFieldId(TT_ARG_READ ttbyte *name);

/** return the name of a field from its `id' - this is the opposite of TTGetFieldId() */
TT_FN_ATTR_CONST TT_CONST ttbyte *TTGetFieldName(ttuint id);


/** get the value of `which' field from object `o' */
ttfn TTGetField_ttobj(TT_ARG_READ ttobj o, ttuint which, ttany *value);

/** set the value of `which' field into object `o' to `value' */
ttfn TTSetField_ttobj(ttobj o, ttuint which, ttany value);

/** change the value of `which' field into object `o' to `(old_value & ~nand_value) ^ xor_value' */
ttfn TTChangeField_ttobj(ttobj o, ttuint which, ttany nand_value, ttany xor_value);


/** get the current mask of blocked events */
tteventmask TTGetInstalled_tteventmask(void);
/** get the default mask of blocked events to be used while listeners are being called */
tteventmask TTGetDefault_tteventmask(void);
/** set the current mask of blocked events */
void TTSetInstalled_tteventmask(tteventmask installed_event_mask);
/** set the default mask of blocked events to be used while listeners are being called */
void TTSetDefault_tteventmask(tteventmask default_event_mask);

/** create a ttlistener with given fields */
ttlistener TTCreateL_ttlistener(ttcomponent o, ttevent ev, ttuint flags, ttuint narg_component, ttuint narg_event, ttuint nargs, ttlistener_fn function, ...);

#endif /* _TT_H */
