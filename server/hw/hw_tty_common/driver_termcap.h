

#ifdef CONF_HW_TTY_TERMCAP

inline void termcap_SetCursorType(uldat type) {
  fprintf(stdOUT, "%s", (type & 0xFFFFFFl) == NOCURSOR ? tc_cursor_off : tc_cursor_on);
}
inline void termcap_MoveToXY(udat x, udat y) {
  fputs(tgoto(tc_cursor_goto, x, y), stdOUT);
}

static udat termcap_LookupKey(udat *ShiftFlags, byte *slen, char *s, byte *retlen,
                              const char **ret) {
  struct linux_keys {
    udat k;
    byte l;
    const char *s;
  };
  static struct linux_keys const linux_key[] = {
#define IS(k, l, s) {CAT(TW_, k), l, s},
      IS(F1, 4, "\033[[A") IS(F2, 4, "\033[[B") IS(F3, 4, "\033[[C") IS(F4, 4, "\033[[D")
          IS(F5, 4, "\033[[E") IS(F6, 5, "\033[17~") IS(F7, 5, "\033[18~") IS(F8, 5, "\033[19~")
              IS(F9, 5, "\033[20~") IS(F10, 5, "\033[21~") IS(F11, 5, "\033[23~")
                  IS(F12, 5, "\033[24~")

                      IS(Pause, 3, "\033[P") IS(Home, 4, "\033[1~") IS(End, 4, "\033[4~")
                          IS(Delete, 4, "\033[3~") IS(Insert, 4, "\033[2~") IS(Next, 4, "\033[6~")
                              IS(Prior, 4, "\033[5~")

                                  IS(Left, 3, "\033[D") IS(Up, 3, "\033[A") IS(Right, 3, "\033[C")
                                      IS(Down, 3, "\033[B")
#undef IS
  };
  struct linux_keys const *lk;

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

    for (key = tc_cap + tc_key_first; key < tc_cap + tc_key_last; key++) {
      if (*key && **key && (keylen = strlen(*key)) <= len && !memcmp(*key, s, keylen)) {
        lk = linux_key + (key - &tc_cap[tc_key_first]);
        *slen = keylen;
        *retlen = lk->l;
        *ret = lk->s;
        return lk->k;
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

static void termcap_cleanup(void) {
  char **n;
  for (n = tc_cap; n < tc_cap + tc_cap_N; n++) {
    if (*n)
      FreeMem(*n);
  }
}

static void fixup_colorbug(void) {
  uldat len = strlen(tc_attr_off);
  char *s = (char *)AllocMem(len + 9);
  if (s) {
    CopyMem(tc_attr_off, s, len);
    CopyMem("\033[37;40m", s + len, 9);
    FreeMem(tc_attr_off);
    tc_attr_off = s;
  }
}

static byte termcap_InitVideo(void) {
  const char *term = tty_TERM.data(); // guaranteed to be '\0' terminated
  const char *tc_name[tc_cap_N + 1] = {"cl", "cm", "ve", "vi", "md", "mb", "me", "ks", "ke",
                                       "bl", "as", "ae", "k1", "k2", "k3", "k4", "k5", "k6",
                                       "k7", "k8", "k9", "k;", "F1", "F2", "&7", "kh", "@7",
                                       "kD", "kI", "kN", "kP", "kl", "ku", "kr", "kd", NULL};
  const char **n;
  char **d;
  char tcbuf[4096]; /* by convention, this is enough */

  if (!term) {
    log(ERROR, "      termcap_InitVideo() failed: unknown terminal type.\n");
    return tfalse;
  }

  switch (tgetent(tcbuf, term)) {
  case 1:
    break;
  case 0:
    log(ERROR, "      termcap_InitVideo() failed: no entry for `", Chars::from_c(term),
        "' in the terminal database.\n"
        "      Please set your $TERM environment variable correctly.\n");
    return tfalse;
  default:
    log(ERROR, "      termcap_InitVideo() failed: system call error in tgetent(): ",
        Chars::from_c(strerror(errno)), "\n");
    return tfalse;
  }

  tty_is_xterm = !strncmp(term, "xterm", 5);

  /* do not use alternate character set mode, i.e. "as" and "ae" ... it causes more problems than it
   * solves */
  tc_name[tc_seq_charset_start] = tc_name[tc_seq_charset_end] = "";
  tc_charset_start = tc_charset_end = NULL;

  for (n = tc_name, d = tc_cap; *n; n++, d++) {
    if (**n && !termcap_extract(*n, d)) {
      log(ERROR, "      termcap_InitVideo() failed: Out of memory!\n");
      termcap_cleanup();
      return tfalse;
    }
  }

  if (!*tc_cursor_goto) {
    log(ERROR, "      termcap_InitVideo() failed: terminal misses `cursor goto' capability\n");
    termcap_cleanup();
    return tfalse;
  }

  if (tty_use_utf8 == ttrue + ttrue) {
    /* cannot really autodetect an utf8-capable terminal... use a whitelist */
    uldat termlen = strlen(term);
    tty_use_utf8 = ((termlen == 5 && (!memcmp(term, "xterm", 5) || !memcmp(term, "linux", 5))) ||
                    (termlen >= 6 && !memcmp(term, "xterm-", 6)) ||
                    (termlen >= 12 && !memcmp(term, "rxvt-unicode", 12)));
  }
  if (tty_use_utf8 == ttrue) {
    if (!(tc_charset_start = CloneStr("\033%G")) || !(tc_charset_end = CloneStr("\033%@"))) {
      log(ERROR, "      termcap_InitVideo() failed: Out of memory!\n");
      termcap_cleanup();
      return tfalse;
    }
  }

  wrapglitch = tgetflag("xn");
  if (colorbug)
    fixup_colorbug();

  fprintf(stdOUT, "%s%s%s", tc_attr_off, (tc_charset_start ? (const char *)tc_charset_start : ""),
          (tty_is_xterm ? "\033[?1h" : ""));

  HW->FlushVideo = termcap_FlushVideo;
  HW->FlushHW = stdout_FlushHW;

  HW->ShowMouse = termcap_ShowMouse;
  HW->HideMouse = termcap_HideMouse;
  HW->UpdateMouseAndCursor = termcap_UpdateMouseAndCursor;

  HW->DetectSize = stdin_DetectSize;
  HW->CheckResize = stdin_CheckResize;
  HW->Resize = stdin_Resize;

  HW->HWSelectionImport = AlwaysFalse;
  HW->HWSelectionExport = NoOp;
  HW->HWSelectionRequest = (void (*)(obj, uldat))NoOp;
  HW->HWSelectionNotify = (void (*)(uldat, e_id, const char *, Chars))NoOp;
  HW->HWSelectionPrivate = 0;

  HW->CanDragArea = termcap_CanDragArea;
  HW->DragArea = termcap_DragArea;

  HW->XY[0] = HW->XY[1] = -1;
  HW->TT = -1; /* force updating the cursor */

  HW->Beep = termcap_Beep;
  HW->Configure = termcap_Configure;
  HW->ConfigureKeyboard = termcap_ConfigureKeyboard;
  HW->SetPalette = (void (*)(udat, udat, udat, udat))NoOp;
  HW->ResetPalette = (void (*)(void))NoOp;

  HW->QuitVideo = termcap_QuitVideo;

  HW->FlagsHW |= FlHWNeedOldVideo;
  HW->FlagsHW &= ~FlHWExpensiveFlushVideo;
  HW->NeedHW = 0;
  HW->merge_Threshold = 0;

  LookupKey = termcap_LookupKey;

  return ttrue;
}

static void termcap_QuitVideo(void) {

  termcap_MoveToXY(0, DisplayHeight - 1);
  termcap_SetCursorType(LINECURSOR);
  /* reset colors and charset */
  fprintf(stdOUT, "%s%s", tc_attr_off, tc_charset_end ? tc_charset_end : "");

  /* restore original alt cursor keys, keypad settings */
  HW->Configure(HW_KBDAPPLIC, ttrue, 0);
  HW->Configure(HW_ALTCURSKEYS, ttrue, 0);

  termcap_cleanup();

  HW->QuitVideo = NoOp;
}

#define termcap_MogrifyInit()                                                                      \
  fputs(tc_attr_off, stdOUT);                                                                      \
  _col = TCOL(twhite, tblack)
#define termcap_MogrifyFinish()                                                                    \
  do {                                                                                             \
  } while (0)

inline char *termcap_CopyAttr(char *attr, char *dest) {
  while ((*dest++ = *attr++))
    ;
  return --dest;
}

inline void termcap_SetColor(tcolor col) {
  static char colbuf[80];
  char *colp = colbuf;
  byte c;

  if ((col & TCOL(thigh, thigh)) != (_col & TCOL(thigh, thigh))) {

    if (((_col & TCOL(0, thigh)) && !(col & TCOL(0, thigh))) ||
        ((_col & TCOL(thigh, 0)) && !(col & TCOL(thigh, 0)))) {

      /* cannot turn off blinking or standout, reset everything */
      colp = termcap_CopyAttr(tc_attr_off, colp);
      _col = TCOL(twhite, tblack);
    }
    if ((col & TCOL(thigh, 0)) && !(_col & TCOL(thigh, 0)))
      colp = termcap_CopyAttr(tc_bold_on, colp);
    if ((col & TCOL(0, thigh)) && !(_col & TCOL(0, thigh)))
      colp = termcap_CopyAttr(tc_blink_on, colp);
  }

  if ((col & TCOL(twhite, twhite)) != (_col & TCOL(twhite, twhite))) {
    *colp++ = '\033';
    *colp++ = '[';

    if ((col & TCOL(twhite, 0)) != (_col & TCOL(twhite, 0))) {
      *colp++ = '3';
      c = TCOLFG(col) & ~thigh;
      *colp++ = TVGA2ANSI(c) + '0';
      *colp++ = ';';
    }

    if ((col & TCOL(0, twhite)) != (_col & TCOL(0, twhite))) {
      *colp++ = '4';
      c = TCOLBG(col) & ~thigh;
      *colp++ = TVGA2ANSI(c) + '0';
      *colp++ = 'm';
    } else if (colp[-1] == ';') {
      colp[-1] = 'm';
    }
  }

  *colp = '\0';
  _col = col;

  fputs(colbuf, stdOUT);
}

inline void termcap_Mogrify(dat x, dat y, uldat len) {
  uldat delta = x + y * (uldat)DisplayWidth;
  tcell *V, *oV;
  tcolor col;
  trune c, _c;
  byte sending = tfalse;

  if (!wrapglitch && delta + len >= (uldat)DisplayWidth * DisplayHeight)
    len = (uldat)DisplayWidth * DisplayHeight - delta - 1;

  V = Video + delta;
  oV = OldVideo + delta;

  for (; len; V++, oV++, x++, len--) {
    if (!ValidOldVideo || *V != *oV) {
      if (!sending)
        sending = ttrue, termcap_MoveToXY(x, y);

      col = TCOLOR(*V);

      if (col != _col)
        termcap_SetColor(col);

      c = _c = TRUNE(*V);
      if (c >= 128) {
        if (tty_use_utf8) {
          /* use utf-8 to output this non-ASCII char */
          tty_MogrifyUTF8(_c);
          continue;
        } else if (tty_charset_to_UTF_32[c] != c) {
          c = tty_UTF_32_to_charset(_c);
        }
      }
      if (c < 32 || c == 127 || c == 128 + 27) {
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

inline void termcap_SingleMogrify(dat x, dat y, tcell V) {
  trune c, _c;

  if (!wrapglitch && x == DisplayWidth - 1 && y == DisplayHeight - 1)
    /* wrapglitch is required to write to last screen position without scrolling */
    return;

  termcap_MoveToXY(x, y);

  if (TCOLOR(V) != _col)
    termcap_SetColor(TCOLOR(V));

  c = _c = TRUNE(V);
  if (c >= 128) {
    if (tty_use_utf8) {
      /* use utf-8 to output this non-ASCII char */
      tty_MogrifyUTF8(_c);
      return;
    } else if (tty_charset_to_UTF_32[c] != c) {
      c = tty_UTF_32_to_charset(_c);
    }
  }
  if (c < 32 || c == 127 || c == 128 + 27) {
    /* can't display it */
    c = Tutf_UTF_32_to_ASCII(_c);
    if (c < 32 || c >= 127)
      c = 32;
  }
  putc((char)c, stdOUT);
}

/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have linux_ and termcap_ versions, not GPM_ ones... */
static void termcap_ShowMouse(void) {
  uldat pos =
      (HW->Last_x = HW->MouseState.x) + (HW->Last_y = HW->MouseState.y) * (ldat)DisplayWidth;
  tcell h = Video[pos], c = TCELL_COLMASK(~h) ^ TCELL(TCOL(thigh, thigh), 0);

  termcap_SingleMogrify(HW->MouseState.x, HW->MouseState.y, c | TCELL_FONTMASK(h));

  /* force updating the cursor */
  HW->XY[0] = HW->XY[1] = -1;
  setFlush();
}

static void termcap_HideMouse(void) {
  uldat pos = HW->Last_x + HW->Last_y * (ldat)DisplayWidth;

  termcap_SingleMogrify(HW->Last_x, HW->Last_y, Video[pos]);

  /* force updating the cursor */
  HW->XY[0] = HW->XY[1] = -1;
  setFlush();
}

static void termcap_Beep(void) {
  fputs(tc_audio_bell, stdOUT);
  setFlush();
}

static void termcap_UpdateCursor(void) {
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR && (CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
    termcap_MoveToXY(HW->XY[0] = CursorX, HW->XY[1] = CursorY);
    setFlush();
  }
  if (!ValidOldVideo || CursorType != HW->TT) {
    termcap_SetCursorType(HW->TT = CursorType);
    setFlush();
  }
}

static void termcap_UpdateMouseAndCursor(void) {
  if ((HW->FlagsHW & FlHWSoftMouse) && (HW->FlagsHW & FlHWChangedMouseFlag)) {
    HW->HideMouse();
    HW->ShowMouse();
    HW->FlagsHW &= ~FlHWChangedMouseFlag;
  }

  termcap_UpdateCursor();
}

static void termcap_ConfigureKeyboard(udat resource, byte todefault, udat value) {
  switch (resource) {
  case HW_KBDAPPLIC:
    if (tty_is_xterm) {
      fputs(todefault || !value ? tc_kpad_on : tc_kpad_off, stdOUT);
      /*
       * on xterm, tc_kpad_off has the undesired side-effect
       * of changing the sequences produced by cursor keys,
       * so we must restore the usual sequences
       */
      fputs("\033[?1h", stdOUT);
      setFlush();
    }
    break;
  case HW_ALTCURSKEYS:
    /*
     fputs(todefault || !value ? "\033[?1l" : "\033[?1h", stdOUT);
     setFlush();
     */
    break;
  }
}

static void termcap_Configure(udat resource, byte todefault, udat value) {
  switch (resource) {
  case HW_KBDAPPLIC:
  case HW_ALTCURSKEYS:
    HW->ConfigureKeyboard(resource, todefault, value);
    break;
  case HW_BELLPITCH:
  case HW_BELLDURATION:
    /* unsupported */
    break;
  case HW_MOUSEMOTIONEVENTS:
    HW->ConfigureMouse(resource, todefault, value);
    break;
  default:
    break;
  }
}

static byte termcap_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  return Left == 0 && Rgt == HW->X - 1 && Dwn == HW->Y - 1 && DstUp == 0;
}

static void termcap_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  udat delta = Up - DstUp;

  HW->HideMouse();
  HW->FlagsHW |= FlHWChangedMouseFlag;

  fprintf(stdOUT, "%s\033[0m%s",                               /* hide cursor, reset color */
          tc_cursor_off, tgoto(tc_cursor_goto, 0, HW->Y - 1)); /* go to last line */

  while (delta--)
    putc('\n', stdOUT);

  setFlush();

  /* force updating the cursor */
  HW->XY[0] = HW->XY[1] = -1;

  /*
   * now the last trick: tty scroll erased the part
   * below DstUp + (Dwn - Up) so we must redraw it.
   */
  NeedRedrawVideo(0, DstUp + (Dwn - Up) + 1, HW->X - 1, HW->Y - 1);
}

static void termcap_FlushVideo(void) {
  dat i, j;
  dat start, end;
  byte FlippedVideo = tfalse, FlippedOldVideo = tfalse;
  tcell savedOldVideo;

  if (!ChangedVideoFlag) {
    HW->UpdateMouseAndCursor();
    return;
  }

  /* hide the mouse if needed */

  /* first, check the old mouse position */
  if (HW->FlagsHW & FlHWSoftMouse) {
    if (HW->FlagsHW & FlHWChangedMouseFlag) {
      /* dirty the old mouse position, so that it will be overwritten */

      /*
       * with multi-display this is a hack, but since OldVideo gets restored
       * below *BEFORE* returning from termcap_FlushVideo(), that's ok.
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

  termcap_MogrifyInit();
  if (HW->TT != NOCURSOR)
    termcap_SetCursorType(HW->TT = NOCURSOR);
  for (i = 0; i < DisplayHeight * 2; i++) {
    start = ChangedVideo[i >> 1][i & 1][0];
    end = ChangedVideo[i >> 1][i & 1][1];

    if (start != -1)
      termcap_Mogrify(start, i >> 1, end - start + 1);
  }

  /* force updating the cursor */
  HW->XY[0] = HW->XY[1] = -1;

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

  termcap_UpdateCursor();

  HW->FlagsHW &= ~FlHWChangedMouseFlag;
}

#endif /* CONF_HW_TTY_TERMCAP */
