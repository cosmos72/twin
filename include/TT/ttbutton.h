




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttbutton.h  --  public header for libTT declarations related to "ttbutton"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTBUTTON_H
#define _TT_TTBUTTON_H







/* object structure */
#ifndef s_ttbutton


/** ttbutton: a normal click-to-activate button */
        struct s_ttbutton;
typedef struct s_ttbutton *	ttbutton;

#endif /* s_ttbutton */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttbutton */
extern ttclass	TTClass_ttbutton;





/* ttbutton methods */

/** set whether the ttbutton `o' is pressed or not */    
void TTSetPressed_ttbutton(ttbutton o,ttbyte pressed);

/** get whether the ttbutton `o' is pressed or not */
ttbyte TTIsPressed_ttbutton(TT_ARG_READ ttbutton o);

/** create a ttbutton with given text */
ttbutton TTCreate_ttbutton(ttshort w,ttshort h,ttshort pitch,TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);









/* TTGet*_ttbutton and TTSet*_ttbutton methods */










#endif /* _TT_TTBUTTON_H */

