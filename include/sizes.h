#ifndef _TW_SIZES_H
#define _TW_SIZES_H

#define _SIZEOFBYTE	1
#define _SIZEOFUDAT	2
#define _SIZEOFULDAT	4
#define _SIZEOFTIME_T	4
#define _SIZEOFFRAC_T	4
#define _SIZEOFVOIDP	4

#define _MAXNUM 	0x7F
#define _MAXBYTE	0xFF
#define _MAXDAT 	0x7FFF
#define _MAXUDAT	0xFFFF
#define _MAXLDAT	0x7FFFFFFFul
#define _MAXULDAT	0xFFFFFFFFul
#define _MAXTIME_T	0x7FFFFFFFul
#define _MAXFRAC_T	0x7FFFFFFFul

#define _MINNUM 	0x80
#define _MINDAT 	0x8000
#define _MINLDAT	0x80000000ul
#define _MINTIME_T	0x80000000ul
#define _MINFRAC_T	0x80000000ul

#define TW_PAGE_SIZE	4096

#define TW_BYTE_ORDER	1234
#define TW_LITTLE_ENDIAN	1234
#define TW_BIG_ENDIAN	4321

#endif /* _TW_SIZES_H */
