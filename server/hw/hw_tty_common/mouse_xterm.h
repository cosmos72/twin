
/*
 * xterm-style mouse input
 *
 * used to let twin run inside twin terminal, xterm, etc.
 *
 * it is always compiled in.
 */

static void xterm_QuitMouse(void);
static void xterm_MouseEvent(int fd, display_hw hw);
static void xterm_ConfigureMouse(udat resource, byte todefault, udat value);

/* return tfalse if failed */
static byte xterm_InitMouse(byte force) {
  const char *term = tty_TERM.data(); // guaranteed to be '\0' terminated

  if (force == ttrue) {
    log(WARNING, "      xterm_InitMouse(): xterm-style mouse FORCED.\n"
                 "      Assuming terminal has xterm compatible mouse reporting.\n");
    term = "xterm";
  } else if (force == ttrue + ttrue) {
    log(WARNING, "      xterm_InitMouse(): twterm-style mouse FORCED.\n"
                 "      Assuming terminal has twterm compatible mouse reporting.\n");
    term = "twterm";
  }

  if (!term) {
    log(ERROR, "%s", "      xterm_InitMouse() failed: unknown terminal type.\n");
    return tfalse;
  }

  mouse_start_seq = "\033[?9h";
  mouse_end_seq = "\033[?9l";
  mouse_motion_seq = "\033[?999h";

  if (!strcmp(term, "twterm")) {
    ;
  } else if (!strcmp(term, "linux")) {
    /*
     * additional check: out-of-the box linux
     * doesn't have twterm-style mouse reporting
     */
    if (ttypar[0] < 6 || (ttypar[0] == 6 && ttypar[1] < 3)) {
      log(ERROR, "%s",
          "      xterm_InitMouse() failed: this `linux' terminal\n"
          "      has no support for xterm-style mouse reporting.\n");
      return tfalse;
    }
    if (ttypar[0] == 6 && ttypar[1] < 4) {
      log(WARNING, "%s",
          "      xterm_InitMouse() warning: this `linux' terminal\n"
          "      can only report click, drag and release, not motion.\n");
      mouse_motion_seq = mouse_start_seq;
    }
  } else if (!strncmp(term, "xterm", 5) || !strncmp(term, "rxvt", 4) ||
             !strncmp(term, "Eterm", 5)) {
    /* try to enable more common protocols first,
     * then try to enable progressively more complete protocols */
    mouse_start_seq = "\033[?1001s\033[?1000h\033[?1002h\033[?1006h";
    mouse_end_seq = "\033[?1006l\033[?1002l\033[?1000l\033[?1001r";
    mouse_motion_seq = mouse_start_seq;
  } else {
    log(ERROR, "      xterm_InitMouse() failed: terminal `", Chars::from_c(term),
        "' is not supported.\n");
    return tfalse;
  }

  fputs(mouse_start_seq, stdOUT);
  setFlush();

  HW->mouse_slot = NOSLOT; /* shared with keyboard */

  HW->MouseEvent = xterm_MouseEvent;
  HW->ConfigureMouse = xterm_ConfigureMouse;
  HW->QuitMouse = xterm_QuitMouse;

  HW->FlagsHW &= ~FlHWSoftMouse;        /* no need to Hide/Show it */
  HW->ShowMouse = HW->HideMouse = NoOp; /* override the ones set by InitVideo() */

  return ttrue;
}

static void xterm_QuitMouse(void) {
  fputs(mouse_end_seq, stdOUT);
  HW->QuitMouse = NoOp;
}

static void xterm_ConfigureMouse(udat resource, byte todefault, udat value) {
  switch (resource) {
  case HW_MOUSEMOTIONEVENTS:
    /* either enable new style + mouse motion, or switch back to new style */
    fputs(value && !todefault ? mouse_motion_seq : mouse_start_seq, stdOUT);
    setFlush();
    break;
  default:
    break;
  }
}

/*
 * classic xterm mouse protocol:
 * id = 0: button 1 pressed
 * id = 1: button 2 pressed
 * id = 2: button 3 pressed
 * id = 3: release
 * id = 64: button 4 (wheel back) pressed
 * id = 65: button 5 (wheel fwd) pressed
 * ignore keyboard modifiers: |4 (shift) |8 (alt) |16 (ctrl) |32 (misc)
 */
static udat xterm_Id2Buttons(udat id) {
  id &= 0xC3;
  if (id == 0)
    return HOLD_LEFT;
  else if (id == 1)
    return HOLD_MIDDLE;
  else if (id == 2)
    return HOLD_RIGHT;
  else if (id == 64)
    return HOLD_WHEEL_REV;
  else if (id == 65)
    return HOLD_WHEEL_FWD;
  else
    return 0;
}

static udat xterm_ParseBase10(const char **s_ptr, byte *len_ptr) {
  const char *s = *s_ptr;
  udat val = 0;
  byte len = *len_ptr;

  while (len) {
    const char ch = *(s++);
    len--;
    if (ch >= '0' && ch <= '9') {
      val = val * 10 + (udat)(ch - '0');
    } else {
      // skip non-decimal digit and return
      break;
    }
  }
  *s_ptr = s;
  *len_ptr = len;
  return val;
}

static void xterm_MouseEvent(int fd, display_hw hw) {
  udat buttons = 0, id;
  dat x, y, dx, dy;
  dat prev_x = xterm_prev_x, prev_y = xterm_prev_y;
  const char *s = xterm_mouse_seq;
  byte len = xterm_mouse_len;
  bool release_buttons = false;

  if (s[0] != '\033' || s[1] != '[')
    return;

  if (len == 6 && s[2] == 'M' && (id = (byte)s[3]) >= 32) {
    /* classic xterm mouse reporting: X11 specs, ESC [?1002h */
    buttons = xterm_Id2Buttons(id);
    release_buttons = buttons >= HOLD_WHEEL_REV;

    x = (byte)s[4] - '!'; // x range is 0..222
    y = (byte)s[5] - '!'; // y range is 0..222
  } else if (len >= 9 && s[2] == '<' && (s[len - 1] == 'M' || s[len - 1] == 'm')) {
    /**
     * enhanced xterm mouse reporting: X11 specs, ESC [?1002h ESC [?1006h
     * similar to classic xterm mouse reporting above, main difference is that numbers
     * are printed in base 10 and separated by ;
     *
     * useful because it supports arbitrary large x and y
     */
    bool is_press_or_drag = s[len - 1] == 'M';
    s += 3;
    len -= 3;
    id = xterm_ParseBase10(&s, &len);
    if (is_press_or_drag) {
      buttons = xterm_Id2Buttons(id);
      release_buttons = buttons >= HOLD_WHEEL_REV;
    }

    x = xterm_ParseBase10(&s, &len) - 1;
    y = xterm_ParseBase10(&s, &len) - 1;

  } else if (len == 9 && s[2] == '5' && s[3] == 'M' && (id = (byte)s[4]) >= ' ' &&
             (id -= ' ') <= (HOLD_ANY >> HOLD_BITSHIFT)) {
    /* twterm-style reporting: twin specs, ESC [?999h */
    buttons = id << HOLD_BITSHIFT;

    x = (udat)(((byte)s[5] - '!') & 0x7f) | (udat)((udat)(((byte)s[6] - '!') & 0x7f) << 7);
    if (x & ((udat)1 << 13))
      /* top bit is set, set also higher ones */
      x |= (udat)~0 << 14;

    y = (udat)(((byte)s[7] - '!') & 0x7f) | (udat)((udat)(((byte)s[8] - '!') & 0x7f) << 7);
    if (y & ((udat)1 << 13))
      /* top bit is set, set also higher ones */
      y |= (udat)~0 << 14;
  } else {
    return;
  }

  x = Max2(x, 0);
  x = Min2(x, DisplayWidth - 1);

  y = Max2(y, 0);
  y = Min2(y, DisplayHeight - 1);

  if (x == 0 && prev_x == 0)
    dx = -1;
  else if (x == DisplayWidth - 1 && prev_x == DisplayWidth - 1)
    dx = 1;
  else
    dx = 0;
  if (y == 0 && prev_y == 0)
    dy = -1;
  else if (y == DisplayHeight - 1 && prev_y == DisplayHeight - 1)
    dy = 1;
  else
    dy = 0;

  xterm_prev_x = x;
  xterm_prev_y = y;

  MouseEventCommon(x, y, dx, dy, buttons);
  if (release_buttons) {
    // we never get xterm events 'wheel back/fwd release' => synthetize them
    MouseEventCommon(x, y, 0, 0, 0);
  }
}
