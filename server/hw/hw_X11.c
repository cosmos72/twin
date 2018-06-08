/*
 *  hw_X11.c  --  functions to let twin display on X11
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

#define THIS "hw_X11"

#include "hw_x/flavor.h"
#undef HW_X_FLAVOR
#define HW_X_FLAVOR HW_X11

#include "hw_x/features.h"
#include "hw_x/x11_data.h"
#include "hw_x/keyboard.h"
#include "hw_x/xchar16.h"
#include "hw_x/x11_setcolors.c"


/* this can stay static, X11_FlushHW() is not reentrant */
static hwcol _col;

# define myXDrawImageString XDrawImageString16

#define XDRAW(col, buf, buflen) do { \
    X11_SetColors(col); \
    myXDrawImageString(xdisplay, xwindow, xgc, xbegin, ybegin + xupfont, buf, buflen); \
} while(0)

#define XDRAW_ANY(buf, buflen, col, gfx) XDRAW(col, buf, buflen)


INLINE void X11_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV;
    hwcol col;
    udat buflen = 0;
    hwfont f;
    XChar16 buf[TW_SMALLBUFF];
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
	if (buflen && (col != _col || (ValidOldVideo && *V == *oV) || buflen == TW_SMALLBUFF)) {
	    XDRAW(_col, buf, buflen);
	    buflen = 0;
	}
	if (!ValidOldVideo || *V != *oV) {
	    if (!buflen) {
		xbegin = (x - xhw_startx) * (ldat)xwfont;
		_col = col;
	    }
	    f = xUTF_32_to_charset(HWFONT(*V));
            XChar16FromRaw(f, &buf[buflen++]);
	}
    }
    if (buflen) {
	XDRAW(_col, buf, buflen);
	buflen = 0;
    }
}

#include "hw_x/util.h"

#undef XDRAW_ANY
#undef XDRAW

INLINE ldat diff(ldat x, ldat y) {
    return x >= y ? x - y : y - x;
}

enum { MAX_FONT_SCORE = 100 };

static ldat calcFontScore(udat fontwidth, udat fontheight, ldat width, ldat height) {
    /*
     * TODO xft: since xft fonts are scalable, we could just consider the aspect ratio
     * compared to fontwidth X fontheight.
     */
    ldat prod1 = (ldat)height * fontwidth;
    ldat prod2 = (ldat)width * fontheight;
    ldat score = MAX_FONT_SCORE - diff(height, fontheight) - 2 * diff(width, fontwidth)
        /* compare height/width ratio with requested one */
        - diff(prod1, prod2) / Max3(1, fontwidth, width);
    return score;
}

/*
 * return ttrue if each font glyph is either 'narrow' (latin, etc.) or 'wide' (CJK...)
 * with 'wide' characters exactly twice as wide as 'narrow' ones
 */
static tbool X11_FontIsDualWidth(CONST XFontStruct *info) {
    XCharStruct * p = info->per_char;
    ldat  wide = info->max_bounds.width,
	narrow = info->min_bounds.width,
	i, n_chars, w;
    if (wide != narrow * 2)
	return tfalse;
    if (p == NULL)
	/*
	 * no way to check per-character bounding box. hope for the best... 
	 * at least GNU unifont falls in this case.
	 */
	return ttrue;

    n_chars = (ldat)(info->max_byte1 - info->min_byte1 + 1) * (info->max_char_or_byte2 - info->min_char_or_byte2 + 1);
    for (i = 0; i < n_chars; i++) {
	w = p[i].width;
	if (w != 0 && w != narrow && w != wide)
	    return tfalse;
    }
    return ttrue;
}

/* if font is monospaced, return its score. otherwise return MINLDAT */
static ldat X11_MonospaceFontScore(CONST XFontStruct *info, udat fontwidth, udat fontheight, ldat best_score) {
    ldat score = TW_MINLDAT,
        width = info->min_bounds.width,
        height = (ldat)info->ascent + info->descent,
        max_width = info->max_bounds.width;
    
    if (width == max_width || X11_FontIsDualWidth(info)) {
        score = calcFontScore(fontwidth, fontheight, width, height);
        if (score > best_score)
            printk("      candidate font %ux%u score %ld\n", (unsigned)width, (unsigned)height, (long)score);
    }
    return score;
}

/* return name of selected font in allocated (char *) */
static char * X11_AutodetectFont(udat fontwidth, udat fontheight) {
    struct {
        CONST char * wildcard;
        ldat score_adj;
    } patterns[] = {
        /* { "-gnu-unifont-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1",  0 }, double-width chars not supported yet */
        { "-misc-console-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1", 0 },
        { "-misc-fixed-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1", 0 },
        { "-*-*-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1",        0 },
        { "-*-*-medium-r-normal-*-%s?-*-*-*-*-*-*-cp437",          -5 },
        { "-*-*-medium-r-normal-*-%s?-*-*-*-*-*-*-cp850",          -5 },
        { "-*-*-medium-r-normal-*-%s?-*-*-*-*-*-ibm-850",          -5 },
        { "-*-*-medium-r-normal-*-%s?-*-*-*-*-*-iso8859-*",       -10 },
    };
    enum { max_fonts = 1000, n_patterns = sizeof(patterns)/sizeof(patterns[0]) };
    
    XFontStruct *info;
    int i, j, k, n_fonts;
    
    char * pattern = AllocMem(LenStr(patterns[0].wildcard) + 1 + 3 * sizeof(unsigned));
    char digits[1 + 3 * sizeof(unsigned)];
    char ** names = NULL;
    char * best = NULL;
    ldat score, best_score = TW_MINLDAT;
    byte beatable_score = ttrue, look_up = fontheight >= 10 && (fontheight % 10) >= 5;
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

            for (k = 0; k < n_fonts && beatable_score; k++)
            {
		/*
		printk("     font        '%s'\t direction = %u, min_byte1 = %u, max_byte1 = %u, min_char_or_byte2 = %d, max_char_or_byte2 = %d\n",
		       names[k], info[k].direction, info[k].min_byte1, info[k].max_byte1, info[k].min_char_or_byte2, info[k].max_char_or_byte2);
		 */

                if (info[k].direction == FontLeftToRight
                    && info[k].min_byte1 == 0
                    && info[k].min_char_or_byte2 <= 32)
                {
                    score = X11_MonospaceFontScore(&info[k], fontwidth, fontheight, best_score - score_adj) + score_adj;
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

static byte X11_LoadFont(CONST char * fontname, udat fontwidth, udat fontheight) {
    char * alloc_fontname = 0;
    byte loaded = tfalse;

    if (!fontname)
        fontname = alloc_fontname = X11_AutodetectFont(fontwidth, fontheight);
    
#if HW_X_FLAVOR == HW_XFT
    if (fontname && (xsfont = XftFontOpenName(xdisplay, DefaultScreen(xdisplay), fontname)))
#else
    if ((fontname && (xsfont = XLoadQueryFont(xdisplay, fontname)))
        || (xsfont = XLoadQueryFont(xdisplay, fontname = "fixed")))
#endif
    {
        loaded = ttrue;

#if HW_X_FLAVOR == HW_XFT
        xwfont = xsfont->max_advance_width;
#else
        xwfont = xsfont->min_bounds.width;
#endif
        xwidth = xwfont * (unsigned)(HW->X = GetDisplayWidth());
        xhfont = (xupfont = xsfont->ascent) + xsfont->descent;
        xheight = xhfont * (unsigned)(HW->Y = GetDisplayHeight());
        
        printk("      selected %ux%u font `%."STR(TW_SMALLBUFF)"s'\n", (unsigned)xwfont, (unsigned)xhfont, fontname);
    }
    if (alloc_fontname)
        FreeMem(alloc_fontname);

    return loaded;
}


static void X11_QuitHW(void) {
#if HW_X_FLAVOR == HW_XFT
    int xscreen;
    Colormap colormap;
    Visual *xvisual;
    if (xdisplay) {
        xscreen = DefaultScreen(xdisplay);
        colormap = DefaultColormap(xdisplay, xscreen);
        xvisual = DefaultVisual(xdisplay, xscreen);
    }
#endif

#ifdef TW_FEATURE_X11_XIM_XIC
    if (xic)    XDestroyIC(xic);
    if (xim)    XCloseIM(xim);
#endif
#if HW_X_FLAVOR == HW_XFT
    if (xsfont) XftFontClose(xdisplay, xsfont);
    if (xftdraw) XftDrawDestroy(xftdraw);
    for (int i = 0; i < MAXCOL; i++) {
        if (xftcolors[i] == NULL) {
            break;
        }
        XftColorFree (xdisplay, xvisual, colormap, xftcolors[i]);
        FreeMem(xftcolors[i]);
        xftcolors[i] = NULL;
    }
#else
    if (xsfont) XFreeFont(xdisplay, xsfont);
#endif
    if (xgc != None) XFreeGC(xdisplay, xgc);
    if (xwindow != None) {
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

static byte X11_InitHW(void) {
    char *arg = HW->Name;
    int xscreen;
    unsigned int xdepth;
    XSetWindowAttributes xattr;
#if HW_X_FLAVOR == HW_XFT
    XRenderColor xcolor;
    XftColor *cur_xft_color;
#else
    XColor xcolor;
#endif
    XSizeHints *xhints;
    XEvent event;
    Visual *xvisual;
    Colormap colormap;
    byte *s, *xdisplay_ = NULL, *xdisplay0 = NULL,
        *fontname = NULL, *fontname0 = NULL,
        *charset = NULL, *charset0 = NULL,
        title[X11_TITLE_MAXLEN];
    int i;
    udat fontwidth = 8, fontheight = 16;
    byte drag = tfalse, noinput = tfalse;
    unsigned long xcreategc_mask = GCForeground|GCBackground|GCGraphicsExposures;
    
    if (!(HW->Private = (struct x11_data *)AllocMem(sizeof(struct x11_data)))) {
	printk("      X11_InitHW(): Out of memory!\n");
	return tfalse;
    }
    WriteMem(HW->Private, 0, sizeof(struct x11_data));

    /* default: show the whole screen */
    xhw_view = xhw_startx = xhw_starty = xhw_endx = xhw_endy = 0;

    /* not yet opened */
    xdisplay = NULL;
    
#if HW_X_FLAVOR == HW_XFT
    if (arg && (strncmp(arg, "-hw=xft", 7) == 0)) {
        arg += 7; /* skip "-hw=xft" */
#else
    if (arg && HW->NameLen > 4) {
	arg += 4; /* skip "-hw=" */
	
	if (*arg++ != 'X')
	    goto fail; /* user said "use <arg> as display, not X11" */
	
	if (*arg == '1' && arg[1] == '1')
	    arg += 2; /* `X11' is same as `X' */
#endif

	if (*arg == '@') {
	    if ((s = strchr(xdisplay_ = ++arg, ','))) {
		*(xdisplay0 = s) = '\0';
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
                    fontwidth  = Min2(TW_MAXUDAT, n2 > 0 ? n1 : n1 / 2);
                    fontheight = Min2(TW_MAXUDAT, n2 > 0 ? n2 : n1);
                }
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
		drag = ttrue;
	    } else if (!strncmp(arg, "noinput", 7)) {
		arg += 7;
		noinput = ttrue;
	    } else
		arg = strchr(arg, ',');
	}
    }

    xsfont = NULL; xhints = NULL;
    xwindow = None; xgc = None;
    xReqCount = XReqCount = 0;
    HW->keyboard_slot = NOSLOT;
    
    if ((xdisplay = XOpenDisplay(xdisplay_))) do {
	
	(void)XSetIOErrorHandler(X11_Die);

	if (!X11_CheckRemapKeys())
	    break;

	xscreen = DefaultScreen(xdisplay);
	xdepth  = DefaultDepth(xdisplay, xscreen);
        xvisual = DefaultVisual(xdisplay, xscreen);
        colormap = DefaultColormap(xdisplay, xscreen);
	
	for (i = 0; i <= MAXCOL; i++) {
	    xcolor.red   = 257 * (udat)Palette[i].Red;
	    xcolor.green = 257 * (udat)Palette[i].Green;
	    xcolor.blue  = 257 * (udat)Palette[i].Blue;
#if HW_X_FLAVOR == HW_XFT
            xcolor.alpha = 65535;
            if (!(cur_xft_color = (XftColor *)AllocMem(sizeof(XftColor)))) {
                printk("      X11_InitHW(): Out of memory!\n");
                break;
            }
            WriteMem(cur_xft_color, 0, sizeof(XftColor));
            if (!XftColorAllocValue(xdisplay,xvisual,colormap,&xcolor,cur_xft_color)) {
                printk("      X11_InitHW() failed to allocate colors\n");
                break;
            }
            xcol[i] = cur_xft_color->pixel;
            xftcolors[i] = cur_xft_color;
#else
	    if (!XAllocColor(xdisplay, colormap, &xcolor)) {
		printk("      X11_InitHW() failed to allocate colors\n");
		break;
	    }
	    xcol[i] = xcolor.pixel;
#endif
	}
	if (i <= MAXCOL)
	    break;
	
	xattr.background_pixel = xcol[0];
	xattr.event_mask = ExposureMask | VisibilityChangeMask |
	    StructureNotifyMask | SubstructureNotifyMask |
	    KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

	if (!X11_LoadFont(fontname, fontwidth, fontheight))
	    break;
	
	if (xhw_view && xhw_startx >= 0 && xhw_starty >= 0 && xhw_endx > xhw_startx && xhw_endy > xhw_starty) {
	    /* a valid view was specified */
	    
	    xwidth  = xwfont * (ldat)(xhw_endx - xhw_startx);
	    xheight = xhfont * (ldat)(xhw_endy - xhw_starty);
	} else {
	    xhw_view = xhw_startx = xhw_starty = 0;
	    xhw_endx = HW->X;
	    xhw_endy = HW->Y;
	}
	
	if ((xwindow = XCreateWindow(xdisplay, DefaultRootWindow(xdisplay), 0, 0,
				     xwidth, xheight, 0, xdepth, InputOutput,
				     xvisual, CWBackPixel | CWEventMask, &xattr)) &&

	    (xsgc.foreground = xsgc.background = xcol[0],
	     xsgc.graphics_exposures = False,
#if HW_X_FLAVOR == HW_XFT
             xforeground = xbackground = xftcolors[0],
#else
	     xsgc.font = xsfont->fid,
             xcreategc_mask = xcreategc_mask|GCFont,
#endif
	     xgc = XCreateGC(xdisplay, xwindow, xcreategc_mask, &xsgc)) &&

	    (xhints = XAllocSizeHints()))
        {
	    
            static XComposeStatus static_xcompose;
            xcompose = static_xcompose;

#if HW_X_FLAVOR == HW_XFT
            xftdraw = XftDrawCreate(xdisplay,xwindow,xvisual,colormap);
#endif

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


	    if (!(xUTF_32_to_charset = X11_UTF_32_to_charset_function(charset)))
		xUTF_32_to_charset = X11_UTF_32_to_UCS_2;
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
	    
	    HW->DisplayIsCTTY = tfalse;
	    HW->FlagsHW &= ~FlHWSoftMouse; /* mouse pointer handled by X11 server */
	    
	    HW->FlagsHW |= FlHWNeedOldVideo;
	    HW->FlagsHW |= FlHWExpensiveFlushVideo;
	    if (noinput)
		HW->FlagsHW |= FlHWNoInput;
	    
	    HW->NeedHW = 0;
	    HW->CanResize = ttrue;
	    HW->merge_Threshold = 0;
	    
	    /*
	     * we must draw everything on our new shiny window
	     * without forcing all other displays
	     * to redraw everything too.
	     */
	    HW->RedrawVideo = tfalse;
	    NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);
	    
	    if (xdisplay0) *xdisplay0 = ',';
	    if (fontname0) *fontname0 = ',';
	    if (charset0) *charset0 = ',';
	    
	    return ttrue;
	}
    } while (0); else {
	if (xdisplay_ || (xdisplay_ = getenv("DISPLAY")))
	    printk("      X11_InitHW() failed to open display %."STR(TW_SMALLBUFF)"s\n", HW->Name);
	else
	    printk("      X11_InitHW() failed: DISPLAY is not set\n");
    }

fail:
    if (xdisplay0) *xdisplay0 = ',';
    if (fontname0) *fontname0 = ',';
    if (charset0) *charset0 = ',';
	
    if (xdisplay)
	X11_QuitHW();

    FreeMem(HW->Private);
    HW->Private = NULL;
    
    return tfalse;
}

byte InitModule(module Module) {
    Module->Private = X11_InitHW;
    return ttrue;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module Module) {
}
