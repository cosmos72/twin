
#ifndef TW_MISSING_H
#define TW_MISSING_H

#include <Tw/prefix.h>
#include <Tw/autoconf.h>

#ifndef TW_HAVE_GETENV
char *Tw_missing_getenv(TW(CONST) char *name);
#define getenv Tw_missing_getenv
#endif

#ifndef TW_HAVE_MEMCMP
int Tw_missing_memcmp(TW(CONST) void *s1, TW(CONST) void *s2, size_t n);
#define memcmp Tw_missing_memcmp
#endif

#ifndef TW_HAVE_STRDUP
char *Tw_missing_strdup(TW(CONST) char *s, void *(*missing_malloc)(size_t size));
#define strdup(s) Tw_missing_strdup((s), Tw(AllocMem))
#endif

#ifndef TW_HAVE_STRERROR
#define strerror(err) ""
#endif

#ifndef TW_HAVE_HSTRERROR
#define hstrerror(herr) ""
#endif

#ifndef TW_HAVE_STRSPN
size_t Tw_missing_strspn(TW(CONST) char *s, TW(CONST) char *accept);
#define strspn Tw_missing_strspn
#endif

#ifndef TW_HAVE_STRSTR
TW(CONST) char *Tw_missing_strstr(TW(CONST) char *haystack, TW(CONST) char *needle);
#define strstr Tw_missing_strstr
#endif

#if defined(TW_HAVE_WAIT3)
#define Tw_wait3 wait3
#elif defined(TW_HAVE_WAIT4)
#define Tw_wait3(status, options, rusage) wait4(-1, status, options, rusage)
#else
#define Tw_wait3(status, options, rusage) ((pid_t)-1)
#endif

int Tw_option_strcmp(TW_CONST char *s1, TW_CONST char *s2);

int Tw_option_strncmp(TW_CONST char *s1, TW_CONST char *s2, size_t n);

#include <Tw/unprefix.h>

#endif /* TW_MISSING_H */
