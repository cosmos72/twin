




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  tttheme.h  --  public header for libTT declarations related to "tttheme"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTTHEME_H
#define _TT_TTTHEME_H







/* object structure */
#ifndef s_tttheme


/** tttheme: the look-n-feel of the application */                        
        struct s_tttheme;
typedef struct s_tttheme *	tttheme;

#endif /* s_tttheme */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of tttheme */
extern ttclass	TTClass_tttheme;





/* tttheme methods */

/** get the default tttheme used by ttvisible:s */
tttheme TTGetDefault_tttheme(void);









/* TTGet*_tttheme and TTSet*_tttheme methods */
    
/** get `name_len' of tttheme `o' */
ttopaque 	TTGetNameLen_tttheme(TT_ARG_READ tttheme o);
    
/** get `name' of tttheme `o' */
TT_ARG_ARRAY((_F(name_len))) ttbyte * 	TTGetName_tttheme(TT_ARG_READ tttheme o);
                









#endif /* _TT_TTTHEME_H */

