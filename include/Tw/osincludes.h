#ifndef _TW_OSINCLUDES_H
#define _TW_OSINCLUDES_H

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#ifdef TW_HAVE_FCNTL_H
# include <fcntl.h>
#endif

#ifdef TW_HAVE_LIMITS_H
# include <limits.h>
#endif

#ifdef TW_HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef TW_HAVE_MACHINE_PARAM_H
# include <machine/param.h>
#endif

#ifdef TW_HAVE_SYS_FILIO_H
# include <sys/filio.h>
#endif

#ifdef TW_HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#ifdef TW_TIME_WITH_SYS_TIME
# include <sys/time.h>
#endif

#endif /* _TW_OSINCLUDES_H */
