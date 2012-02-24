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

#include "autoconf.h"

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
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

#include <Tw/Twkeys.h>

#if !defined(CONF_HW_TTY_LINUX) && !defined(CONF_HW_TTY_TWTERM) && !defined(CONF_HW_TTY_TERMCAP)
# warning trying to compile hw_tty.c without support for any specific terminal.
# warning twin terminal driver will be enabled by default.
# define CONF_HW_TTY_TWTERM
#endif

#ifdef CONF_HW_TTY_LINUX
/*void *_fxstat = __fxstat;*/
# include <gpm.h>
#endif

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
#endif

#ifdef CONF_HW_TTY_TERMCAP
# include "hw_tty_common/driver_termcap1.h"
#endif

struct tty_data {
    int tty_fd, VcsaFd, tty_number;
    byte *tty_name, *tty_TERM;
#ifdef CONF__UNICODE
    uldat tty_charset;
    Tutf_function tty_UTF_16_to_charset;
    Tutf_array tty_charset_to_UTF_16;
    byte tty_can_utf8;
#endif
    dat ttypar[3];
    FILE *stdOUT;
    uldat saveCursorType;
    dat saveX, saveY;
    
    udat (*LookupKey)(udat *ShiftFlags, byte *slen, byte *s, byte *retlen, byte **ret);
    
    byte *mouse_start_seq, *mouse_end_seq, *mouse_motion_seq;
#ifdef CONF_HW_TTY_LINUX
    Gpm_Connect GPM_Conn;
    int GPM_fd;
    int GPM_keys;
#endif

#ifdef CONF_HW_TTY_TERMCAP
    byte *tc_cap[tc_cap_N],
	colorbug, wrapglitch;
#else
    byte *tc_scr_clear;
#endif
};

#define ttydata		((struct tty_data *)HW->Private)
#define tty_fd		(ttydata->tty_fd)
#define VcsaFd		(ttydata->VcsaFd)
#define tty_number	(ttydata->tty_number)
#define tty_name	(ttydata->tty_name)
#define tty_TERM	(ttydata->tty_TERM)
#define tty_charset	(ttydata->tty_charset)
#define tty_UTF_16_to_charset	(ttydata->tty_UTF_16_to_charset)
#define tty_charset_to_UTF_16	(ttydata->tty_charset_to_UTF_16)
#define tty_can_utf8		(ttydata->tty_can_utf8)
#define ttypar		(ttydata->ttypar)
#define stdOUT		(ttydata->stdOUT)
#define saveCursorType	(ttydata->saveCursorType)
#define saveX		(ttydata->saveX)
#define saveY		(ttydata->saveY)
#define LookupKey	(ttydata->LookupKey)
#define mouse_start_seq	(ttydata->mouse_start_seq)
#define mouse_end_seq	(ttydata->mouse_end_seq)
#define mouse_motion_seq (ttydata->mouse_motion_seq)
#define GPM_Conn	(ttydata->GPM_Conn)
#define GPM_fd		(ttydata->GPM_fd)
#define GPM_keys	(ttydata->GPM_keys)

#ifdef CONF_HW_TTY_TERMCAP
# define tc_cap		(ttydata->tc_cap)
# define tc_scr_clear	(tc_cap[tc_seq_scr_clear])
# define tc_cursor_goto	(tc_cap[tc_seq_cursor_goto])
# define tc_cursor_on	(tc_cap[tc_seq_cursor_on])
# define tc_cursor_off	(tc_cap[tc_seq_cursor_off])
# define tc_bold_on	(tc_cap[tc_seq_bold_on])
# define tc_blink_on	(tc_cap[tc_seq_blink_on])
# define tc_attr_off	(tc_cap[tc_seq_attr_off])
# define tc_kpad_on	(tc_cap[tc_seq_kpad_on])
# define tc_kpad_off	(tc_cap[tc_seq_kpad_off])
# define tc_audio_bell	(tc_cap[tc_seq_audio_bell])
# define tc_charset_start	(tc_cap[tc_seq_charset_start])
# define tc_charset_end	(tc_cap[tc_seq_charset_end])
# define tc_audio_bell	(tc_cap[tc_seq_audio_bell])
# define tc_charset_start	(tc_cap[tc_seq_charset_start])
# define tc_charset_end	(tc_cap[tc_seq_charset_end])	
# define colorbug	(ttydata->colorbug)
# define wrapglitch	(ttydata->wrapglitch)
#else
# define tc_scr_clear	(ttydata->tc_scr_clear)
#endif

static void null_InitMouse(void);
static byte null_InitMouseConfirm(void);
static void stdin_DetectSize(dat *x, dat *y);
static void stdin_CheckResize(dat *x, dat *y);
static void stdin_Resize(dat x, dat y);
static void stdout_FlushHW(void);

#ifdef CONF__UNICODE
static void tty_MogrifyUTF8(hwfont h);
static byte utf8used;
#endif

/* this can stay static, as it's used only as temporary storage */
static hwcol _col;

static void tty_QuitHW(void);




#include "hw_tty_common/kbd_stdin.h"

#include "hw_tty_common/mouse_xterm.h"

#if defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
# include "hw_tty_linux/driver_linux.h"
#endif

#if defined(CONF_HW_TTY_TERMCAP)
# include "hw_tty_common/driver_termcap.h"
#endif



static void null_InitMouse(void) {
    HW->mouse_slot = NOSLOT; /* no mouse at all :( */
    
    HW->MouseEvent = (void *)NoOp;
    HW->QuitMouse = NoOp;
    
    HW->FlagsHW &= ~FlHWSoftMouse; /* no need to Hide/Show it */
    HW->ShowMouse = HW->HideMouse = NoOp; /* override the ones set by *_InitVideo() */
}


static byte null_InitMouseConfirm(void) {
    byte c;
    CONST byte *Msg =
	"\n"
	"      \033[1m  ALL  MOUSE  DRIVERS  FAILED.\033[0m\n"
	"\n"
	"      If you really want to run `twin' without mouse\n"
	"      hit RETURN to continue, otherwise hit CTRL-C to quit now.\n";
    
    fprintf(stdOUT, "%s", Msg);
    fflush(stdOUT);

    printk("%s", Msg);
    flushk();
    
    read(tty_fd, &c, 1);
    if (c == '\n' || c == '\r') {
    
	null_InitMouse();
	
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
	fprintf(stdOUT, "\033[0m%s", tc_scr_clear);
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






static void stdout_FlushHW(void) {
    if (fflush(stdOUT) != 0)
	HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
    clrFlush();
}



#ifdef CONF__UNICODE
static void tty_MogrifyUTF8(hwfont h) {
    byte c;
    if (!utf8used)
	utf8used = TRUE, fputs("\033%G", stdOUT);
    if (h >= 0x800) {
	c = (h >> 12) | 0xE0, putc(c, stdOUT);
	c = ((h >> 6) & 0x3F) | 0x80, putc(c, stdOUT);
    } else
	c = (h >> 6) | 0xC0, putc(c, stdOUT);
    c = (h & 0x3F) | 0x80, putc(c, stdOUT);
}
#endif



/*
 * note: during xxx_InitHW() initialization, DON'T use DisplayWidth/DisplayHeight
 * as they may be not up to date. Use GetDisplayWidth() / GetDisplayHeight().
 */
#ifdef THIS_MODULE
static
#endif
byte tty_InitHW(void) {
    byte *arg = HW->Name;
    byte *s;
    byte *charset = NULL;
#define NEVER  0
#define MAYBE  1
#define ALWAYS 2
    /*
     * writing through /dev/vcsa* is slow (too many sycalls),
     * enable only if explicitly requested
     */
    byte autotry_video = MAYBE, try_vcsa = NEVER,
	try_stdout = MAYBE, try_termcap = MAYBE,
	autotry_kbd = MAYBE, try_lrawkbd = MAYBE,
	
	force_mouse = FALSE, tc_colorbug = FALSE,
	need_persistent_slot = FALSE,
	try_ctty = FALSE, display_is_ctty = FALSE;

    if (!(HW->Private = (struct tty_data *)AllocMem(sizeof(struct tty_data)))) {
	printk("      tty_InitHW(): Out of memory!\n");
	return FALSE;
    }
    saveCursorType = (uldat)-1;
#ifdef CONF__UNICODE
    tty_charset = (uldat)-1;
    tty_can_utf8 = TRUE+TRUE;
#endif
    saveX = saveY = 0;
    stdOUT = NULL;
    tty_fd = -1;
    tty_TERM = tty_name = NULL;
    
    if (arg && HW->NameLen > 4) {
	arg += 4;
	
	if (strncmp(arg, "tty", 3))
	    return FALSE; /* user said "use <arg> as display, not tty" */
	arg += 3;
	
	if (*arg == '@') {
	    s = strchr(++arg, ',');
	    if (s) *s = '\0';
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
	    } else if (!strncmp(arg, ",charset=", 9)) {
		s = strchr(arg += 9, ',');
		if (s) *s = '\0';
		charset = CloneStr(arg);
		if (s) *s = ',';
		arg = s;
	    } else if (!strncmp(arg, ",vcsa", 5)) {
		try_vcsa = !(autotry_video = !strncmp(arg+5, "=no", 3)) << 1;
		arg = strchr(arg+5, ',');
	    } else if (!strncmp(arg, ",stdout", 7)) {
		try_stdout = !(autotry_video = !strncmp(arg+7, "=no", 3)) << 1;
		arg = strchr(arg+7, ',');
	    } else if (!strncmp(arg, ",termcap", 8)) {
		try_termcap = !(autotry_video = !strncmp(arg+8, "=no", 3)) << 1;
		arg = strchr(arg+8, ',');
	    } else if (!strncmp(arg, ",raw", 4)) {
		try_lrawkbd = !(autotry_kbd = !strncmp(arg+4, "=no", 3)) << 1;
		arg = strchr(arg+4, ',');
	    } else if (!strncmp(arg, ",ctty", 5)) {
		arg = strchr(arg+5, ',');
		try_ctty = TRUE;
	    } else if (!strncmp(arg, ",colorbug", 9)) {
		arg = strchr(arg+9, ',');
		tc_colorbug = TRUE;
	    } else if (!strncmp(arg, ",mouse=", 7)) {
		if (!strncmp(arg+7, "xterm", 5))
		    force_mouse = TRUE;
		else if (!strncmp(arg+7, "twterm", 5))
		    force_mouse = TRUE+TRUE;
		arg = strchr(arg+7, ',');
	    } else if (!strncmp(arg, ",noinput", 8)) {
		arg = strchr(arg+8, ',');
		HW->FlagsHW |= FlHWNoInput;
	    } else if (!strncmp(arg, ",slow", 5)) {
		arg = strchr(arg + 5, ',');
		HW->FlagsHW |= FlHWExpensiveFlushVideo;
#ifdef CONF__UNICODE
	    } else if (!strncmp(arg, ",utf8", 5)) {
		arg = strchr(arg + 5, ',');
		tty_can_utf8 = TRUE;
#endif
	    } else {
		if (*arg == ',')
		    arg++;
		arg = strchr(arg, ',');
	    }
	}
    }

    if (tty_name) {
	/*
	 * open user-specified tty as display
	 */
	
	/*
	 * avoid fighting for the terminal with a shell
	 * or some other process when we display on something
	 * that was not our controlling tty
	 * (even if we grab it as our new controlling tty)
	 */
	need_persistent_slot = TRUE;
    
	if ((tty_fd = open(tty_name, O_RDWR)) >= 0) {
	    /*
	     * we try to set this tty as our controlling tty
	     * if user asks us to do so. this will greatly help
	     * detecting tty resizes, but may hangup other processes
	     * running on that tty.
	     */
	    if ((display_is_ctty = try_ctty &&
		 (!DisplayHWCTTY || DisplayHWCTTY == HWCTTY_DETACHED) &&
#ifdef TIOCSCTTY
		 ioctl(tty_fd, TIOCSCTTY, 1) >= 0
#else
		 0
#endif
		 )) {

		if (tty_fd != 0) {
		    close(0);
		    dup2(tty_fd, 0);
		    close(tty_fd);
		    tty_fd = 0;
		}
	    }

	    fcntl(tty_fd, F_SETFD, FD_CLOEXEC);
	    stdOUT = fdopen(tty_fd, "r+");
	}
	if (tty_fd == -1 || !stdOUT) {
	    printk("      tty_InitHW(): open(\"%."STR(SMALLBUFF)"s\") failed: %."STR(SMALLBUFF)"s\n", tty_name, strerror(errno));
	    FreeMem(tty_name);
	    if (tty_TERM)
		FreeMem(tty_TERM);
	    return FALSE;
	}
    } else {
	/*
	 * open our controlling tty as display
	 */
	if (DisplayHWCTTY) {
	    printk("      tty_InitHW() failed: controlling tty %."STR(SMALLBUFF)"s\n",
		    DisplayHWCTTY == HWCTTY_DETACHED
		    ? "not usable after Detach"
		    : "is already in use as display");
	    return FALSE;
	} else {
	    display_is_ctty = TRUE;
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
    if (tty_name && (!strncmp(tty_name, "/dev/tty", 8) ||
		     !strncmp(tty_name, "/dev/vc/", 8))) {
	s = tty_name + 8;
	while (*s && *s >= '0' && *s <= '9') {
	    tty_number *= 10;
	    tty_number += *s++ - '0';
	}
    }


#ifdef HW_TTY_TERMCAP
    colorbug = tc_colorbug;
#endif

#ifdef CONF__UNICODE
    if (charset) {
	if ((tty_charset = Tutf_charset_id(charset)) == (uldat)-1)
	    printk("      tty_InitHW(): libTutf warning: unknown charset `%." STR(SMALLBUFF) "s', assuming `CP437'\n", charset);
	else if (tty_charset == Tutf_charset_id(T_NAME(UTF_16))) {
	    printk("      tty_InitHW(): warning: charset `%." STR(SMALLBUFF) "s' is Unicode,\n"
		   "      handling as %." STR(SMALLBUFF) "s (latin1) with utf8\n", charset, T_NAME(ISO8859_1));
	    tty_charset = Tutf_charset_id(T_NAME(ISO8859_1));
	    tty_can_utf8 = TRUE;
	}
	FreeMem(charset);
    }
    if (tty_charset == (uldat)-1) {
	tty_UTF_16_to_charset = Tutf_UTF_16_to_CP437;
	tty_charset_to_UTF_16 = Tutf_CP437_to_UTF_16;
    } else {
	tty_UTF_16_to_charset = Tutf_UTF_16_to_charset_function(tty_charset);
	tty_charset_to_UTF_16 = Tutf_charset_to_UTF_16_array(tty_charset);
    }
#else
    if (charset) {
	FreeMem(charset);
	printk("      tty_InitHW(): warning: `charset=' option requires Unicode support\n");
    }
#endif

#define TRY_V(name) (autotry_video + try_##name >= ALWAYS)
#define TRY_K(name) (autotry_kbd + try_##name >= ALWAYS)

    /*
     * ORDERING IS CRITICAL HERE!
     * 
     * xterm_InitMouse() does not need to manually hide/show the mouse pointer,
     * so it overrides HW->ShowMouse and HW->HideMouse installed by *_InitVideo()
     * 
     * Thus mouse initialization must come *AFTER* video initialization
     * 
     * null_InitMouseConfirm() tries a blocking read() from tty_fd
     * (it asks user if he/she really wants to run twin without mouse),
     * while lrawkbd_InitKeyboard() puts tty_fd in non-blocking mode.
     * 
     * Thus mouse initialization must come *BEFORE* keyboard initialization
     */
    
    if (
#ifdef CONF_HW_TTY_LINUX
	(TRY_V(vcsa) && vcsa_InitVideo()) ||
#endif
#if defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
	(TRY_V(stdout) && linux_InitVideo()) ||
#endif
#ifdef CONF_HW_TTY_TERMCAP
	(TRY_V(termcap) && termcap_InitVideo()) ||
#endif
	FALSE) {
		
        if (
#ifdef CONF_HW_TTY_LINUX
            GPM_InitMouse() ||
#else
            (printk("      tty_InitHW(): gpm mouse support not compiled in, skipping.\n"), FALSE) ||
#endif
            xterm_InitMouse(force_mouse) ||
            null_InitMouseConfirm()) {

            if (
#if defined(CONF_HW_TTY_LINUX) && defined(CONF_HW_TTY_LRAWKBD)
                (TRY_K(lrawkbd) && lrawkbd_InitKeyboard()) ||
#endif
                (autotry_kbd && stdin_InitKeyboard())) {
                
		/*
		 * must be deferred until now, as HW-specific functions
		 * can clobber HW->NeedHW
		 */
		if (need_persistent_slot)
		    HW->NeedHW |= NEEDPersistentSlot;
		if (display_is_ctty) {
		    HW->DisplayIsCTTY = TRUE;
		    DisplayHWCTTY = HW;
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
	HW->QuitVideo();
    }
    if (tty_fd) {
	close(tty_fd);
	fclose(stdOUT);
    }
    return FALSE;
}



static void tty_QuitHW(void) {
    HW->QuitMouse();
    HW->QuitKeyboard();
    HW->QuitVideo();
    HW->QuitHW = NoOp;
    
    if (tty_name)
	FreeMem(tty_name);
    if (tty_TERM)
	FreeMem(tty_TERM);
    if (HW->DisplayIsCTTY && DisplayHWCTTY == HW)
	DisplayHWCTTY = NULL;
    
    fflush(stdOUT);
    if (stdOUT != stdout) {
	
	/* if we forced tty_fd to be fd 0, release it while keeping fd 0 busy */
	if (tty_fd == 0) {
	    if ((tty_fd = open("/dev/null", O_RDWR)) != 0) {
		fclose(stdOUT);
	    
		dup2(tty_fd, 0);
		close(tty_fd);
	    }
	    /*
	     * else we don't fclose(stdOUT) to avoid having fd 0 unused...
	     * it causes leaks, but much better than screwing up badly when
	     * fd 0 will get used by something else (say a socket) and then
	     * abruptly closed by tty_InitHW()
	     */
	} else
	    fclose(stdOUT);
    }
    FreeMem(HW->Private);
}




#ifdef THIS_MODULE

byte InitModule(module Module) {
    Module->Private = tty_InitHW;
    return TRUE;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
void QuitModule(module Module) {
}

#endif /* THIS_MODULE */
