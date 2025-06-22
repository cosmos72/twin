/* Driver class declaration for the various hw displays based on X11. */

#include "stl/view.h"

#if HW_X_DRIVER == HW_XFT
#include "hw/hw_xft/xchar16.h"
#else
#include "hw/hw_x/xchar16.h"
#endif

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

class XDRIVER {
public:
  static bool InitHW(Tdisplay hw);
  static void QuitHW(Tdisplay hw);

private:
  bool InitHW();
  void QuitHW();

  static ldat FontScoreOf(udat fontwidth, udat fontheight, ldat width, ldat height);
  ldat MonospaceFontScore(const XFontStruct *info, udat fontwidth, udat fontheight,
                          ldat best_score);
  char *AutodetectFont(const char *family, udat fontwidth, udat fontheight);
  bool LoadFont(const char *fontname, udat fontwidth, udat fontheight);
  static void Beep(Tdisplay hw);
  static void Configure(Tdisplay hw, udat resource, byte todefault, udat value);
  static int check_hw_name(char *hw_name);
  bool AllocColor(Visual *visual, Colormap colormap, XColor *color, unsigned long *pixel,
                  int color_num);

  void SetFg(const trgb fg);
  void SetBg(const trgb bg);
  void SetColors(tcolor col);
  unsigned long ColorToPixel(trgb rgb);
  bool FontIsDualWidth(const XFontStruct *info);
#if HW_X_DRIVER == HW_XFT
  static ldat CalcFontScore(udat fontwidth, udat fontheight, XftFont *fontp, const char *fontname);
  void DrawString16(int x, int y, XChar16 *string, int length);
  static void CopyColor(trgb rgb, unsigned long pixel, XftColor *dst);
#endif
  void DrawSome(dat x, dat y, ldat len);
  Tutf_function UTF_32_to_charset_function(const char *charset);
  static void utf8_to_wchar(Chars src, Vector<wchar_t> &dst);
  static trune UTF_32_to_UCS_2(trune c);

  static void InitKeys();
  static bool IsNumericKeypad(Twkey key);
  Twkey LookupKey(XEvent *ev, udat *ShiftFlags, udat *len, char *seq);
  void HandleEvent(XEvent *event);
  static void KeyboardEvent(int fd, Tdisplay hw);

  static void FillWindowTitle(char *title, int maxlen);
  void HideCursor(dat x, dat y);
  void ShowCursor(uldat type, dat x, dat y);
  static void FlushVideo(Tdisplay hw);
  static void FlushHW(Tdisplay hw);
  void setFlush() {
    hw->setFlush();
  }
  static void DetectSize(Tdisplay hw, dat *x, dat *y);
  static void CheckResize(Tdisplay hw, dat *x, dat *y);
  static void Resize(Tdisplay hw, dat x, dat y);
  static bool CanDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
  static void DragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);

  static bool SelectionImport_X11(Tdisplay hw);
  static void SelectionExport_X11(Tdisplay hw);
  void SelectionNotify_up(Window win, Atom prop);
  static void SelectionNotify_X11(Tdisplay hw, uldat reqprivate, e_id magic, const Chars mime,
                                  Chars data);
  void SelectionRequest_up(XSelectionRequestEvent *req);
  static void SelectionRequest_X11(Tdisplay hw, Tobj requestor, uldat reqprivate);

  static int Die(Display *d);

  /*
   * Function prototypes for the functions defined in common.c to minimize ordering dependencies
   * when #include-ing into hw_xyz.c.
   */

  static inline ldat diff(ldat x, ldat y) {
    return x >= y ? x - y : y - x;
  }

public:
  class key_to_tw {
  public:
    Twkey tkey;
    char seq[6];

    // reuse termination byte seq[5] as length to save space
    byte len() const {
      return seq[5];
    }
  };

  class key_to_tw_entry {
  public:
    uldat xkey; // actually KeySym
    key_to_tw tmapping;
  };

public:
  Tdisplay hw;

private:
  Tutf_function xUTF_32_to_charset;
  Display *xdisplay;
  Window xwindow;
  GC xgc;
  XGCValues xsgc;
  XSYM(rgb_info) xrgb_info;
  tcolor xcol; // current fg, bg colors
  int xwidth, xheight;
  int xwfont, xhfont, xupfont;
  /* we support showing only a portion of the whole twin display */
  dat xhw_view, xhw_startx, xhw_starty, xhw_endx, xhw_endy;
  byte xnumkeypad; /* ttrue if numeric keypad emits numeric digits */
  byte xtruecolor;

#if HW_X_DRIVER == HW_X11
  XFontStruct *xsfont;
#elif HW_X_DRIVER == HW_XFT
  XftFont *xsfont;
  XftDraw *xtdraw;
  XftColor *xtfg; // current Xft foreground color
  XftColor *xtbg; // current Xft background color
  XftColor foreground_buf, background_buf;
  XftColor *xtpalette[tpalette_n];
#endif
  trgb xrgb_fg;
  trgb xrgb_bg;
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
  XSelectionRequestEvent xReq[NEST];
  unsigned long xpalette[tpalette_n];
  Atom xCOMPOUND_TEXT, xTARGETS, xTEXT, xUTF8_STRING, xWM_DELETE_WINDOW, xWM_PROTOCOLS;
};

#define xdriver(hw) ((XDRIVER *)(hw)->Private)
