
static void X11_HideCursor(dat x, dat y) {
    int xbegin = (x - xhw_startx) * xwfont, ybegin = (y - xhw_starty) * xhfont;
    hwattr V = Video[x + y * DisplayWidth];
    hwcol col = HWCOL(V);
#ifdef CONF__UNICODE
    XChar2b c;
    hwattr extra = HWEXTRA32(V);
    hwfont f = xUTF_16_to_charset(HWFONT(V));
    c.byte1 = f >> 8;
    c.byte2 = f & 0xFF;
#else
    hwattr extra = 0;
    byte c = HWFONT(V);
#endif
    
    XDRAW_ANY(&c, 1, col, extra);
}

static void X11_ShowCursor(uldat type, dat x, dat y) {
    udat i;
    hwcol v;
    hwattr V = Video[x + y * DisplayWidth];
#ifdef CONF__UNICODE
    hwfont f;
    XChar2b c;
#else
    byte c;
#endif

    ldat xbegin = (x - xhw_startx) * xwfont;
    ldat ybegin = (y - xhw_starty) * xhfont;
    
    if (type & 0x10) {
	/* soft cursor */
	v = (HWCOL(V) | ((type >> 16) & 0xff)) ^ ((type >> 8) & 0xff);
	if ((type & 0x20) && (HWCOL(V) & COL(0,WHITE)) == (v & COL(0,WHITE)))
	    v ^= COL(0,WHITE);
	if ((type & 0x40) && ((COLFG(v) & WHITE) == (COLBG(v) & WHITE)))
	    v ^= COL(WHITE,0);
	if (xsgc.foreground != xcol[COLFG(v)])
	    XSetForeground(xdisplay, xgc, xsgc.foreground = xcol[COLFG(v)]);
	if (xsgc.background != xcol[COLBG(v)])
	    XSetBackground(xdisplay, xgc, xsgc.background = xcol[COLBG(v)]);
#ifdef CONF__UNICODE
	f = xUTF_16_to_charset(HWFONT(V));
	c.byte1 = f >> 8;
	c.byte2 = f & 0xFF;
	XDrawImageString16(xdisplay, xwindow, xgc, xbegin, ybegin + xupfont, &c, 1);
#else
	c = HWFONT(V);
	XDrawImageString(xdisplay, xwindow, xgc, xbegin, ybegin + xupfont, &c, 1);
#endif
    }
    if (type & 0xF) {
	/* VGA hw-like cursor */
	i = xhfont * ((type & 0xF)-NOCURSOR) / (SOLIDCURSOR-NOCURSOR);
	
	/* doesn't work as expected on paletted visuals... */
	if (xsgc.foreground != xcol[COLFG(HWCOL(V)) ^ COLBG(HWCOL(V))])
	    XSetForeground(xdisplay, xgc, xsgc.foreground = xcol[COLFG(HWCOL(V)) ^ COLBG(HWCOL(V))]);
	
	XSetFunction(xdisplay, xgc, xsgc.function = GXxor);
	XFillRectangle(xdisplay, xwindow, xgc,
		       xbegin, ybegin + xhfont - i, xwfont, i);
	XSetFunction(xdisplay, xgc, xsgc.function = GXcopy);
    }
}

static void X11_FlushVideo(void) {
    dat start, end;
    udat i;
    byte iff;
    
    if (ValidOldVideo)
	iff = ChangedVideoFlag &&
	Video[HW->XY[0] + HW->XY[1] * DisplayWidth] != OldVideo[HW->XY[0] + HW->XY[1] * DisplayWidth];
    /* TRUE if and only if the cursor will be erased by burst */
    
    
    /* first burst all changes */
    if (ChangedVideoFlag) {
	for (i=0; i<DisplayHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    end   = ChangedVideo[i>>1][i&1][1];
	    
	    if (start != -1)
		X11_Mogrify(start, i>>1, end-start+1);
	}
	setFlush();
    }
    /* then, we may have to erase the old cursor */
    if (!ValidOldVideo || (!iff && HW->TT != NOCURSOR &&
	(CursorType != HW->TT || CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
	
	HW->TT = NOCURSOR;
	X11_HideCursor(HW->XY[0], HW->XY[1]);
	setFlush();
    }
    /* finally, redraw the cursor if forced to redraw or */
    /* (we want a cursor and (the burst erased the cursor or the cursor changed)) */
    if (!ValidOldVideo ||
	(CursorType != NOCURSOR &&
	 (iff || CursorType != HW->TT || CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
	
	X11_ShowCursor(HW->TT = CursorType, HW->XY[0] = CursorX, HW->XY[1]= CursorY);
	setFlush();
    }

    HW->FlagsHW &= ~FlHWChangedMouseFlag;
}

static void X11_FlushHW(void) {
    XFlush(xdisplay);
    clrFlush();
}

static void X11_DetectSize(dat *x, dat *y) {
    if (!xhw_view) {
	*x = HW->X = xwidth  / xwfont;
	*y = HW->Y = xheight / xhfont;
    }
}

static void X11_CheckResize(dat *x, dat *y) {
    /* always ok */
}

static void X11_Resize(dat x, dat y) {
    if (x != HW->X || y != HW->Y) {
	if (!xhw_view) {
	    XResizeWindow(xdisplay, xwindow,
			  xwidth = xwfont * (HW->X = xhw_endx = x),
			  xheight = xhfont * (HW->Y = xhw_endy = y));
	}
	setFlush();
    }
}
     
/*
 * import X11 Selection
 */
static byte X11_SelectionImport_X11(void) {
    return !HW->HWSelectionPrivate;
}

/*
 * export our Selection to X11
 */
static void X11_SelectionExport_X11(void) {
    if (!HW->HWSelectionPrivate) {
	XSetSelectionOwner(xdisplay, XA_PRIMARY, xwindow, CurrentTime);
	HW->HWSelectionPrivate = (tany)xwindow;
	setFlush();
    }
}

/*
 * notify our Selection to X11
 */
static void X11_SelectionNotify_X11(uldat ReqPrivate, uldat Magic, CONST byte MIME[MAX_MIMELEN],
				    uldat Len, CONST byte * Data) {
    XEvent ev;
	
    if (XReqCount == 0) {
	printk(THIS ".c: X11_SelectionNotify_X11(): unexpected Twin Selection Notify event!\n");
	return;
    }
#if 0
    else {
	printk(THIS ".c: X11_SelectionNotify_X11(): %d nested Twin Selection Notify events\n", XReqCount);
    }
#endif
    
    XReqCount--;
    ev.xselection.type      = SelectionNotify;
    ev.xselection.property  = None;
    ev.xselection.display   = XReq(XReqCount).display;
    ev.xselection.requestor = XReq(XReqCount).requestor;
    ev.xselection.selection = XReq(XReqCount).selection;
    ev.xselection.target    = XReq(XReqCount).target;
    ev.xselection.time      = XReq(XReqCount).time;

    if (XReq(XReqCount).target == xTARGETS) {
	/*
	 * On some systems, the Atom typedef is 64 bits wide.
	 * We need to have a typedef that is exactly 32 bits wide,
	 * because a format of 64 is not allowed by the X11 protocol.
	 */
	typedef CARD32 Atom32;
	Atom32 target_list[2];
	
	target_list[0] = (Atom32) xTARGETS;
	target_list[1] = (Atom32) XA_STRING;
	XChangeProperty (xdisplay, XReq(XReqCount).requestor, XReq(XReqCount).property,
			 xTARGETS, 8*sizeof(target_list[0]), PropModeReplace,
			 (char *)target_list,
			 sizeof(target_list)/sizeof(target_list[0]));
	ev.xselection.property = XReq(XReqCount).property;
    } else if (XReq(XReqCount).target == XA_STRING) {
#ifdef CONF__UNICODE
	uldat l;
	byte *_Data = NULL, *d;
	TW_CONST hwfont *s;
	
	/* X11 selection contains text, not unicode */
	if (Magic == SEL_HWFONTMAGIC) {
	    if ((_Data = d = (byte *)AllocMem(Len))) {
		s = (TW_CONST hwfont *)Data;
		for (l = Len; l; l--)
		    *d++ = Tutf_UTF_16_to_CP437(*s++);
		Data = _Data;
		Len /= sizeof(hwfont);
	    } else
		Len = 0;
	}
#endif
	XChangeProperty (xdisplay, XReq(XReqCount).requestor, XReq(XReqCount).property,
			 XA_STRING, 8, PropModeReplace, Data, Len);
	ev.xselection.property = XReq(XReqCount).property;

#ifdef CONF__UNICODE
	if (Magic == SEL_HWFONTMAGIC && _Data)
	    FreeMem(_Data);
#endif
    }
    XSendEvent (xdisplay, XReq(XReqCount).requestor, False, 0, &ev);
    setFlush();
}

/*
 * notify the X11 Selection to twin upper layer
 */
static void X11_SelectionNotify_up(Window win, Atom prop) {
    long nread = 0;
    unsigned long nitems, bytes_after = BIGBUFF;
    Atom actual_type;
    int actual_fmt;
    byte *data, *buff = NULL;

    if (xReqCount == 0) {
	printk(THIS ".c: X11_SelectionNotify_up(): unexpected X Selection Notify event!\n");
	return;
    }
#if 0
    else {
	printk(THIS ".c: X11_SelectionNotify_up(): %d nested X Selection Notify event\n", xReqCount);
    }
#endif
    if (prop == None)
	return;

    xReqCount--;

    do {
	if ((XGetWindowProperty(xdisplay, win, prop,
				nread/4, bytes_after/4, False,
				AnyPropertyType, &actual_type, &actual_fmt,
				&nitems, &bytes_after, &data)
	     != Success) || (actual_type != XA_STRING)) {
	    
	    XFree(data);
	    if (buff)
		FreeMem(buff);
	    return;
	}
	
	if (buff || (buff = AllocMem(nitems + bytes_after))) {
	    CopyMem(data, buff + nread, nitems);
	    nread += nitems;
	}
	XFree(data);
	if (!buff)
	    return;
    } while (bytes_after > 0);
    
    TwinSelectionNotify(xRequestor(xReqCount), xReqPrivate(xReqCount), SEL_TEXTMAGIC, NULL, nread, buff);
    FreeMem(buff);
}

/*
 * request X11 Selection
 */
static void X11_SelectionRequest_X11(obj Requestor, uldat ReqPrivate) {
    if (!HW->HWSelectionPrivate) {

	if (xReqCount == NEST) {
	    printk(THIS ".c: X11_SelectionRequest_X11(): too many nested Twin Selection Request events!\n");
	    return;
	}
#if 0
	else {
	    printk(THIS ".c: X11_SelectionRequest_X11(): %d nested Twin Selection Request events\n", xReqCount+1);
	}
#endif
	xRequestor(xReqCount) = Requestor;
	xReqPrivate(xReqCount) = ReqPrivate;
	xReqCount++;
	
	if (XGetSelectionOwner(xdisplay, XA_PRIMARY) == None)
	    X11_SelectionNotify_up(DefaultRootWindow(xdisplay), XA_CUT_BUFFER0);
	else {
	    Atom prop = XInternAtom (xdisplay, "VT_SELECTION", False);
	    XConvertSelection(xdisplay, XA_PRIMARY, XA_STRING, prop, xwindow, CurrentTime);
	    setFlush();
	    /* we will get an X11 SelectionNotify event */
	}
    }
    /* else race! someone else became Selection owner in the meanwhile... */
}


/*
 * request twin Selection
 */
static void X11_SelectionRequest_up(XSelectionRequestEvent *req) {
    if (XReqCount == NEST) {
	printk(THIS ".c: X11_SelectionRequest_up(): too many nested X Selection Request events!\n");
	return;
    }
#if 0
    else {
	printk(THIS ".c: X11_SelectionRequest_up(): %d nested X Selection Request events\n", XReqCount+1);
    }
#endif
    CopyMem(req, &XReq(XReqCount), sizeof(XSelectionRequestEvent));
    TwinSelectionRequest((obj)HW, XReqCount++, TwinSelectionGetOwner());
    /* we will get a HW->HWSelectionNotify (i.e. X11_SelectionNotify_X11) call */
    /* the call **CAN** arrive while we are still inside TwinSelectionRequest() !!! */
}

static byte X11_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return xwindow_AllVisible /* if window is partially covered, XCopyArea() cannot work */
	&& !HW->RedrawVideo /* if window is not up-to-date, XCopyArea() is unusable */
	&& (Rgt-Left+1) * (Dwn-Up+1) > 20; /* avoid XCopyArea() for very small areas */
}

static void X11_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    dat DstRgt = (Rgt-Left)+DstLeft;
    dat DstDwn = (Dwn-Up)+DstUp;

    if (HW->TT != NOCURSOR) {
	if (HW->XY[0] >= Left && HW->XY[0] <= Rgt && HW->XY[1] >= Up && HW->XY[1] <= Dwn) {
	    /* must hide the cursor before dragging */
	    X11_HideCursor(HW->XY[0], HW->XY[1]);
	    /* and remember to redraw it */
	    HW->TT = (uldat)-1;
	} else if (HW->XY[0] >= DstLeft && HW->XY[0] <= DstRgt &&
		   HW->XY[1] >= DstUp && HW->XY[1] <= DstDwn) {
	    /* cursor will be overwritten by drag, remember to redraw it */
	    HW->TT = (uldat)-1;
	}
    }
    XCopyArea(xdisplay, xwindow, xwindow, xgc,
	      Left*xwfont, Up*xhfont, (Rgt-Left+1)*xwfont, (Dwn-Up+1)*xhfont,
	      DstLeft*xwfont, DstUp*xhfont);
    setFlush();
}


#if 0
/* does NOT work... libX11 insists on doing exit(1) */
static int X11_Die(Display *d) {
    /*
     * this is not exactly trivial:
     * find our HW, shut it down
     * and quit if it was the last HW.
     * 
     * don't rely on HW->Private only, as non-X11 displays
     * may use it differently and have by chance the same value for it.
     */
    forallHW {
	if (HW->QuitHW == X11_QuitHW && HW->Private
	    && d == xdisplay) { /* expands to HW->Private->xdisplay */
	    
	    HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	    
	    break;
	}
    }
    return 0;
}
#else
static int X11_Die(Display *d) {
    Quit(0);
    return 0;
}
#endif


#ifdef CONF__UNICODE
static Tutf_function X11_UTF_16_to_charset_function(CONST byte *charset) {
    XFontProp *fp;
    Atom fontatom;
    CONST byte *s, *fontname = NULL;
    uldat i;
    
    if (!charset) {
	/* attempt to autodetect encoding. */
	fp = xsfont->properties;
	fontatom = XInternAtom (xdisplay, "FONT", False);
	i = xsfont->n_properties;
	
	while (i--) {
	    if (fp->name == fontatom) {
		fontname = XGetAtomName(xdisplay, fp->card32);
		/*fprintf(stderr, "    X11_UTF_16_to...: font name: `%s\'\n", fontname);*/
		break;
	    }
	    fp++;
	}
	if (fontname && !strcmp(fontname, "vga")) {
	    charset = T_NAME_CP437;
	} else if (fontname) {
	    i = 2;
	    for (s = fontname + strlen(fontname) - 1; i && s >= fontname; s--) {
		if (*s == '-')
		    i--;
	    }
	    if (!i)
		charset = s + 2; /* skip current char and '-' */
	}
	    
	if (!charset) {
	    if (xsfont->min_byte1 < xsfont->max_byte1) {
		/* font is more than just 8-bit. For now, assume it's unicode */
		printk("    X11_InitHW: font `%."STR(SMALLBUFF)"s\' has no known charset encoding,\n"
		       "                assuming Unicode.\n", fontname);
		return NULL;
	    }
	    /* else assume codepage437. gross. */
	    printk("    X11_InitHW: font `%."STR(SMALLBUFF)"s\' has no known charset encoding,\n"
		   "                assuming CP437 codepage (\"VGA\").\n", fontname);
	    return Tutf_UTF_16_to_CP437;
	}
    }
    
    i = Tutf_charset_id(charset);
    s = Tutf_charset_name(i);
    if (s && !strcmp(s, T_NAME_UTF_16)) {
	/* this is an Unicode font. good. */
	return NULL;
    }
    
    if (i == (uldat)-1) {
	printk("      X11_InitHW(): libTutf warning: unknown charset `%." STR(SMALLBUFF) "s', assuming `CP437'\n", charset);
	return Tutf_UTF_16_to_CP437;
    }
    
    return Tutf_UTF_16_to_charset_function(i);
}


static hwfont X11_UTF_16_to_UTF_16(hwfont c) {
    if ((c & 0xFE00) == 0xF000)
	/* direct-to-font zone */
	c &= 0x01FF;
    return c;
}

#endif

    
