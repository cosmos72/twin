

/*
 * wrapper file for hand-optimized assembler functions for i386 with gcc
 */

#include "asm_i386.h"

#define Tw_ChangeFieldObj Tw_ChangeField

#ifdef TW_HAVE_I386_ASM
# include "libTw2_i386m4.h"
#endif

