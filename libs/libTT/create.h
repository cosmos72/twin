
#ifndef _TT_CREATE_H
#define _TT_CREATE_H


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
     ttany value, ttany old_value, ttuint len, TT_CONST ttbyte *data) {
	
    tteventbig o;
    if ((o = TNEW(tteventbig))) {
	o->evtype = evtype;
	o->evcode = evcode;
	o->evflags = evflags;
	o->x = x;
	o->y = y;
	o->value = value;
	o->old_value = old_value;
	o->len = len;
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
static ttbitmask CreateA_ttbitmask(ttopaque n, TT_ARG_READ ttany *value) {
    ttbitmask o;
    
    if ((o = TNEW(ttbitmask))) {
	if (TTAddA_ttbitmask((tt_obj)o->id, n, value))
	    return o;
	TDEL(o);
    }
    return (ttbitmask)0;
}


/* tteventmask */
static tteventmask Create_tteventmask(TT_ARG_DIE ttbitmask evtype_mask,
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


/* ttlistener */
static ttlistener CreateA_ttlistener(ttcomponent c, ttevent ev, ttuint lflags,
				    ttuint narg_component, ttuint narg_event, ttuint nargs,
				    ttlistener_fn function, TT_CONST ttany * args) {
    ttlistener o;
    ttopaque order;
    ttbyte ok = TT_FALSE;
    
    if (!c || !ev) {
	SetErrno(TT_EBAD_ARG, c ? 2 : 1);
	return (ttlistener)0;
    }
    
    if (function && (o = TNEW(ttlistener))) {
	/* grab the event, do not clone it */
	o->event = ev;
	ev->component = c;
	
	/* mask ONLY allowed lflags! */
	o->lflags |= lflags & ttlistener_lflags_any;
	
	if ((order = Method2Order((void *)function))) {
	    o->lflags |= ttlistener_lflags_builtin;
	    o->function = (ttlistener_fn)order;
	} else
	    o->function = function;

	if (o->function) {
	    if ((o->nargs = nargs)) {
		o->narg_component = narg_component;
		o->narg_event = narg_event;

		if (args && (lflags & ttlistener_lflags_args_swallow)) {
		    o->args = (ttany *)args;
		    ok = TT_TRUE;
		} else if ((o->args = (ttany *)TTAllocMem(nargs * sizeof(ttany)))) {
		    if (args)
			TTCopyMem(args, o->args, nargs * sizeof(ttany));
		    else
			TTWriteMem(o->args, '\0', nargs * sizeof(ttany));
		    ok = TT_TRUE;
		}
	    }
	}
	
	if (ok)
	    o->FN->AddTo(o, c);
	else {
	    TDEL(o);
	    o = (ttlistener)0;
	}
    }
    return o;
}

/*
 * va_list is not a scalar type on some archs (Alpha),
 * so you cannot cast a scalar type to (va_list), as we would need
 * in CallMethod to invoke CreateV_ttlistener().
 * 
 * For this reason, we use a (va_list *) here.
 */
static ttlistener CreateV_ttlistener(ttcomponent c, ttevent ev, ttuint lflags,
				    ttuint narg_component, ttuint narg_event, ttuint nargs,
				    ttlistener_fn function, va_list *vargs) {
    ttany *args = (ttany *)0;
    ttlistener l = (ttlistener)0;
    ttuint i;
    
    if (ev && TTAssert(IS(ttevent,ev)) &&
	(!nargs || (args = TTAllocMem(nargs * sizeof(ttany))))) {
	
	for (i = 0; i < nargs; i++)
	    args[i] = va_arg((*vargs), ttany);

	l = CreateA_ttlistener(c, ev, lflags | ttlistener_lflags_args_swallow,
			      narg_component, narg_event, nargs, function, args);
	if (!l && nargs)
	    TTFreeMem(args);
    }
    return l;
}

tt_obj TTCreateL_ttlistener(tt_obj c, tt_obj ev, ttuint lflags,
			    ttuint narg_component, ttuint narg_event, ttuint nargs,
			    ttlistener_fn function, ...) {
    ttcomponent o = ID2(ttcomponent,c);
    va_list ap;
    if (o) {
	va_start(ap, function);
	return (tt_obj)OBJ2ID(CreateV_ttlistener(o, ID2(ttevent,ev), lflags,
						 narg_component, narg_event, nargs,
						 function, &ap));
	va_end(ap);
    }
    return (tt_obj)(opaque)0;
}


static ttlistener Create_ttlistener(ttcomponent c, ttuint evtype, ttuint lflags,
				    ttlistener_fn function, ttany arg) {
    return CreateA_ttlistener
	(c, Create_ttevent(evtype, 0, 0), lflags,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttuint)-1,
	 (lflags & ttlistener_lflags_arg0_event) ? 0 : (ttuint)-1,
	 1, function, &arg);
}

static ttlistener CreateDel_ttlistener(ttcomponent c, ttuint lflags,
				       ttlistener_fn function) {
    return CreateA_ttlistener
	(c, Create_ttevent(ttevent_evtype_del, 0, 0), lflags,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttuint)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttuint)-1 : 0,
	 1, function, (ttany *)0);
}

static ttlistener CreateAskclose_ttlistener(ttcomponent c, ttuint lflags,
					    ttlistener_fn function) {
    return CreateA_ttlistener
	(c, Create_ttevent(ttevent_evtype_askclose, 0, 0), lflags,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttuint)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttuint)-1 : 0,
	 1, function, (ttany *)0);
}

static ttlistener CreateKey_ttlistener(ttcomponent c, ttuint evcode, ttuint evflags,
					    ttuint lflags, ttlistener_fn function) {
    return CreateA_ttlistener
	(c, Create_ttevent(ttevent_evtype_key, evcode, evflags), lflags,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttuint)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttuint)-1 : 0,
	 1, function, (ttany *)0);
}

static ttlistener CreateMouse_ttlistener(ttcomponent c, ttuint evcode, ttuint evflags,
					    ttuint lflags, ttlistener_fn function) {
    return CreateA_ttlistener
	(c, Create_ttevent(ttevent_evtype_mouse, evcode, evflags), lflags,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttuint)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttuint)-1 : 0,
	 1, function, (ttany *)0);
}

static ttlistener CreateActivate_ttlistener(ttcomponent c, ttuint lflags,
					    ttlistener_fn function) {
    return CreateA_ttlistener
	(c, Create_ttevent(ttevent_evtype_activate, 0, 0), lflags,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttuint)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttuint)-1 : 0,
	 1, function, (ttany *)0);
}

static ttlistener CreateChange_ttlistener(ttcomponent c, ttuint evcode, ttuint lflags,
					  ttlistener_fn function) {
    return CreateA_ttlistener
	(c, Create_ttevent(ttevent_evtype_change, evcode, 0), lflags,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttuint)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttuint)-1 : 0,
	 1, function, (ttany *)0);
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
    return CreateA_ttlistener
	(c, Create_ttevent(ttevent_evtype_expose, 0, 0), lflags,
	 (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttuint)-1,
	 (lflags & ttlistener_lflags_arg0_component) ? (ttuint)-1 : 0,
	 1, function, (ttany *)0);
}


/* tttimer */
static tttimer CreateA_tttimer(ttcomponent c, ttuint lflags,
			       ttuint narg_component, ttuint narg_timer, ttuint nargs,
			       ttlistener_fn function, TT_CONST ttany * args,
			       ttuint delay_t, ttuint delay_f) {
    tttimer o;
    opaque order;
    ttbyte ok = TRUE;
    
    if ((o = TNEW(tttimer))) {

	/* mask ONLY allowed lflags! */
	o->lflags |= lflags & ttlistener_lflags_any;
	
	if ((order = Method2Order((void *)function))) {
	    o->lflags |= ttlistener_lflags_builtin;
	    o->function = (ttlistener_fn)order;
	} else
	    o->function = function;

	if (o->function) {
	    if ((o->nargs = nargs)) {
		o->narg_component = narg_component;
		o->narg_event = narg_timer;

		if (args && (lflags & ttlistener_lflags_args_swallow))
		    o->args = (ttany *)args;
		else if ((o->args = (ttany *)TTAllocMem(nargs * sizeof(ttany)))) {
		    if (args)
			TTCopyMem(args, o->args, nargs * sizeof(ttany));
		    else
			TTWriteMem(o->args, '\0', nargs * sizeof(ttany));
		} else
		    ok = FALSE;
	    }
	} else
	    ok = FALSE;
	
	if (ok) {
	    GetNow();
	    o->t = TTD.TNow + (time_t)delay_t;
	    o->f = TTD.FNow + (frac_t)delay_f;
	
	    if (c && TTAssert(IS(ttcomponent,c)))
		o->FN->AddTo(o, c);

	    if (lflags & tttimer_lflags_enabled)
		o->FN->SetEnabled(o, TT_TRUE);
	
	    return o;
	}
	
	TDEL(o);
    }
    return NULL;
}
static tttimer Create_tttimer(ttcomponent c, ttuint lflags, ttlistener_fn function, ttany arg,
			      ttuint delay_t, ttuint delay_f) {
    return CreateA_tttimer(c, lflags,
			   (lflags & ttlistener_lflags_arg0_component) ? 0 : (ttuint)-1,
			   (lflags & ttlistener_lflags_arg0_timer) ? 0 : (ttuint)-1,
			   1, function, &arg, delay_t, delay_f);
}


/* ttdata */



/* ttnative */
static ttnative Create_ttnative(ttany id) {
    ttnative o;
    ttobj a;

    if ((a = FindNative(id)))
	return (ttnative)a;
    
    if ((o = TFN_ttnative->New(TFN_ttnative, (ttnative)0))) {
	o->native = id;
	return (ttnative)Build((ttobj)o);
    }
    return o;
}


/* ttlabel */
static ttlabel Create_ttlabel(TT_CONST ttbyte * text) {
    ttlabel o;
    if ((o = TNEW_NOBUILD(ttlabel))) {
	if (!text || (o->text = TTCloneStrL2Font(text, 1 + (o->text_len = TTLenStr(text)))))
	    return (ttlabel)Build((ttobj)o);
	TDEL(o);
    }
    return (ttlabel)0;
}


/* ttlabel */
static ttprogressbar Create_ttprogressbar(ttbyte orientation) {
    ttprogressbar o;
    if ((o = TNEW_NOBUILD(ttprogressbar))) {
	o->orientation = orientation;
	return (ttprogressbar)Build((ttobj)o);
    }
    return (ttprogressbar)0;
}


/* ttbuttongroup */


/* ttanybutton */
static ttanybutton Create_ttanybutton(TT_CONST ttbyte * text, ttshort width, ttshort height, ttshort pitch) {
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
	    return (ttanybutton)Build((ttobj)o);
	}
	TDEL(o);
    }
    return (ttanybutton)0;
}


/* ttbutton */
static ttbutton Create_ttbutton(TT_CONST ttbyte * text, ttshort width, ttshort height, ttshort pitch) {
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
	    return (ttbutton)Build((ttobj)o);
	}
	TDEL(o);
    }
    return (ttbutton)0;
}
    

/* ttcheckbutton */
static ttcheckbutton Create_ttcheckbutton(TT_CONST ttbyte * text, ttshort width, ttshort height, ttshort pitch) {
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
	    return (ttcheckbutton)Build((ttobj)o);
	}
	TDEL(o);
    }
    return (ttcheckbutton)0;
}


/* ttradiobutton */
static ttradiobutton Create_ttradiobutton(TT_CONST ttbyte * text, ttshort width, ttshort height, ttshort pitch) {
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
	    return (ttradiobutton)Build((ttobj)o);
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
	
	return (ttscrollbar)Build((ttobj)o);
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
	
	return (ttscrollbar)Build((ttobj)o);
    }
    return (ttscrollbar)0;
}


/* ttslider */
static ttslider Create_ttslider(ttbyte orientation, ttint real_size) {
    ttslider o;

    if ((o = TNEW_NOBUILD(ttslider))) {
	o->orientation = orientation;
	o->real_size = real_size;
	
	return (ttslider)Build((ttobj)o);
    }
    return (ttslider)0;
}

/* ttscrollpane */
static ttscrollpane Create_ttscrollpane(ttshort w, ttshort h) {
    ttscrollpane o;
    
    if ((o = TNEW_NOBUILD(ttscrollpane))) {
	/* let o keep its scrollbars in sync */
	o->FN->SetWH(o, w, h);
	
	return (ttscrollpane)Build((ttobj)o);
    }
    return (ttscrollpane)0;
}


/* ttwindow */


/* ttframe */
static ttframe Create_ttframe(TT_CONST ttbyte * title) {
    ttframe o;
    if ((o = TNEW_NOBUILD(ttframe))) {
	if (!title || (o->title = TTCloneStr(title)))
	    return (ttframe)Build((ttobj)o);
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
	return (ttscroller)Build((ttobj)o);
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


#endif /* _TT_CREATE_H */


