




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
    }
}


/* ttvisible */


/* ttnative */
static ttnative twin_GetRoot_ttnative(void) {
    return Create_ttnative(TwFirstScreen());
}
static void twin_Del_ttnative(ttnative o) {
    o->native = TW_NOID;
    o->FN->FN_ttvisible->Del((ttvisible)o);
}


/* ttwidget */
static void ExposeListener_ttwidget(tevent_any e, void *id) {
    ttwidget o = ID2(ttwidget,(opaque)id);
    if (o)
	Expose_ttvisible((ttvisible)o, e->EventWidget.X, e->EventWidget.Y, e->EventWidget.XWidth, e->EventWidget.YWidth);
}
static ttlistener AddExposeListener_ttwidget(ttwidget o) {
    ttlistener l;
    if ((l = Create_ttlistener((ttcomponent)o))) {
	if ((l->native = (opaque)TwAddWidgetListener
	     (o->native, TW_MSG_WIDGET_EXPOSE,
	      (tfn_listener)ExposeListener_ttwidget, (void *)OBJ2ID(o))))
	    /* OBJ2ID(o) is (ttopaque), so has same size as (void *) :-) */
	    return l;
	TDEL(l);
    }
    return NULL;
}
static ttwidget twin_Build_ttwidget(ttwidget o) {
    if (TW_NOID != (o->native = TwCreateWidget(1, 1, 0, TW_WIDGETFL_USEFILL, 0, MAXDAT, HWATTR(myTheme(o)->bg[tttheme_bg_normal], myTheme(o)->fill))))
	return o;
    return NULL;
}
static void twin_AddTo_ttwidget(ttwidget o, ttvisible parent) {
    if (parent && !o->parent) {
	/* prepend to list, cannot use o->FN->FN_ttvisible->AddTo() */
	if ((o->next = parent->child_first))
	    parent->child_first->prev = (ttvisible)o;
	else
	    parent->child_last = (ttvisible)o;
	parent->child_first = (ttvisible)o;
	o->prev = NULL;
	o->parent = parent;

	TwMapWidget(o->native, parent->native);
    }
}
static void twin_SetVisible_ttwidget(ttwidget o, byte on_off) {
    on_off = !!on_off;
    if (on_off != !!(o->vflags & ttvisible_vflags_visible)) {
	o->vflags ^= ttvisible_vflags_visible;
	TwSetVisibleWidget(o->native, on_off);
    }
}
static void twin_Remove_ttwidget(ttwidget o) {
    if (o->parent) {
	o->FN->FN_ttvisible->Remove((super_(ttwidget))o);
	
	TwUnMapWidget(o->native);
    }
}
static void twin_SetXYWH_ttwidget(ttwidget o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h) {
    if (mask & 1)
	o->x = x;
    if (mask & 2)
	o->y = y;
    if (mask & 3)
	TwSetXYWidget(o->native, o->x, o->y);
    if (mask & 4)
	o->w = w;
    if (mask & 8)
	o->h = h;
    if (mask & 0x0c)
	TwResizeWidget(o->native, o->w, o->h);
}
static void twin_SetXlYl_ttwidget(ttwidget o, ttuint mask, ttint xl, ttint yl) {
    ttint o_xl = o->xl, o_yl = o->yl;
    if (mask & 1)
	o->xl = xl;
    if (mask & 2)
	o->yl = yl;
    if (mask & 3)
       TwScrollWidget(o->native, o->xl - o_xl, o->yl - o_yl);
}
static void twin_Draw_ttwidget(ttwidget o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch,
			       TT_CONST ttbyte *asciidata, TT_CONST ttfont *fontdata, TT_CONST ttattr *attrdata) {
    TwDraw2Widget(o->native, w, h, x, y, pitch, asciidata, fontdata, attrdata);
}


/* ttlabel */
static ttlabel twin_Build_ttlabel(ttlabel o) {
    if (TW_NOID != (o->native = TwCreateWidget(o->text_len, 1, 0, TW_WIDGETFL_USEEXPOSE, 0, MAXDAT, HWATTR(myTheme(o)->bg[tttheme_bg_normal], myTheme(o)->fill)))) {
	if (AddExposeListener_ttwidget((ttwidget)o))
	    return o;
    }
    return NULL;
}
/* force inheritance from Draw_ttwidget() : twin_Draw_ttlabel; */
#define twin_Draw_ttlabel (void *)twin_Draw_ttwidget

static void twin_BuiltinRepaint_ttlabel(ttlabel o, dat x, dat y, dat w, dat h) {
    if (y == 0 && x < o->text_len)
	TwDrawHWFontWidget(o->native, TT_MIN2(w,o->text_len-x), 1, x, 0, o->text_len, o->text+x);
}

/* ttanybutton */
static void twin_BuiltinRepaint_ttanybutton(ttanybutton o, dat x, dat y, dat w, dat h) {
    if (o->text) {
	if (h+y > o->text_height)
	    h = o->text_height - y;
	if (w+x > o->text_width)
	    w = o->text_width - x;

	if (h > 0 && w > 0 && y < o->text_height && x < o->text_width)
	    TwDrawHWFontWidget(o->native, TT_MIN2(w,o->text_width-x), TT_MIN2(h, o->text_height-y),
				  x, y, o->text_width, o->text + x + (y * o->text_width));
    } else
	TwDrawHWFontWidget(o->native, w, h, x, y, w, NULL);
}
static void MouseEventWrapper_ttanybutton(tevent_any e, tobj id) {
    ttanybutton o = ID2(ttanybutton,id);
    ttevent ev;
    if (o && (ev = (ttevent)Create_tteventbig
	      /* FIXME: convert to libTT codes/shiftflags */
	      (ttevent_evtype_mouse, e->EventMouse.Code, e->EventMouse.ShiftFlags,
	       e->EventMouse.X, e->EventMouse.Y, 0, 0, 0, NULL))) {
	
	/* also deletes ev */
	DispatchEvent((ttcomponent)o, ev, DispatchEvent_dflags_both);
    }
}
static ttlistener AddCommonListeners_ttanybutton(ttanybutton o) {
    ttlistener l[5];
    byte i = 0;
    void *id = (void *)OBJ2ID(o);
    
    if ((l[i] = AddExposeListener_ttwidget((ttwidget)o)) &&
	(i++, (l[i] = Create_ttlistener((ttcomponent)o))) &&
	((l[i]->native = (opaque)TwAddMouseListener(o->native, MOTION_MOUSE, 0, (tfn_listener)MouseEventWrapper_ttanybutton, id))) &&
	(i++, (l[i] = Create_ttlistener((ttcomponent)o))) &&
	((l[i]->native = (opaque)TwAddMouseListener(o->native, DOWN_LEFT, 0, (tfn_listener)MouseEventWrapper_ttanybutton, id))) &&
	(i++, (l[i] = Create_ttlistener((ttcomponent)o))) &&
	((l[i]->native = (opaque)TwAddMouseListener(o->native, DRAG_MOUSE|HOLD_LEFT, 0, (tfn_listener)MouseEventWrapper_ttanybutton, id))) &&
	(i++, (l[i] = Create_ttlistener((ttcomponent)o))) &&
	((l[i]->native = (opaque)TwAddMouseListener(o->native, RELEASE_LEFT, 0, (tfn_listener)MouseEventWrapper_ttanybutton, id)))) {
	
	return l[i];
    }
    while (i) {
    	TDEL(l[i]);
	i--;
    }
    return NULL;
}


/* ttbutton */
static ttbutton twin_Build_ttbutton(ttbutton o) {
    dat w = o->text_width + myTheme(o)->shape[tttheme_shape_ttbutton_normal][0].border[tt_x_left] +
	myTheme(o)->shape[tttheme_shape_ttbutton_normal][0].border[tt_x_right];
    dat h = o->text_height + myTheme(o)->shape[tttheme_shape_ttbutton_normal][0].border[tt_y_up] +
	myTheme(o)->shape[tttheme_shape_ttbutton_normal][0].border[tt_y_down];
    
    if (TW_NOID != (o->native = TwCreateWidget(w, h, TW_WIDGET_WANT_MOUSE|TW_WIDGET_WANT_MOUSE_MOTION,
	TW_WIDGETFL_USEEXPOSE, 0, 0, myTheme(o)->bg[tttheme_bg_normal]))) {
	if (AddCommonListeners_ttanybutton((ttanybutton)o))
	    return o;
    }
    return NULL;
}
static void twin_BuiltinRepaint_ttbutton(ttbutton o, dat x, dat y, dat w, dat h) {
    ttattr *t;
    dat tw, th;
    ttuint i, vflags;
    static TT_CONST ttuint case_flags[][3] = {
	{ ttanybutton_vflags_normal,   ttbutton_shape_normal,   tttheme_shape_ttbutton_normal, },
	{ ttanybutton_vflags_prelight, ttbutton_shape_prelight, tttheme_shape_ttbutton_prelight, },
	{ ttanybutton_vflags_pressed,  ttbutton_shape_pressed,  tttheme_shape_ttbutton_pressed, },
	{ ttanybutton_vflags_disabled, ttbutton_shape_disabled, tttheme_shape_ttbutton_disabled, },
    };
    
    vflags = o->vflags & ttanybutton_vflags_statemask;
    for (i = 0; i < sizeof(case_flags)/sizeof(case_flags[0]); i++) {
	if (case_flags[i][0] == vflags)
	    break;
    }
    if (i >= sizeof(case_flags)/sizeof(case_flags[0]))
	return;
    
    if (!(t = o->text_shape[case_flags[i][1]].attr))
	t = o->text_shape[case_flags[i][1]].attr
	= myTheme(o)->FN->ComputeShapeButton(o, i, case_flags[i][2]);

    if (t) {
	i = case_flags[i][1];
	
	tw = o->text_shape[i].width;
	th = o->text_shape[i].height;
	
	if (h+y > th)
	    h = th - y;
	if (w+x > tw)
	    w = tw - x;

	if (h > 0 && w > 0 && y < th && x < tw)
	    TwDrawHWAttrWidget(o->native, TT_MIN2(w,tw-x), TT_MIN2(h, th-y),
				 x, y, tw, t + x + (y * tw));
    } else
	TwDrawHWFontWidget(o->native, w, h, x, y, w, NULL);
}
static void twin_SetPressed_ttbutton(ttbutton o, byte pressed) {
}


/* ttcheckbutton */


/* ttradiobutton */


/* ttbuttongroup */

static ttbuttongroup twin_Build_ttbuttongroup(ttbuttongroup o) {
    if (TW_NOID != (o->native = TwCreateGroup()))
	return o;
    return NULL;
}


/* ttwindow */

static ttwindow twin_Build_ttwindow(ttwindow o) {
    if (TW_NOID != (o->native = TwCreateWindow
	 (0, NULL, NULL, myMenubar_n(o), myTheme(o)->bg[tttheme_bg_normal],
	  TW_LINECURSOR, TW_WINDOW_DRAG|
	  TW_WINDOW_WANT_CHANGES|TW_WINDOW_WANT_KEYS|TW_WINDOW_WANT_MOUSE,
	  TW_WINDOWFL_USEEXPOSE|TW_WINDOWFL_CURSOR_ON|TW_WINDOWFL_BORDERLESS, 1, 1, 0))) {
	if (AddExposeListener_ttwidget((ttwidget)o))
	    return o;
    }
    return NULL;
}

static void twin_BuiltinRepaint_ttwindow(ttwindow o, dat x, dat y, dat w, dat h) {
    TwDrawHWAttrWidget(o->native, w, h, x, y, w, NULL);    
}

/* ttframe */

static void AskCloseWrapper_ttframe(tevent_any ev, tobj id) {
    ttframe o = ID2(ttframe,id);
    if (o)
	DispatchSimpleEvent((ttcomponent)o, ttevent_evtype_askclose);
}

static ttlistener AddAskCloseListener_ttframe(ttframe o) {
    ttlistener l;
    opaque id = OBJ2ID(o);
    
    if ((l = Create_ttlistener((ttcomponent)o))) {
	if ((l->native = (opaque)TwAddGadgetListener
	     (o->native, 0 /*CLOSE code*/, (tfn_listener)AskCloseWrapper_ttframe, (void *)id)))
	    return l;
	TDEL(l);
    }
    return NULL;
}

static ttframe twin_Build_ttframe(ttframe o) {
    o->vflags &= ~ttvisible_vflags_visible;
    
    if (TW_NOID != (o->native = TwCreateWindow
	 (0, NULL, NULL, myMenubar_n(o), myTheme(o)->bg[tttheme_bg_normal],
	  TW_LINECURSOR, TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_CLOSE|
	  TW_WINDOW_WANT_CHANGES|TW_WINDOW_WANT_KEYS|TW_WINDOW_WANT_MOUSE,
	  TW_WINDOWFL_USEEXPOSE|TW_WINDOWFL_CURSOR_ON, 1, 1, 0)) &&
	AddExposeListener_ttwidget((ttwidget)o) &&
	AddAskCloseListener_ttframe(o)) {

	o->FN->AddTo(o, (ttvisible)TFN_ttnative->GetRoot());
	return o;
    }
    return NULL;
}

static void twin_BuiltinRepaint_ttframe(ttframe o, dat x, dat y, dat w, dat h) {
    TwDrawHWAttrWidget(o->native, w, h, x, y, w, NULL);    
}


/* ttscrollbar */


/* ttslider */


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
	    (parent->private, Code, 
	     *o->name ? (o->vflags & ttanybutton_vflags_disabled) ?
	     TW_ROW_ACTIVE : TW_ROW_INACTIVE
	     : TW_ROW_IGNORE, strlen(o->name), o->name);
	
	o->FN->FN_ttvisible->AddTo((super_(ttmenuitem))o, parent);
    }
}
static void twin_Remove_ttmenuitem(ttmenuitem o) {
    if (o->parent) {
	o->FN->FN_ttvisible->Remove((super_(ttmenuitem))o);
	
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
	
	if (TW_NOID != (o->private = TwCreate4MenuWindow(myMenubar_n(o))) &&
	    TW_NOID != (o->native = TwItem4Menu(parent->native, o->private,
						  !(o->vflags & ttanybutton_vflags_disabled),
						  strlen(o->name), o->name))) {

	    o->FN->FN_ttmenuitem->AddTo((super_(ttmenu))o, parent);
	}
    }
}
static void twin_Remove_ttmenu(ttmenu o) {
    if (o->parent) {
	o->FN->FN_ttmenuitem->Remove((super_(ttmenu))o);
	
	if (o->private != TW_NOID) {
	    TwDeleteObj(o->private);
	    o->private = TW_NOID;
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
		    (t->bg[tttheme_bg_menu], t->bg[tttheme_bg_menu_select],
		     t->bg[tttheme_bg_menu_disabled], t->bg[tttheme_bg_menu_selectdisabled],
		     t->bg[tttheme_bg_menu_shcut], t->bg[tttheme_bg_menu_selectshcut], 0))) {
	TwItem4MenuCommon(o->native);
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
static byte twin_MainLoop(void) {
    return TwMainLoop();
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
static void twin_ExitMainLoop(void) {
    TwExitMainLoop();
}
static void twin_DeleteListener(ttlistener o) {
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
    
    TwConfigMalloc(TTAllocMem, TTReAllocMem, TTFreeMem);
    
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
  },
  
  {
    order_ttevent,
    magicmask_ttevent,
    magic_ttevent,
    sizeof(s_ttevent),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,
    
  },
 
  {
    order_tteventbig,
    magicmask_tteventbig,
    magic_tteventbig,
    sizeof(s_tteventbig),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,
    
    TFN_ttevent,

    
  },
 
  {
    order_ttlistener,
    magicmask_ttlistener,
    magic_ttlistener,
    sizeof(s_ttlistener),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    (void *)NULL /* WARNING: undefined Create */,

  },
 
  {
    order_ttcallback,
    magicmask_ttcallback,
    magic_ttcallback,
    sizeof(s_ttcallback),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    (void *)NULL /* WARNING: undefined Create */,

    TFN_ttlistener,
            
  },
 
  {
    order_ttcomponent,
    magicmask_ttcomponent,
    magic_ttcomponent,
    sizeof(s_ttcomponent),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

  },
 
  {
    order_ttvisible,
    magicmask_ttvisible,
    magic_ttvisible,
    sizeof(s_ttvisible),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined AddTo */,
    (void *)NULL /* WARNING: undefined Remove */,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined Draw */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
  },
 
  {
    order_ttnative,
    magicmask_ttnative,
    magic_ttnative,
    sizeof(s_ttnative),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    twin_Del_ttnative,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined AddTo */,
    (void *)NULL /* WARNING: undefined Remove */,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined Draw */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    twin_GetRoot_ttnative,
    
  },
 
  {
    order_ttwidget,
    magicmask_ttwidget,
    magic_ttwidget,
    sizeof(s_ttwidget),
    (void *)NULL /* WARNING: undefined New */,
    twin_Build_ttwidget,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    twin_AddTo_ttwidget,
    twin_Remove_ttwidget,
    twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    twin_Draw_ttwidget,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    twin_SetXYWH_ttwidget,
    twin_SetXlYl_ttwidget,
                
  },
 
  {
    order_ttlabel,
    magicmask_ttlabel,
    magic_ttlabel,
    sizeof(s_ttlabel),
    (void *)NULL /* WARNING: undefined New */,
    twin_Build_ttlabel,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    twin_Draw_ttlabel,
    twin_BuiltinRepaint_ttlabel,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
  },
 
  {
    order_ttanybutton,
    magicmask_ttanybutton,
    magic_ttanybutton,
    sizeof(s_ttanybutton),
    (void *)NULL /* WARNING: undefined New */,
    (void *)twin_Build_ttwidget,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    twin_BuiltinRepaint_ttanybutton,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
  },
 
  {
    order_ttbutton,
    magicmask_ttbutton,
    magic_ttbutton,
    sizeof(s_ttbutton),
    (void *)NULL /* WARNING: undefined New */,
    twin_Build_ttbutton,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    twin_BuiltinRepaint_ttbutton,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
    TFN_ttanybutton,
    twin_SetPressed_ttbutton,
        
  },
 
  {
    order_ttcheckbutton,
    magicmask_ttcheckbutton,
    magic_ttcheckbutton,
    sizeof(s_ttcheckbutton),
    (void *)NULL /* WARNING: undefined New */,
    (void *)twin_Build_ttbutton,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    (void *)twin_BuiltinRepaint_ttbutton,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
    TFN_ttanybutton,
    (void *)twin_SetPressed_ttbutton,
        
    TFN_ttbutton,

  },
 
  {
    order_ttradiobutton,
    magicmask_ttradiobutton,
    magic_ttradiobutton,
    sizeof(s_ttradiobutton),
    (void *)NULL /* WARNING: undefined New */,
    (void *)twin_Build_ttbutton,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    (void *)twin_BuiltinRepaint_ttbutton,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
    TFN_ttanybutton,
    (void *)twin_SetPressed_ttbutton,
        
    TFN_ttbutton,

    TFN_ttcheckbutton,

  },
 
  {
    order_ttscrollbar,
    magicmask_ttscrollbar,
    magic_ttscrollbar,
    sizeof(s_ttscrollbar),
    (void *)NULL /* WARNING: undefined New */,
    (void *)twin_Build_ttwidget,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    (void *)twin_BuiltinRepaint_ttanybutton,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
    TFN_ttanybutton,

  },
 
  {
    order_ttbuttongroup,
    magicmask_ttbuttongroup,
    magic_ttbuttongroup,
    sizeof(s_ttbuttongroup),
    (void *)NULL /* WARNING: undefined New */,
    twin_Build_ttbuttongroup,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,

  },
 
  {
    order_ttwindow,
    magicmask_ttwindow,
    magic_ttwindow,
    sizeof(s_ttwindow),
    (void *)NULL /* WARNING: undefined New */,
    twin_Build_ttwindow,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    twin_BuiltinRepaint_ttwindow,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,

  },
 
  {
    order_ttframe,
    magicmask_ttframe,
    magic_ttframe,
    sizeof(s_ttframe),
    (void *)NULL /* WARNING: undefined New */,
    twin_Build_ttframe,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    twin_BuiltinRepaint_ttframe,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_ttscroller,
    magicmask_ttscroller,
    magic_ttscroller,
    sizeof(s_ttscroller),
    (void *)NULL /* WARNING: undefined New */,
    (void *)twin_Build_ttwindow,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    (void *)twin_BuiltinRepaint_ttwindow,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_ttmenuitem,
    magicmask_ttmenuitem,
    magic_ttmenuitem,
    sizeof(s_ttmenuitem),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    twin_AddTo_ttmenuitem,
    twin_Remove_ttmenuitem,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined Draw */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,

  },
 
  {
    order_ttcheckmenuitem,
    magicmask_ttcheckmenuitem,
    magic_ttcheckmenuitem,
    sizeof(s_ttcheckmenuitem),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttmenuitem,
    (void *)twin_Remove_ttmenuitem,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined Draw */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

  },
 
  {
    order_ttradiomenuitem,
    magicmask_ttradiomenuitem,
    magic_ttradiomenuitem,
    sizeof(s_ttradiomenuitem),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttmenuitem,
    (void *)twin_Remove_ttmenuitem,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined Draw */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

    TFN_ttcheckmenuitem,

  },
 
  {
    order_ttmenu,
    magicmask_ttmenu,
    magic_ttmenu,
    sizeof(s_ttmenu),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    twin_AddTo_ttmenu,
    twin_Remove_ttmenu,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined Draw */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

  },
 
  {
    order_ttmenubar,
    magicmask_ttmenubar,
    magic_ttmenubar,
    sizeof(s_ttmenubar),
    (void *)NULL /* WARNING: undefined New */,
    twin_Build_ttmenubar,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined AddTo */,
    (void *)NULL /* WARNING: undefined Remove */,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined Draw */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    
  },
 
  {
    order_ttanytext,
    magicmask_ttanytext,
    magic_ttanytext,
    sizeof(s_ttanytext),
    (void *)NULL /* WARNING: undefined New */,
    (void *)twin_Build_ttwindow,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    (void *)twin_BuiltinRepaint_ttwindow,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_tttextfield,
    magicmask_tttextfield,
    magic_tttextfield,
    sizeof(s_tttextfield),
    (void *)NULL /* WARNING: undefined New */,
    (void *)twin_Build_ttwindow,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    (void *)twin_BuiltinRepaint_ttwindow,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,

    TFN_ttwindow,

    TFN_ttanytext,

  },
 
  {
    order_tttextarea,
    magicmask_tttextarea,
    magic_tttextarea,
    sizeof(s_tttextarea),
    (void *)NULL /* WARNING: undefined New */,
    (void *)twin_Build_ttwindow,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)twin_AddTo_ttwidget,
    (void *)twin_Remove_ttwidget,
    (void *)twin_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)twin_Draw_ttwidget,
    (void *)twin_BuiltinRepaint_ttwindow,
            
    TFN_ttvisible,
    (void *)twin_SetXYWH_ttwidget,
    (void *)twin_SetXlYl_ttwidget,
                
    TFN_ttwidget,

    TFN_ttwindow,

    TFN_ttanytext,

    TFN_tttextfield,

  },
 
  {
    order_tttheme,
    magicmask_tttheme,
    magic_tttheme,
    sizeof(s_tttheme),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined ComputeShapeButton */,
    
  },
 
  {
    order_ttapplication,
    magicmask_ttapplication,
    magic_ttapplication,
    sizeof(s_ttapplication),
    (void *)NULL /* WARNING: undefined New */,
    twin_Build_ttapplication,
    (void *)twin_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    
  },



  {

    twin_Sync, 
    twin_Flush, 
    twin_TimidFlush, 
    twin_MainLoop, 
    twin_MainLoopOnce, 
    twin_ExitMainLoop,    
    twin_DeleteListener, 
    twin_Close, 
    twin_ConnectionFd, 
    twin_GetErrno, 
    twin_GetErrnoDetail,    
    twin_StrError, 
    twin_StrErrorDetail,   
  },
};



