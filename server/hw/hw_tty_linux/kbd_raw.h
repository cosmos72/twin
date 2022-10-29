
/* Linux console keyboard input */

#ifdef TW_HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#include <signal.h>

#include "kbd_raw1.h"

/* only one display at time can be in raw-keyboard mode... results in much simpler code */
static Tdisplay lrawkbd_HW;

static int lrawkbd_mode_save;

static void lrawkbd_QuitKeyboard(void);
static void lrawkbd_ConfigureKeyboard(udat resource, byte todefault, udat value);

static udat lrawkbd_LookupKey(udat *ShiftFlags, byte *slen, char *s, byte *retlen, char **ret);
static void lrawkbd_KeyboardEvent(int fd, Tdisplay hw);

static bool lrawkbd_GetKeyboard(void);
static bool lrawkbd_SetKeyboard(void);
static void lrawkbd_RestoreKeyboard(void);

static bool lrawkbd_LoadKeymaps(void);
static void lrawkbd_FreeKeymaps(void);

static bool lrawkbd_GrabConsole(void);
static void lrawkbd_ReleaseConsole(void);

static void lrawkbd_InitSignals(void);
static void lrawkbd_QuitSignals(void);

/* return tfalse if failed */
static byte lrawkbd_InitKeyboard(void) {
  struct termios ttyb;

  if (lrawkbd_HW) {
    log(ERROR) << "      lrawkbd_InitKeyboard(): error: another display is already using "
                  "raw-keyboard mode.\n";
    return tfalse;
  } else if (!lrawkbd_GetKeyboard()) {
    log(ERROR) << "      lrawkbd_InitKeyboard(): error: tty is not a Linux console: "
                  "ioctl(KDGKBMODE) failed!\n";
    return tfalse;
  }

  lrawkbd_HW = HW;

  HW->keyboard_slot = RegisterRemote(tty_fd, (obj)HW, (void (*)(int, obj))lrawkbd_KeyboardEvent);
  if (HW->keyboard_slot == NOSLOT)
    return tfalse;

  HW->KeyboardEvent = lrawkbd_KeyboardEvent;
  HW->ConfigureKeyboard = lrawkbd_ConfigureKeyboard;
  HW->QuitKeyboard = lrawkbd_QuitKeyboard;

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

  if (tty_setioctl(tty_fd, &ttyb) < 0) {
    log(ERROR) << "      lrawkbd_InitKeyboard() error setting console tty flags: "
#if defined(TCSETS)
                  "ioctl(TCSETS)"
#else
                  "tcsetattr(TCSANOW)"
#endif
                  " failed\n";

    goto failed_ttyioctl;
  }

  lrawkbd_InitSignals();
  if (!lrawkbd_GrabConsole()) {
    log(ERROR) << "      lrawkbd_InitKeyboard() error grabbing console: "
                  "ioctl(VT_SETMODE, VT_PROCESS) failed\n";
    goto failed_grabconsole;
  }
  if (!lrawkbd_LoadKeymaps()) {
    log(ERROR) << "      lrawkbd_InitKeyboard() error setting unicode keyboard mode: "
                  "ioctl(tty_fd, KDSKBMODE, K_UNICODE) failed\n";
    goto failed_loadkeymaps;
  }
  if (!lrawkbd_SetKeyboard()) {
    log(ERROR) << "      lrawkbd_InitKeyboard() error setting raw keyboard mode: "
                  "ioctl(KDSKBMODE, K_MEDIUMRAW) failed\n";
    goto failed_setkeyboard;
  }
  log(INFO) << "      enabled Linux console raw keyboard mode\n";
  return ttrue;

failed_setkeyboard:
  lrawkbd_FreeKeymaps();

failed_loadkeymaps:
  lrawkbd_ReleaseConsole();

failed_grabconsole:
  lrawkbd_QuitSignals();

failed_ttyioctl:
  UnRegisterRemote(HW->keyboard_slot);
  HW->keyboard_slot = NOSLOT;
  HW->QuitKeyboard = NoOp;
  lrawkbd_HW = NULL;
  return tfalse;
}

static void lrawkbd_QuitKeyboard(void) {

  lrawkbd_RestoreKeyboard();
  lrawkbd_FreeKeymaps();
  lrawkbd_ReleaseConsole();
  lrawkbd_QuitSignals();

  (void)tty_setioctl(tty_fd, &ttysave);

  UnRegisterRemote(HW->keyboard_slot);
  HW->keyboard_slot = NOSLOT;

  HW->QuitKeyboard = NoOp;

  lrawkbd_HW = NULL;
}

static void lrawkbd_ConfigureKeyboard(udat resource, byte todefault, udat value) {
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

  if (todefault || !value)
    clr_vc_kbd_mode(kbd, flag);
  else
    set_vc_kbd_mode(kbd, flag);
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

static void lrawkbd_KeyboardEvent(int fd, Tdisplay hw) {
  char buf[16], *s, *ret;
  udat Code, ShiftFlags;
  byte got, chunk, retlen;
  SaveHW;

  SetHW(hw);

  got = read(fd, s = buf, 16);

  if (got == 0 || (got == (byte)-1 && errno != EINTR && errno != EWOULDBLOCK)) {
    /* BIG troubles */
    HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
    return;
  }

  while (got > 0) {
    /* ok, now try to find out the correct KeyCode for the scancodes we got */

    chunk = got;

#ifdef DEBUG_HW_TTY_LRAWKBD
    dump_bytes(s, got);
#endif // DEBUG_HW_TTY_LRAWKBD

    Code = lrawkbd_LookupKey(&ShiftFlags, &chunk, s, &retlen, &ret);

    if (Code != TW_Null)
      KeyboardEventCommon(Code, ShiftFlags, retlen, ret);

    s += chunk, got -= chunk;
  }

  RestoreHW;
}

static bool lrawkbd_GetKeyboard(void) {
  /* get original (probably XLATE) mode */
  return ioctl(tty_fd, KDGKBMODE, &lrawkbd_mode_save) >= 0;
}

static bool lrawkbd_SetKeyboard(void) {
  /* set LED mode */
  char leds = 0;
  (void)ioctl(tty_fd, KDGETLED, &leds);
  (void)ioctl(tty_fd, KDSETLED, lrawkbd_leds = leds & 7);

  /* set MEDIUMRAW mode */
  return ioctl(tty_fd, KDSKBMODE, K_MEDIUMRAW) >= 0;
}

static void lrawkbd_RestoreKeyboard(void) {
  /* restore LED mode */
  (void)ioctl(tty_fd, KDSETLED, lrawkbd_leds | 8);

  /* restore original (probably XLATE) mode */
  (void)ioctl(tty_fd, KDSKBMODE, lrawkbd_mode_save);
}

static bool lrawkbd_GrabConsole(void) {
  struct vt_mode vt;

#if 0
    struct vt_mode {
        char mode;                /* vt mode */
        char waitv;                /* if set, hang on writes if not active */
        short relsig;                /* signal to raise on release req */
        short acqsig;                /* signal to raise on acquisition */
        short frsig;                /* unused (set to 0) */
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

  return ioctl(tty_fd, VT_SETMODE, &vt) >= 0;
}

static void lrawkbd_ReleaseConsole(void) {
  struct vt_mode vt;

  vt.mode = VT_AUTO;
  vt.waitv = 0;
  vt.relsig = 0;
  vt.acqsig = 0;
  vt.frsig = 0;

  (void)ioctl(tty_fd, VT_SETMODE, &vt);
}

static void lrawkbd_ReactSignalOut(int sig) {
  /* HW may not be set here... use lrawkbd_HW */
  SaveHW;
  SetHW(lrawkbd_HW);

  /* we just got SIGUSR1. restore settings and tell kernel we allow the switch */

  signal(sig, lrawkbd_ReactSignalOut);

  lrawkbd_RestoreKeyboard();

  ioctl(tty_fd, VT_RELDISP, 1);

  RestoreHW;

  TW_RETFROMSIGNAL(0);
}

static void lrawkbd_ReactSignalIn(int sig) {
  /* HW may not be set here... use lrawkbd_HW */
  SaveHW;
  SetHW(lrawkbd_HW);

  /* we just got SIGUSR2. tell kernel we allow the switch and initialize settings */

  (void)signal(sig, lrawkbd_ReactSignalIn);

  (void)ioctl(tty_fd, VT_RELDISP, 2);

  lrawkbd_SetKeyboard();

  RestoreHW;

  TW_RETFROMSIGNAL(0);
}

static void lrawkbd_InitSignals(void) {
  (void)signal(SIGUSR1, lrawkbd_ReactSignalIn);
  (void)signal(SIGUSR2, lrawkbd_ReactSignalOut);
}

static void lrawkbd_QuitSignals(void) {
  (void)signal(SIGUSR1, SIG_DFL);
  (void)signal(SIGUSR2, SIG_DFL);
}
