




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttcheckbutton.h  --  public header for libTT declarations related to "ttcheckbutton"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTCHECKBUTTON_H
#define _TT_TTCHECKBUTTON_H







/* object structure */
#ifndef s_ttcheckbutton


/** ttcheckbutton: a two-state (on/off) button */
        struct s_ttcheckbutton;
typedef struct s_ttcheckbutton *	ttcheckbutton;

#endif /* s_ttcheckbutton */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttcheckbutton */
extern ttclass	TTClass_ttcheckbutton;





/* ttcheckbutton methods */

/** set whether the ttbutton `o' is checked or not */    
void TTSetChecked_ttcheckbutton(ttcheckbutton o,ttbyte checked);

/** get whether the ttbutton `o' is checked or not */
ttbyte TTIsChecked_ttcheckbutton(TT_ARG_READ ttcheckbutton o);

/** create a ttcheckbutton with given text */
ttcheckbutton TTCreate_ttcheckbutton(ttshort w,ttshort h,ttshort pitch,TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);









/* TTGet*_ttcheckbutton and TTSet*_ttcheckbutton methods */










#endif /* _TT_TTCHECKBUTTON_H */

