#ifndef _TW_OSINCLUDES_H
#define _TW_OSINCLUDES_H

#include <errno.h>

#ifdef TW_HAVE_STDDEF_H
# include <stddef.h>
#endif

#include <stdio.h>

#ifdef TW_HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef TW_HAVE_STDINT_H
# include <stdint.h>
#endif

#ifdef TW_HAVE_STDARG_H
# include <stdarg.h>
#endif

#ifdef TW_HAVE_STRING_H
# include <string.h>
#endif

#include <time.h>
#include <sys/types.h>

#ifdef TW_HAVE_LIMITS_H
# include <limits.h>
#endif

#ifdef TW_HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef TW_TIME_WITH_SYS_TIME
# include <sys/time.h>
#endif

#endif /* _TW_OSINCLUDES_H */
