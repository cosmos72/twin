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

hwfont Tutf_CP850_to_UTF_16[0x100] = {
#define EL(x) T_UTF(UTF_16,x),
	T_LIST(CP850,EL)
#undef EL
};

static byte flag_CP850;

static utf_to_ch array_CP850 [] = {
#define EL(x) { T_UTF(UTF_16,x), T_UTF(CP850,x) },
    T_NLIST(CP850,EL)
#undef EL
    { T_UTF_16_IDENTICAL_TO, '=' },
    { T_UTF_16_CHECK_MARK, 'V' },
};

hwfont Tutf_UTF_16_to_CP850(hwfont c) {
    static utf_to_ch key;
    TW_CONST utf_to_ch *res;
    
    /* Codepage 850 obviously cannot contain all unicode chars. this is just a best effort. */
    if (!flag_CP850) {
	flag_CP850 = TRUE;
	QSORT(array_CP850);
    }
    if (c == key.utf)
	return key.ch;
    if ((c & ~0x00ff) == 0xf000 ||
	/* direct-to-font area */
	(c >= ' ' && c <= '~') ||
	/* ASCII area */
	(c > '~' && c < 0x100 && Tutf_CP850_to_UTF_16[c] == c))
	/* c has the same meaning in Unicode and this charset... sheer luck! */
	
	return c & 0x00ff;
    
    key.utf = c;
    res = BSEARCH(&key, array_CP850);
    
    if (res)
	c = res->ch;
    else if (c > '~')
	/* try to approximate */
	c = T_CAT(Tutf_CP437_to_,T_MAP(ASCII)) [ Tutf_UTF_16_to_CP437(c) ];
    /* else c = c; */

    return key.ch = c;
}

