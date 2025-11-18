/** generic implementations, suitable at least for xterm and termcap */

static inline void vt100MoveToXY(Tdisplay hw, udat x, udat y) {
  tty_driver *self = ttydriver(hw);
  fprintf(self->out, "\033[%d;%dH", y + 1, x + 1);
}

TW_ATTR_HIDDEN void tty_driver::genericBeep(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  fputs(self->tc[tc_seq_audio_bell], self->out);
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::genericConfigure(Tdisplay hw, udat resource, byte todefault,
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

TW_ATTR_HIDDEN void tty_driver::genericConfigureKeyboard(Tdisplay hw, udat resource, byte todefault,
                                                         udat value) {
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

TW_ATTR_HIDDEN bool tty_driver::genericCanDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn,
                                                   dat DstLeft, dat DstUp) {
  (void)Up;
  (void)DstLeft;
  return Left == 0 && Rgt == hw->X - 1 && Dwn == hw->Y - 1 && DstUp == 0;
}

static inline void genericDrawStart(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  self->col = ~TCOL0; // i.e. unknown
  self->fg = ~(trgb)0;
  self->bg = ~(trgb)0;
}

#define genericDrawFinish(hw) ((void)0)

TW_ATTR_HIDDEN udat tty_driver::genericLookupKey(Tdisplay hw, udat *ShiftFlags, byte *slen, char *s,
                                                 byte *retlen, const char **ret) {
  struct xterm_keys {
    udat k;
    byte l;
    const char *s;
  };
#define IS(k, l, s) {CAT(TW_, k), l, s}
  static const xterm_keys keys[] = {
      IS(F1, 3, "\033OP"),      IS(F2, 3, "\033OQ"),      IS(F3, 3, "\033OR"),
      IS(F4, 3, "\033OS"),      IS(F5, 5, "\033[15~"),    IS(F6, 5, "\033[17~"),
      IS(F7, 5, "\033[18~"),    IS(F8, 5, "\033[19~"),    IS(F9, 5, "\033[20~"),
      IS(F10, 5, "\033[21~"),   IS(F11, 5, "\033[23~"),   IS(F12, 5, "\033[24~"),
      IS(Pause, 3, "\033[P"),   IS(Home, 4, "\033[1~"),   IS(End, 4, "\033[4~"),
      IS(Delete, 4, "\033[3~"), IS(Insert, 4, "\033[2~"), IS(Next, 4, "\033[6~"),
      IS(Prior, 4, "\033[5~"),  IS(Left, 3, "\033[D"),    IS(Up, 3, "\033[A"),
      IS(Right, 3, "\033[C"),   IS(Down, 3, "\033[B"),
  };
  static const xterm_keys altkeys[] = {
      IS(Left, 3, "\033OD"),
      IS(Up, 3, "\033OA"),
      IS(Right, 3, "\033OC"),
      IS(Down, 3, "\033OB"),
  };
#undef IS
  tty_driver *self = ttydriver(hw);
  const xterm_keys *lk;
  const char **key;
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
        if (self->altcurskeys) {
          if (code >= TW_Left && code <= TW_Down) {
            lk = &altkeys[code - TW_Left];
          } else if (code >= TW_KP_Left && code <= TW_KP_Down) {
            lk = &altkeys[code - TW_KP_Left];
          }
        }
        *slen = keylen;
        *retlen = lk->l;
        *ret = lk->s;
        return code;
      }
    }
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

/******************************************* colors **********************************************/

TW_ATTR_HIDDEN void tty_driver::genericSetColor(Tdisplay hw, tcolor col) {
  tty_driver *self = ttydriver(hw);
  switch (tty_colormode(self->colormode)) {
  case tty_color16m:
    genericSetColor16m(hw, col);
    break;
  case tty_color256:
    genericSetColor256(hw, col);
    break;
  default:
    genericSetColor8(hw, col);
    break;
  }
}

static inline byte fastDiv10(byte value) {
  /* faster than value / 10 */
  return ((udat)value * 205) / 2048;
}

static inline char *genericCopyAttr(const char *src, char *dst) {
  while ((*dst = *src) != '\0') {
    dst++;
    src++;
  }
  return dst;
}

static char *genericPrintSepByte(char *dst, char sep, byte value) {
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
  const byte tens = fastDiv10(value);
  if (hundreds || tens) {
    *dst++ = tens + '0';
    value -= tens * 10;
  }
  *dst++ = value + '0';
  return dst;
}

static char *genericPrintSepRgb(char *dst, char sep, trgb rgb) {
  dst = genericPrintSepByte(dst, sep, TRED(rgb));
  dst = genericPrintSepByte(dst, sep, TGREEN(rgb));
  dst = genericPrintSepByte(dst, sep, TBLUE(rgb));
  return dst;
}

TW_ATTR_HIDDEN void tty_driver::genericSetColor8(Tdisplay hw, tcolor col) {
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
      colp = genericCopyAttr(self->tc[tc_seq_bold_on], colp);
    } else {
      /*
       * cannot turn off standout, reset everything.
       * it may or may not set fg_ = twhite, bg_ = tblack
       * => assume unknown fg, bg
       */
      colp = genericCopyAttr(self->tc[tc_seq_attr_off], colp);
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

TW_ATTR_HIDDEN void tty_driver::genericSetColor256(Tdisplay hw, tcolor col) {
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
    colp = genericPrintSepByte(colp + 4, ';', fg);
  }
  if (bg != bg_) {
    if (fg != fg_) {
      *colp++ = ';';
    }
    *colp++ = '4';
    *colp++ = '8';
    *colp++ = ';';
    *colp++ = '5';
    colp = genericPrintSepByte(colp, ';', bg);
  }
  *colp++ = 'm';
  fwrite(colbuf, 1, colp - colbuf, self->out);
}

TW_ATTR_HIDDEN void tty_driver::genericSetColor16m(Tdisplay hw, tcolor col) {
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
    colp = genericPrintSepRgb(colp + 4, ';', fg);
  }
  if (bg != bg_) {
    if (fg != fg_) {
      *colp++ = ';';
    }
    *colp++ = '4';
    *colp++ = '8';
    *colp++ = ';';
    *colp++ = '2';
    colp = genericPrintSepRgb(colp, ';', bg);
  }
  *colp++ = 'm';
  fwrite(colbuf, 1, colp - colbuf, self->out);
}

/*************************************** end colors **********************************************/

TW_ATTR_HIDDEN void tty_driver::genericSetCursorType(Tdisplay hw, uldat type) {
  tty_driver *self = ttydriver(hw);
  fputs(self->tc[(type & 0xFFFFFFl) == NOCURSOR ? tc_seq_cursor_off : tc_seq_cursor_on], self->out);
}
