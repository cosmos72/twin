
#include "Tw/Tw.h"

#ifndef TW_HAVE_MEMCMP
int Tw_fix_memcmp(TW_CONST void *s1, TW_CONST void *s2, size_t n) {
    TW_CONST byte *b1, *b2;
    
    for (b1 = s1, b2 = s2; n; n--, b1++, b2++) {
	if (*b1 != *b2)
	    return *b2 - *b1;
    }
    return 0;
}
#endif

#ifndef TW_HAVE_STRDUP
char *Tw_fix_strdup(TW_CONST char *s, void *(*missing_malloc)(size_t size)) {
    char *q;
    size_t len;
    
    if (s) {
	len = 1 + strlen(s);
	if ((q = missing_malloc(len)))
	    memcpy(s, q, len);
	return q;
    }
    return NULL;
}
#endif

#ifndef TW_HAVE_STRSPN
size_t Tw_missing_strspn(TW_CONST char *s, TW_CONST char *accept) {
    if (s) while (*s && strchr(accept, *s))
	s++;
    return s;
}
#endif

#ifndef TW_HAVE_STRSTR
TW_CONST char *Tw_missing_strstr(TW_CONST char *haystack, TW_CONST char *needle) {
    TW_CONST char *h;
    size_t needle_len;
    if (!haystack || !needle)
	return haystack;
    
    needle_len = strlen(needle);
    while (*haystack && (h = strchr(haystack, *needle))) {
	if (!memcmp(h, needle, needle_len))
	    return h;
	haystack = h+1;
    }
    return NULL;
}
#endif
