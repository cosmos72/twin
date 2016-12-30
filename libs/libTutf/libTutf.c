
/*
 *   libTwUTF.c -- ASCII/VGA/UTF/UNICODE pseudographics characters database
 * 
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA
 *
 */

#include <Tw/Twtypes.h>

#include <Tutf/Tutf.h>
#include <Tutf/Tutf_defs.h>

typedef struct {
    hwfont utf, ch;
} utf_to_ch;

static int Cmp(TUTF_CONST utf_to_ch *u1, TUTF_CONST utf_to_ch *u2) {
    return u1->utf - u2->utf;
}

static TUTF_CONST utf_to_ch *my_bsearch(TUTF_CONST utf_to_ch *key, TUTF_CONST utf_to_ch *base, size_t nmemb) {
    TUTF_CONST utf_to_ch *low = base, *high = base + nmemb;

    while (low + 1 < high) {
	base = low + (high - low) / 2;
	
	if (base->utf > key->utf)
	    high = base;
	else if (base->utf < key->utf)
	    low = base;
	else
	    return base;
    }
    return NULL;
}


#define QSORT(array) qsort((void *)(array), sizeof(array)/sizeof((array)[0]), \
			       sizeof((array)[0]), (int (*)(TUTF_CONST void *, TUTF_CONST void *))Cmp)

#define BSEARCH(key, array) \
	my_bsearch((key), (array), sizeof(array)/sizeof((array)[0]))

#include "ascii.c" /* hand-tuned */

#define ISO_8859_X ISO_8859_1
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_2
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_3
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_4
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_5
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_6
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_7
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_8
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_9
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_10
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_11
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_13
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_14
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_15
#include "iso_8859_x.c"
#undef ISO_8859_X
#define ISO_8859_X ISO_8859_16
#include "iso_8859_x.c"
#undef ISO_8859_X

#include "cp437.c" /* hand-tuned */
#include "cp850.c" /* hand-tuned */
#include "cp865.c" /* hand-tuned */

#define TEMPLATE CP424
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP737
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP775
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP852
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP855
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP856
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP857
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP860
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP861
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP862
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP863
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP864
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP866
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP869
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP874
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP932
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP936
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP949
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP950
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP1006
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP1250
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP1251
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP1252
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP1253
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP1254
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP1255
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP1256
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP1257
#include "template.c"
#undef TEMPLATE
#define TEMPLATE CP1258
#include "template.c"
#undef TEMPLATE
#define TEMPLATE ADOBE_STDENC
#include "template.c"
#undef TEMPLATE
#define TEMPLATE ADOBE_SYMBOL
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_ARABIC
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_CELTIC
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_CENTEURO
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_CHINSIMP
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_CHINTRAD
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_CROATIAN
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_CYRILLIC
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_DEVANAGA
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_DINGBATS
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_FARSI
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_GAELIC
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_GREEK
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_GUJARATI
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_GURMUKHI
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_HEBREW
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_ICELAND
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_INUIT
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_JAPANESE
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_KOREAN
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_ROMAN
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_ROMANIAN
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_SYMBOL
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_THAI
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_TURKISH
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APPLE_UKRAINE
#include "template.c"
#undef TEMPLATE
#define TEMPLATE MS_MAC_CYRILLIC
#include "template.c"
#undef TEMPLATE
#define TEMPLATE MS_MAC_GREEK
#include "template.c"
#undef TEMPLATE
#define TEMPLATE MS_MAC_ICELAND
#include "template.c"
#undef TEMPLATE
#define TEMPLATE MS_MAC_LATIN2
#include "template.c"
#undef TEMPLATE
#define TEMPLATE MS_MAC_ROMAN
#include "template.c"
#undef TEMPLATE
#define TEMPLATE MS_MAC_TURKISH
#include "template.c"
#undef TEMPLATE
#define TEMPLATE APL_ISO_IR_68 
#include "template.c"
#undef TEMPLATE
#define TEMPLATE ATARIST
#include "template.c"
#undef TEMPLATE
#define TEMPLATE JISX0201
#include "template.c"
#undef TEMPLATE
#define TEMPLATE HP_ROMAN8
#include "template.c"
#undef TEMPLATE
#define TEMPLATE KOI8_R
#include "template.c"
#undef TEMPLATE
#define TEMPLATE KOI8_U
#include "template.c"
#undef TEMPLATE
#define TEMPLATE KZ1048
#include "template.c"
#undef TEMPLATE
#define TEMPLATE NEXTSTEP
#include "template.c"
#undef TEMPLATE
#define TEMPLATE VT100GR
#include "template.c"
#undef TEMPLATE



#define _NLIST(EL) \
	EL(T_MAP(ASCII)) \
	EL(T_MAP(ISO_8859_1)) EL(T_MAP(ISO_8859_2)) EL(T_MAP(ISO_8859_3)) \
	EL(T_MAP(ISO_8859_4)) EL(T_MAP(ISO_8859_5)) EL(T_MAP(ISO_8859_6)) \
	EL(T_MAP(ISO_8859_7)) EL(T_MAP(ISO_8859_8)) EL(T_MAP(ISO_8859_9)) \
	EL(T_MAP(ISO_8859_10)) EL(T_MAP(ISO_8859_11)) EL(T_MAP(ISO_8859_13)) \
	EL(T_MAP(ISO_8859_14)) EL(T_MAP(ISO_8859_15)) EL(T_MAP(ISO_8859_16)) \
        \
	EL(T_MAP(CP424)) EL(T_MAP(CP437)) EL(T_MAP(CP737)) EL(T_MAP(CP775)) \
        EL(T_MAP(CP850)) EL(T_MAP(CP852)) EL(T_MAP(CP855)) EL(T_MAP(CP856)) \
        EL(T_MAP(CP857)) EL(T_MAP(CP860)) EL(T_MAP(CP861)) EL(T_MAP(CP862)) \
        EL(T_MAP(CP863)) EL(T_MAP(CP864)) EL(T_MAP(CP865)) EL(T_MAP(CP866)) \
        EL(T_MAP(CP869)) EL(T_MAP(CP874)) EL(T_MAP(CP932)) EL(T_MAP(CP936)) \
        EL(T_MAP(CP949)) EL(T_MAP(CP950)) EL(T_MAP(CP1006)) EL(T_MAP(CP1250)) \
        EL(T_MAP(CP1251)) EL(T_MAP(CP1252)) EL(T_MAP(CP1253)) EL(T_MAP(CP1254)) \
        EL(T_MAP(CP1255)) EL(T_MAP(CP1256)) EL(T_MAP(CP1257)) EL(T_MAP(CP1258)) \
        \
	EL(T_MAP(ADOBE_STDENC)) EL(T_MAP(ADOBE_SYMBOL)) \
	\
	EL(T_MAP(APPLE_ARABIC)) EL(T_MAP(APPLE_CELTIC)) EL(T_MAP(APPLE_CENTEURO)) \
	EL(T_MAP(APPLE_CHINSIMP)) EL(T_MAP(APPLE_CHINTRAD)) EL(T_MAP(APPLE_CORPCHAR)) \
	EL(T_MAP(APPLE_CROATIAN)) EL(T_MAP(APPLE_CYRILLIC)) EL(T_MAP(APPLE_DEVANAGA)) \
	EL(T_MAP(APPLE_DINGBATS)) EL(T_MAP(APPLE_FARSI)) EL(T_MAP(APPLE_GAELIC)) \
	EL(T_MAP(APPLE_GREEK)) EL(T_MAP(APPLE_GUJARATI)) EL(T_MAP(APPLE_GURMUKHI)) \
	EL(T_MAP(APPLE_HEBREW)) EL(T_MAP(APPLE_ICELAND)) EL(T_MAP(APPLE_INUIT)) \
	EL(T_MAP(APPLE_JAPANESE)) EL(T_MAP(APPLE_KOREAN)) EL(T_MAP(APPLE_ROMAN)) \
	EL(T_MAP(APPLE_ROMANIAN)) EL(T_MAP(APPLE_SYMBOL)) EL(T_MAP(APPLE_THAI)) \
	EL(T_MAP(APPLE_TURKISH)) EL(T_MAP(APPLE_UKRAINE)) \
	\
	EL(T_MAP(MS_MAC_CYRILLIC)) EL(T_MAP(MS_MAC_GREEK)) EL(T_MAP(MS_MAC_ICELAND)) \
	EL(T_MAP(MS_MAC_LATIN2)) EL(T_MAP(MS_MAC_ROMAN)) EL(T_MAP(MS_MAC_TURKISH)) \
	\
	EL(T_MAP(APL_ISO_IR_68 )) EL(T_MAP(ATARIST)) EL(T_MAP(JISX0201)) \
	EL(T_MAP(HP_ROMAN8)) EL(T_MAP(KOI8_R)) EL(T_MAP(KOI8_U)) \
	EL(T_MAP(KZ1048)) EL(T_MAP(NEXTSTEP)) EL(T_MAP(VT100GR))

#define _LIST(EL) \
	EL(T_MAP(UTF_16)) _NLIST(EL)

typedef enum {
#define DECL_TYPE(x) T_CAT(T_ID_,x) ,
    _LIST(DECL_TYPE)
#undef DECL_TYPE
} id_e;

#define DECL_NAME(ch) static byte *T_CAT(names_,ch) [] = { T_CAT((byte *)T_NAME_,ch), T_CAT((byte *)T_ALIASES_,ch), NULL };
_LIST(DECL_NAME)
#undef DECL_NAME

typedef struct {
    byte * TUTF_CONST * names;
    Tutf_array array;
    Tutf_function function;
} Tutf_struct;

#define DECL_CH(ch) { T_CAT(names_,ch), T_CAT3(Tutf_,ch,_to_UTF_16), T_CAT(Tutf_UTF_16_to_,ch) },

static Tutf_struct Tutf_structs[] = {
    { T_CAT(names_,UTF_16), NULL, NULL },
    _NLIST(DECL_CH)
    { NULL }
};

#undef DECL_CH

static int strloosecmp(TUTF_CONST byte *s1, TUTF_CONST byte *s2) {
    byte c1, c2;

    do {
	while ((c1 = *s1++) && (c1 == ' ' || c1 == '-' || c1 == '_' || c1 == '.' || c1 == ':'))
	    ;
	
	while ((c2 = *s2++) && (c2 == ' ' || c2 == '-' || c2 == '_' || c2 == '.' || c2 == ':'))
	    ;

        if (c1 >= 'A' && c1 <= 'Z')
            c1 += 'a'-'A';

        if (c2 >= 'A' && c2 <= 'Z')
            c2 += 'a'-'A';

    } while (c1 == c2 && c1 && c2);

    return (int)c1-(int)c2;
}

uldat Tutf_charset_id(TUTF_CONST byte * alias) {
    Tutf_struct *CH;
    byte * TUTF_CONST * names;
    if (alias) for (CH = Tutf_structs; (names = CH->names); CH++) {
	for (; *names; names++) {
	    if (!strloosecmp(alias, *names))
		return (uldat)(CH - Tutf_structs);
	}
    }
    return (uldat)-1;
}

TUTF_CONST byte *Tutf_charset_name(uldat id) {
    return id < sizeof(Tutf_structs)/sizeof(Tutf_structs[0]) ? Tutf_structs[id].names[0] : NULL;
}

TUTF_CONST byte *Tutf_charset_alias(TUTF_CONST byte * alias) {
    uldat id = Tutf_charset_id(alias);
    return id != (uldat)-1 ? Tutf_structs[id].names[0] : NULL;
}


/* return a function capable to translate from UTF_16 to given charset */
Tutf_function Tutf_UTF_16_to_charset_function(uldat id) {
    return id < sizeof(Tutf_structs)/sizeof(Tutf_structs[0]) ? Tutf_structs[id].function : NULL;
}

/* return the array to translate from given charset to UTF_16 */
Tutf_array Tutf_charset_to_UTF_16_array(uldat id) {
    return id < sizeof(Tutf_structs)/sizeof(Tutf_structs[0]) ? Tutf_structs[id].array : NULL;
}

