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
#include <signal.h>
#include <termios.h>
#include <errno.h>

#include "twin.h"
#include "main.h"
#include "data.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"

#include "libTwkeys.h"

#ifdef CONF_HW_TTY_LINUX
#include <gpm.h>
#include <linux/kd.h>
#endif

#if !defined(CONF_HW_TTY_LINUX) && !defined(CONF_HW_TTY_TWTERM)
# warning trying to compile textmode terminals driver without support for any specific ttys.
# warning you will get only a 'generic tty' driver, without mouse support.
#endif

static display_hw TTY;

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

static int tty_number, VcsaFd;
static byte *tty_name;
static dat ttypar[2];

static void stdin_QuitKeyboard(void);
static void stdin_KeyboardEvent(int fd, uldat slot);

static struct termios ttyb;

/* return FALSE if failed */
static byte stdin_InitKeyboard(void) {
    int i;
    byte buf[16], *s = buf+3, c;

    InitTtysave();
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
    ioctl(0, TCSETS, &ttyb);

    write(1, "\033Z", 2); /* request ID */
    /* ensure we CAN read from the tty */
    do {
	i = read(0, buf, 15);
    } while (i < 0 && (errno == EAGAIN || errno == EINTR));
    if (i <= 0) {
	fputs("      stdin_InitKeyboard() failed: unable to read from the terminal!\n", errFILE);
	ioctl(0, TCSETS, &ttysave);
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

    All->keyboard_slot = RegisterRemote(0, stdin_KeyboardEvent);
    if (All->keyboard_slot == NOSLOT) {
	stdin_QuitKeyboard();
	return FALSE;
    }
    TTY.KeyboardEvent = stdin_KeyboardEvent;
    TTY.QuitKeyboard = stdin_QuitKeyboard;

    return TRUE;
}

static void stdin_QuitKeyboard(void) {
    ioctl(0, TCSETS, &ttysave);
    FD_CLR(0, &save_rfds);
    
    UnRegisterRemote(All->keyboard_slot);
    All->keyboard_slot = NOSLOT;
    
    TTY.QuitKeyboard = NoOp;
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

static void stdin_KeyboardEvent(int fd, uldat slot) {
    static void xterm_MouseEvent(int, uldat);
    static byte *match;
    byte got, chunk, buf[SMALLBUFF], *s;
    udat Code;

    got = read(fd, s=buf, SMALLBUFF-1);
    
    if (got == 0 || (got == (byte)-1 && errno != EINTR && errno != EAGAIN)) {
	/* BIG troubles */
	Quit(1);
    }
    
    while (got > 0) {
#ifdef CONF_HW_TTY_TWTERM
	if (TTY.MouseEvent == xterm_MouseEvent) {
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
		xterm_MouseEvent(fd, slot);
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
}

/*
 * Linux gpm mouse input:
 * mouse input uses libgpm to connect to `gpm' mouse daemon
 * and read mouse state, but draws mouse pointer manually
 */

#ifdef CONF_HW_TTY_LINUX

static Gpm_Connect Gpm_CONN;
static int gpm_fd;
static Gpm_Event Gpm_EV;

static void gpm_QuitMouse(void);
static void gpm_MouseEvent(int fd, uldat slot);


/* return FALSE if failed */
static byte gpm_InitMouse(void) {
    
    /*
     * HACK! this works around a quirk in libgpm:
     * if Gpm_Open fails, it sets gpm_tried to non-zero
     * and following calls will just fail, not even trying anymore
     */
    extern int gpm_tried;
    gpm_tried = 0;
    
    gpm_zerobased = 1;
    gpm_visiblepointer = 0;
    Gpm_CONN.eventMask = ~0;		/* Get everything */
    Gpm_CONN.defaultMask = ~GPM_HARD;	/* Pass everything unused */
    Gpm_CONN.minMod = 0;		/* Run always... */
    Gpm_CONN.maxMod = ~0;		/* ...with any modifier */

    if ((gpm_fd = Gpm_Open(&Gpm_CONN, 0)) < 0) {
	fputs("      gpm_InitMouse() failed: unable to connect to `gpm'.\n"
	      "      make sure you started `twin' from the console\n"
	      "      and/or check that `gpm' is running.\n", errFILE);
	return FALSE;
    }
    fcntl(gpm_fd, F_SETFD, FD_CLOEXEC);
    fcntl(gpm_fd, F_SETFL, O_NONBLOCK);

    /* gpm_consolefd is opened by GpmOpen() */
    fcntl(gpm_consolefd, F_SETFD, FD_CLOEXEC);
    
    All->mouse_slot = RegisterRemote(gpm_fd, gpm_MouseEvent);
    if (All->mouse_slot == NOSLOT) {
	gpm_QuitMouse();
	return FALSE;
    }
    
    TTY.SoftMouse = TRUE; /* _we_ Hide/Show it */
    
    TTY.MouseEvent = gpm_MouseEvent;
    TTY.QuitMouse = gpm_QuitMouse;

    return TRUE;
}

static void gpm_QuitMouse(void) {
    FD_CLR(gpm_fd, &save_rfds);
    TTY.HideMouse();
    Gpm_Close();

    UnRegisterRemote(All->mouse_slot);
    All->mouse_slot = NOSLOT;
    
    TTY.QuitMouse = NoOp;
}

static void gpm_MouseEvent(int fd, uldat slot) {
    static Gpm_keys = 0;
    udat IdButtons, Buttons = 0;
    /*
     * All other parts of twin read and parse data from fds in big chunks,
     * while Gpm_GetEvent() reads and parses only a single event at time.
     * To compensate this and avoid mouse to lag behind, we do a small loop.
     */
    byte loopN = 30;
    
    while (loopN--) {
	if (Gpm_GetEvent(&Gpm_EV) <= 0)
	    return;
	
	All->FullShiftFlags =
	    (Gpm_EV.modifiers & 1 ? FULL_LEFT_SHIFT_PRESSED : 0)
	    | (Gpm_EV.modifiers & 2 ? FULL_RIGHT_ALT_PRESSED  : 0)
	    | (Gpm_EV.modifiers & 4 ? FULL_LEFT_CTRL_PRESSED  : 0)
	    | (Gpm_EV.modifiers & 8 ? FULL_LEFT_ALT_PRESSED   : 0);
	
	/*
	 * Gpm differs from us about what buttons to report on release events:
	 * it reports which buttons get _released_, not which are still _pressed_
	 * Fixed here. SIGH.
	 */
	IdButtons = Gpm_EV.buttons;
	
	if (Gpm_EV.type & GPM_UP)
	    IdButtons = Gpm_keys & ~IdButtons;
	Gpm_keys = IdButtons;
	
	if (IdButtons & GPM_B_LEFT)
	    Buttons |= HOLD_LEFT;
	if (IdButtons & GPM_B_MIDDLE)
	    Buttons |= HOLD_MIDDLE;
	if (IdButtons & GPM_B_RIGHT)
	    Buttons |= HOLD_RIGHT;
	
	MouseEventCommon(Gpm_EV.x, Gpm_EV.y, Gpm_EV.dx, Gpm_EV.dy, Buttons);
    }
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
static void xterm_MouseEvent(int fd, uldat slot);


/* return FALSE if failed */
static byte xterm_InitMouse(void) {
    byte *term = origTERM, *seq;
    
    if (!strcmp(term, "linux")) {
	/*
	 * additional check... out-of-the box linux
	 * doesn't have xterm-style mouse reporting
	 */
	if (ttypar[0]<6 || (ttypar[0]==6 && ttypar[1]<3)) {
	    fputs("      xterm_InitMouse() failed: this `linux' terminal\n"
		  "      has no support for xterm-style mouse reporting.\n", errFILE);
	    return FALSE;
	}
	seq = "\033[?9h";
    } else if (!strncmp(term, "xterm", 5) || !strncmp(term, "rxvt", 4)) {
	/* twin doesn't fully support xterm, rxvt etc...
	 * this is just a rough attempt */
	seq = "\033[?1001s\033[?1000h";
    } else {
	fprintf(errFILE, "      xterm_InitMouse() failed: terminal `%s' is not supported.\n", term);
	return FALSE;
    }

    fputs(seq, stdout);
    
    All->mouse_slot = NOSLOT; /* shared with keyboard */
    
    TTY.MouseEvent = xterm_MouseEvent;
    TTY.QuitMouse = xterm_QuitMouse;
    
    TTY.SoftMouse = FALSE; /* no need to Hide/Show it */
    TTY.ShowMouse = TTY.HideMouse = NoOp; /* override the ones set by InitVideo() */
    
    return TRUE;
}

static void xterm_QuitMouse(void) {
    if (ttypar[0]==6 && ttypar[1]>=3)
	fputs("\033[?9l", stdout);
    else /* xterm or similar */
	fputs("\033[?1000l\033[?1001r", stdout);
    setFlush();

    TTY.QuitMouse = NoOp;
}

static void xterm_MouseEvent(int fd, uldat slot) {
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
	    return;
    } else
	return;
    
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
	  "      hit RETURN to continue, otherwise hit CTRL-C to quit now.\n", errFILE);
    fflush(errFILE);
    
    read(0, &c, 1);
    if (c == '\n' || c == '\r') {
    
	All->mouse_slot = NOSLOT; /* no mouse at all :( */
    
	TTY.MouseEvent = (void *)NoOp;
	TTY.QuitMouse = NoOp;
    
	TTY.SoftMouse = FALSE; /* no need to Hide/Show it */
	TTY.ShowMouse = TTY.HideMouse = NoOp; /* override the ones set by InitVideo() */
	return TRUE;
    }
    return FALSE;
}



static void stdin_DetectSize(udat *x, udat *y) {
    struct winsize wsiz;

    if (ioctl(0, TIOCGWINSZ, &wsiz) < 0 || wsiz.ws_row <= 0 || wsiz.ws_col <= 0)
	return;
	
    *x = wsiz.ws_col;
    *y = wsiz.ws_row;
}


static void stdout_MoveToXY(udat x, udat y);
static void stdout_SetCursorType(uldat CursorType);
static void stdout_Beep(void);


/* output through /dev/vcsaXX */


#ifdef CONF_HW_TTY_LINUX


static void vcsa_QuitVideo(void);
static void vcsa_FlushVideo(void);
static void vcsa_ShowMouse(void);
static void vcsa_HideMouse(void);
static byte linux_getconsolemap(void);

/* return FALSE if failed */
static byte vcsa_InitVideo(void) {
    static byte vcsa_name[] = "/dev/vcsaXX";
    byte *tty_s = tty_name;
    
    if (!tty_s)
	return FALSE;
    
    if (!strncmp(tty_s, "/dev/tty", 8)) {
	tty_s += 8;
	if (*tty_s && *tty_s >= '0' && *tty_s <= '9') {
	    tty_number = (vcsa_name[9] = *tty_s++) - '0';
	    if ((vcsa_name[10] = *tty_s)) {
		tty_number *= 10;
		tty_number += *tty_s++ - '0';
	    }
	}
    }
    
    if (*tty_s || tty_number < 1 || tty_number > 63) {
	fprintf(errFILE, "      vcsa_InitVideo() failed: terminal `%s'\n"
			 "      is not a local linux console.\n", tty_name);
	return FALSE;
    }
	        
    GetPrivileges();
    VcsaFd = open(vcsa_name, O_WRONLY|O_NOCTTY);
    DropPrivileges();
    
    if (VcsaFd < 0) {
	fprintf(errFILE, "      vcsa_InitVideo() failed: unable to open `%s': %s\n",
		vcsa_name, strerror(errno));
	return FALSE;
    }
    fcntl(VcsaFd, F_SETFD, FD_CLOEXEC);
    
    (void)linux_getconsolemap();
    
    TTY.DetectSize = stdin_DetectSize;
    TTY.canDragArea = NULL; 
   
    TTY.FlushVideo = vcsa_FlushVideo;
    TTY.QuitVideo = vcsa_QuitVideo;
    TTY.FlushHW = NULL;
    
    TTY.NeedOldVideo = TRUE;
    TTY.merge_Threshold = 40;
    TTY.ExpensiveFlushVideo = FALSE;
    
    TTY.ShowMouse = vcsa_ShowMouse;
    TTY.HideMouse = vcsa_HideMouse;

    TTY.MoveToXY = stdout_MoveToXY;           All->gotoxybuf[0] = '\0';
    TTY.SetCursorType = stdout_SetCursorType; All->cursorbuf[0] = '\0';
    TTY.Beep = stdout_Beep;
    
    return TRUE;
}

static void vcsa_QuitVideo(void) {
    TTY.MoveToXY(0, ScreenHeight-1);
    TTY.SetCursorType(LINECURSOR);
    vcsa_FlushVideo();
    fputs("\033[0m\033[3l\n", stdout); /* clear colors, TTY_DISPCTRL */
    fflush(stdout);
    
    close(VcsaFd);
    
    signal(SIGWINCH, SIG_DFL);
    
    TTY.QuitVideo = NoOp;
}


static void vcsa_FlushVideo(void) {
    dat i, j;
    uldat _s = (uldat)-1, _e = (uldat)-1, start, end;
    byte FlippedVideo = FALSE;
    hwattr *V;
    uldat t;
    
    if (ChangedVideoFlag) {
	/* this hides the mouse if needed ... */
    
	/* first, check the old mouse position */
	if (TTY.SoftMouse) {
	    if (ChangedMouseFlag) {
		/* dirty the old mouse position, so that it will be overwritten */
		DirtyVideo(Last_x, Last_y, Last_x, Last_y);
		if (ValidOldVideo)
		    OldVideo[Last_x + Last_y * ScreenWidth] = ~Video[Last_x + Last_y * ScreenWidth];
	    }
	    
	    i = All->MouseState->x;
	    j = All->MouseState->y;
	    /*
	     * instead of calling ShowMouse(),
	     * we flip the new mouse position in Video[] and dirty it if necessary.
	     * this avoids glitches if the mouse is between two dirty areas
	     * that get merged.
	     */
	    if (ChangedMouseFlag || (FlippedVideo = Threshold_isDirtyVideo(i, j))) {
		VideoFlipMouse();
		if (!FlippedVideo)
		    DirtyVideo(i, j, i, j);
		ChangedMouseFlag = FALSE;
		FlippedVideo = TRUE;
	    } else
		FlippedVideo = FALSE;
	}
	
	for (i=0; i<ScreenHeight*2; i++) {
	    start = (uldat)ChangedVideo[i>>1][i&1][0];
	    end   = (uldat)ChangedVideo[i>>1][i&1][1];
	    ChangedVideo[i>>1][i&1][0] = -1;
	    
	    if (start != (uldat)-1) {
		start += (i>>1) * ScreenWidth;
		end += (i>>1) * ScreenWidth;

		if (All->SetUp->Flags & SETUP_NOBLINK) {
		    for (V = &Video[start], t = start; t <= end; V++, t++)
			*V &= ~HWATTR(COL(0,HIGH), (hwfont)0);
		}
		
		
		if (ValidOldVideo) {
		    while (start <= end && Video[start] == OldVideo[start])
			start++;
		    while (start <= end && Video[end] == OldVideo[end])
			end--;
		    if (start > end)
			continue;
		}
		if (_s != (uldat)-1) {
		    if (start - _e < TTY.merge_Threshold) {
			/* the two chunks are (almost) contiguous, merge them */
			_e = end;
			continue;
		    }
		    lseek(VcsaFd, 4+_s*sizeof(hwattr), SEEK_SET);
		    write(VcsaFd, (void *)&Video[_s], (_e-_s+1)*sizeof(hwattr));
		    CopyMem(&Video[_s], &OldVideo[_s], (_e-_s+1)*sizeof(hwattr));
		}
		_s = start;
		_e = end;
	    }
	}
	if (_s != (uldat)-1) {
	    lseek(VcsaFd, 4+_s*sizeof(hwattr), SEEK_SET);
	    write(VcsaFd, (char *)&Video[_s], (_e-_s+1)*sizeof(hwattr));
	    CopyMem(&Video[_s], &OldVideo[_s], (_e-_s+1)*sizeof(hwattr));
	}
    } else if (TTY.SoftMouse && ChangedMouseFlag)
	TTY.HideMouse();
    
    /* ... and this redraws the mouse */
    if (TTY.SoftMouse) {
	if (FlippedVideo)
	    VideoFlipMouse();
	else if (ChangedMouseFlag)
	    TTY.ShowMouse();
    }
    
    /* now the cursor */
    
    if (All->gotoxybuf[0])
	printf(All->gotoxybuf), All->gotoxybuf[0] = '\0', setFlush();

    if (All->cursorbuf[0])
	printf(All->cursorbuf), All->cursorbuf[0] = '\0', setFlush();

    ChangedVideoFlag = ChangedMouseFlag = FALSE;
    ValidOldVideo = TRUE;
}


/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have vcsa_ and stdout_ versions, not gpm_ ones... */
static void vcsa_ShowMouse(void) {
    uldat pos = (Last_x = All->MouseState->x) + (Last_y = All->MouseState->y) * ScreenWidth;
    hwattr h  = Video[pos];
    hwcol c = ~HWCOL(h) ^ COL(HIGH,HIGH);

    h = HWATTR( c, HWFONT(h) );

    lseek(VcsaFd, 4+pos*sizeof(hwattr), SEEK_SET);
    write(VcsaFd, (char *)&h, sizeof(hwattr));
}

static void vcsa_HideMouse(void) {
    uldat pos = Last_x + Last_y * ScreenWidth;

    lseek(VcsaFd, 4+pos*sizeof(hwattr), SEEK_SET);
    write(VcsaFd, (char *)&Video[pos], sizeof(hwattr));
}


static byte linux_getconsolemap(void) {
    scrnmap_t map[E_TABSZ];
    byte c, ret;
    
    if ((ret = ioctl(0, GIO_SCRNMAP, map)) == 0) {
	if (sizeof(scrnmap_t) == 1)
	    CopyMem(map+0x80, All->Gtranslations[USER_MAP], 0x80);
	else {
	    for (c = 0; c < 0x80; c++)
		All->Gtranslations[USER_MAP][c] = (byte)map[c | 0x80];
	}
    }
    return ret == 0;
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
    byte *term = origTERM;
    
    if (!term) {
	fprintf(errFILE, "      stdout_InitVideo() failed: unknown terminal.\n");
	return FALSE;
    }
    
    do if (strcmp(term, "linux")) {
	if (!strncmp(term, "xterm", 5) || !strncmp(term, "rxvt", 4)) {
	    byte c;
	    
	    fprintf(errFILE, "\n"
		    "      \033[1m  WARNING: terminal `%s' is not fully supported.\033[0m\n"
		    "\n"
		    "      If you really want to run `twin' on this terminal\n"
		    "      hit RETURN to continue, otherwise hit CTRL-C to quit now.\n", term);
	    fflush(errFILE);
    
	    read(0, &c, 1);
	    if (c == '\n' || c == '\r')
		break;
	}
	fprintf(errFILE, "      stdout_InitVideo() failed: terminal type `%s' not supported.\n", term);
	return FALSE;
    } while (0);

    fputs("\033[0;11m\033[3h", stdout); /* clear colors, set IBMPC consolemap, set TTY_DISPCTRL */
    
#ifdef CONF_HW_TTY_LINUX
    /* try to get consolemap...*/
    (void)linux_getconsolemap();
#endif
    
    TTY.DetectSize = stdin_DetectSize;
    TTY.canDragArea = NULL;

    TTY.FlushVideo = stdout_FlushVideo;
    TTY.QuitVideo = stdout_QuitVideo;
    TTY.FlushHW = NULL;

    TTY.NeedOldVideo = TRUE;
    TTY.merge_Threshold = 0;
    TTY.ExpensiveFlushVideo = FALSE;
    
    TTY.ShowMouse = stdout_ShowMouse;
    TTY.HideMouse = stdout_HideMouse;

    TTY.MoveToXY = stdout_MoveToXY;           All->gotoxybuf[0] = '\0';
    TTY.SetCursorType = stdout_SetCursorType; All->cursorbuf[0] = '\0';
    TTY.Beep = stdout_Beep;
    
    return TRUE;
}

static void stdout_QuitVideo(void) {
    TTY.MoveToXY(0, ScreenHeight-1);
    TTY.SetCursorType(LINECURSOR);
    TTY.FlushVideo();

    fputs("\033[0;10m\033[3l\n", stdout); /* restore original colors, consolemap and TTY_DISPCTRL */
    fflush(stdout);
    signal(SIGWINCH, SIG_DFL);
    
    TTY.QuitVideo = NoOp;
}


static hwcol _col;

#define CTRL_ALWAYS 0x0800f501	/* Cannot be overridden by TTY_DISPCTRL */

#define stdout_MogrifyNoCursor() fputs("\033[?25l", stdout);
#define stdout_MogrifyYesCursor() fputs("\033[?25h", stdout);
#define stdout_MogrifyInit() fputs("\033[0m", stdout); _col = COL(WHITE,BLACK)

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
    
    fputs(colbuf, stdout);
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
		sending = TRUE, printf("\033[%d;%dH", 1 + y, 1 + x);

	    col = HWCOL(*V);
	    
	    if (col != _col)
		stdout_SetColor(col);
	
	    *oV = *V;
	    c = HWFONT(*V);
	    if ((c < 32 && ((CTRL_ALWAYS >> c) & 1)) || c == 128+27)
		putchar(' '); /* can't display it */
	    else
		putchar(c);
	} else
	    sending = FALSE;
    }
}

static void stdout_FlushVideo(void) {
    dat i;
    dat start, end;
    byte FlippedVideo = FALSE;
    hwattr *V;
    udat t;
    
    if (ChangedVideoFlag) {
	/* hide the mouse if needed */
	
	if (TTY.SoftMouse) {
	    /* first, check the old mouse position */
	    if (ChangedMouseFlag && !Plain_isDirtyVideo(Last_x, Last_y))
		TTY.HideMouse();
	    
	    /* then, the new position */
	    if (Plain_isDirtyVideo(All->MouseState->x, All->MouseState->y)) {
		/* we'll trample on it. no problem, just tweak Video[]
		 * so that it automagically redraws it */
		VideoFlipMouse();
		ChangedMouseFlag = FALSE;
		FlippedVideo = TRUE;
	    }
	}
	
	stdout_MogrifyNoCursor();
	stdout_MogrifyInit();
	for (i=0; i<ScreenHeight*2; i++) {
	    start = ChangedVideo[i>>1][i&1][0];
	    end   = ChangedVideo[i>>1][i&1][1];
	    ChangedVideo[i>>1][i&1][0] = -1;
	
	    if (All->SetUp->Flags & SETUP_NOBLINK) {
		V = &Video[start + (i>>1) * ScreenWidth];
		for (t = end - start + 1; t; V++, t--)
		    *V &= ~HWATTR(COL(0,HIGH), (hwfont)0);
	    }
	    
	    if (start != -1)
		stdout_Mogrify(start, i>>1, end-start+1);
	}
	/* put the cursor back in place */
	stdout_MogrifyYesCursor();
	All->gotoxybuf[0] = '\033';
	
	/* setFlush() will be called by fputs(All->gotoxybuf...)... */
    } else if (TTY.SoftMouse && ChangedMouseFlag)
	TTY.HideMouse();
    
    /* ... and this redraws the mouse */
    if (TTY.SoftMouse) {
	if (FlippedVideo)
	    VideoFlipMouse();
	else if (ChangedMouseFlag)
	    TTY.ShowMouse();
    }
    
    if (All->gotoxybuf[0])
	fputs(All->gotoxybuf, stdout), All->gotoxybuf[0] = '\0', setFlush();
    
    if (All->cursorbuf[0])
	fputs(All->cursorbuf, stdout), All->cursorbuf[0] = '\0', setFlush();

    ChangedVideoFlag = ChangedMouseFlag = FALSE;
    ValidOldVideo = TRUE;
}


/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have vcsa_ and stdout_ versions, not gpm_ ones... */
static void stdout_ShowMouse(void) {
    uldat pos = (Last_x = All->MouseState->x) + (Last_y = All->MouseState->y) * ScreenWidth;
    hwattr h  = Video[pos];
    hwcol c = ~HWCOL(h) ^ COL(HIGH,HIGH);

    Video[pos] = HWATTR( c, HWFONT(h) );
    stdout_MogrifyInit();
    stdout_Mogrify(All->MouseState->x, All->MouseState->y, 1);
    Video[pos] = h;

    /* put the cursor back in place */
    All->gotoxybuf[0] = '\033';
    setFlush();
}

static void stdout_HideMouse(void) {
    stdout_MogrifyInit();
    stdout_Mogrify(Last_x, Last_y, 1);

    /* put the cursor back in place */
    All->gotoxybuf[0] = '\033';
    setFlush();
}

static void stdout_SetCursorType(uldat CursorType) {
    static uldat saveCursorType = (uldat)-1;
    if (CursorType != saveCursorType) {
	sprintf(All->cursorbuf, "\033[?%d;%d;%dc",
		(int)(CursorType & 0xFF),
		(int)((CursorType >> 8) & 0xFF),
		(int)((CursorType >> 16) & 0xFF));
	saveCursorType = CursorType;
    }
}

static void stdout_MoveToXY(udat x, udat y) {
    static udat saveX = 0, saveY = 0;
    x++, y++;
    if (x != saveX || y != saveY) {
	sprintf(All->gotoxybuf, "\033[%d;%dH", y, x);
	saveX = x;
	saveY = y;
    }
}

static void stdout_Beep(void) {
    fputs("\033[3l\007\033[3h", stdout);
    All->NeedHW |= NEEDFlushStdout;
}

#if 0
/* it works, but it's MUCH slower than doing it the normal way */
void stdout_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    dat DstRgt = DstLeft + (Rgt - Left), DstDwn = DstUp + (Dwn - Up);
    ldat len, count;
    hwattr *src = Video, *dst = Video;
    
    count = Dwn - Up + 1;
    len   = (Rgt-Left+1) * sizeof(hwattr);

    if (DstUp <= 1 && DstUp < Up &&
	Left == DstLeft && Left == 0 && Rgt == ScreenWidth - 1 &&
	Dwn == ScreenHeight - 1 && Up <= ScreenHeight/2 &&
	DstUp <= Dwn && DstDwn >= Up) {
	/* ok, prepare for direct tty scroll */
	
	if (ChangedVideoFlag)
	    FlushVideo();
	HideMouse();
	ChangedMouseFlag = TRUE;
	
	printf("\033[?1c\033[0m"	/* hide cursor, reset color */
	       "\033[1;1H"		/* go to first line */
	       "\033[%dM",Up-DstUp);/* delete lines (i.e. scroll up) */
	if (FlushVideo != stdout_FlushVideo)
	    fflush(stdout);
	/*
	 * now the last trick. with scrolling, we have erased part of Up,Dwn area
	 * and also everything above DstUp.
	 * since this is supposed to be a drag, for the former is going to be junked
	 * and will be redrawn by other code in any case, so no need to worry.
	 * about the latter, we dirty it so that it will be redrawn.
	 */
	
	if (DstUp > 0)
	    DirtyVideo(0, 0, ScreenWidth - 1, DstUp-1);
	
	/* this will restore the cursor */
	All->gotoxybuf[0] = '\033';
	All->cursorbuf[0] = '\033';
    } else
    	/* tty scrolls can't do this :( */
	DirtyVideo(DstLeft, DstUp, DstRgt, DstDwn);
}
#endif /* 0 */

static void tty_QuitHW(void) {
    TTY.QuitMouse();
    TTY.QuitVideo();
    TTY.QuitKeyboard();
    
    if (tty_name) {
	free(tty_name);
	tty_name = NULL;
    }
    
    TTY.QuitHW = NoOp;
}

display_hw *tty_InitHW(byte *arg) {
    byte *s;
    byte force_stdout = FALSE;

    if (arg) {
	if (strncmp(arg, "tty", 3))
	    return NULL; /* user said "use <arg> as display, not tty" */
	
	arg += 3;
	
	if (*arg == '@') {
	    int ttyfd;

	    s = strchr(++arg, ',');
	    if (s) *s = '\0';
	    ttyfd = open(arg, O_RDWR);  /* use specified tty */

	    if (ttyfd < 0) {
		if (s) *s++ = ',';
		return NULL;
	    }

	    tty_name = strdup(arg);
	    if (s) *s = ',';
	    arg = s;

	    close(0);
	    close(1);
	    dup2(ttyfd, 0);
	    dup2(ttyfd, 1);
	    close(ttyfd);
	}
	
	while (arg && *arg) {
	    /* parse options */
	    if (!strncmp(arg, ",TERM=", 6)) {
		s = strchr(arg += 6, ',');
		if (s) *s = '\0';
		if (origTERM) free(origTERM);
		origTERM = strdup(arg);
		if (s) *s = ',';
		arg = s;
	    } else if (!strncmp(arg, ",stdout", 7)) {
		s = strchr(arg += 7, ',');
		arg = s;
		force_stdout = TRUE;
	    } else
		break;
	}
		    
#if 0
	{
	    pid_t pgrp;
	    byte fail;
	    
	    GetPrivileges();
	    fail = ioctl(ttyfd, TIOCGPGRP, &pgrp) < 0 || setpgid(0, pgrp) < 0;
	    DropPrivileges();
	    
	    if (fail) {
		close(ttyfd);
		return NULL;
	    }
	}
#endif	    
    }
    
    if (!tty_name && (tty_name = ttyname(0)))
	tty_name = strdup(tty_name);

    if (stdin_InitKeyboard()) {
	
	if (
#ifdef CONF_HW_TTY_TWTERM
	    xterm_InitMouse()
#else
	    FALSE
#endif
	    ||
#ifdef CONF_HW_TTY_LINUX
	    gpm_InitMouse()
#else
	    FALSE
#endif
	    || warn_NoMouse()) {
	    
	    if (
#ifdef CONF_HW_TTY_LINUX
		(!force_stdout && vcsa_InitVideo()) ||
#endif
		stdout_InitVideo()) {
	    
		TTY.QuitHW = tty_QuitHW;
		return &TTY;
	    }
	    TTY.QuitMouse();
	}
	TTY.QuitKeyboard();
    }
    return NULL;
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
