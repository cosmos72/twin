
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

#include "Tw/Tw.h"

#include "Tutf/Tutf.h"
#include "Tutf/Tutf_defs.h"

typedef struct {
    hwfont utf, ch;
} utf_to_ch;

static int Cmp(TW_CONST utf_to_ch *u1, TW_CONST utf_to_ch *u2) {
    return u1->utf - u2->utf;
}

static TW_CONST utf_to_ch *my_bsearch(TW_CONST utf_to_ch *key, TW_CONST utf_to_ch *base, size_t nmemb) {
    TW_CONST utf_to_ch *low = base, *high = base + nmemb;

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
			       sizeof((array)[0]), (void *)Cmp)

#define BSEARCH(key, array) \
	(utf_to_ch *)my_bsearch((key), (array), sizeof(array)/sizeof((array)[0]))

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

#include "ibm437.c"
#include "ibm850.c"
#include "ibm865.c"

#define TEMPLATE CP866
#include "template.c"
#undef TEMPLATE

#define _NLIST(EL) \
	EL(T_MAP(ASCII)) \
	EL(T_MAP(ISO_8859_1)) EL(T_MAP(ISO_8859_2)) EL(T_MAP(ISO_8859_3)) \
	EL(T_MAP(ISO_8859_4)) EL(T_MAP(ISO_8859_5)) EL(T_MAP(ISO_8859_6)) \
	EL(T_MAP(ISO_8859_7)) EL(T_MAP(ISO_8859_8)) EL(T_MAP(ISO_8859_9)) \
	EL(T_MAP(IBM437))     EL(T_MAP(IBM850))     EL(T_MAP(IBM865))     \
	EL(T_MAP(CP866))

#define _LIST(EL) \
	EL(T_MAP(UTF_16)) _NLIST(EL)

typedef enum {
#define DECL_TYPE(x) T_CAT(T_ID_,x) ,
    _LIST(DECL_TYPE)
#undef DECL_TYPE
} id_e;

#define DECL_NAME(ch) static byte *T_CAT(names_,ch) [] = { T_CAT(T_NAME_,ch), T_CAT(T_ALIASES_,ch), NULL };
_LIST(DECL_NAME)
#undef DECL_NAME

typedef struct {
    byte * TW_CONST * names;
    Tutf_array array;
    Tutf_function function;
} Tutf_struct;

#define DECL_CH(ch) { T_CAT(names_,ch), T_CAT(T_CAT(Tutf_,ch),_to_UTF_16), T_CAT(Tutf_UTF_16_to_,ch) },

static Tutf_struct Tutf_structs[] = {
    { T_CAT(names_,UTF_16), NULL, NULL },
    _NLIST(DECL_CH)
    { NULL }
};

#undef DECL_CH

static int strloosecmp(TW_CONST byte *s1, TW_CONST byte *s2) {
    byte c1, c2;

    for (;;) {
	c1 = *s1++;
	c2 = *s2++;
	if (!c1 || !c2 ||
	    !(c1 == c2 ||
	      (
	       (c1 == '-' || c1 == '_' || c1 == '.' || c1 == ':') &&
	       (c2 == '-' || c2 == '_' || c2 == '.' || c2 == ':')
	      ) ||
	      (c1 >= 'A' && c1 <= 'Z' && c1 + ('a'-'A') == c2) ||
	      (c2 >= 'A' && c2 <= 'Z' && c2 + ('a'-'A') == c1)
	     )
	   )
	    
	    return (int)c1-(int)c2;
    }
}

uldat Tutf_charset_id(TW_CONST byte * alias) {
    Tutf_struct *CH;
    byte * TW_CONST * names;
    if (alias) for (CH = Tutf_structs; (names = CH->names); CH++) {
	for (; *names; names++) {
	    if (!strloosecmp(alias, *names))
		return (uldat)(CH - Tutf_structs);
	}
    }
    return (uldat)-1;
}

TW_CONST byte *Tutf_charset_name(uldat id) {
    return id < sizeof(Tutf_structs)/sizeof(Tutf_structs[0]) ? Tutf_structs[id].names[0] : NULL;
}

TW_CONST byte *Tutf_charset_alias(TW_CONST byte * alias) {
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




