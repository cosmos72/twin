




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttanybutton.h  --  public header for libTT declarations related to "ttanybutton"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTANYBUTTON_H
#define _TT_TTANYBUTTON_H







/* object structure */
#ifndef s_ttanybutton


/** ttanybutton: the abstract class all buttons are derived from */                
        struct s_ttanybutton;
typedef struct s_ttanybutton *	ttanybutton;

#endif /* s_ttanybutton */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttanybutton */
extern ttclass	TTClass_ttanybutton;





/* ttanybutton methods */
    
/** set the text of the ttanybutton `o' and swallow `text' */    
void TTSetAttrD_ttanybutton(ttanybutton o,ttshort w,ttshort h,ttshort pitch,TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

/** set the text of the ttanybutton `o' to a copy of `text' */
ttbyte TTSetAttr_ttanybutton(ttanybutton o,ttshort w,ttshort h,ttshort pitch,TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

/** set the text of the ttanybutton `o' to a copy of `text' */
ttbyte TTSetFont_ttanybutton(ttanybutton o,ttshort w,ttshort h,ttshort pitch,TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttfont * text);

/** set the text of the ttanybutton `o' to a copy of `text' */
ttbyte TTSetText_ttanybutton(ttanybutton o,ttshort w,ttshort h,ttshort pitch,TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttbyte * text);

/** create a ttanybutton with a copy of given text */
ttanybutton TTCreate_ttanybutton(ttshort w,ttshort h,ttshort pitch,TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);









/* TTGet*_ttanybutton and TTSet*_ttanybutton methods */
    
/** get `text_width' of ttanybutton `o' */
ttshort 	TTGetTextWidth_ttanybutton(TT_ARG_READ ttanybutton o);
    
/** get `text_height' of ttanybutton `o' */
ttshort 	TTGetTextHeight_ttanybutton(TT_ARG_READ ttanybutton o);
    
/** get `text' of ttanybutton `o' */
TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * 	TTGetText_ttanybutton(TT_ARG_READ ttanybutton o);
    









#endif /* _TT_TTANYBUTTON_H */

