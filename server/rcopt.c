/*
 *  rcrun.c  --  virtual machine to run byte-compiled ~/.twinrc code
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
    All->SetUp->Flags = 0
#ifdef CONF_OPT_SHADOWS
	| SETUP_SHADOWS
#endif
#ifdef CONF_OPT_BLINK
	| SETUP_BLINK
#endif
#ifdef CONF_OPT_CURSOR_ALWAYS
	| SETUP_CURSOR_ALWAYS
#endif
#ifdef CONF_OPT_MENU_HIDE
	| SETUP_MENU_HIDE
#endif
#ifdef CONF_OPT_MENU_RELAX
	| SETUP_MENU_RELAX
#endif
#ifdef CONF_OPT_MENU_INFO
	| SETUP_MENU_INFO
#endif
#ifdef CONF_OPT_SCREEN_SCROLL
	| SETUP_SCREEN_SCROLL
#endif
	;

    return TRUE;
}

