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

#ifdef DATADIR
# define GFXDIR DATADIR "/twin/themes/hw_gfx"
#else
# define GFXDIR "themes/hw_gfx"
#endif

#include <Tw/Twkeys.h>

#include <Tutf/Tutf.h>
#include <Tutf/Tutf_defs.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>                /* CARD32 */

#include <X11/xpm.h>

#include <unistd.h>

#define HW_THIS "hw_gfx"
#define THIS 	"gfx"
#define INIT_THIS gfx_InitHW


#include "hw_X11_gfx_common0.h"


/* Display variables */

#define NEST 4

/* how to draw screen root and widgets interior: */
#define GFX_USE_NONE	0	/* no pixmap */
#define GFX_USE_THEME	1	/* use pixmap tile from theme */
#define GFX_USE_ROOT	2	/* use root pixmap */
#define GFX_USE_BG	3	/* use background pixmap */


struct x11_data {
    unsigned int xwidth, xheight;
    int xwfont, xhfont, xupfont;

    /* we support showing only a portion of the whole twin display */
    dat xhw_view, xhw_startx, xhw_starty, xhw_endx, xhw_endy;
    
    int xmonochrome;
#ifdef CONF__UNICODE
    Tutf_function xUTF_16_to_charset;
#endif
    Display     *xdisplay;
    Window       xwindow;
    Pixmap       xtheme, xroot, xbg;
    GC           xgc, xthemegc, xrootgc, xbggc;
    XFontStruct *xsfont;
#ifdef HW_X11_HAVE_XIM_XIC
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

#include "hw_X11_gfx_common1.h"


/* this can stay static, X11_FlushHW() is not reentrant */
static hwcol _col;


#define pitch 15

#ifdef CONF__UNICODE
# define myXDrawImageString XDrawImageString16
# define myXDrawString XDrawString16
# define myXChar XChar2b
#else
# define myXDrawImageString XDrawImageString
# define myXDrawString XDrawString
# define myXChar byte
#endif

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
#define XDRAW_GFX(buf, buflen, col, gfx) (xmonochrome ? X11_DrawGfxMono : X11_DrawGfxColor)(buf, buflen, col, gfx, xbegin, ybegin)
#define XDRAW_GFX1(font, col, gfx)       (xmonochrome ? X11_DrawGfx1Mono : X11_DrawGfx1Color)(font, col, gfx, xbegin, ybegin)


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

INLINE void X11_DrawGfx1Mono(myXChar *f, hwcol col, hwattr gfx, int xbegin, int ybegin) {
    int i = (gfx % pitch) * xwfont;
    int j = (gfx / pitch) * xhfont;
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

static void X11_DrawGfxMono(myXChar *buf, udat buflen, hwcol col, hwattr gfx, int xbegin, int ybegin) {
    for (; buflen; buf++, buflen--, xbegin += xwfont) {
	X11_DrawGfx1Mono(buf, col, gfx, xbegin, ybegin);
    }
}

INLINE void X11_DrawRoot(myXChar *f, udat flen, hwcol col, int xbegin, int ybegin) {
    XFillRectangle(xdisplay, xwindow, xrootgc, xbegin, ybegin, xwfont * flen, xhfont);
    XDRAW_S(f, flen, col);
}

INLINE void X11_DrawBg(myXChar *f, udat flen, hwcol col, int xbegin, int ybegin) {
    XFillRectangle(xdisplay, xwindow, xbggc, xbegin, ybegin, xwfont * flen, xhfont);
    XDRAW_S(f, flen, col);
}


INLINE void X11_DrawGfx1Color(myXChar *f, hwcol col, hwattr gfx, int xbegin, int ybegin) {
    int i = (gfx % pitch) * xwfont;
    int j = (gfx / pitch) * xhfont;

    XCopyArea(xdisplay, xtheme, xwindow, xgc, i, j, xwfont, xhfont, xbegin, ybegin);
    
    if (IS_GFX_TITLE(gfx) || IS_GFX_INSIDE(gfx) || IS_GFX_MENU(gfx) || IS_GFX_ROOT(gfx)) {
	XDRAW_S(f, 1, col);
    }
}

static void X11_DrawGfxColor(myXChar *buf, udat buflen, hwcol col, hwattr gfx, int xbegin, int ybegin) {
    for (; buflen; buf++, buflen--, xbegin += xwfont) {
	X11_DrawGfx1Color(buf, col, gfx, xbegin, ybegin);
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
#ifdef CONF__UNICODE
    hwfont f;
    XChar2b buf[SMALLBUFF];
#else
    byte buf[SMALLBUFF];
#endif
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

    xbegin = (x - xhw_startx) * xwfont;
    ybegin = (y - xhw_starty) * xhfont;
    
    V = Video + x + y * DisplayWidth;
    oV = OldVideo + x + y * DisplayWidth;
    
    for (_col = ~HWCOL(*V); len; x++, V++, oV++, len--) {
	col = HWCOL(*V);
	gfx = HWEXTRA32(*V);
	if (buflen && (col != _col || gfx != bufgfx || (ValidOldVideo && *V == *oV) || buflen == SMALLBUFF)) {
	    XDRAW_ANY(buf, buflen, _col, bufgfx);
	    buflen = 0;
	}
	if (!ValidOldVideo || *V != *oV) {
	    if (!buflen) {
		xbegin = (x - xhw_startx) * xwfont;
		_col = col;
		bufgfx = gfx;
	    }
#ifdef CONF__UNICODE
	    f = xUTF_16_to_charset(HWFONT(*V));
	    buf[buflen  ].byte1 = f >> 8;
	    buf[buflen++].byte2 = f & 0xFF;
#else
	    buf[buflen++] = HWFONT(*V);
#endif
	}
    }
    if (buflen) {
	XDRAW_ANY(buf, buflen, _col, bufgfx);
	buflen = 0;
    }
}

#include "hw_X11_gfx_common2.h"

#undef XDRAW_ANY


static void X11_QuitHW(void) {

#ifdef HW_X11_HAVE_XIM_XIC
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
}

static byte X11_LoadPixmap(Pixmap *px, byte *name, int nlen, byte strict) {
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
	    printk("      gfx_InitHW(): failed to open `%."STR(SMALLBUFF)"s': %."STR(SMALLBUFF)"s\n",
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

#ifdef THIS_MODULE
static
#endif
byte gfx_InitHW(void) {
    char *arg = HW->Name;
    int xscreen;
    unsigned int xdepth;
    int i;
    byte *s, *dpy = NULL, *dpy0 = NULL,
	*fontname = NULL, *fontname0 = NULL,
	*charset = NULL, *charset0 = NULL,
	*file_bg = NULL, *file_root = NULL,
	*file_theme = "default",
	name[] = "twin :??? on gfx";
    uldat file_bg_len, file_root_len, file_theme_len = strlen(file_theme);
    
    byte drag = FALSE, noinput = FALSE;
    
    XSetWindowAttributes xattr;
    XColor xcolor;
    XSizeHints *xhints;
    XEvent event;
    XGCValues xs_gc;
    
    if (!(HW->Private = (struct x11_data *)AllocMem(sizeof(struct x11_data)))) {
	printk("      gfx_InitHW(): Out of memory!\n");
	return FALSE;
    }
    
    /* autodetect */
    xmonochrome = TRUE + TRUE;
    
    /* conservative default settings to reduce CPU usage */
    xroot_flag = xbg_flag = GFX_USE_NONE;

    /* default: show the whole screen */
    xhw_view = xhw_startx = xhw_starty = xhw_endx = xhw_endy = 0;

    if (arg && HW->NameLen > 4) {
	arg += 4; /* skip "-hw=" */
	
	if (strncmp(arg, "gfx", 3))
	    return FALSE; /* user said "use <arg> as display, not gfx" */
	
	/* skip "gfx" */
	arg += 3;
	
	if (*arg == '@') {
	    if ((s = strchr(dpy = ++arg, ','))) {
		*(dpy0 = s) = '\0';
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
		if (s) *(fontname0 = s++) = '\0';
		arg = s;
	    } else if (!strncmp(arg, "charset=", 8)) {
		charset = arg += 8;
		s = strchr(arg, ',');
		if (s) *(charset0 = s++) = '\0';
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
		drag = TRUE;
	    } else if (!strncmp(arg, "noinput", 7)) {
		arg += 7;
		noinput = TRUE;
	    } else if (!strncmp(arg, "color", 5)) {
		arg += 5;
		xmonochrome = FALSE;
	    } else if (!strncmp(arg, "mono", 4)) {
		arg += 4;
		xmonochrome = TRUE;
	    } else if (!strncmp(arg, "bg=", 3)) {
		arg = GfxFile(arg + 3, &file_bg, &file_bg_len);
		xbg_flag = GFX_USE_BG;
	    } else if (!strncmp(arg, "root=", 5)) {
		arg = GfxFile(arg + 5, &file_root, &file_root_len);
		xroot_flag = GFX_USE_ROOT;
	    } else if (!strncmp(arg, "theme=", 6)) {
		arg = GfxFile(arg + 6, &file_theme, &file_theme_len);
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
    }
    
    xsfont = NULL; xhints = NULL;
    xwindow = None; xgc = None;
    xReqCount = XReqCount = 0;
    HW->keyboard_slot = NOSLOT;
    
    if (xmonochrome == TRUE + TRUE)
	xmonochrome = file_theme_len >= 4 && !strcmp(file_theme + file_theme_len - 4, "mono");

    /* sanity checks: must specify pixmap name to use it */
    if (!file_root) {
	if (xroot_flag == GFX_USE_ROOT)
	    xroot_flag = GFX_USE_THEME;
	if (xbg_flag == GFX_USE_ROOT)
	    xbg_flag = GFX_USE_THEME;
    }
    if (!file_bg) {
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
	file_root = NULL;
    if (xroot_flag != GFX_USE_BG && xbg_flag != GFX_USE_BG)
	file_bg = NULL;

    
    /* XCopyArea() not supported if using background or root */
    if (xroot_flag > GFX_USE_THEME || xbg_flag > GFX_USE_THEME)
	drag = FALSE;
    
    if ((xdisplay = XOpenDisplay(dpy))) do {
	
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

        if (((fontname && (xsfont = XLoadQueryFont(xdisplay, fontname))) ||
	     (xsfont = XLoadQueryFont(xdisplay, "vga")) ||
	     (xsfont = XLoadQueryFont(xdisplay, "fixed"))) &&
	    (xwfont = xsfont->min_bounds.width,
	     xwidth = xwfont * (HW->X = GetDisplayWidth()),
	     xhfont = (xupfont = xsfont->ascent) + xsfont->descent,
	     xheight = xhfont * (HW->Y = GetDisplayHeight()),
	     xwindow = XCreateWindow(xdisplay, DefaultRootWindow(xdisplay), 0, 0,
				     xwidth, xheight, 0, xdepth, InputOutput,
				     DefaultVisual(xdisplay, xscreen),
				     CWBackPixel | CWEventMask, &xattr)) &&
	    
	    (xsgc.foreground = xsgc.background = xcol[0],
	     xsgc.graphics_exposures = False,
	     xsgc.font = xsfont->fid,
	     xgc = XCreateGC(xdisplay, xwindow, GCFont|GCForeground|
			     GCBackground|GCGraphicsExposures, &xsgc)) &&
	    
	    X11_LoadPixmap(&xtheme, file_theme, file_theme_len, TRUE) &&
	    (!file_root || X11_LoadPixmap(&xroot, file_root, file_root_len, FALSE)) &&
	    (!file_bg || X11_LoadPixmap(&xbg, file_bg, file_bg_len, FALSE)) &&
	    
	    (xmonochrome ?
	     (xthemesgc.foreground = xthemesgc.background = xcol[0],
	      xthemesgc.graphics_exposures = False,
	      xthemesgc.stipple = xtheme,
	      xthemesgc.fill_style = FillOpaqueStippled,
	      !!(xthemegc = XCreateGC(xdisplay, xwindow, GCStipple|GCFillStyle|
				      GCForeground|GCBackground|GCGraphicsExposures, &xthemesgc)))
	     : TRUE) &&
	    
	    (file_root ?
	     (xs_gc.foreground = xs_gc.background = xcol[0],
	      xs_gc.graphics_exposures = False,
	      xs_gc.tile = xroot,
	      xs_gc.fill_style = FillTiled,
	      xs_gc.ts_x_origin = xs_gc.ts_y_origin = 0,
	      !!(xrootgc = XCreateGC(xdisplay, xwindow, GCTile|GCFillStyle|GCTileStipXOrigin|GCTileStipYOrigin|
				     GCForeground|GCBackground|GCGraphicsExposures, &xs_gc)))
	     : TRUE) &&
	    
	    (file_bg ?
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
            
#ifdef HW_X11_HAVE_XIM_XIC
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
	    sprintf(name, "twin %s on gfx", TWDisplay);
	    XStoreName(xdisplay, xwindow, name);
	    
	    
#ifdef CONF__UNICODE
	    if (!(xUTF_16_to_charset = X11_UTF_16_to_charset_function(charset)))
		xUTF_16_to_charset = X11_UTF_16_to_UTF_16;
#endif
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
	    
	    if (drag) {
		HW->CanDragArea = X11_CanDragArea;
		HW->DragArea    = X11_DragArea;
	    } else
		HW->CanDragArea = NULL;
	    
	    HW->Beep = X11_Beep;
	    HW->Configure = X11_Configure;
	    HW->SetPalette = (void *)NoOp;
	    HW->ResetPalette = NoOp;
	    
	    HW->QuitHW = X11_QuitHW;
	    HW->QuitKeyboard  = NoOp;
	    HW->QuitMouse = NoOp;
	    HW->QuitVideo = NoOp;
	    
	    HW->DisplayIsCTTY = FALSE;
	    HW->FlagsHW &= ~FlHWSoftMouse; /* mouse pointer handled by X11 server */
	    
	    HW->FlagsHW |= FlHWNeedOldVideo;
	    HW->FlagsHW |= FlHWExpensiveFlushVideo;
	    if (noinput)
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
	    
	    if (dpy0) *dpy0 = ',';
	    if (fontname0) *fontname0 = ',';
	    if (charset0) *charset0 = ',';
	    
	    return TRUE;
	}
    } while (0); else {
	if (dpy || (dpy = getenv("DISPLAY")))
	    printk("      gfx_InitHW() failed to open display %."STR(SMALLBUFF)"s\n", HW->Name);
	else
	    printk("      gfx_InitHW() failed: DISPLAY is not set\n");
    }
	
    if (dpy0)       *dpy0       = ',';
    if (fontname0)  *fontname0  = ',';
    if (charset0)   *charset0   = ',';
	
    if (xdisplay)
	X11_QuitHW();

    FreeMem(HW->Private);
    
    return FALSE;
}

#ifdef THIS_MODULE

byte InitModule(module Module) {
    Module->Private = gfx_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module Module) {
}

#endif /* THIS_MODULE */
