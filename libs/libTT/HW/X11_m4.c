




/* This file was automatically generated from X11_c by m4/hw.m4h, do not edit! */


/*
 *  X11_m4.c  --  implementation of libTT methods on "X11" target display
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
 * this file is X11_c and is preprocessed by m4 to produce X11_m4.c
 * 
 * It must be a valid m4 file, and must produce a valid C include file.
 * 
 */


#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>



/* order is crucial here! */

#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

/* include our internal copy of TT.h instead of the public one */
#include "TT.h"
#include <TT/TTerrno.h>

#include "mutex.h"

#include "TTextern.h"
#include "TTassert.h"

#include "utils.h"
#include "inlines.h"
#include "seterrno.h"
#include "theme.h"
#include "common_m4.h"

static s_ttclasses X11_TTClasses;



/*
 * We need to write down only overloaded methods.
 */
 
#define XNUMCOL 16

static Display *dpy;
static Window root;
static int xwfont, xhfont, xupfont;
static int screen;
static int depth;
static Colormap colormap;
static unsigned long xcol[XNUMCOL];
static XFontStruct *xsfont;
static GC           xgc;
static XGCValues    xsgc;
static Atom         xWM_PROTOCOLS, xWM_DELETE_WINDOW, xTARGETS;
static XSizeHints   xhints;

static Tutf_function xUTF_16_to_charset;

#if 0
static ttbyte exitmainloop;
#endif

static ttfont X11_UTF_16_to_UTF_16(ttfont c) {
    if ((c & 0xFE00) == 0xF000)
	/* direct-to-font zone */
	c &= 0x01FF;
    return c;
}



/* this can stay static, X11_FlushHW() is not reentrant */
static ttcol _col;



#ifdef CONF__UNICODE
# define XDRAW(col, buf, buflen) \
    if (xsgc.foreground != xcol[COLFG(col)]) \
	XSetForeground(dpy, xgc, xsgc.foreground = xcol[COLFG(col)]); \
    if (xsgc.background != xcol[COLBG(col)]) \
	XSetBackground(dpy, xgc, xsgc.background = xcol[COLBG(col)]); \
    XDrawImageString16(dpy, w, xgc, xbegin, ybegin + xupfont, buf, buflen)
#else
# define XDRAW(col, buf, buflen) \
    if (xsgc.foreground != xcol[COLFG(col)]) \
	XSetForeground(dpy, xgc, xsgc.foreground = xcol[COLFG(col)]); \
    if (xsgc.background != xcol[COLBG(col)]) \
	XSetBackground(dpy, xgc, xsgc.background = xcol[COLBG(col)]); \
    XDrawImageString(dpy, w, xgc, xbegin, ybegin + xupfont, buf, buflen)
#endif

TT_INLINE void X11_Mogrify(Window w, ttshort x, ttshort y, ttattr *Text, ttattr *OldText, ttuint len, ttbyte ValidOldText) {
    ttattr *V, *oV;
    ttcol col;
    ttushort buflen = 0;
#ifdef CONF__UNICODE
    ttfont f;
    XChar2b buf[TT_SMALLBUFF];
#else
    ttbyte buf[TT_SMALLBUFF];
#endif
    int xbegin = x * xwfont, ybegin = y * xhfont;
    
    V = Text;
    oV = OldText;
    
    for (_col = ~HWCOL(*V); len; x++, V++, oV++, len--) {
	col = HWCOL(*V);
	if (buflen && (col != _col || (ValidOldText && *V == *oV) || buflen == TT_SMALLBUFF)) {
	    XDRAW(_col, buf, buflen);
	    buflen = 0;
	}
	if (!ValidOldText || *V != *oV) {
	    if (!buflen) {
		xbegin = x * xwfont;
		_col = col;
	    }
#ifdef CONF__UNICODE
	    f = xUTF_16_to_charset(HWFONT(*V));
	    buf[buflen  ].byte1 = f >> 8;
	    buf[buflen++].byte2 = f & 0xFF;
#else
	    buf[buflen++] = HWFONT(*V);
#endif
	}
    }
    if (buflen) {
	XDRAW(_col, buf, buflen);
	buflen = 0;
    }
}




/* ttobj */


/* ttobject */
static void X11_Break_ttobject(ttobject o) {
    if (o) {
	if (o->native) {
	    XDestroyWindow(dpy, (Window)o->native);
	    o->native = (opaque)0;
	}
    }
}

/* ttnative */
static ttnative X11_GetRoot_ttnative(void) {
    return Create_ttnative(root);
}
static void X11_Break_ttnative(ttnative o) {
    o->native = TT_NOID;
    TClass_default(ttnative)->Break(o);
}


/* ttwidget */
static ttwidget X11_Build_ttwidget(ttwidget o) {
    if ((o->native = (opaque)XCreateSimpleWindow
	 (dpy, root, o->x * xwfont, o->y * xhfont,
	  o->w * xwfont, o->h * xhfont, 0, 0, xcol[o->col]))) {
	
	return o;
    }
    return NULL;
}
static void X11_AddTo_ttwidget(ttwidget o, ttvisible parent, ttany constraints) {
    if (parent && !o->parent) {
	TClass_default(ttwidget)->AddTo(o, parent, constraints);

	XReparentWindow(dpy, (Window)o->native, (Window)parent->native, o->x * xwfont, o->y * xhfont);
	
	if (o->vflags & ttvisible_vflags_visible)
	    XMapWindow(dpy, (Window)o->native);
    }
}
static void X11_SetVisible_ttwidget(ttwidget o, ttbyte on_off) {
    if (!on_off != !(o->vflags & ttvisible_vflags_visible)) {
	o->vflags ^= ttvisible_vflags_visible;
	if (o->parent && on_off)
	    XMapWindow(dpy, (Window)o->native);
	else
	    XUnmapWindow(dpy, (Window)o->native);
    }
}
static void X11_Remove_ttwidget(ttwidget o) {
    if (o->parent) {
	TClass_default(ttwidget)->Remove(o);
	
	XUnmapWindow(dpy, (Window)o->native);
    }
}


/* ttframe */
static ttframe X11_Build_ttframe(ttframe o) {
    /* ttframe are non-visible when created */
    o->vflags &= ~ttvisible_vflags_visible;
    
    if (TClass_default(ttframe)->Build(o)) {
	
	XChangeProperty(dpy, (Window)o->native, xWM_PROTOCOLS,
			XA_ATOM, 32, PropModeReplace,
			(unsigned char *) &xWM_DELETE_WINDOW, 1);
	XSetWMNormalHints(dpy, (Window)o->native, &xhints);

	o->Class->AddTo(o, (ttvisible)TClass(ttnative)->GetRoot(), ttlayout_constraint_root);

	return o;
    }
    return NULL;
}







static ttbyte X11_FireEvent(XEvent *ev) {
    /* FIXME: finish this */
    return TRUE;
}

static ttbyte X11_Sync(void) {
    return XSync(dpy, False);
}
static ttbyte X11_Flush(void) {
    return XFlush(dpy);
}
static ttbyte X11_TimidFlush(void) {
    return XFlush(dpy);
}
#if 0
static ttbyte X11_MainLoop(void) {
    XEvent ev;

    while (!exitmainloop && XNextEvent(dpy, &ev)) {
	if (!X11_FireEvent(&ev))
	    break;
    }
    
    exitmainloop = FALSE;
    return 1;
}
static void X11_ExitMainLoop(void) {
    exitmainloop = TT_TRUE;
}
#endif
static ttbyte X11_MainLoopOnce(ttbyte wait) {
    XEvent ev;

    do {
	if (wait || XEventsQueued(dpy, QueuedAfterFlush)) {
	    XNextEvent(dpy, &ev);
	    X11_FireEvent(&ev);
	}
    } while (XEventsQueued(dpy, QueuedAlready));

    return 1;
}
static void X11_DeleteCallback(ttcallback o) {
}
static void X11_Close(void) {
    XCloseDisplay(dpy);
    dpy = NULL;
}
static int X11_ConnectionFd(void) {
  return XConnectionNumber(dpy);
}
static ttuint X11_GetErrno(void) {
  return 0;
}
static ttuint X11_GetErrnoDetail(void) {
  return 0;
}
static TT_CONST ttbyte *X11_StrError(ttuint E) {
    switch (E) {
    }
    return "";
}
static TT_CONST ttbyte *X11_StrErrorDetail(ttuint E, ttuint S) {
    return "";
}







#ifdef CONF__UNICODE
static Tutf_function X11_UTF_16_to_charset_function(TT_CONST ttbyte *charset) {
    XFontProp *fp;
    Atom fontatom;
    TT_CONST ttbyte *s, *fontname = NULL;
    ttuint i;
    
    if (!charset) {
	/* attempt to autodetect encoding. */
	fp = xsfont->properties;
	fontatom = XInternAtom (dpy, "FONT", False);
	i = xsfont->n_properties;
	
	while (i--) {
	    if (fp->name == fontatom) {
		fontname = XGetAtomName(dpy, fp->card32);
		/*fprintf(stderr, "    X11_UTF_16_to...: font name: %s\\n", fontname);*/
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
		/*
		printk("    _TT_X11_Init: font %."TTSB"s\ has no known charset encoding,\n"
		       "                assuming Unicode.\n", fontname);
		 */
		return NULL;
	    }
	    /* else assume codepage437. gross. */
	    /*
	    printk("    _TT_X11_Init: font %."TTSB"s\ has no known charset encoding,\n"
		   "                assuming CP437 codepage (\"VGA\").\n", fontname);
	     */
	    return Tutf_UTF_16_to_CP437;
	}
    }
    
    i = Tutf_charset_id(charset);
    s = Tutf_charset_name(i);
    if (s && !strcmp(s, T_NAME_UTF_16)) {
	/* this is an Unicode font. good. */
	return NULL;
    }
    
    if (i == (ttuint)-1) {
	/*
	printk("      X11_InitHW(): libTutf warning: unknown charset %."TTSB"s, assuming CP437\n", charset);
	 */
	return Tutf_UTF_16_to_CP437;
    }
    
    return Tutf_UTF_16_to_charset_function(i);
}
#endif







#ifdef THIS_MODULE
ttclasses InitModule(tthw *HW)
#else
ttclasses _TT_X11_InitHW(tthw *HW)
#endif
{
    XColor xcolor;
    ttuint i;
    TT_CONST ttbyte *fontname = NULL, *charset = NULL;
    
    /*
     * (*HW) must be set even if initialization fails,
     * to report error messages.
     */
    *HW = &X11_TTClasses.HW;

    do if ((dpy = XOpenDisplay(NULL))) {
	root = DefaultRootWindow(dpy);
	screen = DefaultScreen(dpy);
	depth = DefaultDepth(dpy, screen);
	colormap = DefaultColormap(dpy, screen);
	
	for (i = 0; i < XNUMCOL; i++) {
	    xcolor.red   = 0x5555 * ((i & 4 ? 2 : 0) | (i & 8 ? 1 : 0));
	    xcolor.green = 0x5555 * ((i & 2 ? 2 : 0) | (i & 8 ? 1 : 0));
	    xcolor.blue  = 0x5555 * ((i & 1 ? 2 : 0) | (i & 8 ? 1 : 0));
	    if (!XAllocColor(dpy, colormap, &xcolor)) {
		
		break;
	    }
	    xcol[i] = xcolor.pixel;
	}
	if (i < XNUMCOL)
	    break;

	if (((fontname && (xsfont = XLoadQueryFont(dpy, fontname))) ||
	     (xsfont = XLoadQueryFont(dpy, "vga")) ||
	     (xsfont = XLoadQueryFont(dpy, "fixed")))) {
	    
	    
	    xwfont = xsfont->min_bounds.width;
	    xhfont = (xupfont = xsfont->ascent) + xsfont->descent;

	    xsgc.foreground = xsgc.background = xcol[0];
	    xsgc.graphics_exposures = False;
	    xsgc.font = xsfont->fid;
	    
	    if ((xgc = XCreateGC(dpy, root, GCFont|GCForeground|GCBackground|
				 GCGraphicsExposures, &xsgc))) {

		/*
		 * ask ICCCM-compliant window manager to tell us when close window
		 * has been chosen, rather than just killing us
		 */
		xWM_PROTOCOLS = XInternAtom(dpy, "WM_PROTOCOLS", False);
		xWM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
		xTARGETS = XInternAtom(dpy, "TARGETS", False);
		
		xhints.flags = PResizeInc;
		xhints.width_inc  = xwfont;
		xhints.height_inc = xhfont;
		
#ifdef CONF__UNICODE
		if (!(xUTF_16_to_charset = X11_UTF_16_to_charset_function(charset)))
		    xUTF_16_to_charset = X11_UTF_16_to_UTF_16;
#endif	    
		return &X11_TTClasses;
	    }
	}
    } while (0);
    
    if (dpy)
	X11_Close();
    
    /*FAIL(, ); */

    return (ttclasses)0;
}






/*
 * use default values for methods not implemented in X11_c
 *
 * null display target MUST IMPLEMENT ALL METHODS, even if as stubs.
 */











static s_ttclasses X11_TTClasses = {

 
  {
    Class_ttobj,
    id_ttobj,
    magicmask_ttobj,
    magic_ttobj,
    size_ttobj,

  },
 
  {
    Class_ttclass,
    id_ttclass,
    magicmask_ttclass,
    magic_ttclass,
    size_ttclass,

    /* extends ttclass_ttobj */

  },
 
  {
    Class_ttobject,
    id_ttobject,
    magicmask_ttobject,
    magic_ttobject,
    size_ttobject,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttobject */,
    (void *)NULL /* UNDEFINED: Build_ttobject */,
    X11_Break_ttobject,
    (void *)NULL /* UNDEFINED: Del_ttobject */,

  },
 
  {
    Class_ttvector,
    id_ttvector,
    magicmask_ttvector,
    magic_ttvector,
    size_ttvector,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttvector */,
    (void *)NULL /* UNDEFINED: Build_ttvector */,
    (void *)NULL /* inherits:  Break_ttvector >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttvector */,

    /* extends ttclass_ttobject */
    (void *)NULL /* UNDEFINED: GetAt_ttvector */,
    (void *)NULL /* UNDEFINED: SetAt_ttvector */,
    (void *)NULL /* UNDEFINED: AddY_ttvector */,
    (void *)NULL /* UNDEFINED: ContainsValue_ttvector */,
    (void *)NULL /* UNDEFINED: RemoveRange_ttvector */,

  },
 
  {
    Class_ttfield,
    id_ttfield,
    magicmask_ttfield,
    magic_ttfield,
    size_ttfield,

    /* extends ttclass_ttobj */

  },
 
  {
    Class_ttmethod,
    id_ttmethod,
    magicmask_ttmethod,
    magic_ttmethod,
    size_ttmethod,

    /* extends ttclass_ttobj */

  },
 
  {
    Class_ttevent,
    id_ttevent,
    magicmask_ttevent,
    magic_ttevent,
    size_ttevent,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttevent */,
    (void *)NULL /* UNDEFINED: Build_ttevent */,
    (void *)NULL /* inherits:  Break_ttevent >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttevent */,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_tteventbig,
    id_tteventbig,
    magicmask_tteventbig,
    magic_tteventbig,
    size_tteventbig,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tteventbig */,
    (void *)NULL /* UNDEFINED: Build_tteventbig */,
    (void *)NULL /* inherits:  Break_tteventbig >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tteventbig */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttevent */

  },
 
  {
    Class_ttbitmask,
    id_ttbitmask,
    magicmask_ttbitmask,
    magic_ttbitmask,
    size_ttbitmask,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttbitmask */,
    (void *)NULL /* UNDEFINED: Build_ttbitmask */,
    (void *)NULL /* inherits:  Break_ttbitmask >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttbitmask */,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_tteventmask,
    id_tteventmask,
    magicmask_tteventmask,
    magic_tteventmask,
    size_tteventmask,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tteventmask */,
    (void *)NULL /* UNDEFINED: Build_tteventmask */,
    (void *)NULL /* inherits:  Break_tteventmask >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tteventmask */,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_ttcallback,
    id_ttcallback,
    magicmask_ttcallback,
    magic_ttcallback,
    size_ttcallback,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttcallback */,
    (void *)NULL /* UNDEFINED: Build_ttcallback */,
    (void *)NULL /* inherits:  Break_ttcallback >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttcallback */,

    /* extends ttclass_ttobject */
    (void *)NULL /* UNDEFINED: Create_ttcallback */,

  },
 
  {
    Class_ttlistener,
    id_ttlistener,
    magicmask_ttlistener,
    magic_ttlistener,
    size_ttlistener,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttlistener */,
    (void *)NULL /* UNDEFINED: Build_ttlistener */,
    (void *)NULL /* inherits:  Break_ttlistener >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttlistener */,

    /* extends ttclass_ttobject */
    (void *)NULL /* UNDEFINED: Create_ttlistener */,

    /* extends ttclass_ttcallback */
    (void *)NULL /* UNDEFINED: AddTo_ttlistener */,
    (void *)NULL /* UNDEFINED: Remove_ttlistener */,

  },
 
  {
    Class_tttimer,
    id_tttimer,
    magicmask_tttimer,
    magic_tttimer,
    size_tttimer,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tttimer */,
    (void *)NULL /* UNDEFINED: Build_tttimer */,
    (void *)NULL /* inherits:  Break_tttimer >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tttimer */,

    /* extends ttclass_ttobject */
    (void *)NULL /* UNDEFINED: Create_tttimer */,

    /* extends ttclass_ttcallback */
    (void *)NULL /* UNDEFINED: AddTo_tttimer */,
    (void *)NULL /* UNDEFINED: Remove_tttimer */,

    /* extends ttclass_ttlistener */
    (void *)NULL /* UNDEFINED: SetEnabled_tttimer */,
    (void *)NULL /* UNDEFINED: IsEnabled_tttimer */,

  },
 
  {
    Class_ttcomponent,
    id_ttcomponent,
    magicmask_ttcomponent,
    magic_ttcomponent,
    size_ttcomponent,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttcomponent */,
    (void *)NULL /* UNDEFINED: Build_ttcomponent */,
    (void *)NULL /* inherits:  Break_ttcomponent >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttcomponent */,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_ttdata,
    id_ttdata,
    magicmask_ttdata,
    magic_ttdata,
    size_ttdata,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttdata */,
    (void *)NULL /* UNDEFINED: Build_ttdata */,
    (void *)NULL /* inherits:  Break_ttdata >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttdata */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttdata */,
    (void *)NULL /* UNDEFINED: Remove_ttdata */,

  },
 
  {
    Class_ttvisible,
    id_ttvisible,
    magicmask_ttvisible,
    magic_ttvisible,
    size_ttvisible,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttvisible */,
    (void *)NULL /* UNDEFINED: Build_ttvisible */,
    (void *)NULL /* inherits:  Break_ttvisible >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttvisible */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttvisible */,
    (void *)NULL /* UNDEFINED: Validate_ttvisible */,
    (void *)NULL /* UNDEFINED: Remove_ttvisible */,
    (void *)NULL /* UNDEFINED: Invalidate_ttvisible */,
    (void *)NULL /* UNDEFINED: SetVisible_ttvisible */,
    (void *)NULL /* UNDEFINED: SetTheme_ttvisible */,
    (void *)NULL /* UNDEFINED: Draw_ttvisible */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttvisible */,


  },
 
  {
    Class_ttlayout,
    id_ttlayout,
    magicmask_ttlayout,
    magic_ttlayout,
    size_ttlayout,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttlayout */,
    (void *)NULL /* UNDEFINED: Build_ttlayout */,
    (void *)NULL /* inherits:  Break_ttlayout >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttlayout */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: SetWidget_ttlayout */,
    (void *)NULL /* UNDEFINED: DoLayout_ttlayout */,

  },
 
  {
    Class_ttboxlayout,
    id_ttboxlayout,
    magicmask_ttboxlayout,
    magic_ttboxlayout,
    size_ttboxlayout,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttboxlayout */,
    (void *)NULL /* UNDEFINED: Build_ttboxlayout */,
    (void *)NULL /* inherits:  Break_ttboxlayout >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttboxlayout */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: SetWidget_ttboxlayout */,
    (void *)NULL /* UNDEFINED: DoLayout_ttboxlayout */,

    /* extends ttclass_ttlayout */

  },
 
  {
    Class_ttborderlayout,
    id_ttborderlayout,
    magicmask_ttborderlayout,
    magic_ttborderlayout,
    size_ttborderlayout,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttborderlayout */,
    (void *)NULL /* UNDEFINED: Build_ttborderlayout */,
    (void *)NULL /* inherits:  Break_ttborderlayout >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttborderlayout */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: SetWidget_ttborderlayout */,
    (void *)NULL /* UNDEFINED: DoLayout_ttborderlayout */,

    /* extends ttclass_ttlayout */

  },
 
  {
    Class_ttnative,
    id_ttnative,
    magicmask_ttnative,
    magic_ttnative,
    size_ttnative,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttnative */,
    (void *)NULL /* UNDEFINED: Build_ttnative */,
    X11_Break_ttnative,
    (void *)NULL /* UNDEFINED: Del_ttnative */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttnative */,
    (void *)NULL /* UNDEFINED: Validate_ttnative */,
    (void *)NULL /* UNDEFINED: Remove_ttnative */,
    (void *)NULL /* UNDEFINED: Invalidate_ttnative */,
    (void *)NULL /* UNDEFINED: SetVisible_ttnative */,
    (void *)NULL /* UNDEFINED: SetTheme_ttnative */,
    (void *)NULL /* UNDEFINED: Draw_ttnative */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttnative */,


    /* extends ttclass_ttvisible */
    X11_GetRoot_ttnative,
    (void *)NULL /* UNDEFINED: GetW_ttnative */,
    (void *)NULL /* UNDEFINED: GetH_ttnative */,

  },
 
  {
    Class_ttwidget,
    id_ttwidget,
    magicmask_ttwidget,
    magic_ttwidget,
    size_ttwidget,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttwidget */,
    X11_Build_ttwidget,
    (void *)NULL /* inherits:  Break_ttwidget >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttwidget */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    X11_AddTo_ttwidget,
    (void *)NULL /* UNDEFINED: Validate_ttwidget */,
    X11_Remove_ttwidget,
    (void *)NULL /* UNDEFINED: Invalidate_ttwidget */,
    X11_SetVisible_ttwidget,
    (void *)NULL /* UNDEFINED: SetTheme_ttwidget */,
    (void *)NULL /* UNDEFINED: Draw_ttwidget */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttwidget */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttwidget */,
    (void *)NULL /* UNDEFINED: SetXY_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWH_ttwidget */,
    (void *)NULL /* UNDEFINED: SetXl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetYl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetWl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetHl_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttwidget */,

  },
 
  {
    Class_ttlabel,
    id_ttlabel,
    magicmask_ttlabel,
    magic_ttlabel,
    size_ttlabel,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttlabel */,
    (void *)NULL /* inherits:  Build_ttlabel >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttlabel >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttlabel */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttlabel >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttlabel */,
    (void *)NULL /* inherits:  Remove_ttlabel >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttlabel */,
    (void *)NULL /* inherits:  SetVisible_ttlabel >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttlabel */,
    (void *)NULL /* UNDEFINED: Draw_ttlabel */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttlabel */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttlabel */,
    (void *)NULL /* UNDEFINED: SetXY_ttlabel */,
    (void *)NULL /* UNDEFINED: SetWH_ttlabel */,
    (void *)NULL /* UNDEFINED: SetXl_ttlabel */,
    (void *)NULL /* UNDEFINED: SetYl_ttlabel */,
    (void *)NULL /* UNDEFINED: SetWl_ttlabel */,
    (void *)NULL /* UNDEFINED: SetHl_ttlabel */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttlabel */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetFontD_ttlabel */,

  },
 
  {
    Class_tttooltip,
    id_tttooltip,
    magicmask_tttooltip,
    magic_tttooltip,
    size_tttooltip,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tttooltip */,
    (void *)NULL /* inherits:  Build_tttooltip >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_tttooltip >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tttooltip */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttooltip >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_tttooltip */,
    (void *)NULL /* inherits:  Remove_tttooltip >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_tttooltip */,
    (void *)NULL /* inherits:  SetVisible_tttooltip >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttooltip */,
    (void *)NULL /* UNDEFINED: Draw_tttooltip */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_tttooltip */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_tttooltip */,
    (void *)NULL /* UNDEFINED: SetXY_tttooltip */,
    (void *)NULL /* UNDEFINED: SetWH_tttooltip */,
    (void *)NULL /* UNDEFINED: SetXl_tttooltip */,
    (void *)NULL /* UNDEFINED: SetYl_tttooltip */,
    (void *)NULL /* UNDEFINED: SetWl_tttooltip */,
    (void *)NULL /* UNDEFINED: SetHl_tttooltip */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttooltip */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetFontD_tttooltip */,

    /* extends ttclass_ttlabel */
    (void *)NULL /* UNDEFINED: SetWidget_tttooltip */,

  },
 
  {
    Class_ttbuttongroup,
    id_ttbuttongroup,
    magicmask_ttbuttongroup,
    magic_ttbuttongroup,
    size_ttbuttongroup,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttbuttongroup */,
    (void *)NULL /* UNDEFINED: Build_ttbuttongroup */,
    (void *)NULL /* inherits:  Break_ttbuttongroup >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttbuttongroup */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: Add_ttbuttongroup */,
    (void *)NULL /* UNDEFINED: Remove_ttbuttongroup */,
    (void *)NULL /* UNDEFINED: SetChecked_ttbuttongroup */,

  },
 
  {
    Class_ttanybutton,
    id_ttanybutton,
    magicmask_ttanybutton,
    magic_ttanybutton,
    size_ttanybutton,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttanybutton */,
    (void *)NULL /* inherits:  Build_ttanybutton >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttanybutton >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttanybutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanybutton >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttanybutton */,
    (void *)NULL /* inherits:  Remove_ttanybutton >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttanybutton >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanybutton */,
    (void *)NULL /* UNDEFINED: Draw_ttanybutton */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttanybutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetXY_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetWH_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetXl_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetYl_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetWl_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetHl_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanybutton */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttanybutton */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttanybutton */,

  },
 
  {
    Class_ttbutton,
    id_ttbutton,
    magicmask_ttbutton,
    magic_ttbutton,
    size_ttbutton,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttbutton */,
    (void *)NULL /* inherits:  Build_ttbutton >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttbutton >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttbutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttbutton >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttbutton */,
    (void *)NULL /* inherits:  Remove_ttbutton >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttbutton */,
    (void *)NULL /* inherits:  SetVisible_ttbutton >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttbutton */,
    (void *)NULL /* UNDEFINED: Draw_ttbutton */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttbutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttbutton */,
    (void *)NULL /* UNDEFINED: SetXY_ttbutton */,
    (void *)NULL /* UNDEFINED: SetWH_ttbutton */,
    (void *)NULL /* UNDEFINED: SetXl_ttbutton */,
    (void *)NULL /* UNDEFINED: SetYl_ttbutton */,
    (void *)NULL /* UNDEFINED: SetWl_ttbutton */,
    (void *)NULL /* UNDEFINED: SetHl_ttbutton */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttbutton */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttbutton */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttbutton */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttbutton */,

  },
 
  {
    Class_ttcheckbutton,
    id_ttcheckbutton,
    magicmask_ttcheckbutton,
    magic_ttcheckbutton,
    size_ttcheckbutton,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttcheckbutton */,
    (void *)NULL /* inherits:  Build_ttcheckbutton >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttcheckbutton >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttcheckbutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttcheckbutton >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttcheckbutton */,
    (void *)NULL /* inherits:  Remove_ttcheckbutton >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttcheckbutton */,
    (void *)NULL /* inherits:  SetVisible_ttcheckbutton >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: Draw_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttcheckbutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetXY_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetWH_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetXl_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetYl_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetWl_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetHl_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttcheckbutton */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttcheckbutton */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttcheckbutton */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttcheckbutton */,

    /* extends ttclass_ttbutton */
    (void *)NULL /* UNDEFINED: SetChecked_ttcheckbutton */,

  },
 
  {
    Class_ttradiobutton,
    id_ttradiobutton,
    magicmask_ttradiobutton,
    magic_ttradiobutton,
    size_ttradiobutton,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttradiobutton */,
    (void *)NULL /* inherits:  Build_ttradiobutton >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttradiobutton >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttradiobutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttradiobutton >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttradiobutton */,
    (void *)NULL /* inherits:  Remove_ttradiobutton >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttradiobutton */,
    (void *)NULL /* inherits:  SetVisible_ttradiobutton >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttradiobutton */,
    (void *)NULL /* UNDEFINED: Draw_ttradiobutton */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttradiobutton */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetXY_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetWH_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetXl_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetYl_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetWl_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetHl_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttradiobutton */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttradiobutton */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttradiobutton */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttradiobutton */,

    /* extends ttclass_ttbutton */
    (void *)NULL /* UNDEFINED: SetChecked_ttradiobutton */,

    /* extends ttclass_ttcheckbutton */

  },
 
  {
    Class_ttanyscroll,
    id_ttanyscroll,
    magicmask_ttanyscroll,
    magic_ttanyscroll,
    size_ttanyscroll,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttanyscroll */,
    (void *)NULL /* inherits:  Build_ttanyscroll >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttanyscroll >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttanyscroll */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanyscroll >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttanyscroll */,
    (void *)NULL /* inherits:  Remove_ttanyscroll >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttanyscroll */,
    (void *)NULL /* inherits:  SetVisible_ttanyscroll >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanyscroll */,
    (void *)NULL /* UNDEFINED: Draw_ttanyscroll */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttanyscroll */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetXY_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetWH_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetXl_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetYl_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetWl_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetHl_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanyscroll */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttanyscroll */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetState_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetSize_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttanyscroll */,
    (void *)NULL /* UNDEFINED: SetPosition_ttanyscroll */,

  },
 
  {
    Class_ttscrollbar,
    id_ttscrollbar,
    magicmask_ttscrollbar,
    magic_ttscrollbar,
    size_ttscrollbar,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttscrollbar */,
    (void *)NULL /* inherits:  Build_ttscrollbar >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttscrollbar >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttscrollbar */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscrollbar >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttscrollbar */,
    (void *)NULL /* inherits:  Remove_ttscrollbar >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttscrollbar */,
    (void *)NULL /* inherits:  SetVisible_ttscrollbar >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscrollbar */,
    (void *)NULL /* UNDEFINED: Draw_ttscrollbar */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttscrollbar */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetXY_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetWH_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetXl_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetYl_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetWl_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetHl_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscrollbar */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttscrollbar */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetState_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetSize_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttscrollbar */,
    (void *)NULL /* UNDEFINED: SetPosition_ttscrollbar */,

    /* extends ttclass_ttanyscroll */

  },
 
  {
    Class_ttslider,
    id_ttslider,
    magicmask_ttslider,
    magic_ttslider,
    size_ttslider,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttslider */,
    (void *)NULL /* inherits:  Build_ttslider >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttslider >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttslider */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttslider >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttslider */,
    (void *)NULL /* inherits:  Remove_ttslider >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttslider */,
    (void *)NULL /* inherits:  SetVisible_ttslider >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttslider */,
    (void *)NULL /* UNDEFINED: Draw_ttslider */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttslider */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttslider */,
    (void *)NULL /* UNDEFINED: SetXY_ttslider */,
    (void *)NULL /* UNDEFINED: SetWH_ttslider */,
    (void *)NULL /* UNDEFINED: SetXl_ttslider */,
    (void *)NULL /* UNDEFINED: SetYl_ttslider */,
    (void *)NULL /* UNDEFINED: SetWl_ttslider */,
    (void *)NULL /* UNDEFINED: SetHl_ttslider */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttslider */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttslider */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttslider */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttslider */,
    (void *)NULL /* UNDEFINED: SetState_ttslider */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttslider */,
    (void *)NULL /* UNDEFINED: SetSize_ttslider */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttslider */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttslider */,
    (void *)NULL /* UNDEFINED: SetPosition_ttslider */,

    /* extends ttclass_ttanyscroll */
    (void *)NULL /* UNDEFINED: SetSlideMin_ttslider */,
    (void *)NULL /* UNDEFINED: SetSlideMax_ttslider */,
    (void *)NULL /* UNDEFINED: SetSlideValue_ttslider */,

  },
 
  {
    Class_ttprogressbar,
    id_ttprogressbar,
    magicmask_ttprogressbar,
    magic_ttprogressbar,
    size_ttprogressbar,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttprogressbar */,
    (void *)NULL /* inherits:  Build_ttprogressbar >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttprogressbar >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttprogressbar */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttprogressbar >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttprogressbar */,
    (void *)NULL /* inherits:  Remove_ttprogressbar >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttprogressbar */,
    (void *)NULL /* inherits:  SetVisible_ttprogressbar >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttprogressbar */,
    (void *)NULL /* UNDEFINED: Draw_ttprogressbar */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttprogressbar */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetXY_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetWH_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetXl_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetYl_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetWl_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetHl_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttprogressbar */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetAttrD_ttprogressbar */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetState_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetSize_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttprogressbar */,
    (void *)NULL /* UNDEFINED: SetPosition_ttprogressbar */,

    /* extends ttclass_ttanyscroll */

  },
 
  {
    Class_ttscrollpane,
    id_ttscrollpane,
    magicmask_ttscrollpane,
    magic_ttscrollpane,
    size_ttscrollpane,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttscrollpane */,
    (void *)NULL /* inherits:  Build_ttscrollpane >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttscrollpane >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttscrollpane */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscrollpane >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttscrollpane */,
    (void *)NULL /* inherits:  Remove_ttscrollpane >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttscrollpane */,
    (void *)NULL /* inherits:  SetVisible_ttscrollpane >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscrollpane */,
    (void *)NULL /* UNDEFINED: Draw_ttscrollpane */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttscrollpane */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetXY_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetWH_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetXl_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetYl_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetWl_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetHl_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscrollpane */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetBarX_ttscrollpane */,
    (void *)NULL /* UNDEFINED: SetBarY_ttscrollpane */,

  },
 
  {
    Class_ttwindow,
    id_ttwindow,
    magicmask_ttwindow,
    magic_ttwindow,
    size_ttwindow,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttwindow */,
    (void *)NULL /* inherits:  Build_ttwindow >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttwindow >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttwindow */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttwindow >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttwindow */,
    (void *)NULL /* inherits:  Remove_ttwindow >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttwindow */,
    (void *)NULL /* inherits:  SetVisible_ttwindow >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttwindow */,
    (void *)NULL /* UNDEFINED: Draw_ttwindow */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttwindow */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttwindow */,
    (void *)NULL /* UNDEFINED: SetXY_ttwindow */,
    (void *)NULL /* UNDEFINED: SetWH_ttwindow */,
    (void *)NULL /* UNDEFINED: SetXl_ttwindow */,
    (void *)NULL /* UNDEFINED: SetYl_ttwindow */,
    (void *)NULL /* UNDEFINED: SetWl_ttwindow */,
    (void *)NULL /* UNDEFINED: SetHl_ttwindow */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttwindow */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttwindow */,

  },
 
  {
    Class_ttframe,
    id_ttframe,
    magicmask_ttframe,
    magic_ttframe,
    size_ttframe,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttframe */,
    X11_Build_ttframe,
    (void *)NULL /* inherits:  Break_ttframe >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttframe */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttframe >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttframe */,
    (void *)NULL /* inherits:  Remove_ttframe >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttframe */,
    (void *)NULL /* inherits:  SetVisible_ttframe >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttframe */,
    (void *)NULL /* UNDEFINED: Draw_ttframe */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttframe */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttframe */,
    (void *)NULL /* UNDEFINED: SetXY_ttframe */,
    (void *)NULL /* UNDEFINED: SetWH_ttframe */,
    (void *)NULL /* UNDEFINED: SetXl_ttframe */,
    (void *)NULL /* UNDEFINED: SetYl_ttframe */,
    (void *)NULL /* UNDEFINED: SetWl_ttframe */,
    (void *)NULL /* UNDEFINED: SetHl_ttframe */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttframe */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttframe */,

    /* extends ttclass_ttwindow */
    (void *)NULL /* UNDEFINED: SetMenubar_ttframe */,

  },
 
  {
    Class_ttscroller,
    id_ttscroller,
    magicmask_ttscroller,
    magic_ttscroller,
    size_ttscroller,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttscroller */,
    (void *)NULL /* inherits:  Build_ttscroller >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttscroller >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttscroller */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscroller >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttscroller */,
    (void *)NULL /* inherits:  Remove_ttscroller >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttscroller */,
    (void *)NULL /* inherits:  SetVisible_ttscroller >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscroller */,
    (void *)NULL /* UNDEFINED: Draw_ttscroller */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttscroller */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttscroller */,
    (void *)NULL /* UNDEFINED: SetXY_ttscroller */,
    (void *)NULL /* UNDEFINED: SetWH_ttscroller */,
    (void *)NULL /* UNDEFINED: SetXl_ttscroller */,
    (void *)NULL /* UNDEFINED: SetYl_ttscroller */,
    (void *)NULL /* UNDEFINED: SetWl_ttscroller */,
    (void *)NULL /* UNDEFINED: SetHl_ttscroller */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscroller */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttscroller */,

    /* extends ttclass_ttwindow */

  },
 
  {
    Class_ttmenuitem,
    id_ttmenuitem,
    magicmask_ttmenuitem,
    magic_ttmenuitem,
    size_ttmenuitem,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Build_ttmenuitem */,
    (void *)NULL /* inherits:  Break_ttmenuitem >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttmenuitem */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Validate_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Remove_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenuitem */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenuitem */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenuitem */,
    (void *)NULL /* UNDEFINED: Draw_ttmenuitem */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenuitem */,


    /* extends ttclass_ttvisible */

  },
 
  {
    Class_ttcheckmenuitem,
    id_ttcheckmenuitem,
    magicmask_ttcheckmenuitem,
    magic_ttcheckmenuitem,
    size_ttcheckmenuitem,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: Build_ttcheckmenuitem */,
    (void *)NULL /* inherits:  Break_ttcheckmenuitem >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttcheckmenuitem */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: Validate_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: Remove_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: Invalidate_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: SetVisible_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: SetTheme_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: Draw_ttcheckmenuitem */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttcheckmenuitem */,


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

  },
 
  {
    Class_ttradiomenuitem,
    id_ttradiomenuitem,
    magicmask_ttradiomenuitem,
    magic_ttradiomenuitem,
    size_ttradiomenuitem,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: Build_ttradiomenuitem */,
    (void *)NULL /* inherits:  Break_ttradiomenuitem >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttradiomenuitem */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: Validate_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: Remove_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: Invalidate_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: SetVisible_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: SetTheme_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: Draw_ttradiomenuitem */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttradiomenuitem */,


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

    /* extends ttclass_ttcheckmenuitem */

  },
 
  {
    Class_ttmenu,
    id_ttmenu,
    magicmask_ttmenu,
    magic_ttmenu,
    size_ttmenu,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttmenu */,
    (void *)NULL /* UNDEFINED: Build_ttmenu */,
    (void *)NULL /* inherits:  Break_ttmenu >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttmenu */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttmenu */,
    (void *)NULL /* UNDEFINED: Validate_ttmenu */,
    (void *)NULL /* UNDEFINED: Remove_ttmenu */,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenu */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenu */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenu */,
    (void *)NULL /* UNDEFINED: Draw_ttmenu */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenu */,


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

  },
 
  {
    Class_ttmenubar,
    id_ttmenubar,
    magicmask_ttmenubar,
    magic_ttmenubar,
    size_ttmenubar,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttmenubar */,
    (void *)NULL /* UNDEFINED: Build_ttmenubar */,
    (void *)NULL /* inherits:  Break_ttmenubar >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttmenubar */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttmenubar */,
    (void *)NULL /* UNDEFINED: Validate_ttmenubar */,
    (void *)NULL /* UNDEFINED: Remove_ttmenubar */,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenubar */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenubar */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenubar */,
    (void *)NULL /* UNDEFINED: Draw_ttmenubar */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenubar */,


    /* extends ttclass_ttvisible */

  },
 
  {
    Class_ttanytext,
    id_ttanytext,
    magicmask_ttanytext,
    magic_ttanytext,
    size_ttanytext,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttanytext */,
    (void *)NULL /* inherits:  Build_ttanytext >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttanytext >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttanytext */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanytext >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_ttanytext */,
    (void *)NULL /* inherits:  Remove_ttanytext >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttanytext */,
    (void *)NULL /* inherits:  SetVisible_ttanytext >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanytext */,
    (void *)NULL /* UNDEFINED: Draw_ttanytext */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttanytext */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_ttanytext */,
    (void *)NULL /* UNDEFINED: SetXY_ttanytext */,
    (void *)NULL /* UNDEFINED: SetWH_ttanytext */,
    (void *)NULL /* UNDEFINED: SetXl_ttanytext */,
    (void *)NULL /* UNDEFINED: SetYl_ttanytext */,
    (void *)NULL /* UNDEFINED: SetWl_ttanytext */,
    (void *)NULL /* UNDEFINED: SetHl_ttanytext */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanytext */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttanytext */,

    /* extends ttclass_ttwindow */

  },
 
  {
    Class_tttextfield,
    id_tttextfield,
    magicmask_tttextfield,
    magic_tttextfield,
    size_tttextfield,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tttextfield */,
    (void *)NULL /* inherits:  Build_tttextfield >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_tttextfield >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tttextfield */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttextfield >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_tttextfield */,
    (void *)NULL /* inherits:  Remove_tttextfield >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_tttextfield */,
    (void *)NULL /* inherits:  SetVisible_tttextfield >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttextfield */,
    (void *)NULL /* UNDEFINED: Draw_tttextfield */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_tttextfield */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_tttextfield */,
    (void *)NULL /* UNDEFINED: SetXY_tttextfield */,
    (void *)NULL /* UNDEFINED: SetWH_tttextfield */,
    (void *)NULL /* UNDEFINED: SetXl_tttextfield */,
    (void *)NULL /* UNDEFINED: SetYl_tttextfield */,
    (void *)NULL /* UNDEFINED: SetWl_tttextfield */,
    (void *)NULL /* UNDEFINED: SetHl_tttextfield */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttextfield */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_tttextfield */,

    /* extends ttclass_ttwindow */

    /* extends ttclass_ttanytext */

  },
 
  {
    Class_tttextarea,
    id_tttextarea,
    magicmask_tttextarea,
    magic_tttextarea,
    size_tttextarea,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tttextarea */,
    (void *)NULL /* inherits:  Build_tttextarea >> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_tttextarea >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tttextarea */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttextarea >> AddTo_ttwidget */,
    (void *)NULL /* UNDEFINED: Validate_tttextarea */,
    (void *)NULL /* inherits:  Remove_tttextarea >> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_tttextarea */,
    (void *)NULL /* inherits:  SetVisible_tttextarea >> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttextarea */,
    (void *)NULL /* UNDEFINED: Draw_tttextarea */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_tttextarea */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* UNDEFINED: SetLayout_tttextarea */,
    (void *)NULL /* UNDEFINED: SetXY_tttextarea */,
    (void *)NULL /* UNDEFINED: SetWH_tttextarea */,
    (void *)NULL /* UNDEFINED: SetXl_tttextarea */,
    (void *)NULL /* UNDEFINED: SetYl_tttextarea */,
    (void *)NULL /* UNDEFINED: SetWl_tttextarea */,
    (void *)NULL /* UNDEFINED: SetHl_tttextarea */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttextarea */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_tttextarea */,

    /* extends ttclass_ttwindow */

    /* extends ttclass_ttanytext */

    /* extends ttclass_tttextfield */

  },
 
  {
    Class_tttheme,
    id_tttheme,
    magicmask_tttheme,
    magic_tttheme,
    size_tttheme,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_tttheme */,
    (void *)NULL /* UNDEFINED: Build_tttheme */,
    (void *)NULL /* inherits:  Break_tttheme >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_tttheme */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */

  },
 
  {
    Class_ttapplication,
    id_ttapplication,
    magicmask_ttapplication,
    magic_ttapplication,
    size_ttapplication,

    /* extends ttclass_ttobj */
    (void *)NULL /* UNDEFINED: New_ttapplication */,
    (void *)NULL /* UNDEFINED: Build_ttapplication */,
    (void *)NULL /* inherits:  Break_ttapplication >> Break_ttobject */,
    (void *)NULL /* UNDEFINED: Del_ttapplication */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */

  },



  {

    X11_Sync, 
    X11_Flush, 
    X11_TimidFlush, 
    X11_MainLoopOnce, 
    X11_DeleteCallback,    
    X11_Close, 
    X11_ConnectionFd, 
    X11_GetErrno, 
    X11_GetErrnoDetail,    
    X11_StrError, 
    X11_StrErrorDetail,   
  },
};






