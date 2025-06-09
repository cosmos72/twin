#ifndef TW_DATATYPES_H
#define TW_DATATYPES_H

#ifndef TWTWAUTOCONF_H
#include <Tw/autoconf.h>
#endif

#if defined(TW_HAVE_STDDEF_H)
#include <stddef.h> /* for SIZE_T */
#endif

#if defined(TW_HAVE_STDINT_H)
#include <stdint.h> /* for uint8_t, uint32_t */
#endif

#ifdef TW_HAVE_STRING_H
#include <string.h> /* for memcpy() */
#endif

#include <Tw/compiler.h> // for TW_INLINE

#ifdef __cplusplus
extern "C" {
#endif

/* common datatypes */
typedef signed char sbyte;
typedef unsigned char byte;
typedef signed short dat;
typedef unsigned short udat;
typedef signed int ldat;
typedef unsigned int uldat;

/* tobj, topaque, tany */

typedef uldat tobj;
typedef size_t topaque;

/* tbool */

typedef enum { tfalse, ttrue } tbool;

/* trgb, tcolor, trune, tcell, tany */

#if defined(UINT32_MAX) || defined(uint32_t)
typedef uint32_t trgb;
typedef uint32_t trune;
#else
typedef uldat trgb;
typedef uldat trune;
#endif

#if defined(UINT64_MAX) || defined(uint64_t)

typedef uint64_t tcolor; /* pair: trgb fg; trgb bg */
typedef uint64_t tcell;
#if defined(TW_SIZEOF_SIZE_T) && TW_SIZEOF_SIZE_T < 8
typedef uint64_t tany;
#else
typedef size_t tany;
#endif

#elif defined(TW_SIZEOF_UNSIGNED_LONG_LONG) && TW_SIZEOF_UNSIGNED_LONG_LONG >= 8

typedef unsigned long long tcolor; /* pair: trgb fg; trgb bg */
typedef unsigned long long tcell;
#if defined(TW_SIZEOF_SIZE_T) && TW_SIZEOF_SIZE_T < TW_SIZEOF_UNSIGNED_LONG_LONG
typedef unsigned long long tany;
#else
typedef size_t tany;
#endif

#else
#error "no 64-bit unsigned integer type found"
#endif

/* miscellaneous types and constants */

/* tcell <-> tcolor+trune conversion */
TW_INLINE tcell TCELL(tcolor col, trune rune) {
  return (tcell)col << 22 | rune;
}
TW_INLINE tcolor TCOLOR(tcell cell) {
  return (tcolor)(cell >> 22);
}
TW_INLINE trune TRUNE(tcell cell) {
  return (trune)(cell & 0x3FFFFF);
}

/* foreground / background colors handling */
/*
 * NOTE: draw.c:DoShadowColor() assumes that
 * TCOL(fg1, bg1) | TCOL(fg2, bg2) == TCOL(fg1|fg2, bg1|bg2)
 * and
 * TCOL(fg1, bg1) & TCOL(fg2, bg2) == TCOL(fg1&fg2, bg1&bg2)
 */
TW_INLINE tcolor TCOL(trgb fg, trgb bg) {
  return (tcolor)fg | (tcolor)bg << 21;
}
TW_INLINE trgb TCOLBG(tcolor col) {
  return (trgb)(col >> 21) & 0x1fffff;
}
TW_INLINE trgb TCOLFG(tcolor col) {
  return (trgb)(col) & 0x1fffff;
}

TW_INLINE trgb TRGB(byte red, byte green, byte blue) {
  return (trgb)(red & 0xFE) << 13 | (trgb)(green & 0xFE) << 6 | (trgb)(blue & 0xFE) >> 1;
}

TW_INLINE byte TRED(trgb rgb) {
  byte red = (rgb >> 13) & 0xFE;
  return red | red >> 7;
}
TW_INLINE byte TGREEN(trgb rgb) {
  byte green = (rgb >> 6) & 0xFE;
  return green | green >> 7;
}
TW_INLINE byte TBLUE(trgb rgb) {
  byte blue = (rgb << 1) & 0xFE;
  return blue | blue >> 7;
}

TW_INLINE tcell TCELL_COLMASK(tcell cell) {
  return cell & 0xFFFFFFFFFFC00000ull;
}
TW_INLINE tcell TCELL_RUNEMASK(tcell cell) {
  return cell & 0x3FFFFF;
}

#define TW_NOID ((uldat)0)
#define TW_BADID ((uldat) - 1)
#define TW_NOFD (-1)
#define TW_NOSLOT TW_MAXULDAT

#define TW_DECL_MAGIC(id)                                                                          \
  static byte id[10 + sizeof(uldat)] = {                                                           \
      10 + sizeof(uldat), sizeof(byte), sizeof(udat),  sizeof(uldat), sizeof(tcolor),              \
      sizeof(topaque),    sizeof(tany), sizeof(trune), sizeof(tcell), 0}

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
typedef struct s_timevalue {
  tany Seconds;
  tany Fraction;
} timevalue;

#define THOUSAND ((tany)1000)

#define NanoSEC 1 /* i.e. (tany)1 is a nanosecond */
#define MicroSEC (THOUSAND * NanoSEC)
#define MilliSEC (THOUSAND * MicroSEC)
#define FullSEC (THOUSAND * MilliSEC)

#define NanoSECs *NanoSEC
#define MicroSECs *MicroSEC
#define MilliSECs *MilliSEC
#define FullSECs *FullSEC

#ifdef __cplusplus
}
#endif

#endif /* TW_DATATYPES_H */
