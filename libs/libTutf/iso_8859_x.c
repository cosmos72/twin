/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

#define T_ISO_8859_X T_MAP(ISO_8859_X)

hwfont T_CAT3(Tutf_,T_ISO_8859_X,_to_UTF_16)[0x100] = {
#define EL(x) T_UTF(UTF_16,x),
    T_LIST(ISO_8859_X,EL)
#undef EL
};

static byte T_CAT(flag_,T_ISO_8859_X);

static utf_to_ch T_CAT(array_,T_ISO_8859_X) [] = {
#define EL(x) { T_UTF(UTF_16,x), T_UTF(ISO_8859_X,x) },
    T_NLIST(ISO_8859_X,EL)
#undef EL

#define BOX_(x,y) { T_UTF(UTF_16,T_CAT(_BOX_DRAWINGS_LIGHT,x)), y }, \
    { T_UTF(UTF_16,T_CAT(_BOX_DRAWINGS_HEAVY,x)), y }, \
    { T_UTF(UTF_16,T_CAT(_BOX_DRAWINGS_DOUBLE,x)), y }
	
    BOX_(_UP_AND_LEFT, '+'),
    BOX_(_DOWN_AND_LEFT, '+'),
    BOX_(_DOWN_AND_RIGHT, '+'),
    BOX_(_UP_AND_RIGHT, '+'),
    BOX_(_VERTICAL_AND_HORIZONTAL, '+'),
    BOX_(_VERTICAL_AND_RIGHT, '+'),
    BOX_(_VERTICAL_AND_LEFT, '+'),
    BOX_(_UP_AND_HORIZONTAL, '+'),
    BOX_(_DOWN_AND_HORIZONTAL, '+'),
    BOX_(_VERTICAL, '|'),

#undef BOX_
    
    { T_UTF_16_LIGHT_SHADE,	'+' },
    { T_UTF_16_MEDIUM_SHADE,	'*' },
    { T_UTF_16_DARK_SHADE,	'@' },
    { T_UTF_16_FULL_BLOCK,	'#' },
    { T_UTF_16_BOX_DRAWINGS_LIGHT_HORIZONTAL,	'-' },
    { T_UTF_16_BOX_DRAWINGS_HEAVY_HORIZONTAL,	'-' },
    { T_UTF_16_BOX_DRAWINGS_DOUBLE_HORIZONTAL,	'=' },
    { T_UTF_16_UP_DOWN_ARROW,	'|' },
    { T_UTF_16_BLACK_RIGHT_POINTING_TRIANGLE,	'>' },
    { T_UTF_16_BLACK_LEFT_POINTING_TRIANGLE,	'<' },
    { T_UTF_16_BLACK_UP_POINTING_TRIANGLE,	'^' },
    { T_UTF_16_BLACK_DOWN_POINTING_TRIANGLE,	'v' },
    { T_UTF_16_UPPER_HALF_BLOCK,	'"' },
    { T_UTF_16_LOWER_HALF_BLOCK,	'_' },
};

hwfont T_CAT(Tutf_UTF_16_to_,T_ISO_8859_X)(hwfont c) {
    static utf_to_ch key;
    TW_CONST utf_to_ch *res;
    
    /* ISO_8859_X obviously cannot contain all unicode chars. this is just a best effort. */
    if (!T_CAT(flag_,T_ISO_8859_X)) {
	T_CAT(flag_,T_ISO_8859_X) = TRUE;
	QSORT(T_CAT(array_,T_ISO_8859_X));
    }
    if (c == key.utf)
	return key.ch;
    if ((c & ~0x00ff) == 0xf000 ||
	/* direct-to-font area */
	(c >= ' ' && c <= '~') ||
	/* ASCII area */
	(c > '~' && c < 0x100 && T_CAT3(Tutf_,T_ISO_8859_X,_to_UTF_16)[c] == c))
	/* c has the same meaning in Unicode and this charset... sheer luck! */

	return c & 0x00ff;
    
    key.utf = c;
    res = BSEARCH(&key, T_CAT(array_,T_ISO_8859_X));
    
    if (res)
	c = res->ch;
    else if (c > '~')
	/* try to approximate */
	c = T_CAT(Tutf_CP437_to_,T_MAP(ASCII)) [ Tutf_UTF_16_to_CP437(c) ];
    /* else c = c; */

    return key.ch = c;
}

#undef T_ISO_8859_X
