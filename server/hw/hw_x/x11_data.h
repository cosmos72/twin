/* State data for the various hw modes that use X11. */

class XSYM(color_info) {
public:
  byte width, shift;

  XSYM(color_info)() : width(0), shift(0) {
  }

  void init(unsigned long mask) {
    width = 0;
    shift = 0;
    while (mask != 0 && ((mask & 1) == 0)) {
      mask >>= 1;
      shift++;
    }
    while (mask & 1) {
      mask >>= 1;
      width++;
    }
  }

  unsigned long pixel(byte intensity) const {
    if (width == 8) {
      return (unsigned long)intensity << shift;
    } else if (width > 8) {
      return (unsigned long)intensity << (width - 8 + shift);
    } else {
      return (unsigned long)(intensity >> (8 - width)) << shift;
    }
  }
};

class XSYM(rgb_info) {
public:
  XSYM(color_info) red, green, blue;

  XSYM(rgb_info)() : red(), green(), blue() {
  }

  void init(const XVisualInfo *visinfo) {
    red.init(visinfo->red_mask);
    green.init(visinfo->green_mask);
    blue.init(visinfo->blue_mask);
  }

  unsigned long pixel(trgb rgb) const {
    return red.pixel(TRED(rgb)) | green.pixel(TGREEN(rgb)) | blue.pixel(TBLUE(rgb));
  }

  bool is_truecolor(const XVisualInfo *visinfo) const {
    unsigned bits_per_component = (unsigned)visinfo->bits_per_rgb;

    return (visinfo->c_class == TrueColor || visinfo->c_class == DirectColor) /**/
           && red.width != 0 && red.width <= bits_per_component               /**/
           && green.width != 0 && green.width <= bits_per_component           /**/
           && blue.width != 0 && blue.width <= bits_per_component             /**/
           && (visinfo->red_mask & visinfo->green_mask) == 0                  /**/
           && (visinfo->green_mask & visinfo->blue_mask) == 0                 /**/
           && (visinfo->blue_mask & visinfo->red_mask) == 0;                  /**/
  }
};

#define NEST 4
struct XSYM(data) {
  int xwidth, xheight;
  int xwfont, xhfont, xupfont;
  /* we support showing only a portion of the whole twin display */
  dat xhw_view, xhw_startx, xhw_starty, xhw_endx, xhw_endy;
  byte xnumkeypad; /* ttrue if numeric keypad emits numeric digits */
  byte xtruecolor;

  Tutf_function xUTF_32_to_charset;
  Display *xdisplay;
  Window xwindow;
  GC xgc;
  XGCValues xsgc;
  XSYM(rgb_info) xrgb_info;
#if HW_X_DRIVER == HW_X11
  XFontStruct *xsfont;
#elif HW_X_DRIVER == HW_XFT
  XftFont *xsfont;
  XftDraw *xftdraw;
  XftColor *foreground; // current foreground color
  XftColor *background; // current background color
  XftColor *xftcolors[tpalette_n];
#endif
  trgb xforeground_rgb;
  trgb xbackground_rgb;
#ifdef TW_FEATURE_X11_XIM_XIC /* autodetected */
  XIM xim;
  XIC xic;
#endif
  XComposeStatus xcompose;
  byte xwindow_AllVisible, xfont_map;
  Tobj xRequestor[NEST];
  uldat xReqPrivate[NEST];
  uldat xReqCount;
  uldat XReqCount;
  XSelectionRequestEvent XReq[NEST];
  unsigned long xcol[tpalette_n];
  Atom xCOMPOUND_TEXT, xTARGETS, xTEXT, xUTF8_STRING, xWM_DELETE_WINDOW, xWM_PROTOCOLS;
};

#define xdata ((struct XSYM(data) *)HW->Private)
#define xwidth (xdata->xwidth)
#define xheight (xdata->xheight)
#define xwfont (xdata->xwfont)
#define xhfont (xdata->xhfont)
#define xupfont (xdata->xupfont)

#define xhw_view (xdata->xhw_view)
#define xhw_startx (xdata->xhw_startx)
#define xhw_starty (xdata->xhw_starty)
#define xhw_endx (xdata->xhw_endx)
#define xhw_endy (xdata->xhw_endy)
#define xnumkeypad (xdata->xnumkeypad)
#define xtruecolor (xdata->xtruecolor)

#define xUTF_32_to_charset (xdata->xUTF_32_to_charset)
#define xdisplay (xdata->xdisplay)
#define xwindow (xdata->xwindow)
#define xgc (xdata->xgc)
#define xsgc (xdata->xsgc)
#define xsfont (xdata->xsfont)
#define xforeground_rgb (xdata->xforeground_rgb)
#define xbackground_rgb (xdata->xbackground_rgb)
#define xim (xdata->xim)
#define xic (xdata->xic)
#define xcompose (xdata->xcompose)

#define xrgb_info (xdata->xrgb_info)

#define xwindow_AllVisible (xdata->xwindow_AllVisible)
#define xfont_map (xdata->xfont_map)
#define xRequestor(j) (xdata->xRequestor[j])
#define xReqPrivate(j) (xdata->xReqPrivate[j])
#define xReqCount (xdata->xReqCount)
#define XReqCount (xdata->XReqCount)
#define XReq(j) (xdata->XReq[j])
#define xcol (xdata->xcol)
#define xCOMPOUND_TEXT (xdata->xCOMPOUND_TEXT)
#define xTARGETS (xdata->xTARGETS)
#define xTEXT (xdata->xTEXT)
#define xUTF8_STRING (xdata->xUTF8_STRING)
#define xWM_PROTOCOLS (xdata->xWM_PROTOCOLS)
#define xWM_DELETE_WINDOW (xdata->xWM_DELETE_WINDOW)

#if HW_X_DRIVER == HW_XFT
#define xftdraw (xdata->xftdraw)
#define xforeground (xdata->foreground)
#define xbackground (xdata->background)
#define xftcolors (xdata->xftcolors)
#endif
