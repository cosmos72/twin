/*
 *  hw_X11.h  --  functions to let twin display on X11
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"
#include "util.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"

#include "libTwkeys.h"

static display_hw X11;

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>                /* CARD32 */

/* Display variables */

static unsigned int xwidth, xheight;
static int xwfont, xhfont, xupfont;

static Display *xdisplay;
static Window  xwindow;
static GC      xgc;
static XGCValues xsgc;
static int     xscreen;
static XFontStruct *xsfont;
static byte    xwindow_AllVisible = TRUE;

#define L 0x55
#define M 0xaa
#define H 0xFF

static unsigned long colpixel[MAXCOL+1];
static byte xpalette[MAXCOL+1][3] = {
    /* the default colour table, for VGA+ colour systems */
    {0,0,0}, {0,0,M}, {0,M,0}, {0,M,M}, {M,0,0}, {M,0,M}, {M,M,0}, {M,M,M},
    {L,L,L}, {L,L,H}, {L,H,L}, {L,H,H}, {H,L,L}, {H,L,H}, {H,H,L}, {H,H,H}};

#undef H
#undef M
#undef L

#define XY ((udat  *)All->gotoxybuf)
#define TT ((uldat *)All->cursorbuf)


static void X11_doImportClipBoard(Window win, Atom prop, Bool Delete);
static void X11_ExportClipBoard(void);
static void X11_UnExportClipBoard(void);
static void X11_SendClipBoard(XSelectionRequestEvent *rq);


static void X11_MoveToXY(udat x, udat y) {
    XY[2] = x;
    XY[3] = y;
}

static void X11_SetCursorType(uldat CursorType) {
    if ((CursorType & 0xF) == 0)
	CursorType |= LINECURSOR;
    else if ((CursorType & 0xF) > SOLIDCURSOR)
	CursorType = (CursorType & ~(uldat)0xF) | SOLIDCURSOR;
    TT[1] = CursorType;
}

static void X11_Beep(void) {
    XBell(xdisplay, 0);
    All->NeedHW |= NEEDFlushHW;
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
		  "      compiled from a non-sorted server/hw_keys.h file.\n"
		  "      someone probably messed up that file,\n"
		  "      in any case X11 support is unusable.\n"
		  "      X11_InitHW failed: internal error, server/hw_keys.h is not sorted.\n",
		  errFILE);
	    return FALSE;
	}
    }
    return TRUE;
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
	if (event->type == MotionNotify && !All->MouseState->keys)
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
	    if (dx || dy || x != All->MouseState->x || y != All->MouseState->y)
		MouseEventCommon(x, y, dx, dy, All->MouseState->keys);
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
	if (xwidth != event->xconfigure.width || xheight != event->xconfigure.height) {
	    xwidth  = event->xconfigure.width;
	    xheight = event->xconfigure.height;
	    All->NeedHW |= NEEDResizeDisplay;
	}
	break;
      case Expose:
	x = event->xexpose.x / xwfont;
	y = event->xexpose.y / xhfont;
	dx = (event->xexpose.x + event->xexpose.width - 1) / xwfont;
	dy = (event->xexpose.y + event->xexpose.height - 1) / xhfont;
	FillOldVideo(x, y, dx, dy, HWATTR(COL(WHITE,BLACK), ' '));
	DirtyVideo(x, y, dx, dy);
	/* must we redraw the cursor too ? */
	if (XY[0] >= x && XY[0] <= dx && XY[1] >= y && XY[1] <= dy)
	    TT[0] = (uldat)-1;
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
static void X11_KeyboardEvent(int fd, uldat slot) {
    XEvent event;
    
    while (XPending(xdisplay) > 0) {
	XNextEvent(xdisplay, &event);
	X11_HandleEvent(&event);
    }
}

static hwcol _col;

#define XDRAW(col, buf, buflen) \
    if (xsgc.foreground != colpixel[COLFG(col)]) \
	XSetForeground(xdisplay, xgc, xsgc.foreground = colpixel[COLFG(col)]); \
    if (xsgc.background != colpixel[COLBG(col)]) \
	XSetBackground(xdisplay, xgc, xsgc.background = colpixel[COLBG(col)]); \
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
	    *oV = *V;
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

static void X11_ShowCursor(dat x, dat y, uldat type) {
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
	if (xsgc.foreground != colpixel[COLFG(v)])
	    XSetForeground(xdisplay, xgc, xsgc.foreground = colpixel[COLFG(v)]);
	if (xsgc.background != colpixel[COLBG(v)])
	    XSetBackground(xdisplay, xgc, xsgc.background = colpixel[COLBG(v)]);
	c = HWFONT(V);
	XDrawImageString(xdisplay, xwindow, xgc, x * xwfont, y * xhfont + xupfont, &c, 1);
    }
    if (type & 0xF) {
	/* VGA hw-like cursor */
	i = xhfont * ((type & 0xF)-NOCURSOR) / (SOLIDCURSOR-NOCURSOR);
	
	/* doesn't work in paletted visuals */
	if (xsgc.foreground != colpixel[COLFG(HWCOL(V)) ^ COLBG(HWCOL(V))])
	    XSetForeground(xdisplay, xgc, xsgc.foreground = colpixel[COLFG(HWCOL(V)) ^ COLBG(HWCOL(V))]);
	
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
	 ? Video[XY[0] + XY[1] * ScreenWidth] != OldVideo[XY[0] + XY[1] * ScreenWidth] 
	 : Plain_isDirtyVideo(XY[0], XY[1]));
    /* TRUE iff the cursor will be erased by burst */
    
    
    /* first burst all changes */
    if (ChangedVideoFlag) {
	for (i=0; i<ScreenHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    end   = ChangedVideo[i>>1][i&1][1];
	    ChangedVideo[i>>1][i&1][0] = -1;
	    
	    if (start != -1)
		X11_Mogrify(start, i>>1, end-start+1);
	}
	All->NeedHW |= NEEDFlushHW;
    }
    /* then, we may have to erase the old cursor */
    if (!c && TT[0] != NOCURSOR && (TT[0] != TT[1] || XY[0] != XY[2] || XY[1] != XY[3])) {
	X11_HideCursor(XY[0], XY[1]);
	All->NeedHW |= NEEDFlushHW;
    }
    /* finally, redraw the cursor if */
    /* (we want a cursor and (the burst erased the cursor or the cursor changed)) */
    if (TT[1] != NOCURSOR && (c || TT[0] != TT[1] || XY[0] != XY[2] || XY[1] != XY[3])) {
	X11_ShowCursor(XY[2], XY[3], TT[1]);
	All->NeedHW |= NEEDFlushHW;
    }
    
    ChangedVideoFlag = ChangedMouseFlag = FALSE;
    ValidOldVideo = TRUE;
    
    TT[0] = TT[1];
    XY[0] = XY[2];
    XY[1] = XY[3];
}

static void X11_FlushHW(void) {
    XFlush(xdisplay);
    All->NeedHW &= ~NEEDFlushHW;
}

static void X11_DetectSize(udat *x, udat *y) {
    *x = xwidth  / xwfont;
    *y = xheight / xhfont;
}

static void X11_ExportClipBoard(void) {
    if (!X11.PrivateClipBoard) {
	/* we don't own the selection buffer of X server, replace it */
	XSetSelectionOwner(xdisplay, XA_PRIMARY, xwindow, CurrentTime);
	X11.PrivateClipBoard = (void *)xwindow;
    }
    All->NeedHW &= ~NEEDExportClipBoard;
}

static void X11_UnExportClipBoard(void) {
    X11.PrivateClipBoard = (void *)0;
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
			 All->ClipData, All->ClipLen);
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
    if (!X11.PrivateClipBoard) {
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


static byte X11_canDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return xwindow_AllVisible && (Rgt-Left+1) * (Dwn-Up+1) > 20;
}

static void X11_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    dat DstRgt = (Rgt-Left)+DstLeft;
    dat DstDwn = (Dwn-Up)+DstUp;
    
    X11_FlushVideo(); /* a *must* before any direct X11 operation */
    
    if (TT[1] != NOCURSOR) {
	if (XY[2] >= Left && XY[2] <= Rgt && XY[3] >= Up && XY[3] <= Dwn) {
	    /* must hide the cursor before dragging */
	    X11_HideCursor(XY[2], XY[3]);
	    /* and remember redrawing it */
	    TT[0] = (uldat)-1;
	} else if (XY[2] >= DstLeft && XY[2] <= DstRgt && XY[3] >= DstUp && XY[3] <= DstDwn) {
	    /* cursor will be overwritten by drag, remember to redraw it */
	    TT[0] = (uldat)-1;
	}
    }
    XCopyArea(xdisplay, xwindow, xwindow, xgc,
	      Left*xwfont, Up*xhfont, (Rgt-Left+1)*xwfont, (Dwn-Up+1)*xhfont,
	      DstLeft*xwfont, DstUp*xhfont);
    All->NeedHW |= NEEDFlushHW;
}


static void X11_QuitHW(void) {
    XFreeFont(xdisplay, xsfont);
    XFreeGC(xdisplay, xgc);
    XUnmapWindow(xdisplay, xwindow);
    XDestroyWindow(xdisplay, xwindow);
    XCloseDisplay(xdisplay);
    xdisplay = NULL;
    
    UnRegisterRemote(All->keyboard_slot);
    All->keyboard_slot = NOSLOT;
    
    X11.KeyboardEvent = (void *)NoOp;
    
    X11.QuitHW = NoOp;
}

display_hw *X11_InitHW(byte *arg) {
    unsigned int xdepth;
    XSetWindowAttributes xattr;
    XColor xcolor;
    XSizeHints *xhints;
    XEvent event;
    int i;
    byte name[] = "twin :??? on X";
    
    if (arg) {
	if (*arg != 'X')
	    return NULL; /* user said "use <arg> as display, not X" */
	else if (*++arg == '@')
	    ++arg; /* use specified X DISPLAY */
	else if (*arg == '1' && *++arg == '1' && *++arg == '@')
	    ++arg; /* use specified X11 DISPLAY */
	else
	    arg = NULL; /* use default X DISPLAY */
    }
    
    if ((xdisplay = XOpenDisplay(arg))) do {
	
	if (!X11_RemapKeys())
	    break;

	xscreen = DefaultScreen(xdisplay);
	xdepth  = DefaultDepth(xdisplay, xscreen);
	
	for (i = 0; i <= MAXCOL; i++) {
	    xcolor.red   = 257 * xpalette[i][0];
	    xcolor.green = 257 * xpalette[i][1];
	    xcolor.blue  = 257 * xpalette[i][2];
	    if (!XAllocColor(xdisplay, DefaultColormap(xdisplay, xscreen), &xcolor))
		break;
	    colpixel[i] = xcolor.pixel;
	}
	if (i <= MAXCOL)
	    break;
	
	xattr.background_pixel = colpixel[0];
	xattr.event_mask = ExposureMask | VisibilityChangeMask |
	    StructureNotifyMask | SubstructureNotifyMask |
	    KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
	
	if (((xsfont = XLoadQueryFont(xdisplay, "9x19")) ||
	     (xsfont = XLoadQueryFont(xdisplay, "vga")) ||
	     (xsfont = XLoadQueryFont(xdisplay, "fixed"))) &&
	    (xwfont = xsfont->min_bounds.width, xwidth = xwfont * ScreenWidth,
	     xhfont = (xupfont = xsfont->ascent) + xsfont->descent, xheight = xhfont * ScreenHeight,
	     xwindow = XCreateWindow(xdisplay, RootWindow(xdisplay, xscreen), 0, 0,
				     xwidth, xheight, 0, xdepth, InputOutput,
				     DefaultVisual(xdisplay, xscreen),
				     CWBackPixel | CWEventMask, &xattr)) &&
	    (xgc = XCreateGC(xdisplay, xwindow, 0, NULL)) &&
	    (xhints = XAllocSizeHints())) {
	    
	    xsgc.foreground = xsgc.background = colpixel[0];
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


	    All->keyboard_slot = RegisterRemote(XConnectionNumber(xdisplay), X11_KeyboardEvent);
	    if (All->keyboard_slot == NOSLOT)
		break;	    
	    X11.KeyboardEvent = X11_KeyboardEvent;
	    X11.QuitKeyboard  = NoOp;
	    

	    X11.SoftMouse = FALSE; /* mouse pointer handled by X11 server */
	    X11.MouseEvent = (void *)NoOp; /* mouse events handled by X11_KeyboardEvent */
	    X11.ShowMouse = NoOp;
	    X11.HideMouse = NoOp;
	    X11.QuitMouse = NoOp;
	    
	    All->mouse_slot = NOSLOT;
		
	    X11.canDragArea = X11_canDragArea;
	    X11.DragArea    = X11_DragArea;

	    X11.DetectSize  = X11_DetectSize;
	    X11.SetCharset  = (void *)NoOp;
	    
	    X11.ExportClipBoard = X11_ExportClipBoard;
	    X11.ImportClipBoard = X11_ImportClipBoard;
	    X11.PrivateClipBoard = (void *)0;

	    X11.FlushVideo = X11_FlushVideo;
	    X11.QuitVideo = NoOp;
	    X11.FlushHW = X11_FlushHW;

	    X11.NeedOldVideo = TRUE;
	    X11.merge_Threshold = 0;
	    X11.ExpensiveFlushVideo = TRUE;
	    
	    X11.MoveToXY = X11_MoveToXY;	       XY[0] = XY[1] = XY[2] = XY[3] = 0;
	    X11.SetCursorType = X11_SetCursorType; TT[0] = TT[1] = NOCURSOR;
	    X11.Beep = X11_Beep;
	    
	    X11.QuitHW = X11_QuitHW;
	    
	    InitTtysave();
	    
	    return &X11;
	}
    } while (0); else {
	if (arg || (arg = getenv("DISPLAY")))
	    fprintf(errFILE, "      X11_InitHW() failed to open display %s\n", arg);
	else
	    fprintf(errFILE, "      X11_InitHW() failed: DISPLAY is not set\n");
    }
    
    if (xdisplay)
	X11_QuitHW();
    
    return NULL;
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
