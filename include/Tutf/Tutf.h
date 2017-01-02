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
#define T_UTF(alias,name)	T_CAT3(T_,T_MAP(alias),name)


#define T_MAP_UTF_16	UTF_16
#define T_MAP_TF_16	UTF_16
#define T_MAP_U6	UTF_16
#define T_MAP_UNICODE	UTF_16
#define T_MAP_ISO_10646_1	UTF_16
#define T_MAP_ISO10646_1	UTF_16
#define T_NAME_UTF_16	"UTF-16"
#define T_ALIASES_UTF_16	"TF-16", "u6", "Unicode", "ISO-10646-1", "ISO10646-1"


#define T_MAP_ANSI_X3_4		ASCII
#define T_MAP_ANSI_X3_4_1968	ASCII
#define T_MAP_ANSI_X3_4_1986	ASCII
#define T_MAP_ASCII		ASCII
#define T_MAP_ISO_646_IRV_1991	ASCII
#define T_MAP_ISO646_US		ASCII
#define T_MAP_ISO_IR_6		ASCII
#define T_MAP_US_ASCII		ASCII
#define T_MAP_US		ASCII
#define T_MAP_367		ASCII
#define T_MAP_CP367		ASCII
#define T_MAP_IBM367		ASCII
#define T_NAME_ASCII		"ASCII"
#define T_ALIASES_ASCII		"ANSI_X3.4" "ANSI_X3.4-1968", "ANSI_X3.4-1986", "ISO_646.irv:1991", "ISO646-US", "iso-ir-6", "US-ASCII", "us", "367", "cp367", "ibm367"


/*******************  ISO-8859-x  charsets  *************/

#define T_MAP_ISO_8859_1	ISO8859_1
#define T_MAP_ISO_8859_1_1987	ISO8859_1
#define T_MAP_ISO8859_1		ISO8859_1
#define T_MAP_819		ISO8859_1
#define T_MAP_CP819		ISO8859_1
#define T_MAP_IBM819		ISO8859_1
#define T_MAP_ISO_IR_100	ISO8859_1
#define T_MAP_L1		ISO8859_1
#define T_MAP_LAT1		ISO8859_1
#define T_MAP_LATIN1		ISO8859_1
#define T_MAP_LATIN_1		ISO8859_1
#define T_NAME_ISO8859_1	"ISO-8859-1"
#define T_ALIASES_ISO8859_1	"ISO_8859-1:1987", "ISO8859-1", "819", "cp819", "ibm819", "iso-ir-100", "l1", "lat1", "latin1", "Latin-1"


#define T_MAP_ISO_8859_2	ISO8859_2
#define T_MAP_ISO_8859_2_1987	ISO8859_2
#define T_MAP_ISO8859_2		ISO8859_2
#define T_MAP_912		ISO8859_2
#define T_MAP_CP912		ISO8859_2
#define T_MAP_IBM912		ISO8859_2
#define T_MAP_ISO_IR_101	ISO8859_2
#define T_MAP_L2		ISO8859_2
#define T_MAP_LATIN2		ISO8859_2
#define T_NAME_ISO8859_2	"ISO-8859-2"
#define T_ALIASES_ISO8859_2	"ISO_8859-2:1987", "ISO8859-2", "912", "cp912", "ibm912", "iso-ir-101", "l2", "latin2"


#define T_MAP_ISO_8859_3	ISO8859_3
#define T_MAP_ISO_8859_3_1988	ISO8859_3
#define T_MAP_ISO8859_3		ISO8859_3
#define T_MAP_ISO_IR_109	ISO8859_3
#define T_MAP_L3		ISO8859_3
#define T_MAP_LATIN3		ISO8859_3
#define T_NAME_ISO8859_3	"ISO-8859-3"
#define T_ALIASES_ISO8859_3	"ISO_8859-3:1988", "ISO8859-3", "iso-ir-109", "l3", "latin3"


#define T_MAP_ISO_8859_4	ISO8859_4
#define T_MAP_ISO_8859_4_1988	ISO8859_4
#define T_MAP_ISO8859_4		ISO8859_4
#define T_MAP_ISO_IR_110	ISO8859_4
#define T_MAP_L4		ISO8859_4
#define T_MAP_LATIN4		ISO8859_4
#define T_NAME_ISO8859_4	"ISO-8859-4"
#define T_ALIASES_ISO8859_4	"ISO_8859-4:1988", "ISO8859-4", "iso-ir-109", "l4", "latin4"


#define T_MAP_ISO_8859_5	ISO8859_5
#define T_MAP_ISO_8859_5_1988	ISO8859_5
#define T_MAP_ISO8859_5		ISO8859_5
#define T_MAP_ISO_IR_144	ISO8859_5
#define T_MAP_CYRILLIC		ISO8859_5
#define T_NAME_ISO8859_5	"ISO-8859-5"
#define T_ALIASES_ISO8859_5	"ISO_8859-5:1988", "ISO8859-5", "iso-ir-144", "cyrillic"


#define T_MAP_ISO_8859_6	ISO8859_6
#define T_MAP_ISO_8859_6_1987	ISO8859_6
#define T_MAP_ISO8859_6		ISO8859_6
#define T_MAP_ARABIC		ISO8859_6
#define T_MAP_ASMO_708		ISO8859_6
#define T_MAP_ECMA_114		ISO8859_6
#define T_MAP_ISO_IR_127	ISO8859_6
#define T_NAME_ISO8859_6	"ISO-8859-6"
#define T_ALIASES_ISO8859_6	"ISO_8859-6:1987", "ISO8859-6", "arabic", "ASMO-708", "ECMA-114", "iso-ir-127"


#define T_MAP_ISO_8859_7	ISO8859_7
#define T_MAP_ISO_8859_7_1987	ISO8859_7
#define T_MAP_ISO8859_7		ISO8859_7
#define T_MAP_GREEK		ISO8859_7
#define T_MAP_GREEK8		ISO8859_7
#define T_MAP_ISO_IR_126	ISO8859_7
#define T_MAP_ELOT_928		ISO8859_7
#define T_MAP_ECMA_118		ISO8859_7
#define T_NAME_ISO8859_7	"ISO-8859-7"
#define T_ALIASES_ISO8859_7	"ISO_8859-7:1987", "ISO8859-7", "greek", "greek8", "iso-ir-126", "ECMA-118", "ELOT_928"


#define T_MAP_ISO_8859_8	ISO8859_8
#define T_MAP_ISO_8859_8_1988	ISO8859_8
#define T_MAP_ISO8859_8		ISO8859_8
#define T_MAP_HEBREW		ISO8859_8
#define T_MAP_ISO_IR_138	ISO8859_8
#define T_NAME_ISO8859_8	"ISO-8859-8"
#define T_ALIASES_ISO8859_8	"ISO_8859-8:1988", "ISO8859-8", "hebrew", "iso-ir-138"


#define T_MAP_ISO_8859_9	ISO8859_9
#define T_MAP_ISO_8859_9_1989	ISO8859_9
#define T_MAP_ISO8859_9		ISO8859_9
#define T_MAP_L5		ISO8859_9
#define T_MAP_LATIN5		ISO8859_9
#define T_MAP_ISO_IR_148	ISO8859_9
#define T_MAP_TURKISH		ISO8859_9
#define T_NAME_ISO8859_9	"ISO-8859-9"
#define T_ALIASES_ISO8859_9	"ISO_8859-9:1989", "ISO8859-9", "l5", "latin5", "iso-ir-148", "turkish"


#define T_MAP_ISO_8859_10	ISO8859_10
#define T_MAP_ISO8859_10	ISO8859_10
#define T_NAME_ISO8859_10	"ISO-8859-10"
#define T_ALIASES_ISO8859_10	"ISO8859-10"


#define T_MAP_ISO_8859_11	ISO8859_11
#define T_MAP_ISO8859_11	ISO8859_11
#define T_NAME_ISO8859_11	"ISO-8859-11"
#define T_ALIASES_ISO8859_11	"ISO_8859-11:2001", "ISO8859-11"


#define T_MAP_ISO_8859_13	ISO8859_13
#define T_MAP_ISO8859_13	ISO8859_13
#define T_MAP_L7		ISO8859_13
#define T_MAP_LATIN7		ISO8859_13
#define T_MAP_BALTIC		ISO8859_13
#define T_MAP_ISO_IR_179A	ISO8859_13
#define T_NAME_ISO8859_13	"ISO-8859-13"
#define T_ALIASES_ISO8859_13	"ISO8859-13", "l7", "latin7", "iso-baltic", "iso-ir-179a"


#define T_MAP_ISO_8859_14	ISO8859_14
#define T_MAP_ISO8859_14	ISO8859_14
#define T_MAP_L8		ISO8859_14
#define T_MAP_LATIN8		ISO8859_14
#define T_MAP_CELTIC		ISO8859_14
#define T_MAP_ISO_IR_199	ISO8859_14
#define T_NAME_ISO8859_14	"ISO-8859-14"
#define T_ALIASES_ISO8859_14	"ISO8859-14", "l8", "latin8", "iso-celtic", "iso-ir-199"


#define T_MAP_ISO_8859_15	ISO8859_15
#define T_MAP_ISO8859_15	ISO8859_15
#define T_MAP_L9		ISO8859_15
#define T_MAP_LATIN9		ISO8859_15
#define T_MAP_EURO		ISO8859_15
#define T_MAP_ISO_IR_203	ISO8859_15
#define T_NAME_ISO8859_15	"ISO-8859-15"
#define T_ALIASES_ISO8859_15	"ISO8859-15", "l9", "latin9", "euro", "iso-ir-203"


#define T_MAP_ISO_8859_16	ISO8859_16
#define T_MAP_ISO8859_16	ISO8859_16
#define T_NAME_ISO8859_16	"ISO-8859-16"
#define T_ALIASES_ISO8859_16	"ISO_8859-16:2001", "ISO8859-16", "latin10"


/*******************  Codepage / IBM  charsets  *************/

#define T_MAP_CP424	CP424
#define T_MAP_424	CP424
#define T_MAP_IBM424	CP424
#define T_NAME_CP424	"cp424"
#define T_ALIASES_CP424	"424", "ibm424"


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


#define T_MAP_CP856	CP856
#define T_MAP_856	CP856
#define T_MAP_IBM856	CP856
#define T_NAME_CP856	"cp856"
#define T_ALIASES_CP856	"856", "ibm856"


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


#define T_MAP_CP932	CP932
#define T_MAP_932	CP932
#define T_MAP_IBM932	CP932
#define T_NAME_CP932	"cp932"
#define T_ALIASES_CP932	"932", "ibm932"


#define T_MAP_CP936	CP936
#define T_MAP_936	CP936
#define T_MAP_IBM936	CP936
#define T_NAME_CP936	"cp936"
#define T_ALIASES_CP936	"936", "ibm936"


#define T_MAP_CP949	CP949
#define T_MAP_949	CP949
#define T_MAP_IBM949	CP949
#define T_NAME_CP949	"cp949"
#define T_ALIASES_CP949	"949", "ibm949"


#define T_MAP_CP950	CP950
#define T_MAP_950	CP950
#define T_MAP_IBM950	CP950
#define T_NAME_CP950	"cp950"
#define T_ALIASES_CP950	"950", "ibm950"


#define T_MAP_CP1006	CP1006
#define T_MAP_1006	CP1006
#define T_MAP_IBM1006	CP1006
#define T_NAME_CP1006	"cp1006"
#define T_ALIASES_CP1006	"1006", "ibm1006"


#define T_MAP_CP1250	CP1250
#define T_MAP_1250	CP1250
#define T_MAP_MS_EE	CP1250
#define T_MAP_WINDOWS_1250	CP1250
#define T_MAP_SLAVIC	CP1250
#define T_NAME_CP1250	"cp1250"
#define T_ALIASES_CP1250	"1250", "ms-ee", "windows-1250", "slavic", "ms-latin2"


#define T_MAP_CP1251	CP1251
#define T_MAP_1251	CP1251
#define T_MAP_MS_CYRL	CP1251
#define T_MAP_WINDOWS_1251	CP1251
#define T_MAP_BULGARIAN	CP1251
#define T_NAME_CP1251	"cp1251"
#define T_ALIASES_CP1251	"1251", "windows-1251", "bulgarian", "ms-cyrillic", "ms-cyrl"


#define T_MAP_CP1252	CP1252
#define T_MAP_1252	CP1252
#define T_MAP_WINDOWS_1252	CP1252
#define T_NAME_CP1252	"cp1252"
#define T_ALIASES_CP1252	"1252", "windows-1252", "ms-latin1"


#define T_MAP_CP1253	CP1253
#define T_MAP_1253	CP1253
#define T_MAP_WINDOWS_1253	CP1253
#define T_NAME_CP1253	"cp1253"
#define T_ALIASES_CP1253	"1253", "windows-1253", "ms-greek"


#define T_MAP_CP1254	CP1254
#define T_MAP_1254	CP1254
#define T_MAP_WINDOWS_1254	CP1254
#define T_NAME_CP1254	"cp1254"
#define T_ALIASES_CP1254	"1254", "windows-1254", "ms-turkish"


#define T_MAP_CP1255	CP1255
#define T_MAP_1255	CP1255
#define T_MAP_WINDOWS_1255	CP1255
#define T_NAME_CP1255	"cp1255"
#define T_ALIASES_CP1255	"1255", "windows-1255", "ms-hebrew"


#define T_MAP_CP1256	CP1256
#define T_MAP_1256	CP1256
#define T_MAP_WINDOWS_1256	CP1256
#define T_NAME_CP1256	"cp1256"
#define T_ALIASES_CP1256	"1256", "windows-1256", "ms-arabic"


#define T_MAP_CP1257	CP1257
#define T_MAP_1257	CP1257
#define T_MAP_WINDOWS_1257	CP1257
#define T_NAME_CP1257	"cp1257"
#define T_ALIASES_CP1257	"1257", "windows-1257", "baltic-rim", "ms-baltic-rim"


#define T_MAP_CP1258	CP1258
#define T_MAP_1258	CP1258
#define T_MAP_WINDOWS_1258	CP1258
#define T_NAME_CP1258	"cp1258"
#define T_ALIASES_CP1258	"1258", "windows-1258", "vietnamese", "ms-vietnamese"


#define T_MAP_ADOBE_STDENC	ADOBE_STDENC
#define T_MAP_ADOBE_STANDARD	ADOBE_STDENC
#define T_MAP_ADOBE_STANDARD_ENCODING	ADOBE_STDENC
#define T_NAME_ADOBE_STDENC	"adobe-stdenc"
#define T_ALIASES_ADOBE_STDENC  "adobe-standard", "adobe-standard-encoding"


#define T_MAP_ADOBE_SYMBOL	ADOBE_SYMBOL
#define T_MAP_ADOBE_SYMBOL_ENCODING	ADOBE_SYMBOL
#define T_NAME_ADOBE_SYMBOL	"adobe-symbol"
#define T_ALIASES_ADOBE_SYMBOL  "adobe-symbol-encoding"


/******************* APPLE charsets for Mac OS *************/


#define T_MAP_MAC_ARABIC	MAC_ARABIC
#define T_MAP_APPLE_ARABIC	MAC_ARABIC
#define T_NAME_MAC_ARABIC	"mac-arabic"
#define T_ALIASES_MAC_ARABIC	"apple-arabic"


#define T_MAP_MAC_CELTIC	MAC_CELTIC
#define T_MAP_APPLE_CELTIC	MAC_CELTIC
#define T_NAME_MAC_CELTIC	"mac-celtic"
#define T_ALIASES_MAC_CELTIC	"apple-celtic"


#define T_MAP_MAC_CENTRALEUROPE		MAC_CENTRALEUROPE
#define T_MAP_APPLE_CENTRALEUROPE	MAC_CENTRALEUROPE
#define T_NAME_MAC_CENTRALEUROPE	"mac-central-europe"
#define T_ALIASES_MAC_CENTRALEUROPE	"apple-central-europe"


#define T_MAP_MAC_CROATIAN	MAC_CROATIAN
#define T_MAP_APPLE_CROATIAN	MAC_CROATIAN
#define T_NAME_MAC_CROATIAN	"mac-croatian"
#define T_ALIASES_MAC_CROATIAN	"apple-croatian"


#define T_MAP_MAC_CYRILLIC	MAC_CYRILLIC
#define T_MAP_APPLE_CYRILLIC	MAC_CYRILLIC
#define T_NAME_MAC_CYRILLIC	"mac-cyrillic"
#define T_ALIASES_MAC_CYRILLIC	"apple-cyrillic"


#define T_MAP_MAC_DEVANAGARI	MAC_DEVANAGARI
#define T_MAP_APPLE_DEVANAGARI	MAC_DEVANAGARI
#define T_NAME_MAC_DEVANAGARI	"mac-devanagari"
#define T_ALIASES_MAC_DEVANAGARI	"apple-devanagari"


#define T_MAP_MAC_DINGBATS	MAC_DINGBATS
#define T_MAP_APPLE_DINGBATS	MAC_DINGBATS
#define T_NAME_MAC_DINGBATS	"mac-dingbats"
#define T_ALIASES_MAC_DINGBATS	"apple-dingbats"


#define T_MAP_MAC_FARSI		MAC_FARSI
#define T_MAP_APPLE_FARSI	MAC_FARSI
#define T_NAME_MAC_FARSI	"mac-farsi"
#define T_ALIASES_MAC_FARSI	"apple-farsi"


#define T_MAP_MAC_GAELIC	MAC_GAELIC
#define T_MAP_APPLE_GAELIC	MAC_GAELIC
#define T_NAME_MAC_GAELIC	"mac-gaelic"
#define T_ALIASES_MAC_GAELIC	"apple-gaelic"


#define T_MAP_MAC_GREEK		MAC_GREEK
#define T_MAP_APPLE_GREEK	MAC_GREEK
#define T_NAME_MAC_GREEK	"mac-greek"
#define T_ALIASES_MAC_GREEK	"apple-greek"


#define T_MAP_MAC_GUJARATI	MAC_GUJARATI
#define T_MAP_APPLE_GUJARATI	MAC_GUJARATI
#define T_NAME_MAC_GUJARATI	"mac-gujarati"
#define T_ALIASES_MAC_GUJARATI	"apple-gujarati"


#define T_MAP_MAC_GURMUKHI	MAC_GURMUKHI
#define T_MAP_APPLE_GURMUKHI	MAC_GURMUKHI
#define T_NAME_MAC_GURMUKHI	"mac-gurmukhi"
#define T_ALIASES_MAC_GURMUKHI	"apple-gurmukhi"


#define T_MAP_MAC_HEBREW	MAC_HEBREW
#define T_MAP_APPLE_HEBREW	MAC_HEBREW
#define T_NAME_MAC_HEBREW	"mac-hebrew"
#define T_ALIASES_MAC_HEBREW	"apple-hebrew"


#define T_MAP_MAC_ICELANDIC	MAC_ICELANDIC
#define T_MAP_APPLE_ICELANDIC	MAC_ICELANDIC
#define T_NAME_MAC_ICELANDIC	"mac-icelandic"
#define T_ALIASES_MAC_ICELANDIC	"apple-icelandic"


#define T_MAP_MAC_INUIT		MAC_INUIT
#define T_MAP_APPLE_INUIT	MAC_INUIT
#define T_NAME_MAC_INUIT	"mac-inuit"
#define T_ALIASES_MAC_INUIT	"apple-inuit"


#define T_MAP_MAC_ROMANIAN	MAC_ROMANIAN
#define T_MAP_APPLE_ROMANIAN	MAC_ROMANIAN
#define T_NAME_MAC_ROMANIAN	"mac-romanian"
#define T_ALIASES_MAC_ROMANIAN	"apple-romanian"


#define T_MAP_MAC_ROMAN		MAC_ROMAN
#define T_MAP_APPLE_ROMAN	MAC_ROMAN
#define T_NAME_MAC_ROMAN	"mac-roman"
#define T_ALIASES_MAC_ROMAN	"apple-roman"


#define T_MAP_MAC_SYMBOL	MAC_SYMBOL
#define T_MAP_APPLE_SYMBOL	MAC_SYMBOL
#define T_NAME_MAC_SYMBOL	"mac-symbol"
#define T_ALIASES_MAC_SYMBOL	"apple-symbol"


#define T_MAP_MAC_THAI		MAC_THAI
#define T_MAP_APPLE_THAI	MAC_THAI
#define T_NAME_MAC_THAI		"mac-thai"
#define T_ALIASES_MAC_THAI	"apple-thai"


#define T_MAP_MAC_TURKISH	MAC_TURKISH
#define T_MAP_APPLE_TURKISH	MAC_TURKISH
#define T_NAME_MAC_TURKISH	"mac-turkish"
#define T_ALIASES_MAC_TURKISH	"apple-turkish"


/*********** Microsoft charsets for Mac OS ******************/

#define T_MAP_MS_MAC_CYRILLIC		MS_MAC_CYRILLIC
#define T_MAP_CP10007			MS_MAC_CYRILLIC
#define T_NAME_MS_MAC_CYRILLIC		"ms-mac-cyrillic"
#define T_ALIASES_MS_MAC_CYRILLIC	"cp10007"


#define T_MAP_MS_MAC_GREEK		MS_MAC_GREEK
#define T_MAP_CP10006			MS_MAC_GREEK
#define T_NAME_MS_MAC_GREEK		"ms-mac-greek"
#define T_ALIASES_MS_MAC_GREEK		"cp10006"


#define T_MAP_MS_MAC_ICELANDIC		MS_MAC_ICELANDIC
#define T_MAP_CP10079			MS_MAC_ICELANDIC
#define T_NAME_MS_MAC_ICELANDIC		"ms-mac-icelandic"
#define T_ALIASES_MS_MAC_ICELANDIC	"cp10079"


#define T_MAP_MS_MAC_LATIN2		MS_MAC_LATIN2
#define T_MAP_MS_MAC_CENTRAL_EUROPE	MS_MAC_LATIN2
#define T_MAP_CP10029			MS_MAC_LATIN2
#define T_NAME_MS_MAC_LATIN2		"ms-mac-latin2"
#define T_ALIASES_MS_MAC_LATIN2		"ms-mac-central-europe", "cp10029"


#define T_MAP_MS_MAC_ROMAN		MS_MAC_ROMAN
#define T_MAP_CP10000			MS_MAC_ROMAN
#define T_NAME_MS_MAC_ROMAN		"ms-mac-roman"
#define T_ALIASES_MS_MAC_ROMAN		"cp10000"


#define T_MAP_MS_MAC_TURKISH		MS_MAC_TURKISH
#define T_MAP_CP10081			MS_MAC_TURKISH
#define T_NAME_MS_MAC_TURKISH		"ms-mac-turkish"
#define T_ALIASES_MS_MAC_TURKISH	"cp10081"


/*******************  misc  charsets  *************/

#define T_MAP_APL_ISO_IR_68	APL_ISO_IR_68
#define T_MAP_ISO_IR_68		APL_ISO_IR_68
#define T_MAP_IR_68		APL_ISO_IR_68
#define T_NAME_APL_ISO_IR_68	"apl-iso-ir-68"
#define T_ALIASES_APL_ISO_IR_68  "iso-ir-68", "ir-68"


#define T_MAP_ATARIST		ATARIST
#define T_MAP_ATARI_ST		ATARIST
#define T_NAME_ATARIST		"Atari ST"
#define T_ALIASES_ATARIST	"atarist"


#define T_MAP_HP_ROMAN8		HP_ROMAN8
#define T_MAP_HP_ROMAN_8	HP_ROMAN8
#define T_NAME_HP_ROMAN8	"hp-roman8"
#define T_ALIASES_HP_ROMAN8	"hp-roman-8"


#define T_MAP_JISX0201		JISX0201
#define T_MAP_JISC6220		JISX0201
#define T_NAME_JISX0201		"jis-x-0201"
#define T_ALIASES_JISX0201	"jis-c-6220"


#define T_MAP_KZ1048		KZ1048
#define T_MAP_KZ_1048		KZ1048
#define T_MAP_STRK1048		KZ1048
#define T_MAP_STRK1048_2002	KZ1048
#define T_NAME_KZ1048		"KZ-1048"
#define T_ALIASES_KZ1048	"STRK1048", "STRK1048-2002"


#define T_MAP_KOI8_R		KOI8_R
#define T_MAP_CP878		KOI8_R
#define T_MAP_IBM878		KOI8_R
#define T_NAME_KOI8_R		"koi8-r"
#define T_ALIASES_KOI8_R	"cp878", "ibm878"


#define T_MAP_KOI8_U		KOI8_U
#define T_MAP_CP1168		KOI8_U
#define T_MAP_IBM1168		KOI8_U
#define T_NAME_KOI8_U		"koi8-u"
#define T_ALIASES_KOI8_U	"cp1168", "ibm1168"


#define T_MAP_NEXTSTEP		NEXTSTEP
#define T_MAP_NEXT_NEXTSTEP	NEXTSTEP
#define T_NAME_NEXTSTEP		"NextStep"
#define T_ALIASES_NEXTSTEP	"Next-NextStep"


#define T_MAP_VT100		VT100GR
#define T_MAP_VT100GR		VT100GR
#define T_MAP_VT100_GR		VT100GR
#define T_MAP_VT100_GRAPHICS	VT100GR
#define T_NAME_VT100GR		"vt100-graphics"
#define T_ALIASES_VT100GR	"vt100-gr", "vt100"




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

#define Tutf_UTF_16_to_ASCII      	Tutf_UTF_16_to_ANSI_X3_4
#define Tutf_UTF_16_to_ISO8859_1	Tutf_UTF_16_to_ISO_8859_1
#define Tutf_UTF_16_to_ISO8859_2	Tutf_UTF_16_to_ISO_8859_2
#define Tutf_UTF_16_to_ISO8859_3	Tutf_UTF_16_to_ISO_8859_3
#define Tutf_UTF_16_to_ISO8859_4	Tutf_UTF_16_to_ISO_8859_4
#define Tutf_UTF_16_to_ISO8859_5	Tutf_UTF_16_to_ISO_8859_5
#define Tutf_UTF_16_to_ISO8859_6	Tutf_UTF_16_to_ISO_8859_6
#define Tutf_UTF_16_to_ISO8859_7	Tutf_UTF_16_to_ISO_8859_7
#define Tutf_UTF_16_to_ISO8859_8	Tutf_UTF_16_to_ISO_8859_8
#define Tutf_UTF_16_to_ISO8859_9	Tutf_UTF_16_to_ISO_8859_9
#define Tutf_UTF_16_to_ISO8859_10	Tutf_UTF_16_to_ISO_8859_10
#define Tutf_UTF_16_to_ISO8859_11	Tutf_UTF_16_to_ISO_8859_11
#define Tutf_UTF_16_to_ISO8859_13	Tutf_UTF_16_to_ISO_8859_13
#define Tutf_UTF_16_to_ISO8859_14	Tutf_UTF_16_to_ISO_8859_14
#define Tutf_UTF_16_to_ISO8859_15	Tutf_UTF_16_to_ISO_8859_15
#define Tutf_UTF_16_to_ISO8859_16	Tutf_UTF_16_to_ISO_8859_16

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
hwfont Tutf_UTF_16_to_ISO_8859_11(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_13(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_14(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_15(hwfont c);
hwfont Tutf_UTF_16_to_ISO_8859_16(hwfont c);

hwfont Tutf_UTF_16_to_CP424(hwfont c);
hwfont Tutf_UTF_16_to_CP437(hwfont c);
hwfont Tutf_UTF_16_to_CP737(hwfont c);
hwfont Tutf_UTF_16_to_CP775(hwfont c);
hwfont Tutf_UTF_16_to_CP850(hwfont c);
hwfont Tutf_UTF_16_to_CP852(hwfont c);
hwfont Tutf_UTF_16_to_CP855(hwfont c);
hwfont Tutf_UTF_16_to_CP856(hwfont c);
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
hwfont Tutf_UTF_16_to_CP932(hwfont c);
hwfont Tutf_UTF_16_to_CP936(hwfont c);
hwfont Tutf_UTF_16_to_CP949(hwfont c);
hwfont Tutf_UTF_16_to_CP950(hwfont c);
hwfont Tutf_UTF_16_to_CP1006(hwfont c);
hwfont Tutf_UTF_16_to_CP1250(hwfont c);
hwfont Tutf_UTF_16_to_CP1251(hwfont c);
hwfont Tutf_UTF_16_to_CP1252(hwfont c);
hwfont Tutf_UTF_16_to_CP1253(hwfont c);
hwfont Tutf_UTF_16_to_CP1254(hwfont c);
hwfont Tutf_UTF_16_to_CP1255(hwfont c);
hwfont Tutf_UTF_16_to_CP1256(hwfont c);
hwfont Tutf_UTF_16_to_CP1257(hwfont c);
hwfont Tutf_UTF_16_to_CP1258(hwfont c);

hwfont Tutf_UTF_16_to_ADOBE_STDENC(hwfont c);
hwfont Tutf_UTF_16_to_ADOBE_SYMBOL(hwfont c);

hwfont Tutf_UTF_16_to_MAC_ARABIC(hwfont c);
hwfont Tutf_UTF_16_to_MAC_CELTIC(hwfont c);
hwfont Tutf_UTF_16_to_MAC_CENTRALEUROPE(hwfont c);
hwfont Tutf_UTF_16_to_MAC_CROATIAN(hwfont c);
hwfont Tutf_UTF_16_to_MAC_CYRILLIC(hwfont c);
hwfont Tutf_UTF_16_to_MAC_DEVANAGARI(hwfont c);
hwfont Tutf_UTF_16_to_MAC_DINGBATS(hwfont c);
hwfont Tutf_UTF_16_to_MAC_FARSI(hwfont c);
hwfont Tutf_UTF_16_to_MAC_GAELIC(hwfont c);
hwfont Tutf_UTF_16_to_MAC_GREEK(hwfont c);
hwfont Tutf_UTF_16_to_MAC_GUJARATI(hwfont c);
hwfont Tutf_UTF_16_to_MAC_GURMUKHI(hwfont c);
hwfont Tutf_UTF_16_to_MAC_HEBREW(hwfont c);
hwfont Tutf_UTF_16_to_MAC_ICELANDIC(hwfont c);
hwfont Tutf_UTF_16_to_MAC_INUIT(hwfont c);
hwfont Tutf_UTF_16_to_MAC_ROMAN(hwfont c);
hwfont Tutf_UTF_16_to_MAC_ROMANIAN(hwfont c);
hwfont Tutf_UTF_16_to_MAC_SYMBOL(hwfont c);
hwfont Tutf_UTF_16_to_MAC_THAI(hwfont c);
hwfont Tutf_UTF_16_to_MAC_TURKISH(hwfont c);

hwfont Tutf_UTF_16_to_MS_MAC_CYRILLIC(hwfont c);
hwfont Tutf_UTF_16_to_MS_MAC_GREEK(hwfont c);
hwfont Tutf_UTF_16_to_MS_MAC_ICELANDIC(hwfont c);
hwfont Tutf_UTF_16_to_MS_MAC_LATIN2(hwfont c);
hwfont Tutf_UTF_16_to_MS_MAC_ROMAN(hwfont c);
hwfont Tutf_UTF_16_to_MS_MAC_TURKISH(hwfont c);

hwfont Tutf_UTF_16_to_APL_ISO_IR_68(hwfont c);
hwfont Tutf_UTF_16_to_ATARIST(hwfont c);
hwfont Tutf_UTF_16_to_HP_ROMAN8(hwfont c);
hwfont Tutf_UTF_16_to_JISX0201(hwfont c);
hwfont Tutf_UTF_16_to_KZ1048(hwfont c);
hwfont Tutf_UTF_16_to_KOI8_R(hwfont c);
hwfont Tutf_UTF_16_to_KOI8_U(hwfont c);
hwfont Tutf_UTF_16_to_NEXTSTEP(hwfont c);
hwfont Tutf_UTF_16_to_VT100GR(hwfont c);


/* specific translation arrays */

#define Tutf_ASCII_to_UTF_16    	Tutf_ANSI_X3_4_to_UTF_16
#define Tutf_ISO8859_1_to_UTF_16	Tutf_ISO_8859_1_to_UTF_16
#define Tutf_ISO8859_2_to_UTF_16	Tutf_ISO_8859_2_to_UTF_16
#define Tutf_ISO8859_3_to_UTF_16	Tutf_ISO_8859_3_to_UTF_16
#define Tutf_ISO8859_4_to_UTF_16	Tutf_ISO_8859_4_to_UTF_16
#define Tutf_ISO8859_5_to_UTF_16	Tutf_ISO_8859_5_to_UTF_16
#define Tutf_ISO8859_6_to_UTF_16	Tutf_ISO_8859_6_to_UTF_16
#define Tutf_ISO8859_7_to_UTF_16	Tutf_ISO_8859_7_to_UTF_16
#define Tutf_ISO8859_8_to_UTF_16	Tutf_ISO_8859_8_to_UTF_16
#define Tutf_ISO8859_9_to_UTF_16	Tutf_ISO_8859_9_to_UTF_16
#define Tutf_ISO8859_10_to_UTF_16	Tutf_ISO_8859_10_to_UTF_16
#define Tutf_ISO8859_11_to_UTF_16	Tutf_ISO_8859_11_to_UTF_16
#define Tutf_ISO8859_13_to_UTF_16	Tutf_ISO_8859_13_to_UTF_16
#define Tutf_ISO8859_14_to_UTF_16	Tutf_ISO_8859_14_to_UTF_16
#define Tutf_ISO8859_15_to_UTF_16	Tutf_ISO_8859_15_to_UTF_16
#define Tutf_ISO8859_16_to_UTF_16	Tutf_ISO_8859_16_to_UTF_16

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

extern hwfont Tutf_CP424_to_UTF_16[0x100];
extern hwfont Tutf_CP437_to_UTF_16[0x100];
extern hwfont Tutf_CP737_to_UTF_16[0x100];
extern hwfont Tutf_CP775_to_UTF_16[0x100];
extern hwfont Tutf_CP850_to_UTF_16[0x100];
extern hwfont Tutf_CP852_to_UTF_16[0x100];
extern hwfont Tutf_CP855_to_UTF_16[0x100];
extern hwfont Tutf_CP856_to_UTF_16[0x100];
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
extern hwfont Tutf_CP932_to_UTF_16[0x100];
extern hwfont Tutf_CP936_to_UTF_16[0x100];
extern hwfont Tutf_CP949_to_UTF_16[0x100];
extern hwfont Tutf_CP950_to_UTF_16[0x100];
extern hwfont Tutf_CP1006_to_UTF_16[0x100];
extern hwfont Tutf_CP1250_to_UTF_16[0x100];
extern hwfont Tutf_CP1251_to_UTF_16[0x100];
extern hwfont Tutf_CP1252_to_UTF_16[0x100];
extern hwfont Tutf_CP1253_to_UTF_16[0x100];
extern hwfont Tutf_CP1254_to_UTF_16[0x100];
extern hwfont Tutf_CP1255_to_UTF_16[0x100];
extern hwfont Tutf_CP1256_to_UTF_16[0x100];
extern hwfont Tutf_CP1257_to_UTF_16[0x100];
extern hwfont Tutf_CP1258_to_UTF_16[0x100];

extern hwfont Tutf_ADOBE_STDENC_to_UTF_16[0x100];
extern hwfont Tutf_ADOBE_SYMBOL_to_UTF_16[0x100];

extern hwfont Tutf_MAC_ARABIC_to_UTF_16[0x100];
extern hwfont Tutf_MAC_CELTIC_to_UTF_16[0x100];
extern hwfont Tutf_MAC_CENTRALEUROPE_to_UTF_16[0x100];
extern hwfont Tutf_MAC_CROATIAN_to_UTF_16[0x100];
extern hwfont Tutf_MAC_CYRILLIC_to_UTF_16[0x100];
extern hwfont Tutf_MAC_DEVANAGARI_to_UTF_16[0x100];
extern hwfont Tutf_MAC_DINGBATS_to_UTF_16[0x100];
extern hwfont Tutf_MAC_FARSI_to_UTF_16[0x100];
extern hwfont Tutf_MAC_GAELIC_to_UTF_16[0x100];
extern hwfont Tutf_MAC_GREEK_to_UTF_16[0x100];
extern hwfont Tutf_MAC_GUJARATI_to_UTF_16[0x100];
extern hwfont Tutf_MAC_GURMUKHI_to_UTF_16[0x100];
extern hwfont Tutf_MAC_HEBREW_to_UTF_16[0x100];
extern hwfont Tutf_MAC_ICELANDIC_to_UTF_16[0x100];
extern hwfont Tutf_MAC_INUIT_to_UTF_16[0x100];
extern hwfont Tutf_MAC_ROMAN_to_UTF_16[0x100];
extern hwfont Tutf_MAC_ROMANIAN_to_UTF_16[0x100];
extern hwfont Tutf_MAC_SYMBOL_to_UTF_16[0x100];
extern hwfont Tutf_MAC_THAI_to_UTF_16[0x100];
extern hwfont Tutf_MAC_TURKISH_to_UTF_16[0x100];
extern hwfont Tutf_MAC_UKRAINE_to_UTF_16[0x100];

extern hwfont Tutf_MS_MAC_CYRILLIC_to_UTF_16[0x100];
extern hwfont Tutf_MS_MAC_GREEK_to_UTF_16[0x100];
extern hwfont Tutf_MS_MAC_ICELANDIC_to_UTF_16[0x100];
extern hwfont Tutf_MS_MAC_LATIN2_to_UTF_16[0x100];
extern hwfont Tutf_MS_MAC_ROMAN_to_UTF_16[0x100];
extern hwfont Tutf_MS_MAC_TURKISH_to_UTF_16[0x100];

extern hwfont Tutf_APL_ISO_IR_68_to_UTF_16[0x100];
extern hwfont Tutf_ATARIST_to_UTF_16[0x100];
extern hwfont Tutf_HP_ROMAN8_to_UTF_16[0x100];
extern hwfont Tutf_JISX0201_to_UTF_16[0x100];
extern hwfont Tutf_KZ1048_to_UTF_16[0x100];
extern hwfont Tutf_KOI8_R_to_UTF_16[0x100];
extern hwfont Tutf_KOI8_U_to_UTF_16[0x100];
extern hwfont Tutf_NEXTSTEP_to_UTF_16[0x100];
extern hwfont Tutf_VT100GR_to_UTF_16[0x100];

#define Tutf_CP437_to_ASCII		Tutf_CP437_to_ANSI_X3_4
extern hwfont Tutf_CP437_to_ANSI_X3_4[0x100];


#ifdef __cplusplus
}
#endif

#endif /* _TUTF_H */

