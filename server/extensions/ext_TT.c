
/*
 * TT.c -- extension to load libTT library within twin server
 */

#include "twin.h"
#include "extreg.h"
#include "unaligned.h"
#include "util.h"

#include <Tw/Tw.h>     /* for #include <Tw/Twstat.h> */
#include <Tw/Twstat.h> /* for TWS_* */

#include <TT/TT.h>
#include <TT/TTmem.h>

#include "ext_TT.h"

/*
 * We cannot allow clients to invoke our TTExitMainLoop() function,
 * since we may be serving multiple clients.
 * 
 * We cannot allow clients to invoke our TTCall*_ttmethod() methods,
 * since we have no way to properly receive arrays that may be
 * hidden in their args... we could SEGFAULT the server!
 * 
 * also, invoking TTCall*_ttmethod() on the client uses client-side
 * libTT logic to invoke the correct method and we receive that call,
 * not a call to TTCall*_ttmethod().
 * 
 * So the only possibile case we receive a call to our TTCall*_ttmethod()
 * is a malicious client or client-side libTT violating the protocol:
 * one more reason not to allow that.
 * 
 * Exception: invoking TTCallR_ttmethod would be safe, but it's still useless,
 * so we do not allow it for uniformity.
 * 
 * Warning: TTCallL_ttmethod is a variadic function (takes a variable number of
 * arguments like printf()), so libTT may omit it from the list of ttmethods
 * i.e. TTGetByAddress_ttmethod(TTCallL_ttmethod) may return TT_NOID.
 */
static ttfunction_fn method_unsafe_addresses[] = {
    (ttfunction_fn)TTExitMainLoop,
    (ttfunction_fn)TTCallA_ttmethod,
    (ttfunction_fn)TTCallL_ttmethod,
    (ttfunction_fn)TTCallR_ttmethod,
    (ttfunction_fn)TTCallV_ttmethod
};

#define method_unsafe_n (sizeof(method_unsafe_addresses)/sizeof(method_unsafe_addresses[0]))

static ttopaque method_unsafe_array[method_unsafe_n];


static ttopaque method_first, method_last;


TT_INLINE ttbyte ext_TT_init_data(void) {
    ttopaque n;
    
    if ((method_first = (ttopaque)TTGetFirst(TTClass_ttmethod)) &&
	(method_last = 1 + (ttopaque)TTGetLast(TTClass_ttmethod))) {
	
	for (n = 0; n < method_unsafe_n; n++) {
	    method_unsafe_array[n] =
		(ttopaque)TTGetByAddress_ttmethod(method_unsafe_addresses[n]);
	}
	
	return TT_TRUE;
    }
    return TT_FALSE;
}


static udat TTType2TWS(ttopaque type) {
    ttclass Class = TTFromType_ttclass(type);
    
    if (TTIsArrayType_ttclass(type))
	return TWS_vec|TWS_vecW|TWS_byte;

#define C(t1,t2) if (Class == TT_CAT(TTClass_,t1)) return TT_CAT(TWS_,t2)
	
    C(void,     void); /* used for return type only */
    C(ttbyte,   byte);
    C(ttshort,  dat);
    C(ttint,    ldat);
    C(ttcol,    hwcol);
    C(ttfont,   hwfont);
    C(ttattr,   hwattr);
    C(ttopaque, topaque);
    C(ttany,    tany);
#undef C

    return TWS_topaque; /* also used by all libTT objects */
}


/*
 * Fill tws and args with method arguments types, encoding as needed
 */
TT_INLINE ttbyte FillTypes(ttopaque m, ttopaque *args_n, tsfield tws, ttarg * args) {
    ttvector v;
    ttopaque n;
    ttany *proto;
    
    if (m >= method_first && m < method_last &&
	(v = TTGetParameterType_ttmethod((ttmethod)m)) &&
	(!(n = TTGetArrayN_ttvector(v)) || (proto = TTGetArray_ttvector(v))) &&
	n <= *args_n) {
	    
	while (n--) {
	    tws->type = TTType2TWS(args->type = (ttopaque)*proto++);
	    tws++;
	    args++;
	}
	return TT_TRUE;
    }
    return TT_FALSE;
}

TT_INLINE void CopyArgsTWS2TT(ttopaque args_n, tsfield tws, ttarg * args) {
    while (args_n--) {
	if (tws->type != (TWS_vec|TWS_vecW|TWS_byte))
	    args->value = tws->TWS_field_scalar;
	else {
	    args->value = (ttopaque)tws->TWS_field_vecV;
	    args->size  = tws->TWS_field_vecL;
	}
	tws++;
	args++;
    }
}

TT_INLINE void CopyArgsTT2TWS(ttopaque args_n, ttarg * args, tsfield tws) {
    while (args_n--) {
	if (!TTIsArrayType_ttclass(args->type))
	    tws->TWS_field_scalar = args->value;
	else {
	    tws->TWS_field_vecV = (void *)(ttopaque)args->value;
	    tws->TWS_field_vecL = args->size;
	}
	tws++;
	args++;
    }
}

static ttbyte IsSafeMethod(ttopaque m) {
    ttopaque n;
    
    if (m >= method_first && m < method_last) {
	for (n = 0; n < method_unsafe_n; n++)
	    if (m == method_unsafe_array[n])
		return TT_FALSE;
	return TT_TRUE;
    }
    return TT_FALSE;
}


#ifdef THIS_MODULE
static
#endif
tany ext_TT_CallBExtension(extension Extension, topaque len, TT_CONST byte *data, void *return_type) {
    ttarg args[TW_MAX_ARGS_N];
    struct s_tsfield tws[TW_MAX_ARGS_N];
    ttarg arg0[1];
    tsfield tws0;
    ttopaque args_n;
    ttmethod m;

    /*
     * actually, we receive a (tsfield) in return_type.
     * it is initialized with the return type expected by the client,
     * but we do not trust that data, and we may overwrite/integrate it
     * if it makes sense.
     */
    tws0 = (tsfield)return_type;
  
    /*
     * first arg is the method to call: is always (tany), extract it first to get
     * types for the other args.
     */
    args_n = 1;
    tws->type = TWS_tany;
    
    if (Ext(Socket,DecodeExtension)(&len, &data, &args_n, tws) &&
	
	(m = (ttmethod)(ttopaque)tws->TWS_field_scalar) &&
	IsSafeMethod((ttopaque)m) &&
	FillTypes((ttopaque)m, &args_n, tws, args) &&
	
	(args_n = TW_MAX_ARGS_N,
	 /* extract all other args */
	 Ext(Socket,DecodeExtension)(&len, &data, &args_n, tws)) &&
	
	/* check there are no spurious bytes after end of data */
	len == 0 &&
	
	/* check all arrays have the length we expect */
	(CopyArgsTWS2TT(args_n, tws, args),
	 TTCheckArraySizesA_ttmethod(m, args_n, args)) &&

	/* finally call the method */
	TTCallA_ttmethod(m, arg0, args_n, args)) {

	/* collect return value and type */
	CopyArgsTT2TWS(1, arg0, tws0);
	return arg0->value;
    }
    return (tany)0;
}




TT_DECL_MAGIC(ext_TT_magic);

#ifdef THIS_MODULE
byte InitModule(extension E)
#else
byte ext_TT_Init(extension E)
#endif
{
    return
	TTCheckMagic(ext_TT_magic) &&
#ifdef CONF__ALLOC
	(TTConfigAllocMem(AllocMem, ReAllocMem, FreeMem), TT_TRUE) &&
#endif
	TTOpen("twin@:2" /* "twin_detunnel" */) &&
	ext_TT_init_data() &&
	(E->CallB = ext_TT_CallBExtension);
}


TT_INLINE void ext_TT_quit_data(void) {
}


#ifdef THIS_MODULE
void QuitModule(extension E)
#else
void ext_TT_Quit(extension E)
#endif
{
    ext_TT_quit_data();
    TTClose();
}

