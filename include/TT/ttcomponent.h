




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttcomponent.h  --  public header for libTT declarations related to "ttcomponent"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTCOMPONENT_H
#define _TT_TTCOMPONENT_H







/* object structure */
#ifndef s_ttcomponent


/** ttcomponent: The superclass of all components (visible objects, groups, applications, ...) */                
        struct s_ttcomponent;
typedef struct s_ttcomponent *	ttcomponent;

#endif /* s_ttcomponent */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttcomponent */
extern ttclass	TTClass_ttcomponent;





/* ttcomponent methods */

/** add the ttlistener `c' to `o' */
void TTAddListener_ttcomponent(ttcomponent o,ttlistener l);

/** remove the ttlistener `c' from `o' */
void TTRemoveListener_ttcomponent(ttcomponent o,ttlistener l);

/** associate `data' with `key' in ttcomponent `o' */
ttdata TTSetKeyData_ttcomponent(ttcomponent o,TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key,ttany data);

/** get `data' associated with with `key' in ttcomponent `o' */
ttany TTGetKeyData_ttcomponent(TT_ARG_READ ttcomponent o,TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key);

/** remove `data' associated with `key' in ttcomponent `o' */
void TTUnsetKeyData_ttcomponent(ttcomponent o,TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key);

/** get the ttdata containing `data' associated with with `key' in ttcomponent `o' */
ttdata TTGetData_ttcomponent(TT_ARG_READ ttcomponent o,TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key);









/* TTGet*_ttcomponent and TTSet*_ttcomponent methods */
    
/** get `callbacks' of ttcomponent `o' */
ttcallback 	TTGetCallbacks_ttcomponent(TT_ARG_READ ttcomponent o);
    
/** get `listeners' of ttcomponent `o' */
ttlistener 	TTGetListeners_ttcomponent(TT_ARG_READ ttcomponent o);
    
/** get `timers' of ttcomponent `o' */
tttimer 	TTGetTimers_ttcomponent(TT_ARG_READ ttcomponent o);
    
/** get `datas' of ttcomponent `o' */
ttdata 	TTGetDatas_ttcomponent(TT_ARG_READ ttcomponent o);










#endif /* _TT_TTCOMPONENT_H */

