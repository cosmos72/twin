




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttapplication.h  --  public header for libTT declarations related to "ttapplication"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTAPPLICATION_H
#define _TT_TTAPPLICATION_H







/* object structure */
#ifndef s_ttapplication


/** ttapplication: the topmost object */        
        struct s_ttapplication;
typedef struct s_ttapplication *	ttapplication;

#endif /* s_ttapplication */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttapplication */
extern ttclass	TTClass_ttapplication;





/* ttapplication methods */

/** create (only one allowed) a ttapplication with given name */
ttapplication TTCreate_ttapplication(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name);

/** get the current application */
ttapplication TTGet_ttapplication(void);









/* TTGet*_ttapplication and TTSet*_ttapplication methods */
    
/** get `name_len' of ttapplication `o' */
ttopaque 	TTGetNameLen_ttapplication(TT_ARG_READ ttapplication o);
    
/** get `name' of ttapplication `o' */
TT_ARG_ARRAY((_F(name_len))) ttbyte * 	TTGetName_ttapplication(TT_ARG_READ ttapplication o);










#endif /* _TT_TTAPPLICATION_H */

