/*
 *  field.c  --  exported libTT conversion functions between strings
 *               and ttfield objects to access object fields using
 *               TT{Get,Set}_ttfield(...)
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
#include "TTextern.h"
#include "TTassert.h"

#include "inlines.h"
#include "utils.h"

#include "tree_m4.h"

#include "array.h"
#include "field_m4.h"

static TT_CONST struct s_ttfield * field_byid[field_array_n];
static TT_CONST struct s_ttfield * field_byname[field_array_n];

    
static int CompareFieldName(TT_CONST ttfield *ff1, TT_CONST ttfield *ff2) {
    int l, c;
    TT_CONST ttfield f1 = *ff1, f2 = *ff2;
    TT_CONST ttbyte *n1, *n2;
    
    if ((c = f1->name_len - f2->name_len))
	return c;
    /* compare backwards not to waste time on common prefixes */
    l = f1->name_len;
    n1 = f1->name + l - 1;
    n2 = f2->name + l - 1;
    while (l-- && !(c = *n1-- - *n2--))
	;
    return c;
}

TT_ARG_READ tt_obj TTGet_ttfield(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name) {
    s_ttfield s_key;
    ttfield key = &s_key, *m;

    if (name) {
	
	s_key.name_len = TTLenStr( s_key.name = name );
	
	if ((m = bsearch(&key, field_byname, field_array_n, sizeof(ttfield),
			 (int (*)(TT_CONST void *, TT_CONST void *))CompareFieldName))
	    && *m) {
	    
	    return (tt_obj)(*m)->id;
	}
    }
    return (tt_obj)0;
}


ttopaque TTToEvcode_ttfield(/*TT_CONST*/ tt_obj f) {
    if (ID2(ttfield,f)) {
	return (ttopaque)f;
    }
    return (ttopaque)0;
}

TT_ARG_READ tt_obj TTFromEvcode_ttfield(ttopaque i) {
    if (ID2(ttfield,i)) {
	return (tt_obj)i;
    }
    return (tt_obj)0;
}

ttopaque TTGetEvcode_ttfield(TT_CONST ttbyte *name) {
    tt_obj f;
    
    if ((f = TTGet_ttfield(name))) {
	return TTToEvcode_ttfield(f);
    }
    return (ttopaque)0;
}


static ttbyte field_init_once(void) {
    ttuint i;
    
    for (i = 0; i < field_array_n; i++)
	field_byname[i] = field_byid[i] = &field_array[i];

    qsort(field_byname, field_array_n, sizeof(ttfield),
	  (int (*)(TT_CONST void *, TT_CONST void *))CompareFieldName);
    
    return TT_TRUE;
}


ttbyte _TT_field_Init(void) {
    static ttbyte field_init_once_done = TT_FALSE;

    return
	(field_init_once_done ||
	 (field_init_once_done = field_init_once())) &&
	
	FixedAssignIdArray(id_ttclass_ttfield, field_array_n, (TT_CONST ttobj *)field_byid);
}
