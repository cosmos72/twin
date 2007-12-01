/*
 * hw_X11_gfx_common.h  --  common functions between hw_X11 and hw_gfx
 *
 *  Copyright (C) 2007 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

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


static void X11_SelectionNotify_up(Window win, Atom prop);
static void X11_SelectionRequest_up(XSelectionRequestEvent *req);


static void X11_Beep(void) {
    XBell(xdisplay, 0);
    setFlush();
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


static struct {
    KeySym xkey;
    udat tkey;
    byte len;
    CONST byte *seq;
} X11_keys[] = {

#define IS(sym,l,s) { XK_##sym, TW_##sym, l, s },
#  include "hw_keys.h"
#undef IS

};

static uldat X11_keyn = sizeof(X11_keys) / sizeof(X11_keys[0]);

/*
 * calling XRebindKeysym() on non-english keyboards disrupts keys that use Mode_switch (AltGr),
 * so we implement a manual translation using binary search on the table above.
 * But first, check the table is sorted, or binary search would become bugged.
 */
static byte X11_CheckRemapKeys(void) {
    uldat i;

    for (i = 0; i < X11_keyn; i++) {
	if (i && X11_keys[i-1].xkey >= X11_keys[i].xkey) {
	    printk("\n"
		   "      ERROR: twin compiled from a bad server/hw_keys.h file"
		   "             (data in file is not sorted). -hw=" THIS " driver is unusable.\n"
		   "      " THIS "_InitHW() failed: internal error.\n");
	    return FALSE;
	}
    }
    return TRUE;
}

/* convert an X11 KeySym into a libTw key code and ASCII sequence */

static udat X11_LookupKey(XEvent *ev, udat *ShiftFlags, udat *len, char *seq) {
    static udat lastTW = TW_Null;
    static uldat lastI = MAXULDAT;
    static KeySym lastXK = NoSymbol;
    KeySym sym = XK_VoidSymbol;
    XKeyEvent * kev = &ev->xkey;
    
    uldat i, low, up, _len = *len;

    *ShiftFlags = 0;
    if (kev->state & ShiftMask)
	*ShiftFlags |= KBD_SHIFT_FL;
    if (kev->state & LockMask)
	*ShiftFlags |= KBD_CAPS_LOCK;
    if (kev->state & ControlMask)
	*ShiftFlags |= KBD_CTRL_FL;
    if (kev->state & (Mod1Mask|Mod3Mask)) /* Alt|AltGr */
	*ShiftFlags |= KBD_ALT_FL;
    if (kev->state & Mod2Mask) /* Num_Lock */
	*ShiftFlags |= KBD_NUM_LOCK;

#ifdef HW_X11_HAVE_XIM_XIC
    if (xic) {
        Status status_return;
        *len = XmbLookupString(xic, kev, seq, _len, &sym, &status_return);
        if (XFilterEvent(ev, None))
            return TW_Null;
        
        if (status_return != XLookupBoth && status_return != XLookupChars && status_return != XLookupKeySym)
            sym = XK_VoidSymbol;
    }
#endif
    if (sym == XK_VoidSymbol || sym == 0)
        *len = XLookupString(kev, seq, _len, &sym, &xcompose);
    
#ifdef DEBUG_HW_X11
    printf(THIS ": keysym=%d (%s) string='%.*s'\n", (int)sym, XKeysymToString(sym), (int)*len, seq);
#endif
    if (sym == XK_BackSpace && kev->state & (ControlMask|Mod1Mask)) {
	if (kev->state & ControlMask)
	    *len = 1, *seq = '\x1F';
	else
	    *len = 2, seq[0] = '\x1B', seq[1] = '\x7F';
	return TW_BackSpace;
    }

    if ((sym >= ' ' && sym <= '~') || (sym >= 0xA0 && sym <= 0xFF)) {
	/* turn ALT+A into ESC+A etc. */
	if (kev->state & Mod1Mask && *len == 1 && (byte)sym == *seq) {
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
    /* XLookupString() returned empty string, or no ShiftFlags: steal sequence from hw_keys.h */
    if (lastI < X11_keyn && X11_keys[lastI].len && (*len == 0 || *ShiftFlags == 0)) {
	if (_len > X11_keys[lastI].len)
	    CopyMem(X11_keys[lastI].seq, seq, *len = X11_keys[lastI].len);
    }
    return lastTW;
}


static void X11_HandleEvent(XEvent *event) {
    /* this can stay static, X11_HandleEvent() is not reentrant */
    static byte seq[SMALLBUFF];
    dat x, y, dx, dy;
    udat len = sizeof(seq), TW_key, ShiftFlags;

    if (event->xany.window == xwindow) switch (event->type) {
      case KeyPress:
	TW_key = X11_LookupKey(event, &ShiftFlags, &len, seq);
	if (TW_key != TW_Null)
	    KeyboardEventCommon(TW_key, ShiftFlags, len, seq);
	break;
      case KeyRelease:
	break;
      case MotionNotify:
      case ButtonPress:
      case ButtonRelease:
	x = event->xbutton.x / xwfont + xhw_startx;
	if (x < 0) x = 0;
	else if (x >= DisplayWidth) x = DisplayWidth - 1;
	
	y = event->xbutton.y / xhfont + xhw_starty;
	if (y < 0) y = 0;
	else if (y >= DisplayHeight) y = DisplayHeight - 1;
	
	if (event->type == MotionNotify) {
	    dx = event->xbutton.x < xwfont/2 ? -1 : xwidth - event->xbutton.x <= xwfont/2 ? 1 : 0;
	    dy = event->xbutton.y < xhfont/2 ? -1 : xheight- event->xbutton.y <= xhfont/2 ? 1 : 0;
	    if (dx || dy || x != HW->MouseState.x || y != HW->MouseState.y)
		MouseEventCommon(x, y, dx, dy, HW->MouseState.keys);
	    break;
	}
	
	dx = event->xbutton.state;
	dy =(dx & Button1Mask ? HOLD_LEFT : 0) |
	    (dx & Button2Mask ? HOLD_MIDDLE : 0) |
	    (dx & Button3Mask ? HOLD_RIGHT : 0) |
#ifdef HOLD_WHEEL_REV
	    (dx & Button4Mask ? HOLD_WHEEL_REV : 0) |
#endif
#ifdef HOLD_WHEEL_FWD
	    (dx & Button5Mask ? HOLD_WHEEL_FWD : 0) |
#endif
	    0;
	
	
	/*
	 * when sending ButtonRelease event, X11 reports that button
	 * as still held down in event->xbutton.state ButtonMask...
	 * 
	 * instead when sending ButtonPress event, X11 reports that
	 * button as still NOT held down in event->xbutton.state ButtonMask...
	 * 
	 * fix both cases here.
	 */
	dx = event->xbutton.button;
	dx = (dx == Button1 ? HOLD_LEFT :
	      dx == Button2 ? HOLD_MIDDLE :
	      dx == Button3 ? HOLD_RIGHT :
#ifdef HOLD_WHEEL_REV
	      dx == Button4 ? HOLD_WHEEL_REV :
#endif
#ifdef HOLD_WHEEL_FWD
	      dx == Button5 ? HOLD_WHEEL_FWD :
#endif
	      0);
	if (event->type == ButtonPress)
	    dy |= dx;
	else
	    dy &= ~dx;
	    
	MouseEventCommon(x, y, 0, 0, dy);
	
	break;
      case ConfigureNotify:
	if (!xhw_view) {
	    if (HW->X != event->xconfigure.width  / xwfont ||
		HW->Y != event->xconfigure.height / xhfont) {
	    
		HW->X = (xwidth  = event->xconfigure.width) / xwfont;
		HW->Y = (xheight = event->xconfigure.height) / xhfont;
		ResizeDisplayPrefer(HW);
	    }
	}
	break;
      case Expose:
	x = event->xexpose.x / xwfont + xhw_startx;
	y = event->xexpose.y / xhfont + xhw_starty;
	dx = (event->xexpose.x + xhw_startx * xwfont + event->xexpose.width + xwfont - 2) / xwfont;
	dy = (event->xexpose.y + xhw_starty * xhfont + event->xexpose.height + xhfont - 2) / xhfont;
	
	NeedRedrawVideo(x, y, dx, dy);
	/* must we redraw the cursor too ? */
	if (HW->XY[0] >= x && HW->XY[0] <= dx && HW->XY[1] >= y && HW->XY[1] <= dy)
	    HW->TT = NOCURSOR;
	break;
      case VisibilityNotify:
	xwindow_AllVisible = event->xvisibility.state == VisibilityUnobscured;
	break;
      case SelectionClear:
	HW->HWSelectionPrivate = 0; /* selection now owned by some other X11 client */
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

