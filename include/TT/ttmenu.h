




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttmenu.h  --  public header for libTT declarations related to "ttmenu"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTMENU_H
#define _TT_TTMENU_H







/* object structure */
#ifndef s_ttmenu


/** ttmenu: a line in a menu pointing to a sub menu */    
        struct s_ttmenu;
typedef struct s_ttmenu *	ttmenu;

#endif /* s_ttmenu */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttmenu */
extern ttclass	TTClass_ttmenu;





/* ttmenu methods */









/* TTGet*_ttmenu and TTSet*_ttmenu methods */
    
/** get `menubar' of ttmenu `o' */
ttmenubar 	TTGetMenubar_ttmenu(TT_ARG_READ ttmenu o);










#endif /* _TT_TTMENU_H */

