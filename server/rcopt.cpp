/*
 *  rcrun.c  --  virtual machine to run byte-compiled configuration file ~/.config/twin/twinrc
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "data.h"

static byte strfuzzy_ends_with(const char *s1, const char *s2) {
  const char *p1 = s1 + strlen(s1), *p2 = s2 + strlen(s2);
  char c1, c2;

  while (p1 != s1 && p2 != s2) {
    do {
      c1 = *--p1;
    } while (p1 != s1 && (c1 == ' ' || c1 == '-' || c1 == '_' || c1 == '.' || c1 == ':'));

    do {
      c2 = *--p2;
    } while (p2 != s2 && (c2 == ' ' || c2 == '-' || c2 == '_' || c2 == '.' || c2 == ':'));

    if (c1 >= 'A' && c1 <= 'Z')
      c1 += 'a' - 'A';

    if (c2 >= 'A' && c2 <= 'Z')
      c2 += 'a' - 'A';

    if (c1 != c2)
      return tfalse;
  }
  return p2 == s2;
}

byte InitRCOptions(void) {
  /*
     if any of these environment variables ends with "UTF-8" or similar
     then initialize tty emulator in UTF-8 mode
   */
  const char *keys[] = {
      "LANG",    "LANGUAGE",   "LC_ALL",       "LC_CTYPE",       "LC_NUMERIC",
      "LC_TIME", "LC_COLLATE", "LC_MONETARY",  "LC_MESSAGES",    "LC_PAPER",
      "LC_NAME", "LC_ADDRESS", "LC_TELEPHONE", "LC_MEASUREMENT", "LC_IDENTIFICATION",
  };
  const char *env;
  size_t i;
  byte utf8 = tfalse;
  for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
    if ((env = getenv(keys[i])) && *env) {
      if (strfuzzy_ends_with(env, "utf8")) {
        utf8 = ttrue;
        break;
      }
    }
  }

  All->SetUp->Flags = (utf8 ? setup_terminals_utf8 : 0)
#ifdef CONF_OPT_SHADOWS
                      | setup_shadows
#endif
#ifdef CONF_OPT_BLINK
                      | setup_blink
#endif
#ifdef CONF_OPT_CURSOR_ALWAYS
                      | setup_cursor_always
#endif
#ifdef CONF_OPT_MENU_HIDE
                      | setup_menu_hide
#endif
#ifdef CONF_OPT_MENU_RELAX
                      | setup_menu_relax
#endif
#ifdef CONF_OPT_MENU_INFO
                      | setup_menu_info
#endif
#ifdef CONF_OPT_SCREEN_SCROLL
                      | setup_screen_scroll
#endif
      ;

  return ttrue;
}
