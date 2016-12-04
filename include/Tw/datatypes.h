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


/* miscellaneous types and constants */

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

/*
 * Notes about the timevalue struct:
 * 
 * it is used to represent both time intervals and absolute times;
 * the ->Seconds is a tany numeric field.
 * DON'T assume tany is 32 bit (or any other arbitrary size)
 * since in 19 Jan 2038 at 04:14:08 any signed, 32 bit tany will overflow.
 * So use sizeof(tany) if you really need.
 * 
 * the ->Fraction is a tany numeric field.
 * As above, DON'T assume tany is 32 bit (or any other arbitrary size)
 * since in the future we may want a finer granularity than the nanosecond one
 * possible with a 32 bit tany.
 * So :
 * 1) use sizeof(tany) if you really need
 * 2) don't assume (tany)1 is a nanosecond (or any other arbitrary time),
 *    but always use the form '1 NanoSECs', '250 MilliSECs + 7 MicroSECs', etc.
 * 3) if you _absolutely_ need to know to what time (tany)1 corresponds,
 *    use this: '1 FullSECs' is the number of (tany)1 intervals in a second.
 * 4) for the moment, the only defined fractions of a second are:
 *    FullSECs, MilliSECs, MicroSECs, NanoSECs.
 *    Others may be added in the future (PicoSECs, FemtoSECs, AttoSECs, ...)
 */
typedef struct s_timevalue  {
    tany Seconds;
    tany Fraction;
} timevalue;

#define THOUSAND	((tany)1000)

#define NanoSEC		1 /* i.e. (tany)1 is a nanosecond */
#define MicroSEC	(THOUSAND * NanoSEC)
#define MilliSEC	(THOUSAND * MicroSEC)
#define FullSEC		(THOUSAND * MilliSEC)


#define NanoSECs	* NanoSEC
#define MicroSECs	* MicroSEC
#define MilliSECs	* MilliSEC
#define FullSECs	* FullSEC

#endif /* _TW_DATATYPES_H */
