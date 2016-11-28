#ifndef _TW_DATATYPES_H
#define _TW_DATATYPES_H

#ifndef _TW_TWAUTOCONF_H
# include <Tw/autoconf.h>
#endif

#if defined(TW_HAVE_STDDEF_H)
# include <stddef.h> /* for SIZE_T */
#endif


/* common datatypes */
typedef   signed char  sbyte;
typedef unsigned char	byte;
typedef   signed short	 dat;
typedef unsigned short	udat;
typedef   signed int    ldat;
typedef unsigned int   uldat;


/* tobj, topaque, tany */

typedef uldat	        tobj;
typedef size_t       topaque;
typedef size_t          tany;

/* hw* datatypes */

typedef uint8_t        hwcol;
typedef uint16_t      hwfont;
typedef uint32_t      hwattr;

/* miscellaneous constants */

#define TW_NOID		((uldat)0)
#define TW_BADID	((uldat)-1)
#define TW_NOFD		(-1)
#define TW_NOSLOT	TW_MAXULDAT

#ifndef FALSE
# define FALSE	0
#endif
#ifndef TRUE
# define TRUE	(!FALSE)
#endif

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

#endif /* _TW_DATATYPES_H */
