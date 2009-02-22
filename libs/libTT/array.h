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
#ifndef _TT_ARRAY_H
#define _TT_ARRAY_H


/* useful defines to encode type attributes */




/* the corresponding field or parameter is read-only */
#define ttclass_type_read		(0x1<<order_ttclass_usedbits)
/* the corresponding field or parameter is write-only */
#define ttclass_type_write		(0x2<<order_ttclass_usedbits)
/* the corresponding field or parameter may be destroyed */
#define ttclass_type_die		(0x4<<order_ttclass_usedbits)
/* the corresponding field or parameter is an array */
#define ttclass_type_array		(0x8<<order_ttclass_usedbits)

#define ttclass_type_usedbits		4
#define ttclass_type_bitmask		(((1<<ttclass_type_usedbits)-1) << order_ttclass_usedbits)


/* bits used to encode the array attributes */
#define ttclass_type_array_firstbit	(ttclass_type_usedbits + order_ttclass_usedbits)
#define ttclass_type_array_usedbits	(e_WFP*2 + e_WOP)
#define ttclass_type_array_lastbit	(ttclass_type_array_firstbit + ttclass_type_array_usedbits - 1)


#define ttclass_type_array_bitmask	(((1<<ttclass_type_array_usedbits)-1) << ttclass_type_array_firstbit)

#define ttclass_type_full_bitmask	(ttclass_type_bitmask|ttclass_type_array_bitmask)
#define ttclass_type_full_usedbits	(ttclass_type_usedbits+ttclass_type_array_usedbits)








/* TT_ARG_READ */
#define e_R	ttclass_type_read
/* TT_ARG_WRITE */
#define e_W	ttclass_type_write
/* TT_ARG_DIE */
#define e_D	ttclass_type_die
#define e_AA	ttclass_type_array
#define e_FB	ttclass_type_array_firstbit
#define e_LB	ttclass_type_array_lastbit
/* TT_ARG_ARRAY(...) */
#define e_A(x)	((x)<<e_FB|e_AA)
/* TT_ARG_ARRAY_Z */
#define e_Z	e_A(e_OP(e_OPZ))

/* width of _F() and _P() bitfields */
#define e_WFP	6
/* mask of _F() and _P() bitfields */
#define e_MFP	((1<<e_WFP) - 1)
/* flag of _F() and _P() bitfields */
#define e_FFP	(1 << (e_WFP-1))

/* width of e_OP bitfields */
#define e_WOP	2
/* mask of e_OP bitfields */
#define e_MOP	((1<<e_WOP)-1)

/* _F(...) */
#define e_F(t,f)	(e_FFP | (TT_CAT3(t,_,f)-TT_CAT(t,_field_first)))
/* _P(...) */
#define e_P(p)		(e_FFP | p)

/* shift of e_OP bitfields with respect to the first _F() and _P() bitfield */
#define e_OP(p)		(p << (e_WFP*2))
/* +, *, ... */
#define e_SET		0
#define e_ADD		1
#define e_MUL		2
/* operation corresponding to TT_ARG_ARRAY_Z */
#define e_OPZ		3



#if !defined(TT_MAGIC_SHIFT)
# error internal error: undefined TT_MAGIC_SHIFT !
#endif
#if TT_MAGIC_SHIFT < ttclass_type_full_usedbits + order_ttclass_usedbits
# error internal error: ttclass_type_full_usedbits + order_ttclass_usedbits overflows TT_MAGIC_SHIFT !
#endif


#endif /* _TT_ARRAY_H */

