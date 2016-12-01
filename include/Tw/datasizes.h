#ifndef _TW_DATASIZES_H
#define _TW_DATASIZES_H

#ifndef _TW_TWAUTOCONF_H
# include <Tw/autoconf.h>
#endif

#if defined(TW_HAVE_LIMITS_H)
# include <limits.h> /* for CHAR_BIT and (U|S)?(CHAR|SHRT|INT)_(MIN|MAX) */
#endif

#ifdef TW_WORDS_BIGENDIAN
# define TW_IS_LITTLE_ENDIAN 0
# define TW_IS_BIG_ENDIAN    1
#else
# define TW_IS_LITTLE_ENDIAN 1
# define TW_IS_BIG_ENDIAN    0
#endif

#ifdef CHAR_BIT
# define TW_CHAR_BIT CHAR_BIT
#else
# define TW_CHAR_BIT 8 /* guess and hope... */
#endif

#define TW_MAXU(t)	((t)~(t)0)
#define TW_MINS(t)	((t)((t)1<<(TW_CHAR_BIT*sizeof(t)-1)))
#define TW_MAXS(t)	((t)~TW_MINS(t))


#define TW_SIZEOF_BYTE		TW_SIZEOF_CHAR
#define TW_SIZEOF_SBYTE		TW_SIZEOF_CHAR
#define TW_SIZEOF_DAT		TW_SIZEOF_SHORT
#define TW_SIZEOF_UDAT		TW_SIZEOF_SHORT
#define TW_SIZEOF_LDAT		TW_SIZEOF_INT
#define TW_SIZEOF_ULDAT		TW_SIZEOF_INT
#define TW_SIZEOF_TOBJ		TW_SIZEOF_INT
#define TW_SIZEOF_TOPAQUE	TW_SIZEOF_SIZE_T
#define TW_SIZEOF_TANY		TW_SIZEOF_SIZE_T
#define TW_SIZEOF_HWCOL		1
#define TW_SIZEOF_HWFONT	2
#define TW_SIZEOF_HWATTR	4


#ifdef UCHAR_MAX
# define TW_MAXBYTE	UCHAR_MAX
# define TW_MAXUDAT	USHRT_MAX
# define TW_MAXULDAT	UINT_MAX

# define TW_MINSBYTE	SCHAR_MIN
# define TW_MAXSBYTE	SCHAR_MAX
# define TW_MINDAT	SHRT_MIN
# define TW_MAXDAT	SHRT_MAX
# define TW_MINLDAT	INT_MIN
# define TW_MAXLDAT	INT_MAX
#else
# define TW_MAXBYTE	TW_MAXU(byte)
# define TW_MAXUDAT	TW_MAXU(udat)
# define TW_MAXULDAT	TW_MAXU(uldat)

# define TW_MINSBYTE	TW_MINS(sbyte)
# define TW_MAXSBYTE	TW_MAXS(sbyte)
# define TW_MINDAT	TW_MINS(dat)
# define TW_MAXDAT	TW_MAXS(dat)
# define TW_MINLDAT	TW_MINS(ldat)
# define TW_MAXLDAT	TW_MAXS(ldat)
#endif

#ifdef SIZE_MAX
# define TW_MAXTOBJ	TW_MAXULDAT
# define TW_MAXTOPAQUE	SIZE_MAX
# define TW_MAXTANY	SIZE_MAX
#else
# define TW_MAXTOBJ	TW_MAXULDAT
# define TW_MAXTOPAQUE	TW_MAXU(topaque)
# define TW_MAXTANY	TW_MAXU(tany)
#endif

#define TW_MAXWCOL      0xFF
#define TW_MAXWFONT     0xFFFF
#define TW_MAXWATTR     0xFFFFFFFF

#define HWATTR(col,ascii)     HWATTR32(col,ascii)
#define HWATTR_COLMASK(attr)  HWATTR_COLMASK32(attr)
#define HWATTR_FONTMASK(attr) HWATTR_FONTMASK32(attr)
#define HWCOL(attr)           HWCOL32(attr)
#define HWFONT(attr)          HWFONT32(attr)


#endif /* _TW_DATASIZES_H */
