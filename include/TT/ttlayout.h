




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttlayout.h  --  public header for libTT declarations related to "ttlayout"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTLAYOUT_H
#define _TT_TTLAYOUT_H







/* object structure */
#ifndef s_ttlayout


/** ttlayout: The superclass of all layouts */    
        struct s_ttlayout;
typedef struct s_ttlayout *	ttlayout;

#endif /* s_ttlayout */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttlayout */
extern ttclass	TTClass_ttlayout;





/* ttlayout methods */

/** set the current widget */    
/** apply layout to the contents of current widget */    








/* TTGet*_ttlayout and TTSet*_ttlayout methods */
    
/** get `widget' of ttlayout `o' */
ttwidget 	TTGetWidget_ttlayout(TT_ARG_READ ttlayout o);










#endif /* _TT_TTLAYOUT_H */

