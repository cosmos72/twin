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

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
#endif

/* setup configuration paths */

#ifdef LIBDIR
CONST byte *conf_destdir_lib_twin = LIBDIR "/twin";
CONST byte *conf_destdir_lib_twin_modules_ = LIBDIR "/twin/modules/";
#else
CONST byte *conf_destdir_lib_twin = ".";
CONST byte *conf_destdir_lib_twin_modules_ = "./";
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

#ifdef CONF__UNICODE
    
    {	    /* LAT1_MAP mapped to Unicode -- it's the identity */
	    NULL,
	    /* IBMPC_MAP mapped to Unicode -- get from libTutf */
	    Tutf_CP437_to_UTF_16,
	    /* USER_MAP */
	    GtransUser
    },
    
#else /* !CONF__UNICODE */
    
    {	    /* LAT1_MAP mapped to CP437 -- just a best fit */
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
	    "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
	    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
	    "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
	    "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
	    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
	    "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\xFE"
	    "\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE"
	    "\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE"
	    "\xFF\xAD\x9B\x9C\xFE\x9D\xFE\x15\x22\x43\xA6\xAE\xAA\x2D\x52\xFE"
	    "\xF8\xF1\xFD\xFE\xFE\xE6\x14\xFA\x2C\xFE\xA7\xAF\xAC\xAB\xFE\xA8"
	    "\x41\x41\x41\x41\x8E\x8F\x92\x80\x45\x90\x45\x45\x49\x49\x49\x49"
	    "\xFE\xA5\x4F\x4F\x4F\x4F\x99\x78\xE8\x55\x55\x55\x9A\x59\xFE\xE1"
	    "\x85\xA0\x83\x61\x84\x86\x91\x87\x8A\x82\x88\x89\x8D\xA1\x8C\x8B"
	    "\xFE\xA4\x95\xA2\x93\x6F\x94\xF6\xED\x97\xA3\x96\x81\x79\xFE\x98",
	    /* IBMPC_MAP to CP437 -- it's the identity */
	    NULL,
	    /* USER_MAP */
	    GtransUser
    },
#endif /* CONF__UNICODE */
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
#ifdef CONF__UNICODE
    hwfont *vec [] = { GadgetResize, ScrollBarX, ScrollBarY, &TabX, &TabY, StdBorder[0], StdBorder[1], Screen_Back };
    byte  sizes [] = {            2,          3,          3,     1,     1,            9,            9,           2 };
    int i, j;
    
    for (i = 0; i < sizeof(vec)/sizeof(vec[0]); i++) {
	for (j = 0; j < sizes[i]; j++)
	    vec[i][j] = Tutf_CP437_to_UTF_16[vec[i][j]];
    }
#endif /* CONF__UNICODE */
    return AssignId_all(All);
}
