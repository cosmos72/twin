




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttscroller.h  --  public header for libTT declarations related to "ttscroller"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTSCROLLER_H
#define _TT_TTSCROLLER_H







/* object structure */
#ifndef s_ttscroller


/** ttscroller: a scrollable container;
 * you can put any ttwidget inside its scrollpane
 */    
        struct s_ttscroller;
typedef struct s_ttscroller *	ttscroller;

#endif /* s_ttscroller */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttscroller */
extern ttclass	TTClass_ttscroller;





/* ttscroller methods */

/** create a ttscroller with given size */
ttscroller TTCreate_ttscroller(ttshort w,ttshort h);









/* TTGet*_ttscroller and TTSet*_ttscroller methods */
    
/** get `scrollpane' of ttscroller `o' */
ttscrollpane 	TTGetScrollpane_ttscroller(TT_ARG_READ ttscroller o);










#endif /* _TT_TTSCROLLER_H */

