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

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"

#include <ggi/ggi.h>

/*
 * I should flame to death libggi people, as
 * their idea of having to use ggiEventSelect() instead of plain select()
 * doesn't allow for multiple simultaneous libggi displays!
 */

/*
 * TODO
 *	-  use ggiFlushRegion() for better X performance.
 */

static int (*gOrigSelect)(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
static display_hw GGI_HW;

struct GGI_data {
    ggi_visual_t gvis;
    ggi_mode  gmode;
    ggi_coord gfont;	/* character size */

    ggi_pixel gforeground, gbackground, gcol[MAXCOL+1];
};

#define GGIdata ((struct GGI_data *)HW->Private)
#define gvis	(GGIdata->gvis)
#define gmode	(GGIdata->gmode)
#define gfont	(GGIdata->gfont)
#define gforeground	(GGIdata->gforeground)
#define gbackground	(GGIdata->gbackground)
#define gcol	(GGIdata->gcol)

#define WANTED_EVENTS  (emKey | emPointer | emExpose)


static void GGI_Beep(void) {
    /* not exactly clean... */
    putchar('\7');
    NeedHW |= NEEDFlushStdout;
}

static void GGI_KeyboardEvent(int fd, display_hw hw) {
    ggi_event ev;
    struct timeval tv;
    dat x, y, dx = 0, dy = 0;
    byte keys;

    SaveHW;
    SetHW(hw);
    
    x = HW->MouseState.x;
    y = HW->MouseState.y;
    keys = HW->MouseState.keys;
    
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
		 x += HW->MouseState.x;
		 y += HW->MouseState.y;
	     }
	     */
	    break;
	    
	  case evPtrAbsolute:
	    
	    x = ev.pmove.x / gfont.x;
	    y = ev.pmove.y / gfont.y;
	    
	    x = Max2(x, 0); x = Min2(x, DisplayWidth - 1);
	    y = Max2(y, 0); y = Min2(y, DisplayHeight - 1);
	    
	    dx = ev.pmove.x < gfont.x/2 ? -1 : DisplayWidth *gfont.x - ev.pmove.x <= gfont.x/2 ? 1 : 0;
	    dy = ev.pmove.y < gfont.y/2 ? -1 : DisplayHeight*gfont.y - ev.pmove.y <= gfont.y/2 ? 1 : 0;
	    
	    break;
	    
	  default:
	    break;
	}
    }
    
    if (keys != HW->MouseState.keys || dx || dy || x != HW->MouseState.x || y != HW->MouseState.y) {
	MouseEventCommon(x, y, dx, dy, keys);
	HW->MouseState.keys = keys;
	HW->MouseState.x = x;
	HW->MouseState.y = y;
    }
    
    RestoreHW;
}

/* this can stay static */
static hwcol _col;

#define GFG(col) \
    if (gforeground != gcol[COLFG(col)]) \
	ggiSetGCForeground(gvis, gforeground = gcol[COLFG(col)])

#define GBG(col) \
    if (gbackground != gcol[COLBG(col)]) \
	ggiSetGCBackground(gvis, gbackground = gcol[COLBG(col)])

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
    
    V = Video + x + y * DisplayWidth;
    oV = OldVideo + x + y * DisplayWidth;
    
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
    hwattr V = Video[x + y * DisplayWidth];
    hwcol col = HWCOL(V);
    
    GFG(col);
    GBG(col);
    ggiPutc(gvis, x * gfont.x, y * gfont.y, HWFONT(V));
}

static void GGI_ShowCursor(uldat type, dat x, dat y) {
    hwattr V = Video[x + y * DisplayWidth];
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
    byte iff;
    
    if (ValidOldVideo)
	iff = ChangedVideoFlag &&
	Video[HW->XY[0] + HW->XY[1] * DisplayWidth]
	!= OldVideo[HW->XY[0] + HW->XY[1] * DisplayWidth];
    /* TRUE if and only if the cursor will be erased by burst */
    
    
    /* first burst all changes */
    if (ChangedVideoFlag) {
	for (i=0; i<DisplayHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    end   = ChangedVideo[i>>1][i&1][1];
	    
	    if (start != -1)
		GGI_Mogrify(start, i>>1, end-start+1);
	}
	setFlush();
    }
    /* then, we may have to erase the old cursor */
    if (!ValidOldVideo ||
	(!iff && HW->TT != NOCURSOR &&
	 (CursorType != HW->TT || CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
	
	HW->TT = NOCURSOR;
	GGI_HideCursor(HW->XY[0], HW->XY[1]);
	setFlush();
    }
    /* finally, redraw the cursor if forced to redraw (!ValidOldVideo) or */
    /* (we want a cursor and (the burst erased the cursor or the cursor changed)) */
    if (!ValidOldVideo ||
	(CursorType != NOCURSOR &&
	 (iff || CursorType != HW->TT || CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
	
	GGI_ShowCursor(HW->TT = CursorType, HW->XY[0] = CursorX, HW->XY[1]= CursorY);
	setFlush();
    }
    
    HW->FlagsHW &= ~FlHWChangedMouseFlag;
}

static void GGI_FlushHW(void) {
    ggiFlush(gvis);
    clrFlush();
}

static void GGI_DetectSize(dat *x, dat *y) {
    *x = HW->X;
    *y = HW->Y;
}

static void GGI_CheckResize(dat *x, dat *y) {
    /* TODO */
    /* I am lazy, libggi has ggiCheckMode() for this. */
    *x = Min2(*x, HW->X);
    *y = Min2(*y, HW->Y);
}

static void GGI_Resize(dat x, dat y) {
    if (x < HW->usedX || y < HW->usedY) {
	/* clear screen so that extra area will be padded with blanks */
	ggiSetGCForeground(gvis, gforeground = gcol[0]);
	ggiFillscreen(gvis);
	ggiFlush(gvis);

	/* using ggiSetMode() would also probably avoid having to do this */
	NeedRedrawVideo(0, 0, x - 1, y - 1);
    }
    HW->usedX = x;
    HW->usedX = y;
}

#if 0
static byte GGI_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return (Rgt-Left+1) * (Dwn-Up+1) > 20;
}

static void GGI_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    ggiCopyBox(gvis, Left*gfont.x, Up*gfont.y,
	       (Rgt-Left+1)*gfont.x, (Dwn-Up+1)*gfont.y, DstLeft*gfont.x, DstUp*gfont.y);
    setFlush();
}
#endif


static int GGI_Select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
    int ret;
    ggi_event_mask gmask = WANTED_EVENTS;
    struct timeval mytm;
    SaveHW;
    SetHW(GGI_HW);
    
    if (!timeout) {
	mytm.tv_sec = 86400;
	mytm.tv_usec = 0;
	timeout = &mytm;
    }
    
    ret = ggiEventSelect(gvis, &gmask, n, readfds, writefds, exceptfds, timeout);
    GGI_KeyboardEvent(NOFD, HW);

    RestoreHW;
    return ret;
}
	
static void GGI_InitPalette(void) {
    int i;
    ggi_color pixel;
    for (i=0; i <= MAXCOL; i++) {
	pixel.r = 257 * (udat)Palette[i].Red;
	pixel.g = 257 * (udat)Palette[i].Green;
	pixel.b = 257 * (udat)Palette[i].Blue;
	pixel.a = 0;
	gcol[i] = ggiMapColor(gvis, &pixel);
    }
}

static void GGI_QuitHW(void) {
    HW->keyboard_slot = HW->mouse_slot = NOSLOT;
    
    HW->KeyboardEvent = (void *)NoOp;
    HW->QuitHW = NoOp;

    if (OverrideSelect == GGI_Select)
	OverrideSelect = gOrigSelect;

    ggiClose(gvis);
    ggiExit();
    
    FreeMem(HW->Private);
    
    if (GGI_HW == HW)
	GGI_HW = NULL;
}

#ifdef THIS_MODULE
static
#endif
byte GGI_InitHW(void) {
    byte *arg = HW->Name, *opt = NULL;
    uldat len = HW->NameLen;
    int i, j;
    
    if (GGI_HW) {
	printk("      GGI_InitHW() failed: libggi already in use.\n");
	return FALSE;
    }

    if (arg && len > 4) {

	arg += 4;
	if (strncmp(arg, "ggi", 3))
	    return FALSE; /* user said "use <arg> as display, not ggi" */
	arg += 3;
	
	if ((opt = strstr(arg, ",noinput"))) {
	    *opt = '\0';
	    HW->FlagsHW |= FlHWNoInput;
	}
	
	if (*arg == '@')
	    arg++;
	else
	    arg = NULL;
    } else
	arg = NULL;

    if (ggiInit() < 0) {
	if (opt) *opt = ',';
	return FALSE;
    }
    if (!(HW->Private = (struct GGI_data *)AllocMem(sizeof(struct GGI_data)))) {
	printk("      GGI_InitHW(): Out of memory!\n");
	return FALSE;
    }

    gvis = arg ? ggiOpen(arg, NULL) : ggiOpen(NULL);

    if (gvis) do {

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
	HW->X = gmode.virt.x / gfont.x;
	HW->Y = gmode.virt.y / gfont.y;
	HW->usedX = GetDisplayWidth();
	HW->usedY = GetDisplayHeight();

#if 0
	if (inputs)
		ggiJoinInputs(gvis, giiOpen(inputs, NULL));
#endif
	
	ggiSetGCForeground(gvis, gforeground = gcol[0]);
	ggiSetGCBackground(gvis, gbackground = gcol[0]);
	ggiFillscreen(gvis);
	

	HW->keyboard_slot = HW->mouse_slot = NOSLOT;
	    
	HW->FlushVideo = GGI_FlushVideo;
	HW->FlushHW = GGI_FlushHW;

	HW->KeyboardEvent = GGI_KeyboardEvent;
	HW->MouseEvent = (void *)NoOp; /* mouse events handled by GGI_KeyboardEvent */

	HW->XY[0] = HW->XY[1] = 0;
	HW->TT = (uldat)-1; /* force updating the cursor */
	
	HW->ShowMouse = NoOp;
	HW->HideMouse = NoOp;
	HW->UpdateMouseAndCursor = NoOp;
	HW->MouseState.x = HW->MouseState.y = HW->MouseState.keys = 0;
	
	HW->DetectSize  = GGI_DetectSize;
	HW->CheckResize = GGI_CheckResize;
	HW->Resize      = GGI_Resize;

	HW->HWSelectionImport  = AlwaysFalse;
	HW->HWSelectionExport  = NoOp;
	HW->HWSelectionRequest = (void *)NoOp;
	HW->HWSelectionNotify  = (void *)NoOp;
	HW->HWSelectionPrivate = NULL;

#if 1
	HW->CanDragArea = NULL;
#else
	HW->CanDragArea = GGI_CanDragArea;
	HW->DragArea    = GGI_DragArea;
#endif
	
	HW->Beep = GGI_Beep;
	HW->Configure = (void *)NoOp; /* TODO... */
	HW->SetPalette = (void *)NoOp;/* TODO... */
	HW->ResetPalette = NoOp;

	HW->QuitHW = GGI_QuitHW;
	HW->QuitKeyboard = NoOp;
	HW->QuitMouse = NoOp;
	HW->QuitVideo = NoOp;

	HW->DisplayIsCTTY = FALSE;
	HW->FlagsHW &= ~FlHWSoftMouse; /* mouse pointer handled by X11 server */

	HW->FlagsHW |= FlHWNeedOldVideo;
	HW->FlagsHW |= FlHWExpensiveFlushVideo;
	HW->NeedHW = 0;
	HW->CanResize = FALSE; /* TODO: a real GGI_Resize() */
	HW->merge_Threshold = 0;

	gOrigSelect = OverrideSelect;
	OverrideSelect = GGI_Select;

	GGI_HW = HW;
	
	/*
	 * we must draw everything on our new shiny window
	 * without forcing all other displays
	 * to redraw everything too.
	 */
	HW->RedrawVideo = FALSE;
	NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);

	if (opt) *opt = ',';
	return TRUE;
	
    } while (0); else {
	if (arg || (arg = getenv("GGI_DISPLAY")))
	    printk("      GGI_InitHW() failed to open display %."STR(SMALLBUFF)"s\n", arg);
	else
	    printk("      GGI_InitHW() failed: GGI_DISPLAY is not set\n");
	
    }
    if (gvis)
	ggiClose(gvis);
    ggiExit();
    
    if (opt) *opt = ',';
    return FALSE;
}



#ifdef THIS_MODULE

byte InitModule(module Module) {
    Module->Private = GGI_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module Module) {
}

#endif /* THIS_MODULE */


#if 0
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
#endif
