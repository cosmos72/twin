




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

/* global methods structures */


extern ttfn	TTFN_ttobj;  
extern ttfn	TTFN_ttevent; 
extern ttfn	TTFN_tteventbig; 
extern ttfn	TTFN_ttlistener; 
extern ttfn	TTFN_ttcallback; 
extern ttfn	TTFN_ttcomponent; 
extern ttfn	TTFN_ttvisible; 
extern ttfn	TTFN_ttnative; 
extern ttfn	TTFN_ttwidget; 
extern ttfn	TTFN_ttlabel; 
extern ttfn	TTFN_ttanybutton; 
extern ttfn	TTFN_ttbutton; 
extern ttfn	TTFN_ttcheckbutton; 
extern ttfn	TTFN_ttradiobutton; 
extern ttfn	TTFN_ttscrollbar; 
extern ttfn	TTFN_ttbuttongroup; 
extern ttfn	TTFN_ttwindow; 
extern ttfn	TTFN_ttframe; 
extern ttfn	TTFN_ttscroller; 
extern ttfn	TTFN_ttmenuitem; 
extern ttfn	TTFN_ttcheckmenuitem; 
extern ttfn	TTFN_ttradiomenuitem; 
extern ttfn	TTFN_ttmenu; 
extern ttfn	TTFN_ttmenubar; 
extern ttfn	TTFN_ttanytext; 
extern ttfn	TTFN_tttextfield; 
extern ttfn	TTFN_tttextarea; 
extern ttfn	TTFN_tttheme; 
extern ttfn	TTFN_ttapplication;

extern ttfn	TTFN_ttfn;

/* #defines for objects hierarchy */



                              
#define TTsuper_ttevent	ttobj                     
#define TTsuper_tteventbig	ttevent                         
#define TTsuper_ttlistener	ttobj                 
#define TTsuper_ttcallback	ttlistener                             
#define TTsuper_ttcomponent	ttobj         
#define TTsuper_ttvisible	ttcomponent                                 
#define TTsuper_ttnative	ttvisible 
#define TTsuper_ttwidget	ttvisible                         
#define TTsuper_ttlabel	ttwidget         
#define TTsuper_ttanybutton	ttwidget                 
#define TTsuper_ttbutton	ttanybutton 
#define TTsuper_ttcheckbutton	ttbutton 
#define TTsuper_ttradiobutton	ttcheckbutton 
#define TTsuper_ttscrollbar	ttanybutton 
#define TTsuper_ttbuttongroup	ttcomponent 
#define TTsuper_ttwindow	ttwidget     
#define TTsuper_ttframe	ttwindow 
#define TTsuper_ttscroller	ttwindow         
#define TTsuper_ttmenuitem	ttvisible     
#define TTsuper_ttcheckmenuitem	ttmenuitem 
#define TTsuper_ttradiomenuitem	ttcheckmenuitem 
#define TTsuper_ttmenu	ttmenuitem
     
#define TTsuper_ttmenubar	ttvisible 
#define TTsuper_ttanytext	ttwindow         
#define TTsuper_tttextfield	ttanytext 
#define TTsuper_tttextarea	tttextfield 
#define TTsuper_tttheme	ttcomponent
            
     
#define TTsuper_ttapplication	ttcomponent    






/* public and exported methods */









/* ttobj public and exported methods */                                
ttbyte TTGetValue_ttobj(ttobj o, ttuint which, ttany *value);    
ttbyte TTSetValue_ttobj(ttobj o, ttuint which, ttany value);    
ttbyte TTChangeValue_ttobj(ttobj o, ttuint which, ttany nand_value, ttany xor_value);    
void TTRef_ttobj(ttobj o);    
void TTUnref_ttobj(ttobj o);  

/* ttevent public and exported methods */    
ttevent TTCreate_ttevent(ttuint evtype, ttuint evcode, ttuint evflags); 

/* tteventbig public and exported methods */
    
tteventbig TTCreate_tteventbig(ttuint evtype, ttuint evcode, ttuint evflags             ,ttshort x, ttshort y, ttshort w, ttshort h, ttuint len, TT_CONST ttbyte *data); 

/* ttlistener public and exported methods */
     

/* ttcallback public and exported methods */    
ttcallback TTCreate_ttcallback(ttcomponent o, ttuint evtype, ttuint flags, ttcallback_fn function, ttany arg);    
ttcallback TTCreateA_ttcallback(ttcomponent o, ttevent ev, ttuint flags, ttuint narg_component, ttuint narg_event, ttuint nargs, ttcallback_fn function, ttany TT_CONST *args);    
ttcallback TTCreateV_ttcallback(ttcomponent o, ttevent ev, ttuint flags, ttuint narg_component, ttuint narg_event, ttuint nargs, ttcallback_fn function, va_list *vargs); 

/* ttcomponent public and exported methods */ 

/* ttvisible public and exported methods */    
void TTAddTo_ttvisible(ttvisible o, ttvisible parent);    
void TTRemove_ttvisible(ttvisible o);    
void TTSetVisible_ttvisible(ttvisible o, ttbyte on_off);    
void TTSetTheme_ttvisible(ttvisible o, tttheme theme);    
void TTBuiltinRepaint_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h);    
void TTSetRepaint_ttvisible(ttvisible o, ttvisible_repaint_fn repaint);    
void TTAdd_ttvisible(ttvisible o, ttvisible child);    
void TTExpose_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h); 

/* ttnative public and exported methods */    
ttnative TTGetRoot_ttnative(void);    
ttnative TTCreate_ttnative(ttany); 

/* ttwidget public and exported methods */    
void TTSetXYWH_ttwidget(ttwidget o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);    
void TTSetXlYl_ttwidget(ttwidget o, ttuint mask, ttint xl, ttint yl);    
void TTSetXY_ttwidget(ttwidget o, ttshort x, ttshort y);    
void TTSetWH_ttwidget(ttwidget o, ttshort w, ttshort h);    
void TTSetXl_ttwidget(ttwidget o, ttint xl);    
void TTSetYl_ttwidget(ttwidget o, ttint yl); 

/* ttlabel public and exported methods */    
ttbyte TTSetText_ttlabel(ttlabel o, TT_CONST ttbyte * text);    
ttlabel TTCreate_ttlabel(TT_CONST ttbyte * text); 

/* ttanybutton public and exported methods */    
ttbyte TTSetText_ttanybutton(ttanybutton o, TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch);    
ttanybutton TTCreate_ttanybutton(TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch); 

/* ttbutton public and exported methods */    
void TTSetPressed_ttbutton(ttbutton o, ttbyte pressed);    
ttbutton TTCreate_ttbutton(TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch);    
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
ttmenubar TTGetDefault_ttmenubar(void); 

/* ttanytext public and exported methods */ 

/* tttextfield public and exported methods */ 

/* tttextarea public and exported methods */ 

/* tttheme public and exported methods */        
tttheme TTGetDefault_tttheme(void); 

/* ttapplication public and exported methods */    
ttapplication TTSet_ttapplication(TT_CONST ttbyte * name);






#endif /* _TT_DEFSM4_H */

