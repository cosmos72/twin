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


/*
 * I always said X11 Selection is a nightmare... this is the worst bug I have ever seen:
 * 
 * Suppose twin is running on X, and you also have twdisplay running on the same X.
 * You select something inside a twterm. Twin will export the selection on all displays,
 * in this case hw_X11 and hw_display (which talks to twdisplay and its other hw_X11).
 * Both the hw_X11 will do a XSetSelectionOwner, causing a race: one wins,
 * the other receives a SelectionClear event from X.
 * 
 * Suppose twdisplay wins.
 * 
 * Ok, so you have twin holding some selected data, but twdisplay actually holding
 * X11 Selection ownership. Now try to click on an xterm to paste that data.... nightmare!
 * 
 * 
 * note the nesting... here is what happens:
 *   |      |
 *   v      v
 * twdisplay   receives a "Selection Request" from X (xterm), and knows twin Selection owner is twin -> hw_X11 (twdisplay)
 *             so it forwards the request to twin
 * 
 *  twin       receives a "Selection Request" from twdisplay, and knows twin Selection owner is twin -> hw_X11 (twdisplay)
 *             so it resets the Selection owner (displays hold selection only for a single request) and forwards
 *             the request to hw_X11 (twdisplay), so that twdisplay receives the request AS AN X11 SELECTION REQUEST!
 * 
 *   twdisplay receives a "Selection Request" from X (twin), and knows twin Selection owner is None
 *             so it forwards the request to twin.
 *             PROBLEM: twdisplay must remember the TWO NESTED X11 SELECTION REQUESTS!
 * 
 *    twin     receives a "Selection Request" from twdisplay, and knows twin Selection owner is None
 *             so it picks its internal Selection data and Notifies it back to twdisplay
 * 
 *   twdisplay receives a "Selection Notify" from twin and forwards it to whoever asked it...
 *             in this case to X (twin).
 * 
 *  twin       receives a "Selection Notify" from twdisplay, and knows Requestor is twdisplay
 *             so it forwards the notify to twdisplay.
 * 
 * twdisplay   receives a "Selection Notify" from twin and forwards it to whoever asked it...
 *             If it remembered the nesting, it will correctly remove the previous
 *             request from its list, and orward this notify to X (xterm), else will happily
 *             forward this notify to X (twin), causing an INFINITE LOOP!
 * 
 * 
 * solution:
 * 
 * Twdisplay must remember the two nested x11 selection requests.
 * Will only TWO nested requests suffice for any case?
 * Adding more twdisplay/twattach to the same X server
 * will not change anything, as only one will own X11 Selection, so I think so.
 * In any case, I add a counter and make twdisplay complain loud if it overflows.
 * 
 */

/* Display variables */

#define NEST 2

struct x11_data {
    unsigned int xwidth, xheight;
    int xwfont, xhfont, xupfont;

    Display     *xdisplay;
    Window       xwindow;
    GC           xgc;
    XGCValues    xsgc;
    XFontStruct *xsfont;
    byte         xwindow_AllVisible;
    obj         *xRequestor[NEST];
    uldat        xReqPrivate[NEST];
    uldat        xReqCount;
    uldat        XReqCount;
    XSelectionRequestEvent XReq[NEST];
    unsigned long xcol[MAXCOL+1];
};

#define xdata	((struct x11_data *)HW->Private)
#define xwidth	(xdata->xwidth)
#define xheight	(xdata->xheight)
#define xwfont	(xdata->xwfont)
#define xhfont	(xdata->xhfont)
#define xupfont	(xdata->xupfont)
#define xdisplay	(xdata->xdisplay)
#define xwindow	(xdata->xwindow)
#define xgc	(xdata->xgc)
#define xsgc	(xdata->xsgc)
#define xsfont	(xdata->xsfont)
#define xwindow_AllVisible	(xdata->xwindow_AllVisible)
#define xRequestor(j)	(xdata->xRequestor[j])
#define xReqPrivate(j)	(xdata->xReqPrivate[j])
#define xReqCount	(xdata->xReqCount)
#define XReqCount	(xdata->XReqCount)
#define XReq(j)	(xdata->XReq[j])
#define xcol	(xdata->xcol)

static void X11_SelectionNotify_up(Window win, Atom prop);
static void X11_SelectionRequest_up(XSelectionRequestEvent *req);


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
	HW->HWSelectionPrivate = NULL; /* selection now owned by some other X11 client */
	TwinSelectionSetOwner((obj *)HW, SEL_CURRENTTIME, SEL_CURRENTTIME);
	break;
      case SelectionRequest:
	X11_SelectionRequest_up(&event->xselectionrequest);
	break;
      case SelectionNotify:
	X11_SelectionNotify_up(event->xselection.requestor, event->xselection.property);
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

    HW->FlagsHW &= ~FlHWChangedMouseFlag;
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
	HW->HWSelectionPrivate = (void *)xwindow;
	setFlush();
    }
}

/*
 * notify our Selection to X11
 */
static void X11_SelectionNotify_X11(uldat ReqPrivate, uldat Magic, byte MIME[MAX_MIMELEN],
				    uldat Len, byte *Data) {
    XEvent ev;
    static Atom xa_targets = None;
    if (xa_targets == None)
	xa_targets = XInternAtom (xdisplay, "TARGETS", False);
	
    if (XReqCount == 0) {
	fprintf(stderr, "hw_X11.c: X11_SelectionNotify_X11(): unexpected Twin Selection Notify event!\n");
	fflush(stderr);
	return;
    }
#if 0
    else {
	fprintf(stderr, "hw_X11.c: X11_SelectionNotify_X11(): %d nested Twin Selection Notify events\n", XReqCount);
	fflush(stderr);
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
	
    if (XReq(XReqCount).target == xa_targets) {
	/*
	 * On some systems, the Atom typedef is 64 bits wide.
	 * We need to have a typedef that is exactly 32 bits wide,
	 * because a format of 64 is not allowed by the X11 protocol.
	 */
	typedef CARD32 Atom32;
	Atom32 target_list[2];
	
	target_list[0] = (Atom32) xa_targets;
	target_list[1] = (Atom32) XA_STRING;
	XChangeProperty (xdisplay, XReq(XReqCount).requestor, XReq(XReqCount).property,
			 xa_targets, 8*sizeof(target_list[0]), PropModeReplace,
			 (char *)target_list,
			 sizeof(target_list)/sizeof(target_list[0]));
	ev.xselection.property = XReq(XReqCount).property;
    } else if (XReq(XReqCount).target == XA_STRING) {
	XChangeProperty (xdisplay, XReq(XReqCount).requestor, XReq(XReqCount).property,
			 XA_STRING, 8, PropModeReplace,
			 Data, Len);
	ev.xselection.property = XReq(XReqCount).property;
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
	fprintf(stderr, "hw_X11.c: X11_SelectionNotify_up(): unexpected X Selection Notify event!\n");
	fflush(stderr);
	return;
    }
#if 0
    else {
	fprintf(stderr, "hw_X11.c: X11_SelectionNotify_up(): %d nested X Selection Notify event\n", xReqCount);
	fflush(stderr);
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
static void X11_SelectionRequest_X11(obj *Requestor, uldat ReqPrivate) {
    if (!HW->HWSelectionPrivate) {

	if (xReqCount == NEST) {
	    fprintf(stderr, "hw_X11.c: X11_SelectionRequest_X11(): too many nested Twin Selection Request events!\n");
	    fflush(stderr);
	    return;
	}
#if 0
	else {
	    fprintf(stderr, "hw_X11.c: X11_SelectionRequest_X11(): %d nested Twin Selection Request events\n", xReqCount+1);
	    fflush(stderr);
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
	fprintf(stderr, "hw_X11.c: X11_SelectionRequest_up(): too many nested X Selection Request events!\n");
	fflush(stderr);
	return;
    }
#if 0
    else {
	fprintf(stderr, "hw_X11.c: X11_SelectionRequest_up(): %d nested X Selection Request events\n", XReqCount+1);
	fflush(stderr);
    }
#endif
    CopyMem(req, &XReq(XReqCount), sizeof(XSelectionRequestEvent));
    XReqCount++;
    TwinSelectionRequest((obj *)HW, NOID, TwinSelectionGetOwner());
    /* we will get a HW->HWSelectionNotify (i.e. X11_SelectionNotify_X11) call */
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
    byte *opt = NULL, *drag = NULL, *noinput = NULL,
	*fontname = NULL, name[] = "twin :??? on X";
    
    if (arg && HW->NameLen > 4) {
	
	arg += 4; /* skip "-hw=" */
	
	if (*arg++ != 'X')
	    return FALSE; /* user said "use <arg> as display, not X" */
	
	if (*arg == '1' && arg[1] == '1')
	    arg += 2; /* `X11' is same as `X' */

	opt = strstr(arg, ",font=");
	drag = strstr(arg, ",drag");
	noinput = strstr(arg, ",noinput");
	
	if (opt)  *opt = '\0', fontname = opt + 6;
	if (drag) *drag = '\0';
	if (noinput) *noinput = '\0';
	    
	if (*arg == '@')
	    arg++; /* use specified X DISPLAY */
	else
	    arg = NULL;
    } else
	arg = NULL;
    
    if (!(HW->Private = (struct x11_data *)AllocMem(sizeof(struct x11_data)))) {
	fprintf(stderr, "      X11_InitHW(): Out of memory!\n");
	if (opt) *opt = ',';
	if (drag) *drag = ',';
	if (noinput) *noinput = ',';
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
	     xwidth = xwfont * (HW->X = GetDisplayWidth()),
	     xhfont = (xupfont = xsfont->ascent) + xsfont->descent,
	     xheight = xhfont * (HW->Y = GetDisplayHeight()),
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
	    HW->UpdateMouseAndCursor = NoOp;

	    HW->DetectSize  = X11_DetectSize;
	    HW->CheckResize = X11_CheckResize;
	    HW->Resize      = X11_Resize;
	    
	    HW->HWSelectionImport  = X11_SelectionImport_X11;
	    HW->HWSelectionExport  = X11_SelectionExport_X11;
	    HW->HWSelectionRequest = X11_SelectionRequest_X11;
	    HW->HWSelectionNotify  = X11_SelectionNotify_X11;
	    HW->HWSelectionPrivate = NULL;

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
	    NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);

	    if (opt) *opt = ',';
	    if (drag) *drag = ',';
	    if (noinput) *noinput = ',';

	    xReqCount = XReqCount = 0;
	    
	    return TRUE;
	}
    } while (0); else {
	if (arg || (arg = getenv("DISPLAY")))
	    fprintf(stderr, "      X11_InitHW() failed to open display %s\n", arg);
	else
	    fprintf(stderr, "      X11_InitHW() failed: DISPLAY is not set\n");
    }

    if (opt) *opt = ',';
    if (drag) *drag = ',';
    if (noinput) *noinput = ',';
	
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
