




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttnative.h  --  public header for libTT declarations related to "ttnative"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTNATIVE_H
#define _TT_TTNATIVE_H







/* object structure */
#ifndef s_ttnative


/** ttnative: class representing all server visible objects not created by us */
        struct s_ttnative;
typedef struct s_ttnative *	ttnative;

#endif /* s_ttnative */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttnative */
extern ttclass	TTClass_ttnative;





/* ttnative methods */

/** get the ttnative `root' object into which each top level ttvisible is added */    
ttnative TTGetRoot_ttnative(void);

/** get `w' (width) field of given ttnative */    
ttshort TTGetW_ttnative(TT_ARG_READ ttnative o);

/** get `h' (height) field of given ttnative */    
ttshort TTGetH_ttnative(TT_ARG_READ ttnative o);

/** create a ttnative with given fields */
ttnative TTCreate_ttnative(ttany native_value);









/* TTGet*_ttnative and TTSet*_ttnative methods */










#endif /* _TT_TTNATIVE_H */

