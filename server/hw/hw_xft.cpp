/*
 *  hw_xft.c  --  functions to let twin display on X11 with truetype fonts.
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
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
#include <X11/Xft/Xft.h>
#include <fontconfig/fontconfig.h>

#define THIS "hw_xft"

#include "hw_x/flavor.h"
#undef HW_X_DRIVER
#define HW_X_DRIVER HW_XFT

#include "hw_x/features.h"
#include "hw_x/driver.h"
#include "hw_x/keyboard.h"

#define XDRAW(col, buf, buflen)                                                                    \
  do {                                                                                             \
    SetColors(col);                                                                                \
    DrawString16(xbegin, ybegin + this->xupfont, buf, buflen);                                     \
  } while (0)

#include "hw_x/util.h"
#include "hw_x/common.h"

void XDRIVER::DrawString16(int x, int y, XChar16 *string, int length) {
  /*
   * XftDrawString16 doesn't erase the existing character before it draws a new one, and when
   * it draws the new one, it only draws the strokes, so you see some of the previous character
   * "underneath" the new one.  So we first draw a rectangle with the background color, and then
   * draw the text on top of it in the foreground color.
   */
  XftDrawRect(this->xtdraw, this->xtbg, x, y - this->xsfont->ascent,
              length * this->xsfont->max_advance_width,
              this->xsfont->ascent + this->xsfont->descent);
  XftDrawString16(this->xtdraw, this->xtfg, this->xsfont, x, y, string, length);
}

void XDRIVER::CopyColor(trgb rgb, unsigned long pixel, XftColor *dst) {
  dst->pixel = pixel;
  dst->color.red = 257 * (udat)TRED(rgb);
  dst->color.green = 257 * (udat)TGREEN(rgb);
  dst->color.blue = 257 * (udat)TBLUE(rgb);
  dst->color.alpha = 0xFFFF;
}

/* manage foreground/background colors */

void XDRIVER::SetFg(const trgb fg) {
  if (this->xrgb_fg != fg) {
    this->xrgb_fg = fg;
    if (this->xtruecolor) {
      const unsigned long pixel = ColorToPixel(fg);
      XSetForeground(this->xdisplay, this->xgc, this->xsgc.foreground = pixel);
      CopyColor(fg, pixel, this->xtfg);
    } else {
      const byte index = TrueColorToPalette256(fg);
      XSetForeground(this->xdisplay, this->xgc, this->xsgc.foreground = this->xpalette[index]);
      this->xtfg = this->xtpalette[index];
    }
  }
}

void XDRIVER::SetBg(const trgb bg) {
  if (this->xrgb_bg != bg) {
    this->xrgb_bg = bg;
    if (this->xtruecolor) {
      const unsigned long pixel = ColorToPixel(bg);
      XSetBackground(this->xdisplay, this->xgc, this->xsgc.background = pixel);
      CopyColor(bg, pixel, this->xtbg);
    } else {
      const byte index = TrueColorToPalette256(bg);
      XSetBackground(this->xdisplay, this->xgc, this->xsgc.background = this->xpalette[index]);
      this->xtbg = this->xtpalette[index];
    }
  }
}

void XDRIVER::SetColors(const tcolor col) {
  SetFg(TCOLFG(col));
  SetBg(TCOLBG(col));
}

ldat XDRIVER::CalcFontScore(udat fontwidth, udat fontheight, XftFont *fontp, const char *fontname) {
  if (FC_CHARSET_MAP_SIZE >= 256 / 32) {
    FcChar32 map[FC_CHARSET_MAP_SIZE] = {}, *ptr = map, mask = (FcChar32)-1;
    FcChar32 next, first = FcCharSetFirstPage(fontp->charset, map, &next);
    /* check that font contains at least ASCII (space is not necessary since it's blank) */
    if (first == FC_CHARSET_DONE || first > ' ' + 1)
      return 0;
    if (first == 0) {
      first = 32;
    } else {
      if (first < ' ') {
        FcChar32 n = ' ' - first;
        first = 32;
        map[0] >>= n;
        mask >>= n;
      }
      if ((map[0] & mask) != mask)
        return 0;
    }
    ptr++;
    while (first < '~') {
      if (ptr >= map + FC_CHARSET_MAP_SIZE)
        return 0;
      mask = (FcChar32)-1;
      if ('~' - first < 32)
        mask >>= '~' - first;
      if ((ptr[0] & mask) != mask)
        return 0;
      first += 32;
      ptr++;
    }
  }
  ldat score = FontScoreOf(fontwidth, fontheight, (ldat)fontp->max_advance_width,
                           (ldat)fontp->ascent + fontp->descent);
  /* slightly prefer fonts with "DejaVu" "Sans" or "Mono" in their name */
  if (!strstr(fontname, "DejaVu") && !strstr(fontname, "dejavu"))
    score -= 2;
  if (!strstr(fontname, "Sans") && !strstr(fontname, "sans") && !strstr(fontname, "Mono") &&
      !strstr(fontname, "mono"))
    score--;
  return score;
}

/* return name of selected font in allocated (char *) */
char *XDRIVER::AutodetectFont(const char *family, udat fontwidth, udat fontheight) {
  char *fontname = NULL;
  FcPattern *best_pattern = NULL;
  ldat best_score = TW_MINLDAT;

  /*
   * find a usable font as follows
   *    an xft font (outline=true, scalable=true)
   *    monospace (spacing=100)
   *    not italic (slant=0)
   *    medium weight (75 <= weight <= 100)
   *    highest font score (closest to fontwidth X fontheight)
   */
  FcFontSet *fontset =
      XftListFonts(this->xdisplay, DefaultScreen(this->xdisplay), XFT_OUTLINE, XftTypeBool, FcTrue,
                   XFT_SCALABLE, XftTypeBool, FcTrue, XFT_SPACING, XftTypeInteger, 100, XFT_SLANT,
                   XftTypeInteger, 0, (char *)0, XFT_WEIGHT, XFT_FAMILY, XFT_FILE, (char *)0);
  if (fontset) {
    for (int i = 0; i < fontset->nfont; i++) {
      XftFont *fontp;
      FcChar8 *file;
      FcChar8 *t_family;
      FcPattern *t_pattern;
      int weight;

      if (FcPatternGetInteger(fontset->fonts[i], XFT_WEIGHT, 0, &weight) != FcResultMatch) {
        continue;
      }
      if ((weight < FC_WEIGHT_BOOK) || (weight > FC_WEIGHT_MEDIUM)) {
        continue;
      }
      if (FcPatternGetString(fontset->fonts[i], XFT_FILE, 0, &file) != FcResultMatch) {
        continue;
      }
      if (family &&
          FcPatternGetString(fontset->fonts[i], XFT_FAMILY, 0, &t_family) == FcResultMatch) {
        if (!strstr((const char *)t_family, family)) {
          continue;
        }
      }

      t_pattern = FcPatternCreate();
      FcPatternAddInteger(t_pattern, XFT_PIXEL_SIZE, fontheight * 5 / 6);
      FcPatternAddString(t_pattern, XFT_FILE, file);

      fontp = XftFontOpenPattern(this->xdisplay, t_pattern);
      if (fontp) {
        ldat score = CalcFontScore(fontwidth, fontheight, fontp, (const char *)file);

        if (best_pattern == NULL || score > best_score) {
          best_score = score;
          if (best_pattern) {
            FcPatternDestroy(best_pattern);
          }
          best_pattern = FcPatternDuplicate(t_pattern);
        }
        /* XftFontClose() also destroys the XftPattern passed to XftFontOpenPattern() */
        XftFontClose(this->xdisplay, fontp);
      }
    }
    FcFontSetDestroy(fontset);
  }

  if (best_pattern) {
    fontname = (char *)FcNameUnparse(best_pattern);
    FcPatternDestroy(best_pattern);
  } else {
    fontname = strdup("Mono");
  }
  return fontname;
}

bool XDRIVER::AllocColor(Visual *visual, Colormap colormap, XColor *color, unsigned long *pixel,
                         int color_num) {
  XftColor *fcolor;
  XRenderColor rcolor;

  if (!(fcolor = (XftColor *)AllocMem0(sizeof(XftColor)))) {
    log(ERROR) << "      " XSTR(XDRIVER) ".AllocColor(): Out of memory!\n";
    return false;
  }
  rcolor.red = color->red;
  rcolor.green = color->green;
  rcolor.blue = color->blue;
  rcolor.alpha = 0xFFFF;

  if (!XftColorAllocValue(this->xdisplay, visual, colormap, &rcolor, fcolor)) {
    FreeMem(fcolor);
    return false;
  }
  *pixel = fcolor->pixel;
  this->xtpalette[color_num] = fcolor;

  return true;
}

Tutf_function XDRIVER::UTF_32_to_charset_function(const char *charset) {
  /* this is sufficient for xft fonts which are 16-bit unicode */
  return UTF_32_to_UCS_2;
}
