




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttclass.h  --  public header for libTT declarations related to "ttclass"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTCLASS_H
#define _TT_TTCLASS_H







/* object structure */
#ifndef s_ttclass


/** ttclass: The runtime type of all objects */            
        struct s_ttclass;
typedef struct s_ttclass *	ttclass;

#endif /* s_ttclass */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttclass */
extern ttclass	TTClass_ttclass;





/* ttclass methods */

/** get the name of the ttclass `o' */
TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * TTGetName_ttclass(TT_ARG_READ ttclass o);

/** get the size (bytes) used by objects of type represented by ttclass `o' */
ttopaque TTGetSize_ttclass(TT_ARG_READ ttclass o);

/** get the parent class of the ttclass `o' */
TT_ARG_READ ttclass TTGetSuper_ttclass(TT_ARG_READ ttclass o);

/** return TT_TRUE if type `t' can be cast to type `o' */
ttbyte TTIsInstance_ttclass(TT_ARG_READ ttclass o,TT_ARG_READ ttclass t);

/** get the bare class of given type (removing attribute like _R, _W, _D, _A((x))) */
TT_ARG_READ ttclass TTFromType_ttclass(ttopaque type);

/** return TT_TRUE if given type indicates an array */
ttbyte TTIsArrayType_ttclass(ttopaque type);

/** return TT_TRUE if given type indicates a primitive type (not a ttobj or derivate) */
ttbyte TTIsPrimitiveType_ttclass(ttopaque type);









/* TTGet*_ttclass and TTSet*_ttclass methods */
            









#endif /* _TT_TTCLASS_H */

