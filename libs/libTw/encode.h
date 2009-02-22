/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

/*
 * encoding functions to serialize libTw call args in a buffer
 * that will be written in the socket connection to the server.
 */


#if defined(CONF__ASM) && defined(TW_HAVE_ASM)
#define _ TWS_field_scalar	
/*
 * arguments of EncodeCall and EncodeArgs are different if using
 * asm functions Tw_* : array sizes are not placed as arguments
 * before the array, so must be calculated separately.
 */
static topaque EncodeArraySize(fn_order o, uldat n, tsfield a) {
    topaque L = 0;
    switch (o) {

#include "libTw3_m4.h"

      case order_StatObj:
	switch (n) {
	    case 3: L = a[2]._; break;
	}
      default:
	break;
    }
    return L;
}
# undef _
#endif /* defined(CONF__ASM) && defined(TW_HAVE_ASM) */


/*
 * read (va_list va) and fill (tsfield a)
 * 
 * let (tobj) fields decade into (uldat),
 * since from client side they are the same.
 */
TW_INLINE udat EncodeArgs(fn_order o, uldat *Space, va_list va, tsfield a) {
    TW_CONST byte *Format = Functions[o].format + 1;
    uldat arglen, space;
    udat N;
    byte c, t, variable_return_type = FALSE;
    
    for (N = space = 0; (c = *Format++); N++, a++) {
	t = *Format++;
	if (t >= TWS_highest)
	    /*
	     * let (tobj) fields decade into (uldat),
	     * since from client side they are the same .
	     */
	    t = TWS_uldat;
	
	if (N) switch (c) {
	  case '_':
	  case 'x':
	    space += Tw_MagicData[a->type = t];
	    a->TWS_field_scalar = va_arg(va, tany);
	    break;
	  case 'W':
	  case 'Y':
	    a->type = TWS_vec | TWS_vecW | t;
#if defined(CONF__ASM) && defined(TW_HAVE_ASM)
	    arglen = EncodeArraySize(o, N, a-N);
#else
	    arglen = va_arg(va, tany);
#endif
	    if (!(a->TWS_field_vecV = (TW_CONST void *)(topaque)va_arg(va, tany)))
		arglen = 0;
	    a->TWS_field_vecL = arglen;
	    space += sizeof(topaque) + arglen;
	    break;
	  case 'V':
	  case 'X':
	    a->type = TWS_vec | t;
	    space += 
#if defined(CONF__ASM) && defined(TW_HAVE_ASM)
		a->TWS_field_vecL = EncodeArraySize(o, N, a-N);
#else
		a->TWS_field_vecL = va_arg(va, tany);
#endif
	    a->TWS_field_vecV = (TW_CONST void *)(topaque)va_arg(va, tany);
	    break;
	  default:
	    return (udat)-1;

	} else switch (c) {
	    /* parse arg 0 (return value) */
	  case '_':
	  case 'x':
	  case 'v':
	    a->type = t;
	    a->hash = Tw_MagicData[t]; /* sizeof(return type) */
	    break;
	  case 'O':
	    /* will be filled by last arg */
	    variable_return_type = TRUE;
	    break;
	  default:
	    return (udat)-1;
	}
    }
    if (variable_return_type) {
	/* fill return type from first arg - which must be 'udat' */

	if (N > 1 && a[-1].type == (TWS_vec|TWS_byte) && a[-1].TWS_field_vecL == 2) {
	    Format = (TW_CONST byte *)a[-1].TWS_field_vecV;
	    switch (*Format++) {
	      case '_':
	      case 'x':
	      case 'v':
		if ((t = *Format++) < TWS_highest) {
		    a->type = t;
		    a->hash = Tw_MagicData[t]; /* sizeof(return type) */
		    break;
		}
		return (udat)-1;
	      default:
		return (udat)-1;
	    }
	} else
	    return (udat)-1;
    }
    *Space = space;
    return N - 1; /* arg 0 is not a call argument */
}


/* append (tsfield)a to the serialization buffer being constructed */
static byte *PushArg(byte *buf, tsfield a) {
    switch (a->type) {
#define ENC_CASE(type) case TW_CAT(TWS_,type): { type tmp = (type)a->TWS_field_scalar; Push(buf,type,tmp); }; break
	ENC_CASE(byte);
	ENC_CASE(udat);
#if 0
	/* we never meet this here, as EncodeArgs() above turns (tobj) into (uldat) */
      case TWS_tobj:
	/* FALLTHROUGH */
#endif
	ENC_CASE(uldat);
	ENC_CASE(hwcol);
	ENC_CASE(topaque);
	ENC_CASE(tany);
	ENC_CASE(hwfont);
	ENC_CASE(hwattr);
#undef ENC_CASE
      default:
	if (a->type & TWS_vec) {
	    if (a->type & TWS_vecW)
		Push(buf, topaque, a->TWS_field_vecL);
	    if (a->TWS_field_vecL)
		PushV(buf, a->TWS_field_vecL, a->TWS_field_vecV);
	}
	break;
    }
    return buf;
}

static void DecodeReply(byte *buf, tsfield a) {
    switch (a->type) {
#define DEC_CASE(type) case TW_CAT(TWS_,type): { type tmp; Pop(buf,type,tmp); a->TWS_field_scalar = tmp; }; break
	DEC_CASE(byte);
	DEC_CASE(udat);
#if 0
	/* we never meet this here, as EncodeArgs() above turns (tobj) into (uldat) */
      case TWS_tobj:
	/* FALLTHROUGH */
#endif
	DEC_CASE(uldat);
	DEC_CASE(hwcol);
	DEC_CASE(topaque);
	DEC_CASE(tany);
	DEC_CASE(hwfont);
	DEC_CASE(hwattr);
      default:
	break;
    }
}












/*
 * encoding functions to serialize libTw *EXTENSIONS* call args in a buffer
 * that will be written in the socket connection to the server.
 */



/*
 * read (va_list va) and fill (tsfield a)
 * 
 * this is very similar to vanilla EncodeArgs()
 */
static udat ExtensionEncodeArgs(TW_CONST byte *Format, uldat *Space, va_list va, tsfield a) {
#if 0
    TW_CONST byte *Format = ExtensionGetFormat(o);
#endif
    uldat space;
    udat N;
    byte c, t;
    
    if (!Format)
	return 0;
    
    for (N = space = 0; (c = *Format++); N++, a++) {
	a->type = t = *Format++;
	
	if (N) {
	    if (t < TWS_highest) {
		space += Tw_MagicData[t];
		a->TWS_field_scalar = va_arg(va, tany);
		break;
	    } else if (t == TWS_array) {
		/* here TWS_array is used to specify W()-style arrays */
		a->type = TWS_vec | TWS_vecW | TWS_byte;
		space += sizeof(topaque);
		space += a->TWS_field_vecL = va_arg(va, tany);
		a->TWS_field_vecV = (TW_CONST void *)(topaque)va_arg(va, tany);
	    }
	} else {
	    /* parse arg 0 (return value) */
	    if (t < TWS_highest) {
		a->type = t;
		a->hash = Tw_MagicData[t]; /* sizeof(return type) */
	    } else if (t == TWS_array) {
		/* here TWS_array is used to specify V()-style arrays */
		a->type = TWS_vec | TWS_byte;
		a->hash = 0;
	    }
	}
    }
    *Space = space;
    return N - 1; /* arg 0 is not a call argument */
}


/* WARNING: here a[0] is the return value ! */
static uldat ExtensionComputeSpace(tany N, tsfield a) {
    uldat space;
    udat t;
    
    /* parse arg0 (return value) */
    if ((t = a->type) != TWS_void && t < TWS_highest)
	a->hash = Tw_MagicData[t]; /* sizeof(return type) */
    else
	a->hash = 0;
    
    /* skip a[0] */
    for (a++, space = 0; N; N--, a++) {
	if ((t = a->type) && t < TWS_highest) {
	    space += Tw_MagicData[t];
	} else if (t & TWS_vec) {
	    space += sizeof(topaque) + a->TWS_field_vecL;
	}
    }
    return space;
}


