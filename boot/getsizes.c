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


#include "twin.h"

#include <asm/page.h>

int main(void) {
    printf("#ifndef _TW_SIZES_H\n"
	   "#define _TW_SIZES_H\n"
	   "\n"
	   "#define _SIZEOFNUM	%d\n"
	   "#define _SIZEOFBYTE	%d\n"
	   "#define _SIZEOFDAT	%d\n"
	   "#define _SIZEOFUDAT	%d\n"
	   "#define _SIZEOFLDAT	%d\n"
	   "#define _SIZEOFULDAT	%d\n"
	   "#define _SIZEOFTIME_T	%d\n"
	   "#define _SIZEOFFRAC_T	%d\n"
	   "#define _SIZEOFVOIDP	%d\n"
	   "\n"
	   "#define _MAXNUM 	0x%X\n"
	   "#define _MAXBYTE	0x%X\n"
	   "#define _MAXDAT 	0x%X\n"
	   "#define _MAXUDAT	0x%X\n"
	   "#define _MAXLDAT	0x%lXul\n"
	   "#define _MAXULDAT	0x%lXul\n"
	   "#define _MAXTIME_T	0x%lXul\n"
	   "#define _MAXFRAC_T	0x%lXul\n"
	   "\n"
	   "#define _MINNUM 	0x%X\n"
	   "#define _MINDAT 	0x%X\n"
	   "#define _MINLDAT	0x%lXul\n"
	   "#define _MINTIME_T	0x%lXul\n"
	   "#define _MINFRAC_T	0x%lXul\n"
	   "\n"
	   "#define BLOCK_SIZE	%d\n"
	   "\n"
	   "#define TW_BYTE_ORDER	%d\n"
	   "#define TW_LITTLE_ENDIAN	1234\n"
	   "#define TW_BIG_ENDIAN	4321\n"
	   "\n"
	   "#endif /* _TW_SIZES_H */\n",
	   sizeof(num), sizeof(byte),
	   sizeof(dat), sizeof(udat),
	   sizeof(ldat),sizeof(uldat),
	   sizeof(time_t), sizeof(frac_t),
	   sizeof(void *),
	   (int)MAXNUM, (int)MAXBYTE,
	   (int)MAXDAT, (int)MAXUDAT,
	   (long)MAXLDAT, (long)MAXULDAT,
	   (long)MAXTIME_T, (long)MAXFRAC_T,
	   (int)(byte)MINNUM, (int)(udat)MINDAT, (long)(uldat)MINLDAT,
	   (long)(unsigned long)MINTIME_T, (long)(unsigned long)MINFRAC_T,
	   (int)PAGE_SIZE,
	   *(int *)"\1\2\3\4" == 0x04030201 ? 1234 : 4321
	   );
    return 0;
}

