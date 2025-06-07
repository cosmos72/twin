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
typedef size_t tany;

/* tbool */

typedef enum { tfalse, ttrue } tbool;

/* tcolor, trune, tcell */

#ifdef TW_HAVE_STDINT_H
typedef uint16_t tcolor;
typedef uint32_t trune;
#else
typedef udat tcolor;
typedef uldat trune;
#endif

typedef struct {
  byte fg;
  byte bg;
  byte runes[3];
} tcell;

/* miscellaneous types and constants */

/* tcell <-> tcolor+trune conversion */
#define TCELL(col, rune) Tw_make_tcell(col, rune)
#define TCOLOR(cell) Tw_color(cell)
#define TRUNE(cell) Tw_trune(cell)

/* foreground / background colors handling */
/*
 * NOTE: draw.c:DoShadowColor() assumes that
 * TCOL(fg1, bg1) | TCOL(fg2, bg2) == TCOL(fg1|fg2, bg1|bg2)
 * and
 * TCOL(fg1, bg1) & TCOL(fg2, bg2) == TCOL(fg1&fg2, bg1&bg2)
 */
#define TCOL(fg, bg) Tw_make_tcolor(fg, bg)
#define TCOLBG(col) ((byte)((col) >> 8))
#define TCOLFG(col) ((byte)(col))

#define TW_NOID ((uldat)0)
#define TW_BADID ((uldat) - 1)
#define TW_NOFD (-1)
#define TW_NOSLOT TW_MAXULDAT

#define TW_DECL_MAGIC(id)                                                                          \
  static byte id[10 + sizeof(uldat)] = {                                                           \
      10 + sizeof(uldat), sizeof(byte), sizeof(udat),  sizeof(uldat), sizeof(tcolor),              \
      sizeof(topaque),    sizeof(tany), sizeof(trune), sizeof(tcell), 0}

#define TCELL_COLMASK(cell) ((cell) & 0xFF000000)
#define TCELL_FONTMASK(cell) ((cell) & 0x00FFFFFF)
#define TRUNEEXTRA(cell) ((cell) & 0x00FFFFFF)

TW_INLINE trune Tw_trune(tcell cell) {
  trune ret = 0;
#ifdef TW_WORDS_BIGENDIAN
  memcpy((byte *)&ret + 1, cell.runes, 3);
#else
  memcpy(&ret, cell.runes, 3);
#endif
  return ret;
}

TW_INLINE Tw_color(tcell cell) {
  return cell.fg | (tcolor)cell.bg << 8;
}

TW_INLINE tcell Tw_make_tcell(tcolor col, trune rune) {
  tcell cell;
  cell.fg = TCOLFG(col);
  cell.bg = TCOLBG(col);
#ifdef TW_WORDS_BIGENDIAN
  memcpy(cell.runes, (const byte *)&rune + 1, 3);
#else
  memcpy(cell.runes, &rune, 3);
#endif
  return cell;
}

TW_INLINE tcolor Tw_make_tcolor(byte fg, byte bg) {
  return fg | (tcolor)bg << 8;
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
