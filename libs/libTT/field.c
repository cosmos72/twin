/*
 *  field.c  --  exported libTT conversion functions between strings
 *               and numeric ids to access object fields using
 *               TT{Get,Set,Change}Value_ttobj(...)
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

#include "TT.h"

#include "tree_m4.h"

typedef struct s_ttfield s_ttfield;
typedef struct s_ttfield *ttfield;
struct s_ttfield {
    ttuint id;
    ttopaque len;
    TT_CONST ttbyte *name;
};


#include "field_m4.h"

static ttbyte fields_needsort = TRUE;

static int CompareFields(TT_CONST ttfield f1, TT_CONST ttfield f2) {
    int l, c;
    
    if ((l = f1->len - f2->len))
	return l;
    /* compare backwards not to waste time on common prefixes */
    for (l = f1->len - 1; l; l--) {
	if ((c = f1->name[l] - f2->name[l]))
	    return c;
    }
    return 0;
}

TT_FN_ATTR_CONST ttuint TTGetValueId(TT_CONST ttbyte *name) {
    s_ttfield key, *m;
    
    if (fields_needsort) {
	fields_needsort = FALSE;
	
	memcpy(field_array, field_array_c, sizeof(field_array));
	qsort(field_array, ttobj_field_n, sizeof(struct s_ttfield),
	      (int (*)(TT_CONST void *, TT_CONST void *))CompareFields);
    }
    
    if (!(key.name = name))
	return (ttuint)0;
    key.len = strlen(name);

    if ((m = bsearch(&key, field_array, ttobj_field_n, sizeof(struct s_ttfield),
	             (int (*)(TT_CONST void *, TT_CONST void *))CompareFields))) {
	return m->id;
    }
    return (ttuint)0;
}
TT_FN_ATTR_CONST TT_CONST ttbyte *TTGetValueName(ttuint n) {
    if (n < ttobj_field_n)
	return field_array_c[n].name;
    return NULL;
}


