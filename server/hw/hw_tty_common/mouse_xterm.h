
/*
 * xterm-style mouse input
 *
 * used to let twin run inside twin terminal, xterm, etc.
 *
 * it is always compiled in.
 */

/* return false if failed */
TW_ATTR_HIDDEN bool tty_driver::xtermInitMouse(Tdisplay hw, byte force) {
  tty_driver *self = ttydriver(hw);
  Chars term = self->tty_term; // guaranteed to be '\0' terminated

  if (force == ttrue) {
    log(WARNING) << "      xtermInitMouse(): xterm-style mouse FORCED.\n"
                    "      Assuming terminal has xterm compatible mouse reporting.\n";
    term = Chars("xterm");
  } else if (force == ttrue + ttrue) {
    log(WARNING) << "      xtermInitMouse(): twterm-style mouse FORCED.\n"
                    "      Assuming terminal has twterm compatible mouse reporting.\n";
    term = Chars("twterm");
  }

  if (term.empty()) {
    log(ERROR) << "      xtermInitMouse() failed: unknown terminal type.\n";
    return false;
  }

  self->mouse_start_seq = "\033[?9h";
  self->mouse_end_seq = "\033[?9l";
  self->mouse_motion_seq = "\033[?999h";

  if (term == Chars("twterm")) {
    ;
  } else if (term == Chars("linux")) {
    /*
     * additional check: out-of-the box linux
     * doesn't have twterm-style mouse reporting
     */
    if (self->ttypar[0] < 6 || (self->ttypar[0] == 6 && self->ttypar[1] < 3)) {
      log(ERROR) << "      xtermInitMouse() failed: this `linux' terminal\n"
                    "      has no support for xterm-style mouse reporting.\n";
      return false;
    }
    if (self->ttypar[0] == 6 && self->ttypar[1] < 4) {
      log(WARNING) << "      xtermInitMouse() warning: this `linux' terminal\n"
                      "      can only report click, drag and release, not motion.\n";
      self->mouse_motion_seq = self->mouse_start_seq;
    }
  } else if (term.starts_with(Chars("xterm")) || term.starts_with(Chars("mlterm")) ||
             term.starts_with(Chars("rxvt")) || term.starts_with(Chars("alacritty")) ||
             term.starts_with(Chars("Eterm"))) {
    /* try to enable more common protocols first,
     * then try to enable progressively more complete protocols */
    self->mouse_start_seq = "\033[?1001s\033[?1000h\033[?1002h\033[?1006h";
    self->mouse_end_seq = "\033[?1006l\033[?1002l\033[?1000l\033[?1001r";
    self->mouse_motion_seq = self->mouse_start_seq;
  } else {
    log(ERROR) << "      xtermInitMouse() failed: terminal `" << term << "' is not supported.\n";
    return false;
  }

  fputs(self->mouse_start_seq, self->out);
  hw->setFlush();

  hw->mouse_slot = NOSLOT; /* shared with keyboard */
  hw->fnMouseEvent = &tty_driver::xtermMouseEvent;
  hw->fnConfigureMouse = &tty_driver::xtermConfigureMouse;
  hw->fnQuitMouse = &tty_driver::xtermQuitMouse;
  hw->FlagsHW &= ~FlagSoftMouseHW; /* no need to Hide/Show it */
                                   /* override the ones set by InitVideo() */
  hw->fnShowMouse = hw->fnHideMouse = NULL;

  log(INFO) << "     xtermInitMouse() ok: enabled xterm mouse support.\n";
  return true;
}

TW_ATTR_HIDDEN void tty_driver::xtermQuitMouse(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  fputs(self->mouse_end_seq, self->out);
  hw->fnQuitMouse = NULL;
}

TW_ATTR_HIDDEN void tty_driver::xtermConfigureMouse(Tdisplay hw, udat resource, byte todefault,
                                                    udat value) {
  tty_driver *self = ttydriver(hw);
  switch (resource) {
  case HW_MOUSEMOTIONEVENTS:
    /* either enable new style + mouse motion, or switch back to new style */
    fputs(value && !todefault ? self->mouse_motion_seq : self->mouse_start_seq, self->out);
    hw->setFlush();
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

static udat xtermParseBase10(const char **s_ptr, byte *len_ptr) {
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

TW_ATTR_HIDDEN void tty_driver::xtermMouseEvent(int fd, Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  const char *s = self->xterm_mouse_seq;
  udat buttons = 0, id;
  dat x, y, dx, dy;
  dat prev_x = self->xterm_prev_x, prev_y = self->xterm_prev_y;
  byte len = self->xterm_mouse_len;
  bool release_buttons = false;

  if (s[0] != '\033' || s[1] != '[') {
    return;
  }
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
    id = xtermParseBase10(&s, &len);
    if (is_press_or_drag) {
      buttons = xterm_Id2Buttons(id);
      release_buttons = buttons >= HOLD_WHEEL_REV;
    }

    x = xtermParseBase10(&s, &len) - 1;
    y = xtermParseBase10(&s, &len) - 1;

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

  self->xterm_prev_x = x;
  self->xterm_prev_y = y;

  MouseEventCommon(hw, x, y, dx, dy, buttons);
  if (release_buttons) {
    // we never get xterm events 'wheel back/fwd release' => synthetize them
    MouseEventCommon(hw, x, y, 0, 0, 0);
  }
}
