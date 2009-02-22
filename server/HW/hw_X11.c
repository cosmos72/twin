/*
 *  hw_X11.c  --  functions to let twin display on X11
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <Tw/Twkeys.h>

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
# include <Tutf/Tutf_defs.h>
#endif

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>                /* CARD32 */


#define HW_THIS "hw_X11"
#define THIS	"X11"
#define INIT_THIS X11_InitHW


#include "hw_X11_gfx_common0.h"


/* Display variables */

#define NEST 4


struct x11_data {
    unsigned int xwidth, xheight;
    int xwfont, xhfont, xupfont;
    
    /* we support showing only a portion of the whole twin display */
    dat xhw_view, xhw_startx, xhw_starty, xhw_endx, xhw_endy;
    
#ifdef CONF__UNICODE
    Tutf_function xUTF_16_to_charset;
#endif    
    Display     *xdisplay;
    Window       xwindow;
    GC           xgc;
    XGCValues    xsgc;
    XFontStruct *xsfont;
#ifdef HW_X11_HAVE_XIM_XIC
    XIM		 xim;
    XIC		 xic;
#endif
    XComposeStatus xcompose;
    byte         xwindow_AllVisible, xfont_map;
    obj          xRequestor[NEST];
    uldat        xReqPrivate[NEST];
    uldat        xReqCount;
    uldat        XReqCount;
    XSelectionRequestEvent XReq[NEST];
    unsigned long xcol[MAXCOL+1];
    Atom xWM_PROTOCOLS, xWM_DELETE_WINDOW, xTARGETS;
};

#define xdata		((struct x11_data *)HW->Private)
#define xwidth		(xdata->xwidth)
#define xheight		(xdata->xheight)
#define xwfont		(xdata->xwfont)
#define xhfont		(xdata->xhfont)
#define xupfont		(xdata->xupfont)

#define xhw_view	(xdata->xhw_view)
#define xhw_startx	(xdata->xhw_startx)
#define xhw_starty	(xdata->xhw_starty)
#define xhw_endx	(xdata->xhw_endx)
#define xhw_endy	(xdata->xhw_endy)

#define xUTF_16_to_charset	(xdata->xUTF_16_to_charset)
#define xdisplay	(xdata->xdisplay)
#define xwindow		(xdata->xwindow)
#define xgc		(xdata->xgc)
#define xsgc		(xdata->xsgc)
#define xsfont		(xdata->xsfont)
#define xim		(xdata->xim)
#define xic		(xdata->xic)
#define xcompose	(xdata->xcompose)

#define xwindow_AllVisible	(xdata->xwindow_AllVisible)
#define xfont_map	(xdata->xfont_map)
#define xRequestor(j)	(xdata->xRequestor[j])
#define xReqPrivate(j)	(xdata->xReqPrivate[j])
#define xReqCount	(xdata->xReqCount)
#define XReqCount	(xdata->XReqCount)
#define XReq(j)		(xdata->XReq[j])
#define xcol		(xdata->xcol)
#define xWM_PROTOCOLS	(xdata->xWM_PROTOCOLS)
#define xWM_DELETE_WINDOW	(xdata->xWM_DELETE_WINDOW)
#define xTARGETS	(xdata->xTARGETS)

#include "hw_X11_gfx_common1.h"


/* this can stay static, X11_FlushHW() is not reentrant */
static hwcol _col;

#ifdef CONF__UNICODE
# define myXDrawImageString XDrawImageString16
#else
# define myXDrawImageString XDrawImageString
#endif

#define XDRAW(col, buf, buflen) do { \
    if (xsgc.foreground != xcol[COLFG(col)]) \
	XSetForeground(xdisplay, xgc, xsgc.foreground = xcol[COLFG(col)]); \
    if (xsgc.background != xcol[COLBG(col)]) \
	XSetBackground(xdisplay, xgc, xsgc.background = xcol[COLBG(col)]); \
    myXDrawImageString(xdisplay, xwindow, xgc, xbegin, ybegin + xupfont, buf, buflen); \
} while(0)

#define XDRAW_ANY(buf, buflen, col, gfx) XDRAW(col, buf, buflen)


INLINE void X11_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV;
    hwcol col;
    udat buflen = 0;
#ifdef CONF__UNICODE
    hwfont f;
    XChar2b buf[SMALLBUFF];
#else
    byte buf[SMALLBUFF];
#endif
    int xbegin, ybegin;
    
    if (xhw_view) {
	if (x >= xhw_endx || x + len < xhw_startx ||
	    y < xhw_starty || y >= xhw_endy)
	    return;
    
	if (x + len >= xhw_endx)
	    len = xhw_endx - x;
    
	if (x < xhw_startx) {
	    len -= xhw_startx - x;
	    x = xhw_startx;
	}
    }

    xbegin = (x - xhw_startx) * xwfont;
    ybegin = (y - xhw_starty) * xhfont;
    
    V = Video + x + y * DisplayWidth;
    oV = OldVideo + x + y * DisplayWidth;
    
    for (_col = ~HWCOL(*V); len; x++, V++, oV++, len--) {
	col = HWCOL(*V);
	if (buflen && (col != _col || (ValidOldVideo && *V == *oV) || buflen == SMALLBUFF)) {
	    XDRAW(_col, buf, buflen);
	    buflen = 0;
	}
	if (!ValidOldVideo || *V != *oV) {
	    if (!buflen) {
		xbegin = (x - xhw_startx) * xwfont;
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

#include "hw_X11_gfx_common2.h"

#undef XDRAW_ANY
#undef XDRAW

static void X11_QuitHW(void) {

#ifdef HW_X11_HAVE_XIM_XIC
    if (xic)    XDestroyIC(xic);
    if (xim)    XCloseIM(xim);
#endif
    if (xsfont) XFreeFont(xdisplay, xsfont);
    if (xgc != None) XFreeGC(xdisplay, xgc);
    if (xwindow != None) {
	XUnmapWindow(xdisplay, xwindow);
	XDestroyWindow(xdisplay, xwindow);
    }
    XCloseDisplay(xdisplay);
    xdisplay = NULL;
    
    if (HW->keyboard_slot != NOSLOT)
	UnRegisterRemote(HW->keyboard_slot);
    HW->keyboard_slot = NOSLOT;
    HW->KeyboardEvent = (void *)NoOp;
    
    HW->QuitHW = NoOp;

    FreeMem(HW->Private);
}

#ifdef THIS_MODULE
static
#endif
byte X11_InitHW(void) {
    char *arg = HW->Name;
    int xscreen;
    unsigned int xdepth;
    XSetWindowAttributes xattr;
    XColor xcolor;
    XSizeHints *xhints;
    XEvent event;
    int i;
    byte *s, *xdisplay_ = NULL, *xdisplay0 = NULL,
    *fontname = NULL, *fontname0 = NULL,
    *charset = NULL, *charset0 = NULL,
    name[] = "twin :??? on X";
    byte drag = FALSE, noinput = FALSE;
    
    if (!(HW->Private = (struct x11_data *)AllocMem(sizeof(struct x11_data)))) {
	printk("      X11_InitHW(): Out of memory!\n");
	return FALSE;
    }

    /* default: show the whole screen */
    xhw_view = xhw_startx = xhw_starty = xhw_endx = xhw_endy = 0;
    
    if (arg && HW->NameLen > 4) {
	arg += 4; /* skip "-hw=" */
	
	if (*arg++ != 'X')
	    return FALSE; /* user said "use <arg> as display, not X" */
	
	if (*arg == '1' && arg[1] == '1')
	    arg += 2; /* `X11' is same as `X' */

	if (*arg == '@') {
	    if ((s = strchr(xdisplay_ = ++arg, ','))) {
		*(xdisplay0 = s) = '\0';
		arg = s + 1;
	    } else
		arg = NULL;
	}

	while (arg && *arg) {
	    /* parse options */
	    if (*arg == ',') {
		arg++;
		continue;
	    }
	    if (!strncmp(arg, "font=", 5)) {
		fontname = arg += 5;
		s = strchr(arg, ',');
		if (s) *(fontname0 = s++) = '\0';
		arg = s;
	    } else if (!strncmp(arg, "charset=", 8)) {
		charset = arg += 8;
		s = strchr(arg, ',');
		if (s) *(charset0 = s++) = '\0';
		arg = s;
	    } else if (!strncmp(arg, "view=", 5)) {
		xhw_view = 1;
		xhw_endx = strtol(arg+5, &arg, 0);
		xhw_endy = strtol(arg+1, &arg, 0);
		xhw_startx = strtol(arg+1, &arg, 0);
		xhw_starty = strtol(arg+1, &arg, 0);
		xhw_endx += xhw_startx;
		xhw_endy += xhw_starty;
	    } else if (!strncmp(arg, "drag", 4)) {
		arg += 4;
		drag = TRUE;
	    } else if (!strncmp(arg, "noinput", 7)) {
		arg += 7;
		noinput = TRUE;
	    } else
		arg = strchr(arg, ',');
	}
    }

    xsfont = NULL; xhints = NULL;
    xwindow = None; xgc = None;
    xReqCount = XReqCount = 0;
    HW->keyboard_slot = NOSLOT;
    
    if ((xdisplay = XOpenDisplay(xdisplay_))) do {
	
	(void)XSetIOErrorHandler(X11_Die);

	if (!X11_CheckRemapKeys())
	    break;

	xscreen = DefaultScreen(xdisplay);
	xdepth  = DefaultDepth(xdisplay, xscreen);
	
	for (i = 0; i <= MAXCOL; i++) {
	    xcolor.red   = 257 * (udat)Palette[i].Red;
	    xcolor.green = 257 * (udat)Palette[i].Green;
	    xcolor.blue  = 257 * (udat)Palette[i].Blue;
	    if (!XAllocColor(xdisplay, DefaultColormap(xdisplay, xscreen), &xcolor)) {
		printk("      X11_InitHW() failed to allocate colors\n");
		break;
	    }
	    xcol[i] = xcolor.pixel;
	}
	if (i <= MAXCOL)
	    break;
	
	xattr.background_pixel = xcol[0];
	xattr.event_mask = ExposureMask | VisibilityChangeMask |
	    StructureNotifyMask | SubstructureNotifyMask |
	    KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

	if (!((fontname && (xsfont = XLoadQueryFont(xdisplay, fontname))) ||
	     (xsfont = XLoadQueryFont(xdisplay, "vga")) ||
	     (xsfont = XLoadQueryFont(xdisplay, "fixed"))))
	    break;
	
	xwfont = xsfont->min_bounds.width;
	xwidth = xwfont * (HW->X = GetDisplayWidth());
	xhfont = (xupfont = xsfont->ascent) + xsfont->descent;
	xheight = xhfont * (HW->Y = GetDisplayHeight());
	
	if (xhw_view && xhw_startx >= 0 && xhw_starty >= 0 && xhw_endx > xhw_startx && xhw_endy > xhw_starty) {
	    /* a valid view was specified */
	    
	    xwidth  = xwfont * (xhw_endx - xhw_startx);
	    xheight = xhfont * (xhw_endy - xhw_starty);
	} else {
	    xhw_view = xhw_startx = xhw_starty = 0;
	    xhw_endx = HW->X;
	    xhw_endy = HW->Y;
	}
	
	if ((xwindow = XCreateWindow(xdisplay, DefaultRootWindow(xdisplay), 0, 0,
				     xwidth, xheight, 0, xdepth, InputOutput,
				     DefaultVisual(xdisplay, xscreen),
				     CWBackPixel | CWEventMask, &xattr)) &&

	    (xsgc.foreground = xsgc.background = xcol[0],
	     xsgc.graphics_exposures = False,
	     xsgc.font = xsfont->fid,
	     xgc = XCreateGC(xdisplay, xwindow, GCFont|GCForeground|GCBackground|GCGraphicsExposures, &xsgc)) &&

	    (xhints = XAllocSizeHints()))
        {
	    
            static XComposeStatus static_xcompose;
            xcompose = static_xcompose;

#ifdef HW_X11_HAVE_XIM_XIC
            xim = XOpenIM(xdisplay, NULL, NULL, NULL);
            if (xim != NULL) {
                xic = XCreateIC(xim, XNInputStyle, XIMStatusNothing|XIMPreeditNothing,
                                XNClientWindow, xwindow, XNFocusWindow, xwindow, NULL);
                if (xic == NULL) {
                    XCloseIM(xim);
                    xim = NULL;
                }
            } else
                xic = NULL;
#endif
	    sprintf(name, "twin %s on X", TWDisplay);
	    XStoreName(xdisplay, xwindow, name);


#ifdef CONF__UNICODE
	    if (!(xUTF_16_to_charset = X11_UTF_16_to_charset_function(charset)))
		xUTF_16_to_charset = X11_UTF_16_to_UTF_16;
#endif	    
	    /*
	     * ask ICCCM-compliant window manager to tell us when close window
	     * has been chosen, rather than just killing us
	     */
	    xWM_PROTOCOLS = XInternAtom(xdisplay, "WM_PROTOCOLS", False);
	    xWM_DELETE_WINDOW = XInternAtom(xdisplay, "WM_DELETE_WINDOW", False);
	    xTARGETS = XInternAtom(xdisplay, "TARGETS", False);

	    XChangeProperty(xdisplay, xwindow, xWM_PROTOCOLS, XA_ATOM, 32, PropModeReplace,
			    (unsigned char *) &xWM_DELETE_WINDOW, 1);

	    if (xhw_view) {
		xhints->flags = PMinSize|PMaxSize;
		xhints->min_width = xhints->max_width = xwidth;
		xhints->min_height = xhints->max_height = xheight;
	    } else {
		xhints->flags = PResizeInc;
		xhints->width_inc  = xwfont;
		xhints->height_inc = xhfont;
	    }
	    XSetWMNormalHints(xdisplay, xwindow, xhints);
	    
	    XMapWindow(xdisplay, xwindow);
	    
	    do {
		XNextEvent(xdisplay, &event);
	    } while (event.type != MapNotify);
	    
	    XFree(xhints); xhints = NULL;
	    
	    HW->mouse_slot = NOSLOT;
	    HW->keyboard_slot = RegisterRemote(i = XConnectionNumber(xdisplay), (obj)HW,
					       X11_KeyboardEvent);
	    if (HW->keyboard_slot == NOSLOT)
		break;
	    fcntl(i, F_SETFD, FD_CLOEXEC);
	    
	    HW->FlushVideo = X11_FlushVideo;
	    HW->FlushHW = X11_FlushHW;
	    
	    HW->KeyboardEvent = X11_KeyboardEvent;
	    HW->MouseEvent = (void *)NoOp; /* mouse events handled by X11_KeyboardEvent */
	    
	    HW->XY[0] = HW->XY[1] = 0;
	    HW->TT = NOCURSOR;
	    
	    HW->ShowMouse = NoOp;
	    HW->HideMouse = NoOp;
	    HW->UpdateMouseAndCursor = NoOp;
	    HW->MouseState.x = HW->MouseState.y = HW->MouseState.keys = 0;
	    
	    HW->DetectSize  = X11_DetectSize;
	    HW->CheckResize = X11_CheckResize;
	    HW->Resize      = X11_Resize;
	    
	    HW->HWSelectionImport  = X11_SelectionImport_X11;
	    HW->HWSelectionExport  = X11_SelectionExport_X11;
	    HW->HWSelectionRequest = X11_SelectionRequest_X11;
	    HW->HWSelectionNotify  = X11_SelectionNotify_X11;
	    HW->HWSelectionPrivate = 0;
	    
	    if (drag) {
		HW->CanDragArea = X11_CanDragArea;
		HW->DragArea    = X11_DragArea;
	    } else
		HW->CanDragArea = NULL;
	    
	    HW->Beep = X11_Beep;
	    HW->Configure = X11_Configure;
	    HW->SetPalette = (void *)NoOp;
	    HW->ResetPalette = NoOp;
	    
	    HW->QuitHW = X11_QuitHW;
	    HW->QuitKeyboard  = NoOp;
	    HW->QuitMouse = NoOp;
	    HW->QuitVideo = NoOp;
	    
	    HW->DisplayIsCTTY = FALSE;
	    HW->FlagsHW &= ~FlHWSoftMouse; /* mouse pointer handled by X11 server */
	    
	    HW->FlagsHW |= FlHWNeedOldVideo;
	    HW->FlagsHW |= FlHWExpensiveFlushVideo;
	    if (noinput)
		HW->FlagsHW |= FlHWNoInput;
	    
	    HW->NeedHW = 0;
	    HW->CanResize = TRUE;
	    HW->merge_Threshold = 0;
	    
	    /*
	     * we must draw everything on our new shiny window
	     * without forcing all other displays
	     * to redraw everything too.
	     */
	    HW->RedrawVideo = FALSE;
	    NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);
	    
	    if (xdisplay0) *xdisplay0 = ',';
	    if (fontname0) *fontname0 = ',';
	    if (charset0) *charset0 = ',';
	    
	    return TRUE;
	}
    } while (0); else {
	if (xdisplay_ || (xdisplay_ = getenv("DISPLAY")))
	    printk("      X11_InitHW() failed to open display %."STR(SMALLBUFF)"s\n", HW->Name);
	else
	    printk("      X11_InitHW() failed: DISPLAY is not set\n");
    }
	
    if (xdisplay0) *xdisplay0 = ',';
    if (fontname0) *fontname0 = ',';
    if (charset0) *charset0 = ',';
	
    if (xdisplay)
	X11_QuitHW();

    FreeMem(HW->Private);
    
    return FALSE;
}

#ifdef THIS_MODULE

byte InitModule(module Module) {
    Module->Private = X11_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module Module) {
}

#endif /* THIS_MODULE */
