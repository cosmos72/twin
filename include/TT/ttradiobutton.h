




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttradiobutton.h  --  public header for libTT declarations related to "ttradiobutton"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTRADIOBUTTON_H
#define _TT_TTRADIOBUTTON_H







/* object structure */
#ifndef s_ttradiobutton


/** ttradiobutton: a two-state mutual-exclusion button */            
        struct s_ttradiobutton;
typedef struct s_ttradiobutton *	ttradiobutton;

#endif /* s_ttradiobutton */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttradiobutton */
extern ttclass	TTClass_ttradiobutton;





/* ttradiobutton methods */

/** add the ttradiobutton `o' to the ttbuttongroup `g' */
void TTAddToGroup_ttradiobutton(ttradiobutton o,ttbuttongroup g);

/** remove the ttradiobutton `o' from its ttbuttongroup */
void TTRemoveFromGroup_ttradiobutton(ttradiobutton o);

/** create a ttradiobutton with given text */
ttradiobutton TTCreate_ttradiobutton(ttshort w,ttshort h,ttshort pitch,TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);









/* TTGet*_ttradiobutton and TTSet*_ttradiobutton methods */
    
/** get `group' of ttradiobutton `o' */
ttbuttongroup 	TTGetGroup_ttradiobutton(TT_ARG_READ ttradiobutton o);
    
/** get `group_prev' of ttradiobutton `o' */
ttradiobutton 	TTGetGroupPrev_ttradiobutton(TT_ARG_READ ttradiobutton o);
    
/** get `group_next' of ttradiobutton `o' */
ttradiobutton 	TTGetGroupNext_ttradiobutton(TT_ARG_READ ttradiobutton o);










#endif /* _TT_TTRADIOBUTTON_H */

