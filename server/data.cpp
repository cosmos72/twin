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

#define L 0x55
#define M 0xAA
#define H 0xFF

#define A 0x40 /*  64 */
#define B 0x68 /* 104 */
#define C 0xA0 /* 144 */
#define D 0xC4 /* 196 */
#define E 0xFF /* 255 */

#ifdef TWIN_PALETTE_SOLARIZED
#define TDARK TRGB(0x04, 0x08, 0x10)
#define TLIGHT TRGB(0xBB, 0xBB, 0xBB)
#else
#define TDARK TRGB(0, 0, 0)
#define TLIGHT TRGB(M, M, M)
#endif

/* the 256-color palette, compatible with xterm-256color palette */
const trgb Palette[tpalette_n] = {
    TDARK,         TRGB(M, 0, 0), TRGB(0, M, 0), TRGB(M, M, 0), TRGB(0, 0, M), TRGB(M, 0, M),
    TRGB(0, M, M), TLIGHT, /**/
    TRGB(L, L, L), TRGB(H, L, L), TRGB(L, H, L), TRGB(H, H, L), TRGB(L, L, H), TRGB(H, L, H),
    TRGB(L, H, H), TRGB(H, H, H), /**/

    TRGB(0, 0, 0), TRGB(0, 0, A), TRGB(0, 0, B), TRGB(0, 0, C), TRGB(0, 0, D), TRGB(0, 0, E),
    TRGB(0, A, 0), TRGB(0, A, A), TRGB(0, A, B), TRGB(0, A, C), TRGB(0, A, D), TRGB(0, A, E),
    TRGB(0, B, 0), TRGB(0, B, A), TRGB(0, B, B), TRGB(0, B, C), TRGB(0, B, D), TRGB(0, B, E),
    TRGB(0, C, 0), TRGB(0, C, A), TRGB(0, C, B), TRGB(0, C, C), TRGB(0, C, D), TRGB(0, C, E),
    TRGB(0, D, 0), TRGB(0, D, A), TRGB(0, D, B), TRGB(0, D, C), TRGB(0, D, D), TRGB(0, D, E),
    TRGB(0, E, 0), TRGB(0, E, A), TRGB(0, E, B), TRGB(0, E, C), TRGB(0, E, D), TRGB(0, E, E),

    TRGB(A, 0, 0), TRGB(A, 0, A), TRGB(A, 0, B), TRGB(A, 0, C), TRGB(A, 0, D), TRGB(A, 0, E),
    TRGB(A, A, 0), TRGB(A, A, A), TRGB(A, A, B), TRGB(A, A, C), TRGB(A, A, D), TRGB(A, A, E),
    TRGB(A, B, 0), TRGB(A, B, A), TRGB(A, B, B), TRGB(A, B, C), TRGB(A, B, D), TRGB(A, B, E),
    TRGB(A, C, 0), TRGB(A, C, A), TRGB(A, C, B), TRGB(A, C, C), TRGB(A, C, D), TRGB(A, C, E),
    TRGB(A, D, 0), TRGB(A, D, A), TRGB(A, D, B), TRGB(A, D, C), TRGB(A, D, D), TRGB(A, D, E),
    TRGB(A, E, 0), TRGB(A, E, A), TRGB(A, E, B), TRGB(A, E, C), TRGB(A, E, D), TRGB(A, E, E),

    TRGB(B, 0, 0), TRGB(B, 0, A), TRGB(B, 0, B), TRGB(B, 0, C), TRGB(B, 0, D), TRGB(B, 0, E),
    TRGB(B, A, 0), TRGB(B, A, A), TRGB(B, A, B), TRGB(B, A, C), TRGB(B, A, D), TRGB(B, A, E),
    TRGB(B, B, 0), TRGB(B, B, A), TRGB(B, B, B), TRGB(B, B, C), TRGB(B, B, D), TRGB(B, B, E),
    TRGB(B, C, 0), TRGB(B, C, A), TRGB(B, C, B), TRGB(B, C, C), TRGB(B, C, D), TRGB(B, C, E),
    TRGB(B, D, 0), TRGB(B, D, A), TRGB(B, D, B), TRGB(B, D, C), TRGB(B, D, D), TRGB(B, D, E),
    TRGB(B, E, 0), TRGB(B, E, A), TRGB(B, E, B), TRGB(B, E, C), TRGB(B, E, D), TRGB(B, E, E),

    TRGB(C, 0, 0), TRGB(C, 0, A), TRGB(C, 0, B), TRGB(C, 0, C), TRGB(C, 0, D), TRGB(C, 0, E),
    TRGB(C, A, 0), TRGB(C, A, A), TRGB(C, A, B), TRGB(C, A, C), TRGB(C, A, D), TRGB(C, A, E),
    TRGB(C, B, 0), TRGB(C, B, A), TRGB(C, B, B), TRGB(C, B, C), TRGB(C, B, D), TRGB(C, B, E),
    TRGB(C, C, 0), TRGB(C, C, A), TRGB(C, C, B), TRGB(C, C, C), TRGB(C, C, D), TRGB(C, C, E),
    TRGB(C, D, 0), TRGB(C, D, A), TRGB(C, D, B), TRGB(C, D, C), TRGB(C, D, D), TRGB(C, D, E),
    TRGB(C, E, 0), TRGB(C, E, A), TRGB(C, E, B), TRGB(C, E, C), TRGB(C, E, D), TRGB(C, E, E),

    TRGB(D, 0, 0), TRGB(D, 0, A), TRGB(D, 0, B), TRGB(D, 0, C), TRGB(D, 0, D), TRGB(D, 0, E),
    TRGB(D, A, 0), TRGB(D, A, A), TRGB(D, A, B), TRGB(D, A, C), TRGB(D, A, D), TRGB(D, A, E),
    TRGB(D, B, 0), TRGB(D, B, A), TRGB(D, B, B), TRGB(D, B, C), TRGB(D, B, D), TRGB(D, B, E),
    TRGB(D, C, 0), TRGB(D, C, A), TRGB(D, C, B), TRGB(D, C, C), TRGB(D, C, D), TRGB(D, C, E),
    TRGB(D, D, 0), TRGB(D, D, A), TRGB(D, D, B), TRGB(D, D, C), TRGB(D, D, D), TRGB(D, D, E),
    TRGB(D, E, 0), TRGB(D, E, A), TRGB(D, E, B), TRGB(D, E, C), TRGB(D, E, D), TRGB(D, E, E),

    TRGB(E, 0, 0), TRGB(E, 0, A), TRGB(E, 0, B), TRGB(E, 0, C), TRGB(E, 0, D), TRGB(E, 0, E),
    TRGB(E, A, 0), TRGB(E, A, A), TRGB(E, A, B), TRGB(E, A, C), TRGB(E, A, D), TRGB(E, A, E),
    TRGB(E, B, 0), TRGB(E, B, A), TRGB(E, B, B), TRGB(E, B, C), TRGB(E, B, D), TRGB(E, B, E),
    TRGB(E, C, 0), TRGB(E, C, A), TRGB(E, C, B), TRGB(E, C, C), TRGB(E, C, D), TRGB(E, C, E),
    TRGB(E, D, 0), TRGB(E, D, A), TRGB(E, D, B), TRGB(E, D, C), TRGB(E, D, D), TRGB(E, D, E),
    TRGB(E, E, 0), TRGB(E, E, A), TRGB(E, E, B), TRGB(E, E, C), TRGB(E, E, D), TRGB(E, E, E),
};

#undef H
#undef M
#undef L

#undef A
#undef B
#undef C
#undef D
#undef E

#undef TDARK
#undef TLIGHT

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

tcolor DEFAULT_ColGadgets = TCOL(tYELLOW, tcyan), DEFAULT_ColArrows = TCOL(tGREEN, tBLUE),
       DEFAULT_ColBars = TCOL(twhite, tBLUE), DEFAULT_ColTabs = TCOL(tWHITE, tBLUE),
       DEFAULT_ColBorder = TCOL(tWHITE, tBLUE), DEFAULT_ColDisabled = TCOL(tBLACK, tblack),
       DEFAULT_ColSelectDisabled = TCOL(tblack, tBLACK);

bool InitData(void) {
  trune *vec[] = {GadgetResize, ScrollBarX,   ScrollBarY,   &TabX,
                  &TabY,        StdBorder[0], StdBorder[1], Screen_Back};
  byte sizes[] = {2, 3, 3, 1, 1, 9, 9, 2};
  uldat i, j;

  for (i = 0; i < N_OF(vec); i++) {
    for (j = 0; j < sizes[i]; j++)
      vec[i][j] = Tutf_CP437_to_UTF_32[(byte)vec[i][j]];
  }
  return AssignId_all(All);
}
