




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttbitmask.h  --  public header for libTT declarations related to "ttbitmask"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTBITMASK_H
#define _TT_TTBITMASK_H







/* object structure */
#ifndef s_ttbitmask


/** ttbitmask: a bitmap mask of generic (ttany) values;
 * used, among others, to hold bitmaps of ttevent evtypes, evcodes and
 * components to block specific ttevents while calling event listeners.
 */                
        struct s_ttbitmask;
typedef struct s_ttbitmask *	ttbitmask;

#endif /* s_ttbitmask */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttbitmask */
extern ttclass	TTClass_ttbitmask;





/* ttbitmask methods */

/** create a ttbitmask with given value */
ttbitmask TTCreate_ttbitmask(ttany value);

/** create a ttbitmask equal to given one */
ttbitmask TTCreateB2_ttbitmask(TT_ARG_READ ttbitmask o);

/** create a ttbitmask with given `value' array and swallow `value' */
ttbitmask TTCreateR_ttbitmask(TT_ARG_DIE ttvector value);

/** create a ttbitmask with a copy of given `value' array */
ttbitmask TTCreateR2_ttbitmask(TT_ARG_READ ttvector value);

/** initialize the ttbitmask `o' to empty */
void TTClear_ttbitmask(ttbitmask o);

/** initialize the ttbitmask `o' to full */
void TTFill_ttbitmask(ttbitmask o);

/** negate (i.e. make complement of) the ttbitmask `o' */
void TTNegate_ttbitmask(ttbitmask o);

/** change ttbitmask `o' applying given truth table (i.e. logical operation) */
void TTChange_ttbitmask(ttbitmask o,ttuint truth_table);

/** check whether `value' is member of the ttbitmask `o' */
ttbyte TTContains_ttbitmask(TT_ARG_READ ttbitmask o,ttany value);

/** add `value' to the ttbitmask `o' */
ttbyte TTAdd_ttbitmask(ttbitmask o,ttany value);

/** add `value' bitmask to the ttbitmask `o' */
ttbyte TTAddB_ttbitmask(ttbitmask o,TT_ARG_READ ttbitmask value);

/** add `value' array to the ttbitmask `o' */
ttbyte TTAddR_ttbitmask(ttbitmask o,TT_ARG_READ ttvector value);

/** remove `value' from the ttbitmask `o' */
ttbyte TTRemove_ttbitmask(ttbitmask o,ttany value);

/** remove `value' bitmask from the ttbitmask `o' */
ttbyte TTRemoveB_ttbitmask(ttbitmask o,TT_ARG_READ ttbitmask value);

/** remove `value' array from the ttbitmask `o' */
ttbyte TTRemoveR_ttbitmask(ttbitmask o,TT_ARG_READ ttvector value);

/** combine ttbitmask `o' with `value' appling given truth table (i.e. logical operation) */
ttbyte TTCombine_ttbitmask(ttbitmask o,ttany value,ttuint truth_table);

/** combine ttbitmask `o' with `value' array appling given truth table (i.e. logical operation) */
ttbyte TTCombineR_ttbitmask(ttbitmask o,TT_ARG_READ ttvector value,ttuint truth_table);

/** combine ttbitmask `o' with `value' appling given truth table (i.e. logical operation) */
ttbyte TTCombineB_ttbitmask(ttbitmask o,TT_ARG_READ ttbitmask value,ttuint truth_table);









/* TTGet*_ttbitmask and TTSet*_ttbitmask methods */
                









#endif /* _TT_TTBITMASK_H */

