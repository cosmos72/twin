/*
 *  hw_X11.c  --  functions to let twin display on X11
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <fcntl.h>

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"

#include "libTwkeys.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>                /* CARD32 */

/* Display variables */

struct x11_data {
    unsigned int xwidth, xheight;
    int xwfont, xhfont, xupfont;

    Display *xdisplay;
    Window  xwindow;
    GC      xgc;
    XGCValues xsgc;
    XFontStruct *xsfont;
    byte    xwindow_AllVisible;
    unsigned long xcol[MAXCOL+1];
};

#define xdata ((struct x11_data *)HW->Private)
#define xwidth	(xdata->xwidth)
#define xheight	(xdata->xheight)
#define xwfont	(xdata->xwfont)
#define xhfont	(xdata->xhfont)
#define xupfont	(xdata->xupfont)
#define xdisplay (xdata->xdisplay)
#define xwindow	(xdata->xwindow)
#define xgc	(xdata->xgc)
#define xsgc	(xdata->xsgc)
#define xsfont	(xdata->xsfont)
#define xwindow_AllVisible (xdata->xwindow_AllVisible)
#define xcol	(xdata->xcol)


static void X11_doImportClipBoard(Window win, Atom prop, Bool Delete);
static void X11_ExportClipBoard(void);
static void X11_UnExportClipBoard(void);
static void X11_SendClipBoard(XSelectionRequestEvent *rq);


static void X11_Beep(void) {
    XBell(xdisplay, 0);
    setFlush();
}

static struct {
    KeySym xkey;
    udat tkey;
    byte len;
    byte *seq;
} X11_keys[] = {

#define IS(sym,l,s) { XK_##sym, TW_##sym, l, s },
#  include "hw_keys.h"
#undef IS

};

static uldat X11_keyn = sizeof(X11_keys) / sizeof(X11_keys[0]);

static byte X11_RemapKeys(void) {
    uldat i;
    
    for (i = 0; i < X11_keyn; i++) {
	XRebindKeysym(xdisplay, X11_keys[i].xkey, (KeySym *)0, 0, X11_keys[i].seq, X11_keys[i].len);
	if (i && X11_keys[i-1].xkey >= X11_keys[i].xkey) {
	    fputs("\n"
		  "      ERROR: twin compiled from a bad server/hw_keys.h file"
		  "             (data in file is not sorted). X11 support is unusable.\n"
		  "      X11_InitHW failed: internal error.\n",
		  stderr);
	    return FALSE;
	}
    }
    return TRUE;
}

static void X11_Configure(udat resource, byte todefault, udat value) {
    XKeyboardControl xctrl;
    
    switch (resource) {
      case HW_KBDAPPLIC:
	/* TODO */
	break;
      case HW_ALTCURSKEYS:
	/* TODO */
	break;
      case HW_BELLPITCH:
	xctrl.bell_pitch = todefault ? -1 : value;
	XChangeKeyboardControl(xdisplay, KBBellPitch, &xctrl);
	setFlush();
	break;
      case HW_BELLDURATION:
	xctrl.bell_duration = todefault ? -1 : value;
	XChangeKeyboardControl(xdisplay, KBBellDuration, &xctrl);
	setFlush();
	break;
      default:
	break;
    }
}

    
static udat X11_LookupKey(KeySym sym) {
    static udat lastTW = TW_Null;
    static KeySym lastXK = NoSymbol;
    
    uldat i, low, up;
    
    if (sym >= ' ' && sym <= '~')
	return (udat)sym;

    if (sym == lastXK)
	return lastTW;
    
    low = 0; up = X11_keyn;
    
    while (low < up) {
	i = (low + up) / 2;
	if (sym == X11_keys[i].xkey) {
	    lastXK = sym;
	    return lastTW = X11_keys[i].tkey;
	} else if (sym > X11_keys[i].xkey)
	    low = i;
	else
	    up = i;
    }
    lastXK = sym;
    return lastTW = TW_Null;
};


static void X11_HandleEvent(XEvent *event) {
    static byte seq[SMALLBUFF], *s, len;
    KeySym key = NoSymbol;
    dat x, y, dx, dy;
    udat TW_key;

    if (event->xany.window == xwindow) switch (event->type) {
      case KeyPress:
	if ((len = XLookupString(&event->xkey, s=seq, SMALLBUFF, &key, NULL))) {
	    TW_key = X11_LookupKey(key);
	    KeyboardEventCommon(TW_key, len, s);
	}
	break;
      case KeyRelease:
	break;
      case MotionNotify:
      case ButtonPress:
      case ButtonRelease:
	if (event->type == MotionNotify && !HW->MouseState.keys)
	    break;
	
	x = event->xbutton.x / xwfont;
	if (x < 0) x = 0;
	else if (x >= ScreenWidth) x = ScreenWidth - 1;
	
	y = event->xbutton.y / xhfont;
	if (y < 0) y = 0;
	else if (y >= ScreenHeight) y = ScreenHeight - 1;
	
	if (event->type == MotionNotify) {
	    dx = event->xbutton.x < xwfont/2 ? -1 : xwidth - event->xbutton.x <= xwfont/2 ? 1 : 0;
	    dy = event->xbutton.y < xhfont/2 ? -1 : xheight- event->xbutton.y <= xhfont/2 ? 1 : 0;
	    if (dx || dy || x != HW->MouseState.x || y != HW->MouseState.y)
		MouseEventCommon(x, y, dx, dy, HW->MouseState.keys);
	    break;
	}
	
	dx= event->xbutton.state;
	dy= (dx & Button1Mask ? HOLD_LEFT : 0) |
	    (dx & Button2Mask ? HOLD_MIDDLE : 0) |
	    (dx & Button3Mask ? HOLD_RIGHT : 0);
	dx = event->xbutton.button;
	
	if (event->type == ButtonPress)
	    dy|= (dx == Button1 ? HOLD_LEFT :
		  dx == Button2 ? HOLD_MIDDLE :
		  dx == Button3 ? HOLD_RIGHT : 0);
	else
	    dy &= ~(dx == Button1 ? HOLD_LEFT :
		    dx == Button2 ? HOLD_MIDDLE :
		    dx == Button3 ? HOLD_RIGHT : 0);
	
	MouseEventCommon(x, y, 0, 0, dy);
	
	break;
      case ConfigureNotify:
	if (HW->X != event->xconfigure.width  / xwfont ||
	    HW->Y != event->xconfigure.height / xhfont) {
	    
	    HW->X = (xwidth  = event->xconfigure.width ) / xwfont;
	    HW->Y = (xheight = event->xconfigure.height) / xhfont;
	    ResizeDisplayPrefer(HW);
	}
	break;
      case Expose:
	x = event->xexpose.x / xwfont;
	y = event->xexpose.y / xhfont;
	dx = (event->xexpose.x + event->xexpose.width + xwfont - 2) / xwfont;
	dy = (event->xexpose.y + event->xexpose.height + xhfont - 2) / xhfont;
	
	NeedRedrawVideo(x, y, dx, dy);
	/* must we redraw the cursor too ? */
	if (HW->XY[0] >= x && HW->XY[0] <= dx && HW->XY[1] >= y && HW->XY[1] <= dy)
	    HW->TT = NOCURSOR;
	break;
      case VisibilityNotify:
	xwindow_AllVisible = event->xvisibility.state == VisibilityUnobscured;
	break;
      case SelectionClear:
	X11_UnExportClipBoard();
	break;
      case SelectionRequest:
	X11_SendClipBoard(&event->xselectionrequest);
	break;
      case SelectionNotify:
	X11_doImportClipBoard(event->xselection.requestor, event->xselection.property, False);
	break;
      default:
	break;
    }
}
static void X11_KeyboardEvent(int fd, display_hw *D_HW) {
    XEvent event;
    SaveHW;
    SetHW(D_HW);
    
    while (XPending(xdisplay) > 0) {
	XNextEvent(xdisplay, &event);
	X11_HandleEvent(&event);
    }
    
    RestoreHW;
}

static hwcol _col;

#define XDRAW(col, buf, buflen) \
    if (xsgc.foreground != xcol[COLFG(col)]) \
	XSetForeground(xdisplay, xgc, xsgc.foreground = xcol[COLFG(col)]); \
    if (xsgc.background != xcol[COLBG(col)]) \
	XSetBackground(xdisplay, xgc, xsgc.background = xcol[COLBG(col)]); \
	XDrawImageString(xdisplay, xwindow, xgc, xbegin, ybegin + xupfont, buf, buflen)

INLINE void X11_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV;
    hwcol col;
    udat buflen = 0;
    byte buf[SMALLBUFF];
    int xbegin = x * xwfont, ybegin = y * xhfont;
    
    V = Video + x + y * ScreenWidth;
    oV = OldVideo + x + y * ScreenWidth;
    
    for (_col = ~HWCOL(*V); len; x++, V++, oV++, len--) {
	col = HWCOL(*V);
	if (buflen && (col != _col || (ValidOldVideo && *V == *oV) || buflen == SMALLBUFF)) {
	    XDRAW(_col, buf, buflen);
	    buflen = 0;
	}
	if (!ValidOldVideo || *V != *oV) {
	    if (!buflen) {
		xbegin = x * xwfont;
		_col = col;
	    }
	    buf[buflen++] = HWFONT(*V);
	}
    }
    if (buflen) {
	XDRAW(_col, buf, buflen);
	buflen = 0;
    }
}

static void X11_HideCursor(dat x, dat y) {
    hwattr V = Video[x + y * ScreenWidth];
    byte c = HWFONT(V);
    hwcol col = HWCOL(V);
    int xbegin = x * xwfont, ybegin = y * xhfont;
    
    XDRAW(col, &c, 1);
}

#undef XDRAW

static void X11_ShowCursor(uldat type, dat x, dat y) {
    hwattr V = Video[x + y * ScreenWidth];
    hwcol v;
    byte c;
    udat i;
    
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
	c = HWFONT(V);
	XDrawImageString(xdisplay, xwindow, xgc, x * xwfont, y * xhfont + xupfont, &c, 1);
    }
    if (type & 0xF) {
	/* VGA hw-like cursor */
	i = xhfont * ((type & 0xF)-NOCURSOR) / (SOLIDCURSOR-NOCURSOR);
	
	/* doesn't work in paletted visuals */
	if (xsgc.foreground != xcol[COLFG(HWCOL(V)) ^ COLBG(HWCOL(V))])
	    XSetForeground(xdisplay, xgc, xsgc.foreground = xcol[COLFG(HWCOL(V)) ^ COLBG(HWCOL(V))]);
	
	XSetFunction(xdisplay, xgc, xsgc.function = GXxor);
	XFillRectangle(xdisplay, xwindow, xgc,
		       x * xwfont, y * xhfont + xhfont - i, xwfont, i);
	XSetFunction(xdisplay, xgc, xsgc.function = GXcopy);
    }
}

static void X11_FlushVideo(void) {
    dat start, end;
    udat i;
    byte c = ChangedVideoFlag &&
	(ValidOldVideo
	 ? Video[HW->XY[0] + HW->XY[1] * ScreenWidth]
	 != OldVideo[HW->XY[0] + HW->XY[1] * ScreenWidth] 
	 : Plain_isDirtyVideo(HW->XY[0], HW->XY[1]));
    /* TRUE iff the cursor will be erased by burst */
    
    
    /* first burst all changes */
    if (ChangedVideoFlag) {
	for (i=0; i<ScreenHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    end   = ChangedVideo[i>>1][i&1][1];
	    
	    if (start != -1)
		X11_Mogrify(start, i>>1, end-start+1);
	}
	setFlush();
    }
    /* then, we may have to erase the old cursor */
    if (!c && HW->TT != NOCURSOR &&
	(CursorType != HW->TT || CursorX != HW->XY[0] || CursorY != HW->XY[1])) {
	
	HW->TT = NOCURSOR;
	X11_HideCursor(HW->XY[0], HW->XY[1]);
	setFlush();
    }
    /* finally, redraw the cursor if */
    /* (we want a cursor and (the burst erased the cursor or the cursor changed)) */
    if (CursorType != NOCURSOR &&
	(c || CursorType != HW->TT || CursorX != HW->XY[0] || CursorY != HW->XY[1])) {
	
	X11_ShowCursor(HW->TT = CursorType, HW->XY[0] = CursorX, HW->XY[1]= CursorY);
	setFlush();
    }

    HW->ChangedMouseFlag = FALSE;
}

static void X11_FlushHW(void) {
    XFlush(xdisplay);
    clrFlush();
}

static void X11_DetectSize(udat *x, udat *y) {
    *x = HW->X = xwidth  / xwfont;
    *y = HW->Y = xheight / xhfont;
}

static void X11_CheckResize(udat *x, udat *y) {
    /* always ok */
}

static void X11_Resize(udat x, udat y) {
    if (x != HW->X || y != HW->Y) {
	XResizeWindow(xdisplay, xwindow,
		      xwidth = xwfont * (HW->X = x),
		      xheight = xhfont * (HW->Y = y));
	setFlush();
    }
}
     
static void X11_ExportClipBoard(void) {
    if (!HW->PrivateClipBoard) {
	/* we don't own the selection buffer of X server, replace it */
	XSetSelectionOwner(xdisplay, XA_PRIMARY, xwindow, CurrentTime);
	HW->PrivateClipBoard = (void *)xwindow;
	setFlush();
    }
}

static void X11_UnExportClipBoard(void) {
    HW->PrivateClipBoard = (void *)0;
}

static void X11_SendClipBoard(XSelectionRequestEvent *rq) {
    XEvent ev;
    static Atom xa_targets = None;
    if (xa_targets == None)
	xa_targets = XInternAtom (xdisplay, "TARGETS", False);
	
    ev.xselection.type      = SelectionNotify;
    ev.xselection.property  = None;
    ev.xselection.display   = rq->display;
    ev.xselection.requestor = rq->requestor;
    ev.xselection.selection = rq->selection;
    ev.xselection.target    = rq->target;
    ev.xselection.time      = rq->time;
	
    if (rq->target == xa_targets) {
	/*
	 *          * On some systems, the Atom typedef is 64 bits wide.
	 *          * We need to have a typedef that is exactly 32 bits wide,
	 *          * because a format of 64 is not allowed by the X11 protocol.
	 *          */
	typedef CARD32 Atom32;
	Atom32 target_list[2];
	
	target_list[0] = (Atom32) xa_targets;
	target_list[1] = (Atom32) XA_STRING;
	XChangeProperty (xdisplay, rq->requestor, rq->property,
			 xa_targets, 8*sizeof(target_list[0]), PropModeReplace,
			 (char *)target_list,
			 sizeof(target_list)/sizeof(target_list[0]));
	ev.xselection.property = rq->property;
    } else if (rq->target == XA_STRING) {
	XChangeProperty (xdisplay, rq->requestor, rq->property,
			 XA_STRING, 8, PropModeReplace,
			 /* GetClipData() MUST ALWAYS precede GetClipLen() */
			 GetClipData(), GetClipLen());
	ev.xselection.property = rq->property;
    }
    XSendEvent (xdisplay, rq->requestor, False, 0, &ev);
}

static void X11_doImportClipBoard(Window win, Atom prop, Bool Delete) {
    long nread = 0;
    unsigned long nitems, bytes_after = BIGBUFF;
    byte *data;
    Atom actual_type;
    int actual_fmt;
    
    if (prop == None)
	return;
    
    SetClipBoard(CLIP_TEXTMAGIC, 0, NULL);
    
    do {	
	if ((XGetWindowProperty(xdisplay, win, prop,
				nread/4, bytes_after/4, Delete,
				AnyPropertyType, &actual_type, &actual_fmt,
				&nitems, &bytes_after, &data)
	     != Success) || (actual_type != XA_STRING)) {
	    
	    XFree (data);
	    return;
	}
	nread += nitems;
	
	AddToClipBoard(nitems, data);
	XFree (data);
	
    } while (bytes_after > 0);
}

static void X11_ImportClipBoard(byte Wait) {
    if (!HW->PrivateClipBoard) {
	/* we don't own the selection buffer of X server, ask for it */
	if (XGetSelectionOwner(xdisplay, XA_PRIMARY) == None)
	    X11_doImportClipBoard(DefaultRootWindow(xdisplay), XA_CUT_BUFFER0, False);
	else {
	    Atom prop = XInternAtom (xdisplay, "VT_SELECTION", False);
	    XConvertSelection (xdisplay, XA_PRIMARY, XA_STRING, prop, xwindow, CurrentTime);
	    if (Wait) {
		XEvent event;
		do {
		    XNextEvent(xdisplay, &event);
		    X11_HandleEvent(&event);
		} while (event.type != SelectionNotify);
	    }
	}
    }
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
	    /* and remember redrawing it */
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


static void X11_QuitHW(void) {
    XFreeFont(xdisplay, xsfont);
    XFreeGC(xdisplay, xgc);
    XUnmapWindow(xdisplay, xwindow);
    XDestroyWindow(xdisplay, xwindow);
    XCloseDisplay(xdisplay);
    xdisplay = NULL;
    
    UnRegisterRemote(HW->keyboard_slot);
    HW->keyboard_slot = NOSLOT;
    HW->KeyboardEvent = (void *)NoOp;
    
    HW->QuitHW = NoOp;

    FreeMem(HW->Private);
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

byte X11_InitHW(void) {
    byte *arg = HW->Name;
    int xscreen;
    unsigned int xdepth;
    XSetWindowAttributes xattr;
    XColor xcolor;
    XSizeHints *xhints;
    XEvent event;
    int i;
    byte *opt = NULL, *fontname = NULL, name[] = "twin :??? on X";
    
    if (arg && HW->NameLen > 4) {
	
	arg += 4; /* skip "-hw=" */
	
	if (*arg++ != 'X')
	    return FALSE; /* user said "use <arg> as display, not X" */
	
	if (*arg == '1' && arg[1] == '1')
	    arg += 2; /* `X11' is same as `X' */

	if ((opt = strchr(arg, ','))) {
	    if (!strncmp(opt + 1, "font=", 5))
		fontname = opt + 6;
	    *opt = '\0';
	}
	
	if (*arg == '@')
	    arg++; /* use specified X DISPLAY */
	else if (*arg) {
	    if (opt)
		*opt = ',';
	    return FALSE;
	} else
	    arg = NULL;
    } else
	arg = NULL;
    
    if (!(HW->Private = (struct x11_data *)AllocMem(sizeof(struct x11_data)))) {
	fprintf(stderr, "      X11_InitHW(): Out of memory!\n");
	return FALSE;
    }
    
    if ((xdisplay = XOpenDisplay(arg))) do {
	
	(void)XSetIOErrorHandler(X11_Die);
	
	if (!X11_RemapKeys())
	    break;

	xscreen = DefaultScreen(xdisplay);
	xdepth  = DefaultDepth(xdisplay, xscreen);
	
	for (i = 0; i <= MAXCOL; i++) {
	    xcolor.red   = 257 * (udat)Palette[i].Red;
	    xcolor.green = 257 * (udat)Palette[i].Green;
	    xcolor.blue  = 257 * (udat)Palette[i].Blue;
	    if (!XAllocColor(xdisplay, DefaultColormap(xdisplay, xscreen), &xcolor))
		break;
	    xcol[i] = xcolor.pixel;
	}
	if (i <= MAXCOL)
	    break;
	
	xattr.background_pixel = xcol[0];
	xattr.event_mask = ExposureMask | VisibilityChangeMask |
	    StructureNotifyMask | SubstructureNotifyMask |
	    KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
	
	if (((fontname && (xsfont = XLoadQueryFont(xdisplay, fontname))) ||
	     (xsfont = XLoadQueryFont(xdisplay, "vga")) ||
	     (xsfont = XLoadQueryFont(xdisplay, "fixed"))) &&
	    (xwfont = xsfont->min_bounds.width,
	     xwidth = xwfont * (HW->X = ScreenWidth),
	     xhfont = (xupfont = xsfont->ascent) + xsfont->descent,
	     xheight = xhfont * (HW->Y = ScreenHeight),
	     xwindow = XCreateWindow(xdisplay, RootWindow(xdisplay, xscreen), 0, 0,
				     xwidth, xheight, 0, xdepth, InputOutput,
				     DefaultVisual(xdisplay, xscreen),
				     CWBackPixel | CWEventMask, &xattr)) &&
	    (xgc = XCreateGC(xdisplay, xwindow, 0, NULL)) &&
	    (xhints = XAllocSizeHints())) {
	    
	    xsgc.foreground = xsgc.background = xcol[0];
	    xsgc.graphics_exposures = False;
	    XChangeGC(xdisplay, xgc, GCForeground|GCBackground|GCGraphicsExposures, &xsgc);
	    XSetFont(xdisplay, xgc, xsfont->fid);
	    
	    strcpy(name+5, TWDisplay);
	    strcat(name+5, " on X");
	    XStoreName(xdisplay, xwindow, name);
	    
	    xhints->flags = PResizeInc;
	    xhints->width_inc  = xwfont;
	    xhints->height_inc = xhfont;
	    XSetWMNormalHints(xdisplay, xwindow, xhints);

	    XMapWindow(xdisplay, xwindow);

	    do {
		XNextEvent(xdisplay, &event);
	    } while (event.type != MapNotify);

	    HW->mouse_slot = NOSLOT;
	    HW->keyboard_slot = RegisterRemote(i = XConnectionNumber(xdisplay), HW,
					       (void (*)(int fd, void *))X11_KeyboardEvent);
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

	    HW->DetectSize  = X11_DetectSize;
	    HW->CheckResize = X11_CheckResize;
	    HW->Resize      = X11_Resize;
	    
	    HW->ExportClipBoard = X11_ExportClipBoard;
	    HW->ImportClipBoard = X11_ImportClipBoard;
	    HW->PrivateClipBoard = NULL;

	    if (arg && strstr(arg, ",drag")) {
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
	    HW->SoftMouse = FALSE; /* mouse pointer handled by X11 server */
	    
	    HW->NeedOldVideo = TRUE;
	    HW->ExpensiveFlushVideo = TRUE;
	    HW->NeedHW = 0;
	    HW->CanResize = TRUE;
	    HW->merge_Threshold = 0;
	    
	    /*
	     * we must draw everything on our new shiny window
	     * without forcing all other displays
	     * to redraw everything too.
	     */
	    NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);

	    if (opt) *opt = ',';

	    return TRUE;
	}
    } while (0); else {
	if (arg || (arg = getenv("DISPLAY")))
	    fprintf(stderr, "      X11_InitHW() failed to open display %s\n", arg);
	else
	    fprintf(stderr, "      X11_InitHW() failed: DISPLAY is not set\n");
    }

    if (opt) *opt = ',';
	
    if (xdisplay)
	X11_QuitHW();

    FreeMem(HW->Private);
    
    return FALSE;
}

#ifdef MODULE

byte InitModule(module *Module) {
    Module->Private = X11_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module *Module) {
}

#endif /* MODULE */
