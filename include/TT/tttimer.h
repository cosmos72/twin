




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  tttimer.h  --  public header for libTT declarations related to "tttimer"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTTIMER_H
#define _TT_TTTIMER_H







/* object structure */
#ifndef s_tttimer


/** tttimer: A ttlistener derivative for timer-based callbacks */                
        struct s_tttimer;
typedef struct s_tttimer *	tttimer;

#endif /* s_tttimer */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of tttimer */
extern ttclass	TTClass_tttimer;





/* tttimer methods */

/** create a tttimer with given fields */
tttimer TTCreateR_tttimer(ttcomponent r,ttuint lflags,TT_ARG_READ ttlistener_fn function,ttopaque arg_component_n,ttopaque arg_timer_n,TT_ARG_DIE ttvector args,ttany delay_t,ttany delay_f);

/** create a tttimer with given fields */
tttimer TTCreate_tttimer(ttcomponent r,ttuint lflags,TT_ARG_READ ttlistener_fn function,ttany arg,ttany delay_t,ttany delay_f);

/** set delay (seconds and fraction of seconds) */
void TTSetDelay_tttimer(tttimer o,ttany delay_t,ttany delay_s);

/** enable or disable tttimer `o' */    
void TTSetEnabled_tttimer(tttimer o,ttbyte enabled);

/** get whether tttimer `o' is enabled or disabled */    
ttbyte TTIsEnabled_tttimer(TT_ARG_READ tttimer o);









/* TTGet*_tttimer and TTSet*_tttimer methods */
    
/** get `delay_t' of tttimer `o' */
ttany 	TTGetDelayT_tttimer(TT_ARG_READ tttimer o);
    
/** get `delay_f' of tttimer `o' */
ttany 	TTGetDelayF_tttimer(TT_ARG_READ tttimer o);
    
/** get `timer_prev' of tttimer `o' */
tttimer 	TTGetTimerPrev_tttimer(TT_ARG_READ tttimer o);
    
/** get `timer_next' of tttimer `o' */
tttimer 	TTGetTimerNext_tttimer(TT_ARG_READ tttimer o);










#endif /* _TT_TTTIMER_H */

