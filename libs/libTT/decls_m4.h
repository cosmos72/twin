



/* This file was automatically generated from m4/decls.m4, do not edit! */

/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

/*
 *  decls_m4.h  --  INTERNAL (!!) header for libTT types declarations
 *
 *  Note: you cannot include both this file and <TT/decls_m4.h>, as they conflict!
 */

#ifndef _TT_DECLSM4_H
#define _TT_DECLSM4_H

/*  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */





















/* many useful macros */

#define TT_OFFSETOF(type,field)	((size_t)&((type)0)->field)




/* forward declarations for objects structures */

 
typedef struct s_ttobj *	ttobj; 
typedef struct s_ttclass *	ttclass; 
typedef struct s_ttobject *	ttobject; 
typedef struct s_ttvector *	ttvector; 
typedef struct s_ttfield *	ttfield; 
typedef struct s_ttmethod *	ttmethod; 
typedef struct s_ttevent *	ttevent; 
typedef struct s_tteventbig *	tteventbig; 
typedef struct s_ttbitmask *	ttbitmask; 
typedef struct s_tteventmask *	tteventmask; 
typedef struct s_ttcallback *	ttcallback; 
typedef struct s_ttlistener *	ttlistener; 
typedef struct s_tttimer *	tttimer; 
typedef struct s_ttcomponent *	ttcomponent; 
typedef struct s_ttdata *	ttdata; 
typedef struct s_ttvisible *	ttvisible; 
typedef struct s_ttlayout *	ttlayout; 
typedef struct s_ttboxlayout *	ttboxlayout; 
typedef struct s_ttborderlayout *	ttborderlayout; 
typedef struct s_ttnative *	ttnative; 
typedef struct s_ttwidget *	ttwidget; 
typedef struct s_ttlabel *	ttlabel; 
typedef struct s_tttooltip *	tttooltip; 
typedef struct s_ttbuttongroup *	ttbuttongroup; 
typedef struct s_ttanybutton *	ttanybutton; 
typedef struct s_ttbutton *	ttbutton; 
typedef struct s_ttcheckbutton *	ttcheckbutton; 
typedef struct s_ttradiobutton *	ttradiobutton; 
typedef struct s_ttanyscroll *	ttanyscroll; 
typedef struct s_ttscrollbar *	ttscrollbar; 
typedef struct s_ttslider *	ttslider; 
typedef struct s_ttprogressbar *	ttprogressbar; 
typedef struct s_ttscrollpane *	ttscrollpane; 
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



/* forward declarations for methods structures */

 
typedef struct s_ttclass_ttobj *	ttclass_ttobj; 
typedef struct s_ttclass_ttclass *	ttclass_ttclass; 
typedef struct s_ttclass_ttobject *	ttclass_ttobject; 
typedef struct s_ttclass_ttvector *	ttclass_ttvector; 
typedef struct s_ttclass_ttfield *	ttclass_ttfield; 
typedef struct s_ttclass_ttmethod *	ttclass_ttmethod; 
typedef struct s_ttclass_ttevent *	ttclass_ttevent; 
typedef struct s_ttclass_tteventbig *	ttclass_tteventbig; 
typedef struct s_ttclass_ttbitmask *	ttclass_ttbitmask; 
typedef struct s_ttclass_tteventmask *	ttclass_tteventmask; 
typedef struct s_ttclass_ttcallback *	ttclass_ttcallback; 
typedef struct s_ttclass_ttlistener *	ttclass_ttlistener; 
typedef struct s_ttclass_tttimer *	ttclass_tttimer; 
typedef struct s_ttclass_ttcomponent *	ttclass_ttcomponent; 
typedef struct s_ttclass_ttdata *	ttclass_ttdata; 
typedef struct s_ttclass_ttvisible *	ttclass_ttvisible; 
typedef struct s_ttclass_ttlayout *	ttclass_ttlayout; 
typedef struct s_ttclass_ttboxlayout *	ttclass_ttboxlayout; 
typedef struct s_ttclass_ttborderlayout *	ttclass_ttborderlayout; 
typedef struct s_ttclass_ttnative *	ttclass_ttnative; 
typedef struct s_ttclass_ttwidget *	ttclass_ttwidget; 
typedef struct s_ttclass_ttlabel *	ttclass_ttlabel; 
typedef struct s_ttclass_tttooltip *	ttclass_tttooltip; 
typedef struct s_ttclass_ttbuttongroup *	ttclass_ttbuttongroup; 
typedef struct s_ttclass_ttanybutton *	ttclass_ttanybutton; 
typedef struct s_ttclass_ttbutton *	ttclass_ttbutton; 
typedef struct s_ttclass_ttcheckbutton *	ttclass_ttcheckbutton; 
typedef struct s_ttclass_ttradiobutton *	ttclass_ttradiobutton; 
typedef struct s_ttclass_ttanyscroll *	ttclass_ttanyscroll; 
typedef struct s_ttclass_ttscrollbar *	ttclass_ttscrollbar; 
typedef struct s_ttclass_ttslider *	ttclass_ttslider; 
typedef struct s_ttclass_ttprogressbar *	ttclass_ttprogressbar; 
typedef struct s_ttclass_ttscrollpane *	ttclass_ttscrollpane; 
typedef struct s_ttclass_ttwindow *	ttclass_ttwindow; 
typedef struct s_ttclass_ttframe *	ttclass_ttframe; 
typedef struct s_ttclass_ttscroller *	ttclass_ttscroller; 
typedef struct s_ttclass_ttmenuitem *	ttclass_ttmenuitem; 
typedef struct s_ttclass_ttcheckmenuitem *	ttclass_ttcheckmenuitem; 
typedef struct s_ttclass_ttradiomenuitem *	ttclass_ttradiomenuitem; 
typedef struct s_ttclass_ttmenu *	ttclass_ttmenu; 
typedef struct s_ttclass_ttmenubar *	ttclass_ttmenubar; 
typedef struct s_ttclass_ttanytext *	ttclass_ttanytext; 
typedef struct s_ttclass_tttextfield *	ttclass_tttextfield; 
typedef struct s_ttclass_tttextarea *	ttclass_tttextarea; 
typedef struct s_ttclass_tttheme *	ttclass_tttheme; 
typedef struct s_ttclass_ttapplication *	ttclass_ttapplication;







/* declarations for objects structures */



 
typedef struct s_ttobj {
    ttclass_ttobj Class;
    ttopaque id;
} s_ttobj;
 
typedef struct s_ttclass {
    ttclass_ttclass Class;
    ttopaque id;
    /* extends ttobj */
    ttuint magicmask;
    ttuint magic;
    ttuint size;
} s_ttclass;
 
typedef struct s_ttobject {
    ttclass_ttobject Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
} s_ttobject;
 
typedef struct s_ttvector {
    ttclass_ttvector Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttopaque array_size;
    ttopaque array_n;
    TT_ARG_ARRAY((_F(array_n))) ttany * array;
} s_ttvector;
 
typedef struct s_ttfield {
    ttclass_ttfield Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque name_len;
    TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name;
    ttopaque type;
} s_ttfield;
 
typedef struct s_ttmethod {
    ttclass_ttmethod Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque name_len;
    TT_ARG_READ TT_ARG_ARRAY((_F(name_len))) ttbyte * name;
    ttfunction_fn address;
    ttopaque return_type;
    ttvector parameter_type;
} s_ttmethod;
 
typedef struct s_ttevent {
    ttclass_ttevent Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcomponent component;
    ttuint evtype;
    ttuint evcode;
    ttuint evflags;
    ttevent prev;
    ttevent next;
} s_ttevent;
 
typedef struct s_tteventbig {
    ttclass_tteventbig Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcomponent component;
    ttuint evtype;
    ttuint evcode;
    ttuint evflags;
    ttevent prev;
    ttevent next;
    /* extends ttevent */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttany value;
    ttany old_value;
    ttopaque data_len;
    TT_ARG_READ TT_ARG_ARRAY((_F(data_len))) ttbyte * data;
} s_tteventbig;
 
typedef struct s_ttbitmask {
    ttclass_ttbitmask Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttopaque mask0;
    ttopaque mask_max;
    ttopaque mask_n;
    TT_ARG_ARRAY((_F(mask_n))) ttany * mask;
} s_ttbitmask;
 
typedef struct s_tteventmask {
    ttclass_tteventmask Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttbitmask evtype_mask;
    ttbitmask evcode_mask;
    ttbitmask component_mask;
    ttuint truth_table;
} s_tteventmask;
 
typedef struct s_ttcallback {
    ttclass_ttcallback Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttuint lflags;
    ttcomponent component;
    ttcallback prev;
    ttcallback next;
} s_ttcallback;
 
typedef struct s_ttlistener {
    ttclass_ttlistener Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttuint lflags;
    ttcomponent component;
    ttlistener prev;
    ttlistener next;
    /* extends ttcallback */
    s_ttavl AVL;
    ttevent event;
    ttlistener_fn function;
    ttopaque arg_component_n;
    ttopaque arg_event_n;
    ttvector args;
    tteventmask event_mask;
    ttlistener inprogress_prev;
    ttlistener inprogress_next;
} s_ttlistener;
 
typedef struct s_tttimer {
    ttclass_tttimer Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttuint lflags;
    ttcomponent component;
    tttimer prev;
    tttimer next;
    /* extends ttcallback */
    s_ttavl AVL;
    ttevent event;
    ttlistener_fn function;
    ttopaque arg_component_n;
    ttopaque arg_event_n;
    ttvector args;
    tteventmask event_mask;
    ttlistener inprogress_prev;
    ttlistener inprogress_next;
    /* extends ttlistener */
    ttany delay_t;
    ttany delay_f;
    tttimer timer_prev;
    tttimer timer_next;
} s_tttimer;
 
typedef struct s_ttcomponent {
    ttclass_ttcomponent Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
} s_ttcomponent;
 
typedef struct s_ttdata {
    ttclass_ttdata Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    s_ttavl AVL;
    ttcomponent component;
    ttopaque key_len;
    TT_ARG_ARRAY((_F(key_len))) ttbyte * key;
    ttany data;
} s_ttdata;
 
typedef struct s_ttvisible {
    ttclass_ttvisible Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
} s_ttvisible;
 
typedef struct s_ttlayout {
    ttclass_ttlayout Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttwidget widget;
} s_ttlayout;
 
typedef struct s_ttboxlayout {
    ttclass_ttboxlayout Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttwidget widget;
    /* extends ttlayout */
    ttuint orientation;
} s_ttboxlayout;
 
typedef struct s_ttborderlayout {
    ttclass_ttborderlayout Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttwidget widget;
    /* extends ttlayout */
} s_ttborderlayout;
 
typedef struct s_ttnative {
    ttclass_ttnative Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
} s_ttnative;
 
typedef struct s_ttwidget {
    ttclass_ttwidget Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
} s_ttwidget;
 
typedef struct s_ttlabel {
    ttclass_ttlabel Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque text_len;
    TT_ARG_ARRAY((_F(text_len))) ttfont * text;
} s_ttlabel;
 
typedef struct s_tttooltip {
    ttclass_tttooltip Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque text_len;
    TT_ARG_ARRAY((_F(text_len))) ttfont * text;
    /* extends ttlabel */
    ttopaque widget_id;
    ttwidget widget;
} s_tttooltip;
 
typedef struct s_ttbuttongroup {
    ttclass_ttbuttongroup Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttradiobutton group_first;
    ttradiobutton group_last;
    ttradiobutton checked;
} s_ttbuttongroup;
 
typedef struct s_ttanybutton {
    ttclass_ttanybutton Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttshort text_width;
    ttshort text_height;
    TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * text;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
} s_ttanybutton;
 
typedef struct s_ttbutton {
    ttclass_ttbutton Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttshort text_width;
    ttshort text_height;
    TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * text;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
    /* extends ttanybutton */
} s_ttbutton;
 
typedef struct s_ttcheckbutton {
    ttclass_ttcheckbutton Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttshort text_width;
    ttshort text_height;
    TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * text;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
    /* extends ttanybutton */
    /* extends ttbutton */
} s_ttcheckbutton;
 
typedef struct s_ttradiobutton {
    ttclass_ttradiobutton Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttshort text_width;
    ttshort text_height;
    TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * text;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
    /* extends ttanybutton */
    /* extends ttbutton */
    /* extends ttcheckbutton */
    ttbuttongroup group;
    ttradiobutton group_prev;
    ttradiobutton group_next;
} s_ttradiobutton;
 
typedef struct s_ttanyscroll {
    ttclass_ttanyscroll Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttshort text_width;
    ttshort text_height;
    TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * text;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
    /* extends ttanybutton */
    ttbyte orientation;
    ttint size;
    ttint real_size;
    ttint view_size;
    ttint real_view_size;
    ttint position;
    ttint real_position;
    ttuint state;
} s_ttanyscroll;
 
typedef struct s_ttscrollbar {
    ttclass_ttscrollbar Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttshort text_width;
    ttshort text_height;
    TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * text;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
    /* extends ttanybutton */
    ttbyte orientation;
    ttint size;
    ttint real_size;
    ttint view_size;
    ttint real_view_size;
    ttint position;
    ttint real_position;
    ttuint state;
    /* extends ttanyscroll */
    ttscrollpane scrollpane;
} s_ttscrollbar;
 
typedef struct s_ttslider {
    ttclass_ttslider Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttshort text_width;
    ttshort text_height;
    TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * text;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
    /* extends ttanybutton */
    ttbyte orientation;
    ttint size;
    ttint real_size;
    ttint view_size;
    ttint real_view_size;
    ttint position;
    ttint real_position;
    ttuint state;
    /* extends ttanyscroll */
    ttint slide_min;
    ttint slide_max;
    ttint slide_value;
} s_ttslider;
 
typedef struct s_ttprogressbar {
    ttclass_ttprogressbar Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttshort text_width;
    ttshort text_height;
    TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * text;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
    /* extends ttanybutton */
    ttbyte orientation;
    ttint size;
    ttint real_size;
    ttint view_size;
    ttint real_view_size;
    ttint position;
    ttint real_position;
    ttuint state;
    /* extends ttanyscroll */
    ttint real_position_frac_above;
    ttint real_position_frac_below;
} s_ttprogressbar;
 
typedef struct s_ttscrollpane {
    ttclass_ttscrollpane Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttscrollbar bar_x;
    ttscrollbar bar_y;
} s_ttscrollpane;
 
typedef struct s_ttwindow {
    ttclass_ttwindow Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    TT_ARG_ARRAY((_F(title_len))) ttbyte * title;
} s_ttwindow;
 
typedef struct s_ttframe {
    ttclass_ttframe Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    TT_ARG_ARRAY((_F(title_len))) ttbyte * title;
    /* extends ttwindow */
    ttmenubar menubar;
} s_ttframe;
 
typedef struct s_ttscroller {
    ttclass_ttscroller Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    TT_ARG_ARRAY((_F(title_len))) ttbyte * title;
    /* extends ttwindow */
    ttscrollpane scrollpane;
} s_ttscroller;
 
typedef struct s_ttmenuitem {
    ttclass_ttmenuitem Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttopaque name_len;
    TT_ARG_ARRAY((_F(name_len))) ttbyte * name;
} s_ttmenuitem;
 
typedef struct s_ttcheckmenuitem {
    ttclass_ttcheckmenuitem Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttopaque name_len;
    TT_ARG_ARRAY((_F(name_len))) ttbyte * name;
    /* extends ttmenuitem */
} s_ttcheckmenuitem;
 
typedef struct s_ttradiomenuitem {
    ttclass_ttradiomenuitem Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttopaque name_len;
    TT_ARG_ARRAY((_F(name_len))) ttbyte * name;
    /* extends ttmenuitem */
    /* extends ttcheckmenuitem */
} s_ttradiomenuitem;
 
typedef struct s_ttmenu {
    ttclass_ttmenu Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttopaque name_len;
    TT_ARG_ARRAY((_F(name_len))) ttbyte * name;
    /* extends ttmenuitem */
    ttmenubar menubar;
} s_ttmenu;
 
typedef struct s_ttmenubar {
    ttclass_ttmenubar Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
} s_ttmenubar;
 
typedef struct s_ttanytext {
    ttclass_ttanytext Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    TT_ARG_ARRAY((_F(title_len))) ttbyte * title;
    /* extends ttwindow */
    ttopaque text_len;
    TT_ARG_ARRAY((_F(text_len))) ttattr * text;
} s_ttanytext;
 
typedef struct s_tttextfield {
    ttclass_tttextfield Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    TT_ARG_ARRAY((_F(title_len))) ttbyte * title;
    /* extends ttwindow */
    ttopaque text_len;
    TT_ARG_ARRAY((_F(text_len))) ttattr * text;
    /* extends ttanytext */
} s_tttextfield;
 
typedef struct s_tttextarea {
    ttclass_tttextarea Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttuint vflags;
    ttvisible prev;
    ttvisible next;
    ttvisible parent;
    ttvisible child_first;
    ttvisible child_last;
    ttany constraint;
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttlayout layout;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    TT_ARG_ARRAY((_F(title_len))) ttbyte * title;
    /* extends ttwindow */
    ttopaque text_len;
    TT_ARG_ARRAY((_F(text_len))) ttattr * text;
    /* extends ttanytext */
    /* extends tttextfield */
} s_tttextarea;
 
typedef struct s_tttheme {
    ttclass_tttheme Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttopaque name_len;
    TT_ARG_ARRAY((_F(name_len))) ttbyte * name;
    ttfont fill;
    TT_ARG_ARRAY((_F(tttheme_bg_max))) ttcol bg[tttheme_bg_max];
    TT_ARG_ARRAY((_F(tttheme_val_max))) ttint val[tttheme_val_max];
    TT_ARG_ARRAY((_F(tttheme_shape_max)*2)) struct s_ttshape shape[tttheme_shape_max][2];
} s_tttheme;
 
typedef struct s_ttapplication {
    ttclass_ttapplication Class;
    ttopaque id;
    /* extends ttobj */
    ttopaque refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobject */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttopaque name_len;
    TT_ARG_ARRAY((_F(name_len))) ttbyte * name;
} s_ttapplication;






/* declarations for methods structures */





 
typedef struct s_ttclass_ttobj {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

} s_ttclass_ttobj;
 
typedef struct s_ttclass_ttclass {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */

} s_ttclass_ttclass;
 
typedef struct s_ttclass_ttobject {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttobject 	 (*New)	(TT_ARG_READ ttclass_ttobject Class, ttobject o);
    ttobject 	 (*Build)	(ttobject o);
    void 	 (*Break)	(ttobject o);
    void 	 (*Del)	(TT_ARG_DIE ttobject o);

} s_ttclass_ttobject;
 
typedef struct s_ttclass_ttvector {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttvector 	 (*New)	(TT_ARG_READ ttclass_ttvector Class, ttvector o);
    ttvector 	 (*Build)	(ttvector o);
    void 	 (*Break)	(ttvector o);
    void 	 (*Del)	(TT_ARG_DIE ttvector o);

    /* extends ttclass_ttobject */
    ttany 	 (*GetAt)	(TT_ARG_READ ttvector o, ttopaque pos);
    ttbyte 	 (*SetAt)	(ttvector o, ttopaque pos, ttany value);
    ttbyte 	 (*AddY)	(ttvector o, ttopaque pos, ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttany * values);
    ttopaque 	 (*ContainsValue)	(TT_ARG_READ ttvector o, ttopaque pos_start, ttany value);
    ttbyte 	 (*RemoveRange)	(ttvector o, ttopaque pos_start, ttopaque pos_n);

} s_ttclass_ttvector;
 
typedef struct s_ttclass_ttfield {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */

} s_ttclass_ttfield;
 
typedef struct s_ttclass_ttmethod {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */

} s_ttclass_ttmethod;
 
typedef struct s_ttclass_ttevent {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttevent 	 (*New)	(TT_ARG_READ ttclass_ttevent Class, ttevent o);
    ttevent 	 (*Build)	(ttevent o);
    void 	 (*Break)	(ttevent o);
    void 	 (*Del)	(TT_ARG_DIE ttevent o);

    /* extends ttclass_ttobject */

} s_ttclass_ttevent;
 
typedef struct s_ttclass_tteventbig {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    tteventbig 	 (*New)	(TT_ARG_READ ttclass_tteventbig Class, tteventbig o);
    tteventbig 	 (*Build)	(tteventbig o);
    void 	 (*Break)	(tteventbig o);
    void 	 (*Del)	(TT_ARG_DIE tteventbig o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttevent */

} s_ttclass_tteventbig;
 
typedef struct s_ttclass_ttbitmask {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttbitmask 	 (*New)	(TT_ARG_READ ttclass_ttbitmask Class, ttbitmask o);
    ttbitmask 	 (*Build)	(ttbitmask o);
    void 	 (*Break)	(ttbitmask o);
    void 	 (*Del)	(TT_ARG_DIE ttbitmask o);

    /* extends ttclass_ttobject */

} s_ttclass_ttbitmask;
 
typedef struct s_ttclass_tteventmask {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    tteventmask 	 (*New)	(TT_ARG_READ ttclass_tteventmask Class, tteventmask o);
    tteventmask 	 (*Build)	(tteventmask o);
    void 	 (*Break)	(tteventmask o);
    void 	 (*Del)	(TT_ARG_DIE tteventmask o);

    /* extends ttclass_ttobject */

} s_ttclass_tteventmask;
 
typedef struct s_ttclass_ttcallback {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttcallback 	 (*New)	(TT_ARG_READ ttclass_ttcallback Class, ttcallback o);
    ttcallback 	 (*Build)	(ttcallback o);
    void 	 (*Break)	(ttcallback o);
    void 	 (*Del)	(TT_ARG_DIE ttcallback o);

    /* extends ttclass_ttobject */
    ttcallback 	 (*Create)	(ttcomponent o);

} s_ttclass_ttcallback;
 
typedef struct s_ttclass_ttlistener {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttlistener 	 (*New)	(TT_ARG_READ ttclass_ttlistener Class, ttlistener o);
    ttlistener 	 (*Build)	(ttlistener o);
    void 	 (*Break)	(ttlistener o);
    void 	 (*Del)	(TT_ARG_DIE ttlistener o);

    /* extends ttclass_ttobject */
    ttcallback 	 (*Create)	(ttcomponent o);

    /* extends ttclass_ttcallback */
    void 	 (*AddTo)	(ttlistener o, ttcomponent c);
    void 	 (*Remove)	(ttlistener o);

} s_ttclass_ttlistener;
 
typedef struct s_ttclass_tttimer {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    tttimer 	 (*New)	(TT_ARG_READ ttclass_tttimer Class, tttimer o);
    tttimer 	 (*Build)	(tttimer o);
    void 	 (*Break)	(tttimer o);
    void 	 (*Del)	(TT_ARG_DIE tttimer o);

    /* extends ttclass_ttobject */
    ttcallback 	 (*Create)	(ttcomponent o);

    /* extends ttclass_ttcallback */
    void 	 (*AddTo)	(tttimer o, ttcomponent c);
    void 	 (*Remove)	(tttimer o);

    /* extends ttclass_ttlistener */
    void 	 (*SetEnabled)	(tttimer o, ttbyte enabled);
    ttbyte 	 (*IsEnabled)	(TT_ARG_READ tttimer o);

} s_ttclass_tttimer;
 
typedef struct s_ttclass_ttcomponent {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttcomponent 	 (*New)	(TT_ARG_READ ttclass_ttcomponent Class, ttcomponent o);
    ttcomponent 	 (*Build)	(ttcomponent o);
    void 	 (*Break)	(ttcomponent o);
    void 	 (*Del)	(TT_ARG_DIE ttcomponent o);

    /* extends ttclass_ttobject */

} s_ttclass_ttcomponent;
 
typedef struct s_ttclass_ttdata {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttdata 	 (*New)	(TT_ARG_READ ttclass_ttdata Class, ttdata o);
    ttdata 	 (*Build)	(ttdata o);
    void 	 (*Break)	(ttdata o);
    void 	 (*Del)	(TT_ARG_DIE ttdata o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttdata data, ttcomponent c);
    void 	 (*Remove)	(ttdata data);

} s_ttclass_ttdata;
 
typedef struct s_ttclass_ttvisible {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttvisible 	 (*New)	(TT_ARG_READ ttclass_ttvisible Class, ttvisible o);
    ttvisible 	 (*Build)	(ttvisible o);
    void 	 (*Break)	(ttvisible o);
    void 	 (*Del)	(TT_ARG_DIE ttvisible o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttvisible o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttvisible o);
    void 	 (*Remove)	(ttvisible o);
    void 	 (*Invalidate)	(ttvisible o);
    void 	 (*SetVisible)	(ttvisible o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttvisible o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h);


} s_ttclass_ttvisible;
 
typedef struct s_ttclass_ttlayout {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttlayout 	 (*New)	(TT_ARG_READ ttclass_ttlayout Class, ttlayout o);
    ttlayout 	 (*Build)	(ttlayout o);
    void 	 (*Break)	(ttlayout o);
    void 	 (*Del)	(TT_ARG_DIE ttlayout o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*SetWidget)	(ttlayout o, ttwidget w);
    void 	 (*DoLayout)	(ttlayout o);

} s_ttclass_ttlayout;
 
typedef struct s_ttclass_ttboxlayout {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttboxlayout 	 (*New)	(TT_ARG_READ ttclass_ttboxlayout Class, ttboxlayout o);
    ttboxlayout 	 (*Build)	(ttboxlayout o);
    void 	 (*Break)	(ttboxlayout o);
    void 	 (*Del)	(TT_ARG_DIE ttboxlayout o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*SetWidget)	(ttboxlayout o, ttwidget w);
    void 	 (*DoLayout)	(ttboxlayout o);

    /* extends ttclass_ttlayout */

} s_ttclass_ttboxlayout;
 
typedef struct s_ttclass_ttborderlayout {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttborderlayout 	 (*New)	(TT_ARG_READ ttclass_ttborderlayout Class, ttborderlayout o);
    ttborderlayout 	 (*Build)	(ttborderlayout o);
    void 	 (*Break)	(ttborderlayout o);
    void 	 (*Del)	(TT_ARG_DIE ttborderlayout o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*SetWidget)	(ttborderlayout o, ttwidget w);
    void 	 (*DoLayout)	(ttborderlayout o);

    /* extends ttclass_ttlayout */

} s_ttclass_ttborderlayout;
 
typedef struct s_ttclass_ttnative {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttnative 	 (*New)	(TT_ARG_READ ttclass_ttnative Class, ttnative o);
    ttnative 	 (*Build)	(ttnative o);
    void 	 (*Break)	(ttnative o);
    void 	 (*Del)	(TT_ARG_DIE ttnative o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttnative o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttnative o);
    void 	 (*Remove)	(ttnative o);
    void 	 (*Invalidate)	(ttnative o);
    void 	 (*SetVisible)	(ttnative o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttnative o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttnative o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttnative o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    ttnative 	 (*GetRoot)	(void);
    ttshort 	 (*GetW)	(TT_ARG_READ ttnative o);
    ttshort 	 (*GetH)	(TT_ARG_READ ttnative o);

} s_ttclass_ttnative;
 
typedef struct s_ttclass_ttwidget {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttwidget 	 (*New)	(TT_ARG_READ ttclass_ttwidget Class, ttwidget o);
    ttwidget 	 (*Build)	(ttwidget o);
    void 	 (*Break)	(ttwidget o);
    void 	 (*Del)	(TT_ARG_DIE ttwidget o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttwidget o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttwidget o);
    void 	 (*Remove)	(ttwidget o);
    void 	 (*Invalidate)	(ttwidget o);
    void 	 (*SetVisible)	(ttwidget o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttwidget o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttwidget o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttwidget o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttwidget o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttwidget o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttwidget o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttwidget o, ttint xl);
    ttbyte 	 (*SetYl)	(ttwidget o, ttint yl);
    ttbyte 	 (*SetWl)	(ttwidget o, ttint wl);
    ttbyte 	 (*SetHl)	(ttwidget o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttwidget o, TT_ARG_DIE tttooltip t);

} s_ttclass_ttwidget;
 
typedef struct s_ttclass_ttlabel {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttlabel 	 (*New)	(TT_ARG_READ ttclass_ttlabel Class, ttlabel o);
    ttlabel 	 (*Build)	(ttlabel o);
    void 	 (*Break)	(ttlabel o);
    void 	 (*Del)	(TT_ARG_DIE ttlabel o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttlabel o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttlabel o);
    void 	 (*Remove)	(ttlabel o);
    void 	 (*Invalidate)	(ttlabel o);
    void 	 (*SetVisible)	(ttlabel o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttlabel o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttlabel o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttlabel o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttlabel o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttlabel o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttlabel o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttlabel o, ttint xl);
    ttbyte 	 (*SetYl)	(ttlabel o, ttint yl);
    ttbyte 	 (*SetWl)	(ttlabel o, ttint wl);
    ttbyte 	 (*SetHl)	(ttlabel o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttlabel o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    void 	 (*SetFontD)	(ttlabel o, ttopaque text_len, TT_ARG_DIE TT_ARG_ARRAY((_P(2))) ttfont * text);

} s_ttclass_ttlabel;
 
typedef struct s_ttclass_tttooltip {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    tttooltip 	 (*New)	(TT_ARG_READ ttclass_tttooltip Class, tttooltip o);
    tttooltip 	 (*Build)	(tttooltip o);
    void 	 (*Break)	(tttooltip o);
    void 	 (*Del)	(TT_ARG_DIE tttooltip o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(tttooltip o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(tttooltip o);
    void 	 (*Remove)	(tttooltip o);
    void 	 (*Invalidate)	(tttooltip o);
    void 	 (*SetVisible)	(tttooltip o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(tttooltip o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(tttooltip o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(tttooltip o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(tttooltip o, ttlayout l);
    ttbyte 	 (*SetXY)	(tttooltip o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(tttooltip o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(tttooltip o, ttint xl);
    ttbyte 	 (*SetYl)	(tttooltip o, ttint yl);
    ttbyte 	 (*SetWl)	(tttooltip o, ttint wl);
    ttbyte 	 (*SetHl)	(tttooltip o, ttint hl);
    ttbyte 	 (*SetTooltip)	(tttooltip o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    void 	 (*SetFontD)	(tttooltip o, ttopaque text_len, TT_ARG_DIE TT_ARG_ARRAY((_P(2))) ttfont * text);

    /* extends ttclass_ttlabel */
    ttbyte 	 (*SetWidget)	(TT_ARG_DIE tttooltip o, ttwidget w);

} s_ttclass_tttooltip;
 
typedef struct s_ttclass_ttbuttongroup {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttbuttongroup 	 (*New)	(TT_ARG_READ ttclass_ttbuttongroup Class, ttbuttongroup o);
    ttbuttongroup 	 (*Build)	(ttbuttongroup o);
    void 	 (*Break)	(ttbuttongroup o);
    void 	 (*Del)	(TT_ARG_DIE ttbuttongroup o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*Add)	(ttbuttongroup o, ttradiobutton r);
    void 	 (*Remove)	(ttbuttongroup o, ttradiobutton r);
    void 	 (*SetChecked)	(ttbuttongroup o, ttradiobutton r);

} s_ttclass_ttbuttongroup;
 
typedef struct s_ttclass_ttanybutton {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttanybutton 	 (*New)	(TT_ARG_READ ttclass_ttanybutton Class, ttanybutton o);
    ttanybutton 	 (*Build)	(ttanybutton o);
    void 	 (*Break)	(ttanybutton o);
    void 	 (*Del)	(TT_ARG_DIE ttanybutton o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttanybutton o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttanybutton o);
    void 	 (*Remove)	(ttanybutton o);
    void 	 (*Invalidate)	(ttanybutton o);
    void 	 (*SetVisible)	(ttanybutton o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttanybutton o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttanybutton o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttanybutton o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttanybutton o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttanybutton o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttanybutton o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttanybutton o, ttint xl);
    ttbyte 	 (*SetYl)	(ttanybutton o, ttint yl);
    ttbyte 	 (*SetWl)	(ttanybutton o, ttint wl);
    ttbyte 	 (*SetHl)	(ttanybutton o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttanybutton o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttuint 	 (*CacheThemeShape)	(ttanybutton o);
    void 	 (*SetAttrD)	(ttanybutton o, ttshort w, ttshort h, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

} s_ttclass_ttanybutton;
 
typedef struct s_ttclass_ttbutton {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttbutton 	 (*New)	(TT_ARG_READ ttclass_ttbutton Class, ttbutton o);
    ttbutton 	 (*Build)	(ttbutton o);
    void 	 (*Break)	(ttbutton o);
    void 	 (*Del)	(TT_ARG_DIE ttbutton o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttbutton o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttbutton o);
    void 	 (*Remove)	(ttbutton o);
    void 	 (*Invalidate)	(ttbutton o);
    void 	 (*SetVisible)	(ttbutton o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttbutton o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttbutton o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttbutton o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttbutton o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttbutton o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttbutton o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttbutton o, ttint xl);
    ttbyte 	 (*SetYl)	(ttbutton o, ttint yl);
    ttbyte 	 (*SetWl)	(ttbutton o, ttint wl);
    ttbyte 	 (*SetHl)	(ttbutton o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttbutton o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttuint 	 (*CacheThemeShape)	(ttbutton o);
    void 	 (*SetAttrD)	(ttbutton o, ttshort w, ttshort h, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

    /* extends ttclass_ttanybutton */
    void 	 (*SetPressed)	(ttbutton o, ttbyte pressed);

} s_ttclass_ttbutton;
 
typedef struct s_ttclass_ttcheckbutton {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttcheckbutton 	 (*New)	(TT_ARG_READ ttclass_ttcheckbutton Class, ttcheckbutton o);
    ttcheckbutton 	 (*Build)	(ttcheckbutton o);
    void 	 (*Break)	(ttcheckbutton o);
    void 	 (*Del)	(TT_ARG_DIE ttcheckbutton o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttcheckbutton o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttcheckbutton o);
    void 	 (*Remove)	(ttcheckbutton o);
    void 	 (*Invalidate)	(ttcheckbutton o);
    void 	 (*SetVisible)	(ttcheckbutton o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttcheckbutton o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttcheckbutton o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttcheckbutton o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttcheckbutton o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttcheckbutton o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttcheckbutton o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttcheckbutton o, ttint xl);
    ttbyte 	 (*SetYl)	(ttcheckbutton o, ttint yl);
    ttbyte 	 (*SetWl)	(ttcheckbutton o, ttint wl);
    ttbyte 	 (*SetHl)	(ttcheckbutton o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttcheckbutton o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttuint 	 (*CacheThemeShape)	(ttcheckbutton o);
    void 	 (*SetAttrD)	(ttcheckbutton o, ttshort w, ttshort h, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

    /* extends ttclass_ttanybutton */
    void 	 (*SetPressed)	(ttcheckbutton o, ttbyte pressed);

    /* extends ttclass_ttbutton */
    void 	 (*SetChecked)	(ttcheckbutton o, ttbyte checked);

} s_ttclass_ttcheckbutton;
 
typedef struct s_ttclass_ttradiobutton {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttradiobutton 	 (*New)	(TT_ARG_READ ttclass_ttradiobutton Class, ttradiobutton o);
    ttradiobutton 	 (*Build)	(ttradiobutton o);
    void 	 (*Break)	(ttradiobutton o);
    void 	 (*Del)	(TT_ARG_DIE ttradiobutton o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttradiobutton o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttradiobutton o);
    void 	 (*Remove)	(ttradiobutton o);
    void 	 (*Invalidate)	(ttradiobutton o);
    void 	 (*SetVisible)	(ttradiobutton o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttradiobutton o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttradiobutton o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttradiobutton o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttradiobutton o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttradiobutton o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttradiobutton o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttradiobutton o, ttint xl);
    ttbyte 	 (*SetYl)	(ttradiobutton o, ttint yl);
    ttbyte 	 (*SetWl)	(ttradiobutton o, ttint wl);
    ttbyte 	 (*SetHl)	(ttradiobutton o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttradiobutton o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttuint 	 (*CacheThemeShape)	(ttradiobutton o);
    void 	 (*SetAttrD)	(ttradiobutton o, ttshort w, ttshort h, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

    /* extends ttclass_ttanybutton */
    void 	 (*SetPressed)	(ttradiobutton o, ttbyte pressed);

    /* extends ttclass_ttbutton */
    void 	 (*SetChecked)	(ttradiobutton o, ttbyte checked);

    /* extends ttclass_ttcheckbutton */

} s_ttclass_ttradiobutton;
 
typedef struct s_ttclass_ttanyscroll {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttanyscroll 	 (*New)	(TT_ARG_READ ttclass_ttanyscroll Class, ttanyscroll o);
    ttanyscroll 	 (*Build)	(ttanyscroll o);
    void 	 (*Break)	(ttanyscroll o);
    void 	 (*Del)	(TT_ARG_DIE ttanyscroll o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttanyscroll o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttanyscroll o);
    void 	 (*Remove)	(ttanyscroll o);
    void 	 (*Invalidate)	(ttanyscroll o);
    void 	 (*SetVisible)	(ttanyscroll o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttanyscroll o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttanyscroll o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttanyscroll o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttanyscroll o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttanyscroll o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttanyscroll o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttanyscroll o, ttint xl);
    ttbyte 	 (*SetYl)	(ttanyscroll o, ttint yl);
    ttbyte 	 (*SetWl)	(ttanyscroll o, ttint wl);
    ttbyte 	 (*SetHl)	(ttanyscroll o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttanyscroll o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttuint 	 (*CacheThemeShape)	(ttanyscroll o);
    void 	 (*SetAttrD)	(ttanyscroll o, ttshort w, ttshort h, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

    /* extends ttclass_ttanybutton */
    void 	 (*Recalculate)	(ttanyscroll o, ttint size, ttint real_size, ttint view_size, ttint position);
    ttbyte 	 (*SetState)	(ttanyscroll o, ttuint state);
    ttbyte 	 (*SetOrientation)	(ttanyscroll o, ttbyte orientation);
    ttbyte 	 (*SetSize)	(ttanyscroll o, ttint size);
    ttbyte 	 (*SetRealSize)	(ttanyscroll o, ttint real_size);
    ttbyte 	 (*SetViewSize)	(ttanyscroll o, ttint view_size);
    ttbyte 	 (*SetPosition)	(ttanyscroll o, ttint position);

} s_ttclass_ttanyscroll;
 
typedef struct s_ttclass_ttscrollbar {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttscrollbar 	 (*New)	(TT_ARG_READ ttclass_ttscrollbar Class, ttscrollbar o);
    ttscrollbar 	 (*Build)	(ttscrollbar o);
    void 	 (*Break)	(ttscrollbar o);
    void 	 (*Del)	(TT_ARG_DIE ttscrollbar o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttscrollbar o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttscrollbar o);
    void 	 (*Remove)	(ttscrollbar o);
    void 	 (*Invalidate)	(ttscrollbar o);
    void 	 (*SetVisible)	(ttscrollbar o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttscrollbar o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttscrollbar o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttscrollbar o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttscrollbar o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttscrollbar o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttscrollbar o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttscrollbar o, ttint xl);
    ttbyte 	 (*SetYl)	(ttscrollbar o, ttint yl);
    ttbyte 	 (*SetWl)	(ttscrollbar o, ttint wl);
    ttbyte 	 (*SetHl)	(ttscrollbar o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttscrollbar o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttuint 	 (*CacheThemeShape)	(ttscrollbar o);
    void 	 (*SetAttrD)	(ttscrollbar o, ttshort w, ttshort h, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

    /* extends ttclass_ttanybutton */
    void 	 (*Recalculate)	(ttscrollbar o, ttint size, ttint real_size, ttint view_size, ttint position);
    ttbyte 	 (*SetState)	(ttscrollbar o, ttuint state);
    ttbyte 	 (*SetOrientation)	(ttscrollbar o, ttbyte orientation);
    ttbyte 	 (*SetSize)	(ttscrollbar o, ttint size);
    ttbyte 	 (*SetRealSize)	(ttscrollbar o, ttint real_size);
    ttbyte 	 (*SetViewSize)	(ttscrollbar o, ttint view_size);
    ttbyte 	 (*SetPosition)	(ttscrollbar o, ttint position);

    /* extends ttclass_ttanyscroll */

} s_ttclass_ttscrollbar;
 
typedef struct s_ttclass_ttslider {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttslider 	 (*New)	(TT_ARG_READ ttclass_ttslider Class, ttslider o);
    ttslider 	 (*Build)	(ttslider o);
    void 	 (*Break)	(ttslider o);
    void 	 (*Del)	(TT_ARG_DIE ttslider o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttslider o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttslider o);
    void 	 (*Remove)	(ttslider o);
    void 	 (*Invalidate)	(ttslider o);
    void 	 (*SetVisible)	(ttslider o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttslider o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttslider o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttslider o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttslider o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttslider o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttslider o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttslider o, ttint xl);
    ttbyte 	 (*SetYl)	(ttslider o, ttint yl);
    ttbyte 	 (*SetWl)	(ttslider o, ttint wl);
    ttbyte 	 (*SetHl)	(ttslider o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttslider o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttuint 	 (*CacheThemeShape)	(ttslider o);
    void 	 (*SetAttrD)	(ttslider o, ttshort w, ttshort h, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

    /* extends ttclass_ttanybutton */
    void 	 (*Recalculate)	(ttslider o, ttint size, ttint real_size, ttint view_size, ttint position);
    ttbyte 	 (*SetState)	(ttslider o, ttuint state);
    ttbyte 	 (*SetOrientation)	(ttslider o, ttbyte orientation);
    ttbyte 	 (*SetSize)	(ttslider o, ttint size);
    ttbyte 	 (*SetRealSize)	(ttslider o, ttint real_size);
    ttbyte 	 (*SetViewSize)	(ttslider o, ttint view_size);
    ttbyte 	 (*SetPosition)	(ttslider o, ttint position);

    /* extends ttclass_ttanyscroll */
    ttbyte 	 (*SetSlideMin)	(ttslider o, ttint slide_min);
    ttbyte 	 (*SetSlideMax)	(ttslider o, ttint slide_max);
    ttbyte 	 (*SetSlideValue)	(ttslider o, ttint slide_value);

} s_ttclass_ttslider;
 
typedef struct s_ttclass_ttprogressbar {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttprogressbar 	 (*New)	(TT_ARG_READ ttclass_ttprogressbar Class, ttprogressbar o);
    ttprogressbar 	 (*Build)	(ttprogressbar o);
    void 	 (*Break)	(ttprogressbar o);
    void 	 (*Del)	(TT_ARG_DIE ttprogressbar o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttprogressbar o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttprogressbar o);
    void 	 (*Remove)	(ttprogressbar o);
    void 	 (*Invalidate)	(ttprogressbar o);
    void 	 (*SetVisible)	(ttprogressbar o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttprogressbar o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttprogressbar o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttprogressbar o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttprogressbar o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttprogressbar o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttprogressbar o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttprogressbar o, ttint xl);
    ttbyte 	 (*SetYl)	(ttprogressbar o, ttint yl);
    ttbyte 	 (*SetWl)	(ttprogressbar o, ttint wl);
    ttbyte 	 (*SetHl)	(ttprogressbar o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttprogressbar o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttuint 	 (*CacheThemeShape)	(ttprogressbar o);
    void 	 (*SetAttrD)	(ttprogressbar o, ttshort w, ttshort h, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

    /* extends ttclass_ttanybutton */
    void 	 (*Recalculate)	(ttprogressbar o, ttint size, ttint real_size, ttint view_size, ttint position);
    ttbyte 	 (*SetState)	(ttprogressbar o, ttuint state);
    ttbyte 	 (*SetOrientation)	(ttprogressbar o, ttbyte orientation);
    ttbyte 	 (*SetSize)	(ttprogressbar o, ttint size);
    ttbyte 	 (*SetRealSize)	(ttprogressbar o, ttint real_size);
    ttbyte 	 (*SetViewSize)	(ttprogressbar o, ttint view_size);
    ttbyte 	 (*SetPosition)	(ttprogressbar o, ttint position);

    /* extends ttclass_ttanyscroll */

} s_ttclass_ttprogressbar;
 
typedef struct s_ttclass_ttscrollpane {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttscrollpane 	 (*New)	(TT_ARG_READ ttclass_ttscrollpane Class, ttscrollpane o);
    ttscrollpane 	 (*Build)	(ttscrollpane o);
    void 	 (*Break)	(ttscrollpane o);
    void 	 (*Del)	(TT_ARG_DIE ttscrollpane o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttscrollpane o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttscrollpane o);
    void 	 (*Remove)	(ttscrollpane o);
    void 	 (*Invalidate)	(ttscrollpane o);
    void 	 (*SetVisible)	(ttscrollpane o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttscrollpane o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttscrollpane o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttscrollpane o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttscrollpane o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttscrollpane o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttscrollpane o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttscrollpane o, ttint xl);
    ttbyte 	 (*SetYl)	(ttscrollpane o, ttint yl);
    ttbyte 	 (*SetWl)	(ttscrollpane o, ttint wl);
    ttbyte 	 (*SetHl)	(ttscrollpane o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttscrollpane o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttbyte 	 (*SetBarX)	(ttscrollpane o, ttscrollbar bar_x);
    ttbyte 	 (*SetBarY)	(ttscrollpane o, ttscrollbar bar_y);

} s_ttclass_ttscrollpane;
 
typedef struct s_ttclass_ttwindow {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttwindow 	 (*New)	(TT_ARG_READ ttclass_ttwindow Class, ttwindow o);
    ttwindow 	 (*Build)	(ttwindow o);
    void 	 (*Break)	(ttwindow o);
    void 	 (*Del)	(TT_ARG_DIE ttwindow o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttwindow o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttwindow o);
    void 	 (*Remove)	(ttwindow o);
    void 	 (*Invalidate)	(ttwindow o);
    void 	 (*SetVisible)	(ttwindow o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttwindow o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttwindow o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttwindow o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttwindow o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttwindow o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttwindow o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttwindow o, ttint xl);
    ttbyte 	 (*SetYl)	(ttwindow o, ttint yl);
    ttbyte 	 (*SetWl)	(ttwindow o, ttint wl);
    ttbyte 	 (*SetHl)	(ttwindow o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttwindow o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttbyte 	 (*SetTitle)	(ttwindow o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title);

} s_ttclass_ttwindow;
 
typedef struct s_ttclass_ttframe {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttframe 	 (*New)	(TT_ARG_READ ttclass_ttframe Class, ttframe o);
    ttframe 	 (*Build)	(ttframe o);
    void 	 (*Break)	(ttframe o);
    void 	 (*Del)	(TT_ARG_DIE ttframe o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttframe o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttframe o);
    void 	 (*Remove)	(ttframe o);
    void 	 (*Invalidate)	(ttframe o);
    void 	 (*SetVisible)	(ttframe o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttframe o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttframe o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttframe o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttframe o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttframe o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttframe o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttframe o, ttint xl);
    ttbyte 	 (*SetYl)	(ttframe o, ttint yl);
    ttbyte 	 (*SetWl)	(ttframe o, ttint wl);
    ttbyte 	 (*SetHl)	(ttframe o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttframe o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttbyte 	 (*SetTitle)	(ttframe o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title);

    /* extends ttclass_ttwindow */
    ttbyte 	 (*SetMenubar)	(ttframe o, TT_ARG_READ ttmenubar m);

} s_ttclass_ttframe;
 
typedef struct s_ttclass_ttscroller {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttscroller 	 (*New)	(TT_ARG_READ ttclass_ttscroller Class, ttscroller o);
    ttscroller 	 (*Build)	(ttscroller o);
    void 	 (*Break)	(ttscroller o);
    void 	 (*Del)	(TT_ARG_DIE ttscroller o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttscroller o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttscroller o);
    void 	 (*Remove)	(ttscroller o);
    void 	 (*Invalidate)	(ttscroller o);
    void 	 (*SetVisible)	(ttscroller o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttscroller o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttscroller o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttscroller o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttscroller o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttscroller o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttscroller o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttscroller o, ttint xl);
    ttbyte 	 (*SetYl)	(ttscroller o, ttint yl);
    ttbyte 	 (*SetWl)	(ttscroller o, ttint wl);
    ttbyte 	 (*SetHl)	(ttscroller o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttscroller o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttbyte 	 (*SetTitle)	(ttscroller o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title);

    /* extends ttclass_ttwindow */

} s_ttclass_ttscroller;
 
typedef struct s_ttclass_ttmenuitem {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttmenuitem 	 (*New)	(TT_ARG_READ ttclass_ttmenuitem Class, ttmenuitem o);
    ttmenuitem 	 (*Build)	(ttmenuitem o);
    void 	 (*Break)	(ttmenuitem o);
    void 	 (*Del)	(TT_ARG_DIE ttmenuitem o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttmenuitem o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttmenuitem o);
    void 	 (*Remove)	(ttmenuitem o);
    void 	 (*Invalidate)	(ttmenuitem o);
    void 	 (*SetVisible)	(ttmenuitem o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttmenuitem o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttmenuitem o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttmenuitem o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */

} s_ttclass_ttmenuitem;
 
typedef struct s_ttclass_ttcheckmenuitem {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttcheckmenuitem 	 (*New)	(TT_ARG_READ ttclass_ttcheckmenuitem Class, ttcheckmenuitem o);
    ttcheckmenuitem 	 (*Build)	(ttcheckmenuitem o);
    void 	 (*Break)	(ttcheckmenuitem o);
    void 	 (*Del)	(TT_ARG_DIE ttcheckmenuitem o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttcheckmenuitem o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttcheckmenuitem o);
    void 	 (*Remove)	(ttcheckmenuitem o);
    void 	 (*Invalidate)	(ttcheckmenuitem o);
    void 	 (*SetVisible)	(ttcheckmenuitem o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttcheckmenuitem o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttcheckmenuitem o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttcheckmenuitem o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

} s_ttclass_ttcheckmenuitem;
 
typedef struct s_ttclass_ttradiomenuitem {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttradiomenuitem 	 (*New)	(TT_ARG_READ ttclass_ttradiomenuitem Class, ttradiomenuitem o);
    ttradiomenuitem 	 (*Build)	(ttradiomenuitem o);
    void 	 (*Break)	(ttradiomenuitem o);
    void 	 (*Del)	(TT_ARG_DIE ttradiomenuitem o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttradiomenuitem o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttradiomenuitem o);
    void 	 (*Remove)	(ttradiomenuitem o);
    void 	 (*Invalidate)	(ttradiomenuitem o);
    void 	 (*SetVisible)	(ttradiomenuitem o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttradiomenuitem o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttradiomenuitem o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttradiomenuitem o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

    /* extends ttclass_ttcheckmenuitem */

} s_ttclass_ttradiomenuitem;
 
typedef struct s_ttclass_ttmenu {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttmenu 	 (*New)	(TT_ARG_READ ttclass_ttmenu Class, ttmenu o);
    ttmenu 	 (*Build)	(ttmenu o);
    void 	 (*Break)	(ttmenu o);
    void 	 (*Del)	(TT_ARG_DIE ttmenu o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttmenu o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttmenu o);
    void 	 (*Remove)	(ttmenu o);
    void 	 (*Invalidate)	(ttmenu o);
    void 	 (*SetVisible)	(ttmenu o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttmenu o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttmenu o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttmenu o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

} s_ttclass_ttmenu;
 
typedef struct s_ttclass_ttmenubar {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttmenubar 	 (*New)	(TT_ARG_READ ttclass_ttmenubar Class, ttmenubar o);
    ttmenubar 	 (*Build)	(ttmenubar o);
    void 	 (*Break)	(ttmenubar o);
    void 	 (*Del)	(TT_ARG_DIE ttmenubar o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttmenubar o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttmenubar o);
    void 	 (*Remove)	(ttmenubar o);
    void 	 (*Invalidate)	(ttmenubar o);
    void 	 (*SetVisible)	(ttmenubar o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttmenubar o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttmenubar o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttmenubar o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */

} s_ttclass_ttmenubar;
 
typedef struct s_ttclass_ttanytext {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttanytext 	 (*New)	(TT_ARG_READ ttclass_ttanytext Class, ttanytext o);
    ttanytext 	 (*Build)	(ttanytext o);
    void 	 (*Break)	(ttanytext o);
    void 	 (*Del)	(TT_ARG_DIE ttanytext o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(ttanytext o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(ttanytext o);
    void 	 (*Remove)	(ttanytext o);
    void 	 (*Invalidate)	(ttanytext o);
    void 	 (*SetVisible)	(ttanytext o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(ttanytext o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(ttanytext o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(ttanytext o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(ttanytext o, ttlayout l);
    ttbyte 	 (*SetXY)	(ttanytext o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(ttanytext o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(ttanytext o, ttint xl);
    ttbyte 	 (*SetYl)	(ttanytext o, ttint yl);
    ttbyte 	 (*SetWl)	(ttanytext o, ttint wl);
    ttbyte 	 (*SetHl)	(ttanytext o, ttint hl);
    ttbyte 	 (*SetTooltip)	(ttanytext o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttbyte 	 (*SetTitle)	(ttanytext o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title);

    /* extends ttclass_ttwindow */

} s_ttclass_ttanytext;
 
typedef struct s_ttclass_tttextfield {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    tttextfield 	 (*New)	(TT_ARG_READ ttclass_tttextfield Class, tttextfield o);
    tttextfield 	 (*Build)	(tttextfield o);
    void 	 (*Break)	(tttextfield o);
    void 	 (*Del)	(TT_ARG_DIE tttextfield o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(tttextfield o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(tttextfield o);
    void 	 (*Remove)	(tttextfield o);
    void 	 (*Invalidate)	(tttextfield o);
    void 	 (*SetVisible)	(tttextfield o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(tttextfield o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(tttextfield o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(tttextfield o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(tttextfield o, ttlayout l);
    ttbyte 	 (*SetXY)	(tttextfield o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(tttextfield o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(tttextfield o, ttint xl);
    ttbyte 	 (*SetYl)	(tttextfield o, ttint yl);
    ttbyte 	 (*SetWl)	(tttextfield o, ttint wl);
    ttbyte 	 (*SetHl)	(tttextfield o, ttint hl);
    ttbyte 	 (*SetTooltip)	(tttextfield o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttbyte 	 (*SetTitle)	(tttextfield o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title);

    /* extends ttclass_ttwindow */

    /* extends ttclass_ttanytext */

} s_ttclass_tttextfield;
 
typedef struct s_ttclass_tttextarea {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    tttextarea 	 (*New)	(TT_ARG_READ ttclass_tttextarea Class, tttextarea o);
    tttextarea 	 (*Build)	(tttextarea o);
    void 	 (*Break)	(tttextarea o);
    void 	 (*Del)	(TT_ARG_DIE tttextarea o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    void 	 (*AddTo)	(tttextarea o, ttvisible parent, ttany constraint);
    void 	 (*Validate)	(tttextarea o);
    void 	 (*Remove)	(tttextarea o);
    void 	 (*Invalidate)	(tttextarea o);
    void 	 (*SetVisible)	(tttextarea o, ttbyte visible);
    ttbyte 	 (*SetTheme)	(tttextarea o, TT_ARG_READ tttheme theme);
    void 	 (*Draw)	(tttextarea o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    void 	 (*BuiltinRepaint)	(tttextarea o, ttshort x, ttshort y, ttshort w, ttshort h);


    /* extends ttclass_ttvisible */
    void 	 (*SetLayout)	(tttextarea o, ttlayout l);
    ttbyte 	 (*SetXY)	(tttextarea o, ttshort x, ttshort y);
    ttbyte 	 (*SetWH)	(tttextarea o, ttshort w, ttshort h);
    ttbyte 	 (*SetXl)	(tttextarea o, ttint xl);
    ttbyte 	 (*SetYl)	(tttextarea o, ttint yl);
    ttbyte 	 (*SetWl)	(tttextarea o, ttint wl);
    ttbyte 	 (*SetHl)	(tttextarea o, ttint hl);
    ttbyte 	 (*SetTooltip)	(tttextarea o, TT_ARG_DIE tttooltip t);

    /* extends ttclass_ttwidget */
    ttbyte 	 (*SetTitle)	(tttextarea o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title);

    /* extends ttclass_ttwindow */

    /* extends ttclass_ttanytext */

    /* extends ttclass_tttextfield */

} s_ttclass_tttextarea;
 
typedef struct s_ttclass_tttheme {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    tttheme 	 (*New)	(TT_ARG_READ ttclass_tttheme Class, tttheme o);
    tttheme 	 (*Build)	(tttheme o);
    void 	 (*Break)	(tttheme o);
    void 	 (*Del)	(TT_ARG_DIE tttheme o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */

} s_ttclass_tttheme;
 
typedef struct s_ttclass_ttapplication {
    ttclass_ttclass 	Class;
    ttopaque 	id;
    ttuint 	magicmask;
    ttuint 	magic;
    ttuint 	size;

    /* extends ttclass_ttobj */
    ttapplication 	 (*New)	(TT_ARG_READ ttclass_ttapplication Class, ttapplication o);
    ttapplication 	 (*Build)	(ttapplication o);
    void 	 (*Break)	(ttapplication o);
    void 	 (*Del)	(TT_ARG_DIE ttapplication o);

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */

} s_ttclass_ttapplication;







typedef struct s_tthw *tthw;
typedef struct s_tthw {
    byte (*Sync)(void);
    byte (*Flush)(void);
    byte (*TimidFlush)(void);
    byte (*MainLoopOnce)(byte);
    void (*DeleteCallback)(ttcallback);

    void (*Close)(void);
    int (*ConnectionFd)(void);
    
    ttuint (*GetErrno)(void);
    ttuint (*GetErrnoDetail)(void);
    TT_CONST byte *(*StrError)(ttuint E);
    TT_CONST byte *(*StrErrorDetail)(ttuint E, ttuint S);

} s_tthw;


typedef struct s_ttclasses *ttclasses;
typedef struct s_ttclasses {
 
    s_ttclass_ttobj	class_ttobj; 
    s_ttclass_ttclass	class_ttclass; 
    s_ttclass_ttobject	class_ttobject; 
    s_ttclass_ttvector	class_ttvector; 
    s_ttclass_ttfield	class_ttfield; 
    s_ttclass_ttmethod	class_ttmethod; 
    s_ttclass_ttevent	class_ttevent; 
    s_ttclass_tteventbig	class_tteventbig; 
    s_ttclass_ttbitmask	class_ttbitmask; 
    s_ttclass_tteventmask	class_tteventmask; 
    s_ttclass_ttcallback	class_ttcallback; 
    s_ttclass_ttlistener	class_ttlistener; 
    s_ttclass_tttimer	class_tttimer; 
    s_ttclass_ttcomponent	class_ttcomponent; 
    s_ttclass_ttdata	class_ttdata; 
    s_ttclass_ttvisible	class_ttvisible; 
    s_ttclass_ttlayout	class_ttlayout; 
    s_ttclass_ttboxlayout	class_ttboxlayout; 
    s_ttclass_ttborderlayout	class_ttborderlayout; 
    s_ttclass_ttnative	class_ttnative; 
    s_ttclass_ttwidget	class_ttwidget; 
    s_ttclass_ttlabel	class_ttlabel; 
    s_ttclass_tttooltip	class_tttooltip; 
    s_ttclass_ttbuttongroup	class_ttbuttongroup; 
    s_ttclass_ttanybutton	class_ttanybutton; 
    s_ttclass_ttbutton	class_ttbutton; 
    s_ttclass_ttcheckbutton	class_ttcheckbutton; 
    s_ttclass_ttradiobutton	class_ttradiobutton; 
    s_ttclass_ttanyscroll	class_ttanyscroll; 
    s_ttclass_ttscrollbar	class_ttscrollbar; 
    s_ttclass_ttslider	class_ttslider; 
    s_ttclass_ttprogressbar	class_ttprogressbar; 
    s_ttclass_ttscrollpane	class_ttscrollpane; 
    s_ttclass_ttwindow	class_ttwindow; 
    s_ttclass_ttframe	class_ttframe; 
    s_ttclass_ttscroller	class_ttscroller; 
    s_ttclass_ttmenuitem	class_ttmenuitem; 
    s_ttclass_ttcheckmenuitem	class_ttcheckmenuitem; 
    s_ttclass_ttradiomenuitem	class_ttradiomenuitem; 
    s_ttclass_ttmenu	class_ttmenu; 
    s_ttclass_ttmenubar	class_ttmenubar; 
    s_ttclass_ttanytext	class_ttanytext; 
    s_ttclass_tttextfield	class_tttextfield; 
    s_ttclass_tttextarea	class_tttextarea; 
    s_ttclass_tttheme	class_tttheme; 
    s_ttclass_ttapplication	class_ttapplication;
    s_tthw HW;
} s_ttclasses;





/* client-visible global methods structures */
 
extern tt_obj TTClass_ttobj; 
extern tt_obj TTClass_ttclass; 
extern tt_obj TTClass_ttobject; 
extern tt_obj TTClass_ttvector; 
extern tt_obj TTClass_ttfield; 
extern tt_obj TTClass_ttmethod; 
extern tt_obj TTClass_ttevent; 
extern tt_obj TTClass_tteventbig; 
extern tt_obj TTClass_ttbitmask; 
extern tt_obj TTClass_tteventmask; 
extern tt_obj TTClass_ttcallback; 
extern tt_obj TTClass_ttlistener; 
extern tt_obj TTClass_tttimer; 
extern tt_obj TTClass_ttcomponent; 
extern tt_obj TTClass_ttdata; 
extern tt_obj TTClass_ttvisible; 
extern tt_obj TTClass_ttlayout; 
extern tt_obj TTClass_ttboxlayout; 
extern tt_obj TTClass_ttborderlayout; 
extern tt_obj TTClass_ttnative; 
extern tt_obj TTClass_ttwidget; 
extern tt_obj TTClass_ttlabel; 
extern tt_obj TTClass_tttooltip; 
extern tt_obj TTClass_ttbuttongroup; 
extern tt_obj TTClass_ttanybutton; 
extern tt_obj TTClass_ttbutton; 
extern tt_obj TTClass_ttcheckbutton; 
extern tt_obj TTClass_ttradiobutton; 
extern tt_obj TTClass_ttanyscroll; 
extern tt_obj TTClass_ttscrollbar; 
extern tt_obj TTClass_ttslider; 
extern tt_obj TTClass_ttprogressbar; 
extern tt_obj TTClass_ttscrollpane; 
extern tt_obj TTClass_ttwindow; 
extern tt_obj TTClass_ttframe; 
extern tt_obj TTClass_ttscroller; 
extern tt_obj TTClass_ttmenuitem; 
extern tt_obj TTClass_ttcheckmenuitem; 
extern tt_obj TTClass_ttradiomenuitem; 
extern tt_obj TTClass_ttmenu; 
extern tt_obj TTClass_ttmenubar; 
extern tt_obj TTClass_ttanytext; 
extern tt_obj TTClass_tttextfield; 
extern tt_obj TTClass_tttextarea; 
extern tt_obj TTClass_tttheme; 
extern tt_obj TTClass_ttapplication;

/* client-visible global types */

extern tt_obj TTClass_void; 
extern tt_obj TTClass_ttbyte; 
extern tt_obj TTClass_ttshort; 
extern tt_obj TTClass_ttint; 
extern tt_obj TTClass_ttcol; 
extern tt_obj TTClass_ttfont; 
extern tt_obj TTClass_ttattr; 
extern tt_obj TTClass_ttopaque; 
extern tt_obj TTClass_ttany; 
extern tt_obj TTClass_ttfunction_fn; 
extern tt_obj TTClass_va_list; 
extern tt_obj TTClass_ttarg;
#define TTClass_ttsbyte	TTClass_ttbyte
#define TTClass_ttushort	TTClass_ttshort
#define TTClass_ttuint	TTClass_ttint



/* #defines for global methods structures */
#define TClasses	(TTD.Classes)

#define THW	(TTD.Class.HW)


 
#define TClass_ttobj	(&TTD.Class.class_ttobj) 
#define TClass_ttclass	(&TTD.Class.class_ttclass) 
#define TClass_ttobject	(&TTD.Class.class_ttobject) 
#define TClass_ttvector	(&TTD.Class.class_ttvector) 
#define TClass_ttfield	(&TTD.Class.class_ttfield) 
#define TClass_ttmethod	(&TTD.Class.class_ttmethod) 
#define TClass_ttevent	(&TTD.Class.class_ttevent) 
#define TClass_tteventbig	(&TTD.Class.class_tteventbig) 
#define TClass_ttbitmask	(&TTD.Class.class_ttbitmask) 
#define TClass_tteventmask	(&TTD.Class.class_tteventmask) 
#define TClass_ttcallback	(&TTD.Class.class_ttcallback) 
#define TClass_ttlistener	(&TTD.Class.class_ttlistener) 
#define TClass_tttimer	(&TTD.Class.class_tttimer) 
#define TClass_ttcomponent	(&TTD.Class.class_ttcomponent) 
#define TClass_ttdata	(&TTD.Class.class_ttdata) 
#define TClass_ttvisible	(&TTD.Class.class_ttvisible) 
#define TClass_ttlayout	(&TTD.Class.class_ttlayout) 
#define TClass_ttboxlayout	(&TTD.Class.class_ttboxlayout) 
#define TClass_ttborderlayout	(&TTD.Class.class_ttborderlayout) 
#define TClass_ttnative	(&TTD.Class.class_ttnative) 
#define TClass_ttwidget	(&TTD.Class.class_ttwidget) 
#define TClass_ttlabel	(&TTD.Class.class_ttlabel) 
#define TClass_tttooltip	(&TTD.Class.class_tttooltip) 
#define TClass_ttbuttongroup	(&TTD.Class.class_ttbuttongroup) 
#define TClass_ttanybutton	(&TTD.Class.class_ttanybutton) 
#define TClass_ttbutton	(&TTD.Class.class_ttbutton) 
#define TClass_ttcheckbutton	(&TTD.Class.class_ttcheckbutton) 
#define TClass_ttradiobutton	(&TTD.Class.class_ttradiobutton) 
#define TClass_ttanyscroll	(&TTD.Class.class_ttanyscroll) 
#define TClass_ttscrollbar	(&TTD.Class.class_ttscrollbar) 
#define TClass_ttslider	(&TTD.Class.class_ttslider) 
#define TClass_ttprogressbar	(&TTD.Class.class_ttprogressbar) 
#define TClass_ttscrollpane	(&TTD.Class.class_ttscrollpane) 
#define TClass_ttwindow	(&TTD.Class.class_ttwindow) 
#define TClass_ttframe	(&TTD.Class.class_ttframe) 
#define TClass_ttscroller	(&TTD.Class.class_ttscroller) 
#define TClass_ttmenuitem	(&TTD.Class.class_ttmenuitem) 
#define TClass_ttcheckmenuitem	(&TTD.Class.class_ttcheckmenuitem) 
#define TClass_ttradiomenuitem	(&TTD.Class.class_ttradiomenuitem) 
#define TClass_ttmenu	(&TTD.Class.class_ttmenu) 
#define TClass_ttmenubar	(&TTD.Class.class_ttmenubar) 
#define TClass_ttanytext	(&TTD.Class.class_ttanytext) 
#define TClass_tttextfield	(&TTD.Class.class_tttextfield) 
#define TClass_tttextarea	(&TTD.Class.class_tttextarea) 
#define TClass_tttheme	(&TTD.Class.class_tttheme) 
#define TClass_ttapplication	(&TTD.Class.class_ttapplication)


#define TClass(type) TT_CAT(TClass_,type)



 
#define TClass_default_ttobj	(&TTD.Class_default.class_ttobj) 
#define TClass_default_ttclass	(&TTD.Class_default.class_ttclass) 
#define TClass_default_ttobject	(&TTD.Class_default.class_ttobject) 
#define TClass_default_ttvector	(&TTD.Class_default.class_ttvector) 
#define TClass_default_ttfield	(&TTD.Class_default.class_ttfield) 
#define TClass_default_ttmethod	(&TTD.Class_default.class_ttmethod) 
#define TClass_default_ttevent	(&TTD.Class_default.class_ttevent) 
#define TClass_default_tteventbig	(&TTD.Class_default.class_tteventbig) 
#define TClass_default_ttbitmask	(&TTD.Class_default.class_ttbitmask) 
#define TClass_default_tteventmask	(&TTD.Class_default.class_tteventmask) 
#define TClass_default_ttcallback	(&TTD.Class_default.class_ttcallback) 
#define TClass_default_ttlistener	(&TTD.Class_default.class_ttlistener) 
#define TClass_default_tttimer	(&TTD.Class_default.class_tttimer) 
#define TClass_default_ttcomponent	(&TTD.Class_default.class_ttcomponent) 
#define TClass_default_ttdata	(&TTD.Class_default.class_ttdata) 
#define TClass_default_ttvisible	(&TTD.Class_default.class_ttvisible) 
#define TClass_default_ttlayout	(&TTD.Class_default.class_ttlayout) 
#define TClass_default_ttboxlayout	(&TTD.Class_default.class_ttboxlayout) 
#define TClass_default_ttborderlayout	(&TTD.Class_default.class_ttborderlayout) 
#define TClass_default_ttnative	(&TTD.Class_default.class_ttnative) 
#define TClass_default_ttwidget	(&TTD.Class_default.class_ttwidget) 
#define TClass_default_ttlabel	(&TTD.Class_default.class_ttlabel) 
#define TClass_default_tttooltip	(&TTD.Class_default.class_tttooltip) 
#define TClass_default_ttbuttongroup	(&TTD.Class_default.class_ttbuttongroup) 
#define TClass_default_ttanybutton	(&TTD.Class_default.class_ttanybutton) 
#define TClass_default_ttbutton	(&TTD.Class_default.class_ttbutton) 
#define TClass_default_ttcheckbutton	(&TTD.Class_default.class_ttcheckbutton) 
#define TClass_default_ttradiobutton	(&TTD.Class_default.class_ttradiobutton) 
#define TClass_default_ttanyscroll	(&TTD.Class_default.class_ttanyscroll) 
#define TClass_default_ttscrollbar	(&TTD.Class_default.class_ttscrollbar) 
#define TClass_default_ttslider	(&TTD.Class_default.class_ttslider) 
#define TClass_default_ttprogressbar	(&TTD.Class_default.class_ttprogressbar) 
#define TClass_default_ttscrollpane	(&TTD.Class_default.class_ttscrollpane) 
#define TClass_default_ttwindow	(&TTD.Class_default.class_ttwindow) 
#define TClass_default_ttframe	(&TTD.Class_default.class_ttframe) 
#define TClass_default_ttscroller	(&TTD.Class_default.class_ttscroller) 
#define TClass_default_ttmenuitem	(&TTD.Class_default.class_ttmenuitem) 
#define TClass_default_ttcheckmenuitem	(&TTD.Class_default.class_ttcheckmenuitem) 
#define TClass_default_ttradiomenuitem	(&TTD.Class_default.class_ttradiomenuitem) 
#define TClass_default_ttmenu	(&TTD.Class_default.class_ttmenu) 
#define TClass_default_ttmenubar	(&TTD.Class_default.class_ttmenubar) 
#define TClass_default_ttanytext	(&TTD.Class_default.class_ttanytext) 
#define TClass_default_tttextfield	(&TTD.Class_default.class_tttextfield) 
#define TClass_default_tttextarea	(&TTD.Class_default.class_tttextarea) 
#define TClass_default_tttheme	(&TTD.Class_default.class_tttheme) 
#define TClass_default_ttapplication	(&TTD.Class_default.class_ttapplication)

#define TClass_default(type) TT_CAT(TClass_default_,type)





/* prototypes for public, public_set, final and final_var methods */


 
/* ttobj methods */
                    
ttbyte  TTGetField_ttobj(TT_ARG_READ tt_obj o, TT_ARG_READ tt_obj which, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * value);

ttbyte  TTSetField_ttobj(tt_obj o, TT_ARG_READ tt_obj which, TT_ARG_READ TT_ARG_ARRAY((1)) ttarg * value);

ttbyte  TTChangeField_ttobj(tt_obj o, TT_ARG_READ tt_obj which, ttany nand_value, ttany xor_value);

 
/* ttclass methods */

TT_ARG_READ TT_ARG_ARRAY_Z ttbyte *  TTGetName_ttclass(TT_ARG_READ tt_obj o);

ttopaque  TTGetSize_ttclass(TT_ARG_READ tt_obj o);

TT_ARG_READ tt_class  TTGetSuper_ttclass(TT_ARG_READ tt_obj o);

ttbyte  TTIsInstance_ttclass(TT_ARG_READ tt_obj o, TT_ARG_READ tt_obj t);

TT_ARG_READ tt_class  TTFromType_ttclass(ttopaque type);

ttbyte  TTIsArrayType_ttclass(ttopaque type);

ttbyte  TTIsPrimitiveType_ttclass(ttopaque type);

 
/* ttobject methods */
                
void  TTRef_ttobject(tt_obj o);

void  TTUnref_ttobject(tt_obj o);

ttbyte  TTSetUserData_ttobject(tt_obj o, ttany user_data);

 
/* ttvector methods */
    
ttany  TTGetAt_ttvector(TT_ARG_READ tt_obj o, ttopaque pos);
    
ttbyte  TTSetAt_ttvector(tt_obj o, ttopaque pos, ttany value);

tt_obj  TTCreate1_ttvector(ttany value);

tt_obj  TTCreateA_ttvector(ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttarg * values);

tt_obj  TTCreateL_ttvector(ttopaque value_n, ... );

tt_obj  TTCreateR_ttvector(TT_ARG_READ tt_obj v);

tt_obj  TTCreateV_ttvector(ttopaque value_n, TT_ARG_ARRAY((1)) va_list * values);

tt_obj  TTCreateY_ttvector(ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttany * values);

ttbyte  TTAppend_ttvector(tt_obj o, ttany value);

ttbyte  TTAdd_ttvector(tt_obj o, ttopaque pos, ttany value);

ttbyte  TTAddA_ttvector(tt_obj o, ttopaque pos, ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttarg * values);

ttbyte  TTAddL_ttvector(tt_obj o, ttopaque pos, ttopaque value_n, ... );

ttbyte  TTAddR_ttvector(tt_obj o, ttopaque pos, TT_ARG_READ tt_obj v);

ttbyte  TTAddV_ttvector(tt_obj o, ttopaque pos, ttopaque value_n, TT_ARG_ARRAY((1)) va_list * values);
    
ttbyte  TTAddY_ttvector(tt_obj o, ttopaque pos, ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttany * values);
    
ttopaque  TTContainsValue_ttvector(TT_ARG_READ tt_obj o, ttopaque pos_start, ttany value);

ttbyte  TTRemoveAt_ttvector(tt_obj o, ttopaque pos);
    
ttbyte  TTRemoveRange_ttvector(tt_obj o, ttopaque pos_start, ttopaque pos_n);

 
/* ttfield methods */

TT_ARG_READ tt_obj  TTGet_ttfield(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name);

ttbyte  TTGetValue_ttfield(TT_ARG_READ tt_obj f, TT_ARG_READ tt_obj o, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * value);

ttbyte  TTSetValue_ttfield(TT_ARG_READ tt_obj f, tt_obj o, TT_ARG_READ TT_ARG_ARRAY((1)) ttarg * value);

ttbyte  TTChangeValue_ttfield(TT_ARG_READ tt_obj f, tt_obj o, ttany nand_value, ttany xor_value);

ttopaque  TTToEvcode_ttfield(TT_ARG_READ tt_obj f);

TT_ARG_READ tt_obj  TTFromEvcode_ttfield(ttopaque i);

ttopaque  TTGetEvcode_ttfield(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name);

 
/* ttmethod methods */

TT_ARG_READ tt_obj  TTGet_ttmethod(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name);

TT_ARG_READ tt_obj  TTGetByAddress_ttmethod(TT_ARG_READ ttfunction_fn address);

ttopaque  TTGetArraySizeA_ttmethod(TT_ARG_READ tt_obj m, ttopaque arg_array_n, ttopaque args_n, TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttarg * args);

ttbyte  TTCheckArraySizesA_ttmethod(TT_ARG_READ tt_obj m, ttopaque args_n, TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttarg * args);

ttbyte  TTCallA_ttmethod(TT_ARG_READ tt_obj m, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value, ttopaque args_n, TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttarg * args);

ttbyte  TTCallL_ttmethod(TT_ARG_READ tt_obj m, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value, ttopaque args_n, ... );

ttbyte  TTCallR_ttmethod(TT_ARG_READ tt_obj m, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value, TT_ARG_READ tt_obj args);

ttbyte  TTCallV_ttmethod(TT_ARG_READ tt_obj m, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value, ttopaque args_n, TT_ARG_ARRAY((1)) va_list * args);

ttbyte  TTCallY_ttmethod(TT_ARG_READ tt_obj m, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg * return_value, ttopaque args_n, TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttany * args);

 
/* ttevent methods */

tt_obj  TTCreate_ttevent(ttuint evtype, ttuint evcode, ttuint evflags);

void  TTFire_ttevent(TT_ARG_DIE tt_obj ev, TT_ARG_READ tt_obj o);

 
/* tteventbig methods */

tt_obj  TTCreate8_tteventbig(ttuint evtype, ttuint evcode, ttuint evflags, ttshort x, ttshort y, ttshort w, ttshort h, ttany value, ttany old_value, ttopaque data_len, TT_ARG_READ TT_ARG_ARRAY((_P(10))) ttbyte * data);

tt_obj  TTCreate4_tteventbig(ttuint evtype, ttuint evcode, ttuint evflags, ttshort x, ttshort y, ttshort w, ttshort h);

tt_obj  TTCreate4s4_tteventbig(ttuint evtype, ttuint evcode, ttuint evflags, ttany value, ttany old_value, ttopaque data_len, TT_ARG_READ TT_ARG_ARRAY((_P(6))) ttbyte * data);

 
/* ttbitmask methods */

tt_obj  TTCreate_ttbitmask(ttany value);

tt_obj  TTCreateB2_ttbitmask(TT_ARG_READ tt_obj o);

tt_obj  TTCreateR_ttbitmask(TT_ARG_DIE tt_obj value);

tt_obj  TTCreateR2_ttbitmask(TT_ARG_READ tt_obj value);

void  TTClear_ttbitmask(tt_obj o);

void  TTFill_ttbitmask(tt_obj o);

void  TTNegate_ttbitmask(tt_obj o);

void  TTChange_ttbitmask(tt_obj o, ttuint truth_table);

ttbyte  TTContains_ttbitmask(TT_ARG_READ tt_obj o, ttany value);

ttbyte  TTAdd_ttbitmask(tt_obj o, ttany value);

ttbyte  TTAddB_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value);

ttbyte  TTAddR_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value);

ttbyte  TTRemove_ttbitmask(tt_obj o, ttany value);

ttbyte  TTRemoveB_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value);

ttbyte  TTRemoveR_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value);

ttbyte  TTCombine_ttbitmask(tt_obj o, ttany value, ttuint truth_table);

ttbyte  TTCombineR_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value, ttuint truth_table);

ttbyte  TTCombineB_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value, ttuint truth_table);

 
/* tteventmask methods */

tt_obj  TTCreateB_tteventmask(TT_ARG_DIE tt_obj evtype_mask, TT_ARG_DIE tt_obj evcode_mask, TT_ARG_DIE tt_obj component_mask);

tt_obj  TTCreateB2_tteventmask(TT_ARG_READ tt_obj evtype_mask, TT_ARG_READ tt_obj evcode_mask, TT_ARG_READ tt_obj component_mask);

ttbyte  TTSetEvtypeMask_tteventmask(tt_obj o, TT_ARG_DIE tt_obj evtype_mask);

ttbyte  TTSetEvcodeMask_tteventmask(tt_obj o, TT_ARG_DIE tt_obj evcode_mask);

ttbyte  TTSetComponentMask_tteventmask(tt_obj o, TT_ARG_DIE tt_obj component_mask);

ttbyte  TTSetTruthTable_tteventmask(tt_obj o, ttuint truth_table);

 
/* ttcallback methods */
    
 
/* ttlistener methods */

tt_obj  TTCreate_ttlistener(tt_obj o, ttuint evtype, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttany arg);

tt_obj  TTCreateE_ttlistener(tt_obj o, TT_ARG_DIE tt_obj ev, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttany arg);

tt_obj  TTCreateR_ttlistener(tt_obj o, TT_ARG_DIE tt_obj ev, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttopaque arg_component_n, ttopaque arg_event_n, TT_ARG_DIE tt_obj args);

tt_obj  TTCreateDel_ttlistener(tt_obj o, ttuint lflags, TT_ARG_READ ttlistener_fn function);

tt_obj  TTCreateAskclose_ttlistener(tt_obj o, ttuint lflags, TT_ARG_READ ttlistener_fn function);

tt_obj  TTCreateKey_ttlistener(tt_obj o, ttuint evcode, ttuint evflags, ttuint lflags, TT_ARG_READ ttlistener_fn function);

tt_obj  TTCreateMouse_ttlistener(tt_obj o, ttuint evcode, ttuint evflags, ttuint lflags, TT_ARG_READ ttlistener_fn function);

tt_obj  TTCreateActivate_ttlistener(tt_obj o, ttuint lflags, TT_ARG_READ ttlistener_fn function);

tt_obj  TTCreateChange_ttlistener(tt_obj o, ttuint evcode, ttuint lflags, TT_ARG_READ ttlistener_fn function);

tt_obj  TTCreateKeyData_ttlistener(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key, ttuint lflags, TT_ARG_READ ttlistener_fn function);

tt_obj  TTCreateExpose_ttlistener(tt_obj o, ttuint lflags, TT_ARG_READ ttlistener_fn function);
    
void  TTAddTo_ttlistener(tt_obj o, tt_obj c);
    
void  TTRemove_ttlistener(tt_obj o);

ttbyte  TTSetArgsR_ttlistener(tt_obj o, TT_ARG_DIE tt_obj args);

ttbyte  TTSetEventMask_ttlistener(tt_obj o, TT_ARG_DIE tt_obj event_mask);

 
/* tttimer methods */

tt_obj  TTCreateR_tttimer(tt_obj r, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttopaque arg_component_n, ttopaque arg_timer_n, TT_ARG_DIE tt_obj args, ttany delay_t, ttany delay_f);

tt_obj  TTCreate_tttimer(tt_obj r, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttany arg, ttany delay_t, ttany delay_f);

void  TTSetDelay_tttimer(tt_obj o, ttany delay_t, ttany delay_s);
    
void  TTSetEnabled_tttimer(tt_obj o, ttbyte enabled);
    
ttbyte  TTIsEnabled_tttimer(TT_ARG_READ tt_obj o);

 
/* ttcomponent methods */

void  TTAddListener_ttcomponent(tt_obj o, tt_obj l);

void  TTRemoveListener_ttcomponent(tt_obj o, tt_obj l);

tt_obj  TTSetKeyData_ttcomponent(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key, ttany data);

ttany  TTGetKeyData_ttcomponent(TT_ARG_READ tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key);

void  TTUnsetKeyData_ttcomponent(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key);

tt_obj  TTGetData_ttcomponent(TT_ARG_READ tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key);

 
/* ttdata methods */
        
ttbyte  TTSetData_ttdata(tt_obj o, ttany data);

 
/* ttvisible methods */
    
void  TTAddTo_ttvisible(tt_obj o, tt_obj parent, ttany constraint);
    
void  TTValidate_ttvisible(tt_obj o);
    
void  TTRemove_ttvisible(tt_obj o);
    
void  TTInvalidate_ttvisible(tt_obj o);
    
void  TTSetVisible_ttvisible(tt_obj o, ttbyte visible);
    
ttbyte  TTSetTheme_ttvisible(tt_obj o, TT_ARG_READ tt_obj theme);
    
void  TTDraw_ttvisible(tt_obj o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata);
    
void  TTBuiltinRepaint_ttvisible(tt_obj o, ttshort x, ttshort y, ttshort w, ttshort h);


ttbyte  TTSetRepaint_ttvisible(tt_obj o, TT_ARG_READ ttvisible_repaint_fn repaint);

void  TTExpose_ttvisible(tt_obj o, ttshort x, ttshort y, ttshort w, ttshort h);

void  TTAdd_ttvisible(tt_obj o, tt_obj child, ttany constraint);

 
/* ttlayout methods */
        
 
/* ttboxlayout methods */

tt_obj  TTCreate_ttboxlayout(ttany orientation);

 
/* ttborderlayout methods */

 
/* ttnative methods */
    
tt_obj  TTGetRoot_ttnative(void);
    
ttshort  TTGetW_ttnative(TT_ARG_READ tt_obj o);
    
ttshort  TTGetH_ttnative(TT_ARG_READ tt_obj o);

tt_obj  TTCreate_ttnative(ttany native_value);

 
/* ttwidget methods */
    
void  TTSetLayout_ttwidget(tt_obj o, tt_obj l);
    
ttbyte  TTSetXY_ttwidget(tt_obj o, ttshort x, ttshort y);
    
ttbyte  TTSetWH_ttwidget(tt_obj o, ttshort w, ttshort h);
    
ttbyte  TTSetXl_ttwidget(tt_obj o, ttint xl);
    
ttbyte  TTSetYl_ttwidget(tt_obj o, ttint yl);
    
ttbyte  TTSetWl_ttwidget(tt_obj o, ttint wl);
    
ttbyte  TTSetHl_ttwidget(tt_obj o, ttint hl);
    
ttbyte  TTSetTooltip_ttwidget(tt_obj o, TT_ARG_DIE tt_obj t);

ttbyte  TTSetXlYl_ttwidget(tt_obj o, ttuint mask, ttint xl, ttint yl);

ttbyte  TTSetWlHl_ttwidget(tt_obj o, ttuint mask, ttint wl, ttint hl);

ttbyte  TTSetXYWH_ttwidget(tt_obj o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);

ttbyte  TTSetX_ttwidget(tt_obj o, ttshort x);

ttbyte  TTSetY_ttwidget(tt_obj o, ttshort y);

ttbyte  TTSetW_ttwidget(tt_obj o, ttshort w);

ttbyte  TTSetH_ttwidget(tt_obj o, ttshort h);

 
/* ttlabel methods */
    
void  TTSetFontD_ttlabel(tt_obj o, ttopaque text_len, TT_ARG_DIE TT_ARG_ARRAY((_P(2))) ttfont * text);

ttbyte  TTSetFont_ttlabel(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttfont * text);

ttbyte  TTSetText_ttlabel(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * text);

tt_obj  TTCreate_ttlabel(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * text);

 
/* tttooltip methods */
    
ttbyte  TTSetWidget_tttooltip(TT_ARG_DIE tt_obj o, tt_obj w);

 
/* ttbuttongroup methods */
    
void  TTAdd_ttbuttongroup(tt_obj o, tt_obj r);
    
void  TTRemove_ttbuttongroup(tt_obj o, tt_obj r);
    
void  TTSetChecked_ttbuttongroup(tt_obj o, tt_obj r);

 
/* ttanybutton methods */
        
void  TTSetAttrD_ttanybutton(tt_obj o, ttshort w, ttshort h, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

ttbyte  TTSetAttr_ttanybutton(tt_obj o, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text);

ttbyte  TTSetFont_ttanybutton(tt_obj o, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttfont * text);

ttbyte  TTSetText_ttanybutton(tt_obj o, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttbyte * text);

tt_obj  TTCreate_ttanybutton(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);

 
/* ttbutton methods */
    
void  TTSetPressed_ttbutton(tt_obj o, ttbyte pressed);

ttbyte  TTIsPressed_ttbutton(TT_ARG_READ tt_obj o);

tt_obj  TTCreate_ttbutton(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);

 
/* ttcheckbutton methods */
    
void  TTSetChecked_ttcheckbutton(tt_obj o, ttbyte checked);

ttbyte  TTIsChecked_ttcheckbutton(TT_ARG_READ tt_obj o);

tt_obj  TTCreate_ttcheckbutton(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);

 
/* ttradiobutton methods */

void  TTAddToGroup_ttradiobutton(tt_obj o, tt_obj g);

void  TTRemoveFromGroup_ttradiobutton(tt_obj o);

tt_obj  TTCreate_ttradiobutton(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);

 
/* ttanyscroll methods */
            
ttbyte  TTSetOrientation_ttanyscroll(tt_obj o, ttbyte orientation);
    
ttbyte  TTSetSize_ttanyscroll(tt_obj o, ttint size);
    
ttbyte  TTSetRealSize_ttanyscroll(tt_obj o, ttint real_size);
    
ttbyte  TTSetViewSize_ttanyscroll(tt_obj o, ttint view_size);
    
ttbyte  TTSetPosition_ttanyscroll(tt_obj o, ttint position);

 
/* ttscrollbar methods */

tt_obj  TTCreate1_ttscrollbar(ttbyte orientation);

tt_obj  TTCreate4_ttscrollbar(ttbyte orientation, ttint size, ttint real_size, ttint view_size);

 
/* ttslider methods */
    
ttbyte  TTSetSlideMin_ttslider(tt_obj o, ttint slide_min);
    
ttbyte  TTSetSlideMax_ttslider(tt_obj o, ttint slide_max);
    
ttbyte  TTSetSlideValue_ttslider(tt_obj o, ttint slide_value);

tt_obj  TTCreate_ttslider(ttbyte orientation, ttint real_size);

 
/* ttprogressbar methods */

tt_obj  TTCreate_ttprogressbar(ttbyte orientation);

 
/* ttscrollpane methods */

tt_obj  TTCreate_ttscrollpane(ttshort w, ttshort h);
    
ttbyte  TTSetBarX_ttscrollpane(tt_obj o, tt_obj bar_x);
    
ttbyte  TTSetBarY_ttscrollpane(tt_obj o, tt_obj bar_y);

 
/* ttwindow methods */
    
ttbyte  TTSetTitle_ttwindow(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title);

 
/* ttframe methods */
    
ttbyte  TTSetMenubar_ttframe(tt_obj o, TT_ARG_READ tt_obj m);

tt_obj  TTCreate_ttframe(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title);

 
/* ttscroller methods */

tt_obj  TTCreate_ttscroller(ttshort w, ttshort h);

 
/* ttmenuitem methods */

 
/* ttcheckmenuitem methods */

 
/* ttradiomenuitem methods */

 
/* ttmenu methods */

 
/* ttmenubar methods */

tt_obj  TTGetDefault_ttmenubar(void);

 
/* ttanytext methods */

 
/* tttextfield methods */

 
/* tttextarea methods */

 
/* tttheme methods */

tt_obj  TTGetDefault_tttheme(void);

 
/* ttapplication methods */

tt_obj  TTCreate_ttapplication(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name);

tt_obj  TTGet_ttapplication(void);








/* prototypes for TTGet*_* and TTSet*_* methods */







 

/* prototypes for TTGet*_ttobj and TTSet*_ttobj methods */         

/* prototypes for TTGet*_ttclass and TTSet*_ttclass methods */             

/* prototypes for TTGet*_ttobject and TTSet*_ttobject methods */    
ttopaque 	TTGetRefcount_ttobject(TT_ARG_READ tt_obj o);    
ttuint 	TTGetOflags_ttobject(TT_ARG_READ tt_obj o);            
ttany 	TTGetUserData_ttobject(TT_ARG_READ tt_obj o);
ttbyte 	TTSetUserData_ttobject(tt_obj o, ttany user_data);    
ttopaque 	TTGetEventsInprogress_ttobject(TT_ARG_READ tt_obj o);    
ttopaque 	TTGetEventsInformed_ttobject(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttvector and TTSet*_ttvector methods */    
ttopaque 	TTGetArraySize_ttvector(TT_ARG_READ tt_obj o);    
ttopaque 	TTGetArrayN_ttvector(TT_ARG_READ tt_obj o);    
TT_ARG_ARRAY((_F(array_n))) ttany * 	TTGetArray_ttvector(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttfield and TTSet*_ttfield methods */    
ttopaque 	TTGetNameLen_ttfield(TT_ARG_READ tt_obj o);    
TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * 	TTGetName_ttfield(TT_ARG_READ tt_obj o);    
ttopaque 	TTGetType_ttfield(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttmethod and TTSet*_ttmethod methods */    
ttopaque 	TTGetNameLen_ttmethod(TT_ARG_READ tt_obj o);    
TT_ARG_READ TT_ARG_ARRAY((_F(name_len))) ttbyte * 	TTGetName_ttmethod(TT_ARG_READ tt_obj o);    
ttfunction_fn 	TTGetAddress_ttmethod(TT_ARG_READ tt_obj o);    
ttopaque 	TTGetReturnType_ttmethod(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetParameterType_ttmethod(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttevent and TTSet*_ttevent methods */    
tt_obj 	TTGetComponent_ttevent(TT_ARG_READ tt_obj o);    
ttuint 	TTGetEvtype_ttevent(TT_ARG_READ tt_obj o);    
ttuint 	TTGetEvcode_ttevent(TT_ARG_READ tt_obj o);    
ttuint 	TTGetEvflags_ttevent(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetPrev_ttevent(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetNext_ttevent(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_tteventbig and TTSet*_tteventbig methods */    
ttshort 	TTGetX_tteventbig(TT_ARG_READ tt_obj o);    
ttshort 	TTGetY_tteventbig(TT_ARG_READ tt_obj o);    
ttshort 	TTGetW_tteventbig(TT_ARG_READ tt_obj o);    
ttshort 	TTGetH_tteventbig(TT_ARG_READ tt_obj o);    
ttany 	TTGetValue_tteventbig(TT_ARG_READ tt_obj o);    
ttany 	TTGetOldValue_tteventbig(TT_ARG_READ tt_obj o);    
ttopaque 	TTGetDataLen_tteventbig(TT_ARG_READ tt_obj o);    
TT_ARG_READ TT_ARG_ARRAY((_F(data_len))) ttbyte * 	TTGetData_tteventbig(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttbitmask and TTSet*_ttbitmask methods */                 

/* prototypes for TTGet*_tteventmask and TTSet*_tteventmask methods */    
tt_obj 	TTGetEvtypeMask_tteventmask(TT_ARG_READ tt_obj o);
ttbyte 	TTSetEvtypeMask_tteventmask(tt_obj o, tt_obj evtype_mask);    
tt_obj 	TTGetEvcodeMask_tteventmask(TT_ARG_READ tt_obj o);
ttbyte 	TTSetEvcodeMask_tteventmask(tt_obj o, tt_obj evcode_mask);    
tt_obj 	TTGetComponentMask_tteventmask(TT_ARG_READ tt_obj o);
ttbyte 	TTSetComponentMask_tteventmask(tt_obj o, tt_obj component_mask);    
ttuint 	TTGetTruthTable_tteventmask(TT_ARG_READ tt_obj o);
ttbyte 	TTSetTruthTable_tteventmask(tt_obj o, ttuint truth_table); 

/* prototypes for TTGet*_ttcallback and TTSet*_ttcallback methods */    
ttuint 	TTGetLflags_ttcallback(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetComponent_ttcallback(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetPrev_ttcallback(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetNext_ttcallback(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttlistener and TTSet*_ttlistener methods */        
tt_obj 	TTGetEvent_ttlistener(TT_ARG_READ tt_obj o);    
ttlistener_fn 	TTGetFunction_ttlistener(TT_ARG_READ tt_obj o);    
ttopaque 	TTGetArgComponentN_ttlistener(TT_ARG_READ tt_obj o);    
ttopaque 	TTGetArgEventN_ttlistener(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetArgs_ttlistener(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetEventMask_ttlistener(TT_ARG_READ tt_obj o);
ttbyte 	TTSetEventMask_ttlistener(tt_obj o, tt_obj event_mask);         

/* prototypes for TTGet*_tttimer and TTSet*_tttimer methods */    
ttany 	TTGetDelayT_tttimer(TT_ARG_READ tt_obj o);    
ttany 	TTGetDelayF_tttimer(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetTimerPrev_tttimer(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetTimerNext_tttimer(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttcomponent and TTSet*_ttcomponent methods */    
tt_obj 	TTGetCallbacks_ttcomponent(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetListeners_ttcomponent(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetTimers_ttcomponent(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetDatas_ttcomponent(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttdata and TTSet*_ttdata methods */        
tt_obj 	TTGetComponent_ttdata(TT_ARG_READ tt_obj o);    
ttopaque 	TTGetKeyLen_ttdata(TT_ARG_READ tt_obj o);    
TT_ARG_ARRAY((_F(key_len))) ttbyte * 	TTGetKey_ttdata(TT_ARG_READ tt_obj o);    
ttany 	TTGetData_ttdata(TT_ARG_READ tt_obj o);
ttbyte 	TTSetData_ttdata(tt_obj o, ttany data); 

/* prototypes for TTGet*_ttvisible and TTSet*_ttvisible methods */    
ttuint 	TTGetVflags_ttvisible(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetPrev_ttvisible(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetNext_ttvisible(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetParent_ttvisible(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetChildFirst_ttvisible(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetChildLast_ttvisible(TT_ARG_READ tt_obj o);    
ttany 	TTGetConstraint_ttvisible(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetTheme_ttvisible(TT_ARG_READ tt_obj o);
ttbyte 	TTSetTheme_ttvisible(tt_obj o, tt_obj theme);    
ttvisible_repaint_fn 	TTGetRepaint_ttvisible(TT_ARG_READ tt_obj o);
ttbyte 	TTSetRepaint_ttvisible(tt_obj o, ttvisible_repaint_fn repaint); 

/* prototypes for TTGet*_ttlayout and TTSet*_ttlayout methods */    
tt_obj 	TTGetWidget_ttlayout(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttboxlayout and TTSet*_ttboxlayout methods */    
ttuint 	TTGetOrientation_ttboxlayout(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttborderlayout and TTSet*_ttborderlayout methods */ 

/* prototypes for TTGet*_ttnative and TTSet*_ttnative methods */ 

/* prototypes for TTGet*_ttwidget and TTSet*_ttwidget methods */    
ttshort 	TTGetX_ttwidget(TT_ARG_READ tt_obj o);
ttbyte 	TTSetX_ttwidget(tt_obj o, ttshort x);    
ttshort 	TTGetY_ttwidget(TT_ARG_READ tt_obj o);
ttbyte 	TTSetY_ttwidget(tt_obj o, ttshort y);    
ttshort 	TTGetW_ttwidget(TT_ARG_READ tt_obj o);
ttbyte 	TTSetW_ttwidget(tt_obj o, ttshort w);    
ttshort 	TTGetH_ttwidget(TT_ARG_READ tt_obj o);
ttbyte 	TTSetH_ttwidget(tt_obj o, ttshort h);    
tt_obj 	TTGetLayout_ttwidget(TT_ARG_READ tt_obj o);    
ttint 	TTGetXl_ttwidget(TT_ARG_READ tt_obj o);
ttbyte 	TTSetXl_ttwidget(tt_obj o, ttint xl);    
ttint 	TTGetYl_ttwidget(TT_ARG_READ tt_obj o);
ttbyte 	TTSetYl_ttwidget(tt_obj o, ttint yl);    
ttint 	TTGetWl_ttwidget(TT_ARG_READ tt_obj o);
ttbyte 	TTSetWl_ttwidget(tt_obj o, ttint wl);    
ttint 	TTGetHl_ttwidget(TT_ARG_READ tt_obj o);
ttbyte 	TTSetHl_ttwidget(tt_obj o, ttint hl);    
ttcol 	TTGetCol_ttwidget(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetTooltip_ttwidget(TT_ARG_READ tt_obj o);
ttbyte 	TTSetTooltip_ttwidget(tt_obj o, tt_obj tooltip); 

/* prototypes for TTGet*_ttlabel and TTSet*_ttlabel methods */    
ttopaque 	TTGetTextLen_ttlabel(TT_ARG_READ tt_obj o);    
TT_ARG_ARRAY((_F(text_len))) ttfont * 	TTGetText_ttlabel(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_tttooltip and TTSet*_tttooltip methods */        
tt_obj 	TTGetWidget_tttooltip(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttbuttongroup and TTSet*_ttbuttongroup methods */    
tt_obj 	TTGetGroupFirst_ttbuttongroup(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetGroupLast_ttbuttongroup(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetChecked_ttbuttongroup(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttanybutton and TTSet*_ttanybutton methods */    
ttshort 	TTGetTextWidth_ttanybutton(TT_ARG_READ tt_obj o);    
ttshort 	TTGetTextHeight_ttanybutton(TT_ARG_READ tt_obj o);    
TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * 	TTGetText_ttanybutton(TT_ARG_READ tt_obj o);     

/* prototypes for TTGet*_ttbutton and TTSet*_ttbutton methods */ 

/* prototypes for TTGet*_ttcheckbutton and TTSet*_ttcheckbutton methods */ 

/* prototypes for TTGet*_ttradiobutton and TTSet*_ttradiobutton methods */    
tt_obj 	TTGetGroup_ttradiobutton(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetGroupPrev_ttradiobutton(TT_ARG_READ tt_obj o);    
tt_obj 	TTGetGroupNext_ttradiobutton(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttanyscroll and TTSet*_ttanyscroll methods */    
ttbyte 	TTGetOrientation_ttanyscroll(TT_ARG_READ tt_obj o);
ttbyte 	TTSetOrientation_ttanyscroll(tt_obj o, ttbyte orientation);    
ttint 	TTGetSize_ttanyscroll(TT_ARG_READ tt_obj o);
ttbyte 	TTSetSize_ttanyscroll(tt_obj o, ttint size);    
ttint 	TTGetRealSize_ttanyscroll(TT_ARG_READ tt_obj o);
ttbyte 	TTSetRealSize_ttanyscroll(tt_obj o, ttint real_size);    
ttint 	TTGetViewSize_ttanyscroll(TT_ARG_READ tt_obj o);
ttbyte 	TTSetViewSize_ttanyscroll(tt_obj o, ttint view_size);        
ttint 	TTGetPosition_ttanyscroll(TT_ARG_READ tt_obj o);
ttbyte 	TTSetPosition_ttanyscroll(tt_obj o, ttint position);    
ttint 	TTGetRealPosition_ttanyscroll(TT_ARG_READ tt_obj o);    
ttuint 	TTGetState_ttanyscroll(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttscrollbar and TTSet*_ttscrollbar methods */     

/* prototypes for TTGet*_ttslider and TTSet*_ttslider methods */    
ttint 	TTGetSlideMin_ttslider(TT_ARG_READ tt_obj o);
ttbyte 	TTSetSlideMin_ttslider(tt_obj o, ttint slide_min);    
ttint 	TTGetSlideMax_ttslider(TT_ARG_READ tt_obj o);
ttbyte 	TTSetSlideMax_ttslider(tt_obj o, ttint slide_max);    
ttint 	TTGetSlideValue_ttslider(TT_ARG_READ tt_obj o);
ttbyte 	TTSetSlideValue_ttslider(tt_obj o, ttint slide_value); 

/* prototypes for TTGet*_ttprogressbar and TTSet*_ttprogressbar methods */         

/* prototypes for TTGet*_ttscrollpane and TTSet*_ttscrollpane methods */    
tt_obj 	TTGetBarX_ttscrollpane(TT_ARG_READ tt_obj o);
ttbyte 	TTSetBarX_ttscrollpane(tt_obj o, tt_obj bar_x);    
tt_obj 	TTGetBarY_ttscrollpane(TT_ARG_READ tt_obj o);
ttbyte 	TTSetBarY_ttscrollpane(tt_obj o, tt_obj bar_y); 

/* prototypes for TTGet*_ttwindow and TTSet*_ttwindow methods */    
ttopaque 	TTGetTitleLen_ttwindow(TT_ARG_READ tt_obj o);    
TT_ARG_ARRAY((_F(title_len))) ttbyte * 	TTGetTitle_ttwindow(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttframe and TTSet*_ttframe methods */    
tt_obj 	TTGetMenubar_ttframe(TT_ARG_READ tt_obj o);
ttbyte 	TTSetMenubar_ttframe(tt_obj o, tt_obj menubar); 

/* prototypes for TTGet*_ttscroller and TTSet*_ttscroller methods */    
tt_obj 	TTGetScrollpane_ttscroller(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttmenuitem and TTSet*_ttmenuitem methods */    
ttopaque 	TTGetNameLen_ttmenuitem(TT_ARG_READ tt_obj o);    
TT_ARG_ARRAY((_F(name_len))) ttbyte * 	TTGetName_ttmenuitem(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttcheckmenuitem and TTSet*_ttcheckmenuitem methods */ 

/* prototypes for TTGet*_ttradiomenuitem and TTSet*_ttradiomenuitem methods */ 

/* prototypes for TTGet*_ttmenu and TTSet*_ttmenu methods */    
tt_obj 	TTGetMenubar_ttmenu(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_ttmenubar and TTSet*_ttmenubar methods */ 

/* prototypes for TTGet*_ttanytext and TTSet*_ttanytext methods */    
ttopaque 	TTGetTextLen_ttanytext(TT_ARG_READ tt_obj o);    
TT_ARG_ARRAY((_F(text_len))) ttattr * 	TTGetText_ttanytext(TT_ARG_READ tt_obj o); 

/* prototypes for TTGet*_tttextfield and TTSet*_tttextfield methods */ 

/* prototypes for TTGet*_tttextarea and TTSet*_tttextarea methods */ 

/* prototypes for TTGet*_tttheme and TTSet*_tttheme methods */    
ttopaque 	TTGetNameLen_tttheme(TT_ARG_READ tt_obj o);    
TT_ARG_ARRAY((_F(name_len))) ttbyte * 	TTGetName_tttheme(TT_ARG_READ tt_obj o);                 

/* prototypes for TTGet*_ttapplication and TTSet*_ttapplication methods */    
ttopaque 	TTGetNameLen_ttapplication(TT_ARG_READ tt_obj o);    
TT_ARG_ARRAY((_F(name_len))) ttbyte * 	TTGetName_ttapplication(TT_ARG_READ tt_obj o);







#endif /* _TT_DECLSM4_H */

