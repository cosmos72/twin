




/* This file was automatically generated from twin_c by m4/hw.m4h, do not edit! */


/*
 *  twin_m4.c  --  implementation of libTT methods on "twin" target display
 *
 *  Copyright (C) 2001,2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */


























/*
 *                 WARNING!
 * 
 * this file is twin_c and is preprocessed by m4 to produce twin_m4.c
 * 
 * It must be a valid m4 file, and must produce a valid C file.
 * 
 */


#include <Tw/Tw.h>
#include <Tw/Twavl.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>
#include <Tw/Twerrno.h>




/* order is crucial here! */

#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

/* include our internal copy of TT.h instead of the public one */
#include "TT.h"
#include <TT/TTerrno.h>

#include "mutex.h"

#include "TTextern.h"
#include "TTassert.h"

#include "utils.h"
#include "inlines.h"
#include "seterrno.h"
#include "theme.h"
#include "common_m4.h"

static s_ttclasses twin_TTClasses;







/*
 *                 WARNING!
 * 
 * this file is twin_h and is included by m4 files twin_c and twin_detunnel_c
 * 
 * It must be a valid m4 file, and must produce a valid C file.
 * 
 */


/*
 * We need to write down only overloaded methods.
 */
 

/* ttobj */


/* ttobject */
static void twin_Break_ttobject(ttobject o) {
    if (o) {
	if (o->native != TW_NOID) {
	    TwDeleteObj(o->native);
	    o->native = TW_NOID;
	}
	/* call null_Break_ttobj: */
	TClass_default(ttobject)->Break(o);
    }
}


/* ttvisible */
static void twin_AtBuild_ttvisible(ttvisible o) {
    /*
     * add all children of this ttvisible:
     * they have not been added yet, since we just
     * called Build() on the parent ttvisible
     */
    opaque native = o->native, child_native;
    ttvisible child = o->child_first;
    
    if (IS(ttwidget,o) && native != TW_NOID) while (child) {
	if ((child_native = child->native))
	    TwMapWidget(child_native,native);
	child = child->next;
    }
}

/* ttnative */
static ttnative twin_GetRoot_ttnative(void) {
    return Create_ttnative(TwFirstScreen());
}
static void twin_Break_ttnative(ttnative o) {
    o->native = TW_NOID;
    TClass_default(ttnative)->Break(o);
}
static ttshort twin_GetW_ttnative(ttnative o) {
    ttnative r = TClass_ttnative->GetRoot();
    if (r && r->native == o->native)
	return TwGetDisplayWidth();
    return TwStat(o->native,TWS_widget_Width);
}
static ttshort twin_GetH_ttnative(ttnative o) {
    ttnative r = TClass_ttnative->GetRoot();
    if (r && r->native == o->native)
	return TwGetDisplayHeight();
    return TwStat(o->native,TWS_widget_Height);
}


/* ttwidget */
static void twin_BuiltinRepaint_ttwidget(ttwidget o, dat x, dat y, dat w, dat h) {
    TwDrawHWAttrWidget(o->native,w,h,x,y,w,NULL);
}
static void twin_ExposeCallback_ttwidget(tevent_any e, void *id) {
    ttwidget o = ID2(ttwidget,(opaque)id);
    if (o)
	Expose_ttvisible((ttvisible)o, e->EventWidget.X, e->EventWidget.Y, e->EventWidget.XWidth, e->EventWidget.YWidth);
}
static void twin_ResizeCallback_ttwidget(tevent_any e, void *id) {
    ttwidget o = ID2(ttwidget,(opaque)id);
    if (o) {
	FIRE_EVENT(o->w = e->EventWidget.XWidth, o, ttwidget_w, e->EventWidget.XWidth, o->w);
	FIRE_EVENT(o->h = e->EventWidget.YWidth, o, ttwidget_h, e->EventWidget.YWidth, o->h);
    }
}
static ttcallback twin_AddCommonCallbacks_ttwidget(ttwidget o) {
    ttcallback l[5];
    byte i = 0;
    void *id = (void *)OBJ2ID(o);
    
	
    if ((l[i] = Create_ttcallback((ttcomponent)o)) &&
	(l[i]->native = (opaque)TwAddWidgetListener(o->native,TW_MSG_WIDGET_EXPOSE,(tfn_listener)twin_ExposeCallback_ttwidget,id)) &&
	(i++, (l[i] = Create_ttcallback((ttcomponent)o))) &&
	(l[i]->native = (opaque)TwAddWidgetListener(o->native,TW_MSG_WIDGET_RESIZE,(tfn_listener)twin_ResizeCallback_ttwidget,id))) {

	return l[i];
    }
    while (i) {
    	TDEL(l[i]);
	i--;
    }
    return NULL;
}
static ttwidget twin_Build_ttwidget(ttwidget o) {
    tttheme t = myTheme(o);
    hwfont f = t->fill;
    hwcol c = o->col;
    if (TW_NOID != (o->native = TwCreateWidget(o->w,o->h,0,TW_WIDGETFL_USEFILL,0,MAXDAT,HWATTR(c, f)))) {
	twin_AtBuild_ttvisible((ttvisible)o);
	return o;
    }
    return NULL;
}
static void twin_AddTo_ttwidget(ttwidget o, ttvisible parent, ttany constraint) {
    if (parent && !o->parent) {
	TClass_default(ttwidget)->AddTo(o, parent, constraint);
	
	if (o->native != TW_NOID && parent->native != TW_NOID)
	    TwMapWidget(o->native,parent->native);
    }
}
static void twin_SetVisible_ttwidget(ttwidget o, ttbyte visible) {
    ttuint isvisible, wasvisibile;
    
    wasvisibile = o->vflags & ttvisible_vflags_visible;
    
    TClass_default(ttwidget)->SetVisible(o, visible);
    
    isvisible = o->vflags & ttvisible_vflags_visible;
    
    if (isvisible != wasvisibile && o->native != TW_NOID)
	TwSetVisibleWidget(o->native,visible);
}
static void twin_Remove_ttwidget(ttwidget o) {
    if (o->parent) {
	TClass_default(ttwidget)->Remove(o);
	
	if (o->native != TW_NOID)
	    TwUnMapWidget(o->native);
    }
}
static ttbyte twin_SetXY_ttwidget(ttwidget o, ttshort x, ttshort y) {
    TwSetXYWidget(o->native,o->x = x,o->y = y);
    return TT_TRUE;
}
static ttbyte twin_SetWH_ttwidget(ttwidget o, ttshort w, ttshort h) {
    TwResizeWidget(o->native,o->w = w,o->h = h);
    return TT_TRUE;
}
static ttbyte twin_SetXl_ttwidget(ttwidget o, ttint xl) {
    TwScrollWidget(o->native,xl - o->xl,0);
    o->xl = xl;
    return TT_TRUE;
}
static ttbyte twin_SetYl_ttwidget(ttwidget o, ttint yl) {
    TwScrollWidget(o->native,0,yl - o->yl);
    o->yl = yl;
    return TT_TRUE;
}
static void twin_Draw_ttwidget(ttwidget o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch,
			       TT_CONST ttbyte *asciidata, TT_CONST ttfont *fontdata, TT_CONST ttattr *attrdata) {
    TwDraw2Widget(o->native,w,h,x,y,pitch,asciidata,fontdata,attrdata);
}


/* ttlabel */
static ttlabel twin_Build_ttlabel(ttlabel o) {
    if (TW_NOID != (o->native = TwCreateWidget(o->text_len,1,0,TW_WIDGETFL_USEEXPOSE,0,MAXDAT,HWATTR(o->col, myTheme(o)->fill)))) {
	if (twin_AddCommonCallbacks_ttwidget((ttwidget)o)) {
	    twin_AtBuild_ttvisible((ttvisible)o);
	    return o;
	}
    }
    return NULL;
}
static void twin_Invalidate_ttwidget(ttwidget o) {
    tttheme t;
    
    TClass_default(ttwidget)->Invalidate(o);
    
    t = myTheme(o);
    
    TwSetFillWidget(o->native,HWATTR(o->col, t->fill));
}

static void twin_BuiltinRepaint_ttlabel(ttlabel o, dat x, dat y, dat w, dat h) {
    if (y == 0) {
	if (x < o->text_len)
	    TwDrawHWFontWidget(o->native,TT_MIN2(w,o->text_len-x),1,x,0,o->text_len,o->text+x);
	if (x+w > o->text_len)
	    TwDrawHWFontWidget(o->native,o->text_len-x-w,1,o->text_len,0,0,NULL);
    }
}


/* ttbuttongroup */



/* ttanybutton */
static void twin_Invalidate_ttanybutton(ttanybutton o) {
    /* needed because we have overridden Invalidate_ttwidget above */
    ttbyte i;
    
#ifdef DEBUG_TT_HW_TWIN
    fprintf(stderr, "twin_Invalidate_ttanybutton(o=0x%X)\n",
	    (unsigned)o->id);
#endif
    for (i = 0; i < sizeof(o->theme_shape)/sizeof(o->theme_shape[0]); i++) {
	if (o->theme_shape[i].attr) {
	    TTFreeMem(o->theme_shape[i].attr);
	    o->theme_shape[i].attr = NULL;
	}
    }
}
static void twin_BuiltinRepaint_ttanybutton(ttanybutton o, dat x, dat y, dat w, dat h) {
    ttattr *t;
    dat tw, th;
    ttuint i;
    
    i = o->Class->CacheThemeShape(o);

    if (i != (ttuint)-1) {
	t = o->theme_shape[i].attr;
	tw = o->theme_shape[i].width;
	th = o->theme_shape[i].height;
	
	if (h+y > th)
	    h = th - y;
	if (w+x > tw)
	    w = tw - x;

	if (h > 0 && w > 0 && y < th && x < tw)
	    TwDrawHWAttrWidget(o->native,TT_MIN2(w,tw-x),TT_MIN2(h, th-y),x,y,tw,t + x + (y * tw));
    } else if (o->text && o->Class == TClass_ttanybutton) {
	if (h+y > o->text_height)
	    h = o->text_height - y;
	if (w+x > o->text_width)
	    w = o->text_width - x;

	if (h > 0 && w > 0 && y < o->text_height && x < o->text_width)
	    TwDrawHWAttrWidget(o->native,TT_MIN2(w,o->text_width-x),TT_MIN2(h, o->text_height-y),x,y,o->text_width,o->text + x + (y * o->text_width));
    } else
	TwDrawHWAttrWidget(o->native,w,h,x,y,w,NULL);
}
static void twin_MouseCallback_ttanybutton(tevent_any e, tobj id) {
    ttanybutton o = ID2(ttanybutton,id);
    ttevent ev;
    ttuint evcode;
    udat code;
    /*
     * libTw and libTT have the different encoding for mouse event codes,
     * but shiftflags are the same
     */
    if (o) {
	
	code = e->EventMouse.Code;
	evcode = 0;
	if (isMOVE(code)) {
	    evcode = ttevent_evcode_mouse_move;
	} else if (isPRESS(code)) {
	    switch (code & PRESS_ANY) {
	      case PRESS_LEFT:
		evcode = ttevent_evcode_mouse_press_1;
		break;
	      case PRESS_MIDDLE:
		evcode = ttevent_evcode_mouse_press_2;
		break;
	      case PRESS_RIGHT:
		evcode = ttevent_evcode_mouse_press_3;
		break;
#ifdef HOLD_WHEEL_REV
	      case PRESS_WHEEL_REV:
		evcode = ttevent_evcode_mouse_press_4;
		break;
#endif
#ifdef HOLD_WHEEL_FWD
	      case PRESS_WHEEL_FWD:
		evcode = ttevent_evcode_mouse_press_5;
		break;
#endif
	      default:
		/* unknown code! */
		return;
	    }
	} else if (isRELEASE(code)) {
	    switch (code & RELEASE_ANY) {
	      case RELEASE_LEFT:
		evcode = ttevent_evcode_mouse_release_1;
		break;
	      case RELEASE_MIDDLE:
		evcode = ttevent_evcode_mouse_release_2;
		break;
	      case RELEASE_RIGHT:
		evcode = ttevent_evcode_mouse_release_3;
		break;
#ifdef HOLD_WHEEL_REV
	      case RELEASE_WHEEL_REV:
		evcode = ttevent_evcode_mouse_release_4;
		break;
#endif
#ifdef HOLD_WHEEL_FWD
	      case RELEASE_WHEEL_FWD:
		evcode = ttevent_evcode_mouse_release_5;
		break;
#endif
	      default:
		/* unknown code! */
		return;
	    }
	} else {
	    /* unknown code! */
	    return;
	}
	
	if ((code & HOLD_ANY)) {
	    evcode |=
		(code & HOLD_LEFT      ? ttevent_evcode_mouse_hold_1 : 0) |
		(code & HOLD_MIDDLE    ? ttevent_evcode_mouse_hold_2 : 0) |
		(code & HOLD_RIGHT     ? ttevent_evcode_mouse_hold_3 : 0) |
#ifdef HOLD_WHEEL_REV
		(code & HOLD_WHEEL_REV ? ttevent_evcode_mouse_hold_4 : 0) |
#endif
#ifdef HOLD_WHEEL_FWD
		(code & HOLD_WHEEL_FWD ? ttevent_evcode_mouse_hold_5 : 0) |
#endif
		0;
	}

	if (evcode >= ttevent_evcode_mouse_first &&
	    evcode <= ttevent_evcode_mouse_last &&
	    (ev = (ttevent)Create4_tteventbig
	     (ttevent_evtype_mouse, evcode, e->EventMouse.ShiftFlags,
	      e->EventMouse.X, e->EventMouse.Y, 0, 0))) {
	
	    FireEvent(ev, (ttcomponent)o);
	    /* FireEvent also deletes ev */
	}
    }
}
static ttcallback twin_AddCommonCallbacks_ttanybutton(ttanybutton o) {
    ttcallback l[5];
    byte i = 0;
    void *id = (void *)OBJ2ID(o);
    
    if ((l[i] = twin_AddCommonCallbacks_ttwidget((ttwidget)o)) &&
	(i++, (l[i] = Create_ttcallback((ttcomponent)o))) &&
	(l[i]->native = (opaque)TwAddMouseListener(o->native,MOVE_MOUSE,0,(tfn_listener)twin_MouseCallback_ttanybutton,id)) &&
	(i++, (l[i] = Create_ttcallback((ttcomponent)o))) &&
	(l[i]->native = (opaque)TwAddMouseListener(o->native,PRESS_LEFT,0,(tfn_listener)twin_MouseCallback_ttanybutton,id)) &&
	(i++, (l[i] = Create_ttcallback((ttcomponent)o))) &&
	(l[i]->native = (opaque)TwAddMouseListener(o->native,MOVE_MOUSE|HOLD_LEFT,0,(tfn_listener)twin_MouseCallback_ttanybutton,id)) &&
	(i++, (l[i] = Create_ttcallback((ttcomponent)o))) &&
	(l[i]->native = (opaque)TwAddMouseListener(o->native,RELEASE_LEFT,0,(tfn_listener)twin_MouseCallback_ttanybutton,id))) {
	
	return l[i];
    }
    while (i) {
    	TDEL(l[i]);
	i--;
    }
    return NULL;
}
static ttanybutton twin_Build_ttanybutton(ttanybutton o) {
    /* calculate o->w and o->h using theme: */
    o->Class->CacheThemeShape(o);
    
    if (TW_NOID != (o->native = TwCreateWidget(o->w,o->h,TW_WIDGET_WANT_MOUSE|TW_WIDGET_WANT_MOUSE_MOTION|TW_WIDGET_AUTO_FOCUS,TW_WIDGETFL_USEEXPOSE,0,0,o->col))) {
	if (twin_AddCommonCallbacks_ttanybutton((ttanybutton)o)) {
	    twin_AtBuild_ttvisible((ttvisible)o);
	    return o;
	}
    }
    return NULL;
}


/* ttbutton */


/* ttcheckbutton */


/* ttradiobutton */


/* ttscrollbar */


/* ttslider */


/* ttscrollpane */


/* ttwindow */
static ttwindow twin_Build_ttwindow(ttwindow o) {
    if (TW_NOID != (o->native = TwCreateWindow(o->title ? TTLenStr(o->title) : 0,o->title,NULL,TW_NOID,o->col,TW_LINECURSOR,TW_WINDOW_DRAG|TW_WINDOW_WANT_CHANGES|TW_WINDOW_WANT_KEYS|
	  TW_WINDOW_WANT_MOUSE|TW_WINDOW_WANT_MOUSE_MOTION|TW_WIDGET_AUTO_FOCUS,TW_WINDOWFL_USEEXPOSE|TW_WINDOWFL_CURSOR_ON|TW_WINDOWFL_BORDERLESS,o->w,o->h,0))) {
	if (twin_AddCommonCallbacks_ttwidget((ttwidget)o)) {
	    twin_AtBuild_ttvisible((ttvisible)o);
	    return o;
	}
    }
    return NULL;
}


/* ttframe */
static void twin_AskCloseCallback_ttframe(tevent_any ev, tobj id) {
    ttframe o = ID2(ttframe,id);
    if (o)
	FireSimpleEvent((ttcomponent)o, ttevent_evtype_askclose);
}
static ttcallback twin_AddAskCloseCallback_ttframe(ttframe o) {
    ttcallback l;
    opaque id = OBJ2ID(o);
    
    if ((l = Create_ttcallback((ttcomponent)o))) {
	if ((l->native = (opaque)TwAddGadgetListener(o->native,0 /*CLOSE code*/,(tfn_listener)twin_AskCloseCallback_ttframe,(void *)id)))
	    return l;
	TDEL(l);
    }
    return NULL;
}
static ttframe twin_Build_ttframe(ttframe o) {
    /* ttframe are non-visible when created */
    o->vflags &= ~ttvisible_vflags_visible;
    
    if (TW_NOID != (o->native = TwCreateWindow(o->title ? TTLenStr(o->title) : 0,o->title,NULL,myMenubar_n(o),o->col,TW_LINECURSOR,TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_CLOSE|TW_WINDOW_WANT_CHANGES|
	  TW_WINDOW_WANT_KEYS|TW_WINDOW_WANT_MOUSE|TW_WINDOW_WANT_MOUSE_MOTION|TW_WIDGET_AUTO_FOCUS,TW_WINDOWFL_USEEXPOSE|TW_WINDOWFL_CURSOR_ON,o->w,o->h,0)) &&
	twin_AddCommonCallbacks_ttwidget((ttwidget)o) &&
	twin_AddAskCloseCallback_ttframe(o)) {

	twin_AtBuild_ttvisible((ttvisible)o);
	
	o->Class->AddTo(o, (ttvisible)TClass_ttnative->GetRoot(), 0);
	return o;
    }
    return NULL;
}
/*
 * HACK WARNING: libTw does not support non-visible
 * top-level widgets, so we emulate them with TwUnmapWidget().
 * This is why we override AddTo_ttframe() and SetVisible_ttframe()
 */
static void twin_AddTo_ttframe(ttframe o, ttvisible parent, ttany constraint) {
    if (parent && !o->parent) {
	/* prepend to list */
	if ((o->next = parent->child_first))
	    parent->child_first->prev = (ttvisible)o;
	else
	    parent->child_last = (ttvisible)o;
	parent->child_first = (ttvisible)o;
	o->prev = NULL;
	o->parent = parent;

	if (o->vflags & ttvisible_vflags_visible)
	    TwMapWidget(o->native,parent->native);
    }
}
static void twin_SetVisible_ttframe(ttframe o, byte on_off) {
    on_off = !!on_off;
    if (on_off != !!(o->vflags & ttvisible_vflags_visible)) {
	o->vflags ^= ttvisible_vflags_visible;
	if (o->parent) {
	    if (on_off)
		TwMapWidget(o->native,o->parent->native);
	    else
		TwUnMapWidget(o->native);
	}
    }
}
static ttbyte twin_SetTitle_ttframe(ttframe o, TT_ARG_READ ttbyte *title) {
    if (TClass_default(ttframe)->SetTitle(o, title)) {

	TwSetTitleWindow(o->native,o->title_len,o->title);

	return TT_TRUE;
    }
    return TT_FALSE;
}

static void twin_BuiltinRepaint_ttframe(ttframe o, dat x, dat y, dat w, dat h) {
    TwDrawHWAttrWidget(o->native,w,h,x,y,w,NULL);    
}


/* ttscroller */


/* ttmenuitem */
static void twin_AddTo_ttmenuitem(ttmenuitem o, ttvisible parent, ttany constraint) {
    /* append */
    tobj Code;
    
    if (parent && !o->parent && TTAssert(IS(ttmenu,parent)) &&
	(o->name || (o->name = TTCloneStr("")))) {

	/* hack warning: this is dirty */
	Code = OBJ2ID(o) + 1;
	while (Code && ((udat)Code == 0 || (udat)Code >= TW_COD_RESERVED))
	    Code >>= (8*sizeof(udat));
	if (!(udat)Code)
	    Code = 1 + (rand() & 0xFFF);
	o->native = TwRow4Menu(parent->target_private,Code,*o->name ? (o->vflags & ttanybutton_vflags_disabled) ?
	     TW_ROW_ACTIVE : TW_ROW_INACTIVE
	     : TW_ROW_IGNORE,strlen(o->name),o->name);
	
	TClass_default(ttmenuitem)->AddTo(o, parent, constraint);
    }
}
static void twin_Remove_ttmenuitem(ttmenuitem o) {
    if (o->parent) {
	TClass_default(ttmenuitem)->Remove(o);
	
	if (TW_NOID != o->native)
	    TwDeleteObj(o->native);
    }
}

/* ttcheckmenuitem */


/* ttradiomenuitem */


/* ttmenu */

static void twin_AddTo_ttmenu(ttmenu o, ttvisible parent, ttany constraint) {
    if (!o->parent && parent && TTAssert(IS(ttmenu,parent) || IS(ttmenubar,parent)) &&
	(o->name || (o->name = TTCloneStr("")))) {
	
	if (!o->menubar) {
	    if (IS(ttmenubar,parent))
		o->menubar = (ttmenubar)parent;
	    else if (IS(ttmenu,parent))
		o->menubar = ((ttmenu)parent)->menubar;
	}
	
	if (TW_NOID != (o->target_private = TwCreate4MenuWindow(myMenubar_n(o))) &&
	    TW_NOID != (o->native = TwItem4Menu(parent->native,o->target_private,!(o->vflags & ttanybutton_vflags_disabled),strlen(o->name),o->name))) {

	    TClass_default(ttmenu)->AddTo(o, parent, constraint);
	}
    }
}
static void twin_Remove_ttmenu(ttmenu o) {
    if (o->parent) {
	TClass_default(ttmenu)->Remove(o);
	
	if (o->target_private != TW_NOID) {
	    TwDeleteObj(o->target_private);
	    o->target_private = TW_NOID;
	}
	if (o->native != TW_NOID) {
	    TwDeleteObj(o->native);
	    o->native = TW_NOID;
	}
    }
}



/* ttmenubar */

static ttmenubar twin_Build_ttmenubar(ttmenubar o) {
    tttheme t = myTheme(o);
    if (TW_NOID != (o->native = TwCreateMenu(t->bg[tttheme_bg_menu_normal],t->bg[tttheme_bg_menu_select],t->bg[tttheme_bg_menu_disabled],t->bg[tttheme_bg_menu_selectdisabled],t->bg[tttheme_bg_menu_shcut],t->bg[tttheme_bg_menu_selectshcut],0))) {

	TwItem4MenuCommon(o->native);
	twin_AtBuild_ttvisible((ttvisible)o);
	    
	return o;
    }
    return NULL;
}


/* ttanytext */


/* tttextfield */


/* tttextarea */


/* tttheme */


/* ttapplication */

static ttapplication twin_Build_ttapplication(ttapplication o) {
    if (o->name && TW_NOID != (o->native = TwCreateMsgPort(strlen(o->name),o->name)))
	return o;
    return NULL;
}



static ttbyte twin_Sync(void) {
    return TwSync();
}
static ttbyte twin_Flush(void) {
    return TwFlush();
}
static ttbyte twin_TimidFlush(void) {
    return TwTimidFlush();
}
static ttbyte twin_MainLoopOnce(ttbyte wait) {
    tmsg Msg;
    
    /* loop until all already queued messages are dispatched */
    do {
        if ((Msg = TwCloneReadMsg(wait))) {
	    TwDispatchMsg(Msg);
	    TwFreeMem(Msg);
	}
    } while (TwPendingMsg());
    
    return !TwInPanic();
}
static void twin_DeleteCallback(ttcallback o) {
    if (o->native != (opaque)TW_NOID)
	TwDeleteListener((tlistener)(opaque)o->native);
}
static void twin_Close(void) {
    TwClose();
}
static int twin_ConnectionFd(void) {
    return TwConnectionFd();
}
static ttuint twin_GetErrno(void) {
    return TwErrno;
}
static ttuint twin_GetErrnoDetail(void) {
    return TwErrnoDetail;
}
static TT_CONST ttbyte *twin_StrError(ttuint E) {
    return TwStrError(E);
}
static TT_CONST ttbyte *twin_StrErrorDetail(ttuint E, ttuint S) {
    return TwStrErrorDetail(E,S);
}



TW_DECL_MAGIC(TT_Tw_magic);

#ifdef THIS_MODULE
ttclasses InitModule(tthw *HW)
#else
ttclasses _TT_twin_InitHW(tthw *HW)
#endif
{
    /*
     * (*HW) must be set even if initialization fails,
     * to report error messages.
     */
    *HW = &twin_TTClasses.HW;
    
    TwConfigMalloc(TTAllocMem,TTReallocMem,TTFreeMem);
    
    if (TwCheckMagic(TT_Tw_magic) && TwOpen(TTD.HWOptions))
        return &twin_TTClasses;

    FAIL(TwErrno, TwErrnoDetail);
    
    return (ttclasses)0;
}





/*
 * use default values for methods not implemented in twin_c
 *
 * null display target MUST IMPLEMENT ALL METHODS, even if as stubs.
 */











static s_ttclasses twin_TTClasses = {

 
  {
    Class_ttobj,
    id_ttobj,
    magicmask_ttobj,
    magic_ttobj,
    size_ttobj,

  },
 
  {
    Class_ttclass,
    id_ttclass,
    magicmask_ttclass,
    magic_ttclass,
    size_ttclass,

    /* extends ttclass_ttobj */

  },
 
  {
    Class_ttobject,
    id_ttobject,
    magicmask_ttobject,
    magic_ttobject,
    size_ttobject,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttobject */,
    (void *)NULL /* UNDEFINED: Build_ttobject */,
    twin_Break_ttobject,
    (void *)NULL /* UNDEFINED: Del_ttobject */,

  },
 
  {
    Class_ttvector,
    id_ttvector,
    magicmask_ttvector,
    magic_ttvector,
    size_ttvector,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttvector */,
    (void *)NULL /* UNDEFINED: Build_ttvector */,
    (void *)NULL /* inherits:  Break_ttvector >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttvector */,

    /* extends ttclass_ttobject */
    (void *)NULL /* UNDEFINED: GetAt_ttvector */,
    (void *)NULL /* UNDEFINED: SetAt_ttvector */,
    (void *)NULL /* UNDEFINED: AddY_ttvector */,
    (void *)NULL /* UNDEFINED: ContainsValue_ttvector */,
    (void *)NULL /* UNDEFINED: RemoveRange_ttvector */,

  },
 
  {
    Class_ttfield,
    id_ttfield,
    magicmask_ttfield,
    magic_ttfield,
    size_ttfield,

    /* extends ttclass_ttobj */

  },
 
  {
    Class_ttmethod,
    id_ttmethod,
    magicmask_ttmethod,
    magic_ttmethod,
    size_ttmethod,

    /* extends ttclass_ttobj */

  },
 
  {
    Class_ttevent,
    id_ttevent,
    magicmask_ttevent,
    magic_ttevent,
    size_ttevent,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttevent */,
    (void *)NULL /* UNDEFINED: Build_ttevent */,
    (void *)NULL /* inherits:  Break_ttevent >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttevent */,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_tteventbig,
    id_tteventbig,
    magicmask_tteventbig,
    magic_tteventbig,
    size_tteventbig,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tteventbig */,
    (void *)NULL /* UNDEFINED: Build_tteventbig */,
    (void *)NULL /* inherits:  Break_tteventbig >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tteventbig */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttevent */

  },
 
  {
    Class_ttbitmask,
    id_ttbitmask,
    magicmask_ttbitmask,
    magic_ttbitmask,
    size_ttbitmask,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttbitmask */,
    (void *)NULL /* UNDEFINED: Build_ttbitmask */,
    (void *)NULL /* inherits:  Break_ttbitmask >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttbitmask */,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_tteventmask,
    id_tteventmask,
    magicmask_tteventmask,
    magic_tteventmask,
    size_tteventmask,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tteventmask */,
    (void *)NULL /* UNDEFINED: Build_tteventmask */,
    (void *)NULL /* inherits:  Break_tteventmask >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tteventmask */,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_ttcallback,
    id_ttcallback,
    magicmask_ttcallback,
    magic_ttcallback,
    size_ttcallback,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttcallback */,
    (void *)NULL /* UNDEFINED: Build_ttcallback */,
    (void *)NULL /* inherits:  Break_ttcallback >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttcallback */,

    /* extends ttclass_ttobject */
    (void *)NULL /* UNDEFINED: Create_ttcallback */,

  },
 
  {
    Class_ttlistener,
    id_ttlistener,
    magicmask_ttlistener,
    magic_ttlistener,
    size_ttlistener,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttlistener */,
    (void *)NULL /* UNDEFINED: Build_ttlistener */,
    (void *)NULL /* inherits:  Break_ttlistener >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttlistener */,

    /* extends ttclass_ttobject */
    (void *)NULL /* UNDEFINED: Create_ttlistener */,

    /* extends ttclass_ttcallback */
    (void *)NULL /* UNDEFINED: AddTo_ttlistener */,
    (void *)NULL /* UNDEFINED: Remove_ttlistener */,

  },
 
  {
    Class_tttimer,
    id_tttimer,
    magicmask_tttimer,
    magic_tttimer,
    size_tttimer,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tttimer */,
    (void *)NULL /* UNDEFINED: Build_tttimer */,
    (void *)NULL /* inherits:  Break_tttimer >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tttimer */,

    /* extends ttclass_ttobject */
    (void *)NULL /* UNDEFINED: Create_tttimer */,

    /* extends ttclass_ttcallback */
    (void *)NULL /* UNDEFINED: AddTo_tttimer */,
    (void *)NULL /* UNDEFINED: Remove_tttimer */,

    /* extends ttclass_ttlistener */
    (void *)NULL /* UNDEFINED: SetEnabled_tttimer */,
    (void *)NULL /* UNDEFINED: IsEnabled_tttimer */,

  },
 
  {
    Class_ttcomponent,
    id_ttcomponent,
    magicmask_ttcomponent,
    magic_ttcomponent,
    size_ttcomponent,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttcomponent */,
    (void *)NULL /* UNDEFINED: Build_ttcomponent */,
    (void *)NULL /* inherits:  Break_ttcomponent >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttcomponent */,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_ttdata,
    id_ttdata,
    magicmask_ttdata,
    magic_ttdata,
    size_ttdata,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttdata */,
    (void *)NULL /* UNDEFINED: Build_ttdata */,
    (void *)NULL /* inherits:  Break_ttdata >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttdata */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttdata */,
    (void *)NULL /* UNDEFINED: Remove_ttdata */,

  },
 
  {
    Class_ttvisible,
    id_ttvisible,
    magicmask_ttvisible,
    magic_ttvisible,
    size_ttvisible,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttvisible */,
    (void *)NULL /* UNDEFINED: Build_ttvisible */,
    (void *)NULL /* inherits:  Break_ttvisible >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttvisible */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttvisible */,
    (void *)NULL /* UNDEFINED: Validate_ttvisible */,
    (void *)NULL /* UNDEFINED: Remove_ttvisible */,
    (void *)NULL /* UNDEFINED: Invalidate_ttvisible */,
    (void *)NULL /* UNDEFINED: SetVisible_ttvisible */,
    (void *)NULL /* UNDEFINED: SetTheme_ttvisible */,
    (void *)NULL /* UNDEFINED: Draw_ttvisible */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttvisible */,


  },
 
  {
    Class_ttlayout,
    id_ttlayout,
    magicmask_ttlayout,
    magic_ttlayout,
    size_ttlayout,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttlayout */,
    (void *)NULL /* UNDEFINED: Build_ttlayout */,
    (void *)NULL /* inherits:  Break_ttlayout >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttlayout */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: SetWidget_ttlayout */,
    (void *)NULL /* UNDEFINED: DoLayout_ttlayout */,

  },
 
  {
    Class_ttboxlayout,
    id_ttboxlayout,
    magicmask_ttboxlayout,
    magic_ttboxlayout,
    size_ttboxlayout,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttboxlayout */,
    (void *)NULL /* UNDEFINED: Build_ttboxlayout */,
    (void *)NULL /* inherits:  Break_ttboxlayout >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttboxlayout */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: SetWidget_ttboxlayout */,
    (void *)NULL /* UNDEFINED: DoLayout_ttboxlayout */,

    /* extends ttclass_ttlayout */

  },
 
  {
    Class_ttborderlayout,
    id_ttborderlayout,
    magicmask_ttborderlayout,
    magic_ttborderlayout,
    size_ttborderlayout,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttborderlayout */,
    (void *)NULL /* UNDEFINED: Build_ttborderlayout */,
    (void *)NULL /* inherits:  Break_ttborderlayout >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttborderlayout */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: SetWidget_ttborderlayout */,
    (void *)NULL /* UNDEFINED: DoLayout_ttborderlayout */,

    /* extends ttclass_ttlayout */

  },
 
  {
    Class_ttnative,
    id_ttnative,
    magicmask_ttnative,
    magic_ttnative,
    size_ttnative,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttnative */,
    (void *)NULL /* UNDEFINED: Build_ttnative */,
    twin_Break_ttnative,
    (void *)NULL /* UNDEFINED: Del_ttnative */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttnative */,
    (void *)NULL /* UNDEFINED: Validate_ttnative */,
    (void *)NULL /* UNDEFINED: Remove_ttnative */,
    (void *)NULL /* UNDEFINED: Invalidate_ttnative */,
    (void *)NULL /* UNDEFINED: SetVisible_ttnative */,
    (void *)NULL /* UNDEFINED: SetTheme_ttnative */,
    (void *)NULL /* UNDEFINED: Draw_ttnative */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttnative */,


    /* extends ttclass_ttvisible */
    twin_GetRoot_ttnative,
    twin_GetW_ttnative,
    twin_GetH_ttnative,

  },
 
  {
    Class_ttwidget,
    id_ttwidget,
    magicmask_ttwidget,
    magic_ttwidget,
    size_ttwidget,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttwidget */,
    twin_Build_ttwidget,
    (void *)NULL /* inherits:  Break_ttwidget >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttwidget */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    twin_AddTo_ttwidget,
    (void *)NULL /* UNDEFINED: Validate_ttwidget */,
    twin_Remove_ttwidget,
    twin_Invalidate_ttwidget,
    twin_SetVisible_ttwidget,
    (void *)NULL /* UNDEFINED: SetTheme_ttwidget */,
    twin_Draw_ttwidget,
    twin_BuiltinRepaint_ttwidget,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttwidget */,
    twin_SetXY_ttwidget,
    twin_SetWH_ttwidget,
    twin_SetXl_ttwidget,
    twin_SetYl_ttwidget,
    (void *)NULL /* UNDEFINED: SetWl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetHl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttwidget */,

  },
 
  {
    Class_ttlabel,
    id_ttlabel,
    magicmask_ttlabel,
    magic_ttlabel,
    size_ttlabel,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttlabel */,
    twin_Build_ttlabel,
    (void *)NULL /* inherits:  Break_ttlabel >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttlabel */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttlabel >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttlabel */,
    (void *)NULL /* inherits:  Remove_ttlabel >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttlabel >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttlabel >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttlabel */,
    (void *)NULL /* inherits:  Draw_ttlabel >> Draw_ttwidget */,
    twin_BuiltinRepaint_ttlabel,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttlabel */,
    (void *)NULL /* inherits:  SetXY_ttlabel >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttlabel >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttlabel >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttlabel >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttlabel */,
    (void *)NULL /* UNDEFINED: SetHl_ttlabel */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttlabel */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetFontD_ttlabel */,

  },
 
  {
    Class_tttooltip,
    id_tttooltip,
    magicmask_tttooltip,
    magic_tttooltip,
    size_tttooltip,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tttooltip */,
    (void *)NULL /* inherits:  Build_tttooltip >> Build_ttlabel */,
    (void *)NULL /* inherits:  Break_tttooltip >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tttooltip */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttooltip >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_tttooltip */,
    (void *)NULL /* inherits:  Remove_tttooltip >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_tttooltip >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_tttooltip >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttooltip */,
    (void *)NULL /* inherits:  Draw_tttooltip >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_tttooltip >> BuiltinRepaint_ttlabel */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_tttooltip */,
    (void *)NULL /* inherits:  SetXY_tttooltip >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_tttooltip >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_tttooltip >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_tttooltip >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_tttooltip */,
    (void *)NULL /* UNDEFINED: SetHl_tttooltip */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttooltip */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetFontD_tttooltip */,

    /* extends ttclass_ttlabel */
    (void *)NULL /* UNDEFINED: SetWidget_tttooltip */,

  },
 
  {
    Class_ttbuttongroup,
    id_ttbuttongroup,
    magicmask_ttbuttongroup,
    magic_ttbuttongroup,
    size_ttbuttongroup,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttbuttongroup */,
    (void *)NULL /* UNDEFINED: Build_ttbuttongroup */,
    (void *)NULL /* inherits:  Break_ttbuttongroup >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttbuttongroup */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: Add_ttbuttongroup */,
    (void *)NULL /* UNDEFINED: Remove_ttbuttongroup */,
    (void *)NULL /* UNDEFINED: SetChecked_ttbuttongroup */,

  },
 
  {
    Class_ttanybutton,
    id_ttanybutton,
    magicmask_ttanybutton,
    magic_ttanybutton,
    size_ttanybutton,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttanybutton */,
    twin_Build_ttanybutton,
    (void *)NULL /* inherits:  Break_ttanybutton >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttanybutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanybutton >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttanybutton */,
    (void *)NULL /* inherits:  Remove_ttanybutton >> Remove_ttwidget */,
    twin_Invalidate_ttanybutton,
    (void *)NULL /* inherits:  SetVisible_ttanybutton >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanybutton */,
    (void *)NULL /* inherits:  Draw_ttanybutton >> Draw_ttwidget */,
    twin_BuiltinRepaint_ttanybutton,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttanybutton */,
    (void *)NULL /* inherits:  SetXY_ttanybutton >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttanybutton >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttanybutton >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttanybutton >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetHl_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanybutton */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttanybutton */,

  },
 
  {
    Class_ttbutton,
    id_ttbutton,
    magicmask_ttbutton,
    magic_ttbutton,
    size_ttbutton,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttbutton */,
    (void *)NULL /* inherits:  Build_ttbutton >> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttbutton >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttbutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttbutton >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttbutton */,
    (void *)NULL /* inherits:  Remove_ttbutton >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttbutton >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttbutton >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttbutton */,
    (void *)NULL /* inherits:  Draw_ttbutton >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttbutton >> BuiltinRepaint_ttanybutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttbutton */,
    (void *)NULL /* inherits:  SetXY_ttbutton >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttbutton >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttbutton >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttbutton >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttbutton */,
    (void *)NULL /* UNDEFINED: SetHl_ttbutton */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttbutton */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttbutton */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttbutton */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttbutton */,

  },
 
  {
    Class_ttcheckbutton,
    id_ttcheckbutton,
    magicmask_ttcheckbutton,
    magic_ttcheckbutton,
    size_ttcheckbutton,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttcheckbutton */,
    (void *)NULL /* inherits:  Build_ttcheckbutton >> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttcheckbutton >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttcheckbutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttcheckbutton >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttcheckbutton */,
    (void *)NULL /* inherits:  Remove_ttcheckbutton >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttcheckbutton >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttcheckbutton >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttcheckbutton */,
    (void *)NULL /* inherits:  Draw_ttcheckbutton >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttcheckbutton >> BuiltinRepaint_ttanybutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttcheckbutton */,
    (void *)NULL /* inherits:  SetXY_ttcheckbutton >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttcheckbutton >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttcheckbutton >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttcheckbutton >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetHl_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttcheckbutton */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttcheckbutton */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttcheckbutton */,

    /* extends ttclass_ttbutton */
    (void *)NULL /* UNDEFINED: SetChecked_ttcheckbutton */,

  },
 
  {
    Class_ttradiobutton,
    id_ttradiobutton,
    magicmask_ttradiobutton,
    magic_ttradiobutton,
    size_ttradiobutton,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttradiobutton */,
    (void *)NULL /* inherits:  Build_ttradiobutton >> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttradiobutton >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttradiobutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttradiobutton >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttradiobutton */,
    (void *)NULL /* inherits:  Remove_ttradiobutton >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttradiobutton >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttradiobutton >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttradiobutton */,
    (void *)NULL /* inherits:  Draw_ttradiobutton >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttradiobutton >> BuiltinRepaint_ttanybutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttradiobutton */,
    (void *)NULL /* inherits:  SetXY_ttradiobutton >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttradiobutton >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttradiobutton >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttradiobutton >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetHl_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttradiobutton */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttradiobutton */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttradiobutton */,

    /* extends ttclass_ttbutton */
    (void *)NULL /* UNDEFINED: SetChecked_ttradiobutton */,

    /* extends ttclass_ttcheckbutton */

  },
 
  {
    Class_ttanyscroll,
    id_ttanyscroll,
    magicmask_ttanyscroll,
    magic_ttanyscroll,
    size_ttanyscroll,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttanyscroll */,
    (void *)NULL /* inherits:  Build_ttanyscroll >> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttanyscroll >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttanyscroll */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanyscroll >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttanyscroll */,
    (void *)NULL /* inherits:  Remove_ttanyscroll >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttanyscroll >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttanyscroll >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanyscroll */,
    (void *)NULL /* inherits:  Draw_ttanyscroll >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttanyscroll >> BuiltinRepaint_ttanybutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttanyscroll */,
    (void *)NULL /* inherits:  SetXY_ttanyscroll >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttanyscroll >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttanyscroll >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttanyscroll >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetHl_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanyscroll */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttanyscroll */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetState_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetSize_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetPosition_ttanyscroll */,

  },
 
  {
    Class_ttscrollbar,
    id_ttscrollbar,
    magicmask_ttscrollbar,
    magic_ttscrollbar,
    size_ttscrollbar,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttscrollbar */,
    (void *)NULL /* inherits:  Build_ttscrollbar >> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttscrollbar >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttscrollbar */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscrollbar >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttscrollbar */,
    (void *)NULL /* inherits:  Remove_ttscrollbar >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttscrollbar >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttscrollbar >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscrollbar */,
    (void *)NULL /* inherits:  Draw_ttscrollbar >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttscrollbar >> BuiltinRepaint_ttanybutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttscrollbar */,
    (void *)NULL /* inherits:  SetXY_ttscrollbar >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttscrollbar >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttscrollbar >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttscrollbar >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetHl_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscrollbar */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttscrollbar */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetState_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetSize_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetPosition_ttscrollbar */,

    /* extends ttclass_ttanyscroll */

  },
 
  {
    Class_ttslider,
    id_ttslider,
    magicmask_ttslider,
    magic_ttslider,
    size_ttslider,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttslider */,
    (void *)NULL /* inherits:  Build_ttslider >> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttslider >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttslider */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttslider >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttslider */,
    (void *)NULL /* inherits:  Remove_ttslider >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttslider >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttslider >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttslider */,
    (void *)NULL /* inherits:  Draw_ttslider >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttslider >> BuiltinRepaint_ttanybutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttslider */,
    (void *)NULL /* inherits:  SetXY_ttslider >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttslider >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttslider >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttslider >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttslider */,
    (void *)NULL /* UNDEFINED: SetHl_ttslider */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttslider */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttslider */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttslider */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttslider */,
    (void *)NULL /* UNDEFINED: SetState_ttslider */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttslider */,
    (void *)NULL /* UNDEFINED: SetSize_ttslider */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttslider */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttslider */,
    (void *)NULL /* UNDEFINED: SetPosition_ttslider */,

    /* extends ttclass_ttanyscroll */
    (void *)NULL /* UNDEFINED: SetSlideMin_ttslider */,
    (void *)NULL /* UNDEFINED: SetSlideMax_ttslider */,
    (void *)NULL /* UNDEFINED: SetSlideValue_ttslider */,

  },
 
  {
    Class_ttprogressbar,
    id_ttprogressbar,
    magicmask_ttprogressbar,
    magic_ttprogressbar,
    size_ttprogressbar,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttprogressbar */,
    (void *)NULL /* inherits:  Build_ttprogressbar >> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttprogressbar >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttprogressbar */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttprogressbar >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttprogressbar */,
    (void *)NULL /* inherits:  Remove_ttprogressbar >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttprogressbar >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttprogressbar >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttprogressbar */,
    (void *)NULL /* inherits:  Draw_ttprogressbar >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttprogressbar >> BuiltinRepaint_ttanybutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttprogressbar */,
    (void *)NULL /* inherits:  SetXY_ttprogressbar >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttprogressbar >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttprogressbar >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttprogressbar >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetHl_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttprogressbar */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttprogressbar */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetState_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetSize_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetPosition_ttprogressbar */,

    /* extends ttclass_ttanyscroll */

  },
 
  {
    Class_ttscrollpane,
    id_ttscrollpane,
    magicmask_ttscrollpane,
    magic_ttscrollpane,
    size_ttscrollpane,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttscrollpane */,
    (void *)NULL /* inherits:  Build_ttscrollpane >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttscrollpane >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttscrollpane */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscrollpane >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttscrollpane */,
    (void *)NULL /* inherits:  Remove_ttscrollpane >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttscrollpane >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttscrollpane >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscrollpane */,
    (void *)NULL /* inherits:  Draw_ttscrollpane >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttscrollpane >> BuiltinRepaint_ttwidget */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttscrollpane */,
    (void *)NULL /* inherits:  SetXY_ttscrollpane >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttscrollpane >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttscrollpane >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttscrollpane >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetHl_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscrollpane */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetBarX_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetBarY_ttscrollpane */,

  },
 
  {
    Class_ttwindow,
    id_ttwindow,
    magicmask_ttwindow,
    magic_ttwindow,
    size_ttwindow,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttwindow */,
    twin_Build_ttwindow,
    (void *)NULL /* inherits:  Break_ttwindow >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttwindow */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttwindow >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttwindow */,
    (void *)NULL /* inherits:  Remove_ttwindow >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttwindow >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttwindow >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttwindow */,
    (void *)NULL /* inherits:  Draw_ttwindow >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttwindow >> BuiltinRepaint_ttwidget */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttwindow */,
    (void *)NULL /* inherits:  SetXY_ttwindow >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttwindow >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttwindow >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttwindow >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttwindow */,
    (void *)NULL /* UNDEFINED: SetHl_ttwindow */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttwindow */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttwindow */,

  },
 
  {
    Class_ttframe,
    id_ttframe,
    magicmask_ttframe,
    magic_ttframe,
    size_ttframe,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttframe */,
    twin_Build_ttframe,
    (void *)NULL /* inherits:  Break_ttframe >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttframe */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    twin_AddTo_ttframe,
    (void *)NULL /* UNDEFINED: Validate_ttframe */,
    (void *)NULL /* inherits:  Remove_ttframe >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttframe >> Invalidate_ttwidget */,
    twin_SetVisible_ttframe,
    (void *)NULL /* UNDEFINED: SetTheme_ttframe */,
    (void *)NULL /* inherits:  Draw_ttframe >> Draw_ttwidget */,
    twin_BuiltinRepaint_ttframe,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttframe */,
    (void *)NULL /* inherits:  SetXY_ttframe >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttframe >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttframe >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttframe >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttframe */,
    (void *)NULL /* UNDEFINED: SetHl_ttframe */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttframe */,

    /* extends ttclass_ttwidget */
    twin_SetTitle_ttframe,

    /* extends ttclass_ttwindow */
    (void *)NULL /* UNDEFINED: SetMenubar_ttframe */,

  },
 
  {
    Class_ttscroller,
    id_ttscroller,
    magicmask_ttscroller,
    magic_ttscroller,
    size_ttscroller,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttscroller */,
    (void *)NULL /* inherits:  Build_ttscroller >> Build_ttwindow */,
    (void *)NULL /* inherits:  Break_ttscroller >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttscroller */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscroller >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttscroller */,
    (void *)NULL /* inherits:  Remove_ttscroller >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttscroller >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttscroller >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscroller */,
    (void *)NULL /* inherits:  Draw_ttscroller >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttscroller >> BuiltinRepaint_ttwidget */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttscroller */,
    (void *)NULL /* inherits:  SetXY_ttscroller >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttscroller >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttscroller >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttscroller >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttscroller */,
    (void *)NULL /* UNDEFINED: SetHl_ttscroller */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscroller */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttscroller */,

    /* extends ttclass_ttwindow */

  },
 
  {
    Class_ttmenuitem,
    id_ttmenuitem,
    magicmask_ttmenuitem,
    magic_ttmenuitem,
    size_ttmenuitem,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Build_ttmenuitem */,
    (void *)NULL /* inherits:  Break_ttmenuitem >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttmenuitem */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    twin_AddTo_ttmenuitem,
    (void *)NULL /* UNDEFINED: Validate_ttmenuitem */,
    twin_Remove_ttmenuitem,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenuitem */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenuitem */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Draw_ttmenuitem */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenuitem */,


    /* extends ttclass_ttvisible */

  },
 
  {
    Class_ttcheckmenuitem,
    id_ttcheckmenuitem,
    magicmask_ttcheckmenuitem,
    magic_ttcheckmenuitem,
    size_ttcheckmenuitem,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: Build_ttcheckmenuitem */,
    (void *)NULL /* inherits:  Break_ttcheckmenuitem >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttcheckmenuitem */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttcheckmenuitem >> AddTo_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Validate_ttcheckmenuitem */,
    (void *)NULL /* inherits:  Remove_ttcheckmenuitem >> Remove_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Invalidate_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: SetVisible_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: SetTheme_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: Draw_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttcheckmenuitem */,


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

  },
 
  {
    Class_ttradiomenuitem,
    id_ttradiomenuitem,
    magicmask_ttradiomenuitem,
    magic_ttradiomenuitem,
    size_ttradiomenuitem,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: Build_ttradiomenuitem */,
    (void *)NULL /* inherits:  Break_ttradiomenuitem >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttradiomenuitem */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttradiomenuitem >> AddTo_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Validate_ttradiomenuitem */,
    (void *)NULL /* inherits:  Remove_ttradiomenuitem >> Remove_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Invalidate_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: SetVisible_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: SetTheme_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: Draw_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttradiomenuitem */,


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

    /* extends ttclass_ttcheckmenuitem */

  },
 
  {
    Class_ttmenu,
    id_ttmenu,
    magicmask_ttmenu,
    magic_ttmenu,
    size_ttmenu,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttmenu */,
    (void *)NULL /* UNDEFINED: Build_ttmenu */,
    (void *)NULL /* inherits:  Break_ttmenu >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttmenu */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    twin_AddTo_ttmenu,
    (void *)NULL /* UNDEFINED: Validate_ttmenu */,
    twin_Remove_ttmenu,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenu */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenu */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenu */,
    (void *)NULL /* UNDEFINED: Draw_ttmenu */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenu */,


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

  },
 
  {
    Class_ttmenubar,
    id_ttmenubar,
    magicmask_ttmenubar,
    magic_ttmenubar,
    size_ttmenubar,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttmenubar */,
    twin_Build_ttmenubar,
    (void *)NULL /* inherits:  Break_ttmenubar >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttmenubar */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttmenubar */,
    (void *)NULL /* UNDEFINED: Validate_ttmenubar */,
    (void *)NULL /* UNDEFINED: Remove_ttmenubar */,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenubar */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenubar */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenubar */,
    (void *)NULL /* UNDEFINED: Draw_ttmenubar */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenubar */,


    /* extends ttclass_ttvisible */

  },
 
  {
    Class_ttanytext,
    id_ttanytext,
    magicmask_ttanytext,
    magic_ttanytext,
    size_ttanytext,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttanytext */,
    (void *)NULL /* inherits:  Build_ttanytext >> Build_ttwindow */,
    (void *)NULL /* inherits:  Break_ttanytext >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttanytext */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanytext >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttanytext */,
    (void *)NULL /* inherits:  Remove_ttanytext >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttanytext >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttanytext >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanytext */,
    (void *)NULL /* inherits:  Draw_ttanytext >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttanytext >> BuiltinRepaint_ttwidget */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttanytext */,
    (void *)NULL /* inherits:  SetXY_ttanytext >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttanytext >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttanytext >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttanytext >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttanytext */,
    (void *)NULL /* UNDEFINED: SetHl_ttanytext */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanytext */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttanytext */,

    /* extends ttclass_ttwindow */

  },
 
  {
    Class_tttextfield,
    id_tttextfield,
    magicmask_tttextfield,
    magic_tttextfield,
    size_tttextfield,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tttextfield */,
    (void *)NULL /* inherits:  Build_tttextfield >> Build_ttwindow */,
    (void *)NULL /* inherits:  Break_tttextfield >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tttextfield */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttextfield >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_tttextfield */,
    (void *)NULL /* inherits:  Remove_tttextfield >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_tttextfield >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_tttextfield >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttextfield */,
    (void *)NULL /* inherits:  Draw_tttextfield >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_tttextfield >> BuiltinRepaint_ttwidget */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_tttextfield */,
    (void *)NULL /* inherits:  SetXY_tttextfield >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_tttextfield >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_tttextfield >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_tttextfield >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_tttextfield */,
    (void *)NULL /* UNDEFINED: SetHl_tttextfield */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttextfield */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_tttextfield */,

    /* extends ttclass_ttwindow */

    /* extends ttclass_ttanytext */

  },
 
  {
    Class_tttextarea,
    id_tttextarea,
    magicmask_tttextarea,
    magic_tttextarea,
    size_tttextarea,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tttextarea */,
    (void *)NULL /* inherits:  Build_tttextarea >> Build_ttwindow */,
    (void *)NULL /* inherits:  Break_tttextarea >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tttextarea */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttextarea >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_tttextarea */,
    (void *)NULL /* inherits:  Remove_tttextarea >> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_tttextarea >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_tttextarea >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttextarea */,
    (void *)NULL /* inherits:  Draw_tttextarea >> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_tttextarea >> BuiltinRepaint_ttwidget */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_tttextarea */,
    (void *)NULL /* inherits:  SetXY_tttextarea >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_tttextarea >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_tttextarea >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_tttextarea >> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_tttextarea */,
    (void *)NULL /* UNDEFINED: SetHl_tttextarea */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttextarea */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_tttextarea */,

    /* extends ttclass_ttwindow */

    /* extends ttclass_ttanytext */

    /* extends ttclass_tttextfield */

  },
 
  {
    Class_tttheme,
    id_tttheme,
    magicmask_tttheme,
    magic_tttheme,
    size_tttheme,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tttheme */,
    (void *)NULL /* UNDEFINED: Build_tttheme */,
    (void *)NULL /* inherits:  Break_tttheme >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tttheme */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */

  },
 
  {
    Class_ttapplication,
    id_ttapplication,
    magicmask_ttapplication,
    magic_ttapplication,
    size_ttapplication,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttapplication */,
    twin_Build_ttapplication,
    (void *)NULL /* inherits:  Break_ttapplication >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttapplication */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */

  },



  {

    twin_Sync, 
    twin_Flush, 
    twin_TimidFlush, 
    twin_MainLoopOnce, 
    twin_DeleteCallback,    
    twin_Close, 
    twin_ConnectionFd, 
    twin_GetErrno, 
    twin_GetErrnoDetail,    
    twin_StrError, 
    twin_StrErrorDetail,   
  },
};





