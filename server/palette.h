/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_PALETTE_H
#define TWIN_PALETTE_H

#include <Tw/datatypes.h> // byte, udat, tcolor, trune
#include <Tw/Tw_defs.h>   // tpalette_n

extern const trgb Palette[tpalette_n];

/* find and return the index of Palette[0..7] color nearest to specified rgb */
byte TrueColorToPalette8(trgb rgb);

/* find and return the index of Palette[0..15] color nearest to specified rgb */
byte TrueColorToPalette16(trgb rgb);

/* find and return the index of Palette[] color nearest to specified rgb */
byte TrueColorToPalette256(trgb rgb);

#endif /* TWIN_PALETTE_H */
