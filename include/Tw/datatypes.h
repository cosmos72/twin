
#ifndef _TW_DATATYPES_H
#define _TW_DATATYPES_H

typedef   signed char	 num;
typedef unsigned char	byte;
typedef   signed short	 dat;
typedef unsigned short	udat;
typedef   signed int    ldat;
typedef unsigned int   uldat;

typedef unsigned short hwattr;
typedef unsigned char  hwcol;
typedef unsigned char  hwfont;

typedef unsigned int   frac_t;

#define MAXU(t)		((t)~(t)0)
#define MINS(t)		((t)((t)1<<(8*sizeof(t)-1)))
#define MAXS(t)		((t)~MINS(t))

#define MINNUM		MINS(num)
#define MAXNUM		MAXS(num)
#define MAXBYTE		MAXU(byte)
#define MINDAT		MINS(dat)
#define MAXDAT		MAXS(dat)
#define MAXUDAT		MAXU(udat)
#define MINLDAT		MINS(ldat)
#define MAXLDAT		MAXS(ldat)
#define MAXULDAT	MAXU(uldat)

/* this is tricky... we don't know signedness nor sizeof(time_t) during preprocess */
#define MAXTIME_T	( (time_t)-1 > (time_t)0 ? MAXU(time_t) : MAXS(time_t) )
#define MINTIME_T	( (time_t)-1 > (time_t)0 ? (time_t)0 : MINS(time_t) )

#define MAXFRAC_T	MAXS(frac_t)
#define MINFRAC_T	MINS(frac_t)

#endif /* _TW_DATATYPES_H */
