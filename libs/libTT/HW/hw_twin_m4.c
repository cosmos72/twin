




/* This file was automatically generated from hw_twin_c by m4/hw.m4h, do not edit! */


/*
 *  hw_twin_m4.c  --  implementation of libTT methods on "twin" target display
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
 * this file is hw_twin_c and is preprocessed by m4 to produce hw_twin_m4.c
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

/* include our internal copy of TT.h instead of the  one */
#include "TT.h"
#include <TT/TTerrno.h>

#include "mutex.h"

#include "TTextern.h"
#include "TTassert.h"

#include "utils.h"
#include "inlines.h"
#include "seterrno.h"
#include "theme.h"

static s_ttfns twin_TTFNs;



/*
 * We need to write down only overloaded methods.
 */
 


/* ttobj */
static void twin_Break_ttobj(ttobj o) {
    if (o) {
	if (o->native != TW_NOID) {
	    TwDeleteObj(o->native);
	    o->native = TW_NOID;
	}
	/* call null_Break_ttobj: */
	TFNdefault(ttobj)->Break(o);
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
	    TwMapWidget(child_native, native);
	child = child->next;
    }
}

/* ttnative */
static ttnative twin_GetRoot_ttnative(void) {
    return Create_ttnative(TwFirstScreen());
}
static void twin_Break_ttnative(ttnative o) {
    o->native = TW_NOID;
    TFNdefault(ttnative)->Break(o);
}
static ttshort twin_GetW_ttnative(ttnative o) {
    ttnative r = TFN_ttnative->GetRoot();
    if (r && r->native == o->native)
	return TwGetDisplayWidth();
    return TwStat(o->native, TWS_widget_Width);
}
static ttshort twin_GetH_ttnative(ttnative o) {
    ttnative r = TFN_ttnative->GetRoot();
    if (r && r->native == o->native)
	return TwGetDisplayHeight();
    return TwStat(o->native, TWS_widget_Height);
}


/* ttwidget */
static void twin_BuiltinRepaint_ttwidget(ttwidget o, dat x, dat y, dat w, dat h) {
    TwDrawHWAttrWidget(o->native, w, h, x, y, w, NULL);
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
	(l[i]->native = (opaque)TwAddWidgetListener
	 (o->native, TW_MSG_WIDGET_EXPOSE,
	  (tfn_listener)twin_ExposeCallback_ttwidget, id)) &&
	(i++, (l[i] = Create_ttcallback((ttcomponent)o))) &&
	(l[i]->native = (opaque)TwAddWidgetListener
	 (o->native, TW_MSG_WIDGET_RESIZE,
	  (tfn_listener)twin_ResizeCallback_ttwidget, id))) {

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
    if (TW_NOID != (o->native = TwCreateWidget(o->w, o->h, 0, TW_WIDGETFL_USEFILL, 0, MAXDAT, HWATTR(c, f)))) {
	twin_AtBuild_ttvisible((ttvisible)o);
	return o;
    }
    return NULL;
}
static void twin_AddTo_ttwidget(ttwidget o, ttvisible parent) {
    if (parent && !o->parent) {
	TFNdefault(ttwidget)->AddTo(o, parent);
	
	if (o->native != TW_NOID && parent->native != TW_NOID)
	    TwMapWidget(o->native, parent->native);
    }
}
static void twin_SetVisible_ttwidget(ttwidget o, ttbyte visible) {
    ttuint isvisible, wasvisibile;
    
    wasvisibile = o->vflags & ttvisible_vflags_visible;
    
    TFNdefault(ttwidget)->SetVisible(o, visible);
    
    isvisible = o->vflags & ttvisible_vflags_visible;
    
    if (isvisible != wasvisibile && o->native != TW_NOID)
	TwSetVisibleWidget(o->native, visible);
}
static void twin_Remove_ttwidget(ttwidget o) {
    if (o->parent) {
	TFNdefault(ttwidget)->Remove(o);
	
	if (o->native != TW_NOID)
	    TwUnMapWidget(o->native);
    }
}
static void twin_SetXY_ttwidget(ttwidget o, ttshort x, ttshort y) {
    TwSetXYWidget(o->native, o->x = x, o->y = y);
}
static void twin_SetWH_ttwidget(ttwidget o, ttshort w, ttshort h) {
    TwResizeWidget(o->native, o->w = w, o->h = h);
}
static void twin_SetXl_ttwidget(ttwidget o, ttint xl) {
    TwScrollWidget(o->native, xl - o->xl, 0);
    o->xl = xl;
}
static void twin_SetYl_ttwidget(ttwidget o, ttint yl) {
    TwScrollWidget(o->native, 0, yl - o->yl);
    o->yl = yl;
}
static void twin_Draw_ttwidget(ttwidget o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch,
			       TT_CONST ttbyte *asciidata, TT_CONST ttfont *fontdata, TT_CONST ttattr *attrdata) {
    TwDraw2Widget(o->native, w, h, x, y, pitch, asciidata, fontdata, attrdata);
}


/* ttlabel */
static ttlabel twin_Build_ttlabel(ttlabel o) {
    if (TW_NOID != (o->native = TwCreateWidget(o->text_len, 1, 0, TW_WIDGETFL_USEEXPOSE, 0, MAXDAT, HWATTR(o->col, myTheme(o)->fill)))) {
	if (twin_AddCommonCallbacks_ttwidget((ttwidget)o)) {
	    twin_AtBuild_ttvisible((ttvisible)o);
	    return o;
	}
    }
    return NULL;
}
static void twin_Invalidate_ttwidget(ttwidget o) {
    tttheme t;
    
    TFNdefault(ttwidget)->Invalidate(o);
    
    t = myTheme(o);
    
    TwSetFillWidget(o->native, HWATTR(o->col, t->fill));
}

/* force inheritance from Draw_ttwidget() (m4 trick: twin_Draw_ttlabel); */
#define twin_Draw_ttlabel (void *)twin_Draw_ttwidget

static void twin_BuiltinRepaint_ttlabel(ttlabel o, dat x, dat y, dat w, dat h) {
    if (y == 0) {
	if (x < o->text_len)
	    TwDrawHWFontWidget(o->native, TT_MIN2(w,o->text_len-x), 1, x, 0, o->text_len, o->text+x);
	if (x+w > o->text_len)
	    TwDrawHWFontWidget(o->native, o->text_len-x-w, 1, o->text_len, 0, 0, NULL);
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
    
    i = o->FN->CacheThemeShape(o);

    if (i != (ttuint)-1) {
	t = o->theme_shape[i].attr;
	tw = o->theme_shape[i].width;
	th = o->theme_shape[i].height;
	
	if (h+y > th)
	    h = th - y;
	if (w+x > tw)
	    w = tw - x;

	if (h > 0 && w > 0 && y < th && x < tw)
	    TwDrawHWAttrWidget(o->native, TT_MIN2(w,tw-x), TT_MIN2(h, th-y),
				 x, y, tw, t + x + (y * tw));
    } else if (o->text && o->FN == TFN_ttanybutton) {
	if (h+y > o->text_height)
	    h = o->text_height - y;
	if (w+x > o->text_width)
	    w = o->text_width - x;

	if (h > 0 && w > 0 && y < o->text_height && x < o->text_width)
	    TwDrawHWAttrWidget(o->native, TT_MIN2(w,o->text_width-x), TT_MIN2(h, o->text_height-y),
				  x, y, o->text_width, o->text + x + (y * o->text_width));
    } else
	TwDrawHWAttrWidget(o->native, w, h, x, y, w, NULL);
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
	if (isMOTION(code) || isDRAG(code)) {
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
	(l[i]->native = (opaque)TwAddMouseListener
	 (o->native, MOTION_MOUSE, 0, 
	  (tfn_listener)twin_MouseCallback_ttanybutton, id)) &&
	(i++, (l[i] = Create_ttcallback((ttcomponent)o))) &&
	(l[i]->native = (opaque)TwAddMouseListener
	 (o->native, PRESS_LEFT, 0,
	  (tfn_listener)twin_MouseCallback_ttanybutton, id)) &&
	(i++, (l[i] = Create_ttcallback((ttcomponent)o))) &&
	(l[i]->native = (opaque)TwAddMouseListener
	 (o->native, DRAG_MOUSE|HOLD_LEFT, 0,
	  (tfn_listener)twin_MouseCallback_ttanybutton, id)) &&
	(i++, (l[i] = Create_ttcallback((ttcomponent)o))) &&
	(l[i]->native = (opaque)TwAddMouseListener
	 (o->native, RELEASE_LEFT, 0,
	  (tfn_listener)twin_MouseCallback_ttanybutton, id))) {
	
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
    o->FN->CacheThemeShape(o);
    
    if (TW_NOID != (o->native = TwCreateWidget
		    (o->w, o->h, TW_WIDGET_WANT_MOUSE|TW_WIDGET_WANT_MOUSE_MOTION|TW_WIDGET_AUTO_FOCUS,
		     TW_WIDGETFL_USEEXPOSE, 0, 0, o->col))) {
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
    if (TW_NOID != (o->native = TwCreateWindow
	 (o->title ? TTLenStr(o->title) : 0, o->title, NULL, TW_NOID, o->col, TW_LINECURSOR,
	  TW_WINDOW_DRAG|TW_WINDOW_WANT_CHANGES|TW_WINDOW_WANT_KEYS|
	  TW_WINDOW_WANT_MOUSE|TW_WINDOW_WANT_MOUSE_MOTION|TW_WIDGET_AUTO_FOCUS,
	  TW_WINDOWFL_USEEXPOSE|TW_WINDOWFL_CURSOR_ON|TW_WINDOWFL_BORDERLESS, o->w, o->h, 0))) {
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
	if ((l->native = (opaque)TwAddGadgetListener
	     (o->native, 0 /*CLOSE code*/, (tfn_listener)twin_AskCloseCallback_ttframe, (void *)id)))
	    return l;
	TDEL(l);
    }
    return NULL;
}
static ttframe twin_Build_ttframe(ttframe o) {
    /* ttframe are non-visible when created */
    o->vflags &= ~ttvisible_vflags_visible;
    
    if (TW_NOID != (o->native = TwCreateWindow
	 (o->title ? TTLenStr(o->title) : 0, o->title, NULL, myMenubar_n(o), o->col, TW_LINECURSOR,
	  TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_CLOSE|TW_WINDOW_WANT_CHANGES|
	  TW_WINDOW_WANT_KEYS|TW_WINDOW_WANT_MOUSE|TW_WINDOW_WANT_MOUSE_MOTION|TW_WIDGET_AUTO_FOCUS,
	  TW_WINDOWFL_USEEXPOSE|TW_WINDOWFL_CURSOR_ON, o->w, o->h, 0)) &&
	twin_AddCommonCallbacks_ttwidget((ttwidget)o) &&
	twin_AddAskCloseCallback_ttframe(o)) {

	twin_AtBuild_ttvisible((ttvisible)o);
	
	o->FN->AddTo(o, (ttvisible)TFN_ttnative->GetRoot());
	return o;
    }
    return NULL;
}
/*
 * HACK WARNING: libTw does not support non-visible
 * top-level widgets, so we emulate them with TwUnmapWidget().
 * This is why we override AddTo_ttframe() and SetVisible_ttframe()
 */
static void twin_AddTo_ttframe(ttframe o, ttvisible parent) {
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
	    TwMapWidget(o->native, parent->native);
    }
}
static void twin_SetVisible_ttframe(ttframe o, byte on_off) {
    on_off = !!on_off;
    if (on_off != !!(o->vflags & ttvisible_vflags_visible)) {
	o->vflags ^= ttvisible_vflags_visible;
	if (o->parent) {
	    if (on_off)
		TwMapWidget(o->native, o->parent->native);
	    else
		TwUnMapWidget(o->native);
	}
    }
}
static ttbyte twin_SetTitle_ttframe(ttframe o, TT_ARG_READ ttbyte *title) {
    if (TFNdefault(ttframe)->SetTitle(o, title)) {

	TwSetTitleWindow(o->native, o->title_len, o->title);

	return TT_TRUE;
    }
    return TT_FALSE;
}

static void twin_BuiltinRepaint_ttframe(ttframe o, dat x, dat y, dat w, dat h) {
    TwDrawHWAttrWidget(o->native, w, h, x, y, w, NULL);    
}


/* ttscroller */


/* ttmenuitem */
static void twin_AddTo_ttmenuitem(ttmenuitem o, ttvisible parent) {
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
	o->native = TwRow4Menu
	    (parent->target_private, Code, 
	     *o->name ? (o->vflags & ttanybutton_vflags_disabled) ?
	     TW_ROW_ACTIVE : TW_ROW_INACTIVE
	     : TW_ROW_IGNORE, strlen(o->name), o->name);
	
	TFNdefault(ttmenuitem)->AddTo(o, parent);
    }
}
static void twin_Remove_ttmenuitem(ttmenuitem o) {
    if (o->parent) {
	TFNdefault(ttmenuitem)->Remove(o);
	
	if (TW_NOID != o->native)
	    TwDeleteObj(o->native);
    }
}

/* ttcheckmenuitem */


/* ttradiomenuitem */


/* ttmenu */

static void twin_AddTo_ttmenu(ttmenu o, ttvisible parent) {
    if (!o->parent && parent && TTAssert(IS(ttmenu,parent) || IS(ttmenubar,parent)) &&
	(o->name || (o->name = TTCloneStr("")))) {
	
	if (!o->menubar) {
	    if (IS(ttmenubar,parent))
		o->menubar = (ttmenubar)parent;
	    else if (IS(ttmenu,parent))
		o->menubar = ((ttmenu)parent)->menubar;
	}
	
	if (TW_NOID != (o->target_private = TwCreate4MenuWindow(myMenubar_n(o))) &&
	    TW_NOID != (o->native = TwItem4Menu(parent->native, o->target_private,
						  !(o->vflags & ttanybutton_vflags_disabled),
						  strlen(o->name), o->name))) {

	    TFNdefault(ttmenu)->AddTo(o, parent);
	}
    }
}
static void twin_Remove_ttmenu(ttmenu o) {
    if (o->parent) {
	TFNdefault(ttmenu)->Remove(o);
	
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
    if (TW_NOID != (o->native = TwCreateMenu
		    (t->bg[tttheme_bg_menu_normal], t->bg[tttheme_bg_menu_select],
		     t->bg[tttheme_bg_menu_disabled], t->bg[tttheme_bg_menu_selectdisabled],
		     t->bg[tttheme_bg_menu_shcut], t->bg[tttheme_bg_menu_selectshcut], 0))) {

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
    if (o->name && TW_NOID != (o->native = TwCreateMsgPort(strlen(o->name), o->name, (time_t)0, (frac_t)0, 0)))
	return o;
    return NULL;
}


static byte twin_Sync(void) {
    return TwSync();
}
static byte twin_Flush(void) {
    return TwFlush();
}
static byte twin_TimidFlush(void) {
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
static TT_CONST byte *twin_StrError(ttuint E) {
    return TwStrError(E);
}
static TT_CONST byte *twin_StrErrorDetail(ttuint E, ttuint S) {
    return TwStrErrorDetail(E, S);
}


TW_DECL_MAGIC(TT_Tw_magic);

#ifdef THIS_MODULE
ttfns InitModule(tthw *HW)
#else
ttfns _TT_twin_InitHW(tthw *HW)
#endif
{
    /*
     * (*HW) must be set even if initialization fails,
     * to report error messages.
     */
    *HW = &twin_TTFNs.HW;
    
    TwConfigMalloc(TTAllocMem, TTReallocMem, TTFreeMem);
    
    if (TwCheckMagic(TT_Tw_magic) && TwOpen(TTD.HWOptions))
        return &twin_TTFNs;

    FAIL(TwErrno, TwErrnoDetail);
    
    return (ttfns)0;
}




                                      


/*
 * use default values for methods not implemented in hw_twin_c
 *
 * null display target MUST IMPLEMENT ALL METHODS, even if as stubs.
 */



static s_ttfns twin_TTFNs = {


  {
    order_ttobj,
    magicmask_ttobj,
    magic_ttobj,
    sizeof(s_ttobj),
    (void *)NULL /* UNDEFINED: New_ttobj  */,
    (void *)NULL /* UNDEFINED: Build_ttobj  */,
    twin_Break_ttobj,
    (void *)NULL /* UNDEFINED: Del_ttobj  */,

  },
  
  {
    order_ttevent,
    magicmask_ttevent,
    magic_ttevent,
    sizeof(s_ttevent),
    (void *)NULL /* UNDEFINED: New_ttevent  */,
    (void *)NULL /* UNDEFINED: Build_ttevent  */,
    (void *)NULL /* inherits:  Break_ttevent -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttevent  */,

    /* extends ttfn_ttobj */

  },
 
  {
    order_tteventbig,
    magicmask_tteventbig,
    magic_tteventbig,
    sizeof(s_tteventbig),
    (void *)NULL /* UNDEFINED: New_tteventbig  */,
    (void *)NULL /* UNDEFINED: Build_tteventbig  */,
    (void *)NULL /* inherits:  Break_tteventbig -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tteventbig  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttevent */


  },
 
  {
    order_ttbitmask,
    magicmask_ttbitmask,
    magic_ttbitmask,
    sizeof(s_ttbitmask),
    (void *)NULL /* UNDEFINED: New_ttbitmask  */,
    (void *)NULL /* UNDEFINED: Build_ttbitmask  */,
    (void *)NULL /* inherits:  Break_ttbitmask -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttbitmask  */,

    /* extends ttfn_ttobj */

  },
 
  {
    order_tteventmask,
    magicmask_tteventmask,
    magic_tteventmask,
    sizeof(s_tteventmask),
    (void *)NULL /* UNDEFINED: New_tteventmask  */,
    (void *)NULL /* UNDEFINED: Build_tteventmask  */,
    (void *)NULL /* inherits:  Break_tteventmask -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tteventmask  */,

    /* extends ttfn_ttobj */

  },
 
  {
    order_ttcallback,
    magicmask_ttcallback,
    magic_ttcallback,
    sizeof(s_ttcallback),
    (void *)NULL /* UNDEFINED: New_ttcallback  */,
    (void *)NULL /* UNDEFINED: Build_ttcallback  */,
    (void *)NULL /* inherits:  Break_ttcallback -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttcallback  */,

    /* extends ttfn_ttobj */
    (void *)NULL /* UNDEFINED: Create_ttcallback  */,

  },
 
  {
    order_ttlistener,
    magicmask_ttlistener,
    magic_ttlistener,
    sizeof(s_ttlistener),
    (void *)NULL /* UNDEFINED: New_ttlistener  */,
    (void *)NULL /* UNDEFINED: Build_ttlistener  */,
    (void *)NULL /* inherits:  Break_ttlistener -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttlistener  */,

    /* extends ttfn_ttobj */
    (void *)NULL /* UNDEFINED: Create_ttlistener  */,

    /* extends ttfn_ttcallback */


    (void *)NULL /* UNDEFINED: AddTo_ttlistener  */,
    (void *)NULL /* UNDEFINED: Remove_ttlistener  */,

  },
 
  {
    order_tttimer,
    magicmask_tttimer,
    magic_tttimer,
    sizeof(s_tttimer),
    (void *)NULL /* UNDEFINED: New_tttimer  */,
    (void *)NULL /* UNDEFINED: Build_tttimer  */,
    (void *)NULL /* inherits:  Break_tttimer -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tttimer  */,

    /* extends ttfn_ttobj */
    (void *)NULL /* UNDEFINED: Create_tttimer  */,

    /* extends ttfn_ttcallback */


    (void *)NULL /* UNDEFINED: AddTo_tttimer  */,
    (void *)NULL /* UNDEFINED: Remove_tttimer  */,

    /* extends ttfn_ttlistener */
    (void *)NULL /* UNDEFINED: SetEnabled_tttimer  */,
    (void *)NULL /* UNDEFINED: IsEnabled_tttimer  */,

  },
 
  {
    order_ttcomponent,
    magicmask_ttcomponent,
    magic_ttcomponent,
    sizeof(s_ttcomponent),
    (void *)NULL /* UNDEFINED: New_ttcomponent  */,
    (void *)NULL /* UNDEFINED: Build_ttcomponent  */,
    (void *)NULL /* inherits:  Break_ttcomponent -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttcomponent  */,

    /* extends ttfn_ttobj */

  },
 
  {
    order_ttdata,
    magicmask_ttdata,
    magic_ttdata,
    sizeof(s_ttdata),
    (void *)NULL /* UNDEFINED: New_ttdata  */,
    (void *)NULL /* UNDEFINED: Build_ttdata  */,
    (void *)NULL /* inherits:  Break_ttdata -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttdata  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttdata  */,
    (void *)NULL /* UNDEFINED: Remove_ttdata  */,

  },
 
  {
    order_ttvisible,
    magicmask_ttvisible,
    magic_ttvisible,
    sizeof(s_ttvisible),
    (void *)NULL /* UNDEFINED: New_ttvisible  */,
    (void *)NULL /* UNDEFINED: Build_ttvisible  */,
    (void *)NULL /* inherits:  Break_ttvisible -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttvisible  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttvisible  */,
    (void *)NULL /* UNDEFINED: Remove_ttvisible  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttvisible  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttvisible  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttvisible  */,
    (void *)NULL /* UNDEFINED: Draw_ttvisible  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttvisible  */,

  },
 
  {
    order_ttnative,
    magicmask_ttnative,
    magic_ttnative,
    sizeof(s_ttnative),
    (void *)NULL /* UNDEFINED: New_ttnative  */,
    (void *)NULL /* UNDEFINED: Build_ttnative  */,
    twin_Break_ttnative,
    (void *)NULL /* UNDEFINED: Del_ttnative  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttnative  */,
    (void *)NULL /* UNDEFINED: Remove_ttnative  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttnative  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttnative  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttnative  */,
    (void *)NULL /* UNDEFINED: Draw_ttnative  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttnative  */,

    /* extends ttfn_ttvisible */
    twin_GetRoot_ttnative,
    twin_GetW_ttnative,
    twin_GetH_ttnative,

  },
 
  {
    order_ttwidget,
    magicmask_ttwidget,
    magic_ttwidget,
    sizeof(s_ttwidget),
    (void *)NULL /* UNDEFINED: New_ttwidget  */,
    twin_Build_ttwidget,
    (void *)NULL /* inherits:  Break_ttwidget -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttwidget  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    twin_AddTo_ttwidget,
    twin_Remove_ttwidget,
    twin_Invalidate_ttwidget,
    twin_SetVisible_ttwidget,
    (void *)NULL /* UNDEFINED: SetTheme_ttwidget  */,
    twin_Draw_ttwidget,
    twin_BuiltinRepaint_ttwidget,

    /* extends ttfn_ttvisible */
    twin_SetXY_ttwidget,
    twin_SetWH_ttwidget,
    twin_SetXl_ttwidget,
    twin_SetYl_ttwidget,
    (void *)NULL /* UNDEFINED: SetWl_ttwidget  */,
    (void *)NULL /* UNDEFINED: SetHl_ttwidget  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttwidget  */,

  },
 
  {
    order_ttlabel,
    magicmask_ttlabel,
    magic_ttlabel,
    sizeof(s_ttlabel),
    (void *)NULL /* UNDEFINED: New_ttlabel  */,
    twin_Build_ttlabel,
    (void *)NULL /* inherits:  Break_ttlabel -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttlabel  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttlabel -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttlabel -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttlabel -> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttlabel -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttlabel  */,
    twin_Draw_ttlabel,
    twin_BuiltinRepaint_ttlabel,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttlabel -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttlabel -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttlabel -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttlabel -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttlabel  */,
    (void *)NULL /* UNDEFINED: SetHl_ttlabel  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttlabel  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: DoSetFont_ttlabel  */,

  },
 
  {
    order_tttooltip,
    magicmask_tttooltip,
    magic_tttooltip,
    sizeof(s_tttooltip),
    (void *)NULL /* UNDEFINED: New_tttooltip  */,
    (void *)NULL /* inherits:  Build_tttooltip -> Build_ttlabel */,
    (void *)NULL /* inherits:  Break_tttooltip -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tttooltip  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttooltip -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_tttooltip -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_tttooltip -> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_tttooltip -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttooltip  */,
    (void *)NULL /* inherits:  Draw_tttooltip -> Draw_ttlabel */,
    (void *)NULL /* inherits:  BuiltinRepaint_tttooltip -> BuiltinRepaint_ttlabel */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_tttooltip -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_tttooltip -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_tttooltip -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_tttooltip -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_tttooltip  */,
    (void *)NULL /* UNDEFINED: SetHl_tttooltip  */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttooltip  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: DoSetFont_tttooltip  */,

    /* extends ttfn_ttlabel */
    (void *)NULL /* UNDEFINED: SetWidget_tttooltip  */,

  },
 
  {
    order_ttbuttongroup,
    magicmask_ttbuttongroup,
    magic_ttbuttongroup,
    sizeof(s_ttbuttongroup),
    (void *)NULL /* UNDEFINED: New_ttbuttongroup  */,
    (void *)NULL /* UNDEFINED: Build_ttbuttongroup  */,
    (void *)NULL /* inherits:  Break_ttbuttongroup -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttbuttongroup  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: Add_ttbuttongroup  */,
    (void *)NULL /* UNDEFINED: Remove_ttbuttongroup  */,
    (void *)NULL /* UNDEFINED: SetChecked_ttbuttongroup  */,

  },
 
  {
    order_ttanybutton,
    magicmask_ttanybutton,
    magic_ttanybutton,
    sizeof(s_ttanybutton),
    (void *)NULL /* UNDEFINED: New_ttanybutton  */,
    twin_Build_ttanybutton,
    (void *)NULL /* inherits:  Break_ttanybutton -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttanybutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanybutton -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttanybutton -> Remove_ttwidget */,
    twin_Invalidate_ttanybutton,
    (void *)NULL /* inherits:  SetVisible_ttanybutton -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanybutton  */,
    (void *)NULL /* inherits:  Draw_ttanybutton -> Draw_ttwidget */,
    twin_BuiltinRepaint_ttanybutton,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttanybutton -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttanybutton -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttanybutton -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttanybutton -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttanybutton  */,
    (void *)NULL /* UNDEFINED: SetHl_ttanybutton  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanybutton  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttanybutton  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttanybutton  */,

  },
 
  {
    order_ttbutton,
    magicmask_ttbutton,
    magic_ttbutton,
    sizeof(s_ttbutton),
    (void *)NULL /* UNDEFINED: New_ttbutton  */,
    (void *)NULL /* inherits:  Build_ttbutton -> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttbutton -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttbutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttbutton -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttbutton -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttbutton -> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttbutton -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttbutton  */,
    (void *)NULL /* inherits:  Draw_ttbutton -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttbutton -> BuiltinRepaint_ttanybutton */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttbutton -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttbutton -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttbutton -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttbutton -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttbutton  */,
    (void *)NULL /* UNDEFINED: SetHl_ttbutton  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttbutton  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttbutton  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttbutton  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttbutton  */,

  },
 
  {
    order_ttcheckbutton,
    magicmask_ttcheckbutton,
    magic_ttcheckbutton,
    sizeof(s_ttcheckbutton),
    (void *)NULL /* UNDEFINED: New_ttcheckbutton  */,
    (void *)NULL /* inherits:  Build_ttcheckbutton -> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttcheckbutton -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttcheckbutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttcheckbutton -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttcheckbutton -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttcheckbutton -> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttcheckbutton -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttcheckbutton  */,
    (void *)NULL /* inherits:  Draw_ttcheckbutton -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttcheckbutton -> BuiltinRepaint_ttanybutton */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttcheckbutton -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttcheckbutton -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttcheckbutton -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttcheckbutton -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: SetHl_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttcheckbutton  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttcheckbutton  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttcheckbutton  */,

    /* extends ttfn_ttbutton */
    (void *)NULL /* UNDEFINED: SetChecked_ttcheckbutton  */,

  },
 
  {
    order_ttradiobutton,
    magicmask_ttradiobutton,
    magic_ttradiobutton,
    sizeof(s_ttradiobutton),
    (void *)NULL /* UNDEFINED: New_ttradiobutton  */,
    (void *)NULL /* inherits:  Build_ttradiobutton -> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttradiobutton -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttradiobutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttradiobutton -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttradiobutton -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttradiobutton -> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttradiobutton -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttradiobutton  */,
    (void *)NULL /* inherits:  Draw_ttradiobutton -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttradiobutton -> BuiltinRepaint_ttanybutton */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttradiobutton -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttradiobutton -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttradiobutton -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttradiobutton -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: SetHl_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttradiobutton  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttradiobutton  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttradiobutton  */,

    /* extends ttfn_ttbutton */
    (void *)NULL /* UNDEFINED: SetChecked_ttradiobutton  */,

    /* extends ttfn_ttcheckbutton */

  },
 
  {
    order_ttanyscroll,
    magicmask_ttanyscroll,
    magic_ttanyscroll,
    sizeof(s_ttanyscroll),
    (void *)NULL /* UNDEFINED: New_ttanyscroll  */,
    (void *)NULL /* inherits:  Build_ttanyscroll -> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttanyscroll -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttanyscroll  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanyscroll -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttanyscroll -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttanyscroll -> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttanyscroll -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanyscroll  */,
    (void *)NULL /* inherits:  Draw_ttanyscroll -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttanyscroll -> BuiltinRepaint_ttanybutton */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttanyscroll -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttanyscroll -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttanyscroll -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttanyscroll -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetHl_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanyscroll  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttanyscroll  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetState_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetSize_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetPosition_ttanyscroll  */,

  },
 
  {
    order_ttscrollbar,
    magicmask_ttscrollbar,
    magic_ttscrollbar,
    sizeof(s_ttscrollbar),
    (void *)NULL /* UNDEFINED: New_ttscrollbar  */,
    (void *)NULL /* inherits:  Build_ttscrollbar -> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttscrollbar -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttscrollbar  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscrollbar -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttscrollbar -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttscrollbar -> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttscrollbar -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscrollbar  */,
    (void *)NULL /* inherits:  Draw_ttscrollbar -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttscrollbar -> BuiltinRepaint_ttanybutton */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttscrollbar -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttscrollbar -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttscrollbar -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttscrollbar -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetHl_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscrollbar  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttscrollbar  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetState_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetSize_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetPosition_ttscrollbar  */,

    /* extends ttfn_ttanyscroll */

  },
 
  {
    order_ttslider,
    magicmask_ttslider,
    magic_ttslider,
    sizeof(s_ttslider),
    (void *)NULL /* UNDEFINED: New_ttslider  */,
    (void *)NULL /* inherits:  Build_ttslider -> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttslider -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttslider  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttslider -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttslider -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttslider -> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttslider -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttslider  */,
    (void *)NULL /* inherits:  Draw_ttslider -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttslider -> BuiltinRepaint_ttanybutton */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttslider -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttslider -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttslider -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttslider -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttslider  */,
    (void *)NULL /* UNDEFINED: SetHl_ttslider  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttslider  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttslider  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttslider  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttslider  */,
    (void *)NULL /* UNDEFINED: SetState_ttslider  */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttslider  */,
    (void *)NULL /* UNDEFINED: SetSize_ttslider  */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttslider  */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttslider  */,
    (void *)NULL /* UNDEFINED: SetPosition_ttslider  */,

    /* extends ttfn_ttanyscroll */
    (void *)NULL /* UNDEFINED: SetSlideMin_ttslider  */,
    (void *)NULL /* UNDEFINED: SetSlideMax_ttslider  */,
    (void *)NULL /* UNDEFINED: SetSlideValue_ttslider  */,

  },
 
  {
    order_ttprogressbar,
    magicmask_ttprogressbar,
    magic_ttprogressbar,
    sizeof(s_ttprogressbar),
    (void *)NULL /* UNDEFINED: New_ttprogressbar  */,
    (void *)NULL /* inherits:  Build_ttprogressbar -> Build_ttanybutton */,
    (void *)NULL /* inherits:  Break_ttprogressbar -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttprogressbar  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttprogressbar -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttprogressbar -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttprogressbar -> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttprogressbar -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttprogressbar  */,
    (void *)NULL /* inherits:  Draw_ttprogressbar -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttprogressbar -> BuiltinRepaint_ttanybutton */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttprogressbar -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttprogressbar -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttprogressbar -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttprogressbar -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetHl_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttprogressbar  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttprogressbar  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetState_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetSize_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetPosition_ttprogressbar  */,

    /* extends ttfn_ttanyscroll */

  },
 
  {
    order_ttscrollpane,
    magicmask_ttscrollpane,
    magic_ttscrollpane,
    sizeof(s_ttscrollpane),
    (void *)NULL /* UNDEFINED: New_ttscrollpane  */,
    (void *)NULL /* inherits:  Build_ttscrollpane -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttscrollpane -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttscrollpane  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscrollpane -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttscrollpane -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttscrollpane -> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttscrollpane -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscrollpane  */,
    (void *)NULL /* inherits:  Draw_ttscrollpane -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttscrollpane -> BuiltinRepaint_ttwidget */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttscrollpane -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttscrollpane -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttscrollpane -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttscrollpane -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetHl_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscrollpane  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetBarX_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetBarY_ttscrollpane  */,

  },
 
  {
    order_ttwindow,
    magicmask_ttwindow,
    magic_ttwindow,
    sizeof(s_ttwindow),
    (void *)NULL /* UNDEFINED: New_ttwindow  */,
    twin_Build_ttwindow,
    (void *)NULL /* inherits:  Break_ttwindow -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttwindow  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttwindow -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttwindow -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttwindow -> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttwindow -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttwindow  */,
    (void *)NULL /* inherits:  Draw_ttwindow -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttwindow -> BuiltinRepaint_ttwidget */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttwindow -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttwindow -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttwindow -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttwindow -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttwindow  */,
    (void *)NULL /* UNDEFINED: SetHl_ttwindow  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttwindow  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttwindow  */,

  },
 
  {
    order_ttframe,
    magicmask_ttframe,
    magic_ttframe,
    sizeof(s_ttframe),
    (void *)NULL /* UNDEFINED: New_ttframe  */,
    twin_Build_ttframe,
    (void *)NULL /* inherits:  Break_ttframe -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttframe  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    twin_AddTo_ttframe,
    (void *)NULL /* inherits:  Remove_ttframe -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttframe -> Invalidate_ttwidget */,
    twin_SetVisible_ttframe,
    (void *)NULL /* UNDEFINED: SetTheme_ttframe  */,
    (void *)NULL /* inherits:  Draw_ttframe -> Draw_ttwidget */,
    twin_BuiltinRepaint_ttframe,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttframe -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttframe -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttframe -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttframe -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttframe  */,
    (void *)NULL /* UNDEFINED: SetHl_ttframe  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttframe  */,

    /* extends ttfn_ttwidget */
    twin_SetTitle_ttframe,

    /* extends ttfn_ttwindow */
    (void *)NULL /* UNDEFINED: SetMenubar_ttframe  */,

  },
 
  {
    order_ttscroller,
    magicmask_ttscroller,
    magic_ttscroller,
    sizeof(s_ttscroller),
    (void *)NULL /* UNDEFINED: New_ttscroller  */,
    (void *)NULL /* inherits:  Build_ttscroller -> Build_ttwindow */,
    (void *)NULL /* inherits:  Break_ttscroller -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttscroller  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscroller -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttscroller -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttscroller -> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttscroller -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscroller  */,
    (void *)NULL /* inherits:  Draw_ttscroller -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttscroller -> BuiltinRepaint_ttwidget */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttscroller -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttscroller -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttscroller -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttscroller -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttscroller  */,
    (void *)NULL /* UNDEFINED: SetHl_ttscroller  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscroller  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttscroller  */,

    /* extends ttfn_ttwindow */

  },
 
  {
    order_ttmenuitem,
    magicmask_ttmenuitem,
    magic_ttmenuitem,
    sizeof(s_ttmenuitem),
    (void *)NULL /* UNDEFINED: New_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: Build_ttmenuitem  */,
    (void *)NULL /* inherits:  Break_ttmenuitem -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttmenuitem  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    twin_AddTo_ttmenuitem,
    twin_Remove_ttmenuitem,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: Draw_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenuitem  */,

    /* extends ttfn_ttvisible */

  },
 
  {
    order_ttcheckmenuitem,
    magicmask_ttcheckmenuitem,
    magic_ttcheckmenuitem,
    sizeof(s_ttcheckmenuitem),
    (void *)NULL /* UNDEFINED: New_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: Build_ttcheckmenuitem  */,
    (void *)NULL /* inherits:  Break_ttcheckmenuitem -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttcheckmenuitem  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttcheckmenuitem -> AddTo_ttmenuitem */,
    (void *)NULL /* inherits:  Remove_ttcheckmenuitem -> Remove_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Invalidate_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: Draw_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttcheckmenuitem  */,

    /* extends ttfn_ttvisible */

    /* extends ttfn_ttmenuitem */

  },
 
  {
    order_ttradiomenuitem,
    magicmask_ttradiomenuitem,
    magic_ttradiomenuitem,
    sizeof(s_ttradiomenuitem),
    (void *)NULL /* UNDEFINED: New_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: Build_ttradiomenuitem  */,
    (void *)NULL /* inherits:  Break_ttradiomenuitem -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttradiomenuitem  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttradiomenuitem -> AddTo_ttmenuitem */,
    (void *)NULL /* inherits:  Remove_ttradiomenuitem -> Remove_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Invalidate_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: Draw_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttradiomenuitem  */,

    /* extends ttfn_ttvisible */

    /* extends ttfn_ttmenuitem */

    /* extends ttfn_ttcheckmenuitem */

  },
 
  {
    order_ttmenu,
    magicmask_ttmenu,
    magic_ttmenu,
    sizeof(s_ttmenu),
    (void *)NULL /* UNDEFINED: New_ttmenu  */,
    (void *)NULL /* UNDEFINED: Build_ttmenu  */,
    (void *)NULL /* inherits:  Break_ttmenu -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttmenu  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    twin_AddTo_ttmenu,
    twin_Remove_ttmenu,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenu  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenu  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenu  */,
    (void *)NULL /* UNDEFINED: Draw_ttmenu  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenu  */,

    /* extends ttfn_ttvisible */

    /* extends ttfn_ttmenuitem */

  },
 
  {
    order_ttmenubar,
    magicmask_ttmenubar,
    magic_ttmenubar,
    sizeof(s_ttmenubar),
    (void *)NULL /* UNDEFINED: New_ttmenubar  */,
    twin_Build_ttmenubar,
    (void *)NULL /* inherits:  Break_ttmenubar -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttmenubar  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttmenubar  */,
    (void *)NULL /* UNDEFINED: Remove_ttmenubar  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenubar  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenubar  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenubar  */,
    (void *)NULL /* UNDEFINED: Draw_ttmenubar  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenubar  */,

    /* extends ttfn_ttvisible */

  },
 
  {
    order_ttanytext,
    magicmask_ttanytext,
    magic_ttanytext,
    sizeof(s_ttanytext),
    (void *)NULL /* UNDEFINED: New_ttanytext  */,
    (void *)NULL /* inherits:  Build_ttanytext -> Build_ttwindow */,
    (void *)NULL /* inherits:  Break_ttanytext -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttanytext  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanytext -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttanytext -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_ttanytext -> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttanytext -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanytext  */,
    (void *)NULL /* inherits:  Draw_ttanytext -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttanytext -> BuiltinRepaint_ttwidget */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_ttanytext -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttanytext -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttanytext -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttanytext -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttanytext  */,
    (void *)NULL /* UNDEFINED: SetHl_ttanytext  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanytext  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttanytext  */,

    /* extends ttfn_ttwindow */

  },
 
  {
    order_tttextfield,
    magicmask_tttextfield,
    magic_tttextfield,
    sizeof(s_tttextfield),
    (void *)NULL /* UNDEFINED: New_tttextfield  */,
    (void *)NULL /* inherits:  Build_tttextfield -> Build_ttwindow */,
    (void *)NULL /* inherits:  Break_tttextfield -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tttextfield  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttextfield -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_tttextfield -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_tttextfield -> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_tttextfield -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttextfield  */,
    (void *)NULL /* inherits:  Draw_tttextfield -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_tttextfield -> BuiltinRepaint_ttwidget */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_tttextfield -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_tttextfield -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_tttextfield -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_tttextfield -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_tttextfield  */,
    (void *)NULL /* UNDEFINED: SetHl_tttextfield  */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttextfield  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_tttextfield  */,

    /* extends ttfn_ttwindow */

    /* extends ttfn_ttanytext */

  },
 
  {
    order_tttextarea,
    magicmask_tttextarea,
    magic_tttextarea,
    sizeof(s_tttextarea),
    (void *)NULL /* UNDEFINED: New_tttextarea  */,
    (void *)NULL /* inherits:  Build_tttextarea -> Build_ttwindow */,
    (void *)NULL /* inherits:  Break_tttextarea -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tttextarea  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttextarea -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_tttextarea -> Remove_ttwidget */,
    (void *)NULL /* inherits:  Invalidate_tttextarea -> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_tttextarea -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttextarea  */,
    (void *)NULL /* inherits:  Draw_tttextarea -> Draw_ttwidget */,
    (void *)NULL /* inherits:  BuiltinRepaint_tttextarea -> BuiltinRepaint_ttwidget */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* inherits:  SetXY_tttextarea -> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_tttextarea -> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_tttextarea -> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_tttextarea -> SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_tttextarea  */,
    (void *)NULL /* UNDEFINED: SetHl_tttextarea  */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttextarea  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_tttextarea  */,

    /* extends ttfn_ttwindow */

    /* extends ttfn_ttanytext */

    /* extends ttfn_tttextfield */

  },
 
  {
    order_tttheme,
    magicmask_tttheme,
    magic_tttheme,
    sizeof(s_tttheme),
    (void *)NULL /* UNDEFINED: New_tttheme  */,
    (void *)NULL /* UNDEFINED: Build_tttheme  */,
    (void *)NULL /* inherits:  Break_tttheme -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tttheme  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */

  },
 
  {
    order_ttapplication,
    magicmask_ttapplication,
    magic_ttapplication,
    sizeof(s_ttapplication),
    (void *)NULL /* UNDEFINED: New_ttapplication  */,
    twin_Build_ttapplication,
    (void *)NULL /* inherits:  Break_ttapplication -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttapplication  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */

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



