




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttanyscroll.h  --  public header for libTT declarations related to "ttanyscroll"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTANYSCROLL_H
#define _TT_TTANYSCROLL_H







/* object structure */
#ifndef s_ttanyscroll


/** ttanyscroll: the abstract class all scroll-like widgets are derived from */                                
        struct s_ttanyscroll;
typedef struct s_ttanyscroll *	ttanyscroll;

#endif /* s_ttanyscroll */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttanyscroll */
extern ttclass	TTClass_ttanyscroll;





/* ttanyscroll methods */
        
/** set the orientation of the ttanyscroll `o' */    
ttbyte TTSetOrientation_ttanyscroll(ttanyscroll o,ttbyte orientation);

/** set the (virtual) size of the ttanyscroll `o' */    
ttbyte TTSetSize_ttanyscroll(ttanyscroll o,ttint size);

/** set the (real) size of the ttanyscroll `o' */    
ttbyte TTSetRealSize_ttanyscroll(ttanyscroll o,ttint real_size);

/** set the (virtual) view size of the ttanyscroll `o' */    
ttbyte TTSetViewSize_ttanyscroll(ttanyscroll o,ttint view_size);

/** set the (virtual) position of the ttanyscroll `o' */    
ttbyte TTSetPosition_ttanyscroll(ttanyscroll o,ttint position);









/* TTGet*_ttanyscroll and TTSet*_ttanyscroll methods */
    
/** get `orientation' of ttanyscroll `o' */
ttbyte 	TTGetOrientation_ttanyscroll(TT_ARG_READ ttanyscroll o);

/** set `orientation' of ttanyscroll `o' */
ttbyte 	TTSetOrientation_ttanyscroll(ttanyscroll o, ttbyte orientation);
    
/** get `size' of ttanyscroll `o' */
ttint 	TTGetSize_ttanyscroll(TT_ARG_READ ttanyscroll o);

/** set `size' of ttanyscroll `o' */
ttbyte 	TTSetSize_ttanyscroll(ttanyscroll o, ttint size);
    
/** get `real_size' of ttanyscroll `o' */
ttint 	TTGetRealSize_ttanyscroll(TT_ARG_READ ttanyscroll o);

/** set `real_size' of ttanyscroll `o' */
ttbyte 	TTSetRealSize_ttanyscroll(ttanyscroll o, ttint real_size);
    
/** get `view_size' of ttanyscroll `o' */
ttint 	TTGetViewSize_ttanyscroll(TT_ARG_READ ttanyscroll o);

/** set `view_size' of ttanyscroll `o' */
ttbyte 	TTSetViewSize_ttanyscroll(ttanyscroll o, ttint view_size);
        
/** get `position' of ttanyscroll `o' */
ttint 	TTGetPosition_ttanyscroll(TT_ARG_READ ttanyscroll o);

/** set `position' of ttanyscroll `o' */
ttbyte 	TTSetPosition_ttanyscroll(ttanyscroll o, ttint position);
    
/** get `real_position' of ttanyscroll `o' */
ttint 	TTGetRealPosition_ttanyscroll(TT_ARG_READ ttanyscroll o);
    
/** get `state' of ttanyscroll `o' */
ttuint 	TTGetState_ttanyscroll(TT_ARG_READ ttanyscroll o);










#endif /* _TT_TTANYSCROLL_H */

