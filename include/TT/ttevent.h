




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttevent.h  --  public header for libTT declarations related to "ttevent"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTEVENT_H
#define _TT_TTEVENT_H







/* object structure */
#ifndef s_ttevent


/** ttevent: The common part of all events. */                        
        struct s_ttevent;
typedef struct s_ttevent *	ttevent;

#endif /* s_ttevent */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttevent */
extern ttclass	TTClass_ttevent;





/* ttevent methods */

/** create a ttevent with given `evtype', `evcode' and `evflags' */
ttevent TTCreate_ttevent(ttuint evtype,ttuint evcode,ttuint evflags);

/** fire the ttevent `ev' on ttcomponent `o' */
void TTFire_ttevent(TT_ARG_DIE ttevent ev,TT_ARG_READ ttcomponent o);









/* TTGet*_ttevent and TTSet*_ttevent methods */
    
/** get `component' of ttevent `o' */
ttcomponent 	TTGetComponent_ttevent(TT_ARG_READ ttevent o);
    
/** get `evtype' of ttevent `o' */
ttuint 	TTGetEvtype_ttevent(TT_ARG_READ ttevent o);
    
/** get `evcode' of ttevent `o' */
ttuint 	TTGetEvcode_ttevent(TT_ARG_READ ttevent o);
    
/** get `evflags' of ttevent `o' */
ttuint 	TTGetEvflags_ttevent(TT_ARG_READ ttevent o);
    
/** get `prev' of ttevent `o' */
ttevent 	TTGetPrev_ttevent(TT_ARG_READ ttevent o);
    
/** get `next' of ttevent `o' */
ttevent 	TTGetNext_ttevent(TT_ARG_READ ttevent o);










#endif /* _TT_TTEVENT_H */

