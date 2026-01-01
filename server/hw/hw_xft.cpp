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

#include "common.h"
#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "log.h"

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

#define XDRAW(col, rune_count, buf, buflen)                                                        \
  do {                                                                                             \
    SetColors(col);                                                                                \
    DrawStringUtf8(startx, starty + this->xupfont, rune_count, buf, buflen);                       \
  } while (0)

#include "hw_x/util.h"
#include "hw_x/common.h"

TW_ATTR_HIDDEN void XDRIVER::DrawStringUtf8(int x, int y, int rune_count, const char *string,
                                            int string_length) {
  /*
   * XftDrawStringUtf8 doesn't erase the existing character before it draws a new one,
   * and when it draws the new character, it only draws the strokes, so you see some of the previous
   * character "underneath" the new one.
   * Solution: first draw a rectangle using background color, then draw the text on top of it
   * using foreground color.
   */
  XftDrawRect(this->xtdraw, this->xtbg, x, y - this->xsfont->ascent,
              rune_count * this->xsfont->max_advance_width,
              this->xsfont->ascent + this->xsfont->descent);
  XftDrawStringUtf8(this->xtdraw, this->xtfg, this->xsfont, x, y, (const FcChar8 *)string,
                    string_length);
}

TW_ATTR_HIDDEN void XDRIVER::CopyColor(trgb rgb, unsigned long pixel, XftColor *dst) {
  dst->pixel = pixel;
  dst->color.red = 257 * (udat)TRED(rgb);
  dst->color.green = 257 * (udat)TGREEN(rgb);
  dst->color.blue = 257 * (udat)TBLUE(rgb);
  dst->color.alpha = 0xFFFF;
}

/* manage foreground/background colors */

TW_ATTR_HIDDEN void XDRIVER::SetFg(const trgb fg) {
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

TW_ATTR_HIDDEN void XDRIVER::SetBg(const trgb bg) {
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

TW_ATTR_HIDDEN void XDRIVER::SetColors(const tcolor col) {
  SetFg(TCOLFG(col));
  SetBg(TCOLBG(col));
}

TW_ATTR_HIDDEN ldat XDRIVER::CalcFontScore(udat fontwidth, udat fontheight, int weight,
                                           XftFont *fontp, const char *fontname) {
  if (FC_CHARSET_MAP_SIZE >= 256 / 32) {
    FcChar32 map[FC_CHARSET_MAP_SIZE] = {}, *ptr = map, mask = (FcChar32)-1;
    FcChar32 next, first = FcCharSetFirstPage(fontp->charset, map, &next);
    /* check that font contains at least ASCII (space is not necessary since it's blank) */
    if (first == FC_CHARSET_DONE || first > ' ' + 1) {
      return 0;
    }
    if (first == 0) {
      first = 32;
    } else {
      if (first < ' ') {
        FcChar32 n = ' ' - first;
        first = 32;
        map[0] >>= n;
        mask >>= n;
      }
      if ((map[0] & mask) != mask) {
        return 0;
      }
    }
    ptr++;
    while (first < '~') {
      if (ptr >= map + FC_CHARSET_MAP_SIZE) {
        return 0;
      }
      mask = (FcChar32)-1;
      if ('~' - first < 32)
        mask >>= '~' - first;
      if ((ptr[0] & mask) != mask) {
        return 0;
      }
      first += 32;
      ptr++;
    }
  }
  ldat score = FontScoreOf(fontwidth, fontheight, (ldat)fontp->max_advance_width,
                           (ldat)fontp->ascent + fontp->descent);
  /* slightly prefer regular weight fonts */
  if (weight < FC_WEIGHT_REGULAR) {
    score -= (FC_WEIGHT_REGULAR - weight + 8) / 16;
  } else if (weight > FC_WEIGHT_REGULAR) {
    score -= (weight - FC_WEIGHT_REGULAR + 8) / 16;
  }
  /* slightly prefer fonts with "DejaVu" "Sans" or "Mono" in their name */
  if (!strstr(fontname, "DejaVu") && !strstr(fontname, "dejavu")) {
    score -= 2;
  }
  if (!strstr(fontname, "Sans") && !strstr(fontname, "sans") && !strstr(fontname, "Mono") &&
      !strstr(fontname, "mono")) {
    score--;
  }
  return score;
}

/* return name of selected font in allocated (char *) */
TW_ATTR_HIDDEN char *XDRIVER::AutodetectFont(const char *family, udat fontwidth, udat fontheight) {
  char *fontname = NULL;
  FcPattern *best_pattern = NULL;
  ldat best_score = TW_MINLDAT;
  const char *path = family && (strchr(family, '.') || strchr(family, '/')) ? family : NULL;

  /*
   * find a usable font as follows
   *    an xft font (outline=true, scalable=true)
   *    monospace (spacing=100)
   *    not italic (slant=0)
   *    medium weight (50 <= weight <= 200)
   *    highest font score (closest to fontwidth X fontheight)
   */
  FcFontSet *fontset =
      XftListFonts(this->xdisplay, DefaultScreen(this->xdisplay), XFT_OUTLINE, XftTypeBool, FcTrue,
                   XFT_SCALABLE, XftTypeBool, FcTrue, XFT_SPACING, XftTypeInteger, 100, XFT_SLANT,
                   XftTypeInteger, 0, (char *)0, XFT_WEIGHT, XFT_FAMILY, XFT_FILE, (char *)0);
  if (fontset) {
    log(INFO) << "      found monospace fonts: ";
    for (int i = 0; i < fontset->nfont; i++) {
      XftFont *fontp;
      FcChar8 *file;
      FcChar8 *t_family;
      FcPattern *t_pattern;
      int weight;

      if (FcPatternGetInteger(fontset->fonts[i], XFT_WEIGHT, 0, &weight) != FcResultMatch) {
        continue;
      }
      if ((weight < FC_WEIGHT_LIGHT) || (weight > FC_WEIGHT_BOLD)) {
        continue;
      }
      if (FcPatternGetString(fontset->fonts[i], XFT_FILE, 0, &file) != FcResultMatch) {
        continue;
      }
      if (FcPatternGetString(fontset->fonts[i], XFT_FAMILY, 0, &t_family) != FcResultMatch) {
        continue;
      }
      log(INFO) << Chars(", ", i == 0 ? 0 : 2) << Chars::from_c((const char *)t_family);
      if (path     ? strstr((const char *)file, path) != NULL
          : family ? strstr((const char *)t_family, family) != NULL
                   : true) {
        /* ok, font matches requested text */
      } else {
        log(INFO) << " [x]";
        continue;
      }

      t_pattern = FcPatternCreate();
      FcPatternAddInteger(t_pattern, XFT_PIXEL_SIZE, fontheight * 5 / 6);
      FcPatternAddString(t_pattern, XFT_FILE, file);

      fontp = XftFontOpenPattern(this->xdisplay, t_pattern);
      if (fontp) {
        ldat score = CalcFontScore(fontwidth, fontheight, weight, fontp, (const char *)file);
        log(INFO) << " [" << score << "]";
        if (best_pattern == NULL || score > best_score) {
          best_score = score;
          if (best_pattern) {
            FcPatternDestroy(best_pattern);
          }
          best_pattern = FcPatternDuplicate(t_pattern);
        }
        /* XftFontClose() also destroys the XftPattern passed to XftFontOpenPattern() */
        XftFontClose(this->xdisplay, fontp);
      } else {
        log(INFO) << " [-]";
      }
    }
    FcFontSetDestroy(fontset);
  }
  log(INFO) << "\n";

  if (best_pattern) {
    fontname = (char *)FcNameUnparse(best_pattern);
    FcPatternDestroy(best_pattern);
  } else {
    fontname = strdup("Mono");
  }
  return fontname;
}

TW_ATTR_HIDDEN bool XDRIVER::AllocColor(Visual *visual, Colormap colormap, XColor *color,
                                        unsigned long *pixel, int color_num) {
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

Tutf_function TW_ATTR_HIDDEN XDRIVER::UTF_32_to_charset_function(const char *charset) {
  (void)charset;
  /* this is sufficient for XftDrawStringUtf8 */
  return UTF_32_identity;
}
