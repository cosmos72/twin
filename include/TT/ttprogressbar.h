




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttprogressbar.h  --  public header for libTT declarations related to "ttprogressbar"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTPROGRESSBAR_H
#define _TT_TTPROGRESSBAR_H







/* object structure */
#ifndef s_ttprogressbar


/** ttprogressbar: a percentage completion bar */        
        struct s_ttprogressbar;
typedef struct s_ttprogressbar *	ttprogressbar;

#endif /* s_ttprogressbar */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttprogressbar */
extern ttclass	TTClass_ttprogressbar;





/* ttprogressbar methods */

/** create a ttprogressbar with given fields */
ttprogressbar TTCreate_ttprogressbar(ttbyte orientation);









/* TTGet*_ttprogressbar and TTSet*_ttprogressbar methods */
        









#endif /* _TT_TTPROGRESSBAR_H */

