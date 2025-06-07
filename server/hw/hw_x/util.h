/* Functions that are common between hw_x11 and hw_xft */

#include "log.h"

#define X_TITLE_MAXLEN 80

static void XSYM(FillWindowTitle)(char *title, int maxlen) {
  int left = maxlen;
  int chunk = 0;

  memset(title, '\0', left);
  if (left < 6)
    return;

  left--; /* reserve space for final '\0' */

  memcpy(title, "twin ", 5);
  title += 5;
  left -= 5;

  if (gethostname(title, left) == 0) {
    char *end = (char *)memchr(title, '\0', left);
    chunk = (end == NULL) ? 0 : (int)(end - title);
    if (chunk > 0 && chunk <= left) {
      title[-1] = '@';
      title += chunk;
      left -= chunk;
    }
  }
  chunk = strlen(TWDisplay);
  if (chunk > 0 && chunk <= left) {
    memcpy(title, TWDisplay, chunk);
    title += chunk;
    left -= chunk;
  }
  if (left <= 0)
    title--;
  title[0] = '\0';
}

static void XSYM(HideCursor)(dat x, dat y) {
  int xbegin = (x - xhw_startx) * xwfont;
  int ybegin = (y - xhw_starty) * xhfont; /* needed by XDRAW */

  tcell V = (x >= 0 && x < DisplayWidth && y >= 0 && y < DisplayHeight)
                ? Video[x + y * (ldat)DisplayWidth]
                : TCELL(TCOL(thigh | twhite, tblack), ' ');
  tcolor col = TCOLOR(V);
  trune f = xUTF_32_to_charset(TRUNE(V));

  XChar16 c = RawToXChar16(f);

  XDRAW(col, &c, 1);
}

static void XSYM(ShowCursor)(uldat type, dat x, dat y) {
  tcell V = (x >= 0 && x < DisplayWidth && y >= 0 && y < DisplayHeight)
                ? Video[x + y * (ldat)DisplayWidth]
                : TCELL(TCOL(thigh | twhite, tblack), ' ');

  ldat xbegin = (x - xhw_startx) * xwfont;
  ldat ybegin = (y - xhw_starty) * xhfont;

  if (type & 0x10) {
    /* soft cursor */
    tcolor v = (TCOLOR(V) | ((type >> 16) & 0xff)) ^ ((type >> 8) & 0xff);
    trune f;
    XChar16 c;
    if ((type & 0x20) && (TCOLOR(V) & TCOL(0, twhite)) == (v & TCOL(0, twhite)))
      v ^= TCOL(0, twhite);
    if ((type & 0x40) && ((TCOLFG(v) & twhite) == (TCOLBG(v) & twhite)))
      v ^= TCOL(twhite, 0);
    f = xUTF_32_to_charset(TRUNE(V));
    c = RawToXChar16(f);
    XDRAW(v, &c, 1);
  } else if (type & 0xF) {
    /* VGA hw-like cursor */

    /* doesn't work as expected on paletted visuals... */
    unsigned long fg = xcol[TCOLFG(TCOLOR(V)) ^ TCOLBG(TCOLOR(V))];

    udat i = xhfont * ((type & 0xF) - NOCURSOR) / (SOLIDCURSOR - NOCURSOR);

    if (xsgc.foreground != fg) {
      XSetForeground(xdisplay, xgc, xsgc.foreground = fg);
#if HW_X_DRIVER == HW_XFT
      xforeground = xftcolors[TCOLFG(TCOLOR(V)) ^ TCOLBG(TCOLOR(V))];
#endif
    }

    XSetFunction(xdisplay, xgc, xsgc.function = GXxor);
    XFillRectangle(xdisplay, xwindow, xgc, xbegin, ybegin + xhfont - i, xwfont, i);
    XSetFunction(xdisplay, xgc, xsgc.function = GXcopy);
  }
}

static void XSYM(FlushVideo)(void) {
  uldat i;
  dat start, end;
  byte iff = tfalse;

  if (ValidOldVideo) {
    iff = ChangedVideoFlag && Video[HW->XY[0] + HW->XY[1] * (ldat)DisplayWidth] !=
                                  OldVideo[HW->XY[0] + HW->XY[1] * (ldat)DisplayWidth];
    /* ttrue if and only if the cursor will be erased by burst */
  }

  /* first burst all changes */
  if (ChangedVideoFlag) {
    for (i = 0; i < (uldat)DisplayHeight * 2; i++) {
      start = ChangedVideo[i >> 1][i & 1][0];
      end = ChangedVideo[i >> 1][i & 1][1];

      if (start != -1)
        XSYM(DrawSome)(start, i >> 1, end - start + 1);
    }
    setFlush();
  }
  /* then, we may have to erase the old cursor */
  if (!ValidOldVideo || (!iff && HW->TT != NOCURSOR &&
                         (CursorType != HW->TT || CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {

    HW->TT = NOCURSOR;
    XSYM(HideCursor)(HW->XY[0], HW->XY[1]);
    setFlush();
  }
  /* finally, redraw the cursor if forced to redraw or */
  /* (we want a cursor and (the burst erased the cursor or the cursor changed)) */
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR &&
       (iff || CursorType != HW->TT || CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {

    XSYM(ShowCursor)(HW->TT = CursorType, HW->XY[0] = CursorX, HW->XY[1] = CursorY);
    setFlush();
  }

  HW->FlagsHW &= ~FlHWChangedMouseFlag;
}

static void XSYM(FlushHW)(void) {
  XFlush(xdisplay);
  clrFlush();
}

static void XSYM(DetectSize)(dat *x, dat *y) {
  if (!xhw_view) {
    *x = HW->X = xwidth / xwfont;
    *y = HW->Y = xheight / xhfont;
  }
}

static void XSYM(CheckResize)(dat * /*x*/, dat * /*y*/) {
  /* always ok */
}

static void XSYM(Resize)(dat x, dat y) {
  if (x != HW->X || y != HW->Y) {
    if (!xhw_view) {
      XResizeWindow(xdisplay, xwindow, xwidth = xwfont * (HW->X = xhw_endx = x),
                    xheight = xhfont * (HW->Y = xhw_endy = y));
    }
    setFlush();
  }
}

/*
 * import X11 Selection
 */
static byte XSYM(SelectionImport_X11)(void) {
  return !HW->HWSelectionPrivate;
}

/*
 * export our Selection to X11
 */
static void XSYM(SelectionExport_X11)(void) {
  if (!HW->HWSelectionPrivate) {
    XSetSelectionOwner(xdisplay, XA_PRIMARY, xwindow, CurrentTime);
    HW->HWSelectionPrivate = (tany)xwindow;
    setFlush();
  }
}

static void XSYM(utf8_to_wchar)(Chars src, Vector<wchar_t> &dst) {
  dst.reserve(src.size());
  Utf8 seq;
  while (src) {
    seq.parse(src, &src);
    dst.append(seq.rune());
  }
}

/*
 * notify our Selection to X11
 */
static void XSYM(SelectionNotify_X11)(uldat reqprivate, e_id Magic, Chars /*mime*/, Chars data) {
  XEvent ev;
  Atom target;

  if (XReqCount == 0) {
    log(ERROR) << THIS
        ".c: " XSYM_STR(SelectionNotify_X11) "(): unexpected Twin Selection Notify event!\n";
    return;
  }
#if 0
  else {
    log(ERROR) << THIS ".c: " XSYM_STR(SelectionNotify_X11) "(): " << XReqCount
               << " nested Twin Selection Notify events\n";
  }
#endif

  XReqCount--;
  XSelectionRequestEvent &req = XReq(XReqCount);

  ev.xselection.type = SelectionNotify;
  ev.xselection.property = None;
  ev.xselection.display = req.display;
  ev.xselection.requestor = req.requestor;
  ev.xselection.selection = req.selection;
  ev.xselection.target = target = req.target;
  ev.xselection.time = req.time;

  if (target == xTARGETS) {
    /*
     * On some systems, the Atom typedef is 64 bits wide.
     * We need a type that is exactly 32 bits wide,
     * because a format of 64 is not allowed by the X11 protocol.
     */
    typedef CARD32 Atom32;
    Atom32 target_list[5];

    target_list[0] = (Atom32)xTARGETS;
    target_list[1] = (Atom32)XA_STRING;
    target_list[2] = (Atom32)xUTF8_STRING;
    target_list[3] = (Atom32)xTEXT;
    target_list[4] = (Atom32)xCOMPOUND_TEXT;
    XChangeProperty(xdisplay, req.requestor, req.property, xTARGETS, 32, PropModeReplace,
                    (const byte *)target_list, sizeof(target_list));

  } else if (target == xUTF8_STRING) {

    /* notify X11 selection as UTF-8 */
    XChangeProperty(xdisplay, req.requestor, req.property, xUTF8_STRING, 8, PropModeReplace,
                    (const byte *)data.data(), data.size());

  } else {
    XICCEncodingStyle style;
    if (target == XA_STRING) {
      style = XStringStyle;
    } else if (target == xTEXT) {
      style = XStdICCTextStyle;
    } else /*if (target == xCOMPOUND_TEXT)*/ {
      style = XCompoundTextStyle;
    }
    Vector<wchar_t> wtext;
    XSYM(utf8_to_wchar)(data, wtext);
    wtext.append('\0');
    wchar_t *waddr = wtext.data();

    XTextProperty ct = {};
    bool freect = false;
    if (XwcTextListToTextProperty(xdisplay, &waddr, 1, style, &ct) >= 0) {
      freect = true;
    } else {
      ct.value = (byte *)const_cast<char *>(data.data());
      ct.nitems = data.size();
      ct.encoding = target;
    }

    XChangeProperty(xdisplay, req.requestor, req.property, ct.encoding, 8, PropModeReplace,
                    ct.value, (int)ct.nitems);
    if (freect) {
      XFree(ct.value);
    }
  }
  ev.xselection.property = req.property;
  XSendEvent(xdisplay, req.requestor, False, 0, &ev);
  setFlush();
}

/*
 * notify the X11 Selection to twin upper layer
 */
static void XSYM(SelectionNotify_up)(Window win, Atom prop) {
  long nread = 0;
  unsigned long i, nitems, bytes_after = TW_BIGBUFF;
  Atom actual_type;
  int actual_fmt;
  byte *data;
  String buff;
  bool ok = true;

  if (xReqCount == 0) {
    log(WARNING) << THIS
        ".c: " XSYM_STR(SelectionNotify_up) "(): unexpected X Selection Notify event!\n";
    return;
  }
#if 0
  else {
    log(INFO) << THIS ".c: " XSYM_STR(SelectionNotify_up) "(): " << xReqCount
              << " nested X Selection Notify event\n";
  }
#endif
  if (prop == None) {
    return;
  }

  xReqCount--;

  do {
    data = NULL;
    if (bytes_after > TW_BIGBUFF) {
      bytes_after = TW_BIGBUFF;
    }
    ok = XGetWindowProperty(xdisplay, win, prop, (nread + 3) / 4, (bytes_after + 3) / 4, False,
                            AnyPropertyType, &actual_type, &actual_fmt, &nitems, &bytes_after,
                            &data) == Success;
    if (ok) {
      nread += nitems * (actual_fmt / 8);
    }

    if (actual_type == xUTF8_STRING) {
      /* X11 selection contains UTF-8 */
      ok = buff.append((const char *)data, nitems);
    } else if (actual_type == XA_STRING) {
      /* X11 selection contains ISO8859-1: convert to UTF-8 */
      for (i = 0; ok && i < nitems; i++) {
        ok = buff.append(Utf8(trune(data[i])));
      }
    } else {
      ok = false;
    }
    if (data) {
      XFree(data);
    }
  } while (ok && bytes_after > 0);

  /* Signal the selection owner that we have successfully read the data. */
  XDeleteProperty(xdisplay, win, prop);

  if (buff) {
    TwinSelectionNotify(xRequestor(xReqCount), xReqPrivate(xReqCount), SEL_UTF8MAGIC, Chars(),
                        buff);
  }
}

/*
 * request X11 Selection
 */
static void XSYM(SelectionRequest_X11)(Tobj requestor, uldat reqprivate) {
  if (!HW->HWSelectionPrivate) {

    if (xReqCount == NEST) {
      log(ERROR) << THIS ".c: " XSYM_STR(
          SelectionRequest_X11) "(): too many nested Twin Selection Request events!\n";
      return;
    }
#if 0
    else {
      log(INFO) << THIS ".c: " XSYM_STR(SelectionRequest_X11) "(): " << (xReqCount + 1)
                << " nested Twin Selection Request events\n";
    }
#endif
    xRequestor(xReqCount) = requestor;
    xReqPrivate(xReqCount) = reqprivate;
    xReqCount++;

    if (XGetSelectionOwner(xdisplay, XA_PRIMARY) == None) {
      XSYM(SelectionNotify_up)(DefaultRootWindow(xdisplay), XA_CUT_BUFFER0);
    } else {
      Atom prop = XInternAtom(xdisplay, "VT_SELECTION", False);
      /* Request conversion to UTF-8. Not all owners will be able to fulfill that request. */
      XConvertSelection(xdisplay, XA_PRIMARY, xUTF8_STRING, prop, xwindow, CurrentTime);

      setFlush();
      /* we will get an X11 SelectionNotify event */
    }
  }
  /* else race! someone else became Selection owner in the meanwhile... */
}

/*
 * request twin Selection
 */
static void XSYM(SelectionRequest_up)(XSelectionRequestEvent *req) {
  if (XReqCount == NEST) {
    log(ERROR) << THIS
        ".c: " XSYM_STR(SelectionRequest_up) "(): too many nested X Selection Request events!\n";
    return;
  }
#if 0
  else {
    log(INFO) << THIS ".c: " XSYM_STR(SelectionRequest_up) "(): " << (XReqCount + 1)
              << " nested X Selection Request events\n";
  }
#endif
  CopyMem(req, &XReq(XReqCount), sizeof(XSelectionRequestEvent));
  TwinSelectionRequest((Tobj)HW, XReqCount++, TwinSelectionGetOwner());
  /* we will get a call to HW->HWSelectionNotify i.e. XSYM(SelectionNotify_X11) */
  /* the call **CAN** arrive while we are still inside TwinSelectionRequest() !!! */
}

static byte XSYM(CanDragArea)(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  return xwindow_AllVisible  /* if window is partially covered, XCopyArea() cannot work */
         && !HW->RedrawVideo /* if window is not up-to-date, XCopyArea() is unusable */
         && (Rgt - Left + 1) * (Dwn - Up + 1) > 20; /* avoid XCopyArea() for very small areas */
}

static void XSYM(DragArea)(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  dat DstRgt = (Rgt - Left) + DstLeft;
  dat DstDwn = (Dwn - Up) + DstUp;

  if (HW->TT != NOCURSOR) {
    if (HW->XY[0] >= Left && HW->XY[0] <= Rgt && HW->XY[1] >= Up && HW->XY[1] <= Dwn) {
      /* must hide the cursor before dragging */
      XSYM(HideCursor)(HW->XY[0], HW->XY[1]);
      /* and remember to redraw it */
      HW->TT = (uldat)-1;
    } else if (HW->XY[0] >= DstLeft && HW->XY[0] <= DstRgt && HW->XY[1] >= DstUp &&
               HW->XY[1] <= DstDwn) {
      /* cursor will be overwritten by drag, remember to redraw it */
      HW->TT = (uldat)-1;
    }
  }
  XCopyArea(xdisplay, xwindow, xwindow, xgc, Left * xwfont, Up * xhfont, (Rgt - Left + 1) * xwfont,
            (Dwn - Up + 1) * xhfont, DstLeft * xwfont, DstUp * xhfont);
  setFlush();
}

#if 0
/* does NOT work... libX11 insists on doing exit(1) */
static int XSYM(Die)(Display *d) {
    /*
     * this is not exactly trivial:
     * find our HW, shut it down
     * and quit if it was the last HW.
     *
     * don't rely on HW->Private only, as non-X11 displays
     * may use it differently and have by chance the same value for it.
     */
    forallHW {
        if (HW->QuitHW == XSYM(QuitHW) && HW->Private
            && d == xdisplay) { /* expands to HW->Private->xdisplay */

            HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;

            break;
        }
    }
    return 0;
}
#else
static int XSYM(Die)(Display *d) {
  Quit(0);
  return 0;
}
#endif

#if HW_X_DRIVER != HW_XFT
static Tutf_function XSYM(UTF_32_to_charset_function)(const char *charset) {
  unsigned long prop;
  const char *s, *fontname = NULL;
  uldat i;

  if (!charset) {
    /* attempt to autodetect encoding from fontname */
    if (XGetFontProperty(xsfont, XA_FONT, &prop))
      fontname = XGetAtomName(xdisplay, (Atom)prop);

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
        log(WARNING) << "    " XSYM_STR(InitHW) ": font `" << Chars::from_c(fontname)
                     << "\' has unknown charset encoding,\n"
                        "                assuming Unicode.\n";
        return NULL;
      }
      /* else assume codepage437. gross. */
      log(WARNING) << "    " XSYM_STR(InitHW) ": font `" << Chars::from_c(fontname)
                   << "\' has unknown charset encoding,\n"
                      "                assuming charset `CP437' i.e. `VGA'.\n";
      return Tutf_UTF_32_to_CP437;
    }
  }

  i = Tutf_charset_id(charset);
  s = Tutf_charset_name(i);
  if (s && !strcmp(s, T_NAME_UTF_32)) {
    /* this is an Unicode font. good. */
    return NULL;
  }

  if (i == (uldat)-1) {
    log(WARNING) << "      " XSYM_STR(InitHW) ": libtutf warning: unknown charset `"
                 << Chars::from_c(charset) << "', assuming charset `CP437' i.e. `VGA'.\n";
    return Tutf_UTF_32_to_CP437;
  }

  return Tutf_UTF_32_to_charset_function(i);
}
#endif

static trune XSYM(UTF_32_to_UCS_2)(trune c) {
  if ((c & 0x1FFE00) == 0xF000)
    /* private use codepoints. for compatibility, treat as "direct-to-font" zone */
    c &= 0x01FF;
  if (c > 0x10FFFF)
    /* not representable in two bytes */
    c = 0xFFFD;
  return c;
}
