
#include "log.h"

/*
 * libgpm is stacked, not multi-headed (so no multiplex too)
 */
static bool gpm_used;

/*
 * Linux gpm mouse input:
 * mouse input uses libgpm to connect to `gpm' mouse daemon
 * and read mouse state, but draws mouse pointer manually
 */
TW_ATTR_HIDDEN int tty_driver::gpm_Open(Tdisplay hw) {
  /*
   * HACK! this works around a quirk in libgpm:
   * if Gpm_Open fails, it sets gpm_tried to non-zero
   * and following calls will just fail, not even trying anymore
   */
  extern int gpm_tried;
  tty_driver *self = ttydriver(hw);

  if (!self->tty_name) {
    log(ERROR) << "      gpm_InitMouse() failed: unable to detect tty device\n";
    return NOFD;
  }
  if (self->tty_number < 1 || self->tty_number > 63) {
    log(ERROR) << "      gpm_InitMouse() failed: terminal `" << self->tty_name
               << "'\n      is not a local linux console.\n";
    return NOFD;
  }

  gpm_tried = 0;

  gpm_zerobased = 1;
  gpm_visiblepointer = 0;
  self->gpm_conn.eventMask = ~0;          /* Get everything */
  self->gpm_conn.defaultMask = ~GPM_HARD; /* Pass everything unused */
  self->gpm_conn.minMod = 0;              /* Run always... */
  self->gpm_conn.maxMod = ~0;             /* ...with any modifier */

  self->gpm_fd = Gpm_Open(&self->gpm_conn, self->tty_number);

  if (self->gpm_fd >= 0) {
    /* gpm_consolefd is opened by GPM_Open() */
    fcntl(gpm_consolefd, F_SETFD, FD_CLOEXEC);
    log(INFO) << "      enabled GPM mouse support.\n";
  } else {
    log(ERROR) << "      gpm_InitMouse() failed: unable to connect to `gpm'.\n"
                  "      make sure you started `twin' from the console\n"
                  "      and/or check that `gpm' is running.\n";
  }
  return self->gpm_fd;
}

/* return tfalse if failed */
TW_ATTR_HIDDEN bool tty_driver::gpm_InitMouse(Tdisplay hw) {
  if (gpm_used) {
    log(ERROR) << "      gpm_InitMouse() failed: already connected to `gpm'.\n";
    return false;
  }
  if (gpm_Open(hw) < 0) {
    return false;
  }
  tty_driver *self = ttydriver(hw);

  fcntl(self->gpm_fd, F_SETFD, FD_CLOEXEC);
  fcntl(self->gpm_fd, F_SETFL, O_NONBLOCK);

  hw->mouse_slot = RegisterRemote(self->gpm_fd, (Tobj)hw, (void (*)(int, Tobj))gpm_MouseEvent);
  if (hw->mouse_slot == NOSLOT) {
    Gpm_Close();
    return false;
  }

  hw->FlagsHW |= FlagSoftMouseHW; /* _we_ Hide/Show it */

  hw->fnMouseEvent = gpm_MouseEvent;
  hw->fnConfigureMouse = gpm_ConfigureMouse;
  hw->fnQuitMouse = gpm_QuitMouse;

  gpm_used = true;

  return true;
}

TW_ATTR_HIDDEN void tty_driver::gpm_QuitMouse(Tdisplay hw) {
#if 0
  hw->HideMouse();
#endif
  Gpm_Close();

  UnRegisterRemote(hw->mouse_slot);
  hw->mouse_slot = NOSLOT;

  gpm_used = tfalse;

  hw->fnQuitMouse = NULL;
}

TW_ATTR_HIDDEN void tty_driver::gpm_ConfigureMouse(Tdisplay hw, udat resource, byte todefault,
                                                   udat value) {
  switch (resource) {
  case HW_MOUSEMOTIONEVENTS:
    /* nothing to do */
    break;
  default:
    break;
  }
}

TW_ATTR_HIDDEN void tty_driver::gpm_MouseEvent(int fd, Tdisplay hw) {
  Gpm_Event ev;
  tty_driver *self = ttydriver(hw);
  int left = 0;
  udat IdButtons, Buttons = 0;

  /*
   * All other parts of twin read and parse data from fds in big chunks,
   * while Gpm_GetEvent() reads and parses only a single event at time.
   * To compensate this and avoid mouse to lag behind, we do a small loop.
   */
  byte loopN = 30;

  do {
    if ((left = Gpm_GetEvent(&ev)) <= 0) {
      if (loopN == 30) {
        log(ERROR) << "gpm_MouseEvent(): connection to gpm lost. Continuing without mouse :-(\n";
        hw->QuitMouse();
        null_InitMouse(hw);
      }
      break;
    }

#if 0
        All->FullShiftFlags =
            (ev.modifiers & 1 ? FULL_LEFT_SHIFT_PRESSED : 0)
            | (ev.modifiers & 2 ? FULL_RIGHT_ALT_PRESSED  : 0)
            | (ev.modifiers & 4 ? FULL_LEFT_CTRL_PRESSED  : 0)
            | (ev.modifiers & 8 ? FULL_LEFT_ALT_PRESSED   : 0);
#endif

    /*
     * Gpm differs from us about what buttons to report on release events:
     * it reports which buttons get _released_, not which are still _pressed_
     * Fixed here. SIGH.
     */
    IdButtons = ev.buttons;

    if (ev.type & GPM_UP) {
      IdButtons = self->gpm_keys & ~IdButtons;
    }
    self->gpm_keys = IdButtons;

    Buttons |= (IdButtons & GPM_B_LEFT ? HOLD_LEFT : 0) |
               (IdButtons & GPM_B_MIDDLE ? HOLD_MIDDLE : 0) |
               (IdButtons & GPM_B_RIGHT ? HOLD_RIGHT : 0) |
#if defined(GPM_B_UP)
               (IdButtons & GPM_B_UP ? HOLD_WHEEL_REV : 0) |
#endif
#if defined(GPM_B_DOWN)
               (IdButtons & GPM_B_DOWN ? HOLD_WHEEL_FWD : 0) |
#endif
               0;

#ifdef TW_HAVE_STRUCT_GPM_EVENT_WDY
    if (ev.wdy != 0) {
      while (ev.wdy > 0) {
        MouseEventCommon(hw, ev.x, ev.y, ev.dx, ev.dy, Buttons | HOLD_WHEEL_REV);
        MouseEventCommon(hw, ev.x, ev.y, 0, 0, Buttons);
        ev.dx = ev.dy = 0;
        ev.wdy--;
      }
      while (ev.wdy < 0) {
        MouseEventCommon(hw, ev.x, ev.y, ev.dx, ev.dy, Buttons | HOLD_WHEEL_FWD);
        MouseEventCommon(hw, ev.x, ev.y, 0, 0, Buttons);
        ev.wdy++;
        ev.dx = ev.dy = 0;
      }
    } else
#endif
      MouseEventCommon(hw, ev.x, ev.y, ev.dx, ev.dy, Buttons);

  } while (loopN-- && ioctl(self->gpm_fd, FIONREAD, &left) >= 0 && left > 0);
}
