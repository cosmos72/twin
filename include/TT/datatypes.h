
#ifndef _TT_DATATYPES_H
#define _TT_DATATYPES_H

/* typedefs for objects and method structures */

/** type == 1-byte, unsigned */
typedef  byte    ttbyte;
/** type == 1-byte, signed */
typedef sbyte    ttchar;
/** type >= 2-bytes, signed */
typedef   dat    ttshort;
/** type >= 2-bytes type, unsigned */
typedef  udat    ttushort;
/** type >= 4-bytes type, signed */
typedef  ldat    ttint;
/** type >= 4-bytes type, unsigned */
typedef uldat    ttuint;

/** type >= 1-byte, unsigned, used to hold characters (possibly including unicode) */
typedef hwfont   ttfont;
/** type >= 1-byte, unsigned, used to hold colors */
typedef hwcol    ttcol;
/** type >= 2-bytes, unsigned, used to hold characters+colors */
typedef hwattr   ttattr;

/** type == sizeof(void *), unsigned, used to hold pointers and (size_t) integers */
typedef topaque  ttopaque;
/** type == MAX2(ttuint,ttopaque), unsigned, used to hold anything */
typedef tany     ttany;

/** alias for (void *) */
typedef void *	 ttpointer;
/** alias for C strings (TT_CONST ttbyte *), almost equal to (const char *) */
typedef TT_CONST ttbyte * ttstring;
/** alias for function pointers (void (*)(void)) */
typedef void (*ttfunction)(void);

#endif /* _TT_DATATYPES_H */

