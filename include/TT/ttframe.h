




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttframe.h  --  public header for libTT declarations related to "ttframe"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTFRAME_H
#define _TT_TTFRAME_H







/* object structure */
#ifndef s_ttframe


/** ttframe: a toplevel window */    
        struct s_ttframe;
typedef struct s_ttframe *	ttframe;

#endif /* s_ttframe */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttframe */
extern ttclass	TTClass_ttframe;





/* ttframe methods */

/** set the menubar of the ttframe `o' */    
ttbyte TTSetMenubar_ttframe(ttframe o,TT_ARG_READ ttmenubar m);

/** create a ttframe with given title */
ttframe TTCreate_ttframe(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title);









/* TTGet*_ttframe and TTSet*_ttframe methods */
    
/** get `menubar' of ttframe `o' */
ttmenubar 	TTGetMenubar_ttframe(TT_ARG_READ ttframe o);

/** set `menubar' of ttframe `o' */
ttbyte 	TTSetMenubar_ttframe(ttframe o, ttmenubar menubar);










#endif /* _TT_TTFRAME_H */

