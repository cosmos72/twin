




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttlistener.h  --  public header for libTT declarations related to "ttlistener"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTLISTENER_H
#define _TT_TTLISTENER_H







/* object structure */
#ifndef s_ttlistener


/** ttlistener: A ttcallback derivative for user-set event-based callbacks */                                    
        struct s_ttlistener;
typedef struct s_ttlistener *	ttlistener;

#endif /* s_ttlistener */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttlistener */
extern ttclass	TTClass_ttlistener;





/* ttlistener methods */

/** create a ttlistener for a ttcomponent with given fields */
ttlistener TTCreate_ttlistener(ttcomponent o,ttuint evtype,ttuint lflags,TT_ARG_READ ttlistener_fn function,ttany arg);

/** create a ttlistener for a ttcomponent with given fields */
ttlistener TTCreateE_ttlistener(ttcomponent o,TT_ARG_DIE ttevent ev,ttuint lflags,TT_ARG_READ ttlistener_fn function,ttany arg);

/** create a ttlistener for a ttcomponent with given fields */
ttlistener TTCreateR_ttlistener(ttcomponent o,TT_ARG_DIE ttevent ev,ttuint lflags,TT_ARG_READ ttlistener_fn function,ttopaque arg_component_n,ttopaque arg_event_n,TT_ARG_DIE ttvector args);

/** create a ttlistener for a ttcomponent "del" ttevent with given fields */
ttlistener TTCreateDel_ttlistener(ttcomponent o,ttuint lflags,TT_ARG_READ ttlistener_fn function);

/** create a ttlistener for a ttcomponent "askclose" ttevent with given fields */
ttlistener TTCreateAskclose_ttlistener(ttcomponent o,ttuint lflags,TT_ARG_READ ttlistener_fn function);

/** create a ttlistener for a ttcomponent "key" ttevent with given fields */
ttlistener TTCreateKey_ttlistener(ttcomponent o,ttuint evcode,ttuint evflags,ttuint lflags,TT_ARG_READ ttlistener_fn function);

/** create a ttlistener for a ttcomponent "mouse" ttevent with given fields */
ttlistener TTCreateMouse_ttlistener(ttcomponent o,ttuint evcode,ttuint evflags,ttuint lflags,TT_ARG_READ ttlistener_fn function);

/** create a ttlistener for a ttcomponent "activate" ttevent with given fields */
ttlistener TTCreateActivate_ttlistener(ttcomponent o,ttuint lflags,TT_ARG_READ ttlistener_fn function);

/** create a ttlistener for a ttcomponent "value change" ttevent with given fields */
ttlistener TTCreateChange_ttlistener(ttcomponent o,ttuint evcode,ttuint lflags,TT_ARG_READ ttlistener_fn function);

/** create a ttlistener for a ttcomponent "key_data value change" ttevent with given fields (WARNING: listener will be called with `ttdata' as event component!)*/
ttlistener TTCreateKeyData_ttlistener(ttcomponent o,TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key,ttuint lflags,TT_ARG_READ ttlistener_fn function);

/** create a ttlistener for a ttcomponent "expose" ttevent with given fields */
ttlistener TTCreateExpose_ttlistener(ttcomponent o,ttuint lflags,TT_ARG_READ ttlistener_fn function);

/** add `o' to the ttcomponent `c' */    
void TTAddTo_ttlistener(ttlistener o,ttcomponent c);

/** remove `o' from the ttcomponent it is attached to */    
void TTRemove_ttlistener(ttlistener o);

/** set the function args of the listener `o' */
ttbyte TTSetArgsR_ttlistener(ttlistener o,TT_ARG_DIE ttvector args);

/** set the mask of blocked events to be used while listener `o' is being called */
ttbyte TTSetEventMask_ttlistener(ttlistener o,TT_ARG_DIE tteventmask event_mask);









/* TTGet*_ttlistener and TTSet*_ttlistener methods */
        
/** get `event' of ttlistener `o' */
ttevent 	TTGetEvent_ttlistener(TT_ARG_READ ttlistener o);
    
/** get `function' of ttlistener `o' */
ttlistener_fn 	TTGetFunction_ttlistener(TT_ARG_READ ttlistener o);
    
/** get `arg_component_n' of ttlistener `o' */
ttopaque 	TTGetArgComponentN_ttlistener(TT_ARG_READ ttlistener o);
    
/** get `arg_event_n' of ttlistener `o' */
ttopaque 	TTGetArgEventN_ttlistener(TT_ARG_READ ttlistener o);
    
/** get `args' of ttlistener `o' */
ttvector 	TTGetArgs_ttlistener(TT_ARG_READ ttlistener o);
    
/** get `event_mask' of ttlistener `o' */
tteventmask 	TTGetEventMask_ttlistener(TT_ARG_READ ttlistener o);

/** set `event_mask' of ttlistener `o' */
ttbyte 	TTSetEventMask_ttlistener(ttlistener o, tteventmask event_mask);
        









#endif /* _TT_TTLISTENER_H */

