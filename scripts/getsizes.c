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
#ifdef __sun__
# include <sys/param.h>
#endif

#ifdef __linux__
# include <asm/page.h>
#endif

#if !defined(PAGE_SIZE) && defined(PAGESIZE)
# define PAGE_SIZE PAGESIZE
#endif

#include "Tw/datatypes.h"


char endian_str[sizeof(uldat)] = "\1\2\3\4";

typedef unsigned long ul;
#define UL "ul"
#define LX "%lX"

int main(void) {
    char *byte_order, *str_byte16, *str_byte32;
    
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
    
    if (sizeof(byte) != 1 || sizeof(udat) < 2 || sizeof(uldat) < 4 ||
	sizeof(hwcol) != 1 || sizeof(time_t) < 4 || sizeof(frac_t) < 4) {
	
	fprintf(stderr, "Fatal: minimum requirements on data sizes not satisfied.\nSee scripts/getsizes.c\n");
	return 1;
    }

    if (sizeof(udat) == 2)
	str_byte16 = "udat";
    else if (sizeof(unsigned short) == 2)
	str_byte16 = "unsigned short";
    else {
	fprintf(stderr, "Fatal: could not find a 16-bit type to use as byte16.\nSee scripts/getsizes.c\n");
	return 1;
    }

    if (sizeof(udat) == 4)
	str_byte32 = "udat";
    else if (sizeof(uldat) == 4)
	str_byte32 = "uldat";
    else if (sizeof(unsigned int) == 4)
	str_byte32 = "unsigned int";
    else if (sizeof(unsigned long) == 4)
	str_byte32 = "unsigned long";
    else {
	fprintf(stderr, "Fatal: could not find a 32-bit type to use as byte32.\nSee scripts/getsizes.c\n");
	return 1;
    }

    if (sizeof(time_t) > sizeof(tlargest)) {
	fprintf(stderr, "Fatal: `time_t' is %d bytes, which is bigger than `tlargest' (%d bytes).\n"
		"Please edit include/Tw/datatypes.h and use a wider type for `tlargest'.\n", sizeof(time_t), sizeof(tlargest));
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
	   "#define TW_SIZEOFBYTE         %d\n"
	   "#define TW_SIZEOFUDAT         %d\n"
	   "#define TW_SIZEOFULDAT        %d\n"
	   "#define TW_SIZEOFTIME_T       %d\n"
	   "#define TW_SIZEOFFRAC_T       %d\n"
	   "#define TW_SIZEOFTLARGEST     %d\n"
	   "#define TW_SIZEOFVOIDP        %d\n"
	   "\n"
	   "#define TW_MAXNUM      0x" LX "\n"
	   "#define TW_MAXBYTE     0x" LX "\n"
	   "#define TW_MAXDAT      0x" LX "\n"
	   "#define TW_MAXUDAT     0x" LX "\n"
	   "#define TW_MAXLDAT     0x" LX "\n"
	   "#define TW_MAXULDAT    0x" LX "\n"
	   "#define TW_MAXTIME_T   0x" LX "\n"
	   "#define TW_MAXFRAC_T   0x" LX "\n"
	   "#define TW_MAXTLARGEST 0x" LX "\n"
	   "\n"
	   "#define TW_MINNUM      0x" LX "\n"
	   "#define TW_MINDAT      0x" LX "\n"
	   "#define TW_MINLDAT     0x" LX "\n"
	   "#define TW_MINTIME_T   0x" LX "\n"
	   "#define TW_MINFRAC_T   0x" LX "\n"
	   "\n"
	   "#define TW_BYTE16      %s\n"
	   "#define TW_BYTE32      %s\n"
	   "\n"
	   "#define TW_PAGE_SIZE        %d\n"
	   "\n"
	   "#define TW_BYTE_ORDER       %s\n"
	   "#define TW_LITTLE_ENDIAN    1234\n"
	   "#define TW_BIG_ENDIAN       4321\n"
	   "\n"
	   "#endif /* _TW_DATASIZES_H */\n",
	   sizeof(byte), sizeof(udat), sizeof(uldat),
	   sizeof(time_t), sizeof(frac_t), sizeof(tlargest),
	   sizeof(void *),
	   (ul)MAXNUM, (ul)MAXBYTE,
	   (ul)MAXDAT, (ul)MAXUDAT,
	   (ul)MAXLDAT, (ul)MAXULDAT,
	   (ul)MAXTIME_T, (ul)MAXFRAC_T,
	   (ul)MAXTLARGEST,
	   -(ul)MINNUM, -(ul)MINDAT, -(ul)MINLDAT,
	   -(ul)MINTIME_T, -(ul)MINFRAC_T,
	   str_byte16,
	   str_byte32,
	   (int)PAGE_SIZE,
	   byte_order
	   );
    return 0;
}

