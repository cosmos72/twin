#ifndef _TWINSYS_H
#define _TWINSYS_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _AIX
# include <sys/select.h>
# include <termio.h>
#endif

#ifdef __FreeBSD__
# include <sys/types.h>
# include <machine/param.h>
# include <unistd.h>
#endif

#endif /* _TWINSYS_H */
