/*
 * keyboard.h
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
#define DefaultRootWindow(xdpy) XDefaultRootWindow(xdpy)
#endif
#ifndef DefaultScreen
#define DefaultScreen(xdpy) XDefaultScreen(xdpy)
#endif
#ifndef DefaultDepth
#define DefaultDepth(xdpy, xscreen) XDefaultDepth(xdpy, xscreen)
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
 * twdisplay   receives a "Selection Request" from X (xterm), and knows twin Selection owner is twin
 * -> hw_X11 (twdisplay) so it forwards the request to twin
 *
 *  twin       receives a "Selection Request" from twdisplay, and knows twin Selection owner is twin
 * -> hw_X11 (twdisplay) so it resets the Selection owner (displays hold selection only for a single
 * request) and forwards the request to hw_X11 (twdisplay), so that twdisplay receives the request
 * AS AN X11 SELECTION REQUEST!
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

#if defined(__cplusplus) && __cplusplus >= 201100
#include <unordered_map>
#define CXX_STD_MAP std::unordered_map
#else
#include <map>
#define CXX_STD_MAP std::map
#endif

/* Display variables */

static void XSYM(SelectionNotify_up)(Window win, Atom prop);
static void XSYM(SelectionRequest_up)(XSelectionRequestEvent *req);

static void XSYM(Beep)(void) {
  XBell(xdisplay, 0);
  setFlush();
}

static void XSYM(Configure)(udat resource, byte todefault, udat value) {
  XKeyboardControl xctrl;

  switch (resource) {
  case HW_KBDAPPLIC:
    xnumkeypad = todefault || !value;
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

/*
 * calling XRebindKeysym() on non-english keyboards disrupts keys that use Mode_switch (AltGr),
 * so we implement a manual translation using a map from X11 KeySym to Twkey + char sequence.
 */

struct XSYM(key_to_TW) {
  Twkey tkey;
  char seq[6];

  // reuse termination byte seq[5] as length to save space
  byte len() const {
    return seq[5];
  }
};

struct XSYM(key_to_TW_entry) {
  uldat xkey; // actually KeySym
  XSYM(key_to_TW) tmapping;
};

static const XSYM(key_to_TW_entry) XSYM(keys_impl)[] = {
#define IS(sym, l, s) {XK_##sym, {TW_##sym, s}},
#include "hw_keys.h"
#undef IS
};

typedef CXX_STD_MAP<KeySym, XSYM(key_to_TW)> XSYM(keymap_to_TW);

static XSYM(keymap_to_TW) XSYM(keys);

static void XSYM(init_keys)(void) {
  for (size_t i = 0; i < N_OF(XSYM(keys_impl)); i++) {
    KeySym xkey = XSYM(keys_impl)[i].xkey;
    XSYM(key_to_TW) &entry = XSYM(keys)[xkey];
    entry = XSYM(keys_impl)[i].tmapping;
    entry.seq[5] = strlen(entry.seq); // reuse termination byte seq[5] as length
  }
}

#ifdef DEBUG_HW_X11
void X_DEBUG_SHOW_KEY(const char *prefix, KeySym sym, udat len, const char *seq) {
  udat i;
  byte ch;
  printf(XSYM_STR(LookupKey) "(): %s xkeysym=%d[%s] string=[", prefix, (int)sym,
         XKeysymToString(sym));
  for (i = 0; i < len; i++) {
    ch = (byte)seq[i];
    if (ch >= ' ' && ch <= '~' && ch != '\\')
      putchar((int)ch);
    else
      printf("\\x%02x", (unsigned)ch);
  }
  printf("] locale=%s\n", XLocaleOfIM(xim));
}
#else
#define X_DEBUG_SHOW_KEY(prefix, sym, len, seq) ((void)0)
#endif

static byte XSYM(IsNumericKeypad)(Twkey key) {
  return key >= TW_KP_Begin && key <= TW_KP_9;
}

/* convert an X11 KeySym into a libtw key code and ASCII sequence */

static Twkey XSYM(LookupKey)(XEvent *ev, udat *ShiftFlags, udat *len, char *seq) {
  KeySym sym = XK_VoidSymbol;
  XKeyEvent *kev = &ev->xkey;

  uldat maxlen = *len;

  *ShiftFlags = ((kev->state & ShiftMask) ? KBD_SHIFT_FL : 0) |
                ((kev->state & LockMask) ? KBD_CAPS_LOCK : 0) |
                ((kev->state & ControlMask) ? KBD_CTRL_FL : 0) |
                ((kev->state & (Mod1Mask | Mod3Mask)) ? KBD_ALT_FL : 0) | /* Alt|AltGr */
                ((kev->state & Mod2Mask) ? KBD_NUM_LOCK : 0);             /* Num_Lock */

#ifdef TW_FEATURE_X11_XIM_XIC
  if (xic) {
    Status status_return;
#ifdef TW_FEATURE_X11_Xutf8LookupString
    *len = Xutf8LookupString(xic, kev, seq, maxlen, &sym, &status_return);
#else
    *len = XmbLookupString(xic, kev, seq, maxlen, &sym, &status_return);
#endif
    if (XFilterEvent(ev, None)) {
      return TW_Null;
    }
    if (status_return != XLookupBoth && status_return != XLookupChars &&
        status_return != XLookupKeySym) {
      sym = XK_VoidSymbol;
    }
  }
#endif
  if (sym == XK_VoidSymbol || sym == 0) {
    *len = XLookupString(kev, seq, maxlen, &sym, &xcompose);
  }
  X_DEBUG_SHOW_KEY("", sym, *len, seq);

  if (sym == XK_BackSpace && (kev->state & (ControlMask | Mod1Mask)) != 0) {
    /* generate escape sequences for Backspace and Ctrl+Backspace */
    if (kev->state & ControlMask) {
      *len = 1, *seq = '\x1F';
    } else {
      *len = 2, seq[0] = '\x1B', seq[1] = '\x7F';
    }
    return TW_BackSpace;
  }

  if ((sym >= ' ' && sym <= '~') || (sym >= 0xA0 && sym <= 0xFF)) {
    /* turn ALT+A into ESC+A etc. */
    if ((kev->state & Mod1Mask) != 0 && *len == 1 && (byte)sym == *seq) {
      *len = 2;
      seq[1] = seq[0];
      seq[0] = '\x1B';

      X_DEBUG_SHOW_KEY("replaced(1)", sym, *len, seq);
    }
    return (Twkey)sym;
  }

  const XSYM(key_to_TW) *tkey_entry = NULL;
  {
    XSYM(keymap_to_TW)::const_iterator iter = XSYM(keys).find(sym);
    if (iter != XSYM(keys).end()) {
      tkey_entry = &iter->second;
    }
  }
  Twkey tkey = tkey_entry ? tkey_entry->tkey : TW_Null;

  if (tkey != TW_Null && *len == 1 && (*ShiftFlags & ~KBD_CAPS_LOCK) == KBD_NUM_LOCK &&
      xnumkeypad && XSYM(IsNumericKeypad)(tkey)) {
    /* xnumkeypad = ttrue and only NumLock led is set (ignoring CapsLock):
     * honor X11 numeric keypad mapping to numbers 0..9 and to / * - + . ENTER */

  } else if (tkey_entry != NULL && tkey_entry->len() &&
             (*len == 0 || (*ShiftFlags & ~(KBD_CAPS_LOCK | KBD_NUM_LOCK)) == 0)) {
    /* XLookupString() returned empty string, or no ShiftFlags (ignoring CapsLock/NumLock): use the
     * sequence stated in hw_keys.h */
    if (tkey_entry->len() < maxlen) {
      CopyMem(tkey_entry->seq, seq, *len = tkey_entry->len());

      X_DEBUG_SHOW_KEY("replaced(2)", sym, *len, seq);
    }
  }
  return tkey == TW_Null && *len != 0 ? TW_Other : tkey;
}

static void XSYM(HandleEvent)(XEvent *event) {
  /* this can stay static, XSYM(HandleEvent)() is not reentrant */
  static char seq[TW_SMALLBUFF];
  dat x, y, dx, dy;
  udat len = sizeof(seq), ShiftFlags;
  Twkey TW_key;

  if (event->xany.window == xwindow)
    switch (event->type) {
    case KeyPress:
      TW_key = XSYM(LookupKey)(event, &ShiftFlags, &len, seq);
      if (TW_key != TW_Null) {
        KeyboardEventCommon(TW_key, ShiftFlags, len, seq);
      }
      break;
    case KeyRelease:
      break;
    case MotionNotify:
    case ButtonPress:
    case ButtonRelease:
      x = event->xbutton.x / xwfont + xhw_startx;
      if (x < 0)
        x = 0;
      else if (x >= DisplayWidth)
        x = DisplayWidth - 1;

      y = event->xbutton.y / xhfont + xhw_starty;
      if (y < 0)
        y = 0;
      else if (y >= DisplayHeight)
        y = DisplayHeight - 1;

      if (event->type == MotionNotify) {
        dx = event->xbutton.x < xwfont / 2 ? -1 : xwidth - event->xbutton.x <= xwfont / 2 ? 1 : 0;
        dy = event->xbutton.y < xhfont / 2 ? -1 : xheight - event->xbutton.y <= xhfont / 2 ? 1 : 0;
        if (dx || dy || x != HW->MouseState.x || y != HW->MouseState.y)
          MouseEventCommon(x, y, dx, dy, HW->MouseState.keys);
        break;
      }

      dx = event->xbutton.state;
      dy = (dx & Button1Mask ? HOLD_LEFT : 0) | (dx & Button2Mask ? HOLD_MIDDLE : 0) |
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
      dx = (dx == Button1   ? HOLD_LEFT
            : dx == Button2 ? HOLD_MIDDLE
            : dx == Button3 ? HOLD_RIGHT
            :
#ifdef HOLD_WHEEL_REV
            dx == Button4 ? HOLD_WHEEL_REV
            :
#endif
#ifdef HOLD_WHEEL_FWD
            dx == Button5 ? HOLD_WHEEL_FWD
                          :
#endif
                          0);
      if (event->type == ButtonPress) {
        dy |= dx;
      } else {
        dy &= ~dx;
      }
      MouseEventCommon(x, y, 0, 0, dy);

      break;
    case ConfigureNotify:
      if (!xhw_view) {
        if (HW->X != event->xconfigure.width / xwfont ||
            HW->Y != event->xconfigure.height / xhfont) {

          HW->X = (xwidth = event->xconfigure.width) / xwfont;
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
      if (HW->XY[0] >= x && HW->XY[0] <= dx && HW->XY[1] >= y && HW->XY[1] <= dy) {
        HW->TT = NOCURSOR;
      }
      break;
    case VisibilityNotify:
      xwindow_AllVisible = event->xvisibility.state == VisibilityUnobscured;
      break;
    case SelectionClear:
      HW->HWSelectionPrivate = 0; /* selection now owned by some other X11 client */
      TwinSelectionSetOwner((Tobj)HW, SEL_CURRENTTIME, SEL_CURRENTTIME);
      break;
    case SelectionRequest:
      XSYM(SelectionRequest_up)(&event->xselectionrequest);
      break;
    case SelectionNotify:
      XSYM(SelectionNotify_up)(event->xselection.requestor, event->xselection.property);
      break;
    case ClientMessage:
      if (event->xclient.message_type == xWM_PROTOCOLS && event->xclient.format == 32 &&
          (Atom)event->xclient.data.l[0] == xWM_DELETE_WINDOW) {

        /* going to close this display */
        HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
      }
      break;
    default:
      break;
    }
}

static void XSYM(KeyboardEvent)(int fd, Tdisplay D_HW) {
  XEvent event;
  SaveHW;
  SetHW(D_HW);

  while (XPending(xdisplay) > 0) {
    XNextEvent(xdisplay, &event);
    XSYM(HandleEvent)(&event);
  }

  RestoreHW;
}
