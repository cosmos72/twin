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

#ifndef _TT_CREATE_H
#define _TT_CREATE_H


/* ttvector */
static ttvector Create1_ttvector(ttany value) {
    ttvector o;
    if ((o = TNEW(ttvector))) {
	if (o->Class->AddY(o, 0, 1, &value))
	    return o;
	TDEL(o);
    }
    return (ttvector)0;
}
static ttvector CreateA_ttvector(ttopaque value_n, TT_ARG_READ ttarg *values) {
    ttvector o;
    if ((o = TNEW(ttvector))) {
	if (TTAddA_ttvector((tt_obj)o->id, 0, value_n, values))
	    return o;
	TDEL(o);
    }
    return (ttvector)0;
}
static ttvector CreateV_ttvector(ttopaque value_n, va_list *values) {
    ttvector o;
    if ((o = TNEW(ttvector))) {
	if (TTAddV_ttvector((tt_obj)o->id, 0, value_n, values))
	    return o;
	TDEL(o);
    }
    return (ttvector)0;
}
static ttvector CreateL_ttvector(ttopaque value_n, ...) {
    ttvector o;
    va_list values;
    
    va_start(values, value_n);
    o = CreateV_ttvector(value_n, &values);
    va_end(values);

    return o;
}
static ttvector CreateR_ttvector(TT_ARG_READ ttvector v) {
    ttvector o;
    if (v && TTAssert(IS(ttvector,v)) && (o = TNEW(ttvector))) {
	if (TTAddR_ttvector((tt_obj)o->id, 0, (tt_obj)v->id))
	    return o;
	TDEL(o);
    }
    return (ttvector)0;
}
static ttvector CreateY_ttvector(ttopaque value_n, TT_ARG_READ ttany *values) {
    ttvector o;
    if ((o = TNEW(ttvector))) {
	if (o->Class->AddY(o, 0, value_n, values))
	    return o;
	TDEL(o);
    }
    return (ttvector)0;
}
tt_obj TTCreateL_ttvector(ttopaque value_n, ...) {
    tt_obj o;
    va_list values;
    
    va_start(values, value_n);
    o = (tt_obj)OBJ2ID(CreateV_ttvector(value_n, &values));
    va_end(values);

    return o;
}

/* ttevent */
static ttevent Create_ttevent(ttuint evtype, ttuint evcode, ttuint evflags) {
    ttevent o;
    if ((o = TNEW(ttevent))) {
	o->evtype = evtype;
	o->evcode = evcode;
	o->evflags = evflags;
    }
    return o;
}


/* tteventbig */
static tteventbig Create8_tteventbig
    (ttuint evtype, ttuint evcode, ttuint evflags,
     ttshort x, ttshort y, ttshort w, ttshort h,
     ttany value, ttany old_value, ttuint data_len, TT_CONST ttbyte *data) {
	
    tteventbig o;
    if ((o = TNEW(tteventbig))) {
	o->evtype = evtype;
	o->evcode = evcode;
	o->evflags = evflags;
	o->x = x;
	o->y = y;
	o->value = value;
	o->old_value = old_value;
	o->data_len = data_len;
	o->data = data;
    }
    return o;
}

static tteventbig Create4_tteventbig
    (ttuint evtype, ttuint evcode, ttuint evflags,
     ttshort x, ttshort y, ttshort w, ttshort h) {

    return Create8_tteventbig(evtype, evcode, evflags, x, y, w, h,
			      (ttany)0, (ttany)0, (ttuint)0, (TT_CONST ttbyte *)0);
}

static tteventbig Create4s4_tteventbig
    (ttuint evtype, ttuint evcode, ttuint evflags,
     ttany value, ttany old_value, ttuint len, TT_CONST ttbyte *data) {

    return Create8_tteventbig(evtype, evcode, evflags,
			      (ttshort)0, (ttshort)0, (ttshort)0, (ttshort)0,
			      value, old_value, len, data);
}


/* ttbitmask */
static ttbitmask Create_ttbitmask(ttany value) {
    ttbitmask o;
    
    if ((o = TNEW(ttbitmask))) {
	if (TTAdd_ttbitmask((tt_obj)o->id, value))
	    return o;
	TDEL(o);
    }
    return (ttbitmask)0;
}
static ttbitmask CreateB2_ttbitmask(TT_ARG_READ ttbitmask b) {
    ttbitmask o;
    
    if (b && TTAssert(IS(ttbitmask,b)) && (o = TNEW(ttbitmask))) {
	if (TTAddB_ttbitmask((tt_obj)o->id, (tt_obj)b->id)) {
	    return o;
	}
    }
    return (ttbitmask)0;
}
static ttbitmask CreateR_ttbitmask(TT_ARG_DIE ttvector v) {
    ttbitmask o;
    
    if (v && TTAssert(IS(ttvector,v)) && (o = TNEW(ttbitmask))) {
	if (TTAddR_ttbitmask((tt_obj)o->id, (tt_obj)v->id)) {
	    TDEL(v);
	    return o;
	}
	TDEL(o);
    }
    return (ttbitmask)0;
}
static ttbitmask CreateR2_ttbitmask(TT_ARG_READ ttvector v) {
    ttbitmask o;
    
    if (v && TTAssert(IS(ttvector,v)) && (o = TNEW(ttbitmask))) {
	if (TTAddR_ttbitmask((tt_obj)o->id, (tt_obj)v->id)) {
	    return o;
	}
	TDEL(o);
    }
    return (ttbitmask)0;
}

/* tteventmask */
static tteventmask CreateB_tteventmask(TT_ARG_DIE ttbitmask evtype_mask,
				       TT_ARG_DIE ttbitmask evcode_mask,
				       TT_ARG_DIE ttbitmask component_mask) {
    tteventmask o;
    if ((o = TNEW(tteventmask))) {
	o->evtype_mask = evtype_mask;
	o->evcode_mask = evcode_mask;
	o->component_mask = component_mask;
    }
    return o;
}
static tteventmask CreateB2_tteventmask(TT_ARG_READ ttbitmask evtype_mask,
					TT_ARG_READ ttbitmask evcode_mask,
					TT_ARG_READ ttbitmask component_mask) {

    ttbitmask evtype_mask2 = NULL, evcode_mask2 = NULL, component_mask2 = NULL;
    tteventmask o = NULL;
    
    if ((evtype_mask2 = CreateB2_ttbitmask(evtype_mask2)) &&
	(evcode_mask2 = CreateB2_ttbitmask(evcode_mask2)) &&
	(component_mask2 = CreateB2_ttbitmask(component_mask2)) &&
	(o = CreateB_tteventmask(evtype_mask, evcode_mask, component_mask))) {
	    
	return o;
    }
    if (evtype_mask2)
	TDEL(evtype_mask2);
    if (evcode_mask)
	TDEL(evcode_mask);
    if (component_mask)
	TDEL(component_mask);
    return NULL;
}


/* ttlistener */
static ttlistener CreateR_ttlistener(ttcomponent c, TT_ARG_DIE ttevent ev, ttuint lflags,
				     ttlistener_fn function,
				     ttopaque arg_component_n, ttopaque arg_event_n,
				     TT_ARG_DIE ttvector args) {
    ttlistener o = (ttlistener)0;
    opaque order;
    
    if (!c || !ev || !function || !args) {
	SetErrno(TT_EBAD_ARG, c ? ev ? function ? 7 : 4 : 2 : 1);
	return o;
    }
    
    if ((o = TNEW(ttlistener))) {
	
	/* swallow the event, do not clone it */
	o->event = ev;
	/* swallow the args, do not clone them */
	o->args = args;
	ev->component = c;
	    
	/* mask ONLY allowed lflags! */
	o->lflags |= lflags & ttlistener_lflags_any;
	
	if ((order = (opaque)TTGetByAddress_ttmethod((void *)function))) {
	    o->lflags |= ttlistener_lflags_ttmethod;
	    o->function = (ttlistener_fn)order;
	} else
	    o->function = function;
	
	o->arg_component_n = arg_component_n;
	o->arg_event_n = arg_event_n;
	
	o->Class->AddTo(o, c);
    }
    return o;
}

static ttlistener CreateE_ttlistener(ttcomponent c, ttevent ev, ttuint lflags,
				    ttlistener_fn function, ttany arg) {
    return CreateR_ttlistener
	(c, ev, lflags, function,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttopaque)-1,
	 (lflags & ttlistener_lflags_arg0_event) ? 0 : (ttopaque)-1,
	 Create1_ttvector(arg));
}

static ttlistener Create_ttlistener(ttcomponent c, ttuint evtype, ttuint lflags,
				    ttlistener_fn function, ttany arg) {
    return CreateR_ttlistener
	(c, Create_ttevent(evtype, 0, 0), lflags, function,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttopaque)-1,
	 (lflags & ttlistener_lflags_arg0_event) ? 0 : (ttopaque)-1,
	 Create1_ttvector(arg));
}

static ttlistener CreateDel_ttlistener(ttcomponent c, ttuint lflags,
				       ttlistener_fn function) {
    return CreateR_ttlistener
	(c, Create_ttevent(ttevent_evtype_del, 0, 0), lflags, function,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttopaque)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttopaque)-1 : 0,
	 Create1_ttvector(0));
}

static ttlistener CreateAskclose_ttlistener(ttcomponent c, ttuint lflags,
					    ttlistener_fn function) {
    return CreateR_ttlistener
	(c, Create_ttevent(ttevent_evtype_askclose, 0, 0), lflags, function,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttopaque)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttopaque)-1 : 0,
	 Create1_ttvector(0));
}

static ttlistener CreateKey_ttlistener(ttcomponent c, ttuint evcode, ttuint evflags,
				       ttuint lflags, ttlistener_fn function) {
    return CreateR_ttlistener
	(c, Create_ttevent(ttevent_evtype_key, evcode, evflags), lflags, function,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttopaque)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttopaque)-1 : 0,
	 Create1_ttvector(0));
}

static ttlistener CreateMouse_ttlistener(ttcomponent c, ttuint evcode, ttuint evflags,
					 ttuint lflags, ttlistener_fn function) {
    return CreateR_ttlistener
	(c, Create_ttevent(ttevent_evtype_mouse, evcode, evflags), lflags, function,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttopaque)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttopaque)-1 : 0,
	 Create1_ttvector(0));
}

static ttlistener CreateActivate_ttlistener(ttcomponent c, ttuint lflags,
					    ttlistener_fn function) {
    return CreateR_ttlistener
	(c, Create_ttevent(ttevent_evtype_activate, 0, 0), lflags, function,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttopaque)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttopaque)-1 : 0,
	 Create1_ttvector(0));
}

static ttlistener CreateChange_ttlistener(ttcomponent c, ttuint evcode, ttuint lflags,
					  ttlistener_fn function) {
    return CreateR_ttlistener
	(c, Create_ttevent(ttevent_evtype_change, evcode, 0), lflags, function,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttopaque)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttopaque)-1 : 0,
	 Create1_ttvector(0));
}

/*
 * this is tricky... the listener must be created
 * on component's ttdata, which may not yet exist!
 */
static ttlistener CreateKeyData_ttlistener(ttcomponent c, TT_ARG_READ ttbyte *key,
					   ttuint lflags, ttlistener_fn function) {
    ttdata d;
    ttopaque key_len;

    if (TTAssert(c && IS(ttcomponent,c)) && key) {
	key_len = TTLenStr(key);
	if ((d = FindByKey_ttdata(c->datas, key, key_len)) ||
	    (d = protected_Create_ttdata(c, key, key_len, (ttany)0))) {
	    
	    return CreateChange_ttlistener((ttcomponent)d, ttdata_data, lflags, function);
	}
	    
    }
    return (ttlistener)0;
}

static ttlistener CreateExpose_ttlistener(ttcomponent c, ttuint lflags,
					  ttlistener_fn function) {
    return CreateR_ttlistener
	(c, Create_ttevent(ttevent_evtype_expose, 0, 0), lflags, function,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttopaque)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttopaque)-1 : 0,
	 Create1_ttvector(0));
}


/* tttimer */
static tttimer CreateR_tttimer(ttcomponent c, ttuint lflags, ttlistener_fn function,
			       ttopaque arg_component_n, ttopaque arg_timer_n,
			       TT_ARG_DIE ttvector args,
			       ttany delay_t, ttany delay_f) {
    tttimer o = (tttimer)0;
    opaque order;
    
    if (!c || !function || !args) {
	SetErrno(TT_EBAD_ARG, c ? function ? 6 : 3 : 1);
	return o;
    }
    
    if ((o = TNEW(tttimer))) {
	
	/* mask ONLY allowed lflags! */
	o->lflags |= lflags & ttlistener_lflags_any;
	
	if ((order = (opaque)TTGetByAddress_ttmethod((void *)function))) {
	    o->lflags |= ttlistener_lflags_ttmethod;
	    o->function = (ttlistener_fn)order;
	} else
	    o->function = function;

	o->arg_component_n = arg_component_n;
	o->arg_event_n = arg_timer_n;
	/* swallow the args, do not clone them */
	o->args = args;
	
	GetNow();
	o->delay_t = TTD.TNow + delay_t;
	o->delay_f = TTD.FNow + delay_f;
	
	if (c && TTAssert(IS(ttcomponent,c)))
	    o->Class->AddTo(o, c);
	
	if (lflags & tttimer_lflags_enabled)
	    o->Class->SetEnabled(o, TT_TRUE);
	
    }
    return o;
}

static tttimer Create_tttimer(ttcomponent c, ttuint lflags, ttlistener_fn function, ttany arg,
			      ttany delay_t, ttany delay_f) {
    return CreateR_tttimer(c, lflags, function,
			   (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttopaque)-1,
			   (lflags & ttlistener_lflags_arg0_timer) ? 0 : (ttopaque)-1,
			   Create1_ttvector(arg), delay_t, delay_f);
}


/* ttdata */



/* ttboxlayout */
static ttboxlayout Create_ttboxlayout(ttany orientation) {
    ttboxlayout o;
    
    if ((o = TNEW_NOBUILD(ttboxlayout))) {
	o->orientation = orientation;
	return (ttboxlayout)Build((ttobject)o);
    }
    return o;
}


/* ttnative */
static ttnative Create_ttnative(ttany id) {
    ttnative o;
    ttobject a;

    if ((a = FindNative(id)))
	return (ttnative)a;
    
    if ((o = TNEW_NOBUILD(ttnative))) {
	o->native = id;
	return (ttnative)Build((ttobject)o);
    }
    return o;
}


/* ttlabel */
static ttlabel Create_ttlabel(TT_CONST ttbyte * text) {
    ttlabel o;
    if ((o = TNEW_NOBUILD(ttlabel))) {
	if (!text || (o->text = TTCloneStrL2Font(text, 1 + (o->text_len = TTLenStr(text)))))
	    return (ttlabel)Build((ttobject)o);
	TDEL(o);
    }
    return (ttlabel)0;
}


/* ttlabel */
static ttprogressbar Create_ttprogressbar(ttbyte orientation) {
    ttprogressbar o;
    if ((o = TNEW_NOBUILD(ttprogressbar))) {
	o->orientation = orientation;
	return (ttprogressbar)Build((ttobject)o);
    }
    return (ttprogressbar)0;
}


/* ttbuttongroup */


/* ttanybutton */
static ttanybutton Create_ttanybutton(ttshort width, ttshort height, ttshort pitch, TT_CONST ttbyte * text) {
    ttanybutton o;
    ttattr *_text;
    if ((o = TNEW_NOBUILD(ttanybutton))) {
	if (!text || !width || !height ||
	    (o->text = _text = TTAllocMem((ttint)width * height * sizeof(ttattr)))) {
	    
	    o->text_width = width;
	    o->text_height = height;
	    if (text && width) while (height--) {
		TTCopyStrL2Attr(text, _text, width, o->col);
		text += pitch;
		_text += width;
	    }
	    return (ttanybutton)Build((ttobject)o);
	}
	TDEL(o);
    }
    return (ttanybutton)0;
}


/* ttbutton */
static ttbutton Create_ttbutton(ttshort width, ttshort height, ttshort pitch, TT_CONST ttbyte * text) {
    ttbutton o;
    ttattr *_text;
    if ((o = TNEW_NOBUILD(ttbutton))) {
	if (!text || !width || !height ||
	    (o->text = _text = TTAllocMem((ttint)width * height * sizeof(ttattr)))) {
	    
	    o->text_width = width;
	    o->text_height = height;
	    if (text && width) while (height--) {
		TTCopyStrL2Attr(text, _text, width, o->col);
		text += pitch;
		_text += width;
	    }
	    return (ttbutton)Build((ttobject)o);
	}
	TDEL(o);
    }
    return (ttbutton)0;
}
    

/* ttcheckbutton */
static ttcheckbutton Create_ttcheckbutton(ttshort width, ttshort height, ttshort pitch, TT_CONST ttbyte * text) {
    ttcheckbutton o;
    ttattr *_text;
    if ((o = TNEW_NOBUILD(ttcheckbutton))) {
	if (!text || !width || !height ||
	    (o->text = _text = TTAllocMem((ttint)width * height * sizeof(ttattr)))) {
	    
	    o->text_width = width;
	    o->text_height = height;
	    if (text && width) while (height--) {
		TTCopyStrL2Attr(text, _text, width, o->col);
		text += pitch;
		_text += width;
	    }
	    return (ttcheckbutton)Build((ttobject)o);
	}
	TDEL(o);
    }
    return (ttcheckbutton)0;
}


/* ttradiobutton */
static ttradiobutton Create_ttradiobutton(ttshort width, ttshort height, ttshort pitch, TT_CONST ttbyte * text) {
    ttradiobutton o;
    ttattr *_text;
    if ((o = TNEW_NOBUILD(ttradiobutton))) {
	if (!text || !width || !height ||
	    (o->text = _text = TTAllocMem((ttint)width * height * sizeof(ttattr)))) {
	    
	    o->text_width = width;
	    o->text_height = height;
	    if (text && width) while (height--) {
		TTCopyStrL2Attr(text, _text, width, o->col);
		text += pitch;
		_text += width;
	    }
	    return (ttradiobutton)Build((ttobject)o);
	}
	TDEL(o);
    }
    return (ttradiobutton)0;
}


/* ttscrollbar */
static ttscrollbar Create1_ttscrollbar(ttbyte orientation) {
    ttscrollbar o;

    if ((o = TNEW_NOBUILD(ttscrollbar))) {
	o->orientation = orientation;
	
	return (ttscrollbar)Build((ttobject)o);
    }
    return (ttscrollbar)0;
}
static ttscrollbar Create4_ttscrollbar(ttbyte orientation, ttint size, ttint real_size, ttint view_size) {
    ttscrollbar o;

    if ((o = TNEW_NOBUILD(ttscrollbar))) {
	if (orientation == ttanyscroll_orientation_x)
	    o->orientation = ttanyscroll_orientation_x;
	else
	    o->orientation = ttanyscroll_orientation_y;
	if (size >= 0)
	    o->size = size;
	if (real_size >= 0)
	    o->real_size = real_size;
	if (view_size >= 0)
	    o->view_size = view_size;
	
	return (ttscrollbar)Build((ttobject)o);
    }
    return (ttscrollbar)0;
}


/* ttslider */
static ttslider Create_ttslider(ttbyte orientation, ttint real_size) {
    ttslider o;

    if ((o = TNEW_NOBUILD(ttslider))) {
	o->orientation = orientation;
	o->real_size = real_size;
	
	return (ttslider)Build((ttobject)o);
    }
    return (ttslider)0;
}

/* ttscrollpane */
static ttscrollpane Create_ttscrollpane(ttshort w, ttshort h) {
    ttscrollpane o;
    
    if ((o = TNEW_NOBUILD(ttscrollpane))) {
	/* let o keep its scrollbars in sync */
	o->Class->SetWH(o, w, h);
	
	return (ttscrollpane)Build((ttobject)o);
    }
    return (ttscrollpane)0;
}


/* ttwindow */


/* ttframe */
static ttframe Create_ttframe(TT_CONST ttbyte * title) {
    ttframe o;
    if ((o = TNEW_NOBUILD(ttframe))) {
	if (!title || (o->title = TTCloneStr(title)))
	    return (ttframe)Build((ttobject)o);
	TDEL(o);
    }
    return (ttframe)0;
}


/* ttscrollbar */


/* ttslider */


/* ttscroller */
static ttscroller Create_ttscroller(ttshort w, ttshort h) {
    ttscroller o;
    
    if ((o = TNEW_NOBUILD(ttscroller))) {
	/* could FIRE_EVENT here, but this is more compact */
	TTSetWH_ttwidget((tt_obj)o->id, w, h);
	return (ttscroller)Build((ttobject)o);
    }
    return (ttscroller)0;
}


/* ttmenuitem */


/* ttcheckmenuitem */


/* ttradiomenuitem */


/* ttmenu */


/* ttmenubar */


/* ttanytext */


/* tttextfield */


/* tttextarea */


/* tttheme */


/* ttapplication */
TT_INLINE ttapplication Create_ttapplication(TT_CONST ttbyte * name) {
    ttbyte *apname;
    
    if (!name)
	return (ttapplication)0;
    
    if (TTD.Application || (TTD.Application = TNEW_NOBUILD(ttapplication))) {

	if ((apname = TTCloneStr(name))) {
	    if (TTD.Application->name)
		TTFreeMem(TTD.Application->name);
	    TTD.Application->name = apname;

	    return (ttapplication)Build((ttobject)TTD.Application);
	}
	TDEL(TTD.Application);
    }
    return TTD.Application;
}


#endif /* _TT_CREATE_H */


