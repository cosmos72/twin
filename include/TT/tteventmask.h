




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  tteventmask.h  --  public header for libTT declarations related to "tteventmask"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTEVENTMASK_H
#define _TT_TTEVENTMASK_H







/* object structure */
#ifndef s_tteventmask


/** tteventmask: a set of ttbitmasks used to block
 * events matching certain criteria */                
        struct s_tteventmask;
typedef struct s_tteventmask *	tteventmask;

#endif /* s_tteventmask */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of tteventmask */
extern ttclass	TTClass_tteventmask;





/* tteventmask methods */

/** create a tteventmask with given fields and swallow the bitmasks */
tteventmask TTCreateB_tteventmask(TT_ARG_DIE ttbitmask evtype_mask,TT_ARG_DIE ttbitmask evcode_mask,TT_ARG_DIE ttbitmask component_mask);

/** create a tteventmask with a copy of given bitbasks */
tteventmask TTCreateB2_tteventmask(TT_ARG_READ ttbitmask evtype_mask,TT_ARG_READ ttbitmask evcode_mask,TT_ARG_READ ttbitmask component_mask);

/** set the mask of blocked event types and swallow the evtype bitmask */
ttbyte TTSetEvtypeMask_tteventmask(tteventmask o,TT_ARG_DIE ttbitmask evtype_mask);

/** set the mask of blocked event codes and swallow the evtype bitmask */
ttbyte TTSetEvcodeMask_tteventmask(tteventmask o,TT_ARG_DIE ttbitmask evcode_mask);

/** set the mask of blocked event types and swallow the evtype bitmask */
ttbyte TTSetComponentMask_tteventmask(tteventmask o,TT_ARG_DIE ttbitmask component_mask);

/** set the truth table to combine masks of evtypes, evcodes and components of blocked events */
ttbyte TTSetTruthTable_tteventmask(tteventmask o,ttuint truth_table);









/* TTGet*_tteventmask and TTSet*_tteventmask methods */
    
/** get `evtype_mask' of tteventmask `o' */
ttbitmask 	TTGetEvtypeMask_tteventmask(TT_ARG_READ tteventmask o);

/** set `evtype_mask' of tteventmask `o' */
ttbyte 	TTSetEvtypeMask_tteventmask(tteventmask o, ttbitmask evtype_mask);
    
/** get `evcode_mask' of tteventmask `o' */
ttbitmask 	TTGetEvcodeMask_tteventmask(TT_ARG_READ tteventmask o);

/** set `evcode_mask' of tteventmask `o' */
ttbyte 	TTSetEvcodeMask_tteventmask(tteventmask o, ttbitmask evcode_mask);
    
/** get `component_mask' of tteventmask `o' */
ttbitmask 	TTGetComponentMask_tteventmask(TT_ARG_READ tteventmask o);

/** set `component_mask' of tteventmask `o' */
ttbyte 	TTSetComponentMask_tteventmask(tteventmask o, ttbitmask component_mask);
    
/** get `truth_table' of tteventmask `o' */
ttuint 	TTGetTruthTable_tteventmask(TT_ARG_READ tteventmask o);

/** set `truth_table' of tteventmask `o' */
ttbyte 	TTSetTruthTable_tteventmask(tteventmask o, ttuint truth_table);










#endif /* _TT_TTEVENTMASK_H */

