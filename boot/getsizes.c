/*
 *  getsizes.c  --  get sizeof() various data types and print it.
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <stdio.h>
#include <time.h>

/* Try to get PAGE_SIZE */
#ifdef __linux__
# include <asm/page.h>
#endif

#include "Tw/datatypes.h"


char endian_str[sizeof(uldat)] = "\1\2\3\4";

typedef unsigned long ul;
#define UL "ul"
#define LX "%lX"

int main(void) {
    char *byte_order;
    
    if (*(uldat *)endian_str == (uldat)0x04030201)
	byte_order = "1234"; /* little endian */
    else if (*(uldat *)endian_str == ((uldat)0x01020304 << ((sizeof(uldat)-4) * 8) ))
	byte_order = "4321"; /* big endian */
    else {
	fprintf(stderr, "Fatal: cannot determine byte order: not little endian, not big endian!\n"
		"       endianity test on string \"\\1\\2\\3\\4\" returned 0x" LX "!\n",
		(ul)*(uldat *)endian_str);
	return 1;
    }
    /*
     * note about MINDAT, MINLDAT, MINTIME_T:
     * if the type is unsigned, -(ul)0 == 0;
     * if the type is signed, extending to (ul) will fill higher bytes with 0xFF.
     * For example doing (ul)0x80000000 when (ul) is 8 bytes will give
     * 0xFFFFFFFF80000000, and -(ul)0x80000000 will give back the correct 0x80000000.
     */
    printf("#ifndef _TW_DATASIZES_H\n"
	   "#define _TW_DATASIZES_H\n"
	   "\n"
	   "#define _SIZEOFBYTE         %d\n"
	   "#define _SIZEOFUDAT         %d\n"
	   "#define _SIZEOFULDAT        %d\n"
	   "#define _SIZEOFTIME_T       %d\n"
	   "#define _SIZEOFFRAC_T       %d\n"
	   "#define _SIZEOFVOIDP        %d\n"
	   "\n"
	   "#define _MAXNUM     0x" LX "\n"
	   "#define _MAXBYTE    0x" LX "\n"
	   "#define _MAXDAT     0x" LX "\n"
	   "#define _MAXUDAT    0x" LX "\n"
	   "#define _MAXLDAT    0x" LX "\n"
	   "#define _MAXULDAT   0x" LX "\n"
	   "#define _MAXTIME_T  0x" LX "\n"
	   "#define _MAXFRAC_T  0x" LX "\n"
	   "\n"
	   "#define _MINNUM     0x" LX "\n"
	   "#define _MINDAT     0x" LX "\n"
	   "#define _MINLDAT    0x" LX "\n"
	   "#define _MINTIME_T  0x" LX "\n"
	   "#define _MINFRAC_T  0x" LX "\n"
	   "\n"
	   "#define TW_PAGE_SIZE        %d\n"
	   "\n"
	   "#define TW_BYTE_ORDER       %s\n"
	   "#define TW_LITTLE_ENDIAN    1234\n"
	   "#define TW_BIG_ENDIAN       4321\n"
	   "\n"
	   "#endif /* _TW_DATASIZES_H */\n",
	   sizeof(byte), sizeof(udat), sizeof(uldat),
	   sizeof(time_t), sizeof(frac_t),
	   sizeof(void *),
	   (ul)MAXNUM, (ul)MAXBYTE,
	   (ul)MAXDAT, (ul)MAXUDAT,
	   (ul)MAXLDAT, (ul)MAXULDAT,
	   (ul)MAXTIME_T, (ul)MAXFRAC_T,
	   -(ul)MINNUM, -(ul)MINDAT, -(ul)MINLDAT,
	   -(ul)MINTIME_T, -(ul)MINFRAC_T,
	   (int)PAGE_SIZE,
	   byte_order
	   );
    return 0;
}

