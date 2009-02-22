/*
 *  twconfig.h  --  cleanup configuration
 *
 *  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef _TWIN_CONFIG_H
#define _TWIN_CONFIG_H

#ifndef CONF_SOCKET
# undef CONF_TT_HW_TWIN
# undef CONF_HW_TWIN
# undef CONF_HW_DISPLAY
#endif


/*
 * generically disable assembler code
 * if (tany) does not fit a machine register
 * (assumed to be as wide as (void *))
 */
#ifdef CONF__ASM
# if TW_SIZEOFTANY > TW_SIZEOFTOPAQUE
#  undef CONF__ASM
# endif
#endif

/*
 * disable i386 gcc assembler code
 * if (tany) does not fit a i386 machine register (4 bytes)
 */
#ifdef TW_HAVE_GCC_I386_ASM
# if TW_SIZEOFTANY > 4
#  undef TW_HAVE_GCC_I386_ASM
# endif
#endif


#endif /* _TWIN_CONFIG_H */
