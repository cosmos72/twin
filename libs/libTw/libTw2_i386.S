
/* tell includes we are an assebler source, a C source! */
#define __asm



#include "check_asm.h"

/* remove the `Obj' suffix from Tw_ChangeFieldObj() */
#define Tw_ChangeFieldObj Tw_ChangeField


#if defined(CONF__ASM) && defined(TW_HAVE_ASM) && defined(TW_HAVE_GCC_I386_ASM)

 /* backward compatibility. will be removed */
# define Tw_Create4MenuMenuItem _Tw_Create4MenuMenuItem

# include "libTw2_i386_m4.h"

 /* backward compatibility. will be removed */
# undef Tw_Create4MenuMenuItem

#endif /* defined(CONF__ASM) && defined(TW_HAVE_ASM) && defined(TW_HAVE_GCC_I386_ASM) */


