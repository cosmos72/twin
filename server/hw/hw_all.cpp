/*
 *  hw_all.cpp  --  aggregates all functions to let twin display on various hardware
 *
 *  Copyright (C) 1999-2025 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/module.h"
#include "hw/hw_display.h"
#include "hw/hw_tty.h"
#include "hw/hw_twin.h"
#include "hw/hw_x11.h"
#include "hw/hw_xft.h"

/**
 * pretend to dlopen() a hw display driver.
 * Actually only knows about hw display drivers enabled at compile time.
 */
bool DlOpenHW(Tmodule m) {
  const Chars name = m->Name;
#ifdef TWIN_HW_DISPLAY_STATIC
  if (name == Chars("hw_display")) {
    m->DoInit = &display_InitHW;
    return true;
  }
#endif
#ifdef TWIN_HW_TTY_STATIC
  if (name == Chars("hw_tty")) {
    m->DoInit = &tty_InitHW;
    return true;
  }
#endif
#ifdef TWIN_HW_TWIN_STATIC
  if (name == Chars("hw_twin")) {
    m->DoInit = &tw_InitHW;
    return true;
  }
#endif
#ifdef TWIN_HW_X11_STATIC
  if (name == Chars("hw_x11")) {
    m->DoInit = &x11_InitHW;
    return true;
  }
#endif
#ifdef TWIN_HW_XFT_STATIC
  if (name == Chars("hw_xft")) {
    m->DoInit = &xft_InitHW;
    return true;
  }
#endif
  return false;
}