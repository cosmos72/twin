/* Functions that are common between hw_x11 and hw_xft. */

#include "algo.h"
#include "log.h"
#include "palette.h"

/** FIXME: refactor as XDRIVER field */
static tcolor _col;

unsigned long XDRIVER::ColorToPixel(trgb rgb) {
  if (this->xtruecolor) {
    return this->xrgb_info.pixel(rgb);
  }
  return this->xpalette[TrueColorToPalette256(rgb)];
}

void XDRIVER::DrawSome(dat x, dat y, ldat len) {
  tcell *V, *oV;
  tcolor col;
  udat buflen = 0;
  trune f;
  XChar16 buf[TW_SMALLBUFF];
  int xbegin, ybegin;

  if (len <= 0) {
    return;
  }
  if (this->xhw_view) {
    if (x >= this->xhw_endx || x + len < this->xhw_startx || y < this->xhw_starty ||
        y >= this->xhw_endy) {
      return;
    }
    if (x + len >= this->xhw_endx)
      len = this->xhw_endx - x;

    if (x < this->xhw_startx) {
      len -= this->xhw_startx - x;
      x = this->xhw_startx;
    }
  }

  xbegin = (x - this->xhw_startx) * (ldat)this->xwfont;
  ybegin = (y - this->xhw_starty) * (ldat)this->xhfont;

  V = Video + x + y * (ldat)DisplayWidth;
  oV = OldVideo + x + y * (ldat)DisplayWidth;

  for (_col = ~TCOLOR(*V); len; x++, V++, oV++, len--) {
    col = TCOLOR(*V);
    if (buflen && (col != _col || (ValidOldVideo && *V == *oV) || buflen == TW_SMALLBUFF)) {
      XDRAW(_col, buf, buflen);
      buflen = 0;
    }
    if (!ValidOldVideo || *V != *oV) {
      if (!buflen) {
        xbegin = (x - this->xhw_startx) * (ldat)this->xwfont;
        _col = col;
      }
      f = this->xUTF_32_to_charset(TRUNE(*V));
      buf[buflen++] = RawToXChar16(f);
    }
  }
  if (buflen) {
    XDRAW(_col, buf, buflen);
    buflen = 0;
  }
}

enum { MAX_FONT_SCORE = 100 };

ldat XDRIVER::FontScoreOf(udat fontwidth, udat fontheight, ldat width, ldat height) {
  /*
   * TODO xft: since xft fonts are scalable, we could just consider the aspect ratio
   * compared to fontwidth X fontheight.
   */
  ldat prod1 = (ldat)height * fontwidth;
  ldat prod2 = (ldat)width * fontheight;
  ldat score = MAX_FONT_SCORE - diff(height, fontheight) -
               2 * diff(width, fontwidth)
               /* compare height/width ratio with requested one */
               - diff(prod1, prod2) / Max3(1, fontwidth, width);
  return score;
}

bool XDRIVER::LoadFont(const char *fontname, udat fontwidth, udat fontheight) {
  char *alloc_fontname = NULL;
  byte loaded = false;

  fontname = alloc_fontname = AutodetectFont(fontname, fontwidth, fontheight);
#if HW_X_DRIVER == HW_X11
  if ((fontname && (this->xsfont = XLoadQueryFont(this->xdisplay, fontname))) ||
      (this->xsfont = XLoadQueryFont(this->xdisplay, fontname = "fixed")))
#elif HW_X_DRIVER == HW_XFT
  if (fontname &&
      (this->xsfont = XftFontOpenName(this->xdisplay, DefaultScreen(this->xdisplay), fontname)))
#endif
  {
    loaded = true;

#if HW_X_DRIVER == HW_X11
    this->xwfont = this->xsfont->min_bounds.width;
#elif HW_X_DRIVER == HW_XFT
    this->xwfont = this->xsfont->max_advance_width;
#endif
    this->xwidth = this->xwfont * (unsigned)(hw->X = GetDisplayWidth());
    this->xhfont = (this->xupfont = this->xsfont->ascent) + this->xsfont->descent;
    this->xheight = this->xhfont * (unsigned)(hw->Y = GetDisplayHeight());

    log(INFO) << "      selected " << this->xwfont << "x" << this->xhfont << " font `"
              << Chars::from_c(fontname) << "'\n";
  }
  if (alloc_fontname) {
    free(alloc_fontname); /* allocated by FcNameUnparse() */
  }
  return loaded;
}

int XDRIVER::check_hw_name(char *hw_name) {
  char *comma, *at;
  if (strncmp(hw_name, "-hw=", 4) != 0) {
    return -1;
  }
  comma = strchr(hw_name, ',');
  at = strchr(hw_name, '@');
  if (at == NULL) {
    at = comma;
  } else if (comma != NULL && comma < at) {
    at = comma;
  }
  return at ? at - hw_name : -1;
}

bool XDRIVER::InitHW(Tdisplay hw) {
  XDRIVER *self;
  if (!(hw->Private = self = (XDRIVER *)AllocMem0(sizeof(XDRIVER)))) {
    log(ERROR) << "      " XSTR(XDRIVER) ".InitHW() Out of memory!\n";
    return false;
  }
  self->hw = hw;
  if (self->InitHW()) {
    return true;
  }
  QuitHW(hw);
  return false;
}

bool XDRIVER::InitHW() {
  char *arg = hw->Name.data(); // guaranteed to be '\0' terminated

  XSetWindowAttributes attr;
  XSizeHints *hints;
  XEvent event;
  XDRIVER *p;
  char *s, *display = NULL, *display0 = NULL, *fontname = NULL, *fontname0 = NULL, *charset = NULL,
           *charset0 = NULL, title[X_TITLE_MAXLEN];
  unsigned long creategc_mask = GCForeground | GCBackground | GCGraphicsExposures;
  int i, nskip = 0;
  udat fontwidth = 10, fontheight = 20;
  bool drag = false, noinput = false, palette = false;

  /* default: show the whole screen */
  this->xhw_view = this->xhw_startx = this->xhw_starty = this->xhw_endx = this->xhw_endy = 0;

  /* not yet opened */
  this->xdisplay = NULL;

  if (arg && *arg && ((nskip = check_hw_name(arg)) >= 0)) {
    arg += nskip;

    if (*arg == '@') {
      if ((s = strchr(display = ++arg, ','))) {
        *(display0 = s) = '\0';
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
        if (s)
          *(fontname0 = s++) = '\0';
        arg = s;
      } else if (!strncmp(arg, "fontsize=", 9)) {
        /* parse fontsize */
        int n1 = atoi(arg += 9), n2 = 0;
        byte ch;
        if (n1 > 0) {
          while ((ch = (byte) * ++arg) && ch != ',') {
            /* skip fontsize digits */
            if (ch == 'x') {
              /* parse fontheight */
              n2 = atoi(arg + 1);
              while ((ch = (byte) * ++arg) && ch != ',') {
                /* skip fontheight digits */
              }
              break;
            }
          }
          fontwidth = Min2(TW_MAXUDAT, n2 > 0 ? n1 : n1 / 2);
          fontheight = Min2(TW_MAXUDAT, n2 > 0 ? n2 : n1);
        }
      } else if (!strncmp(arg, "charset=", 8)) {
        charset = arg += 8;
        s = strchr(arg, ',');
        if (s)
          *(charset0 = s++) = '\0';
        arg = s;
      } else if (!strncmp(arg, "view=", 5)) {
        this->xhw_view = 1;
        this->xhw_endx = strtol(arg + 5, &arg, 0);
        this->xhw_endy = strtol(arg + 1, &arg, 0);
        this->xhw_startx = strtol(arg + 1, &arg, 0);
        this->xhw_starty = strtol(arg + 1, &arg, 0);
        this->xhw_endx += this->xhw_startx;
        this->xhw_endy += this->xhw_starty;
      } else if (!strncmp(arg, "drag", 4)) {
        arg += 4;
        drag = true;
      } else if (!strncmp(arg, "noinput", 7)) {
        arg += 7;
        noinput = true;
      } else if (!strncmp(arg, "palette", 7)) {
        arg += 7;
        palette = true;
      } else {
        log(INFO)
            << "   --hw=X11 and --hw=xft options:\n"
               "      @DPY             connect to DPY instead of $DISPLAY (must be first option)\n"
               "      ,charset=NAME    use specified charset encoding\n"
               "      ,drag            use XCopyArea() for scroll/drag when possible\n"
               "      ,font=FONT_NAME  use specified font name or pattern\n"
               "      ,fontsize=WxH    use specified font size\n"
               "      ,help            show this help\n"
               "      ,noinput         open a view-only X11 window - ignore input\n"
               "      ,palette         use 256 color palette, disabling truecolor support\n"
               "      ,view=WxH+X+Y    only show a subarea of twin's desktop\n";
        goto fail;
      }
    }
  }

  this->xsfont = NULL;
  hints = NULL;
  this->xwindow = None;
  this->xgc = None;
  this->xReqCount = this->XReqCount = 0;
  hw->keyboard_slot = NOSLOT;

  if ((this->xdisplay = XOpenDisplay(display))) {
    do {
      (void)XSetIOErrorHandler(&XDRIVER::Die);

      const int screen = DefaultScreen(this->xdisplay);
      const unsigned depth = DefaultDepth(this->xdisplay, screen);
      Visual *visual = DefaultVisual(this->xdisplay, screen);

      if (!palette) {
        XVisualInfo vistemplate;
        XVisualInfo *visinfo = NULL;
        int visinfo_n = 0;
        vistemplate.visualid = XVisualIDFromVisual(visual);
        visinfo = XGetVisualInfo(this->xdisplay, VisualIDMask, &vistemplate, &visinfo_n);

        if (visinfo != NULL && visinfo_n > 0) {
          this->xrgb_info.init(visinfo);
          this->xtruecolor = this->xrgb_info.is_truecolor(visinfo);
        }
        if (visinfo != NULL) {
          XFree(visinfo);
          visinfo = NULL;
        }
      }
      Colormap colormap = DefaultColormap(this->xdisplay, screen);

      if (!this->xtruecolor) {
        XColor color;
        for (i = 0; i < tpalette_n; i++) {
          color.red = 257 * (udat)TRED(Palette[i]);
          color.green = 257 * (udat)TGREEN(Palette[i]);
          color.blue = 257 * (udat)TBLUE(Palette[i]);
          if (!AllocColor(visual, colormap, &color, &this->xpalette[i], i)) {
            log(ERROR) << "      " XSTR(XDRIVER) ".InitHW() failed to allocate colors\n";
            break;
          }
        }
        if (i < tpalette_n) {
          break;
        }
      }

      attr.background_pixel = this->xpalette[0];
      attr.event_mask = ExposureMask | VisibilityChangeMask | StructureNotifyMask |
                        SubstructureNotifyMask | KeyPressMask | ButtonPressMask |
                        ButtonReleaseMask | PointerMotionMask;

      if (!LoadFont(fontname, fontwidth, fontheight)) {
        break;
      }

      if (this->xhw_view && this->xhw_startx >= 0 && this->xhw_starty >= 0 &&
          this->xhw_endx > this->xhw_startx && this->xhw_endy > this->xhw_starty) {
        /* a valid view was specified */

        this->xwidth = this->xwfont * (ldat)(this->xhw_endx - this->xhw_startx);
        this->xheight = this->xhfont * (ldat)(this->xhw_endy - this->xhw_starty);
      } else {
        this->xhw_view = this->xhw_startx = this->xhw_starty = 0;
        this->xhw_endx = hw->X;
        this->xhw_endy = hw->Y;
      }

      if ((this->xwindow = XCreateWindow(this->xdisplay, DefaultRootWindow(this->xdisplay), 0, 0,
                                         this->xwidth, this->xheight, 0, depth, InputOutput, visual,
                                         CWBackPixel | CWEventMask, &attr)) &&

          (this->xrgb_fg = this->xrgb_bg = tblack,                            /**/
           this->xsgc.foreground = this->xsgc.background = this->xpalette[0], /**/
           this->xsgc.graphics_exposures = False,
#if HW_X_DRIVER == HW_X11
           this->xsgc.font = this->xsfont->fid, creategc_mask = creategc_mask | GCFont,
#endif
           this->xgc = XCreateGC(this->xdisplay, this->xwindow, creategc_mask, &this->xsgc)) &&

          (hints = XAllocSizeHints())) {

        static XComposeStatus static_xcompose;
        this->xcompose = static_xcompose;

#if HW_X_DRIVER == HW_XFT
        if (this->xtruecolor) {
          this->xtfg = &this->foreground_buf;
          this->xtbg = &this->background_buf;
          /* force xtfg and xtbg initialization */
          this->xrgb_fg = this->xrgb_bg = tWHITE;
          SetColors(TCOL0);
        } else {
          this->xtfg = this->xtbg = this->xtpalette[0];
        }
        this->xtdraw = XftDrawCreate(this->xdisplay, this->xwindow, visual, colormap);
#endif

#ifdef TW_FEATURE_X11_XIM_XIC
        this->xim = XOpenIM(this->xdisplay, NULL, NULL, NULL);
        if (this->xim != NULL) {
          this->xic = XCreateIC(this->xim, XNInputStyle, XIMStatusNothing | XIMPreeditNothing,
                                XNClientWindow, this->xwindow, XNFocusWindow, this->xwindow, NULL);
          if (this->xic == NULL) {
            XCloseIM(this->xim);
            this->xim = NULL;
          }
        } else {
          this->xic = NULL;
        }
#endif
        FillWindowTitle(title, sizeof(title));
        XStoreName(this->xdisplay, this->xwindow, title);

        if (!(this->xUTF_32_to_charset = UTF_32_to_charset_function(charset))) {
          this->xUTF_32_to_charset = &XDRIVER::UTF_32_to_UCS_2;
        }
        /*
         * ask ICCCM-compliant window manager to tell us when close window
         * has been chosen, rather than just killing us
         */
        this->xCOMPOUND_TEXT = XInternAtom(this->xdisplay, "COMPOUND_TEXT", False);
        this->xTARGETS = XInternAtom(this->xdisplay, "TARGETS", False);
        this->xTEXT = XInternAtom(this->xdisplay, "TEXT", False);
        this->xUTF8_STRING = XInternAtom(this->xdisplay, "UTF8_STRING", False);
        this->xWM_DELETE_WINDOW = XInternAtom(this->xdisplay, "WM_DELETE_WINDOW", False);
        this->xWM_PROTOCOLS = XInternAtom(this->xdisplay, "WM_PROTOCOLS", False);

        XChangeProperty(this->xdisplay, this->xwindow, this->xWM_PROTOCOLS, XA_ATOM, 32,
                        PropModeReplace, (unsigned char *)&this->xWM_DELETE_WINDOW, 1);

        if (this->xhw_view) {
          hints->flags = PMinSize | PMaxSize;
          hints->min_width = hints->max_width = this->xwidth;
          hints->min_height = hints->max_height = this->xheight;
        } else {
          hints->flags = PResizeInc;
          hints->width_inc = this->xwfont;
          hints->height_inc = this->xhfont;
        }
        XSetWMNormalHints(this->xdisplay, this->xwindow, hints);

        XMapWindow(this->xdisplay, this->xwindow);

        do {
          XNextEvent(this->xdisplay, &event);
        } while (event.type != MapNotify);

        XFree(hints);
        hints = NULL;

        hw->mouse_slot = NOSLOT;
        hw->keyboard_slot = RegisterRemote(i = XConnectionNumber(this->xdisplay), (Tobj)hw,
                                           (void (*)(int, Tobj))&XDRIVER::KeyboardEvent);
        if (hw->keyboard_slot == NOSLOT)
          break;
        fcntl(i, F_SETFD, FD_CLOEXEC);

        hw->fnFlushVideo = &XDRIVER::FlushVideo;
        hw->fnFlushHW = &XDRIVER::FlushHW;

        hw->fnKeyboardEvent = &XDRIVER::KeyboardEvent;
        /* mouse events handled by XSYM(KeyboardEvent) */
        hw->fnMouseEvent = NULL;

        hw->XY[0] = hw->XY[1] = 0;
        hw->TT = NOCURSOR;

        hw->fnShowMouse = NULL;
        hw->fnHideMouse = NULL;
        hw->fnUpdateMouseAndCursor = NULL;
        hw->MouseState.x = hw->MouseState.y = hw->MouseState.keys = 0;

        hw->fnDetectSize = &XDRIVER::DetectSize;
        hw->fnCheckResize = &XDRIVER::CheckResize;
        hw->fnResize = &XDRIVER::Resize;

        hw->fnSelectionImport = &XDRIVER::SelectionImport_X11;
        hw->fnSelectionExport = &XDRIVER::SelectionExport_X11;
        hw->fnSelectionRequest = &XDRIVER::SelectionRequest_X11;
        hw->fnSelectionNotify = &XDRIVER::SelectionNotify_X11;
        hw->SelectionPrivate = 0;

        if (drag) {
          hw->fnCanDragArea = &XDRIVER::CanDragArea;
          hw->fnDragArea = &XDRIVER::DragArea;
        } else
          hw->fnCanDragArea = NULL;

        hw->fnBeep = &XDRIVER::Beep;
        hw->fnConfigure = &XDRIVER::Configure;
        hw->fnSetPalette = (void (*)(Tdisplay, udat, udat, udat, udat))NoOp;
        hw->fnResetPalette = NULL;

        hw->fnQuitHW = &XDRIVER::QuitHW;
        hw->fnQuitKeyboard = NULL;
        hw->fnQuitMouse = NULL;
        hw->fnQuitVideo = NULL;

        hw->DisplayIsCTTY = tfalse;
        hw->FlagsHW &= ~FlHWSoftMouse; /* mouse pointer handled by X11 server */

        hw->FlagsHW |= FlHWNeedOldVideo;
        hw->FlagsHW |= FlHWExpensiveFlushVideo;
        if (noinput)
          hw->FlagsHW |= FlHWNoInput;

        hw->NeedHW = 0;
        hw->CanResize = ttrue;
        hw->merge_Threshold = 0;

        /*
         * we must draw everything on our new shiny window
         * without forcing all other displays
         * to redraw everything too.
         */
        hw->RedrawVideo = tfalse;
        NeedRedrawVideo(hw, 0, 0, hw->X - 1, hw->Y - 1);

        if (display0) {
          *display0 = ',';
        }
        if (fontname0) {
          *fontname0 = ',';
        }
        if (charset0) {
          *charset0 = ',';
        }
        InitKeys();

        return true;
      }
    } while (0);
  } else {
    if (display || (display = getenv("DISPLAY"))) {
      log(ERROR) << "      " XSTR(XDRIVER) ".InitHW() failed to open display " << hw->Name << "\n";
    } else {
      log(ERROR) << "      " XSTR(XDRIVER) ".InitHW() failed: DISPLAY is not set\n";
    }
  }

fail:
  if (display0) {
    *display0 = ',';
  }
  if (fontname0) {
    *fontname0 = ',';
  }
  if (charset0) {
    *charset0 = ',';
  }
  return false;
}

void XDRIVER::QuitHW(Tdisplay hw) {
  XDRIVER *self = xdriver(hw);
  if (self) {
    self->QuitHW();

    FreeMem(hw->Private);
    hw->Private = NULL;
  }
}

void XDRIVER::QuitHW() {
#ifdef TW_FEATURE_X11_XIM_XIC
  if (this->xic) {
    XDestroyIC(this->xic);
  }
  if (this->xim) {
    XCloseIM(this->xim);
  }
#endif

#if HW_X_DRIVER == HW_X11
  if (this->xsfont) {
    XFreeFont(this->xdisplay, this->xsfont);
    this->xsfont = NULL;
  }
#elif HW_X_DRIVER == HW_XFT
  Visual *visual = NULL;
  Colormap colormap = (Colormap)0;
  int screen = 0;

  if (this->xdisplay) {
    if (this->xsfont) {
      XftFontClose(this->xdisplay, this->xsfont);
      this->xsfont = NULL;
    }
    screen = DefaultScreen(this->xdisplay);
    colormap = DefaultColormap(this->xdisplay, screen);
    visual = DefaultVisual(this->xdisplay, screen);
  }
  if (this->xtdraw) {
    XftDrawDestroy(this->xtdraw);
    this->xtdraw = NULL;
  }
  for (int i = 0; i < tpalette_n; i++) {
    if (this->xtpalette[i] == NULL) {
      break;
    }
    if (this->xdisplay) {
      XftColorFree(this->xdisplay, visual, colormap, this->xtpalette[i]);
    }
    FreeMem(this->xtpalette[i]);
    this->xtpalette[i] = NULL;
  }
#endif

  if (this->xgc != None) {
    XFreeGC(this->xdisplay, this->xgc);
  }
  if (this->xwindow != None) {
    XUnmapWindow(this->xdisplay, this->xwindow);
    XDestroyWindow(this->xdisplay, this->xwindow);
  }
  XCloseDisplay(this->xdisplay);
  this->xdisplay = NULL;

  if (hw->keyboard_slot != NOSLOT)
    UnRegisterRemote(hw->keyboard_slot);
  hw->keyboard_slot = NOSLOT;
  hw->fnKeyboardEvent = NULL;

  hw->fnQuitHW = NULL;
}

EXTERN_C byte InitModule(Tmodule mod) {
  mod->DoInit = &XDRIVER::InitHW;
  return ttrue;
}

/* this MUST be defined, otherwise it seems to trigger a bug in dlsym() */
EXTERN_C void QuitModule(Tmodule mod) {
}
