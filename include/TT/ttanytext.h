




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttanytext.h  --  public header for libTT declarations related to "ttanytext"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTANYTEXT_H
#define _TT_TTANYTEXT_H







/* object structure */
#ifndef s_ttanytext


/** ttanytext: the superclass of all text-only widgets */        
        struct s_ttanytext;
typedef struct s_ttanytext *	ttanytext;

#endif /* s_ttanytext */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttanytext */
extern ttclass	TTClass_ttanytext;





/* ttanytext methods */









/* TTGet*_ttanytext and TTSet*_ttanytext methods */
    
/** get `text_len' of ttanytext `o' */
ttopaque 	TTGetTextLen_ttanytext(TT_ARG_READ ttanytext o);
    
/** get `text' of ttanytext `o' */
TT_ARG_ARRAY((_F(text_len))) ttattr * 	TTGetText_ttanytext(TT_ARG_READ ttanytext o);










#endif /* _TT_TTANYTEXT_H */

