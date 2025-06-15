/*
 *  hw_tty.c  --  functions to let twin display on linux console
 *                  and inside a twin terminal
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twconfig.h"

#ifdef TW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef TW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include "tty_ioctl.h"

#include "twin.h"
#include "algo.h"
#include "alloc.h"
#include "main.h"
#include "data.h"
#include "remote.h"
#include "stl/string.h"
#include "util.h" /* for SetAlarm() and AlarmReceived */

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"
#include "log.h"

#include <Tw/Twkeys.h>
#include <Tutf/Tutf.h>

#include <new> // placement new

#if !defined(CONF_HW_TTY_LINUX) && !defined(CONF_HW_TTY_TWTERM) && !defined(CONF_HW_TTY_TERMCAP)
#warning trying to compile hw_tty.c without support for any specific terminal.
#warning twterm driver will be enabled by default.
#define CONF_HW_TTY_TWTERM
#endif

#ifdef CONF_HW_TTY_LINUX
/*void *_fxstat = __fxstat;*/
#include <gpm.h>
#endif

#ifdef CONF_HW_TTY_TERMCAP
#include "hw_tty_common/driver_termcap_decls.h"
#endif

enum tty_palettemode {
  tty_palette_autodetect = 0,
  tty_palette8 = 1,
  tty_palette256 = 2,
  tty_palette16M = 3,
};

struct tty_data {
  String tty_NAME, tty_TERM;
  int tty_fd, VcsaFd, tty_number;
  uldat tty_charset;
  Tutf_function tty_UTF_32_to_charset;
  Tutf_array tty_charset_to_UTF_32;
  byte tty_use_utf8, tty_is_xterm;
  dat ttypar[3];
  FILE *stdOUT;
  uldat saveCursorType;
  dat saveX, saveY;

  udat (*LookupKey)(udat *ShiftFlags, byte *slen, char *s, byte *retlen, const char **ret);

  const char *mouse_start_seq, *mouse_end_seq, *mouse_motion_seq;
#ifdef CONF_HW_TTY_LINUX
  Gpm_Connect GPM_Conn;
  int GPM_fd;
  int GPM_keys;
#endif
  char xterm_mouse_seq[31];
  byte xterm_mouse_len;
  dat xterm_prev_x, xterm_prev_y;

#ifdef CONF_HW_TTY_TERMCAP
  char *tc_cap[tc_cap_N];
  byte colorbug, wrapglitch, paletteN;
#else
  char *tc_scr_clear;
#endif
};

#define ttydata ((struct tty_data *)HW->Private)
#define tty_fd (ttydata->tty_fd)
#define VcsaFd (ttydata->VcsaFd)
#define tty_number (ttydata->tty_number)
#define tty_NAME (ttydata->tty_NAME)
#define tty_TERM (ttydata->tty_TERM)
#define tty_charset (ttydata->tty_charset)
#define tty_UTF_32_to_charset (ttydata->tty_UTF_32_to_charset)
#define tty_charset_to_UTF_32 (ttydata->tty_charset_to_UTF_32)
#define tty_use_utf8 (ttydata->tty_use_utf8)
#define tty_is_xterm (ttydata->tty_is_xterm)
#define ttypar (ttydata->ttypar)
#define stdOUT (ttydata->stdOUT)
#define saveCursorType (ttydata->saveCursorType)
#define saveX (ttydata->saveX)
#define saveY (ttydata->saveY)
#define LookupKey (ttydata->LookupKey)
#define mouse_start_seq (ttydata->mouse_start_seq)
#define mouse_end_seq (ttydata->mouse_end_seq)
#define mouse_motion_seq (ttydata->mouse_motion_seq)
#define GPM_Conn (ttydata->GPM_Conn)
#define GPM_fd (ttydata->GPM_fd)
#define GPM_keys (ttydata->GPM_keys)

#define xterm_mouse_seq (ttydata->xterm_mouse_seq)
#define xterm_mouse_len (ttydata->xterm_mouse_len)
#define xterm_prev_x (ttydata->xterm_prev_x)
#define xterm_prev_y (ttydata->xterm_prev_y)

#ifdef CONF_HW_TTY_TERMCAP
#define tc_cap (ttydata->tc_cap)
#define tc_scr_clear (tc_cap[tc_seq_scr_clear])
#define tc_cursor_goto (tc_cap[tc_seq_cursor_goto])
#define tc_cursor_on (tc_cap[tc_seq_cursor_on])
#define tc_cursor_off (tc_cap[tc_seq_cursor_off])
#define tc_bold_on (tc_cap[tc_seq_bold_on])
#define tc_blink_on (tc_cap[tc_seq_blink_on])
#define tc_attr_off (tc_cap[tc_seq_attr_off])
#define tc_kpad_on (tc_cap[tc_seq_kpad_on])
#define tc_kpad_off (tc_cap[tc_seq_kpad_off])
#define tc_audio_bell (tc_cap[tc_seq_audio_bell])
#define tc_charset_start (tc_cap[tc_seq_charset_start])
#define tc_charset_end (tc_cap[tc_seq_charset_end])
#define tc_audio_bell (tc_cap[tc_seq_audio_bell])
#define tc_charset_start (tc_cap[tc_seq_charset_start])
#define tc_charset_end (tc_cap[tc_seq_charset_end])
#define colorbug (ttydata->colorbug)
#define wrapglitch (ttydata->wrapglitch)
#define paletteN (ttydata->paletteN)
#else
#define tc_scr_clear (ttydata->tc_scr_clear)
#endif

static void null_InitMouse(void);
static byte null_InitMouseConfirm(void);
static void stdin_DetectSize(dat *x, dat *y);
static void stdin_CheckResize(dat *x, dat *y);
static void stdin_Resize(dat x, dat y);
static void stdout_FlushHW(void);

static void tty_DrawRune(trune h);

/* this can stay static, as it's used only as temporary storage */
static tcolor _col;

static void tty_QuitHW(void);

#include "hw_tty_common/kbd_stdin.h"

#include "hw_tty_common/mouse_xterm.h"

#if defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
#include "hw_tty_linux/driver_linux.h"
#endif

#if defined(CONF_HW_TTY_TERMCAP)
#include "hw_tty_common/driver_termcap.h"
#endif

static void null_InitMouse(void) {
  HW->mouse_slot = NOSLOT; /* no mouse at all :( */
  HW->ConfigureMouse = (void (*)(udat, byte, udat))NoOp;
  HW->MouseEvent = (void (*)(int, Tdisplay))NoOp;
  HW->QuitMouse = NoOp;

  HW->FlagsHW &= ~FlHWSoftMouse;        /* no need to Hide/Show it */
  HW->ShowMouse = HW->HideMouse = NoOp; /* override the ones set by *_InitVideo() */
}

static byte null_InitMouseConfirm(void) {
  byte c = '\0';

  fflush(stdOUT);
  log(ERROR) << "\n"
                "      \033[1m  ALL  MOUSE  DRIVERS  FAILED.\033[0m\n"
                "\n"
                "      If you really want to run `twin' without mouse\n"
                "      hit RETURN within 10 seconds to continue,\n"
                "      otherwise hit CTRL-C (or wait 10 seconds) to cancel.\n";
  flushk();

  SetAlarm(10);
  read(tty_fd, &c, 1);
  SetAlarm(0);

  if (c == '\n' || c == '\r') {

    null_InitMouse();

    return ttrue;
  }
  return tfalse;
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
    fprintf(stdOUT, "\033[m%s", tc_scr_clear);
    fflush(stdOUT);
    /*
     * flush now not to risk arriving late
     * and clearing the screen AFTER *_FlushVideo()
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

static void tty_DrawRune(trune h) {
  uldat len;
  char buf[4];

  if (h <= 0x7FF) {
    buf[0] = (h >> 6) | 0xC0;
    buf[1] = (h & 0x3F) | 0x80;
    len = 2;
  } else if (h <= 0xFFFF) {
    buf[0] = (h >> 12) | 0xE0;
    buf[1] = ((h >> 6) & 0x3F) | 0x80;
    buf[2] = (h & 0x3F) | 0x80;
    len = 3;
  } else {
    buf[0] = (h >> 18) | 0xF0;
    buf[1] = ((h >> 12) & 0x3F) | 0x80;
    buf[2] = ((h >> 6) & 0x3F) | 0x80;
    buf[3] = (h & 0x3F) | 0x80;
    len = 4;
  }
  fwrite(buf, 1, len, stdOUT);
}

/*
 * note: during xxx_InitHW() initialization, DON'T use DisplayWidth/DisplayHeight
 * as they may be not up to date. Use GetDisplayWidth() / GetDisplayHeight().
 */
static bool tty_InitHW(void) {
  String charset;
  Chars arg = HW->Name;
  enum /*: byte*/ { NEVER = 0, MAYBE = 1, ALWAYS = 2 };
  byte autotry_video = MAYBE, try_stdout = MAYBE, try_termcap = MAYBE, autotry_kbd = MAYBE,
       try_lrawkbd = MAYBE, force_mouse = tfalse, tc_colorbug = tfalse,
       need_persistent_slot = tfalse, try_ctty = tfalse, display_is_ctty = tfalse,
       tty_TERM_override = tfalse;

  if (!(HW->Private = (struct tty_data *)AllocMem0(sizeof(struct tty_data)))) {
    log(ERROR) << "      tty_InitHW(): Out of memory!\n";
    return false;
  }
  saveCursorType = (uldat)-1;
  tty_charset = (uldat)-1;
  tty_use_utf8 = ttrue + ttrue; /* i.e. unknown */
  saveX = saveY = 0;
  stdOUT = NULL;
  tty_fd = -1;
  new (&tty_TERM) String();
  new (&tty_NAME) String();

  HW->QuitHW = tty_QuitHW;

  if (arg.size() > 4) {
    arg = arg.view(4, arg.size()); // skip -hw=

    if (!arg.starts_with(Chars("tty"))) {
      return false; /* user said "use <arg> as display, not tty" */
    }
    arg = arg.view(3, arg.size());

    if (arg && arg[0] == '@') {
      size_t comma = arg.find(Chars(","));
      const size_t end = arg.size();
      if (comma == size_t(-1)) {
        comma = end;
      }
      if (!tty_NAME.format(arg.view(1, comma))) {
        log(ERROR) << "      tty_InitHW(): out of memory!\n";
        return false;
      }
      arg = arg.view(comma, end);
    }

    while (arg) {
      /* skip comma, find next one */
      const size_t end = arg.size();
      size_t comma = arg.view(1, end).find(Chars(","));
      if (comma == size_t(-1)) {
        comma = end;
      } else {
        comma++; // skip initial comma
      }
      /* parse options */
      Chars arg0 = arg.view(0, comma);
      if (arg0.starts_with(Chars(",TERM="))) {
        if (!tty_TERM.format(arg0.view(6, comma))) {
          log(ERROR) << "      tty_InitHW(): out of memory!\n";
          return false;
        }
        tty_TERM_override = true;
      } else if (arg0.starts_with(Chars(",charset="))) {
        if (!charset.format(arg0.view(9, comma))) {
          log(ERROR) << "      tty_InitHW(): out of memory!\n";
          return false;
        }
      } else if (arg0.starts_with(Chars(",stdout"))) {
        try_stdout = !(autotry_video = arg0.view(7, comma) == Chars("=no")) << 1;
      } else if (arg0.starts_with(Chars(",termcap"))) {
        try_termcap = !(autotry_video = arg0.view(8, comma) == Chars("=no")) << 1;
      } else if (arg0.starts_with(Chars(",raw"))) {
        try_lrawkbd = !(autotry_kbd = arg0.view(4, comma) == Chars("=no")) << 1;
      } else if (arg0 == Chars(",ctty")) {
        try_ctty = ttrue;
      } else if (arg0 == Chars(",colorbug")) {
        tc_colorbug = ttrue;
      } else if (arg0.starts_with(Chars(",palette="))) {
        arg0 = arg0.view(9, comma);
        if (arg0 == Chars("8")) {
          paletteN = tty_palette8;
        } else if (arg0 == Chars("256")) {
          paletteN = tty_palette256;
        } else if (arg0 == Chars("16M")) {
          paletteN = tty_palette16M;
        } else {
          log(ERROR) << "      tty_InitHW(): unsupported 'palette=" << arg0
                     << "', expecting one of: 8, 256 or 16M\n";
          return false;
        }
      } else if (arg0.starts_with(Chars(",mouse="))) {
        arg0 = arg0.view(7, comma);
        if (arg0 == Chars("xterm")) {
          force_mouse = ttrue;
        } else if (arg0 == Chars("twterm")) {
          force_mouse = ttrue + ttrue;
        } else {
          log(ERROR) << "      tty_InitHW(): unsupported 'mouse=" << arg0
                     << "', expecting one of: xterm twterm\n";
          return false;
        }
      } else if (arg0 == Chars(",noinput")) {
        HW->FlagsHW |= FlHWNoInput;
      } else if (arg0 == Chars(",slow")) {
        HW->FlagsHW |= FlHWExpensiveFlushVideo;
      } else if (arg0.starts_with(Chars(",utf8"))) {
        tty_use_utf8 = arg0.view(5, comma) != Chars("=no");
      } else {
        log(INFO)
            << "   --hw=tty options:\n"
               "      @/dev/SOME_TTY_NAME   attach to specified tty device (must be first option)\n"
               "      ,charset=CHARSET_NAME use specified charset encoding\n"
               "      ,ctty[=no]            set tty device as the controlling tty\n"
               "      ,colorbug             assume terminal has colorbug\n"
               "      ,help                 show this help\n"
               "      ,mouse=[xterm|twterm] assume specified mouse reporting protocol\n"
               "      ,noinput              open a view-only display on tty - ignore input\n"
               "      ,palette=[8|256|16M]  assume terminal supports this many colors\n"
               "      ,raw[=no]             use Linux raw keyboard\n"
               "      ,slow                 assume terminal is slow\n"
               "      ,stdout[=no]          use hard-coded escape sequences\n"
               "      ,TERM=TERM_NAME       assume terminal type is TERM_NAME\n"
               "      ,termcap[=no]         use libtermcap escape sequences\n"
               "      ,truecolor            same as palette=16M\n"
               "      ,utf8[=no]            assume terminal supports UTF-8";
        return false;
      }
      arg = arg.view(comma, end);
    }
  }

  if (tty_NAME) {
    /*
     * open user-specified tty as display
     */

    /*
     * avoid fighting for the terminal with a shell
     * or some other process when we display on something
     * that was not our controlling tty
     * (even if we grab it as our new controlling tty)
     */
    need_persistent_slot = ttrue;

    if ((tty_fd = open(tty_NAME.data(), O_RDWR)) >= 0) {
      /*
       * try to set this tty as our controlling tty if user asked us.
       * this will greatly help detecting tty resizes,
       * but may hangup other processes running on that tty.
       */
      if ((display_is_ctty = try_ctty && (!DisplayHWCTTY || DisplayHWCTTY == HWCTTY_DETACHED) &&
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
      log(ERROR) << "      tty_InitHW(): open(\"" << tty_NAME
                 << "\") failed: " << Chars::from_c(strerror(errno)) << "\n";
      return false;
    }
  } else {
    /*
     * open our controlling tty as display
     */
    if (DisplayHWCTTY) {
      log(ERROR) << "      tty_InitHW() failed: controlling tty "
                 << (DisplayHWCTTY == HWCTTY_DETACHED ? Chars("not usable after Detach\n")
                                                      : Chars("is already in use as display\n"));
      return false;
    } else {
      display_is_ctty = ttrue;
      tty_fd = 0;
      stdOUT = stdout;
      if (!tty_NAME.format(Chars::from_c(ttyname(0)))) {
        log(ERROR) << "      tty_InitHW(): out of memory!\n";
        return false;
      }
      if (!tty_TERM) {
        if (!tty_TERM.format(Chars::from_c(origTERM))) {
          log(ERROR) << "      tty_InitHW(): out of memory!\n";
          return false;
        }
      }
    }
  }
  if (paletteN == tty_palette_autodetect) {
    Chars env_colorterm;
    if (!tty_TERM_override &&
        ((env_colorterm = Chars::from_c(getenv("COLORTERM"))) == Chars("truecolor") ||
         env_colorterm == Chars("24bit"))) {
      paletteN = tty_palette16M;
    } else if (tty_TERM.ends_with(Chars("256")) || tty_TERM.ends_with(Chars("256color"))) {
      paletteN = tty_palette256;
    } else {
      paletteN = tty_palette8;
    }
  }
  fflush(stdOUT);
  setvbuf(stdOUT, NULL, _IOFBF, BUFSIZ);

  tty_number = 0;
  if (tty_NAME.starts_with(Chars("/dev/tty")) || tty_NAME.starts_with(Chars("/dev/vc/"))) {
    const char *s = tty_NAME.data() + 8;
    const char *end = tty_NAME.end();
    while (s < end && *s >= '0' && *s <= '9') {
      tty_number *= 10;
      tty_number += *s++ - '0';
    }
  }

#ifdef CONF_HW_TTY_TERMCAP
  colorbug = tc_colorbug;
#endif

  if (charset) {
    /* honor user-specified charset */
    if ((tty_charset = Tutf_charset_id(charset.data())) == (uldat)-1) {
      log(ERROR) << "      tty_InitHW(): libtutf warning: unknown charset `" << charset
                 << "', assuming `ASCII'\n";
    } else if (tty_charset == Tutf_charset_id(T_NAME(UTF_32))) {
      log(ERROR) << "      tty_InitHW(): charset `" << charset
                 << "' is Unicode, assuming terminal supports UTF-8\n";
      tty_use_utf8 = ttrue;
      tty_charset = (uldat)-1;
    }
  }

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
   * (it asks user if he/she really wants to run without mouse),
   * while lrawkbd_InitKeyboard() puts tty_fd in non-blocking mode.
   *
   * Thus mouse initialization must come *BEFORE* keyboard initialization
   */

  if (!stdin_TestTty()) {
    log(ERROR) << "      tty_InitHW() failed: unable to read from the terminal: " << Errstr << "\n";
  } else if (

#if defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
      (TRY_V(stdout) && linux_InitVideo()) ||
#endif
#ifdef CONF_HW_TTY_TERMCAP
      (TRY_V(termcap) && termcap_InitVideo()) ||
#endif
      false) {

    if (
#ifdef CONF_HW_TTY_LINUX
        GPM_InitMouse() ||
#else
        (log(WARNING) << "      tty_InitHW(): gpm mouse support not compiled, skipping it.\n",
         false) ||
#endif
        xterm_InitMouse(force_mouse) || null_InitMouseConfirm()) {

      if (
#if defined(CONF_HW_TTY_LINUX) && defined(CONF_HW_TTY_LRAWKBD)
          (TRY_K(lrawkbd) && lrawkbd_InitKeyboard()) ||
#endif
          (autotry_kbd && stdin_InitKeyboard())) {

        if (tty_charset == (uldat)-1) {
          tty_UTF_32_to_charset = Tutf_UTF_32_to_ASCII;
          tty_charset_to_UTF_32 = Tutf_ASCII_to_UTF_32;
        } else {
          tty_UTF_32_to_charset = Tutf_UTF_32_to_charset_function(tty_charset);
          tty_charset_to_UTF_32 = Tutf_charset_to_UTF_32_array(tty_charset);
        }

        /*
         * must be deferred until now, as HW-specific functions
         * can clobber HW->NeedHW
         */
        if (need_persistent_slot)
          HW->NeedHW |= NEEDPersistentSlot;
        if (display_is_ctty) {
          HW->DisplayIsCTTY = ttrue;
          DisplayHWCTTY = HW;
        }
        HW->MouseState.x = HW->MouseState.y = HW->MouseState.keys = HW->Last_x = HW->Last_y = 0;

        /*
         * we must draw everything on our new shiny window
         * without forcing all other displays
         * to redraw everything too.
         */
        stdin_DetectSize(&HW->usedX, &HW->usedY);
        HW->usedX = GetDisplayWidth();
        HW->usedY = GetDisplayHeight();

        HW->RedrawVideo = tfalse;
        NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);

        if (tc_scr_clear)
          fputs(tc_scr_clear, stdOUT);
        fflush(stdOUT);

        return true;
      }
      HW->QuitMouse();
    }
    HW->QuitVideo();
  }
  if (tty_fd >= 0)
    tty_setioctl(tty_fd, &ttysave);
  if (tty_fd > 0)
    close(tty_fd);
  if (stdOUT && stdOUT != stdout)
    fclose(stdOUT);
  return false;
}

static void tty_QuitHW(void) {
  HW->QuitMouse();
  HW->QuitKeyboard();
  HW->QuitVideo();
  HW->QuitHW = NoOp;

  // destroy tty_NAME and tty_TERM
  String().swap(tty_NAME);
  String().swap(tty_TERM);

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

EXTERN_C byte InitModule(Tmodule Module) {
  Module->DoInit = tty_InitHW;
  return ttrue;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
EXTERN_C void QuitModule(Tmodule Module) {
}
