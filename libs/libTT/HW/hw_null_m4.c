




/* This file was automatically generated from hw_null_c by m4/hw.m4h, do not edit! */


/*
 *  hw_null_m4.c  --  implementation of libTT methods on "null" target display
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
 * this file is hw_null_c and is preprocessed by m4 to produce hw_null_m4.c
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

#include "utils.h"
#include "inlines.h"
#include "seterrno.h"
#include "theme.h"

static s_ttfns null_TTFNs;



#include "Tw/Tw_defs.h"

/* ttobj */
static ttobj null_New_ttobj(ttfn_ttobj FN, ttobj o) {
    /*
     * if argument (o) is non-NULL, use it instead of allocating memory.
     * in this case, set o->oflags to ttobj_oflags_static to avoid
     * deallocating it later with TTFreeMem().
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
static ttobj null_Build_ttobj(ttobj o) {
    /*
     * this is necessary for all derived classes that do not overload FN->Build.
     * It is not actually useful for ttobj.
     */
    return o;
}
static void null_Break_ttobj(ttobj o) {
    /*
     * this is necessary for all derived classes that do not overload FN->Break.
     * It is not actually useful for ttobj.
     */
}
static void null_Del_ttobj(ttobj o) {
    if (o) {
	o->FN->Break(o);
	
	DropId(o);
	/* ttobj_refcount_alive flag should already be removed... but better clear it */
	if (!(o->refcount &= ~ttobj_refcount_alive) &&
	    !(o->oflags & ttobj_oflags_static))
	    
	    TTFreeMem(o);
    }
}

static ttbyte null_SetValue_ttobj(ttobj o, ttuint which, ttany value) {
    /* FIXME: finish this */
    return FALSE;
}
static ttbyte null_ChangeValue_ttobj(ttobj o, ttuint which, ttany nand_value, ttany xor_value) {
    /* FIXME: finish this */
    /* o->{which} = (o->{which} & ~nand_value) ^ xor_value; */
    return FALSE;
}



/* ttevent */
static ttevent null_New_ttevent(ttfn_ttevent FN, ttevent o) {
    if ((o = (ttevent)FN->FN_ttobj->New((ttfn_ttobj)FN, (ttobj)o))) {
	o->component = (ttcomponent)0;
	o->evtype = o->evcode = o->evflags = 0;
    }
    return o;
}


/* ttlistener */
static ttlistener null_New_ttlistener(ttfn_ttlistener FN, ttlistener o) {
    if ((o = (ttlistener)FN->FN_ttobj->New((ttfn_ttobj)FN, (ttobj)o))) {
	o->lflags = 0;
	o->component = (ttcomponent)0;
	o->prev = o->next = (ttlistener)0;
    }
    return o;
}
static void null_Del_ttlistener(ttlistener o) {
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

static ttlistener null_Create_ttlistener(ttcomponent c) {
    ttlistener o;
    if ((o = TNEW(ttlistener))) {
	if ((o->next = c->listeners))
	    o->next->prev = o;
	o->component = c;
	c->listeners = o;
    }
    return o;
}

static TT_CONST struct s_tavl empty_AVL;

/* ttcallback */
static ttcallback null_New_ttcallback(ttfn_ttcallback FN, ttcallback o) {
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
static void null_Del_ttcallback(ttcallback o) {
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
static ttcomponent null_New_ttcomponent(ttfn_ttcomponent FN, ttcomponent o) {
    if ((o = (ttcomponent)FN->FN_ttobj->New((ttfn_ttobj)FN, (ttobj)o))) {
	o->listeners = (ttlistener)0;
	o->callbacks = (ttcallback)0;
    }
    return o;
}
static void null_Del_ttcomponent(ttcomponent o) {
    if (o) {
	DispatchSimpleEvent(o, ttevent_evtype_del);

	DelAllListeners_ttcomponent(o);
	DelAllCallbacks_ttcomponent(o);
	
	o->FN->FN_ttobj->Del((ttobj)o);
    }
}

/* ttvisible */
static ttvisible null_New_ttvisible(ttfn_ttvisible FN, ttvisible o) {
    if ((o = (ttvisible)FN->FN_ttcomponent->New((ttfn_ttcomponent)FN, (ttcomponent)o))) {
	o->vflags = ttvisible_vflags_visible;
	o->prev = o->next = o->parent = o->child_first = o->child_last = NULL;
	o->theme = NULL;
	o->repaint = (ttvisible_repaint_fn)0;
    }
    return o;
}
static void null_AddTo_ttvisible(ttvisible o, ttvisible parent) {
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
static void null_SetVisible_ttvisible(ttvisible o, byte on_off) {
    if (on_off)
	o->vflags |= ttvisible_vflags_visible;
    else
	o->vflags &= ~ttvisible_vflags_visible;
}
static void null_Remove_ttvisible(ttvisible o) {
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
static void null_SetTheme_ttvisible(ttvisible o, tttheme theme) {
    tttheme oldtheme = myTheme(o);
    o->theme = theme;
    if (oldtheme != theme && o->parent && (o->vflags & ttvisible_vflags_visible))
	Expose_ttvisible(o, ttvisible_repaint_args_WHOLE);
}
static void null_Draw_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch,
				TT_CONST ttbyte *asciidata, TT_CONST ttfont *fontdata, TT_CONST ttattr *attrdata) {
    /* do-nothing */
}
static void null_BuiltinRepaint_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h) {
    /* do-nothing */
}

/* ttnative */
static ttnative null_GetRoot_ttnative(void) {
    return Create_ttnative((ttany)0);
}

/* ttwidget */
static ttwidget null_New_ttwidget(ttfn_ttwidget FN, ttwidget o) {
    if ((o = (ttwidget)FN->FN_ttvisible->New((ttfn_ttvisible)FN, (ttvisible)o))) {
	o->x = o->y = o->w = o->h = o->xl = o->yl = (ttuint)0;
    }
    return o;
}
static void null_SetXYWH_ttwidget(ttwidget o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h) {
    if (mask & 1)
	o->x = x;
    if (mask & 2)
	o->y = y;
    if (mask & 4)
	o->w = w;
    if (mask & 8)
	o->h = h;
}
static void null_SetXlYl_ttwidget(ttwidget o, ttuint mask, ttint xl, ttint yl) {
    if (mask & 1)
	o->xl = xl;
    if (mask & 2)
	o->yl = yl;
}


/* ttlabel */
static ttlabel null_New_ttlabel(ttfn_ttlabel FN, ttlabel o) {
    if ((o = (ttlabel)FN->FN_ttwidget->New((ttfn_ttwidget)FN, (ttwidget)o))) {
	o->text_len = 0;
	o->text = (ttfont *)0;
    }
    return o;
}
static void null_Del_ttlabel(ttlabel o) {
    if (o->text)
	TTFreeMem(o->text);
    o->text = (ttfont *)0;
    o->FN->FN_ttwidget->Del((ttwidget)o);
}
static byte null_SetText_ttlabel(ttlabel o, TT_CONST byte * text) {
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
static ttanybutton null_New_ttanybutton(ttfn_ttanybutton FN, ttanybutton o) {
    if ((o = (ttanybutton)FN->FN_ttwidget->New((ttfn_ttwidget)FN, (ttwidget)o))) {
	o->text = NULL;
	o->text_width = o->text_height = 0;
	TTWriteMem(o->text_shape, '\0', sizeof(o->text_shape));
	return o;
    }
    return NULL;
}
static void null_Del_ttanybutton(ttanybutton o) {
    if (o->text)
	TTFreeMem(o->text);
    o->text = (ttfont *)0;
    o->FN->FN_ttwidget->Del((ttwidget)o);
}
static byte null_SetText_ttanybutton(ttanybutton o, TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch) {
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

static ttbutton null_New_ttbutton(ttfn_ttbutton FN, ttbutton o) {
    if ((o = (ttbutton)FN->FN_ttanybutton->New((ttfn_ttanybutton)FN, (ttanybutton)o))) {
	if (Create_ttcallback((ttcomponent)o, ttevent_evtype_mouse,
			      ttcallback_lflags_after|ttcallback_lflags_arg0_event|ttcallback_lflags_function_plain,
			      (ttcallback_fn)DefaultMouseEvent_ttbutton, (ttany)0))
	    return o;
	
	TDEL(o);
    }
    return (ttbutton)0;
}

static void null_SetPressed_ttbutton(ttbutton o, byte pressed) {
}


/* ttcheckbutton */


/* ttradiobutton */


/* ttbuttongroup */


/* ttwindow */


/* ttframe */
static ttframe null_Build_ttframe(ttframe o) {
    o->vflags &= ~ttvisible_vflags_visible;
    o->FN->AddTo(o, (ttvisible)TFN_ttnative->GetRoot());
    return o;
}


/* ttscrollbar */


/* ttslider */


/* ttscroller */

static ttscroller null_New_ttscroller(ttfn_ttscroller FN, ttscroller o) {
    if ((o = (ttscroller)FN->FN_ttwindow->New((ttfn_ttwindow)FN, (ttwindow)o))) {
	o->scrollx = o->scrolly = NULL;
    }
    return o;
}
static void null_Del_ttscroller(ttscroller o) {
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

static ttmenu null_New_ttmenu(ttfn_ttmenu FN, ttmenu o) {
    if ((o = (ttmenu)FN->FN_ttmenuitem->New((ttfn_ttmenuitem)FN, (ttmenuitem)o))) {
	o->menubar = (ttmenubar)0;
	    return o;
	TDEL(o);
    }
    return o;
}

/* ttmenubar */

static void null_Del_ttmenubar(ttmenubar o) {
    if (TTD.Menubar == o)
	TTD.Menubar = (ttmenubar)0;
    o->FN->FN_ttvisible->Del((ttvisible)o);
}

/* ttanytext */

static ttanytext null_New_ttanytext(ttfn_ttanytext FN, ttanytext o) {
    if ((o = (ttanytext)FN->FN_ttwindow->New((ttfn_ttwindow)FN, (ttwindow)o))) {
	o->text = NULL;
	o->text_len = 0;
    }
    return o;
}


/* tttextfield */


/* tttextarea */


/* tttheme */

static tttheme null_Build_tttheme(tttheme o) {
    /* FIXME finish this */
    return o;
}
static void null_Del_tttheme(tttheme o) {
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

static ttattr * null_ComputeShapeButton_tttheme(ttbutton o, byte button_i, byte theme_i) {
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
static ttapplication null_New_ttapplication(ttfn_ttapplication FN, ttapplication o) {
    if ((o = (ttapplication)FN->FN_ttcomponent->New((ttfn_ttcomponent)FN, (ttcomponent)o))) {
	o->name = (ttbyte *)0;
    }
    return o;
}

static void null_Del_ttapplication(ttapplication o) {
    if (TTD.Application == o)
	TTD.Application = (ttapplication)0;
    if (o->name)
	TTFreeMem(o->name);
    o->name = (ttbyte *)0;
    o->FN->FN_ttcomponent->Del((ttcomponent)o);
}


/* common stuff: */




                            
static ttfn null_GetValue_ttobj(ttobj o, ttuint which, ttany *value) {
    if (which && which < ttobj_field_last && o && TTAssert(IS(ttobj,o))) {
	if (which > ttobj_field_first) switch (which) {
    
	  case ttobj_id:
	    *value = (ttany)o->id; return TTFN_ttopaque;    
	  case ttobj_refcount:
	    *value = (ttany)o->refcount; return TTFN_ttuint;    
	  case ttobj_oflags:
	    *value = (ttany)o->oflags; return TTFN_ttuint;            
	  case ttobj_user_data:
	    *value = (ttany)o->user_data; return TTFN_ttany;    
	  case ttobj_events_inprogress:
	    *value = (ttany)o->events_inprogress; return TTFN_ttuint;
	}
	
    }
    return (ttfn)0;
}  

                    
static ttfn null_GetValue_ttevent(ttevent o, ttuint which, ttany *value) {
    if (which && which < ttevent_field_last && o && TTAssert(IS(ttevent,o))) {
	if (which > ttevent_field_first) switch (which) {
    
	  case ttevent_callback:
	    *value = (ttany)OBJ2ID(o->callback); return TTFN_ttcallback;    
	  case ttevent_component:
	    *value = (ttany)OBJ2ID(o->component); return TTFN_ttcomponent;    
	  case ttevent_evtype:
	    *value = (ttany)o->evtype; return TTFN_ttuint;    
	  case ttevent_evcode:
	    *value = (ttany)o->evcode; return TTFN_ttuint;    
	  case ttevent_evflags:
	    *value = (ttany)o->evflags; return TTFN_ttuint;
	}
	else
	    return o->FN->FN_ttobj->GetValue((ttobj)o, which, value);
    }
    return (ttfn)0;
} 

                        
static ttfn null_GetValue_tteventbig(tteventbig o, ttuint which, ttany *value) {
    if (which && which < tteventbig_field_last && o && TTAssert(IS(tteventbig,o))) {
	if (which > tteventbig_field_first) switch (which) {
    
	  case tteventbig_x:
	    *value = (ttany)o->x; return TTFN_ttshort;    
	  case tteventbig_y:
	    *value = (ttany)o->y; return TTFN_ttshort;    
	  case tteventbig_w:
	    *value = (ttany)o->w; return TTFN_ttshort;    
	  case tteventbig_h:
	    *value = (ttany)o->h; return TTFN_ttshort;    
	  case tteventbig_len:
	    *value = (ttany)o->len; return TTFN_ttuint;    
	  case tteventbig_data:
	    *value = (ttany)(opaque)o->data; return TTFN_ttstring;
	}
	else
	    return o->FN->FN_ttevent->GetValue((ttevent)o, which, value);
    }
    return (ttfn)0;
} 

                
static ttfn null_GetValue_ttlistener(ttlistener o, ttuint which, ttany *value) {
    if (which && which < ttlistener_field_last && o && TTAssert(IS(ttlistener,o))) {
	if (which > ttlistener_field_first) switch (which) {
    
	  case ttlistener_lflags:
	    *value = (ttany)o->lflags; return TTFN_ttuint;    
	  case ttlistener_component:
	    *value = (ttany)OBJ2ID(o->component); return TTFN_ttcomponent;    
	  case ttlistener_prev:
	    *value = (ttany)OBJ2ID(o->prev); return TTFN_ttlistener;    
	  case ttlistener_next:
	    *value = (ttany)OBJ2ID(o->next); return TTFN_ttlistener;
	}
	else
	    return o->FN->FN_ttobj->GetValue((ttobj)o, which, value);
    }
    return (ttfn)0;
} 

                            
static ttfn null_GetValue_ttcallback(ttcallback o, ttuint which, ttany *value) {
    if (which && which < ttcallback_field_last && o && TTAssert(IS(ttcallback,o))) {
	if (which > ttcallback_field_first) switch (which) {
        
	  case ttcallback_event:
	    *value = (ttany)OBJ2ID(o->event); return TTFN_ttevent;    
	  case ttcallback_narg_component:
	    *value = (ttany)o->narg_component; return TTFN_ttuint;    
	  case ttcallback_narg_event:
	    *value = (ttany)o->narg_event; return TTFN_ttuint;    
	  case ttcallback_nargs:
	    *value = (ttany)o->nargs; return TTFN_ttuint;    
	  case ttcallback_function:
	    *value = (ttany)(opaque)o->function; return TTFN_ttfunction;    
	}
	else
	    return o->FN->FN_ttlistener->GetValue((ttlistener)o, which, value);
    }
    return (ttfn)0;
} 

        
static ttfn null_GetValue_ttcomponent(ttcomponent o, ttuint which, ttany *value) {
    if (which && which < ttcomponent_field_last && o && TTAssert(IS(ttcomponent,o))) {
	if (which > ttcomponent_field_first) switch (which) {
    
	  case ttcomponent_listeners:
	    *value = (ttany)OBJ2ID(o->listeners); return TTFN_ttlistener;    
	  case ttcomponent_callbacks:
	    *value = (ttany)OBJ2ID(o->callbacks); return TTFN_ttcallback;
	}
	else
	    return o->FN->FN_ttobj->GetValue((ttobj)o, which, value);
    }
    return (ttfn)0;
} 

                                
static ttfn null_GetValue_ttvisible(ttvisible o, ttuint which, ttany *value) {
    if (which && which < ttvisible_field_last && o && TTAssert(IS(ttvisible,o))) {
	if (which > ttvisible_field_first) switch (which) {
    
	  case ttvisible_vflags:
	    *value = (ttany)o->vflags; return TTFN_ttuint;    
	  case ttvisible_prev:
	    *value = (ttany)OBJ2ID(o->prev); return TTFN_ttvisible;    
	  case ttvisible_next:
	    *value = (ttany)OBJ2ID(o->next); return TTFN_ttvisible;    
	  case ttvisible_parent:
	    *value = (ttany)OBJ2ID(o->parent); return TTFN_ttvisible;    
	  case ttvisible_child_first:
	    *value = (ttany)OBJ2ID(o->child_first); return TTFN_ttvisible;    
	  case ttvisible_child_last:
	    *value = (ttany)OBJ2ID(o->child_last); return TTFN_ttvisible;    
	  case ttvisible_theme:
	    *value = (ttany)OBJ2ID(o->theme); return TTFN_tttheme;    
	  case ttvisible_repaint:
	    *value = (ttany)(opaque)o->repaint; return TTFN_ttfunction;
	}
	else
	    return o->FN->FN_ttcomponent->GetValue((ttcomponent)o, which, value);
    }
    return (ttfn)0;
} 

 

                        
static ttfn null_GetValue_ttwidget(ttwidget o, ttuint which, ttany *value) {
    if (which && which < ttwidget_field_last && o && TTAssert(IS(ttwidget,o))) {
	if (which > ttwidget_field_first) switch (which) {
    
	  case ttwidget_x:
	    *value = (ttany)o->x; return TTFN_ttshort;    
	  case ttwidget_y:
	    *value = (ttany)o->y; return TTFN_ttshort;    
	  case ttwidget_w:
	    *value = (ttany)o->w; return TTFN_ttshort;    
	  case ttwidget_h:
	    *value = (ttany)o->h; return TTFN_ttshort;    
	  case ttwidget_xl:
	    *value = (ttany)o->xl; return TTFN_ttint;    
	  case ttwidget_yl:
	    *value = (ttany)o->yl; return TTFN_ttint;
	}
	else
	    return o->FN->FN_ttvisible->GetValue((ttvisible)o, which, value);
    }
    return (ttfn)0;
} 

        
static ttfn null_GetValue_ttlabel(ttlabel o, ttuint which, ttany *value) {
    if (which && which < ttlabel_field_last && o && TTAssert(IS(ttlabel,o))) {
	if (which > ttlabel_field_first) switch (which) {
    
	  case ttlabel_text_len:
	    *value = (ttany)o->text_len; return TTFN_ttshort;    
	}
	else
	    return o->FN->FN_ttwidget->GetValue((ttwidget)o, which, value);
    }
    return (ttfn)0;
} 

                
static ttfn null_GetValue_ttanybutton(ttanybutton o, ttuint which, ttany *value) {
    if (which && which < ttanybutton_field_last && o && TTAssert(IS(ttanybutton,o))) {
	if (which > ttanybutton_field_first) switch (which) {
        
	  case ttanybutton_text_width:
	    *value = (ttany)o->text_width; return TTFN_ttshort;    
	  case ttanybutton_text_height:
	    *value = (ttany)o->text_height; return TTFN_ttshort;    
	}
	else
	    return o->FN->FN_ttwidget->GetValue((ttwidget)o, which, value);
    }
    return (ttfn)0;
} 

 

 

 

 

 

        
static ttfn null_GetValue_ttwindow(ttwindow o, ttuint which, ttany *value) {
    if (which && which < ttwindow_field_last && o && TTAssert(IS(ttwindow,o))) {
	if (which > ttwindow_field_first) switch (which) {
    
	  case ttwindow_title:
	    *value = (ttany)(opaque)o->title; return TTFN_ttstring;    
	  case ttwindow_menubar:
	    *value = (ttany)OBJ2ID(o->menubar); return TTFN_ttmenubar;
	}
	else
	    return o->FN->FN_ttwidget->GetValue((ttwidget)o, which, value);
    }
    return (ttfn)0;
} 

 

        
static ttfn null_GetValue_ttscroller(ttscroller o, ttuint which, ttany *value) {
    if (which && which < ttscroller_field_last && o && TTAssert(IS(ttscroller,o))) {
	if (which > ttscroller_field_first) switch (which) {
    
	  case ttscroller_scrollx:
	    *value = (ttany)OBJ2ID(o->scrollx); return TTFN_ttscrollbar;    
	  case ttscroller_scrolly:
	    *value = (ttany)OBJ2ID(o->scrolly); return TTFN_ttscrollbar;
	}
	else
	    return o->FN->FN_ttwindow->GetValue((ttwindow)o, which, value);
    }
    return (ttfn)0;
} 

     

 

 


    
static ttfn null_GetValue_ttmenu(ttmenu o, ttuint which, ttany *value) {
    if (which && which < ttmenu_field_last && o && TTAssert(IS(ttmenu,o))) {
	if (which > ttmenu_field_first) switch (which) {

    
	  case ttmenu_menubar:
	    *value = (ttany)OBJ2ID(o->menubar); return TTFN_ttmenubar;
	}
	else
	    return o->FN->FN_ttmenuitem->GetValue((ttmenuitem)o, which, value);
    }
    return (ttfn)0;
} 

 

        
static ttfn null_GetValue_ttanytext(ttanytext o, ttuint which, ttany *value) {
    if (which && which < ttanytext_field_last && o && TTAssert(IS(ttanytext,o))) {
	if (which > ttanytext_field_first) switch (which) {
    
	  case ttanytext_text_len:
	    *value = (ttany)o->text_len; return TTFN_ttshort;    
	}
	else
	    return o->FN->FN_ttwindow->GetValue((ttwindow)o, which, value);
    }
    return (ttfn)0;
} 

 

 


            
     

    
static ttfn null_GetValue_ttapplication(ttapplication o, ttuint which, ttany *value) {
    if (which && which < ttapplication_field_last && o && TTAssert(IS(ttapplication,o))) {
	if (which > ttapplication_field_first) switch (which) {
    
	  case ttapplication_name:
	    *value = (ttany)(opaque)o->name; return TTFN_ttstring;
	}
	else
	    return o->FN->FN_ttcomponent->GetValue((ttcomponent)o, which, value);
    }
    return (ttfn)0;
}


static byte null_Sync(void) {
    return TRUE;
}
static byte null_Flush(void) {
    return TRUE;
}
static byte null_TimidFlush(void) {
    return TRUE;
}
static byte null_MainLoop(void) {
    return TRUE;
}
static byte null_MainLoopOnce(ttbyte wait) {
    return TRUE;
}
static void null_ExitMainLoop(void) {
}
static void null_DeleteListener(ttlistener o) {
}
static void null_Close(void) {
}
static int null_ConnectionFd(void) {
    return TT_NOFD;
}
static ttuint null_GetErrno(void) {
    return 0;
}
static ttuint null_GetErrnoDetail(void) {
    return 0;
}
static TT_CONST byte *null_StrError(ttuint E) {
    return "";
}
static TT_CONST byte *null_StrErrorDetail(ttuint E, ttuint S) {
    return "";
}


ttfns _TT_null_InitHW(tthw *HW) {
    *HW = &null_TTFNs.HW;
    return &null_TTFNs;
}





                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             


/*
 * use default values for methods not implemented in hw_null_c
 *
 * null display target MUST IMPLEMENT ALL METHODS, even if as stubs.
 */











static s_ttfns null_TTFNs = {


  {
    order_ttobj,
    magicmask_ttobj,
    magic_ttobj,
    sizeof(s_ttobj),
    null_New_ttobj,
    null_Build_ttobj,
    null_Break_ttobj,
    null_Del_ttobj,
    null_GetValue_ttobj,
    null_SetValue_ttobj,
    null_ChangeValue_ttobj,
        
  },
  
  {
    order_ttevent,
    magicmask_ttevent,
    magic_ttevent,
    sizeof(s_ttevent),
    null_New_ttevent,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttobj,
    null_GetValue_ttevent,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,
    
  },
 
  {
    order_tteventbig,
    magicmask_tteventbig,
    magic_tteventbig,
    sizeof(s_tteventbig),
    (void *)null_New_ttevent,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttobj,
    null_GetValue_tteventbig,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,
    
    TFN_ttevent,

    
  },
 
  {
    order_ttlistener,
    magicmask_ttlistener,
    magic_ttlistener,
    sizeof(s_ttlistener),
    null_New_ttlistener,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    null_Del_ttlistener,
    null_GetValue_ttlistener,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    null_Create_ttlistener,

  },
 
  {
    order_ttcallback,
    magicmask_ttcallback,
    magic_ttcallback,
    sizeof(s_ttcallback),
    null_New_ttcallback,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    null_Del_ttcallback,
    null_GetValue_ttcallback,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    (void *)null_Create_ttlistener,

    TFN_ttlistener,
            
  },
 
  {
    order_ttcomponent,
    magicmask_ttcomponent,
    magic_ttcomponent,
    sizeof(s_ttcomponent),
    null_New_ttcomponent,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    null_Del_ttcomponent,
    null_GetValue_ttcomponent,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

  },
 
  {
    order_ttvisible,
    magicmask_ttvisible,
    magic_ttvisible,
    sizeof(s_ttvisible),
    null_New_ttvisible,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    null_GetValue_ttvisible,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    null_AddTo_ttvisible,
    null_Remove_ttvisible,
    null_SetVisible_ttvisible,
    null_SetTheme_ttvisible,
    null_Draw_ttvisible,
    null_BuiltinRepaint_ttvisible,
            
  },
 
  {
    order_ttnative,
    magicmask_ttnative,
    magic_ttnative,
    sizeof(s_ttnative),
    (void *)null_New_ttvisible,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    (void *)null_GetValue_ttvisible,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    null_GetRoot_ttnative,
    
  },
 
  {
    order_ttwidget,
    magicmask_ttwidget,
    magic_ttwidget,
    sizeof(s_ttwidget),
    null_New_ttwidget,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    null_GetValue_ttwidget,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    null_SetXYWH_ttwidget,
    null_SetXlYl_ttwidget,
                
  },
 
  {
    order_ttlabel,
    magicmask_ttlabel,
    magic_ttlabel,
    sizeof(s_ttlabel),
    null_New_ttlabel,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    null_Del_ttlabel,
    null_GetValue_ttlabel,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    null_SetText_ttlabel,
    
  },
 
  {
    order_ttanybutton,
    magicmask_ttanybutton,
    magic_ttanybutton,
    sizeof(s_ttanybutton),
    null_New_ttanybutton,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    null_Del_ttanybutton,
    null_GetValue_ttanybutton,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    null_SetText_ttanybutton,
    
  },
 
  {
    order_ttbutton,
    magicmask_ttbutton,
    magic_ttbutton,
    sizeof(s_ttbutton),
    null_New_ttbutton,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttanybutton,
    (void *)null_GetValue_ttanybutton,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    (void *)null_SetText_ttanybutton,
    
    TFN_ttanybutton,
    null_SetPressed_ttbutton,
        
  },
 
  {
    order_ttcheckbutton,
    magicmask_ttcheckbutton,
    magic_ttcheckbutton,
    sizeof(s_ttcheckbutton),
    (void *)null_New_ttbutton,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttanybutton,
    (void *)null_GetValue_ttanybutton,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    (void *)null_SetText_ttanybutton,
    
    TFN_ttanybutton,
    (void *)null_SetPressed_ttbutton,
        
    TFN_ttbutton,

  },
 
  {
    order_ttradiobutton,
    magicmask_ttradiobutton,
    magic_ttradiobutton,
    sizeof(s_ttradiobutton),
    (void *)null_New_ttbutton,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttanybutton,
    (void *)null_GetValue_ttanybutton,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    (void *)null_SetText_ttanybutton,
    
    TFN_ttanybutton,
    (void *)null_SetPressed_ttbutton,
        
    TFN_ttbutton,

    TFN_ttcheckbutton,

  },
 
  {
    order_ttscrollbar,
    magicmask_ttscrollbar,
    magic_ttscrollbar,
    sizeof(s_ttscrollbar),
    (void *)null_New_ttanybutton,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttanybutton,
    (void *)null_GetValue_ttanybutton,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,
    (void *)null_SetText_ttanybutton,
    
    TFN_ttanybutton,

  },
 
  {
    order_ttbuttongroup,
    magicmask_ttbuttongroup,
    magic_ttbuttongroup,
    sizeof(s_ttbuttongroup),
    (void *)null_New_ttcomponent,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    (void *)null_GetValue_ttcomponent,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,

  },
 
  {
    order_ttwindow,
    magicmask_ttwindow,
    magic_ttwindow,
    sizeof(s_ttwindow),
    (void *)null_New_ttwidget,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    null_GetValue_ttwindow,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,

  },
 
  {
    order_ttframe,
    magicmask_ttframe,
    magic_ttframe,
    sizeof(s_ttframe),
    (void *)null_New_ttwidget,
    null_Build_ttframe,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    (void *)null_GetValue_ttwindow,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_ttscroller,
    magicmask_ttscroller,
    magic_ttscroller,
    sizeof(s_ttscroller),
    null_New_ttscroller,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    null_Del_ttscroller,
    null_GetValue_ttscroller,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_ttmenuitem,
    magicmask_ttmenuitem,
    magic_ttmenuitem,
    sizeof(s_ttmenuitem),
    (void *)null_New_ttvisible,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    (void *)null_GetValue_ttvisible,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,

  },
 
  {
    order_ttcheckmenuitem,
    magicmask_ttcheckmenuitem,
    magic_ttcheckmenuitem,
    sizeof(s_ttcheckmenuitem),
    (void *)null_New_ttvisible,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    (void *)null_GetValue_ttvisible,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

  },
 
  {
    order_ttradiomenuitem,
    magicmask_ttradiomenuitem,
    magic_ttradiomenuitem,
    sizeof(s_ttradiomenuitem),
    (void *)null_New_ttvisible,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    (void *)null_GetValue_ttvisible,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

    TFN_ttcheckmenuitem,

  },
 
  {
    order_ttmenu,
    magicmask_ttmenu,
    magic_ttmenu,
    sizeof(s_ttmenu),
    null_New_ttmenu,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    null_GetValue_ttmenu,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

  },
 
  {
    order_ttmenubar,
    magicmask_ttmenubar,
    magic_ttmenubar,
    sizeof(s_ttmenubar),
    (void *)null_New_ttvisible,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    null_Del_ttmenubar,
    (void *)null_GetValue_ttvisible,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    
  },
 
  {
    order_ttanytext,
    magicmask_ttanytext,
    magic_ttanytext,
    sizeof(s_ttanytext),
    null_New_ttanytext,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    null_GetValue_ttanytext,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_tttextfield,
    magicmask_tttextfield,
    magic_tttextfield,
    sizeof(s_tttextfield),
    (void *)null_New_ttanytext,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    (void *)null_GetValue_ttanytext,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
    TFN_ttwidget,

    TFN_ttwindow,

    TFN_ttanytext,

  },
 
  {
    order_tttextarea,
    magicmask_tttextarea,
    magic_tttextarea,
    sizeof(s_tttextarea),
    (void *)null_New_ttanytext,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    (void *)null_Del_ttcomponent,
    (void *)null_GetValue_ttanytext,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)null_AddTo_ttvisible,
    (void *)null_Remove_ttvisible,
    (void *)null_SetVisible_ttvisible,
    (void *)null_SetTheme_ttvisible,
    (void *)null_Draw_ttvisible,
    (void *)null_BuiltinRepaint_ttvisible,
            
    TFN_ttvisible,
    (void *)null_SetXYWH_ttwidget,
    (void *)null_SetXlYl_ttwidget,
                
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
    (void *)null_New_ttcomponent,
    null_Build_tttheme,
    (void *)null_Break_ttobj,
    null_Del_tttheme,
    (void *)null_GetValue_ttcomponent,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    null_ComputeShapeButton_tttheme,
    
  },
 
  {
    order_ttapplication,
    magicmask_ttapplication,
    magic_ttapplication,
    sizeof(s_ttapplication),
    null_New_ttapplication,
    (void *)null_Build_ttobj,
    (void *)null_Break_ttobj,
    null_Del_ttapplication,
    null_GetValue_ttapplication,
    (void *)null_SetValue_ttobj,
    (void *)null_ChangeValue_ttobj,
        
    TFN_ttobj,

    TFN_ttcomponent,
    
  },



  {

    null_Sync, 
    null_Flush, 
    null_TimidFlush, 
    null_MainLoop, 
    null_MainLoopOnce, 
    null_ExitMainLoop,    
    null_DeleteListener, 
    null_Close, 
    null_ConnectionFd, 
    null_GetErrno, 
    null_GetErrnoDetail,    
    null_StrError, 
    null_StrErrorDetail,   
  },
};



