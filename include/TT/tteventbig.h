




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  tteventbig.h  --  public header for libTT declarations related to "tteventbig"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTEVENTBIG_H
#define _TT_TTEVENTBIG_H







/* object structure */
#ifndef s_tteventbig


/** tteventbig: The union of all events. */                                
        struct s_tteventbig;
typedef struct s_tteventbig *	tteventbig;

#endif /* s_tteventbig */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of tteventbig */
extern ttclass	TTClass_tteventbig;





/* tteventbig methods */

/** create a tteventbig with given fields */
tteventbig TTCreate8_tteventbig(ttuint evtype,ttuint evcode,ttuint evflags,ttshort x,ttshort y,ttshort w,ttshort h,ttany value,ttany old_value,ttopaque data_len,TT_ARG_READ TT_ARG_ARRAY((_P(10))) ttbyte * data);

/** create a tteventbig with given fields */
tteventbig TTCreate4_tteventbig(ttuint evtype,ttuint evcode,ttuint evflags,ttshort x,ttshort,ttshort w,ttshort h);

/** create a tteventbig with given fields */
tteventbig TTCreate4s4_tteventbig(ttuint evtype,ttuint evcode,ttuint evflags,ttany value,ttany old_value,ttopaque data_len,TT_ARG_READ TT_ARG_ARRAY((_P(6))) ttbyte * data);









/* TTGet*_tteventbig and TTSet*_tteventbig methods */
    
/** get `x' of tteventbig `o' */
ttshort 	TTGetX_tteventbig(TT_ARG_READ tteventbig o);
    
/** get `y' of tteventbig `o' */
ttshort 	TTGetY_tteventbig(TT_ARG_READ tteventbig o);
    
/** get `w' of tteventbig `o' */
ttshort 	TTGetW_tteventbig(TT_ARG_READ tteventbig o);
    
/** get `h' of tteventbig `o' */
ttshort 	TTGetH_tteventbig(TT_ARG_READ tteventbig o);
    
/** get `value' of tteventbig `o' */
ttany 	TTGetValue_tteventbig(TT_ARG_READ tteventbig o);
    
/** get `old_value' of tteventbig `o' */
ttany 	TTGetOldValue_tteventbig(TT_ARG_READ tteventbig o);
    
/** get `data_len' of tteventbig `o' */
ttopaque 	TTGetDataLen_tteventbig(TT_ARG_READ tteventbig o);
    
/** get `data' of tteventbig `o' */
TT_ARG_READ TT_ARG_ARRAY((_F(data_len))) ttbyte * 	TTGetData_tteventbig(TT_ARG_READ tteventbig o);










#endif /* _TT_TTEVENTBIG_H */

