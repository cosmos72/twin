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

hwfont T_CAT(Tutf_UTF_16_to_,T_TEMPLATE) (hwfont c)
{
#define EL(x) +1
    enum {
        n = T_NLIST(T_TEMPLATE,EL) + 0, /* +0 in case T_NLIST() expands to nothing */
        n_power_of_2 = NEXT_POWER_OF_2(n)
    };
#undef EL
    
    static utf16_hash_table * table = NULL;
    
    /* a single 8-bit charset obviously cannot contain all unicode chars. this is just a best effort. */
    if (!table)
	table = utf16_hash_create(T_CAT3(Tutf_,T_TEMPLATE,_to_UTF_16), n, n_power_of_2);

#ifdef TEMPLATE_REDEFINES_ASCII
    return utf16_hash_search(table, c, FALSE);
#else
    return utf16_hash_search(table, c, TRUE);
#endif
}

#undef T_TEMPLATE
