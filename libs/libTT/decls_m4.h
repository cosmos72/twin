



/* This file was automatically generated from m4/decls.m4, do not edit! */


/*
 *  decls_m4.h  --  INTERNAL (!!) header for libTT types declarations
 *
 *  Note: you cannot include both this file and <TT/decls_m4.h>, as they conflict!
 */

#ifndef _TT_DECLSM4_H
#define _TT_DECLSM4_H

















/* many useful macros */

#define Min2(a,b) ((a) < (b) ? (a) : (b))

#define TT_OFFSETOF(type,field)	((size_t)&((type)0)->field)

/* #defines for objects hierarchy */



                                      
#define super_ttevent	ttobj                             
#define super_tteventbig	ttevent                                 
#define super_ttbitmask	ttobj                 
#define super_tteventmask	ttobj                 
#define super_ttcallback	ttobj                 
#define super_ttlistener	ttcallback                                         
#define super_tttimer	ttlistener                 
#define super_ttcomponent	ttobj                 
#define super_ttdata	ttcomponent                     
#define super_ttvisible	ttcomponent                                 
#define super_ttnative	ttvisible 
#define super_ttwidget	ttvisible                                         
#define super_ttlabel	ttwidget         
#define super_tttooltip	ttlabel             
#define super_ttbuttongroup	ttcomponent             
#define super_ttanybutton	ttwidget                 
#define super_ttbutton	ttanybutton 
#define super_ttcheckbutton	ttbutton 
#define super_ttradiobutton	ttcheckbutton             
#define super_ttanyscroll	ttanybutton                                 
#define super_ttscrollbar	ttanyscroll     
#define super_ttslider	ttanyscroll             
#define super_ttprogressbar	ttanyscroll         
#define super_ttscrollpane	ttwidget         
#define super_ttwindow	ttwidget         
#define super_ttframe	ttwindow     
#define super_ttscroller	ttwindow     
#define super_ttmenuitem	ttvisible     
#define super_ttcheckmenuitem	ttmenuitem 
#define super_ttradiomenuitem	ttcheckmenuitem 
#define super_ttmenu	ttmenuitem
     
#define super_ttmenubar	ttvisible 
#define super_ttanytext	ttwindow         
#define super_tttextfield	ttanytext 
#define super_tttextarea	tttextfield 
#define super_tttheme	ttcomponent
                     
#define super_ttapplication	ttcomponent    





typedef struct s_ttobj TT_CONST *	ttobj_c;

/* forward declarations for objects structures */


typedef struct s_ttobj *   ttobj;  
typedef struct s_ttevent *   ttevent; 
typedef struct s_tteventbig *   tteventbig; 
typedef struct s_ttbitmask *   ttbitmask; 
typedef struct s_tteventmask *   tteventmask; 
typedef struct s_ttcallback *   ttcallback; 
typedef struct s_ttlistener *   ttlistener; 
typedef struct s_tttimer *   tttimer; 
typedef struct s_ttcomponent *   ttcomponent; 
typedef struct s_ttdata *   ttdata; 
typedef struct s_ttvisible *   ttvisible; 
typedef struct s_ttnative *   ttnative; 
typedef struct s_ttwidget *   ttwidget; 
typedef struct s_ttlabel *   ttlabel; 
typedef struct s_tttooltip *   tttooltip; 
typedef struct s_ttbuttongroup *   ttbuttongroup; 
typedef struct s_ttanybutton *   ttanybutton; 
typedef struct s_ttbutton *   ttbutton; 
typedef struct s_ttcheckbutton *   ttcheckbutton; 
typedef struct s_ttradiobutton *   ttradiobutton; 
typedef struct s_ttanyscroll *   ttanyscroll; 
typedef struct s_ttscrollbar *   ttscrollbar; 
typedef struct s_ttslider *   ttslider; 
typedef struct s_ttprogressbar *   ttprogressbar; 
typedef struct s_ttscrollpane *   ttscrollpane; 
typedef struct s_ttwindow *   ttwindow; 
typedef struct s_ttframe *   ttframe; 
typedef struct s_ttscroller *   ttscroller; 
typedef struct s_ttmenuitem *   ttmenuitem; 
typedef struct s_ttcheckmenuitem *   ttcheckmenuitem; 
typedef struct s_ttradiomenuitem *   ttradiomenuitem; 
typedef struct s_ttmenu *   ttmenu; 
typedef struct s_ttmenubar *   ttmenubar; 
typedef struct s_ttanytext *   ttanytext; 
typedef struct s_tttextfield *   tttextfield; 
typedef struct s_tttextarea *   tttextarea; 
typedef struct s_tttheme *   tttheme; 
typedef struct s_ttapplication *   ttapplication;



/* forward declarations for methods structures */


typedef struct s_ttfn_ttobj *	ttfn_ttobj;  
typedef struct s_ttfn_ttevent *	ttfn_ttevent; 
typedef struct s_ttfn_tteventbig *	ttfn_tteventbig; 
typedef struct s_ttfn_ttbitmask *	ttfn_ttbitmask; 
typedef struct s_ttfn_tteventmask *	ttfn_tteventmask; 
typedef struct s_ttfn_ttcallback *	ttfn_ttcallback; 
typedef struct s_ttfn_ttlistener *	ttfn_ttlistener; 
typedef struct s_ttfn_tttimer *	ttfn_tttimer; 
typedef struct s_ttfn_ttcomponent *	ttfn_ttcomponent; 
typedef struct s_ttfn_ttdata *	ttfn_ttdata; 
typedef struct s_ttfn_ttvisible *	ttfn_ttvisible; 
typedef struct s_ttfn_ttnative *	ttfn_ttnative; 
typedef struct s_ttfn_ttwidget *	ttfn_ttwidget; 
typedef struct s_ttfn_ttlabel *	ttfn_ttlabel; 
typedef struct s_ttfn_tttooltip *	ttfn_tttooltip; 
typedef struct s_ttfn_ttbuttongroup *	ttfn_ttbuttongroup; 
typedef struct s_ttfn_ttanybutton *	ttfn_ttanybutton; 
typedef struct s_ttfn_ttbutton *	ttfn_ttbutton; 
typedef struct s_ttfn_ttcheckbutton *	ttfn_ttcheckbutton; 
typedef struct s_ttfn_ttradiobutton *	ttfn_ttradiobutton; 
typedef struct s_ttfn_ttanyscroll *	ttfn_ttanyscroll; 
typedef struct s_ttfn_ttscrollbar *	ttfn_ttscrollbar; 
typedef struct s_ttfn_ttslider *	ttfn_ttslider; 
typedef struct s_ttfn_ttprogressbar *	ttfn_ttprogressbar; 
typedef struct s_ttfn_ttscrollpane *	ttfn_ttscrollpane; 
typedef struct s_ttfn_ttwindow *	ttfn_ttwindow; 
typedef struct s_ttfn_ttframe *	ttfn_ttframe; 
typedef struct s_ttfn_ttscroller *	ttfn_ttscroller; 
typedef struct s_ttfn_ttmenuitem *	ttfn_ttmenuitem; 
typedef struct s_ttfn_ttcheckmenuitem *	ttfn_ttcheckmenuitem; 
typedef struct s_ttfn_ttradiomenuitem *	ttfn_ttradiomenuitem; 
typedef struct s_ttfn_ttmenu *	ttfn_ttmenu; 
typedef struct s_ttfn_ttmenubar *	ttfn_ttmenubar; 
typedef struct s_ttfn_ttanytext *	ttfn_ttanytext; 
typedef struct s_ttfn_tttextfield *	ttfn_tttextfield; 
typedef struct s_ttfn_tttextarea *	ttfn_tttextarea; 
typedef struct s_ttfn_tttheme *	ttfn_tttheme; 
typedef struct s_ttfn_ttapplication *	ttfn_ttapplication;


typedef ttfn_ttobj ttfn;


/* declarations for objects structures */




typedef struct s_ttobj {
    ttfn_ttobj FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
} s_ttobj;
  
typedef struct s_ttevent {
    ttfn_ttevent FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttlistener callback;
    ttcomponent component;
    ttuint evtype;
    ttuint evcode;
    ttuint evflags;
    ttevent prev;
    ttevent next;
} s_ttevent;
 
typedef struct s_tteventbig {
    ttfn_tteventbig FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttlistener callback;
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
    ttuint len;
    TT_CONST ttbyte * data;
} s_tteventbig;
 
typedef struct s_ttbitmask {
    ttfn_ttbitmask FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttopaque mask0;
    ttopaque mask_max;
    ttopaque mask_n;
    ttany * mask;
} s_ttbitmask;
 
typedef struct s_tteventmask {
    ttfn_tteventmask FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttbitmask evtype_mask;
    ttbitmask evcode_mask;
    ttbitmask component_mask;
    ttuint truth_table;
} s_tteventmask;
 
typedef struct s_ttcallback {
    ttfn_ttcallback FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttuint lflags;
    ttcomponent component;
    ttcallback prev;
    ttcallback next;
} s_ttcallback;
 
typedef struct s_ttlistener {
    ttfn_ttlistener FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttuint lflags;
    ttcomponent component;
    ttlistener prev;
    ttlistener next;
    /* extends ttcallback */
    s_ttavl AVL;
    ttevent event;
    ttuint narg_component;
    ttuint narg_event;
    ttuint nargs;
    ttlistener_fn function;
    ttany * args;
    tteventmask event_mask;
    ttlistener inprogress_prev;
    ttlistener inprogress_next;
} s_ttlistener;
 
typedef struct s_tttimer {
    ttfn_tttimer FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttuint lflags;
    ttcomponent component;
    tttimer prev;
    tttimer next;
    /* extends ttcallback */
    s_ttavl AVL;
    ttevent event;
    ttuint narg_component;
    ttuint narg_event;
    ttuint nargs;
    ttlistener_fn function;
    ttany * args;
    tteventmask event_mask;
    ttlistener inprogress_prev;
    ttlistener inprogress_next;
    /* extends ttlistener */
    time_t t;
    frac_t f;
    tttimer timer_prev;
    tttimer timer_next;
} s_tttimer;
 
typedef struct s_ttcomponent {
    ttfn_ttcomponent FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
} s_ttcomponent;
 
typedef struct s_ttdata {
    ttfn_ttdata FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    s_ttavl AVL;
    ttcomponent component;
    ttbyte * key;
    ttopaque key_len;
    ttany data;
} s_ttdata;
 
typedef struct s_ttvisible {
    ttfn_ttvisible FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
} s_ttvisible;
 
typedef struct s_ttnative {
    ttfn_ttnative FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
} s_ttnative;
 
typedef struct s_ttwidget {
    ttfn_ttwidget FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
} s_ttwidget;
 
typedef struct s_ttlabel {
    ttfn_ttlabel FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque text_len;
    ttfont * text;
} s_ttlabel;
 
typedef struct s_tttooltip {
    ttfn_tttooltip FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque text_len;
    ttfont * text;
    /* extends ttlabel */
    ttopaque widget_id;
    ttwidget widget;
    ttlistener widget_listener;
} s_tttooltip;
 
typedef struct s_ttbuttongroup {
    ttfn_ttbuttongroup FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    ttfn_ttanybutton FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttattr * text;
    ttshort text_width;
    ttshort text_height;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
} s_ttanybutton;
 
typedef struct s_ttbutton {
    ttfn_ttbutton FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttattr * text;
    ttshort text_width;
    ttshort text_height;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
    /* extends ttanybutton */
} s_ttbutton;
 
typedef struct s_ttcheckbutton {
    ttfn_ttcheckbutton FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttattr * text;
    ttshort text_width;
    ttshort text_height;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
    /* extends ttanybutton */
    /* extends ttbutton */
} s_ttcheckbutton;
 
typedef struct s_ttradiobutton {
    ttfn_ttradiobutton FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttattr * text;
    ttshort text_width;
    ttshort text_height;
    struct s_ttshape theme_shape[ttderived_ttanybutton_shape_max];
    /* extends ttanybutton */
    /* extends ttbutton */
    /* extends ttcheckbutton */
    ttbuttongroup group;
    ttradiobutton group_prev;
    ttradiobutton group_next;
} s_ttradiobutton;
 
typedef struct s_ttanyscroll {
    ttfn_ttanyscroll FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttattr * text;
    ttshort text_width;
    ttshort text_height;
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
    ttfn_ttscrollbar FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttattr * text;
    ttshort text_width;
    ttshort text_height;
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
    ttfn_ttslider FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttattr * text;
    ttshort text_width;
    ttshort text_height;
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
    ttfn_ttprogressbar FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttattr * text;
    ttshort text_width;
    ttshort text_height;
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
    ttfn_ttscrollpane FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
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
    ttfn_ttwindow FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    ttbyte * title;
} s_ttwindow;
 
typedef struct s_ttframe {
    ttfn_ttframe FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    ttbyte * title;
    /* extends ttwindow */
    ttmenubar menubar;
} s_ttframe;
 
typedef struct s_ttscroller {
    ttfn_ttscroller FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    ttbyte * title;
    /* extends ttwindow */
    ttscrollpane scrollpane;
} s_ttscroller;
 
typedef struct s_ttmenuitem {
    ttfn_ttmenuitem FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttbyte * name;
} s_ttmenuitem;
 
typedef struct s_ttcheckmenuitem {
    ttfn_ttcheckmenuitem FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttbyte * name;
    /* extends ttmenuitem */
} s_ttcheckmenuitem;
 
typedef struct s_ttradiomenuitem {
    ttfn_ttradiomenuitem FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttbyte * name;
    /* extends ttmenuitem */
    /* extends ttcheckmenuitem */
} s_ttradiomenuitem;
 
typedef struct s_ttmenu {
    ttfn_ttmenu FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttbyte * name;
    /* extends ttmenuitem */

    ttmenubar menubar;
} s_ttmenu;
 
typedef struct s_ttmenubar {
    ttfn_ttmenubar FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
} s_ttmenubar;
 
typedef struct s_ttanytext {
    ttfn_ttanytext FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    ttbyte * title;
    /* extends ttwindow */
    ttopaque text_len;
    ttattr * text;
} s_ttanytext;
 
typedef struct s_tttextfield {
    ttfn_tttextfield FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    ttbyte * title;
    /* extends ttwindow */
    ttopaque text_len;
    ttattr * text;
    /* extends ttanytext */
} s_tttextfield;
 
typedef struct s_tttextarea {
    ttfn_tttextarea FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
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
    tttheme theme;
    ttvisible_repaint_fn repaint;
    /* extends ttvisible */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttint xl;
    ttint yl;
    ttint wl;
    ttint hl;
    ttcol col;
    tttooltip tooltip;
    /* extends ttwidget */
    ttopaque title_len;
    ttbyte * title;
    /* extends ttwindow */
    ttopaque text_len;
    ttattr * text;
    /* extends ttanytext */
    /* extends tttextfield */
} s_tttextarea;
 
typedef struct s_tttheme {
    ttfn_tttheme FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */

    ttbyte * name;
    ttfont fill;
    ttcol bg[tttheme_bg_max];
    ttint val[tttheme_val_max];
    struct s_ttshape shape[tttheme_shape_max][2];
} s_tttheme;
 
typedef struct s_ttapplication {
    ttfn_ttapplication FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany target_private;
    ttany user_data;
    ttopaque events_inprogress;
    ttopaque events_informed;
    /* extends ttobj */
    ttcallback callbacks;
    ttlistener listeners;
    tttimer timers;
    ttdata datas;
    /* extends ttcomponent */
    ttbyte * name;
} s_ttapplication;






/* declarations for methods structures */






typedef struct s_ttfn_ttobj {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttobj (*New)(ttfn_ttobj FN, ttobj o);
    ttobj (*Build)(ttobj o);
    void (*Break)(ttobj o);
    void (*Del)(TT_ARG_DIE ttobj o);

} s_ttfn_ttobj;
  
typedef struct s_ttfn_ttevent {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttevent (*New)(ttfn_ttevent FN, ttevent o);
    ttevent (*Build)(ttevent o);
    void (*Break)(ttevent o);
    void (*Del)(TT_ARG_DIE ttevent o);

    /* extends ttfn_ttobj */

} s_ttfn_ttevent;
 
typedef struct s_ttfn_tteventbig {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tteventbig (*New)(ttfn_tteventbig FN, tteventbig o);
    tteventbig (*Build)(tteventbig o);
    void (*Break)(tteventbig o);
    void (*Del)(TT_ARG_DIE tteventbig o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttevent */


} s_ttfn_tteventbig;
 
typedef struct s_ttfn_ttbitmask {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttbitmask (*New)(ttfn_ttbitmask FN, ttbitmask o);
    ttbitmask (*Build)(ttbitmask o);
    void (*Break)(ttbitmask o);
    void (*Del)(TT_ARG_DIE ttbitmask o);

    /* extends ttfn_ttobj */

} s_ttfn_ttbitmask;
 
typedef struct s_ttfn_tteventmask {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tteventmask (*New)(ttfn_tteventmask FN, tteventmask o);
    tteventmask (*Build)(tteventmask o);
    void (*Break)(tteventmask o);
    void (*Del)(TT_ARG_DIE tteventmask o);

    /* extends ttfn_ttobj */

} s_ttfn_tteventmask;
 
typedef struct s_ttfn_ttcallback {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttcallback (*New)(ttfn_ttcallback FN, ttcallback o);
    ttcallback (*Build)(ttcallback o);
    void (*Break)(ttcallback o);
    void (*Del)(TT_ARG_DIE ttcallback o);

    /* extends ttfn_ttobj */
    ttcallback (*Create)(ttcomponent o);

} s_ttfn_ttcallback;
 
typedef struct s_ttfn_ttlistener {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttlistener (*New)(ttfn_ttlistener FN, ttlistener o);
    ttlistener (*Build)(ttlistener o);
    void (*Break)(ttlistener o);
    void (*Del)(TT_ARG_DIE ttlistener o);

    /* extends ttfn_ttobj */
    ttcallback (*Create)(ttcomponent o);

    /* extends ttfn_ttcallback */


    void (*AddTo)(ttlistener o, ttcomponent c);
    void (*Remove)(ttlistener o);

} s_ttfn_ttlistener;
 
typedef struct s_ttfn_tttimer {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tttimer (*New)(ttfn_tttimer FN, tttimer o);
    tttimer (*Build)(tttimer o);
    void (*Break)(tttimer o);
    void (*Del)(TT_ARG_DIE tttimer o);

    /* extends ttfn_ttobj */
    ttcallback (*Create)(ttcomponent o);

    /* extends ttfn_ttcallback */


    void (*AddTo)(tttimer o, ttcomponent c);
    void (*Remove)(tttimer o);

    /* extends ttfn_ttlistener */
    void (*SetEnabled)(tttimer o, ttbyte enabled);
    ttbyte (*IsEnabled)(TT_ARG_READ tttimer o);

} s_ttfn_tttimer;
 
typedef struct s_ttfn_ttcomponent {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttcomponent (*New)(ttfn_ttcomponent FN, ttcomponent o);
    ttcomponent (*Build)(ttcomponent o);
    void (*Break)(ttcomponent o);
    void (*Del)(TT_ARG_DIE ttcomponent o);

    /* extends ttfn_ttobj */

} s_ttfn_ttcomponent;
 
typedef struct s_ttfn_ttdata {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttdata (*New)(ttfn_ttdata FN, ttdata o);
    ttdata (*Build)(ttdata o);
    void (*Break)(ttdata o);
    void (*Del)(TT_ARG_DIE ttdata o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttdata o, ttcomponent c);
    void (*Remove)(ttdata o);

} s_ttfn_ttdata;
 
typedef struct s_ttfn_ttvisible {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttvisible (*New)(ttfn_ttvisible FN, ttvisible o);
    ttvisible (*Build)(ttvisible o);
    void (*Break)(ttvisible o);
    void (*Del)(TT_ARG_DIE ttvisible o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttvisible o, ttvisible parent);
    void (*Remove)(ttvisible o);
    void (*Invalidate)(ttvisible o);
    void (*SetVisible)(ttvisible o, ttbyte on_off);
    void (*SetTheme)(ttvisible o, tttheme theme);
    void (*Draw)(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h);

} s_ttfn_ttvisible;
 
typedef struct s_ttfn_ttnative {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttnative (*New)(ttfn_ttnative FN, ttnative o);
    ttnative (*Build)(ttnative o);
    void (*Break)(ttnative o);
    void (*Del)(TT_ARG_DIE ttnative o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttnative o, ttvisible parent);
    void (*Remove)(ttnative o);
    void (*Invalidate)(ttnative o);
    void (*SetVisible)(ttnative o, ttbyte on_off);
    void (*SetTheme)(ttnative o, tttheme theme);
    void (*Draw)(ttnative o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttnative o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    ttnative (*GetRoot)(void);
    ttshort (*GetW)(TT_ARG_READ ttnative o);
    ttshort (*GetH)(TT_ARG_READ ttnative o);

} s_ttfn_ttnative;
 
typedef struct s_ttfn_ttwidget {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttwidget (*New)(ttfn_ttwidget FN, ttwidget o);
    ttwidget (*Build)(ttwidget o);
    void (*Break)(ttwidget o);
    void (*Del)(TT_ARG_DIE ttwidget o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttwidget o, ttvisible parent);
    void (*Remove)(ttwidget o);
    void (*Invalidate)(ttwidget o);
    void (*SetVisible)(ttwidget o, ttbyte on_off);
    void (*SetTheme)(ttwidget o, tttheme theme);
    void (*Draw)(ttwidget o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttwidget o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttwidget o, ttshort x, ttshort y);
    void (*SetWH)(ttwidget o, ttshort w, ttshort h);
    void (*SetXl)(ttwidget o, ttint xl);
    void (*SetYl)(ttwidget o, ttint yl);
    void (*SetWl)(ttwidget o, ttint wl);
    void (*SetHl)(ttwidget o, ttint hl);
    void (*SetTooltip)(ttwidget o, tttooltip t);

} s_ttfn_ttwidget;
 
typedef struct s_ttfn_ttlabel {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttlabel (*New)(ttfn_ttlabel FN, ttlabel o);
    ttlabel (*Build)(ttlabel o);
    void (*Break)(ttlabel o);
    void (*Del)(TT_ARG_DIE ttlabel o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttlabel o, ttvisible parent);
    void (*Remove)(ttlabel o);
    void (*Invalidate)(ttlabel o);
    void (*SetVisible)(ttlabel o, ttbyte on_off);
    void (*SetTheme)(ttlabel o, tttheme theme);
    void (*Draw)(ttlabel o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttlabel o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttlabel o, ttshort x, ttshort y);
    void (*SetWH)(ttlabel o, ttshort w, ttshort h);
    void (*SetXl)(ttlabel o, ttint xl);
    void (*SetYl)(ttlabel o, ttint yl);
    void (*SetWl)(ttlabel o, ttint wl);
    void (*SetHl)(ttlabel o, ttint hl);
    void (*SetTooltip)(ttlabel o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttbyte (*DoSetFont)(ttlabel o, TT_ARG_DIE ttfont * text, ttopaque len);

} s_ttfn_ttlabel;
 
typedef struct s_ttfn_tttooltip {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tttooltip (*New)(ttfn_tttooltip FN, tttooltip o);
    tttooltip (*Build)(tttooltip o);
    void (*Break)(tttooltip o);
    void (*Del)(TT_ARG_DIE tttooltip o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(tttooltip o, ttvisible parent);
    void (*Remove)(tttooltip o);
    void (*Invalidate)(tttooltip o);
    void (*SetVisible)(tttooltip o, ttbyte on_off);
    void (*SetTheme)(tttooltip o, tttheme theme);
    void (*Draw)(tttooltip o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(tttooltip o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(tttooltip o, ttshort x, ttshort y);
    void (*SetWH)(tttooltip o, ttshort w, ttshort h);
    void (*SetXl)(tttooltip o, ttint xl);
    void (*SetYl)(tttooltip o, ttint yl);
    void (*SetWl)(tttooltip o, ttint wl);
    void (*SetHl)(tttooltip o, ttint hl);
    void (*SetTooltip)(tttooltip o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttbyte (*DoSetFont)(tttooltip o, TT_ARG_DIE ttfont * text, ttopaque len);

    /* extends ttfn_ttlabel */
    void (*SetWidget)(tttooltip o, ttwidget widget);

} s_ttfn_tttooltip;
 
typedef struct s_ttfn_ttbuttongroup {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttbuttongroup (*New)(ttfn_ttbuttongroup FN, ttbuttongroup o);
    ttbuttongroup (*Build)(ttbuttongroup o);
    void (*Break)(ttbuttongroup o);
    void (*Del)(TT_ARG_DIE ttbuttongroup o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*Add)(ttbuttongroup o, ttradiobutton r);
    void (*Remove)(ttbuttongroup o, ttradiobutton r);
    void (*SetChecked)(ttbuttongroup o, ttradiobutton r);

} s_ttfn_ttbuttongroup;
 
typedef struct s_ttfn_ttanybutton {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttanybutton (*New)(ttfn_ttanybutton FN, ttanybutton o);
    ttanybutton (*Build)(ttanybutton o);
    void (*Break)(ttanybutton o);
    void (*Del)(TT_ARG_DIE ttanybutton o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttanybutton o, ttvisible parent);
    void (*Remove)(ttanybutton o);
    void (*Invalidate)(ttanybutton o);
    void (*SetVisible)(ttanybutton o, ttbyte on_off);
    void (*SetTheme)(ttanybutton o, tttheme theme);
    void (*Draw)(ttanybutton o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttanybutton o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttanybutton o, ttshort x, ttshort y);
    void (*SetWH)(ttanybutton o, ttshort w, ttshort h);
    void (*SetXl)(ttanybutton o, ttint xl);
    void (*SetYl)(ttanybutton o, ttint yl);
    void (*SetWl)(ttanybutton o, ttint wl);
    void (*SetHl)(ttanybutton o, ttint hl);
    void (*SetTooltip)(ttanybutton o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttuint (*CacheThemeShape)(ttanybutton o);
    ttbyte (*DoSetAttr)(ttanybutton o, TT_ARG_DIE ttattr * text, ttshort width, ttshort height, ttshort pitch);

} s_ttfn_ttanybutton;
 
typedef struct s_ttfn_ttbutton {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttbutton (*New)(ttfn_ttbutton FN, ttbutton o);
    ttbutton (*Build)(ttbutton o);
    void (*Break)(ttbutton o);
    void (*Del)(TT_ARG_DIE ttbutton o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttbutton o, ttvisible parent);
    void (*Remove)(ttbutton o);
    void (*Invalidate)(ttbutton o);
    void (*SetVisible)(ttbutton o, ttbyte on_off);
    void (*SetTheme)(ttbutton o, tttheme theme);
    void (*Draw)(ttbutton o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttbutton o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttbutton o, ttshort x, ttshort y);
    void (*SetWH)(ttbutton o, ttshort w, ttshort h);
    void (*SetXl)(ttbutton o, ttint xl);
    void (*SetYl)(ttbutton o, ttint yl);
    void (*SetWl)(ttbutton o, ttint wl);
    void (*SetHl)(ttbutton o, ttint hl);
    void (*SetTooltip)(ttbutton o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttuint (*CacheThemeShape)(ttbutton o);
    ttbyte (*DoSetAttr)(ttbutton o, TT_ARG_DIE ttattr * text, ttshort width, ttshort height, ttshort pitch);

    /* extends ttfn_ttanybutton */
    void (*SetPressed)(ttbutton o, ttbyte pressed);

} s_ttfn_ttbutton;
 
typedef struct s_ttfn_ttcheckbutton {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttcheckbutton (*New)(ttfn_ttcheckbutton FN, ttcheckbutton o);
    ttcheckbutton (*Build)(ttcheckbutton o);
    void (*Break)(ttcheckbutton o);
    void (*Del)(TT_ARG_DIE ttcheckbutton o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttcheckbutton o, ttvisible parent);
    void (*Remove)(ttcheckbutton o);
    void (*Invalidate)(ttcheckbutton o);
    void (*SetVisible)(ttcheckbutton o, ttbyte on_off);
    void (*SetTheme)(ttcheckbutton o, tttheme theme);
    void (*Draw)(ttcheckbutton o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttcheckbutton o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttcheckbutton o, ttshort x, ttshort y);
    void (*SetWH)(ttcheckbutton o, ttshort w, ttshort h);
    void (*SetXl)(ttcheckbutton o, ttint xl);
    void (*SetYl)(ttcheckbutton o, ttint yl);
    void (*SetWl)(ttcheckbutton o, ttint wl);
    void (*SetHl)(ttcheckbutton o, ttint hl);
    void (*SetTooltip)(ttcheckbutton o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttuint (*CacheThemeShape)(ttcheckbutton o);
    ttbyte (*DoSetAttr)(ttcheckbutton o, TT_ARG_DIE ttattr * text, ttshort width, ttshort height, ttshort pitch);

    /* extends ttfn_ttanybutton */
    void (*SetPressed)(ttcheckbutton o, ttbyte pressed);

    /* extends ttfn_ttbutton */
    void (*SetChecked)(ttcheckbutton o, ttbyte checked);

} s_ttfn_ttcheckbutton;
 
typedef struct s_ttfn_ttradiobutton {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttradiobutton (*New)(ttfn_ttradiobutton FN, ttradiobutton o);
    ttradiobutton (*Build)(ttradiobutton o);
    void (*Break)(ttradiobutton o);
    void (*Del)(TT_ARG_DIE ttradiobutton o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttradiobutton o, ttvisible parent);
    void (*Remove)(ttradiobutton o);
    void (*Invalidate)(ttradiobutton o);
    void (*SetVisible)(ttradiobutton o, ttbyte on_off);
    void (*SetTheme)(ttradiobutton o, tttheme theme);
    void (*Draw)(ttradiobutton o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttradiobutton o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttradiobutton o, ttshort x, ttshort y);
    void (*SetWH)(ttradiobutton o, ttshort w, ttshort h);
    void (*SetXl)(ttradiobutton o, ttint xl);
    void (*SetYl)(ttradiobutton o, ttint yl);
    void (*SetWl)(ttradiobutton o, ttint wl);
    void (*SetHl)(ttradiobutton o, ttint hl);
    void (*SetTooltip)(ttradiobutton o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttuint (*CacheThemeShape)(ttradiobutton o);
    ttbyte (*DoSetAttr)(ttradiobutton o, TT_ARG_DIE ttattr * text, ttshort width, ttshort height, ttshort pitch);

    /* extends ttfn_ttanybutton */
    void (*SetPressed)(ttradiobutton o, ttbyte pressed);

    /* extends ttfn_ttbutton */
    void (*SetChecked)(ttradiobutton o, ttbyte checked);

    /* extends ttfn_ttcheckbutton */

} s_ttfn_ttradiobutton;
 
typedef struct s_ttfn_ttanyscroll {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttanyscroll (*New)(ttfn_ttanyscroll FN, ttanyscroll o);
    ttanyscroll (*Build)(ttanyscroll o);
    void (*Break)(ttanyscroll o);
    void (*Del)(TT_ARG_DIE ttanyscroll o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttanyscroll o, ttvisible parent);
    void (*Remove)(ttanyscroll o);
    void (*Invalidate)(ttanyscroll o);
    void (*SetVisible)(ttanyscroll o, ttbyte on_off);
    void (*SetTheme)(ttanyscroll o, tttheme theme);
    void (*Draw)(ttanyscroll o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttanyscroll o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttanyscroll o, ttshort x, ttshort y);
    void (*SetWH)(ttanyscroll o, ttshort w, ttshort h);
    void (*SetXl)(ttanyscroll o, ttint xl);
    void (*SetYl)(ttanyscroll o, ttint yl);
    void (*SetWl)(ttanyscroll o, ttint wl);
    void (*SetHl)(ttanyscroll o, ttint hl);
    void (*SetTooltip)(ttanyscroll o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttuint (*CacheThemeShape)(ttanyscroll o);
    ttbyte (*DoSetAttr)(ttanyscroll o, TT_ARG_DIE ttattr * text, ttshort width, ttshort height, ttshort pitch);

    /* extends ttfn_ttanybutton */
    void (*Recalculate)(ttanyscroll o, ttint size, ttint real_size, ttint view_size, ttint position);
    void (*SetState)(ttanyscroll o, ttuint state);
    void (*SetOrientation)(ttanyscroll o, ttbyte orientation);
    void (*SetSize)(ttanyscroll o, ttint size);
    void (*SetRealSize)(ttanyscroll o, ttint real_size);
    void (*SetViewSize)(ttanyscroll o, ttint view_size);
    void (*SetPosition)(ttanyscroll o, ttint position);

} s_ttfn_ttanyscroll;
 
typedef struct s_ttfn_ttscrollbar {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttscrollbar (*New)(ttfn_ttscrollbar FN, ttscrollbar o);
    ttscrollbar (*Build)(ttscrollbar o);
    void (*Break)(ttscrollbar o);
    void (*Del)(TT_ARG_DIE ttscrollbar o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttscrollbar o, ttvisible parent);
    void (*Remove)(ttscrollbar o);
    void (*Invalidate)(ttscrollbar o);
    void (*SetVisible)(ttscrollbar o, ttbyte on_off);
    void (*SetTheme)(ttscrollbar o, tttheme theme);
    void (*Draw)(ttscrollbar o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttscrollbar o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttscrollbar o, ttshort x, ttshort y);
    void (*SetWH)(ttscrollbar o, ttshort w, ttshort h);
    void (*SetXl)(ttscrollbar o, ttint xl);
    void (*SetYl)(ttscrollbar o, ttint yl);
    void (*SetWl)(ttscrollbar o, ttint wl);
    void (*SetHl)(ttscrollbar o, ttint hl);
    void (*SetTooltip)(ttscrollbar o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttuint (*CacheThemeShape)(ttscrollbar o);
    ttbyte (*DoSetAttr)(ttscrollbar o, TT_ARG_DIE ttattr * text, ttshort width, ttshort height, ttshort pitch);

    /* extends ttfn_ttanybutton */
    void (*Recalculate)(ttscrollbar o, ttint size, ttint real_size, ttint view_size, ttint position);
    void (*SetState)(ttscrollbar o, ttuint state);
    void (*SetOrientation)(ttscrollbar o, ttbyte orientation);
    void (*SetSize)(ttscrollbar o, ttint size);
    void (*SetRealSize)(ttscrollbar o, ttint real_size);
    void (*SetViewSize)(ttscrollbar o, ttint view_size);
    void (*SetPosition)(ttscrollbar o, ttint position);

    /* extends ttfn_ttanyscroll */

} s_ttfn_ttscrollbar;
 
typedef struct s_ttfn_ttslider {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttslider (*New)(ttfn_ttslider FN, ttslider o);
    ttslider (*Build)(ttslider o);
    void (*Break)(ttslider o);
    void (*Del)(TT_ARG_DIE ttslider o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttslider o, ttvisible parent);
    void (*Remove)(ttslider o);
    void (*Invalidate)(ttslider o);
    void (*SetVisible)(ttslider o, ttbyte on_off);
    void (*SetTheme)(ttslider o, tttheme theme);
    void (*Draw)(ttslider o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttslider o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttslider o, ttshort x, ttshort y);
    void (*SetWH)(ttslider o, ttshort w, ttshort h);
    void (*SetXl)(ttslider o, ttint xl);
    void (*SetYl)(ttslider o, ttint yl);
    void (*SetWl)(ttslider o, ttint wl);
    void (*SetHl)(ttslider o, ttint hl);
    void (*SetTooltip)(ttslider o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttuint (*CacheThemeShape)(ttslider o);
    ttbyte (*DoSetAttr)(ttslider o, TT_ARG_DIE ttattr * text, ttshort width, ttshort height, ttshort pitch);

    /* extends ttfn_ttanybutton */
    void (*Recalculate)(ttslider o, ttint size, ttint real_size, ttint view_size, ttint position);
    void (*SetState)(ttslider o, ttuint state);
    void (*SetOrientation)(ttslider o, ttbyte orientation);
    void (*SetSize)(ttslider o, ttint size);
    void (*SetRealSize)(ttslider o, ttint real_size);
    void (*SetViewSize)(ttslider o, ttint view_size);
    void (*SetPosition)(ttslider o, ttint position);

    /* extends ttfn_ttanyscroll */
    void (*SetSlideMin)(ttslider o, ttint slide_min);
    void (*SetSlideMax)(ttslider o, ttint slide_max);
    void (*SetSlideValue)(ttslider o, ttint slide_value);

} s_ttfn_ttslider;
 
typedef struct s_ttfn_ttprogressbar {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttprogressbar (*New)(ttfn_ttprogressbar FN, ttprogressbar o);
    ttprogressbar (*Build)(ttprogressbar o);
    void (*Break)(ttprogressbar o);
    void (*Del)(TT_ARG_DIE ttprogressbar o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttprogressbar o, ttvisible parent);
    void (*Remove)(ttprogressbar o);
    void (*Invalidate)(ttprogressbar o);
    void (*SetVisible)(ttprogressbar o, ttbyte on_off);
    void (*SetTheme)(ttprogressbar o, tttheme theme);
    void (*Draw)(ttprogressbar o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttprogressbar o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttprogressbar o, ttshort x, ttshort y);
    void (*SetWH)(ttprogressbar o, ttshort w, ttshort h);
    void (*SetXl)(ttprogressbar o, ttint xl);
    void (*SetYl)(ttprogressbar o, ttint yl);
    void (*SetWl)(ttprogressbar o, ttint wl);
    void (*SetHl)(ttprogressbar o, ttint hl);
    void (*SetTooltip)(ttprogressbar o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttuint (*CacheThemeShape)(ttprogressbar o);
    ttbyte (*DoSetAttr)(ttprogressbar o, TT_ARG_DIE ttattr * text, ttshort width, ttshort height, ttshort pitch);

    /* extends ttfn_ttanybutton */
    void (*Recalculate)(ttprogressbar o, ttint size, ttint real_size, ttint view_size, ttint position);
    void (*SetState)(ttprogressbar o, ttuint state);
    void (*SetOrientation)(ttprogressbar o, ttbyte orientation);
    void (*SetSize)(ttprogressbar o, ttint size);
    void (*SetRealSize)(ttprogressbar o, ttint real_size);
    void (*SetViewSize)(ttprogressbar o, ttint view_size);
    void (*SetPosition)(ttprogressbar o, ttint position);

    /* extends ttfn_ttanyscroll */

} s_ttfn_ttprogressbar;
 
typedef struct s_ttfn_ttscrollpane {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttscrollpane (*New)(ttfn_ttscrollpane FN, ttscrollpane o);
    ttscrollpane (*Build)(ttscrollpane o);
    void (*Break)(ttscrollpane o);
    void (*Del)(TT_ARG_DIE ttscrollpane o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttscrollpane o, ttvisible parent);
    void (*Remove)(ttscrollpane o);
    void (*Invalidate)(ttscrollpane o);
    void (*SetVisible)(ttscrollpane o, ttbyte on_off);
    void (*SetTheme)(ttscrollpane o, tttheme theme);
    void (*Draw)(ttscrollpane o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttscrollpane o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttscrollpane o, ttshort x, ttshort y);
    void (*SetWH)(ttscrollpane o, ttshort w, ttshort h);
    void (*SetXl)(ttscrollpane o, ttint xl);
    void (*SetYl)(ttscrollpane o, ttint yl);
    void (*SetWl)(ttscrollpane o, ttint wl);
    void (*SetHl)(ttscrollpane o, ttint hl);
    void (*SetTooltip)(ttscrollpane o, tttooltip t);

    /* extends ttfn_ttwidget */
    void (*SetBarX)(ttscrollpane o, ttscrollbar bar_x);
    void (*SetBarY)(ttscrollpane o, ttscrollbar bar_y);

} s_ttfn_ttscrollpane;
 
typedef struct s_ttfn_ttwindow {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttwindow (*New)(ttfn_ttwindow FN, ttwindow o);
    ttwindow (*Build)(ttwindow o);
    void (*Break)(ttwindow o);
    void (*Del)(TT_ARG_DIE ttwindow o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttwindow o, ttvisible parent);
    void (*Remove)(ttwindow o);
    void (*Invalidate)(ttwindow o);
    void (*SetVisible)(ttwindow o, ttbyte on_off);
    void (*SetTheme)(ttwindow o, tttheme theme);
    void (*Draw)(ttwindow o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttwindow o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttwindow o, ttshort x, ttshort y);
    void (*SetWH)(ttwindow o, ttshort w, ttshort h);
    void (*SetXl)(ttwindow o, ttint xl);
    void (*SetYl)(ttwindow o, ttint yl);
    void (*SetWl)(ttwindow o, ttint wl);
    void (*SetHl)(ttwindow o, ttint hl);
    void (*SetTooltip)(ttwindow o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttbyte (*SetTitle)(ttwindow o, TT_ARG_READ ttbyte *title);

} s_ttfn_ttwindow;
 
typedef struct s_ttfn_ttframe {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttframe (*New)(ttfn_ttframe FN, ttframe o);
    ttframe (*Build)(ttframe o);
    void (*Break)(ttframe o);
    void (*Del)(TT_ARG_DIE ttframe o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttframe o, ttvisible parent);
    void (*Remove)(ttframe o);
    void (*Invalidate)(ttframe o);
    void (*SetVisible)(ttframe o, ttbyte on_off);
    void (*SetTheme)(ttframe o, tttheme theme);
    void (*Draw)(ttframe o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttframe o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttframe o, ttshort x, ttshort y);
    void (*SetWH)(ttframe o, ttshort w, ttshort h);
    void (*SetXl)(ttframe o, ttint xl);
    void (*SetYl)(ttframe o, ttint yl);
    void (*SetWl)(ttframe o, ttint wl);
    void (*SetHl)(ttframe o, ttint hl);
    void (*SetTooltip)(ttframe o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttbyte (*SetTitle)(ttframe o, TT_ARG_READ ttbyte *title);

    /* extends ttfn_ttwindow */
    void (*SetMenubar)(ttframe o, ttmenubar m);

} s_ttfn_ttframe;
 
typedef struct s_ttfn_ttscroller {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttscroller (*New)(ttfn_ttscroller FN, ttscroller o);
    ttscroller (*Build)(ttscroller o);
    void (*Break)(ttscroller o);
    void (*Del)(TT_ARG_DIE ttscroller o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttscroller o, ttvisible parent);
    void (*Remove)(ttscroller o);
    void (*Invalidate)(ttscroller o);
    void (*SetVisible)(ttscroller o, ttbyte on_off);
    void (*SetTheme)(ttscroller o, tttheme theme);
    void (*Draw)(ttscroller o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttscroller o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttscroller o, ttshort x, ttshort y);
    void (*SetWH)(ttscroller o, ttshort w, ttshort h);
    void (*SetXl)(ttscroller o, ttint xl);
    void (*SetYl)(ttscroller o, ttint yl);
    void (*SetWl)(ttscroller o, ttint wl);
    void (*SetHl)(ttscroller o, ttint hl);
    void (*SetTooltip)(ttscroller o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttbyte (*SetTitle)(ttscroller o, TT_ARG_READ ttbyte *title);

    /* extends ttfn_ttwindow */

} s_ttfn_ttscroller;
 
typedef struct s_ttfn_ttmenuitem {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttmenuitem (*New)(ttfn_ttmenuitem FN, ttmenuitem o);
    ttmenuitem (*Build)(ttmenuitem o);
    void (*Break)(ttmenuitem o);
    void (*Del)(TT_ARG_DIE ttmenuitem o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttmenuitem o, ttvisible parent);
    void (*Remove)(ttmenuitem o);
    void (*Invalidate)(ttmenuitem o);
    void (*SetVisible)(ttmenuitem o, ttbyte on_off);
    void (*SetTheme)(ttmenuitem o, tttheme theme);
    void (*Draw)(ttmenuitem o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttmenuitem o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */

} s_ttfn_ttmenuitem;
 
typedef struct s_ttfn_ttcheckmenuitem {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttcheckmenuitem (*New)(ttfn_ttcheckmenuitem FN, ttcheckmenuitem o);
    ttcheckmenuitem (*Build)(ttcheckmenuitem o);
    void (*Break)(ttcheckmenuitem o);
    void (*Del)(TT_ARG_DIE ttcheckmenuitem o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttcheckmenuitem o, ttvisible parent);
    void (*Remove)(ttcheckmenuitem o);
    void (*Invalidate)(ttcheckmenuitem o);
    void (*SetVisible)(ttcheckmenuitem o, ttbyte on_off);
    void (*SetTheme)(ttcheckmenuitem o, tttheme theme);
    void (*Draw)(ttcheckmenuitem o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttcheckmenuitem o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */

    /* extends ttfn_ttmenuitem */

} s_ttfn_ttcheckmenuitem;
 
typedef struct s_ttfn_ttradiomenuitem {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttradiomenuitem (*New)(ttfn_ttradiomenuitem FN, ttradiomenuitem o);
    ttradiomenuitem (*Build)(ttradiomenuitem o);
    void (*Break)(ttradiomenuitem o);
    void (*Del)(TT_ARG_DIE ttradiomenuitem o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttradiomenuitem o, ttvisible parent);
    void (*Remove)(ttradiomenuitem o);
    void (*Invalidate)(ttradiomenuitem o);
    void (*SetVisible)(ttradiomenuitem o, ttbyte on_off);
    void (*SetTheme)(ttradiomenuitem o, tttheme theme);
    void (*Draw)(ttradiomenuitem o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttradiomenuitem o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */

    /* extends ttfn_ttmenuitem */

    /* extends ttfn_ttcheckmenuitem */

} s_ttfn_ttradiomenuitem;
 
typedef struct s_ttfn_ttmenu {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttmenu (*New)(ttfn_ttmenu FN, ttmenu o);
    ttmenu (*Build)(ttmenu o);
    void (*Break)(ttmenu o);
    void (*Del)(TT_ARG_DIE ttmenu o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttmenu o, ttvisible parent);
    void (*Remove)(ttmenu o);
    void (*Invalidate)(ttmenu o);
    void (*SetVisible)(ttmenu o, ttbyte on_off);
    void (*SetTheme)(ttmenu o, tttheme theme);
    void (*Draw)(ttmenu o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttmenu o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */

    /* extends ttfn_ttmenuitem */

} s_ttfn_ttmenu;
 
typedef struct s_ttfn_ttmenubar {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttmenubar (*New)(ttfn_ttmenubar FN, ttmenubar o);
    ttmenubar (*Build)(ttmenubar o);
    void (*Break)(ttmenubar o);
    void (*Del)(TT_ARG_DIE ttmenubar o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttmenubar o, ttvisible parent);
    void (*Remove)(ttmenubar o);
    void (*Invalidate)(ttmenubar o);
    void (*SetVisible)(ttmenubar o, ttbyte on_off);
    void (*SetTheme)(ttmenubar o, tttheme theme);
    void (*Draw)(ttmenubar o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttmenubar o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */

} s_ttfn_ttmenubar;
 
typedef struct s_ttfn_ttanytext {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttanytext (*New)(ttfn_ttanytext FN, ttanytext o);
    ttanytext (*Build)(ttanytext o);
    void (*Break)(ttanytext o);
    void (*Del)(TT_ARG_DIE ttanytext o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(ttanytext o, ttvisible parent);
    void (*Remove)(ttanytext o);
    void (*Invalidate)(ttanytext o);
    void (*SetVisible)(ttanytext o, ttbyte on_off);
    void (*SetTheme)(ttanytext o, tttheme theme);
    void (*Draw)(ttanytext o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(ttanytext o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(ttanytext o, ttshort x, ttshort y);
    void (*SetWH)(ttanytext o, ttshort w, ttshort h);
    void (*SetXl)(ttanytext o, ttint xl);
    void (*SetYl)(ttanytext o, ttint yl);
    void (*SetWl)(ttanytext o, ttint wl);
    void (*SetHl)(ttanytext o, ttint hl);
    void (*SetTooltip)(ttanytext o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttbyte (*SetTitle)(ttanytext o, TT_ARG_READ ttbyte *title);

    /* extends ttfn_ttwindow */

} s_ttfn_ttanytext;
 
typedef struct s_ttfn_tttextfield {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tttextfield (*New)(ttfn_tttextfield FN, tttextfield o);
    tttextfield (*Build)(tttextfield o);
    void (*Break)(tttextfield o);
    void (*Del)(TT_ARG_DIE tttextfield o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(tttextfield o, ttvisible parent);
    void (*Remove)(tttextfield o);
    void (*Invalidate)(tttextfield o);
    void (*SetVisible)(tttextfield o, ttbyte on_off);
    void (*SetTheme)(tttextfield o, tttheme theme);
    void (*Draw)(tttextfield o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(tttextfield o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(tttextfield o, ttshort x, ttshort y);
    void (*SetWH)(tttextfield o, ttshort w, ttshort h);
    void (*SetXl)(tttextfield o, ttint xl);
    void (*SetYl)(tttextfield o, ttint yl);
    void (*SetWl)(tttextfield o, ttint wl);
    void (*SetHl)(tttextfield o, ttint hl);
    void (*SetTooltip)(tttextfield o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttbyte (*SetTitle)(tttextfield o, TT_ARG_READ ttbyte *title);

    /* extends ttfn_ttwindow */

    /* extends ttfn_ttanytext */

} s_ttfn_tttextfield;
 
typedef struct s_ttfn_tttextarea {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tttextarea (*New)(ttfn_tttextarea FN, tttextarea o);
    tttextarea (*Build)(tttextarea o);
    void (*Break)(tttextarea o);
    void (*Del)(TT_ARG_DIE tttextarea o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    void (*AddTo)(tttextarea o, ttvisible parent);
    void (*Remove)(tttextarea o);
    void (*Invalidate)(tttextarea o);
    void (*SetVisible)(tttextarea o, ttbyte on_off);
    void (*SetTheme)(tttextarea o, tttheme theme);
    void (*Draw)(tttextarea o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ ttbyte *asciidata, TT_ARG_READ ttfont *fontdata, TT_ARG_READ ttattr *attrdata);
    void (*BuiltinRepaint)(tttextarea o, ttshort x, ttshort y, ttshort w, ttshort h);

    /* extends ttfn_ttvisible */
    void (*SetXY)(tttextarea o, ttshort x, ttshort y);
    void (*SetWH)(tttextarea o, ttshort w, ttshort h);
    void (*SetXl)(tttextarea o, ttint xl);
    void (*SetYl)(tttextarea o, ttint yl);
    void (*SetWl)(tttextarea o, ttint wl);
    void (*SetHl)(tttextarea o, ttint hl);
    void (*SetTooltip)(tttextarea o, tttooltip t);

    /* extends ttfn_ttwidget */
    ttbyte (*SetTitle)(tttextarea o, TT_ARG_READ ttbyte *title);

    /* extends ttfn_ttwindow */

    /* extends ttfn_ttanytext */

    /* extends ttfn_tttextfield */

} s_ttfn_tttextarea;
 
typedef struct s_ttfn_tttheme {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tttheme (*New)(ttfn_tttheme FN, tttheme o);
    tttheme (*Build)(tttheme o);
    void (*Break)(tttheme o);
    void (*Del)(TT_ARG_DIE tttheme o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */

} s_ttfn_tttheme;
 
typedef struct s_ttfn_ttapplication {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttapplication (*New)(ttfn_ttapplication FN, ttapplication o);
    ttapplication (*Build)(ttapplication o);
    void (*Break)(ttapplication o);
    void (*Del)(TT_ARG_DIE ttapplication o);

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */

} s_ttfn_ttapplication;







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


typedef struct s_ttfns *ttfns;
typedef struct s_ttfns {

    s_ttfn_ttobj fn_ttobj;  
    s_ttfn_ttevent fn_ttevent; 
    s_ttfn_tteventbig fn_tteventbig; 
    s_ttfn_ttbitmask fn_ttbitmask; 
    s_ttfn_tteventmask fn_tteventmask; 
    s_ttfn_ttcallback fn_ttcallback; 
    s_ttfn_ttlistener fn_ttlistener; 
    s_ttfn_tttimer fn_tttimer; 
    s_ttfn_ttcomponent fn_ttcomponent; 
    s_ttfn_ttdata fn_ttdata; 
    s_ttfn_ttvisible fn_ttvisible; 
    s_ttfn_ttnative fn_ttnative; 
    s_ttfn_ttwidget fn_ttwidget; 
    s_ttfn_ttlabel fn_ttlabel; 
    s_ttfn_tttooltip fn_tttooltip; 
    s_ttfn_ttbuttongroup fn_ttbuttongroup; 
    s_ttfn_ttanybutton fn_ttanybutton; 
    s_ttfn_ttbutton fn_ttbutton; 
    s_ttfn_ttcheckbutton fn_ttcheckbutton; 
    s_ttfn_ttradiobutton fn_ttradiobutton; 
    s_ttfn_ttanyscroll fn_ttanyscroll; 
    s_ttfn_ttscrollbar fn_ttscrollbar; 
    s_ttfn_ttslider fn_ttslider; 
    s_ttfn_ttprogressbar fn_ttprogressbar; 
    s_ttfn_ttscrollpane fn_ttscrollpane; 
    s_ttfn_ttwindow fn_ttwindow; 
    s_ttfn_ttframe fn_ttframe; 
    s_ttfn_ttscroller fn_ttscroller; 
    s_ttfn_ttmenuitem fn_ttmenuitem; 
    s_ttfn_ttcheckmenuitem fn_ttcheckmenuitem; 
    s_ttfn_ttradiomenuitem fn_ttradiomenuitem; 
    s_ttfn_ttmenu fn_ttmenu; 
    s_ttfn_ttmenubar fn_ttmenubar; 
    s_ttfn_ttanytext fn_ttanytext; 
    s_ttfn_tttextfield fn_tttextfield; 
    s_ttfn_tttextarea fn_tttextarea; 
    s_ttfn_tttheme fn_tttheme; 
    s_ttfn_ttapplication fn_ttapplication;
    s_tthw HW;
} s_ttfns;




/* client-visible global types */

extern tt_fn TTFN_ttbyte; 
extern tt_fn TTFN_ttshort; 
extern tt_fn TTFN_ttint; 
extern tt_fn TTFN_ttcol; 
extern tt_fn TTFN_ttfont; 
extern tt_fn TTFN_ttattr; 
extern tt_fn TTFN_ttopaque; 
extern tt_fn TTFN_ttany; 
extern tt_fn TTFN_ttpointer; 
extern tt_fn TTFN_ttstring; 
extern tt_fn TTFN_ttfunction;
#define TTFN_ttsbyte	TTFN_ttbyte
#define TTFN_ttushort	TTFN_ttshort
#define TTFN_ttuint	TTFN_ttint

/* client-visible global methods structures */

extern tt_fn TTFN_ttobj;  
extern tt_fn TTFN_ttevent; 
extern tt_fn TTFN_tteventbig; 
extern tt_fn TTFN_ttbitmask; 
extern tt_fn TTFN_tteventmask; 
extern tt_fn TTFN_ttcallback; 
extern tt_fn TTFN_ttlistener; 
extern tt_fn TTFN_tttimer; 
extern tt_fn TTFN_ttcomponent; 
extern tt_fn TTFN_ttdata; 
extern tt_fn TTFN_ttvisible; 
extern tt_fn TTFN_ttnative; 
extern tt_fn TTFN_ttwidget; 
extern tt_fn TTFN_ttlabel; 
extern tt_fn TTFN_tttooltip; 
extern tt_fn TTFN_ttbuttongroup; 
extern tt_fn TTFN_ttanybutton; 
extern tt_fn TTFN_ttbutton; 
extern tt_fn TTFN_ttcheckbutton; 
extern tt_fn TTFN_ttradiobutton; 
extern tt_fn TTFN_ttanyscroll; 
extern tt_fn TTFN_ttscrollbar; 
extern tt_fn TTFN_ttslider; 
extern tt_fn TTFN_ttprogressbar; 
extern tt_fn TTFN_ttscrollpane; 
extern tt_fn TTFN_ttwindow; 
extern tt_fn TTFN_ttframe; 
extern tt_fn TTFN_ttscroller; 
extern tt_fn TTFN_ttmenuitem; 
extern tt_fn TTFN_ttcheckmenuitem; 
extern tt_fn TTFN_ttradiomenuitem; 
extern tt_fn TTFN_ttmenu; 
extern tt_fn TTFN_ttmenubar; 
extern tt_fn TTFN_ttanytext; 
extern tt_fn TTFN_tttextfield; 
extern tt_fn TTFN_tttextarea; 
extern tt_fn TTFN_tttheme; 
extern tt_fn TTFN_ttapplication;



/* #defines for global methods structures */
#define TFNs	(TTD.FNs)

#define THW	(TTD.FN.HW)



#define TFN_ttobj	(&TTD.FN.fn_ttobj)  
#define TFN_ttevent	(&TTD.FN.fn_ttevent) 
#define TFN_tteventbig	(&TTD.FN.fn_tteventbig) 
#define TFN_ttbitmask	(&TTD.FN.fn_ttbitmask) 
#define TFN_tteventmask	(&TTD.FN.fn_tteventmask) 
#define TFN_ttcallback	(&TTD.FN.fn_ttcallback) 
#define TFN_ttlistener	(&TTD.FN.fn_ttlistener) 
#define TFN_tttimer	(&TTD.FN.fn_tttimer) 
#define TFN_ttcomponent	(&TTD.FN.fn_ttcomponent) 
#define TFN_ttdata	(&TTD.FN.fn_ttdata) 
#define TFN_ttvisible	(&TTD.FN.fn_ttvisible) 
#define TFN_ttnative	(&TTD.FN.fn_ttnative) 
#define TFN_ttwidget	(&TTD.FN.fn_ttwidget) 
#define TFN_ttlabel	(&TTD.FN.fn_ttlabel) 
#define TFN_tttooltip	(&TTD.FN.fn_tttooltip) 
#define TFN_ttbuttongroup	(&TTD.FN.fn_ttbuttongroup) 
#define TFN_ttanybutton	(&TTD.FN.fn_ttanybutton) 
#define TFN_ttbutton	(&TTD.FN.fn_ttbutton) 
#define TFN_ttcheckbutton	(&TTD.FN.fn_ttcheckbutton) 
#define TFN_ttradiobutton	(&TTD.FN.fn_ttradiobutton) 
#define TFN_ttanyscroll	(&TTD.FN.fn_ttanyscroll) 
#define TFN_ttscrollbar	(&TTD.FN.fn_ttscrollbar) 
#define TFN_ttslider	(&TTD.FN.fn_ttslider) 
#define TFN_ttprogressbar	(&TTD.FN.fn_ttprogressbar) 
#define TFN_ttscrollpane	(&TTD.FN.fn_ttscrollpane) 
#define TFN_ttwindow	(&TTD.FN.fn_ttwindow) 
#define TFN_ttframe	(&TTD.FN.fn_ttframe) 
#define TFN_ttscroller	(&TTD.FN.fn_ttscroller) 
#define TFN_ttmenuitem	(&TTD.FN.fn_ttmenuitem) 
#define TFN_ttcheckmenuitem	(&TTD.FN.fn_ttcheckmenuitem) 
#define TFN_ttradiomenuitem	(&TTD.FN.fn_ttradiomenuitem) 
#define TFN_ttmenu	(&TTD.FN.fn_ttmenu) 
#define TFN_ttmenubar	(&TTD.FN.fn_ttmenubar) 
#define TFN_ttanytext	(&TTD.FN.fn_ttanytext) 
#define TFN_tttextfield	(&TTD.FN.fn_tttextfield) 
#define TFN_tttextarea	(&TTD.FN.fn_tttextarea) 
#define TFN_tttheme	(&TTD.FN.fn_tttheme) 
#define TFN_ttapplication	(&TTD.FN.fn_ttapplication)


#define TFN(type) TT_CAT(TFN_,type)




#define TFNdefault_ttobj	(&TTD.FNdefault.fn_ttobj)  
#define TFNdefault_ttevent	(&TTD.FNdefault.fn_ttevent) 
#define TFNdefault_tteventbig	(&TTD.FNdefault.fn_tteventbig) 
#define TFNdefault_ttbitmask	(&TTD.FNdefault.fn_ttbitmask) 
#define TFNdefault_tteventmask	(&TTD.FNdefault.fn_tteventmask) 
#define TFNdefault_ttcallback	(&TTD.FNdefault.fn_ttcallback) 
#define TFNdefault_ttlistener	(&TTD.FNdefault.fn_ttlistener) 
#define TFNdefault_tttimer	(&TTD.FNdefault.fn_tttimer) 
#define TFNdefault_ttcomponent	(&TTD.FNdefault.fn_ttcomponent) 
#define TFNdefault_ttdata	(&TTD.FNdefault.fn_ttdata) 
#define TFNdefault_ttvisible	(&TTD.FNdefault.fn_ttvisible) 
#define TFNdefault_ttnative	(&TTD.FNdefault.fn_ttnative) 
#define TFNdefault_ttwidget	(&TTD.FNdefault.fn_ttwidget) 
#define TFNdefault_ttlabel	(&TTD.FNdefault.fn_ttlabel) 
#define TFNdefault_tttooltip	(&TTD.FNdefault.fn_tttooltip) 
#define TFNdefault_ttbuttongroup	(&TTD.FNdefault.fn_ttbuttongroup) 
#define TFNdefault_ttanybutton	(&TTD.FNdefault.fn_ttanybutton) 
#define TFNdefault_ttbutton	(&TTD.FNdefault.fn_ttbutton) 
#define TFNdefault_ttcheckbutton	(&TTD.FNdefault.fn_ttcheckbutton) 
#define TFNdefault_ttradiobutton	(&TTD.FNdefault.fn_ttradiobutton) 
#define TFNdefault_ttanyscroll	(&TTD.FNdefault.fn_ttanyscroll) 
#define TFNdefault_ttscrollbar	(&TTD.FNdefault.fn_ttscrollbar) 
#define TFNdefault_ttslider	(&TTD.FNdefault.fn_ttslider) 
#define TFNdefault_ttprogressbar	(&TTD.FNdefault.fn_ttprogressbar) 
#define TFNdefault_ttscrollpane	(&TTD.FNdefault.fn_ttscrollpane) 
#define TFNdefault_ttwindow	(&TTD.FNdefault.fn_ttwindow) 
#define TFNdefault_ttframe	(&TTD.FNdefault.fn_ttframe) 
#define TFNdefault_ttscroller	(&TTD.FNdefault.fn_ttscroller) 
#define TFNdefault_ttmenuitem	(&TTD.FNdefault.fn_ttmenuitem) 
#define TFNdefault_ttcheckmenuitem	(&TTD.FNdefault.fn_ttcheckmenuitem) 
#define TFNdefault_ttradiomenuitem	(&TTD.FNdefault.fn_ttradiomenuitem) 
#define TFNdefault_ttmenu	(&TTD.FNdefault.fn_ttmenu) 
#define TFNdefault_ttmenubar	(&TTD.FNdefault.fn_ttmenubar) 
#define TFNdefault_ttanytext	(&TTD.FNdefault.fn_ttanytext) 
#define TFNdefault_tttextfield	(&TTD.FNdefault.fn_tttextfield) 
#define TFNdefault_tttextarea	(&TTD.FNdefault.fn_tttextarea) 
#define TFNdefault_tttheme	(&TTD.FNdefault.fn_tttheme) 
#define TFNdefault_ttapplication	(&TTD.FNdefault.fn_ttapplication)

#define TFNdefault(type) TT_CAT(TFNdefault_,type)














/* prototypes for public, public_set and exported methods */



/* ttobj methods */
                                
void TTRef_ttobj(tt_obj a1);

void TTUnref_ttobj(tt_obj a1);

  
/* ttevent methods */

tt_obj TTCreate_ttevent(ttuint a1, ttuint a2, ttuint a3);

void TTFire_ttevent(tt_obj a1, tt_obj a2);

 
/* tteventbig methods */


tt_obj TTCreate8_tteventbig(ttuint a1, ttuint a2, ttuint a3, ttshort a4, ttshort a5, ttshort a6, ttshort a7, ttany a8, ttany a9, ttuint a10, TT_CONST ttbyte * a11);

tt_obj TTCreate4_tteventbig(ttuint a1, ttuint a2, ttuint a3, ttshort a4, ttshort a5, ttshort a6, ttshort a7);

tt_obj TTCreate4s4_tteventbig(ttuint a1, ttuint a2, ttuint a3, ttany a4, ttany a5, ttuint a6, TT_CONST ttbyte * a7);

 
/* ttbitmask methods */

tt_obj TTClone_ttbitmask(tt_obj a1);

tt_obj TTCreate_ttbitmask(ttany a1);

tt_obj TTCreateA_ttbitmask(ttopaque a1, TT_ARG_READ ttany * a2);

void TTClear_ttbitmask(tt_obj a1);

void TTFill_ttbitmask(tt_obj a1);

void TTNegate_ttbitmask(tt_obj a1);

void TTChange_ttbitmask(tt_obj a1, ttuint a2);

ttbyte TTIsMember_ttbitmask(tt_obj a1, ttany a2);

ttbyte TTAdd_ttbitmask(tt_obj a1, ttany a2);

ttbyte TTAddA_ttbitmask(tt_obj a1, ttopaque a2, TT_ARG_READ ttany * a3);

ttbyte TTRemove_ttbitmask(tt_obj a1, ttany a2);

ttbyte TTRemoveA_ttbitmask(tt_obj a1, ttopaque a2, TT_ARG_READ ttany * a3);

ttbyte TTCombine_ttbitmask(tt_obj a1, ttany a2, ttuint a3);

ttbyte TTCombineA_ttbitmask(tt_obj a1, ttopaque a2, TT_ARG_READ ttany * a3, ttuint a4);

ttbyte TTCombineB_ttbitmask(tt_obj a1, tt_obj a2, ttuint a3);

 
/* tteventmask methods */

tt_obj TTCreate_tteventmask(tt_obj a1, tt_obj a2, tt_obj a3);

void TTSetEvtypeMask_tteventmask(tt_obj a1, tt_obj a2);

void TTSetEvcodeMask_tteventmask(tt_obj a1, tt_obj a2);

void TTSetComponentMask_tteventmask(tt_obj a1, tt_obj a2);

void TTSetTruthTable_tteventmask(tt_obj a1, ttuint a2);

 
/* ttcallback methods */
    
 
/* ttlistener methods */

tt_obj TTCreate_ttlistener(tt_obj a1, ttuint a2, ttuint a3, ttlistener_fn a4, ttany a5);

tt_obj TTCreateA_ttlistener(tt_obj a1, tt_obj a2, ttuint a3, ttuint a4, ttuint a5, ttuint a6, ttlistener_fn a7, TT_ARG_READ ttany * a8);

tt_obj TTCreateV_ttlistener(tt_obj a1, tt_obj a2, ttuint a3, ttuint a4, ttuint a5, ttuint a6, ttlistener_fn a7, va_list * a8);


tt_obj TTCreateDel_ttlistener(tt_obj a1, ttuint a2, ttlistener_fn a3);

tt_obj TTCreateAskclose_ttlistener(tt_obj a1, ttuint a2, ttlistener_fn a3);

tt_obj TTCreateKey_ttlistener(tt_obj a1, ttuint a2, ttuint a3, ttuint a4, ttlistener_fn a5);

tt_obj TTCreateMouse_ttlistener(tt_obj a1, ttuint a2, ttuint a3, ttuint a4, ttlistener_fn a5);

tt_obj TTCreateActivate_ttlistener(tt_obj a1, ttuint a2, ttlistener_fn a3);

tt_obj TTCreateChange_ttlistener(tt_obj a1, ttuint a2, ttuint a3, ttlistener_fn a4);

tt_obj TTCreateKeyData_ttlistener(tt_obj a1, TT_ARG_READ ttbyte * a2, ttuint a3, ttlistener_fn a4);

tt_obj TTCreateExpose_ttlistener(tt_obj a1, ttuint a2, ttlistener_fn a3);

    
void TTAddTo_ttlistener(tt_obj a1, tt_obj a2);
    
void TTRemove_ttlistener(tt_obj a1);

ttbyte TTSetArgs_ttlistener(tt_obj a1, ttuint a2, ttuint a3, TT_ARG_READ ttany * a4);

void TTSetEventMask_ttlistener(tt_obj a1, tt_obj a2);

 
/* tttimer methods */

tt_obj TTCreateA_tttimer(tt_obj a1, ttuint a2, ttuint a3, ttuint a4, ttuint a5, ttlistener_fn a6, TT_ARG_READ ttany * a7, ttuint a8, ttuint a9);

tt_obj TTCreate_tttimer(tt_obj a1, ttuint a2, ttlistener_fn a3, ttany a4, ttuint a5, ttuint a6);

void TTSetDelay_tttimer(tt_obj a1, ttuint a2, ttuint a3);
    
void TTSetEnabled_tttimer(tt_obj a1, ttbyte a2);
    
ttbyte TTIsEnabled_tttimer(tt_obj a1);

 
/* ttcomponent methods */

void TTAddListener_ttcomponent(tt_obj a1, tt_obj a2);

void TTRemoveListener_ttcomponent(tt_obj a1, tt_obj a2);

tt_obj TTSetKeyData_ttcomponent(tt_obj a1, TT_ARG_READ ttbyte * a2, ttany a3);

ttany TTGetKeyData_ttcomponent(tt_obj a1, TT_ARG_READ ttbyte * a2);

void TTUnsetKeyData_ttcomponent(tt_obj a1, TT_ARG_READ ttbyte * a2);

tt_obj TTGetData_ttcomponent(tt_obj a1, TT_ARG_READ ttbyte * a2);

 
/* ttdata methods */
        
 
/* ttvisible methods */
    
void TTAddTo_ttvisible(tt_obj a1, tt_obj a2);
    
void TTRemove_ttvisible(tt_obj a1);
    
void TTInvalidate_ttvisible(tt_obj a1);
    
void TTSetVisible_ttvisible(tt_obj a1, ttbyte a2);
    
void TTSetTheme_ttvisible(tt_obj a1, tt_obj a2);
    
void TTDraw_ttvisible(tt_obj a1, ttshort a2, ttshort a3, ttshort a4, ttshort a5, ttshort a6, TT_ARG_READ ttbyte * a7, TT_ARG_READ ttfont * a8, TT_ARG_READ ttattr * a9);
    
void TTBuiltinRepaint_ttvisible(tt_obj a1, ttshort a2, ttshort a3, ttshort a4, ttshort a5);

void TTSetRepaint_ttvisible(tt_obj a1, ttvisible_repaint_fn a2);

void TTAdd_ttvisible(tt_obj a1, tt_obj a2);

void TTExpose_ttvisible(tt_obj a1, ttshort a2, ttshort a3, ttshort a4, ttshort a5);

 
/* ttnative methods */
    
tt_obj TTGetRoot_ttnative(void);
    
ttshort TTGetW_ttnative(tt_obj a1);
    
ttshort TTGetH_ttnative(tt_obj a1);

tt_obj TTCreate_ttnative(ttany a1);

 
/* ttwidget methods */
    
void TTSetXY_ttwidget(tt_obj a1, ttshort a2, ttshort a3);
    
void TTSetWH_ttwidget(tt_obj a1, ttshort a2, ttshort a3);
    
void TTSetXl_ttwidget(tt_obj a1, ttint a2);
    
void TTSetYl_ttwidget(tt_obj a1, ttint a2);
    
void TTSetWl_ttwidget(tt_obj a1, ttint a2);
    
void TTSetHl_ttwidget(tt_obj a1, ttint a2);
    
void TTSetTooltip_ttwidget(tt_obj a1, tt_obj a2);

void TTSetXlYl_ttwidget(tt_obj a1, ttbyte a2, ttint a3, ttint a4);

void TTSetWlHl_ttwidget(tt_obj a1, ttbyte a2, ttint a3, ttint a4);

void TTSetXYWH_ttwidget(tt_obj a1, ttbyte a2, ttshort a3, ttshort a4, ttshort a5, ttshort a6);

void TTSetX_ttwidget(tt_obj a1, ttshort a2);

void TTSetY_ttwidget(tt_obj a1, ttshort a2);

void TTSetW_ttwidget(tt_obj a1, ttshort a2);

void TTSetH_ttwidget(tt_obj a1, ttshort a2);

 
/* ttlabel methods */
    
ttbyte TTDoSetFont_ttlabel(tt_obj a1, TT_ARG_DIE ttfont * a2, ttopaque a3);

ttbyte TTSetFont_ttlabel(tt_obj a1, TT_ARG_READ ttfont * a2);

ttbyte TTSetText_ttlabel(tt_obj a1, TT_ARG_READ ttbyte * a2);

tt_obj TTCreate_ttlabel(TT_ARG_READ ttbyte * a1);

 
/* tttooltip methods */
    
void TTSetWidget_tttooltip(tt_obj a1, tt_obj a2);

 
/* ttbuttongroup methods */
    
void TTAdd_ttbuttongroup(tt_obj a1, tt_obj a2);
    
void TTRemove_ttbuttongroup(tt_obj a1, tt_obj a2);
    
void TTSetChecked_ttbuttongroup(tt_obj a1, tt_obj a2);

 
/* ttanybutton methods */
        
ttbyte TTDoSetAttr_ttanybutton(tt_obj a1, TT_ARG_DIE ttattr * a2, ttshort a3, ttshort a4, ttshort a5);

ttbyte TTSetAttr_ttanybutton(tt_obj a1, TT_ARG_READ ttattr * a2, ttshort a3, ttshort a4, ttshort a5);

ttbyte TTSetFont_ttanybutton(tt_obj a1, TT_ARG_READ ttfont * a2, ttshort a3, ttshort a4, ttshort a5);

ttbyte TTSetText_ttanybutton(tt_obj a1, TT_ARG_READ ttbyte * a2, ttshort a3, ttshort a4, ttshort a5);

tt_obj TTCreate_ttanybutton(TT_ARG_READ ttbyte * a1, ttshort a2, ttshort a3, ttshort a4);

 
/* ttbutton methods */
    
void TTSetPressed_ttbutton(tt_obj a1, ttbyte a2);

ttbyte TTIsPressed_ttbutton(tt_obj a1);

tt_obj TTCreate_ttbutton(TT_ARG_READ ttbyte * a1, ttshort a2, ttshort a3, ttshort a4);

 
/* ttcheckbutton methods */
    
void TTSetChecked_ttcheckbutton(tt_obj a1, ttbyte a2);

ttbyte TTIsChecked_ttcheckbutton(tt_obj a1);

tt_obj TTCreate_ttcheckbutton(TT_ARG_READ ttbyte * a1, ttshort a2, ttshort a3, ttshort a4);

 
/* ttradiobutton methods */

void TTAddToGroup_ttradiobutton(tt_obj a1, tt_obj a2);

void TTRemoveFromGroup_ttradiobutton(tt_obj a1);

tt_obj TTCreate_ttradiobutton(TT_ARG_READ ttbyte * a1, ttshort a2, ttshort a3, ttshort a4);

 
/* ttanyscroll methods */
            
void TTSetOrientation_ttanyscroll(tt_obj a1, ttbyte a2);
    
void TTSetSize_ttanyscroll(tt_obj a1, ttint a2);
    
void TTSetRealSize_ttanyscroll(tt_obj a1, ttint a2);
    
void TTSetViewSize_ttanyscroll(tt_obj a1, ttint a2);
    
void TTSetPosition_ttanyscroll(tt_obj a1, ttint a2);

 
/* ttscrollbar methods */

tt_obj TTCreate1_ttscrollbar(ttbyte a1);

tt_obj TTCreate4_ttscrollbar(ttbyte a1, ttint a2, ttint a3, ttint a4);

 
/* ttslider methods */
    
void TTSetSlideMin_ttslider(tt_obj a1, ttint a2);
    
void TTSetSlideMax_ttslider(tt_obj a1, ttint a2);
    
void TTSetSlideValue_ttslider(tt_obj a1, ttint a2);

tt_obj TTCreate_ttslider(ttbyte a1, ttint a2);

 
/* ttprogressbar methods */

tt_obj TTCreate_ttprogressbar(ttbyte a1);

 
/* ttscrollpane methods */

tt_obj TTCreate_ttscrollpane(ttshort a1, ttshort a2);
    
void TTSetBarX_ttscrollpane(tt_obj a1, tt_obj a2);
    
void TTSetBarY_ttscrollpane(tt_obj a1, tt_obj a2);

 
/* ttwindow methods */
    
ttbyte TTSetTitle_ttwindow(tt_obj a1, TT_ARG_READ ttbyte * a2);

 
/* ttframe methods */
    
void TTSetMenubar_ttframe(tt_obj a1, tt_obj a2);

tt_obj TTCreate_ttframe(TT_ARG_READ ttbyte * a1);

 
/* ttscroller methods */

tt_obj TTCreate_ttscroller(ttshort a1, ttshort a2);

 
/* ttmenuitem methods */

 
/* ttcheckmenuitem methods */

 
/* ttradiomenuitem methods */

 
/* ttmenu methods */

 
/* ttmenubar methods */

tt_obj TTGetDefault_ttmenubar(void);

 
/* ttanytext methods */

 
/* tttextfield methods */

 
/* tttextarea methods */

 
/* tttheme methods */

tt_obj TTGetDefault_tttheme(void);

 
/* ttapplication methods */

tt_obj TTSet_ttapplication(TT_ARG_READ ttbyte * a1);

tt_obj TTGet_ttapplication(void);








/* prototypes for handy public, public_set and exported methods */


















/* prototypes for handy ttobj methods */
            
ttopaque TTGetId_ttobj(tt_obj o);
    
ttuint TTGetRefcount_ttobj(tt_obj o);
    
ttuint TTGetOflags_ttobj(tt_obj o);
            
ttany TTGetUserData_ttobj(tt_obj o);

tt_fn TTSetUserData_ttobj(tt_obj o, ttany user_data);
    
ttopaque TTGetEventsInprogress_ttobj(tt_obj o);
    
ttopaque TTGetEventsInformed_ttobj(tt_obj o);
  
/* prototypes for handy ttevent methods */
        
tt_obj TTGetCallback_ttevent(tt_obj o);
    
tt_obj TTGetComponent_ttevent(tt_obj o);
    
ttuint TTGetEvtype_ttevent(tt_obj o);
    
ttuint TTGetEvcode_ttevent(tt_obj o);
    
ttuint TTGetEvflags_ttevent(tt_obj o);
            
void TTConsume_ttevent(tt_obj o);
 
/* prototypes for handy tteventbig methods */
        
ttshort TTGetX_tteventbig(tt_obj o);
    
ttshort TTGetY_tteventbig(tt_obj o);
    
ttshort TTGetW_tteventbig(tt_obj o);
    
ttshort TTGetH_tteventbig(tt_obj o);
    
ttany TTGetValue_tteventbig(tt_obj o);
    
ttany TTGetOldValue_tteventbig(tt_obj o);
    
ttuint TTGetLen_tteventbig(tt_obj o);
    
TT_CONST ttbyte * TTGetData_tteventbig(tt_obj o);
 
/* prototypes for handy ttbitmask methods */
                     
/* prototypes for handy tteventmask methods */
        
tt_obj TTGetEvtypeMask_tteventmask(tt_obj o);
    
tt_obj TTGetEvcodeMask_tteventmask(tt_obj o);
    
tt_obj TTGetComponentMask_tteventmask(tt_obj o);
    
ttuint TTGetTruthTable_tteventmask(tt_obj o);
 
/* prototypes for handy ttcallback methods */
        
ttuint TTGetLflags_ttcallback(tt_obj o);
    
tt_obj TTGetComponent_ttcallback(tt_obj o);
    
tt_obj TTGetPrev_ttcallback(tt_obj o);
    
tt_obj TTGetNext_ttcallback(tt_obj o);
 
/* prototypes for handy ttlistener methods */
            
tt_obj TTGetEvent_ttlistener(tt_obj o);
    
ttuint TTGetNargComponent_ttlistener(tt_obj o);
    
ttuint TTGetNargEvent_ttlistener(tt_obj o);
    
ttuint TTGetNargs_ttlistener(tt_obj o);
    
ttlistener_fn TTGetFunction_ttlistener(tt_obj o);
    
ttany * TTGetArgs_ttlistener(tt_obj o);
    
tt_obj TTGetEventMask_ttlistener(tt_obj o);
         
/* prototypes for handy tttimer methods */
                     
/* prototypes for handy ttcomponent methods */
        
tt_obj TTGetCallbacks_ttcomponent(tt_obj o);
    
tt_obj TTGetListeners_ttcomponent(tt_obj o);
    
tt_obj TTGetTimers_ttcomponent(tt_obj o);
    
tt_obj TTGetDatas_ttcomponent(tt_obj o);
 
/* prototypes for handy ttdata methods */
            
tt_obj TTGetComponent_ttdata(tt_obj o);
    
ttbyte * TTGetKey_ttdata(tt_obj o);
    
ttopaque TTGetKeyLen_ttdata(tt_obj o);
    
ttany TTGetData_ttdata(tt_obj o);

tt_fn TTSetData_ttdata(tt_obj o, ttany data);
 
/* prototypes for handy ttvisible methods */
        
ttuint TTGetVflags_ttvisible(tt_obj o);
    
tt_obj TTGetPrev_ttvisible(tt_obj o);
    
tt_obj TTGetNext_ttvisible(tt_obj o);
    
tt_obj TTGetParent_ttvisible(tt_obj o);
    
tt_obj TTGetChildFirst_ttvisible(tt_obj o);
    
tt_obj TTGetChildLast_ttvisible(tt_obj o);
    
tt_obj TTGetTheme_ttvisible(tt_obj o);
    
ttvisible_repaint_fn TTGetRepaint_ttvisible(tt_obj o);
 
/* prototypes for handy ttnative methods */
     
/* prototypes for handy ttwidget methods */
        
ttshort TTGetX_ttwidget(tt_obj o);
    
ttshort TTGetY_ttwidget(tt_obj o);
    
ttshort TTGetW_ttwidget(tt_obj o);
    
ttshort TTGetH_ttwidget(tt_obj o);
    
ttint TTGetXl_ttwidget(tt_obj o);
    
ttint TTGetYl_ttwidget(tt_obj o);
    
ttint TTGetWl_ttwidget(tt_obj o);
    
ttint TTGetHl_ttwidget(tt_obj o);
        
tt_obj TTGetTooltip_ttwidget(tt_obj o);
 
/* prototypes for handy ttlabel methods */
        
ttopaque TTGetTextLen_ttlabel(tt_obj o);
    
ttfont * TTGetText_ttlabel(tt_obj o);
 
/* prototypes for handy tttooltip methods */
            
tt_obj TTGetWidget_tttooltip(tt_obj o);
     
/* prototypes for handy ttbuttongroup methods */
        
tt_obj TTGetGroupFirst_ttbuttongroup(tt_obj o);
    
tt_obj TTGetGroupLast_ttbuttongroup(tt_obj o);
    
tt_obj TTGetChecked_ttbuttongroup(tt_obj o);
 
/* prototypes for handy ttanybutton methods */
        
ttattr * TTGetText_ttanybutton(tt_obj o);
    
ttshort TTGetTextWidth_ttanybutton(tt_obj o);
    
ttshort TTGetTextHeight_ttanybutton(tt_obj o);
     
/* prototypes for handy ttbutton methods */
     
/* prototypes for handy ttcheckbutton methods */
     
/* prototypes for handy ttradiobutton methods */
        
tt_obj TTGetGroup_ttradiobutton(tt_obj o);
    
tt_obj TTGetGroupPrev_ttradiobutton(tt_obj o);
    
tt_obj TTGetGroupNext_ttradiobutton(tt_obj o);
 
/* prototypes for handy ttanyscroll methods */
        
ttbyte TTGetOrientation_ttanyscroll(tt_obj o);
    
ttint TTGetSize_ttanyscroll(tt_obj o);
    
ttint TTGetRealSize_ttanyscroll(tt_obj o);
    
ttint TTGetViewSize_ttanyscroll(tt_obj o);
        
ttint TTGetPosition_ttanyscroll(tt_obj o);
    
ttint TTGetRealPosition_ttanyscroll(tt_obj o);
     
/* prototypes for handy ttscrollbar methods */
         
/* prototypes for handy ttslider methods */
        
ttint TTGetSlideMin_ttslider(tt_obj o);
    
ttint TTGetSlideMax_ttslider(tt_obj o);
    
ttint TTGetSlideValue_ttslider(tt_obj o);
 
/* prototypes for handy ttprogressbar methods */
             
/* prototypes for handy ttscrollpane methods */
        
tt_obj TTGetBarX_ttscrollpane(tt_obj o);
    
tt_obj TTGetBarY_ttscrollpane(tt_obj o);
 
/* prototypes for handy ttwindow methods */
        
ttopaque TTGetTitleLen_ttwindow(tt_obj o);
    
ttbyte * TTGetTitle_ttwindow(tt_obj o);
 
/* prototypes for handy ttframe methods */
        
tt_obj TTGetMenubar_ttframe(tt_obj o);
 
/* prototypes for handy ttscroller methods */
        
tt_obj TTGetScrollpane_ttscroller(tt_obj o);
 
/* prototypes for handy ttmenuitem methods */
        
ttbyte * TTGetName_ttmenuitem(tt_obj o);
 
/* prototypes for handy ttcheckmenuitem methods */
     
/* prototypes for handy ttradiomenuitem methods */
     
/* prototypes for handy ttmenu methods */
    
    
tt_obj TTGetMenubar_ttmenu(tt_obj o);
 
/* prototypes for handy ttmenubar methods */
     
/* prototypes for handy ttanytext methods */
        
ttopaque TTGetTextLen_ttanytext(tt_obj o);
    
ttattr * TTGetText_ttanytext(tt_obj o);
 
/* prototypes for handy tttextfield methods */
     
/* prototypes for handy tttextarea methods */
     
/* prototypes for handy tttheme methods */
    
    
ttbyte * TTGetName_tttheme(tt_obj o);
                 
/* prototypes for handy ttapplication methods */
        
ttbyte * TTGetName_ttapplication(tt_obj o);








#endif /* _TT_DECLSM4_H */

