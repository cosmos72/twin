#ifndef _TW_DATASIZES_H
#define _TW_DATASIZES_H

#define TW_SIZEOFBYTE         1
#define TW_SIZEOFUDAT         2
#define TW_SIZEOFULDAT        4
#define TW_SIZEOFTIME_T       4
#define TW_SIZEOFFRAC_T       4
#define TW_SIZEOFTLARGEST     4
#define TW_SIZEOFVOIDP        4

#define TW_MAXNUM      0x7F
#define TW_MAXBYTE     0xFF
#define TW_MAXDAT      0x7FFF
#define TW_MAXUDAT     0xFFFF
#define TW_MAXLDAT     0x7FFFFFFF
#define TW_MAXULDAT    0xFFFFFFFF
#define TW_MAXTIME_T   0x7FFFFFFF
#define TW_MAXFRAC_T   0x7FFFFFFF
#define TW_MAXTLARGEST 0xFFFFFFFF

#define TW_MINNUM      0x80
#define TW_MINDAT      0x8000
#define TW_MINLDAT     0x80000000
#define TW_MINTIME_T   0x80000000
#define TW_MINFRAC_T   0x80000000

#define TW_BYTE16      udat
#define TW_BYTE32      uldat

#define TW_PAGE_SIZE        4096

#define TW_BYTE_ORDER       1234
#define TW_LITTLE_ENDIAN    1234
#define TW_BIG_ENDIAN       4321

#endif /* _TW_DATASIZES_H */
