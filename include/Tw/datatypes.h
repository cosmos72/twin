
#ifndef _TW_DATATYPES_H
#define _TW_DATATYPES_H

typedef   signed char	 num;
typedef unsigned char	byte;
typedef   signed short	 dat;
typedef unsigned short	udat;
typedef   signed int    ldat;
typedef unsigned int   uldat;

typedef unsigned char  hwcol;

typedef   signed int   frac_t;

/* the biggest data type used byt libTw. usually will be same as uldat */
typedef unsigned int   tlargest;

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

#define MAXTLARGEST     MAXU(tlargest)




#define TW_DECL_MAGIC(id) \
	static TW_CONST byte id[10+sizeof(uldat)] = { \
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


#endif /* _TW_DATATYPES_H */
