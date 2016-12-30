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
#include "util.h"
#include "data.h"

#include <Tw/Twkeys.h>
#include <Tutf/Tutf.h>

/* setup configuration paths */

#ifndef PKG_LIBDIR
# warning PKG_LIBDIR is not #defined, assuming "/usr/local/lib/twin"
# define PKG_LIBDIR "/usr/local/lib/twin"
#endif

CONST byte * CONST pkg_libdir = PKG_LIBDIR;



/* First, some structures */

static hwfont GtransUser[0x100];


static setup SetUp = {
    (dat)1,	 /* MaxMouseSnap */
	(udat)0x7,	 /* MinAllocSize */
	(udat)0,	 /* Flags */
	HOLD_LEFT,	 /* SelectionButton */
	HOLD_MIDDLE, /* PasteButton */
	(byte)3, (byte)2 /* DeltaXShade, DeltaYShade */
};

static selection Selection = {
    { (tany)0, (tany)0 },
    (msgport)0, (display_hw)0,
	SEL_TEXTMAGIC,
	"",
	(uldat)0, (uldat)0,
	NULL
};

#define L 0x55
#define M 0xAA
#define H 0xFF

palette Palette[MAXCOL+1] = {
    /* the default colour table, for VGA+ colour systems */
    {0,0,0}, {0,0,M}, {0,M,0}, {0,M,M}, {M,0,0}, {M,0,M}, {M,M,0}, {M,M,M},
    {L,L,L}, {L,L,H}, {L,H,L}, {L,H,H}, {H,L,L}, {H,L,H}, {H,H,L}, {H,H,H}};

palette defaultPalette[MAXCOL+1] = {
    /* the default colour table, for VGA+ colour systems */
    {0,0,0}, {0,0,M}, {0,M,0}, {0,M,M}, {M,0,0}, {M,0,M}, {M,M,0}, {M,M,M},
    {L,L,L}, {L,L,H}, {L,H,L}, {L,H,H}, {H,L,L}, {H,L,H}, {H,H,L}, {H,H,H}};

#undef H
#undef M
#undef L

static struct s_all _All = {
    NOID, NULL, NULL, NULL, NULL,
	
	(screen)0, (screen)0,
	(msgport)0, (msgport)0, (msgport)0,
	(mutex)0, (mutex)0,
	(module)0, (module)0,
	(fn_hook)0, (widget)0,
	(display_hw)0, (display_hw)0,
	(display_hw)0, (display_hw)0,
	(fn_hook)0, (widget)0,
	
	1, 1, /* DisplayWidth, DisplayHeight */
	STATE_DEFAULT,
	
    { (tany)0, (tany)0 },
	&Selection,
	&SetUp,
	(void (*)(void))0, /* AtQuit */

	(menu)0, (menu)0,
	(row)0,

    0, /* MouseMotionN */
    
    { { {0, }, }, }, /* ButtonVec[] */

    
    {
	/* VT100GR_MAP mapped to Unicode -- get from libTutf */
	Tutf_VT100GR_to_UTF_16,
        /* LATIN1_MAP mapped to Unicode -- it's the identity */
        NULL,
	/* IBMPC_MAP mapped to Unicode -- get from libTutf */
	Tutf_CP437_to_UTF_16,
	/* USER_MAP */
	GtransUser
    },
};
all CONST All = &_All;


keylist TW_KeyList[] = {
#define IS(key, len, seq) { #key, TW_##key, len, seq },
#   include "hw_keys.h"
#undef IS
    { NULL, TW_Null, 0, NULL }
};


hwfont GadgetResize[2] = {0xCD, 0xBC},
ScrollBarX[3] = {0xB1, 0x11, 0x10},
ScrollBarY[3] = {0xB1, 0x1E, 0x1f},
TabX = 0xDB, TabY = 0xDB,
StdBorder[2][9] =
{
    {
	0xC9,0xCD,0xBB,
	0xBA,0x20,0xBA,
	0xC8,0xCD,0xBC
    }, {
	0xDA,0xC4,0xBF,
	0xB3,0x20,0xB3,
	0xC0,0xC4,0xD9
    }
},
Screen_Back[2] = { 0x12, 0x12 };


hwcol DEFAULT_ColGadgets = COL(HIGH|YELLOW,CYAN),
DEFAULT_ColArrows = COL(HIGH|GREEN,HIGH|BLUE),
DEFAULT_ColBars = COL(WHITE,HIGH|BLUE),
DEFAULT_ColTabs = COL(HIGH|WHITE,HIGH|BLUE),
DEFAULT_ColBorder = COL(HIGH|WHITE,HIGH|BLUE),
DEFAULT_ColDisabled = COL(HIGH|BLACK,BLACK),
DEFAULT_ColSelectDisabled = COL(BLACK,HIGH|BLACK);

byte InitData(void) {
    hwfont *vec [] = { GadgetResize, ScrollBarX, ScrollBarY, &TabX, &TabY, StdBorder[0], StdBorder[1], Screen_Back };
    byte  sizes [] = {            2,          3,          3,     1,     1,            9,            9,           2 };
    int i, j;
    
    for (i = 0; i < sizeof(vec)/sizeof(vec[0]); i++) {
	for (j = 0; j < sizes[i]; j++)
	    vec[i][j] = Tutf_CP437_to_UTF_16[vec[i][j]];
    }
    return AssignId_all(All);
}
