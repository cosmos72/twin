




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttbuttongroup.h  --  public header for libTT declarations related to "ttbuttongroup"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTBUTTONGROUP_H
#define _TT_TTBUTTONGROUP_H







/* object structure */
#ifndef s_ttbuttongroup


/** ttbuttongroup: a mutual-exclusion group of radiobuttons */            
        struct s_ttbuttongroup;
typedef struct s_ttbuttongroup *	ttbuttongroup;

#endif /* s_ttbuttongroup */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttbuttongroup */
extern ttclass	TTClass_ttbuttongroup;





/* ttbuttongroup methods */

/** add the ttradiobutton `r' to the ttbuttongroup `o' */    
void TTAdd_ttbuttongroup(ttbuttongroup o,ttradiobutton r);

/** remove the ttradiobutton `r' from the ttbuttongroup `o' */    
void TTRemove_ttbuttongroup(ttbuttongroup o,ttradiobutton r);

/** set which ttradiobutton in the ttbuttongroup `o' is checked */    
void TTSetChecked_ttbuttongroup(ttbuttongroup o,ttradiobutton r);









/* TTGet*_ttbuttongroup and TTSet*_ttbuttongroup methods */
    
/** get `group_first' of ttbuttongroup `o' */
ttradiobutton 	TTGetGroupFirst_ttbuttongroup(TT_ARG_READ ttbuttongroup o);
    
/** get `group_last' of ttbuttongroup `o' */
ttradiobutton 	TTGetGroupLast_ttbuttongroup(TT_ARG_READ ttbuttongroup o);
    
/** get `checked' of ttbuttongroup `o' */
ttradiobutton 	TTGetChecked_ttbuttongroup(TT_ARG_READ ttbuttongroup o);










#endif /* _TT_TTBUTTONGROUP_H */

