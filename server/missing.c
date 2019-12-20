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
char *Tw_missing_getenv(TW_CONST char *name) { return NULL; }
#endif

#ifndef TW_HAVE_MEMCMP
int Tw_missing_memcmp(TW_CONST void *s1, TW_CONST void *s2, size_t n) {
  TW_CONST byte *b1, *b2;

  for (b1 = s1, b2 = s2; n; n--, b1++, b2++) {
    if (*b1 != *b2)
      return *b2 - *b1;
  }
  return 0;
}
#endif

#ifndef TW_HAVE_STRDUP
char *Tw_missing_strdup(TW_CONST char *s, void *(*missing_malloc)(size_t size)) {
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
  size_t accept_len = strlen(accept);

  if (s)
    while (*s && memchr(accept, *s, accept_len))
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
    haystack = h + 1;
  }
  return NULL;
}
#endif

int Tw_option_strcmp(TW_CONST char *s1, TW_CONST char *s2) {
  if (s1[0] == '-' && s1[1] == '-' && s1[2])
    s1++;
  if (s2[0] == '-' && s2[1] == '-' && s2[2])
    s2++;
  return strcmp(s1, s2);
}

int Tw_option_strncmp(TW_CONST char *s1, TW_CONST char *s2, size_t n) {
  if (n > 2 && s1[0] == '-' && s1[1] == '-' && s1[2])
    n--, s1++;
  if (s2[0] == '-' && s2[1] == '-' && s2[2])
    s2++;
  return strncmp(s1, s2, n);
}

enum {
  pitch = 15,
  menu_bar = 1,
  window_title = 4 * pitch + 4,         /* == 64 */
  window_title_focus = 5 * pitch + 4,   /* == 79 */
  window_title_pressed = 6 * pitch + 4, /* == 94 */
};
enum {
  utf16_replacement_char = 0xFFFD,
  utf16_size = 0x10000,
  utf21_size = 0x110000,
  extra_flag = 1 << 23,
};

hwattr Tw_hwattr3(hwcol col, hwfont font, hwattr extra) {
  hwattr attr;
  if (font >= utf21_size)
    font = utf16_replacement_char;
  switch (extra) {
  case 0:
    return HWATTR(col, font);
  case menu_bar:
    attr = 1;
    break;
  case window_title:
    attr = 2;
    break;
  case window_title_focus:
    attr = 3;
    break;
  case window_title_pressed:
    attr = 4;
    break;
  default:
    if (font >= utf16_size)
      /*
       * window borders support only the first 64k unicode characters:
       * not enough bits for full unicode support in this case...
       */
      font = utf16_replacement_char;
    return HWATTR(col, font) | (extra << 16) | extra_flag;
  }
  return HWATTR(col, attr * utf21_size + font);
}

hwfont Tw_hwfont(hwattr attr) {
  attr &= 0x00FFFFFF;
  if (attr & extra_flag)
    return attr & 0xFFFF;
  return attr % utf21_size;
}

static TW_CONST byte decode_hwextra[5] = {
    0, menu_bar, window_title, window_title_focus, window_title_pressed,
};

hwattr Tw_hwextra(hwattr attr) {
  byte i;
  attr &= 0x00FFFFFF;
  if (attr & extra_flag)
    return (attr >> 16) & 0x7F;
  i = attr / utf21_size;
  return decode_hwextra[i];
}
