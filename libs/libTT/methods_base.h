
/*
 *                 WARNING!
 * 
 * this file is `methods_base.h' and is preprocessed by m4 to produce `methodsm4.h'
 * 
 * It must be a valid m4 file, and must produce a valid C include file.
 * 
 */


#define myTheme (o->theme?o->theme:TTD.DefaultTheme)

#define myMenubar (o->Menubar?o->Menubar:TTD.DefaultMenubar?TTD.DefaultMenubar:NULL)
#define myMenubar_id (o->Menubar?o->Menubar->id:TTD.DefaultMenubar?TTD.DefaultMenubar->id:TW_NOID)

/*
 * We need to write down only base and overloaded methods.
 * All the other ones will automagically be set to default values by m4 logic.
 */
 



/* ttobj */
static NEW(ttobj) {
    ttobj o;
    if ((o = (ttobj)TTAllocMem(FN->size))) {
	o->FN = FN;
	o->id = TW_NOID;
	o->userdata = NULL;
	o->listeners = NULL;
    }
    return o;
}
static REALIZE(ttobj) {
    /*
     * this is necessary for all derived classes that do not overload FN->Realize.
     * It is not actually useful for ttobj.
     */
    return o;
}
static DEL(ttobj) {
    if (o->id != TW_NOID)
	TwDeleteObj(o->id);
    DelAllListeners_ttobj(o);
    TTFreeMem(o);
    return;
}


/* ttvisible */
static NEW(ttvisible) {
    ttvisible o;
    if ((o = SUPER(ttvisible))) {
	o->vflags = ttvisible_vflags_visible;
	o->prev = o->next = o->parent = o->firstchild = o->lastchild = NULL;
	o->theme = NULL;
    }
    return o;
}
static void DEF(Add_ttvisible)(ttvisible o, ttvisible child) {
    child->FN->AddTo(child, o);
}
static void DEF(AddTo_ttvisible)(ttvisible o, ttvisible parent) {
    /* append */
    if (parent && !o->parent) {
	if ((o->prev = parent->lastchild))
	    parent->lastchild->next = o;
	else
	    parent->firstchild = o;
	o->next = NULL;
	o->parent = parent;
    }
}
static void DEF(SetVisible_ttvisible)(ttvisible o, byte on_off) {
    if (on_off)
	o->vflags |= ttvisible_vflags_visible;
    else
	o->vflags &= ~ttvisible_vflags_visible;
}
static void DEF(Remove_ttvisible)(ttvisible o) {
    ttvisible parent;
    if ((parent = o->parent)) {
	if (o->prev)
	    o->prev->next = o->next;
	else
	    parent->firstchild = o->next;
	if (o->next)
	    o->next->prev = o->prev;
	else
	    parent->lastchild = o->prev;
	o->parent = NULL;
    }
}
static void DEF(SetTheme_ttvisible)(ttvisible o, tttheme theme) {
    tttheme oldtheme = myTheme;
    o->theme = theme;
    if (oldtheme != theme && o->parent && (o->vflags & ttvisible_vflags_visible))
	o->FN->Repaint(o, ttvisible_Repaint_args_WHOLE);
}
static void DEF(Repaint_ttvisible)(ttvisible o, dat x, dat y, dat w, dat h) {
    /* no-op */
    TwExposeTextWidget(o->id, w, h, x, y, w, NULL); 
}


/* ttnative */
static DEL(ttnative) {
    o->id = TW_NOID;
    DELSUPER(ttnative);
}


/* ttwidget */
static void ExposeWrapper_ttwidget(tevent_any e, ttwidget o) {
    if (o && TTAssert(IS(ttwidget,o)))
	o->FN->Repaint(o, e->EventWidget.X, e->EventWidget.Y, e->EventWidget.XWidth, e->EventWidget.YWidth);
}

static ttlistener AddExposeListener_ttwidget(ttwidget o) {
    ttlistener l;
    if ((l = AddListener_ttobj((ttobj)o))) {
	if ((l->t = TwAddWidgetListener(o->id, TW_MSG_WIDGET_EXPOSE, (void *)ExposeWrapper_ttwidget, (void *)o)))
	    return l;
	DelListener_ttobj((ttobj)o, l);
    }
    return NULL;
}
static REALIZE(ttwidget) {
    if (TW_NOID != (o->id = TwCreateWidget(1, 1, 0, TW_WIDGETFL_USEFILL, 0, MAXDAT, HWATTR(myTheme->bg[tttheme_bg_normal], myTheme->fill)))) {
	return o;
    }
    return NULL;
}
static void DEF(AddTo_ttwidget)(ttwidget o, ttvisible parent) {
    if (parent && !o->parent) {
	/* prepend to list, cannot use o->FN->FN_ttvisible->AddTo() */
	if ((o->next = parent->firstchild))
	    parent->firstchild->prev = (ttvisible)o;
	else
	    parent->lastchild = (ttvisible)o;
	o->prev = NULL;
	o->parent = parent;

	if (o->vflags & ttvisible_vflags_visible)
	    TwMapWidget(o->id, parent->id);
    }
}
static void DEF(SetVisible_ttwidget)(ttwidget o, byte on_off) {
    if (!on_off != !(o->vflags & ttvisible_vflags_visible)) {
	o->vflags ^= ttvisible_vflags_visible;
	if (o->parent) {
	    if (on_off)
		TwMapWidget(o->id, o->parent->id);
	    else
		TwUnMapWidget(o->id);
	}
    }
}
static void DEF(Remove_ttwidget)(ttwidget o) {
    if (o->parent) {
	o->FN->FN_ttvisible->Remove((ttvisible)o);
	
	if (o->vflags & ttvisible_vflags_visible)
	    TwUnMapWidget(o->id);
    }
}


/* ttlabel */
static NEW(ttlabel) {
    ttlabel o;
    if ((o = SUPER(ttlabel))) {
	o->textlen = 0;
	o->text = NULL;
    }
    return o;
}
static REALIZE(ttlabel) {
    if (TW_NOID != (o->id = TwCreateWidget(o->textlen, 1, 0, TW_WIDGETFL_USEEXPOSE, 0, MAXDAT, HWATTR(myTheme->bg[tttheme_bg_normal], myTheme->fill)))) {
	if (AddExposeListener_ttwidget((ttwidget)o))
	    return o;
	TT_DEL(o);
    }
    return NULL;
}
static DEL(ttlabel) {
    if (o->text)
	TTFreeMem(o->text);
    DELSUPER(ttlabel);
}
static void DEF(Repaint_ttlabel)(ttlabel o, dat x, dat y, dat w, dat h) {
    if (y == 0 && x < o->textlen)
	TwExposeHWFontWidget(o->id, TT_MIN2(w,o->textlen-x), 1, x, 0, o->textlen, o->text+x);
}
static byte DEF(SetText_ttlabel)(ttlabel o, byte TT_CONST * text) {
    hwfont *_text;
    dat _textlen;
    
    if (text)
	_textlen = strlen(text);
    else
	_textlen = 0;
    
    if (!text || (_text = CloneStr2HWFont(text, _textlen))) {
	if (o->text)
	    TTFreeMem(o->text);
	o->textlen = _textlen;
	o->text = _text;
	if (o->id != TW_NOID && o->parent && o->vflags & ttvisible_vflags_visible)
	    o->FN->Repaint(o, ttvisible_Repaint_args_WHOLE);
	return TRUE;
    }
    return FALSE;
}

/* ttanybutton */
static NEW(ttanybutton) {
    ttanybutton o;
    if ((o = SUPER(ttanybutton))) {
	o->text = NULL;
	o->textwidth = o->textheight = 0;
	TTWriteMem(o->textshape, '\0', sizeof(o->textshape));
	o->MouseListener = o->KeyListener = NULL;
	return o;
    }
    return NULL;
}
static DEL(ttanybutton) {
    if (o->text)
	TTFreeMem(o->text);
    DELSUPER(ttanybutton);
}
static void DEF(Repaint_ttanybutton)(ttanybutton o, dat x, dat y, dat w, dat h) {
    if (o->text) {
	if (h+y > o->textheight)
	    h = o->textheight - y;
	if (w+x > o->textwidth)
	    w = o->textwidth - x;

	if (h > 0 && w > 0 && y < o->textheight && x < o->textwidth)
	    TwExposeHWFontWidget(o->id, TT_MIN2(w,o->textwidth-x), TT_MIN2(h, o->textheight-y),
				  x, y, o->textwidth, o->text + x + (y * o->textwidth));
    } else
	TwExposeHWFontWidget(o->id, w, h, x, y, w, NULL);
}
static void DEF(MouseEvent_ttanybutton)(ttanybutton o, udat Code, udat ShiftFlags, dat x, dat y) {
    if (o->MouseListener)
	((void (*)(ttanybutton,udat,udat,dat,dat)) o->MouseListener )   (o, Code, ShiftFlags, x, y);
}
static byte DEF(SetText_ttanybutton)(ttanybutton o, hwfont TT_CONST * text, dat width, dat height, dat pitch) {
    hwfont *_text = NULL;
    uldat _textlen = width * height * sizeof(hwfont);
    byte i;
    
    if (!text || !_textlen || (_text = TTAllocMem(_textlen))) {
	o->textwidth = width;
	o->textheight = height;
	if (o->text)
	    TTFreeMem(o->text);
	o->text = _textlen ? _text : NULL;
	while (_textlen) {
	    TTCopyMem(text, _text, width * sizeof(hwfont));
	    text += pitch;
	    _text += width;
	    _textlen -= width;
	}
	for (i = 0; i < ttbutton_shape_max; i++) {
	    if (o->textshape[i].attr) {
		TTFreeMem(o->textshape[i].attr);
		o->textshape[i].attr = NULL;
	    }
	}
	if (o->id != TW_NOID && o->parent && o->vflags & ttvisible_vflags_visible)
	    o->FN->Repaint(o, ttvisible_Repaint_args_WHOLE);
	return TRUE;
    }
    return FALSE;
    
}
static void MouseWrapper_ttanybutton(tevent_any e, ttanybutton o) {
    if (o && TTAssert(IS(ttanybutton,o)))
	o->FN->MouseEvent(o, e->EventMouse.Code, e->EventMouse.ShiftFlags, e->EventMouse.X, e->EventMouse.Y);
}
static ttlistener AddCommonListeners_ttanybutton(ttanybutton o) {
    ttlistener l[5];
    byte i = 0;
    
    if ((l[i] = AddExposeListener_ttwidget((ttwidget)o)) &&
	(i++, (l[i] = AddListener_ttobj((ttobj)o))) &&
	((l[i]->t = TwAddMouseListener(o->id, MOTION_MOUSE, 0, (void *)MouseWrapper_ttanybutton, (void *)o))) &&
	(i++, (l[i] = AddListener_ttobj((ttobj)o))) &&
	((l[i]->t = TwAddMouseListener(o->id, DOWN_LEFT, 0, (void *)MouseWrapper_ttanybutton, (void *)o))) &&
	(i++, (l[i] = AddListener_ttobj((ttobj)o))) &&
	((l[i]->t = TwAddMouseListener(o->id, DRAG_MOUSE|HOLD_LEFT, 0, (void *)MouseWrapper_ttanybutton, (void *)o))) &&
	(i++, (l[i] = AddListener_ttobj((ttobj)o))) &&
	((l[i]->t = TwAddMouseListener(o->id, RELEASE_LEFT, 0, (void *)MouseWrapper_ttanybutton, (void *)o)))) {
	
	return l[i];
    }
    while (i) {
	if (l[i])
	    DelListener_ttobj((ttobj)o, l[i]);
	i--;
    }
    return NULL;
}


/* ttbutton */
static REALIZE(ttbutton) {
    dat w = o->textwidth + myTheme->shape[tttheme_shape_ttbutton_normal][0].border[x_left] +
	myTheme->shape[tttheme_shape_ttbutton_normal][0].border[x_right];
    dat h = o->textheight + myTheme->shape[tttheme_shape_ttbutton_normal][0].border[y_up] +
	myTheme->shape[tttheme_shape_ttbutton_normal][0].border[y_down];
    
    if (TW_NOID != (o->id = TwCreateWidget(w, h, TW_WIDGET_WANT_MOUSE|TW_WIDGET_WANT_MOUSE_MOTION,
	TW_WIDGETFL_USEEXPOSE, 0, 0, myTheme->bg[tttheme_bg_normal]))) {
	if (AddCommonListeners_ttanybutton((ttanybutton)o))
	    return o;
	TT_DEL(o);
    }
    return NULL;
}
static void DEF(Repaint_ttbutton)(ttbutton o, dat x, dat y, dat w, dat h) {
    hwattr *t;
    dat tw, th;
    byte i;
    
    switch ((o->vflags & ttanybutton_vflags_statemask)) {
      case ttanybutton_vflags_normal:
	i = ttbutton_shape_normal;
	if (!(t = o->textshape[i].attr))
	    t = o->textshape[i].attr
	    = myTheme->FN->ComputeShapeButton(o, i, tttheme_shape_ttbutton_normal);
	break;
      case ttanybutton_vflags_prelight:
	i = ttbutton_shape_prelight;
	if (!(t = o->textshape[i].attr))
	    t = o->textshape[i].attr
	    = myTheme->FN->ComputeShapeButton(o, i, tttheme_shape_ttbutton_prelight);
	break;
      case ttanybutton_vflags_pressed:
	i = ttbutton_shape_pressed;
	if (!(t = o->textshape[i].attr))
	    t = o->textshape[i].attr
	    = myTheme->FN->ComputeShapeButton(o, i, tttheme_shape_ttbutton_pressed);
	break;
      case ttanybutton_vflags_disabled:
	i = ttbutton_shape_disabled;
	if (!(t = o->textshape[i].attr))
	    t = o->textshape[i].attr
	    = myTheme->FN->ComputeShapeButton(o, i, tttheme_shape_ttbutton_disabled);
	break;
      default:
	t = NULL;
	break;
    }
    if (t) {
	tw = o->textshape[i].width;
	th = o->textshape[i].height;
	
	if (h+y > th)
	    h = th - y;
	if (w+x > tw)
	    w = tw - x;

	if (h > 0 && w > 0 && y < th && x < tw)
	    TwExposeHWAttrWidget(o->id, TT_MIN2(w,tw-x), TT_MIN2(h, th-y),
				 x, y, tw, t + x + (y * tw));
    } else
	TwExposeHWFontWidget(o->id, w, h, x, y, w, NULL);
}
static void DEF(MouseEvent_ttbutton)(ttbutton o, udat Code, udat ShiftFlags, dat x, dat y) {
    uldat old_vflags = o->vflags;
    
    fprintf(stderr, "x=%d, y=%d, Code=%x\n", x, y, Code);
    switch (Code) {
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
	if (x >= 0 && y >= 0 && x < o->textwidth && y < o->textheight)
	    o->vflags |= ttanybutton_vflags_pressed;
	else
	    o->vflags &= ~ttanybutton_vflags_pressed;
	break;
      case RELEASE_LEFT:
	o->vflags &= ~ttanybutton_vflags_pressed;
	if (x >= 0 && y >= 0 && x < o->textwidth && y < o->textheight) {
	    o->vflags |= ttanybutton_vflags_prelight;
	    if (o->MouseListener)
		((void (*)(ttbutton,udat,udat,dat,dat)) o->MouseListener )   (o, Code, ShiftFlags, x, y);
	}
      default:
	break;
    }
    
    if (old_vflags != o->vflags)
	o->FN->Repaint(o, ttvisible_Repaint_args_WHOLE);
}
static void DEF(SetPressed_ttbutton)(ttbutton o, byte pressed) {
}


/* ttcheckbutton */


/* ttradiobutton */


/* ttbuttongroup */

static REALIZE(ttbuttongroup) {
    if (TW_NOID != (o->id = TwCreateGroup()))
	return o;
    return NULL;
}


/* ttwindow */

static REALIZE(ttwindow) {
    if (TW_NOID != (o->id = TwCreateWindow
	 (0, NULL, NULL, myMenubar_id, myTheme->bg[tttheme_bg_normal],
	  TW_LINECURSOR, TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_CLOSE|TW_WINDOW_WANT_CHANGES|TW_WINDOW_WANT_KEYS,
	  TW_WINDOWFL_USEEXPOSE|TW_WINDOWFL_CURSOR_ON|TW_WINDOWFL_BORDERLESS, 1, 1, 0))) {
	if (AddExposeListener_ttwidget((ttwidget)o))
	    return o;
	TT_DEL(o);
    }
    return NULL;
}


/* ttframe */

static REALIZE(ttframe) {
    if (TW_NOID != (o->id = TwCreateWindow
	 (0, NULL, NULL, myMenubar_id, myTheme->bg[tttheme_bg_normal],
	  TW_LINECURSOR, TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_CLOSE|TW_WINDOW_WANT_CHANGES|TW_WINDOW_WANT_KEYS,
	  TW_WINDOWFL_USEEXPOSE|TW_WINDOWFL_CURSOR_ON, 1, 1, 0))) {
	if (AddExposeListener_ttwidget((ttwidget)o))
	    return o;
	TT_DEL(o);
    }
    return NULL;
}


/* ttscrollbar */


/* ttslider */


/* ttscroller */

static NEW(ttscroller) {
    ttscroller o;
    if ((o = SUPER(ttscroller))) {
	o->scrollx = o->scrolly = NULL;
    }
    return o;
}
static DEL(ttscroller) {
    if (o->scrollx)
	TT_DEL(o->scrollx);
    if (o->scrolly)
	TT_DEL(o->scrolly);
    DELSUPER(ttscroller);
    return;
}


/* ttmenuitem */


/* ttcheckmenuitem */


/* ttradiomenuitem */


/* ttmenuwindow */

static REALIZE(ttmenuwindow) {
    if (TW_NOID != (o->id = TwCreate4MenuWindow(myMenubar_id)))
	return o;
    return NULL;
}


/* ttmenu */

static NEW(ttmenu) {
    ttmenu o;
    if ((o = SUPER(ttmenu))) {
	o->menuwindow = TT_NEW(ttmenuwindow);
	    return o;
	TT_DEL(o);
    }
    return o;
}

static DEL(ttmenu) {
    if (o->menuwindow)
	TT_DEL(o->menuwindow);
    DELSUPER(ttmenu);
}

static void DEF(AddTo_ttmenu)(ttmenu o, ttvisible parent) {
    if (!o->parent && parent && (o->name || (o->name = TTCloneStr(""))) &&
	TW_NOID != (o->id = TwItem4Menu(parent->id, o->menuwindow->id, TRUE, strlen(o->name), o->name))) {
		    
	o->FN->TT_CAT(FN_,TT_CAT(TTsuper_,ttmenu))->AddTo((void *)o, parent);
    }
}
static void DEF(Remove_ttmenu)(ttmenu o) {
    if (o->parent) {
	o->FN->FN_ttvisible->Remove((ttvisible)o);
	
	if (o->id != TW_NOID) {
	    TwDeleteObj(o->id);
	    o->id = TW_NOID;
	}
    }
}


/* ttmenubar */

static REALIZE(ttmenubar) {
    if (TW_NOID != (o->id = TwCreateMenu(myTheme->bg[tttheme_bg_menu], myTheme->bg[tttheme_bg_menu_select],
			      myTheme->bg[tttheme_bg_menu_disabled], myTheme->bg[tttheme_bg_menu_selectdisabled],
			      myTheme->bg[tttheme_bg_menu_shcut], myTheme->bg[tttheme_bg_menu_selectshcut], 0))) {
	TwItem4MenuCommon(o->id);
	return o;
    }
    return NULL;
}


/* ttanytext */

static NEW(ttanytext) {
    ttanytext o;
    if ((o = SUPER(ttanytext))) {
	o->text = NULL;
	o->textlen = 0;
    }
    return o;
}


/* tttextfield */


/* tttextarea */


/* tttheme */

static REALIZE(tttheme) {
    /* FIXME finish this */
    return o;
}
static DEL(tttheme) {
    if (TTD.DefaultTheme == o)
	TTD.DefaultTheme = &s_DummyTheme;
    DELSUPER(tttheme);
    return;
}

static void AddTransparentShapeButton(hwattr *attr, dat w, dat h, hwattr *tattr, dat tw, dat th,
				      dat b_left, dat b_up, dat b_right, dat b_down) {
    dat ti, i, tj, j;
    
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
				     
static void AddTextButton(hwattr *attr, dat w, hwfont *text, dat tw, dat th, hwcol bg) {
    dat _tw;
    
    for (; th; th--) {
	for (_tw = tw; _tw; _tw--) {
	    *attr++ = HWATTR(bg, *text);
	    text++;
	}
	attr += w - tw;
    }
}

static hwattr * DEF(ComputeShapeButton_tttheme)(ttbutton o, byte button_i, byte theme_i) {
    tttheme t = myTheme;
    ttshape t_shape = &t->shape[theme_i][0], s_shape =  &t->shape[theme_i][1];
    hwattr *shape;
    dat w = o->textwidth, h = o->textheight;
    dat b_left = t_shape->border[x_left], b_up = t_shape->border[y_up];
    dat b_right = t_shape->border[x_right], b_down = t_shape->border[y_down];
    
    if ((shape = (hwattr *)TTAllocMem((w += b_left+b_right) * (h += b_up+b_down) * sizeof(hwattr) ))) {
	
	TTWriteMem(shape, '\0', w * h * sizeof(hwattr));
	
	o->textshape[button_i].width = w;
	o->textshape[button_i].height = h;
	TTCopyMem(t_shape->border, o->textshape[button_i].border, 4*sizeof(dat));
	
	AddTransparentShapeButton(shape, w, h, t_shape->attr, t_shape->width, t_shape->height,
				  b_left, b_up, b_right, b_down);
    
    	AddTransparentShapeButton(shape, w, h, s_shape->attr, s_shape->width, s_shape->height,
				  s_shape->border[x_left], s_shape->border[y_up],
				  s_shape->border[x_right], s_shape->border[y_down]);

	AddTextButton(shape + b_left + w * b_up, w, o->text, o->textwidth, o->textheight,
		      myTheme->bg[tttheme_bg_normal]);
	
    }
    return shape;
}


/* ttapplication */

static REALIZE(ttapplication) {
    if (o->name && TW_NOID != (o->id = TwCreateMsgPort(strlen(o->name), o->name, (time_t)0, (frac_t)0, 0)))
	return o;
    return NULL;
}
