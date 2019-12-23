/* State data for the various hw modes that use X11. */

#define NEST 4
struct x11_data {
  int xwidth, xheight;
  int xwfont, xhfont, xupfont;

  /* we support showing only a portion of the whole twin display */
  dat xhw_view, xhw_startx, xhw_starty, xhw_endx, xhw_endy;

  Tutf_function xUTF_32_to_charset;
  Display *xdisplay;
  Window xwindow;
  GC xgc;
  XGCValues xsgc;
#if HW_X_DRIVER == HW_X11
  XFontStruct *xsfont;
#elif HW_X_DRIVER == HW_GFX
  XFontStruct *xsfont;
  int xmonochrome;
  Pixmap xtheme, xroot, xbg;
  GC xthemegc, xrootgc, xbggc;
  XGCValues xthemesgc;
  byte xroot_flag, xbg_flag;
#elif HW_X_DRIVER == HW_XFT
  XftFont *xsfont;
  XftDraw *xftdraw;
  XftColor *foreground; // current foreground color
  XftColor *background; // current background color
  XftColor *xftcolors[MAXCOL + 1];
#endif
#ifdef TW_FEATURE_X11_XIM_XIC /* autodetected */
  XIM xim;
  XIC xic;
#endif
  XComposeStatus xcompose;
  byte xwindow_AllVisible, xfont_map;
  obj xRequestor[NEST];
  uldat xReqPrivate[NEST];
  uldat xReqCount;
  uldat XReqCount;
  XSelectionRequestEvent XReq[NEST];
  unsigned long xcol[MAXCOL + 1];
  Atom xWM_PROTOCOLS, xWM_DELETE_WINDOW, xTARGETS;
};

#define xdata ((struct x11_data *)HW->Private)
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

#define xUTF_32_to_charset (xdata->xUTF_32_to_charset)
#define xdisplay (xdata->xdisplay)
#define xwindow (xdata->xwindow)
#define xgc (xdata->xgc)
#define xsgc (xdata->xsgc)
#define xsfont (xdata->xsfont)
#define xim (xdata->xim)
#define xic (xdata->xic)
#define xcompose (xdata->xcompose)

#define xwindow_AllVisible (xdata->xwindow_AllVisible)
#define xfont_map (xdata->xfont_map)
#define xRequestor(j) (xdata->xRequestor[j])
#define xReqPrivate(j) (xdata->xReqPrivate[j])
#define xReqCount (xdata->xReqCount)
#define XReqCount (xdata->XReqCount)
#define XReq(j) (xdata->XReq[j])
#define xcol (xdata->xcol)
#define xWM_PROTOCOLS (xdata->xWM_PROTOCOLS)
#define xWM_DELETE_WINDOW (xdata->xWM_DELETE_WINDOW)
#define xTARGETS (xdata->xTARGETS)

#if HW_X_DRIVER == HW_GFX
#define xmonochrome (xdata->xmonochrome)
#define xtheme (xdata->xtheme)
#define xthemegc (xdata->xthemegc)
#define xthemesgc (xdata->xthemesgc)
#define xroot_flag (xdata->xroot_flag)
#define xroot (xdata->xroot)
#define xrootgc (xdata->xrootgc)
#define xbg_flag (xdata->xbg_flag)
#define xbg (xdata->xbg)
#define xbggc (xdata->xbggc)
#elif HW_X_DRIVER == HW_XFT
#define xftdraw (xdata->xftdraw)
#define xforeground (xdata->foreground)
#define xbackground (xdata->background)
#define xftcolors (xdata->xftcolors)
#endif
