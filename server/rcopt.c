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
#ifdef CONF_OPT_ALWAYSCURSOR
	| SETUP_ALWAYSCURSOR
#endif
#ifdef CONF_OPT_HIDEMENU
	| SETUP_HIDEMENU
#endif
#ifdef CONF_OPT_MENUINFO
	| SETUP_MENUINFO
#endif
#ifdef CONF_OPT_EDGESCROLL
	| SETUP_EDGESCROLL
#endif
#ifdef CONF_OPT_ALTFONT
	| SETUP_ALTFONT
#endif
	;

    return TRUE;
}

