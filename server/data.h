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

extern CONST byte *conf_destdir_lib_twin;
extern CONST byte *conf_destdir_lib_twin_modules_;

extern palette Palette[MAXCOL+1], defaultPalette[MAXCOL+1];

extern all All;

typedef struct keylist keylist;
struct keylist {
    CONST byte *name;
    udat key;
    byte len;
    CONST byte *seq;
};

extern keylist TW_KeyList[];

extern gadget GadgetFlag, GadgetSwitch;

extern hwfont GadgetResize[2],
    ScrollBarX[3],
    ScrollBarY[3],
    TabX,
    TabY,
    StdBorder[2][9],
    Screen_Back[2];

extern hwcol DEFAULT_ColGadgets, DEFAULT_ColArrows, DEFAULT_ColBars, DEFAULT_ColTabs,
	DEFAULT_ColBorder, DEFAULT_ColDisabled, DEFAULT_ColSelectDisabled;

byte InitData(void);


#endif /* _TWIN_DATA_H */
