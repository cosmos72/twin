




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttcallback.h  --  public header for libTT declarations related to "ttcallback"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTCALLBACK_H
#define _TT_TTCALLBACK_H







/* object structure */
#ifndef s_ttcallback


/** ttcallback: The entry point for internal event-based callbacks from target drivers */                
        struct s_ttcallback;
typedef struct s_ttcallback *	ttcallback;

#endif /* s_ttcallback */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttcallback */
extern ttclass	TTClass_ttcallback;





/* ttcallback methods */
    








/* TTGet*_ttcallback and TTSet*_ttcallback methods */
    
/** get `lflags' of ttcallback `o' */
ttuint 	TTGetLflags_ttcallback(TT_ARG_READ ttcallback o);
    
/** get `component' of ttcallback `o' */
ttcomponent 	TTGetComponent_ttcallback(TT_ARG_READ ttcallback o);
    
/** get `prev' of ttcallback `o' */
ttcallback 	TTGetPrev_ttcallback(TT_ARG_READ ttcallback o);
    
/** get `next' of ttcallback `o' */
ttcallback 	TTGetNext_ttcallback(TT_ARG_READ ttcallback o);










#endif /* _TT_TTCALLBACK_H */

