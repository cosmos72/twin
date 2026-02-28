/*
 *  data.c  --  create fundamental structures:
 *              `All', screens, table for mouse/keyboard actions
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "methods.h"
#include "obj/id.h"
#include "data.h"
#include "util.h"
#include "ini.h"
#include "log.h"
#include "main.h" // HOME

#include <Tw/Twkeys.h>
#include <Tutf/Tutf.h>

/* setup configuration paths */

#ifndef PLUGINDIR
#warning PLUGINDIR is not #defined, assuming "/usr/local/lib/twin"
#define PLUGINDIR "/usr/local/lib/twin"
#endif
#ifndef CONFDIR
#warning CONFDIR is not #defined, assuming "/usr/local/etc/twin"
#define CONFDIR "/usr/local/etc/twin"
#endif

Chars plugindir = PLUGINDIR;
Chars confdir = CONFDIR;

/* First, some structures */

keylist TW_KeyList[] = {
#define IS(key, len, seq) {#key, TW_##key, len, seq},
#include "hw_keys.h"
#undef IS
    {NULL, TW_Null, 0, NULL}};

trune GadgetResize[2] = {0xCD, 0xBC};
trune ScrollBarX[3] = {0xB1, 0x11, 0x10};
trune ScrollBarY[3] = {0xB1, 0x1E, 0x1F};
trune TabX = 0xDB;
trune TabY = 0xDB;
trune StdBorder[2][9] = {{0xC9, 0xCD, 0xBB, 0xBA, 0x20, 0xBA, 0xC8, 0xCD, 0xBC},
                         {0xDA, 0xC4, 0xBF, 0xB3, 0x20, 0xB3, 0xC0, 0xC4, 0xD9}};
trune Screen_Back[2] = {0x12, 0x12};

ColorTheme::ColorTheme() {
  for (unsigned i = 0; i < N_OF(vec); i++) {
    vec[i] = TCOL0;
  }
}

ColorTheme::ColorTheme(tcolor gadgets, tcolor arrows, tcolor bars, tcolor tabs, tcolor borders,
                       tcolor text, tcolor selected, tcolor disabled, tcolor selectdisabled) {
  Gadgets() = gadgets;
  Arrows() = arrows;
  Bars() = bars;
  Tabs() = tabs;
  Borders() = borders;
  Text() = text;
  Selected() = selected;
  Disabled() = disabled;
  SelectDisabled() = selectdisabled;
}

ColorTheme DefaultTheme(TCOL(tYELLOW, tcyan),  // Gadgets
                        TCOL(tGREEN, tBLUE),   // Arrows
                        TCOL(twhite, tBLUE),   // Bars
                        TCOL(tWHITE, tBLUE),   // Tabs
                        TCOL(tWHITE, tBLUE),   // Borders
                        TCOL(twhite, tblue),   // Text
                        TCOL(tBLUE, twhite),   // Selected
                        TCOL(tBLACK, tblack),  // Disabled
                        TCOL(tblack, tBLACK)); // SelectDisabled

ColorTheme MenuTheme(TCOL(tGREEN, twhite),  // Gadgets
                     TCOL(tWHITE, twhite),  // Arrows
                     TCOL(tBLUE, twhite),   // Bars
                     TCOL(tBLUE, tWHITE),   // Tabs
                     TCOL(tWHITE, twhite),  // Borders
                     TCOL(tblack, twhite),  // Text
                     TCOL(tblack, tgreen),  // Selected
                     TCOL(tBLACK, twhite),  // Disabled
                     TCOL(tBLACK, tblack)); // SelectDisabled

static const Chars color_keys[] = {
    Chars("Gadgets"),  Chars("Arrows"),   Chars("Bars"),
    Chars("Tabs"),     Chars("Borders"),  Chars("Text"),
    Chars("Selected"), Chars("Disabled"), Chars("SelectDisabled"),
};

enum e_color_err {
  e_color_ok = 0,
  e_color_badkey = 1,
  e_color_badvalue = 2,
};

enum e_ini_section {
  e_section_unknown = 0,
  e_section_window_colors = 1,
  e_section_menuwindow_colors = 2,
};

static bool ParseHex(Chars str, uint32_t &out) {
  size_t i, n;
  uint32_t val = 0, digit;
  for (i = 0, n = str.size(); i < n; i++) {
    char ch = str[i];
    if (ch >= '0' && ch <= '9') {
      digit = ch - '0';
    } else if (ch >= 'A' && ch <= 'F') {
      digit = ch - 'A' + 10;
    } else if (ch >= 'a' && ch <= 'f') {
      digit = ch - 'a' + 10;
    } else {
      return false;
    }
    val = (val << 4) | digit;
  }
  out = val;
  return true;
}

static e_color_err ParseRgbHex4(Chars str, trgb &rgb) {
  uint32_t n;
  str.pop_front(); // remove inital '#'
  if (!ParseHex(str, n)) {
    return e_color_badvalue;
  }
  byte r = 0x11 * ((n >> 16) & 0xF);
  byte g = 0x11 * ((n >> 8) & 0xF);
  byte b = 0x11 * ((n >> 0) & 0xF);
  rgb = TRGB(r, g, b);
  return e_color_ok;
}

static e_color_err ParseRgbHex7(Chars str, trgb &rgb) {
  uint32_t n;
  str.pop_front(); // remove inital '#'
  if (!ParseHex(str, n)) {
    return e_color_badvalue;
  }
  byte r = ((n >> 16) & 0xFF);
  byte g = ((n >> 8) & 0xFF);
  byte b = ((n >> 0) & 0xFF);
  rgb = TRGB(r, g, b);
  return e_color_ok;
}

static e_color_err ParseRgbHex(Chars str, trgb &rgb) {
  switch (str.size()) {
  case 4:
    return ParseRgbHex4(str, rgb);
  case 7:
    return ParseRgbHex7(str, rgb);
  default:
    return e_color_badvalue;
  }
}

static e_color_err ParseColorHex(Chars str, tcolor &col) {
  size_t comma = str.find(Chars(","));
  if (comma == size_t(-1)) {
    return e_color_badvalue;
  }
  Chars fg = str.view(0, comma).trim();
  Chars bg = str.view(comma + 1, str.size()).trim();
  if (!fg.starts_with(Chars("#")) || !bg.starts_with(Chars("#"))) {
    return e_color_badvalue;
  }
  e_color_err err = ParseRgbHex(fg, col.fg);
  if (err == e_color_ok) {
    err = ParseRgbHex(bg, col.bg);
  }
  return err;
}

static e_color_err SetThemeColorByName(ColorTheme &theme, Chars key, Chars value) {
  for (unsigned i = 0; i < N_OF(color_keys); i++) {
    if (key == color_keys[i]) {
      return ParseColorHex(value, theme[i]);
    }
  }
  return e_color_badkey;
}

static Chars SectionName(e_ini_section section) {
  switch (section) {
  case e_section_unknown:
  default:
    return Chars("???");
  case e_section_window_colors:
    return Chars("WindowColors");
  case e_section_menuwindow_colors:
    return Chars("MenuWindowColors");
  }
}

static void LogColorError(Chars path, Ini &ini, e_ini_section section, e_color_err err) {
  switch (err) {
  case e_color_ok:
  default:
    break;
  case e_color_badkey:
    log(ERROR) << "unknown key [" << SectionName(section) << "]:" << ini.key() << " in file "
               << path << ":" << ini.linenum() << "\n";
    break;
  case e_color_badvalue:
    log(ERROR) << "invalid color " << ini.value() << " for key [" << SectionName(section)
               << "]:" << ini.key() << " in file " << path << ":" << ini.linenum() << "\n";
    break;
  }
}

static bool LoadTheme(Ini &ini, Chars path) {
  e_ini_section section = e_section_unknown;
  for (;;) {
    const IniToken token = ini.next();
    switch (token) {
    case INI_SECTION:
      // log(INFO) << "theme [" << ini.section() << "]\n";
      if (ini.section() == Chars("WindowColors")) {
        section = e_section_window_colors;
      } else if (ini.section() == Chars("MenuWindowColors")) {
        section = e_section_menuwindow_colors;
      } else {
        section = e_section_unknown;
      }
      break;
    case INI_KEY_VALUE:
      // log(INFO) << "theme " << ini.key() << " " << ini.value() << "\n";
      if (section != e_section_unknown) {
        ColorTheme &theme = section == e_section_menuwindow_colors ? MenuTheme : DefaultTheme;
        e_color_err err = SetThemeColorByName(theme, ini.key(), ini.value());
        LogColorError(path, ini, section, err);
      }
      break;
    case INI_ERROR:
      log(ERROR) << "error loading theme at " << path << ":" << ini.linenum() << "\n";
      break;
    case INI_EOF:
      return true;
    }
  }
}

static bool InitTheme(void) {
  // requires HOME, set by InitGlobalVariables()
  const Chars prefix[2] = {
      Chars::from_c(getenv("XDG_CONFIG_HOME")),
      HOME,
  };
  const Chars infix[2] = {
      "",
      "/.config",
  };
  String path;
  for (unsigned i = 0; i < 2; i++) {
    if (prefix[i].empty()) {
      continue;
    }
    if (!path.format(prefix[i], infix[i], "/twin/theme.ini")) {
      return false; /* out of memory */
    }
    Ini ini(path);
    if (!ini.is_open()) {
      continue;
    }
    return LoadTheme(ini, path);
  }
  return true; // no theme.ini found
}

bool InitData(void) {
  trune *vec[] = {GadgetResize, ScrollBarX,   ScrollBarY,   &TabX,
                  &TabY,        StdBorder[0], StdBorder[1], Screen_Back};
  byte sizes[] = {2, 3, 3, 1, 1, 9, 9, 2};
  uldat i, j;

  for (i = 0; i < N_OF(vec); i++) {
    for (j = 0; j < sizes[i]; j++) {
      vec[i][j] = Tutf_CP437_to_UTF_32[(byte)vec[i][j]];
    }
  }
  return AssignId_all(All) && InitTheme();
}
