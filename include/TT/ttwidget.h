




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttwidget.h  --  public header for libTT declarations related to "ttwidget"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTWIDGET_H
#define _TT_TTWIDGET_H







/* object structure */
#ifndef s_ttwidget


/** ttwidget: The superclass of all window-like visible containers */                                            
        struct s_ttwidget;
typedef struct s_ttwidget *	ttwidget;

#endif /* s_ttwidget */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttwidget */
extern ttclass	TTClass_ttwidget;





/* ttwidget methods */

/** set the layout of the ttvisible `o' */    
void TTSetLayout_ttwidget(ttwidget o,ttlayout l);

/** set the position `x' and `y' of the ttwidget `o' */    
ttbyte TTSetXY_ttwidget(ttwidget o,ttshort x,ttshort y);

/** set the size `w' and `h' of the ttwidget `o' */    
ttbyte TTSetWH_ttwidget(ttwidget o,ttshort w,ttshort h);

/** set the inside scroll position `xl' of the ttwidget `o' */    
ttbyte TTSetXl_ttwidget(ttwidget o,ttint xl);

/** set the inside scroll position `yl' of the ttwidget `o' */    
ttbyte TTSetYl_ttwidget(ttwidget o,ttint yl);

/** set the inside scroll size `wl' of the ttwidget `o' */    
ttbyte TTSetWl_ttwidget(ttwidget o,ttint wl);

/** set the inside scroll size `hl' of the ttwidget `o' */    
ttbyte TTSetHl_ttwidget(ttwidget o,ttint hl);

/** set the the tttooltip of the ttwidget `o' */    
ttbyte TTSetTooltip_ttwidget(ttwidget o,TT_ARG_DIE tttooltip t);

/** set some (depeding on `mask') of the inside scroll position `xl' and `yl' of the ttwidget `o' */
ttbyte TTSetXlYl_ttwidget(ttwidget o,ttuint mask,ttint xl,ttint yl);

/** set some (depeding on `mask') of the inside scroll sizes `wl' and `hl' of the ttwidget `o' */
ttbyte TTSetWlHl_ttwidget(ttwidget o,ttuint mask,ttint wl,ttint hl);

/** set some (depeding on `mask') of the position `x' and `y' and the size `w' and `h' of the ttwidget `o' */
ttbyte TTSetXYWH_ttwidget(ttwidget o,ttuint mask,ttshort x,ttshort y,ttshort w,ttshort h);

/** set the position `x' of the ttwidget `o' */
ttbyte TTSetX_ttwidget(ttwidget o,ttshort x);

/** set the position `y' of the ttwidget `o' */
ttbyte TTSetY_ttwidget(ttwidget o,ttshort y);

/** set the size `w' of the ttwidget `o' */
ttbyte TTSetW_ttwidget(ttwidget o,ttshort w);

/** set the size `h' of the ttwidget `o' */
ttbyte TTSetH_ttwidget(ttwidget o,ttshort h);









/* TTGet*_ttwidget and TTSet*_ttwidget methods */
    
/** get `x' of ttwidget `o' */
ttshort 	TTGetX_ttwidget(TT_ARG_READ ttwidget o);

/** set `x' of ttwidget `o' */
ttbyte 	TTSetX_ttwidget(ttwidget o, ttshort x);
    
/** get `y' of ttwidget `o' */
ttshort 	TTGetY_ttwidget(TT_ARG_READ ttwidget o);

/** set `y' of ttwidget `o' */
ttbyte 	TTSetY_ttwidget(ttwidget o, ttshort y);
    
/** get `w' of ttwidget `o' */
ttshort 	TTGetW_ttwidget(TT_ARG_READ ttwidget o);

/** set `w' of ttwidget `o' */
ttbyte 	TTSetW_ttwidget(ttwidget o, ttshort w);
    
/** get `h' of ttwidget `o' */
ttshort 	TTGetH_ttwidget(TT_ARG_READ ttwidget o);

/** set `h' of ttwidget `o' */
ttbyte 	TTSetH_ttwidget(ttwidget o, ttshort h);
    
/** get `layout' of ttwidget `o' */
ttlayout 	TTGetLayout_ttwidget(TT_ARG_READ ttwidget o);
    
/** get `xl' of ttwidget `o' */
ttint 	TTGetXl_ttwidget(TT_ARG_READ ttwidget o);

/** set `xl' of ttwidget `o' */
ttbyte 	TTSetXl_ttwidget(ttwidget o, ttint xl);
    
/** get `yl' of ttwidget `o' */
ttint 	TTGetYl_ttwidget(TT_ARG_READ ttwidget o);

/** set `yl' of ttwidget `o' */
ttbyte 	TTSetYl_ttwidget(ttwidget o, ttint yl);
    
/** get `wl' of ttwidget `o' */
ttint 	TTGetWl_ttwidget(TT_ARG_READ ttwidget o);

/** set `wl' of ttwidget `o' */
ttbyte 	TTSetWl_ttwidget(ttwidget o, ttint wl);
    
/** get `hl' of ttwidget `o' */
ttint 	TTGetHl_ttwidget(TT_ARG_READ ttwidget o);

/** set `hl' of ttwidget `o' */
ttbyte 	TTSetHl_ttwidget(ttwidget o, ttint hl);
    
/** get `col' of ttwidget `o' */
ttcol 	TTGetCol_ttwidget(TT_ARG_READ ttwidget o);
    
/** get `tooltip' of ttwidget `o' */
tttooltip 	TTGetTooltip_ttwidget(TT_ARG_READ ttwidget o);

/** set `tooltip' of ttwidget `o' */
ttbyte 	TTSetTooltip_ttwidget(ttwidget o, tttooltip tooltip);










#endif /* _TT_TTWIDGET_H */

