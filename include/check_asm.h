/*
 *  check_asm.h  --  cleanup CONF_ASM configuration based on hardware constraints
 *
 *  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef _TWIN_CHECK_ASM_H
#define _TWIN_CHECK_ASM_H

#include <Tw/autoconf.h>
#include <Tw/datasizes.h>

/*
 * disable i386 gcc assembler code
 * if (tany) does not fit a i386 machine register (4 bytes)
 */
# ifdef TW_HAVE_ASM
#  ifdef TW_HAVE_GCC_I386_ASM
#   if TW_SIZEOF_TANY > 4
#    undef TW_HAVE_GCC_I386_ASM
#    undef TW_HAVE_ASM
#    warning gcc-i386 assembler support disabled because sizeof(tany) > 4
#   endif
#  endif
# endif


/*
 * generically disable assembler code
 * if (tany) does not fit a machine register
 * (assumed to be as wide as (void *))
 */
# ifdef TW_HAVE_ASM
#  if TW_SIZEOF_TANY_T > TW_SIZEOF_VOID_P
#   undef TW_HAVE_ASM
#   warning assembler support disabled because sizeof(tany) > sizeof(void *)
#  endif
# endif


/*
 * generically disable assembler code
 * if (tany) does not match size of (int).
 * This is needed to rely on default argument promotion
 * (which passes char and short args as int)
 * to pass all function arguments as (tany)
 */
# ifdef TW_HAVE_ASM
#  if TW_SIZEOF_TANY != TW_SIZEOF_INT
#   undef TW_HAVE_ASM
#   warning assembler support disabled because sizeof(tany) != sizeof(int)
#  endif
# endif


#endif /* _TWIN_CONFIG_H */

