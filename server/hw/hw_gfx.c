/*
 *  hw_gfx.c  --  functions to let twin display on X11 using libXpm
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"

#ifndef PKG_DATADIR
# define PKG_DATADIR "/usr/local/share/twin"
#endif

#define GFXDIR PKG_DATADIR "/themes/hw_gfx"

#include <Tw/Twkeys.h>

#include <Tutf/Tutf.h>
#include <Tutf/Tutf_defs.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>                /* CARD32 */

#include <X11/xpm.h>

#ifdef TW_HAVE_UNISTD_H
# include <unistd.h>
#endif

#define THIS "hw_gfx"


#include "hw_x/features.h"


/* Display variables */

#define NEST 4

/* how to draw screen root and widgets interior: */
#define GFX_USE_NONE	0	/* no pixmap */
#define GFX_USE_THEME	1	/* use pixmap tile from theme */
#define GFX_USE_ROOT	2	/* use root pixmap */
#define GFX_USE_BG	3	/* use background pixmap */


struct x11_data {
    unsigned xwidth, xheight;
    int xwfont, xhfont, xupfont;

    /* we support showing only a portion of the whole twin display */
    dat xhw_view, xhw_startx, xhw_starty, xhw_endx, xhw_endy;
    
    int xmonochrome;
    Tutf_function xUTF_16_to_charset;
    Display     *xdisplay;
    Window       xwindow;
    Pixmap       xtheme, xroot, xbg;
    GC           xgc, xthemegc, xrootgc, xbggc;
    XFontStruct *xsfont;
#ifdef TW_FEATURE_X11_XIM_XIC /* autodetected by hw_x/features.h */
    XIM		 xim;
    XIC		 xic;
#endif
    XComposeStatus xcompose;
    byte         xwindow_AllVisible, xfont_map, xroot_flag, xbg_flag;
    unsigned long xcol[MAXCOL+1];
    obj          xRequestor[NEST];
    uldat        xReqPrivate[NEST];
    uldat        xReqCount;
    uldat        XReqCount;
    XSelectionRequestEvent XReq[NEST];
    Atom xWM_PROTOCOLS, xWM_DELETE_WINDOW, xTARGETS;
    XGCValues    xsgc, xthemesgc;
};

#define xdata		((struct x11_data *)HW->Private)
#define xwidth		(xdata->xwidth)
#define xheight		(xdata->xheight)
#define xwfont		(xdata->xwfont)
#define xhfont		(xdata->xhfont)
#define xupfont		(xdata->xupfont)

#define xhw_view	(xdata->xhw_view)
#define xhw_startx	(xdata->xhw_startx)
#define xhw_starty	(xdata->xhw_starty)
#define xhw_endx	(xdata->xhw_endx)
#define xhw_endy	(xdata->xhw_endy)

#define xmonochrome	(xdata->xmonochrome)
#define xUTF_16_to_charset	(xdata->xUTF_16_to_charset)
#define xdisplay	(xdata->xdisplay)
#define xwindow		(xdata->xwindow)
#define xtheme		(xdata->xtheme)
#define xroot		(xdata->xroot)
#define xbg		(xdata->xbg)
#define xgc		(xdata->xgc)
#define xthemegc	(xdata->xthemegc)
#define xrootgc		(xdata->xrootgc)
#define xbggc		(xdata->xbggc)
#define xsfont		(xdata->xsfont)
#define xim		(xdata->xim)
#define xic		(xdata->xic)
#define xcompose	(xdata->xcompose)

#define xwindow_AllVisible	(xdata->xwindow_AllVisible)
#define xfont_map	(xdata->xfont_map)
#define xroot_flag	(xdata->xroot_flag)
#define xbg_flag	(xdata->xbg_flag)
#define xcol		(xdata->xcol)
#define xRequestor(j)	(xdata->xRequestor[j])
#define xReqPrivate(j)	(xdata->xReqPrivate[j])
#define xReqCount	(xdata->xReqCount)
#define XReqCount	(xdata->XReqCount)
#define XReq(j)		(xdata->XReq[j])
#define xWM_PROTOCOLS	(xdata->xWM_PROTOCOLS)
#define xWM_DELETE_WINDOW	(xdata->xWM_DELETE_WINDOW)
#define xTARGETS	(xdata->xTARGETS)
#define xsgc		(xdata->xsgc)
#define xthemesgc	(xdata->xthemesgc)

#include "hw_x/keyboard.h"


/* this can stay static, X11_FlushHW() is not reentrant */
static hwcol _col;


#define pitch 15

# define myXDrawImageString XDrawImageString16
# define myXDrawString XDrawString16
# define myXChar XChar2b


#define XDRAW_FN(FN, col, buf, buflen) do { \
    unsigned long __mask = 0; \
    if (xsgc.foreground != xcol[COLFG(col)]) \
	xsgc.foreground = xcol[COLFG(col)], __mask |= GCForeground; \
    if (xsgc.background != xcol[COLBG(col)]) \
	xsgc.background = xcol[COLBG(col)], __mask |= GCBackground; \
    if (__mask) \
	XChangeGC(xdisplay, xgc, __mask, &xsgc); \
    (FN)(xdisplay, xwindow, xgc, xbegin, ybegin + xupfont, buf, buflen); \
} while (0)

INLINE void X11_DrawImageString(myXChar *buf, uldat buflen, hwcol col, int xbegin, int ybegin) {
    XDRAW_FN(myXDrawImageString, col, buf, buflen);
}

INLINE void X11_DrawString(myXChar *buf, uldat buflen, hwcol col, int xbegin, int ybegin) {
    XDRAW_FN(myXDrawString, col, buf, buflen);
}

#undef XDRAW_FN

#define XDRAW_I(buf, buflen, col)    X11_DrawImageString(buf, buflen, col, xbegin, ybegin)
#define XDRAW_S(buf, buflen, col)    X11_DrawString(buf, buflen, col, xbegin, ybegin)
#define XDRAW_ROOT(buf, buflen, col) X11_DrawRoot(buf, buflen, col, xbegin, ybegin)
#define XDRAW_BG(buf, buflen, col)   X11_DrawBg(buf, buflen, col, xbegin, ybegin)
#define XDRAW_GFX(buf, buflen, col, gfx) (xmonochrome ? gfx_DrawMono : gfx_DrawColor)(buf, buflen, col, gfx, xbegin, ybegin)
#define XDRAW_GFX1(font, col, gfx)       (xmonochrome ? gfx_Draw1Mono : gfx_Draw1Color)(font, col, gfx, xbegin, ybegin)


# define IS_POS_TITLE(i, j)  ((i) == 4 && (j) >= 4 && (j) <= 6)
# define IS_POS_INSIDE(i, j) ((i) == 1 && (j) == 1)
# define IS_POS_MENU(i, j)   ((i) == 1 && (j) == 0)
# define IS_POS_ROOT(i, j)   ((i) == 0 && (j) == 1)

# define GFX_INSIDE  (1 + pitch)
# define GFX_MENU    (1)
# define GFX_ROOT    (pitch)

# define IS_GFX_TITLE(gfx)   ((gfx) % pitch == 4 && (gfx) / pitch >= 4 && (gfx) / pitch <= 6)
# define IS_GFX_INSIDE(gfx)  ((gfx) == GFX_INSIDE)
# define IS_GFX_MENU(gfx)    ((gfx) == GFX_MENU)
# define IS_GFX_ROOT(gfx)    ((gfx) == GFX_ROOT)

INLINE void gfx_Draw1Mono(myXChar *f, hwcol col, hwattr gfx, int xbegin, int ybegin) {
    int i = (gfx % pitch) * (ldat)xwfont;
    int j = (gfx / pitch) * (ldat)xhfont;
    unsigned long mask = 0;
    if (xthemesgc.foreground != xcol[COLFG(col)])
	xthemesgc.foreground = xcol[COLFG(col)], mask |= GCForeground;
    if (xthemesgc.background != xcol[COLBG(col)])
	xthemesgc.background = xcol[COLBG(col)], mask |= GCBackground;    
    if (xthemesgc.ts_x_origin != xbegin - i)
	xthemesgc.ts_x_origin = xbegin - i, mask |= GCTileStipXOrigin;
    if (xthemesgc.ts_y_origin != ybegin - j)
	xthemesgc.ts_y_origin = ybegin - j, mask |= GCTileStipYOrigin;
    if (mask)
	XChangeGC(xdisplay, xthemegc, mask, &xthemesgc);
    XFillRectangle(xdisplay, xwindow, xthemegc, xbegin, ybegin, xwfont, xhfont);
    
    if (IS_GFX_TITLE(gfx) || IS_GFX_INSIDE(gfx) || IS_GFX_MENU(gfx) || IS_GFX_ROOT(gfx)) {
	XDRAW_S(f, 1, col);
    }
}

static void gfx_DrawMono(myXChar *buf, udat buflen, hwcol col, hwattr gfx, int xbegin, int ybegin) {
    for (; buflen; buf++, buflen--, xbegin += xwfont) {
	gfx_Draw1Mono(buf, col, gfx, xbegin, ybegin);
    }
}

INLINE void X11_DrawRoot(myXChar *f, udat flen, hwcol col, int xbegin, int ybegin) {
    XFillRectangle(xdisplay, xwindow, xrootgc, xbegin, ybegin, xwfont * (uldat)flen, xhfont);
    XDRAW_S(f, flen, col);
}

INLINE void X11_DrawBg(myXChar *f, udat flen, hwcol col, int xbegin, int ybegin) {
    XFillRectangle(xdisplay, xwindow, xbggc, xbegin, ybegin, xwfont * (uldat)flen, xhfont);
    XDRAW_S(f, flen, col);
}


INLINE void gfx_Draw1Color(myXChar *f, hwcol col, hwattr gfx, int xbegin, int ybegin) {
    int i = (gfx % pitch) * (ldat)xwfont;
    int j = (gfx / pitch) * (ldat)xhfont;

    XCopyArea(xdisplay, xtheme, xwindow, xgc, i, j, xwfont, xhfont, xbegin, ybegin);
    
    if (IS_GFX_TITLE(gfx) || IS_GFX_INSIDE(gfx) || IS_GFX_MENU(gfx) || IS_GFX_ROOT(gfx)) {
	XDRAW_S(f, 1, col);
    }
}

static void gfx_DrawColor(myXChar *buf, udat buflen, hwcol col, hwattr gfx, int xbegin, int ybegin) {
    for (; buflen; buf++, buflen--, xbegin += xwfont) {
	gfx_Draw1Color(buf, col, gfx, xbegin, ybegin);
    }
}

#define XDRAW_ANY(buf, buflen, col, gfx) do { \
    if (!gfx || \
	    (IS_GFX_INSIDE(gfx) && (xbg_flag == GFX_USE_NONE || COLBG(col) != BLACK)) || \
	    (IS_GFX_ROOT(gfx) && xroot_flag == GFX_USE_NONE)) { \
	XDRAW_I(buf, buflen, col); \
    } else if ((IS_GFX_INSIDE(gfx) && xbg_flag == GFX_USE_ROOT) || \
		   (IS_GFX_ROOT(gfx) && xroot_flag == GFX_USE_ROOT)) { \
	XDRAW_ROOT(buf, buflen, col); \
    } else if ((IS_GFX_INSIDE(gfx) && xbg_flag == GFX_USE_BG) || \
		   (IS_GFX_ROOT(gfx) && xroot_flag == GFX_USE_BG)) { \
	XDRAW_BG(buf, buflen, col); \
    } else { \
	XDRAW_GFX(buf, buflen, col, gfx); \
    } \
} while (0)


INLINE void X11_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV, bufgfx;
    hwcol col;
    udat buflen = 0;
    hwattr gfx;
    hwfont f;
    myXChar buf[TW_SMALLBUFF];
    int xbegin, ybegin;
    
    if (xhw_view) {
	if (x >= xhw_endx || x + len < xhw_startx ||
	    y < xhw_starty || y >= xhw_endy)
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
    
    for (_col = ~HWCOL(*V); len; x++, V++, oV++, len--) {
	col = HWCOL(*V);
	gfx = HWEXTRA32(*V);
	if (buflen && (col != _col || gfx != bufgfx || (ValidOldVideo && *V == *oV) || buflen == TW_SMALLBUFF)) {
	    XDRAW_ANY(buf, buflen, _col, bufgfx);
	    buflen = 0;
	}
	if (!ValidOldVideo || *V != *oV) {
	    if (!buflen) {
		xbegin = (x - xhw_startx) * (ldat)xwfont;
		_col = col;
		bufgfx = gfx;
	    }
	    f = xUTF_16_to_charset(HWFONT(*V));
	    buf[buflen  ].byte1 = f >> 8;
	    buf[buflen++].byte2 = f & 0xFF;
	}
    }
    if (buflen) {
	XDRAW_ANY(buf, buflen, _col, bufgfx);
	buflen = 0;
    }
}

#include "hw_x/util.h"

#undef XDRAW_ANY


static byte gfx_LoadPixmap(Pixmap *px, byte *name, int nlen, byte strict) {
    byte *path[3] = { NULL, NULL, NULL }, *prefix[3], *infix[3];
    byte i, ret = FALSE;
    XpmAttributes pxattr;

    prefix[0] = HOME;		infix[0] = "/.twin/themes/hw_gfx/";
    prefix[1] = GFXDIR;		infix[1] = "/";
    prefix[2] = "";		infix[2] = "";
    
    /*
     * for monochrome themes, we must convert from a .xpm file with arbitrary depth
     * to a pixmap with depth == 1 to use it as stipple pattern
     */
    pxattr.valuemask = XpmDepth;
    pxattr.depth = xmonochrome ? 1 : DefaultDepth(xdisplay,  DefaultScreen(xdisplay));

    for (i = 0; i < 3 && !ret; i++) {
	if (prefix[i] && (path[i] = AllocMem(strlen(prefix[i]) + strlen(infix[i]) + nlen + 17))) {
	    if (strict)
		sprintf(path[i], "%s%s%.*s_%dx%d.xpm", prefix[i], infix[i], nlen, name, xwfont & 0xFFFF, xhfont & 0xFFFF);
	    else
		sprintf(path[i], "%s%s%.*s.xpm", prefix[i], infix[i], nlen, name);
	    
	    ret = XpmReadFileToPixmap(xdisplay, DefaultRootWindow(xdisplay), path[i], px, NULL, &pxattr)
		== XpmSuccess;
	}
    }

    if (!ret) {
	if (!path[0] && !path[1] && !path[2])
	    printk("      gfx_InitHW(): Out of memory!\n");
	else
	    printk("      gfx_InitHW(): failed to open `%."STR(TW_SMALLBUFF)"s': %."STR(TW_SMALLBUFF)"s\n",
		   path[0] ? path[0] : path[1] ? path[1] : path[2],
		   strerror(errno));
    }
    
    for (i = 0; i < 3 && !ret; i++) {
	if (path[i])
	    FreeMem(path[i]);
    }

    return ret;
}

static byte *GfxFile(byte *arg, byte **ret_file, uldat *ret_len) {
    *ret_file = arg;
    arg = strchr(arg, ',');
    *ret_len = arg ? arg - *ret_file : strlen(*ret_file);

#define TRIM(str, len) \
    if (*ret_len >= (len) && !memcmp(*ret_file + *ret_len - (len), (str), (len))) \
	*ret_len -= (len)
    
    TRIM(".xpm", 4);
    else TRIM(".xpm.Z", 6);
    else TRIM(".xpm.gz", 7);

#undef TRIM

    return arg;
}

static void GfxUse(byte *arg, byte *how) {
    if (!strncmp(arg, "none", 4))
	*how = GFX_USE_NONE;
    else if (!strncmp(arg, "theme", 5))
	*how = GFX_USE_THEME;
    else if (!strncmp(arg, "root", 4))
	*how = GFX_USE_ROOT;
    else if (!strncmp(arg, "bg", 2))
	*how = GFX_USE_BG;
}

typedef struct {
    byte *dpy, *dpy0, *fontname, *fontname0;
    byte *charset, *charset0, *file_bg, *file_root, *file_theme;
    uldat file_bg_len, file_root_len, file_theme_len;
    udat fontwidth, fontheight;
    byte drag, noinput;
} gfx_options;


static byte gfx_ParseOptions(gfx_options * opt, char * arg) {
    byte * s;
    
    /* autodetect */
    xmonochrome = TRUE + TRUE;
    
    /* conservative default settings to reduce CPU usage */
    xroot_flag = xbg_flag = GFX_USE_NONE;

    /* default: show the whole screen */
    xhw_view = xhw_startx = xhw_starty = xhw_endx = xhw_endy = 0;

    xsfont = NULL;
    xwindow = None; xgc = None;
    xReqCount = XReqCount = 0;
    HW->keyboard_slot = NOSLOT;
    
    if (!arg || !*arg)
        goto cleanup;
    else if (strncmp(arg, "-hw=gfx", 7))
        return FALSE;
    
    arg += 7; /* skip "-hw=gfx" */
	
    if (*arg == '@') {
        if ((s = strchr(opt->dpy = ++arg, ','))) {
            *(opt->dpy0 = s) = '\0';
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
            opt->fontname = arg += 5;
            s = strchr(arg, ',');
            if (s)
                *(opt->fontname0 = s++) = '\0';
            arg = s;
        } else if (!strncmp(arg, "fontsize=", 9)) {
            int n1 = atoi(arg += 9), n2 = 0;
            byte ch;
            if (n1 > 0) {
                while ((ch = (byte)*++arg) && ch != ',') {
                    if (ch == 'x') {
                        n2 = atoi(arg+1);
                        break;
                    }
                }
                opt->fontwidth = Min2(TW_MAXUDAT, n2 > 0 ? n1 : n1 / 2);
                opt->fontheight = Min2(TW_MAXUDAT, n2 > 0 ? n2 : n1);
            }
            arg = strchr(arg, ',');
        } else if (!strncmp(arg, "charset=", 8)) {
            opt->charset = arg += 8;
            s = strchr(arg, ',');
            if (s)
                *(opt->charset0 = s++) = '\0';
            arg = s;
        } else if (!strncmp(arg, "view=", 5)) {
            xhw_view = 1;
            xhw_endx = strtol(arg+5, &arg, 0);
            xhw_endy = strtol(arg+1, &arg, 0);
            xhw_startx = strtol(arg+1, &arg, 0);
            xhw_starty = strtol(arg+1, &arg, 0);
            xhw_endx += xhw_startx;
            xhw_endy += xhw_starty;
        } else if (!strncmp(arg, "drag", 4)) {
            arg += 4;
            opt->drag = TRUE;
        } else if (!strncmp(arg, "noinput", 7)) {
            arg += 7;
            opt->noinput = TRUE;
        } else if (!strncmp(arg, "color", 5)) {
            arg += 5;
            xmonochrome = FALSE;
        } else if (!strncmp(arg, "mono", 4)) {
            arg += 4;
            xmonochrome = TRUE;
        } else if (!strncmp(arg, "bg=", 3)) {
            arg = GfxFile(arg + 3, &opt->file_bg, &opt->file_bg_len);
            xbg_flag = GFX_USE_BG;
        } else if (!strncmp(arg, "root=", 5)) {
            arg = GfxFile(arg + 5, &opt->file_root, &opt->file_root_len);
            xroot_flag = GFX_USE_ROOT;
        } else if (!strncmp(arg, "theme=", 6)) {
            arg = GfxFile(arg + 6, &opt->file_theme, &opt->file_theme_len);
        } else if (!strncmp(arg, "bg-as-", 6)) {
            arg += 6;
            GfxUse(arg, &xbg_flag);
            arg = strchr(arg, ',');
        } else if (!strncmp(arg, "root-as-", 8)) {
            arg += 8;
            GfxUse(arg, &xroot_flag);
            arg = strchr(arg, ',');
        } else
            arg = strchr(arg, ',');
    }

cleanup:
    if (xmonochrome == TRUE + TRUE)
	xmonochrome = opt->file_theme_len >= 4 && !strcmp(opt->file_theme + opt->file_theme_len - 4, "mono");

    /* sanity checks: must specify pixmap name to use it */
    if (!opt->file_root) {
	if (xroot_flag == GFX_USE_ROOT)
	    xroot_flag = GFX_USE_THEME;
	if (xbg_flag == GFX_USE_ROOT)
	    xbg_flag = GFX_USE_THEME;
    }
    if (!opt->file_bg) {
	if (xroot_flag == GFX_USE_BG)
	    xroot_flag = GFX_USE_THEME;
	if (xbg_flag == GFX_USE_BG)
	    xbg_flag = GFX_USE_THEME;
    }
    
    /* root and background not supported in monochrome mode */
    if (xmonochrome) {
	if (xroot_flag > GFX_USE_THEME)
	    xroot_flag = GFX_USE_THEME;
	if (xbg_flag > GFX_USE_THEME)
	    xbg_flag = GFX_USE_THEME;
    }

    /* sanity checks: clear pixmap name if not used */
    if (xroot_flag != GFX_USE_ROOT && xbg_flag != GFX_USE_ROOT)
	opt->file_root = NULL;
    if (xroot_flag != GFX_USE_BG && xbg_flag != GFX_USE_BG)
	opt->file_bg = NULL;
    
    /* XCopyArea() not supported if using background or root */
    if (xroot_flag > GFX_USE_THEME || xbg_flag > GFX_USE_THEME)
	opt->drag = FALSE;

    return TRUE;
}

/* return name of selected font in allocated (char *) */
static char * gfx_AutodetectFont(udat fontwidth, udat fontheight) {
    CONST char * patterns[] = {
        "-misc-console-medium-r-normal-*-%u-*-*-*-*-*-iso10646-1",
        "-misc-fixed-medium-r-normal-*-%u-*-*-*-*-*-iso10646-1",
        "-*-*-medium-r-normal-*-%u-*-*-*-*-*-iso10646-1",
        "-*-*-medium-r-normal-*-%u-*-*-*-*-*-*-cp437",
        "-*-*-medium-r-normal-*-%u-*-*-*-*-*-*-cp850",
        "-*-*-medium-r-normal-*-%u-*-*-*-*-*-ibm-850",
        "-*-*-medium-r-normal-*-%u-*-*-*-*-*-iso8859-*",
    };
    enum { max_fonts = 100, n_patterns = sizeof(patterns)/sizeof(patterns[0]) };
    
    XFontStruct *info;
    char * pattern = AllocMem(LenStr(patterns[0]) + 1 + 3 * sizeof(unsigned));
    char * selected = NULL;
    char ** names = NULL;
    int i, j, n_fonts;
    if (!pattern)
        return NULL;
    
    for (i = 0; i < n_patterns && !selected; i++) {
        n_fonts = 0;
        info = NULL;
        sprintf(pattern, patterns[i], (unsigned)fontheight);
        names = XListFontsWithInfo(xdisplay, pattern, max_fonts, &n_fonts, &info);

        if (names == NULL)
            continue;

        for (j = 0; j < n_fonts && !selected; j++) {
            uldat width = info[j].max_bounds.width;
            
            if (width == fontwidth && width == info[j].min_bounds.width
                && fontheight == info[j].ascent + info[j].descent
                && info[j].direction == FontLeftToRight
                && info[j].min_byte1 == 0 && info[j].min_char_or_byte2 <= 32)
            {
                selected = CloneStr(names[j]);
            }
        }
        XFreeFontInfo(names, info, n_fonts);
    }
    FreeMem(pattern);
    return selected;
}


static byte gfx_LoadFont(CONST char * fontname, udat fontwidth, udat fontheight) {
    char * alloc_fontname = 0;
    byte loaded = FALSE;

    if (!fontname)
        fontname = alloc_fontname = gfx_AutodetectFont(fontwidth, fontheight);
    
    if ((fontname && (xsfont = XLoadQueryFont(xdisplay, fontname)))
        || (xsfont = XLoadQueryFont(xdisplay, fontname = "vga"))
        || (xsfont = XLoadQueryFont(xdisplay, fontname = "fixed")))
    {
        loaded = TRUE;

        xwfont = xsfont->min_bounds.width;
        xwidth = xwfont * (unsigned)(HW->X = GetDisplayWidth());
        xhfont = (xupfont = xsfont->ascent) + xsfont->descent;
        xheight = xhfont * (unsigned)(HW->Y = GetDisplayHeight());
        
        printk("      selected %ux%u font `%."STR(TW_SMALLBUFF)"s'\n", (unsigned)xwfont, (unsigned)xhfont, fontname);
    }
    if (alloc_fontname)
        FreeMem(alloc_fontname);

    return loaded;
}

static void gfx_QuitHW(void);

static byte gfx_InitHW(void) {
    gfx_options opt;
    byte title[X11_TITLE_MAXLEN];
    int i, xscreen;
    unsigned int xdepth;
    
    XSetWindowAttributes xattr;
    XColor xcolor;
    XSizeHints *xhints = NULL;
    XEvent event;
    XGCValues xs_gc;
    
    if (!(HW->Private = (struct x11_data *)AllocMem0(sizeof(struct x11_data), 1))) {
	printk("      gfx_InitHW(): Out of memory!\n");
	return FALSE;
    }

    /* not yet opened */
    xdisplay = (Display *)0;
    
    WriteMem(&opt, '\0', sizeof(gfx_options));
    opt.fontwidth = 8;
    opt.fontheight = 16;
    opt.file_theme = "default";
    opt.file_theme_len = strlen(opt.file_theme);
    
    if (!gfx_ParseOptions(&opt, HW->Name))
        goto fail;
    
    if ((xdisplay = XOpenDisplay(opt.dpy))) do {
	
	(void)XSetIOErrorHandler(X11_Die);

	if (!X11_CheckRemapKeys())
	    break;

	xscreen = DefaultScreen(xdisplay);
	xdepth  = DefaultDepth(xdisplay, xscreen);
	
	for (i = 0; i <= MAXCOL; i++) {
	    xcolor.red   = 257 * (udat)Palette[i].Red;
	    xcolor.green = 257 * (udat)Palette[i].Green;
	    xcolor.blue  = 257 * (udat)Palette[i].Blue;
	    if (!XAllocColor(xdisplay, DefaultColormap(xdisplay, xscreen), &xcolor)) {
		printk("      gfx_InitHW() failed to allocate colors\n");
		break;
	    }
	    xcol[i] = xcolor.pixel;
	}
	if (i <= MAXCOL)
	    break;
	
	xattr.background_pixel = xcol[0];
	xattr.event_mask = ExposureMask | VisibilityChangeMask |
	    StructureNotifyMask | SubstructureNotifyMask |
	    KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

        if (gfx_LoadFont(opt.fontname, opt.fontwidth, opt.fontheight) &&
            
            (xwindow = XCreateWindow(xdisplay, DefaultRootWindow(xdisplay), 0, 0,
				     xwidth, xheight, 0, xdepth, InputOutput,
				     DefaultVisual(xdisplay, xscreen),
				     CWBackPixel | CWEventMask, &xattr)) &&
	    
	    (xsgc.foreground = xsgc.background = xcol[0],
	     xsgc.graphics_exposures = False,
	     xsgc.font = xsfont->fid,
	     xgc = XCreateGC(xdisplay, xwindow, GCFont|GCForeground|
			     GCBackground|GCGraphicsExposures, &xsgc)) &&
	    
	    gfx_LoadPixmap(&xtheme, opt.file_theme, opt.file_theme_len, TRUE) &&
	    (!opt.file_root || gfx_LoadPixmap(&xroot, opt.file_root, opt.file_root_len, FALSE)) &&
	    (!opt.file_bg || gfx_LoadPixmap(&xbg, opt.file_bg, opt.file_bg_len, FALSE)) &&
	    
	    (xmonochrome ?
	     (xthemesgc.foreground = xthemesgc.background = xcol[0],
	      xthemesgc.graphics_exposures = False,
	      xthemesgc.stipple = xtheme,
	      xthemesgc.fill_style = FillOpaqueStippled,
	      !!(xthemegc = XCreateGC(xdisplay, xwindow, GCStipple|GCFillStyle|
				      GCForeground|GCBackground|GCGraphicsExposures, &xthemesgc)))
	     : TRUE) &&
	    
	    (opt.file_root ?
	     (xs_gc.foreground = xs_gc.background = xcol[0],
	      xs_gc.graphics_exposures = False,
	      xs_gc.tile = xroot,
	      xs_gc.fill_style = FillTiled,
	      xs_gc.ts_x_origin = xs_gc.ts_y_origin = 0,
	      !!(xrootgc = XCreateGC(xdisplay, xwindow, GCTile|GCFillStyle|GCTileStipXOrigin|GCTileStipYOrigin|
				     GCForeground|GCBackground|GCGraphicsExposures, &xs_gc)))
	     : TRUE) &&
	    
	    (opt.file_bg ?
	     (xs_gc.foreground = xs_gc.background = xcol[0],
	      xs_gc.graphics_exposures = False,
	      xs_gc.tile = xbg,
	      xs_gc.fill_style = FillTiled,
	      xs_gc.ts_x_origin = xs_gc.ts_y_origin = 0,
	      !!(xbggc = XCreateGC(xdisplay, xwindow, GCTile|GCFillStyle|GCTileStipXOrigin|GCTileStipYOrigin|
				   GCForeground|GCBackground|GCGraphicsExposures, &xs_gc)))
	     : TRUE) &&
	    
	    (xhints = XAllocSizeHints()))
        {

            static XComposeStatus static_xcompose;
            xcompose = static_xcompose;

#ifdef TW_FEATURE_X11_XIM_XIC
            xim = XOpenIM(xdisplay, NULL, NULL, NULL);
            if (xim != NULL) {
                xic = XCreateIC(xim, XNInputStyle, XIMStatusNothing|XIMPreeditNothing,
                                XNClientWindow, xwindow, XNFocusWindow, xwindow, NULL);
                if (xic == NULL) {
                    XCloseIM(xim);
                    xim = NULL;
                }
            } else
                xic = NULL;
#endif            
            X11_FillWindowTitle(title, sizeof(title));
	    XStoreName(xdisplay, xwindow, title);
	    
	    
	    if (!(xUTF_16_to_charset = X11_UTF_16_to_charset_function(opt.charset)))
		xUTF_16_to_charset = X11_UTF_16_to_UTF_16;
	    /*
	     * ask ICCCM-compliant window manager to tell us when close window
	     * has been chosen, rather than just killing us
	     */
	    xWM_PROTOCOLS = XInternAtom(xdisplay, "WM_PROTOCOLS", False);
	    xWM_DELETE_WINDOW = XInternAtom(xdisplay, "WM_DELETE_WINDOW", False);
	    xTARGETS = XInternAtom(xdisplay, "TARGETS", False);

	    XChangeProperty(xdisplay, xwindow, xWM_PROTOCOLS, XA_ATOM, 32, PropModeReplace,
			    (unsigned char *) &xWM_DELETE_WINDOW, 1);
	    
	    if (xhw_view) {
		xhints->flags = PMinSize|PMaxSize;
		xhints->min_width = xhints->max_width = xwidth;
		xhints->min_height = xhints->max_height = xheight;
	    } else {
		xhints->flags = PResizeInc;
		xhints->width_inc  = xwfont;
		xhints->height_inc = xhfont;
	    }
	    XSetWMNormalHints(xdisplay, xwindow, xhints);
	    
	    XMapWindow(xdisplay, xwindow);
	    
	    do {
		XNextEvent(xdisplay, &event);
	    } while (event.type != MapNotify);
	    
	    XFree(xhints); xhints = NULL;
	    
	    HW->mouse_slot = NOSLOT;
	    HW->keyboard_slot = RegisterRemote(i = XConnectionNumber(xdisplay), (obj)HW,
					       X11_KeyboardEvent);
	    if (HW->keyboard_slot == NOSLOT)
		break;
	    fcntl(i, F_SETFD, FD_CLOEXEC);
	    
	    HW->FlushVideo = X11_FlushVideo;
	    HW->FlushHW = X11_FlushHW;
	    
	    HW->KeyboardEvent = X11_KeyboardEvent;
	    HW->MouseEvent = (void *)NoOp; /* mouse events handled by X11_KeyboardEvent */
	    
	    HW->XY[0] = HW->XY[1] = 0;
	    HW->TT = NOCURSOR;
	    
	    HW->ShowMouse = NoOp;
	    HW->HideMouse = NoOp;
	    HW->UpdateMouseAndCursor = NoOp;
	    HW->MouseState.x = HW->MouseState.y = HW->MouseState.keys = 0;
	    
	    HW->DetectSize  = X11_DetectSize;
	    HW->CheckResize = X11_CheckResize;
	    HW->Resize      = X11_Resize;
	    
	    HW->HWSelectionImport  = X11_SelectionImport_X11;
	    HW->HWSelectionExport  = X11_SelectionExport_X11;
	    HW->HWSelectionRequest = X11_SelectionRequest_X11;
	    HW->HWSelectionNotify  = X11_SelectionNotify_X11;
	    HW->HWSelectionPrivate = 0;
	    
	    if (opt.drag) {
		HW->CanDragArea = X11_CanDragArea;
		HW->DragArea    = X11_DragArea;
	    } else
		HW->CanDragArea = NULL;
	    
	    HW->Beep = X11_Beep;
	    HW->Configure = X11_Configure;
	    HW->SetPalette = (void *)NoOp;
	    HW->ResetPalette = NoOp;
	    
	    HW->QuitHW = gfx_QuitHW;
	    HW->QuitKeyboard  = NoOp;
	    HW->QuitMouse = NoOp;
	    HW->QuitVideo = NoOp;
	    
	    HW->DisplayIsCTTY = FALSE;
	    HW->FlagsHW &= ~FlHWSoftMouse; /* mouse pointer handled by X11 server */
	    
	    HW->FlagsHW |= FlHWNeedOldVideo;
	    HW->FlagsHW |= FlHWExpensiveFlushVideo;
	    if (opt.noinput)
		HW->FlagsHW |= FlHWNoInput;
	    
	    HW->NeedHW = 0;
	    HW->CanResize = TRUE;
	    HW->merge_Threshold = 0;
	    
	    /*
	     * we must draw everything on our new shiny window
	     * without forcing all other displays
	     * to redraw everything too.
	     */
	    HW->RedrawVideo = FALSE;
	    NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);
	    
	    if (opt.dpy0)      *opt.dpy0      = ',';
	    if (opt.fontname0) *opt.fontname0 = ',';
	    if (opt.charset0)  *opt.charset0  = ',';
	    
	    return TRUE;
	}
    } while (0); else {
	if (opt.dpy || (opt.dpy = getenv("DISPLAY")))
	    printk("      gfx_InitHW() failed to open display %."STR(TW_SMALLBUFF)"s\n", HW->Name);
	else
	    printk("      gfx_InitHW() failed: DISPLAY is not set\n");
    }

fail:
    if (opt.dpy0)       *opt.dpy0       = ',';
    if (opt.fontname0)  *opt.fontname0  = ',';
    if (opt.charset0)   *opt.charset0   = ',';
	
    if (xdisplay)
	gfx_QuitHW();

    FreeMem(HW->Private);
    HW->Private = NULL;
    
    return FALSE;
}

static void gfx_QuitHW(void) {

#ifdef TW_FEATURE_X11_XIM_XIC
    if (xic)              XDestroyIC(xic);
    if (xim)              XCloseIM(xim);
#endif
    if (xsfont)           XFreeFont(xdisplay, xsfont);
    if (xgc      != None) XFreeGC(xdisplay, xgc);
    if (xthemegc != None) XFreeGC(xdisplay, xthemegc);
    if (xroot    != None) XFreePixmap(xdisplay, xroot);
    if (xbg      != None) XFreePixmap(xdisplay, xbg);
    if (xwindow  != None) {
	XUnmapWindow(xdisplay, xwindow);
	XDestroyWindow(xdisplay, xwindow);
    }
    XCloseDisplay(xdisplay);
    xdisplay = NULL;
    
    if (HW->keyboard_slot != NOSLOT)
	UnRegisterRemote(HW->keyboard_slot);
    HW->keyboard_slot = NOSLOT;
    HW->KeyboardEvent = (void *)NoOp;
    
    HW->QuitHW = NoOp;

    FreeMem(HW->Private);
    HW->Private = NULL;
}

byte InitModule(module Module) {
    Module->Private = gfx_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module Module) {
}
