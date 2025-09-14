/*
 *  rcrun.c  --  virtual machine to run byte-compiled configuration file ~/.config/twin/twinrc
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "data.h"

byte InitRCOptions(void) {
  /*
   * by default, new terminals start in UTF-8 mode.
   * To disable it, add a line
   *   GlobalFlags -TerminalsUtf8
   * to your ~/.config/twin/twinrc
   */

  All->SetUp->Flags = setup_terminals_utf8
#ifdef CONF_OPT_SHADOWS
                      | setup_shadows
#endif
#ifdef CONF_OPT_CURSOR_ALWAYS
                      | setup_cursor_always
#endif
#ifdef CONF_OPT_MENU_HIDE
                      | setup_menu_hide
#endif
#ifdef CONF_OPT_MENU_RELAX
                      | setup_menu_relax
#endif
#ifdef CONF_OPT_MENU_INFO
                      | setup_menu_info
#endif
#ifdef CONF_OPT_SCREEN_SCROLL
                      | setup_screen_scroll
#endif
      ;

  return ttrue;
}
