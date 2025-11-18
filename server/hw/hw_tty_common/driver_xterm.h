

TW_ATTR_HIDDEN void tty_driver::xtermCleanup(Tdisplay hw) {
  (void)hw;
}

TW_ATTR_HIDDEN bool tty_driver::xtermInitVideo(Tdisplay hw) {

  tty_driver *self = ttydriver(hw);
  const Chars term = self->tty_term;

  if (!term.starts_with(Chars("xterm"))) {
    log(ERROR) << "      xtermInitVideo() failed: terminal type '" << term
               << "' is not an xterm.\n";
    return false;
  }

  self->tty_is_xterm = true;

  self->tc[tc_seq_scr_clear] = "\033[H\033[2J";
  self->tc[tc_seq_cursor_on] = "\033[?12l\033[?25h";
  self->tc[tc_seq_cursor_off] = "\033[?25l";
  self->tc[tc_seq_bold_on] = "\033[1m";
  self->tc[tc_seq_blink_on] = "\033[5m";
  self->tc[tc_seq_attr_off] = "\033[m";
  self->tc[tc_seq_kpad_on] = "\033[?1h\033=";
  self->tc[tc_seq_kpad_off] = "\033[?1l\033>";
  self->tc[tc_seq_audio_bell] = "\a";
  if (self->tty_use_utf8) {
    self->tc[tc_seq_charset_start] = "\033%G";
    self->tc[tc_seq_charset_end] = "\033%@";
  } else {
    self->tc[tc_seq_charset_start] = "";
    self->tc[tc_seq_charset_end] = "";
  }

  self->tc[tc_key_FN + 0] = "\033OP";    /* F1 */
  self->tc[tc_key_FN + 1] = "\033OQ";    /* F2 */
  self->tc[tc_key_FN + 2] = "\033OR";    /* F3 */
  self->tc[tc_key_FN + 3] = "\033OS";    /* F4 */
  self->tc[tc_key_FN + 4] = "\033[15~";  /* F5 */
  self->tc[tc_key_FN + 5] = "\033[17~";  /* F6 */
  self->tc[tc_key_FN + 6] = "\033[18~";  /* F7 */
  self->tc[tc_key_FN + 7] = "\033[19~";  /* F8 */
  self->tc[tc_key_FN + 8] = "\033[20~";  /* F9 */
  self->tc[tc_key_FN + 9] = "\033[21~";  /* F10 */
  self->tc[tc_key_FN + 10] = "\033[23~"; /* F11 */
  self->tc[tc_key_FN + 11] = "\033[24~"; /* F12 */
  self->tc[tc_key_Suspend] = "";
  self->tc[tc_key_Home] = "\033OH";
  self->tc[tc_key_End] = "\033OF";
  self->tc[tc_key_Delete] = "\033[3~";
  self->tc[tc_key_Insert] = "\033[2~";
  self->tc[tc_key_Page_Down] = "\033[6~";
  self->tc[tc_key_Page_Up] = "\033[5~";
  self->tc[tc_key_Left] = "\033OD";
  self->tc[tc_key_Up] = "\033OA";
  self->tc[tc_key_Right] = "\033OC";
  self->tc[tc_key_Down] = "\033OB";

  if (self->colormode == tty_color_autodetect) {
    const char *env = getenv("COLORTERM");
    if (env != NULL && strcmp(env, "truecolor") == 0) {
      self->colormode = tty_color16m;
    } else if (term.starts_with(Chars("xterm-256color"))) {
      self->colormode = tty_color256;
    } else {
      self->colormode = tty_color8;
    }
  }

  self->wrapglitch = true;
  fputs(self->tc[tc_seq_attr_off], self->out);
  fputs(self->tc[tc_seq_charset_start], self->out);
  fputs(self->tc[tc_seq_kpad_on], self->out);

  hw->fnFlushVideo = &tty_driver::xtermFlushVideo;
  hw->fnFlushHW = &tty_driver::FlushHW;

  hw->fnShowMouse = &tty_driver::xtermShowMouse;
  hw->fnHideMouse = &tty_driver::xtermHideMouse;
  hw->fnUpdateMouseAndCursor = &tty_driver::xtermUpdateMouseAndCursor;

  hw->fnDetectSize = &tty_driver::stdinDetectSize;
  hw->fnCheckResize = &tty_driver::stdinCheckResize;
  hw->fnResize = &tty_driver::stdinResize;

  hw->fnSelectionImport = NULL;
  hw->fnSelectionExport = NULL;
  hw->fnSelectionRequest = NULL;
  hw->fnSelectionNotify = NULL;
  hw->SelectionPrivate = 0;

  hw->fnCanDragArea = &tty_driver::genericCanDragArea;
  hw->fnDragArea = &tty_driver::xtermDragArea;

  hw->XY[0] = hw->XY[1] = -1;
  hw->TT = -1; /* force updating the cursor */

  hw->fnBeep = &tty_driver::genericBeep;
  hw->fnConfigure = &tty_driver::genericConfigure;
  hw->fnConfigureKeyboard = &tty_driver::genericConfigureKeyboard;
  hw->fnSetPalette = NULL;
  hw->fnResetPalette = NULL;

  hw->fnQuitVideo = &tty_driver::xtermQuitVideo;

  hw->FlagsHW |= FlagNeedOldVideoHW;
  hw->FlagsHW &= ~FlagExpensiveFlushVideoHW;
  hw->NeedHW = 0;

  self->fnLookupKey = &tty_driver::genericLookupKey;

  log(INFO) << "      xtermInitVideo() ok.\n";
  return true;
}

TW_ATTR_HIDDEN void tty_driver::xtermQuitVideo(Tdisplay hw) {
  vt100MoveToXY(hw, 0, DisplayHeight - 1);
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

  xtermCleanup(hw);

  hw->fnQuitVideo = NULL;
}

TW_ATTR_HIDDEN void tty_driver::xtermDrawSome(Tdisplay hw, dat x, dat y, uldat len) {
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
        vt100MoveToXY(hw, x, y);
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

TW_ATTR_HIDDEN void tty_driver::xtermDrawTCell(Tdisplay hw, dat x, dat y, tcell V) {
  tty_driver *self = ttydriver(hw);

  if (!self->wrapglitch && x == DisplayWidth - 1 && y == DisplayHeight - 1) {
    /* wrapglitch is required to write to last screen position without scrolling */
    return;
  }
  vt100MoveToXY(hw, x, y);

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
TW_ATTR_HIDDEN void tty_driver::xtermShowMouse(Tdisplay hw) {
  uldat pos =
      (hw->Last_x = hw->MouseState.x) + (hw->Last_y = hw->MouseState.y) * (ldat)DisplayWidth;
  tcell h = Video[pos], c = TCELL_COLMASK(~h) ^ TCELL(TCOL(thigh, thigh), 0);

  xtermDrawTCell(hw, hw->MouseState.x, hw->MouseState.y, c | TCELL_RUNEMASK(h));

  /* force updating the cursor */
  hw->XY[0] = hw->XY[1] = -1;
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::xtermHideMouse(Tdisplay hw) {
  uldat pos = hw->Last_x + hw->Last_y * (ldat)DisplayWidth;

  xtermDrawTCell(hw, hw->Last_x, hw->Last_y, Video[pos]);

  /* force updating the cursor */
  hw->XY[0] = hw->XY[1] = -1;
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::xtermUpdateCursor(Tdisplay hw) {
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR && (CursorX != hw->XY[0] || CursorY != hw->XY[1]))) {
    vt100MoveToXY(hw, hw->XY[0] = CursorX, hw->XY[1] = CursorY);
    hw->setFlush();
  }
  if (!ValidOldVideo || CursorType != hw->TT) {
    genericSetCursorType(hw, hw->TT = CursorType);
    hw->setFlush();
  }
}

TW_ATTR_HIDDEN void tty_driver::xtermUpdateMouseAndCursor(Tdisplay hw) {
  if ((hw->FlagsHW & FlagSoftMouseHW) && (hw->FlagsHW & FlagChangedMouseFlagHW)) {
    hw->HideMouse();
    hw->ShowMouse();
    hw->FlagsHW &= ~FlagChangedMouseFlagHW;
  }
  xtermUpdateCursor(hw);
}

TW_ATTR_HIDDEN void tty_driver::xtermDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn,
                                              dat DstLeft, dat DstUp) {
  tty_driver *self = ttydriver(hw);
  udat delta = Up - DstUp;

  (void)Rgt;
  (void)Left;
  (void)DstLeft;

  hw->HideMouse();
  hw->FlagsHW |= FlagChangedMouseFlagHW;

  fprintf(self->out, "%s\033[m\033[%d;1H", /* hide cursor, reset color, go to last line */
          self->tc[tc_seq_cursor_off], (int)hw->Y);

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

TW_ATTR_HIDDEN void tty_driver::xtermFlushVideo(Tdisplay hw) {
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
       * below *BEFORE* returning from xtermFlushVideo(), that's ok.
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
      xtermDrawSome(hw, start, i >> 1, end - start + 1);
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

  xtermUpdateCursor(hw);
  genericDrawFinish(hw);

  hw->FlagsHW &= ~FlagChangedMouseFlagHW;
}
