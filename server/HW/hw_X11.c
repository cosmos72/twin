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

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"

#include "Tw/Twkeys.h"

#ifdef CONF__UNICODE
# include "Tutf/Tutf.h"
# include "Tutf/Tutf_defs.h"
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>                /* CARD32 */

/*
 * a user reported his system lacks DefaultRootWindow() ...
 */
#ifndef DefaultRootWindow
# define DefaultRootWindow(xdpy) XDefaultRootWindow(xdpy)
#endif
#ifndef DefaultScreen
# define DefaultScreen(xdpy) XDefaultScreen(xdpy)
#endif
#ifndef DefaultDepth
# define DefaultDepth(xdpy,xscreen) XDefaultDepth(xdpy,xscreen)
#endif

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
 *             request from its list, and forward this notify to X (xterm), else will happily
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

#define NEST 4



struct x11_data {
    unsigned int xwidth, xheight;
    int xwfont, xhfont, xupfont;
#ifdef CONF__UNICODE
    Tutf_function xUTF_16_to_charset;
#endif    
    Display     *xdisplay;
    Window       xwindow;
    GC           xgc;
    XGCValues    xsgc;
    XFontStruct *xsfont;
    byte         xwindow_AllVisible, xfont_map;
    obj          xRequestor[NEST];
    uldat        xReqPrivate[NEST];
    uldat        xReqCount;
    uldat        XReqCount;
    XSelectionRequestEvent XReq[NEST];
    unsigned long xcol[MAXCOL+1];
    Atom xWM_PROTOCOLS, xWM_DELETE_WINDOW;
};

#define xdata	((struct x11_data *)HW->Private)
#define xwidth	(xdata->xwidth)
#define xheight	(xdata->xheight)
#define xwfont	(xdata->xwfont)
#define xhfont	(xdata->xhfont)
#define xupfont	(xdata->xupfont)
#define xUTF_16_to_charset	(xdata->xUTF_16_to_charset)
#define xdisplay	(xdata->xdisplay)
#define xwindow	(xdata->xwindow)
#define xgc	(xdata->xgc)
#define xsgc	(xdata->xsgc)
#define xsfont	(xdata->xsfont)
#define xwindow_AllVisible	(xdata->xwindow_AllVisible)
#define xfont_map	(xdata->xfont_map)
#define xRequestor(j)	(xdata->xRequestor[j])
#define xReqPrivate(j)	(xdata->xReqPrivate[j])
#define xReqCount	(xdata->xReqCount)
#define XReqCount	(xdata->XReqCount)
#define XReq(j)	(xdata->XReq[j])
#define xcol	(xdata->xcol)
#define xWM_PROTOCOLS (xdata->xWM_PROTOCOLS)
#define xWM_DELETE_WINDOW (xdata->xWM_DELETE_WINDOW)

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
	    printk("\n"
		   "      ERROR: twin compiled from a bad server/hw_keys.h file"
		   "             (data in file is not sorted). X11 support is unusable.\n"
		   "      X11_InitHW() failed: internal error.\n");
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
      case HW_MOUSEMOTIONEVENTS:
	/* nothing to do */
	break;
      default:
	break;
    }
}

    
/* convert an X11 KeySym into a libTw key code and ASCII sequence */

static udat X11_LookupKey(XKeyEvent *ev, udat *ShiftFlags, udat *len, char *seq) {
    static udat lastTW = TW_Null;
    static uldat lastI = MAXULDAT;
    static KeySym sym, lastXK = NoSymbol;
    
    uldat i, low, up, _len = *len;

    *ShiftFlags = 0;
    if (ev->state & ShiftMask)
	*ShiftFlags |= KBD_SHIFT_FL;
    if (ev->state & LockMask)
	*ShiftFlags |= KBD_CAPS_LOCK;
    if (ev->state & ControlMask)
	*ShiftFlags |= KBD_CTRL_FL;
    if (ev->state & (Mod1Mask|Mod3Mask)) /* Alt|AltGr */
	*ShiftFlags |= KBD_ALT_FL;
    if (ev->state & Mod2Mask) /* Num_Lock */
	*ShiftFlags |= KBD_NUM_LOCK;
    
    *len = XLookupString(ev, seq, _len, &sym, NULL);
    
    if (sym == XK_BackSpace && ev->state & (ControlMask|Mod1Mask)) {
	if (ev->state & ControlMask)
	    *len = 1, *seq = '\x1F';
	else
	    *len = 2, seq[0] = '\x1B', seq[1] = '\x7F';
	return TW_BackSpace;
    }

    if (sym >= ' ' && sym <= '~') {
	/* turn ALT+A into ESC+A etc. */
	if (ev->state & Mod1Mask && *len == 1 && (byte)sym == *seq) {
	    *len = 2;
	    seq[1] = seq[0];
	    seq[0] = '\x1B';
	}
	return (udat)sym;
    }
    
    if (sym != lastXK) {
	low = 0;		/* the first possible */
	up = X11_keyn;	/* 1 + the last possible */
    
	while (low < up) {
	    i = (low + up) / 2;
	    if (sym == X11_keys[i].xkey) {
		lastTW = X11_keys[lastI = i].tkey;
		break;
	    } else if (sym > X11_keys[i].xkey)
		low = i + 1;
	    else
		up = i;
	}
	if (low == up) {
	    lastI = X11_keyn;
	    lastTW = TW_Null;
	}
	lastXK = sym;
    }
    if (*len == 0 && lastI < X11_keyn && X11_keys[lastI].len) {
	/* XLookupString() returned empty string, steal sequence from hw_keys.h */
	if (_len > X11_keys[lastI].len)
	    CopyMem(X11_keys[lastI].seq, seq, *len = X11_keys[lastI].len);
    }
    return lastTW;
};


static void X11_HandleEvent(XEvent *event) {
    static byte seq[SMALLBUFF];
    dat x, y, dx, dy;
    udat len = SMALLBUFF, TW_key, ShiftFlags;

    if (event->xany.window == xwindow) switch (event->type) {
      case KeyPress:
	TW_key = X11_LookupKey(&event->xkey, &ShiftFlags, &len, seq);
	if (TW_key != TW_Null)
	    KeyboardEventCommon(TW_key, ShiftFlags, len, seq);
	break;
      case KeyRelease:
	break;
      case MotionNotify:
      case ButtonPress:
      case ButtonRelease:
	x = event->xbutton.x / xwfont;
	if (x < 0) x = 0;
	else if (x >= DisplayWidth) x = DisplayWidth - 1;
	
	y = event->xbutton.y / xhfont;
	if (y < 0) y = 0;
	else if (y >= DisplayHeight) y = DisplayHeight - 1;
	
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
	    
	    HW->X = (xwidth  = event->xconfigure.width) / xwfont;
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
	TwinSelectionSetOwner((obj)HW, SEL_CURRENTTIME, SEL_CURRENTTIME);
	break;
      case SelectionRequest:
	X11_SelectionRequest_up(&event->xselectionrequest);
	break;
      case SelectionNotify:
	X11_SelectionNotify_up(event->xselection.requestor, event->xselection.property);
	break;
      case ClientMessage:
	if (event->xclient.message_type == xWM_PROTOCOLS &&
	    event->xclient.format == 32 &&
	    event->xclient.data.l[0] == xWM_DELETE_WINDOW) {

	    /* going to close this display */
	    HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	    
	}
	break;
      default:
	break;
    }
}

static void X11_KeyboardEvent(int fd, display_hw D_HW) {
    XEvent event;
    SaveHW;
    SetHW(D_HW);
    
    while (XPending(xdisplay) > 0) {
	XNextEvent(xdisplay, &event);
	X11_HandleEvent(&event);
    }
    
    RestoreHW;
}

/* this can stay static, X11_FlushHW() is not reentrant */
static hwcol _col;



#ifdef CONF__UNICODE
# define XDRAW(col, buf, buflen) \
    if (xsgc.foreground != xcol[COLFG(col)]) \
	XSetForeground(xdisplay, xgc, xsgc.foreground = xcol[COLFG(col)]); \
    if (xsgc.background != xcol[COLBG(col)]) \
	XSetBackground(xdisplay, xgc, xsgc.background = xcol[COLBG(col)]); \
	XDrawImageString16(xdisplay, xwindow, xgc, xbegin, ybegin + xupfont, buf, buflen)
#else
# define XDRAW(col, buf, buflen) \
    if (xsgc.foreground != xcol[COLFG(col)]) \
	XSetForeground(xdisplay, xgc, xsgc.foreground = xcol[COLFG(col)]); \
    if (xsgc.background != xcol[COLBG(col)]) \
	XSetBackground(xdisplay, xgc, xsgc.background = xcol[COLBG(col)]); \
	XDrawImageString(xdisplay, xwindow, xgc, xbegin, ybegin + xupfont, buf, buflen)
#endif

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
    int xbegin = x * xwfont, ybegin = y * xhfont;
    
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

static void X11_HideCursor(dat x, dat y) {
    int xbegin = x * xwfont, ybegin = y * xhfont;
    hwattr V = Video[x + y * DisplayWidth];
    hwcol col = HWCOL(V);
#ifdef CONF__UNICODE
    XChar2b c;
    hwfont f = xUTF_16_to_charset(HWFONT(V));
    c.byte1 = f >> 8;
    c.byte2 = f & 0xFF;
#else
    byte c = HWFONT(V);
#endif
    
    XDRAW(col, &c, 1);
}

#undef XDRAW

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
	XDrawImageString16(xdisplay, xwindow, xgc, x * xwfont, y * xhfont + xupfont, &c, 1);
#else
	c = HWFONT(V);
	XDrawImageString(xdisplay, xwindow, xgc, x * xwfont, y * xhfont + xupfont, &c, 1);
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
		       x * xwfont, y * xhfont + xhfont - i, xwfont, i);
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
    *x = HW->X = xwidth  / xwfont;
    *y = HW->Y = xheight / xhfont;
}

static void X11_CheckResize(dat *x, dat *y) {
    /* always ok */
}

static void X11_Resize(dat x, dat y) {
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
static void X11_SelectionNotify_X11(uldat ReqPrivate, uldat Magic, CONST byte MIME[MAX_MIMELEN],
				    uldat Len, byte CONST * Data) {
    XEvent ev;
    static Atom xa_targets = None;
    if (xa_targets == None)
	xa_targets = XInternAtom (xdisplay, "TARGETS", False);
	
    if (XReqCount == 0) {
	printk("hw_X11.c: X11_SelectionNotify_X11(): unexpected Twin Selection Notify event!\n");
	return;
    }
#if 0
    else {
	printk("hw_X11.c: X11_SelectionNotify_X11(): %d nested Twin Selection Notify events\n", XReqCount);
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
	printk("hw_X11.c: X11_SelectionNotify_up(): unexpected X Selection Notify event!\n");
	return;
    }
#if 0
    else {
	printk("hw_X11.c: X11_SelectionNotify_up(): %d nested X Selection Notify event\n", xReqCount);
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
	    printk("hw_X11.c: X11_SelectionRequest_X11(): too many nested Twin Selection Request events!\n");
	    return;
	}
#if 0
	else {
	    printk("hw_X11.c: X11_SelectionRequest_X11(): %d nested Twin Selection Request events\n", xReqCount+1);
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
	printk("hw_X11.c: X11_SelectionRequest_up(): too many nested X Selection Request events!\n");
	return;
    }
#if 0
    else {
	printk("hw_X11.c: X11_SelectionRequest_up(): %d nested X Selection Request events\n", XReqCount+1);
    }
#endif
    CopyMem(req, &XReq(XReqCount), sizeof(XSelectionRequestEvent));
    TwinSelectionRequest((obj)HW, XReqCount++, TwinSelectionGetOwner());
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


static void X11_QuitHW(void) {

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
	    charset = T_NAME_IBM437;
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
		printk("    X11_InitHW: font `%s\' has no known charset encoding,\n"
		       "                assuming Unicode.\n", fontname);
		return NULL;
	    }
	    /* else assume codepage437. gross. */
	    printk("    X11_InitHW: font `%s\' has no known charset encoding,\n"
		   "                assuming IBM437 codepage (\"VGA\").\n", fontname);
	    return Tutf_UTF_16_to_IBM437;
	}
    }
    
    i = Tutf_charset_id(charset);
    s = Tutf_charset_name(i);
    if (s && !strcmp(s, T_NAME_UTF_16)) {
	/* this is an Unicode font. good. */
	return NULL;
    }
    
    if (i == (uldat)-1) {
	printk("      X11_InitHW(): libTutf warning: unknown charset `%." STR(SMALLBUFF) "s', assuming `IBM437'\n", charset);
	return Tutf_UTF_16_to_IBM437;
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

    
#ifdef CONF_THIS_MODULE
static
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
    byte *s, *dpy = NULL, *dpy0 = NULL,
    *fontname = NULL, *fontname0 = NULL,
    *charset = NULL, *charset0 = NULL,
    name[] = "twin :??? on X";
    byte drag = FALSE, noinput = FALSE;
    
    if (!(HW->Private = (struct x11_data *)AllocMem(sizeof(struct x11_data)))) {
	printk("      X11_InitHW(): Out of memory!\n");
	return FALSE;
    }
    
    if (arg && HW->NameLen > 4) {
	arg += 4; /* skip "-hw=" */
	
	if (*arg++ != 'X')
	    return FALSE; /* user said "use <arg> as display, not X" */
	
	if (*arg == '1' && arg[1] == '1')
	    arg += 2; /* `X11' is same as `X' */

	if (*arg == '@') {
	    if ((s = strchr(dpy = ++arg, ','))) {
		*(dpy0 = s) = '\0';
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
	    } else if (!strncmp(arg, "drag", 4)) {
		arg += 4;
		drag = TRUE;
	    } else if (!strncmp(arg, "noinput", 7)) {
		arg += 7;
		noinput = TRUE;
	    }
	}
    }

    xsfont = NULL; xhints = NULL;
    xwindow = None; xgc = None;
    xReqCount = XReqCount = 0;
    HW->keyboard_slot = NOSLOT;
    
    if ((xdisplay = XOpenDisplay(dpy))) do {
	
	(void)XSetIOErrorHandler(X11_Die);
	
	if (!X11_RemapKeys())
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
	    
	    XChangeProperty(xdisplay, xwindow, xWM_PROTOCOLS, XA_ATOM, 32, PropModeReplace,
			    (unsigned char *) &xWM_DELETE_WINDOW, 1);
	    
	    xhints->flags = PResizeInc;
	    xhints->width_inc  = xwfont;
	    xhints->height_inc = xhfont;
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
	    HW->RedrawVideo = FALSE;
	    NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);
	    
	    if (dpy0) *dpy0 = ',';
	    if (fontname0) *fontname0 = ',';
	    if (charset0) *charset0 = ',';
	    
	    return TRUE;
	}
    } while (0); else {
	if (dpy || (dpy = getenv("DISPLAY")))
	    printk("      X11_InitHW() failed to open display %s\n", HW->Name);
	else
	    printk("      X11_InitHW() failed: DISPLAY is not set\n");
    }
	
    if (dpy0) *dpy0 = ',';
    if (fontname0) *fontname0 = ',';
    if (charset0) *charset0 = ',';
	
    if (xdisplay)
	X11_QuitHW();

    FreeMem(HW->Private);
    
    return FALSE;
}

#ifdef CONF_THIS_MODULE

#include "version.h"
MODULEVERSION;
		       
byte InitModule(module Module) {
    Module->Private = X11_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module Module) {
}

#endif /* CONF_THIS_MODULE */
