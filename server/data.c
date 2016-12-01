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

#ifdef PACKAGE_LIBDIR
CONST byte * CONST conf_destdir_lib_twin = PACKAGE_LIBDIR "/twin";
CONST byte * CONST conf_destdir_lib_twin_modules_ = PACKAGE_LIBDIR "/twin/modules/";
#else
CONST byte * CONST conf_destdir_lib_twin = ".";
CONST byte * CONST conf_destdir_lib_twin_modules_ = "./";
#endif



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

    
    {	    /* LAT1_MAP mapped to Unicode -- it's the identity */
	    NULL,
	    /* IBMPC_MAP mapped to Unicode -- get from libTutf */
	    Tutf_CP437_to_UTF_16,
	    /* USER_MAP */
	    GtransUser
    },
};
all All = &_All;


keylist TW_KeyList[] = {
#define IS(key, len, seq) { #key, TW_##key, len, seq },
#   include "hw_keys.h"
#undef IS
    { NULL, TW_Null, 0, NULL }
};


hwfont GadgetResize[2] = {(byte)'Í', (byte)'¼'},
ScrollBarX[3] = {(byte)'±', (byte)'\x11', (byte)'\x10'},
ScrollBarY[3] = {(byte)'±', (byte)'\x1E', (byte)'\x1f'},
TabX = (byte)'Û', TabY = (byte)'Û',
StdBorder[2][9] =
{
    {
	(byte)'É',(byte)'Í',(byte)'»',
	(byte)'º',(byte)' ',(byte)'º',
	(byte)'È',(byte)'Í',(byte)'¼'
    }, {
	(byte)'Ú',(byte)'Ä',(byte)'¿',
	(byte)'³',(byte)' ',(byte)'³',
	(byte)'À',(byte)'Ä',(byte)'Ù'
    }
},
Screen_Back[2] = { (byte)'\x12', (byte)'\x12' };


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
