/*  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
/* utf8.c - collect routines for conversion to/from utf8 */
#include "utf8.h"

/*
 * Convert utf8 to long.
 * On success: update *inptr to be the first nonread character,
 *   set *err to 0, and return the obtained value.
 * On failure: leave *inptr unchanged, set *err to some nonzero error value:
 *   UTF8_BAD: bad utf8, UTF8_SHORT: input too short
 *   and return 0;
 *
 * cnt is either 0 or gives the number of available bytes
 */
unsigned long
from_utf8(char **inptr, int cnt, int *err) {
	unsigned char *in;
	unsigned int uc, uc2;
	int need, bit, bad = 0;

	in = (unsigned char *)(* inptr);
	uc = *in++;
	need = 0;
	bit = 0x80;
	while(uc & bit) {
		need++;
		bit >>= 1;
	}
	uc &= (bit-1);
	if (cnt && cnt < need) {
		*err = UTF8_SHORT;
		return 0;
	}
	if (need == 1)
		bad = 1;
	else if (need) while(--need) {
		uc2 = *in++;
		if ((uc2 & 0xc0) != 0x80) {
			bad = 1;
			break;
		}
		uc = ((uc << 6) | (uc2 & 0x3f));
	}
	if (bad) {
		*err = UTF8_BAD;
		return 0;
	}
	*inptr = in;
	*err = 0;
	return uc;
}
