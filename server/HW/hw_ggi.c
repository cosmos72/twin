/*
 *  hw_ggi.h  --  functions to let twin display on libggi
 *
 *  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *
 *  this code is derived from:
 *
 *  Nixterm  -  basic terminal for LibGGI.
 *  Copyright (c) 1998-1999  Andrew Apted  <ajapted@netspace.net.au>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"

#include <ggi/ggi.h>

/*
 * TODO
 *	-  speed up text using ggiPuts() for multible chars.
 *	-  use ggiFlushRegion() for better X performance.
 */


/* Global Vars */

static int (*gOrigSelect)(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

static display_hw GGI;

static ggi_visual_t gvis;
static ggi_mode  gmode;
static ggi_coord gscreen;	/* window size */
static ggi_coord gfont;		/* character size */

#define L 0x5555
#define M 0xAAAA
#define H 0xFFFF

static ggi_pixel gforeground, gbackground, colpixel[MAXCOL+1];

static ggi_color gpalette[MAXCOL+1] = {
    /* the default colour table, for VGA+ colour systems */
    {0,0,0}, {0,0,M}, {0,M,0}, {0,M,M}, {M,0,0}, {M,0,M}, {M,M,0}, {M,M,M},
    {L,L,L}, {L,L,H}, {L,H,L}, {L,H,H}, {H,L,L}, {H,L,H}, {H,H,L}, {H,H,H}
};

#undef H
#undef M
#undef L

#define XY ((udat  *)All->gotoxybuf)
#define TT ((uldat *)All->cursorbuf)

#define WANTED_EVENTS  (emKey | emPointer | emExpose)


static void GGI_MoveToXY(udat x, udat y) {
    XY[2] = x;
    XY[3] = y;
}

static void GGI_SetCursorType(uldat CursorType) {
    if ((CursorType & 0xF) == 0)
	CursorType |= LINECURSOR;
    else if ((CursorType & 0xF) > SOLIDCURSOR)
	CursorType = (CursorType & ~(uldat)0xF) | SOLIDCURSOR;
    TT[1] = CursorType;
}

static void GGI_Beep(void) {
    /* not exactly clean... */
    putchar('\7');
    All->NeedHW |= NEEDFlushStdout;
}

static void GGI_InitPalette(void) {
    int i;
    for (i=0; i <= MAXCOL; i++)
	colpixel[i] = ggiMapColor(gvis, gpalette + i);
}

static hwcol _col;

#define GFG(col) \
    if (gforeground != colpixel[COLFG(col)]) \
	ggiSetGCForeground(gvis, gforeground = colpixel[COLFG(col)])

#define GBG(col) \
    if (gbackground != colpixel[COLBG(col)]) \
	ggiSetGCBackground(gvis, gbackground = colpixel[COLBG(col)])

#define GDRAW(col, buf, buflen) \
    GFG(col); \
    GBG(col); \
    ggiPuts(gvis, xbegin, ybegin, buf)


INLINE void GGI_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV;
    hwcol col;
    udat buflen = 0;
    byte buf[SMALLBUFF];
    int xbegin = x * gfont.x, ybegin = y * gfont.y;
    
    V = Video + x + y * ScreenWidth;
    oV = OldVideo + x + y * ScreenWidth;
    
    for (_col = ~HWCOL(*V); len; x++, V++, oV++, len--) {
	col = HWCOL(*V);
	if (buflen && (col != _col || (ValidOldVideo && *V == *oV) || buflen == SMALLBUFF-1)) {
	    buf[buflen] = '\0';
	    GDRAW(_col, buf, buflen);
	    buflen = 0;
	}
	if (!ValidOldVideo || *V != *oV) {
	    if (!buflen) {
		xbegin = x * gfont.x;
		_col = col;
	    }
	    *oV = *V;
	    buf[buflen++] = HWFONT(*V) ? HWFONT(*V) : ' ';
	    /* ggiPuts cannot handle '\0' */
	}
    }
    if (buflen) {
	buf[buflen] = '\0';
	GDRAW(_col, buf, buflen);
	buflen = 0;
    }
}

#undef GDRAW

static void GGI_HideCursor(dat x, dat y) {
    hwattr V = Video[x + y * ScreenWidth];
    hwcol col = HWCOL(V);
    
    GFG(col);
    GBG(col);
    ggiPutc(gvis, x * gfont.x, y * gfont.y, HWFONT(V));
}

static void GGI_ShowCursor(dat x, dat y, uldat type) {
    hwattr V = Video[x + y * ScreenWidth];
    hwcol v;
    udat i;
    
    if (type & 0x10) {
	/* soft cursor */
	v = (HWCOL(V) | ((type >> 16) & 0xff)) ^ ((type >> 8) & 0xff);
	if ((type & 0x20) && (HWCOL(V) & COL(0,WHITE)) == (v & COL(0,WHITE)))
	    v ^= COL(0,WHITE);
	if ((type & 0x40) && ((COLFG(v) & WHITE) == (COLBG(v) & WHITE)))
	    v ^= COL(WHITE,0);
	
	GFG(COLFG(v));
	GBG(COLBG(v));
	ggiPutc(gvis, x * gfont.x, y * gfont.y, HWFONT(V));
    }
    if (type & 0xF) {
	/* VGA hw-like cursor */
	i = gfont.x * ((type & 0xF)-NOCURSOR) / (SOLIDCURSOR-NOCURSOR);
	v = HWCOL(V);
	
	GFG(v);
	ggiDrawBox(gvis, x * gfont.x, y * gfont.y + gfont.y - i, gfont.x, i);
    }
}

static void GGI_FlushVideo(void) {
    dat start, end;
    udat i;
    byte c = ChangedVideoFlag &&
	(ValidOldVideo
	 ? Video[XY[0] + XY[1] * ScreenWidth] != OldVideo[XY[0] + XY[1] * ScreenWidth] 
	 : Plain_isDirtyVideo(XY[0], XY[1]));
    /* TRUE iff the cursor will be erased by burst */
    
    
    /* first burst all changes */
    if (ChangedVideoFlag) {
	for (i=0; i<ScreenHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    end   = ChangedVideo[i>>1][i&1][1];
	    ChangedVideo[i>>1][i&1][0] = -1;
	    
	    if (start != -1)
		GGI_Mogrify(start, i>>1, end-start+1);
	}
	All->NeedHW |= NEEDFlushHW;
    }
    /* then, we may have to erase the old cursor */
    if (!c && TT[0] != NOCURSOR && (TT[0] != TT[1] || XY[0] != XY[2] || XY[1] != XY[3])) {
	GGI_HideCursor(XY[0], XY[1]);
	All->NeedHW |= NEEDFlushHW;
    }
    /* finally, redraw the cursor if */
    /* (we want a cursor and (the burst erased the cursor or the cursor changed)) */
    if (TT[1] != NOCURSOR && (c || TT[0] != TT[1] || XY[0] != XY[2] || XY[1] != XY[3])) {
	GGI_ShowCursor(XY[2], XY[3], TT[1]);
	All->NeedHW |= NEEDFlushHW;
    }
    
    ChangedVideoFlag = ChangedMouseFlag = FALSE;
    ValidOldVideo = TRUE;
    
    TT[0] = TT[1];
    XY[0] = XY[2];
    XY[1] = XY[3];
}

static void GGI_FlushHW(void) {
    ggiFlush(gvis);
    All->NeedHW &= ~NEEDFlushHW;
}

static void GGI_DetectSize(udat *x, udat *y) {
    *x = gscreen.x;
    *y = gscreen.y;
}


static void GGI_KeyboardEvent(int fd, uldat slot) {
    ggi_event ev;
    struct timeval tv;
    dat x = All->MouseState->x, y = All->MouseState->y, dx = 0, dy = 0;
    byte keys = All->MouseState->keys;
    int ret;
    
    for (;;) {
	
	tv.tv_sec = tv.tv_usec = 0;
	
	if (ggiEventPoll(gvis, emAll, &tv) <= 0 || ggiEventRead(gvis, &ev, emAll) <= 0)
	    break;
	
	switch (ev.any.type) {

	  case evKeyPress:
	  case evKeyRepeat:
	    /*handle_keyboard(ev.key.sym, ev.key.modifiers, master_fd);*/
	    break;
	    
	  case evExpose:
	    /*
	     min_x = MIN(min_x, ev.expose.x / font.x);
	     min_y = MIN(min_y, ev.expose.y / font.y);
	     * 
	     max_x = MAX(max_x, (ev.expose.x+ev.expose.w +
				 font.x - 1) / font.x);
	     max_y = MAX(max_y, (ev.expose.y+ev.expose.h +
				 font.y - 1) / font.y);
	     */
	    break;
	    
	  case evPtrButtonPress:
	    switch (ev.pbutton.button) {
	      case GII_PBUTTON_FIRST:
		keys |= HOLD_LEFT;
		break;
	      case GII_PBUTTON_SECOND:
		keys |= HOLD_RIGHT;
		break;
	      case GII_PBUTTON_THIRD:
		keys |= HOLD_MIDDLE;
		break;
	      default:
		break;
	    }
	    break;
	    
	  case evPtrButtonRelease:
	    switch (ev.pbutton.button) {
	      case GII_PBUTTON_FIRST:
		keys &= ~HOLD_LEFT;
		break;
	      case GII_PBUTTON_SECOND:
		keys &= ~HOLD_RIGHT;
		break;
	      case GII_PBUTTON_THIRD:
		keys &= ~HOLD_MIDDLE;
		break;
	      default:
		break;
	    }
	    break;
	    
	  case evPtrRelative:
	    /*
	     if (ev.any.type == evPtrRelative) {
		 x += All->MouseState->x;
		 y += All->MouseState->y;
	     }
	     */
	    break;
	    
	  case evPtrAbsolute:
	    
	    x = ev.pmove.x / gfont.x;
	    y = ev.pmove.y / gfont.y;
	    
	    x = Max2(x, 0); x = Min2(x, ScreenWidth - 1);
	    y = Max2(y, 0); y = Min2(y, ScreenHeight - 1);
	    
	    dx = ev.pmove.x < gfont.x/2 ? -1 : ScreenWidth *gfont.x - ev.pmove.x <= gfont.x/2 ? 1 : 0;
	    dy = ev.pmove.y < gfont.y/2 ? -1 : ScreenHeight*gfont.y - ev.pmove.y <= gfont.y/2 ? 1 : 0;
	    
	    break;
	    
	  default:
	    break;
	}
    }
    
    if (keys != All->MouseState->keys ||
	((keys || All->MouseState->keys) &&
	 (dx || dy || x != All->MouseState->x || y != All->MouseState->y)))
	 
	 MouseEventCommon(x, y, dx, dy, keys);
    else {
	All->MouseState->keys = keys;
	All->MouseState->x = x;
	All->MouseState->y = y;
    }
}

    
static int GGI_Select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
    int ret;
    ggi_event_mask gmask = WANTED_EVENTS;
    struct timeval mytm;
    if (!timeout) {
	mytm.tv_sec = 86400;
	mytm.tv_usec = 0;
	timeout = &mytm;
    }
    
    ret = ggiEventSelect(gvis, &gmask, n, readfds, writefds, exceptfds, timeout);
    GGI_KeyboardEvent(NOFD, NOSLOT);
    
    return ret;
}
	


static void GGI_QuitHW(void) {
    All->keyboard_slot = All->mouse_slot = NOSLOT;
    
    GGI.KeyboardEvent = (void *)NoOp;
    GGI.QuitHW = NoOp;

    OverrideSelect = gOrigSelect;

    ggiClose(gvis);
    ggiExit();
}

display_hw *GGI_InitHW(byte *arg) {
    int i, j;
    
    if (arg) {
	if (strncmp("ggi", arg, 3))
	    return NULL; /* user said "use <arg> as display, not ggi" */
	    
	arg = NULL;
    }

    if (ggiInit() < 0) 
	return NULL;

    if ((gvis = ggiOpen(/*target,*/ NULL))) do {

	ggiParseMode("", &gmode);
	ggiCheckMode(gvis, &gmode);

	if (ggiSetMode(gvis, &gmode) != 0)
	    break;

	ggiSetFlags(gvis, GGIFLAG_ASYNC);

	if (GT_SCHEME(gmode.graphtype) == GT_PALETTE)
	    ggiSetColorfulPalette(gvis);
	GGI_InitPalette();

	ggiGetCharSize(gvis, &i, &j);
	gfont.x = i;
	gfont.y = j;
	gscreen.x = gmode.virt.x / gfont.x;
	gscreen.y = gmode.virt.y / gfont.y;

#if 0
	if (inputs)
		ggiJoinInputs(gvis, giiOpen(inputs, NULL));
#endif
	
	ggiSetGCForeground(gvis, gforeground = colpixel[0]);
	ggiSetGCBackground(gvis, gbackground = colpixel[0]);
	ggiFillscreen(gvis);

	
	All->keyboard_slot = All->mouse_slot = NOSLOT;
	
	gOrigSelect = OverrideSelect;
	OverrideSelect = GGI_Select;
	
	GGI.KeyboardEvent = GGI_KeyboardEvent;
	GGI.QuitKeyboard  = NoOp;
	    

	GGI.SoftMouse = FALSE; /* mouse pointer handled by X11 server */
	GGI.MouseEvent = (void *)NoOp; /* mouse events handled by GGI_KeyboardEvent */
	GGI.ShowMouse = NoOp;
	GGI.HideMouse = NoOp;
	GGI.QuitMouse = NoOp;

#if 1
	GGI.canDragArea = NULL;
#else
	GGI.canDragArea = GGI_canDragArea;
	GGI.DragArea    = GGI_DragArea;
#endif
	
	GGI.DetectSize  = GGI_DetectSize;
	
	GGI.ExportClipBoard = NULL;
	GGI.ImportClipBoard = NULL;
	GGI.PrivateClipBoard = NULL;

	GGI.FlushVideo = GGI_FlushVideo;
	GGI.QuitVideo = NoOp;
	GGI.FlushHW = GGI_FlushHW;

	GGI.NeedOldVideo = TRUE;
	GGI.merge_Threshold = 0;
	GGI.ExpensiveFlushVideo = FALSE;
	
	GGI.MoveToXY = GGI_MoveToXY;	       XY[0] = XY[1] = XY[2] = XY[3] = 0;
	GGI.SetCursorType = GGI_SetCursorType; TT[0] = TT[1] = NOCURSOR;
	GGI.Beep = GGI_Beep;
	    
	GGI.QuitHW = GGI_QuitHW;
	
	InitTtysave();
	
	return &GGI;
	
    } while (0); else {
	if (arg || (arg = getenv("GGI_DISPLAY")))
	    fprintf(errFILE, "      GGI_InitHW() failed to open display %s\n", arg);
	else
	    fprintf(errFILE, "      GGI_InitHW() failed: GGI_DISPLAY is not set\n");
	
    }
    if (gvis)
	ggiClose(gvis);
    ggiExit();
    
    return NULL;
}



#ifdef MODULE

byte InitModule(module *Module) {
    Module->Private = GGI_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module *Module) {
}

#endif /* MODULE */


#if 0
static void do_move(int x, int y, int nx, int ny, int w, int h)
{
	if (tfr_h > 0) {
		ggiCopyBox(gvis, x*font.x,  sy*font.y, 
				w*font.x,  tfr_h*font.y,
				nx*font.x, dy*font.y);
	}

}

static void handle_keyboard(int sym, int modifiers, int out_fd) {
	int shift = modifiers & (1 << GII_KM_SHIFT);
	
	if (GII_KTYP(sym) == GII_KT_LATIN1) {
		send_char(out_fd, GII_KVAL(sym)); 
		return;
	}

	if (shift) switch (sym) {

		case GIIK_PageUp:   sym = GIIK_ScrollBack; break;
		case GIIK_PageDown: sym = GIIK_ScrollForw; break;

		case GIIK_F1: sym = GIIK_F11; break;
		case GIIK_F2: sym = GIIK_F12; break;
	}

	switch (sym) {

		/* keys we pass on */

		case GIIK_Enter: send_char(out_fd, '\n'); return;
		case GIIK_Up:    send_str(out_fd, "\033[A"); return;
		case GIIK_Down:  send_str(out_fd, "\033[B"); return;
		case GIIK_Right: send_str(out_fd, "\033[C"); return;
		case GIIK_Left:  send_str(out_fd, "\033[D"); return;

		case GIIK_Home:     send_str(out_fd, "\033[1~"); return;
		case GIIK_End:      send_str(out_fd, "\033[4~"); return;
		case GIIK_Insert:   send_str(out_fd, "\033[2~"); return;
/* ???		case GIIK_Remove:   send_str(out_fd, "\033[3~"); return; */
		case GIIK_PageUp:   send_str(out_fd, "\033[5~"); return;
		case GIIK_PageDown: send_str(out_fd, "\033[6~"); return;

		case GIIK_F1:  send_str(out_fd, "\033[[A"); return;
		case GIIK_F2:  send_str(out_fd, "\033[[B"); return;
		case GIIK_F3:  send_str(out_fd, "\033[[C"); return;
		case GIIK_F4:  send_str(out_fd, "\033[[D"); return;
		case GIIK_F5:  send_str(out_fd, "\033[[E"); return;
		case GIIK_F6:  send_str(out_fd, "\033[17~"); return;
		case GIIK_F7:  send_str(out_fd, "\033[18~"); return;
		case GIIK_F8:  send_str(out_fd, "\033[19~"); return;
		case GIIK_F9:  send_str(out_fd, "\033[20~"); return;
		case GIIK_F10: send_str(out_fd, "\033[21~"); return;

		/* keys we greedily keep to ourselves */

		case GIIK_F11: quit=1; return;

		case GIIK_ScrollForw: move_window( sb_step); return;
		case GIIK_ScrollBack: move_window(-sb_step); return;
	}
}


/* ---------------------------------------------------------------------- */


static void usage(void)
{
	fprintf(stderr,
		"\n"
		"USAGE: nixterm [OPTIONS...]\n"
		"\n"
		"where option is one of:\n"
		"\n"
		"      -h  --help\n"
		"      -V  --version\n"
		"      -m  --mode        <mode-string>\n"
		"      -t  --target      <target-string>\n"
		"      -i  --input       <input-string>\n"
		"      -f  --font        <width>x<height>\n"
		"      -s  --scrollback  <num-lines>\n"
		"      -b  --blank       <minutes>\n"
		"      -c  --color\n"
		"      -q  --quiet\n"
		"      -r  --reverse\n"
		"\n");
}

static int parse_font(char *s, ggi_coord *f)
{
	int w, h;

	if ((sscanf(s, " %d x %d", &w, &h) != 2) ||
	    (w <= 0) || (h <= 0)) {
		
		fprintf(stderr, "nixterm: Bad font size '%s'.\n", s);
		return -1;
	}

	f->x = w; 
	f->y = h;

	return 0;
}


/* ---------------------------------------------------------------------- */


#define CMP_OPT(short,long,extra)  \
    ((argc > extra) && \
     ((strcmp(*argv, short) == 0) || (strcmp(*argv, long) == 0)))

static int woulbe_main(int argc, char **argv, char **envp)
{
	char *target = NULL;
	char *inputs = NULL;
	char *shell  = "/bin/sh";

	struct winsize window_size;

	int master_fd, slave_fd;
	int w, h;
	int err;


	/* initialize */

	ggiParseMode("", &gmode);
	
	if (getenv("SHELL") != NULL) {
		shell = getenv("SHELL");
	}


	/* handle arguments */

	argv++; argc--;

	while (argc > 0) {

		if (CMP_OPT("-h", "--help", 0)) {

			usage();
			return 0;
		}

		if (CMP_OPT("-V", "--version", 0)) {

			printf("Nixterm %s\n", VERSION_STR);
			return 0;
		}

		if (CMP_OPT("-m", "--mode", 1)) {
		    
			ggiParseMode(argv[1], &gmode);
			argv += 2; argc -= 2;
			continue;
		}

		if (CMP_OPT("-t", "--target", 1)) {

			target = argv[1];
			argv += 2; argc -= 2;
			continue;
		}

		if (CMP_OPT("-i", "--input", 1)) {

			inputs = argv[1];
			argv += 2; argc -= 2;
			continue;
		}

		if (CMP_OPT("-f", "--font", 0)) {

			if (parse_font(argv[1], &font) < 0) {
				return 1;
			}
			argv += 2; argc -= 2;
			continue;
		}

		if (CMP_OPT("-s", "--scrollback", 0)) {

			if ((sscanf(argv[1], "%d", &scrollback) != 1) ||
			    (scrollback < 0)) {

				fprintf(stderr, "nixterm: Bad scrollback.\n");
				return 1;
			}
			argv += 2; argc -= 2;
			continue;
		}

		if (CMP_OPT("-b", "--blank_time", 0)) {

			if ((sscanf(argv[1], "%d", &blank_time) != 1) ||
			    (blank_time < 0)) {

				fprintf(stderr, "nixterm: Bad blank_time.\n");
				return 1;
			}
			blank_time = blank_time * 60 * 1000;
			argv += 2; argc -= 2;
			continue;
		}

		if (CMP_OPT("-q", "--quiet", 0)) {

			options |= NIX_OPT_QUIET; 
			argv++; argc--;
			continue;
		}

		if (CMP_OPT("-r", "--reverse", 0)) {

			options |= NIX_OPT_REVERSE;
			argv++; argc--;
			continue;
		}

		if (CMP_OPT("-c", "--color", 0)) {

			options |= NIX_OPT_COLOR; 
			argv++; argc--;
			continue;
		}

		fprintf(stderr, "nixterm: Unknown option '%s'\n", *argv);
		return 1;
	}


	/* now setup screen */
}

#endif
