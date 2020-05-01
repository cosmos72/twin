/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_DATA_H
#define _TWIN_DATA_H

extern const char *const pkg_libdir;
extern const char *const pkg_libdir_modules_;

extern rgb Palette[tmaxcol + 1], defaultPalette[tmaxcol + 1];

typedef struct keylist keylist;
struct keylist {
  const char *name;
  udat key;
  byte len;
  const char *seq;
};

extern keylist TW_KeyList[];

extern gadget GadgetFlag, GadgetSwitch;

extern trune GadgetResize[2], ScrollBarX[3], ScrollBarY[3], TabX, TabY, StdBorder[2][9],
    Screen_Back[2];

extern tcolor DEFAULT_ColGadgets, DEFAULT_ColArrows, DEFAULT_ColBars, DEFAULT_ColTabs,
    DEFAULT_ColBorder, DEFAULT_ColDisabled, DEFAULT_ColSelectDisabled;

byte InitData(void);

#endif /* _TWIN_DATA_H */
