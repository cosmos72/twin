
/*
 *  wrap.c  --  wrappers around exported libTT methods
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

/* ttobj */
TT_INLINE void Ref_ttobj(ttobj o) {
    if (o)
	o->refcount++;
}
TT_INLINE void Unref_ttobj(ttobj o) {
    if (o && !--o->refcount)
	Del(o);
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
static ttbyte IsMember_ttbitmask(ttbitmask o, ttany value) {
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
TT_INLINE ttbitmask Clone_ttbitmask(ttbitmask old) {
    ttbitmask o;
    ttopaque max, n;
    
    if (TTAssert(old && IS(ttbitmask,old))) {
	if ((o = TNEW(ttbitmask))) {
	    n = old->mask_n;
	    if ((max = old->mask_max)) {
		if ((o->mask = TTReallocMem(o->mask, max * TT_SIZEOF_ttany))) {
		    if (n)
			TTCopyMem(old->mask, o->mask, n * TT_SIZEOF_ttany);
		} else {
		    TDEL(o);
		    return (ttbitmask)0;
		}
	    }
	    o->mask_n = n;
	    o->mask_max = max;
	    o->mask0 = old->mask0;
	    o->user_data = old->user_data;
	    o->oflags = old->oflags & ttbitmask_oflags_mask_reverse;
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
static ttbyte AddA_ttbitmask(ttbitmask o, ttopaque n, TT_ARG_READ ttany *value) {
    ttopaque i;
    /* exactly one of Add_ttbitmask(), Remove_ttbitmask() cannot fail */
    if (TTAssert(o && IS(ttbitmask,o))) {
	if (n && value) {
	    for (i = 0; i < n; i++) {
		if (!Add_ttbitmask(o, value[i]))
		    break;
	    }
	    if (i == n)
		return TT_TRUE;
	} else
	    return TT_TRUE;
	
	/* failed. undo. */
	n = i;
	for (i = 0; i < n; i++)
	    Remove_ttbitmask(o, value[i]);
    }
    return TT_FALSE;
}
static ttbyte RemoveA_ttbitmask(ttbitmask o, ttopaque n, TT_ARG_READ ttany *value) {
    ttopaque i;
    /* exactly one of Add_ttbitmask(), Remove_ttbitmask() cannot fail */
    if (TTAssert(o && IS(ttbitmask,o))) {
	if (n && value) {
	    for (i = 0; i < n; i++) {
		if (!Remove_ttbitmask(o, value[i]))
		    break;
	    }
	    if (i == n)
		return TT_TRUE;
	} else
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
	(o1 = Clone_ttbitmask(o))) {
	
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
	    
	    if ((true1 = IsMember_ttbitmask(o1, value)))
		truth >>= 1;
	    if (!(o2->oflags & ttbitmask_oflags_mask_reverse))
		/* IsMember_ttbitmask(o2, value) */
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
		/* IsMember_ttbitmask(o1, value) */
		truth >>= 1;
	    if (IsMember_ttbitmask(o2, value))
		truth >>= 2;
	    if ((truth & 1) ^ reverse0)
		ok = real_Add_ttbitmask(o, value);
	}
	if (ok) {
	    ttany m0, m1, m2;
	    
	    m1 = reverse1 ? ~o1->mask0 : o1->mask0;
	    m2 = reverse2 ? ~o2->mask0 : o2->mask0;
	    m0 = Combine_ttany(m1, truth_table, m2, MINS(ttany));
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
static ttbyte CombineA_ttbitmask(ttbitmask o, ttopaque n, TT_ARG_READ ttany * value, ttuint truth_table) {
    ttbitmask o2;
    ttbyte ret = TT_FALSE;
    if (TTAssert(o && IS(ttbitmask,o)) &&
	(o2 = CreateA_ttbitmask(n, value))) {
	
	ret = CombineB_ttbitmask(o, o2, truth_table);
	TDEL(o2);
    }
    return ret;
}


/* tteventmask */
TT_INLINE void SetEvtypeMask_tteventmask(tteventmask o, TT_ARG_DIE ttbitmask em) {
    if (TTAssert(o && IS(tteventmask,o)) && em != o->evtype_mask && (!em || TTAssert(IS(ttbitmask,em)))) {
	if (o->evtype_mask)
	    TDEL(o->evtype_mask);
	o->evtype_mask = em;
    }
}
TT_INLINE void SetEvcodeMask_tteventmask(tteventmask o, TT_ARG_DIE ttbitmask em) {
    if (TTAssert(o && IS(tteventmask,o)) && em != o->evcode_mask && (!em || TTAssert(IS(ttbitmask,em)))) {
	if (o->evcode_mask)
	    TDEL(o->evcode_mask);
	o->evcode_mask = em;
    }
}
TT_INLINE void SetComponentMask_tteventmask(tteventmask o, TT_ARG_DIE ttbitmask cm) {
    if (TTAssert(o && IS(tteventmask,o)) && cm != o->component_mask && (!cm || TTAssert(IS(ttbitmask,cm)))) {
	if (o->component_mask)
	    TDEL(o->component_mask);
	o->component_mask = cm;
    }
}
TT_INLINE void SetTruthTable_tteventmask(tteventmask o, ttuint truth_table) {
    if (TTAssert(o && IS(tteventmask,o)) && truth_table != o->truth_table) {
	o->truth_table = truth_table;
    }
}

/* ttlistener */
TT_INLINE ttbyte SetArgs_ttlistener(ttlistener o, ttuint lflags, ttuint nargs, TT_CONST ttany * args) {
    if (TTAssert(o && IS(ttlistener,o))) {
	ttany *old_args = o->args;
	ttbyte ok = TRUE;
	
	if (args && (lflags & ttlistener_lflags_args_swallow))
	    o->args = (ttany *)args;
	else if ((o->args = (ttany *)TTAllocMem(nargs * sizeof(ttany)))) {
	    if (args)
		TTCopyMem(args, o->args, nargs * sizeof(ttany));
	    else
		TTWriteMem(o->args, '\0', nargs * sizeof(ttany));
	} else {
	    o->args = old_args;
	    ok = FALSE;
	}

	if (ok) {
	    o->nargs = nargs;
	    if (old_args)
		TTFreeMem(old_args);
	}
	return ok;
    }
    return FALSE;
}
TT_INLINE void SetEventMask_ttlistener(ttlistener o, TT_ARG_DIE tteventmask em) {
    if (TTAssert(o && IS(ttlistener,o)) && em != o->event_mask && (!em || TTAssert(IS(tteventmask,em)))) {
	if (o->event_mask)
	    TDEL(o->event_mask);
	o->event_mask = em;
    }
}


/* tttimer */
TT_INLINE void SetDelay_tttimer(tttimer o, ttuint delay_t, ttuint delay_f) {
    ttbyte enabled;
    
    if (TTAssert(o && IS(tttimer,o))) {
	GetNow();
	
	if ((enabled = !!(o->lflags & tttimer_lflags_enabled)))
	    Activate_tttimer(o, TT_FALSE);
	o->t = TNow + (time_t)delay_t;
	o->f = FNow + (frac_t)delay_f;
	if (enabled)
	    Activate_tttimer(o, TT_TRUE);
    }
}


/* ttdata */


/* ttcomponent */
TT_INLINE void AddListener_ttcomponent(ttcomponent o, ttlistener l) {
    if (TTAssert(o && IS(ttcomponent,o)) && l && TTAssert(IS(ttlistener,l)) && !l->component)
	l->FN->AddTo(l, o);
}
TT_INLINE void RemoveListener_ttcomponent(ttcomponent o, ttlistener l) {
    if (TTAssert(o && IS(ttcomponent,o)) && l && TTAssert(IS(ttlistener,l)) && l->component == o)
	l->FN->Remove(l);
}
TT_INLINE ttdata SetKeyData_ttcomponent(ttcomponent o, TT_CONST ttbyte *key, ttany data) {
    ttdata d;
    ttopaque key_len;
    
    if (TTAssert(o && IS(ttcomponent,o)) && key) {
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
TT_INLINE void AddTo_ttvisible(ttvisible o, ttvisible parent) {
    if (TTAssert(o && IS(ttvisible,o)) && parent && TTAssert(IS(ttvisible,parent)) &&
	!o->parent) {
	
	FIRE_EVENT(o->FN->AddTo(o, parent), o, ttvisible_parent, (opaque)parent, 0);
	
	/* HACK warning: this is just to let clients bind listeners on parent `child_first' field */
	FIRE_EVENT((void)0, parent, ttvisible_child_first, (opaque)o, 0);
    }
}
TT_INLINE void Remove_ttvisible(ttvisible o) {
    ttvisible parent;
    if (TTAssert(o && IS(ttvisible,o)) && (parent = o->parent)) {

	FIRE_EVENT(o->FN->Remove(o), o, ttvisible_parent, 0, (opaque)parent);
	
	/* HACK warning: this is just to let clients bind listeners on parent `child_first' field */
	FIRE_EVENT((void)0, parent, ttvisible_child_first, 0, (opaque)o);
    }
}
TT_INLINE void SetVisible_ttvisible(ttvisible o, ttbyte on_off) {
    if (TTAssert(o && IS(ttvisible,o))) {
	ttany value, old_value;
	
	value = old_value = o->vflags;
	if (on_off != !!(value & ttvisible_vflags_visible))
	    value ^= ttvisible_vflags_visible;
	
	FIRE_EVENT(o->FN->SetVisible(o, on_off), o, ttvisible_vflags, value, old_value);
    }
}
TT_INLINE void SetTheme_ttvisible(ttvisible o, tttheme theme) {
    if (TTAssert(o && IS(ttvisible,o)))
	FIRE_EVENT(o->FN->SetTheme(o, theme), o, ttvisible_theme, (opaque)theme, (opaque)o->theme);
}
TT_INLINE void Add_ttvisible(ttvisible o, ttvisible child) {
    if (TTAssert(o && IS(ttvisible,o)) && child && TTAssert(IS(ttvisible,child)))
	AddTo_ttvisible(child, o);
}
TT_INLINE void SetRepaint_ttvisible(ttvisible o, ttvisible_repaint_fn repaint) {
    if (TTAssert(o && IS(ttvisible,o)) && o->repaint != repaint) {
	
	FIRE_EVENT(o->repaint = repaint, o, ttvisible_repaint, (opaque)repaint, (opaque)o->repaint);
	
	if (o->vflags & ttvisible_vflags_visible)
	    Expose_ttvisible(o, ttvisible_repaint_args_WHOLE);
    }
}


/* ttnative */



/* ttwidget */
static void SetXl_ttwidget(ttwidget o, ttint xl) {
    if (TTAssert(o && IS(ttwidget,o)))
	FIRE_EVENT(o->FN->SetXl(o, xl), o, ttwidget_xl, xl, o->xl);
}
static void SetYl_ttwidget(ttwidget o, ttint yl) {
    if (TTAssert(o && IS(ttwidget,o)))
	FIRE_EVENT(o->FN->SetYl(o, yl), o, ttwidget_yl, yl, o->yl);
}
TT_INLINE void SetXlYl_ttwidget(ttwidget o, ttuint mask, ttint xl, ttint yl) {
    if (mask & 1)
	SetXl_ttwidget(o, xl);
    if (mask & 2)
	SetYl_ttwidget(o, yl);
}

static void SetWl_ttwidget(ttwidget o, ttint wl) {
    if (TTAssert(o && IS(ttwidget,o)))
	FIRE_EVENT(o->FN->SetWl(o, wl), o, ttwidget_wl, wl, o->wl);
}
static void SetHl_ttwidget(ttwidget o, ttint hl) {
    if (TTAssert(o && IS(ttwidget,o)))
	FIRE_EVENT(o->FN->SetHl(o, hl), o, ttwidget_hl, hl, o->hl);
}
TT_INLINE void SetWlHl_ttwidget(ttwidget o, ttuint mask, ttint wl, ttint hl) {
    if (mask & 1)
	SetWl_ttwidget(o, wl);
    if (mask & 2)
	SetHl_ttwidget(o, hl);
}

static void SetXY_ttwidget(ttwidget o, ttshort x, ttshort y) {
    if (TTAssert(o && IS(ttwidget,o))) {
	ttshort ox = o->x, oy = o->y;
	ttbyte fx = x != ox, fy = y != oy;
	
	if (fx || fy) o->FN->SetXY(o, x, y);
	if (fx) FIRE_EVENT((void)0, o, ttwidget_x, x, ox);
	if (fy) FIRE_EVENT((void)0, o, ttwidget_y, y, oy);
    }
}
static void SetWH_ttwidget(ttwidget o, ttshort w, ttshort h) {
    if (TTAssert(o && IS(ttwidget,o))) {
	ttshort ow = o->w, oh = o->h;
	ttbyte fw = w != ow, fh = h != oh;
	
	if (fw || fh) o->FN->SetWH(o, w, h);
	if (fw) FIRE_EVENT((void)0, o, ttwidget_w, w, ow);
	if (fh) FIRE_EVENT((void)0, o, ttwidget_h, h, oh);
    }
}
TT_INLINE void SetXYWH_ttwidget(ttwidget o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h) {
    if (mask & 0x3)
	SetXY_ttwidget(o, mask & 1 ? x : o->x, mask & 2 ? y : o->y);
    if (mask & 0xA)
	SetWH_ttwidget(o, mask & 4 ? w : o->w, mask & 8 ? h : o->h);
}
TT_INLINE void SetX_ttwidget(ttwidget o, ttshort x) {
    SetXY_ttwidget(o, x, o->y);
}
TT_INLINE void SetY_ttwidget(ttwidget o, ttshort y) {
    SetXY_ttwidget(o, o->x, y);
}
TT_INLINE void SetW_ttwidget(ttwidget o, ttshort w) {
    SetWH_ttwidget(o, w, o->h);
}
TT_INLINE void SetH_ttwidget(ttwidget o, ttshort h) {
    SetWH_ttwidget(o, o->w, h);
}

TT_INLINE void SetTooltip_ttwidget(ttwidget o, tttooltip t) {
    if (t != o->tooltip) {
	FIRE_EVENT(o->FN->SetTooltip(o, t), o, ttwidget_tooltip, t, o->tooltip);
    }
}

/* ttlabel */
static ttbyte DoSetFont_ttlabel(ttlabel o, ttfont *text, ttopaque text_len) {
    ttbyte ret;
    FIRE_EVENT(ret = o->FN->DoSetFont(o, text, text_len), o, ttlabel_text, (ttany)(opaque)text, (ttany)(opaque)o->text);
    if (text && !ret)
	TTFreeMem(text);
    return ret;
}
TT_INLINE ttbyte SetFont_ttlabel(ttlabel o, TT_CONST ttfont * text) {
    ttfont *_text = NULL;
    ttopaque text_len;
    return TTAssert(o && IS(ttlabel,o)) &&
	(!text_len || (_text = TTCloneFontL(text, text_len = TTLenFont1(text)))) &&
	DoSetFont_ttlabel(o, _text, text_len);
}
TT_INLINE ttbyte SetText_ttlabel(ttlabel o, TT_CONST ttbyte * text) {
    ttfont *_text = NULL;
    ttopaque text_len;
    return TTAssert(o && IS(ttlabel,o)) &&
	(!text_len || (_text = TTCloneStrL2Font(text, text_len = TTLenStr1(text)))) &&
	DoSetFont_ttlabel(o, _text, text_len);
}



/* tttooltip */
TT_INLINE void SetWidget_tttooltip(tttooltip o, ttwidget w) {
    if (TTAssert(o && IS(tttooltip, o)) && (!w || TTAssert(IS(ttwidget,w))) && w != o->widget)
	FIRE_EVENT(o->FN->SetWidget(o, w), o,
		   tttooltip_widget, w, o->widget);
}


/* ttbuttongroup */
TT_INLINE void Add_ttbuttongroup(ttbuttongroup g, ttradiobutton o) {
    if (TTAssert(g && IS(ttbuttongroup,g)) && o && TTAssert(IS(ttradiobutton,o)) &&
	!o->group) {
	
	FIRE_EVENT(g->FN->Add(g, o), o, ttradiobutton_group, (opaque)g, 0);
	
	/* HACK warning: this is just to let clients bind listeners on g `group_first' field */
	FIRE_EVENT((void)0, g, ttbuttongroup_group_first, (opaque)o, 0);
	
    }
}
TT_INLINE void Remove_ttbuttongroup(ttbuttongroup g, ttradiobutton o) {
    if (TTAssert(g && IS(ttbuttongroup,g)) && o && TTAssert(IS(ttradiobutton,o)) &&
	o->group == g) {
	
	FIRE_EVENT(g->FN->Remove(g, o), o, ttradiobutton_group, 0, (opaque)g);
	
	/* HACK warning: this is just to let clients bind listeners on g `group_first' field */
	FIRE_EVENT((void)0, g, ttbuttongroup_group_first, 0, (opaque)o);
    }
}
TT_INLINE void SetChecked_ttbuttongroup(ttbuttongroup g, ttradiobutton o) {
    if (TTAssert(g && IS(ttbuttongroup,g)) &&
	(!o || (TTAssert(IS(ttradiobutton,o)) && o->group == g)) &&
	o != g->checked) {
	
	FIRE_EVENT(g->FN->SetChecked(g, o), g, ttbuttongroup_checked, o, g->checked);
    }
}


/* ttanybutton */
static ttbyte DoSetAttr_ttanybutton(ttanybutton o, ttattr * text,
				    ttshort width, ttshort height, ttshort pitch) {
    tteventbig e;
    ttbyte ret = FALSE;

    if ((e = Create4_tteventbig(ttevent_evtype_change, ttanybutton_text, 0,
				pitch, 0, width, height))) {
	e->value = (ttany)(opaque)text;
	e->old_value = (ttany)(opaque)o->text;
	
	ret = o->FN->DoSetAttr(o, text, width, height, pitch);
	
	FireEvent((ttevent)e, (ttcomponent)o);
    }
    if (text && !ret)
	TTFreeMem(text);
    return ret;
}
TT_INLINE ttbyte SetAttr_ttanybutton(ttanybutton o, TT_CONST ttattr * text,
				     ttshort width, ttshort height, ttshort pitch) {
    ttattr *_text = NULL;
    ttshort i;
    
    if (TTAssert(o && IS(ttanybutton,o))) {
	if (!width || !height || (_text = TTAllocMem(width * height * sizeof(ttattr)))) {
	    for (i = height; i; i--)
		TTCopyMem(text, _text, width * sizeof(ttattr));
	    
	    return DoSetAttr_ttanybutton(o, _text, width, height, width);
	}
    }
    return FALSE;
}
TT_INLINE ttbyte SetFont_ttanybutton(ttanybutton o, TT_CONST ttfont * text,
				     ttshort width, ttshort height, ttshort pitch) {
    ttattr *_text = NULL;
    ttshort i;
    
    if (TTAssert(o && IS(ttanybutton,o))) {
	if (!width || !height || (_text = TTAllocMem(width * height * sizeof(ttattr)))) {
	    for (i = height; i; i--)
		TTCopyFontL2Attr(text, _text, width, o->col);

	    return DoSetAttr_ttanybutton(o, _text, width, height, width);
	}
    }
    return FALSE;
}
TT_INLINE ttbyte SetText_ttanybutton(ttanybutton o, TT_CONST ttbyte * text,
				     ttshort width, ttshort height, ttshort pitch) {
    ttattr *_text = NULL;
    ttshort i;
    
    if (TTAssert(o && IS(ttanybutton,o))) {
	if (!width || !height || (_text = TTAllocMem(width * height * sizeof(ttattr)))) {
	    for (i = height; i; i--)
		TTCopyStrL2Attr(text, _text, width, o->col);

	    return DoSetAttr_ttanybutton(o, _text, width, height, width);
	}
    }
    return FALSE;
}


/* ttbutton */
TT_INLINE ttbyte IsPressed_ttbutton(ttbutton o) {
    if (TTAssert(o && IS(ttbutton,o)))
	return !!(o->vflags & ttanybutton_vflags_pressed);
    return FALSE;
}
TT_INLINE ttbyte SetPressed_ttbutton(ttbutton o, ttbyte pressed) {
    if (TTAssert(o && IS(ttbutton,o))) {
	ttany value, old_value;
	
	value = old_value = o->vflags;
	if (pressed != !!(value & ttanybutton_vflags_pressed))
	    value ^= ttanybutton_vflags_pressed;
	
	FIRE_EVENT(o->FN->SetPressed(o, pressed), o, ttvisible_vflags, value, old_value);
    }
    return FALSE;
}


/* ttcheckbutton */
TT_INLINE void SetChecked_ttcheckbutton(ttcheckbutton o, ttbyte checked) {
    if (TTAssert(o && IS(ttvisible,o))) {
	ttany value, old_value;
	
	value = old_value = o->vflags;
	if (checked != !!(value & ttcheckbutton_vflags_checked))
	    value ^= ttcheckbutton_vflags_checked;
	
	FIRE_EVENT(o->FN->SetChecked(o, checked), o, ttvisible_vflags, value, old_value);
    }
}
TT_INLINE ttbyte IsChecked_ttcheckbutton(ttcheckbutton o) {
    if (TTAssert(o && IS(ttbutton,o)))
	return !!(o->vflags & ttcheckbutton_vflags_checked);
    return FALSE;
}


/* ttradiobutton */
TT_INLINE void AddToGroup_ttradiobutton(ttradiobutton o, ttbuttongroup g) {
    if (TTAssert(o && IS(ttradiobutton, o)) && g && TTAssert(IS(ttbuttongroup, g))) {
	g->FN->Add(g, o);
    }
}
TT_INLINE void RemoveFromGroup_ttradiobutton(ttradiobutton o) {
    ttbuttongroup g;
    
    if (TTAssert(o && IS(ttradiobutton, o)) && (g = o->group)) {
	g->FN->Remove(g, o);
    }
}


/* ttanyscroll */
TT_INLINE void SetOrientation_ttanyscroll(ttanyscroll o, ttbyte orientation) {
    if (TTAssert(o && IS(ttanyscroll, o)) &&
	orientation != o->orientation)
	
	FIRE_EVENT(o->FN->SetOrientation(o, orientation), o,
		   ttanyscroll_orientation, orientation, o->orientation);
}
TT_INLINE void SetSize_ttanyscroll(ttanyscroll o, ttint size) {
    if (TTAssert(o && IS(ttanyscroll, o)) &&
	(size = minBOUND(size, 0)) != o->size)
	
	FIRE_EVENT(o->FN->SetSize(o, size), o,
		   ttanyscroll_size, size, o->size);
}
TT_INLINE void SetRealSize_ttanyscroll(ttanyscroll o, ttint real_size) {
    if (TTAssert(o && IS(ttanyscroll, o)) &&
	(real_size = minBOUND(real_size, 0)) != o->real_size)
	
	FIRE_EVENT(o->FN->SetRealSize(o, real_size), o,
		   ttanyscroll_real_size, real_size, o->real_size);
}
TT_INLINE void SetViewSize_ttanyscroll(ttanyscroll o, ttint view_size) {
    if (TTAssert(o && IS(ttanyscroll, o)) &&
	(view_size = BOUND(view_size, 0, o->size)) != o->view_size)

	FIRE_EVENT(o->FN->SetViewSize(o, view_size), o,
		   ttanyscroll_view_size, view_size, o->view_size);
}
TT_INLINE void SetPosition_ttanyscroll(ttanyscroll o, ttint position) {
    if (TTAssert(o && IS(ttanyscroll, o)) &&
	(position = BOUND(position, 0, o->size - o->view_size)) != o->position)
	
	FIRE_EVENT(o->FN->SetPosition(o, position), o,
		   ttanyscroll_position, position, o->position);
}
#if 0
TT_INLINE void SetState_ttanyscroll(ttanyscroll o, ttuint state) {
    if (TTAssert(o && IS(ttanyscroll, o)) &&
	state != o->state)
	
	FIRE_EVENT(o->FN->SetState(o, state), o,
		   ttanyscroll_state, state, o->state);
}
#endif

/* ttscrollbar */


/* ttslider */
TT_INLINE void SetSlideMin_ttslider(ttslider o, ttint slide_min) {
    if (TTAssert(o && IS(ttslider, o)) &&
	slide_min != o->slide_min)
	
	FIRE_EVENT(o->FN->SetSlideMin(o, slide_min), o,
		   ttslider_slide_min, slide_min, o->slide_min);
}
TT_INLINE void SetSlideMax_ttslider(ttslider o, ttint slide_max) {
    if (TTAssert(o && IS(ttslider, o)) &&
	slide_max != o->slide_max)
	
	FIRE_EVENT(o->FN->SetSlideMax(o, slide_max), o,
		   ttslider_slide_max, slide_max, o->slide_max);
}
TT_INLINE void SetSlideValue_ttslider(ttslider o, ttint slide_value) {
    if (TTAssert(o && IS(ttslider, o)) &&
	(slide_value = BOUND(slide_value, o->slide_min, o->slide_max)) != o->slide_value)
	
	FIRE_EVENT(o->FN->SetSlideValue(o, slide_value), o,
		   ttslider_slide_value, slide_value, o->slide_value);
}


/* ttprogressbar */



/* ttscrollpane */
TT_INLINE void SetBarX_ttscrollpane(ttscrollpane o, ttscrollbar b) {
    if (TTAssert(o && IS(ttscrollpane, o)) && (!b || TTAssert(IS(ttscrollbar,b)))) {
	FIRE_EVENT(o->FN->SetBarX(o, b), o, ttscrollpane_bar_x, b, o->bar_x);
    }
}
TT_INLINE void SetBarY_ttscrollpane(ttscrollpane o, ttscrollbar b) {
    if (TTAssert(o && IS(ttscrollpane, o)) && (!b || TTAssert(IS(ttscrollbar,b)))) {
	FIRE_EVENT(o->FN->SetBarY(o, b), o, ttscrollpane_bar_y, b, o->bar_y);
    }
}


/* ttwindow */
TT_INLINE ttbyte SetTitle_ttwindow(ttwindow o, TT_ARG_READ ttbyte *title) {
    ttbyte ret = TT_FALSE;
    if (TTAssert(o && IS(ttwindow, o)))
	FIRE_EVENT(ret = o->FN->SetTitle(o, title), o,
		   ttwindow_title, title, o->title);
    return ret;
}

/* ttframe */
TT_INLINE void SetMenubar_ttframe(ttframe o, ttmenubar m) {
    if (TTAssert(o && IS(ttwindow, o)) && (!m || TTAssert(IS(ttmenubar,m))))
	FIRE_EVENT(o->FN->SetMenubar(o, m), o,
		   ttframe_menubar, m, o->menubar);
}


/* ttscroller */


/* ttapplication */
TT_INLINE ttapplication Set_ttapplication(TT_CONST ttbyte * name) {
    ttbyte *apname;
    
    if (!name)
	return (ttapplication)0;
    
    if (TTD.Application || (TTD.Application = TFN_ttapplication->New(TFN_ttapplication, NULL))) {

	if ((apname = TTCloneStr(name))) {
	    if (TTD.Application->name)
		TTFreeMem(TTD.Application->name);
	    TTD.Application->name = apname;

	    return (ttapplication)Build((ttobj)TTD.Application);
	}
	TDEL(TTD.Application);
    }
    return TTD.Application;
}

    

#include "wrap_m4.h"



