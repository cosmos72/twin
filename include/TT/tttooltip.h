




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  tttooltip.h  --  public header for libTT declarations related to "tttooltip"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTTOOLTIP_H
#define _TT_TTTOOLTIP_H







/* object structure */
#ifndef s_tttooltip


/** tttooltip: A simple tooltip */        
        struct s_tttooltip;
typedef struct s_tttooltip *	tttooltip;

#endif /* s_tttooltip */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of tttooltip */
extern ttclass	TTClass_tttooltip;





/* tttooltip methods */

/** set the widget the tttooltip `o' is connected to */    
ttbyte TTSetWidget_tttooltip(TT_ARG_DIE tttooltip o,ttwidget w);









/* TTGet*_tttooltip and TTSet*_tttooltip methods */
        
/** get `widget' of tttooltip `o' */
ttwidget 	TTGetWidget_tttooltip(TT_ARG_READ tttooltip o);










#endif /* _TT_TTTOOLTIP_H */

