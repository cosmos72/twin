/*
 *  array.c  --  libTT conversion functions between
 *               types and classes and array sizes
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
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

#include "array.h"




/*
 * get the bare class of given type (removing attribute)
 */
TT_ARG_READ tt_class TTFromType_ttclass(ttopaque type) {
    opaque Class = type & ~ttclass_type_full_bitmask;
    
    return
	(Class >= id_ttclass_first && Class < id_ttclass_last) ||
	(Class >= id_ttclass_type_first && Class < id_ttclass_type_last) ?
	(tt_class)Class : (tt_class)0;
}


/**
 * return TT_TRUE if given type indicates an array
 */
ttbyte TTIsArrayType_ttclass(ttopaque type) {
    return type & ttclass_type_array ? TT_TRUE : TT_FALSE;
}

ttbyte TTIsPrimitiveType_ttclass(ttopaque type) {
    opaque Class = type & ~ttclass_type_full_bitmask;

    return Class >= id_ttclass_first && Class < id_ttclass_last;
}



static ttopaque GetArrayLenZ(ttbyte * addr, ttopaque size, ttopaque max_n) {
    ttany zero = 0;
    ttbyte *saved_addr = addr;
    
    if (addr) {
	while (max_n--) {
	    if (!TTCmpMem(addr, &zero, size))
		/* also return space needed by final \0 */
		return addr - saved_addr + size;
	    addr += size;
	}
    }
    return (ttopaque)zero;
}





static ttopaque GetArraySizeA_ttmethod(TT_ARG_READ ttmethod m, ttopaque type, ttopaque arg_array_n, ttopaque args_n, TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttarg *args) {
    ttuint i;
    ttopaque size = 0;
    ttany param[2] = {0, 0}, op;

    /*
    if (arg_array_n >= args_n)
	return (ttopaque)0;
    */
    
    size = TTGetSize_ttclass(TTFromType_ttclass(type));

    /*
    if (type & ttclass_type_array) {
     */
    param[0] = (type >>= ttclass_type_array_firstbit) & e_MFP;
    param[1] = (type >>= e_WFP) & e_MFP;
    op = (type >>= e_WFP) & e_MOP;
    
    
    if (op == e_OPZ) {
	/* zero-terminated array */
	return GetArrayLenZ((ttbyte *)(opaque)args[arg_array_n].value, size, args[arg_array_n].size);
    }
    
    /* extract length params from arguments if needed */
    for (i = 0; i < 2; i++) {
	if (param[i] & e_FFP) {
	    param[i] &= ~e_FFP;
	    /* param[i] == 1 means first arg, i.e. args[0] */
	    if (param[i] && param[i] <= args_n && !TTIsArrayType_ttclass(args[param[i] - 1].type))
		param[i] = args[param[i] - 1].value;
	}
    }
    switch (op) {
      case e_SET:
	/* param[0].value = param[0].value; */
	break;
	  case e_ADD:
	    param[0] += param[1];
	    break;
	  case e_MUL:
	    param[0] *= param[1];
	    break;
	  default:
	    /* illegal operation ? */
	    param[0] = 0;
	    break;
    }
    return size * param[0];
}


ttopaque TTGetArraySizeA_ttmethod(TT_ARG_READ tt_obj id, ttopaque arg_array_n, ttopaque args_n, TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttarg *args) {
    ttmethod m;
    ttvector v;
    ttopaque type;
    
    if ((m = ID2(ttmethod,id)) && (v = m->parameter_type) && v->array_n <= args_n) {
	if ((type = v->array[arg_array_n], type & ttclass_type_array))
	    return GetArraySizeA_ttmethod(m, type, arg_array_n, args_n, args);
    }
    return (ttopaque)0;
}

ttbyte TTCheckArraySizesA_ttmethod(TT_ARG_READ tt_obj id, ttopaque args_n, TT_ARG_READ TT_ARG_ARRAY((_P(3))) ttarg * args) {
    ttmethod m;
    ttvector v;
    ttopaque i, type;
    
    if ((m = ID2(ttmethod,id)) && (v = m->parameter_type) && v->array_n <= args_n) {
	for (i = 0; i < v->array_n; i++) {
	    if ((type = v->array[i], type & ttclass_type_array) &&
		args[i].size < GetArraySizeA_ttmethod(m, type, i, args_n, args)) {
		
		return TT_FALSE;
	    }
	}
	return TT_TRUE;
    }
    return TT_FALSE;
}

