

#ifdef CONF_HW_TTY_TERMCAP

#include "palette.h"

TW_ATTR_HIDDEN void tty_driver::termcap_SetCursorType(Tdisplay hw, uldat type) {
  tty_driver *self = ttydriver(hw);
  fputs((type & 0xFFFFFFl) == NOCURSOR ? self->tc[tc_seq_cursor_off] : self->tc[tc_seq_cursor_on],
        self->out);
}
TW_ATTR_HIDDEN void tty_driver::termcap_MoveToXY(Tdisplay hw, udat x, udat y) {
  tty_driver *self = ttydriver(hw);
  fputs(tgoto(self->tc[tc_seq_cursor_goto], x, y), self->out);
}

TW_ATTR_HIDDEN udat tty_driver::termcap_LookupKey(Tdisplay hw, udat *ShiftFlags, byte *slen,
                                                  char *s, byte *retlen, const char **ret) {
  struct xterm_keys {
    udat k;
    byte l;
    const char *s;
  };
#define IS(k, l, s) {CAT(TW_, k), l, s}
  static const xterm_keys keys[] = {
      IS(F1, 4, "\033[[A"),     IS(F2, 4, "\033[[B"),     IS(F3, 4, "\033[[C"),
      IS(F4, 4, "\033[[D"),     IS(F5, 4, "\033[[E"),     IS(F6, 5, "\033[17~"),
      IS(F7, 5, "\033[18~"),    IS(F8, 5, "\033[19~"),    IS(F9, 5, "\033[20~"),
      IS(F10, 5, "\033[21~"),   IS(F11, 5, "\033[23~"),   IS(F12, 5, "\033[24~"),
      IS(Pause, 3, "\033[P"),   IS(Home, 4, "\033[1~"),   IS(End, 4, "\033[4~"),
      IS(Delete, 4, "\033[3~"), IS(Insert, 4, "\033[2~"), IS(Next, 4, "\033[6~"),
      IS(Prior, 4, "\033[5~"),  IS(Left, 3, "\033[D"),    IS(Up, 3, "\033[A"),
      IS(Right, 3, "\033[C"),   IS(Down, 3, "\033[B"),
  };
  static const xterm_keys altkeys[] = {
      IS(Left, 3, "\033OD"),    IS(Up, 3, "\033OA"),
      IS(Right, 3, "\033OC"),   IS(Down, 3, "\033OB"),
  };
#undef IS
  tty_driver *self = ttydriver(hw);
  const xterm_keys *lk;
  char **key;
  byte keylen, len = *slen;

  *ret = s;
  *ShiftFlags = 0;

  if (len == 0) {
    *retlen = len;
    return TW_Null;
  }

  if (len > 1 && *s == '\033') {

    if (len == 2 && s[1] >= ' ' && s[1] <= '~') {
      /* try to handle ALT + <some key> */
      *slen = *retlen = len;
      *ShiftFlags = KBD_ALT_FL;
      return (udat)s[1];
    }

    for (key = self->tc + tc_key_first; key < self->tc + tc_key_last; key++) {
      if (*key && **key && (keylen = strlen(*key)) <= len && !memcmp(*key, s, keylen)) {
	lk = &keys[key - &self->tc[tc_key_first]];
	udat code = lk->k;
	if (self->altcurskeys && code >= TW_Left && code <= TW_Down) {
	  lk = &altkeys[code - TW_Left];
	}
        *slen = keylen;
        *retlen = lk->l;
        *ret = lk->s;
        return code;
      }
    }

    /*
     * return stdin_LookupKey(ShiftFlags, slen, s, retlen, ret);
     */
  }

  *slen = *retlen = 1;

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

static char *termcap_extract(const char *cap, char **dest) {
  // man page declares tgetstr(char *, char **) although it does not modify the first argument
  char buf[20], *d = buf, *s = tgetstr(const_cast<char *>(cap), &d);

  if (!s || !*s) {
    return *dest = CloneStr("");
  }
  /*
   * Remove the padding information. We assume that no terminals
   * need padding nowadays. At least it makes things much easier.
   */
  s += strspn(s, "0123456789*");

  for (d = buf; *s; *d++ = *s++) {
    if (*s == '$' && s[1] == '<')
      if (!(s = strchr(s, '>')) || !*++s)
        break;
  }
  *d = '\0';
  return *dest = CloneStr(buf);
}

TW_ATTR_HIDDEN void tty_driver::termcap_Cleanup(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  for (char **n = self->tc; n < self->tc + tc_seq_N; n++) {
    if (*n) {
      FreeMem(*n);
      *n = NULL;
    }
  }
}

TW_ATTR_HIDDEN bool tty_driver::termcap_InitVideo(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  const char *term = self->tty_term.data(); // guaranteed to be '\0' terminated
  const char *tc_name[tc_seq_N + 1] = {"cl", "cm", "ve", "vi", "md", "mb", "me", "ks", "ke",
                                       "bl", "as", "ae", "k1", "k2", "k3", "k4", "k5", "k6",
                                       "k7", "k8", "k9", "k;", "F1", "F2", "&7", "kh", "@7",
                                       "kD", "kI", "kN", "kP", "kl", "ku", "kr", "kd", NULL};
  const char **n;
  char **d;
  char tcbuf[4096]; /* by convention, this is enough */

  if (!term) {
    log(ERROR) << "      termcap_InitVideo() failed: unknown terminal type.\n";
    return false;
  }

  switch (tgetent(tcbuf, term)) {
  case 1:
    break;
  case 0:
    log(ERROR) << "      termcap_InitVideo() failed: no entry for `" << Chars::from_c(term)
               << "' in the terminal database.\n"
                  "      Please set your $TERM environment variable correctly.\n";
    return false;
  default:
    log(ERROR) << "      termcap_InitVideo() failed: system call error in tgetent(): "
               << Chars::from_c(strerror(errno)) << "\n";
    return false;
  }

  self->tty_is_xterm = !strncmp(term, "xterm", 5);

  /* do not use alternate character set mode, i.e. "as" and "ae" ...
   * it causes more problems than it solves */
  tc_name[tc_seq_charset_start] = tc_name[tc_seq_charset_end] = "";
  self->tc[tc_seq_charset_start] = self->tc[tc_seq_charset_end] = NULL;

  for (n = tc_name, d = self->tc; *n; n++, d++) {
    if (**n && !termcap_extract(*n, d)) {
      log(ERROR) << "      termcap_InitVideo() failed: Out of memory!\n";
      termcap_Cleanup(hw);
      return false;
    }
  }

  if (!*self->tc[tc_seq_cursor_goto]) {
    log(ERROR) << "      termcap_InitVideo() failed: terminal lacks `cursor goto' capability\n";
    termcap_Cleanup(hw);
    return false;
  }

  if (self->tty_use_utf8) {
    if (!(self->tc[tc_seq_charset_start] = CloneStr("\033%G")) ||
        !(self->tc[tc_seq_charset_end] = CloneStr("\033%@"))) {
      log(ERROR) << "      termcap_InitVideo() failed: Out of memory!\n";
      termcap_Cleanup(hw);
      return false;
    }
  }

  if (self->colormode == tty_color_autodetect && tgetflag("RGB") != 0) {
    self->colormode = tty_color16m;
  }

  self->wrapglitch = tgetflag("xn");
  fputs(self->tc[tc_seq_attr_off], self->out);
  if (self->tc[tc_seq_charset_start]) {
    fputs(self->tc[tc_seq_charset_start], self->out);
  }
  if (self->tty_is_xterm) {
    fputs("\033[?1h", self->out);
  }

  hw->fnFlushVideo = &tty_driver::termcap_FlushVideo;
  hw->fnFlushHW = &tty_driver::FlushHW;

  hw->fnShowMouse = &tty_driver::termcap_ShowMouse;
  hw->fnHideMouse = &tty_driver::termcap_HideMouse;
  hw->fnUpdateMouseAndCursor = &tty_driver::termcap_UpdateMouseAndCursor;

  hw->fnDetectSize = &tty_driver::stdin_DetectSize;
  hw->fnCheckResize = &tty_driver::stdin_CheckResize;
  hw->fnResize = &tty_driver::stdin_Resize;

  hw->fnSelectionImport = NULL;
  hw->fnSelectionExport = NULL;
  hw->fnSelectionRequest = NULL;
  hw->fnSelectionNotify = NULL;
  hw->SelectionPrivate = 0;

  hw->fnCanDragArea = &tty_driver::termcap_CanDragArea;
  hw->fnDragArea = &tty_driver::termcap_DragArea;

  hw->XY[0] = hw->XY[1] = -1;
  hw->TT = -1; /* force updating the cursor */

  hw->fnBeep = &tty_driver::termcap_Beep;
  hw->fnConfigure = &tty_driver::termcap_Configure;
  hw->fnConfigureKeyboard = &tty_driver::termcap_ConfigureKeyboard;
  hw->fnSetPalette = NULL;
  hw->fnResetPalette = NULL;

  hw->fnQuitVideo = termcap_QuitVideo;

  hw->FlagsHW |= FlagNeedOldVideoHW;
  hw->FlagsHW &= ~FlagExpensiveFlushVideoHW;
  hw->NeedHW = 0;

  self->fnLookupKey = &tty_driver::termcap_LookupKey;

  return true;
}

TW_ATTR_HIDDEN void tty_driver::termcap_QuitVideo(Tdisplay hw) {
  termcap_MoveToXY(hw, 0, DisplayHeight - 1);
  termcap_SetCursorType(hw, LINECURSOR);

  tty_driver *self = ttydriver(hw);

  /* reset colors and charset */
  fputs(self->tc[tc_seq_attr_off], self->out);
  if (self->tc[tc_seq_charset_end]) {
    fputs(self->tc[tc_seq_charset_end], self->out);
  }

  /* restore original alt cursor keys, keypad settings */
  hw->Configure(HW_KBDAPPLIC, ttrue, 0);
  hw->Configure(HW_ALTCURSKEYS, ttrue, 0);

  termcap_Cleanup(hw);

  hw->fnQuitVideo = NULL;
}

inline void termcap_DrawStart(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  self->col = ~TCOL0; // i.e. unknown
  self->fg = ~(trgb)0;
  self->bg = ~(trgb)0;
}

#define termcap_DrawFinish(hw) ((void)0)

inline char *termcap_CopyAttr(const char *src, char *dst) {
  while ((*dst = *src) != '\0') {
    dst++;
    src++;
  }
  return dst;
}

TW_ATTR_HIDDEN void tty_driver::termcap_SetColor8(Tdisplay hw, tcolor col) {
  tty_driver *self = ttydriver(hw);
  byte fg = TrueColorToPalette16(TCOLFG(col));
  byte bg = TrueColorToPalette8(TCOLBG(col));
  // (byte)~(trgb)0 & 7 may accidentally match fg or bg
  // => check for ~TCOL0
  byte fg_ = self->col == ~TCOL0 ? ~fg : self->fg;
  byte bg_ = self->col == ~TCOL0 ? ~bg : self->bg;

  self->col = col;
  self->fg = fg;
  self->bg = bg;
  if (fg == fg_ && bg == bg_) {
    return;
  }
  char colbuf[80];
  char *colp = colbuf;
  byte fg_high = fg & 8;
  byte fg_high_ = fg_ & 8;

  /* ignore high-intensity background: rendering it as blinking is visually annoying */
  if (fg_high != fg_high_) {
    if (fg_high) {
      colp = termcap_CopyAttr(self->tc[tc_seq_bold_on], colp);
    } else {
      /*
       * cannot turn off standout, reset everything.
       * it may or may not set fg_ = twhite, bg_ = tblack
       * => assume unknown fg, bg
       */
      colp = termcap_CopyAttr(self->tc[tc_seq_attr_off], colp);
      fg_ = ~fg;
      bg_ = ~bg;
    }
  }
  fg &= 7;
  fg_ &= 7;

  if (fg != fg_ || bg != bg_) {
    *colp++ = '\033';
    *colp++ = '[';

    if (fg != fg_) {
      *colp++ = '3';
      *colp++ = fg + '0';
      *colp++ = ';';
    }

    if (bg != bg_) {
      *colp++ = '4';
      *colp++ = bg + '0';
      *colp++ = 'm';
    } else if (colp[-1] == ';') {
      colp[-1] = 'm';
    }
  }
  fwrite(colbuf, 1, colp - colbuf, self->out);
}

inline byte termcap_div10(byte value) {
  /* faster than value / 10 */
  return ((udat)value * 205) / 2048;
}

static char *termcap_print_sep_number(char *dst, char sep, byte value) {
  byte hundreds;
  *dst++ = sep;
  if (value >= 200) {
    hundreds = 2;
    *dst++ = '2';
    value -= 200;
  } else if (value >= 100) {
    hundreds = 1;
    *dst++ = '1';
    value -= 100;
  } else {
    hundreds = 0;
  }
  const byte tens = termcap_div10(value);
  if (hundreds || tens) {
    *dst++ = tens + '0';
    value -= tens * 10;
  }
  *dst++ = value + '0';
  return dst;
}

static char *termcap_print_sep_rgb(char *dst, char sep, trgb rgb) {
  dst = termcap_print_sep_number(dst, sep, TRED(rgb));
  dst = termcap_print_sep_number(dst, sep, TGREEN(rgb));
  dst = termcap_print_sep_number(dst, sep, TBLUE(rgb));
  return dst;
}

TW_ATTR_HIDDEN void tty_driver::termcap_SetColor256(Tdisplay hw, tcolor col) {
  tty_driver *self = ttydriver(hw);
  byte fg = TrueColorToPalette256(TCOLFG(col));
  byte bg = TrueColorToPalette256(TCOLBG(col));
  // (byte)~(trgb)0 may accidentally match fg or bg
  // => check for ~TCOL0
  byte fg_ = self->col == ~TCOL0 ? ~fg : self->fg;
  byte bg_ = self->col == ~TCOL0 ? ~bg : self->bg;

  self->col = col;
  self->fg = fg;
  self->bg = bg;
  if (fg == fg_ && bg == bg_) {
    return;
  }
  char colbuf[] = "\033[38;5;xxx;48;5;xxxm";
  char *colp = colbuf + 2;
  if (fg != fg_) {
    /* colp + 4 skips "38;5" */
    colp = termcap_print_sep_number(colp + 4, ';', fg);
  }
  if (bg != bg_) {
    if (fg != fg_) {
      *colp++ = ';';
    }
    *colp++ = '4';
    *colp++ = '8';
    *colp++ = ';';
    *colp++ = '5';
    colp = termcap_print_sep_number(colp, ';', bg);
  }
  *colp++ = 'm';
  fwrite(colbuf, 1, colp - colbuf, self->out);
}

TW_ATTR_HIDDEN void tty_driver::termcap_SetColor16m(Tdisplay hw, tcolor col) {
  tty_driver *self = ttydriver(hw);
  trgb fg = TCOLFG(col);
  trgb bg = TCOLBG(col);
  // ~(trgb)0 is intentionally an invalid color
  // it cannot match fg or bg
  trgb fg_ = self->fg;
  trgb bg_ = self->bg;

  self->col = col;
  self->fg = fg;
  self->bg = bg;
  if (fg == fg_ && bg == bg_) {
    return;
  }
  char colbuf[] = "\033[38;2;xxx;xxx;xxx;48;2;xxx;xxx;xxxm";
  char *colp = colbuf + 2;
  if (fg != fg_) {
    /* colp + 4 skips "38;2" */
    colp = termcap_print_sep_rgb(colp + 4, ';', fg);
  }
  if (bg != bg_) {
    if (fg != fg_) {
      *colp++ = ';';
    }
    *colp++ = '4';
    *colp++ = '8';
    *colp++ = ';';
    *colp++ = '2';
    colp = termcap_print_sep_rgb(colp, ';', bg);
  }
  *colp++ = 'm';
  fwrite(colbuf, 1, colp - colbuf, self->out);
}

TW_ATTR_HIDDEN void tty_driver::termcap_SetColor(Tdisplay hw, tcolor col) {
  tty_driver *self = ttydriver(hw);
  switch (tty_colormode(self->colormode)) {
  case tty_color16m:
    termcap_SetColor16m(hw, col);
    break;
  case tty_color256:
    termcap_SetColor256(hw, col);
    break;
  default:
    termcap_SetColor8(hw, col);
    break;
  }
}

TW_ATTR_HIDDEN void tty_driver::termcap_DrawSome(Tdisplay hw, dat x, dat y, uldat len) {
  tty_driver *self = ttydriver(hw);
  const tcell *V, *oV;
  tcolor col;
  trune c, _c;
  uldat delta = x + y * (uldat)DisplayWidth;
  bool sending = false;

  if (!self->wrapglitch && delta + len >= (uldat)DisplayWidth * DisplayHeight) {
    len = (uldat)DisplayWidth * DisplayHeight - delta - 1;
  }
  V = Video + delta;
  oV = OldVideo + delta;

  for (; len; V++, oV++, x++, len--) {
    if (!ValidOldVideo || *V != *oV) {
      if (!sending) {
        sending = true;
        termcap_MoveToXY(hw, x, y);
      }
      col = TCOLOR(*V);
      if (col != self->col) {
        termcap_SetColor(hw, col);
      }
      c = _c = TRUNE(*V);
      if (c >= 128) {
        if (self->tty_use_utf8) {
          /* use utf-8 to output this non-ASCII char */
          DrawRune(hw, _c);
          continue;
        } else if (self->tty_charset_to_UTF_32[c] != c) {
          c = self->tty_UTF_32_to_charset(_c);
        }
      }
      if (c < 32 || c == 127 || c == 128 + 27) {
        /* can't display it */
        c = Tutf_UTF_32_to_ASCII(_c);
        if (c < 32 || c >= 127) {
          c = 32;
        }
      }
      putc((char)c, self->out);
    } else {
      sending = false;
    }
  }
}

TW_ATTR_HIDDEN void tty_driver::termcap_DrawTCell(Tdisplay hw, dat x, dat y, tcell V) {
  tty_driver *self = ttydriver(hw);
  trune c, _c;

  if (!self->wrapglitch && x == DisplayWidth - 1 && y == DisplayHeight - 1) {
    /* wrapglitch is required to write to last screen position without scrolling */
    return;
  }
  termcap_MoveToXY(hw, x, y);

  if (TCOLOR(V) != self->col) {
    termcap_SetColor(hw, TCOLOR(V));
  }
  c = _c = TRUNE(V);
  if (c >= 128) {
    if (self->tty_use_utf8) {
      /* use utf-8 to output this non-ASCII char */
      DrawRune(hw, _c);
      return;
    } else if (self->tty_charset_to_UTF_32[c] != c) {
      c = self->tty_UTF_32_to_charset(_c);
    }
  }
  if (c < 32 || c == 127 || c == 128 + 27) {
    /* can't display it */
    c = Tutf_UTF_32_to_ASCII(_c);
    if (c < 32 || c >= 127) {
      c = 32;
    }
  }
  putc((char)c, self->out);
}

/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have linux_ and termcap_ versions, not GPM_ ones... */
TW_ATTR_HIDDEN void tty_driver::termcap_ShowMouse(Tdisplay hw) {
  uldat pos =
      (hw->Last_x = hw->MouseState.x) + (hw->Last_y = hw->MouseState.y) * (ldat)DisplayWidth;
  tcell h = Video[pos], c = TCELL_COLMASK(~h) ^ TCELL(TCOL(thigh, thigh), 0);

  termcap_DrawTCell(hw, hw->MouseState.x, hw->MouseState.y, c | TCELL_RUNEMASK(h));

  /* force updating the cursor */
  hw->XY[0] = hw->XY[1] = -1;
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::termcap_HideMouse(Tdisplay hw) {
  uldat pos = hw->Last_x + hw->Last_y * (ldat)DisplayWidth;

  termcap_DrawTCell(hw, hw->Last_x, hw->Last_y, Video[pos]);

  /* force updating the cursor */
  hw->XY[0] = hw->XY[1] = -1;
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::termcap_Beep(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  fputs(self->tc[tc_seq_audio_bell], self->out);
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::termcap_UpdateCursor(Tdisplay hw) {
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR && (CursorX != hw->XY[0] || CursorY != hw->XY[1]))) {
    termcap_MoveToXY(hw, hw->XY[0] = CursorX, hw->XY[1] = CursorY);
    hw->setFlush();
  }
  if (!ValidOldVideo || CursorType != hw->TT) {
    termcap_SetCursorType(hw, hw->TT = CursorType);
    hw->setFlush();
  }
}

TW_ATTR_HIDDEN void tty_driver::termcap_UpdateMouseAndCursor(Tdisplay hw) {
  if ((hw->FlagsHW & FlagSoftMouseHW) && (hw->FlagsHW & FlagChangedMouseFlagHW)) {
    hw->HideMouse();
    hw->ShowMouse();
    hw->FlagsHW &= ~FlagChangedMouseFlagHW;
  }
  termcap_UpdateCursor(hw);
}

TW_ATTR_HIDDEN void tty_driver::termcap_ConfigureKeyboard(Tdisplay hw, udat resource,
                                                          byte todefault, udat value) {
  tty_driver *self = ttydriver(hw);
  switch (resource) {
  case HW_KBDAPPLIC:
    if (self->tty_is_xterm) {
      fputs(self->tc[todefault || !value ? tc_seq_kpad_on : tc_seq_kpad_off], self->out);
      /*
       * on xterm, tc_kpad_off has the undesired side-effect
       * of changing the sequences produced by cursor keys,
       * so we must restore the usual sequences
       */
      fputs("\033[?1h", self->out);
      hw->setFlush();
    }
    break;
  case HW_ALTCURSKEYS:
    self->altcurskeys = value != 0 && !todefault;
    break;
  }
}

TW_ATTR_HIDDEN void tty_driver::termcap_Configure(Tdisplay hw, udat resource, byte todefault,
                                                  udat value) {
  switch (resource) {
  case HW_KBDAPPLIC:
  case HW_ALTCURSKEYS:
    hw->ConfigureKeyboard(resource, todefault, value);
    break;
  case HW_BELLPITCH:
  case HW_BELLDURATION:
    /* unsupported */
    break;
  case HW_MOUSEMOTIONEVENTS:
    hw->ConfigureMouse(resource, todefault, value);
    break;
  default:
    break;
  }
}

TW_ATTR_HIDDEN bool tty_driver::termcap_CanDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn,
                                                    dat DstLeft, dat DstUp) {
  return Left == 0 && Rgt == hw->X - 1 && Dwn == hw->Y - 1 && DstUp == 0;
}

TW_ATTR_HIDDEN void tty_driver::termcap_DragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn,
                                                 dat DstLeft, dat DstUp) {
  tty_driver *self = ttydriver(hw);
  udat delta = Up - DstUp;

  hw->HideMouse();
  hw->FlagsHW |= FlagChangedMouseFlagHW;

  fprintf(self->out, "%s\033[0m%s", /* hide cursor, reset color */
          self->tc[tc_seq_cursor_off],
          tgoto(self->tc[tc_seq_cursor_goto], 0, hw->Y - 1)); /* go to last line */

  while (delta--) {
    putc('\n', self->out);
  }
  hw->setFlush();

  /* force updating the cursor */
  hw->XY[0] = hw->XY[1] = -1;

  /*
   * now the last trick: tty scroll erased the part
   * below DstUp + (Dwn - Up) so we must redraw it.
   */
  NeedRedrawVideo(hw, 0, DstUp + (Dwn - Up) + 1, hw->X - 1, hw->Y - 1);
}

TW_ATTR_HIDDEN void tty_driver::termcap_FlushVideo(Tdisplay hw) {
  tcell savedOldVideo;
  dat i, j;
  dat start, end;
  bool flippedVideo = false, flippedOldVideo = false;

  if (!ChangedVideoFlag) {
    hw->UpdateMouseAndCursor();
    return;
  }

  /* hide the mouse if needed */

  /* first, check the old mouse position */
  if (hw->FlagsHW & FlagSoftMouseHW) {
    if (hw->FlagsHW & FlagChangedMouseFlagHW) {
      /* dirty the old mouse position, so that it will be overwritten */

      /*
       * with multi-display this is a hack, but since OldVideo gets restored
       * below *BEFORE* returning from termcap_FlushVideo(), that's ok.
       */
      DirtyVideo(hw->Last_x, hw->Last_y, hw->Last_x, hw->Last_y);
      if (ValidOldVideo) {
        flippedOldVideo = true;
        savedOldVideo = OldVideo[hw->Last_x + hw->Last_y * (ldat)DisplayWidth];
        OldVideo[hw->Last_x + hw->Last_y * (ldat)DisplayWidth] =
            ~Video[hw->Last_x + hw->Last_y * (ldat)DisplayWidth];
      }
    }

    i = hw->MouseState.x;
    j = hw->MouseState.y;
    /*
     * instead of calling ShowMouse(),
     * we flip the new mouse position in Video[] and dirty it if necessary.
     */
    if ((hw->FlagsHW & FlagChangedMouseFlagHW) || (flippedVideo = Plain_isDirtyVideo(i, j))) {
      VideoFlip(i, j);
      if (!flippedVideo)
        DirtyVideo(i, j, i, j);
      hw->FlagsHW &= ~FlagChangedMouseFlagHW;
      flippedVideo = true;
    } else
      flippedVideo = false;
  }
  termcap_DrawStart(hw);

  if (hw->TT != NOCURSOR) {
    termcap_SetCursorType(hw, hw->TT = NOCURSOR);
  }
  for (i = 0; i < DisplayHeight * 2; i++) {
    start = ChangedVideo[i >> 1][i & 1][0];
    end = ChangedVideo[i >> 1][i & 1][1];
    if (start != -1) {
      termcap_DrawSome(hw, start, i >> 1, end - start + 1);
    }
  }

  /* force updating the cursor */
  hw->XY[0] = hw->XY[1] = -1;

  hw->setFlush();

  /* ... and this redraws the mouse */
  if (hw->FlagsHW & FlagSoftMouseHW) {
    if (flippedOldVideo) {
      OldVideo[hw->Last_x + hw->Last_y * (ldat)DisplayWidth] = savedOldVideo;
    }
    if (flippedVideo) {
      VideoFlip(hw->Last_x = hw->MouseState.x, hw->Last_y = hw->MouseState.y);
    } else if (hw->FlagsHW & FlagChangedMouseFlagHW) {
      hw->ShowMouse();
    }
  }

  termcap_UpdateCursor(hw);
  termcap_DrawFinish(hw);

  hw->FlagsHW &= ~FlagChangedMouseFlagHW;
}

#endif /* CONF_HW_TTY_TERMCAP */
