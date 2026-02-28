/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_DATA_H
#define TWIN_DATA_H

#include "stl/chars.h"

#include <Tw/datatypes.h> // byte, udat, tcolor, trune
#include <Tw/Tw_defs.h>   // tpalette_n

// directory containing plugins: libhw_*.* librcparse.* libsocket.* libterm.*
extern Chars plugindir;

// directory containing system-wide configuration file twinrc
extern Chars confdir;

struct keylist {
  const char *name;
  udat key;
  byte len;
  const char *seq;
};

extern keylist TW_KeyList[];

extern Tgadget GadgetFlag, GadgetSwitch;

extern trune GadgetResize[2], ScrollBarX[3], ScrollBarY[3], TabX, TabY, StdBorder[2][9],
    Screen_Back[2];

// user-customizable window colors theme
class ColorTheme {
public:
  ColorTheme();
  ColorTheme(tcolor gadgets, tcolor arrows, tcolor bars, tcolor tabs, tcolor borders, tcolor text,
             tcolor selected, tcolor disabled, tcolor selectdisabled);

  tcolor Gadgets() const {
    return vec[0];
  }
  tcolor &Gadgets() {
    return vec[0];
  }
  tcolor Arrows() const {
    return vec[1];
  }
  tcolor &Arrows() {
    return vec[1];
  }
  tcolor Bars() const {
    return vec[2];
  }
  tcolor &Bars() {
    return vec[2];
  }
  tcolor Tabs() const {
    return vec[3];
  }
  tcolor &Tabs() {
    return vec[3];
  }
  tcolor Borders() const {
    return vec[4];
  }
  tcolor &Borders() {
    return vec[4];
  }
  tcolor Text() const {
    return vec[5];
  }
  tcolor &Text() {
    return vec[5];
  }
  tcolor Selected() const {
    return vec[6];
  }
  tcolor &Selected() {
    return vec[6];
  }
  tcolor Disabled() const {
    return vec[7];
  }
  tcolor &Disabled() {
    return vec[7];
  }
  tcolor SelectDisabled() const {
    return vec[8];
  }
  tcolor &SelectDisabled() {
    return vec[8];
  }
  tcolor operator[](unsigned index) const {
    return vec[index < N_OF(vec) ? index : 5];
  }
  tcolor &operator[](unsigned index) {
    return vec[index < N_OF(vec) ? index : 5];
  }

private:
  tcolor vec[9];
};

extern ColorTheme DefaultTheme, MenuTheme;

bool InitData(void);

#endif /* TWIN_DATA_H */
