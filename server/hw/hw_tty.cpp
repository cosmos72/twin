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

enum tty_colormode {
  tty_color_autodetect = 0,
  tty_color8 = 1,
  tty_color256 = 2,
  tty_color16M = 3,
};

class tty_driver {
public:
  String tty_name, tty_term;
  FILE *out;
  Tutf_function tty_UTF_32_to_charset;
  Tutf_array tty_charset_to_UTF_32;
  Tdisplay hw;
  int tty_fd, tty_number;
  uldat tty_charset;
  dat ttypar[3];
  dat saveX, saveY;
  bool tty_use_utf8, tty_is_xterm;

  udat (*fnLookupKey)(Tdisplay hw, udat *ShiftFlags, byte *slen, char *s, byte *retlen,
                      const char **ret);

  const char *mouse_start_seq, *mouse_end_seq, *mouse_motion_seq;
#ifdef CONF_HW_TTY_LINUX
  Gpm_Connect gpm_conn;
  int gpm_fd;
  int gpm_keys;
#endif
  char xterm_mouse_seq[31];
  byte xterm_mouse_len;
  dat xterm_prev_x, xterm_prev_y;

  char *tc_scr_clear;
#ifdef CONF_HW_TTY_TERMCAP
  char *tc[tc_seq_N];
  byte colorbug, wrapglitch, colormode;
#endif

  static void DrawRune(Tdisplay hw, trune h);
  static void FlushHW(Tdisplay hw);
  static bool InitHW(Tdisplay hw);
  static void QuitHW(Tdisplay hw);

  static void gpm_ConfigureMouse(Tdisplay hw, udat resource, byte todefault, udat value);
  static bool gpm_InitMouse(Tdisplay hw);
  static void gpm_MouseEvent(int fd, Tdisplay hw);
  static int gpm_Open(Tdisplay hw);
  static void gpm_QuitMouse(Tdisplay hw);

  static void linux_Beep(Tdisplay hw);
  static void linux_Configure(Tdisplay hw, udat resource, byte todefault, udat value);
  static void linux_ConfigureKeyboard(Tdisplay hw, udat resource, byte todefault, udat value);
  static bool linux_CanDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft,
                                dat DstUp);
  static void linux_DragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft,
                             dat DstUp);
  static void linux_DrawSome(Tdisplay hw, dat x, dat y, uldat len);
  static void linux_DrawStart(Tdisplay hw);
  static void linux_DrawTCell(Tdisplay hw, dat x, dat y, tcell V);
  static void linux_FlushVideo(Tdisplay hw);
  static void linux_HideMouse(Tdisplay hw);
  static udat linux_LookupKey(Tdisplay hw, udat *ShiftFlags, byte *slen, char *s, byte *retlen,
                              const char **ret);
  static void linux_ResetPalette(Tdisplay hw);
  static void linux_SetColor(Tdisplay hw, tcolor col);
  static void linux_SetPalette(Tdisplay hw, udat N, udat R, udat G, udat B);
  static void linux_ShowMouse(Tdisplay hw);
  static void linux_QuitVideo(Tdisplay hw);
  static void linux_UpdateCursor(Tdisplay hw);
  static void linux_UpdateMouseAndCursor(Tdisplay hw);

  static bool lrawkbd_InitKeyboard(Tdisplay hw);
  static void lrawkbd_QuitKeyboard(Tdisplay hw);
  static void lrawkbd_ConfigureKeyboard(Tdisplay hw, udat resource, byte todefault, udat value);
  static udat lrawkbd_LookupKey(udat *ShiftFlags, byte *slen, char *s, byte *retlen, char **ret);
  static void lrawkbd_KeyboardEvent(int fd, Tdisplay hw);
  static bool lrawkbd_GetKeyboard(Tdisplay hw);
  static bool lrawkbd_SetKeyboard(Tdisplay hw);
  static void lrawkbd_RestoreKeyboard(Tdisplay hw);
  static bool lrawkbd_LoadKeymaps();
  static void lrawkbd_FreeKeymaps();
  static bool lrawkbd_GrabConsole(Tdisplay hw);
  static void lrawkbd_ReleaseConsole(Tdisplay hw);
  static void lrawkbd_InitSignals();
  static void lrawkbd_QuitSignals();
  static void lrawkbd_ReactSignalIn(int sig);
  static void lrawkbd_ReactSignalOut(int sig);

  static void null_InitMouse(Tdisplay hw);
  static bool null_InitMouseConfirm(Tdisplay hw);

  static void stdin_CheckResize(Tdisplay hw, dat *x, dat *y);
  static void stdin_DetectSize(Tdisplay hw, dat *x, dat *y);
  static void stdin_KeyboardEvent(int fd, Tdisplay hw);
  static bool stdin_InitKeyboard(Tdisplay hw);
  static void stdin_Resize(Tdisplay hw, dat x, dat y);
  static void stdin_QuitKeyboard(Tdisplay hw);
  static bool stdin_TestTty(Tdisplay hw);

  static void termcap_Beep(Tdisplay hw);
  static void termcap_Cleanup(Tdisplay hw);
  static void termcap_Configure(Tdisplay hw, udat resource, byte todefault, udat value);
  static void termcap_ConfigureKeyboard(Tdisplay hw, udat resource, byte todefault, udat value);
  static bool termcap_CanDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft,
                                  dat DstUp);
  static void termcap_DragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft,
                               dat DstUp);
  static void termcap_DrawSome(Tdisplay hw, dat x, dat y, uldat len);
  static void termcap_DrawTCell(Tdisplay hw, dat x, dat y, tcell V);
  static void termcap_FixColorBug(Tdisplay hw);
  static void termcap_HideMouse(Tdisplay hw);
  static bool termcap_InitVideo(Tdisplay hw);
  static udat termcap_LookupKey(Tdisplay hw, udat *ShiftFlags, byte *slen, char *s, byte *retlen,
                                const char **ret);
  static void termcap_FlushVideo(Tdisplay hw);
  static void termcap_MoveToXY(Tdisplay hw, udat x, udat y);
  static void termcap_QuitVideo(Tdisplay hw);
  static void termcap_SetColor(Tdisplay hw, tcolor col);
  static void termcap_SetColor8(Tdisplay hw, tcolor col);
  static void termcap_SetColor256(Tdisplay hw, tcolor col);
  static void termcap_SetColor16M(Tdisplay hw, tcolor col);
  static void termcap_SetCursorType(Tdisplay hw, uldat type);
  static void termcap_ShowMouse(Tdisplay hw);
  static void termcap_UpdateCursor(Tdisplay hw);
  static void termcap_UpdateMouseAndCursor(Tdisplay hw);

  static void xterm_ConfigureMouse(Tdisplay hw, udat resource, byte todefault, udat value);
  static bool xterm_InitMouse(Tdisplay hw, byte force);
  static void xterm_MouseEvent(int fd, Tdisplay hw);
  static void xterm_QuitMouse(Tdisplay hw);
};

#define ttydriver(hw) ((tty_driver *)(hw)->Private)

/* this can stay static, as it's used only as temporary storage */
static tcolor _col;

#include "hw_tty_common/kbd_stdin.h"

#include "hw_tty_common/mouse_xterm.h"

#if defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
#include "hw_tty_linux/driver_linux.h"
#endif

#if defined(CONF_HW_TTY_TERMCAP)
#include "hw_tty_common/driver_termcap.h"
#endif

TW_ATTR_HIDDEN void tty_driver::null_InitMouse(Tdisplay hw) {
  hw->mouse_slot = NOSLOT; /* no mouse at all :( */
  hw->fnConfigureMouse = (void (*)(Tdisplay, udat, byte, udat))NoOp;
  hw->fnMouseEvent = NULL;
  hw->fnQuitMouse = NULL;

  hw->FlagsHW &= ~FlHWSoftMouse; /* no need to Hide/Show it */

  /* override the ones set by *_InitVideo() */
  hw->fnShowMouse = hw->fnHideMouse = NULL;
}

TW_ATTR_HIDDEN bool tty_driver::null_InitMouseConfirm(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  byte c = '\0';

  fflush(self->out);
  log(ERROR) << "\n"
                "      \033[1m  ALL  MOUSE  DRIVERS  FAILED.\033[0m\n"
                "\n"
                "      If you really want to run `twin' without mouse\n"
                "      hit RETURN within 10 seconds to continue,\n"
                "      otherwise hit CTRL-C (or wait 10 seconds) to cancel.\n";
  flushk();

  SetAlarm(10);
  read(self->tty_fd, &c, 1);
  SetAlarm(0);

  if (c == '\n' || c == '\r') {
    null_InitMouse(hw);
    return true;
  }
  return false;
}

TW_ATTR_HIDDEN void tty_driver::stdin_DetectSize(Tdisplay hw, dat *x, dat *y) {
  struct winsize wsiz;
  tty_driver *self = ttydriver(hw);

  if (ioctl(self->tty_fd, TIOCGWINSZ, &wsiz) >= 0 && wsiz.ws_row > 0 && wsiz.ws_col > 0) {
    hw->X = wsiz.ws_col;
    hw->Y = wsiz.ws_row;
  }
  *x = hw->X;
  *y = hw->Y;
}

TW_ATTR_HIDDEN void tty_driver::stdin_CheckResize(Tdisplay hw, dat *x, dat *y) {
  *x = Min2(*x, hw->X);
  *y = Min2(*y, hw->Y);
}

TW_ATTR_HIDDEN void tty_driver::stdin_Resize(Tdisplay hw, dat x, dat y) {
  if (x < hw->usedX || y < hw->usedY) {
    tty_driver *self = ttydriver(hw);
    /*
     * can't resize the tty, just clear it so that
     * extra size will get padded with blanks
     */
    fprintf(self->out, "\033[m%s", self->tc_scr_clear);
    fflush(self->out);
    /*
     * flush now not to risk arriving late
     * and clearing the screen AFTER *_FlushVideo()
     */
    NeedRedrawVideo(hw, 0, 0, x - 1, y - 1);
  }
  hw->usedX = x;
  hw->usedY = y;
}

TW_ATTR_HIDDEN void tty_driver::FlushHW(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  if (fflush(self->out) != 0)
    hw->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
  hw->clrFlush();
}

TW_ATTR_HIDDEN void tty_driver::DrawRune(Tdisplay hw, trune h) {
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
  tty_driver *self = ttydriver(hw);
  fwrite(buf, 1, len, self->out);
}

/*
 * note: during xxx_InitHW() initialization, DON'T use DisplayWidth/DisplayHeight
 * as they may be not up to date. Use GetDisplayWidth() / GetDisplayHeight().
 */
TW_ATTR_HIDDEN bool tty_driver::InitHW(Tdisplay hw) {
  String charset;
  Chars arg = hw->Name;
  tty_driver *self;
  enum /*: byte*/ { NEVER = 0, MAYBE = 1, ALWAYS = 2 };
  byte autotry_video = MAYBE, try_stdout = MAYBE, try_termcap = MAYBE, autotry_kbd = MAYBE,
       try_lrawkbd = MAYBE;
  bool force_mouse = false, have_colorbug = false, need_persistent_slot = false, try_ctty = false,
       is_ctty = false, term_override = false;

  if (!(hw->Private = self = (tty_driver *)AllocMem0(sizeof(tty_driver)))) {
    log(ERROR) << "      tty.InitHW(): Out of memory!\n";
    return false;
  }
  self->hw = hw;
  self->tty_charset = (uldat)-1;
  self->out = NULL;
  /*
   * nowadays almost all terminals support UTF-8
   * => enable it by default, can be disabled with option "utf8=no"
   */
  self->tty_use_utf8 = true;
  self->saveX = self->saveY = 0;
  self->tty_fd = -1;
  self->colormode = tty_color_autodetect;
  new (&self->tty_term) String();
  new (&self->tty_name) String();

  hw->fnQuitHW = &tty_driver::QuitHW;

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
      if (!self->tty_name.format(arg.view(1, comma))) {
        log(ERROR) << "      tty.InitHW(): out of memory!\n";
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
        if (!self->tty_term.format(arg0.view(6, comma))) {
          log(ERROR) << "      tty.InitHW(): out of memory!\n";
          return false;
        }
        term_override = true;
      } else if (arg0.starts_with(Chars(",charset="))) {
        if (!charset.format(arg0.view(9, comma))) {
          log(ERROR) << "      tty.InitHW(): out of memory!\n";
          return false;
        }
      } else if (arg0.starts_with(Chars(",stdout"))) {
        try_stdout = !(autotry_video = arg0.view(7, comma) == Chars("=no")) << 1;
      } else if (arg0.starts_with(Chars(",termcap"))) {
        try_termcap = !(autotry_video = arg0.view(8, comma) == Chars("=no")) << 1;
      } else if (arg0.starts_with(Chars(",raw"))) {
        try_lrawkbd = !(autotry_kbd = arg0.view(4, comma) == Chars("=no")) << 1;
      } else if (arg0 == Chars(",ctty")) {
        try_ctty = true;
      } else if (arg0 == Chars(",colorbug")) {
        have_colorbug = true;
      } else if (arg0.starts_with(Chars(",colors="))) {
        arg0 = arg0.view(8, comma);
        if (arg0 == Chars("8")) {
          self->colormode = tty_color8;
        } else if (arg0 == Chars("256")) {
          self->colormode = tty_color256;
        } else if (arg0 == Chars("16M")) {
          self->colormode = tty_color16M;
        } else {
          log(ERROR) << "      tty.InitHW(): unsupported 'colors=" << arg0
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
          log(ERROR) << "      tty.InitHW(): unsupported 'mouse=" << arg0
                     << "', expecting one of: xterm twterm\n";
          return false;
        }
      } else if (arg0 == Chars(",noinput")) {
        hw->FlagsHW |= FlHWNoInput;
      } else if (arg0 == Chars(",slow")) {
        hw->FlagsHW |= FlHWExpensiveFlushVideo;
      } else if (arg0.starts_with(Chars(",utf8"))) {
        self->tty_use_utf8 = arg0.view(5, comma) != Chars("=no");
      } else {
        log(INFO)
            << "   --hw=tty options:\n"
               "      @/dev/SOME_TTY_NAME   attach to specified tty device (must be first option)\n"
               "      ,charset=CHARSET_NAME use specified charset encoding\n"
               "      ,ctty[=no]            set tty device as the controlling tty\n"
               "      ,colorbug             assume terminal has colorbug\n"
               "      ,colors=[8|256|16M]   assume terminal supports this many colors\n"
               "      ,help                 show this help\n"
               "      ,mouse=[xterm|twterm] assume specified mouse reporting protocol\n"
               "      ,noinput              open a view-only display on tty - ignore input\n"
               "      ,raw[=no]             use Linux raw keyboard\n"
               "      ,slow                 assume terminal is slow\n"
               "      ,stdout[=no]          use hard-coded escape sequences\n"
               "      ,TERM=TERM_NAME       assume terminal type is TERM_NAME\n"
               "      ,termcap[=no]         use libtermcap escape sequences\n"
               "      ,truecolor            same as colors=16M\n"
               "      ,utf8[=no]            assume terminal supports UTF-8\n";
        return false;
      }
      arg = arg.view(comma, end);
    }
  }

  if (self->tty_name) {
    /*
     * open user-specified tty as display
     */

    /*
     * avoid fighting for the terminal with a shell
     * or some other process when we display on something
     * that was not our controlling tty
     * (even if we grab it as our new controlling tty)
     */
    need_persistent_slot = true;

    if ((self->tty_fd = open(self->tty_name.data(), O_RDWR)) >= 0) {
      /*
       * try to set this tty as our controlling tty if user asked us.
       * this will greatly help detecting tty resizes,
       * but may hangup other processes running on that tty.
       */
      if ((is_ctty = try_ctty && (!DisplayHWCTTY || DisplayHWCTTY == HWCTTY_DETACHED) &&
#ifdef TIOCSCTTY
                     ioctl(self->tty_fd, TIOCSCTTY, 1) >= 0
#else
                     0
#endif
           )) {

        if (self->tty_fd != 0) {
          close(0);
          dup2(self->tty_fd, 0);
          close(self->tty_fd);
          self->tty_fd = 0;
        }
      }

      fcntl(self->tty_fd, F_SETFD, FD_CLOEXEC);
      self->out = fdopen(self->tty_fd, "r+");
    }
    if (self->tty_fd == -1 || !self->out) {
      log(ERROR) << "      tty.InitHW(): open(\"" << self->tty_name
                 << "\") failed: " << Chars::from_c(strerror(errno)) << "\n";
      return false;
    }
  } else {
    /*
     * open our controlling tty as display
     */
    if (DisplayHWCTTY) {
      log(ERROR) << "      tty.InitHW() failed: controlling tty "
                 << (DisplayHWCTTY == HWCTTY_DETACHED ? Chars("not usable after Detach\n")
                                                      : Chars("is already in use as display\n"));
      return false;
    } else {
      is_ctty = true;
      self->tty_fd = 0;
      self->out = stdout;
      if (!self->tty_name.format(Chars::from_c(ttyname(0)))) {
        log(ERROR) << "      tty.InitHW(): out of memory!\n";
        return false;
      }
      if (!self->tty_term) {
        if (!self->tty_term.format(Chars::from_c(origTERM))) {
          log(ERROR) << "      tty.InitHW(): out of memory!\n";
          return false;
        }
      }
    }
  }
  if (self->colormode == tty_color_autodetect) {
    Chars env_colorterm;
    if (!term_override &&
        ((env_colorterm = Chars::from_c(getenv("COLORTERM"))) == Chars("truecolor") ||
         env_colorterm == Chars("24bit"))) {
      self->colormode = tty_color16M;
    } else if (self->tty_term.ends_with(Chars("256")) ||
               self->tty_term.ends_with(Chars("256color"))) {
      self->colormode = tty_color256;
    } else {
      self->colormode = tty_color8;
    }
  }
  fflush(self->out);
  setvbuf(self->out, NULL, _IOFBF, BUFSIZ);

  self->tty_number = 0;
  if (self->tty_name.starts_with(Chars("/dev/tty")) ||
      self->tty_name.starts_with(Chars("/dev/vc/"))) {
    const char *s = self->tty_name.data() + 8;
    const char *end = self->tty_name.end();
    while (s < end && *s >= '0' && *s <= '9') {
      self->tty_number *= 10;
      self->tty_number += *s++ - '0';
    }
  }

#ifdef CONF_HW_TTY_TERMCAP
  self->colorbug = have_colorbug;
#endif

  if (charset) {
    /* honor user-specified charset */
    if ((self->tty_charset = Tutf_charset_id(charset.data())) == (uldat)-1) {
      log(ERROR) << "      tty.InitHW(): libtutf warning: unknown charset `" << charset
                 << "', assuming `ASCII'\n";
      self->tty_use_utf8 = false;
    } else if (self->tty_charset == Tutf_charset_id(T_NAME(UTF_32))) {
      log(ERROR) << "      tty.InitHW(): charset `" << charset
                 << "' is Unicode, assuming terminal supports UTF-8\n";
      self->tty_use_utf8 = true;
      self->tty_charset = (uldat)-1;
    }
  }

#define TRY_V(name) (autotry_video + try_##name >= ALWAYS)
#define TRY_K(name) (autotry_kbd + try_##name >= ALWAYS)

  /*
   * ORDERING IS CRITICAL HERE!
   *
   * xterm_InitMouse() does not need to manually hide/show the mouse pointer,
   * so it overrides hw->ShowMouse and hw->HideMouse installed by *_InitVideo()
   *
   * Thus mouse initialization must come *AFTER* video initialization
   *
   * null_InitMouseConfirm() tries a blocking read() from tty_fd
   * (it asks user if he/she really wants to run without mouse),
   * while lrawkbd_InitKeyboard() puts tty_fd in non-blocking mode.
   *
   * Thus mouse initialization must come *BEFORE* keyboard initialization
   */

  if (!stdin_TestTty(hw)) {
    log(ERROR) << "      tty.InitHW() failed: unable to read from the terminal: " << Errstr << "\n";
  } else if (

#if defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
      (TRY_V(stdout) && linux_InitVideo(hw)) ||
#endif
#ifdef CONF_HW_TTY_TERMCAP
      (TRY_V(termcap) && termcap_InitVideo(hw)) ||
#endif
      false) {

    if (
#ifdef CONF_HW_TTY_LINUX
        gpm_InitMouse(hw) ||
#else
        (log(WARNING) << "      tty.InitHW(): gpm mouse support not compiled, skipping it.\n",
         false) ||
#endif
        xterm_InitMouse(hw, force_mouse) || null_InitMouseConfirm(hw)) {

      if (
#if defined(CONF_HW_TTY_LINUX) && defined(CONF_HW_TTY_LRAWKBD)
          (TRY_K(lrawkbd) && lrawkbd_InitKeyboard(hw)) ||
#endif
          (autotry_kbd && stdin_InitKeyboard(hw))) {

        if (self->tty_charset == (uldat)-1) {
          self->tty_UTF_32_to_charset = Tutf_UTF_32_to_ASCII;
          self->tty_charset_to_UTF_32 = Tutf_ASCII_to_UTF_32;
        } else {
          self->tty_UTF_32_to_charset = Tutf_UTF_32_to_charset_function(self->tty_charset);
          self->tty_charset_to_UTF_32 = Tutf_charset_to_UTF_32_array(self->tty_charset);
        }

        /*
         * must be deferred until now, as hw-specific functions
         * can clobber hw->NeedHW
         */
        if (need_persistent_slot) {
          hw->NeedHW |= NEEDPersistentSlot;
        }
        if (is_ctty) {
          hw->DisplayIsCTTY = ttrue;
          DisplayHWCTTY = hw;
        }
        hw->MouseState.x = hw->MouseState.y = hw->MouseState.keys = hw->Last_x = hw->Last_y = 0;

        /*
         * we must draw everything on our new shiny window
         * without forcing all other displays
         * to redraw everything too.
         */
        stdin_DetectSize(hw, &hw->usedX, &hw->usedY);
        hw->usedX = GetDisplayWidth();
        hw->usedY = GetDisplayHeight();

        hw->RedrawVideo = tfalse;
        NeedRedrawVideo(hw, 0, 0, hw->X - 1, hw->Y - 1);

        if (self->tc_scr_clear) {
          fputs(self->tc_scr_clear, self->out);
        }
        fflush(self->out);

        return true;
      }
      hw->QuitMouse();
    }
    hw->QuitVideo();
  }
  if (self->tty_fd >= 0) {
    tty_setioctl(self->tty_fd, &ttysave);
  }
  if (self->tty_fd > 0) {
    close(self->tty_fd);
  }
  if (self->out && self->out != stdout) {
    fclose(self->out);
  }
  return false;
}

TW_ATTR_HIDDEN void tty_driver::QuitHW(Tdisplay hw) {
  hw->QuitMouse();
  hw->QuitKeyboard();
  hw->QuitVideo();
  hw->fnQuitHW = NULL;

  tty_driver *self = ttydriver(hw);

  // destroy tty_name and tty_term
  String().swap(self->tty_name);
  String().swap(self->tty_term);

  if (hw->DisplayIsCTTY && DisplayHWCTTY == hw) {
    DisplayHWCTTY = NULL;
  }
  FILE *out = self->out;
  fflush(out);
  if (out != stdout) {

    /* if we forced tty_fd to be fd 0, release it while keeping fd 0 busy */
    if (self->tty_fd == 0) {
      if ((self->tty_fd = open("/dev/null", O_RDWR)) != 0) {
        fclose(out);

        dup2(self->tty_fd, 0);
        close(self->tty_fd);
      }
      /*
       * else we don't fclose(out) to avoid having fd 0 unused...
       * it causes leaks, but much better than screwing up badly when
       * fd 0 will get used by something else (say a socket) and then
       * abruptly closed by tty.InitHW()
       */
    } else {
      fclose(out);
    }
  }
  FreeMem(hw->Private);
  hw->Private = NULL;
}

EXTERN_C byte InitModule(Tmodule Module) {
  Module->DoInit = &tty_driver::InitHW;
  return ttrue;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
EXTERN_C void QuitModule(Tmodule Module) {
}
