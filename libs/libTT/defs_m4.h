




/* This file was automatically generated from m4/defs.m4, do not edit! */


/*
 *  defs_m4.h  --  INTERNAL (!!) header for libTT types declarations
 *
 *  Note: you cannot include both this file and <TT/defs_m4.h>, as they conflict!
 */

#ifndef _TT_DEFSM4_H
#define _TT_DEFSM4_H











/* many useful macros */

#define Min2(a,b) ((a) < (b) ? (a) : (b))

#define TT_OFFSETOF(type,field)	((size_t)&((type)0)->field)

/* #defines for objects hierarchy */



                              
#define super_ttevent	ttobj                     
#define super_tteventbig	ttevent                         
#define super_ttlistener	ttobj                 
#define super_ttcallback	ttlistener                             
#define super_ttcomponent	ttobj         
#define super_ttvisible	ttcomponent                                 
#define super_ttnative	ttvisible 
#define super_ttwidget	ttvisible                         
#define super_ttlabel	ttwidget         
#define super_ttanybutton	ttwidget                 
#define super_ttbutton	ttanybutton 
#define super_ttcheckbutton	ttbutton 
#define super_ttradiobutton	ttcheckbutton 
#define super_ttscrollbar	ttanybutton 
#define super_ttbuttongroup	ttcomponent 
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
typedef struct s_ttlistener *   ttlistener; 
typedef struct s_ttcallback *   ttcallback; 
typedef struct s_ttcomponent *   ttcomponent; 
typedef struct s_ttvisible *   ttvisible; 
typedef struct s_ttnative *   ttnative; 
typedef struct s_ttwidget *   ttwidget; 
typedef struct s_ttlabel *   ttlabel; 
typedef struct s_ttanybutton *   ttanybutton; 
typedef struct s_ttbutton *   ttbutton; 
typedef struct s_ttcheckbutton *   ttcheckbutton; 
typedef struct s_ttradiobutton *   ttradiobutton; 
typedef struct s_ttscrollbar *   ttscrollbar; 
typedef struct s_ttbuttongroup *   ttbuttongroup; 
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
typedef struct s_ttfn_ttlistener *	ttfn_ttlistener; 
typedef struct s_ttfn_ttcallback *	ttfn_ttcallback; 
typedef struct s_ttfn_ttcomponent *	ttfn_ttcomponent; 
typedef struct s_ttfn_ttvisible *	ttfn_ttvisible; 
typedef struct s_ttfn_ttnative *	ttfn_ttnative; 
typedef struct s_ttfn_ttwidget *	ttfn_ttwidget; 
typedef struct s_ttfn_ttlabel *	ttfn_ttlabel; 
typedef struct s_ttfn_ttanybutton *	ttfn_ttanybutton; 
typedef struct s_ttfn_ttbutton *	ttfn_ttbutton; 
typedef struct s_ttfn_ttcheckbutton *	ttfn_ttcheckbutton; 
typedef struct s_ttfn_ttradiobutton *	ttfn_ttradiobutton; 
typedef struct s_ttfn_ttscrollbar *	ttfn_ttscrollbar; 
typedef struct s_ttfn_ttbuttongroup *	ttfn_ttbuttongroup; 
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




/* declarations for objects structures */




typedef struct s_ttobj {
    ttfn_ttobj FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
} s_ttobj;
  
typedef struct s_ttevent {
    ttfn_ttevent FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttcallback callback;
    ttcomponent component;
    ttuint evtype;
    ttuint evcode;
    ttuint evflags;
} s_ttevent;
 
typedef struct s_tteventbig {
    ttfn_tteventbig FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttcallback callback;
    ttcomponent component;
    ttuint evtype;
    ttuint evcode;
    ttuint evflags;
    /* extends ttevent */
    ttshort x;
    ttshort y;
    ttshort w;
    ttshort h;
    ttuint len;
    TT_CONST ttbyte * data;
} s_tteventbig;
 
typedef struct s_ttlistener {
    ttfn_ttlistener FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttuint lflags;
    ttcomponent component;
    ttlistener prev;
    ttlistener next;
} s_ttlistener;
 
typedef struct s_ttcallback {
    ttfn_ttcallback FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttuint lflags;
    ttcomponent component;
    ttcallback prev;
    ttcallback next;
    /* extends ttlistener */
    s_ttavl AVL;
    ttevent event;
    ttuint narg_component;
    ttuint narg_event;
    ttuint nargs;
    ttcallback_fn function;
    ttany * args;
} s_ttcallback;
 
typedef struct s_ttcomponent {
    ttfn_ttcomponent FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
} s_ttcomponent;
 
typedef struct s_ttvisible {
    ttfn_ttvisible FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
} s_ttwidget;
 
typedef struct s_ttlabel {
    ttfn_ttlabel FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttshort text_len;
    ttfont * text;
} s_ttlabel;
 
typedef struct s_ttanybutton {
    ttfn_ttanybutton FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttfont * text;
    ttshort text_width;
    ttshort text_height;
    struct s_ttshape text_shape[ttbutton_shape_max];
} s_ttanybutton;
 
typedef struct s_ttbutton {
    ttfn_ttbutton FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttfont * text;
    ttshort text_width;
    ttshort text_height;
    struct s_ttshape text_shape[ttbutton_shape_max];
    /* extends ttanybutton */
} s_ttbutton;
 
typedef struct s_ttcheckbutton {
    ttfn_ttcheckbutton FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttfont * text;
    ttshort text_width;
    ttshort text_height;
    struct s_ttshape text_shape[ttbutton_shape_max];
    /* extends ttanybutton */
    /* extends ttbutton */
} s_ttcheckbutton;
 
typedef struct s_ttradiobutton {
    ttfn_ttradiobutton FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttfont * text;
    ttshort text_width;
    ttshort text_height;
    struct s_ttshape text_shape[ttbutton_shape_max];
    /* extends ttanybutton */
    /* extends ttbutton */
    /* extends ttcheckbutton */
} s_ttradiobutton;
 
typedef struct s_ttscrollbar {
    ttfn_ttscrollbar FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttfont * text;
    ttshort text_width;
    ttshort text_height;
    struct s_ttshape text_shape[ttbutton_shape_max];
    /* extends ttanybutton */
} s_ttscrollbar;
 
typedef struct s_ttbuttongroup {
    ttfn_ttbuttongroup FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
    /* extends ttcomponent */
} s_ttbuttongroup;
 
typedef struct s_ttwindow {
    ttfn_ttwindow FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttmenubar menubar;
} s_ttwindow;
 
typedef struct s_ttframe {
    ttfn_ttframe FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttmenubar menubar;
    /* extends ttwindow */
} s_ttframe;
 
typedef struct s_ttscroller {
    ttfn_ttscroller FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttmenubar menubar;
    /* extends ttwindow */
    ttscrollbar scrollx;
    ttscrollbar scrolly;
} s_ttscroller;
 
typedef struct s_ttmenuitem {
    ttfn_ttmenuitem FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttmenubar menubar;
    /* extends ttwindow */
    ttshort text_len;
    ttattr * text;
} s_ttanytext;
 
typedef struct s_tttextfield {
    ttfn_tttextfield FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttmenubar menubar;
    /* extends ttwindow */
    ttshort text_len;
    ttattr * text;
    /* extends ttanytext */
} s_tttextfield;
 
typedef struct s_tttextarea {
    ttfn_tttextarea FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
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
    /* extends ttwidget */
    ttmenubar menubar;
    /* extends ttwindow */
    ttshort text_len;
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
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
    /* extends ttcomponent */

    ttbyte * name;
    ttfont fill;
    ttcol bg[tttheme_bg_max];

    struct s_ttshape shape[tttheme_shape_max][2];
} s_tttheme;
 
typedef struct s_ttapplication {
    ttfn_ttapplication FN;
    ttopaque id;
    ttuint refcount;
    ttuint oflags;
    ttany native;
    ttany private;
    ttany user_data;
    ttuint events_inprogress;
    /* extends ttobj */
    ttlistener listeners;
    ttcallback callbacks;
    /* extends ttcomponent */
    ttbyte * name;
} s_ttapplication;






/* declarations for methods structures */





typedef struct s_ttfn_ttobj {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttobj (*New)(ttfn_ttobj FN, ttobj);
    ttobj (*Build)(ttobj o);
    void (*Break)(ttobj o);
    void (*Del)(ttobj);
    ttbyte (*GetValue)(ttobj o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttobj o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttobj o, ttuint which, ttany nand_value, ttany xor_value);
        
} s_ttfn_ttobj;
  
typedef struct s_ttfn_ttevent {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttevent (*New)(ttfn_ttevent FN, ttevent);
    ttevent (*Build)(ttevent o);
    void (*Break)(ttevent o);
    void (*Del)(ttevent);
    ttbyte (*GetValue)(ttevent o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttevent o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttevent o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;
    
} s_ttfn_ttevent;
 
typedef struct s_ttfn_tteventbig {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tteventbig (*New)(ttfn_tteventbig FN, tteventbig);
    tteventbig (*Build)(tteventbig o);
    void (*Break)(tteventbig o);
    void (*Del)(tteventbig);
    ttbyte (*GetValue)(tteventbig o, ttuint which, ttany *value);
    ttbyte (*SetValue)(tteventbig o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(tteventbig o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;
    
    ttfn_ttevent FN_ttevent;

    
} s_ttfn_tteventbig;
 
typedef struct s_ttfn_ttlistener {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttlistener (*New)(ttfn_ttlistener FN, ttlistener);
    ttlistener (*Build)(ttlistener o);
    void (*Break)(ttlistener o);
    void (*Del)(ttlistener);
    ttbyte (*GetValue)(ttlistener o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttlistener o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttlistener o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttlistener (*Create)(ttcomponent o);

} s_ttfn_ttlistener;
 
typedef struct s_ttfn_ttcallback {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttcallback (*New)(ttfn_ttcallback FN, ttcallback);
    ttcallback (*Build)(ttcallback o);
    void (*Break)(ttcallback o);
    void (*Del)(ttcallback);
    ttbyte (*GetValue)(ttcallback o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttcallback o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttcallback o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttcallback (*Create)(ttcomponent o);

    ttfn_ttlistener FN_ttlistener;
            
} s_ttfn_ttcallback;
 
typedef struct s_ttfn_ttcomponent {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttcomponent (*New)(ttfn_ttcomponent FN, ttcomponent);
    ttcomponent (*Build)(ttcomponent o);
    void (*Break)(ttcomponent o);
    void (*Del)(ttcomponent);
    ttbyte (*GetValue)(ttcomponent o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttcomponent o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttcomponent o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

} s_ttfn_ttcomponent;
 
typedef struct s_ttfn_ttvisible {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttvisible (*New)(ttfn_ttvisible FN, ttvisible);
    ttvisible (*Build)(ttvisible o);
    void (*Break)(ttvisible o);
    void (*Del)(ttvisible);
    ttbyte (*GetValue)(ttvisible o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttvisible o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttvisible o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttvisible o, ttvisible parent);
    void (*Remove)(ttvisible o);
    void (*SetVisible)(ttvisible o, ttbyte on_off);
    void (*SetTheme)(ttvisible o, tttheme theme);
    void (*BuiltinRepaint)(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h);
            
} s_ttfn_ttvisible;
 
typedef struct s_ttfn_ttnative {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttnative (*New)(ttfn_ttnative FN, ttnative);
    ttnative (*Build)(ttnative o);
    void (*Break)(ttnative o);
    void (*Del)(ttnative);
    ttbyte (*GetValue)(ttnative o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttnative o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttnative o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttnative o, ttvisible parent);
    void (*Remove)(ttnative o);
    void (*SetVisible)(ttnative o, ttbyte on_off);
    void (*SetTheme)(ttnative o, tttheme theme);
    void (*BuiltinRepaint)(ttnative o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    ttnative (*GetRoot)(void);
    
} s_ttfn_ttnative;
 
typedef struct s_ttfn_ttwidget {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttwidget (*New)(ttfn_ttwidget FN, ttwidget);
    ttwidget (*Build)(ttwidget o);
    void (*Break)(ttwidget o);
    void (*Del)(ttwidget);
    ttbyte (*GetValue)(ttwidget o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttwidget o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttwidget o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttwidget o, ttvisible parent);
    void (*Remove)(ttwidget o);
    void (*SetVisible)(ttwidget o, ttbyte on_off);
    void (*SetTheme)(ttwidget o, tttheme theme);
    void (*BuiltinRepaint)(ttwidget o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttwidget o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttwidget o, ttuint mask, ttint xl, ttint yl);
                
} s_ttfn_ttwidget;
 
typedef struct s_ttfn_ttlabel {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttlabel (*New)(ttfn_ttlabel FN, ttlabel);
    ttlabel (*Build)(ttlabel o);
    void (*Break)(ttlabel o);
    void (*Del)(ttlabel);
    ttbyte (*GetValue)(ttlabel o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttlabel o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttlabel o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttlabel o, ttvisible parent);
    void (*Remove)(ttlabel o);
    void (*SetVisible)(ttlabel o, ttbyte on_off);
    void (*SetTheme)(ttlabel o, tttheme theme);
    void (*BuiltinRepaint)(ttlabel o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttlabel o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttlabel o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;
    ttbyte (*SetText)(ttlabel o, TT_CONST ttbyte * text);
    
} s_ttfn_ttlabel;
 
typedef struct s_ttfn_ttanybutton {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttanybutton (*New)(ttfn_ttanybutton FN, ttanybutton);
    ttanybutton (*Build)(ttanybutton o);
    void (*Break)(ttanybutton o);
    void (*Del)(ttanybutton);
    ttbyte (*GetValue)(ttanybutton o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttanybutton o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttanybutton o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttanybutton o, ttvisible parent);
    void (*Remove)(ttanybutton o);
    void (*SetVisible)(ttanybutton o, ttbyte on_off);
    void (*SetTheme)(ttanybutton o, tttheme theme);
    void (*BuiltinRepaint)(ttanybutton o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttanybutton o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttanybutton o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;
    ttbyte (*SetText)(ttanybutton o, TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch);
    
} s_ttfn_ttanybutton;
 
typedef struct s_ttfn_ttbutton {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttbutton (*New)(ttfn_ttbutton FN, ttbutton);
    ttbutton (*Build)(ttbutton o);
    void (*Break)(ttbutton o);
    void (*Del)(ttbutton);
    ttbyte (*GetValue)(ttbutton o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttbutton o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttbutton o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttbutton o, ttvisible parent);
    void (*Remove)(ttbutton o);
    void (*SetVisible)(ttbutton o, ttbyte on_off);
    void (*SetTheme)(ttbutton o, tttheme theme);
    void (*BuiltinRepaint)(ttbutton o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttbutton o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttbutton o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;
    ttbyte (*SetText)(ttbutton o, TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch);
    
    ttfn_ttanybutton FN_ttanybutton;
    void (*SetPressed)(ttbutton o, ttbyte pressed);
        
} s_ttfn_ttbutton;
 
typedef struct s_ttfn_ttcheckbutton {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttcheckbutton (*New)(ttfn_ttcheckbutton FN, ttcheckbutton);
    ttcheckbutton (*Build)(ttcheckbutton o);
    void (*Break)(ttcheckbutton o);
    void (*Del)(ttcheckbutton);
    ttbyte (*GetValue)(ttcheckbutton o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttcheckbutton o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttcheckbutton o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttcheckbutton o, ttvisible parent);
    void (*Remove)(ttcheckbutton o);
    void (*SetVisible)(ttcheckbutton o, ttbyte on_off);
    void (*SetTheme)(ttcheckbutton o, tttheme theme);
    void (*BuiltinRepaint)(ttcheckbutton o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttcheckbutton o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttcheckbutton o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;
    ttbyte (*SetText)(ttcheckbutton o, TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch);
    
    ttfn_ttanybutton FN_ttanybutton;
    void (*SetPressed)(ttcheckbutton o, ttbyte pressed);
        
    ttfn_ttbutton FN_ttbutton;

} s_ttfn_ttcheckbutton;
 
typedef struct s_ttfn_ttradiobutton {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttradiobutton (*New)(ttfn_ttradiobutton FN, ttradiobutton);
    ttradiobutton (*Build)(ttradiobutton o);
    void (*Break)(ttradiobutton o);
    void (*Del)(ttradiobutton);
    ttbyte (*GetValue)(ttradiobutton o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttradiobutton o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttradiobutton o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttradiobutton o, ttvisible parent);
    void (*Remove)(ttradiobutton o);
    void (*SetVisible)(ttradiobutton o, ttbyte on_off);
    void (*SetTheme)(ttradiobutton o, tttheme theme);
    void (*BuiltinRepaint)(ttradiobutton o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttradiobutton o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttradiobutton o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;
    ttbyte (*SetText)(ttradiobutton o, TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch);
    
    ttfn_ttanybutton FN_ttanybutton;
    void (*SetPressed)(ttradiobutton o, ttbyte pressed);
        
    ttfn_ttbutton FN_ttbutton;

    ttfn_ttcheckbutton FN_ttcheckbutton;

} s_ttfn_ttradiobutton;
 
typedef struct s_ttfn_ttscrollbar {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttscrollbar (*New)(ttfn_ttscrollbar FN, ttscrollbar);
    ttscrollbar (*Build)(ttscrollbar o);
    void (*Break)(ttscrollbar o);
    void (*Del)(ttscrollbar);
    ttbyte (*GetValue)(ttscrollbar o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttscrollbar o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttscrollbar o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttscrollbar o, ttvisible parent);
    void (*Remove)(ttscrollbar o);
    void (*SetVisible)(ttscrollbar o, ttbyte on_off);
    void (*SetTheme)(ttscrollbar o, tttheme theme);
    void (*BuiltinRepaint)(ttscrollbar o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttscrollbar o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttscrollbar o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;
    ttbyte (*SetText)(ttscrollbar o, TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch);
    
    ttfn_ttanybutton FN_ttanybutton;

} s_ttfn_ttscrollbar;
 
typedef struct s_ttfn_ttbuttongroup {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttbuttongroup (*New)(ttfn_ttbuttongroup FN, ttbuttongroup);
    ttbuttongroup (*Build)(ttbuttongroup o);
    void (*Break)(ttbuttongroup o);
    void (*Del)(ttbuttongroup);
    ttbyte (*GetValue)(ttbuttongroup o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttbuttongroup o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttbuttongroup o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;

} s_ttfn_ttbuttongroup;
 
typedef struct s_ttfn_ttwindow {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttwindow (*New)(ttfn_ttwindow FN, ttwindow);
    ttwindow (*Build)(ttwindow o);
    void (*Break)(ttwindow o);
    void (*Del)(ttwindow);
    ttbyte (*GetValue)(ttwindow o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttwindow o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttwindow o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttwindow o, ttvisible parent);
    void (*Remove)(ttwindow o);
    void (*SetVisible)(ttwindow o, ttbyte on_off);
    void (*SetTheme)(ttwindow o, tttheme theme);
    void (*BuiltinRepaint)(ttwindow o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttwindow o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttwindow o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;

} s_ttfn_ttwindow;
 
typedef struct s_ttfn_ttframe {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttframe (*New)(ttfn_ttframe FN, ttframe);
    ttframe (*Build)(ttframe o);
    void (*Break)(ttframe o);
    void (*Del)(ttframe);
    ttbyte (*GetValue)(ttframe o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttframe o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttframe o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttframe o, ttvisible parent);
    void (*Remove)(ttframe o);
    void (*SetVisible)(ttframe o, ttbyte on_off);
    void (*SetTheme)(ttframe o, tttheme theme);
    void (*BuiltinRepaint)(ttframe o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttframe o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttframe o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;

    ttfn_ttwindow FN_ttwindow;

} s_ttfn_ttframe;
 
typedef struct s_ttfn_ttscroller {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttscroller (*New)(ttfn_ttscroller FN, ttscroller);
    ttscroller (*Build)(ttscroller o);
    void (*Break)(ttscroller o);
    void (*Del)(ttscroller);
    ttbyte (*GetValue)(ttscroller o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttscroller o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttscroller o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttscroller o, ttvisible parent);
    void (*Remove)(ttscroller o);
    void (*SetVisible)(ttscroller o, ttbyte on_off);
    void (*SetTheme)(ttscroller o, tttheme theme);
    void (*BuiltinRepaint)(ttscroller o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttscroller o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttscroller o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;

    ttfn_ttwindow FN_ttwindow;

} s_ttfn_ttscroller;
 
typedef struct s_ttfn_ttmenuitem {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttmenuitem (*New)(ttfn_ttmenuitem FN, ttmenuitem);
    ttmenuitem (*Build)(ttmenuitem o);
    void (*Break)(ttmenuitem o);
    void (*Del)(ttmenuitem);
    ttbyte (*GetValue)(ttmenuitem o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttmenuitem o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttmenuitem o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttmenuitem o, ttvisible parent);
    void (*Remove)(ttmenuitem o);
    void (*SetVisible)(ttmenuitem o, ttbyte on_off);
    void (*SetTheme)(ttmenuitem o, tttheme theme);
    void (*BuiltinRepaint)(ttmenuitem o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;

} s_ttfn_ttmenuitem;
 
typedef struct s_ttfn_ttcheckmenuitem {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttcheckmenuitem (*New)(ttfn_ttcheckmenuitem FN, ttcheckmenuitem);
    ttcheckmenuitem (*Build)(ttcheckmenuitem o);
    void (*Break)(ttcheckmenuitem o);
    void (*Del)(ttcheckmenuitem);
    ttbyte (*GetValue)(ttcheckmenuitem o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttcheckmenuitem o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttcheckmenuitem o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttcheckmenuitem o, ttvisible parent);
    void (*Remove)(ttcheckmenuitem o);
    void (*SetVisible)(ttcheckmenuitem o, ttbyte on_off);
    void (*SetTheme)(ttcheckmenuitem o, tttheme theme);
    void (*BuiltinRepaint)(ttcheckmenuitem o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;

    ttfn_ttmenuitem FN_ttmenuitem;

} s_ttfn_ttcheckmenuitem;
 
typedef struct s_ttfn_ttradiomenuitem {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttradiomenuitem (*New)(ttfn_ttradiomenuitem FN, ttradiomenuitem);
    ttradiomenuitem (*Build)(ttradiomenuitem o);
    void (*Break)(ttradiomenuitem o);
    void (*Del)(ttradiomenuitem);
    ttbyte (*GetValue)(ttradiomenuitem o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttradiomenuitem o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttradiomenuitem o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttradiomenuitem o, ttvisible parent);
    void (*Remove)(ttradiomenuitem o);
    void (*SetVisible)(ttradiomenuitem o, ttbyte on_off);
    void (*SetTheme)(ttradiomenuitem o, tttheme theme);
    void (*BuiltinRepaint)(ttradiomenuitem o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;

    ttfn_ttmenuitem FN_ttmenuitem;

    ttfn_ttcheckmenuitem FN_ttcheckmenuitem;

} s_ttfn_ttradiomenuitem;
 
typedef struct s_ttfn_ttmenu {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttmenu (*New)(ttfn_ttmenu FN, ttmenu);
    ttmenu (*Build)(ttmenu o);
    void (*Break)(ttmenu o);
    void (*Del)(ttmenu);
    ttbyte (*GetValue)(ttmenu o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttmenu o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttmenu o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttmenu o, ttvisible parent);
    void (*Remove)(ttmenu o);
    void (*SetVisible)(ttmenu o, ttbyte on_off);
    void (*SetTheme)(ttmenu o, tttheme theme);
    void (*BuiltinRepaint)(ttmenu o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;

    ttfn_ttmenuitem FN_ttmenuitem;

} s_ttfn_ttmenu;
 
typedef struct s_ttfn_ttmenubar {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttmenubar (*New)(ttfn_ttmenubar FN, ttmenubar);
    ttmenubar (*Build)(ttmenubar o);
    void (*Break)(ttmenubar o);
    void (*Del)(ttmenubar);
    ttbyte (*GetValue)(ttmenubar o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttmenubar o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttmenubar o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttmenubar o, ttvisible parent);
    void (*Remove)(ttmenubar o);
    void (*SetVisible)(ttmenubar o, ttbyte on_off);
    void (*SetTheme)(ttmenubar o, tttheme theme);
    void (*BuiltinRepaint)(ttmenubar o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    
} s_ttfn_ttmenubar;
 
typedef struct s_ttfn_ttanytext {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttanytext (*New)(ttfn_ttanytext FN, ttanytext);
    ttanytext (*Build)(ttanytext o);
    void (*Break)(ttanytext o);
    void (*Del)(ttanytext);
    ttbyte (*GetValue)(ttanytext o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttanytext o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttanytext o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(ttanytext o, ttvisible parent);
    void (*Remove)(ttanytext o);
    void (*SetVisible)(ttanytext o, ttbyte on_off);
    void (*SetTheme)(ttanytext o, tttheme theme);
    void (*BuiltinRepaint)(ttanytext o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(ttanytext o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(ttanytext o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;

    ttfn_ttwindow FN_ttwindow;

} s_ttfn_ttanytext;
 
typedef struct s_ttfn_tttextfield {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tttextfield (*New)(ttfn_tttextfield FN, tttextfield);
    tttextfield (*Build)(tttextfield o);
    void (*Break)(tttextfield o);
    void (*Del)(tttextfield);
    ttbyte (*GetValue)(tttextfield o, ttuint which, ttany *value);
    ttbyte (*SetValue)(tttextfield o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(tttextfield o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(tttextfield o, ttvisible parent);
    void (*Remove)(tttextfield o);
    void (*SetVisible)(tttextfield o, ttbyte on_off);
    void (*SetTheme)(tttextfield o, tttheme theme);
    void (*BuiltinRepaint)(tttextfield o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(tttextfield o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(tttextfield o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;

    ttfn_ttwindow FN_ttwindow;

    ttfn_ttanytext FN_ttanytext;

} s_ttfn_tttextfield;
 
typedef struct s_ttfn_tttextarea {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tttextarea (*New)(ttfn_tttextarea FN, tttextarea);
    tttextarea (*Build)(tttextarea o);
    void (*Break)(tttextarea o);
    void (*Del)(tttextarea);
    ttbyte (*GetValue)(tttextarea o, ttuint which, ttany *value);
    ttbyte (*SetValue)(tttextarea o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(tttextarea o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    void (*AddTo)(tttextarea o, ttvisible parent);
    void (*Remove)(tttextarea o);
    void (*SetVisible)(tttextarea o, ttbyte on_off);
    void (*SetTheme)(tttextarea o, tttheme theme);
    void (*BuiltinRepaint)(tttextarea o, ttshort x, ttshort y, ttshort w, ttshort h);
            
    ttfn_ttvisible FN_ttvisible;
    void (*SetXYWH)(tttextarea o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h);
    void (*SetXlYl)(tttextarea o, ttuint mask, ttint xl, ttint yl);
                
    ttfn_ttwidget FN_ttwidget;

    ttfn_ttwindow FN_ttwindow;

    ttfn_ttanytext FN_ttanytext;

    ttfn_tttextfield FN_tttextfield;

} s_ttfn_tttextarea;
 
typedef struct s_ttfn_tttheme {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    tttheme (*New)(ttfn_tttheme FN, tttheme);
    tttheme (*Build)(tttheme o);
    void (*Break)(tttheme o);
    void (*Del)(tttheme);
    ttbyte (*GetValue)(tttheme o, ttuint which, ttany *value);
    ttbyte (*SetValue)(tttheme o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(tttheme o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    ttattr *(*ComputeShapeButton)(ttbutton o, ttbyte button_i, ttbyte theme_i);
    
} s_ttfn_tttheme;
 
typedef struct s_ttfn_ttapplication {
    ttuint order;
    ttuint magicmask;
    ttuint magic;
    ttuint size;
    ttapplication (*New)(ttfn_ttapplication FN, ttapplication);
    ttapplication (*Build)(ttapplication o);
    void (*Break)(ttapplication o);
    void (*Del)(ttapplication);
    ttbyte (*GetValue)(ttapplication o, ttuint which, ttany *value);
    ttbyte (*SetValue)(ttapplication o, ttuint which, ttany value);
    ttbyte (*ChangeValue)(ttapplication o, ttuint which, ttany nand_value, ttany xor_value);
        
    ttfn_ttobj FN_ttobj;

    ttfn_ttcomponent FN_ttcomponent;
    
} s_ttfn_ttapplication;






typedef struct s_tthw *tthw;
typedef struct s_tthw {
    byte (*Sync)(void);
    byte (*Flush)(void);
    byte (*TimidFlush)(void);
    byte (*MainLoop)(void);
    void (*ExitMainLoop)(void);
    void (*DeleteListener)(ttlistener);

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
    s_ttfn_ttlistener fn_ttlistener; 
    s_ttfn_ttcallback fn_ttcallback; 
    s_ttfn_ttcomponent fn_ttcomponent; 
    s_ttfn_ttvisible fn_ttvisible; 
    s_ttfn_ttnative fn_ttnative; 
    s_ttfn_ttwidget fn_ttwidget; 
    s_ttfn_ttlabel fn_ttlabel; 
    s_ttfn_ttanybutton fn_ttanybutton; 
    s_ttfn_ttbutton fn_ttbutton; 
    s_ttfn_ttcheckbutton fn_ttcheckbutton; 
    s_ttfn_ttradiobutton fn_ttradiobutton; 
    s_ttfn_ttscrollbar fn_ttscrollbar; 
    s_ttfn_ttbuttongroup fn_ttbuttongroup; 
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



/* #defines for global methods structures */
#define TFNs	(TTD.FN_)

#define THW	(TTD.FN.HW)



#define TFN_ttobj	(&TTD.FN.fn_ttobj)  
#define TFN_ttevent	(&TTD.FN.fn_ttevent) 
#define TFN_tteventbig	(&TTD.FN.fn_tteventbig) 
#define TFN_ttlistener	(&TTD.FN.fn_ttlistener) 
#define TFN_ttcallback	(&TTD.FN.fn_ttcallback) 
#define TFN_ttcomponent	(&TTD.FN.fn_ttcomponent) 
#define TFN_ttvisible	(&TTD.FN.fn_ttvisible) 
#define TFN_ttnative	(&TTD.FN.fn_ttnative) 
#define TFN_ttwidget	(&TTD.FN.fn_ttwidget) 
#define TFN_ttlabel	(&TTD.FN.fn_ttlabel) 
#define TFN_ttanybutton	(&TTD.FN.fn_ttanybutton) 
#define TFN_ttbutton	(&TTD.FN.fn_ttbutton) 
#define TFN_ttcheckbutton	(&TTD.FN.fn_ttcheckbutton) 
#define TFN_ttradiobutton	(&TTD.FN.fn_ttradiobutton) 
#define TFN_ttscrollbar	(&TTD.FN.fn_ttscrollbar) 
#define TFN_ttbuttongroup	(&TTD.FN.fn_ttbuttongroup) 
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


#endif /* _TT_DEFSM4_H */

