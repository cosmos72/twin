




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttobj.h  --  public header for libTT declarations related to "ttobj"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTOBJ_H
#define _TT_TTOBJ_H







/* object structure */
#ifndef s_ttobj


/** ttobj: The base class of all objects */        
        struct s_ttobj;
typedef struct s_ttobj *	ttobj;

#endif /* s_ttobj */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttobj */
extern ttclass	TTClass_ttobj;





/* ttobj methods */
                    
/** get `which' field value from ttobj `o' */
ttbyte TTGetField_ttobj(TT_ARG_READ ttobj o,TT_ARG_READ ttfield which,TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * value);

/** set `which' field to `value' into ttobj `o' */
ttbyte TTSetField_ttobj(ttobj o,TT_ARG_READ ttfield which,TT_ARG_READ TT_ARG_ARRAY((1)) ttarg * value);

/** change `which' field value into ttobj `o' */
ttbyte TTChangeField_ttobj(ttobj o,TT_ARG_READ ttfield which,ttany nand_value,ttany xor_value);









/* TTGet*_ttobj and TTSet*_ttobj methods */
        









#endif /* _TT_TTOBJ_H */

