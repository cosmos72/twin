




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttwindow.h  --  public header for libTT declarations related to "ttwindow"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTWINDOW_H
#define _TT_TTWINDOW_H







/* object structure */
#ifndef s_ttwindow


/** ttwindow: a full-featured window */        
        struct s_ttwindow;
typedef struct s_ttwindow *	ttwindow;

#endif /* s_ttwindow */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttwindow */
extern ttclass	TTClass_ttwindow;





/* ttwindow methods */

/** set the title of the ttwindow `o' */    
ttbyte TTSetTitle_ttwindow(ttwindow o,TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title);









/* TTGet*_ttwindow and TTSet*_ttwindow methods */
    
/** get `title_len' of ttwindow `o' */
ttopaque 	TTGetTitleLen_ttwindow(TT_ARG_READ ttwindow o);
    
/** get `title' of ttwindow `o' */
TT_ARG_ARRAY((_F(title_len))) ttbyte * 	TTGetTitle_ttwindow(TT_ARG_READ ttwindow o);










#endif /* _TT_TTWINDOW_H */

