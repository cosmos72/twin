
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

#include "ascii.c"

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

#include "cp437.c"

#define TEMPLATE CP737
#include "template.c"
#undef TEMPLATE

#define TEMPLATE CP775
#include "template.c"
#undef TEMPLATE

#include "cp850.c"

#define TEMPLATE CP852
#include "template.c"
#undef TEMPLATE

#define TEMPLATE CP855
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

#include "cp865.c"

#define TEMPLATE CP866
#include "template.c"
#undef TEMPLATE

#define TEMPLATE CP869
#include "template.c"
#undef TEMPLATE

#define TEMPLATE CP874
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


#define _NLIST(EL) \
	EL(T_MAP(ASCII)) \
	EL(T_MAP(ISO_8859_1)) EL(T_MAP(ISO_8859_2)) EL(T_MAP(ISO_8859_3)) \
	EL(T_MAP(ISO_8859_4)) EL(T_MAP(ISO_8859_5)) EL(T_MAP(ISO_8859_6)) \
	EL(T_MAP(ISO_8859_7)) EL(T_MAP(ISO_8859_8)) EL(T_MAP(ISO_8859_9)) \
	EL(T_MAP(ISO_8859_10)) EL(T_MAP(ISO_8859_13)) EL(T_MAP(ISO_8859_14)) \
	EL(T_MAP(ISO_8859_15)) EL(T_MAP(ISO_8859_16)) \
        EL(T_MAP(CP437)) EL(T_MAP(CP737)) EL(T_MAP(CP775)) EL(T_MAP(CP850)) EL(T_MAP(CP852)) \
	EL(T_MAP(CP855)) EL(T_MAP(CP857)) EL(T_MAP(CP860)) EL(T_MAP(CP861)) EL(T_MAP(CP862)) \
	EL(T_MAP(CP863)) EL(T_MAP(CP864)) EL(T_MAP(CP865)) EL(T_MAP(CP866)) EL(T_MAP(CP869)) \
	EL(T_MAP(CP874)) \
        EL(T_MAP(CP1250)) EL(T_MAP(CP1251)) EL(T_MAP(CP1252)) EL(T_MAP(CP1253)) \
	EL(T_MAP(CP1254)) EL(T_MAP(CP1255)) EL(T_MAP(CP1256)) EL(T_MAP(CP1257))

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

    for (;;) {
	c1 = *s1++;
	if (c1 == '-' || c1 == '_' || c1 == '.' || c1 == ':')
	    c1 = *s1++;
	
	c2 = *s2++;
	if (c2 == '-' || c2 == '_' || c2 == '.' || c2 == ':')
	    c2 = *s2++;
	
	if (!c1 || !c2 ||
	    !(c1 == c2 ||
	      (c1 >= 'A' && c1 <= 'Z' && c1 + ('a'-'A') == c2) ||
	      (c2 >= 'A' && c2 <= 'Z' && c2 + ('a'-'A') == c1)
	     )
	   )
	    
	    return (int)c1-(int)c2;
    }
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




