#ifndef _TW_OSINCLUDES_H
#define _TW_OSINCLUDES_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#ifdef _AIX
# include <sys/select.h>
# include <termio.h>
#endif

#if defined(__sun__) || defined(__SVR4)
# include <sys/filio.h>
#endif

#ifdef __FreeBSD__
# include <sys/types.h>
# include <machine/param.h>
# include <unistd.h>
#endif

#endif /* _TW_OSINCLUDES_H */
