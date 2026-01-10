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

const tcolor DEFAULT_ColGadgets = TCOL(tYELLOW, tcyan), DEFAULT_ColArrows = TCOL(tGREEN, tBLUE),
             DEFAULT_ColBars = TCOL(twhite, tBLUE), DEFAULT_ColTabs = TCOL(tWHITE, tBLUE),
             DEFAULT_ColBorder = TCOL(tWHITE, tBLUE), DEFAULT_ColDisabled = TCOL(tBLACK, tblack),
             DEFAULT_ColSelectDisabled = TCOL(tblack, tBLACK);

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
  return AssignId_all(All);
}
