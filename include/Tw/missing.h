
#ifndef _TWIN_MISSING_H
#define _TWIN_MISSING_H

#include "Tw/prefix.h"

#ifndef TW_RETSIGTYPE
# include "Tw/Twautoconf.h"
#endif

#ifndef TW_HAVE_MEMCMP
int Tw_missing_memcmp(TW(CONST) void *s1, TW(CONST) void *s2, size_t n);
# define memcmp Tw_missing_memcmp
#endif

#ifdef TW_SETVBUF_REVERSED
# define setvbuf(stream, buf, mode, size) setvbuf(stream, mode, buf, size)
#endif

#ifndef TW_HAVE_STRDUP
char *Tw_missing_strdup(TW(CONST) char *s, void *(*missing_malloc)(size_t size));
# define strdup(s) Tw_missing_strdup((s), Tw(AllocMem))
#endif

#ifndef TW_HAVE_STRERROR
# define strerror(errnum) ""
#endif

#ifndef TW_HAVE_HSTRERROR
# define hstrerror(herrnum) ""
#endif

#ifndef TW_HAVE_STRSPN
size_t Tw_missing_strspn(TW(CONST) char *s, TW(CONST) char *accept);
# define strspn(s, accept) Tw_missing_strspn(s, accept)
#endif

#ifndef TW_HAVE_STRSTR
TW(CONST) char *Tw_missing_strstr(TW(CONST) char *haystack, TW(CONST) char *needle);
# define strstr(haystack, needle) Tw_missing_strstr(haystack, needle)
#endif

#ifndef TW_HAVE_WAIT3
# ifdef TW_HAVE_WAIT4
#  define wait3(status, options, rusage) wait4(-1, status, options, rusage)
# else
#  define wait3(status, options, rusage) ((pid_t)-1)
# endif
#endif

#include "Tw/unprefix.h"

#endif /* _TWIN_MISSING_H */

