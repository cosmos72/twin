/*
 *
 *  Copyright (C) 2002,2017-2018 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 */

const trune Tutf_ISO8859_1_to_UTF_32[0x100] = {
#define EL(x) T_UTF(UTF_32, x),
    T_LIST(ISO8859_1, EL)
#undef EL
};

/* ISO8859_1 maps exactly to the first 256 elements of UTF-32... no need for a lookup table */
trune Tutf_UTF_32_to_ISO8859_1(trune utf32) {
  static trune cache_utf32;
  static byte cache_ch;

  if (utf32 == cache_utf32)
    return cache_ch;

  if (utf32 < 0x100 ||             /* ISO8859_1 maps exactly to the first 256 elements of UTF-32 */
      (utf32 & ~0x00ff) == 0xf000) /* direct-to-font area */
  {
    return utf32 & 0x00ff;
  }

  /* try to approximate going through UTF32 -> CP437 -> ASCII */
  cache_utf32 = utf32;
  return cache_ch = Tutf_UTF_32_to_ASCII(utf32);
}
