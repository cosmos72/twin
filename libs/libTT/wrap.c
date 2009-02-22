
/*
 *  wrap.c  --  wrappers around exported libTT methods
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

#include "TT.h"

#include "mutex.h"

#include "TTextern.h"
#include "TTassert.h"

#include "utils.h"
#include "inlines.h"

TT_MUTEX_HELPER_DEFS(static);

#define CAN_SET(o, t) (TTAssert((o) && IS(t,(o))) && !((o)->oflags & ttobject_oflags_const))

/* ttobj */


/* ttobject */
TT_INLINE void Ref_ttobject(ttobject o) {
    if (o)
	o->refcount++;
}
TT_INLINE void Unref_ttobject(ttobject o) {
    if (o && !--o->refcount)
	Del(o);
}
TT_INLINE ttbyte SetUserData_ttobject(ttobject o, ttany user_data) {
    if (CAN_SET(o, ttobject)) {
	if (IS(ttcomponent,o))
	    return !!TTSetKeyData_ttcomponent((tt_obj)o->id, "user_data", user_data);

	o->user_data = user_data;
	return TT_TRUE;
    }
    return TT_FALSE;
}


/* ttvector */

TT_INLINE ttbyte Add_ttvector(ttvector o, ttopaque pos, ttany value) {
    return
	TTAssert(o && IS(ttvector,o)) && pos <= o->array_n &&
	o->Class->AddY(o, pos, 1, &value);
}
TT_INLINE ttbyte Append_ttvector(ttvector o, ttany value) {
    return
	TTAssert(o && IS(ttvector,o)) &&
	o->Class->AddY(o, o->array_n, 1, &value);
}
TT_INLINE ttbyte AddA_ttvector(ttvector o, ttopaque pos, ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttarg *values) {
    ttany *args = (ttany *)0;
    ttopaque i;
    ttbyte ret = TT_FALSE;
    
    if (TTAssert(o && IS(ttvector,o)) && pos <= o->array_n &&
	(!value_n || (args = TTAllocMem(value_n * sizeof(ttany))))) {

	for (i = 0; i < value_n; i++)
	    args[i] = values[i].value;
	
	ret = o->Class->AddY(o, pos, value_n, args);
	
	if (args)
	    TTFreeMem(args);
    }
    return ret;
}
TT_INLINE ttbyte AddV_ttvector(ttvector o, ttopaque pos, ttopaque value_n, va_list *values) {
    ttany *args = (ttany *)0;
    ttopaque i;
    ttbyte ret = TT_FALSE;
    
    if (TTAssert(o && IS(ttvector,o)) && pos <= o->array_n &&
	(!value_n || (args = TTAllocMem(value_n * sizeof(ttany))))) {

	for (i = 0; i < value_n; i++)
	    args[i] = va_arg((*values), ttany);
	
	ret = o->Class->AddY(o, pos, value_n, args);
	
	if (args)
	    TTFreeMem(args);
    }
    return ret;
}
TT_INLINE ttbyte AddR_ttvector(ttvector o, ttopaque pos, TT_ARG_READ ttvector v) {
    if (TTAssert(o && IS(ttvector,o)) && v && TTAssert(IS(ttvector,v))) {
	return o->Class->AddY(o, pos, v->array_n, v->array);
    }
    return TT_FALSE;
}
TT_INLINE ttbyte RemoveAt_ttvector(ttvector o, ttopaque pos) {
    return TTAssert(o && IS(ttvector,o)) &&
	o->Class->RemoveRange(o, pos, 1);
}


/* ttevent */
TT_INLINE void Fire_ttevent(ttevent ev, ttcomponent o) {
    if (TTAssert(ev && IS(ttevent,ev)) && o && TTAssert(IS(ttcomponent, o))) {
	FireEvent(ev, o);
    }
}


/* utilities */
#define MAX_mask0 (TT_SIZEOF_ttopaque << 3)
static ttany *Bsearch_ttany(ttany key, ttany *vec, ttany n, ttopaque *best_i_up) {
    ttopaque low = 0, up = n, i;
    
    while (low < up) {
	i = (low + up) / 2;
	if (vec[i] > key)
	    up = i;
	else if (vec[i] < key)
	    low = i + 1;
	else
	    return &vec[i];
    }
    if (best_i_up)
	*best_i_up = up;

    return NULL;
}
static ttbyte Combine_bit(ttbyte m1, ttbyte m2, ttuint truth_table) {
    if (m1 & 1)
	truth_table >>= 1;
    if (m2 & 1)
	truth_table >>= 2;
	
    return truth_table & 1;
}
static ttany Combine_ttany(ttany m1, ttany m2, ttany top_bit, ttuint truth_table) {
#if 0
    /* compact, slow version */
    ttany i, ret = 0;
    ttuint truth;
    
    for (i = top_bit; i; i >>= 1) {
	truth = truth_table;
	
	if (m1 & i)
	    truth >>= 1;
	if (m2 & i)
	    truth >>= 2;
	
	if (truth & 1)
	    ret |= i;
    }
    return ret;
#else
    /* big, fast version */
    ttany ret = (ttany)0;
    
    switch (truth_table & 0xF) {
      case 0x0: /* 0b0000 */
	ret =   (ttany)0;
	break;
      case 0x1: /* 0b0001 */
	ret =  ~m1 & ~m2;
	break;
      case 0x2: /* 0b0010 */
	ret =   m1 & ~m2;
	break;
      case 0x3: /* 0b0011 */
	ret =        ~m2;
	break;
      case 0x4: /* 0b0100 */
	ret =  ~m1 &  m2;
	break;
      case 0x5: /* 0b0101 */
	ret =  ~m1;
	break;
      case 0x6: /* 0b0110 */
	ret =   m1 ^  m2;
	break;
      case 0x7: /* 0b0111 */
	ret =   m1 |  m2;
	break;
      case 0x8: /* 0b1000 */
	ret =   m1 &  m2;
	break;
      case 0x9: /* 0b1001 */
	ret =  ~m1 ^  m2;
	break;
      case 0xA: /* 0b1010 */
	ret =   m1;
	break;
      case 0xB: /* 0b1011 */
	ret =   m1 | ~m2;
	break;
      case 0xC: /* 0b1100 */
	ret =         m2;
	break;
      case 0xD: /* 0b1101 */
	ret =  ~m1 |  m2;
	break;
      case 0xE: /* 0b1110 */
	ret =   m1 |  m2;
	break;
      case 0xF: /* 0b1111 */
	ret =   ~(ttany)0;
	break;
    }
    
    return ret & (top_bit | (top_bit - 1));
#endif
}


/* ttbitmask */
static void Clear_ttbitmask(ttbitmask o) {
    if (TTAssert(o && IS(ttbitmask,o))) {
	o->oflags &= ~ttbitmask_oflags_mask_reverse;
	o->mask0 = o->mask_n = (opaque)0;
    }
}
static void Fill_ttbitmask(ttbitmask o) {
    if (TTAssert(o && IS(ttbitmask,o))) {
	o->oflags |= ttbitmask_oflags_mask_reverse;
	o->mask0 = o->mask_n = (opaque)0;
    }
}
static void Negate_ttbitmask(ttbitmask o) {
    if (TTAssert(o && IS(ttbitmask,o))) {
	o->oflags ^= ttbitmask_oflags_mask_reverse;
    }
}
void Change_ttbitmask(ttbitmask o, ttuint truth_table) {
    if (TTAssert(o && IS(ttbitmask,o))) {
	switch (truth_table & 0x3) {
	  case 0x0: /* 0b00 */
	    Clear_ttbitmask(o);
	    break;
	  case 0x1: /* 0b01 */
	    Negate_ttbitmask(o);
	    break;
	  case 0x2: /* 0b10 */
	    break;
	  case 0x3: /* 0b11 */
	    Fill_ttbitmask(o);
	    break;
	}
    }
}
static ttbyte Contains_ttbitmask(ttbitmask o, ttany value) {
    ttbyte ret = TT_FALSE;
    
    if (TTAssert(o && IS(ttbitmask,o))) {
	if (value < MAX_mask0)
	    ret = !!(o->mask0 & ((opaque)1 << value));
	else if (o->mask_n)
	    ret = !!(Bsearch_ttany(value, o->mask, o->mask_n, NULL));
	
	ret ^= !!(o->oflags & ttbitmask_oflags_mask_reverse);
    }
    return ret;
}
TT_INLINE ttbitmask CreateB_ttbitmask(ttbitmask b) {
    ttbitmask o;
    ttopaque max, n;
    
    if (TTAssert(b && IS(ttbitmask,b))) {
	if ((o = TNEW(ttbitmask))) {
	    n = b->mask_n;
	    if ((max = b->mask_max)) {
		if ((o->mask = TTReallocMem(o->mask, max * TT_SIZEOF_ttany))) {
		    if (n)
			TTCopyMem(b->mask, o->mask, n * TT_SIZEOF_ttany);
		} else {
		    TDEL(o);
		    return (ttbitmask)0;
		}
	    }
	    o->mask_n = n;
	    o->mask_max = max;
	    o->mask0 = b->mask0;
	    o->user_data = b->user_data;
	    o->oflags = b->oflags & ttbitmask_oflags_mask_reverse;
	    return o;
	}
    }
    return (ttbitmask)0;
}
TT_INLINE ttbyte Enlarge_ttbitmask(ttbitmask o) {
    ttany *mask;
    ttopaque max = o->mask_max ? o->mask_max << 1 : 30;
    
    if ((mask = TTReallocMem(o->mask, max * TT_SIZEOF_ttany))) {
	o->mask = mask;
	o->mask_max = max;
	return TT_TRUE;
    }
    return TT_FALSE;
}

static ttbyte real_Add_ttbitmask(ttbitmask o, ttany value) {
    ttopaque best_i;
    
    if (value < MAX_mask0)
	o->mask0 |= (opaque)1 << value;
    else if (Bsearch_ttany(value, o->mask, o->mask_n, &best_i))
	/* already set, return */
	;
    else if (o->mask_n < o->mask_max || Enlarge_ttbitmask(o)) {
	/* make room for value */
	if (best_i < o->mask_n)
	    TTMoveMem(o->mask + best_i, o->mask + best_i + 1, (o->mask_n - best_i) * TT_SIZEOF_ttany);
	
	o->mask[best_i] = value;
	o->mask_n++;
    } else {
	/* OUT OF MEMORY! */
	return TT_FALSE;
    }
    return TT_TRUE;
}
static void real_Remove_ttbitmask(ttbitmask o, ttany value) {
    if (value < MAX_mask0)
	o->mask0 &= ~((opaque)1 << value);
    else if (o->mask_n) {
	ttany *match, n;
	if ((match = Bsearch_ttany(value, o->mask, o->mask_n, NULL))) {
	    if ((n = match - o->mask) + 1 < o->mask_n)
		TTMoveMem(o->mask + n + 1, o->mask + n, (o->mask_n - n - 1) * TT_SIZEOF_ttany);
	    
	    o->mask_n--;
	}
    }
}
static ttbyte Add_ttbitmask(ttbitmask o, ttany value) {
    if (TTAssert(o && IS(ttbitmask,o))) {
	if (o->oflags & ttbitmask_oflags_mask_reverse) {
	    real_Remove_ttbitmask(o, value);
	    return TT_TRUE;
	}
	return real_Add_ttbitmask(o, value);
    }
    return TT_FALSE;
}
static ttbyte Remove_ttbitmask(ttbitmask o, ttany value) {
    if (TTAssert(o && IS(ttbitmask,o))) {
	if (o->oflags & ttbitmask_oflags_mask_reverse) {
	    return real_Add_ttbitmask(o, value);
	}
	real_Remove_ttbitmask(o, value);
	return TT_TRUE;
    }
    return TT_FALSE;
}
static ttbyte AddR_ttbitmask(ttbitmask o, ttvector v) {
    ttopaque i, n;
    TT_CONST ttany *value;
    
    /* exactly one of Add_ttbitmask(), Remove_ttbitmask() cannot fail */
    if (TTAssert(o && IS(ttbitmask,o)) && v && TTAssert(IS(ttvector,v))) {
	n = v->array_n;
	value = v->array;
	for (i = 0; i < n; i++) {
	    if (!Add_ttbitmask(o, value[i]))
		break;
	}
	if (i == n)
	    return TT_TRUE;
	
	/* failed. undo. */
	n = i;
	for (i = 0; i < n; i++)
	    Remove_ttbitmask(o, value[i]);
    }
    return TT_FALSE;
}
static ttbyte RemoveR_ttbitmask(ttbitmask o, ttvector v) {
    ttopaque i, n;
    TT_CONST ttany *value;

    /* exactly one of Add_ttbitmask(), Remove_ttbitmask() cannot fail */
    if (TTAssert(o && IS(ttbitmask,o))) {
	n = v->array_n;
	value = v->array;
	for (i = 0; i < n; i++) {
	    if (!Remove_ttbitmask(o, value[i]))
		break;
	}
	if (i == n)
	    return TT_TRUE;
	
	/* failed. undo. */
	n = i;
	for (i = 0; i < n; i++)
	    Add_ttbitmask(o, value[i]);
    }
    return TT_FALSE;
}
static ttbyte CombineB_ttbitmask(ttbitmask o, TT_ARG_READ ttbitmask o2, ttuint truth_table) {
    s_ttbitmask save;
    ttbitmask o1;
    ttopaque i, value;
    ttuint truth;
    ttbyte reverse0, reverse1, reverse2, true1, ok = TT_TRUE;
    
    if (TTAssert(o && IS(ttbitmask,o)) && o2 && TTAssert(IS(ttbitmask,o2)) &&
	(o1 = CreateB_ttbitmask(o))) {
	
	/* save original values */
	save.oflags   = o->oflags;
	save.mask0    = o->mask0;
	save.mask_max = o->mask_max;
	save.mask_n   = o->mask_n;
	save.mask     = o->mask;
	
	o->mask0 = 0;
	o->mask_max = o->mask_n = 0;
	o->mask = NULL;
	
	reverse1 = !!(o1->oflags & ttbitmask_oflags_mask_reverse);
	reverse2 = !!(o2->oflags & ttbitmask_oflags_mask_reverse);
	reverse0 = Combine_bit(reverse1, reverse2, truth_table);

	if (reverse0)
	    o->oflags |= ttbitmask_oflags_mask_reverse;
	else   
	    o->oflags &= ~ttbitmask_oflags_mask_reverse;
	
	/* apply to values in o2 */
	for (i = 0; ok && i < o2->mask_n; i++) {
	    truth = truth_table;
	    value = o2->mask[i];
	    
	    if ((true1 = Contains_ttbitmask(o1, value)))
		truth >>= 1;
	    if (!(o2->oflags & ttbitmask_oflags_mask_reverse))
		/* Contains_ttbitmask(o2, value) */
		truth >>= 2;
	    if ((truth & 1) ^ reverse0)
		ok = real_Add_ttbitmask(o, value);
	    if (true1)
		real_Remove_ttbitmask(o1, value);
	}
	/* apply to remaining values in o1 */
	for (i = 0; ok && i < o1->mask_n; i++) {
	    truth = truth_table;
	    value = o1->mask[i];
	    
	    if (!(o1->oflags & ttbitmask_oflags_mask_reverse))
		/* Contains_ttbitmask(o1, value) */
		truth >>= 1;
	    if (Contains_ttbitmask(o2, value))
		truth >>= 2;
	    if ((truth & 1) ^ reverse0)
		ok = real_Add_ttbitmask(o, value);
	}
	if (ok) {
	    ttany m0, m1, m2;
	    
	    m1 = reverse1 ? ~o1->mask0 : o1->mask0;
	    m2 = reverse2 ? ~o2->mask0 : o2->mask0;
	    m0 = Combine_ttany(m1, m2, MINS(ttany), truth_table);
	    o->mask0 = reverse0 ? ~m0 : m0;
	    
	    TDEL(o1);
	    
	    if (save.mask)
		TTFreeMem(save.mask);
	    
	    return TT_TRUE;
	}
	/* failed. restore original values */
	
	TDEL(o1);
	
	o->oflags   = save.oflags;
	o->mask0    = save.mask0;
	o->mask_max = save.mask_max;
	o->mask_n   = save.mask_n;
	o->mask     = save.mask;

    }
    return TT_FALSE;
}
static ttbyte Combine_ttbitmask(ttbitmask o, ttany value, ttuint truth_table) {
    ttbitmask o2;
    ttbyte ret = TT_FALSE;
    if (TTAssert(o && IS(ttbitmask,o)) &&
	(o2 = Create_ttbitmask(value))) {
	
	ret = CombineB_ttbitmask(o, o2, truth_table);
	TDEL(o2);
    }
    return ret;
}
static ttbyte CombineR_ttbitmask(ttbitmask o, ttvector v, ttuint truth_table) {
    ttbitmask o2;
    ttbyte ret = TT_FALSE;
    if (TTAssert(o && IS(ttbitmask,o)) &&
	v && TTAssert(IS(ttvector,v)) &&
	(o2 = CreateR_ttbitmask(v))) {
	
	ret = CombineB_ttbitmask(o, o2, truth_table);
	TDEL(o2);
    }
    return ret;
}
static ttbyte AddB_ttbitmask(ttbitmask o, TT_ARG_READ ttbitmask value) {
    return CombineB_ttbitmask(o, value, 0xE /* o OR value */);
}
static ttbyte RemoveB_ttbitmask(ttbitmask o, TT_ARG_READ ttbitmask value) {
    return CombineB_ttbitmask(o, value, 0x2 /* o AND (NEGATE value) */);
}


/* tteventmask */
TT_INLINE ttbyte SetEvtypeMask_tteventmask(tteventmask o, TT_ARG_DIE ttbitmask em) {
    if (CAN_SET(o, tteventmask) && (!em || TTAssert(IS(ttbitmask,em)))) {
	
	if (em != o->evtype_mask) {
	    if (o->evtype_mask)
		TDEL(o->evtype_mask);
	    o->evtype_mask = em;
	}
	return TT_TRUE;
    }
    return TT_FALSE;
}
TT_INLINE ttbyte SetEvcodeMask_tteventmask(tteventmask o, TT_ARG_DIE ttbitmask em) {
    if (CAN_SET(o, tteventmask) && (!em || TTAssert(IS(ttbitmask,em)))) {
	
	if (em != o->evcode_mask) {
	    if (o->evcode_mask)
		TDEL(o->evcode_mask);
	    o->evcode_mask = em;
	}
	return TT_TRUE;
    }
    return TT_FALSE;
}
TT_INLINE ttbyte SetComponentMask_tteventmask(tteventmask o, TT_ARG_DIE ttbitmask cm) {
    if (CAN_SET(o, tteventmask) && (!cm || TTAssert(IS(ttbitmask,cm)))) {
	
	if (cm != o->component_mask) {
	    if (o->component_mask)
		TDEL(o->component_mask);
	    o->component_mask = cm;
	}
	return TT_TRUE;
    }
    return TT_FALSE;
}
TT_INLINE ttbyte SetTruthTable_tteventmask(tteventmask o, ttuint truth_table) {
    if (CAN_SET(o, tteventmask)) {
	if (truth_table != o->truth_table) {
	    o->truth_table = truth_table;
	}
	return TT_TRUE;
    }
    return TT_FALSE;
}

/* ttlistener */
TT_INLINE ttbyte SetArgsR_ttlistener(ttlistener o, TT_ARG_DIE ttvector args) {
    if (CAN_SET(o, ttlistener) && (!args || TTAssert(IS(ttvector,args)))) {
	
	if (TTAssertAlways(args != o->args)) {
	    if (o->args)
		TDEL(o->args);
	    o->args = args;
	}
	return TT_TRUE;
    }
    return TT_FALSE;
}
TT_INLINE ttbyte SetEventMask_ttlistener(ttlistener o, TT_ARG_DIE tteventmask em) {
    if (CAN_SET(o, ttlistener) && (!em || TTAssert(IS(tteventmask,em)))) {
	if (em != o->event_mask) {
	    if (o->event_mask)
		TDEL(o->event_mask);
	    o->event_mask = em;
	}
	return TT_TRUE;
    }
    return TT_FALSE;
}


/* tttimer */
TT_INLINE void SetDelay_tttimer(tttimer o, ttany delay_t, ttany delay_f) {
    ttbyte enabled;
    
    if (TTAssert(o && IS(tttimer,o))) {
	GetNow();
	
	if ((enabled = !!(o->lflags & tttimer_lflags_enabled)))
	    Activate_tttimer(o, TT_FALSE);
	o->delay_t = TNow + delay_t;
	o->delay_f = FNow + delay_f;
	if (enabled)
	    Activate_tttimer(o, TT_TRUE);
    }
}


/* ttdata */
#if 0
TT_INLINE ttbyte SetData_ttdata(ttdata o, ttany data) {
    /* implemented in libTT.c */
}
#endif

/* ttcomponent */
TT_INLINE void AddListener_ttcomponent(ttcomponent o, ttlistener l) {
    if (TTAssert(o && IS(ttcomponent,o)) && l && TTAssert(IS(ttlistener,l)) && !l->component)
	l->Class->AddTo(l, o);
}
TT_INLINE void RemoveListener_ttcomponent(ttcomponent o, ttlistener l) {
    if (TTAssert(o && IS(ttcomponent,o)) && l && TTAssert(IS(ttlistener,l)) && l->component == o)
	l->Class->Remove(l);
}
TT_INLINE ttdata SetKeyData_ttcomponent(ttcomponent o, TT_CONST ttbyte *key, ttany data) {
    ttdata d;
    ttopaque key_len;
    
    if (CAN_SET(o, ttcomponent) && key) {
	key_len = TTLenStr(key);
	if ((d = FindByKey_ttdata(o->datas, key, key_len)))
	    /* not d->data = data; we want to keep component->user_data in sync: */
	    SetData_ttdata(d, data);
	else
	    d = protected_Create_ttdata(o, key, key_len, data);

	return d;
    }
    return NULL;
}
TT_INLINE ttdata GetData_ttcomponent(ttcomponent o, TT_CONST ttbyte *key) {
    ttdata d;
    ttopaque key_len;
    
    if (TTAssert(o && IS(ttcomponent,o)) && key) {
	key_len = TTLenStr(key);
	if ((d = FindByKey_ttdata(o->datas, key, key_len)))
	    return d;
    }
    return (ttdata)0;
}
TT_INLINE ttany GetKeyData_ttcomponent(ttcomponent o, TT_CONST ttbyte *key) {
    ttdata d;
    ttopaque key_len;
    
    if (TTAssert(o && IS(ttcomponent,o)) && key) {
	key_len = TTLenStr(key);
	if (key_len == 9 && !TTCmpMem(key, "user_data", 9))
	    return o->user_data;
	if ((d = FindByKey_ttdata(o->datas, key, key_len)))
	    return d->data;
    }
    return (ttany)0;
}
TT_INLINE void UnsetKeyData_ttcomponent(ttcomponent o, TT_CONST ttbyte *key) {
    ttdata d;
    ttopaque key_len;
    
    if (TTAssert(o && IS(ttcomponent,o)) && key) {
	key_len = TTLenStr(key);
	if ((d = FindByKey_ttdata(o->datas, key, key_len)))
	    TDEL(d);
    }
}

/* ttvisible */
TT_INLINE void AddTo_ttvisible(ttvisible o, ttvisible parent, ttany constraints) {
    if (TTAssert(o && IS(ttvisible,o)) && parent && TTAssert(IS(ttvisible,parent)) &&
	!o->parent) {
	
	FIRE_EVENT_O(o->Class->AddTo(o, parent, constraints), o, ttvisible_parent, parent, 0);
	
	/* HACK warning: this is just to let clients bind listeners on parent `child_first' field */
	FIRE_EVENT_O((void)0, parent, ttvisible_child_first, o, 0);
    }
}
TT_INLINE void Remove_ttvisible(ttvisible o) {
    ttvisible parent;
    if (TTAssert(o && IS(ttvisible,o)) && (parent = o->parent)) {

	FIRE_EVENT_O(o->Class->Remove(o), o, ttvisible_parent, 0, parent);
	
	/* HACK warning: this is just to let clients bind listeners on parent `child_first' field */
	FIRE_EVENT_O((void)0, parent, ttvisible_child_first, 0, o);
    }
}
TT_INLINE void SetVisible_ttvisible(ttvisible o, ttbyte on_off) {
    if (TTAssert(o && IS(ttvisible,o))) {
	ttany value, old_value;
	
	value = old_value = o->vflags;
	if (on_off != !!(value & ttvisible_vflags_visible))
	    value ^= ttvisible_vflags_visible;
	
	FIRE_EVENT(o->Class->SetVisible(o, on_off), o, ttvisible_vflags, value, old_value);
    }
}
TT_INLINE ttbyte SetTheme_ttvisible(ttvisible o, tttheme theme) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttvisible)) {
	FIRE_EVENT_O(ret = o->Class->SetTheme(o, theme), o, ttvisible_theme, theme, o->theme);
    }
    return ret;
}
TT_INLINE void Add_ttvisible(ttvisible o, ttvisible child, ttany constraints) {
    if (TTAssert(o && IS(ttvisible,o)) && child && TTAssert(IS(ttvisible,child)))
	AddTo_ttvisible(child, o, constraints);
}
TT_INLINE ttbyte SetRepaint_ttvisible(ttvisible o, ttvisible_repaint_fn repaint) {
    if (CAN_SET(o, ttvisible)) {
	if (o->repaint != repaint) {
	
	    FIRE_EVENT(o->repaint = repaint, o, ttvisible_repaint, (opaque)repaint, (opaque)o->repaint);
	
	    if (o->vflags & ttvisible_vflags_visible)
		Expose_ttvisible(o, ttvisible_repaint_args_WHOLE);
	}
	return TT_TRUE;
    }
    return TT_FALSE;
}


/* ttlayout */



/* ttnative */



/* ttwidget */
static void SetLayout_ttwidget(ttwidget o, ttlayout l) {
    if (CAN_SET(o, ttwidget))
	FIRE_EVENT(o->Class->SetLayout(o, l), o, ttwidget_layout, l, o->layout);
}
static ttbyte SetXl_ttwidget(ttwidget o, ttint xl) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttwidget))
	FIRE_EVENT(ret = o->Class->SetXl(o, xl), o, ttwidget_xl, xl, o->xl);
    return ret;
}
static ttbyte SetYl_ttwidget(ttwidget o, ttint yl) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttwidget))
	FIRE_EVENT(ret = o->Class->SetYl(o, yl), o, ttwidget_yl, yl, o->yl);
    return ret;
}
TT_INLINE ttbyte SetXlYl_ttwidget(ttwidget o, ttuint mask, ttint xl, ttint yl) {
    ttbyte ret = TT_TRUE;
    if (ret && (mask & 1))
	ret &= SetXl_ttwidget(o, xl);
    if (ret && (mask & 2))
	ret &= SetYl_ttwidget(o, yl);
    return ret;
}

static ttbyte SetWl_ttwidget(ttwidget o, ttint wl) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttwidget))
	FIRE_EVENT(ret = o->Class->SetWl(o, wl), o, ttwidget_wl, wl, o->wl);
    return ret;
}
static ttbyte SetHl_ttwidget(ttwidget o, ttint hl) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttwidget))
	FIRE_EVENT(ret = o->Class->SetHl(o, hl), o, ttwidget_hl, hl, o->hl);
    return ret;
}
TT_INLINE ttbyte SetWlHl_ttwidget(ttwidget o, ttuint mask, ttint wl, ttint hl) {
    ttbyte ret = TT_TRUE;
    if (ret && (mask & 1))
	ret &= SetWl_ttwidget(o, wl);
    if (ret && (mask & 2))
	ret &= SetHl_ttwidget(o, hl);
    return ret;
}

static ttbyte SetXY_ttwidget(ttwidget o, ttshort x, ttshort y) {
    ttbyte ret = TT_FALSE;
    
    if (CAN_SET(o, ttwidget)) {
	ttshort ox = o->x, oy = o->y;
	ttbyte fx = x != ox, fy = y != oy;
	
	if (fx || fy)
	    ret = o->Class->SetXY(o, x, y);
	else
	    ret = TT_TRUE;
	
	if (fx) FIRE_EVENT((void)0, o, ttwidget_x, x, ox);
	if (fy) FIRE_EVENT((void)0, o, ttwidget_y, y, oy);
    }
    return ret;
}
static ttbyte SetWH_ttwidget(ttwidget o, ttshort w, ttshort h) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttwidget)) {
	ttshort ow = o->w, oh = o->h;
	ttbyte fw = w != ow, fh = h != oh;
	
	if (fw || fh)
	    ret = o->Class->SetWH(o, w, h);
	else
	    ret = TT_TRUE;

	if (fw) FIRE_EVENT((void)0, o, ttwidget_w, w, ow);
	if (fh) FIRE_EVENT((void)0, o, ttwidget_h, h, oh);
    }
    return ret;
}
TT_INLINE ttbyte SetXYWH_ttwidget(ttwidget o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h) {
    ttbyte ret = TT_TRUE;
    if (ret && (mask & 0x3))
	ret &= SetXY_ttwidget(o, mask & 1 ? x : o->x, mask & 2 ? y : o->y);
    if (ret && (mask & 0xA))
	ret &= SetWH_ttwidget(o, mask & 4 ? w : o->w, mask & 8 ? h : o->h);
    return ret;
}
TT_INLINE ttbyte SetX_ttwidget(ttwidget o, ttshort x) {
    return SetXY_ttwidget(o, x, o->y);
}
TT_INLINE ttbyte SetY_ttwidget(ttwidget o, ttshort y) {
    return SetXY_ttwidget(o, o->x, y);
}
TT_INLINE ttbyte SetW_ttwidget(ttwidget o, ttshort w) {
    return SetWH_ttwidget(o, w, o->h);
}
TT_INLINE ttbyte SetH_ttwidget(ttwidget o, ttshort h) {
    return SetWH_ttwidget(o, o->w, h);
}

TT_INLINE ttbyte SetTooltip_ttwidget(ttwidget o, tttooltip t) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttwidget) && (!t || TTAssert(IS(tttooltip,t)))) {
	if (t != o->tooltip) {
	    FIRE_EVENT_O(ret = o->Class->SetTooltip(o, t), o, ttwidget_tooltip, t, o->tooltip);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}

/* ttlabel */
static ttbyte SetFontD_ttlabel(ttlabel o, ttopaque text_len, TT_ARG_DIE TT_ARG_ARRAY((_P(2))) ttfont *text) {

    FIRE_EVENT(o->Class->SetFontD(o, text_len, text), o, ttlabel_text, (opaque)text, (opaque)o->text);

    return TT_TRUE;
}
TT_INLINE ttbyte SetFont_ttlabel(ttlabel o, TT_CONST ttfont * text) {
    ttfont *_text = NULL;
    ttopaque text_len;
    return CAN_SET(o, ttlabel) &&
	(!text_len || (_text = TTCloneFontL(text, text_len = TTLenFont1(text)))) &&
	SetFontD_ttlabel(o, text_len, _text);
}
TT_INLINE ttbyte SetText_ttlabel(ttlabel o, TT_CONST ttbyte * text) {
    ttfont *_text = NULL;
    ttopaque text_len;
    return CAN_SET(o, ttlabel) &&
	(!text_len || (_text = TTCloneStrL2Font(text, text_len = TTLenStr1(text)))) &&
	SetFontD_ttlabel(o, text_len, _text);
}



/* tttooltip */
TT_INLINE ttbyte SetWidget_tttooltip(tttooltip o, ttwidget w) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, tttooltip) && (!w || TTAssert(IS(ttwidget,w)))) {
	if (w != o->widget)
	    FIRE_EVENT_O(ret = o->Class->SetWidget(o, w), o, tttooltip_widget, w, o->widget);
	else
	    ret = TT_TRUE;
    }
    return ret;
}


/* ttbuttongroup */
TT_INLINE void Add_ttbuttongroup(ttbuttongroup g, ttradiobutton o) {
    if (CAN_SET(g, ttbuttongroup) && o && TTAssert(IS(ttradiobutton,o)) &&
	!o->group) {
	
	FIRE_EVENT_O(g->Class->Add(g, o), o, ttradiobutton_group, g, 0);
	
	/* HACK warning: this is just to let clients bind listeners on g `group_first' field */
	FIRE_EVENT_O((void)0, g, ttbuttongroup_group_first, o, 0);
	
    }
}
TT_INLINE void Remove_ttbuttongroup(ttbuttongroup g, ttradiobutton o) {
    if (TTAssert(g && IS(ttbuttongroup,g)) && o && TTAssert(IS(ttradiobutton,o)) &&
	o->group == g) {
	
	FIRE_EVENT_O(g->Class->Remove(g, o), o, ttradiobutton_group, 0, g);
	
	/* HACK warning: this is just to let clients bind listeners on g `group_first' field */
	FIRE_EVENT_O((void)0, g, ttbuttongroup_group_first, 0, o);
    }
}
TT_INLINE void SetChecked_ttbuttongroup(ttbuttongroup g, ttradiobutton o) {
    if (TTAssert(g && IS(ttbuttongroup,g)) &&
	(!o || (TTAssert(IS(ttradiobutton,o)) && o->group == g)) &&
	o != g->checked) {
	
	FIRE_EVENT_O(g->Class->SetChecked(g, o), g, ttbuttongroup_checked, o, g->checked);
    }
}


/* ttanybutton */
static void SetAttrD_ttanybutton(ttanybutton o,
				   ttshort width, ttshort height, ttshort pitch,
				   TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text) {
    tteventbig e;

    if ((e = Create4_tteventbig(ttevent_evtype_change, ttanybutton_text, 0,
				pitch, 0, width, height))) {
	e->value = (opaque)text;
	e->old_value = (opaque)o->text;
	
	o->Class->SetAttrD(o, width, height, pitch, text);
	
	FireEvent((ttevent)e, (ttcomponent)o);
    } else {
	o->Class->SetAttrD(o, width, height, pitch, text);
    }
}
TT_INLINE ttbyte SetAttr_ttanybutton(ttanybutton o,
				     ttshort width, ttshort height, ttshort pitch,
				     TT_CONST ttattr * text) {
    ttattr *_text = NULL;
    ttshort i;

    if (CAN_SET(o, ttanybutton)) {
	if (!width || !height || (_text = TTAllocMem(width * height * sizeof(ttattr)))) {
	    for (i = height; i; i--)
		TTCopyMem(text, _text, width * sizeof(ttattr));
	    
	    SetAttrD_ttanybutton(o, width, height, width, _text);
	    return TT_TRUE;
	}
    }
    return TT_FALSE;
}
TT_INLINE ttbyte SetFont_ttanybutton(ttanybutton o,
				     ttshort width, ttshort height, ttshort pitch,
				     TT_CONST ttfont * text) {
    ttattr *_text = NULL;
    ttshort i;
    
    if (CAN_SET(o, ttanybutton)) {
	if (!width || !height || (_text = TTAllocMem(width * height * sizeof(ttattr)))) {
	    for (i = height; i; i--)
		TTCopyFontL2Attr(text, _text, width, o->col);

	    SetAttrD_ttanybutton(o, width, height, width, _text);
	    return TT_TRUE;
	}
    }
    return TT_FALSE;
}
TT_INLINE ttbyte SetText_ttanybutton(ttanybutton o,
				     ttshort width, ttshort height, ttshort pitch,
				     TT_CONST ttbyte * text) {
    ttattr *_text = NULL;
    ttshort i;
    
    if (CAN_SET(o, ttanybutton)) {
	if (!width || !height || (_text = TTAllocMem(width * height * sizeof(ttattr)))) {
	    for (i = height; i; i--)
		TTCopyStrL2Attr(text, _text, width, o->col);

	    SetAttrD_ttanybutton(o, width, height, width, _text);
	    return TT_TRUE;
	}
    }
    return TT_FALSE;
}


/* ttbutton */
TT_INLINE ttbyte IsPressed_ttbutton(ttbutton o) {
    if (CAN_SET(o, ttbutton))
	return !!(o->vflags & ttanybutton_vflags_pressed);
    return TT_FALSE;
}
TT_INLINE void SetPressed_ttbutton(ttbutton o, ttbyte pressed) {
    if (CAN_SET(o, ttbutton)) {
	ttany value, old_value;
	
	value = old_value = o->vflags;
	if (pressed != !!(value & ttanybutton_vflags_pressed))
	    value ^= ttanybutton_vflags_pressed;
	
	FIRE_EVENT(o->Class->SetPressed(o, pressed), o, ttvisible_vflags, value, old_value);
    }
}


/* ttcheckbutton */
TT_INLINE void SetChecked_ttcheckbutton(ttcheckbutton o, ttbyte checked) {
    if (CAN_SET(o, ttcheckbutton)) {
	ttany value, old_value;
	
	value = old_value = o->vflags;
	if (checked != !!(value & ttcheckbutton_vflags_checked))
	    value ^= ttcheckbutton_vflags_checked;
	
	FIRE_EVENT(o->Class->SetChecked(o, checked), o, ttvisible_vflags, value, old_value);
    }
}
TT_INLINE ttbyte IsChecked_ttcheckbutton(ttcheckbutton o) {
    if (TTAssert(o && IS(ttcheckbutton,o)))
	return !!(o->vflags & ttcheckbutton_vflags_checked);
    return TT_FALSE;
}


/* ttradiobutton */
TT_INLINE void AddToGroup_ttradiobutton(ttradiobutton o, ttbuttongroup g) {
    if (CAN_SET(o, ttradiobutton) && g && CAN_SET(g, ttbuttongroup)) {
	g->Class->Add(g, o);
    }
}
TT_INLINE void RemoveFromGroup_ttradiobutton(ttradiobutton o) {
    ttbuttongroup g;
    
    if (CAN_SET(o, ttradiobutton) && (g = o->group)) {
	g->Class->Remove(g, o);
    }
}


/* ttanyscroll */
TT_INLINE ttbyte SetOrientation_ttanyscroll(ttanyscroll o, ttbyte orientation) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttanyscroll)) {
	if (orientation != o->orientation) {
	    FIRE_EVENT(ret = o->Class->SetOrientation(o, orientation), o,
		       ttanyscroll_orientation, orientation, o->orientation);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}
TT_INLINE ttbyte SetSize_ttanyscroll(ttanyscroll o, ttint size) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttanyscroll)) {
	if ((size = minBOUND(size, 0)) != o->size) {
	    FIRE_EVENT(ret = o->Class->SetSize(o, size), o,
		       ttanyscroll_size, size, o->size);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}
TT_INLINE ttbyte SetRealSize_ttanyscroll(ttanyscroll o, ttint real_size) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttanyscroll)) {
	if ((real_size = minBOUND(real_size, 0)) != o->real_size) {
	    FIRE_EVENT(ret = o->Class->SetRealSize(o, real_size), o,
		       ttanyscroll_real_size, real_size, o->real_size);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}
TT_INLINE ttbyte SetViewSize_ttanyscroll(ttanyscroll o, ttint view_size) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttanyscroll)) {
	if ((view_size = BOUND(view_size, 0, o->size)) != o->view_size) {
	    FIRE_EVENT(ret = o->Class->SetViewSize(o, view_size), o,
		       ttanyscroll_view_size, view_size, o->view_size);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}
TT_INLINE ttbyte SetPosition_ttanyscroll(ttanyscroll o, ttint position) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttanyscroll)) {
	if ((position = BOUND(position, 0, o->size - o->view_size)) != o->position) {
	    FIRE_EVENT(ret = o->Class->SetPosition(o, position), o,
		       ttanyscroll_position, position, o->position);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}
#if 0
TT_INLINE ttbyte SetState_ttanyscroll(ttanyscroll o, ttuint state) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttanyscroll)) {
	if (state != o->state) {
	    FIRE_EVENT(ret = o->Class->SetState(o, state), o,
		       ttanyscroll_state, state, o->state);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}
#endif

/* ttscrollbar */


/* ttslider */
TT_INLINE ttbyte SetSlideMin_ttslider(ttslider o, ttint slide_min) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttslider)) {
	if (slide_min != o->slide_min) {
	    FIRE_EVENT(ret = o->Class->SetSlideMin(o, slide_min), o,
		       ttslider_slide_min, slide_min, o->slide_min);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}
TT_INLINE ttbyte SetSlideMax_ttslider(ttslider o, ttint slide_max) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttslider)) {
	if (slide_max != o->slide_max) {
	    FIRE_EVENT(ret = o->Class->SetSlideMax(o, slide_max), o,
		       ttslider_slide_max, slide_max, o->slide_max);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}
TT_INLINE ttbyte SetSlideValue_ttslider(ttslider o, ttint slide_value) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttslider)) {
	if ((slide_value = BOUND(slide_value, o->slide_min, o->slide_max)) != o->slide_value) {
	    FIRE_EVENT(ret = o->Class->SetSlideValue(o, slide_value), o,
		       ttslider_slide_value, slide_value, o->slide_value);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}


/* ttprogressbar */



/* ttscrollpane */
TT_INLINE ttbyte SetBarX_ttscrollpane(ttscrollpane o, ttscrollbar b) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttscrollpane) && (!b || TTAssert(IS(ttscrollbar,b)))) {
	if (b != o->bar_x)
	    FIRE_EVENT_O(ret = o->Class->SetBarX(o, b), o, ttscrollpane_bar_x, b, o->bar_x);
	else
	    ret = TT_TRUE;
    }
    return ret;
}
TT_INLINE ttbyte SetBarY_ttscrollpane(ttscrollpane o, ttscrollbar b) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttscrollpane) && (!b || TTAssert(IS(ttscrollbar,b)))) {
	if (b != o->bar_y)
	    FIRE_EVENT_O(ret = o->Class->SetBarY(o, b), o, ttscrollpane_bar_y, b, o->bar_y);
	else
	    ret = TT_TRUE;
    }
    return ret;
}


/* ttwindow */
TT_INLINE ttbyte SetTitle_ttwindow(ttwindow o, TT_ARG_READ ttbyte *title) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttwindow))
	FIRE_EVENT(ret = o->Class->SetTitle(o, title), o,
		   ttwindow_title, (opaque)title, (opaque)o->title);
    return ret;
}

/* ttframe */
TT_INLINE ttbyte SetMenubar_ttframe(ttframe o, ttmenubar m) {
    ttbyte ret = TT_FALSE;
    if (CAN_SET(o, ttframe) && (!m || TTAssert(IS(ttmenubar,m)))) {
	if (m != o->menubar) {
	    FIRE_EVENT_O(ret = o->Class->SetMenubar(o, m), o,
			 ttframe_menubar, m, o->menubar);
	} else
	    ret = TT_TRUE;
    }
    return ret;
}


/* ttscroller */


/* ttapplication */

    

#include "wrap_m4.h"



