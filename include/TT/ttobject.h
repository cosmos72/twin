




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttobject.h  --  public header for libTT declarations related to "ttobject"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTOBJECT_H
#define _TT_TTOBJECT_H







/* object structure */
#ifndef s_ttobject

                            
        struct s_ttobject;
typedef struct s_ttobject *	ttobject;

#endif /* s_ttobject */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttobject */
extern ttclass	TTClass_ttobject;





/* ttobject methods */
                
/** increase by 1 the reference count of object `o' */
void TTRef_ttobject(ttobject o);

/** decrease by 1 the reference count of object `o' */
void TTUnref_ttobject(ttobject o);

/** set `user_data' of object `o' */
ttbyte TTSetUserData_ttobject(ttobject o,ttany user_data);









/* TTGet*_ttobject and TTSet*_ttobject methods */
    
/** get `refcount' of ttobject `o' */
ttopaque 	TTGetRefcount_ttobject(TT_ARG_READ ttobject o);
    
/** get `oflags' of ttobject `o' */
ttuint 	TTGetOflags_ttobject(TT_ARG_READ ttobject o);
            
/** get `user_data' of ttobject `o' */
ttany 	TTGetUserData_ttobject(TT_ARG_READ ttobject o);

/** set `user_data' of ttobject `o' */
ttbyte 	TTSetUserData_ttobject(ttobject o, ttany user_data);
    
/** get `events_inprogress' of ttobject `o' */
ttopaque 	TTGetEventsInprogress_ttobject(TT_ARG_READ ttobject o);
    
/** get `events_informed' of ttobject `o' */
ttopaque 	TTGetEventsInformed_ttobject(TT_ARG_READ ttobject o);










#endif /* _TT_TTOBJECT_H */

