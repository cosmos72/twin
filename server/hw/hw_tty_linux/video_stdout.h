/*
 * Video output on Linux terminals through stdout.
 *
 * this is used both inside twin terminal and inside Linux console.
 */

static void linux_QuitVideo(void);
static void linux_ShowMouse(void);
static void linux_HideMouse(void);
static void linux_FlushVideo(void);

static void linux_Beep(void);
static void linux_Configure(udat resource, byte todefault, udat value);
static void linux_ConfigureKeyboard(udat resource, byte todefault, udat value);
static void linux_SetPalette(udat N, udat R, udat G, udat B);
static void linux_ResetPalette(void);
static void linux_UpdateMouseAndCursor(void);

static byte linux_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
static void linux_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);

inline void linux_SetCursorType(uldat type) {
  fprintf(stdOUT, "\033[?%d;%d;%dc", (int)(type & 0xFF), (int)((type >> 8) & 0xFF),
          (int)((type >> 16) & 0xFF));
}
inline void linux_MoveToXY(udat x, udat y) {
  fprintf(stdOUT, "\033[%d;%dH", y + 1, x + 1);
}

/* return tfalse if failed */
static byte linux_InitVideo(void) {
  char *term = tty_TERM;

  if (!term) {
    printk("      linux_InitVideo() failed: unknown terminal type.\n");
    return tfalse;
  }

  if (strcmp(term, "linux")) {
    printk("      linux_InitVideo() failed: terminal `" SS "' is not `linux'.\n", term);
    return tfalse;
  }

  if (!(tc_scr_clear = CloneStr("\033[2J"))) {
    printk("      linux_InitVideo() failed: out of memory!");
    return tfalse;
  }

  if (tty_use_utf8 == ttrue + ttrue) {
    if (ttypar[0] == 6 && ttypar[1] != 3 && ttypar[1] != 4) {
      /* plain linux console supports utf-8, and also twin >= 0.3.11 does. */
      tty_use_utf8 = ttrue;
    } else {
      tty_use_utf8 = tfalse;

      if (tty_charset == (uldat)-1)
        printk("      linux_InitVideo() warning: this 'linux' terminal seems to be an old twin "
               "(version <= 0.3.11) without UTF-8 support. Setting charset='ASCII'\n");
    }
  }
  /* clear colors, enable/disable UTF-8 mode */
  /* if UTF-8 mode is disabled, set TTY_DISPCTRL */
  fprintf(stdOUT, "\033[0m%s", (tty_use_utf8 ? "\033[3l\033%G" : "\033%@\033[3h"));

  HW->FlushVideo = linux_FlushVideo;
  HW->FlushHW = stdout_FlushHW;

  HW->ShowMouse = linux_ShowMouse;
  HW->HideMouse = linux_HideMouse;
  HW->UpdateMouseAndCursor = linux_UpdateMouseAndCursor;

  HW->DetectSize = stdin_DetectSize;
  HW->CheckResize = stdin_CheckResize;
  HW->Resize = stdin_Resize;

  HW->HWSelectionImport = AlwaysFalse;
  HW->HWSelectionExport = NoOp;
  HW->HWSelectionRequest = (void (*)(obj, uldat))NoOp;
  HW->HWSelectionNotify = (void (*)(uldat, uldat, const char *, Chars))NoOp;
  HW->HWSelectionPrivate = 0;

  HW->CanDragArea = linux_CanDragArea;
  HW->DragArea = linux_DragArea;

  HW->XY[0] = HW->XY[1] = -1;
  HW->TT = (uldat)-1; /* force updating the cursor */

  HW->Beep = linux_Beep;
  HW->Configure = linux_Configure;
  HW->ConfigureKeyboard = linux_ConfigureKeyboard;
  HW->SetPalette = linux_SetPalette;
  HW->ResetPalette = linux_ResetPalette;

  HW->QuitVideo = linux_QuitVideo;

  HW->FlagsHW |= FlHWNeedOldVideo;
  HW->FlagsHW &= ~FlHWExpensiveFlushVideo;
  HW->NeedHW = 0;
  HW->merge_Threshold = 0;

  return ttrue;
}

static void linux_QuitVideo(void) {
  linux_MoveToXY(0, DisplayHeight - 1);
  linux_SetCursorType(LINECURSOR);
  /* restore original colors, TTY_DISPCTRL, alt cursor keys, keypad settings */
  fputs("\033[0m\033[3l\033[?1l\033>\n", stdOUT);

  HW->QuitVideo = NoOp;
}

#define CTRL_ALWAYS 0x0800f501 /* Cannot be overridden by TTY_DISPCTRL */

/*
 * the linux console is a noisy place... kernel and daemons often write there.
 * for better results, reinit UTF-8 mode and TTY_DISPCTRL every time
 */
#define linux_MogrifyInit()                                                                        \
  (fputs(tty_use_utf8 ? "\033[3l\033%G\033[m" : "\033%@\033[3h\033[m", stdOUT),                    \
   _col = TCOL(twhite, tblack))

#define linux_MogrifyFinish() ((void)0)

inline void linux_SetColor(tcolor col) {
  char colbuf[] = "\033[2x;2x;4x;3xm";
  char *colp = colbuf + 2;
  byte c;

  if ((col & TCOL(thigh, 0)) != (_col & TCOL(thigh, 0))) {
    if (col & TCOL(thigh, 0))
      *colp++ = '1';
    /* '22m' is normal intensity. we used '21m', which is actually 'doubly underlined' */
    else
      *colp++ = '2', *colp++ = '2';
    *colp++ = ';';
  }
  if ((col & TCOL(0, thigh)) != (_col & TCOL(0, thigh))) {
    if (_col & TCOL(0, thigh))
      *colp++ = '2';
    *colp++ = '5';
    *colp++ = ';';
  }
  if ((col & TCOL(0, twhite)) != (_col & TCOL(0, twhite))) {
    c = TCOLBG(col) & ~thigh;
    *colp++ = '4';
    *colp++ = TVGA2ANSI(c) + '0';
    *colp++ = ';';
  }
  if ((col & TCOL(twhite, 0)) != (_col & TCOL(twhite, 0))) {
    c = TCOLFG(col) & ~thigh;
    *colp++ = '3';
    *colp++ = TVGA2ANSI(c) + '0';
    *colp++ = ';';
  }
  _col = col;

  if (colp[-1] == ';')
    --colp;
  *colp++ = 'm';
  *colp = '\0';

  fputs(colbuf, stdOUT);
}

inline void linux_Mogrify(dat x, dat y, uldat len) {
  tcell *V, *oV;
  tcolor col;
  trune c, _c;
  byte sending = tfalse;

  V = Video + x + y * (ldat)DisplayWidth;
  oV = OldVideo + x + y * (ldat)DisplayWidth;

  for (; len; V++, oV++, x++, len--) {
    if (!ValidOldVideo || *V != *oV) {
      if (!sending)
        sending = ttrue, linux_MoveToXY(x, y);

      col = TCOLOR(*V);

      if (col != _col)
        linux_SetColor(col);

      c = _c = TRUNE(*V);
      if (c >= 128) {
        if (tty_use_utf8) {
          /* use utf-8 to output this non-ASCII char. */
          tty_MogrifyUTF8(c);
          continue;
        } else if (tty_charset_to_UTF_32[c] != c) {
          c = tty_UTF_32_to_charset(_c);
        }
      }
      if (tty_use_utf8 ? (c < 32 || c == 127)
                       : (c < 32 && ((CTRL_ALWAYS >> c) & 1)) || c == 127 || c == 128 + 27) {
        /* can't display it */
        c = Tutf_UTF_32_to_ASCII(_c);
        if (c < 32 || c >= 127)
          c = 32;
      }
      putc((char)c, stdOUT);
    } else
      sending = tfalse;
  }
}

inline void linux_SingleMogrify(dat x, dat y, tcell V) {
  trune c, _c;

  linux_MoveToXY(x, y);

  if (TCOLOR(V) != _col)
    linux_SetColor(TCOLOR(V));

  c = _c = TRUNE(V);
  if (c >= 128) {
    if (tty_use_utf8) {
      /* use utf-8 to output this non-ASCII char. */
      tty_MogrifyUTF8(c);
      return;
    } else if (tty_charset_to_UTF_32[c] != c) {
      c = tty_UTF_32_to_charset(_c);
    }
  }
  if (tty_use_utf8 ? (c < 32 || c == 127)
                   : (c < 32 && ((CTRL_ALWAYS >> c) & 1)) || c == 127 || c == 128 + 27) {
    /* can't display it */
    c = Tutf_UTF_32_to_ASCII(_c);
    if (c < 32 || c >= 127)
      c = 32;
  }
  putc((char)c, stdOUT);
}

/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have linux_ and termcap_ versions, not GPM_ ones... */
static void linux_ShowMouse(void) {
  uldat pos =
      (HW->Last_x = HW->MouseState.x) + (HW->Last_y = HW->MouseState.y) * (ldat)DisplayWidth;
  tcell h = Video[pos];
  tcolor c = ~TCOLOR(h) ^ TCOL(thigh, thigh);

  linux_SingleMogrify(HW->MouseState.x, HW->MouseState.y, TCELL(c, TRUNEEXTRA(h)));

  /* store current cursor state for correct updating */
  HW->XY[1] = HW->MouseState.y;
  HW->XY[0] = HW->MouseState.x + 1;
  /* linux terminals have VT100 wrapglitch */
  if (HW->XY[0] == HW->X)
    HW->XY[0]--;
  setFlush();
}

static void linux_HideMouse(void) {
  uldat pos = HW->Last_x + HW->Last_y * (ldat)DisplayWidth;

  linux_SingleMogrify(HW->Last_x, HW->Last_y, Video[pos]);

  /* store current cursor state for correct updating */
  HW->XY[1] = HW->Last_y;
  HW->XY[0] = HW->Last_x + 1;
  /* linux terminals have VT100 wrapglitch */
  if (HW->XY[0] == HW->X)
    HW->XY[0]--;
  setFlush();
}

static void linux_UpdateCursor(void) {
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR && (CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
    linux_MoveToXY(HW->XY[0] = CursorX, HW->XY[1] = CursorY);
    setFlush();
  }
  if (!ValidOldVideo || CursorType != HW->TT) {
    linux_SetCursorType(HW->TT = CursorType);
    setFlush();
  }
}

static void linux_UpdateMouseAndCursor(void) {
  if ((HW->FlagsHW & FlHWSoftMouse) && (HW->FlagsHW & FlHWChangedMouseFlag)) {
    HW->HideMouse();
    HW->ShowMouse();
    HW->FlagsHW &= ~FlHWChangedMouseFlag;
  }

  linux_UpdateCursor();
}

static void linux_FlushVideo(void) {
  dat i, j, start, end, XY[2];
  byte FlippedVideo = tfalse, FlippedOldVideo = tfalse;
  tcell savedOldVideo;

  if (!ChangedVideoFlag) {
    HW->UpdateMouseAndCursor();
    linux_MogrifyFinish();
    return;
  }

  /* hide the mouse if needed */

  /* first, check the old mouse position */
  if (HW->FlagsHW & FlHWSoftMouse) {
    if (HW->FlagsHW & FlHWChangedMouseFlag) {
      /* dirty the old mouse position, so that it will be overwritten */

      /*
       * with multi-display this is a hack, but since OldVideo gets restored
       * below *BEFORE* returning from linux_FlushVideo(), that's ok.
       */
      DirtyVideo(HW->Last_x, HW->Last_y, HW->Last_x, HW->Last_y);
      if (ValidOldVideo) {
        FlippedOldVideo = ttrue;
        savedOldVideo = OldVideo[HW->Last_x + HW->Last_y * (ldat)DisplayWidth];
        OldVideo[HW->Last_x + HW->Last_y * (ldat)DisplayWidth] =
            ~Video[HW->Last_x + HW->Last_y * (ldat)DisplayWidth];
      }
    }

    i = HW->MouseState.x;
    j = HW->MouseState.y;
    /*
     * instead of calling ShowMouse(),
     * we flip the new mouse position in Video[] and dirty it if necessary.
     */
    if ((HW->FlagsHW & FlHWChangedMouseFlag) || (FlippedVideo = Plain_isDirtyVideo(i, j))) {
      VideoFlip(i, j);
      if (!FlippedVideo)
        DirtyVideo(i, j, i, j);
      HW->FlagsHW &= ~FlHWChangedMouseFlag;
      FlippedVideo = ttrue;
    } else
      FlippedVideo = tfalse;
  }

  linux_MogrifyInit();
  for (i = 0; i < DisplayHeight * 2; i++) {
    start = ChangedVideo[i >> 1][i & 1][0];
    end = ChangedVideo[i >> 1][i & 1][1];

    if (start != -1)
      /* also keep track of cursor position */
      linux_Mogrify(start, XY[1] = i >> 1, (XY[0] = end) - start + 1);
  }

  /* store current cursor state for correct updating */
  HW->XY[1] = XY[1];
  if ((HW->XY[0] = XY[0] + 1) == HW->X)
    /* linux terminals have VT100 wrapglitch */
    HW->XY[0]--;

  setFlush();

  /* ... and this redraws the mouse */
  if (HW->FlagsHW & FlHWSoftMouse) {
    if (FlippedOldVideo)
      OldVideo[HW->Last_x + HW->Last_y * (ldat)DisplayWidth] = savedOldVideo;
    if (FlippedVideo)
      VideoFlip(HW->Last_x = HW->MouseState.x, HW->Last_y = HW->MouseState.y);
    else if (HW->FlagsHW & FlHWChangedMouseFlag)
      HW->ShowMouse();
  }
  linux_UpdateCursor();

  linux_MogrifyFinish();

  HW->FlagsHW &= ~FlHWChangedMouseFlag;
}

static void linux_Beep(void) {
  fputs("\033[3l\007\033[3h", stdOUT);
  setFlush();
}

static void linux_ConfigureKeyboard(udat resource, byte todefault, udat value) {
  switch (resource) {
  case HW_KBDAPPLIC:
    fputs(todefault || !value ? "\033>" : "\033=", stdOUT);
    setFlush();
    break;
  case HW_ALTCURSKEYS:
    fputs(todefault || !value ? "\033[?1l" : "\033[?1h", stdOUT);
    setFlush();
    break;
  }
}

static void linux_Configure(udat resource, byte todefault, udat value) {
  switch (resource) {
  case HW_KBDAPPLIC:
  case HW_ALTCURSKEYS:
    HW->ConfigureKeyboard(resource, todefault, value);
    break;
  case HW_BELLPITCH:
    if (todefault)
      fputs("\033[10]", stdOUT);
    else
      fprintf(stdOUT, "\033[10;%hd]", value);
    setFlush();
    break;
  case HW_BELLDURATION:
    if (todefault)
      fputs("\033[11]", stdOUT);
    else
      fprintf(stdOUT, "\033[11;%hd]", value);
    setFlush();
    break;
  case HW_MOUSEMOTIONEVENTS:
    HW->ConfigureMouse(resource, todefault, value);
    break;
  default:
    break;
  }
}

static void linux_SetPalette(udat N, udat R, udat G, udat B) {
  fprintf(stdOUT, "\033]P%1hx%02hx%02hx%02hx", N, R, G, B);
  setFlush();
}

static void linux_ResetPalette(void) {
  fputs("\033]R", stdOUT);
  setFlush();
}

static byte linux_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  /*
   * tty scrolling capabilities are very limited...
   * do not even consider using tty `ESC [ <n> M' (delete_line)
   * or `ESC [ <n> L' (insert_line) as they are *SLOW*,
   * so the only usable one is '\n' (newline).
   */
  return Left == 0 && Rgt == HW->X - 1 && Dwn == HW->Y - 1 && DstUp == 0;
}

static void linux_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  udat delta = Up - DstUp;

  HW->HideMouse();
  HW->FlagsHW |= FlHWChangedMouseFlag;

  fprintf(stdOUT, "%s\033[m\033[%d;1H", /* hide cursor, reset color, go to last line */
          HW->TT == NOCURSOR ? "" : "\033[?1c", HW->Y);

  while (delta--)
    putc('\n', stdOUT);

  if (HW->FlushVideo == linux_FlushVideo)
    setFlush();
  else
    fflush(stdOUT);

  /* store actual cursor state for correct updating */
  HW->XY[0] = 0;
  HW->XY[1] = HW->Y - 1;
  HW->TT = NOCURSOR;

  /*
   * Now the last trick: tty scroll erased the part
   * below DstUp + (Dwn - Up) so we must redraw it.
   * This should not be necessary, as after a DragArea()
   * you are supposed to redraw the original position with whatever
   * is under the obj ect you dragged away, but better safe than sorry.
   */
  NeedRedrawVideo(0, DstUp + (Dwn - Up) + 1, HW->X - 1, HW->Y - 1);
}
