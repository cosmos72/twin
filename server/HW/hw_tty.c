/*
 *  hw_tty.c  --  functions to let twin display on linux console and inside a twin terminal
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"

#include "libTwkeys.h"

#ifdef CONF_HW_TTY_LINUX
#include <gpm.h>
#endif

#if !defined(CONF_HW_TTY_LINUX) && !defined(CONF_HW_TTY_TWTERM)
# warning trying to compile textmode terminals driver without support for any specific ttys.
# warning you will get only a 'generic tty' driver, without mouse support.
#endif


/*
 * libgpm is stacked, not multi-headed (so no multiplex too)
 */
static byte GPM_InUse;

/*
 * Linux:
 * 
 * plain Unix-style tty keyboard input;
 * 
 * mouse input uses libgpm to connect to `gpm' mouse daemon
 * and read mouse state, but draws mouse pointer manually;
 * (as fallback, xterm-style mouse input)
 * 
 * output through /dev/vcsaXX;
 * (as fallback, output through stdout)
 */



/* plain Unix-style tty keyboard input */

/* it can't update FullShiftFlags :( */

struct tty_data {
    int tty_fd, VcsaFd, tty_number;
    byte *tty_name, *tty_TERM;
    dat ttypar[2];
    FILE *stdOUT;
    uldat saveCursorType;
    udat saveX, saveY;
	
#ifdef CONF_HW_TTY_LINUX
    Gpm_Connect GPM_Conn;
    int GPM_fd;
    int GPM_keys;
#endif
};

#define ttydata		((struct tty_data *)HW->Private)
#define tty_fd		(ttydata->tty_fd)
#define VcsaFd		(ttydata->VcsaFd)
#define tty_number	(ttydata->tty_number)
#define tty_name	(ttydata->tty_name)
#define tty_TERM	(ttydata->tty_TERM)
#define ttypar		(ttydata->ttypar)
#define stdOUT		(ttydata->stdOUT)
#define saveCursorType	(ttydata->saveCursorType)
#define saveX		(ttydata->saveX)
#define saveY		(ttydata->saveY)
#define GPM_Conn	(ttydata->GPM_Conn)
#define GPM_fd		(ttydata->GPM_fd)
#define GPM_keys	(ttydata->GPM_keys)

static void stdin_QuitKeyboard(void);
static void stdin_KeyboardEvent(int fd, display_hw *hw);


/* return FALSE if failed */
static byte stdin_InitKeyboard(void) {
    struct termios ttyb;
    byte buf[16], *s = buf+3, c;
    int i;

    ttyb = ttysave;
    /* NL=='\n'==^J; CR=='\r'==^M */
    ttyb.c_iflag &= ~(IXON|IXOFF|IGNCR|INLCR|ICRNL);
    /*ttyb.c_oflag &= ~OPOST;*/
    ttyb.c_lflag &= ~(ECHO|ICANON);
    ttyb.c_cc[VTIME] = 0;
    ttyb.c_cc[VMIN] = 1;
    /* disable special handling of suspend key (^Z), quit key (^\), break key (^C) */
    ttyb.c_cc[VSUSP] = 0;
    ttyb.c_cc[VQUIT] = 0;
    ttyb.c_cc[VINTR] = 0;
    ioctl(tty_fd, TCSETS, &ttyb);

    write(tty_fd, "\033Z", 2); /* request ID */
    /* ensure we CAN read from the tty */
    do {
	i = read(tty_fd, buf, 15);
    } while (i < 0 && (errno == EAGAIN || errno == EINTR));
    if (i <= 0) {
	fputs("      stdin_InitKeyboard() failed: unable to read from the terminal!\n", stderr);
	ioctl(tty_fd, TCSETS, &ttysave);
	return FALSE;
    }
    buf[i] = '\0';
    ttypar[0] = ttypar[1] = i = 0;

    while (i<2 && (c=*s)) {
	if (c>='0' && c<='9') {
	    ttypar[i] *= 10; ttypar[i] += c-'0';
	} else if (*s==';')
	    i++;
	s++;
    }

    HW->keyboard_slot = RegisterRemote(tty_fd, HW, (void *)stdin_KeyboardEvent);
    if (HW->keyboard_slot == NOSLOT) {
	stdin_QuitKeyboard();
	return FALSE;
    }
    HW->KeyboardEvent = stdin_KeyboardEvent;
    HW->QuitKeyboard = stdin_QuitKeyboard;

    return TRUE;
}

static void stdin_QuitKeyboard(void) {
    ioctl(tty_fd, TCSETS, &ttysave);
    
    UnRegisterRemote(HW->keyboard_slot);
    HW->keyboard_slot = NOSLOT;
    
    HW->QuitKeyboard = NoOp;
}

static udat stdin_LookupKey(byte *slen, byte *s) {
    byte used = 0, len = *slen;
    
    if (*s == '\x1B') {
	
#define IS(sym, c) case c: *slen = ++used; return TW_##sym
	
	++used, --len; switch (*++s) {
	    
	  case '[':
	    if (++used, --len) switch (*++s) {
		/* ESC [ */
		IS(Up,    'A');
		IS(Down,  'B');
		IS(Right, 'C');
		IS(Left,  'D');
		IS(KP_5,  'G');  /* returned when NumLock is off */
		IS(Pause, 'P');
		
	      case '[':
		/* ESC [ [ */
		if (++used, --len) switch (*++s) {
		    IS(F1, 'A');
		    IS(F2, 'B');
		    IS(F3, 'C');
		    IS(F4, 'D');
		    IS(F5, 'E');
		}
		return TW_Null;
		
	      case '1':
		/* ESC [ 1 */
		if (++used, --len) {
		    ++s;
		    
		    if (len == 1 && *s == '~') {
			/* ESC [ 1 ~ */
			*slen = ++used;
			return TW_Home;
		    }
		    ++used, --len;
		    if (len && s[1] == '~')
			/* ESC [ 1 ? ~ */
			switch (*s) {
			    IS(F6, '7');
			    IS(F7, '8');
			    IS(F8, '9');
			}
		}
		return TW_Null;
		
	      case '2':
		/* ESC [ 2 */
		if (++used, --len) {
		    ++s;
		    
		    if (len == 1 && *s == '~') {
			/* ESC 2 ~ */
			*slen = ++used;
			return TW_Insert;
		    }
		    ++used, --len;
		    if (len && s[1] == '~')
			/* ESC [ 2 ? ~ */
			switch (*s) {
			    IS(F9,  '0');
			    IS(F10, '1');
			    IS(F11, '3');
			    IS(F12, '4');
			}
		}
		return TW_Null;
		
	      case '3':
		/* ESC [ 3 */
		if (++used, --len) switch (*++s) {
		    IS(Delete, '~');
		}
		return TW_Null;
		
	      case '4':
		/* ESC [ 4 */
		if (++used, --len) switch (*++s) {
		    IS(End, '~');
		}
		return TW_Null;
		
	      case '5':
		/* ESC [ 5 */
		if (++used, --len) switch (*++s) {
		    IS(Prior, '~');
		}
		return TW_Null;

	      case '6':
		/* ESC [ 6 */
		if (++used, --len) switch (*++s) {
		    IS(Next, '~');
		}
		return TW_Null;
		
	    }
	    return TW_Null;
	    
	  case 'O':
	    /* ESC O */
	    if (++used, --len) switch (*++s) {
		    
		IS(KP_Enter, 'M');
		IS(Num_Lock, 'P');
		IS(KP_Divide,'Q');
		IS(KP_Multiply,'R');
		IS(KP_Subtract,'S');
		
		IS(KP_Add, 'l');
		IS(KP_Separator, 'm');
		IS(KP_Delete, 'n');
		IS(KP_Equal, 'o');
		
		IS(KP_0, 'p');
		IS(KP_1, 'q');
		IS(KP_2, 'r');
		IS(KP_3, 's');
		IS(KP_4, 't');
		IS(KP_5, 'u');
		IS(KP_6, 'v');
		IS(KP_7, 'w');
		IS(KP_8, 'x');
		IS(KP_9, 'y');
	    }
	    return TW_Null;
	    
	  default:
	    return TW_Null;
	}
	    
#undef IS
	
    }
    return TW_Null;
}
	
static byte xterm_MouseData[10] = "\033[M#!!!!";

static void stdin_KeyboardEvent(int fd, display_hw *hw) {
    static void xterm_MouseEvent(int, display_hw *);
    static byte *match;
    byte got, chunk, buf[SMALLBUFF], *s;
    udat Code;
    SaveHW;
    
    SetHW(hw);
    
    got = read(fd, s=buf, SMALLBUFF-1);
    
    if (got == 0 || (got == (byte)-1 && errno != EINTR && errno != EAGAIN)) {
	/* BIG troubles */
	HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	return;
    }
    
    while (got > 0) {
#ifdef CONF_HW_TTY_TWTERM
	if (HW->MouseEvent == xterm_MouseEvent) {
	    match = s;
	    while ((match = memchr(match, '\033', got))) {
		if (got >= (match - s) + 6 && !strncmp(match, "\033[M", 3)) {
		    /* classic xterm mouse reporting (X11 specs) */
		    CopyMem(match, xterm_MouseData, 6);
		    CopyMem(match+6, match, got - (match - s) - 6);
		    got -= 6;
		} else if (got >= (match - s) + 9 && !strncmp(match, "\033[5M", 4)) {
		    /* enhanced xterm-style reporting (twin specs) */
		    CopyMem(match, xterm_MouseData, 9);
		    CopyMem(match+9, match, got - (match - s) - 9);
		    got -= 9;
		} else {
		    match++;
		    continue;
		}
		xterm_MouseEvent(fd, HW);
	    }
	}
#endif /* CONF_HW_TTY_TWTERM */

	/* ok, now try to find out the correct KeyCode for the ASCII sequence we got */
	
	if (!(chunk = got))
	    break;
	
	if (chunk > 1) {
	    if ((Code = stdin_LookupKey(&chunk, s)) == TW_Null)
		chunk = 1;
	} else
	    Code = (udat)*s;

	KeyboardEventCommon(Code, chunk, s);
	s += chunk, got -= chunk;
    }
    
    RestoreHW;
}





/*
 * Linux gpm mouse input:
 * mouse input uses libgpm to connect to `gpm' mouse daemon
 * and read mouse state, but draws mouse pointer manually
 */

#ifdef CONF_HW_TTY_LINUX


static void GPM_QuitMouse(void);
static void GPM_MouseEvent(int fd, display_hw *hw);

static int wrap_GPM_Open(void) {
    /*
     * HACK! this works around a quirk in libgpm:
     * if Gpm_Open fails, it sets gpm_tried to non-zero
     * and following calls will just fail, not even trying anymore
     */
    extern int gpm_tried;
    
    if (!tty_name) {
	fputs("      gpm_InitMouse() failed: unable to detect tty device\n", stderr);
	return NOFD;
    }
    if (tty_number < 1 || tty_number > 63) {
	fprintf(stderr, "      gpm_InitMouse() failed: terminal `%s'\n"
			 "      is not a local linux console.\n", tty_name);
	return NOFD;
    }

    gpm_tried = 0;
    
    gpm_zerobased = 1;
    gpm_visiblepointer = 0;
    GPM_Conn.eventMask = ~0;		/* Get everything */
    GPM_Conn.defaultMask = ~GPM_HARD;	/* Pass everything unused */
    GPM_Conn.minMod = 0;		/* Run always... */
    GPM_Conn.maxMod = ~0;		/* ...with any modifier */

    GPM_fd = Gpm_Open(&GPM_Conn, tty_number);
    
    if (GPM_fd >= 0) {
	/* gpm_consolefd is opened by GPM_Open() */
	fcntl(gpm_consolefd, F_SETFD, FD_CLOEXEC);
    }
    return GPM_fd;
}



/* return FALSE if failed */
static byte GPM_InitMouse(void) {
    
    if (GPM_InUse) {
	fputs("      GPM_InitMouse() failed: already connected to `gpm'.\n", stderr);
	return FALSE;
    }
    
    if (wrap_GPM_Open() < 0) {
	fputs("      GPM_InitMouse() failed: unable to connect to `gpm'.\n"
	      "      make sure you started `twin' from the console\n"
	      "      and/or check that `gpm' is running.\n", stderr);
	return FALSE;
    }
    fcntl(GPM_fd, F_SETFD, FD_CLOEXEC);
    fcntl(GPM_fd, F_SETFL, O_NONBLOCK);
    
    HW->mouse_slot = RegisterRemote(GPM_fd, HW, (void *)GPM_MouseEvent);
    if (HW->mouse_slot == NOSLOT) {
	Gpm_Close();
	return FALSE;
    }
    
    HW->FlagsHW |= FlHWSoftMouse; /* _we_ Hide/Show it */
    
    HW->MouseEvent = GPM_MouseEvent;
    HW->QuitMouse = GPM_QuitMouse;

    GPM_InUse = TRUE;
    
    return TRUE;
}

static void GPM_QuitMouse(void) {
    HW->HideMouse();
    Gpm_Close();

    UnRegisterRemote(HW->mouse_slot);
    HW->mouse_slot = NOSLOT;

    GPM_InUse = FALSE;
    
    HW->QuitMouse = NoOp;
}

static void GPM_MouseEvent(int fd, display_hw *hw) {
    udat IdButtons, Buttons = 0;
    Gpm_Event GPM_EV;
    
    SaveHW;
    
    /*
     * All other parts of twin read and parse data from fds in big chunks,
     * while Gpm_GetEvent() reads and parses only a single event at time.
     * To compensate this and avoid mouse to lag behind, we do a small loop.
     */
    byte loopN = 30;
    
    SetHW(hw);
    
    while (loopN--) {
	if (Gpm_GetEvent(&GPM_EV) <= 0) {
	    if (loopN == 29)
		HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	    return;
	}
	
#if 0
	All->FullShiftFlags =
	    (GPM_EV.modifiers & 1 ? FULL_LEFT_SHIFT_PRESSED : 0)
	    | (GPM_EV.modifiers & 2 ? FULL_RIGHT_ALT_PRESSED  : 0)
	    | (GPM_EV.modifiers & 4 ? FULL_LEFT_CTRL_PRESSED  : 0)
	    | (GPM_EV.modifiers & 8 ? FULL_LEFT_ALT_PRESSED   : 0);
#endif
	
	/*
	 * Gpm differs from us about what buttons to report on release events:
	 * it reports which buttons get _released_, not which are still _pressed_
	 * Fixed here. SIGH.
	 */
	IdButtons = GPM_EV.buttons;
	
	if (GPM_EV.type & GPM_UP)
	    IdButtons = GPM_keys & ~IdButtons;
	GPM_keys = IdButtons;
	
	if (IdButtons & GPM_B_LEFT)
	    Buttons |= HOLD_LEFT;
	if (IdButtons & GPM_B_MIDDLE)
	    Buttons |= HOLD_MIDDLE;
	if (IdButtons & GPM_B_RIGHT)
	    Buttons |= HOLD_RIGHT;
	
	MouseEventCommon(GPM_EV.x, GPM_EV.y, GPM_EV.dx, GPM_EV.dy, Buttons);
    }
    RestoreHW;
}


#endif /* CONF_HW_TTY_LINUX */



/*
 * Linux:
 * fallback, xterm-style mouse input
 * 
 * used mostly to let twin run inside a twin terminal
 */


#ifdef CONF_HW_TTY_TWTERM

static void xterm_QuitMouse(void);
static void xterm_MouseEvent(int fd, display_hw *hw);


/* return FALSE if failed */
static byte xterm_InitMouse(void) {
    byte *term = tty_TERM, *seq;
    
    if (!term) {
	fputs("      xterm_InitMouse() failed: unknown terminal type.\n", stderr);
	return FALSE;
    }

    if (!strcmp(term, "linux")) {
	/*
	 * additional check... out-of-the box linux
	 * doesn't have xterm-style mouse reporting
	 */
	if (ttypar[0]<6 || (ttypar[0]==6 && ttypar[1]<3)) {
	    fputs("      xterm_InitMouse() failed: this `linux' terminal\n"
		  "      has no support for xterm-style mouse reporting.\n", stderr);
	    return FALSE;
	}
	seq = "\033[?9h";
    } else if (!strncmp(term, "xterm", 5) || !strncmp(term, "rxvt", 4)) {
	/* twin doesn't fully support xterm, rxvt etc...
	 * this is just a rough attempt */
	seq = "\033[?1001s\033[?1000h";
    } else {
	fprintf(stderr, "      xterm_InitMouse() failed: terminal `%s' is not supported.\n", term);
	return FALSE;
    }

    fputs(seq, stdOUT);
    setFlush();
    
    HW->mouse_slot = NOSLOT; /* shared with keyboard */
    
    HW->MouseEvent = xterm_MouseEvent;
    HW->QuitMouse = xterm_QuitMouse;
    
    HW->FlagsHW &= ~FlHWSoftMouse; /* no need to Hide/Show it */
    HW->ShowMouse = HW->HideMouse = NoOp; /* override the ones set by InitVideo() */
    
    return TRUE;
}

static void xterm_QuitMouse(void) {
    if (ttypar[0]==6 && ttypar[1]>=3)
	fputs("\033[?9l", stdOUT);
    else /* xterm or similar */
	fputs("\033[?1000l\033[?1001r", stdOUT);
    HW->QuitMouse = NoOp;
}

static void xterm_MouseEvent(int fd, display_hw *hw) {
    static dat prev_x, prev_y;
    udat Buttons = 0, Id;
    byte *s = xterm_MouseData;
    dat x, y, dx, dy;
    
    if (*s == '\033' && *++s == '[') {
	if (*++s == 'M' && (Id=*++s) <= '#') {
	    /* classic xterm mouse reporting (X11 specs) */
	    if (Id == ' ')
		Buttons |= HOLD_LEFT;
	    else if (Id == '!')
		Buttons |= HOLD_MIDDLE;
	    else if (Id == '\"')
		Buttons |= HOLD_RIGHT;
	    x = *++s - '!'; /* (*s) must be unsigned char */
	    y = *++s - '!';
	} else if (*s == '5' && *++s == 'M' && (Id = *++s) >= ' ' && (Id -= ' ') <= HOLD_ANY) {
	    /* enhanced xterm-style reporting (twin specs) */
	    Buttons = Id;
	    /* it would be: 
	     * (Id & HOLD_LEFT ? HOLD_LEFT : 0) |
	     * (Id & HOLD_MIDDLE ? HOLD_MIDDLE : 0) |
	     * (Id & HOLD_RIGHT ? HOLD_RIGHT : 0);
	     */
	    s++;
	    x = (udat)((s[0]-'!') & 0x7f) | (udat)((udat)((s[1]-'!') & 0x7f) << 7);
	    if (x & ((udat)1 << 13))
		/* top bit is set, set also higher ones */
		x |= (udat)~0 << 14;
	    
	    s+=2;
	    y = (udat)((s[0]-'!') & 0x7f) | (udat)((udat)((s[1]-'!') & 0x7f) << 7);
	    if (y & ((udat)1 << 13))
		y |= (udat)~0 << 14;
	} else
	    s = NULL;
    } else
	s = NULL;

    if (s) {
	x = Max2(x, 0);
	x = Min2(x, ScreenWidth - 1);
    
	y = Max2(y, 0);
	y = Min2(y, ScreenHeight - 1);
    
	if (x == 0 && prev_x == 0)
	    dx = -1;
	else if (x == ScreenWidth - 1 && prev_x == ScreenWidth - 1)
	    dx = 1;
	else
	    dx = 0;
	if (y == 0 && prev_y == 0)
	    dy = -1;
	else if (y == ScreenHeight - 1 && prev_y == ScreenHeight - 1)
	    dy = 1;
	else
	    dy = 0;
	
	prev_x = x;
	prev_y = y;
	
	MouseEventCommon(x, y, dx, dy, Buttons);
    }
}

#endif /* CONF_HW_TTY_TWTERM */


static byte warn_NoMouse(void) {
    byte c;
    
    fputs("\n"
#if defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
	  "      \033[1m  ALL  MOUSE  DRIVERS  FAILED.\033[0m\n"
#else
	  "      \033[1m TTY DRIVER COMPILED WITHOUT ANY MOUSE SUPPORT.\033[0m\n"
#endif
	  "\n"
	  "      If you really want to run `twin' without mouse\n"
	  "      hit RETURN to continue, otherwise hit CTRL-C to quit now.\n", stderr);
    fflush(stderr);
    
    read(tty_fd, &c, 1);
    if (c == '\n' || c == '\r') {
    
	HW->mouse_slot = NOSLOT; /* no mouse at all :( */
    
	HW->MouseEvent = (void *)NoOp;
	HW->QuitMouse = NoOp;
    
	HW->FlagsHW &= ~FlHWSoftMouse; /* no need to Hide/Show it */
	HW->ShowMouse = HW->HideMouse = NoOp; /* override the ones set by InitVideo() */
	
	return TRUE;
    }
    return FALSE;
}



static void stdin_DetectSize(udat *x, udat *y) {
    struct winsize wsiz;

    if (ioctl(tty_fd, TIOCGWINSZ, &wsiz) >= 0 && wsiz.ws_row > 0 && wsiz.ws_col > 0) {
	HW->X = wsiz.ws_col;
	HW->Y = wsiz.ws_row;
    }
    
    *x = HW->X;
    *y = HW->Y;
}

static void stdin_CheckResize(udat *x, udat *y) {
    *x = Min2(*x, HW->X);
    *y = Min2(*y, HW->Y);
}

static void stdin_Resize(udat x, udat y) {
    if (x < HW->usedX || y < HW->usedY) {
	/*
	 * can't resize the tty, just clear it so that
	 * extra size will get padded with blanks
	 */
	fputs("\033[0m\033[2J", stdOUT);
	fflush(stdOUT);
	/*
	 * flush now not to risk arriving late
	 * and clearing the screen AFTER vcsa_FlushVideo()
	 */
	NeedRedrawVideo(0, 0, x - 1, y - 1);
    }
    HW->usedX = x;
    HW->usedY = y;
}
	    

INLINE void stdout_SetCursorType(uldat type) {
    fprintf(stdOUT, "\033[?%d;%d;%dc",
		(int)(type & 0xFF),
		(int)((type >> 8) & 0xFF),
		(int)((type >> 16) & 0xFF));
}
INLINE void stdout_MoveToXY(udat x, udat y) {
    fprintf(stdOUT, "\033[%d;%dH", y+1, x+1);
}
static void stdout_Beep(void);
static void stdout_Configure(udat resource, byte todefault, udat value);
static void stdout_FlushHW(void);
static void stdout_SetPalette(udat N, udat R, udat G, udat B);
static void stdout_ResetPalette(void);
static void tty_UpdateMouseAndCursor(void);

static byte stdout_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
static void stdout_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);

/* output through /dev/vcsaXX */


#ifdef CONF_HW_TTY_LINUX

static void vcsa_QuitVideo(void);
static void vcsa_FlushVideo(void);
static void vcsa_ShowMouse(void);
static void vcsa_HideMouse(void);

/* return FALSE if failed */
static byte vcsa_InitVideo(void) {
    static byte vcsa_name[] = "/dev/vcsaXX";
    
    if (!tty_name) {
	fputs("      vcsa_InitVideo() failed: unable to detect tty device\n", stderr);
	return FALSE;
    }
    
    if (tty_number < 1 || tty_number > 63) {
	fprintf(stderr, "      vcsa_InitVideo() failed: terminal `%s'\n"
			 "      is not a local linux console.\n", tty_name);
	return FALSE;
    }

    vcsa_name[9] = tty_name[8];
    vcsa_name[10] = tty_name[9];
    
    GetPrivileges();
    VcsaFd = open(vcsa_name, O_WRONLY|O_NOCTTY);
    DropPrivileges();
    
    if (VcsaFd < 0) {
	fprintf(stderr, "      vcsa_InitVideo() failed: unable to open `%s': %s\n",
		vcsa_name, strerror(errno));
	return FALSE;
    }
    fcntl(VcsaFd, F_SETFD, FD_CLOEXEC);
    
    fputs("\033[2J", stdOUT); /* clear screen */
    fflush(stdOUT);
    
    HW->FlushVideo = vcsa_FlushVideo;
    HW->FlushHW = stdout_FlushHW;

    HW->ShowMouse = vcsa_ShowMouse;
    HW->HideMouse = vcsa_HideMouse;
    HW->UpdateMouseAndCursor = tty_UpdateMouseAndCursor;

    HW->DetectSize  = stdin_DetectSize; stdin_DetectSize(&HW->usedX, &HW->usedY);
    HW->CheckResize = stdin_CheckResize;
    HW->Resize      = stdin_Resize;
    
    HW->HWSelectionImport  = AlwaysFalse;
    HW->HWSelectionExport  = NoOp;
    HW->HWSelectionRequest = (void *)NoOp;
    HW->HWSelectionNotify  = (void *)NoOp;
    HW->HWSelectionPrivate = NULL;
    
    HW->CanDragArea = stdout_CanDragArea;
    HW->DragArea = stdout_DragArea;
   
    HW->XY[0] = HW->XY[1] = 0;
    HW->TT = (uldat)-1; /* force updating cursor */
    
    HW->Beep = stdout_Beep;
    HW->Configure = stdout_Configure;
    HW->SetPalette = stdout_SetPalette;
    HW->ResetPalette = stdout_ResetPalette;

    HW->QuitVideo = vcsa_QuitVideo;
    
    HW->FlagsHW |= FlHWNeedOldVideo;
    HW->FlagsHW &= ~FlHWExpensiveFlushVideo;
    HW->NeedHW = 0;
    HW->CanResize = FALSE;
    HW->merge_Threshold = 40;

    return TRUE;
}

static void vcsa_QuitVideo(void) {
    stdout_MoveToXY(0, ScreenHeight-1);
    stdout_SetCursorType(LINECURSOR);
    fputs("\033[0m\033[3l\n", stdOUT); /* clear colors, TTY_DISPCTRL */
    
    close(VcsaFd);
    
    HW->QuitVideo = NoOp;
}


static void vcsa_FlushVideo(void) {
    dat i, j;
    uldat prevS = (uldat)-1, prevE = (uldat)-1, _prevS, _prevE, _start, _end, start, end;
    byte FlippedVideo = FALSE, FlippedOldVideo = FALSE;
    hwattr savedOldVideo;
    
    if (!ChangedVideoFlag) {
	HW->UpdateMouseAndCursor();
	return;
    }
    
    /* this hides the mouse if needed ... */
    
    /* first, check the old mouse position */
    if (HW->FlagsHW & FlHWSoftMouse) {
	if (HW->FlagsHW & FlHWChangedMouseFlag) {
	    /* dirty the old mouse position, so that it will be overwritten */
	    
	    /*
	     * with multi-display this is a hack, but since OldVideo gets restored
	     * by VideoFlip() below *BEFORE* returning from vcsa_FlushVideo(),
	     * that's ok.
	     */
	    DirtyVideo(HW->Last_x, HW->Last_y, HW->Last_x, HW->Last_y);
	    if (ValidOldVideo) {
		FlippedOldVideo = TRUE;
		savedOldVideo = OldVideo[HW->Last_x + HW->Last_y * ScreenWidth];
		OldVideo[HW->Last_x + HW->Last_y * ScreenWidth] = ~Video[HW->Last_x + HW->Last_y * ScreenWidth];
	    }
	}
	
	i = HW->MouseState.x;
	j = HW->MouseState.y;
	/*
	 * instead of calling ShowMouse(),
	 * we flip the new mouse position in Video[] and dirty it if necessary.
	 * this avoids glitches if the mouse is between two dirty areas
	 * that get merged.
	 */
	if ((HW->FlagsHW & FlHWChangedMouseFlag) || (FlippedVideo = Threshold_isDirtyVideo(i, j))) {
	    VideoFlip(i, j);
	    if (!FlippedVideo)
		DirtyVideo(i, j, i, j);
	    HW->FlagsHW &= ~FlHWChangedMouseFlag;
	    FlippedVideo = TRUE;
	} else
	    FlippedVideo = FALSE;
    }
    
    for (i=0; i<ScreenHeight*2; i++) {
	_start = start = (uldat)ChangedVideo[i>>1][i&1][0];
	_end   = end   = (uldat)ChangedVideo[i>>1][i&1][1];
	    
	if (start != (uldat)-1) {
		
	    /* actual tty size could be different from ScreenWidth*ScreenHeight... */
	    start += (i>>1) * ScreenWidth;
	    end   += (i>>1) * ScreenWidth;

	    _start += (i>>1) * HW->X;
	    _end   += (i>>1) * HW->X;
		
		
	    if (prevS != (uldat)-1) {
		if (start - prevE < HW->merge_Threshold) {
		    /* the two chunks are (almost) contiguous, merge them */
		    /* if HW->X != ScreenWidth we can merge only if they do not wrap */
		    if (HW->X == ScreenWidth || prevS / ScreenWidth == end / ScreenWidth) {
			_prevE = prevE = end;
			continue;
		    }
		}
		lseek(VcsaFd, 4+_prevS*sizeof(hwattr), SEEK_SET);
		write(VcsaFd, (void *)&Video[prevS], (prevE-prevS+1)*sizeof(hwattr));
	    }
	    prevS = start;
	    prevE = end;
	    _prevS = _start;
	    _prevE = _end;
	}
    }
    if (prevS != (uldat)-1) {
	lseek(VcsaFd, 4+_prevS*sizeof(hwattr), SEEK_SET);
	write(VcsaFd, (char *)&Video[prevS], (prevE-prevS+1)*sizeof(hwattr));
    }
    
    /* ... and this redraws the mouse */
    if (HW->FlagsHW & FlHWSoftMouse) {
	if (FlippedOldVideo)
	    OldVideo[HW->Last_x + HW->Last_y * ScreenWidth] = savedOldVideo;
	if (FlippedVideo)
	    VideoFlip(HW->Last_x = HW->MouseState.x, HW->Last_y = HW->MouseState.y);
	else if (HW->FlagsHW & FlHWChangedMouseFlag)
	    HW->ShowMouse();
    }
    
    /* now the cursor */
    
    if (CursorType != NOCURSOR && (CursorX != HW->XY[0] || CursorY != HW->XY[1])) {
	stdout_MoveToXY(HW->XY[0] = CursorX, HW->XY[1] = CursorY);
	setFlush();
    }
    if (CursorType != HW->TT) {
	stdout_SetCursorType(HW->TT = CursorType);
	setFlush();
    }

    HW->FlagsHW &= ~FlHWChangedMouseFlag;
}


/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have vcsa_ and stdout_ versions, not GPM_ ones... */
static void vcsa_ShowMouse(void) {
    uldat pos = (HW->Last_x = HW->MouseState.x) + (HW->Last_y = HW->MouseState.y) * ScreenWidth;
    uldat _pos = HW->Last_x + HW->Last_y * HW->X;
    
    hwattr h  = Video[pos];
    hwcol c = ~HWCOL(h) ^ COL(HIGH,HIGH);

    h = HWATTR( c, HWFONT(h) );

    lseek(VcsaFd, 4+_pos*sizeof(hwattr), SEEK_SET);
    write(VcsaFd, (char *)&h, sizeof(hwattr));
}

static void vcsa_HideMouse(void) {
    uldat pos = HW->Last_x + HW->Last_y * ScreenWidth;
    uldat _pos = HW->Last_x + HW->Last_y * HW->X;

    lseek(VcsaFd, 4+_pos*sizeof(hwattr), SEEK_SET);
    write(VcsaFd, (char *)&Video[pos], sizeof(hwattr));
}

#endif /* CONF_HW_TTY_LINUX */








/*
 * As alternate method, we also provide
 * output through stdout.
 * 
 * it is slower (?), but in these days it shouldn't be a problem.
 * 
 * this is used both to run inside a twin terminal,
 * and as fallback (if vcsa_InitVideo() fails) to run on a Linux console.
 */


static void stdout_QuitVideo(void);
static void stdout_FlushVideo(void);
static void stdout_ShowMouse(void);
static void stdout_HideMouse(void);

/* return FALSE if failed */
static byte stdout_InitVideo(void) {
    byte *term = tty_TERM;
    
    if (!term) {
	fputs("      stdout_InitVideo() failed: unknown terminal type.\n", stderr);
	return FALSE;
    }
    
    do if (strcmp(term, "linux")) {
	if (!strncmp(term, "xterm", 5) || !strncmp(term, "rxvt", 4)) {
	    byte c;
	    
	    fprintf(stderr, "\n"
		    "      \033[1m  WARNING: terminal `%s' is not fully supported.\033[0m\n"
		    "\n"
		    "      If you really want to run `twin' on this terminal\n"
		    "      hit RETURN to continue, otherwise hit CTRL-C to quit now.\n", term);
	    fflush(stderr);
    
	    read(tty_fd, &c, 1);
	    if (c == '\n' || c == '\r')
		break;
	}
	fprintf(stderr, "      stdout_InitVideo() failed: terminal type `%s' not supported.\n", term);
	return FALSE;
    } while (0);

    fputs("\033[0;11m\033[2J\033[3h", stdOUT); /* clear colors, clear screen, */
					       /* set IBMPC consolemap, set TTY_DISPCTRL */
    
    HW->FlushVideo = stdout_FlushVideo;
    HW->FlushHW = stdout_FlushHW;

    HW->ShowMouse = stdout_ShowMouse;
    HW->HideMouse = stdout_HideMouse;
    HW->UpdateMouseAndCursor = tty_UpdateMouseAndCursor;
    
    HW->DetectSize  = stdin_DetectSize;
    HW->CheckResize = stdin_CheckResize;
    HW->Resize      = stdin_Resize;
    
    HW->HWSelectionExport = (void *)NoOp;
    HW->HWSelectionRequest = (void *)NoOp;
    HW->HWSelectionNotify = (void *)NoOp;
    
    HW->CanDragArea = stdout_CanDragArea;
    HW->DragArea = stdout_DragArea;
   
    HW->XY[0] = HW->XY[1] = 0;
    HW->TT = -1; /* force updating the cursor */
	
    HW->Beep = stdout_Beep;
    HW->Configure = stdout_Configure;
    HW->SetPalette = stdout_SetPalette;
    HW->ResetPalette = stdout_ResetPalette;

    HW->QuitVideo = stdout_QuitVideo;
    
    HW->FlagsHW |= FlHWNeedOldVideo;
    HW->FlagsHW &= ~FlHWExpensiveFlushVideo;
    HW->NeedHW = 0;
    HW->merge_Threshold = 0;
    
    return TRUE;
}

static void stdout_QuitVideo(void) {
    stdout_MoveToXY(0, ScreenHeight-1);
    stdout_SetCursorType(LINECURSOR);
    fputs("\033[0;10m\033[3l\n", stdOUT); /* restore original colors, consolemap and TTY_DISPCTRL */
    
    HW->QuitVideo = NoOp;
}

/* this can stay static, as it's used only as temporary storage */
static hwcol _col;

#define CTRL_ALWAYS 0x0800f501	/* Cannot be overridden by TTY_DISPCTRL */

#define stdout_MogrifyInit() fputs("\033[0m", stdOUT); _col = COL(WHITE,BLACK)
#define stdout_MogrifyNoCursor() fputs("\033[?25l", stdOUT);
#define stdout_MogrifyYesCursor() fputs("\033[?25h", stdOUT);

INLINE void stdout_SetColor(hwcol col) {
    static byte colbuf[] = "\033[2x;2x;4x;3xm";
    byte c, *colp = colbuf+2;
    
    if ((col & COL(HIGH,0)) != (_col & COL(HIGH,0))) {
	if (_col & COL(HIGH,0)) *colp++ = '2';
	*colp++ = '1'; *colp++ = ';';
    }
    if ((col & COL(0,HIGH)) != (_col & COL(0,HIGH))) {
	if (_col & COL(0,HIGH)) *colp++ = '2';
	*colp++ = '5'; *colp++ = ';';
    }
    if ((col & COL(0,WHITE)) != (_col & COL(0,WHITE))) {
	c = COLBG(col) & ~HIGH;
	*colp++ = '4'; *colp++ = VGA2ANSI(c) + '0'; *colp++ = ';';
    }
    if ((col & COL(WHITE,0)) != (_col & COL(WHITE,0))) {
	c = COLFG(col) & ~HIGH;
	*colp++ = '3'; *colp++ = VGA2ANSI(c) + '0'; *colp++ = ';';
    }
    _col = col;
    
    if (colp[-1] == ';') --colp;
    *colp++ = 'm'; *colp   = '\0';
    
    fputs(colbuf, stdOUT);
}

INLINE void stdout_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV;
    hwcol col;
    byte c, sending = FALSE;
    
    V = Video + x + y * ScreenWidth;
    oV = OldVideo + x + y * ScreenWidth;
	
    for (; len; V++, oV++, x++, len--) {
	if (!ValidOldVideo || *V != *oV) {
	    if (!sending)
		sending = TRUE, fprintf(stdOUT, "\033[%d;%dH", 1 + y, 1 + x);

	    col = HWCOL(*V);
	    
	    if (col != _col)
		stdout_SetColor(col);
	
	    c = HWFONT(*V);
	    if ((c < 32 && ((CTRL_ALWAYS >> c) & 1)) || c == 128+27)
		putc(' ', stdOUT); /* can't display it */
	    else
		putc(c, stdOUT);
	} else
	    sending = FALSE;
    }
}

static void stdout_FlushVideo(void) {
    dat i, j;
    dat start, end;
    byte FlippedVideo = FALSE, FlippedOldVideo = FALSE;
    hwattr savedOldVideo;
    
    if (!ChangedVideoFlag) {
	HW->UpdateMouseAndCursor();
	return;
    }

    /* hide the mouse if needed */
    
    /* first, check the old mouse position */
    if (HW->FlagsHW & FlHWSoftMouse) {
	if (HW->FlagsHW & FlHWChangedMouseFlag) {
	    /* dirty the old mouse position, so that it will be overwritten */
	    
	    /*
	     * with multi-display this is a hack, but since OldVideo gets restored
	     * below *BEFORE* returning from stdout_FlushVideo(), that's ok.
	     */
	    DirtyVideo(HW->Last_x, HW->Last_y, HW->Last_x, HW->Last_y);
	    if (ValidOldVideo) {
		FlippedOldVideo = TRUE;
		savedOldVideo = OldVideo[HW->Last_x + HW->Last_y * ScreenWidth];
		OldVideo[HW->Last_x + HW->Last_y * ScreenWidth] = ~Video[HW->Last_x + HW->Last_y * ScreenWidth];
	    }
	}
	
        i = HW->MouseState.x;
	j = HW->MouseState.y;
	/*
	 * instead of calling ShowMouse(),
	 * we flip the new mouse position in Video[] and dirty it if necessary.
	 */
	if ((HW->FlagsHW & FlHWChangedMouseFlag) || (FlippedVideo = Plain_isDirtyVideo(i, j))) {
	    VideoFlip(i, j);
	    if (!FlippedVideo)
		DirtyVideo(i, j, i, j);
	    HW->FlagsHW &= ~FlHWChangedMouseFlag;
	    FlippedVideo = TRUE;
	} else
	    FlippedVideo = FALSE;
    }
	
    stdout_MogrifyInit();
    stdout_MogrifyNoCursor();
    for (i=0; i<ScreenHeight*2; i++) {
	start = ChangedVideo[i>>1][i&1][0];
	end   = ChangedVideo[i>>1][i&1][1];
	
	if (start != -1)
	    stdout_Mogrify(start, i>>1, end-start+1);
    }
    /* put the cursor back in place */
    stdout_MogrifyYesCursor();
    HW->XY[0] = HW->XY[1] = -1;
    
    setFlush();
    
    /* ... and this redraws the mouse */
    if (HW->FlagsHW & FlHWSoftMouse) {
	if (FlippedOldVideo)
	    OldVideo[HW->Last_x + HW->Last_y * ScreenWidth] = savedOldVideo;
	if (FlippedVideo)
	    VideoFlip(HW->Last_x = HW->MouseState.x, HW->Last_y = HW->MouseState.y);
	else if (HW->FlagsHW & FlHWChangedMouseFlag)
	    HW->ShowMouse();
    }
    
    if ((CursorX != HW->XY[0] || CursorY != HW->XY[1]) && (CursorType != NOCURSOR)) {
	stdout_MoveToXY(HW->XY[0] = CursorX, HW->XY[1] = CursorY);
	setFlush();
    }
    if (CursorType != HW->TT) {
	stdout_SetCursorType(HW->TT = CursorType);
	setFlush();
    }
    HW->FlagsHW &= ~FlHWChangedMouseFlag;
}

static void stdout_FlushHW(void) {
    if (fflush(stdOUT) != 0)
	HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
    clrFlush();
}

INLINE void stdout_SingleMogrify(dat x, dat y, hwattr V) {
    byte c;
    
    fprintf(stdOUT, "\033[%d;%dH", 1 + y, 1 + x);

    if (HWCOL(V) != _col)
	stdout_SetColor(HWCOL(V));
	
    c = HWFONT(V);
    if ((c < 32 && ((CTRL_ALWAYS >> c) & 1)) || c == 128+27)
	putc(' ', stdOUT); /* can't display it */
    else
	putc(c, stdOUT);
}

/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have vcsa_ and stdout_ versions, not GPM_ ones... */
static void stdout_ShowMouse(void) {
    uldat pos = (HW->Last_x = HW->MouseState.x) + (HW->Last_y = HW->MouseState.y) * ScreenWidth;
    hwattr h  = Video[pos];
    hwcol c = ~HWCOL(h) ^ COL(HIGH,HIGH);

    stdout_SingleMogrify(HW->MouseState.x, HW->MouseState.y, HWATTR( c, HWFONT(h) ));

    /* put the cursor back in place */
    HW->XY[0] = HW->XY[1] = (udat)-1;
    setFlush();
}

static void stdout_HideMouse(void) {
    uldat pos = HW->Last_x + HW->Last_y * ScreenWidth;

    stdout_SingleMogrify(HW->Last_x, HW->Last_y, Video[pos]);

    /* put the cursor back in place */
    HW->XY[0] = HW->XY[1] = (udat)-1;
    setFlush();
}

static void tty_UpdateMouseAndCursor(void) {
    if ((HW->FlagsHW & FlHWSoftMouse) && (HW->FlagsHW & FlHWChangedMouseFlag)) {
	HW->HideMouse();
	HW->ShowMouse();
	HW->FlagsHW &= ~FlHWChangedMouseFlag;
    }

    if ((CursorX != HW->XY[0] || CursorY != HW->XY[1]) && (CursorType != NOCURSOR)) {
	stdout_MoveToXY(HW->XY[0] = CursorX, HW->XY[1] = CursorY);
	setFlush();
    }
    if (CursorType != HW->TT) {
	stdout_SetCursorType(HW->TT = CursorType);
	setFlush();
    }
}

static void stdout_Beep(void) {
    fputs("\033[3l\007\033[3h", stdOUT);
    setFlush();
}


static void stdout_Configure(udat resource, byte todefault, udat value) {
    switch (resource) {
      case HW_KBDAPPLIC:
	fputs(todefault || !value ? "\033>" : "\033=", stdOUT);
	setFlush();
	break;
      case HW_ALTCURSKEYS:
	fputs(todefault || !value ? "\033[?1l" : "\033[?1h", stdOUT);
	setFlush();
	break;
      case HW_BELLPITCH:
	if (todefault)
	    fputs("\033[10]", stdOUT);
	else
	    fprintf(stdOUT, "\033[10;%hd]", value);
	setFlush();
	break;
      case HW_BELLDURATION:
	if (todefault)
	    fputs("\033[11]", stdOUT);
	else
	    fprintf(stdOUT, "\033[11;%hd]", value);
	setFlush();
	break;
      default:
	break;
    }
}

    
static void stdout_SetPalette(udat N, udat R, udat G, udat B) {
    fprintf(stdOUT, "\033]P%1hx%02hx%02hx%02hx", N, R, G, B);
    setFlush();
}

static void stdout_ResetPalette(void) {
    fputs("\033]R", stdOUT);
    setFlush();
}



#if 1
static byte stdout_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return Left == 0 && Rgt == HW->X-1 && Dwn == HW->Y-1 && DstUp == 0;
}


/* it works, but it's MUCH slower than doing it the normal way */
static void stdout_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    udat delta = Up - DstUp;
	
    HW->HideMouse();
    HW->FlagsHW |= FlHWChangedMouseFlag;
	
    fprintf(stdOUT,
	    "\033[?1c\033[0m"	/* hide cursor, reset color */
	    "\033[%d;1H", HW->Y);/* go to last line */
	    
    while (delta--)
	putc('\n', stdOUT);
    
    if (HW->FlushVideo == stdout_FlushVideo)
	setFlush();
    else
	fflush(stdOUT);
	
    /* this will restore the cursor */
    HW->TT = -1;
    
    /*
     * now the last trick: tty scroll erased the part
     * below DstUp + (Dwn - Up) so we must redraw it.
     */
    NeedRedrawVideo(0, DstUp + (Dwn - Up) + 1, HW->X - 1, HW->Y - 1);
}
#endif /* 0 */

static void tty_QuitHW(void) {
    HW->QuitMouse();
    HW->QuitVideo();
    HW->QuitKeyboard();
    HW->QuitHW = NoOp;
    
    if (tty_name)
	FreeMem(tty_name);
    if (tty_TERM)
	FreeMem(tty_TERM);
    if (HW->DisplayIsCTTY && DisplayHWCTTY == HW)
	DisplayHWCTTY = NULL;
    
    fflush(stdOUT);
    if (tty_fd)
	fclose(stdOUT);

    FreeMem(HW->Private);
}

/*
 * note: during xxx_InitHW() initialization, DON'T use ScreenWidth/ScreenHeight
 * as they may be not up to date. Use GetDisplayWidth() / GetDisplayHeight().
 */
byte tty_InitHW(void) {
    byte *arg = HW->Name;
    byte *s;
    byte force_stdout = FALSE;

    if (!(HW->Private = (struct tty_data *)AllocMem(sizeof(struct tty_data)))) {
	fprintf(stderr, "      tty_InitHW(): Out of memory!\n");
	return FALSE;
    }
    saveCursorType = (uldat)-1;
    saveX = saveY = 0;
    stdOUT = NULL;
    
    if (arg && HW->NameLen > 4) {
	arg += 4;
	
	if (strncmp(arg, "tty", 3))
	    return FALSE; /* user said "use <arg> as display, not tty" */
	arg += 3;
	
	if (*arg == '@') {
	    s = strchr(++arg, ',');
	    if (s) *s = '\0';
	    
	    stdOUT = fopen(arg, "a+"); /* use specified tty */
	    if (!stdOUT) {
		fprintf(stderr, "      tty_InitHW(): fdopen(tty) failed: %s\n", strerror(errno));
		if (s) *s++ = ',';
		return FALSE;
	    }
	    tty_fd = fileno(stdOUT);
	    fcntl(tty_fd, F_SETFD, FD_CLOEXEC);
	    tty_name = CloneStr(arg);
	    if (s) *s = ',';
	    arg = s;

	}
	
	while (arg && *arg) {
	    /* parse options */
	    if (!strncmp(arg, ",TERM=", 6)) {
		s = strchr(arg += 6, ',');
		if (s) *s = '\0';
		tty_TERM = CloneStr(arg);
		if (s) *s = ',';
		arg = s;
	    } else if (!strncmp(arg, ",stdout", 7)) {
		s = strchr(arg += 7, ',');
		arg = s;
		force_stdout = TRUE;
	    } else if (!strncmp(arg, ",noinput", 8)) {
		s = strchr(arg += 8, ',');
		arg = s;
		HW->FlagsHW |= FlHWNoInput;
	    } else
		break;
	}
    }
    
    if (!stdOUT) {
	if (DisplayHWCTTY) {
	    fprintf(stderr, "      tty_InitHW() failed: controlling tty %s\n",
		    DisplayHWCTTY == HWCTTY_DETACHED ? "not usable after Detach" : "is already in use as display");
	    return FALSE;
	} else {
	    tty_fd = 0;
	    stdOUT = stdout;
	    tty_name = CloneStr(ttyname(0));
	    tty_TERM = CloneStr(origTERM);
	}
    }
    fflush(stdOUT);
    setvbuf(stdOUT, NULL, _IOFBF, BUFSIZ);

    tty_number = 0;
    if (tty_name && !strncmp(tty_name, "/dev/tty", 8)) {
	s = tty_name + 8;
	while (*s && *s >= '0' && *s <= '9') {
	    tty_number *= 10;
	    tty_number += *s++ - '0';
	}
    }

    if (stdin_InitKeyboard()) {
	
	if (
#ifdef CONF_HW_TTY_TWTERM
	    xterm_InitMouse()
#else
	    FALSE
#endif
	    ||
#ifdef CONF_HW_TTY_LINUX
	    GPM_InitMouse()
#else
	    FALSE
#endif
	    || warn_NoMouse()) {
	    
	    if (
#ifdef CONF_HW_TTY_LINUX
		(!force_stdout && vcsa_InitVideo()) ||
#endif
		stdout_InitVideo()) {
	    
		if (tty_fd == 0) {
		    HW->DisplayIsCTTY = TRUE;
		    DisplayHWCTTY = HW;
		} else {
		    /*
		     * avoid fighting for the terminal
		     * with a shell or some other process
		     */
		    HW->NeedHW |= NEEDPersistentSlot;

		    HW->DisplayIsCTTY = FALSE;
		}
		HW->QuitHW = tty_QuitHW;


		/*
		 * we must draw everything on our new shiny window
		 * without forcing all other displays
		 * to redraw everything too.
		 */
		stdin_DetectSize(&HW->usedX, &HW->usedY);
		HW->usedX = GetDisplayWidth();
		HW->usedY = GetDisplayHeight();
		NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);

		return TRUE;
	    }
	    HW->QuitMouse();
	}
	HW->QuitKeyboard();
    }
    if (tty_fd) {
	close(tty_fd);
	fclose(stdOUT);
    }
    return FALSE;
}

#ifdef MODULE

byte InitModule(module *Module) {
    Module->Private = tty_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module *Module) {
}

#endif /* MODULE */
