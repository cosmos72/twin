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

hwfont Tutf_CP865_to_UTF_16[0x100] = {
#define EL(x) T_UTF(UTF_16,x),
	T_LIST(CP865,EL)
#undef EL
};

#define EL(x) +1
enum {
   n_CP865 = T_NLIST(CP865,EL) + 1,    /* +1 to manually map T_UTF_16_CHECK_MARK below */

   n_CP865_power_of_2 = NEXT_POWER_OF_2(n_CP865)
};
#undef EL

static byte             index_CP865 [n_CP865_power_of_2];
static utf16_to_charset array_CP865 [n_CP865]; 

hwfont Tutf_UTF_16_to_CP865(hwfont c) {
    static TUTF_CONST utf16_to_charset *last = NULL;
    TUTF_CONST utf16_to_charset *res;
    
    /* Codepage 865 (VGA) obviously cannot contain all unicode chars. this is just a best effort. */
    if (!last) {
	utf16_hash_init(Tutf_CP865_to_UTF_16, index_CP865, array_CP865, n_CP865_power_of_2);
        
        /* manually map T_UTF_16_CHECK_MARK -> T_UTF(CP865,_SQUARE_ROOT) */
        utf16_hash_insert_at(T_UTF_16_CHECK_MARK, T_UTF(CP865,_SQUARE_ROOT), index_CP865,
			     array_CP865, n_CP865 - 1, n_CP865_power_of_2);

        last = array_CP865;
    }
    if (c == last->utf16)
	return last->ch;
   
    if ((c >= ' ' && c <= '~')  || /* ASCII area */
	(c & ~0x00ff) == 0xf000 || /* direct-to-font area */
	(c < 0x100 && Tutf_CP865_to_UTF_16[c] == c)) /* does c have the same meaning in Unicode and this charset? */

        return c & 0x00ff;
    
    res = utf16_hash_search(c, index_CP865, array_CP865, n_CP865_power_of_2);

    if (res)
	c = (last = res)->ch;
    else
	c = Tutf_UTF_16_to_ASCII(c); /* try to approximate */
    return c;
}
