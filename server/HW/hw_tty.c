/*
 *  hw_tty.c  --  functions to let twin display on linux console
 *		  and inside a twin terminal
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "tty_ioctl.h"

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"

#include "libTwkeys.h"


#if !defined(CONF_HW_TTY_LINUX) && !defined(CONF_HW_TTY_TWTERM) && !defined(CONF_HW_TTY_TERMCAP)
# warning trying to compile textmode terminals driver without support for any specific ttys.
# warning twin terminal driver will be enabled by default.
# define CONF_HW_TTY_TWTERM
#endif


#ifdef CONF_HW_TTY_LINUX
/*void *_fxstat = __fxstat;*/
# include <gpm.h>
#endif

#ifdef CONF_HW_TTY_TERMCAP
# include <termcap.h>
#endif

struct tty_data {
    int tty_fd, VcsaFd, tty_number;
    byte *tty_name, *tty_TERM;
    dat ttypar[2];
    FILE *stdOUT;
    uldat saveCursorType;
    dat saveX, saveY;

#ifdef CONF_HW_TTY_LINUX
    Gpm_Connect GPM_Conn;
    int GPM_fd;
    int GPM_keys;
#endif

    byte *scr_clear;
#ifdef CONF_HW_TTY_TERMCAP
    byte *cursor_goto, *cursor_on, *cursor_off,
	*bold_on, *blink_on, *attr_off,
	*kpad_on, *kpad_off, *audio_bell,
	colorbug, wrapglitch;
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
#define cursor_goto	(ttydata->cursor_goto)
#define cursor_on	(ttydata->cursor_on)
#define cursor_off	(ttydata->cursor_off)
#define bold_on		(ttydata->bold_on)
#define blink_on	(ttydata->blink_on)
#define attr_off	(ttydata->attr_off)
#define kpad_on		(ttydata->kpad_on)
#define kpad_off	(ttydata->kpad_off)
#define scr_clear	(ttydata->scr_clear)
#define audio_bell	(ttydata->audio_bell)
#define colorbug	(ttydata->colorbug)
#define wrapglitch	(ttydata->wrapglitch)


/* plain Unix-style tty keyboard input */

/* it can't update FullShiftFlags :( */


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
    tty_setioctl(tty_fd, &ttyb);

    write(tty_fd, "\033Z", 2); /* request ID */
    /* ensure we CAN read from the tty */
    do {
	i = read(tty_fd, buf, 15);
    } while (i < 0 && (errno == EAGAIN || errno == EINTR));
    if (i <= 0) {
	fputs("      stdin_InitKeyboard() failed: unable to read from the terminal!\n", stderr);
	tty_setioctl(tty_fd, &ttysave);
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
    tty_setioctl(tty_fd, &ttysave);
    
    UnRegisterRemote(HW->keyboard_slot);
    HW->keyboard_slot = NOSLOT;
    
    HW->QuitKeyboard = NoOp;
}

static udat stdin_LookupKey(udat *ShiftFlags, byte *slen, byte *s) {
    byte used = 0, len = *slen;

    *ShiftFlags = 0;

    if (len == 0)
	return TW_Null;

    if (len > 1 && *s == '\x1B') {
	
	++used, --len;
	
#define IS(sym, c) case c: *slen = ++used; return TW_##sym
	
	switch (*++s) {
	    
	  case '[':
	    if (++used, --len) switch (*++s) {
		/* ESC [ */
		IS(Up,    'A');
		IS(Down,  'B');
		IS(Right, 'C');
		IS(Left,  'D');
		IS(KP_5,  'G');  /* returned when NumLock is off */
		IS(Pause, 'P');
		
		/* FALLTHROUGH */
		
	      case '[':
		/* ESC [ [ */
		if (++used, --len) switch (*++s) {
		    IS(F1, 'A');
		    IS(F2, 'B');
		    IS(F3, 'C');
		    IS(F4, 'D');
		    IS(F5, 'E');
		}
		break;
		
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
		break;
		
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
		break;
		
	      case '3':
		/* ESC [ 3 */
		if (++used, --len) switch (*++s) {
		    IS(Delete, '~');
		}
		break;
		
	      case '4':
		/* ESC [ 4 */
		if (++used, --len) switch (*++s) {
		    IS(End, '~');
		}
		break;
		
	      case '5':
		/* ESC [ 5 */
		if (++used, --len) switch (*++s) {
		    IS(Prior, '~');
		}
		break;

	      case '6':
		/* ESC [ 6 */
		if (++used, --len) switch (*++s) {
		    IS(Next, '~');
		}
		break;
		
	    }
	    break;
	    
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
	    break;

	  default:
	    break;
	}

#undef IS
	
	/* try to handle ALT + <some key> */
	*slen = ++used;
	*ShiftFlags = KBD_ALT_FL;
	return (udat)*s;
    }

    *slen = ++used;
    
    switch (*s) {
      case TW_Tab:
      case TW_Linefeed:
      case TW_Return:
      case TW_Escape:
	return (udat)*s;
      default:
	if (*s < ' ') {
	    /* try to handle CTRL + <some key> */
	    *ShiftFlags = KBD_CTRL_FL;
	    return (udat)*s | 0x40;
	}
	return (udat)*s;
    }
    
    /* NOTREACHED */
    return TW_Null;
}
	
static byte xterm_MouseData[10] = "\033[M#!!!!";

static void stdin_KeyboardEvent(int fd, display_hw *hw) {
    static void xterm_MouseEvent(int, display_hw *);
    static byte *match;
    byte got, chunk, buf[SMALLBUFF], *s;
    udat Code, ShiftFlags;
    SaveHW;
    
    SetHW(hw);
    
    got = read(fd, s=buf, SMALLBUFF-1);
    
    if (got == 0 || (got == (byte)-1 && errno != EINTR && errno != EAGAIN)) {
	/* BIG troubles */
	HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	return;
    }
    
    while (got > 0) {
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

	/* ok, now try to find out the correct KeyCode for the ASCII sequence we got */
	
	if (!(chunk = got))
	    break;
	
	Code = stdin_LookupKey(&ShiftFlags, &chunk, s);

	KeyboardEventCommon(Code, ShiftFlags, chunk, s);
	s += chunk, got -= chunk;
    }
    
    RestoreHW;
}





static byte warn_NoMouse(void) {
    byte c;
    
    fputs("\n"
	  "      \033[1m  ALL  MOUSE  DRIVERS  FAILED.\033[0m\n"
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



static void stdin_DetectSize(dat *x, dat *y) {
    struct winsize wsiz;

    if (ioctl(tty_fd, TIOCGWINSZ, &wsiz) >= 0 && wsiz.ws_row > 0 && wsiz.ws_col > 0) {
	HW->X = wsiz.ws_col;
	HW->Y = wsiz.ws_row;
    }
    
    *x = HW->X;
    *y = HW->Y;
}

static void stdin_CheckResize(dat *x, dat *y) {
    *x = Min2(*x, HW->X);
    *y = Min2(*y, HW->Y);
}

static void stdin_Resize(dat x, dat y) {
    if (x < HW->usedX || y < HW->usedY) {
	/*
	 * can't resize the tty, just clear it so that
	 * extra size will get padded with blanks
	 */
	fprintf(stdOUT, "\033[0m%s", scr_clear);
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
	    

static void stdout_FlushHW(void);

/* this can stay static, as it's used only as temporary storage */
static hwcol _col;


static void stdout_FlushHW(void) {
    if (fflush(stdOUT) != 0)
	HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
    clrFlush();
}

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
 * xterm-style mouse input
 * 
 * used to let twin run inside twin terminal, xterm, etc.
 * 
 * it is always compiled in.
 */

static void xterm_QuitMouse(void);
static void xterm_MouseEvent(int fd, display_hw *hw);


/* return FALSE if failed */
static byte xterm_InitMouse(byte force) {
    byte *term = tty_TERM, *seq;
    
    if (force) {
	fputs("      xterm_InitMouse(): xterm-style mouse FORCED.\n"
	      "      Assuming terminal has xterm compatible mouse reporting.\n", stderr);
	term = "xterm";
    }

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
	x = Min2(x, DisplayWidth - 1);
    
	y = Max2(y, 0);
	y = Min2(y, DisplayHeight - 1);
    
	if (x == 0 && prev_x == 0)
	    dx = -1;
	else if (x == DisplayWidth - 1 && prev_x == DisplayWidth - 1)
	    dx = 1;
	else
	    dx = 0;
	if (y == 0 && prev_y == 0)
	    dy = -1;
	else if (y == DisplayHeight - 1 && prev_y == DisplayHeight - 1)
	    dy = 1;
	else
	    dy = 0;
	
	prev_x = x;
	prev_y = y;
	
	MouseEventCommon(x, y, dx, dy, Buttons);
    }
}



#if defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
# include "hw_tty_linux.c"
#endif

#if defined(CONF_HW_TTY_TERMCAP)
# include "hw_tty_termcap.c"
#endif

/*
 * note: during xxx_InitHW() initialization, DON'T use DisplayWidth/DisplayHeight
 * as they may be not up to date. Use GetDisplayWidth() / GetDisplayHeight().
 */
#ifdef CONF_THIS_MODULE
static
#endif
byte tty_InitHW(void) {
    byte *arg = HW->Name;
    byte *s;
    byte skip_vcsa = FALSE;
    byte skip_stdout = FALSE;
    byte force_xterm = FALSE;
    byte tc_colorbug = FALSE;
    
    if (!(HW->Private = (struct tty_data *)AllocMem(sizeof(struct tty_data)))) {
	fprintf(stderr, "      tty_InitHW(): Out of memory!\n");
	return FALSE;
    }
    saveCursorType = (uldat)-1;
    saveX = saveY = 0;
    stdOUT = NULL;
    tty_TERM = tty_name = NULL;
    
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
		fprintf(stderr, "      tty_InitHW(): fopen(\"%s\") failed: %s\n", arg, strerror(errno));
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
		arg = strchr(arg + 7, ',');
		skip_vcsa = TRUE;
	    } else if (!strncmp(arg, ",termcap", 8)) {
		arg = strchr(arg + 8, ',');
		skip_vcsa = skip_stdout = TRUE;
	    } else if (!strncmp(arg, ",colorbug", 9)) {
		arg = strchr(arg + 9, ',');
		tc_colorbug = TRUE;
	    } else if (!strncmp(arg, ",xterm", 6)) {
		arg = strchr(arg + 6, ',');
		force_xterm = TRUE;
	    } else if (!strncmp(arg, ",noinput", 8)) {
		arg = strchr(arg + 8, ',');
		HW->FlagsHW |= FlHWNoInput;
	    } else if (!strncmp(arg, ",slow", 5)) {
		arg = strchr(arg + 5, ',');
		HW->FlagsHW |= FlHWExpensiveFlushVideo;
	    } else
		break;
	}
    }

#ifdef HW_TTY_TERMCAP
    colorbug = tc_colorbug;
#endif
    
    if (!stdOUT) {
	if (DisplayHWCTTY) {
	    fprintf(stderr, "      tty_InitHW() failed: controlling tty %s\n",
		    DisplayHWCTTY == HWCTTY_DETACHED
		    ? "not usable after Detach"
		    : "is already in use as display");
	    return FALSE;
	} else {
	    tty_fd = 0;
	    stdOUT = stdout;
	    tty_name = CloneStr(ttyname(0));
	    if (!tty_TERM)
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
#ifdef CONF_HW_TTY_LINUX
	    GPM_InitMouse() ||
#endif
	    xterm_InitMouse(force_xterm) ||
	    warn_NoMouse()) {
	    
	    if (
		FALSE
#ifdef CONF_HW_TTY_LINUX
		|| (!skip_vcsa && vcsa_InitVideo())
#endif
#if defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
		|| (!skip_stdout && linux_InitVideo())
#endif
#ifdef CONF_HW_TTY_TERMCAP
		|| termcap_InitVideo()
#endif
		) {
	    
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

		HW->MouseState.x = HW->MouseState.y = HW->MouseState.keys =
		    HW->Last_x = HW->Last_y = 0;

		/*
		 * we must draw everything on our new shiny window
		 * without forcing all other displays
		 * to redraw everything too.
		 */
		stdin_DetectSize(&HW->usedX, &HW->usedY);
		HW->usedX = GetDisplayWidth();
		HW->usedY = GetDisplayHeight();
		
		HW->RedrawVideo = FALSE;
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

#ifdef CONF_THIS_MODULE

#include "version.h"
MODULEVERSION;
		       
byte InitModule(module *Module) {
    Module->Private = tty_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module *Module) {
}

#endif /* CONF_THIS_MODULE */
