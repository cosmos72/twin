




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttscrollpane.h  --  public header for libTT declarations related to "ttscrollpane"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTSCROLLPANE_H
#define _TT_TTSCROLLPANE_H







/* object structure */
#ifndef s_ttscrollpane


/** ttscrollpane: a scrollable widget;
 * it keeps track of its wl and hl
 * from the ttwidgets you put inside it;
 * also used as the scrollpane of a ttscroller.
 */        
        struct s_ttscrollpane;
typedef struct s_ttscrollpane *	ttscrollpane;

#endif /* s_ttscrollpane */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttscrollpane */
extern ttclass	TTClass_ttscrollpane;





/* ttscrollpane methods */

/** create a ttscrollpane with given size */
ttscrollpane TTCreate_ttscrollpane(ttshort w,ttshort h);

/** set the x scrollbar the ttscrollpane `o' is connected to */    
ttbyte TTSetBarX_ttscrollpane(ttscrollpane o,ttscrollbar bar_x);

/** set the y scrollbar the ttscrollpane `o' is connected to */    
ttbyte TTSetBarY_ttscrollpane(ttscrollpane o,ttscrollbar bar_y);









/* TTGet*_ttscrollpane and TTSet*_ttscrollpane methods */
    
/** get `bar_x' of ttscrollpane `o' */
ttscrollbar 	TTGetBarX_ttscrollpane(TT_ARG_READ ttscrollpane o);

/** set `bar_x' of ttscrollpane `o' */
ttbyte 	TTSetBarX_ttscrollpane(ttscrollpane o, ttscrollbar bar_x);
    
/** get `bar_y' of ttscrollpane `o' */
ttscrollbar 	TTGetBarY_ttscrollpane(TT_ARG_READ ttscrollpane o);

/** set `bar_y' of ttscrollpane `o' */
ttbyte 	TTSetBarY_ttscrollpane(ttscrollpane o, ttscrollbar bar_y);










#endif /* _TT_TTSCROLLPANE_H */

