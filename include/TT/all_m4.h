




/* This file was automatically generated from m4/all.m4, do not edit! */


/*
 *  all_m4.h  --  internal header file that includes all single libTT object header files
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TT_ALL_M4_H
#define _TT_ALL_M4_H

#ifndef _TT_TT_H
#error "Never include <TT/all_m4.h> directly; include <TT/tt.h> instead."
#endif







 
typedef struct s_ttobj	* ttobj; 
typedef struct s_ttclass	* ttclass; 
typedef struct s_ttobject	* ttobject; 
typedef struct s_ttvector	* ttvector; 
typedef struct s_ttfield	* ttfield; 
typedef struct s_ttmethod	* ttmethod; 
typedef struct s_ttevent	* ttevent; 
typedef struct s_tteventbig	* tteventbig; 
typedef struct s_ttbitmask	* ttbitmask; 
typedef struct s_tteventmask	* tteventmask; 
typedef struct s_ttcallback	* ttcallback; 
typedef struct s_ttlistener	* ttlistener; 
typedef struct s_tttimer	* tttimer; 
typedef struct s_ttcomponent	* ttcomponent; 
typedef struct s_ttdata	* ttdata; 
typedef struct s_ttvisible	* ttvisible; 
typedef struct s_ttlayout	* ttlayout; 
typedef struct s_ttboxlayout	* ttboxlayout; 
typedef struct s_ttborderlayout	* ttborderlayout; 
typedef struct s_ttnative	* ttnative; 
typedef struct s_ttwidget	* ttwidget; 
typedef struct s_ttlabel	* ttlabel; 
typedef struct s_tttooltip	* tttooltip; 
typedef struct s_ttbuttongroup	* ttbuttongroup; 
typedef struct s_ttanybutton	* ttanybutton; 
typedef struct s_ttbutton	* ttbutton; 
typedef struct s_ttcheckbutton	* ttcheckbutton; 
typedef struct s_ttradiobutton	* ttradiobutton; 
typedef struct s_ttanyscroll	* ttanyscroll; 
typedef struct s_ttscrollbar	* ttscrollbar; 
typedef struct s_ttslider	* ttslider; 
typedef struct s_ttprogressbar	* ttprogressbar; 
typedef struct s_ttscrollpane	* ttscrollpane; 
typedef struct s_ttwindow	* ttwindow; 
typedef struct s_ttframe	* ttframe; 
typedef struct s_ttscroller	* ttscroller; 
typedef struct s_ttmenuitem	* ttmenuitem; 
typedef struct s_ttcheckmenuitem	* ttcheckmenuitem; 
typedef struct s_ttradiomenuitem	* ttradiomenuitem; 
typedef struct s_ttmenu	* ttmenu; 
typedef struct s_ttmenubar	* ttmenubar; 
typedef struct s_ttanytext	* ttanytext; 
typedef struct s_tttextfield	* tttextfield; 
typedef struct s_tttextarea	* tttextarea; 
typedef struct s_tttheme	* tttheme; 
typedef struct s_ttapplication	* ttapplication;



 
#define s_ttobj	s_ttobj 
#define s_ttclass	s_ttclass 
#define s_ttobject	s_ttobject 
#define s_ttvector	s_ttvector 
#define s_ttfield	s_ttfield 
#define s_ttmethod	s_ttmethod 
#define s_ttevent	s_ttevent 
#define s_tteventbig	s_tteventbig 
#define s_ttbitmask	s_ttbitmask 
#define s_tteventmask	s_tteventmask 
#define s_ttcallback	s_ttcallback 
#define s_ttlistener	s_ttlistener 
#define s_tttimer	s_tttimer 
#define s_ttcomponent	s_ttcomponent 
#define s_ttdata	s_ttdata 
#define s_ttvisible	s_ttvisible 
#define s_ttlayout	s_ttlayout 
#define s_ttboxlayout	s_ttboxlayout 
#define s_ttborderlayout	s_ttborderlayout 
#define s_ttnative	s_ttnative 
#define s_ttwidget	s_ttwidget 
#define s_ttlabel	s_ttlabel 
#define s_tttooltip	s_tttooltip 
#define s_ttbuttongroup	s_ttbuttongroup 
#define s_ttanybutton	s_ttanybutton 
#define s_ttbutton	s_ttbutton 
#define s_ttcheckbutton	s_ttcheckbutton 
#define s_ttradiobutton	s_ttradiobutton 
#define s_ttanyscroll	s_ttanyscroll 
#define s_ttscrollbar	s_ttscrollbar 
#define s_ttslider	s_ttslider 
#define s_ttprogressbar	s_ttprogressbar 
#define s_ttscrollpane	s_ttscrollpane 
#define s_ttwindow	s_ttwindow 
#define s_ttframe	s_ttframe 
#define s_ttscroller	s_ttscroller 
#define s_ttmenuitem	s_ttmenuitem 
#define s_ttcheckmenuitem	s_ttcheckmenuitem 
#define s_ttradiomenuitem	s_ttradiomenuitem 
#define s_ttmenu	s_ttmenu 
#define s_ttmenubar	s_ttmenubar 
#define s_ttanytext	s_ttanytext 
#define s_tttextfield	s_tttextfield 
#define s_tttextarea	s_tttextarea 
#define s_tttheme	s_tttheme 
#define s_ttapplication	s_ttapplication


/* useful types */
typedef void (*ttlistener_fn)(ttany arg0);
typedef void (*ttvisible_repaint_fn)(ttvisible,ttshort,ttshort,ttshort,ttshort);


 
#include <TT/ttobj.h> 
#include <TT/ttclass.h> 
#include <TT/ttobject.h> 
#include <TT/ttvector.h> 
#include <TT/ttfield.h> 
#include <TT/ttmethod.h> 
#include <TT/ttevent.h> 
#include <TT/tteventbig.h> 
#include <TT/ttbitmask.h> 
#include <TT/tteventmask.h> 
#include <TT/ttcallback.h> 
#include <TT/ttlistener.h> 
#include <TT/tttimer.h> 
#include <TT/ttcomponent.h> 
#include <TT/ttdata.h> 
#include <TT/ttvisible.h> 
#include <TT/ttlayout.h> 
#include <TT/ttboxlayout.h> 
#include <TT/ttborderlayout.h> 
#include <TT/ttnative.h> 
#include <TT/ttwidget.h> 
#include <TT/ttlabel.h> 
#include <TT/tttooltip.h> 
#include <TT/ttbuttongroup.h> 
#include <TT/ttanybutton.h> 
#include <TT/ttbutton.h> 
#include <TT/ttcheckbutton.h> 
#include <TT/ttradiobutton.h> 
#include <TT/ttanyscroll.h> 
#include <TT/ttscrollbar.h> 
#include <TT/ttslider.h> 
#include <TT/ttprogressbar.h> 
#include <TT/ttscrollpane.h> 
#include <TT/ttwindow.h> 
#include <TT/ttframe.h> 
#include <TT/ttscroller.h> 
#include <TT/ttmenuitem.h> 
#include <TT/ttcheckmenuitem.h> 
#include <TT/ttradiomenuitem.h> 
#include <TT/ttmenu.h> 
#include <TT/ttmenubar.h> 
#include <TT/ttanytext.h> 
#include <TT/tttextfield.h> 
#include <TT/tttextarea.h> 
#include <TT/tttheme.h> 
#include <TT/ttapplication.h>




#endif /* _TT_ALL_M4_H */

