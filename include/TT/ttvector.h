




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttvector.h  --  public header for libTT declarations related to "ttvector"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTVECTOR_H
#define _TT_TTVECTOR_H







/* object structure */
#ifndef s_ttvector


/** ttvector: A generic vector (array) */            
        struct s_ttvector;
typedef struct s_ttvector *	ttvector;

#endif /* s_ttvector */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttvector */
extern ttclass	TTClass_ttvector;





/* ttvector methods */

/** get vector element at given position */    
ttany TTGetAt_ttvector(TT_ARG_READ ttvector o,ttopaque pos);

/** set vector element at given position */    
ttbyte TTSetAt_ttvector(ttvector o,ttopaque pos,ttany value);

/** create vector containing given value */
ttvector TTCreate1_ttvector(ttany value);

/** create vector containing given values */
ttvector TTCreateA_ttvector(ttopaque value_n,TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttarg * values);

/** create vector containing given values */
ttvector TTCreateL_ttvector(ttopaque value_n,... );

/** create vector containing given values */
ttvector TTCreateR_ttvector(TT_ARG_READ ttvector v);

/** create vector containing given values */
ttvector TTCreateV_ttvector(ttopaque value_n,TT_ARG_ARRAY((1)) va_list * values);

/** create vector containing given values */
ttvector TTCreateY_ttvector(ttopaque value_n,TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttany * values);

/** add `value' at the end of the vector */
ttbyte TTAppend_ttvector(ttvector o,ttany value);

/** add `value' at given pos of the vector */
ttbyte TTAdd_ttvector(ttvector o,ttopaque pos,ttany value);

/** add values at given position of ttvector `o' */
ttbyte TTAddA_ttvector(ttvector o,ttopaque pos,ttopaque value_n,TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttarg * values);

/** add values at given position of ttvector `o' */
ttbyte TTAddL_ttvector(ttvector o,ttopaque pos,ttopaque value_n,... );

/** add values at given position of ttvector `o' */
ttbyte TTAddR_ttvector(ttvector o,ttopaque pos,TT_ARG_READ ttvector v);

/** add values at given position of ttvector `o' */
ttbyte TTAddV_ttvector(ttvector o,ttopaque pos,ttopaque value_n,TT_ARG_ARRAY((1)) va_list * values);

/** add values at given position of ttvector `o' */    
ttbyte TTAddY_ttvector(ttvector o,ttopaque pos,ttopaque value_n,TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttany * values);

/** search for the first occurrence of `value' in the vector */    
ttopaque TTContainsValue_ttvector(TT_ARG_READ ttvector o,ttopaque pos_start,ttany value);

/** remove value at given pos of the vector */
ttbyte TTRemoveAt_ttvector(ttvector o,ttopaque pos);

/** remove given range of values from ttvector `o' */    
ttbyte TTRemoveRange_ttvector(ttvector o,ttopaque pos_start,ttopaque pos_n);









/* TTGet*_ttvector and TTSet*_ttvector methods */
    
/** get `array_size' of ttvector `o' */
ttopaque 	TTGetArraySize_ttvector(TT_ARG_READ ttvector o);
    
/** get `array_n' of ttvector `o' */
ttopaque 	TTGetArrayN_ttvector(TT_ARG_READ ttvector o);
    
/** get `array' of ttvector `o' */
TT_ARG_ARRAY((_F(array_n))) ttany * 	TTGetArray_ttvector(TT_ARG_READ ttvector o);










#endif /* _TT_TTVECTOR_H */

