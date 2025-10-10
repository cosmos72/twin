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
 * in this case hw_x11 and hw_display (which talks to twdisplay and its other hw_x11).
 * Both the hw_x11 will do a XSetSelectionOwner, causing a race: one wins,
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
 * -> hw_x11 (twdisplay) so it forwards the request to twin
 *
 *  twin       receives a "Selection Request" from twdisplay, and knows twin Selection owner is twin
 * -> hw_x11 (twdisplay) so it resets the Selection owner (displays hold selection only for a single
 * request) and forwards the request to hw_x11 (twdisplay), so that twdisplay receives the request
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

TW_ATTR_HIDDEN void XDRIVER::Beep(Tdisplay hw) {
  XBell(xdriver(hw)->xdisplay, 0);
  hw->setFlush();
}

TW_ATTR_HIDDEN void XDRIVER::Configure(Tdisplay hw, udat resource, byte todefault, udat value) {
  XDRIVER *self = xdriver(hw);
  XKeyboardControl xctrl;

  switch (resource) {
  case HW_KBDAPPLIC:
    self->xnumkeypad = todefault || !value;
    break;
  case HW_ALTCURSKEYS:
    self->xaltcursorkeys = !todefault && value;
    break;
  case HW_BELLPITCH:
    xctrl.bell_pitch = todefault ? -1 : value;
    XChangeKeyboardControl(self->xdisplay, KBBellPitch, &xctrl);
    hw->setFlush();
    break;
  case HW_BELLDURATION:
    xctrl.bell_duration = todefault ? -1 : value;
    XChangeKeyboardControl(self->xdisplay, KBBellDuration, &xctrl);
    hw->setFlush();
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

static const XDRIVER::key_to_tw_entry XSYM(keys_impl)[] = {
#define IS(sym, l, s) {XK_##sym, {TW_##sym, s}},
#include "hw_keys.h"
#undef IS
};

typedef CXX_STD_MAP<KeySym, XDRIVER::key_to_tw> XSYM(keymap_to_tw);

static XSYM(keymap_to_tw) XSYM(keys);

TW_ATTR_HIDDEN void XDRIVER::InitKeys() {
  for (size_t i = 0; i < N_OF(XSYM(keys_impl)); i++) {
    KeySym xkey = XSYM(keys_impl)[i].xkey;
    key_to_tw &entry = XSYM(keys)[xkey];
    entry = XSYM(keys_impl)[i].tmapping;
    entry.seq[5] = strlen(entry.seq); // reuse termination byte seq[5] as length
  }
}

#undef DEBUG_HW_X11
#ifdef DEBUG_HW_X11
static void X_DEBUG_SHOW_KEY(XIM xim, const char *prefix, KeySym sym, udat len, const char *seq) {
  udat i;
  byte ch;
  printf(XSTR(XDRIVER) ".LookupKey(): %s xkeysym=%d[%s] string=[", prefix, (int)sym,
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
#define X_DEBUG_SHOW_KEY(xim, prefix, sym, len, seq) ((void)0)
#endif

TW_ATTR_HIDDEN bool XDRIVER::IsNumericKeypad(Twkey key) {
  return key >= TW_KP_Begin && key <= TW_KP_9;
}

/* convert an X11 KeySym into a libtw key code and ASCII sequence */

TW_ATTR_HIDDEN Twkey XDRIVER::LookupKey(XEvent *ev, udat *ShiftFlags, udat *len, char *seq) {
  XKeyEvent *kev = &ev->xkey;
  KeySym sym = XK_VoidSymbol;

  uldat maxlen = *len;

  *ShiftFlags = ((kev->state & ShiftMask) ? KBD_SHIFT_FL : 0) |
                ((kev->state & LockMask) ? KBD_CAPS_LOCK : 0) |
                ((kev->state & ControlMask) ? KBD_CTRL_FL : 0) |
                ((kev->state & (Mod1Mask | Mod3Mask)) ? KBD_ALT_FL : 0) | /* Alt|AltGr */
                ((kev->state & Mod2Mask) ? KBD_NUM_LOCK : 0);             /* Num_Lock */

#ifdef TW_FEATURE_X11_XIM_XIC
  if (this->xic) {
    Status status_return;
#ifdef TW_FEATURE_X11_Xutf8LookupString
    *len = Xutf8LookupString(this->xic, kev, seq, maxlen, &sym, &status_return);
#else
    *len = XmbLookupString(this->xic, kev, seq, maxlen, &sym, &status_return);
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
    *len = XLookupString(kev, seq, maxlen, &sym, &this->xcompose);
  }
  X_DEBUG_SHOW_KEY(xim, "", sym, *len, seq);

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

      X_DEBUG_SHOW_KEY(xim, "replaced(1)", sym, *len, seq);
    }
    return (Twkey)sym;
  }

  const key_to_tw *tkey_entry = NULL;
  {
    XSYM(keymap_to_tw)::const_iterator iter = XSYM(keys).find(sym);
    if (iter != XSYM(keys).end()) {
      tkey_entry = &iter->second;
    }
  }
  Twkey tkey = tkey_entry ? tkey_entry->tkey : TW_Null;

  if (tkey != TW_Null && *len == 1 && (*ShiftFlags & ~KBD_CAPS_LOCK) == KBD_NUM_LOCK &&
      this->xnumkeypad && IsNumericKeypad(tkey)) {
    /* xnumkeypad = ttrue and only NumLock led is set (ignoring CapsLock):
     * honor X11 numeric keypad mapping to numbers 0..9 and to / * - + . ENTER */

  } else if (tkey_entry != NULL && tkey_entry->len() &&
             (*len == 0 || (*ShiftFlags & ~(KBD_CAPS_LOCK | KBD_NUM_LOCK)) == 0)) {
    /* XLookupString() returned empty string, or no ShiftFlags (ignoring CapsLock/NumLock):
     * use the sequence stated in hw_keys.h */

    if (xaltcursorkeys && tkey >= TW_Left && tkey <= TW_Down) {
      seq[0] = '\033';
      seq[1] = 'O';
      seq[2] = "DACB"[tkey - TW_Left];
      *len = 3;
    } else if (xaltcursorkeys && tkey >= TW_KP_Begin && tkey <= TW_KP_Insert) {
      // these are actually vt220 and linux console sequences, but we want them to be unique,
      // while xterm reuses some sequences: KP_Next = Next and KP_Prior = Prior
      seq[0] = '\033';
      seq[1] = 'O';
      seq[2] = "uwtxvrysqp"[tkey - TW_KP_Begin];
      *len = 3;
    } else if (xaltcursorkeys && tkey >= TW_KP_0 && tkey <= TW_KP_9) {
      // match KP_0 ... KP_9 to KP_Home ... KP_Insert
      seq[0] = '\033';
      seq[1] = 'O';
      seq[2] = 'p' + (tkey - TW_KP_0);
      *len = 3;
    } else if (tkey_entry->len() < maxlen) {
      CopyMem(tkey_entry->seq, seq, *len = tkey_entry->len());
    }
    X_DEBUG_SHOW_KEY(xim, "replaced(2)", sym, *len, seq);
  }
  return tkey == TW_Null && *len != 0 ? TW_Other : tkey;
}

TW_ATTR_HIDDEN void XDRIVER::HandleEvent(XEvent *event) {
  char seq[TW_SMALLBUFF];
  dat x, y, dx, dy;
  udat len = sizeof(seq), ShiftFlags;
  Twkey TW_key;

  if (event->xany.window == this->xwindow)
    switch (event->type) {
    case KeyPress:
      TW_key = LookupKey(event, &ShiftFlags, &len, seq);
      if (TW_key != TW_Null) {
        KeyboardEventCommon(hw, TW_key, ShiftFlags, len, seq);
      }
      break;
    case KeyRelease:
      break;
    case MotionNotify:
    case ButtonPress:
    case ButtonRelease:
      x = event->xbutton.x / this->xwfont + this->xhw_startx;
      if (x < 0)
        x = 0;
      else if (x >= DisplayWidth)
        x = DisplayWidth - 1;

      y = event->xbutton.y / this->xhfont + this->xhw_starty;
      if (y < 0)
        y = 0;
      else if (y >= DisplayHeight)
        y = DisplayHeight - 1;

      if (event->type == MotionNotify) {
        dx = event->xbutton.x < this->xwfont / 2                   ? -1
             : this->xwidth - event->xbutton.x <= this->xwfont / 2 ? 1
                                                                   : 0;
        dy = event->xbutton.y < this->xhfont / 2                    ? -1
             : this->xheight - event->xbutton.y <= this->xhfont / 2 ? 1
                                                                    : 0;
        if (dx || dy || x != hw->MouseState.x || y != hw->MouseState.y)
          MouseEventCommon(hw, x, y, dx, dy, hw->MouseState.keys);
        break;
      }

      dx = event->xbutton.state;
      dy = (dx & Button1Mask ? HOLD_LEFT : 0) | (dx & Button2Mask ? HOLD_MIDDLE : 0) |
           (dx & Button3Mask ? HOLD_RIGHT : 0) | (dx & Button4Mask ? HOLD_WHEEL_REV : 0) |
           (dx & Button5Mask ? HOLD_WHEEL_FWD : 0);

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
            : dx == Button4 ? HOLD_WHEEL_REV
            : dx == Button5 ? HOLD_WHEEL_FWD
                            : 0);
      if (event->type == ButtonPress) {
        dy |= dx;
      } else {
        dy &= ~dx;
      }
      MouseEventCommon(hw, x, y, 0, 0, dy);

      break;
    case ConfigureNotify:
      if (!this->xhw_view) {
        if (hw->X != event->xconfigure.width / this->xwfont ||
            hw->Y != event->xconfigure.height / this->xhfont) {

          hw->X = (this->xwidth = event->xconfigure.width) / this->xwfont;
          hw->Y = (this->xheight = event->xconfigure.height) / this->xhfont;
          ResizeDisplayPrefer(hw);
        }
      }
      break;
    case Expose:
      x = event->xexpose.x / this->xwfont + this->xhw_startx;
      y = event->xexpose.y / this->xhfont + this->xhw_starty;
      dx = (event->xexpose.x + this->xhw_startx * this->xwfont + event->xexpose.width +
            this->xwfont - 2) /
           this->xwfont;
      dy = (event->xexpose.y + this->xhw_starty * this->xhfont + event->xexpose.height +
            this->xhfont - 2) /
           this->xhfont;

      NeedRedrawVideo(hw, x, y, dx, dy);
      /* must we redraw the cursor too ? */
      if (hw->XY[0] >= x && hw->XY[0] <= dx && hw->XY[1] >= y && hw->XY[1] <= dy) {
        hw->TT = NOCURSOR;
      }
      break;
    case VisibilityNotify:
      this->xwindow_AllVisible = event->xvisibility.state == VisibilityUnobscured;
      break;
    case SelectionClear:
      hw->SelectionPrivate = 0; /* selection now owned by some other X11 client */
      TwinSelectionSetOwner((Tobj)hw, SEL_CURRENTTIME, SEL_CURRENTTIME);
      break;
    case SelectionRequest:
      SelectionRequest_up(&event->xselectionrequest);
      break;
    case SelectionNotify:
      SelectionNotify_up(event->xselection.requestor, event->xselection.property);
      break;
    case ClientMessage:
      if (event->xclient.message_type == this->xWM_PROTOCOLS && event->xclient.format == 32 &&
          (Atom)event->xclient.data.l[0] == this->xWM_DELETE_WINDOW) {

        /* going to close this display */
        hw->NeedHW |= NeedPanicHW, NeedHW |= NeedPanicHW;
      }
      break;
    default:
      break;
    }
}

TW_ATTR_HIDDEN void XDRIVER::KeyboardEvent(int fd, Tdisplay hw) {
  XEvent event;
  XDRIVER *self = xdriver(hw);

  while (XPending(self->xdisplay) > 0) {
    XNextEvent(self->xdisplay, &event);
    self->HandleEvent(&event);
  }
}
