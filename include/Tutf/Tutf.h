/*
 *  Tutf.h  --  include for ASCII/VGA/UTF/UNICODE pseudographics characters database
 *
 *  This library requires libTw headers too (NOT libTw shared library, just the headers)
 * 
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TUTF_H
#define _TUTF_H

#include <Tutf/Tutftypes.h>

#ifdef __cplusplus
 extern "C" {
#endif
#if 0
 }
#endif


/* get the canonical name of a charset */
#define T_MAP(alias)	T_CAT(T_MAP_,alias)

#define T_LIST(alias,EL)	T_CAT(T_LIST_,T_MAP(alias))(EL)
#define T_NLIST(alias,EL)	T_CAT(T_NLIST_,T_MAP(alias))(EL)
#define T_NAME(alias)		T_CAT(T_NAME_,T_MAP(alias))
#define T_UTF(map,name)		T_CAT3(T_,T_MAP(map),name)


#define T_MAP_UTF_16	UTF_16
#define T_MAP_TF_16	UTF_16
#define T_MAP_U6	UTF_16
#define T_MAP_UNICODE	UTF_16
#define T_MAP_ISO_10646_1	UTF_16
#define T_MAP_ISO10646_1	UTF_16
#define T_NAME_UTF_16	"UTF-16"
#define T_ALIASES_UTF_16	"TF-16", "u6", "Unicode", "ISO-10646-1", "ISO10646-1"


#define T_MAP_ANSI_X3_4		ANSI_X3_4
#define T_MAP_ANSI_X3_4_1968	ANSI_X3_4
#define T_MAP_ANSI_X3_4_1986	ANSI_X3_4
#define T_MAP_ISO_646_IRV_1991	ANSI_X3_4
#define T_MAP_ISO646_US		ANSI_X3_4
#define T_MAP_ISO_IR_6		ANSI_X3_4
#define T_MAP_ASCII		ANSI_X3_4
#define T_MAP_US_ASCII		ANSI_X3_4
#define T_MAP_US		ANSI_X3_4
#define T_MAP_367		ANSI_X3_4
#define T_MAP_CP367		ANSI_X3_4
#define T_MAP_IBM367		ANSI_X3_4
#define T_NAME_ANSI_X3_4	"ANSI_X3.4"
#define T_ALIASES_ANSI_X3_4	"ANSI_X3.4-1968", "ANSI_X3.4-1986", "ISO_646.irv:1991", "ISO646-US", "iso-ir-6", "ASCII", "US-ASCII", "us", "367", "cp367", "IBM367"


#define T_MAP_ISO_8859_1	ISO_8859_1
#define T_MAP_ISO_8859_1_1987	ISO_8859_1
#define T_MAP_ISO8859_1		ISO_8859_1
#define T_MAP_819		ISO_8859_1
#define T_MAP_CP819		ISO_8859_1
#define T_MAP_IBM819		ISO_8859_1
#define T_MAP_ISO_IR_100	ISO_8859_1
#define T_MAP_L1		ISO_8859_1
#define T_MAP_LAT1		ISO_8859_1
#define T_MAP_LATIN1		ISO_8859_1
#define T_MAP_LATIN_1		ISO_8859_1
#define T_NAME_ISO_8859_1	"ISO-8859-1"
#define T_ALIASES_ISO_8859_1	"ISO_8859-1:1987", "ISO8859-1", "819", "CP819", "IBM819", "iso-ir-100", "l1", "lat1", "latin1", "Latin-1"


#define T_MAP_ISO_8859_2	ISO_8859_2
#define T_MAP_ISO_8859_2_1987	ISO_8859_2
#define T_MAP_ISO8859_2		ISO_8859_2
#define T_MAP_912		ISO_8859_2
#define T_MAP_CP912		ISO_8859_2
#define T_MAP_IBM912		ISO_8859_2
#define T_MAP_ISO_IR_101	ISO_8859_2
#define T_MAP_L2		ISO_8859_2
#define T_MAP_LATIN2		ISO_8859_2
#define T_NAME_ISO_8859_2	"ISO-8859-2"
#define T_ALIASES_ISO_8859_2	"ISO_8859-2:1987", "ISO8859-2", "912", "CP912", "IBM912", "iso-ir-101", "l2", "latin2"


#define T_MAP_ISO_8859_3	ISO_8859_3
#define T_MAP_ISO_8859_3_1988	ISO_8859_3
#define T_MAP_ISO8859_3		ISO_8859_3
#define T_MAP_ISO_IR_109	ISO_8859_3
#define T_MAP_L3		ISO_8859_3
#define T_MAP_LATIN3		ISO_8859_3
#define T_NAME_ISO_8859_3	"ISO-8859-3"
#define T_ALIASES_ISO_8859_3	"ISO_8859-3:1988", "ISO8859-3", "iso-ir-109", "l3", "latin3"


#define T_MAP_ISO_8859_4	ISO_8859_4
#define T_MAP_ISO_8859_4_1988	ISO_8859_4
#define T_MAP_ISO8859_4		ISO_8859_4
#define T_MAP_ISO_IR_110	ISO_8859_4
#define T_MAP_L4		ISO_8859_4
#define T_MAP_LATIN4		ISO_8859_4
#define T_NAME_ISO_8859_4	"ISO-8859-4"
#define T_ALIASES_ISO_8859_4	"ISO_8859-4:1988", "ISO8859-4", "iso-ir-109", "l4", "latin4"


#define T_MAP_ISO_8859_5	ISO_8859_5
#define T_MAP_ISO_8859_5_1988	ISO_8859_5
#define T_MAP_ISO8859_5		ISO_8859_5
#define T_MAP_ISO_IR_144	ISO_8859_5
#define T_MAP_CYRILLIC		ISO_8859_5
#define T_NAME_ISO_8859_5	"ISO-8859-5"
#define T_ALIASES_ISO_8859_5	"ISO_8859-5:1988", "ISO8859-5", "iso-ir-144", "cyrillic"


#define T_MAP_ISO_8859_6	ISO_8859_6
#define T_MAP_ISO_8859_6_1987	ISO_8859_6
#define T_MAP_ISO8859_6		ISO_8859_6
#define T_MAP_ARABIC		ISO_8859_6
#define T_MAP_ASMO_708		ISO_8859_6
#define T_MAP_ECMA_114		ISO_8859_6
#define T_MAP_ISO_IR_127	ISO_8859_6
#define T_NAME_ISO_8859_6	"ISO-8859-6"
#define T_ALIASES_ISO_8859_6	"ISO_8859-6:1987", "ISO8859-6", "arabic", "ASMO-708", "ECMA-114", "iso-ir-127"

#define T_MAP_ISO_8859_7	ISO_8859_7
#define T_MAP_ISO_8859_7_1987	ISO_8859_7
#define T_MAP_ISO8859_7		ISO_8859_7
#define T_MAP_GREEK		ISO_8859_7
#define T_MAP_GREEK8		ISO_8859_7
#define T_MAP_ISO_IR_126	ISO_8859_7
#define T_MAP_ELOT_928		ISO_8859_7
#define T_MAP_ECMA_118		ISO_8859_7
#define T_NAME_ISO_8859_7	"ISO-8859-7"
#define T_ALIASES_ISO_8859_7	"ISO_8859-7:1987", "ISO8859-7", "greek", "greek8", "iso-ir-126", "ECMA-118", "ELOT_928"


#define T_MAP_ISO_8859_8	ISO_8859_8
#define T_MAP_ISO_8859_8_1988	ISO_8859_8
#define T_MAP_ISO8859_8		ISO_8859_8
#define T_MAP_HEBREW		ISO_8859_8
#define T_MAP_ISO_IR_138	ISO_8859_8
#define T_NAME_ISO_8859_8	"ISO-8859-8"
#define T_ALIASES_ISO_8859_8	"ISO_8859-8:1988", "ISO8859-8", "hebrew", "iso-ir-138"


#define T_MAP_ISO_8859_9	ISO_8859_9
#define T_MAP_ISO_8859_9_1989	ISO_8859_9
#define T_MAP_ISO8859_9		ISO_8859_9
#define T_MAP_L5		ISO_8859_9
#define T_MAP_LATIN5		ISO_8859_9
#define T_MAP_ISO_IR_148	ISO_8859_9
#define T_NAME_ISO_8859_9	"ISO-8859-9"
#define T_ALIASES_ISO_8859_9	"ISO_8859-9:1989", "ISO8859-9", "l5", "latin5", "iso-ir-148"


#define T_MAP_IBM437	IBM437
#define T_MAP_437	IBM437
#define T_MAP_CP437	IBM437
#define T_NAME_IBM437	"IBM437"
#define T_ALIASES_IBM437	"437", "CP437"


#define T_MAP_IBM850	IBM850
#define T_MAP_850	IBM850
#define T_MAP_CP850	IBM850
#define T_NAME_IBM850	"IBM850"
#define T_ALIASES_IBM850	"850", "CP850"


#define T_MAP_IBM865	IBM865
#define T_MAP_865	IBM865
#define T_MAP_CP865	IBM865
#define T_NAME_IBM865	"IBM865"
#define T_ALIASES_IBM865	"865", "CP865"


#define T_MAP_CP866	CP866
#define T_MAP_866	CP866
#define T_NAME_CP866	"CP866"
#define T_ALIASES_CP866	"866"



typedef hwfont *Tutf_array;
typedef hwfont (*Tutf_function)(hwfont);




/* get the charset `id' (opaque) from a charset alias */
uldat Tutf_charset_id(TUTF_CONST byte * alias);

/* get the canonical name from the charset `id' */
TUTF_CONST byte *Tutf_charset_name(uldat id);

/* get the canonical name from a charset alias */
TUTF_CONST byte *Tutf_charset_alias(TUTF_CONST byte * alias);




/* return a function capable to translate from UTF-16 to given charset */
Tutf_function Tutf_UTF_16_to_charset_function(uldat id);

/* return the array to translate from given charset to UTF-16 */
Tutf_array Tutf_charset_to_UTF_16_array(uldat id);


/* specific translation functions */
hwfont Tutf_UTF_16_to_ANSI_X3_4(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_1(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_2(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_3(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_4(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_5(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_6(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_7(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_8(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_9(hwfont c);
hwfont Tutf_UTF_16_to_IBM437(hwfont c);
hwfont Tutf_UTF_16_to_IBM850(hwfont c);
hwfont Tutf_UTF_16_to_IBM865(hwfont c);
hwfont Tutf_UTF_16_to_CP866(hwfont c);

/* specific translation arrays */
extern hwfont Tutf_ANSI_X3_4_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_1_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_2_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_3_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_4_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_5_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_6_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_7_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_8_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_9_to_UTF_16[0x100];
extern hwfont Tutf_IBM437_to_UTF_16[0x100];
extern hwfont Tutf_IBM850_to_UTF_16[0x100];
extern hwfont Tutf_IBM865_to_UTF_16[0x100];
extern hwfont Tutf_CP866_to_UTF_16[0x100];

extern hwfont Tutf_IBM437_to_ANSI_X3_4[0x100];


#ifdef __cplusplus
}
#endif

#endif /* _TUTF_H */

