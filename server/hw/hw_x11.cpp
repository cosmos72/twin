/*
 *  hw_x11.c  --  functions to let twin display on X11
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

#define THIS "hw_x11"

#include "hw_x/flavor.h"
#undef HW_X_DRIVER
#define HW_X_DRIVER HW_X11

#include "hw_x/features.h"
#include "hw_x/driver.h"
#include "hw_x/keyboard.h"

#define XDRAW(col, rune_count, buf, buflen)                                                        \
  do {                                                                                             \
    SetColors(col);                                                                                \
    XDrawImageString16(this->xdisplay, this->xwindow, this->xgc, startx, starty + this->xupfont,   \
                       buf, buflen);                                                               \
  } while (0)

#include "hw_x/util.h"
#include "hw_x/common.h"

/* manage foreground/background colors */
TW_ATTR_HIDDEN void XDRIVER::SetFg(const trgb fg) {
  if (this->xrgb_fg != fg) {
    this->xrgb_fg = fg;
    XSetForeground(this->xdisplay, this->xgc, this->xsgc.foreground = ColorToPixel(fg));
  }
}

TW_ATTR_HIDDEN void XDRIVER::SetBg(const trgb bg) {
  if (this->xrgb_bg != bg) {
    this->xrgb_bg = bg;
    XSetBackground(this->xdisplay, this->xgc, this->xsgc.background = ColorToPixel(bg));
  }
}

TW_ATTR_HIDDEN void XDRIVER::SetColors(tcolor col) {
  SetFg(TCOLFG(col));
  SetBg(TCOLBG(col));
}

/*
 * return ttrue if each font glyph is either 'narrow' (latin, etc.) or 'wide' (CJK...)
 * with 'wide' characters exactly twice as wide as 'narrow' ones
 */
TW_ATTR_HIDDEN bool XDRIVER::FontIsDualWidth(const XFontStruct *info) {
  XCharStruct *pc = info->per_char;
  ldat wide = info->max_bounds.width, narrow = info->min_bounds.width, i, n_chars, w;
  if (wide != narrow * 2) {
    return false;
  }
  if (pc == NULL) {
    /*
     * no way to check per-character bounding box. hope for the best...
     * at least GNU unifont falls in this case.
     */
    return true;
  }
  n_chars = (ldat)(info->max_byte1 - info->min_byte1 + 1) *
            (info->max_char_or_byte2 - info->min_char_or_byte2 + 1);
  for (i = 0; i < n_chars; i++) {
    w = pc[i].width;
    if (w != 0 && w != narrow && w != wide) {
      return false;
    }
  }
  return true;
}

/* if font is monospaced, return its score. otherwise return MINLDAT */
ldat TW_ATTR_HIDDEN XDRIVER::MonospaceFontScore(const XFontStruct *info, udat fontwidth,
                                                udat fontheight, ldat best_score) {
  ldat score = TW_MINLDAT, width = info->min_bounds.width,
       height = (ldat)info->ascent + info->descent, max_width = info->max_bounds.width;

  if (width == max_width || FontIsDualWidth(info)) {
    score = FontScoreOf(fontwidth, fontheight, width, height);
    if (score > best_score)
      log(INFO) << "      candidate font " << width << "x" << height << " score " << score << "\n";
  }
  return score;
}

/* return name of selected font in allocated (char *) */
TW_ATTR_HIDDEN char *XDRIVER::AutodetectFont(const char *family, udat fontwidth, udat fontheight) {
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
  size_t pattern_len = strlen(patterns[0].wildcard) + 1 + 3 * sizeof(unsigned);
  pattern = (char *)AllocMem(pattern_len);
  if (!pattern) {
    return NULL;
  }
  for (i = 0; i < n_patterns && beatable_score; i++) {
    ldat score_adj = patterns[i].score_adj;

    beatable_score = best_score <= MAX_FONT_SCORE + score_adj;

    for (j = 0; j < 2 && beatable_score; j++) {
      n_fonts = 0;
      info = NULL;
      snprintf(digits, sizeof(digits), "%u",
               (unsigned)(fontheight / 10 + (j != 0 ? look_up ? 1 : -1 : 0)));
      snprintf(pattern, pattern_len, patterns[i].wildcard, digits + (digits[0] == '0'));
      names = XListFontsWithInfo(this->xdisplay, pattern, max_fonts, &n_fonts, &info);

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
          score = MonospaceFontScore(&info[k], fontwidth, fontheight, best_score - score_adj) +
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

TW_ATTR_HIDDEN bool XDRIVER::AllocColor(Visual *xvisual, Colormap colormap, XColor *color,
                                        unsigned long *pixel, int color_num) {
  (void)xvisual;
  (void)color_num;
  if (XAllocColor(this->xdisplay, colormap, color)) {
    *pixel = color->pixel;
    return true;
  }
  return false;
}

Tutf_function TW_ATTR_HIDDEN XDRIVER::UTF_32_to_charset_function(const char *charset) {
  const char *s, *fontname = NULL;
  unsigned long prop;
  uldat i;

  if (!charset) {
    /* attempt to autodetect encoding from fontname */
    if (XGetFontProperty(this->xsfont, XA_FONT, &prop)) {
      fontname = XGetAtomName(this->xdisplay, (Atom)prop);
    }
    if (fontname && !strcmp(fontname, "vga")) {
      charset = T_NAME_CP437;
    } else if (fontname) {
      i = 2;
      for (s = fontname + strlen(fontname) - 1; i && s >= fontname; s--) {
        if (*s == '-') {
          i--;
        }
      }
      if (!i) {
        charset = s + 2; /* skip current char and '-' */
      }
    }

    if (!charset) {
      if (this->xsfont->min_byte1 < this->xsfont->max_byte1) {
        /* font is more than just 8-bit. For now, assume it's unicode */
        log(WARNING) << "    " XSTR(XDRIVER) ".InitHW(): font `" << Chars::from_c(fontname)
                     << "\' has unknown charset encoding,\n"
                        "                assuming Unicode.\n";
        return NULL;
      }
      /* else assume codepage437. gross. */
      log(WARNING) << "    " XSTR(XDRIVER) ".InitHW(): font `" << Chars::from_c(fontname)
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
    log(WARNING) << "      " XSTR(XDRIVER) ".InitHW(): libtutf warning: unknown charset `"
                 << Chars::from_c(charset) << "', assuming charset `CP437' i.e. `VGA'.\n";
    return Tutf_UTF_32_to_CP437;
  }

  return Tutf_UTF_32_to_charset_function(i);
}
