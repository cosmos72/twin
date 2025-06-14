/* Functions that are common between hw_x11 and hw_xft. */

#include "algo.h"
#include "log.h"
#include "palette.h"

/* this can stay static, XSYM(FlushHW)() is not reentrant */
static tcolor _col;

static unsigned long XSYM(ColorToPixel)(trgb rgb) {
  if (xtruecolor) {
    return xrgb_info.pixel(rgb);
  }
  return xcol[TrueColorToPalette256(rgb)];
}

inline void XSYM(DrawSome)(dat x, dat y, ldat len) {
  tcell *V, *oV;
  tcolor col;
  udat buflen = 0;
  trune f;
  XChar16 buf[TW_SMALLBUFF];
  int xbegin, ybegin;

  if (len <= 0) {
    return;
  }
  if (xhw_view) {
    if (x >= xhw_endx || x + len < xhw_startx || y < xhw_starty || y >= xhw_endy)
      return;

    if (x + len >= xhw_endx)
      len = xhw_endx - x;

    if (x < xhw_startx) {
      len -= xhw_startx - x;
      x = xhw_startx;
    }
  }

  xbegin = (x - xhw_startx) * (ldat)xwfont;
  ybegin = (y - xhw_starty) * (ldat)xhfont;

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
        xbegin = (x - xhw_startx) * (ldat)xwfont;
        _col = col;
      }
      f = xUTF_32_to_charset(TRUNE(*V));
      buf[buflen++] = RawToXChar16(f);
    }
  }
  if (buflen) {
    XDRAW(_col, buf, buflen);
    buflen = 0;
  }
}

enum { MAX_FONT_SCORE = 100 };

static ldat XSYM(FontScoreOf)(udat fontwidth, udat fontheight, ldat width, ldat height) {
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

static byte XSYM(LoadFont)(const char *fontname, udat fontwidth, udat fontheight) {
  char *alloc_fontname = NULL;
  byte loaded = tfalse;

  fontname = alloc_fontname = XSYM(AutodetectFont)(fontname, fontwidth, fontheight);
#if HW_X_DRIVER == HW_X11

  if ((fontname && (xsfont = XLoadQueryFont(xdisplay, fontname))) ||
      (xsfont = XLoadQueryFont(xdisplay, fontname = "fixed")))
#elif HW_X_DRIVER == HW_XFT
  if (fontname && (xsfont = XftFontOpenName(xdisplay, DefaultScreen(xdisplay), fontname)))
#endif
  {
    loaded = ttrue;

#if HW_X_DRIVER == HW_X11
    xwfont = xsfont->min_bounds.width;
#elif HW_X_DRIVER == HW_XFT
    xwfont = xsfont->max_advance_width;
#endif
    xwidth = xwfont * (unsigned)(HW->X = GetDisplayWidth());
    xhfont = (xupfont = xsfont->ascent) + xsfont->descent;
    xheight = xhfont * (unsigned)(HW->Y = GetDisplayHeight());

    log(INFO) << "      selected " << xwfont << "x" << xhfont << " font `"
              << Chars::from_c(fontname) << "'\n";
  }
  if (alloc_fontname)
    free(alloc_fontname); /* allocated by FcNameUnparse() */

  return loaded;
}

static void XSYM(QuitHW)(void) {
#ifdef TW_FEATURE_X11_XIM_XIC
  if (xic)
    XDestroyIC(xic);
  if (xim)
    XCloseIM(xim);
#endif
  XSYM(FlavorQuitHW)();
  if (xgc != None)
    XFreeGC(xdisplay, xgc);
  if (xwindow != None) {
    XUnmapWindow(xdisplay, xwindow);
    XDestroyWindow(xdisplay, xwindow);
  }
  XCloseDisplay(xdisplay);
  xdisplay = NULL;

  if (HW->keyboard_slot != NOSLOT)
    UnRegisterRemote(HW->keyboard_slot);
  HW->keyboard_slot = NOSLOT;
  HW->KeyboardEvent = (void (*)(int, Tdisplay))NoOp;

  HW->QuitHW = NoOp;

  FreeMem(HW->Private);
  HW->Private = NULL;
}

static int XSYM(check_hw_name)(char *hw_name) {
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

static bool XSYM(InitHW)(void) {
  char *arg = HW->Name.data(); // guaranteed to be '\0' terminated

  XSetWindowAttributes xattr;
  XColor xcolor;
  XSizeHints *xhints;
  XEvent event;
  char *s, *xdisplay_ = NULL, *xdisplay0 = NULL, *fontname = NULL, *fontname0 = NULL,
           *charset = NULL, *charset0 = NULL, title[X_TITLE_MAXLEN];
  unsigned long xcreategc_mask = GCForeground | GCBackground | GCGraphicsExposures;
  int i, nskip = 0;
  udat fontwidth = 10, fontheight = 20;
  bool drag = false, noinput = false, palette = false;

  if (!(HW->Private = (XSYM(data) *)AllocMem(sizeof(XSYM(data))))) {
    log(ERROR) << "      " XSYM_STR(InitHW) "() Out of memory!\n";
    return false;
  }
  memset(HW->Private, 0, sizeof(XSYM(data)));

  /* default: show the whole screen */
  xhw_view = xhw_startx = xhw_starty = xhw_endx = xhw_endy = 0;

  /* not yet opened */
  xdisplay = NULL;

  if (arg && *arg && ((nskip = XSYM(check_hw_name)(arg)) >= 0)) {
    arg += nskip;

    if (*arg == '@') {
      if ((s = strchr(xdisplay_ = ++arg, ','))) {
        *(xdisplay0 = s) = '\0';
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
        xhw_view = 1;
        xhw_endx = strtol(arg + 5, &arg, 0);
        xhw_endy = strtol(arg + 1, &arg, 0);
        xhw_startx = strtol(arg + 1, &arg, 0);
        xhw_starty = strtol(arg + 1, &arg, 0);
        xhw_endx += xhw_startx;
        xhw_endy += xhw_starty;
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

  xsfont = NULL;
  xhints = NULL;
  xwindow = None;
  xgc = None;
  xReqCount = XReqCount = 0;
  HW->keyboard_slot = NOSLOT;

  if ((xdisplay = XOpenDisplay(xdisplay_))) {
    do {
      (void)XSetIOErrorHandler(XSYM(Die));

      const int screen = DefaultScreen(xdisplay);
      const unsigned depth = DefaultDepth(xdisplay, screen);
      Visual *visual = DefaultVisual(xdisplay, screen);

      if (!palette) {
        XVisualInfo vistemplate;
        XVisualInfo *visinfo = NULL;
        int visinfo_n = 0;
        vistemplate.visualid = XVisualIDFromVisual(visual);
        visinfo = XGetVisualInfo(xdisplay, VisualIDMask, &vistemplate, &visinfo_n);

        if (visinfo != NULL && visinfo_n > 0) {
          xrgb_info.init(visinfo);
          xtruecolor = xrgb_info.is_truecolor(visinfo);
        }
        if (visinfo != NULL) {
          XFree(visinfo);
          visinfo = NULL;
        }
      }
      Colormap colormap = DefaultColormap(xdisplay, screen);

      if (!xtruecolor) {
        for (i = 0; i < tpalette_n; i++) {
          xcolor.red = 257 * (udat)TRED(Palette[i]);
          xcolor.green = 257 * (udat)TGREEN(Palette[i]);
          xcolor.blue = 257 * (udat)TBLUE(Palette[i]);
          if (!XSYM(AllocColor)(xdisplay, visual, colormap, &xcolor, &xcol[i], i)) {
            log(ERROR) << "      " XSYM_STR(InitHW) "() failed to allocate colors\n";
            break;
          }
        }
        if (i < tpalette_n) {
          break;
        }
      }

      xattr.background_pixel = xcol[0];
      xattr.event_mask = ExposureMask | VisibilityChangeMask | StructureNotifyMask |
                         SubstructureNotifyMask | KeyPressMask | ButtonPressMask |
                         ButtonReleaseMask | PointerMotionMask;

      if (!XSYM(LoadFont)(fontname, fontwidth, fontheight)) {
        break;
      }

      if (xhw_view && xhw_startx >= 0 && xhw_starty >= 0 && xhw_endx > xhw_startx &&
          xhw_endy > xhw_starty) {
        /* a valid view was specified */

        xwidth = xwfont * (ldat)(xhw_endx - xhw_startx);
        xheight = xhfont * (ldat)(xhw_endy - xhw_starty);
      } else {
        xhw_view = xhw_startx = xhw_starty = 0;
        xhw_endx = HW->X;
        xhw_endy = HW->Y;
      }

      if ((xwindow =
               XCreateWindow(xdisplay, DefaultRootWindow(xdisplay), 0, 0, xwidth, xheight, 0, depth,
                             InputOutput, visual, CWBackPixel | CWEventMask, &xattr)) &&

          (xforeground_rgb = xbackground_rgb = tblack,  /**/
           xsgc.foreground = xsgc.background = xcol[0], /**/
           xsgc.graphics_exposures = False,
#if HW_X_DRIVER == HW_X11
           xsgc.font = xsfont->fid, xcreategc_mask = xcreategc_mask | GCFont,
#elif HW_X_DRIVER == HW_XFT
           xforeground = xbackground = xftcolors[0],
#endif
           xgc = XCreateGC(xdisplay, xwindow, xcreategc_mask, &xsgc)) &&

          (xhints = XAllocSizeHints())) {

        static XComposeStatus static_xcompose;
        xcompose = static_xcompose;

#if HW_X_DRIVER == HW_XFT
        xftdraw = XftDrawCreate(xdisplay, xwindow, visual, colormap);
#endif

#ifdef TW_FEATURE_X11_XIM_XIC
        xim = XOpenIM(xdisplay, NULL, NULL, NULL);
        if (xim != NULL) {
          xic = XCreateIC(xim, XNInputStyle, XIMStatusNothing | XIMPreeditNothing, XNClientWindow,
                          xwindow, XNFocusWindow, xwindow, NULL);
          if (xic == NULL) {
            XCloseIM(xim);
            xim = NULL;
          }
        } else {
          xic = NULL;
        }
#endif
        XSYM(FillWindowTitle)(title, sizeof(title));
        XStoreName(xdisplay, xwindow, title);

        if (!(xUTF_32_to_charset = XSYM(UTF_32_to_charset_function)(charset)))
          xUTF_32_to_charset = XSYM(UTF_32_to_UCS_2);
        /*
         * ask ICCCM-compliant window manager to tell us when close window
         * has been chosen, rather than just killing us
         */
        xCOMPOUND_TEXT = XInternAtom(xdisplay, "COMPOUND_TEXT", False);
        xTARGETS = XInternAtom(xdisplay, "TARGETS", False);
        xTEXT = XInternAtom(xdisplay, "TEXT", False);
        xUTF8_STRING = XInternAtom(xdisplay, "UTF8_STRING", False);
        xWM_DELETE_WINDOW = XInternAtom(xdisplay, "WM_DELETE_WINDOW", False);
        xWM_PROTOCOLS = XInternAtom(xdisplay, "WM_PROTOCOLS", False);

        XChangeProperty(xdisplay, xwindow, xWM_PROTOCOLS, XA_ATOM, 32, PropModeReplace,
                        (unsigned char *)&xWM_DELETE_WINDOW, 1);

        if (xhw_view) {
          xhints->flags = PMinSize | PMaxSize;
          xhints->min_width = xhints->max_width = xwidth;
          xhints->min_height = xhints->max_height = xheight;
        } else {
          xhints->flags = PResizeInc;
          xhints->width_inc = xwfont;
          xhints->height_inc = xhfont;
        }
        XSetWMNormalHints(xdisplay, xwindow, xhints);

        XMapWindow(xdisplay, xwindow);

        do {
          XNextEvent(xdisplay, &event);
        } while (event.type != MapNotify);

        XFree(xhints);
        xhints = NULL;

        HW->mouse_slot = NOSLOT;
        HW->keyboard_slot = RegisterRemote(i = XConnectionNumber(xdisplay), (Tobj)HW,
                                           (void (*)(int, Tobj))XSYM(KeyboardEvent));
        if (HW->keyboard_slot == NOSLOT)
          break;
        fcntl(i, F_SETFD, FD_CLOEXEC);

        HW->FlushVideo = XSYM(FlushVideo);
        HW->FlushHW = XSYM(FlushHW);

        HW->KeyboardEvent = XSYM(KeyboardEvent);
        /* mouse events handled by XSYM(KeyboardEvent) */
        HW->MouseEvent = (void (*)(int, Tdisplay))NoOp;

        HW->XY[0] = HW->XY[1] = 0;
        HW->TT = NOCURSOR;

        HW->ShowMouse = NoOp;
        HW->HideMouse = NoOp;
        HW->UpdateMouseAndCursor = NoOp;
        HW->MouseState.x = HW->MouseState.y = HW->MouseState.keys = 0;

        HW->DetectSize = XSYM(DetectSize);
        HW->CheckResize = XSYM(CheckResize);
        HW->Resize = XSYM(Resize);

        HW->HWSelectionImport = XSYM(SelectionImport_X11);
        HW->HWSelectionExport = XSYM(SelectionExport_X11);
        HW->HWSelectionRequest = XSYM(SelectionRequest_X11);
        HW->HWSelectionNotify = XSYM(SelectionNotify_X11);
        HW->HWSelectionPrivate = 0;

        if (drag) {
          HW->CanDragArea = XSYM(CanDragArea);
          HW->DragArea = XSYM(DragArea);
        } else
          HW->CanDragArea = NULL;

        HW->Beep = XSYM(Beep);
        HW->Configure = XSYM(Configure);
        HW->SetPalette = (void (*)(udat, udat, udat, udat))NoOp;
        HW->ResetPalette = NoOp;

        HW->QuitHW = XSYM(QuitHW);
        HW->QuitKeyboard = NoOp;
        HW->QuitMouse = NoOp;
        HW->QuitVideo = NoOp;

        HW->DisplayIsCTTY = tfalse;
        HW->FlagsHW &= ~FlHWSoftMouse; /* mouse pointer handled by X11 server */

        HW->FlagsHW |= FlHWNeedOldVideo;
        HW->FlagsHW |= FlHWExpensiveFlushVideo;
        if (noinput)
          HW->FlagsHW |= FlHWNoInput;

        HW->NeedHW = 0;
        HW->CanResize = ttrue;
        HW->merge_Threshold = 0;

        /*
         * we must draw everything on our new shiny window
         * without forcing all other displays
         * to redraw everything too.
         */
        HW->RedrawVideo = tfalse;
        NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);

        if (xdisplay0)
          *xdisplay0 = ',';
        if (fontname0)
          *fontname0 = ',';
        if (charset0)
          *charset0 = ',';

        XSYM(init_keys)();

        return true;
      }
    } while (0);
  } else {
    if (xdisplay_ || (xdisplay_ = getenv("DISPLAY"))) {
      log(ERROR) << "      " XSYM_STR(InitHW) "() failed to open display " << HW->Name << "\n";
    } else {
      log(ERROR) << "      " XSYM_STR(InitHW) "() failed: DISPLAY is not set\n";
    }
  }

fail:
  if (xdisplay0)
    *xdisplay0 = ',';
  if (fontname0)
    *fontname0 = ',';
  if (charset0)
    *charset0 = ',';

  if (xdisplay) {
    XSYM(QuitHW)();
  }
  FreeMem(HW->Private);
  HW->Private = NULL;

  return false;
}

EXTERN_C byte InitModule(Tmodule mod) {
  mod->DoInit = XSYM(InitHW);
  return ttrue;
}

/* this MUST be defined, otherwise it seems to trigger a bug in dlsym() */
EXTERN_C void QuitModule(Tmodule mod) {
}
