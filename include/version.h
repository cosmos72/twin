
#ifndef _TWIN_VERSION_H
#define _TWIN_VERSION_H

#if !defined(CAT)
# define _CAT(a,b) a##b
# define CAT(a,b) _CAT(a,b)
#endif

#if !defined(STR)
# define _STR(s) #s
# define STR(s) _STR(s)
#endif

#define TWIN_VERSION_MAJOR 0
#define TWIN_VERSION_MINOR 4
#define TWIN_VERSION_PATCH 6
#define TWIN_VERSION_EXTRA 0
#define TWIN_VERSION       ((TWIN_VERSION_MAJOR<<16)|(TWIN_VERSION_MINOR<<8)|TWIN_VERSION_PATCH)
#define TWIN_VERSION_STR   STR(TWIN_VERSION_MAJOR) "." STR(TWIN_VERSION_MINOR) "." STR(TWIN_VERSION_PATCH)

#if TWIN_VERSION_EXTRA != 0
# define TWIN_VERSION_EXTRA_STR "_" TW_STR(TWIN_VERSION_EXTRA)
#else
# define TWIN_VERSION_EXTRA_STR ""
#endif

#endif /* _TWIN_VERSION_H */

