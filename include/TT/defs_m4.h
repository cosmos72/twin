




/* This file was automatically generated from m4/defs.m4, do not edit! */


/*
 *  defs_m4.h  --  public header for libTT types declarations
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TT_DEFSM4_H
#define _TT_DEFSM4_H













#define TT_LIST(el) \
el(ttobj)  el(ttevent) el(tteventbig) el(ttlistener) el(ttcallback) el(ttcomponent) el(ttvisible) el(ttnative) el(ttwidget) el(ttlabel) el(ttanybutton) el(ttbutton) el(ttcheckbutton) el(ttradiobutton) el(ttscrollbar) el(ttbuttongroup) el(ttwindow) el(ttframe) el(ttscroller) el(ttmenuitem) el(ttcheckmenuitem) el(ttradiomenuitem) el(ttmenu) el(ttmenubar) el(ttanytext) el(tttextfield) el(tttextarea) el(tttheme) el(ttapplication)

#define TT_NLIST(el) \
 el(ttevent) el(tteventbig) el(ttlistener) el(ttcallback) el(ttcomponent) el(ttvisible) el(ttnative) el(ttwidget) el(ttlabel) el(ttanybutton) el(ttbutton) el(ttcheckbutton) el(ttradiobutton) el(ttscrollbar) el(ttbuttongroup) el(ttwindow) el(ttframe) el(ttscroller) el(ttmenuitem) el(ttcheckmenuitem) el(ttradiomenuitem) el(ttmenu) el(ttmenubar) el(ttanytext) el(tttextfield) el(tttextarea) el(tttheme) el(ttapplication)

#define TT_TYPELIST(el) \
el(ttbyte) el(ttshort) el(ttint) el(ttfont) el(ttcol) el(ttattr) el(ttopaque) el(ttany) el(ttpointer) el(ttstring) el(ttfunction)

/* typedefs for objects structures */


typedef struct s_ttobj *	ttobj;  
typedef struct s_ttevent *	ttevent; 
typedef struct s_tteventbig *	tteventbig; 
typedef struct s_ttlistener *	ttlistener; 
typedef struct s_ttcallback *	ttcallback; 
typedef struct s_ttcomponent *	ttcomponent; 
typedef struct s_ttvisible *	ttvisible; 
typedef struct s_ttnative *	ttnative; 
typedef struct s_ttwidget *	ttwidget; 
typedef struct s_ttlabel *	ttlabel; 
typedef struct s_ttanybutton *	ttanybutton; 
typedef struct s_ttbutton *	ttbutton; 
typedef struct s_ttcheckbutton *	ttcheckbutton; 
typedef struct s_ttradiobutton *	ttradiobutton; 
typedef struct s_ttscrollbar *	ttscrollbar; 
typedef struct s_ttbuttongroup *	ttbuttongroup; 
typedef struct s_ttwindow *	ttwindow; 
typedef struct s_ttframe *	ttframe; 
typedef struct s_ttscroller *	ttscroller; 
typedef struct s_ttmenuitem *	ttmenuitem; 
typedef struct s_ttcheckmenuitem *	ttcheckmenuitem; 
typedef struct s_ttradiomenuitem *	ttradiomenuitem; 
typedef struct s_ttmenu *	ttmenu; 
typedef struct s_ttmenubar *	ttmenubar; 
typedef struct s_ttanytext *	ttanytext; 
typedef struct s_tttextfield *	tttextfield; 
typedef struct s_tttextarea *	tttextarea; 
typedef struct s_tttheme *	tttheme; 
typedef struct s_ttapplication *	ttapplication;


/* typedefs for methods structures (runtime types) */
typedef struct s_ttfn *	ttfn;

/* useful types */
typedef void (*ttcallback_fn)(ttuint nargs, ttany *args);
typedef void (*ttvisible_repaint_fn)(ttvisible,ttshort,ttshort,ttshort,ttshort);

/* globally available runtime class types; returned by TTCLASSOF(<some object>) */


/** class of ttobj */
extern ttfn	TTFN_ttobj;  
/** class of ttevent */
extern ttfn	TTFN_ttevent; 
/** class of tteventbig */
extern ttfn	TTFN_tteventbig; 
/** class of ttlistener */
extern ttfn	TTFN_ttlistener; 
/** class of ttcallback */
extern ttfn	TTFN_ttcallback; 
/** class of ttcomponent */
extern ttfn	TTFN_ttcomponent; 
/** class of ttvisible */
extern ttfn	TTFN_ttvisible; 
/** class of ttnative */
extern ttfn	TTFN_ttnative; 
/** class of ttwidget */
extern ttfn	TTFN_ttwidget; 
/** class of ttlabel */
extern ttfn	TTFN_ttlabel; 
/** class of ttanybutton */
extern ttfn	TTFN_ttanybutton; 
/** class of ttbutton */
extern ttfn	TTFN_ttbutton; 
/** class of ttcheckbutton */
extern ttfn	TTFN_ttcheckbutton; 
/** class of ttradiobutton */
extern ttfn	TTFN_ttradiobutton; 
/** class of ttscrollbar */
extern ttfn	TTFN_ttscrollbar; 
/** class of ttbuttongroup */
extern ttfn	TTFN_ttbuttongroup; 
/** class of ttwindow */
extern ttfn	TTFN_ttwindow; 
/** class of ttframe */
extern ttfn	TTFN_ttframe; 
/** class of ttscroller */
extern ttfn	TTFN_ttscroller; 
/** class of ttmenuitem */
extern ttfn	TTFN_ttmenuitem; 
/** class of ttcheckmenuitem */
extern ttfn	TTFN_ttcheckmenuitem; 
/** class of ttradiomenuitem */
extern ttfn	TTFN_ttradiomenuitem; 
/** class of ttmenu */
extern ttfn	TTFN_ttmenu; 
/** class of ttmenubar */
extern ttfn	TTFN_ttmenubar; 
/** class of ttanytext */
extern ttfn	TTFN_ttanytext; 
/** class of tttextfield */
extern ttfn	TTFN_tttextfield; 
/** class of tttextarea */
extern ttfn	TTFN_tttextarea; 
/** class of tttheme */
extern ttfn	TTFN_tttheme; 
/** class of ttapplication */
extern ttfn	TTFN_ttapplication;

/** class of ttfn */
extern ttfn	TTFN_ttfn;

/* #defines for objects hierarchy */



                              
/** the superclass of ttevent */
#define TTsuper_ttevent	ttobj                     
/** the superclass of tteventbig */
#define TTsuper_tteventbig	ttevent                         
/** the superclass of ttlistener */
#define TTsuper_ttlistener	ttobj                 
/** the superclass of ttcallback */
#define TTsuper_ttcallback	ttlistener                             
/** the superclass of ttcomponent */
#define TTsuper_ttcomponent	ttobj         
/** the superclass of ttvisible */
#define TTsuper_ttvisible	ttcomponent                                 
/** the superclass of ttnative */
#define TTsuper_ttnative	ttvisible 
/** the superclass of ttwidget */
#define TTsuper_ttwidget	ttvisible                         
/** the superclass of ttlabel */
#define TTsuper_ttlabel	ttwidget         
/** the superclass of ttanybutton */
#define TTsuper_ttanybutton	ttwidget                 
/** the superclass of ttbutton */
#define TTsuper_ttbutton	ttanybutton 
/** the superclass of ttcheckbutton */
#define TTsuper_ttcheckbutton	ttbutton 
/** the superclass of ttradiobutton */
#define TTsuper_ttradiobutton	ttcheckbutton 
/** the superclass of ttscrollbar */
#define TTsuper_ttscrollbar	ttanybutton 
/** the superclass of ttbuttongroup */
#define TTsuper_ttbuttongroup	ttcomponent 
/** the superclass of ttwindow */
#define TTsuper_ttwindow	ttwidget         
/** the superclass of ttframe */
#define TTsuper_ttframe	ttwindow 
/** the superclass of ttscroller */
#define TTsuper_ttscroller	ttwindow         
/** the superclass of ttmenuitem */
#define TTsuper_ttmenuitem	ttvisible     
/** the superclass of ttcheckmenuitem */
#define TTsuper_ttcheckmenuitem	ttmenuitem 
/** the superclass of ttradiomenuitem */
#define TTsuper_ttradiomenuitem	ttcheckmenuitem 
/** the superclass of ttmenu */
#define TTsuper_ttmenu	ttmenuitem
     
/** the superclass of ttmenubar */
#define TTsuper_ttmenubar	ttvisible 
/** the superclass of ttanytext */
#define TTsuper_ttanytext	ttwindow         
/** the superclass of tttextfield */
#define TTsuper_tttextfield	ttanytext 
/** the superclass of tttextarea */
#define TTsuper_tttextarea	tttextfield 
/** the superclass of tttheme */
#define TTsuper_tttheme	ttcomponent
            
     
/** the superclass of ttapplication */
#define TTsuper_ttapplication	ttcomponent    






/* public and exported methods */









/* ttobj public and exported methods */                            

/** get the value of `which' field from object `o' */    
ttfn TTGetValue_ttobj(ttobj o, ttuint which, ttany *value);

/** set the value of `which' field into object `o' to `value' */    
ttbyte TTSetValue_ttobj(ttobj o, ttuint which, ttany value);

/** change the value of `which' field into object `o' to `(old_value & ~nand_value) ^ xor_value' */    
ttbyte TTChangeValue_ttobj(ttobj o, ttuint which, ttany nand_value, ttany xor_value);

/** increase by 1 the reference count of object `o' */    
void TTRef_ttobj(ttobj o);

/** decrease by 1 the reference count of object `o' */    
void TTUnref_ttobj(ttobj o);  

/* ttevent public and exported methods */

/** create a ttevent with given `evtype', `evcode' and `evflags' */    
ttevent TTCreate_ttevent(ttuint evtype, ttuint evcode, ttuint evflags); 

/* tteventbig public and exported methods */


/** create a tteventbig with given fields */    
tteventbig TTCreate_tteventbig(ttuint evtype, ttuint evcode, ttuint evflags             ,ttshort x, ttshort y, ttshort w, ttshort h, ttuint len, TT_CONST ttbyte *data); 

/* ttlistener public and exported methods */
     

/* ttcallback public and exported methods */

/** create a ttcallback for a ttcomponent with given fields */    
ttcallback TTCreate_ttcallback(ttcomponent o, ttuint evtype, ttuint flags, ttcallback_fn function, ttany arg);

/** create a ttcallback for a ttcomponent with given fields */    
ttcallback TTCreateA_ttcallback(ttcomponent o, ttevent ev, ttuint flags, ttuint narg_component, ttuint narg_event, ttuint nargs, ttcallback_fn function, ttany TT_CONST *args);

/** create a ttcallback for a ttcomponent with given fields */    
ttcallback TTCreateV_ttcallback(ttcomponent o, ttevent ev, ttuint flags, ttuint narg_component, ttuint narg_event, ttuint nargs, ttcallback_fn function, va_list *vargs); 

/* ttcomponent public and exported methods */ 

/* ttvisible public and exported methods */

/** add the ttvisible `o' into `parent' */    
void TTAddTo_ttvisible(ttvisible o, ttvisible parent);

/** remove the ttvisible `o' from its parent */    
void TTRemove_ttvisible(ttvisible o);

/** set the ttvisible `o' to be visible or not */    
void TTSetVisible_ttvisible(ttvisible o, ttbyte on_off);

/** set the tttheme `theme' of the ttvisible `o' */    
void TTSetTheme_ttvisible(ttvisible o, tttheme theme);

/** actually draw (part of) the contents of the ttvisible `o' - internally called by TTBuiltinRepaint_ttvisible */    
void TTDraw_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_CONST ttbyte *asciidata, TT_CONST ttfont *fontdata, TT_CONST ttattr *attrdata);

/** internal libTT method that repaints (part of) the ttvisible `o' using its theme */    
void TTBuiltinRepaint_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h);

/** change the internal libTT method used to repaint (part of) the ttvisible `o' */    
void TTSetRepaint_ttvisible(ttvisible o, ttvisible_repaint_fn repaint);

/** add the ttvisible `child' into `o' */    
void TTAdd_ttvisible(ttvisible o, ttvisible child);

/** request a repaint for the given part of the ttvisible `o' */    
void TTExpose_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h); 

/* ttnative public and exported methods */

/** get the ttnative `root' object into which each top level ttvisible is added */    
ttnative TTGetRoot_ttnative(void);

/** create a ttnative with given fields */    
ttnative TTCreate_ttnative(ttany); 

/* ttwidget public and exported methods */

/** set some (depeding on `mask') of the position `x' and `y' and the size `w' and `h' of the ttwidget `o' */    
void TTSetXYWH_ttwidget(ttwidget o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);

/** set some (depeding on `mask') of the inside scroll position `xl' and `yl' of the ttwidget `o' */    
void TTSetXlYl_ttwidget(ttwidget o, ttuint mask, ttint xl, ttint yl);

/** set the position `x' and `y' of the ttwidget `o' */    
void TTSetXY_ttwidget(ttwidget o, ttshort x, ttshort y);

/** set the size `w' and `h' of the ttwidget `o' */    
void TTSetWH_ttwidget(ttwidget o, ttshort w, ttshort h);

/** set the inside scroll position `xl' of the ttwidget `o' */    
void TTSetXl_ttwidget(ttwidget o, ttint xl);

/** set the inside scroll position `yl' of the ttwidget `o' */    
void TTSetYl_ttwidget(ttwidget o, ttint yl); 

/* ttlabel public and exported methods */

/** set the text of the ttlabel `o' */    
ttbyte TTSetText_ttlabel(ttlabel o, TT_CONST ttbyte * text);

/** create a ttlabel with given text */    
ttlabel TTCreate_ttlabel(TT_CONST ttbyte * text); 

/* ttanybutton public and exported methods */

/** set the text of the ttanybutton `o' */    
ttbyte TTSetText_ttanybutton(ttanybutton o, TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch);

/** create a ttanybutton with given text */    
ttanybutton TTCreate_ttanybutton(TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch); 

/* ttbutton public and exported methods */

/** set whether the ttbutton `o' is pressed or not */    
void TTSetPressed_ttbutton(ttbutton o, ttbyte pressed);

/** create a ttbutton with given text */    
ttbutton TTCreate_ttbutton(TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch);

/** get whether the ttbutton `o' is pressed or not */    
ttbyte TTIsPressed_ttbutton(ttbutton o); 

/* ttcheckbutton public and exported methods */ 

/* ttradiobutton public and exported methods */ 

/* ttscrollbar public and exported methods */ 

/* ttbuttongroup public and exported methods */ 

/* ttwindow public and exported methods */ 

/* ttframe public and exported methods */ 

/* ttscroller public and exported methods */ 

/* ttmenuitem public and exported methods */ 

/* ttcheckmenuitem public and exported methods */ 

/* ttradiomenuitem public and exported methods */ 

/* ttmenu public and exported methods */ 

/* ttmenubar public and exported methods */

/** get the default ttmenubar used by ttwidget:s */    
ttmenubar TTGetDefault_ttmenubar(void); 

/* ttanytext public and exported methods */ 

/* tttextfield public and exported methods */ 

/* tttextarea public and exported methods */ 

/* tttheme public and exported methods */    

/** get the default tttheme used by ttvisible:s */    
tttheme TTGetDefault_tttheme(void); 

/* ttapplication public and exported methods */

/** create (only one allowed) a ttapplication with given name */    
ttapplication TTSet_ttapplication(TT_CONST ttbyte * name);






#endif /* _TT_DEFSM4_H */

