
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
static tteventbig Create_tteventbig
    (ttuint evtype, ttuint evcode, ttuint evflags, ttshort x, ttshort y,
     ttshort w, ttshort h, ttuint len, TT_CONST ttbyte *data) {
	
    tteventbig o;
    if ((o = TNEW(tteventbig))) {
	o->evtype = evtype;
	o->evcode = evcode;
	o->evflags = evflags;
	o->x = x;
	o->y = y;
	o->len = len;
	o->data = data;
    }
    return o;
}


/* ttcallback */
static ttcallback CreateA_ttcallback(ttcomponent c, ttevent ev, ttuint flags,
				    ttuint narg_component, ttuint narg_event, ttuint nargs,
				    ttcallback_fn function, ttany TT_CONST * args) {
    ttcallback o;
    byte ok = TRUE;
    opaque order;
    
    if (!c || !ev) {
	SetErrno(TT_EBAD_ARG, c ? 2 : 1);
	return (ttcallback)0;
    }
    
    if ((o = TNEW(ttcallback))) {
	/* grab the event, do not clone it */
	o->event = ev;
	ev->component = c;
	
	/* mask ONLY allowed flags! */
	o->lflags = flags & ttcallback_lflags_any;
	
	if ((order = Method2Order((void *)function)) != (opaque)-1 /* order_BAD */) {
	    o->lflags |= ttcallback_lflags_builtin;
	    o->function = (ttcallback_fn)order;
	} else
	    o->function = function;

	if (o->function) {
	    if ((o->nargs = nargs)) {
		o->narg_component = narg_component;
		o->narg_event = narg_event;

		if (args && (flags & ttcallback_lflags_args_swallow))
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
    }
    if (ok)
	ok = !!AddTo_ttcallback(o, c);
    if (!ok) {
	TDEL(o);
	o = (ttcallback)0;
    }
    return o;
}

/*
 * va_list is not a scalar type on some archs (Alpha),
 * so you cannot cast a scalar type to (va_list), as we would need
 * in CallMethod to invoke CreateV_ttcallback().
 * 
 * For this reason, we use a (va_list *) here.
 */
static ttcallback CreateV_ttcallback(ttcomponent c, ttevent ev, ttuint flags,
				    ttuint narg_component, ttuint narg_event, ttuint nargs,
				    ttcallback_fn function, va_list *vargs) {
    ttany *args = (ttany *)0;
    ttcallback l = (ttcallback)0;
    ttuint i;
    
    if (!nargs || (args = TTAllocMem(nargs * sizeof(ttany)))) {
	for (i = 0; i < nargs; i++)
	    args[i] = va_arg((*vargs), ttany);

	l = CreateA_ttcallback(c, ev, flags | ttcallback_lflags_args_swallow,
			      narg_component, narg_event, nargs, function, args);
	if (!l && nargs)
	    TTFreeMem(args);
    }
    return l;
}

tt_obj TTCreateL_ttcallback(tt_obj c, tt_obj ev, ttuint flags,
			    ttuint narg_component, ttuint narg_event, ttuint nargs,
			    ttcallback_fn function, ...) {
    ttcomponent o = ID2(ttcomponent,c);
    va_list ap;
    if (o) {
	va_start(ap, function);
	return (tt_obj)OBJ2ID(CreateV_ttcallback(o, ID2(ttevent,ev), flags,
						 narg_component, narg_event, nargs,
						 function, &ap));
	va_end(ap);
    }
    return (tt_obj)(opaque)0;
}


static ttcallback Create_ttcallback(ttcomponent c, ttuint evtype, ttuint lflags, ttcallback_fn function, ttany arg) {
    return CreateA_ttcallback
	(c, Create_ttevent(evtype, 0, 0), lflags,
	 (lflags & ttcallback_lflags_arg0_component) ? 0 : (ttuint)-1,
	 (lflags & ttcallback_lflags_arg0_event)  ? 0 : (ttuint)-1,
	 1, function, &arg);
}



/* ttnative */
static ttnative Create_ttnative(ttany id) {
    ttnative o; ttobj a;

    if ((a = FindNative(id)))
	return (ttnative)a;
    
    if ((o = TFN_ttnative->New(TFN_ttnative, (ttnative)0))) {
	o->native = id;
	return (ttnative)Build((ttobj)o);
    }
    return (ttnative)0;
}


/* ttlabel */
static ttlabel Create_ttlabel(TT_CONST byte * text) {
    ttlabel o;
    if ((o = TFN_ttlabel->New(TFN_ttlabel, (ttlabel)0))) {
	o->text_len = text ? strlen(text) : 0;
	if (!text || (o->text = CloneStr2TTFont(text, o->text_len)))
	    return (ttlabel)Build((ttobj)o);
	TDEL(o);
    }
    return (ttlabel)0;
}


/* ttanybutton */
static ttanybutton Create_ttanybutton(TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch) {
    ttanybutton o;
    ttfont *_text;
    if ((o = TFN_ttanybutton->New(TFN_ttanybutton, (ttanybutton)0))) {
	if (!text || !width || !height ||
	    (o->text = _text = TTAllocMem((ttint)width * height * sizeof(ttfont)))) {
	    
	    o->text_width = width;
	    o->text_height = height;
	    if (text && width) while (height--) {
		TTCopyMem(text, _text, width * sizeof(ttfont));
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
static ttbutton Create_ttbutton(TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch) {
    ttbutton o;
    ttfont *_text;
    if ((o = TFN_ttbutton->New(TFN_ttbutton, (ttbutton)0))) {
	if (!text || !width || !height ||
	    (o->text = _text = TTAllocMem((ttint)width * height * sizeof(ttfont)))) {
	    
	    o->text_width = width;
	    o->text_height = height;
	    if (text && width) while (height--) {
		TTCopyMem(text, _text, width * sizeof(ttfont));
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


/* ttbuttongroup */


/* ttwindow */


/* ttframe */


/* ttscrollbar */


/* ttslider */


/* ttscroller */


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


