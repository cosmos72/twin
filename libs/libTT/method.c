/*
 *  method.c  -- exported libTT conversion functions between strings
 *               and ttmethod objects to access object methods using
 *               TT{Get*,Call}_ttmethod(...)
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

#include "tree_m4.h"

#include "array.h"
#include "method1_m4.h"

/* these must come AFTER method1_m4.h */
#include "inlines.h"
#include "utils.h"


#include "check_asm.h"
#if defined (CONF__ASM) && defined(TW_HAVE_ASM)
# include "method2_asm.h"
#else
# include "method2_m4.h"
#endif


ttbyte TTCallA_ttmethod(TT_ARG_READ tt_obj id, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *return_value, ttopaque args_n, TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttarg *aargs) {
    ttany args[TT_MAX_ARG_N];
    ttopaque i;
    
    if (args_n <= TT_MAX_ARG_N) {
	for (i = 0; i < args_n; i++)
	    args[i] = aargs[i].value;
	
	return TTCallY_ttmethod(id, return_value, args_n, args);
    }
    return TT_FALSE;
}

ttbyte TTCallV_ttmethod(TT_ARG_READ tt_obj id, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *return_value, ttopaque args_n, TT_ARG_ARRAY((_P(1))) va_list *vargs) {
    ttany args[TT_MAX_ARG_N];
    ttopaque i;
    
    if (args_n <= TT_MAX_ARG_N) {
	for (i = 0; i < args_n; i++)
	    args[i] = va_arg((*vargs), ttany);
	
	return TTCallY_ttmethod(id, return_value, args_n, args);
    }
    return TT_FALSE;
}

ttbyte TTCallL_ttmethod(TT_ARG_READ tt_obj id, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *return_value, ttopaque args_n, ...) {
    ttbyte ret;
    va_list vargs;

    va_start(vargs, args_n);
    ret = TTCallV_ttmethod(id, return_value, args_n, &vargs);
    va_end(vargs);
    
    return ret;
}

ttbyte TTCallR_ttmethod(TT_ARG_READ tt_obj id, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *return_value, TT_ARG_READ tt_obj vargs) {
    TT_ARG_READ ttvector args = ID2(ttvector,vargs);
    if (args)
	return TTCallY_ttmethod(id, return_value, args->array_n, args->array);
    return TT_FALSE;
}

static ttmethod method_array, *method_byid, *method_byname, *method_byaddr;

static ttvector param_array, *param_byid;

static int CompareMethodAddress(TT_CONST ttmethod *m1, TT_CONST ttmethod *m2) {
    return (int)((byte *)(*m1)->address - (byte *)(*m2)->address);
}

static int CompareMethodName(TT_CONST ttmethod *mm1, TT_CONST ttmethod *mm2) {
    int l, c;
    TT_CONST ttmethod m1 = *mm1, m2 = *mm2;
    TT_CONST ttbyte *n1, *n2;
    
    if ((c = m1->name_len - m2->name_len))
	return c;
    /* compare forward not to waste time on common suffixes */
    l = m1->name_len;
    n1 = m1->name;
    n2 = m2->name;
    while (l-- && !(c = *n1++ - *n2++))
	;
    return c;
}

/*
 * use binary search for efficiency
 */
TT_ARG_READ tt_obj TTGetByAddress_ttmethod(TT_ARG_READ ttfunction_fn method) {
    s_ttmethod s_key;
    ttmethod key = &s_key, *m;

    s_key.address = method;
    
    if ((m = bsearch(&key, method_byaddr, method_array_n, sizeof(ttmethod),
		     (int (*)(TT_CONST void *, TT_CONST void *))CompareMethodAddress))
	&& *m) {
	
	return (tt_obj)(*m)->id;
    }
    return (tt_obj)0;
}

TT_ARG_READ tt_obj TTGet_ttmethod(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name) {
    s_ttmethod s_key;
    ttmethod key = &s_key, *m;
    
    if (name) {
	
	s_key.name_len = TTLenStr( s_key.name = name );
	
	if ((m = bsearch(&key, method_byname, method_array_n, sizeof(ttmethod),
			 (int (*)(TT_CONST void *, TT_CONST void *))CompareMethodName))
	    && *m) {
	    
	    return (tt_obj)(*m)->id;
	}
    }
    return (tt_obj)0;
}

static int CompareMethodOrderAndArgN(TT_CONST ttany *m1, TT_CONST ttany *m2) {
    int i;
    return
	(i = (int)m1[0] - (int)m2[0]) ? i
	:    (int)m1[1] - (int)m2[1];
}

static ttuint GetParameterType(ttopaque mid, ttopaque arg_n) {
    ttany key[2];
    ttany *m;
    
    key[0] = (ttany)mid & TT_MAGIC_MASK;
    key[1] = arg_n;
    
    if ((m = bsearch(key, method_type, method_type_n, sizeof(ttany)*3,
		     (int (*)(TT_CONST void *, TT_CONST void *))CompareMethodOrderAndArgN)))
	return (ttuint)m[2];

    return (ttuint)0;
}

static ttbyte method_array_init_once(void) {
    TT_CONST ttbyte *base = str_order_first;
    ttuint i, j, n;
    ttmethod m;
    ttvector v;
    TT_CONST ttbyte *s;
    ttany *dst;

    if (!TTAssertAlways((method_array  = TTAllocMem(method_array_n * sizeof(struct s_ttmethod))) &&
			(method_byid   = TTAllocMem(method_array_n * sizeof(ttmethod))) &&
			(method_byname = TTAllocMem(method_array_n * sizeof(ttmethod))) &&
			(method_byaddr = TTAllocMem(method_array_n * sizeof(ttmethod))) &&
			(param_array   = TTAllocMem(method_array_n * sizeof(struct s_ttvector))) &&
			(param_byid    = TTAllocMem(method_array_n * sizeof(ttvector) * 2))))
	return TT_FALSE;

    /* set to zero all objects */
    TTWriteMem(method_array, '\0', method_array_n * sizeof(struct s_ttmethod));
    TTWriteMem(param_array,  '\0', method_array_n * sizeof(struct s_ttvector));
    
    for (i = 0; i < method_array_n; i++) {
	method_byaddr[i] = method_byname[i] = method_byid[i] = m = &method_array[i];
	
	m->Class = TClass_ttmethod;
	m->id = (order_ttmethod << TT_MAGIC_SHIFT) | i;
	m->name_len = TTLenStr(m->name = method_name[i]);
	m->address = method_addr[i];
	s = method_proto[i];
	m->return_type = (ttany)ORDER2ID_CLASS(*s++ - *base) |
	    GetParameterType(m->id, 0 /* 0 is return type */);

	/* initialize method->parameter_type */
	param_byid[i] = m->parameter_type = v = &param_array[i];
	
	/* cannot use v->Class->AddAt() here, target driver is not initialized yet */
	v->Class = TClass_ttvector;
	v->id = (order_ttvector << TT_MAGIC_SHIFT) | i;
	/* other fields are already set to zero */
	v->array_size = v->array_n = n = TTLenStr(s);
	if (n && !TTAssertAlways(v->array = dst = TTAllocMem(n * sizeof(ttany))))
	    return TT_FALSE;
	for (j = 0; j < n; j++) {
	    *dst++ = (ttany)ORDER2ID_CLASS(*s++ - *base) |
		GetParameterType(m->id, j + 1 /* 0 is return type */);
	}
	/* never modify nor delete m->parameter_type */
	v->oflags |= ttobject_oflags_static|ttobject_oflags_const;
	
	
    }
	
    qsort(method_byaddr, method_array_n, sizeof(ttmethod),
	  (int (*)(TT_CONST void *, TT_CONST void *))CompareMethodAddress);
	
    qsort(method_byname, method_array_n, sizeof(ttmethod),
	  (int (*)(TT_CONST void *, TT_CONST void *))CompareMethodName);
    
    return TT_TRUE;
}

static ttbyte method_array_init(void) {
    return
	/* store method_byid[] ttmethods and they parameter_type ttvectors into global IdList[][] */
	FixedAssignIdArray(id_ttclass_ttmethod, method_array_n, (ttobj *)method_byid) &&
	FixedAssignIdArray(id_ttclass_ttvector, method_array_n, (ttobj *)param_byid);
}

#include "utils.h"

ttbyte _TT_method_Init(void) {
    static ttbyte method_init_once_done = TT_FALSE;
    
    return
	(method_init_once_done ||
	 (method_init_once_done = method_array_init_once())) &&
	
	method_array_init();
}
