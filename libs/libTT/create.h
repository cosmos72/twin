
#ifndef _TT_CREATE_H
#define _TT_CREATE_H


/* ttobj */
TT_INLINE void Ref_ttobj(ttobj o) {
    if (o)
	o->refcount++;
}

TT_INLINE void Unref_ttobj(ttobj o) {
    if (o && !--o->refcount)
	Del(o);
}



/* ttevent */
TT_INLINE ttevent Create_ttevent(ttuint evtype, ttuint evcode, ttuint evflags) {
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

static ttcallback CreateV_ttcallback(ttcomponent c, ttevent ev, ttuint flags,
				    ttuint narg_component, ttuint narg_event, ttuint nargs,
				    ttcallback_fn function, va_list vargs) {
    ttany *args = (ttany *)0;
    ttcallback l = (ttcallback)0;
    ttuint i;
    
    if (!nargs || (args = TTAllocMem(nargs * sizeof(ttany)))) {
	for (i = 0; i < nargs; i++)
	    args[i] = va_arg(vargs, ttany);

	l = CreateA_ttcallback(c, ev, flags | ttcallback_lflags_args_swallow,
			      narg_component, narg_event, nargs, function, args);
	if (!l && nargs)
	    TTFreeMem(args);
    }
    return l;
}

tt_obj TTCreateL_ttcallback(tt_obj a1, tt_obj a2, ttuint a3, ttuint a4, ttuint a5, ttuint a6, ttcallback_fn a7, ...) {
    ttcomponent o = ID2(ttcomponent,a1);
    va_list a8;
    if (o) {
	va_start(a8, a7);
	return (tt_obj)OBJ2ID(CreateV_ttcallback(o, ID2(ttevent,a2), a3, a4, a5, a6, a7, a8));
	va_end(a8);
    }
    return (tt_obj)(opaque)0;
}


TT_INLINE ttcallback Create_ttcallback(ttcomponent c, ttuint evtype, ttuint lflags, ttcallback_fn function, ttany arg) {
    return CreateA_ttcallback
	(c, Create_ttevent(evtype, 0, 0), lflags,
	 (lflags & ttcallback_lflags_arg0_component) ? 0 : (ttuint)-1,
	 (lflags & ttcallback_lflags_arg0_event)  ? 0 : (ttuint)-1,
	 1, function, &arg);
}


/* ttvisible */
TT_INLINE void Add_ttvisible(ttvisible o, ttvisible child) {
    if (o && child && TTAssert(IS(ttvisible,o)) && TTAssert(IS(ttvisible,child)))
	child->FN->AddTo(child, o);
}
TT_INLINE void SetRepaint_ttvisible(ttvisible o, ttvisible_repaint_fn repaint) {
    if (o && TTAssert(IS(ttvisible,o)) && o->repaint != repaint) {
	o->repaint = repaint;
	if (o->vflags & ttvisible_vflags_visible)
	    Expose_ttvisible(o, ttvisible_repaint_args_WHOLE);
    }
}


/* ttnative */
TT_INLINE ttnative Create_ttnative(ttany id) {
    ttnative o; ttobj a;

    if ((a = FindNative(id)))
	return (ttnative)a;
    
    if ((o = TFN_ttnative->New(TFN_ttnative, (ttnative)0))) {
	o->native = id;
	return (ttnative)Build((ttobj)o);
    }
    return (ttnative)0;
}


/* ttwidget */
TT_INLINE void SetXY_ttwidget(ttwidget o, ttshort x, ttshort y) {
    if (o && TTAssert(IS(ttwidget,o)))
	o->FN->SetXYWH(o, 0x03, x, y, 0, 0);
}
TT_INLINE void SetWH_ttwidget(ttwidget o, ttshort w, ttshort h) {
    if (o && TTAssert(IS(ttwidget,o)))
	o->FN->SetXYWH(o, 0x0a, 0, 0, w, h);
}


/* ttlabel */
TT_INLINE ttlabel Create_ttlabel(TT_CONST byte * text) {
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
TT_INLINE ttanybutton Create_ttanybutton(TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch) {
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
TT_INLINE ttbutton Create_ttbutton(TT_CONST ttfont * text, ttshort width, ttshort height, ttshort pitch) {
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
TT_INLINE byte IsPressed_ttbutton(ttbutton o) {
    if (o && TTAssert(IS(ttbutton,o)))
	return !!(o->vflags & ttbutton_vflags_pressed);
    return FALSE;
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
TT_INLINE ttapplication Set_ttapplication(TT_CONST byte * name) {
    byte *apname;
    
    if (!name)
	return (ttapplication)0;
    
    if (TTD.Application || (TTD.Application = TFN_ttapplication->New(TFN_ttapplication, NULL))) {

	if ((apname = TTCloneStr(name))) {
	    if (TTD.Application->name)
		TTFreeMem(TTD.Application->name);
	    TTD.Application->name = apname;

	    return (ttapplication)Build((ttobj)TTD.Application);
	}
	TDEL(TTD.Application);
    }
    return TTD.Application;
}



#endif /* _TT_CREATE_H */


