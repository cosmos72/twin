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
#include <X11/Xmd.h>                /* CARD32 */
#include <X11/Xft/Xft.h>
#include <fontconfig/fontconfig.h>

#define THIS "hw_xft"

#include "hw_x/flavor.h"
#undef HW_X_DRIVER
#define HW_X_DRIVER HW_XFT

#include "hw_x/features.h"
#include "hw_x/x11_data.h"
#include "hw_x/keyboard.h"
#include "hw_xft/xchar16.h"
#include "hw_x/flavor_protos.h"
#include "hw_x/util_protos.h"
#include "hw_x/common_protos.h"

/* forward declaration... */
static void X11_XftDrawString16(Display *display, Drawable d, GC gc, int x, int y,
        XChar16 *string, int length);

# define myXDrawImageString X11_XftDrawString16

#define XDRAW(col, buf, buflen) do { \
    X11_SetColors(col); \
    myXDrawImageString(xdisplay, xwindow, xgc, xbegin, ybegin + xupfont, buf, buflen); \
} while(0)

#define XDRAW_ANY(buf, buflen, col, gfx) XDRAW(col, buf, buflen)

#include "hw_x/util.h"
#include "hw_x/common.cpp"

static void X11_XftDrawString16(Display *display, Drawable d, GC gc, int x, int y,
        XChar16 *string, int length) {
    /*
     * XftDrawString16 doesn't erase the existing character before it draws a new one, and when
     * it draws the new one, it only draws the strokes, so you see some of the previous character
     * "underneath" the new one.  So we first draw a rectangle with the background color, and then
     * draw the text on top of it in the foreground color.
     */
    XftDrawRect (xftdraw, xbackground, x, y - xsfont->ascent, length * xsfont->max_advance_width,
            xsfont->ascent + xsfont->descent);
    XftDrawString16(xftdraw, xforeground, xsfont, x, y, string, length);
}

/* manage foreground/background colors */
static void X11_SetColors(hwcol col) {
    if (xsgc.foreground != xcol[COLFG(col)]) {
        XSetForeground(xdisplay, xgc, xsgc.foreground = xcol[COLFG(col)]);
        xforeground = xftcolors[COLFG(col)];
    }

    if (xsgc.background != xcol[COLBG(col)]) {
        XSetBackground(xdisplay, xgc, xsgc.background = xcol[COLBG(col)]);
        xbackground = xftcolors[COLBG(col)];
    }
}

static ldat xftCalcFontScore(udat fontwidth, udat fontheight, XftFont *fontp, CONST char * fontname) {
    if (FC_CHARSET_MAP_SIZE >= 256/32) {
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
    ldat score = calcFontScore(fontwidth, fontheight, (ldat)fontp->max_advance_width,
			       (ldat)fontp->ascent + fontp->descent);
    /* slightly prefer fonts with "DejaVu" "Sans" and "Mono" in their name */
    if (!strstr(fontname, "DejaVu") && !strstr(fontname, "dejavu"))
	score -= 5;
    if (!strstr(fontname, "Sans") && !strstr(fontname, "sans"))
	score--;
    if (!strstr(fontname, "Mono") && !strstr(fontname, "mono"))
	score -= 2;
    return score;
}

/* return name of selected font in allocated (char *) */
static char * X11_AutodetectFont(udat fontwidth, udat fontheight) {
    FcFontSet *fontset;
    XftFont *fontp;
    char *fontname = NULL;
    char *t_fontname = NULL;
    int t_fontname_len = -1;
    ldat score, best_score = TW_MINLDAT;
    int xscreen = DefaultScreen(xdisplay);

    /*
     * find a usable font as follows
     *    an xft font (outline=true, scalable=true)
     *    monospace (spacing=100)
     *    not italic (slant=0)
     *    medium weight (75 <= weight <= 100)
     *    highest font score (closest to fontwidth X fontheight)
     */
    fontset = XftListFonts (xdisplay, DefaultScreen(xdisplay),
            XFT_OUTLINE, XftTypeBool, FcTrue,
            XFT_SCALABLE, XftTypeBool, FcTrue,
            XFT_SPACING, XftTypeInteger, 100,
            XFT_SLANT, XftTypeInteger, 0,
            (char *)0,
            XFT_WEIGHT, XFT_FILE, (char *)0);
    if (fontset) {
        for (int i = 0; i < fontset->nfont; i++) {
            int weight;
            int len;
            FcChar8 *file;

            if (FcPatternGetInteger (fontset->fonts[i], XFT_WEIGHT, 0, &weight) != FcResultMatch) {
                continue;
            }
            if ((weight < FC_WEIGHT_BOOK) || (weight > FC_WEIGHT_MEDIUM)) {
                continue;
            }
            if (FcPatternGetString (fontset->fonts[i], XFT_FILE, 0, &file) != FcResultMatch) {
                continue;
            }

            /* reuse existing t_fontname if possible, otherwise allocate a new one */
            len = strlen((CONST char *)file) + strlen(":file=") + 1;
            if (!t_fontname || (len > t_fontname_len)) {
                if (t_fontname) {
                    FreeMem(t_fontname);
                }
                t_fontname = (char *)AllocMem(t_fontname_len = len);
                if (!t_fontname) {
                    printk("      X11_AutodetectFont(): Out of memory!\n");
                    break;
                }
            }
            sprintf(t_fontname, ":file=%s", file);

            fontp = XftFontOpenName(xdisplay, xscreen, t_fontname);
            if (fontp) {
                score = xftCalcFontScore(fontwidth, fontheight, fontp, t_fontname);
                XftFontClose(xdisplay, fontp);

                if (!fontname || (score > best_score)) {
                    best_score = score;
                    if (fontname) {
                        FreeMem(fontname);
                    }
                    fontname = t_fontname;
                    t_fontname = NULL;
                }
            }
        }
        FcFontSetDestroy(fontset);
    }

    if (t_fontname) {
        FreeMem(t_fontname);
        t_fontname = NULL;
    }

    return fontname;
}

static int X11_AllocColor(Display *display, Visual *xvisual, Colormap colormap,
        XColor *xcolor, unsigned long *pixel, int color_num) {
    XRenderColor xrcolor;
    XftColor *xft_color;

    if (!(xft_color = (XftColor *)AllocMem(sizeof(XftColor)))) {
        printk("      X11_AllocColor(): Out of memory!\n");
        return -1;
    }
    WriteMem(xft_color, 0, sizeof(XftColor));

    xrcolor.red = xcolor->red;
    xrcolor.green = xcolor->green;
    xrcolor.blue  = xcolor->blue;
    xrcolor.alpha = 65535;

    if (!XftColorAllocValue(xdisplay, xvisual, colormap, &xrcolor, xft_color)) {
        return -1;
    }
    *pixel = xft_color->pixel;
    xftcolors[color_num] = xft_color;

    return 1;
}

static void X11_FlavorQuitHW(void) {
    int xscreen;
    Colormap colormap;
    Visual *xvisual;

    if (xdisplay) {
        if (xsfont) XftFontClose(xdisplay, xsfont);
        xscreen = DefaultScreen(xdisplay);
        colormap = DefaultColormap(xdisplay, xscreen);
        xvisual = DefaultVisual(xdisplay, xscreen);
    }
    if (xftdraw) XftDrawDestroy(xftdraw);
    for (int i = 0; i < MAXCOL; i++) {
        if (xftcolors[i] == NULL) {
            break;
        }
        if (xdisplay) XftColorFree (xdisplay, xvisual, colormap, xftcolors[i]);
        FreeMem(xftcolors[i]);
        xftcolors[i] = NULL;
    }
}

/* custom version of X11_UTF_32_to_charset_function for the XFT driver */
static Tutf_function X11_UTF_32_to_charset_function(CONST char *charset) {
    /* this is sufficient for xft fonts which are 16-bit unicode */
    return X11_UTF_32_to_UCS_2;
}
