
#ifndef _TW_DATATYPES_H
#define _TW_DATATYPES_H

typedef   signed char  sbyte;
typedef unsigned char	byte;
typedef   signed short	 dat;
typedef unsigned short	udat;
typedef   signed int    ldat;
typedef unsigned int   uldat;

typedef unsigned char  hwcol;

typedef   signed long  frac_t;

typedef uldat  tobj;

#define MAXU(t)		((t)~(t)0)
#define MINS(t)		((t)((t)1<<(8*sizeof(t)-1)))
#define MAXS(t)		((t)~MINS(t))

#define MINSU(t)	( (t)-1 > (t)0 ? (t)0 : MINS(t) )
#define MAXSU(t)	( (t)-1 > (t)0 ? MAXU(t) : MAXS(t) )
    
#define MAXSBYTE	MAXS(sbyte)
#define MAXBYTE		MAXU(byte)
#define MAXDAT		MAXS(dat)
#define MAXUDAT		MAXU(udat)
#define MAXLDAT		MAXS(ldat)
#define MAXULDAT	MAXU(uldat)

#define MINSBYTE	MINS(sbyte)
#define MINDAT		MINS(dat)
#define MINLDAT		MINS(ldat)

/* this is tricky... we don't know signedness nor sizeof(time_t) during preprocess */
#define MINTIME_T	MINSU(time_t)
#define MAXTIME_T	MAXSU(time_t)

#define MAXFRAC_T	MAXS(frac_t)
#define MINFRAC_T	MINS(frac_t)


#define TW_DECL_MAGIC(id) \
	static byte id[10+sizeof(uldat)] = { \
	    10+sizeof(uldat), \
	    sizeof(byte), \
	    sizeof(udat), \
	    sizeof(uldat), \
	    sizeof(hwcol), \
	    sizeof(time_t), \
	    sizeof(frac_t), \
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
