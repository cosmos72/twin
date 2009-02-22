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

hwfont Tutf_CP437_to_UTF_16[0x100] = {
#define EL(x) T_UTF(UTF_16,x),
	T_LIST(CP437,EL)
#undef EL
};

static byte flag_CP437;

static utf_to_ch array_CP437 [] = {
#define EL(x) { T_UTF(UTF_16,x), T_UTF(CP437,x) },
    T_NLIST(CP437,EL)
#undef EL
    { T_UTF_16_CHECK_MARK, T_UTF(CP437,_SQUARE_ROOT) },
};

hwfont Tutf_UTF_16_to_CP437(hwfont c) {
    static utf_to_ch key;
    TW_CONST utf_to_ch *res;
    
    /* Codepage 437 (VGA) obviously cannot contain all unicode chars. this is just a best effort. */
    if (!flag_CP437) {
	flag_CP437 = TRUE;
	QSORT(array_CP437);
    }
    if (c == key.utf)
	return key.ch;
    if ((c & ~0x00ff) == 0xf000 ||
	/* direct-to-font area */
	(c >= ' ' && c <= '~') ||
	/* ASCII area */
	(c > '~' && c < 0x100 && Tutf_CP437_to_UTF_16[c] == c))
	/* c has the same meaning in Unicode and this charset... sheer luck! */
	
	return c & 0x00ff;
    
    key.utf = c;
    res = BSEARCH(&key, array_CP437);
    
    if (res)
	c = res->ch;
    /* else c might have the same meaning in Unicode and this charset... */
    else if (c > '~')
	/* try to approximate (todo) */
	c = T_UTF(CP437,_NULL);
    /* else c = c; */

    return key.ch = c;
}

