


/*
 *  method2_asm.h  --  implementation of TTCall*_ttmethod dispatcher
 *                     (also used by ttlisteners)
 *
 *  This file uses DIRTY tricks with function pointer prototypes
 *  and their argument types, assuming default argument promotion
 *  of *ALL* arguments to (ttany) always takes place.
 *
 *  For this reason, the code is machine-dependent and non-portable.
 *
 *  The file method.c performs checks to determine whether this code would work
 *  on the machine in use,
 *  else the portable (and much bigger) method2_m4.h is used.
 *
 */

#ifndef TT_MAX_ARG_N
# error internal error: undefined TT_MAX_ARG_N !
#endif

#if TT_MAX_ARG_N > 11
# error internal error: TT_MAX_ARG_N > 11, must edit method2_asm.h !
#endif

ttbyte TTCallY_ttmethod(tt_obj id, TT_ARG_WRITE TT_ARG_ARRAY((1)) ttarg *return_value, ttopaque nargs, TT_CONST TT_ARG_ARRAY(_P(2)) ttany *a) {
    ttmethod m;
    ttopaque m_nargs, type;
    ttfunction_fn function;
    ttany ret = (ttany)0;
    
    if ((m = ID2(ttmethod,id)) && nargs >= (m_nargs = m->parameter_type->array_n)) {
	function = m->address;
	
# if 0 && defined(TW_HAVE_GCC_I386_ASM)
	{
	    ttany _TT_CallY_ttmethod(ttfunction_fn function, ttopaque nargs, TT_ARG_READ TT_ARG_ARRAY((_P(2))) args);
	    ret = _TT_CallY_ttmethod(function, nargs, a);
	}
# else
	switch (m_nargs) {
	  case 0:
	    ret = ((ttany (*)(void)) function )
		();
	    break;
	  case 1:
	    ret = ((ttany (*)(ttany)) function )
		(a[0]);
	    break;
	  case 2:
	    ret = ((ttany (*)(ttany,ttany)) function ) 
		(a[0],a[1]);
	    break;
	  case 3:
	    ret = ((ttany (*)(ttany,ttany,ttany)) function )
		(a[0],a[1],a[2]);
	    break;
	  case 4:
	    ret = ((ttany (*)(ttany,ttany,ttany,ttany)) function )
		(a[0],a[1],a[2],a[3]);
	  case 5:
	    ret = ((ttany (*)(ttany,ttany,ttany,ttany,ttany)) function )
		(a[0],a[1],a[2],a[3],a[4]);
	    break;
	  case 6:
	    ret = ((ttany (*)(ttany,ttany,ttany,ttany,ttany,ttany)) function )
		(a[0],a[1],a[2],a[3],a[4],a[5]);
	    break;
	  case 7:
	    ret = ((ttany (*)(ttany,ttany,ttany,ttany,ttany,ttany,ttany)) function )
		(a[0],a[1],a[2],a[3],a[4],a[5],a[6]);
	    break;
	  case 8:
	    ret = ((ttany (*)(ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany)) function )
		(a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]);
	    break;
	  case 9:
	    ret = ((ttany (*)(ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany)) function )
		(a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7],a[8]);
	    break;
	  case 10:
	    ret = ((ttany (*)(ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany)) function )
		(a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7],a[8],a[9]);
	    break;
	  case 11:
	    ret = ((ttany (*)(ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany,ttany)) function )
		(a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7],a[8],a[9],a[10]);
	    break;
	  default:
	    /* argh. this is bad */
	    TTAssertAlways(m_nargs <= 11);
	    return TT_FALSE;
	}
# endif
	type = return_value->type = m->return_type;
	if (type == id_ttclass_void)
	    /* avoid returning junk */
	    ret = (ttany)0;
	return_value->value = ret;
	return TT_TRUE;
    }
    return TT_FALSE;
}

