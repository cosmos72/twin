




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttboxlayout.h  --  public header for libTT declarations related to "ttboxlayout"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTBOXLAYOUT_H
#define _TT_TTBOXLAYOUT_H







/* object structure */
#ifndef s_ttboxlayout


/** ttlayout: a box (horizontal or vertical) layout */    
        struct s_ttboxlayout;
typedef struct s_ttboxlayout *	ttboxlayout;

#endif /* s_ttboxlayout */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttboxlayout */
extern ttclass	TTClass_ttboxlayout;





/* ttboxlayout methods */

/** create a ttboxlayout with given orientation */
ttboxlayout TTCreate_ttboxlayout(ttany orientation);









/* TTGet*_ttboxlayout and TTSet*_ttboxlayout methods */
    
/** get `orientation' of ttboxlayout `o' */
ttuint 	TTGetOrientation_ttboxlayout(TT_ARG_READ ttboxlayout o);










#endif /* _TT_TTBOXLAYOUT_H */

