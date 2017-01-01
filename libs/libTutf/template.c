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


#define T_TEMPLATE T_MAP(TEMPLATE)

hwfont T_CAT3(Tutf_,T_TEMPLATE,_to_UTF_16)[0x100] = {
#define EL(x) T_UTF(UTF_16,x),
    T_LIST(T_TEMPLATE,EL)
#undef EL
};


#define EL(x) +1
enum {
   T_CAT(n_,T_TEMPLATE) = 0 T_NLIST(T_TEMPLATE,EL),

   T_CAT3(n_,T_TEMPLATE,_power_of_2) = NEXT_POWER_OF_2(T_CAT(n_,T_TEMPLATE))
};
#undef EL

static byte             T_CAT(index_,T_TEMPLATE) [T_CAT3(n_,T_TEMPLATE,_power_of_2)];
static utf16_to_charset T_CAT(array_,T_TEMPLATE) [T_CAT(n_,T_TEMPLATE)]; 

hwfont T_CAT(Tutf_UTF_16_to_,T_TEMPLATE) (hwfont c) {
    static TUTF_CONST utf16_to_charset *last = NULL;
    TUTF_CONST utf16_to_charset *res;
    
    /* Codepage 865 (VGA) obviously cannot contain all unicode chars. this is just a best effort. */
    if (!last) {
	utf16_hash_init(T_CAT3(Tutf_,T_TEMPLATE,_to_UTF_16),
			T_CAT(index_,T_TEMPLATE),
			T_CAT(array_,T_TEMPLATE),
			T_CAT3(n_,T_TEMPLATE,_power_of_2));
        
        last = T_CAT(array_,T_TEMPLATE);
    }
    if (c == last->utf16)
	return last->ch;
   
    if (
#ifndef TEMPLATE_REDEFINES_ASCII
	(c >= ' ' && c <= '~')  || /* ASCII area */
#endif
	(c & ~0x00ff) == 0xf000 || /* direct-to-font area */
	(c < 0x100 && T_CAT3(Tutf_,T_TEMPLATE,_to_UTF_16[c]) == c)) /* does c have the same meaning in Unicode and this charset? */

        return c & 0x00ff;
    
    res = utf16_hash_search(c, T_CAT(index_,T_TEMPLATE), T_CAT(array_,T_TEMPLATE), T_CAT3(n_,T_TEMPLATE,_power_of_2));

    if (res)
	c = (last = res)->ch;
    else 
    {
	c = Tutf_UTF_16_to_ASCII(c); /* try to approximate */
#ifdef TEMPLATE_REDEFINES_ASCII
        if (c != T_CAT3(Tutf_,T_TEMPLATE,_to_UTF_16)[c])
	    c = ' ';
#endif
    }
    return c;
}

#undef T_TEMPLATE
