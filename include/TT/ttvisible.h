




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttvisible.h  --  public header for libTT declarations related to "ttvisible"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTVISIBLE_H
#define _TT_TTVISIBLE_H







/* object structure */
#ifndef s_ttvisible


/** ttvisible: The superclass of all visible objects */                                    
        struct s_ttvisible;
typedef struct s_ttvisible *	ttvisible;

#endif /* s_ttvisible */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttvisible */
extern ttclass	TTClass_ttvisible;





/* ttvisible methods */

/** add the ttvisible `o' into `parent' */    
void TTAddTo_ttvisible(ttvisible o,ttvisible parent,ttany constraint);

/** apply layout the children of current component */    
void TTValidate_ttvisible(ttvisible o);

/** remove the ttvisible `o' from its parent */    
void TTRemove_ttvisible(ttvisible o);

/** invalidate ttvisible `o' internal caches (to force redrawing it) */    
void TTInvalidate_ttvisible(ttvisible o);

/** set the ttvisible `o' to be visible or not */    
void TTSetVisible_ttvisible(ttvisible o,ttbyte visible);

/** set the tttheme `theme' of the ttvisible `o' */    
ttbyte TTSetTheme_ttvisible(ttvisible o,TT_ARG_READ tttheme theme);

/** actually draw (part of) the contents of the ttvisible `o' - internally called by TTBuiltinRepaint_ttvisible */    
void TTDraw_ttvisible(ttvisible o,ttshort x,ttshort y,ttshort w,ttshort h,ttshort pitch,TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata,TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata,TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);

/** internal libTT method that repaints (part of) the ttvisible `o' using its theme */    
void TTBuiltinRepaint_ttvisible(ttvisible o,ttshort x,ttshort y,ttshort w,ttshort h);


/** change the internal libTT method used to repaint (part of) the ttvisible `o' */
ttbyte TTSetRepaint_ttvisible(ttvisible o,TT_ARG_READ ttvisible_repaint_fn repaint);

/** request a repaint for the given part of the ttvisible `o' */
void TTExpose_ttvisible(ttvisible o,ttshort x,ttshort y,ttshort w,ttshort h);

/** add the ttvisible `child' into `o' */
void TTAdd_ttvisible(ttvisible o,ttvisible child,ttany constraint);









/* TTGet*_ttvisible and TTSet*_ttvisible methods */
    
/** get `vflags' of ttvisible `o' */
ttuint 	TTGetVflags_ttvisible(TT_ARG_READ ttvisible o);
    
/** get `prev' of ttvisible `o' */
ttvisible 	TTGetPrev_ttvisible(TT_ARG_READ ttvisible o);
    
/** get `next' of ttvisible `o' */
ttvisible 	TTGetNext_ttvisible(TT_ARG_READ ttvisible o);
    
/** get `parent' of ttvisible `o' */
ttvisible 	TTGetParent_ttvisible(TT_ARG_READ ttvisible o);
    
/** get `child_first' of ttvisible `o' */
ttvisible 	TTGetChildFirst_ttvisible(TT_ARG_READ ttvisible o);
    
/** get `child_last' of ttvisible `o' */
ttvisible 	TTGetChildLast_ttvisible(TT_ARG_READ ttvisible o);
    
/** get `constraint' of ttvisible `o' */
ttany 	TTGetConstraint_ttvisible(TT_ARG_READ ttvisible o);
    
/** get `theme' of ttvisible `o' */
tttheme 	TTGetTheme_ttvisible(TT_ARG_READ ttvisible o);

/** set `theme' of ttvisible `o' */
ttbyte 	TTSetTheme_ttvisible(ttvisible o, tttheme theme);
    
/** get `repaint' of ttvisible `o' */
ttvisible_repaint_fn 	TTGetRepaint_ttvisible(TT_ARG_READ ttvisible o);

/** set `repaint' of ttvisible `o' */
ttbyte 	TTSetRepaint_ttvisible(ttvisible o, ttvisible_repaint_fn repaint);










#endif /* _TT_TTVISIBLE_H */

