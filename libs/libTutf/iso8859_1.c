/*
 *
 *  Copyright (C) 2002,2017 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */


hwfont Tutf_ISO8859_1_to_UTF_16[0x100] = {
#define EL(x) T_UTF(UTF_16,x),
    T_LIST(ISO8859_1,EL)
#undef EL
};

/* ISO8859_1 maps exactly to the first 256 elements of UTF-16... no need for a lookup table */
hwfont Tutf_UTF_16_to_ISO8859_1(hwfont utf16)
{
    static hwfont cache_utf16;
    static byte   cache_ch;
        
    if (utf16 == cache_utf16)
        return cache_ch;
    
    if (utf16 < 0x100 || /* ISO8859_1 maps exactly to the first 256 elements of UTF-16 */
        (utf16 & ~0x00ff) == 0xf000)  /* direct-to-font area */
    {
        return utf16 & 0x00ff;
    }
    
    /* try to approximate going through UTF16 -> CP437 -> ASCII */
    cache_utf16 = utf16;
    return cache_ch = Tutf_UTF_16_to_ASCII(utf16);
}

