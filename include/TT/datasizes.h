
#ifndef _TT_DATASIZES_H
#define _TT_DATASIZES_H

#define TT_SIZEOF_void		0
#define TT_SIZEOF_ttchar	TW_SIZEOF_BYTE
#define TT_SIZEOF_ttbyte	TW_SIZEOF_BYTE
#define TT_SIZEOF_ttshort	TW_SIZEOF_UDAT
#define TT_SIZEOF_ttushort	TW_SIZEOF_UDAT
#define TT_SIZEOF_ttint		TW_SIZEOF_ULDAT
#define TT_SIZEOF_ttuint	TW_SIZEOF_ULDAT
#define TT_SIZEOF_ttcol		TW_SIZEOF_HWCOL
#define TT_SIZEOF_ttfont	TW_SIZEOF_HWFONT
#define TT_SIZEOF_ttattr	TW_SIZEOF_HWATTR
#define TT_SIZEOF_ttopaque	TW_SIZEOF_TOPAQUE
#define TT_SIZEOF_ttany		TW_SIZEOF_TANY
#define TT_SIZEOF_tttype	TW_SIZEOF_TOPAQUE

#define TT_MAX_ttchar		TW_MAXSBYTE
#define TT_MAX_ttbyte		TW_MAXBYTE
#define TT_MAX_ttshort		TW_MAXDAT
#define TT_MAX_ttushort		TW_MAXUDAT
#define TT_MAX_ttint		TW_MAXLDAT
#define TT_MAX_ttuint		TW_MAXULDAT
#define TT_MAX_ttcol		TW_MAXHWCOL
#define TT_MAX_ttfont		TW_MAXHWFONT
#define TT_MAX_ttattr		TW_MAXHWATTR
#define TT_MAX_ttopaque		TW_MAXTOPAQUE
#define TT_MAX_ttany		TW_MAXTANY
#define TT_MAX_tttype		TW_MAXTOPAQUE

#define TT_MIN_ttchar		TW_MINSBYTE
#define TT_MIN_ttshort		TW_MINDAT 
#define TT_MIN_ttint		TW_MINLDAT
#define TT_MIN_ttany		TW_MINTANY

#define TT_MIN_ST(t)		((t)((t)1<<(8*sizeof(t)-1)))
#define TT_MAX_ST(t)		((t)~TT_MIN_ST(t))
#define TT_MAX_UT(t)		((t)~(t)0)

#define TT_MIN_T(t)		( (t)-1 > (t)0 ? (t)0 : TT_MIN_ST(t) )
#define TT_MAX_T(t)		( (t)-1 > (t)0 ? TT_MAX_UT(t) : TT_MAX_ST(t) )

#define TT_PAGE_SIZE		TW_PAGE_SIZE

#define TT_IS_LITTLE_ENDIAN	TW_IS_LITTLE_ENDIAN
#define TT_IS_BIG_ENDIAN	TW_IS_BIG_ENDIAN

#endif /* _TW_DATASIZES_H */
