
/* plain Unix-style tty keyboard input */

/* it can't update FullShiftFlags :( */

#include "util.h" /* for SetAlarm(), AlarmReceived */

static bool stdin_TestTty(Tdisplay hw) {
  struct termios ttyb;
  tty_driver *self = ttydriver(hw);
  byte buf[16], *s = buf + 3, c;
  int i, alarmReceived;
  bool ok = true;

  ttyb = ttysave;
  /* NL=='\n'==^J; CR=='\r'==^M */
  ttyb.c_iflag &= ~(IXON | IXOFF | IGNCR | INLCR | ICRNL);
  /*ttyb.c_oflag &= ~OPOST;*/
  ttyb.c_lflag &= ~(ECHO | ICANON);
  ttyb.c_cc[VTIME] = 0;
  ttyb.c_cc[VMIN] = 1;
  /* disable special handling of suspend key (^Z), quit key (^\), break key (^C) */
  ttyb.c_cc[VSUSP] = 0;
  ttyb.c_cc[VQUIT] = 0;
  ttyb.c_cc[VINTR] = 0;
  tty_setioctl(self->tty_fd, &ttyb);

  /* request ID */
  if (write(self->tty_fd, "\033[c", 3) != 3) {
    Errstr = Chars("write() to tty failed");
    ok = false;
  } else {
    /* ensure we CAN read from the tty */
    SetAlarm(5);
    do {
      i = read(self->tty_fd, buf, 15);
    } while ((alarmReceived = AlarmReceived) == 0 && i < 0 &&
             (errno == EWOULDBLOCK || errno == EINTR));
    SetAlarm(0);
    if (i <= 0) {
      Error(SYSERROR);
      if (alarmReceived)
        Errstr = Chars("read() from tty timed out");
      else if (i == 0)
        Errstr = Chars("read() from tty returned END-OF-FILE");
      ok = tfalse;
    }
  }
  if (!ok) {
    tty_setioctl(self->tty_fd, &ttysave);
    return ok;
  }

  buf[i] = '\0';
  self->ttypar[0] = self->ttypar[1] = self->ttypar[2] = i = 0;

  while (i < 3 && (c = *s)) {
    if (c >= '0' && c <= '9') {
      self->ttypar[i] *= 10;
      self->ttypar[i] += c - '0';
    } else if (*s == ';')
      i++;
    else
      break;
    s++;
  }
  return ok;
}

/* return tfalse if failed */
bool tty_driver::stdin_InitKeyboard(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);

  if (self->fnLookupKey == NULL
#if defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
      && self->tty_term != Chars("linux") && self->tty_term != Chars("twterm")
#endif
  )
    return false;

  hw->keyboard_slot =
      RegisterRemote(self->tty_fd, (Tobj)hw, (void (*)(int, Tobj))&tty_driver::stdin_KeyboardEvent);
  if (hw->keyboard_slot == NOSLOT) {
    stdin_QuitKeyboard(hw);
    return false;
  }
  hw->fnKeyboardEvent = &tty_driver::stdin_KeyboardEvent;
  hw->fnQuitKeyboard = &tty_driver::stdin_QuitKeyboard;

  if (self->fnLookupKey == NULL) {
    self->fnLookupKey = linux_LookupKey;
  }
  return true;
}

void tty_driver::stdin_QuitKeyboard(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);

  tty_setioctl(self->tty_fd, &ttysave);

  UnRegisterRemote(hw->keyboard_slot);
  hw->keyboard_slot = NOSLOT;
  hw->fnQuitKeyboard = NULL;
}

/* kludge! this is ok for linux terminals only... */
udat tty_driver::linux_LookupKey(Tdisplay hw, udat *ShiftFlags, byte *slen, char *s, byte *retlen,
                                 const char **ret) {
  byte used = 0, len = *slen;

  *ShiftFlags = 0;

  if (len == 0) {
    *retlen = len;
    return TW_Null;
  }

  *ret = s;

  if (len > 1 && *s == '\x1B') {

    ++used, --len;
    ++s;

    if (len == 1) {
      /* try to handle ALT + <some key> */
      *slen = *retlen = ++used;
      *ShiftFlags = KBD_ALT_FL;
      return (udat)*s;
    }

    switch (*s) {

#define K(sym)                                                                                     \
  *slen = *retlen = ++used;                                                                        \
  return TW_##sym
#define IS(sym, c)                                                                                 \
  case c:                                                                                          \
    K(sym)

    case '[':
      if (++used, --len)
        switch (*++s) {
          /* ESC [ */
          IS(Up, 'A');
          IS(Down, 'B');
          IS(Right, 'C');
          IS(Left, 'D');
          IS(KP_5, 'G'); /* returned when NumLock is off */
          IS(Pause, 'P');

          /* FALLTHROUGH */

        case '[':
          /* ESC [ [ */
          if (++used, --len)
            switch (*++s) {
              IS(F1, 'A');
              IS(F2, 'B');
              IS(F3, 'C');
              IS(F4, 'D');
              IS(F5, 'E');
            }
          break;

        case '1':
          /* ESC [ 1 */
          if (++used, --len) {
            ++s;

            if (len == 1 && *s == '~') {
              /* ESC [ 1 ~ */
              K(Home);
            }
            ++used, --len;
            if (len && s[1] == '~')
              /* ESC [ 1 ? ~ */
              switch (*s) {
                IS(F6, '7');
                IS(F7, '8');
                IS(F8, '9');
              }
          }
          break;

        case '2':
          /* ESC [ 2 */
          if (++used, --len) {
            ++s;

            if (len == 1 && *s == '~') {
              /* ESC 2 ~ */
              K(Insert);
            }
            ++used, --len;
            if (len && s[1] == '~')
              /* ESC [ 2 ? ~ */
              switch (*s) {
                IS(F9, '0');
                IS(F10, '1');
                IS(F11, '3');
                IS(F12, '4');
              }
          }
          break;

        case '3':
          /* ESC [ 3 */
          if (++used, --len)
            switch (*++s) { IS(Delete, '~'); }
          break;

        case '4':
          /* ESC [ 4 */
          if (++used, --len)
            switch (*++s) { IS(End, '~'); }
          break;

        case '5':
          /* ESC [ 5 */
          if (++used, --len)
            switch (*++s) { IS(Prior, '~'); }
          break;

        case '6':
          /* ESC [ 6 */
          if (++used, --len)
            switch (*++s) { IS(Next, '~'); }
          break;
        }
      break;

    case 'O':
      /* ESC O */
      if (++used, --len)
        switch (*++s) {

          /* alternate cursor keys... */
          IS(Up, 'A');
          IS(Down, 'B');
          IS(Right, 'C');
          IS(Left, 'D');
          IS(KP_5, 'G');

          IS(KP_Enter, 'M');
          IS(Num_Lock, 'P');
          IS(KP_Divide, 'Q');
          IS(KP_Multiply, 'R');
          IS(KP_Subtract, 'S');

          IS(KP_Add, 'l');
          IS(KP_Separator, 'm');
          IS(KP_Delete, 'n');
          IS(KP_Equal, 'o');

          IS(KP_0, 'p');
          IS(KP_1, 'q');
          IS(KP_2, 'r');
          IS(KP_3, 's');
          IS(KP_4, 't');
          IS(KP_5, 'u');
          IS(KP_6, 'v');
          IS(KP_7, 'w');
          IS(KP_8, 'x');
          IS(KP_9, 'y');
        }
      break;

    default:
      break;
    }

#undef IS
#undef K
  }

  /* undo the increments above */
  s -= used;
  len += used;
  used = 0;

  *slen = *retlen = ++used;

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

void tty_driver::stdin_KeyboardEvent(int fd, Tdisplay hw) {
  static char buf[TW_SMALLBUFF];
  static fd_set rfds;
  static struct timeval t;
  char *s = buf, *end = buf + sizeof(buf) - 1;
  const char *ret;
  tty_driver *self = ttydriver(hw);
  udat Code, ShiftFlags;
  byte got, chunk, retlen;
  SaveHW;

  SetHW(hw);

  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
  t.tv_sec = 0;
  t.tv_usec = 1000;

  do {
    got = read(fd, s, end - s);

  } while (got > 0 && (s += got) < end && select(fd + 1, &rfds, NULL, NULL, &t) == 1);

  if (got == (byte)-1 && errno != EINTR && errno != EWOULDBLOCK) {
    /* BIG troubles */
    hw->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
    return;
  }

  got = s - buf;
  s = buf;

  while (got > 0) {
    if (hw->fnMouseEvent == &tty_driver::xterm_MouseEvent) {
      while (got) {
        if (got >= 9 && !memcmp(s, "\033[5M", 4)) {
          /* twterm-style mouse reporting: twin specs */
          CopyMem(s, self->xterm_mouse_seq, self->xterm_mouse_len = chunk = 9);
        } else if (got >= 6 && !memcmp(s, "\033[M", 3)) {
          /* classic xterm mouse reporting: X11 specs, ESC [?1002h */
          CopyMem(s, self->xterm_mouse_seq, self->xterm_mouse_len = chunk = 6);
        } else if (got >= 9 && !memcmp(s, "\033[<", 3)) {
          /* enhanced xterm-style reporting: xterm specs, ESC [?1002h ESC [?1006h */
          const char *m = (const char *)memchr(s + 8, 'M', got - 8);
          if (!m) {
            m = (const char *)memchr(s + 8, 'm', got - 8);
            if (!m) {
              break; // termination char 'M' or 'm' not found
            }
          }
          chunk = (m - s) + 1;
          if (chunk > sizeof(self->xterm_mouse_seq)) {
            break; // sequence too long
          }
          CopyMem(s, self->xterm_mouse_seq, self->xterm_mouse_len = chunk);
        } else {
          break;
        }
        s += chunk, got -= chunk;
        xterm_MouseEvent(fd, hw);
      }
    }

    /* ok, now try to find out the correct KeyCode for the ASCII sequence we got */

    if (!(chunk = got)) {
      break;
    }
    ret = end;
    Code = self->fnLookupKey(hw, &ShiftFlags, &chunk, s, &retlen, &ret);
    s += chunk, got -= chunk;

    KeyboardEventCommon(hw, Code, ShiftFlags, retlen, ret);
  }

  RestoreHW;
}
