




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttslider.h  --  public header for libTT declarations related to "ttslider"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTSLIDER_H
#define _TT_TTSLIDER_H







/* object structure */
#ifndef s_ttslider


/** ttslider: a sliding knob within a bounded interval */            
        struct s_ttslider;
typedef struct s_ttslider *	ttslider;

#endif /* s_ttslider */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttslider */
extern ttclass	TTClass_ttslider;





/* ttslider methods */

/** set the slide minimum of the ttslider `o' */    
ttbyte TTSetSlideMin_ttslider(ttslider o,ttint slide_min);

/** set the slide maximum of the ttslider `o' */    
ttbyte TTSetSlideMax_ttslider(ttslider o,ttint slide_max);

/** set the slide position of the ttslider `o' */    
ttbyte TTSetSlideValue_ttslider(ttslider o,ttint slide_value);

/** create a ttslider with given fields */
ttslider TTCreate_ttslider(ttbyte orientation,ttint real_size);









/* TTGet*_ttslider and TTSet*_ttslider methods */
    
/** get `slide_min' of ttslider `o' */
ttint 	TTGetSlideMin_ttslider(TT_ARG_READ ttslider o);

/** set `slide_min' of ttslider `o' */
ttbyte 	TTSetSlideMin_ttslider(ttslider o, ttint slide_min);
    
/** get `slide_max' of ttslider `o' */
ttint 	TTGetSlideMax_ttslider(TT_ARG_READ ttslider o);

/** set `slide_max' of ttslider `o' */
ttbyte 	TTSetSlideMax_ttslider(ttslider o, ttint slide_max);
    
/** get `slide_value' of ttslider `o' */
ttint 	TTGetSlideValue_ttslider(TT_ARG_READ ttslider o);

/** set `slide_value' of ttslider `o' */
ttbyte 	TTSetSlideValue_ttslider(ttslider o, ttint slide_value);










#endif /* _TT_TTSLIDER_H */

