
#ifndef _TW_DATATYPES_H
#define _TW_DATATYPES_H

#include <Tw/Twautoconf.h>

#ifdef TW_WORDS_BIGENDIAN
# define TW_IS_LITTLE_ENDIAN 0
# define TW_IS_BIG_ENDIAN    1
#else
# define TW_IS_LITTLE_ENDIAN 1
# define TW_IS_BIG_ENDIAN    0
#endif

typedef   signed char  sbyte;
typedef unsigned char	byte;
typedef   signed short	 dat;
typedef unsigned short	udat;
typedef   signed int    ldat;
typedef unsigned int   uldat;

typedef uldat	        tobj;
typedef size_t       topaque;
typedef size_t          tany;

typedef uint8_t        hwcol;
typedef uint16_t      hwfont;
typedef uint32_t      hwattr;

#define TW_SIZEOF_HWCOL        1
#define TW_SIZEOF_HWFONT       2
#define TW_SIZEOF_HWATTR       4

#define TW_MAXHWCOL           0xFF
#define TW_MAXHWFONT          0xFFFF
#define TW_MAXHWATTR          0xFFFFFFFF

#define HWATTR(col,ascii)     HWATTR32(col,ascii)
#define HWATTR_COLMASK(attr)  HWATTR_COLMASK32(attr)
#define HWATTR_FONTMASK(attr) HWATTR_FONTMASK32(attr)
#define HWCOL(attr)           HWCOL32(attr)
#define HWFONT(attr)          HWFONT32(attr)

#define MAXU(t)		((t)~(t)0)
#define MINS(t)		((t)((t)1<<(8*sizeof(t)-1)))
#define MAXS(t)		((t)~MINS(t))

#define MINSU(t)	( (t)-1 > (t)0 ? (t)0 : MINS(t) )
#define MAXSU(t)	( (t)-1 > (t)0 ? MAXU(t) : MAXS(t) )
    
#define MAXBYTE		MAXU(byte)
#define MAXUDAT		MAXU(udat)
#define MAXULDAT	MAXU(uldat)
#define MAXTOPAQUE	MAXU(topaque)
#define MAXTANY		MAXU(tany)

#define MAXSBYTE	MAXS(sbyte)
#define MAXDAT		MAXS(dat)
#define MAXLDAT		MAXS(ldat)

#define MINSBYTE	MINS(sbyte)
#define MINDAT		MINS(dat)
#define MINLDAT		MINS(ldat)




#define TW_DECL_MAGIC(id) \
	static byte id[10+sizeof(uldat)] = { \
	    10+sizeof(uldat), \
	    sizeof(byte), \
	    sizeof(udat), \
	    sizeof(uldat), \
	    sizeof(hwcol), \
	    sizeof(topaque), \
	    sizeof(tany), \
	    sizeof(hwfont), \
	    sizeof(hwattr), \
	    0 \
	}




#define TW_NOID		((uldat)0)
#define TW_BADID	((uldat)-1)
#define TW_NOFD		(-1)
#define TW_NOSLOT	MAXULDAT

#ifndef FALSE
# define FALSE	0
#endif
#ifndef TRUE
# define TRUE	(!FALSE)
#endif

#endif /* _TW_DATATYPES_H */
