
#include <Tw/Twautoconf.h>
#include "check_asm.h"

/* remove the `Obj' suffix from Tw_ChangeFieldObj() */
#define Tw_ChangeFieldObj Tw_ChangeField

#ifdef TW_HAVE_GCC_I386_ASM
# include "libTw2_i386_m4.h"
#endif

