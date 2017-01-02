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

hwfont Tutf_UTF_16_to_CP437(hwfont c)
{
#define EL(x) +1
    enum {
        n = T_NLIST(CP437,EL) + 1,    /* +1 to manually map T_UTF_16_CHECK_MARK -> T_CP437_SQUARE_ROOT below */
        n_power_of_2 = NEXT_POWER_OF_2(n)
    };
#undef EL
    
    static utf16_hash_table * table = NULL;
    
    /* Codepage 437 (VGA) obviously cannot contain all unicode chars. this is just a best effort. */
    if (!table) {
	table = utf16_hash_create(Tutf_CP437_to_UTF_16, n, n_power_of_2);

        /* manually map T_UTF_16_CHECK_MARK -> T_CP437_SQUARE_ROOT */
        utf16_hash_insert_at(table, n - 1, T_UTF_16_CHECK_MARK, T_CP437_SQUARE_ROOT);
    }
    return utf16_hash_search(table, c, TRUE);
}

