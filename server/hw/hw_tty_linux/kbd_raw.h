
/* Linux console keyboard input */

#ifdef TW_HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#include <signal.h>

/* only one display at time can be in raw-keyboard mode... results in much simpler code */
static Tdisplay lrawkbd_hw;

#include "kbd_raw1.h"

static int lrawkbd_mode_save;

/* return tfalse if failed */
TW_ATTR_HIDDEN bool tty_driver::lrawkbdInitKeyboard(Tdisplay hw) {
  struct termios ttyb;

  if (lrawkbd_hw) {
    log(ERROR) << "      lrawkbdInitKeyboard() failed: another display is already using "
                  "raw-keyboard mode.\n";
    return false;
  } else if (!lrawkbdGetKeyboard(hw)) {
    log(ERROR) << "      lrawkbdInitKeyboard() failed: tty is not a Linux console, "
                  "ioctl(KDGKBMODE) failed.\n";
    return false;
  }
  lrawkbd_hw = hw;

  tty_driver *self = ttydriver(hw);
  hw->keyboard_slot = RegisterRemote(self->tty_fd, (Tobj)hw,
                                     (void (*)(int, Tobj))&tty_driver::lrawkbdKeyboardEvent);
  if (hw->keyboard_slot == NOSLOT) {
    return false;
  }

  hw->fnKeyboardEvent = &tty_driver::lrawkbdKeyboardEvent;
  hw->fnConfigureKeyboard = &tty_driver::lrawkbdConfigureKeyboard;
  hw->fnQuitKeyboard = &tty_driver::lrawkbdQuitKeyboard;

  ttyb = ttysave;
  /* NL=='\n'==^J; CR=='\r'==^M */
  ttyb.c_iflag &= ~(IXON | IXOFF | IGNCR | INLCR | ICRNL);
  ttyb.c_cflag |= CS8;
  /*ttyb.c_oflag &= ~OPOST;*/
  ttyb.c_lflag &= ~(ECHO | ICANON);
  ttyb.c_cc[VTIME] = 0;
  ttyb.c_cc[VMIN] = 0;
  /* disable special handling of suspend key (^Z), quit key (^\), break key (^C) */
  ttyb.c_cc[VSUSP] = 0;
  /* set non-blocking mode */
  ttyb.c_cc[VQUIT] = 0;
  ttyb.c_cc[VINTR] = 0;

  if (tty_setioctl(self->tty_fd, &ttyb) < 0) {
    log(ERROR) << "      lrawkbdInitKeyboard() error setting console tty flags: "
#if defined(TCSETS)
                  "ioctl(TCSETS)"
#else
                  "tcsetattr(TCSANOW)"
#endif
                  " failed\n";

    goto failed_ttyioctl;
  }

  lrawkbdInitSignals();
  if (!lrawkbdGrabConsole(hw)) {
    log(ERROR) << "      lrawkbdInitKeyboard() error grabbing console: "
                  "ioctl(VT_SETMODE, VT_PROCESS) failed\n";
    goto failed_grabconsole;
  }
  if (!lrawkbdLoadKeymaps()) {
    log(ERROR) << "      lrawkbdInitKeyboard() error setting unicode keyboard mode: "
                  "ioctl(self->tty_fd, KDSKBMODE, K_UNICODE) failed\n";
    goto failed_loadkeymaps;
  }
  if (!lrawkbdSetKeyboard(hw)) {
    log(ERROR) << "      lrawkbdInitKeyboard() error setting raw keyboard mode: "
                  "ioctl(KDSKBMODE, K_MEDIUMRAW) failed\n";
    goto failed_setkeyboard;
  }
  log(INFO) << "      lrawkbdInitKeyboard() ok: enabled Linux console raw keyboard.\n";
  return true;

failed_setkeyboard:
  lrawkbdFreeKeymaps();

failed_loadkeymaps:
  lrawkbdReleaseConsole(hw);

failed_grabconsole:
  lrawkbdQuitSignals();

failed_ttyioctl:
  UnRegisterRemote(hw->keyboard_slot);
  hw->keyboard_slot = NOSLOT;
  hw->fnQuitKeyboard = NULL;
  lrawkbd_hw = NULL;
  return false;
}

TW_ATTR_HIDDEN void tty_driver::lrawkbdQuitKeyboard(Tdisplay hw) {

  lrawkbdRestoreKeyboard(hw);
  lrawkbdFreeKeymaps();
  lrawkbdReleaseConsole(hw);
  lrawkbdQuitSignals();

  tty_driver *self = ttydriver(hw);
  (void)tty_setioctl(self->tty_fd, &ttysave);

  UnRegisterRemote(hw->keyboard_slot);
  hw->keyboard_slot = NOSLOT;
  hw->fnQuitKeyboard = NULL;

  lrawkbd_hw = NULL;
}

TW_ATTR_HIDDEN void tty_driver::lrawkbdConfigureKeyboard(Tdisplay /*hw*/, udat resource,
                                                         byte todefault, udat value) {
  uldat flag = 0;

  switch (resource) {
  case HW_KBDAPPLIC:
    flag = VC_APPLIC;
    break;
  case HW_ALTCURSKEYS:
    flag = VC_CKMODE;
    break;
  default:
    return;
  }

  if (todefault || !value) {
    clr_vc_kbd_mode(kbd, flag);
  } else {
    set_vc_kbd_mode(kbd, flag);
  }
}

#ifdef DEBUG_HW_TTY_LRAWKBD
static void dump_bytes(byte *s, uldat len) {
  uldat i;
  char c;

  log(INFO) << "lrawkbd: received `";

  for (i = 0; (c = (char)s[i]) && i < len; i++) {
    if (c >= ' ' && c <= '~')
      log(INFO) << Chars(&c, 1);
    else
      log(INFO) << "\\x" << hex((unsigned char)c);
  }
  log(INFO) << "'\n";
}
#endif // DEBUG_HW_TTY_LRAWKBD

TW_ATTR_HIDDEN void tty_driver::lrawkbdKeyboardEvent(int fd, Tdisplay hw) {
  char buf[16], *s, *ret;
  udat Code, ShiftFlags;
  byte got, chunk, retlen;

  got = read(fd, s = buf, 16);

  if (got == 0 || (got == (byte)-1 && errno != EINTR && errno != EWOULDBLOCK)) {
    /* BIG troubles */
    hw->NeedHW |= NeedPanicHW, NeedHW |= NeedPanicHW;
    return;
  }

  while (got > 0) {
    /* ok, now try to find out the correct KeyCode for the scancodes we got */

    chunk = got;

#ifdef DEBUG_HW_TTY_LRAWKBD
    dump_bytes(s, got);
#endif // DEBUG_HW_TTY_LRAWKBD

    Code = lrawkbdLookupKey(&ShiftFlags, &chunk, s, &retlen, &ret);

    if (Code != TW_Null)
      KeyboardEventCommon(hw, Code, ShiftFlags, retlen, ret);

    s += chunk, got -= chunk;
  }
}

TW_ATTR_HIDDEN bool tty_driver::lrawkbdGetKeyboard(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  /* get original (probably XLATE) mode */
  return ioctl(self->tty_fd, KDGKBMODE, &lrawkbd_mode_save) >= 0;
}

TW_ATTR_HIDDEN bool tty_driver::lrawkbdSetKeyboard(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  /* set LED mode */
  char leds = 0;
  (void)ioctl(self->tty_fd, KDGETLED, &leds);
  (void)ioctl(self->tty_fd, KDSETLED, lrawkbd_leds = leds & 7);

  /* set MEDIUMRAW mode */
  return ioctl(self->tty_fd, KDSKBMODE, K_MEDIUMRAW) >= 0;
}

TW_ATTR_HIDDEN void tty_driver::lrawkbdRestoreKeyboard(Tdisplay hw) {
  tty_driver *self = ttydriver(hw);
  /* restore LED mode */
  (void)ioctl(self->tty_fd, KDSETLED, lrawkbd_leds | 8);

  /* restore original (probably XLATE) mode */
  (void)ioctl(self->tty_fd, KDSKBMODE, lrawkbd_mode_save);
}

TW_ATTR_HIDDEN bool tty_driver::lrawkbdGrabConsole(Tdisplay hw) {
  struct vt_mode vt;
#if 0
  struct vt_mode {
    char mode;    /* vt mode */
    char waitv;   /* if set, hang on writes if not active */
    short relsig; /* signal to raise on release req */
    short acqsig; /* signal to raise on acquisition */
    short frsig;  /* unused (set to 0) */
  };
#define VT_GETMODE 0x5601 /* get mode of active vt */
#define VT_SETMODE 0x5602 /* set mode of active vt */
#define VT_AUTO 0x00      /* auto vt switching */
#define VT_PROCESS 0x01   /* process controls switching */
#define VT_ACKACQ 0x02    /* acknowledge switch */
#endif                    /* 0 */
  vt.mode = VT_PROCESS;
  vt.waitv = 0;
  vt.relsig = SIGUSR2;
  vt.acqsig = SIGUSR1;
  vt.frsig = 0;

  tty_driver *self = ttydriver(hw);
  return ioctl(self->tty_fd, VT_SETMODE, &vt) >= 0;
}

TW_ATTR_HIDDEN void tty_driver::lrawkbdReleaseConsole(Tdisplay hw) {
  struct vt_mode vt;

  vt.mode = VT_AUTO;
  vt.waitv = 0;
  vt.relsig = 0;
  vt.acqsig = 0;
  vt.frsig = 0;

  tty_driver *self = ttydriver(hw);
  (void)ioctl(self->tty_fd, VT_SETMODE, &vt);
}

TW_ATTR_HIDDEN void tty_driver::lrawkbdReactSignalOut(int sig) {
  Tdisplay hw = lrawkbd_hw;

  /* we just got SIGUSR1. restore settings and tell kernel we allow the switch */

  signal(sig, &tty_driver::lrawkbdReactSignalOut);

  lrawkbdRestoreKeyboard(hw);

  tty_driver *self = ttydriver(hw);
  ioctl(self->tty_fd, VT_RELDISP, 1);
}

TW_ATTR_HIDDEN void tty_driver::lrawkbdReactSignalIn(int sig) {
  Tdisplay hw = lrawkbd_hw;

  /* we just got SIGUSR2. tell kernel we allow the switch and initialize settings */

  (void)signal(sig, lrawkbdReactSignalIn);

  tty_driver *self = ttydriver(hw);
  (void)ioctl(self->tty_fd, VT_RELDISP, 2);

  lrawkbdSetKeyboard(hw);
}

TW_ATTR_HIDDEN void tty_driver::lrawkbdInitSignals() {
  (void)signal(SIGUSR1, &tty_driver::lrawkbdReactSignalIn);
  (void)signal(SIGUSR2, &tty_driver::lrawkbdReactSignalOut);
}

TW_ATTR_HIDDEN void tty_driver::lrawkbdQuitSignals() {
  (void)signal(SIGUSR1, SIG_DFL);
  (void)signal(SIGUSR2, SIG_DFL);
}
