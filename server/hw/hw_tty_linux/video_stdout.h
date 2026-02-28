/*
 * Video output on Linux terminals through stdout.
 *
 * this is used both inside twin terminal and inside Linux console.
 */

#include "palette.h"

inline void linuxSetCursorType(Tdisplay hw, uldat type) {
  tty_driver *self = ttydriver(hw);
  fprintf(self->out, "\033[?%d;%d;%dc", (int)(type & 0xFF), (int)((type >> 8) & 0xFF),
          (int)((type >> 16) & 0xFF));
}

/* return tfalse if failed */
static byte linuxInitVideo(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  Chars term = self->tty_term;

  if (!term) {
    log(ERROR) << "      linuxInitVideo() failed: unknown terminal type.\n";
    return tfalse;
  }

  if (term != Chars("linux")) {
    log(ERROR) << "      linuxInitVideo() failed: terminal `" << term << "' is not `linux'.\n";
    return tfalse;
  }

  if (!(self->tc_scr_clear = CloneStr("\033[2J"))) {
    log(ERROR) << "      linuxInitVideo() failed: out of memory!";
    return tfalse;
  }

  /* clear colors, enable/disable UTF-8 mode */
  /* if UTF-8 mode is disabled, set TTY_DISPCTRL */
  fprintf(self->out, "\033[0m%s", (self->tty_use_utf8 ? "\033[3l\033%G" : "\033%@\033[3h"));

  hw->fnFlushVideo = &tty_driver::linuxFlushVideo;
  hw->fnFlushHW = &tty_driver::FlushHW;

  hw->fnShowMouse = &tty_driver::linuxShowMouse;
  hw->fnHideMouse = &tty_driver::linuxHideMouse;
  hw->fnUpdateMouseAndCursor = &tty_driver::linuxUpdateMouseAndCursor;

  hw->fnDetectSize = &tty_driver::stdinDetectSize;
  hw->fnCheckResize = &tty_driver::stdinCheckResize;
  hw->fnResize = &tty_driver::stdinResize;

  hw->fnSelectionImport = NULL;
  hw->fnSelectionExport = NULL;
  hw->fnSelectionRequest = NULL;
  hw->fnSelectionNotify = NULL;
  hw->SelectionPrivate = 0;

  hw->fnCanDragArea = &tty_driver::linuxCanDragArea;
  hw->fnDragArea = &tty_driver::linuxDragArea;

  hw->XY[0] = hw->XY[1] = -1;
  hw->TT = (uldat)-1; /* force updating the cursor */

  hw->fnBeep = &tty_driver::linuxBeep;
  hw->fnConfigure = &tty_driver::linuxConfigure;
  hw->fnConfigureKeyboard = &tty_driver::linuxConfigureKeyboard;
  hw->fnSetPalette = &tty_driver::linuxSetPalette;
  hw->fnResetPalette = &tty_driver::linuxResetPalette;

  hw->fnQuitVideo = &tty_driver::linuxQuitVideo;

  hw->FlagsHW |= FlagNeedOldVideoHW;
  hw->FlagsHW &= ~FlagExpensiveFlushVideoHW;
  hw->NeedHW = 0;

  return ttrue;
}

TW_ATTR_HIDDEN void tty_driver::linuxQuitVideo(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);

  vt100MoveToXY(hw, 0, DisplayHeight - 1);
  linuxSetCursorType(hw, LINECURSOR);
  /* restore original colors, TTY_DISPCTRL, alt cursor keys, keypad settings */
  fputs("\033[0m\033[3l\033[?1l\033>\n", self->out);

  hw->fnQuitVideo = NULL;
}

#define CTRL_ALWAYS 0x0800f501 /* Cannot be overridden by TTY_DISPCTRL */

/*
 * the linux console is a noisy place... kernel and daemons often write there.
 * for better results, reinit UTF-8 mode and TTY_DISPCTRL every time
 */
TW_ATTR_HIDDEN void tty_driver::linuxDrawStart(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);

  fputs(self->tty_use_utf8 ? "\033[3l\033%G\033[m" : "\033%@\033[3h\033[m", self->out);

  self->col.fg = 7; // TrueColorToPalette16(twhite);
  self->col.bg = 0; // TrueColorToPalette16(tblack);
}

#define linuxDrawFinish(hw) ((void)0)

TW_ATTR_HIDDEN void tty_driver::linuxSetColor(Tdisplay hw, tcolor col) {
  tty_driver *self = ttydriver(hw);
  byte fg = TrueColorToPalette16(TCOLFG(col));
  byte bg = TrueColorToPalette16(TCOLBG(col));
  byte fg_ = self->col.fg;
  byte bg_ = self->col.bg;

  if (fg == fg_ && bg == bg_) {
    return;
  }
  self->col.fg = fg;
  self->col.bg = bg;

  char colbuf[] = "\033[2x;2x;3x;4xm";
  char *colp = colbuf + 2;
  const byte fg_high = fg & 8;
  const byte bg_high = bg & 8;
  const byte fg_high_ = fg_ & 8;
  const byte bg_high_ = bg_ & 8;

  fg &= 7;
  bg &= 7;
  fg_ &= 7;
  bg_ &= 7;

  if (fg_high != fg_high_) {
    /* ESC[1m  is bold/high intensity */
    /* ESC[22m is normal intensity */
    if (fg_high) {
      *colp++ = '1';
    } else {
      *colp++ = '2';
      *colp++ = '2';
    }
    *colp++ = ';';
  }
  if (bg_high != bg_high_) {
    /* ESC[25m is blink */
    /* ESC[5m  is don't blink */
    if (bg_high_) {
      *colp++ = '2';
    }
    *colp++ = '5';
    *colp++ = ';';
  }
  if (fg != fg_) {
    *colp++ = '3';
    *colp++ = fg + '0';
    *colp++ = ';';
  }
  if (bg != bg_) {
    *colp++ = '4';
    *colp++ = bg + '0';
    *colp++ = ';';
  }
  if (colp[-1] == ';') {
    --colp;
  }
  *colp++ = 'm';

  fwrite(colbuf, 1, colp - colbuf, self->out);
}

TW_ATTR_HIDDEN void tty_driver::linuxDrawSome(Tdisplay hw, dat x, dat y, uldat len) {
  tty_driver *self = ttydriver(hw);
  const tcell *V = Video + x + y * (ldat)DisplayWidth;
  const tcell *oV = OldVideo + x + y * (ldat)DisplayWidth;
  bool sending = false;

  for (; len; V++, oV++, x++, len--) {
    if (!ValidOldVideo || *V != *oV) {
      if (!sending) {
        sending = true;
        vt100MoveToXY(hw, x, y);
      }
      const tcolor c = TCOLOR(*V);
      if (c != self->col) {
        linuxSetColor(hw, c);
      }
      const trune r = TRUNE(*V);
      byte b = (byte)r;
      if (r >= 128) {
        if (self->tty_use_utf8) {
          /* use utf-8 to output this non-ASCII glyph. */
          DrawRune(hw, r);
          continue;
        } else if (r > 255 || self->tty_charset_to_UTF_32[r] != r) {
          b = self->tty_UTF_32_to_charset(r);
        }
      }
      if (self->tty_use_utf8 ? (b < 32 || b == 127)
                             : (b < 32 && ((CTRL_ALWAYS >> b) & 1)) || b == 127 || b == 128 + 27) {
        /* can't display it */
        b = Tutf_UTF_32_to_ASCII(r);
        if (b < 32 || b >= 127) {
          b = 32;
        }
      }
      putc((char)r, self->out);
    } else {
      sending = false;
    }
  }
}

TW_ATTR_HIDDEN void tty_driver::linuxDrawTCell(Tdisplay hw, dat x, dat y, tcell V) {
  tty_driver *self = ttydriver(hw);

  vt100MoveToXY(hw, x, y);

  const tcolor c = TCOLOR(V);
  if (c != self->col) {
    linuxSetColor(hw, c);
  }
  const trune r = TRUNE(V);
  byte b = (byte)r;
  if (r >= 128) {
    if (self->tty_use_utf8) {
      /* use utf-8 to output this non-ASCII glyph. */
      DrawRune(hw, r);
      return;
    } else if (r > 255 || self->tty_charset_to_UTF_32[r] != r) {
      b = self->tty_UTF_32_to_charset(r);
    }
  }
  if (self->tty_use_utf8 ? (b < 32 || b == 127)
                         : (b < 32 && ((CTRL_ALWAYS >> b) & 1)) || b == 127 || b == 128 + 27) {
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
TW_ATTR_HIDDEN void tty_driver::linuxShowMouse(Tdisplay hw) {
  dat x = hw->Last_x = hw->MouseState.x;
  dat y = hw->Last_y = hw->MouseState.y;
  uldat pos = x + y * (uldat)DisplayWidth;
  tcell cell = Video[pos];
  tcolor &col = cell.color;
  col.fg = (~col.fg ^ thigh) & TRGB_MAX;
  col.bg = (~col.bg ^ thigh) & TRGB_MAX;

  linuxDrawTCell(hw, hw->MouseState.x, hw->MouseState.y, cell);

  /* store current cursor state for correct updating */
  hw->XY[1] = hw->MouseState.y;
  hw->XY[0] = hw->MouseState.x + 1;
  /* linux terminals have VT100 wrapglitch */
  if (hw->XY[0] == hw->X) {
    hw->XY[0]--;
  }
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::linuxHideMouse(Tdisplay hw) {
  dat x = hw->Last_x;
  dat y = hw->Last_y;
  uldat pos = x + y * (uldat)DisplayWidth;

  linuxDrawTCell(hw, x, y, Video[pos]);

  /* store current cursor state for correct updating */
  hw->XY[1] = y;
  /* linux terminals have VT100 wrapglitch */
  if ((hw->XY[0] = x + 1) == hw->X) {
    hw->XY[0]--;
  }
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::linuxUpdateCursor(Tdisplay hw) {
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR && (CursorX != hw->XY[0] || CursorY != hw->XY[1]))) {
    vt100MoveToXY(hw, hw->XY[0] = CursorX, hw->XY[1] = CursorY);
    hw->setFlush();
  }
  if (!ValidOldVideo || CursorType != hw->TT) {
    linuxSetCursorType(hw, hw->TT = CursorType);
    hw->setFlush();
  }
}

TW_ATTR_HIDDEN void tty_driver::linuxUpdateMouseAndCursor(Tdisplay hw) {
  if ((hw->FlagsHW & FlagSoftMouseHW) && (hw->FlagsHW & FlagChangedMouseFlagHW)) {
    hw->HideMouse();
    hw->ShowMouse();
    hw->FlagsHW &= ~FlagChangedMouseFlagHW;
  }
  linuxUpdateCursor(hw);
}

TW_ATTR_HIDDEN void tty_driver::linuxFlushVideo(Tdisplay hw) {
  tcell savedOldVideo;
  dat i, j, start, end, XY[2] = {0, 0};
  bool flippedVideo = false, flippedOldVideo = false;

  if (!ChangedVideoFlag) {
    hw->UpdateMouseAndCursor();
    linuxDrawFinish();
    return;
  }

  /* hide the mouse if needed */

  /* first, check the old mouse position */
  if (hw->FlagsHW & FlagSoftMouseHW) {
    if (hw->FlagsHW & FlagChangedMouseFlagHW) {
      /* dirty the old mouse position, so that it will be overwritten */

      dat x = hw->Last_x;
      dat y = hw->Last_y;
      /*
       * with multi-display this is a hack, but since OldVideo gets restored
       * below *BEFORE* returning from linuxFlushVideo(), that's ok.
       */
      DirtyVideo(x, y, x, y);
      if (ValidOldVideo) {
        uldat pos = x + y * (uldat)DisplayWidth;
        flippedOldVideo = true;
        savedOldVideo = OldVideo[pos];
        OldVideo[pos].color = TCOLOR_BAD;
      }
    }

    i = hw->MouseState.x;
    j = hw->MouseState.y;
    /*
     * instead of calling ShowMouse(),
     * we flip the new mouse position in Video[] and dirty it if necessary.
     */
    if ((hw->FlagsHW & FlagChangedMouseFlagHW) || (flippedVideo = plainIsDirtyVideo(i, j))) {
      VideoFlip(i, j);
      if (!flippedVideo)
        DirtyVideo(i, j, i, j);
      hw->FlagsHW &= ~FlagChangedMouseFlagHW;
      flippedVideo = true;
    } else {
      flippedVideo = false;
    }
  }

  linuxDrawStart(hw);
  for (i = 0; i < DisplayHeight * 2; i++) {
    start = ChangedVideo[i >> 1][i & 1][0];
    end = ChangedVideo[i >> 1][i & 1][1];

    if (start != -1) {
      /* also keep track of cursor position */
      linuxDrawSome(hw, start, (XY[1] = i >> 1), (XY[0] = end) - start + 1);
    }
  }

  /* store current cursor state for correct updating */
  hw->XY[1] = XY[1];
  if ((hw->XY[0] = XY[0] + 1) == hw->X) {
    /* linux terminals have VT100 wrapglitch */
    hw->XY[0]--;
  }

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
  linuxUpdateCursor(hw);
  linuxDrawFinish(hw);

  hw->FlagsHW &= ~FlagChangedMouseFlagHW;
}

TW_ATTR_HIDDEN void tty_driver::linuxBeep(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  fputs("\033[3l\007\033[3h", self->out);
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::linuxConfigureKeyboard(Tdisplay hw, udat resource, byte todefault,
                                                       udat value) {
  tty_driver *self = ttydriver(hw);
  switch (resource) {
  case HW_KBDAPPLIC:
    fputs(todefault || !value ? "\033>" : "\033=", self->out);
    hw->setFlush();
    break;
  case HW_ALTCURSKEYS:
    fputs(todefault || !value ? "\033[?1l" : "\033[?1h", self->out);
    hw->setFlush();
    break;
  }
}

TW_ATTR_HIDDEN void tty_driver::linuxConfigure(Tdisplay hw, udat resource, byte todefault,
                                               udat value) {
  tty_driver *self = ttydriver(hw);
  switch (resource) {
  case HW_KBDAPPLIC:
  case HW_ALTCURSKEYS:
    hw->ConfigureKeyboard(resource, todefault, value);
    break;
  case HW_BELLPITCH:
    if (todefault) {
      fputs("\033[10]", self->out);
    } else {
      fprintf(self->out, "\033[10;%hd]", value);
    }
    hw->setFlush();
    break;
  case HW_BELLDURATION:
    if (todefault) {
      fputs("\033[11]", self->out);
    } else {
      fprintf(self->out, "\033[11;%hd]", value);
    }
    hw->setFlush();
    break;
  case HW_MOUSEMOTIONEVENTS:
    hw->ConfigureMouse(resource, todefault, value);
    break;
  default:
    break;
  }
}

TW_ATTR_HIDDEN void tty_driver::linuxSetPalette(Tdisplay hw, udat N, udat R, udat G, udat B) {
  tty_driver *self = ttydriver(hw);
  fprintf(self->out, "\033]P%1hx%02hx%02hx%02hx", N, R, G, B);
  hw->setFlush();
}

TW_ATTR_HIDDEN void tty_driver::linuxResetPalette(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  fputs("\033]R", self->out);
  hw->setFlush();
}

TW_ATTR_HIDDEN bool tty_driver::linuxCanDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn,
                                                 dat DstLeft, dat DstUp) {
  (void)Up;
  (void)DstLeft;
  /*
   * tty scrolling capabilities are very limited...
   * do not even consider using tty `ESC [ <n> M' (delete_line)
   * or `ESC [ <n> L' (insert_line) as they are *SLOW*,
   * so the only usable one is '\n' (newline).
   */
  return Left == 0 && Rgt == hw->X - 1 && Dwn == hw->Y - 1 && DstUp == 0;
}

TW_ATTR_HIDDEN void tty_driver::linuxDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn,
                                              dat DstLeft, dat DstUp) {
  FILE *out = ttydriver(hw)->out;
  udat delta = Up - DstUp;

  (void)Left;
  (void)Rgt;
  (void)DstLeft;

  hw->HideMouse();
  hw->FlagsHW |= FlagChangedMouseFlagHW;

  fprintf(out, "%s\033[m\033[%d;1H", /* hide cursor, reset color, go to last line */
          hw->TT == NOCURSOR ? "" : "\033[?1c", hw->Y);

  while (delta--) {
    putc('\n', out);
  }
  if (hw->fnFlushVideo == &tty_driver::linuxFlushVideo) {
    hw->setFlush();
  } else {
    fflush(out);
  }
  /* store actual cursor state for correct updating */
  hw->XY[0] = 0;
  hw->XY[1] = hw->Y - 1;
  hw->TT = NOCURSOR;

  /*
   * Now the last trick: tty scroll erased the part
   * below DstUp + (Dwn - Up) so we must redraw it.
   * This should not be necessary, as after a DragArea()
   * you are supposed to redraw the original position with whatever
   * is under the obj ect you dragged away, but better safe than sorry.
   */
  NeedRedrawVideo(hw, 0, DstUp + (Dwn - Up) + 1, hw->X - 1, hw->Y - 1);
}
