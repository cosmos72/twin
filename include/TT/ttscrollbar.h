




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttscrollbar.h  --  public header for libTT declarations related to "ttscrollbar"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTSCROLLBAR_H
#define _TT_TTSCROLLBAR_H







/* object structure */
#ifndef s_ttscrollbar


/** ttscrollbar: a normal, interactive scroll bar */    
        struct s_ttscrollbar;
typedef struct s_ttscrollbar *	ttscrollbar;

#endif /* s_ttscrollbar */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttscrollbar */
extern ttclass	TTClass_ttscrollbar;





/* ttscrollbar methods */

/** create a ttscrollbar with given fields */
ttscrollbar TTCreate1_ttscrollbar(ttbyte orientation);

/** create a ttscrollbar with given fields */
ttscrollbar TTCreate4_ttscrollbar(ttbyte orientation,ttint size,ttint real_size,ttint view_size);









/* TTGet*_ttscrollbar and TTSet*_ttscrollbar methods */
    









#endif /* _TT_TTSCROLLBAR_H */

