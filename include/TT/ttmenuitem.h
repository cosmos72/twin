




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttmenuitem.h  --  public header for libTT declarations related to "ttmenuitem"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTMENUITEM_H
#define _TT_TTMENUITEM_H







/* object structure */
#ifndef s_ttmenuitem


/** ttmenuitem: a simple line in a menu */        
        struct s_ttmenuitem;
typedef struct s_ttmenuitem *	ttmenuitem;

#endif /* s_ttmenuitem */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttmenuitem */
extern ttclass	TTClass_ttmenuitem;





/* ttmenuitem methods */









/* TTGet*_ttmenuitem and TTSet*_ttmenuitem methods */
    
/** get `name_len' of ttmenuitem `o' */
ttopaque 	TTGetNameLen_ttmenuitem(TT_ARG_READ ttmenuitem o);
    
/** get `name' of ttmenuitem `o' */
TT_ARG_ARRAY((_F(name_len))) ttbyte * 	TTGetName_ttmenuitem(TT_ARG_READ ttmenuitem o);










#endif /* _TT_TTMENUITEM_H */

