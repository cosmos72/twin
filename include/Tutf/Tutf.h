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
#define T_ALIASES_ANSI_X3_4	"ANSI_X3.4-1968", "ANSI_X3.4-1986", "ISO_646.irv:1991", "ISO646-US", "iso-ir-6", "ASCII", "US-ASCII", "us", "367", "cp367", "ibm367"


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
#define T_ALIASES_ISO_8859_1	"ISO_8859-1:1987", "ISO8859-1", "819", "cp819", "ibm819", "iso-ir-100", "l1", "lat1", "latin1", "Latin-1"


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
#define T_ALIASES_ISO_8859_2	"ISO_8859-2:1987", "ISO8859-2", "912", "cp912", "ibm912", "iso-ir-101", "l2", "latin2"


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
#define T_MAP_TURKISH		ISO_8859_9
#define T_NAME_ISO_8859_9	"ISO-8859-9"
#define T_ALIASES_ISO_8859_9	"ISO_8859-9:1989", "ISO8859-9", "l5", "latin5", "iso-ir-148", "turkish"


#define T_MAP_ISO_8859_10	ISO_8859_10
#define T_MAP_ISO8859_10	ISO_8859_10
#define T_NAME_ISO_8859_10	"ISO-8859-10"
#define T_ALIASES_ISO_8859_10	"ISO8859-10"


#define T_MAP_ISO_8859_13	ISO_8859_13
#define T_MAP_ISO8859_13	ISO_8859_13
#define T_MAP_L7		ISO_8859_13
#define T_MAP_LATIN7		ISO_8859_13
#define T_MAP_BALTIC		ISO_8859_13
#define T_MAP_ISO_IR_179A	ISO_8859_13
#define T_NAME_ISO_8859_13	"ISO-8859-13"
#define T_ALIASES_ISO_8859_13	"ISO8859-13", "l7", "latin7", "iso-baltic", "iso-ir-179a"


#define T_MAP_ISO_8859_14	ISO_8859_14
#define T_MAP_ISO8859_14	ISO_8859_14
#define T_MAP_L8		ISO_8859_14
#define T_MAP_LATIN8		ISO_8859_14
#define T_MAP_CELTIC		ISO_8859_14
#define T_MAP_ISO_IR_199	ISO_8859_14
#define T_NAME_ISO_8859_14	"ISO-8859-14"
#define T_ALIASES_ISO_8859_14	"ISO8859-14", "l8", "latin8", "iso-celtic", "iso-ir-199"


#define T_MAP_ISO_8859_15	ISO_8859_15
#define T_MAP_ISO8859_15	ISO_8859_15
#define T_MAP_L9		ISO_8859_15
#define T_MAP_LATIN9		ISO_8859_15
#define T_MAP_EURO		ISO_8859_15
#define T_MAP_ISO_IR_203	ISO_8859_15
#define T_NAME_ISO_8859_15	"ISO-8859-15"
#define T_ALIASES_ISO_8859_15	"ISO8859-15", "l9", "latin9", "euro", "iso-ir-203"


#define T_MAP_ISO_8859_16	ISO_8859_16
#define T_MAP_ISO8859_16	ISO_8859_16
#define T_NAME_ISO_8859_16	"ISO-8859-16"
#define T_ALIASES_ISO_8859_16	"ISO8859-16"


#define T_MAP_CP437	CP437
#define T_MAP_437	CP437
#define T_MAP_IBM437	CP437
#define T_NAME_CP437	"cp437"
#define T_ALIASES_CP437	"437", "ibm437"


#define T_MAP_CP737	CP737
#define T_MAP_737	CP737
#define T_NAME_CP737	"cp737"
#define T_ALIASES_CP737	"737"


#define T_MAP_CP775	CP775
#define T_MAP_775	CP775
#define T_NAME_CP775	"cp775"
#define T_ALIASES_CP775	"775"


#define T_MAP_CP850	CP850
#define T_MAP_850	CP850
#define T_MAP_IBM850	CP850
#define T_NAME_CP850	"cp850"
#define T_ALIASES_CP850	"850", "ibm850"


#define T_MAP_CP852	CP852
#define T_MAP_852	CP852
#define T_MAP_IBM852	CP852
#define T_MAP_PCL2	CP852
#define T_MAP_PCLATIN2	CP852
#define T_NAME_CP852	"cp852"
#define T_ALIASES_CP852	"852", "ibm852", "pcl2", "pclatin2"


#define T_MAP_CP855	CP855
#define T_MAP_855	CP855
#define T_MAP_IBM855	CP855
#define T_NAME_CP855	"cp855"
#define T_ALIASES_CP855	"855", "ibm855"


#define T_MAP_CP857	CP857
#define T_MAP_857	CP857
#define T_MAP_IBM857	CP857
#define T_NAME_CP857	"cp857"
#define T_ALIASES_CP857	"857", "ibm857"


#define T_MAP_CP860	CP860
#define T_MAP_860	CP860
#define T_MAP_PORTUGUESE CP860
#define T_NAME_CP860	"cp860"
#define T_ALIASES_CP860	"860", "portuguese"


#define T_MAP_CP861	CP861
#define T_MAP_861	CP861
#define T_MAP_ICELANDIC CP861
#define T_NAME_CP861	"cp861"
#define T_ALIASES_CP861	"861", "icelandic"


#define T_MAP_CP862	CP862
#define T_MAP_862	CP862
#define T_NAME_CP862	"cp862"
#define T_ALIASES_CP862	"862"


#define T_MAP_CP863	CP863
#define T_MAP_863	CP863
#define T_NAME_CP863	"cp863"
#define T_ALIASES_CP863	"863"


#define T_MAP_CP864	CP864
#define T_MAP_864	CP864
#define T_NAME_CP864	"cp864"
#define T_ALIASES_CP864	"864"


#define T_MAP_CP865	CP865
#define T_MAP_865	CP865
#define T_MAP_IBM865	CP865
#define T_MAP_NORWEGIAN	CP865
#define T_MAP_DANISH	CP865
#define T_NAME_CP865	"cp865"
#define T_ALIASES_CP865	"865", "ibm865", "norwegian", "danish"


#define T_MAP_CP866	CP866
#define T_MAP_866	CP866
#define T_NAME_CP866	"cp866"
#define T_ALIASES_CP866	"866"


#define T_MAP_CP869	CP869
#define T_MAP_869	CP869
#define T_NAME_CP869	"cp869"
#define T_ALIASES_CP869	"869"


#define T_MAP_CP874	CP874
#define T_MAP_874	CP874
#define T_MAP_THAI	CP874
#define T_NAME_CP874	"cp874"
#define T_ALIASES_CP874	"874", "thai"

#define T_MAP_CP1250	CP1250
#define T_MAP_1250	CP1250
#define T_MAP_MS_EE	CP1250
#define T_MAP_WINDOWS_1250	CP1250
#define T_MAP_SLAVIC	CP1250
#define T_NAME_CP1250	"cp1250"
#define T_ALIASES_CP1250	"1250", "ms-ee", "windows-1250", "slavic"


#define T_MAP_CP1251	CP1251
#define T_MAP_1251	CP1251
#define T_MAP_MS_CYRL	CP1251
#define T_MAP_WINDOWS_1251	CP1251
#define T_MAP_BULGARIAN	CP1251
#define T_NAME_CP1251	"cp1251"
#define T_ALIASES_CP1251	"1251", "ms-cyrl", "windows-1251", "bulgarian"


#define T_MAP_CP1252	CP1252
#define T_MAP_1252	CP1252
#define T_MAP_WINDOWS_1252	CP1252
#define T_NAME_CP1252	"cp1252"
#define T_ALIASES_CP1252	"1252", "windows-1252"


#define T_MAP_CP1253	CP1253
#define T_MAP_1253	CP1253
#define T_MAP_WINDOWS_1253	CP1253
#define T_NAME_CP1253	"cp1253"
#define T_ALIASES_CP1253	"1253", "windows-1253"


#define T_MAP_CP1254	CP1254
#define T_MAP_1254	CP1254
#define T_MAP_WINDOWS_1254	CP1254
#define T_NAME_CP1254	"cp1254"
#define T_ALIASES_CP1254	"1254", "windows-1254"


#define T_MAP_CP1255	CP1255
#define T_MAP_1255	CP1255
#define T_MAP_WINDOWS_1255	CP1255
#define T_NAME_CP1255	"cp1255"
#define T_ALIASES_CP1255	"1255", "windows-1255"


#define T_MAP_CP1256	CP1256
#define T_MAP_1256	CP1256
#define T_MAP_WINDOWS_1256	CP1256
#define T_NAME_CP1256	"cp1256"
#define T_ALIASES_CP1256	"1256", "windows-1256"


#define T_MAP_CP1257	CP1257
#define T_MAP_1257	CP1257
#define T_MAP_WINDOWS_1257	CP1257
#define T_NAME_CP1257	"cp1257"
#define T_ALIASES_CP1257	"1257", "windows-1257"


#define T_MAP_KOI8_R	KOI8_R
#define T_NAME_KOI8_R	"koi8-r"
#define T_ALIASES_KOI8_R


#define T_MAP_KOI8_U	KOI8_U
#define T_NAME_KOI8_U	"koi8-u"
#define T_ALIASES_KOI8_U



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
hwfont Tutf_UTF_16_to_ISO_8859_10(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_13(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_14(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_15(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_16(hwfont c);

hwfont Tutf_UTF_16_to_CP437(hwfont c);
hwfont Tutf_UTF_16_to_CP737(hwfont c);
hwfont Tutf_UTF_16_to_CP775(hwfont c);
hwfont Tutf_UTF_16_to_CP850(hwfont c);
hwfont Tutf_UTF_16_to_CP852(hwfont c);
hwfont Tutf_UTF_16_to_CP855(hwfont c);
hwfont Tutf_UTF_16_to_CP857(hwfont c);
hwfont Tutf_UTF_16_to_CP860(hwfont c);
hwfont Tutf_UTF_16_to_CP861(hwfont c);
hwfont Tutf_UTF_16_to_CP862(hwfont c);
hwfont Tutf_UTF_16_to_CP863(hwfont c);
hwfont Tutf_UTF_16_to_CP864(hwfont c);
hwfont Tutf_UTF_16_to_CP865(hwfont c);
hwfont Tutf_UTF_16_to_CP866(hwfont c);
hwfont Tutf_UTF_16_to_CP869(hwfont c);
hwfont Tutf_UTF_16_to_CP874(hwfont c);

hwfont Tutf_UTF_16_to_CP1250(hwfont c);
hwfont Tutf_UTF_16_to_CP1251(hwfont c);
hwfont Tutf_UTF_16_to_CP1252(hwfont c);
hwfont Tutf_UTF_16_to_CP1253(hwfont c);
hwfont Tutf_UTF_16_to_CP1254(hwfont c);
hwfont Tutf_UTF_16_to_CP1255(hwfont c);
hwfont Tutf_UTF_16_to_CP1256(hwfont c);
hwfont Tutf_UTF_16_to_CP1257(hwfont c);

hwfont Tutf_UTF_16_to_KOI8_R(hwfont c);
hwfont Tutf_UTF_16_to_KOI8_U(hwfont c);

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
extern hwfont Tutf_ISO_8859_10_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_13_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_14_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_15_to_UTF_16[0x100];
extern hwfont Tutf_ISO_8859_16_to_UTF_16[0x100];

extern hwfont Tutf_CP437_to_UTF_16[0x100];
extern hwfont Tutf_CP737_to_UTF_16[0x100];
extern hwfont Tutf_CP775_to_UTF_16[0x100];
extern hwfont Tutf_CP850_to_UTF_16[0x100];
extern hwfont Tutf_CP852_to_UTF_16[0x100];
extern hwfont Tutf_CP855_to_UTF_16[0x100];
extern hwfont Tutf_CP857_to_UTF_16[0x100];
extern hwfont Tutf_CP860_to_UTF_16[0x100];
extern hwfont Tutf_CP861_to_UTF_16[0x100];
extern hwfont Tutf_CP862_to_UTF_16[0x100];
extern hwfont Tutf_CP863_to_UTF_16[0x100];
extern hwfont Tutf_CP864_to_UTF_16[0x100];
extern hwfont Tutf_CP865_to_UTF_16[0x100];
extern hwfont Tutf_CP866_to_UTF_16[0x100];
extern hwfont Tutf_CP869_to_UTF_16[0x100];
extern hwfont Tutf_CP874_to_UTF_16[0x100];

extern hwfont Tutf_CP1250_to_UTF_16[0x100];
extern hwfont Tutf_CP1251_to_UTF_16[0x100];
extern hwfont Tutf_CP1252_to_UTF_16[0x100];
extern hwfont Tutf_CP1253_to_UTF_16[0x100];
extern hwfont Tutf_CP1254_to_UTF_16[0x100];
extern hwfont Tutf_CP1255_to_UTF_16[0x100];
extern hwfont Tutf_CP1256_to_UTF_16[0x100];
extern hwfont Tutf_CP1257_to_UTF_16[0x100];

extern hwfont Tutf_KOI8_R_to_UTF_16[0x100];
extern hwfont Tutf_KOI8_U_to_UTF_16[0x100];

extern hwfont Tutf_CP437_to_ANSI_X3_4[0x100];


#ifdef __cplusplus
}
#endif

#endif /* _TUTF_H */

