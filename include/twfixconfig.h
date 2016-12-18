/*
 *  twfixconfig.h  --  cleanup configuration
 *
 *  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef _TWFIXCONFIG_H
#define _TWFIXCONFIG_H

/*
 * disable i386 gcc assembler code
 * if (tany) does not fit a i386 machine register (4 bytes)
 */
#ifdef TW_HAVE_GCC_I386_ASM
#  if TW_SIZEOF_TANY > 4
#    warning gcc-compatible i386 assembler cannot be used, disabling it. reason: type 'tany' does not fit a machine register (4 bytes)
#    undef CONF__ASM
#    undef TW_HAVE_GCC_I386_ASM
#   endif
#endif

#endif /* _TWFIXCONFIG_H */
