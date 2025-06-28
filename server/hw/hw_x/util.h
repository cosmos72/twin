/* Functions that are common between hw_x11 and hw_xft */

#include "log.h"

#define X_TITLE_MAXLEN 80

TW_ATTR_HIDDEN void XDRIVER::FillWindowTitle(char *title, int maxlen) {
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

TW_ATTR_HIDDEN void XDRIVER::HideCursor(dat x, dat y) {
  int xbegin = (x - this->xhw_startx) * this->xwfont;
  int ybegin = (y - this->xhw_starty) * this->xhfont; /* needed by XDRAW */

  tcell cell = (x >= 0 && x < DisplayWidth && y >= 0 && y < DisplayHeight)
                   ? Video[x + y * (ldat)DisplayWidth]
                   : TCELL(TCOL(tWHITE, tblack), ' ');
  tcolor col = TCOLOR(cell);
  trune f = this->xUTF_32_to_charset(TRUNE(cell));

  XChar16 c = RawToXChar16(f);

  XDRAW(col, &c, 1);
}

TW_ATTR_HIDDEN void XDRIVER::ShowCursor(uldat type, dat x, dat y) {
  tcell cell = (x >= 0 && x < DisplayWidth && y >= 0 && y < DisplayHeight)
                   ? Video[x + y * (ldat)DisplayWidth]
                   : TCELL(TCOL(tWHITE, tblack), ' ');
  tcolor color = TCOLOR(cell);
  ldat xbegin = (x - this->xhw_startx) * this->xwfont;
  ldat ybegin = (y - this->xhw_starty) * this->xhfont;

  if (type & 0x10) {
    /* soft cursor */
    tcolor v = (color | ((type >> 16) & 0xff)) ^ ((type >> 8) & 0xff);
    trune f;
    XChar16 c;
    if ((type & 0x20) && (color & TCOL(0, twhite)) == (v & TCOL(0, twhite)))
      v ^= TCOL(0, twhite);
    if ((type & 0x40) && ((TCOLFG(v) & twhite) == (TCOLBG(v) & twhite)))
      v ^= TCOL(twhite, 0);
    f = this->xUTF_32_to_charset(TRUNE(cell));
    c = RawToXChar16(f);
    XDRAW(v, &c, 1);
  } else if (type & 0xF) {
    /* VGA hw-like cursor */

    /* doesn't work as expected on paletted visuals... */
    SetFg(TCOLFG(color) ^ TCOLBG(color));

    udat i = this->xhfont * ((type & 0xF) - NOCURSOR) / (SOLIDCURSOR - NOCURSOR);

    XSetFunction(this->xdisplay, this->xgc, this->xsgc.function = GXxor);
    XFillRectangle(this->xdisplay, this->xwindow, this->xgc, xbegin, ybegin + this->xhfont - i,
                   this->xwfont, i);
    XSetFunction(this->xdisplay, this->xgc, this->xsgc.function = GXcopy);
  }
}

TW_ATTR_HIDDEN void XDRIVER::FlushVideo(Tdisplay hw) {
  XDRIVER *self = xdriver(hw);
  uldat i;
  dat start, end;
  byte iff = tfalse;

  if (ValidOldVideo) {
    iff = ChangedVideoFlag && Video[hw->XY[0] + hw->XY[1] * (ldat)DisplayWidth] !=
                                  OldVideo[hw->XY[0] + hw->XY[1] * (ldat)DisplayWidth];
    /* ttrue if and only if the cursor will be erased by burst */
  }

  /* first burst all changes */
  if (ChangedVideoFlag) {
    for (i = 0; i < (uldat)DisplayHeight * 2; i++) {
      start = ChangedVideo[i >> 1][i & 1][0];
      end = ChangedVideo[i >> 1][i & 1][1];

      if (start != -1)
        self->DrawSome(start, i >> 1, end - start + 1);
    }
    hw->setFlush();
  }
  /* then, we may have to erase the old cursor */
  if (!ValidOldVideo || (!iff && hw->TT != NOCURSOR &&
                         (CursorType != hw->TT || CursorX != hw->XY[0] || CursorY != hw->XY[1]))) {

    hw->TT = NOCURSOR;
    self->HideCursor(hw->XY[0], hw->XY[1]);
    hw->setFlush();
  }
  /* finally, redraw the cursor if forced to redraw or */
  /* (we want a cursor and (the burst erased the cursor or the cursor changed)) */
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR &&
       (iff || CursorType != hw->TT || CursorX != hw->XY[0] || CursorY != hw->XY[1]))) {

    self->ShowCursor(hw->TT = CursorType, hw->XY[0] = CursorX, hw->XY[1] = CursorY);
    hw->setFlush();
  }

  hw->FlagsHW &= ~FlagChangedMouseFlagHW;
}

TW_ATTR_HIDDEN void XDRIVER::FlushHW(Tdisplay hw) {
  XFlush(xdriver(hw)->xdisplay);
  hw->clrFlush();
}

TW_ATTR_HIDDEN void XDRIVER::DetectSize(Tdisplay hw, dat *x, dat *y) {
  XDRIVER *self = xdriver(hw);
  if (!self->xhw_view) {
    *x = hw->X = self->xwidth / self->xwfont;
    *y = hw->Y = self->xheight / self->xhfont;
  }
}

TW_ATTR_HIDDEN void XDRIVER::CheckResize(Tdisplay /*hw*/, dat * /*x*/, dat * /*y*/) {
  /* always ok */
}

TW_ATTR_HIDDEN void XDRIVER::Resize(Tdisplay hw, dat x, dat y) {
  XDRIVER *self = xdriver(hw);
  if (x != hw->X || y != hw->Y) {
    if (!self->xhw_view) {
      XResizeWindow(self->xdisplay, self->xwindow,
                    self->xwidth = self->xwfont * (hw->X = self->xhw_endx = x),
                    self->xheight = self->xhfont * (hw->Y = self->xhw_endy = y));
    }
    hw->setFlush();
  }
}

/*
 * import X11 Selection
 */
TW_ATTR_HIDDEN bool XDRIVER::SelectionImport_X11(Tdisplay hw) {
  return !hw->SelectionPrivate;
}

/*
 * export our Selection to X11
 */
TW_ATTR_HIDDEN void XDRIVER::SelectionExport_X11(Tdisplay hw) {
  XDRIVER *self = xdriver(hw);
  if (!hw->SelectionPrivate) {
    XSetSelectionOwner(self->xdisplay, XA_PRIMARY, self->xwindow, CurrentTime);
    hw->SelectionPrivate = (tany)self->xwindow;
    hw->setFlush();
  }
}

TW_ATTR_HIDDEN void XDRIVER::utf8_to_wchar(Chars src, Vector<wchar_t> &dst) {
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
TW_ATTR_HIDDEN void XDRIVER::SelectionNotify_X11(Tdisplay hw, uldat reqprivate, e_id Magic,
                                                 Chars /*mime*/, Chars data) {
  XEvent ev;
  XDRIVER *self = xdriver(hw);
  Atom target;

  if (self->XReqCount == 0) {
    log(ERROR) << THIS
        ".c: " XSTR(XDRIVER) ".SelectionNotify_X11(): unexpected Twin Selection Notify event!\n";
    return;
  }
#if 0
  else {
    log(ERROR) << THIS ".c: " XSTR(XDRIVER) ".SelectionNotify_X11(): " << self->XReqCount
               << " nested Twin Selection Notify events\n";
  }
#endif

  self->XReqCount--;
  XSelectionRequestEvent &req = self->xReq[self->XReqCount];

  ev.xselection.type = SelectionNotify;
  ev.xselection.property = None;
  ev.xselection.display = req.display;
  ev.xselection.requestor = req.requestor;
  ev.xselection.selection = req.selection;
  ev.xselection.target = target = req.target;
  ev.xselection.time = req.time;

  if (target == self->xTARGETS) {
    /*
     * On some systems, the Atom typedef is 64 bits wide.
     * We need a type that is exactly 32 bits wide,
     * because a format of 64 is not allowed by the X11 protocol.
     */
    typedef CARD32 Atom32;
    Atom32 target_list[5];

    target_list[0] = (Atom32)self->xTARGETS;
    target_list[1] = (Atom32)XA_STRING;
    target_list[2] = (Atom32)self->xUTF8_STRING;
    target_list[3] = (Atom32)self->xTEXT;
    target_list[4] = (Atom32)self->xCOMPOUND_TEXT;
    XChangeProperty(self->xdisplay, req.requestor, req.property, self->xTARGETS, 32,
                    PropModeReplace, (const byte *)target_list, sizeof(target_list));

  } else if (target == self->xUTF8_STRING) {

    /* notify X11 selection as UTF-8 */
    XChangeProperty(self->xdisplay, req.requestor, req.property, self->xUTF8_STRING, 8,
                    PropModeReplace, (const byte *)data.data(), data.size());

  } else {
    XICCEncodingStyle style;
    if (target == XA_STRING) {
      style = XStringStyle;
    } else if (target == self->xTEXT) {
      style = XStdICCTextStyle;
    } else /*if (target == xCOMPOUND_TEXT)*/ {
      style = XCompoundTextStyle;
    }
    Vector<wchar_t> wtext;
    utf8_to_wchar(data, wtext);
    wtext.append('\0');
    wchar_t *waddr = wtext.data();

    XTextProperty ct = {};
    bool freect = false;
    if (XwcTextListToTextProperty(self->xdisplay, &waddr, 1, style, &ct) >= 0) {
      freect = true;
    } else {
      ct.value = (byte *)const_cast<char *>(data.data());
      ct.nitems = data.size();
      ct.encoding = target;
    }

    XChangeProperty(self->xdisplay, req.requestor, req.property, ct.encoding, 8, PropModeReplace,
                    ct.value, (int)ct.nitems);
    if (freect) {
      XFree(ct.value);
    }
  }
  ev.xselection.property = req.property;
  XSendEvent(self->xdisplay, req.requestor, False, 0, &ev);
  hw->setFlush();
}

/*
 * notify the X11 Selection to twin upper layer
 */
TW_ATTR_HIDDEN void XDRIVER::SelectionNotify_up(Window win, Atom prop) {
  long nread = 0;
  unsigned long i, nitems, bytes_after = TW_BIGBUFF;
  Atom actual_type;
  int actual_fmt;
  byte *data;
  String buff;
  bool ok = true;

  if (this->xReqCount == 0) {
    log(WARNING) << THIS
        ".c: " XSTR(XDRIVER) ".SelectionNotify_up(): unexpected X Selection Notify event!\n";
    return;
  }
#if 0
  else {
    log(INFO) << THIS ".c: " XSTR(XDRIVER)  ".SelectionNotify_up(): " << xReqCount
              << " nested X Selection Notify event\n";
  }
#endif
  if (prop == None) {
    return;
  }

  this->xReqCount--;

  do {
    data = NULL;
    if (bytes_after > TW_BIGBUFF) {
      bytes_after = TW_BIGBUFF;
    }
    ok = XGetWindowProperty(this->xdisplay, win, prop, (nread + 3) / 4, (bytes_after + 3) / 4,
                            False, AnyPropertyType, &actual_type, &actual_fmt, &nitems,
                            &bytes_after, &data) == Success;
    if (ok) {
      nread += nitems * (actual_fmt / 8);
    }

    if (actual_type == this->xUTF8_STRING) {
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
  XDeleteProperty(this->xdisplay, win, prop);

  if (buff) {
    TwinSelectionNotify(this->xRequestor[this->xReqCount], this->xReqPrivate[this->xReqCount],
                        SEL_UTF8MAGIC, Chars(), buff);
  }
}

/*
 * request X11 Selection
 */
TW_ATTR_HIDDEN void XDRIVER::SelectionRequest_X11(Tdisplay hw, Tobj requestor, uldat reqprivate) {

  if (!hw->SelectionPrivate) {
    XDRIVER *self = xdriver(hw);
    if (self->xReqCount == NEST) {
      log(ERROR) << THIS ".c: " XSTR(
          XDRIVER) ".SelectionRequest_X11(): too many nested Twin Selection Request events!\n";
      return;
    }
#if 0
    else {
      log(INFO) << THIS ".c: " XSTR(XDRIVER) ".SelectionRequest_X11(): " << (self->xReqCount + 1)
                << " nested Twin Selection Request events\n";
    }
#endif
    self->xRequestor[self->xReqCount] = requestor;
    self->xReqPrivate[self->xReqCount] = reqprivate;
    self->xReqCount++;

    if (XGetSelectionOwner(self->xdisplay, XA_PRIMARY) == None) {
      self->SelectionNotify_up(DefaultRootWindow(self->xdisplay), XA_CUT_BUFFER0);
    } else {
      Atom prop = XInternAtom(self->xdisplay, "VT_SELECTION", False);
      /* Request conversion to UTF-8. Not all owners will be able to fulfill that request. */
      XConvertSelection(self->xdisplay, XA_PRIMARY, self->xUTF8_STRING, prop, self->xwindow,
                        CurrentTime);

      hw->setFlush();
      /* we will get an X11 SelectionNotify event */
    }
  }
  /* else race! someone else became Selection owner in the meanwhile... */
}

/*
 * request twin Selection
 */
TW_ATTR_HIDDEN void XDRIVER::SelectionRequest_up(XSelectionRequestEvent *req) {
  if (this->XReqCount == NEST) {
    log(ERROR) << THIS ".c: " XSTR(
        XDRIVER) ".SelectionRequest_up(): too many nested X Selection Request events!\n";
    return;
  }
#if 0
  else {
    log(INFO) << THIS ".c: " XSTR(XDRIVER) ".SelectionRequest_up(): " << (this->XReqCount + 1)
              << " nested X Selection Request events\n";
  }
#endif
  CopyMem(req, &this->xReq[this->XReqCount], sizeof(XSelectionRequestEvent));
  TwinSelectionRequest((Tobj)hw, this->XReqCount++, TwinSelectionGetOwner());
  /* we will get a call to hw->fnSelectionNotify i.e. XDRIVER::SelectionNotify_X11() */
  /* the call **CAN** arrive while we are still inside TwinSelectionRequest() !!! */
}

TW_ATTR_HIDDEN bool XDRIVER::CanDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn,
                                         dat DstLeft, dat DstUp) {
  return !hw->RedrawVideo /* if window is not up-to-date, XCopyArea() is unusable */
         /* if window is partially covered, XCopyArea() cannot work */
         && xdriver(hw)->xwindow_AllVisible         /**/
         && (Rgt - Left + 1) * (Dwn - Up + 1) > 20; /* avoid XCopyArea() for very small areas */
}

TW_ATTR_HIDDEN void XDRIVER::DragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft,
                                      dat DstUp) {
  XDRIVER *self = xdriver(hw);
  dat DstRgt = (Rgt - Left) + DstLeft;
  dat DstDwn = (Dwn - Up) + DstUp;

  if (hw->TT != NOCURSOR) {
    if (hw->XY[0] >= Left && hw->XY[0] <= Rgt && hw->XY[1] >= Up && hw->XY[1] <= Dwn) {
      /* must hide the cursor before dragging */
      self->HideCursor(hw->XY[0], hw->XY[1]);
      /* and remember to redraw it */
      hw->TT = (uldat)-1;
    } else if (hw->XY[0] >= DstLeft && hw->XY[0] <= DstRgt && hw->XY[1] >= DstUp &&
               hw->XY[1] <= DstDwn) {
      /* cursor will be overwritten by drag, remember to redraw it */
      hw->TT = (uldat)-1;
    }
  }
  XCopyArea(self->xdisplay, self->xwindow, self->xwindow, self->xgc, Left * self->xwfont,
            Up * self->xhfont, (Rgt - Left + 1) * self->xwfont, (Dwn - Up + 1) * self->xhfont,
            DstLeft * self->xwfont, DstUp * self->xhfont);
  hw->setFlush();
}

#if 0
/* does NOT work... libX11 insists on doing exit(1) */
TW_ATTR_HIDDEN int XDRIVER::Die(Display *d) {
  /*
   * this is not exactly trivial:
   * find our hw, shut it down
   * and quit if it was the last hw.
   *
   * don't rely on hw->Private only, as non-X11 displays
   * may use it differently and have by chance the same value for it.
   */
  forallHW {
    if (hw->QuitHW == &XDRIVER::QuitHW && hw->Private && d == this->xdisplay) {

      hw->NeedHW |= NeedPanicHW, NeedHW |= NeedPanicHW;

      break;
    }
  }
  return 0;
}
#else
TW_ATTR_HIDDEN int XDRIVER::Die(Display * /*d*/) {
  Quit(0);
  return 0;
}
#endif

TW_ATTR_HIDDEN trune XDRIVER::UTF_32_to_UCS_2(trune c) {
  if ((c & 0x1FFE00) == 0xF000) {
    /* private use codepoints. for compatibility, treat as "direct-to-font" zone */
    c &= 0x01FF;
  }
  if (c >= 0x110000) {
    /* invalid Unicode codepoint, set to "replacement character" */
    c = 0xFFFD;
  }
  return c;
}
