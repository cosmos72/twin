
/*
 *  handy.c  --  wrappers around handy libTT methods
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




#include "handy_m4.h"

/* rw (read/write) fields: */

tt_fn TTSetUserData_ttobj(tt_obj o, ttany user_data) {
    ttobj a0;

    if ((a0 = ID2(ttobj, o))) {
	if (IS(ttcomponent,a0))
	    return TTSetKeyData_ttcomponent(o, "user_data", user_data) ? TTFN_ttany : (tt_fn)0;
	a0->user_data = user_data;
	return TTFN_ttany;
    }
    return (tt_fn)0;
}

tt_fn TTSetData_ttdata(tt_obj o, ttany data) {
    ttdata a0;
    
    if ((a0 = ID2(ttdata, o))) {
	return SetData_ttdata(a0, data);
    }
    return (tt_fn)0;
}

/* TT{Set,Get,Change}Field_ttobj */

tt_fn TTGetField_ttobj(TT_ARG_READ tt_obj o, ttuint which, ttany *value) {
    ttobj a0;
    
    if ((a0 = ID2OBJ(o)))
	return GetField(a0, which, value);

    return (tt_fn)0;
}

tt_fn TTSetField_ttobj(tt_obj o, ttuint which, ttany value) {
    ttobj a0;
    
    if ((a0 = ID2OBJ(o)))
	return SetField(a0, which, value);

    return (tt_fn)0;
}
tt_fn TTChangeField_ttobj(tt_obj o, ttuint which, ttany nand_value, ttany xor_value) {
    ttobj a0;
    ttany old_value;
    
    if ((a0 = ID2OBJ(o)) && GetField(a0, which, &old_value))
	return SetField(a0, which, (old_value & ~nand_value) ^ xor_value);

    return (tt_fn)0;
}

