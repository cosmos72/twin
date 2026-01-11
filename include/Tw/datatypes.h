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
#include <string.h> /* for memcmp(), memcpy() */
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

typedef size_t tany;

/* colors and UTF-32 runes */

#if defined(UINT32_MAX) || defined(uint32_t)
typedef uint32_t trgb;
typedef uint32_t trune;
#else
typedef uldat trgb;
typedef uldat trune;
#endif

typedef struct {
  trgb fg;
  trgb bg;
} tcolor;

typedef struct {
  trune rune;
  tcolor color;
} tcell;

#define TRGB0 ((trgb)0)
#define TRUNE0 ((trune)0)

static const tcolor TCOL0 = {TRGB0, TRGB0};
static const tcell TCELL0 = {TRUNE0, TCOL0};

#define TRGB_MAX ((trgb)0xffffff)
#define TRUNE_MAX ((trune)0x10ffff)

/* intentionally invalid trune: is > TRUNE_MAX */
#define TRUNE_BAD ((trune)~0)

/* intentionally invalid trgb: is > TRGB_MAX */
#define TRGB_BAD ((trgb)~0)

/* intentionally invalid tcolor: fg and bg are > TRGB_MAX */
static const tcolor TCOLOR_BAD = {TRGB_BAD, TRGB_BAD};

/* intentionally invalid tcell: rune is > TRUNE_MAX, fg and bg are > TRGB_MAX */
static const tcell TCELL_BAD = {TRUNE_BAD, TCOLOR_BAD};

/* tcell <-> tcolor+trune conversion */
TW_INLINE tcell TCELL(tcolor col, trune rune) {
  tcell ret = {rune, col};
  return ret;
}

#define TCOLOR(cell) ((cell).color)
#define TRUNE(cell) ((cell).rune)

/* tcolor <-> foreground+background conversion */
TW_INLINE tcolor TCOL(trgb fg, trgb bg) {
  tcolor ret = {fg, bg};
  return ret;
}
#define TCOLFG(col) ((col).fg)
#define TCOLBG(col) ((col).bg)

/**
 * these could become TW_INLINE functions,
 * but they would no longer be usable in constant initialization.
 */
#define TRGB(red, green, blue)                                                                     \
  ((trgb)(((trgb)(byte)(red) << 16) | ((trgb)(byte)(green) << 8) | (trgb)(byte)(blue)))
#define TRGBE(red, green, blue, effects)                                                           \
  ((trgb)(((trgb)(byte)(effects) << 24)((trgb)(byte)(red) << 16) | ((trgb)(byte)(green) << 8) |    \
          (trgb)(byte)(blue)))
#define TRED(rgb) ((byte)((trgb)(rgb) >> 16))
#define TGREEN(rgb) ((byte)((trgb)(rgb) >> 8))
#define TBLUE(rgb) ((byte)(trgb)(rgb))
#define TEFFECTS(rgb) ((byte)((trgb)(rgb) >> 24))

#ifdef __cplusplus
extern "C++" {
TW_INLINE bool operator==(tcolor col1, tcolor col2) {
  return memcmp(&col1, &col2, sizeof(tcolor)) == 0; /* gcc/clang optimize this */
}
TW_INLINE bool operator!=(tcolor col1, tcolor col2) {
  return !(col1 == col2);
}

TW_INLINE bool operator==(const tcell &cell1, const tcell &cell2) {
  return (cell1.rune == cell2.rune) & (cell1.color == cell2.color); /* gcc/clang optimize this */
}
TW_INLINE bool operator!=(const tcell &cell1, const tcell &cell2) {
  return !(cell1 == cell2);
}
} /* extern "C++" */
#endif

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
