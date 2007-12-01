




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttfield.h  --  public header for libTT declarations related to "ttfield"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTFIELD_H
#define _TT_TTFIELD_H







/* object structure */
#ifndef s_ttfield


/** ttfield: a reflective class, represents a field in an object */            
        struct s_ttfield;
typedef struct s_ttfield *	ttfield;

#endif /* s_ttfield */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttfield */
extern ttclass	TTClass_ttfield;





/* ttfield methods */

/** get the field with given name */
TT_ARG_READ ttfield TTGet_ttfield(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name);

/** get the value of field `f' in object `o' */
ttbyte TTGetValue_ttfield(TT_ARG_READ ttfield f,TT_ARG_READ ttobject o,TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * value);

/** set the value of field `f' in object `o' */
ttbyte TTSetValue_ttfield(TT_ARG_READ ttfield f,ttobject o,TT_ARG_READ TT_ARG_ARRAY((1)) ttarg * value);

/** change the value of field `f' in object `o' */
ttbyte TTChangeValue_ttfield(TT_ARG_READ ttfield f,ttobject o,ttany nand_value,ttany xor_value);

/** return the ttuint representing field `f' in evcodes */
ttopaque TTToEvcode_ttfield(TT_ARG_READ ttfield f);

/** return the field corresponding to ttuint `i' in evcodes */
TT_ARG_READ ttfield TTFromEvcode_ttfield(ttopaque i);

/** get the ttuint representing field `f' in evcodes given its name */
ttopaque TTGetEvcode_ttfield(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name);









/* TTGet*_ttfield and TTSet*_ttfield methods */
    
/** get `name_len' of ttfield `o' */
ttopaque 	TTGetNameLen_ttfield(TT_ARG_READ ttfield o);
    
/** get `name' of ttfield `o' */
TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * 	TTGetName_ttfield(TT_ARG_READ ttfield o);
    
/** get `type' of ttfield `o' */
ttopaque 	TTGetType_ttfield(TT_ARG_READ ttfield o);










#endif /* _TT_TTFIELD_H */

