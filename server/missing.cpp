/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 */

#include <Tw/Twtypes.h>

#ifndef TW_HAVE_GETENV
char *Tw_missing_getenv(const char *name) {
  return NULL;
}
#endif

#ifndef TW_HAVE_MEMCMP
int Tw_missing_memcmp(const void *s1, const void *s2, size_t n) {
  const byte *b1, *b2;

  for (b1 = s1, b2 = s2; n; n--, b1++, b2++) {
    if (*b1 != *b2)
      return *b2 - *b1;
  }
  return 0;
}
#endif

#ifndef TW_HAVE_STRDUP
char *Tw_missing_strdup(const char *s, void *(*missing_malloc)(size_t size)) {
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
size_t Tw_missing_strspn(const char *s, const char *accept) {
  size_t accept_len = strlen(accept);

  if (s)
    while (*s && memchr(accept, *s, accept_len))
      s++;
  return s;
}
#endif

#ifndef TW_HAVE_STRSTR
const char *Tw_missing_strstr(const char *haystack, const char *needle) {
  const char *h;
  size_t needle_len;
  if (!haystack || !needle)
    return haystack;

  needle_len = strlen(needle);
  while (*haystack && (h = strchr(haystack, *needle))) {
    if (!memcmp(h, needle, needle_len))
      return h;
    haystack = h + 1;
  }
  return NULL;
}
#endif

int Tw_option_strcmp(const char *s1, const char *s2) {
  if (s1[0] == '-' && s1[1] == '-' && s1[2])
    s1++;
  if (s2[0] == '-' && s2[1] == '-' && s2[2])
    s2++;
  return strcmp(s1, s2);
}

int Tw_option_strncmp(const char *s1, const char *s2, size_t n) {
  if (n > 2 && s1[0] == '-' && s1[1] == '-' && s1[2])
    n--, s1++;
  if (s2[0] == '-' && s2[1] == '-' && s2[2])
    s2++;
  return strncmp(s1, s2, n);
}

enum {
  utf21_size = 0x110000,
};

trune Tw_trune(tcell cell) {
  cell &= 0x001FFFFF;
  if (cell >= utf21_size) {
    cell -= utf21_size;
  }
  return cell;
}
