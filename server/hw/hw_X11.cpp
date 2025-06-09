/*
 *  hw_X11.c  --  functions to let twin display on X11
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <Tw/Twkeys.h>
#include <Tutf/Tutf.h>
#include <Tutf/Tutf_defs.h>

#include "twin.h"
#include "alloc.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h> /* CARD32 */

#define THIS "hw_X11"

#include "hw_x/flavor.h"
#undef HW_X_DRIVER
#define HW_X_DRIVER HW_X11

#include "hw_x/features.h"
#include "hw_x/x11_data.h"
#include "hw_x/keyboard.h"
#include "hw_x/xchar16.h"
#include "hw_x/flavor_protos.h"
#include "hw_x/util_protos.h"
#include "hw_x/common_protos.h"

#define myXDrawImageString XDrawImageString16

#define XDRAW(col, buf, buflen)                                                                    \
  do {                                                                                             \
    XSYM(SetColors)(col);                                                                          \
    myXDrawImageString(xdisplay, xwindow, xgc, xbegin, ybegin + xupfont, buf, buflen);             \
  } while (0)

#include "hw_x/util.h"
#include "hw_x/common.h"

/* manage foreground/background colors */
static void XSYM(SetColors)(tcolor col) {
  unsigned long fg = XSYM(ColorToPixel)(TCOLFG(col));
  if (xsgc.foreground != fg) {
    XSetForeground(xdisplay, xgc, xsgc.foreground = fg);
  }
  unsigned long bg = XSYM(ColorToPixel)(TCOLBG(col));
  if (xsgc.background != bg) {
    XSetBackground(xdisplay, xgc, xsgc.background = bg);
  }
}

/*
 * return ttrue if each font glyph is either 'narrow' (latin, etc.) or 'wide' (CJK...)
 * with 'wide' characters exactly twice as wide as 'narrow' ones
 */
static tbool XSYM(FontIsDualWidth)(const XFontStruct *info) {
  XCharStruct *p = info->per_char;
  ldat wide = info->max_bounds.width, narrow = info->min_bounds.width, i, n_chars, w;
  if (wide != narrow * 2)
    return tfalse;
  if (p == NULL)
    /*
     * no way to check per-character bounding box. hope for the best...
     * at least GNU unifont falls in this case.
     */
    return ttrue;

  n_chars = (ldat)(info->max_byte1 - info->min_byte1 + 1) *
            (info->max_char_or_byte2 - info->min_char_or_byte2 + 1);
  for (i = 0; i < n_chars; i++) {
    w = p[i].width;
    if (w != 0 && w != narrow && w != wide)
      return tfalse;
  }
  return ttrue;
}

/* if font is monospaced, return its score. otherwise return MINLDAT */
static ldat XSYM(MonospaceFontScore)(const XFontStruct *info, udat fontwidth, udat fontheight,
                                     ldat best_score) {
  ldat score = TW_MINLDAT, width = info->min_bounds.width,
       height = (ldat)info->ascent + info->descent, max_width = info->max_bounds.width;

  if (width == max_width || XSYM(FontIsDualWidth)(info)) {
    score = XSYM(FontScoreOf)(fontwidth, fontheight, width, height);
    if (score > best_score)
      log(INFO) << "      candidate font " << width << "x" << height << " score " << score << "\n";
  }
  return score;
}

/* return name of selected font in allocated (char *) */
static char *XSYM(AutodetectFont)(const char *family, udat fontwidth, udat fontheight) {
  struct {
    const char *wildcard;
    ldat score_adj;
  } patterns[] = {
      /* { "-gnu-unifont-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1",  0 }, double-width chars not
         supported yet */
      {"-misc-console-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1", 0},
      {"-misc-fixed-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1", 0},
      {"-*-*-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1", 0},
      {"-*-*-medium-r-normal-*-%s?-*-*-*-*-*-*-cp437", -5},
      {"-*-*-medium-r-normal-*-%s?-*-*-*-*-*-*-cp850", -5},
      {"-*-*-medium-r-normal-*-%s?-*-*-*-*-*-ibm-850", -5},
      {"-*-*-medium-r-normal-*-%s?-*-*-*-*-*-iso8859-*", -10},
  };
  enum { max_fonts = 1000, n_patterns = N_OF(patterns) };

  XFontStruct *info;
  int i, j, k, n_fonts;

  char *pattern = NULL;
  char digits[1 + 3 * sizeof(unsigned)];
  char **names = NULL;
  char *best = NULL;
  ldat score, best_score = TW_MINLDAT;
  byte beatable_score = ttrue, look_up = fontheight >= 10 && (fontheight % 10) >= 5;

  if (family) {
    return strdup(family);
  }
  pattern = (char *)AllocMem(strlen(patterns[0].wildcard) + 1 + 3 * sizeof(unsigned));
  if (!pattern)
    return NULL;

  for (i = 0; i < n_patterns && beatable_score; i++) {
    ldat score_adj = patterns[i].score_adj;

    beatable_score = best_score <= MAX_FONT_SCORE + score_adj;

    for (j = 0; j < 2 && beatable_score; j++) {
      n_fonts = 0;
      info = NULL;
      sprintf(digits, "%u", (unsigned)(fontheight / 10 + (j != 0 ? look_up ? 1 : -1 : 0)));
      sprintf(pattern, patterns[i].wildcard, digits + (digits[0] == '0'));
      names = XListFontsWithInfo(xdisplay, pattern, max_fonts, &n_fonts, &info);

      /* printk("%4d fonts match '%s'\n", names ? n_fonts : 0, pattern); */
      if (names == NULL)
        continue;

      for (k = 0; k < n_fonts && beatable_score; k++) {
        /*
        printk("     font        '%s'\t direction = %u, min_byte1 = %u, max_byte1 = %u,
        min_char_or_byte2 = %d, max_char_or_byte2 = %d\n", names[k], info[k].direction,
        info[k].min_byte1, info[k].max_byte1, info[k].min_char_or_byte2, info[k].max_char_or_byte2);
         */

        if (info[k].direction == FontLeftToRight && info[k].min_byte1 == 0 &&
            info[k].min_char_or_byte2 <= 32) {
          score =
              XSYM(MonospaceFontScore)(&info[k], fontwidth, fontheight, best_score - score_adj) +
              score_adj;
          if (score <= best_score)
            continue;

          best_score = score;
          beatable_score = best_score < MAX_FONT_SCORE + score_adj;

          FreeMem(best);
          best = CloneStr(names[k]);
        }
      }
      XFreeFontInfo(names, info, n_fonts);
    }
  }
  FreeMem(pattern);
  return best;
}

static int XSYM(AllocColor)(Display *display, Visual *xvisual, Colormap colormap, XColor *xcolor,
                            unsigned long *pixel, int color_num) {
  if (!XAllocColor(display, colormap, xcolor)) {
    return -1;
  }
  *pixel = xcolor->pixel;
  return 1;
}

static void XSYM(FlavorQuitHW)(void) {
  if (xsfont)
    XFreeFont(xdisplay, xsfont);
}
