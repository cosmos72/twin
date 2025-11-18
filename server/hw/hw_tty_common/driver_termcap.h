

#ifdef CONF_HW_TTY_TERMCAP

#include "palette.h"

TW_ATTR_HIDDEN void tty_driver::termcapMoveToXY(Tdisplay hw, udat x, udat y) {
  tty_driver *self = ttydriver(hw);
  fputs(tgoto(self->tc[tc_seq_cursor_goto], x, y), self->out);
}

static char *termcapExtract(const char *cap, char **dest) {
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

TW_ATTR_HIDDEN void tty_driver::termcapCleanup(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  for (const char **n = self->tc; n < self->tc + tc_seq_N; n++) {
    if (*n) {
      FreeMem(const_cast<char *>(*n));
      *n = NULL;
    }
  }
}

TW_ATTR_HIDDEN bool tty_driver::termcapInitVideo(Tdisplay hw) {
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
    log(ERROR) << "      termcapInitVideo() failed: unknown terminal type.\n";
    return false;
  }

  switch (tgetent(tcbuf, term)) {
  case 1:
    break;
  case 0:
    log(ERROR) << "      termcapInitVideo() failed: no entry for `" << Chars::from_c(term)
               << "' in the terminal database.\n"
                  "      Please set your $TERM environment variable correctly.\n";
    return false;
  default:
    log(ERROR) << "      termcapInitVideo() failed: system call error in tgetent(): "
               << Chars::from_c(strerror(errno)) << "\n";
    return false;
  }

  self->tty_is_xterm = !strncmp(term, "xterm", 5);

  /* do not use alternate character set mode, i.e. "as" and "ae" ...
   * it causes more problems than it solves */
  tc_name[tc_seq_charset_start] = tc_name[tc_seq_charset_end] = "";
  self->tc[tc_seq_charset_start] = self->tc[tc_seq_charset_end] = NULL;

  for (n = tc_name, d = const_cast<char **>(self->tc); *n; n++, d++) {
    if (**n && !termcapExtract(*n, d)) {
      log(ERROR) << "      termcapInitVideo() failed: Out of memory!\n";
      termcapCleanup(hw);
      return false;
    }
  }

  if (!*self->tc[tc_seq_cursor_goto]) {
    log(ERROR) << "      termcapInitVideo() failed: terminal lacks `cursor goto' capability\n";
    termcapCleanup(hw);
    return false;
  }

  if (self->tty_use_utf8) {
    if (!(self->tc[tc_seq_charset_start] = CloneStr("\033%G")) ||
        !(self->tc[tc_seq_charset_end] = CloneStr("\033%@"))) {
      log(ERROR) << "      termcapInitVideo() failed: Out of memory!\n";
      termcapCleanup(hw);
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

  hw->fnFlushVideo = &tty_driver::termcapFlushVideo;
  hw->fnFlushHW = &tty_driver::FlushHW;

  hw->fnShowMouse = &tty_driver::termcapShowMouse;
  hw->fnHideMouse = &tty_driver::termcapHideMouse;
  hw->fnUpdateMouseAndCursor = &tty_driver::termcapUpdateMouseAndCursor;

  hw->fnDetectSize = &tty_driver::stdinDetectSize;
  hw->fnCheckResize = &tty_driver::stdinCheckResize;
  hw->fnResize = &tty_driver::stdinResize;

  hw->fnSelectionImport = NULL;
  hw->fnSelectionExport = NULL;
  hw->fnSelectionRequest = NULL;
  hw->fnSelectionNotify = NULL;
  hw->SelectionPrivate = 0;

  hw->fnCanDragArea = &tty_driver::genericCanDragArea;
  hw->fnDragArea = &tty_driver::termcapDragArea;

  hw->XY[0] = hw->XY[1] = -1;
  hw->TT = -1; /* force updating the cursor */

  hw->fnBeep = &tty_driver::genericBeep;
  hw->fnConfigure = &tty_driver::genericConfigure;
  hw->fnConfigureKeyboard = &tty_driver::genericConfigureKeyboard;
  hw->fnSetPalette = NULL;
  hw->fnResetPalette = NULL;

  hw->fnQuitVideo = termcapQuitVideo;

  hw->FlagsHW |= FlagNeedOldVideoHW;
  hw->FlagsHW &= ~FlagExpensiveFlushVideoHW;
  hw->NeedHW = 0;

  self->fnLookupKey = &tty_driver::genericLookupKey;

  log(INFO) << "      termcapInitVideo() ok.\n";
  return true;
}

TW_ATTR_HIDDEN void tty_driver::termcapQuitVideo(Tdisplay hw) {
  termcapMoveToXY(hw, 0, DisplayHeight - 1);
  genericSetCursorType(hw, LINECURSOR);

  tty_driver *self = ttydriver(hw);

  /* reset colors and charset */
  fputs(self->tc[tc_seq_attr_off], self->out);
  if (self->tc[tc_seq_charset_end]) {
    fputs(self->tc[tc_seq_charset_end], self->out);
  }

  /* restore original alt cursor keys, keypad settings */
  hw->Configure(HW_KBDAPPLIC, ttrue, 0);
  hw->Configure(HW_ALTCURSKEYS, ttrue, 0);

  termcapCleanup(hw);

  hw->fnQuitVideo = NULL;
}

TW_ATTR_HIDDEN void tty_driver::termcapDrawSome(Tdisplay hw, dat x, dat y, uldat len) {
  tty_driver *self = ttydriver(hw);
  uldat delta = x + y * (uldat)DisplayWidth;

  if (!self->wrapglitch && delta + len >= (uldat)DisplayWidth * DisplayHeight) {
    len = (uldat)DisplayWidth * DisplayHeight - delta - 1;
  }
  const tcell *V = Video + delta;
  const tcell *oV = OldVideo + delta;
  bool sending = false;

  for (; len; V++, oV++, x++, len--) {
    if (!ValidOldVideo || *V != *oV) {
      if (!sending) {
        sending = true;
        termcapMoveToXY(hw, x, y);
      }
      const tcolor col = TCOLOR(*V);
      if (col != self->col) {
        genericSetColor(hw, col);
      }
      const trune r = TRUNE(*V);
      byte b = (byte)r;
      if (r >= 128) {
        if (self->tty_use_utf8) {
          /* use utf-8 to output this non-ASCII glyph */
          DrawRune(hw, r);
          continue;
        } else if (r > 255 || self->tty_charset_to_UTF_32[r] != r) {
          b = self->tty_UTF_32_to_charset(r);
        }
      }
      if (b < 32 || b == 127 || b == 128 + 27) {
        /* can't display it */
        b = Tutf_UTF_32_to_ASCII(r);
        if (b < 32 || b >= 127) {
          b = 32;
        }
      }
      putc((char)b, self->out);
    } else {
      sending = false;
    }
  }
}

TW_ATTR_HIDDEN void tty_driver::termcapDrawTCell(Tdisplay hw, dat x, dat y, tcell V) {
  tty_driver *self = ttydriver(hw);

  if (!self->wrapglitch && x == DisplayWidth - 1 && y == DisplayHeight - 1) {
    /* wrapglitch is required to write to last screen position without scrolling */
    return;
  }
  termcapMoveToXY(hw, x, y);

  const tcolor col = TCOLOR(V);
  if (col != self->col) {
    genericSetColor(hw, col);
  }
  const trune r = TRUNE(V);
  byte b = (byte)r;
  if (r >= 128) {
    if (self->tty_use_utf8) {
      /* use utf-8 to output this non-ASCII glyph */
      DrawRune(hw, r);
      return;
    } else if (r > 255 || self->tty_charset_to_UTF_32[r] != r) {
      b = self->tty_UTF_32_to_charset(r);
    }
  }
  if (b < 32 || b == 127 || b == 128 + 27) {
    /* can't display it */
    b = Tutf_UTF_32_to_ASCII(r);
    if (b < 32 || b >= 127) {
      b = 32;
    }
  }
  putc((char)b, self->out);
}

/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have linux, termcap and xterm versions, not GPM ones... */
TW_ATTR_HIDDEN void tty_driver::termcapShowMouse(Tdisplay hw) {
  uldat pos =
      (hw->Last_x = hw->MouseState.x) + (hw->Last_y = hw->MouseState.y) * (ldat)DisplayWidth;
  tcell h = Video[pos], c = TCELL_COLMASK(~h) ^ TCELL(TCOL(thigh, thigh), 0);

  termcapDrawTCell(hw, hw->MouseState.x, hw->MouseState.y, c | TCELL_RUNEMASK(h));

  /* force updating the cursor */
  hw->XY[0] = hw->XY[1] = -1;
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::termcapHideMouse(Tdisplay hw) {
  uldat pos = hw->Last_x + hw->Last_y * (ldat)DisplayWidth;

  termcapDrawTCell(hw, hw->Last_x, hw->Last_y, Video[pos]);

  /* force updating the cursor */
  hw->XY[0] = hw->XY[1] = -1;
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::termcapUpdateCursor(Tdisplay hw) {
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR && (CursorX != hw->XY[0] || CursorY != hw->XY[1]))) {
    termcapMoveToXY(hw, hw->XY[0] = CursorX, hw->XY[1] = CursorY);
    hw->setFlush();
  }
  if (!ValidOldVideo || CursorType != hw->TT) {
    genericSetCursorType(hw, hw->TT = CursorType);
    hw->setFlush();
  }
}

TW_ATTR_HIDDEN void tty_driver::termcapUpdateMouseAndCursor(Tdisplay hw) {
  if ((hw->FlagsHW & FlagSoftMouseHW) && (hw->FlagsHW & FlagChangedMouseFlagHW)) {
    hw->HideMouse();
    hw->ShowMouse();
    hw->FlagsHW &= ~FlagChangedMouseFlagHW;
  }
  termcapUpdateCursor(hw);
}

TW_ATTR_HIDDEN void tty_driver::termcapDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn,
                                                dat DstLeft, dat DstUp) {
  tty_driver *self = ttydriver(hw);
  udat delta = Up - DstUp;

  (void)Left;
  (void)Rgt;
  (void)DstLeft;

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

TW_ATTR_HIDDEN void tty_driver::termcapFlushVideo(Tdisplay hw) {
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
       * below *BEFORE* returning from termcapFlushVideo(), that's ok.
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
  genericDrawStart(hw);

  if (hw->TT != NOCURSOR) {
    genericSetCursorType(hw, hw->TT = NOCURSOR);
  }
  for (i = 0; i < DisplayHeight * 2; i++) {
    start = ChangedVideo[i >> 1][i & 1][0];
    end = ChangedVideo[i >> 1][i & 1][1];
    if (start != -1) {
      termcapDrawSome(hw, start, i >> 1, end - start + 1);
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

  termcapUpdateCursor(hw);
  genericDrawFinish(hw);

  hw->FlagsHW &= ~FlagChangedMouseFlagHW;
}

#endif /* CONF_HW_TTY_TERMCAP */
