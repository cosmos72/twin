
/*
 *  get.c  --  implementation of TTGet*_* methods
 *
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

#include "mutex.h"

#include "TT.h"
#include "TTextern.h"
#include "TTassert.h"

#include "utils.h"
#include "inlines.h"
#include "array.h"

#include "getset_m4.h"





/* array of tt*_field_first */
static ttopaque field_array_first[] = {
#define el(o) TT_CAT(o,_field_first),
    TT_LIST(el)
#undef el
};

/* array of tt*_field_last */
static ttopaque field_array_last[] = {
#define el(o) TT_CAT(o,_field_first),
    TT_LIST(el)
#undef el
};

#define field_array_n (sizeof(field_array_first)/sizeof(field_array_first[0]))


/* return the first field id of an object given a generic field id of the object */
static ttopaque FieldFirst(tt_obj field) {
    ttopaque i;
    for (i = 0; i < field_array_n; i++) {
	if ((ttopaque)field > field_array_first[i] && (ttopaque)field < field_array_last[i])
	    return field_array_first[i];
    }
    return (ttopaque)0;
}

static ttopaque GetArraySize_ttfield(TT_CONST ttfield f, TT_CONST tt_obj o) {
    ttuint i;
    ttopaque type, saved_type, size = 0;
    ttarg param[2], op;
    
    type = saved_type = f->type;
    size = TTGetSize_ttclass(TTFromType_ttclass(type));
    param[0].value = 0;
    
    if (type & ttclass_type_array) {
	param[0].value = (type >>= ttclass_type_array_firstbit) & e_MFP;
	param[1].value = (type >>= e_WFP) & e_MFP;
	op.value = (type >>= e_WFP) & e_MOP;
	
	if (op.value == e_OPZ) {
		/* zero-terminated array */
		
	    if (TTGetValue_ttfield((tt_obj)f->id, o, &op) && TTIsArrayType_ttclass(op.type))
		return op.size;
	    return 0;
	}
	
	/* extract length params from arguments if needed */
	for (i = 0; i < 2; i++) {
	    if (param[i].value & e_FFP) {
		
		TTAssertAlways(TTGetValue_ttfield
			       ((tt_obj)(FieldFirst((tt_obj)f->id) + (param[i].value & ~e_FFP)),
				o, &param[i]));
	    }
	}
	switch (op.value) {
	  case e_SET:
	    /* param[0].value = param[0].value; */
	    break;
	  case e_ADD:
	    param[0].value += param[1].value;
	    break;
	  case e_MUL:
	    param[0].value *= param[1].value;
	    break;
	  default:
	    /* illegal operation ? */
	    param[0].value = 0;
	    break;
	}
    }
    return size * (ttopaque)param[0].value;
}





/* ttobj exported_field methods */

ttbyte TTGetField_ttobj(/*TT_CONST*/ tt_obj a0, /*TT_CONST*/ tt_obj which, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *value) {
    return TTGetValue_ttfield(which, a0, value);
}

ttbyte TTSetField_ttobj(tt_obj a0, /*TT_CONST*/ tt_obj which, TT_ARG_READ TT_ARG_ARRAY((1)) ttarg *value) {
    return TTSetValue_ttfield(which, a0, value);
}

ttbyte TTChangeField_ttobj(tt_obj a0, /*TT_CONST*/ tt_obj which, ttany nand_value, ttany xor_value) {
    return TTChangeValue_ttfield(which, a0, nand_value, xor_value);
}


/* ttfield exported_field methods */

ttbyte TTGetValue_ttfield(/*TT_CONST*/ tt_obj which, /*TT_CONST*/ tt_obj a0, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *value) {
    TT_ARG_READ ttobj o = ID2OBJ((opaque)a0);
    TT_ARG_READ ttfield f = ID2(ttfield,(opaque)which);
    
    if (o && f && getset_GetField(o, which, &value->value)) {
	if (TTIsArrayType_ttclass(value->type = f->type))
	    value->size = GetArraySize_ttfield(f, a0);
	return TT_TRUE;
    }
    return TT_FALSE;
}

ttbyte TTSetValue_ttfield(/*TT_CONST*/ tt_obj which, tt_obj a0, TT_ARG_READ TT_ARG_ARRAY((1)) ttarg *value) {
    ttobj o = ID2OBJ((opaque)a0);
    
    return o && getset_SetField(o, which, value);
}

ttbyte TTChangeValue_ttfield(/*TT_CONST*/ tt_obj which, tt_obj a0, ttany nand_value, ttany xor_value) {
    ttarg value;
    
    if (TTGetValue_ttfield(which, a0, &value) && !TTIsArrayType_ttclass(value.type)) {
	value.value = (value.value & ~nand_value) ^ xor_value;
	return TTSetValue_ttfield(which, a0, &value);
    }
    return TT_FALSE;
}

