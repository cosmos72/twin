




/* This file was automatically generated from hw_dummy_c by m4/hw.m4h, do not edit! */


/*
 *  hw_dummy_m4.c  --  implementation of libTT methods on "dummy" target display
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
 * this file is hw_dummy_c and is preprocessed by m4 to produce hw_dummy_m4.c
 * 
 * It must be a valid m4 file, and must produce a valid C include file.
 * 
 */


/*
 * We need to write down only overloaded methods.
 */
 


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


#include "inlines.h"
#include "seterrno.h"
#include "theme.h"

static s_ttfns dummy_TTFNs;



/* ttobj */
static ttobj dummy_New_ttobj(ttfn_ttobj FN, ttobj o) {
    /*
     * if argument (o) is non-NULL, use it instead of allocating memory.
     * in this case, increase o->refcount to avoid deallocating it later
     * with TTFreeMem().
     */
    ttuint oflags = (ttopaque)o ? ttobj_oflags_static : 0;
    
    if ((o || (o = (ttobj)TTAllocMem(FN->size)))) {
	if (AssignId(FN, o)) {
	    o->FN = FN;
	    o->refcount = ttobj_refcount_alive;
	    o->oflags = oflags;
	    o->native = o->private = o->user_data = (ttany)0;
	    o->events_inprogress = (ttuint)0;
	    return o;
	}
	if (!(oflags & ttobj_oflags_static))
	    TTFreeMem(o);
    }
    return (ttobj)0;
}
static ttobj dummy_Build_ttobj(ttobj o) {
    /*
     * this is necessary for all derived classes that do not overload FN->Build.
     * It is not actually useful for ttobj.
     */
    return o;
}
static void dummy_Break_ttobj(ttobj o) {
    /*
     * this is necessary for all derived classes that do not overload FN->Break.
     * It is not actually useful for ttobj.
     */
}
static void dummy_Del_ttobj(ttobj o) {
    if (o) {
	o->FN->Break(o);
	
	DropId(o);
	/* ttobj_refcount_alive flag should already be removed... but better clear it */
	if (!(o->refcount &= ~ttobj_refcount_alive) &&
	    !(o->oflags & ttobj_oflags_static))
	    
	    TTFreeMem(o);
    }
}

static void dummy_SetValue_ttobj(ttobj o, ttuint which, ttany value) {
    /* FIXME: finish this */
}
static void dummy_ChangeValue_ttobj(ttobj o, ttuint which, ttany nand_value, ttany xor_value) {
    /* FIXME: finish this */
    /* o->{which} = (o->{which} & ~nand_value) ^ xor_value; */
}



/* ttevent */
static ttevent dummy_New_ttevent(ttfn_ttevent FN, ttevent o) {
    if ((o = (ttevent)FN->FN_ttobj->New((ttfn_ttobj)FN, (ttobj)o))) {
	o->component = (ttcomponent)0;
	o->evtype = o->evcode = o->evflags = 0;
    }
    return o;
}


/* ttlistener */
static ttlistener dummy_New_ttlistener(ttfn_ttlistener FN, ttlistener o) {
    if ((o = (ttlistener)FN->FN_ttobj->New((ttfn_ttobj)FN, (ttobj)o))) {
	o->lflags = 0;
	o->component = (ttcomponent)0;
	o->prev = o->next = (ttlistener)0;
    }
    return o;
}
static void dummy_Del_ttlistener(ttlistener o) {
    if (o) {
	if (o->next)
	    o->next->prev = o->prev;
	if (o->prev)
	    o->prev->next = o->next;
	else if (o->component && o->component->listeners == o)
	    o->component->listeners = o->next;

	o->prev = o->next = (ttlistener)0;
	
	THW.DeleteListener(o);

	o->FN->FN_ttobj->Del((ttobj)o);
    }
}

static ttlistener dummy_Create_ttlistener(ttcomponent c) {
    ttlistener o;
    if ((o = TNEW(ttlistener))) {
	if ((o->next = c->listeners))
	    o->next->prev = o;
	o->component = c;
	c->listeners = o;
    }
    return o;
}

/* ttcallback */
static ttcallback dummy_New_ttcallback(ttfn_ttcallback FN, ttcallback o) {
    if ((o = (ttcallback)FN->FN_ttlistener->New((ttfn_ttlistener)FN, (ttlistener)o))) {
	o->AVL = empty_AVL;
	o->prev = o->next = (ttcallback)0;
	o->event = (ttevent)0;
	o->narg_component = o->narg_event = (ttuint)-1;
	o->nargs = 0;
	o->function = (ttany)0;
	o->args = (ttany *)0;
    }
    return o;
}
static void dummy_Del_ttcallback(ttcallback o) {
    if (o) {
	(void)Remove_ttcallback(o);
	TDEL(o->event);
	o->event = (ttevent)0;
	if (o->args)
	    TTFreeMem(o->args);
	o->args = (ttany *)0;
	o->FN->FN_ttlistener->Del((ttlistener)o);
    }
}

/* ttcomponent */
static ttcomponent dummy_New_ttcomponent(ttfn_ttcomponent FN, ttcomponent o) {
    if ((o = (ttcomponent)FN->FN_ttobj->New((ttfn_ttobj)FN, (ttobj)o))) {
	o->listeners = (ttlistener)0;
	o->callbacks = (ttcallback)0;
    }
    return o;
}
static void dummy_Del_ttcomponent(ttcomponent o) {
    if (o) {
	DispatchSimpleEvent(o, ttevent_evtype_del);

	DelAllListeners_ttcomponent(o);
	DelAllCallbacks_ttcomponent(o);
	
	o->FN->FN_ttobj->Del((ttobj)o);
    }
}

/* ttvisible */
static ttvisible dummy_New_ttvisible(ttfn_ttvisible FN, ttvisible o) {
    if ((o = (ttvisible)FN->FN_ttcomponent->New((ttfn_ttcomponent)FN, (ttcomponent)o))) {
	o->vflags = ttvisible_vflags_visible;
	o->prev = o->next = o->parent = o->child_first = o->child_last = NULL;
	o->theme = NULL;
	o->repaint = (ttvisible_repaint_fn)0;
    }
    return o;
}
static void dummy_AddTo_ttvisible(ttvisible o, ttvisible parent) {
    /* append */
    if (parent && !o->parent) {
	if ((o->prev = parent->child_last))
	    parent->child_last->next = o;
	else
	    parent->child_first = o;
	parent->child_last = o;
	o->next = NULL;
	o->parent = parent;
    }
}
static void dummy_SetVisible_ttvisible(ttvisible o, byte on_off) {
    if (on_off)
	o->vflags |= ttvisible_vflags_visible;
    else
	o->vflags &= ~ttvisible_vflags_visible;
}
static void dummy_Remove_ttvisible(ttvisible o) {
    ttvisible parent;
    if ((parent = o->parent)) {
	if (o->prev)
	    o->prev->next = o->next;
	else
	    parent->child_first = o->next;
	if (o->next)
	    o->next->prev = o->prev;
	else
	    parent->child_last = o->prev;
	o->parent = NULL;
    }
}
static void dummy_SetTheme_ttvisible(ttvisible o, tttheme theme) {
    tttheme oldtheme = myTheme(o);
    o->theme = theme;
    if (oldtheme != theme && o->parent && (o->vflags & ttvisible_vflags_visible))
	Expose_ttvisible(o, ttvisible_repaint_args_WHOLE);
}
static void dummy_BuiltinRepaint_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h) {
    /* do-nothing */
}

/* ttnative */
static ttnative dummy_GetRoot_ttnative(void) {
    return Create_ttnative((ttany)0);
}

/* ttwidget */
static ttwidget dummy_New_ttwidget(ttfn_ttwidget FN, ttwidget o) {
    if ((o = (ttwidget)FN->FN_ttvisible->New((ttfn_ttvisible)FN, (ttvisible)o))) {
	o->x = o->y = o->w = o->h = (ttuint)0;
    }
    return o;
}
static void dummy_SetXYWH_ttwidget(ttwidget o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h) {
    if (mask & 1)
	o->x = x;
    if (mask & 2)
	o->y = y;
    if (mask & 4)
	o->w = w;
    if (mask & 8)
	o->h = h;
}

/* ttlabel */
static ttlabel dummy_New_ttlabel(ttfn_ttlabel FN, ttlabel o) {
    if ((o = (ttlabel)FN->FN_ttwidget->New((ttfn_ttwidget)FN, (ttwidget)o))) {
	o->text_len = 0;
	o->text = (ttfont *)0;
    }
    return o;
}
static void dummy_Del_ttlabel(ttlabel o) {
    if (o->text)
	TTFreeMem(o->text);
    o->text = (ttfont *)0;
    o->FN->FN_ttwidget->Del((ttwidget)o);
}
static byte dummy_SetText_ttlabel(ttlabel o, TT_CONST byte * text) {
    ttfont *_text;
    ttshort _text_len;
    
    if (text)
	_text_len = strlen(text);
    else
	_text_len = 0;
    
    if (!text || (_text = CloneStr2TTFont(text, _text_len))) {
	if (o->text)
	    TTFreeMem(o->text);
	o->text_len = _text_len;
	o->text = _text;
	if (o->parent && o->vflags & ttvisible_vflags_visible)
	    Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE);
	return TRUE;
    }
    return FALSE;
}

/* ttanybutton */
static ttanybutton dummy_New_ttanybutton(ttfn_ttanybutton FN, ttanybutton o) {
    if ((o = (ttanybutton)FN->FN_ttwidget->New((ttfn_ttwidget)FN, (ttwidget)o))) {
	o->text = NULL;
	o->text_width = o->text_height = 0;
	TTWriteMem(o->text_shape, '\0', sizeof(o->text_shape));
	return o;
    }
    return NULL;
}
static void dummy_Del_ttanybutton(ttanybutton o) {
    if (o->text)
	TTFreeMem(o->text);
    o->text = (ttfont *)0;
    o->FN->FN_ttwidget->Del((ttwidget)o);
}
static byte dummy_SetText_ttanybutton(ttanybutton o, TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch) {
    ttfont *_text = NULL;
    ttuint _text_len = width * height * sizeof(ttfont);
    byte i;
    
    if (!text || !_text_len || (_text = TTAllocMem(_text_len))) {
	o->text_width = width;
	o->text_height = height;
	if (o->text)
	    TTFreeMem(o->text);
	o->text = _text_len ? _text : NULL;
	while (_text_len) {
	    TTCopyMem(text, _text, width * sizeof(ttfont));
	    text += pitch;
	    _text += width;
	    _text_len -= width;
	}
	for (i = 0; i < ttbutton_shape_max; i++) {
	    if (o->text_shape[i].attr) {
		TTFreeMem(o->text_shape[i].attr);
		o->text_shape[i].attr = NULL;
	    }
	}
	if (o->parent && o->vflags & ttvisible_vflags_visible)
	    Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE);
	return TRUE;
    }
    return FALSE;
    
}


/* ttbutton */

/* default mouse handler */
static void DefaultMouseEvent_ttbutton(ttany arg) {
    ttbutton o;
    tteventbig ev;
    ttuint old_vflags, code;
    ttshort x, y;
    
    if (!(TTAssert(ev = ID2(tteventbig,arg)) &&
	  TTAssert(ev->evtype == ttevent_evtype_mouse) &&
	  TTAssert(o = (ttbutton)ev->component) && TTAssert(IS(ttbutton,o))))
	
	return;
    
    old_vflags = o->vflags;
    code = ev->evcode;
    x = ev->x;
    y = ev->y;
    
    /*fprintf(stderr, "x=%d, y=%d, code=%x\n", x, y, code);*/
    switch (code) {
      case MOTION_MOUSE:
	if (x < 0 || y < 0)
	    o->vflags &= ~ttanybutton_vflags_prelight;
	else
	    o->vflags |= ttanybutton_vflags_prelight;
	break;
      case DOWN_LEFT:
	o->vflags &= ~ttanybutton_vflags_prelight;
	o->vflags |= ttanybutton_vflags_pressed;
	break;
      case DRAG_MOUSE|HOLD_LEFT:
	if (x >= 0 && y >= 0 && x < o->text_width && y < o->text_height)
	    o->vflags |= ttanybutton_vflags_pressed;
	else
	    o->vflags &= ~ttanybutton_vflags_pressed;
	break;
      case RELEASE_LEFT:
	o->vflags &= ~ttanybutton_vflags_pressed;
	if (x >= 0 && y >= 0 && x < o->text_width && y < o->text_height) {
	    o->vflags |= ttanybutton_vflags_prelight;
	    
	    DispatchSimpleEvent((ttcomponent)o, ttevent_evtype_activate);
	}
      default:
	break;
    }
    
    if (old_vflags != o->vflags)
	Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE);
}

static ttbutton dummy_New_ttbutton(ttfn_ttbutton FN, ttbutton o) {
    if ((o = (ttbutton)FN->FN_ttanybutton->New((ttfn_ttanybutton)FN, (ttanybutton)o))) {
	if (Create_ttcallback((ttcomponent)o, ttevent_evtype_mouse,
			      ttcallback_lflags_after|ttcallback_lflags_arg0_event|ttcallback_lflags_function_plain,
			      (ttcallback_fn)DefaultMouseEvent_ttbutton, (ttany)0))
	    return o;
	
	TDEL(o);
    }
    return (ttbutton)0;
}

static void dummy_SetPressed_ttbutton(ttbutton o, byte pressed) {
}


/* ttcheckbutton */


/* ttradiobutton */


/* ttbuttongroup */


/* ttwindow */


/* ttframe */
static ttframe dummy_Build_ttframe(ttframe o) {
    o->vflags &= ~ttvisible_vflags_visible;
    o->FN->AddTo(o, (ttvisible)TFN_ttnative->GetRoot());
    return o;
}


/* ttscrollbar */


/* ttslider */


/* ttscroller */

static ttscroller dummy_New_ttscroller(ttfn_ttscroller FN, ttscroller o) {
    if ((o = (ttscroller)FN->FN_ttwindow->New((ttfn_ttwindow)FN, (ttwindow)o))) {
	o->scrollx = o->scrolly = NULL;
    }
    return o;
}
static void dummy_Del_ttscroller(ttscroller o) {
    if (o->scrollx)
	TDEL(o->scrollx);
    o->scrollx = (ttscrollbar)0;
    if (o->scrolly)
	TDEL(o->scrolly);
    o->scrolly = (ttscrollbar)0;
    
    o->FN->FN_ttwindow->Del((ttwindow)o);
    return;
}


/* ttmenuitem */


/* ttcheckmenuitem */


/* ttradiomenuitem */


/* ttmenuwindow */


/* ttmenu */

static ttmenu dummy_New_ttmenu(ttfn_ttmenu FN, ttmenu o) {
    if ((o = (ttmenu)FN->FN_ttmenuitem->New((ttfn_ttmenuitem)FN, (ttmenuitem)o))) {
	o->menubar = (ttmenubar)0;
	    return o;
	TDEL(o);
    }
    return o;
}

/* ttmenubar */

static void dummy_Del_ttmenubar(ttmenubar o) {
    if (TTD.Menubar == o)
	TTD.Menubar = (ttmenubar)0;
    o->FN->FN_ttvisible->Del((ttvisible)o);
}

/* ttanytext */

static ttanytext dummy_New_ttanytext(ttfn_ttanytext FN, ttanytext o) {
    if ((o = (ttanytext)FN->FN_ttwindow->New((ttfn_ttwindow)FN, (ttwindow)o))) {
	o->text = NULL;
	o->text_len = 0;
    }
    return o;
}


/* tttextfield */


/* tttextarea */


/* tttheme */

static tttheme dummy_Build_tttheme(tttheme o) {
    /* FIXME finish this */
    return o;
}
static void dummy_Del_tttheme(tttheme o) {
    if (TTD.Theme == o)
	TTD.Theme = TTD.DummyTheme;
    o->FN->FN_ttcomponent->Del((ttcomponent)o);
}

static void AddTransparentShapeButton(ttattr *attr, ttshort w, ttshort h, ttattr *tattr, ttshort tw, ttshort th,
				      ttshort b_left, ttshort b_up, ttshort b_right, ttshort b_down) {
    ttshort ti, i, tj, j;
    
    /* set left and right upper corners */
    for (j = 0; j < b_up; j++) {
	for (i = 0; i < b_left; i++) {
	    if (tattr[i+j*tw])
		attr[i+j*w] = tattr[i+j*tw];
	}
	
	for (i = 0; i < b_right; i++) {
	    if (tattr[i+(j+1)*tw-b_right])
		attr[i+(j+1)*w-b_right] = tattr[i+(j+1)*tw-b_right];
	}
    }
    /* set left and right borders */
    for (j = tj = b_up; j < h - b_down; j++, tj++) {
	if (tj == tw - b_down)
	    tj = b_up;
	for (i = 0; i < b_left; i++) {
	    if (tattr[i+tj*tw])
		attr[i+j*w] = tattr[i+tj*tw];
	}
	for (i = 0; i < b_right; i++) {
	    if (tattr[i+(tj+1)*tw-b_right])
		attr[i+(j+1)*w-b_right] = tattr[i+(tj+1)*tw-b_right];
	}
    }
    /* set left and right lower corners */
    for (j = 0; j < b_down; j++) {
	for (i = 0; i < b_left; i++) {
	    if (tattr[i+(th-b_down+j)*tw])
		attr[i+(h-b_down+j)*w] = tattr[i+(th-b_down+j)*tw];
	}
	for (i = 0; i < b_right; i++) {
	    if (tattr[i+(th-b_down+j+1)*tw-b_right])
		attr[i+(h-b_down+j+1)*w-b_right] = tattr[i+(th-b_down+j+1)*tw-b_right];
	}
    }
    /* set upper border */
    for (j = 0; j < b_up; j++) {
	for (i = ti = b_left; i < w - b_right; i++, ti++) {
	    if (ti == tw - b_right)
		ti = 0;
	    if (tattr[ti+j*tw])
		attr[i+j*w] = tattr[ti+j*tw];
	}
    }
    /* set lower border */
    for (j = 0; j < b_down; j++) {
	for (i = ti = b_left; i < w - b_right; i++, ti++) {
	    if (ti == tw - b_right)
		ti = b_left;
	    if (tattr[ti+(th-b_down+tj)*tw])
		attr[i+(h-b_down+j)*w] = tattr[ti+(th-b_down+tj)*tw];
	}
    }
}
				     
static void AddTextButton(ttattr *attr, ttshort w, ttfont *text, ttshort tw, ttshort th, ttcol bg) {
    ttshort _tw;
    
    for (; th; th--) {
	for (_tw = tw; _tw; _tw--) {
	    *attr++ = HWATTR(bg, *text);
	    text++;
	}
	attr += w - tw;
    }
}

static ttattr * dummy_ComputeShapeButton_tttheme(ttbutton o, byte button_i, byte theme_i) {
    tttheme t = myTheme(o);
    ttshape t_shape = &t->shape[theme_i][0], s_shape =  &t->shape[theme_i][1];
    ttattr *shape;
    ttshort w = o->text_width, h = o->text_height;
    ttshort b_left = t_shape->border[tt_x_left], b_up = t_shape->border[tt_y_up];
    ttshort b_right = t_shape->border[tt_x_right], b_down = t_shape->border[tt_y_down];
    
    if ((shape = (ttattr *)TTAllocMem((w += b_left+b_right) * (h += b_up+b_down) * sizeof(ttattr) ))) {
	
	TTWriteMem(shape, '\0', w * h * sizeof(ttattr));
	
	o->text_shape[button_i].width = w;
	o->text_shape[button_i].height = h;
	TTCopyMem(t_shape->border, o->text_shape[button_i].border, 4*sizeof(ttshort));
	
	AddTransparentShapeButton(shape, w, h, t_shape->attr, t_shape->width, t_shape->height,
				  b_left, b_up, b_right, b_down);
    
    	AddTransparentShapeButton(shape, w, h, s_shape->attr, s_shape->width, s_shape->height,
				  s_shape->border[tt_x_left], s_shape->border[tt_y_up],
				  s_shape->border[tt_x_right], s_shape->border[tt_y_down]);

	AddTextButton(shape + b_left + w * b_up, w, o->text, o->text_width, o->text_height,
		      myTheme(o)->bg[tttheme_bg_normal]);
	
    }
    return shape;
}


/* ttapplication */
static ttapplication dummy_New_ttapplication(ttfn_ttapplication FN, ttapplication o) {
    if ((o = (ttapplication)FN->FN_ttcomponent->New((ttfn_ttcomponent)FN, (ttcomponent)o))) {
	o->name = (ttbyte *)0;
    }
    return o;
}

static void dummy_Del_ttapplication(ttapplication o) {
    if (TTD.Application == o)
	TTD.Application = (ttapplication)0;
    if (o->name)
	TTFreeMem(o->name);
    o->name = (ttbyte *)0;
    o->FN->FN_ttcomponent->Del((ttcomponent)o);
}


/* common stuff: */




                            
static ttany dummy_GetValue_ttobj(ttobj o, ttuint which) {
    if (o && TTAssert(IS(ttobj,o))) {
	switch (which) {
    
	  case ttobj_id:
	    return (ttany)o->id;    
	  case ttobj_refcount:
	    return (ttany)o->refcount;    
	  case ttobj_oflags:
	    return (ttany)o->oflags;            
	  case ttobj_user_data:
	    return (ttany)o->user_data;    
	  case ttobj_events_inprogress:
	    return (ttany)o->events_inprogress;
	  default:
	    break;
	}
    }
    return (ttany)0;
}  

                    
static ttany dummy_GetValue_ttevent(ttevent o, ttuint which) {
    if (o && TTAssert(IS(ttevent,o))) {
	switch (which) {
    
	  case ttevent_callback:
	    return (ttany)OBJ2ID(o->callback);    
	  case ttevent_component:
	    return (ttany)OBJ2ID(o->component);    
	  case ttevent_evtype:
	    return (ttany)o->evtype;    
	  case ttevent_evcode:
	    return (ttany)o->evcode;    
	  case ttevent_evflags:
	    return (ttany)o->evflags;
	  default:
	    return o->FN->FN_ttobj->GetValue((ttobj)o, which);
	}
    }
    return (ttany)0;
} 

                        
static ttany dummy_GetValue_tteventbig(tteventbig o, ttuint which) {
    if (o && TTAssert(IS(tteventbig,o))) {
	switch (which) {
    
	  case tteventbig_x:
	    return (ttany)o->x;    
	  case tteventbig_y:
	    return (ttany)o->y;    
	  case tteventbig_w:
	    return (ttany)o->w;    
	  case tteventbig_h:
	    return (ttany)o->h;    
	  case tteventbig_len:
	    return (ttany)o->len;    
	  case tteventbig_data:
	    return (ttany)(opaque)o->data;
	  default:
	    return o->FN->FN_ttevent->GetValue((ttevent)o, which);
	}
    }
    return (ttany)0;
} 

                
static ttany dummy_GetValue_ttlistener(ttlistener o, ttuint which) {
    if (o && TTAssert(IS(ttlistener,o))) {
	switch (which) {
    
	  case ttlistener_lflags:
	    return (ttany)o->lflags;    
	  case ttlistener_component:
	    return (ttany)OBJ2ID(o->component);    
	  case ttlistener_prev:
	    return (ttany)OBJ2ID(o->prev);    
	  case ttlistener_next:
	    return (ttany)OBJ2ID(o->next);
	  default:
	    return o->FN->FN_ttobj->GetValue((ttobj)o, which);
	}
    }
    return (ttany)0;
} 

                            
static ttany dummy_GetValue_ttcallback(ttcallback o, ttuint which) {
    if (o && TTAssert(IS(ttcallback,o))) {
	switch (which) {
        
	  case ttcallback_event:
	    return (ttany)OBJ2ID(o->event);    
	  case ttcallback_narg_component:
	    return (ttany)o->narg_component;    
	  case ttcallback_narg_event:
	    return (ttany)o->narg_event;    
	  case ttcallback_nargs:
	    return (ttany)o->nargs;    
	  case ttcallback_function:
	    return (ttany)(opaque)o->function;    
	  default:
	    return o->FN->FN_ttlistener->GetValue((ttlistener)o, which);
	}
    }
    return (ttany)0;
} 

        
static ttany dummy_GetValue_ttcomponent(ttcomponent o, ttuint which) {
    if (o && TTAssert(IS(ttcomponent,o))) {
	switch (which) {
    
	  case ttcomponent_listeners:
	    return (ttany)OBJ2ID(o->listeners);    
	  case ttcomponent_callbacks:
	    return (ttany)OBJ2ID(o->callbacks);
	  default:
	    return o->FN->FN_ttobj->GetValue((ttobj)o, which);
	}
    }
    return (ttany)0;
} 

                                
static ttany dummy_GetValue_ttvisible(ttvisible o, ttuint which) {
    if (o && TTAssert(IS(ttvisible,o))) {
	switch (which) {
    
	  case ttvisible_vflags:
	    return (ttany)o->vflags;    
	  case ttvisible_prev:
	    return (ttany)OBJ2ID(o->prev);    
	  case ttvisible_next:
	    return (ttany)OBJ2ID(o->next);    
	  case ttvisible_parent:
	    return (ttany)OBJ2ID(o->parent);    
	  case ttvisible_child_first:
	    return (ttany)OBJ2ID(o->child_first);    
	  case ttvisible_child_last:
	    return (ttany)OBJ2ID(o->child_last);    
	  case ttvisible_theme:
	    return (ttany)OBJ2ID(o->theme);    
	  case ttvisible_repaint:
	    return (ttany)(opaque)o->repaint;
	  default:
	    return o->FN->FN_ttcomponent->GetValue((ttcomponent)o, which);
	}
    }
    return (ttany)0;
} 

 

                
static ttany dummy_GetValue_ttwidget(ttwidget o, ttuint which) {
    if (o && TTAssert(IS(ttwidget,o))) {
	switch (which) {
    
	  case ttwidget_x:
	    return (ttany)o->x;    
	  case ttwidget_y:
	    return (ttany)o->y;    
	  case ttwidget_w:
	    return (ttany)o->w;    
	  case ttwidget_h:
	    return (ttany)o->h;
	  default:
	    return o->FN->FN_ttvisible->GetValue((ttvisible)o, which);
	}
    }
    return (ttany)0;
} 

        
static ttany dummy_GetValue_ttlabel(ttlabel o, ttuint which) {
    if (o && TTAssert(IS(ttlabel,o))) {
	switch (which) {
    
	  case ttlabel_text_len:
	    return (ttany)o->text_len;    
	  default:
	    return o->FN->FN_ttwidget->GetValue((ttwidget)o, which);
	}
    }
    return (ttany)0;
} 

                
static ttany dummy_GetValue_ttanybutton(ttanybutton o, ttuint which) {
    if (o && TTAssert(IS(ttanybutton,o))) {
	switch (which) {
        
	  case ttanybutton_text_width:
	    return (ttany)o->text_width;    
	  case ttanybutton_text_height:
	    return (ttany)o->text_height;    
	  default:
	    return o->FN->FN_ttwidget->GetValue((ttwidget)o, which);
	}
    }
    return (ttany)0;
} 

 

 

 

 

 

     

 

        
static ttany dummy_GetValue_ttscroller(ttscroller o, ttuint which) {
    if (o && TTAssert(IS(ttscroller,o))) {
	switch (which) {
    
	  case ttscroller_scrollx:
	    return (ttany)OBJ2ID(o->scrollx);    
	  case ttscroller_scrolly:
	    return (ttany)OBJ2ID(o->scrolly);
	  default:
	    return o->FN->FN_ttwindow->GetValue((ttwindow)o, which);
	}
    }
    return (ttany)0;
} 

     

 

 


    
static ttany dummy_GetValue_ttmenu(ttmenu o, ttuint which) {
    if (o && TTAssert(IS(ttmenu,o))) {
	switch (which) {

    
	  case ttmenu_menubar:
	    return (ttany)OBJ2ID(o->menubar);
	  default:
	    return o->FN->FN_ttmenuitem->GetValue((ttmenuitem)o, which);
	}
    }
    return (ttany)0;
} 

 

        
static ttany dummy_GetValue_ttanytext(ttanytext o, ttuint which) {
    if (o && TTAssert(IS(ttanytext,o))) {
	switch (which) {
    
	  case ttanytext_text_len:
	    return (ttany)o->text_len;    
	  default:
	    return o->FN->FN_ttwindow->GetValue((ttwindow)o, which);
	}
    }
    return (ttany)0;
} 

 

 


            
     

    


static byte dummy_Sync(void) {
    return TRUE;
}
static byte dummy_Flush(void) {
    return TRUE;
}
static byte dummy_TimidFlush(void) {
    return TRUE;
}
static byte dummy_MainLoop(void) {
    return TRUE;
}
static void dummy_ExitMainLoop(void) {
}
static void dummy_DeleteListener(ttlistener o) {
}
static void dummy_Close(void) {
}
static int dummy_ConnectionFd(void) {
    return TT_NOFD;
}
static ttuint dummy_GetErrno(void) {
    return 0;
}
static ttuint dummy_GetErrnoDetail(void) {
    return 0;
}
static TT_CONST byte *dummy_StrError(ttuint E) {
    return "";
}
static TT_CONST byte *dummy_StrErrorDetail(ttuint E, ttuint S) {
    return "";
}


static ttfns dummy_InitHW(tthw *HW) {
    *HW = &dummy_TTFNs.HW;
    return &dummy_TTFNs;
}





                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             


/*
 * use default values for methods not implemented in hw_dummy_c
 *
 * dummy display target MUST IMPLEMENT ALL METHODS, even if as stubs.
 */











static s_ttfns dummy_TTFNs = {


  {
    order_ttobj,
    magicmask_ttobj,
    magic_ttobj,
    sizeof(s_ttobj),
    dummy_New_ttobj,
    dummy_Build_ttobj,
    dummy_Break_ttobj,
    dummy_Del_ttobj,
    dummy_GetValue_ttobj,
    dummy_SetValue_ttobj,
    dummy_ChangeValue_ttobj,
        
  },
  
  {
    order_ttevent,
    magicmask_ttevent,
    magic_ttevent,
    sizeof(s_ttevent),
    dummy_New_ttevent,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttobj,
    dummy_GetValue_ttevent,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,
    
  },
 
  {
    order_tteventbig,
    magicmask_tteventbig,
    magic_tteventbig,
    sizeof(s_tteventbig),
    (void *)dummy_New_ttevent,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttobj,
    dummy_GetValue_tteventbig,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,
    
    TFN_ttevent,

    
  },
 
  {
    order_ttlistener,
    magicmask_ttlistener,
    magic_ttlistener,
    sizeof(s_ttlistener),
    dummy_New_ttlistener,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    dummy_Del_ttlistener,
    dummy_GetValue_ttlistener,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    dummy_Create_ttlistener,

  },
 
  {
    order_ttcallback,
    magicmask_ttcallback,
    magic_ttcallback,
    sizeof(s_ttcallback),
    dummy_New_ttcallback,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    dummy_Del_ttcallback,
    dummy_GetValue_ttcallback,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    (void *)dummy_Create_ttlistener,

    TFN_ttlistener,
            
  },
 
  {
    order_ttcomponent,
    magicmask_ttcomponent,
    magic_ttcomponent,
    sizeof(s_ttcomponent),
    dummy_New_ttcomponent,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    dummy_Del_ttcomponent,
    dummy_GetValue_ttcomponent,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

  },
 
  {
    order_ttvisible,
    magicmask_ttvisible,
    magic_ttvisible,
    sizeof(s_ttvisible),
    dummy_New_ttvisible,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    dummy_GetValue_ttvisible,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    dummy_AddTo_ttvisible,
    dummy_Remove_ttvisible,
    dummy_SetVisible_ttvisible,
    dummy_SetTheme_ttvisible,
    dummy_BuiltinRepaint_ttvisible,
            
  },
 
  {
    order_ttnative,
    magicmask_ttnative,
    magic_ttnative,
    sizeof(s_ttnative),
    (void *)dummy_New_ttvisible,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    (void *)dummy_GetValue_ttvisible,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    dummy_GetRoot_ttnative,
    
  },
 
  {
    order_ttwidget,
    magicmask_ttwidget,
    magic_ttwidget,
    sizeof(s_ttwidget),
    dummy_New_ttwidget,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    dummy_GetValue_ttwidget,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    dummy_SetXYWH_ttwidget,
        
  },
 
  {
    order_ttlabel,
    magicmask_ttlabel,
    magic_ttlabel,
    sizeof(s_ttlabel),
    dummy_New_ttlabel,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    dummy_Del_ttlabel,
    dummy_GetValue_ttlabel,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,
    dummy_SetText_ttlabel,
    
  },
 
  {
    order_ttanybutton,
    magicmask_ttanybutton,
    magic_ttanybutton,
    sizeof(s_ttanybutton),
    dummy_New_ttanybutton,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    dummy_Del_ttanybutton,
    dummy_GetValue_ttanybutton,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,
    dummy_SetText_ttanybutton,
    
  },
 
  {
    order_ttbutton,
    magicmask_ttbutton,
    magic_ttbutton,
    sizeof(s_ttbutton),
    dummy_New_ttbutton,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttanybutton,
    (void *)dummy_GetValue_ttanybutton,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,
    (void *)dummy_SetText_ttanybutton,
    
    TFN_ttanybutton,
    dummy_SetPressed_ttbutton,
        
  },
 
  {
    order_ttcheckbutton,
    magicmask_ttcheckbutton,
    magic_ttcheckbutton,
    sizeof(s_ttcheckbutton),
    (void *)dummy_New_ttbutton,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttanybutton,
    (void *)dummy_GetValue_ttanybutton,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,
    (void *)dummy_SetText_ttanybutton,
    
    TFN_ttanybutton,
    (void *)dummy_SetPressed_ttbutton,
        
    TFN_ttbutton,

  },
 
  {
    order_ttradiobutton,
    magicmask_ttradiobutton,
    magic_ttradiobutton,
    sizeof(s_ttradiobutton),
    (void *)dummy_New_ttbutton,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttanybutton,
    (void *)dummy_GetValue_ttanybutton,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,
    (void *)dummy_SetText_ttanybutton,
    
    TFN_ttanybutton,
    (void *)dummy_SetPressed_ttbutton,
        
    TFN_ttbutton,

    TFN_ttcheckbutton,

  },
 
  {
    order_ttscrollbar,
    magicmask_ttscrollbar,
    magic_ttscrollbar,
    sizeof(s_ttscrollbar),
    (void *)dummy_New_ttanybutton,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttanybutton,
    (void *)dummy_GetValue_ttanybutton,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,
    (void *)dummy_SetText_ttanybutton,
    
    TFN_ttanybutton,

  },
 
  {
    order_ttbuttongroup,
    magicmask_ttbuttongroup,
    magic_ttbuttongroup,
    sizeof(s_ttbuttongroup),
    (void *)dummy_New_ttcomponent,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    (void *)dummy_GetValue_ttcomponent,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,

  },
 
  {
    order_ttwindow,
    magicmask_ttwindow,
    magic_ttwindow,
    sizeof(s_ttwindow),
    (void *)dummy_New_ttwidget,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    (void *)dummy_GetValue_ttwidget,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,

  },
 
  {
    order_ttframe,
    magicmask_ttframe,
    magic_ttframe,
    sizeof(s_ttframe),
    (void *)dummy_New_ttwidget,
    dummy_Build_ttframe,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    (void *)dummy_GetValue_ttwidget,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_ttscroller,
    magicmask_ttscroller,
    magic_ttscroller,
    sizeof(s_ttscroller),
    dummy_New_ttscroller,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    dummy_Del_ttscroller,
    dummy_GetValue_ttscroller,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_ttmenuitem,
    magicmask_ttmenuitem,
    magic_ttmenuitem,
    sizeof(s_ttmenuitem),
    (void *)dummy_New_ttvisible,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    (void *)dummy_GetValue_ttvisible,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,

  },
 
  {
    order_ttcheckmenuitem,
    magicmask_ttcheckmenuitem,
    magic_ttcheckmenuitem,
    sizeof(s_ttcheckmenuitem),
    (void *)dummy_New_ttvisible,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    (void *)dummy_GetValue_ttvisible,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

  },
 
  {
    order_ttradiomenuitem,
    magicmask_ttradiomenuitem,
    magic_ttradiomenuitem,
    sizeof(s_ttradiomenuitem),
    (void *)dummy_New_ttvisible,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    (void *)dummy_GetValue_ttvisible,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

    TFN_ttcheckmenuitem,

  },
 
  {
    order_ttmenu,
    magicmask_ttmenu,
    magic_ttmenu,
    sizeof(s_ttmenu),
    dummy_New_ttmenu,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    dummy_GetValue_ttmenu,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

  },
 
  {
    order_ttmenubar,
    magicmask_ttmenubar,
    magic_ttmenubar,
    sizeof(s_ttmenubar),
    (void *)dummy_New_ttvisible,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    dummy_Del_ttmenubar,
    (void *)dummy_GetValue_ttvisible,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    
  },
 
  {
    order_ttanytext,
    magicmask_ttanytext,
    magic_ttanytext,
    sizeof(s_ttanytext),
    dummy_New_ttanytext,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    dummy_GetValue_ttanytext,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_tttextfield,
    magicmask_tttextfield,
    magic_tttextfield,
    sizeof(s_tttextfield),
    (void *)dummy_New_ttanytext,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    (void *)dummy_GetValue_ttanytext,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
    TFN_ttwidget,

    TFN_ttwindow,

    TFN_ttanytext,

  },
 
  {
    order_tttextarea,
    magicmask_tttextarea,
    magic_tttextarea,
    sizeof(s_tttextarea),
    (void *)dummy_New_ttanytext,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    (void *)dummy_Del_ttcomponent,
    (void *)dummy_GetValue_ttanytext,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)dummy_AddTo_ttvisible,
    (void *)dummy_Remove_ttvisible,
    (void *)dummy_SetVisible_ttvisible,
    (void *)dummy_SetTheme_ttvisible,
    (void *)dummy_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)dummy_SetXYWH_ttwidget,
        
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
    (void *)dummy_New_ttcomponent,
    dummy_Build_tttheme,
    (void *)dummy_Break_ttobj,
    dummy_Del_tttheme,
    (void *)dummy_GetValue_ttcomponent,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    dummy_ComputeShapeButton_tttheme,
    
  },
 
  {
    order_ttapplication,
    magicmask_ttapplication,
    magic_ttapplication,
    sizeof(s_ttapplication),
    dummy_New_ttapplication,
    (void *)dummy_Build_ttobj,
    (void *)dummy_Break_ttobj,
    dummy_Del_ttapplication,
    (void *)dummy_GetValue_ttcomponent,
    (void *)dummy_SetValue_ttobj,
    (void *)dummy_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    
  },



  {

    dummy_Sync, 
    dummy_Flush, 
    dummy_TimidFlush, 
    dummy_MainLoop, 
    dummy_ExitMainLoop, 
    dummy_DeleteListener,    
    dummy_Close, 
    dummy_ConnectionFd, 
    dummy_GetErrno, 
    dummy_GetErrnoDetail, 
    dummy_StrError, 
    dummy_StrErrorDetail,   
  },
};



