




/* This file was automatically generated from null_c by m4/hw.m4h, do not edit! */


/*
 *  null_m4.c  --  implementation of libTT methods on "null" target display
 *
 *  Copyright (C) 2001,2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



























/*
 *                 WARNING!
 * 
 * this file is null_c and is preprocessed by m4 to produce null_m4.c
 * 
 * It must be a valid m4 file, and must produce a valid C include file.
 * 
 */


/*
 * We need to write down only overloaded methods.
 */
 


/* order is crucial here! */

#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

/* include our internal copy of TT.h instead of the public one */
#include "TT.h"
#include <TT/TTerrno.h>

#include "mutex.h"

#include "TTextern.h"
#include "TTassert.h"

#include "utils.h"
#include "inlines.h"
#include "seterrno.h"
#include "theme.h"
#include "common_m4.h"

static s_ttclasses null_TTClasses;




#define REVALIDATE(call) do { \
    o->Class->Invalidate(o); \
    call; \
    if (o->parent && (o->vflags & ttvisible_vflags_visible)) \
	Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE); \
} while (0)

    
/* ttobj */

/* ttobj has no methods. nothing to do */


/* ttobject */
static ttobject null_New_ttobject(ttclass_ttobject Class, ttobject o) {
    /*
     * if argument (o) is non-NULL, use it instead of allocating memory.
     * in this case, set o->oflags to ttobject_oflags_static to avoid
     * deallocating it later with TTFreeMem().
     */
    ttuint oflags = (ttopaque)o ? ttobject_oflags_static : 0;
    
    if ((o || (o = (ttobject)TTAllocMem(Class->size)))) {
	if (AssignId((ttclass)Class, (ttobj)o)) {
	    o->Class = Class;
	    o->refcount = ttobject_refcount_alive;
	    o->oflags = oflags;
	    o->native = o->target_private = o->user_data = (ttany)0;
	    o->events_inprogress = o->events_informed = (ttopaque)0;
	    return o;
	}
	if (!(oflags & ttobject_oflags_static))
	    TTFreeMem(o);
    }
    return (ttobject)0;
}
static ttobject null_Build_ttobject(ttobject o) {
    /*
     * this is necessary for all derived classes that do not overload Class->Build.
     * It is not actually useful for ttobject.
     */
    return o;
}
static void null_Break_ttobject(ttobject o) {
    /*
     * this is necessary for all derived classes that do not overload Class->Break.
     * It is not actually useful for ttobject.
     */
}
static void null_Del_ttobject(ttobject o) {
    if (o) {
	o->Class->Break(o);
	
	/* ttobject_refcount_alive flag should already be removed */
	if (!o->refcount) {
	    
	    /* drop Id as late as possible (for listeners) */
	    DropId((ttobj)o);
	    
	    if (!(o->oflags & ttobject_oflags_static))
		TTFreeMem(o);
	} else {
	    /*
	     * WARNING: we did not delete o, let's clear
	     * o->events_inprogress so that we can delete it later
	     */
	    o->events_inprogress &= ~ttmask(ttevent_evtype_del);
	}
    }
}


/* ttvector */
static ttvector null_New_ttvector(ttclass_ttvector Class, ttvector o) {
    if ((o = (ttvector)TClass(ttobject)->New((ttclass_ttobject)Class, (ttobject)o))) {
	o->array_size = o->array_n = (ttopaque)0;
	o->array = (ttany *)0;
    }
    return o;
}
static void null_Del_ttvector(ttvector o) {
    if (o) {
	if (o->array) {
	    TTFreeMem(o->array);
	    o->array = (ttany *)0;
	}
	TClass(ttobject)->Del((ttobject)o);
    }
}

static ttbyte null_Grow_ttvector(ttvector o, ttopaque delta_n) {
    ttopaque new_size = (o->array_n + delta_n) * 3 / 2;
    ttany * new_array;

    if (new_size < 6)
	new_size = 6;
    if (!(new_array = TTReallocMem(o->array, new_size * sizeof(ttany)))) {
	new_size = o->array_n + delta_n;
	if (!(new_array = TTReallocMem(o->array, new_size * sizeof(ttany))))
	    return TT_FALSE;
    }
    o->array = new_array;
    o->array_size = new_size;
    return TT_TRUE;
}

ttany null_GetAt_ttvector(ttvector o, ttopaque pos) {
    if (TTAssert(o && IS(ttvector,o)) && pos < o->array_n)
	return o->array[pos];
    return (ttany)0;
}
ttbyte null_SetAt_ttvector(ttvector o, ttopaque pos, ttany value) {
    if (TTAssert(o && IS(ttvector,o)) && pos < o->array_n) {
	o->array[pos] = value;
	return TT_TRUE;
    }
    return TT_FALSE;
}

ttbyte null_AddY_ttvector(ttvector o, ttopaque pos, ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY(_P(2)) ttany *values) {
    
    if (!value_n)
	return TT_TRUE;
    
    if (values && pos <= o->array_n &&
	(o->array_size >= o->array_n + value_n || null_Grow_ttvector(o, value_n))) {

	if (pos < o->array_n)
	    TTMoveMem(o->array + pos, o->array + pos + value_n, (o->array_n - pos) * sizeof(ttany));
	
	TTCopyMem(values, o->array + pos, value_n * sizeof(ttany));
	o->array_n += value_n;
    
	return TT_TRUE;
    }
    return TT_FALSE;
}
ttopaque null_ContainsValue_ttvector(ttvector o, ttopaque pos_start, ttany value) {
    while (pos_start < o->array_n) {
	if (o->array[pos_start] == value)
	    return pos_start;
	pos_start++;
    }
    return (ttopaque)-1;
}
		     
ttbyte null_RemoveRange_ttvector(ttvector o, ttopaque pos, ttopaque pos_n) {
    if (pos + pos_n <= o->array_n) {
	if (pos + pos_n < o->array_n)
	    TTMoveMem(o->array + pos + pos_n, o->array + pos, (o->array_n - pos - pos_n) * sizeof(ttany));
	o->array_n -= pos_n;
    }
    return TT_TRUE;
}
    
/* ttevent */
static ttevent null_New_ttevent(ttclass_ttevent Class, ttevent o) {
    if ((o = (ttevent)TClass(ttobject)->New((ttclass_ttobject)Class, (ttobject)o))) {
	o->component = (ttcomponent)0;
	o->evtype = o->evcode = o->evflags = 0;
	o->prev = o->next = (ttevent)0;
    }
    return o;
}


/* tteventbig */
static tteventbig null_New_tteventbig(ttclass_tteventbig Class, tteventbig o) {
    if ((o = (tteventbig)TClass(ttevent)->New((ttclass_ttevent)Class, (ttevent)o))) {
	o->x = o->y = o->w = o->h = (ttshort)0;
	o->value = o->old_value = (ttany)0;
	o->data = (TT_CONST ttbyte *)0;
	o->data_len = (ttopaque)0;
    }
    return o;
}


/* ttbitmask */
static ttbitmask null_New_ttbitmask(ttclass_ttbitmask Class, ttbitmask o) {
    if ((o = (ttbitmask)TClass(ttobject)->New((ttclass_ttobject)Class, (ttobject)o))) {
	o->mask0 = o->mask_max = o->mask_n = (opaque)0;
	o->mask = (ttany *)0;
    }
    return o;
}

/* tteventmask */
static tteventmask null_New_tteventmask(ttclass_tteventmask Class, tteventmask o) {
    if ((o = (tteventmask)TClass(ttobject)->New((ttclass_ttobject)Class, (ttobject)o))) {
	o->truth_table = 0x80; /* logical AND */
	o->evtype_mask = o->evcode_mask = o->component_mask = (ttbitmask)0;
    }
    return o;
}
static void null_Del_tteventmask(tteventmask o) {
    if (o) {
	if (TTD.InstalledEM == o)
	    TTD.InstalledEM = (tteventmask)0;
	if (TTD.DefaultEM == o)
	    TTD.DefaultEM = (tteventmask)0;

	if (o->evtype_mask) {
	    TDEL(o->evtype_mask);
	    o->evtype_mask = (ttbitmask)0;
	}
	if (o->evcode_mask) {
	    TDEL(o->evcode_mask);
	    o->evcode_mask = (ttbitmask)0;
	}
	if (o->component_mask) {
	    TDEL(o->component_mask);
	    o->component_mask = (ttbitmask)0;
	}
	TClass(ttobject)->Del((ttobject)o);
    }
}

/* ttcallback */
static ttcallback null_New_ttcallback(ttclass_ttcallback Class, ttcallback o) {
    if ((o = (ttcallback)TClass(ttobject)->New((ttclass_ttobject)Class, (ttobject)o))) {
	o->lflags = 0;
	o->component = (ttcomponent)0;
	o->prev = o->next = (ttcallback)0;
    }
    return o;
}
static void null_Del_ttcallback(ttcallback o) {
    if (o) {
	if (o->next)
	    o->next->prev = o->prev;
	if (o->prev)
	    o->prev->next = o->next;
	else if (o->component && o->component->callbacks == o)
	    o->component->callbacks = o->next;

	o->prev = o->next = (ttcallback)0;
	
	THW.DeleteCallback(o);

	TClass(ttobject)->Del((ttobject)o);
    }
}

static ttcallback null_Create_ttcallback(ttcomponent c) {
    ttcallback o;
    if ((o = TNEW(ttcallback))) {
	if ((o->next = c->callbacks))
	    o->next->prev = o;
	o->component = c;
	c->callbacks = o;
    }
    return o;
}

static TT_CONST struct s_tavl empty_AVL;

/* ttlistener */
static ttlistener null_New_ttlistener(ttclass_ttlistener Class, ttlistener o) {
    if ((o = (ttlistener)TClass(ttcallback)->New((ttclass_ttcallback)Class, (ttcallback)o))) {
	o->AVL = empty_AVL;
	o->prev = o->next = (ttlistener)0;
	o->event = (ttevent)0;
	o->function = (ttany)0;
	o->arg_component_n = o->arg_event_n = (ttuint)-1;
	o->args = (ttvector)0;
	o->event_mask = (tteventmask)0;
	o->inprogress_prev = o->inprogress_next = (ttlistener)0;
    }
    return o;
}
static void null_Del_ttlistener(ttlistener o) {
    if (o) {
	o->Class->Remove(o);
	if (o->event) {
	    TDEL(o->event);
	    o->event = (ttevent)0;
	}
	if (o->args) {
	    TDEL(o->args);
	    o->args = (ttvector)0;
	}
	if (o->event_mask) {
	    TDEL(o->event_mask);
	    o->event_mask = (tteventmask)0;
	}
	TClass(ttcallback)->Del((ttcallback)o);
    }
}

static void null_AddTo_ttlistener(ttlistener c, ttcomponent o) {
    /* implemented in libTT.c */
    AddTo_ttlistener(c, o);
}

static void null_Remove_ttlistener(ttlistener o) {
    /* implemented in libTT.c */
    Remove_ttlistener(o);
}


/* tttimer */
static tttimer null_New_tttimer(ttclass_tttimer Class, tttimer o) {
    if ((o = (tttimer)TClass(ttlistener)->New((ttclass_ttlistener)Class, (ttlistener)o))) {
	o->delay_t = o->delay_f = (ttany)0;
	o->timer_prev = o->timer_next = (tttimer)0;
    }
    return o;
}
static void null_Del_tttimer(tttimer o) {
    if (o) {
	o->Class->Remove(o);
	TClass(ttlistener)->Del((ttlistener)o);
    }
}
static tttimer null_Build_tttimer(tttimer o) {
    if (o->lflags & tttimer_lflags_enabled)
	Activate_tttimer(o, TT_TRUE);
    return o;
}
static void null_Break_tttimer(tttimer o) {
    if (o->lflags & tttimer_lflags_enabled)
	Activate_tttimer(o, TT_FALSE);
}
static void null_SetEnabled_tttimer(tttimer o, ttbyte enable) {
    if (!enable != !(o->lflags & tttimer_lflags_enabled)) {
	o->lflags ^= tttimer_lflags_enabled;
	Activate_tttimer(o, enable);
    }
}
static ttbyte null_IsEnabled_tttimer(tttimer o) {
    return !!(o->lflags & tttimer_lflags_enabled);
}
static void null_AddTo_tttimer(tttimer o, ttcomponent c) {
    /* prepend */
    if (c && !o->component) {
	if ((o->next = c->timers))
	    c->timers->prev = o;
	c->timers = o;
	o->prev = NULL;
	o->component = c;
    }
}
static void null_Remove_tttimer(tttimer o) {
    ttcomponent c;
    if ((c = o->component)) {
	if (o->prev)
	    o->prev->next = o->next;
	else
	    c->timers = o->next;
	if (o->next)
	    o->next->prev = o->prev;
	o->component = NULL;
    }
}


/* ttdata */
static ttdata null_New_ttdata(ttclass_ttdata Class, ttdata o) {
    if ((o = (ttdata)TClass(ttcomponent)->New((ttclass_ttcomponent)Class, (ttcomponent)o))) {
	o->AVL = empty_AVL;
	o->component = (ttcomponent)0;
	o->key_len = (ttopaque)0;
	o->key = NULL;
	o->data = (ttany)0;
    }
    return o;
}
static void null_Del_ttdata(ttdata o) {
    if (o) {
	o->Class->Remove(o);
	TClass(ttcomponent)->Del((ttcomponent)o);
    }
}
static void null_AddTo_ttdata(ttdata o, ttcomponent c) {
    /* implemented in libTT.c */
    AddTo_ttdata(o, c, TT_FALSE);
}
static void null_Remove_ttdata(ttdata o) {
    /* implemented in libTT.c */
    Remove_ttdata(o);
}


/* ttcomponent */
static ttcomponent null_New_ttcomponent(ttclass_ttcomponent Class, ttcomponent o) {
    if ((o = (ttcomponent)TClass(ttobject)->New((ttclass_ttobject)Class, (ttobject)o))) {
	o->callbacks = (ttcallback)0;
	o->listeners = (ttlistener)0;
	o->timers = (tttimer)0;
	o->datas = (ttdata)0;
    }
    return o;
}
static void null_Del_ttcomponent(ttcomponent o) {
    if (o) {
	if (!(o->events_informed & ttmask(ttevent_evtype_del))) {
	    /* be sure we fire delete event only ONCE! */
	    o->events_informed |= ttmask(ttevent_evtype_del);
	    FireSimpleEvent(o, ttevent_evtype_del);
	}
	
	if (!o->refcount)
	    /* delete listeners and other stuff only if
	     * it's our last chance to do that */
	    DelAllExtras_ttcomponent(o);
	
	TClass(ttobject)->Del((ttobject)o);
    }
}


/* ttvisible */
static ttvisible null_New_ttvisible(ttclass_ttvisible Class, ttvisible o) {
    if ((o = (ttvisible)TClass(ttcomponent)->New((ttclass_ttcomponent)Class, (ttcomponent)o))) {
	o->vflags = ttvisible_vflags_visible;
	o->prev = o->next = o->parent = o->child_first = o->child_last = NULL;
	o->constraint = (ttany)0;
	o->theme = NULL;
	o->repaint = (ttvisible_repaint_fn)0;
    }
    return o;
}
static void null_Del_ttvisible(ttvisible o) {
    ttvisible c;
    if (o) {
	o->Class->Remove(o);
	while ((c = o->child_first))
	    c->Class->Remove(c);
	TClass(ttcomponent)->Del((ttcomponent)o);
    }
}
static void null_AddTo_ttvisible(ttvisible o, ttvisible parent, ttany constraint) {
    /* append */
    if (parent && !o->parent) {
	if ((o->prev = parent->child_last))
	    parent->child_last->next = o;
	else
	    parent->child_first = o;
	parent->child_last = o;
	o->next = NULL;
	o->parent = parent;

	o->constraint = constraint;
	parent->Class->Validate(parent);
    }
}
static void null_Validate_ttvisible(ttvisible o) {
}
static void null_SetVisible_ttvisible(ttvisible o, byte visible) {
    if (visible)
	o->vflags |= ttvisible_vflags_visible;
    else
	o->vflags &= ~ttvisible_vflags_visible;
}
static void null_Remove_ttvisible(ttvisible o) {
    ttvisible parent;
    if ((parent = o->parent)) {
	if (o->prev)
	    o->prev->next = o->next;
	else
	    parent->child_first = o->next;
	if (o->next)
	    o->next->prev = o->prev;
	else
	    parent->child_last = o->prev;
	o->parent = NULL;
    }
}
static void null_Invalidate_ttvisible(ttvisible o) {
    /* nothing to do here */
}
static ttbyte null_SetTheme_ttvisible(ttvisible o, tttheme theme) {
    tttheme oldtheme = myTheme(o);
    
    o->theme = theme;
    o->Class->Invalidate(o);
    if (theme != oldtheme && o->parent && (o->vflags & ttvisible_vflags_visible))
	Expose_ttvisible(o, ttvisible_repaint_args_WHOLE);

    return TT_TRUE;
}
static void null_Draw_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch,
				TT_CONST ttbyte *asciidata, TT_CONST ttfont *fontdata, TT_CONST ttattr *attrdata) {
    /* do-nothing */
}
static void null_BuiltinRepaint_ttvisible(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h) {
    /* do-nothing */
}


/* ttlayout */
static ttlayout null_New_ttlayout(ttclass_ttlayout Class, ttlayout o) {
    if ((o = (ttlayout)TClass(ttcomponent)->New((ttclass_ttcomponent)Class, (ttcomponent)o))) {
	o->widget = NULL;
    }
    return o;
}
static void null_Del_ttlayout(ttlayout o) {
    if (o) {
	if (o->widget)
	    o->widget->Class->SetLayout(o->widget, NULL);

	TClass(ttcomponent)->Del((ttcomponent)o);
    }
}
static void null_SetWidget_ttlayout(ttlayout o, ttwidget w) {
    o->widget = w;
}
static void null_DoLayout_ttlayout(ttlayout o) {
}


/* ttboxlayout */
static ttboxlayout null_New_ttboxlayout(ttclass_ttboxlayout Class, ttboxlayout o) {
    if ((o = (ttboxlayout)TClass(ttlayout)->New((ttclass_ttlayout)Class, (ttlayout)o))) {
	o->orientation = ttboxlayout_orientation_x;
    }
    return o;
}

/* ttborderlayout */



/* ttnative */
static ttnative null_GetRoot_ttnative(void) {
    return Create_ttnative((ttany)0);
}
static ttshort null_GetW_ttnative(ttnative o) {
    return (ttshort)1;
}
static ttshort null_GetH_ttnative(ttnative o) {
    return (ttshort)1;
}


/* ttwidget */
static ttwidget null_New_ttwidget(ttclass_ttwidget Class, ttwidget o) {
    if ((o = (ttwidget)TClass(ttvisible)->New((ttclass_ttvisible)Class, (ttvisible)o))) {
	o->x = o->y = o->xl = o->yl = 0;
	o->layout = NULL;
	o->w = o->h = o->wl = o->hl = 1;
	o->col = myTheme(o)->bg[tttheme_bg_normal];
	o->tooltip = (tttooltip)0;
    }
    return o;
}
static void null_Del_ttwidget(ttwidget o) {
    if (o) {
	if (o->tooltip) {
	    TDEL(o->tooltip);
	    o->tooltip = (tttooltip)0;
	}
	TClass(ttvisible)->Del((ttvisible)o);
    }
}
static void null_AddTo_ttwidget(ttwidget o, ttvisible parent, ttany constraint) {
    if (parent && !o->parent) {
	/* prepend to list, cannot use TClass(ttvisible)->AddTo() */
	if ((o->next = parent->child_first))
	    parent->child_first->prev = (ttvisible)o;
	else
	    parent->child_last = (ttvisible)o;
	parent->child_first = (ttvisible)o;
	o->prev = NULL;
	o->parent = parent;

	o->constraint = constraint;
	parent->Class->Validate(parent);
    }
}
static void null_Validate_ttwidget(ttwidget o) {
}
static void null_Invalidate_ttwidget(ttwidget o) {
    tttheme t = myTheme(o);
    
#ifdef ttwidget_col
    /* in case o->col is client-visible */
    FIRE_EVENT(o->col = t->bg[tttheme_bg_normal], o, ttwidget_col, t->bg[tttheme_bg_normal], o->col);
#else
    o->col = t->bg[tttheme_bg_normal];
#endif
}
static ttbyte null_SetXY_ttwidget(ttwidget o, ttshort x, ttshort y) {
    o->x = x;
    o->y = y;
    return TT_TRUE;
}
static ttbyte null_SetWH_ttwidget(ttwidget o, ttshort w, ttshort h) {
    o->w = w;
    o->h = h;
    return TT_TRUE;
}
static ttbyte null_SetXl_ttwidget(ttwidget o, ttint xl) {
    o->xl = xl;
    return TT_TRUE;
}
static ttbyte null_SetYl_ttwidget(ttwidget o, ttint yl) {
    o->yl = yl;
    return TT_TRUE;
}
static ttbyte null_SetWl_ttwidget(ttwidget o, ttint wl) {
    o->wl = wl;
    return TT_TRUE;
}
static ttbyte null_SetHl_ttwidget(ttwidget o, ttint hl) {
    o->hl = hl;
    return TT_TRUE;
}
static ttbyte null_SetTooltip_ttwidget(ttwidget o, tttooltip t) {
    if (o->tooltip != t) {
	if (o->tooltip)
	    TDEL(o->tooltip);
	if ((o->tooltip = t))
	    return t->Class->SetWidget(t, o);
    }
    return TT_TRUE;
}
static void null_SetLayout_ttwidget(ttwidget o, ttlayout l) {
    TDEL(o->layout);
    o->layout = l;
    o->Class->Validate(o);
}
    
/* ttlabel */
static ttlabel null_New_ttlabel(ttclass_ttlabel Class, ttlabel o) {
    if ((o = (ttlabel)TClass(ttwidget)->New((ttclass_ttwidget)Class, (ttwidget)o))) {
	o->text_len = 0;
	o->text = NULL;
	o->col = myTheme(o)->bg[tttheme_bg_label];
    }
    return o;
}
static void null_Del_ttlabel(ttlabel o) {
    if (o) {
	if (o->text) {
	    TTFreeMem(o->text);
	    o->text = NULL;
	}
	TClass(ttwidget)->Del((ttwidget)o);
    }
}
static void null_SetFontD_ttlabel(ttlabel o, ttopaque text_len, TT_ARG_DIE TT_ARG_ARRAY((_P(2))) ttfont * text) {
    if (o->text)
	TTFreeMem(o->text);
    o->text_len = text_len;
    o->text = text;
    if (o->parent && o->vflags & ttvisible_vflags_visible)
	Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE);
}

/* tttooltip */
static tttooltip null_New_tttooltip(ttclass_tttooltip Class, tttooltip o) {
    if ((o = (tttooltip)TClass(ttlabel)->New((ttclass_ttlabel)Class, (ttlabel)o))) {
	o->widget = (ttwidget)0;
    }
    return o;
}
static ttbyte null_SetWidget_tttooltip(tttooltip o, ttwidget w) {
    if (o->widget != w) {
	o->widget = w;
	
	/*
	 * CAUTION: infinite loop risk:
	 * we call SetTooltip_ttwidget(), which may call us.
	 * 
	 * solution: we do not call it if w->tooltip is already correct
	 */
	if (w && w->tooltip != o)
	    return w->Class->SetTooltip(w, o);
    }
    return TT_TRUE;
}
static void null_Del_tttooltip(tttooltip o) {
    if (o) {
	if (o->widget) {
	    /* could FIRE_EVENT() here, but this is more compact */
	    TTSetWidget_tttooltip((tt_obj)o->id, TT_NOID);
	}
	TClass(ttlabel)->Del((ttlabel)o);
    }
}


/* ttbuttongroup */
static ttbuttongroup null_New_ttbuttongroup(ttclass_ttbuttongroup Class, ttbuttongroup o) {
    if ((o = (ttbuttongroup)TClass(ttcomponent)->New((ttclass_ttcomponent)Class, (ttcomponent)o))) {
	o->group_first = o->group_last = o->checked = (ttradiobutton)0;
    }
    return o;
}
static void null_Del_ttbuttongroup(ttbuttongroup o) {
    while (o->group_first) {
	o->Class->Remove(o, o->group_first);
    }
    TClass(ttcomponent)->Del((ttcomponent)o);
}
static void null_Add_ttbuttongroup(ttbuttongroup g, ttradiobutton o) {
    if (o && !o->group) {
	
	if (g->checked && (o->vflags & ttcheckbutton_vflags_checked))
	    /* we already have a checked button, uncheck this */
	    o->Class->SetChecked(o, TT_FALSE);
	    
	if ((o->group_prev = g->group_last))
	    g->group_last->group_next = o;
	else
	    g->group_first = o;
	g->group_last = o;
	o->group_next = NULL;
	o->group = g;
	
	    
	if (!g->checked && (o->vflags & ttcheckbutton_vflags_checked)) {
	    /* we do not have a checked button, use this as the checked one */
	    FIRE_EVENT(g->checked = o, g,
		       ttbuttongroup_checked, (ttany)(opaque)o, (ttany)0);
	}
    }
}
static void null_Remove_ttbuttongroup(ttbuttongroup g, ttradiobutton o) {
    if (o && o->group == g) {
	if (o == g->checked)
	    g->Class->SetChecked(g, (ttradiobutton)0);
	
	if (o->group_prev)
	    o->group_prev->group_next = o->group_next;
	else
	    g->group_first = o->group_next;
	if (o->group_next)
	    o->group_next->group_prev = o->group_prev;
	else
	    g->group_last = o->group_prev;
	
	o->group_prev = o->group_next = (ttradiobutton)0;
	o->group = (ttbuttongroup)0;
    }
}
static void null_SetChecked_ttbuttongroup(ttbuttongroup g, ttradiobutton o) {
    ttradiobutton c;
    
    if (o != (c = g->checked)) {
	/*
	 * CAUTION: infinite loop risk:
	 * we call SetChecked_ttradiobutton(), which may call us.
	 * 
	 * solution: we update g->checked *before* calling it.
	 */
	FIRE_EVENT(g->checked = o, g,
		   ttbuttongroup_checked, (ttany)(opaque)o, (ttany)(opaque)c);
	
	if (c)
	    c->Class->SetChecked(c, TT_FALSE);
	if (o)
	    o->Class->SetChecked(o, TT_TRUE);
    }
}


/* ttanybutton */
static ttanybutton null_New_ttanybutton(ttclass_ttanybutton Class, ttanybutton o) {
    if ((o = (ttanybutton)TClass(ttwidget)->New((ttclass_ttwidget)Class, (ttwidget)o))) {
	o->text = NULL;
	o->text_width = o->text_height = 0;
	o->col = myTheme(o)->bg[tttheme_bg_anybutton];
	
	TTWriteMem(o->theme_shape, '\0', sizeof(o->theme_shape));
	return o;
    }
    return NULL;
}
static void null_Invalidate_ttanybutton(ttanybutton o) {
    ttbyte i;
    
#ifdef DEBUG_TT_HW_NULL
    fprintf(stderr, "null_Invalidate_ttanybutton(o=0x%X)\n",
	    (unsigned)o->id);
#endif
    for (i = 0; i < sizeof(o->theme_shape)/sizeof(o->theme_shape[0]); i++) {
	if (o->theme_shape[i].attr) {
	    TTFreeMem(o->theme_shape[i].attr);
	    o->theme_shape[i].attr = NULL;
	}
    }
}
static void null_Del_ttanybutton(ttanybutton o) {
    if (o) {
	o->Class->Invalidate(o);
    
	if (o->text) {
	    TTFreeMem(o->text);
	    o->text = NULL;
	}
	TClass(ttwidget)->Del((ttwidget)o);
    }
}
static void null_SetAttrD_ttanybutton(ttanybutton o, ttshort width, ttshort height, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text) {
    o->Class->Invalidate(o);
    
    o->text_width = width;
    o->text_height = height;
    if (o->text)
	TTFreeMem(o->text);
    o->text = text;
    
    
    if (o->parent && o->vflags & ttvisible_vflags_visible)
	Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE);
}
static ttuint null_CacheThemeShape_ttanybutton(ttanybutton o) {
    return (ttuint)-1;
}
static void null_AddShape(ttattr *attr, ttshort pitch, ttshort w, ttshort h,
			  ttattr *tattr, ttshort tw, ttshort th,
			  ttshort b_left, ttshort b_up, ttshort b_right, ttshort b_down,
			  ttbyte also_inside) {
    ttshort ti, i, tj, j;
    ttattr a;
    
    /* set inside */
    if (also_inside) {
	for (j = tj = b_up; j < h - b_down; j++, tj++) {
	    if (tj >= th - b_down)
		tj = b_up;
	    for (i = ti = b_left; i < w - b_right; i++, ti++) {
		if (ti >= tw - b_right)
		    ti = b_left;
		if ((a = tattr[ti+tj*tw]))
		    attr[i+j*pitch] = a;
	    }
	}
    }
    /* set left and right upper corners */
    for (j = 0; j < b_up; j++) {
	for (i = 0; i < b_left; i++) {
	    if ((a = tattr[i+j*tw]))
		attr[i+j*pitch] = a;
	}
	
	for (i = 0; i < b_right; i++) {
	    if ((a = tattr[i+(j+1)*tw-b_right]))
		attr[i+(j+1)*pitch-b_right] = a;
	}
    }
    /* set left and right borders */
    for (j = tj = b_up; j < h - b_down; j++, tj++) {
	if (tj >= tw - b_down)
	    tj = b_up;
	for (i = 0; i < b_left; i++) {
	    if ((a = tattr[i+tj*tw]))
		attr[i+j*pitch] = a;
	}
	for (i = 0; i < b_right; i++) {
	    if ((a = tattr[i+(tj+1)*tw-b_right]))
		attr[i+(j+1)*pitch-b_right] = a;
	}
    }
    /* set left and right lower corners */
    for (j = 0; j < b_down; j++) {
	for (i = 0; i < b_left; i++) {
	    if ((a = tattr[i+(th-b_down+j)*tw]))
		attr[i+(h-b_down+j)*pitch] = a;
	}
	for (i = 0; i < b_right; i++) {
	    if ((a = tattr[i+(th-b_down+j+1)*tw-b_right]))
		attr[i+(h-b_down+j+1)*pitch-b_right] = a;
	}
    }
    /* set upper border */
    for (j = 0; j < b_up; j++) {
	for (i = ti = b_left; i < w - b_right; i++, ti++) {
	    if (ti >= tw - b_right)
		ti = 0;
	    if ((a = tattr[ti+j*tw]))
		attr[i+j*pitch] = a;
	}
    }
    /* set lower border */
    for (j = 0; j < b_down; j++) {
	for (i = ti = b_left; i < w - b_right; i++, ti++) {
	    if (ti >= tw - b_right)
		ti = b_left;
	    if ((a = tattr[ti+(th-b_down+j)*tw]))
		attr[i+(h-b_down+j)*pitch] = a;
	}
    }
}
#define null_AddShapeI(attr, pitch, w, h, tattr, tw, th, b_left, b_up, b_right, b_down) \
    null_AddShape((attr), (pitch), (w), (h), (tattr), (tw), (th), (b_left), (b_up), (b_right), (b_down), TT_TRUE)

#define null_AddShapeN(attr, pitch, w, h, tattr, tw, th, b_left, b_up, b_right, b_down) \
    null_AddShape((attr), (pitch), (w), (h), (tattr), (tw), (th), (b_left), (b_up), (b_right), (b_down), TT_FALSE)

static void null_AddText(ttattr *attr, ttshort w, ttattr *text, ttshort tw, ttshort th) {
    ttshort _tw;
    ttfont f;
    ttcol c;
    
    for (; th; th--) {
	for (_tw = tw; _tw; _tw--, attr++, text++) {
	    f = TTGetFont_ttattr(*text);
	    c = TTGetCol_ttattr(*attr);
	    *attr = TTAttr(c, f);
	}
	attr += w - tw;
    }
}
static ttuint null_CacheThemeShape_anybutton(ttanybutton o, ttuint statemask, ttuint case_n, TT_CONST ttuint case_flags[][3]) {
    ttuint i, vflags;
    
    vflags = o->vflags & statemask;
    
    for (i = 0; i < case_n; i++) {
	if (case_flags[i][0] == vflags)
	    break;
    }
    if (i >= case_n)
	return (ttuint)-1;
    
    if (o->theme_shape[case_flags[i][1]].attr)
	return case_flags[i][1];

    {
	ttbyte button_i = i;
	ttbyte theme_i = case_flags[i][2];
	tttheme t = myTheme(o);
	ttshape t_shape = &t->shape[theme_i][0], s_shape =  &t->shape[theme_i][1];
	ttattr *shape;
	ttshort w = o->text_width, h = o->text_height;
	ttshort b_left = t_shape->border[tt_x_left], b_up = t_shape->border[tt_y_up];
	ttshort b_right = t_shape->border[tt_x_right], b_down = t_shape->border[tt_y_down];
	
	if ((shape = (ttattr *)TTAllocMem((w += b_left+b_right) * (h += b_up+b_down) * sizeof(ttattr) ))) {
	    
	    TTWriteMem(shape, '\0', w * h * sizeof(ttattr));
	    
	    o->theme_shape[button_i].width = w;
	    o->theme_shape[button_i].height = h;
	    TTCopyMem(t_shape->border, o->theme_shape[button_i].border, 4*sizeof(ttshort));
	    
	    null_AddShapeI(shape, w, w, h, t_shape->attr, t_shape->width, t_shape->height,
			   b_left, b_up, b_right, b_down);
	    
	    null_AddShapeI(shape, w, w, h, s_shape->attr, s_shape->width, s_shape->height,
			   s_shape->border[tt_x_left], s_shape->border[tt_y_up],
			   s_shape->border[tt_x_right], s_shape->border[tt_y_down]);
	    
	    null_AddText(shape + b_left + w * b_up, w, o->text, o->text_width, o->text_height);

	    o->theme_shape[case_flags[i][1]].attr = shape;

	    if (o->w < w || o->h < h)
		/* could FIRE_EVENT() here, but this is more compact */
		TTSetWH_ttwidget((tt_obj)o->id, w, h);

	    return case_flags[i][1];
	}

    }
    return (ttuint)-1;    
}



/* ttbutton */
static void null_MouseListener_ttbutton(ttany arg) {
    ttbutton o;
    tteventbig ev;
    ttuint old_vflags, vflags;
    ttbyte inside, activated = TT_FALSE;
    
    /* default mouse handler */
    
    if (!((ev = ID2(tteventbig,arg)) && TTAssert(ev) &&
	  TTAssert(ev->evtype == ttevent_evtype_mouse) &&
	  (o = (ttbutton)ev->component)
	  && TTAssert(IS(ttbutton,o))))
	
	return;
    
    vflags = old_vflags = o->vflags;
    inside = ev->x >= 0 && ev->y >= 0 && ev->x < o->w && ev->y < o->h;
    
    switch (ev->evcode) {
      case ttevent_evcode_mouse_move:
	if (inside)
	    vflags |= ttanybutton_vflags_prelight;
	else
	    vflags &= ~ttanybutton_vflags_prelight;
	break;
      case ttevent_evcode_mouse_press_1:
	vflags &= ~ttanybutton_vflags_prelight;
	vflags |= ttanybutton_vflags_pressed;
	break;
      case ttevent_evcode_mouse_move | ttevent_evcode_mouse_hold_1:
	if (inside)
	    vflags |= ttanybutton_vflags_pressed;
	else
	    vflags &= ~ttanybutton_vflags_pressed;
	break;
      case ttevent_evcode_mouse_release_1:
	vflags &= ~ttanybutton_vflags_pressed;
	if (inside) {
	    vflags |= ttanybutton_vflags_prelight;
	    
	    activated = TT_TRUE;
	}
	break;
      default:
	break;
    }
    
    if (vflags != old_vflags) {
	FIRE_EVENT(o->vflags = vflags, o, ttvisible_vflags, vflags, old_vflags);
	
	if (o->parent && (o->vflags & ttvisible_vflags_visible))
	    Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE);
    }
    if (activated)
	FireSimpleEvent((ttcomponent)o, ttevent_evtype_activate);
}
static ttlistener null_CreateListener_ttbutton(ttbutton o, ttuint evcode) {
    return CreateE_ttlistener
	((ttcomponent)o, Create_ttevent(ttevent_evtype_mouse, evcode, 0),
	 ttlistener_lflags_arg0_event,
	 (ttlistener_fn)null_MouseListener_ttbutton, (ttany)0);
}
static ttbutton null_New_ttbutton(ttclass_ttbutton Class, ttbutton o) {
    if ((o = (ttbutton)TClass(ttanybutton)->New((ttclass_ttanybutton)Class, (ttanybutton)o))) {
	if (null_CreateListener_ttbutton(o, ttevent_evcode_mouse_move) &&
	    null_CreateListener_ttbutton(o, ttevent_evcode_mouse_press_1) &&
	    null_CreateListener_ttbutton(o, ttevent_evcode_mouse_move | ttevent_evcode_mouse_hold_1) &&
	    null_CreateListener_ttbutton(o, ttevent_evcode_mouse_release_1)) {
	    
	    return o;
	}
	TDEL(o);
    }
    return (ttbutton)0;
}
static void null_SetPressed_ttbutton(ttbutton o, byte pressed) {
    if (pressed)
	o->vflags |= ttanybutton_vflags_pressed;
    else
	o->vflags &= ~ttanybutton_vflags_pressed;

    if (o->vflags != o->vflags && (o->vflags & ttvisible_vflags_visible) && o->parent)
	Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE);
}
static ttuint null_CacheThemeShape_ttbutton(ttbutton o) {
    static TT_CONST ttuint case_flags[][3] = {
	{ ttanybutton_vflags_normal,   ttbutton_shape_normal,   tttheme_shape_ttbutton_normal,   },
	{ ttanybutton_vflags_prelight, ttbutton_shape_prelight, tttheme_shape_ttbutton_prelight, },
	{ ttanybutton_vflags_pressed,  ttbutton_shape_pressed,  tttheme_shape_ttbutton_pressed,  },
	{ ttanybutton_vflags_disabled, ttbutton_shape_disabled, tttheme_shape_ttbutton_disabled, },
    };
    return null_CacheThemeShape_anybutton((ttanybutton)o, ttanybutton_vflags_statemask, sizeof(case_flags)/sizeof(case_flags[0]), case_flags);
}


/* ttcheckbutton */
static void null_MouseListener_ttcheckbutton(ttany arg) {
    ttcheckbutton o;
    ttbyte checked;
    
    
    if (TTAssert(o = ID2(ttcheckbutton,arg))) {
	if (IS(ttradiobutton,o))
	    /* always set to checked state upon activate event */
	    checked = TT_TRUE;
	else /* IS(ttcheckbutton,o) */
	    /* toggle checked state upon activate event */
	    checked = !(o->vflags & ttcheckbutton_vflags_checked);
	
	/* could FIRE_EVENT() here, but this is more compact */
	TTSetChecked_ttcheckbutton((tt_obj)o->id, checked);
    }
}
static ttcheckbutton null_New_ttcheckbutton(ttclass_ttcheckbutton Class, ttcheckbutton o) {
    if ((o = (ttcheckbutton)TClass(ttbutton)->New((ttclass_ttbutton)Class, (ttbutton)o))) {
	if (Create_ttlistener
	    ((ttcomponent)o, ttevent_evtype_activate,
	     ttlistener_lflags_arg0_component,
	     (ttlistener_fn)null_MouseListener_ttcheckbutton, (ttany)0)) {
	    
	    return o;
	}
	TDEL(o);
    }
    return NULL;
}
static void null_SetChecked_ttcheckbutton(ttcheckbutton o, ttbyte checked) {
    ttuint old_vflags = o->vflags;
    
    if (checked)
	o->vflags |= ttcheckbutton_vflags_checked;
    else
	o->vflags &= ~ttcheckbutton_vflags_checked;

    if (o->vflags != old_vflags && (o->vflags & ttvisible_vflags_visible) && o->parent)
	Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE);
}
static ttuint null_CacheThemeShape_ttcheckbutton(ttcheckbutton o) {
#define c ttcheckbutton_vflags_checked
    static TT_CONST ttuint case_flags[][3] = {
	{   ttanybutton_vflags_normal,   ttcheckbutton_shape_normal,           tttheme_shape_ttcheckbutton_normal,   },
	{   ttanybutton_vflags_prelight, ttcheckbutton_shape_prelight,         tttheme_shape_ttcheckbutton_prelight, },
	{   ttanybutton_vflags_pressed,  ttcheckbutton_shape_pressed,          tttheme_shape_ttcheckbutton_pressed,  },
	{   ttanybutton_vflags_disabled, ttcheckbutton_shape_disabled,         tttheme_shape_ttcheckbutton_disabled, },
	{ c|ttanybutton_vflags_normal,   ttcheckbutton_shape_checked_normal,   tttheme_shape_ttcheckbutton_checked_normal,   },
	{ c|ttanybutton_vflags_prelight, ttcheckbutton_shape_checked_prelight, tttheme_shape_ttcheckbutton_checked_prelight, },
	{ c|ttanybutton_vflags_pressed,  ttcheckbutton_shape_checked_pressed,  tttheme_shape_ttcheckbutton_checked_pressed,  },
	{ c|ttanybutton_vflags_disabled, ttcheckbutton_shape_checked_disabled, tttheme_shape_ttcheckbutton_checked_disabled, },
    };
#undef c
    return null_CacheThemeShape_anybutton((ttanybutton)o, ttcheckbutton_vflags_statemask, sizeof(case_flags)/sizeof(case_flags[0]), case_flags);
}


/* ttradiobutton */
static ttradiobutton null_New_ttradiobutton(ttclass_ttradiobutton Class, ttradiobutton o) {
    if ((o = (ttradiobutton)TClass(ttcheckbutton)->New((ttclass_ttcheckbutton)Class, (ttcheckbutton)o))) {
	o->group = (ttbuttongroup)0;
	o->group_prev = o->group_next = (ttradiobutton)0;
    }
    return o;
}
static void null_Del_ttradiobutton(ttradiobutton o) {
    ttbuttongroup g;
    if (o) {
	if ((g = o->group))
	    g->Class->Remove(g, o);
    
	TClass(ttcheckbutton)->Del((ttcheckbutton)o);
    }
}
static void null_SetChecked_ttradiobutton(ttradiobutton o, ttbyte checked) {
    ttbuttongroup g;
    
    TClass(ttcheckbutton)->SetChecked((ttcheckbutton)o, checked);
    
    /*
     * CAUTION: infinite loop risk:
     * we call SetChecked_ttbuttongroup(), which may call us.
     * 
     * solution: we do not call it if o->group->checked is already correct
     */
    if ((g = o->group)) {
	if (checked && g->checked != o)
	    g->Class->SetChecked(g, o);
	else if (!checked && g->checked == o)
	    g->Class->SetChecked(g, (ttradiobutton)0);
    }
}
static ttuint null_CacheThemeShape_ttradiobutton(ttradiobutton o) {
#define c ttcheckbutton_vflags_checked
    static TT_CONST ttuint case_flags[][3] = {
	{   ttanybutton_vflags_normal,   ttradiobutton_shape_normal,           tttheme_shape_ttradiobutton_normal,   },
	{   ttanybutton_vflags_prelight, ttradiobutton_shape_prelight,         tttheme_shape_ttradiobutton_prelight, },
	{   ttanybutton_vflags_pressed,  ttradiobutton_shape_pressed,          tttheme_shape_ttradiobutton_pressed,  },
	{   ttanybutton_vflags_disabled, ttradiobutton_shape_disabled,         tttheme_shape_ttradiobutton_disabled, },
	{ c|ttanybutton_vflags_normal,   ttradiobutton_shape_checked_normal,   tttheme_shape_ttradiobutton_checked_normal,   },
	{ c|ttanybutton_vflags_prelight, ttradiobutton_shape_checked_prelight, tttheme_shape_ttradiobutton_checked_prelight, },
	{ c|ttanybutton_vflags_pressed,  ttradiobutton_shape_checked_pressed,  tttheme_shape_ttradiobutton_checked_pressed,  },
	{ c|ttanybutton_vflags_disabled, ttradiobutton_shape_checked_disabled, tttheme_shape_ttradiobutton_checked_disabled, },
    };
#undef c
    return null_CacheThemeShape_anybutton((ttanybutton)o, ttcheckbutton_vflags_statemask, sizeof(case_flags)/sizeof(case_flags[0]), case_flags);
}


/* ttanyscroll */
static ttanyscroll null_New_ttanyscroll(ttclass_ttanyscroll Class, ttanyscroll o) {
    if ((o = (ttanyscroll)TClass(ttanybutton)->New((ttclass_ttanybutton)Class, (ttanybutton)o))) {
	o->orientation = ttanyscroll_orientation_x;
	o->size = o->real_size = o->view_size = o->real_view_size = 1;
	o->position = o->real_position = 0;
	o->state = ttanyscroll_state_normal;
    }
    return o;
}

static void null_Recalculate_ttanyscroll(ttanyscroll o, ttint size, ttint real_size, ttint view_size, ttint position) {
    if (size != TT_MIN_ttint)
	o->size = minBOUND(size, 0);
    if (real_size != TT_MIN_ttint)
	o->real_size = minBOUND(real_size, 0);
    if (view_size != TT_MIN_ttint)
	o->view_size = BOUND(view_size, 0, o->size);
    if (position != TT_MIN_ttint) {
	if (o->size - o->view_size > 0)
	    o->position = BOUND(position, 0, o->size - o->view_size);
	else
	    o->position = 0;
    }

    if (o->size > 0) {
	/* round up */
	o->real_view_size = (o->view_size * o->real_size + o->size - 1) / o->size;
	/* round to nearest */
	o->real_position = (o->position * o->real_size + o->size / 2) / o->size;
	o->real_position = BOUND(o->real_position, 0, o->real_size - o->real_view_size);
    } else
	o->real_view_size = o->real_position = 0;
}

static ttbyte null_SetOrientation_ttanyscroll(ttanyscroll o, ttbyte orientation) {
    REVALIDATE(o->orientation = orientation);
    return TT_TRUE;
}
static ttbyte null_SetSize_ttanyscroll(ttanyscroll o, ttint size) {
    REVALIDATE(o->Class->Recalculate(o, size, TT_MIN_ttint, TT_MIN_ttint, TT_MIN_ttint));
    return TT_TRUE;
}
static ttbyte null_SetRealSize_ttanyscroll(ttanyscroll o, ttint real_size) {
    REVALIDATE(o->Class->Recalculate(o, TT_MIN_ttint, real_size, TT_MIN_ttint, TT_MIN_ttint));
    return TT_TRUE;
}
static ttbyte null_SetViewSize_ttanyscroll(ttanyscroll o, ttint view_size) {
    REVALIDATE(o->Class->Recalculate(o, TT_MIN_ttint, TT_MIN_ttint, view_size, TT_MIN_ttint));
    return TT_TRUE;
}
static ttbyte null_SetPosition_ttanyscroll(ttanyscroll o, ttint position) {
    REVALIDATE(o->Class->Recalculate(o, TT_MIN_ttint, TT_MIN_ttint, TT_MIN_ttint, position));
    return TT_TRUE;
}
static ttbyte null_SetState_ttanyscroll(ttanyscroll o, ttuint state) {
    REVALIDATE(o->state = state);
    return TT_TRUE;
}

static ttuint null_CacheThemeShape_anyscroll(ttanyscroll o, ttuint statemask,
					     ttuint case_n, TT_CONST ttuint case_flags[][3]) {
    ttuint i, vflags;
    
    vflags = o->vflags & statemask;
    
    for (i = 0; i < case_n; i++) {
	if (case_flags[i][0] == vflags)
	    break;
    }
    if (i >= case_n)
	return (ttuint)-1;
    
    if (o->theme_shape[case_flags[i][1]].attr)
	return case_flags[i][1];

    o->Class->Recalculate(o, TT_MIN_ttint, TT_MIN_ttint, TT_MIN_ttint, TT_MIN_ttint);

    {
	ttbyte x_or_y = o->orientation == ttanyscroll_orientation_x;
	ttbyte scroll_i = i;
	ttbyte theme_i = case_flags[i][2];
	tttheme t = myTheme(o);
	ttshape t_shape = &t->shape[theme_i][0], s_shape =  &t->shape[theme_i][1];
	ttattr *shape;
	ttshort w, h;
	ttshort b_left = t_shape->border[tt_x_left], b_up = t_shape->border[tt_y_up];
	ttshort b_right = t_shape->border[tt_x_right], b_down = t_shape->border[tt_y_down];
	ttint real_pos = o->real_position;
	
	if (x_or_y)
	    w = o->real_size, h = t_shape->height;
	else
	    w = t_shape->width, h = o->real_size;
	    
	if ((shape = (ttattr *)TTAllocMem((w += b_left+b_right) * (h += b_up+b_down) * sizeof(ttattr) ))) {
	    
	    TTWriteMem(shape, '\0', w * h * sizeof(ttattr));
	    
	    o->theme_shape[scroll_i].width = w;
	    o->theme_shape[scroll_i].height = h;
	    TTCopyMem(t_shape->border, o->theme_shape[scroll_i].border, 4*sizeof(ttshort));
	    
	    null_AddShapeI(shape, w, w, h, t_shape->attr, t_shape->width, t_shape->height,
			   b_left, b_up, b_right, b_down);
	    
	    if (x_or_y) {
		null_AddShapeI(shape + real_pos, w, o->real_view_size, h,
			       s_shape->attr, s_shape->width, s_shape->height,
			       s_shape->border[tt_x_left], s_shape->border[tt_y_up],
			       s_shape->border[tt_x_right], s_shape->border[tt_y_down]);
	    } else {
		null_AddShapeI(shape + w * real_pos, w, w, o->real_view_size,
			       s_shape->attr, s_shape->width, s_shape->height,
			       s_shape->border[tt_x_left], s_shape->border[tt_y_up],
			       s_shape->border[tt_x_right], s_shape->border[tt_y_down]);
	    }

	    o->theme_shape[case_flags[i][1]].attr = shape;
	    
	    if (o->w != w || o->h != h)
		/* could FIRE_EVENT() here, but this is more compact */
		TTSetWH_ttwidget((tt_obj)o->id, w, h);

	    return case_flags[i][1];
	}

    }
    return (ttuint)-1;    
}

/* ttscrollbar */
static void null_Del_ttscrollbar(ttscrollbar o) {
    ttscrollpane w;
    
    if (o) {
	if ((w = o->scrollpane)) {
	    o->scrollpane = NULL;
	    
	    if (o->orientation == ttanyscroll_orientation_x)
		TTSetBarX_ttscrollpane((tt_obj)w->id, NULL);
	    else
		TTSetBarY_ttscrollpane((tt_obj)w->id, NULL);
	}
	TClass(ttanyscroll)->Del((ttanyscroll)o);
    }
}
static void null_SyncScrollpane_ttscrollbar(ttscrollbar o, ttscrollpane w) {
    if (o->orientation == ttanyscroll_orientation_x) {
	if (o->position != w->xl)
	    /* could FIRE_EVENT() here, but this is more compact */
	    TTSetXl_ttwidget((tt_obj)w->id, o->position);
    } else if (o->orientation == ttanyscroll_orientation_y) {
	if (o->position != w->yl)
	    /* could FIRE_EVENT() here, but this is more compact */
	    TTSetYl_ttwidget((tt_obj)w->id, o->position);
    }
}
static void null_Recalculate_ttscrollbar(ttscrollbar o, ttint size, ttint real_size, ttint view_size, ttint position) {
    TClass(ttanyscroll)->Recalculate((ttanyscroll)o, size, real_size, view_size, position);
    if (o->scrollpane)
	null_SyncScrollpane_ttscrollbar(o, o->scrollpane);
}
static ttuint null_ComputeMouseState_ttscrollbar(ttscrollbar o, ttint x, ttint y, ttint *ret_p) {
    /* calculate where the mouse is */
    tttheme t = myTheme(o);
    ttint rev1, rev2, fwd1, fwd2, s, S, p, P;
    ttuint i, state = ttscrollbar_state_normal;
    ttbyte inside = TT_FALSE, orientation = o->orientation == ttanyscroll_orientation_x;
    ttint b1, b2;

    i = o->Class->CacheThemeShape(o);
    if (i == (ttuint)-1)
	return state;

    if (orientation) {
	p = x;    P = y;
	s = o->w; S = o->h;
	rev1 = t->val[tttheme_val_ttscrollbar_x_arrow_rev_min];
	rev2 = t->val[tttheme_val_ttscrollbar_x_arrow_rev_max];
	fwd1 = t->val[tttheme_val_ttscrollbar_x_arrow_fwd_min];
	fwd2 = t->val[tttheme_val_ttscrollbar_x_arrow_fwd_max];
	b1 = o->theme_shape[i].border[tt_x_left];
	b2 = o->theme_shape[i].border[tt_x_right];
    } else {
	p = y;    P = x;
	s = o->h; S = o->w;
	rev1 = t->val[tttheme_val_ttscrollbar_y_arrow_rev_min];
	rev2 = t->val[tttheme_val_ttscrollbar_y_arrow_rev_max];
	fwd1 = t->val[tttheme_val_ttscrollbar_y_arrow_fwd_min];
	fwd2 = t->val[tttheme_val_ttscrollbar_y_arrow_fwd_max];
	b1 = o->theme_shape[i].border[tt_y_up];
	b2 = o->theme_shape[i].border[tt_y_down];
    }
    
    if (rev1 < 0) rev1 += s;
    if (rev2 < 0) rev2 += s;
    if (fwd1 < 0) fwd1 += s;
    if (fwd2 < 0) fwd2 += s;
    
    if (P >= 0 && P < S)
	inside = TT_TRUE;
    
    if (inside) {
	if (p >= rev1 && p <= rev2)
	    state = ttscrollbar_state_arrow_rev;
	else if (inside && p >= fwd1 && p <= fwd2)
	    state = ttscrollbar_state_arrow_fwd;
	else if (p >= b1 && p <= s - b2) {
	    p -= b1;
	    if (p < o->real_position)
		state = ttscrollbar_state_page_rev;
	    else if (p >= o->real_position + o->real_view_size)
		state = ttscrollbar_state_page_fwd;
	    else
		state = ttscrollbar_state_tab;
	    p += b1;
	}
    } else
	state = ttscrollbar_state_tab;
	
    *ret_p = p - b1;
    
    return state;
}
static void null_MouseListener_ttscrollbar(ttany arg) {
    ttscrollbar o;
    tteventbig ev;
    tttimer t;
    ttint p, delta = 0, real_delta = 0, position, real_position;
    ttuint old_vflags, vflags, state;
    ttbyte inside, set_state = TT_FALSE, set_timer = TT_FALSE;
    
    /* default mouse handler */
    
    if (!(TTAssert(ev = ID2(tteventbig,arg)) &&
	  TTAssert(ev->evtype == ttevent_evtype_mouse) &&
	  (o = (ttscrollbar)ev->component) &&
	  TTAssert(IS(ttscrollbar,o))))
	
	return;
    
    vflags = old_vflags = o->vflags;
    inside = ev->x >= 0 && ev->y >= 0 && ev->x < o->w && ev->y < o->h;

    state = null_ComputeMouseState_ttscrollbar(o, ev->x, ev->y, &p);
    
    switch (ev->evcode) {
      case ttevent_evcode_mouse_move:
	if (inside)
	    vflags |= ttanybutton_vflags_prelight;
	else
	    vflags &= ~ttanybutton_vflags_prelight;
	break;
      case ttevent_evcode_mouse_press_1:
	vflags &= ~ttanybutton_vflags_prelight;
	vflags |= ttanybutton_vflags_pressed;
	set_state = set_timer = TT_TRUE;
	
	switch (state) {
	  case ttscrollbar_state_page_rev:
	    delta = o->view_size > 1 ? 1 - o->view_size : -1;
	    break;
	  case ttscrollbar_state_page_fwd:
	    delta = o->view_size > 1 ? o->view_size - 1 : 1;
	    break;
	  case ttscrollbar_state_arrow_rev:
	    delta = -1;
	    break;
	  case ttscrollbar_state_arrow_fwd:
	    delta = 1;
	    break;
	  case ttscrollbar_state_tab:
	    state += p - o->real_position;
	    set_timer = TT_FALSE;
	    break;
	  default:
	    set_timer = TT_FALSE;
	    break;
	}
	break;
      case ttevent_evcode_mouse_move | ttevent_evcode_mouse_hold_1:
	if (o->state >= ttscrollbar_state_tab)
	    real_delta = p - o->real_position - (o->state - ttscrollbar_state_tab);
	break;
      case ttevent_evcode_mouse_release_1:
	vflags &= ~ttanybutton_vflags_pressed;
	if (inside)
	    vflags |= ttanybutton_vflags_prelight;
	state = ttscrollbar_state_normal;
	set_state = TT_TRUE;
	break;
      default:
	break;
    }
    
    if (real_delta) {
	/* be careful with rounding! round position, not delta */
	real_position = o->real_position + real_delta;
	/* round to nearest */
	position = (real_position * o->size + o->real_size/2) / o->real_size;
	delta = position - o->position;
    }
    if (delta) {
	/* could FIRE_EVENT() here, but this is more compact */
	TTSetPosition_ttanyscroll((tt_obj)o->id, o->position + delta);
    }
    if (set_state) {
	o->Class->SetState(o, state);
    }
    if ((t = o->timers) && !!(t->lflags & tttimer_lflags_enabled) != set_timer) {
	if (t->args && t->args->array_n > 1)
	    t->args->array[1] = (ttany)delta;
	
	if (set_timer)
	    TTSetDelay_tttimer((tt_obj)t->id, 0, 200 MilliSECs);
	
	t->Class->SetEnabled(t, set_timer);
    }
    if (vflags != old_vflags) {
	FIRE_EVENT(o->vflags = vflags, o, ttvisible_vflags, vflags, old_vflags);
	
	if (o->parent && (o->vflags & ttvisible_vflags_visible))
	    Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE);
    }
}
static ttlistener null_CreateMouseListener_ttscrollbar(ttscrollbar o, ttuint evcode) {
    return CreateE_ttlistener
	((ttcomponent)o, Create_ttevent(ttevent_evtype_mouse, evcode, 0),
	 ttlistener_lflags_arg0_event,
	 (ttlistener_fn)null_MouseListener_ttscrollbar, (ttany)0);
}
static void null_TimerListener_ttscrollbar(ttany arg0, ttany arg1) { 
    tttimer t;
    ttscrollbar o;
    ttint delta;
    
    /* default timer handler */
    if (!((t = ID2(tttimer,arg0)) && TTAssert(t) &&
	  (o = (ttscrollbar)t->component) &&
	  TTAssert(IS(ttscrollbar,o))))
	return;

    if ((delta = (ttint)arg1)) {
	/* could FIRE_EVENT() here, but this is more compact */
	TTSetPosition_ttanyscroll((tt_obj)o->id, o->position + delta);
    }
    /* reactivate the timer itself */
    TTSetDelay_tttimer((tt_obj)t->id, 0, 30 MilliSECs);
}
TT_INLINE tttimer null_CreateTimer_ttscrollbar(ttscrollbar o) {
    return CreateR_tttimer((ttcomponent)o, 0, (ttlistener_fn)null_TimerListener_ttscrollbar,
			   -1, 0, CreateL_ttvector(2, (ttany)0, (ttany)0), 0, 0);
}
static ttscrollbar null_New_ttscrollbar(ttclass_ttscrollbar Class, ttscrollbar o) {
    if ((o = (ttscrollbar)TClass(ttanyscroll)->New((ttclass_ttanyscroll)Class, (ttanyscroll)o))) {
	o->view_size = o->real_view_size = 1;
	o->scrollpane = (ttscrollpane)0;
	
	if (null_CreateMouseListener_ttscrollbar(o, ttevent_evcode_mouse_move) &&
	    null_CreateMouseListener_ttscrollbar(o, ttevent_evcode_mouse_press_1) &&
	    null_CreateMouseListener_ttscrollbar(o, ttevent_evcode_mouse_move | ttevent_evcode_mouse_hold_1) &&
	    null_CreateMouseListener_ttscrollbar(o, ttevent_evcode_mouse_release_1) &&
	    null_CreateTimer_ttscrollbar(o)) {

	    return o;
	}
	TDEL(o);
    }
    return (ttscrollbar)0;
}
static ttuint null_CacheThemeShape_ttscrollbar(ttscrollbar o) {
    static TT_CONST ttuint case_flags_x[][3] = {
	{ ttanybutton_vflags_normal,   ttscrollbar_shape_normal,   tttheme_shape_ttscrollbar_x_normal,   },
	{ ttanybutton_vflags_prelight, ttscrollbar_shape_prelight, tttheme_shape_ttscrollbar_x_prelight, },
	{ ttanybutton_vflags_pressed,  ttscrollbar_shape_pressed,  tttheme_shape_ttscrollbar_x_pressed,  },
	{ ttanybutton_vflags_disabled, ttscrollbar_shape_disabled, tttheme_shape_ttscrollbar_x_disabled, },
    };
    static TT_CONST ttuint case_flags_y[][3] = {
	{ ttanybutton_vflags_normal,   ttscrollbar_shape_normal,   tttheme_shape_ttscrollbar_y_normal,   },
	{ ttanybutton_vflags_prelight, ttscrollbar_shape_prelight, tttheme_shape_ttscrollbar_y_prelight, },
	{ ttanybutton_vflags_pressed,  ttscrollbar_shape_pressed,  tttheme_shape_ttscrollbar_y_pressed,  },
	{ ttanybutton_vflags_disabled, ttscrollbar_shape_disabled, tttheme_shape_ttscrollbar_y_disabled, },
    };
    return null_CacheThemeShape_anyscroll
	((ttanyscroll)o, ttanybutton_vflags_statemask,
	 sizeof(case_flags_x)/sizeof(case_flags_x[0]),
	 o->orientation == ttanyscroll_orientation_x ? case_flags_x : case_flags_y);
}

/* ttslider */
static ttuint null_ComputeMouseState_ttslider(ttslider o, ttint x, ttint y, ttint *ret_p) {
    /* calculate where the mouse is */
    ttint s, S, p, P;
    ttuint i, state = ttslider_state_normal;
    ttbyte inside = TT_FALSE, orientation = o->orientation == ttanyscroll_orientation_x;

    i = o->Class->CacheThemeShape(o);
    if (i == (ttuint)-1)
	return state;

    if (orientation) {
	p = x;    P = y;
	s = o->w; S = o->h;
    } else {
	p = y;    P = x;
	s = o->h; S = o->w;
    }
    
    if (P >= 0 && P < S)
	inside = TT_TRUE;
    
    if (inside) {
	if (p < o->real_position)
	    state = ttslider_state_page_rev;
	else if (p >= o->real_position + o->real_view_size)
	    state = ttslider_state_page_fwd;
	else
	    state = ttslider_state_tab;
    } else
	state = ttslider_state_tab;
	
    *ret_p = p;
    
    return state;
}
static void null_MouseListener_ttslider(ttany arg) {
    ttslider o;
    tteventbig ev;
    tttimer t;
    ttint p, delta = 0, real_delta = 0, position, real_position;
    ttuint old_vflags, vflags, state;
    ttbyte inside, set_state = TT_FALSE, set_timer = TT_FALSE;
    
    /* default mouse handler */
    
    if (!(TTAssert(ev = ID2(tteventbig,arg)) &&
	  TTAssert(ev->evtype == ttevent_evtype_mouse) &&
	  (o = (ttslider)ev->component) &&
	  TTAssert(IS(ttslider,o))))
	
	return;
    
    vflags = old_vflags = o->vflags;
    inside = ev->x >= 0 && ev->y >= 0 && ev->x < o->w && ev->y < o->h;

    state = null_ComputeMouseState_ttslider(o, ev->x, ev->y, &p);
    
    switch (ev->evcode) {
      case ttevent_evcode_mouse_move:
	if (inside)
	    vflags |= ttanybutton_vflags_prelight;
	else
	    vflags &= ~ttanybutton_vflags_prelight;
	break;
      case ttevent_evcode_mouse_press_1:
	vflags &= ~ttanybutton_vflags_prelight;
	vflags |= ttanybutton_vflags_pressed;
	set_state = set_timer = TT_TRUE;
	
	switch (state) {
	  case ttslider_state_page_rev:
	    delta = -1;
	    break;
	  case ttslider_state_page_fwd:
	    delta = 1;
	    break;
	  case ttslider_state_tab:
	    state += p - o->real_position;
	    set_timer = TT_FALSE;
	    break;
	  default:
	    set_timer = TT_FALSE;
	    break;
	}
	break;
      case ttevent_evcode_mouse_move | ttevent_evcode_mouse_hold_1:
	if (o->state >= ttslider_state_tab)
	    real_delta = p - o->real_position - (o->state - ttslider_state_tab);
	break;
      case ttevent_evcode_mouse_release_1:
	vflags &= ~ttanybutton_vflags_pressed;
	if (inside)
	    vflags |= ttanybutton_vflags_prelight;
	state = ttslider_state_normal;
	set_state = TT_TRUE;
	break;
      default:
	break;
    }
    
    if (real_delta) {
	/* be careful with rounding! round position, not delta */
	real_position = o->real_position + real_delta;
	/* round to nearest */
	position = (real_position * o->size + o->real_size/2) / o->real_size;
	delta = position - o->position;
    }
    if (delta) {
	/* could FIRE_EVENT() here, but this is more compact */
	TTSetPosition_ttanyscroll((tt_obj)o->id, o->position + delta);
    }
    if (set_state) {
	o->Class->SetState(o, state);
    }
    if ((t = o->timers) && !!(t->lflags & tttimer_lflags_enabled) != set_timer) {
	if (t->args && t->args->array_n > 1)
	    t->args->array[1] = (ttany)delta;
	
	if (set_timer)
	    TTSetDelay_tttimer((tt_obj)t->id, 0, 200 MilliSECs);
	
	t->Class->SetEnabled(t, set_timer);
    }
    if (vflags != old_vflags) {
	FIRE_EVENT(o->vflags = vflags, o, ttvisible_vflags, vflags, old_vflags);
	
	if (o->parent && (o->vflags & ttvisible_vflags_visible))
	    Expose_ttvisible((ttvisible)o, ttvisible_repaint_args_WHOLE);
    }
}
static ttlistener null_CreateMouseListener_ttslider(ttslider o, ttuint evcode) {
    return CreateE_ttlistener
	((ttcomponent)o, Create_ttevent(ttevent_evtype_mouse, evcode, 0),
	 ttlistener_lflags_arg0_event,
	 (ttlistener_fn)null_MouseListener_ttslider, (ttany)0);
}
static void null_TimerListener_ttslider(ttany arg0, ttany arg1) { 
    tttimer t;
    ttslider o;
    ttint delta;
    
    /* default timer handler */
    if (!((t = ID2(tttimer,arg0)) && TTAssert(t) &&
	  (o = (ttslider)t->component) &&
	  TTAssert(IS(ttslider,o))))
	return;

    if ((delta = (ttint)arg1)) {
	/* could FIRE_EVENT() here, but this is more compact */
	TTSetPosition_ttanyscroll((tt_obj)o->id, o->position + delta);
    }
    /* reactivate the timer itself */
    TTSetDelay_tttimer((tt_obj)t->id, 0, 30 MilliSECs);
}
TT_INLINE tttimer null_CreateTimer_ttslider(ttslider o) {
    return CreateR_tttimer((ttcomponent)o, 0, (ttlistener_fn)null_TimerListener_ttslider,
			   -1, 0, CreateL_ttvector(2, (ttany)0, (ttany)0), 0, 0);
}
static ttslider null_New_ttslider(ttclass_ttslider Class, ttslider o) {
    if ((o = (ttslider)TClass(ttanyscroll)->New((ttclass_ttanyscroll)Class, (ttanyscroll)o))) {
	o->slide_min = o->slide_value = 0;
	o->slide_max = 10;

	if (null_CreateMouseListener_ttslider(o, ttevent_evcode_mouse_move) &&
	    null_CreateMouseListener_ttslider(o, ttevent_evcode_mouse_press_1) &&
	    null_CreateMouseListener_ttslider(o, ttevent_evcode_mouse_move | ttevent_evcode_mouse_hold_1) &&
	    null_CreateMouseListener_ttslider(o, ttevent_evcode_mouse_release_1) &&
	    null_CreateTimer_ttslider(o)) {
	    
	    return o;
	}
	TDEL(o);
    }
    return NULL;
}
static void null_Recalculate_ttslider(ttslider o, ttint size, ttint real_size, ttint view_size, ttint position) {
    tttheme t = myTheme(o);
    ttint real_size_, real_view_size, view_size_;

    if (size < 0) {
	if (o->real_size - o->real_view_size > 0) {
	    size = o->slide_max - o->slide_min;
	    if (size < 0)
		size = -size;
	    size = o->real_size * size / (o->real_size - o->real_view_size);
	} else
	    size = 0;
    }

    real_view_size = size > 0
	? o->orientation == ttanyscroll_orientation_x
	? t->shape[ttslider_shape_normal][1].width
	: t->shape[ttslider_shape_normal][1].height
	: 0;

    real_size_ = real_size >= 0 ? real_size : o->real_size;
    view_size_ = real_size_ > 0 ? real_view_size * size / real_size_ : 0;

    if (size == o->size)
	size = TT_MIN_ttint;

    TClass(ttanyscroll)->Recalculate((ttanyscroll)o, size, real_size, view_size_, position);
}
static ttuint null_CacheThemeShape_ttslider(ttslider o) {
    static TT_CONST ttuint case_flags_x[][3] = {
	{ ttanybutton_vflags_normal,   ttslider_shape_normal,   tttheme_shape_ttslider_x_normal,   },
	{ ttanybutton_vflags_prelight, ttslider_shape_prelight, tttheme_shape_ttslider_x_prelight, },
	{ ttanybutton_vflags_pressed,  ttslider_shape_pressed,  tttheme_shape_ttslider_x_pressed,  },
	{ ttanybutton_vflags_disabled, ttslider_shape_disabled, tttheme_shape_ttslider_x_disabled, },
    };
    static TT_CONST ttuint case_flags_y[][3] = {
	{ ttanybutton_vflags_normal,   ttslider_shape_normal,   tttheme_shape_ttslider_y_normal,   },
	{ ttanybutton_vflags_prelight, ttslider_shape_prelight, tttheme_shape_ttslider_y_prelight, },
	{ ttanybutton_vflags_pressed,  ttslider_shape_pressed,  tttheme_shape_ttslider_y_pressed,  },
	{ ttanybutton_vflags_disabled, ttslider_shape_disabled, tttheme_shape_ttslider_y_disabled, },
    };
    return null_CacheThemeShape_anyscroll
	((ttanyscroll)o, ttanybutton_vflags_statemask,
	 sizeof(case_flags_x)/sizeof(case_flags_x[0]),
	 o->orientation == ttanyscroll_orientation_x ? case_flags_x : case_flags_y);
}
static ttbyte null_SetSize_ttslider(ttslider o, ttint size) {
    /* do-nothing; use SetSlideMin() and SetSlideMax() instead */
    return TT_FALSE;
}
static ttbyte null_SetSlideMin_ttslider(ttslider o, ttint slide_min) {
    REVALIDATE(o->slide_min = slide_min);
    return TT_TRUE;
}
static ttbyte null_SetSlideMax_ttslider(ttslider o, ttint slide_max) {
    REVALIDATE(o->slide_max = slide_max);
    return TT_TRUE;
}
static ttbyte null_SetSlideValue_ttslider(ttslider o, ttint slide_value) {
    ttint low, high, position;
    
    REVALIDATE(
	       if (o->slide_min < o->slide_max) {
		   low = o->slide_min;
		   high = o->slide_max;
	       } else {
		   low = o->slide_max;
		   high = o->slide_min;
	       }
	       o->slide_value = slide_value = BOUND(slide_value, low, high);
	       
	       if ((position = slide_value - o->slide_min) < 0)
	       position = -position;
	       
	       if (position != o->position)
	       FIRE_EVENT(
			  o->Class->Recalculate(o, TT_MIN_ttint, TT_MIN_ttint, TT_MIN_ttint, position),
			  o, ttanyscroll_position, position, o->position
			  )
	       );
    
    return TT_TRUE;
}
static ttbyte null_SetPosition_ttslider(ttslider o, ttint position) {
    ttint sign, slide_value;
    
    REVALIDATE(
	       o->position = position = BOUND(position, 0, o->size);
	       if (o->slide_min < o->slide_max)
	       sign = 1;
	       else
	       sign = -1;
	       slide_value = o->slide_min + sign * position;
	       if (slide_value != o->slide_value)
	       FIRE_EVENT(
			  o->slide_value = slide_value,
			  o, ttslider_slide_value, slide_value, o->slide_value
			  );
	       o->Class->Recalculate(o, TT_MIN_ttint, TT_MIN_ttint, TT_MIN_ttint, position)
	       );

    return TT_TRUE;
}


/* ttprogressbar */
static ttprogressbar null_New_ttprogressbar(ttclass_ttprogressbar Class, ttprogressbar o) {
    if ((o = (ttprogressbar)TClass(ttanyscroll)->New((ttclass_ttanyscroll)Class, (ttanyscroll)o))) {
	o->size = 100;
	o->view_size = o->real_view_size = o->real_position_frac_above = 0;
	o->real_position_frac_below = 1;
    }
    return o;
}
static void null_Recalculate_ttprogressbar(ttprogressbar o, ttint size, ttint real_size, ttint view_size, ttint position) {
    
    TClass(ttanyscroll)->Recalculate((ttanyscroll)o, size, real_size, view_size, position);

    if (o->size > 0) {
	static TT_CONST ttuint case_flags_i[] = {
	    tttheme_shape_ttprogressbar_x_normal,
	    tttheme_shape_ttprogressbar_x_disabled,
	    tttheme_shape_ttprogressbar_y_normal,
	    tttheme_shape_ttprogressbar_y_disabled,
	};
	ttuint theme_i = 
	    (o->orientation == ttanyscroll_orientation_x ? 2 : 0) |
	    ((o->vflags & ttprogressbar_vflags_statemask) == ttprogressbar_vflags_disabled ? 1 : 0);
	
	ttshape shape = & myTheme(o)->shape[case_flags_i[theme_i]][0];
	
	ttuint frac_below = o->real_position_frac_below = 
	    o->orientation == ttanyscroll_orientation_x
	    ? shape->width - shape->border[tt_x_left] - shape->border[tt_x_right]
	    : shape->height- shape->border[tt_y_up] - shape->border[tt_y_down];

	/* round to nearest */
	ttint real_pos = (o->position * o->real_size * frac_below + o->size / 2) / o->size;
	
	o->real_position = real_pos / frac_below;
	o->real_position = BOUND(o->real_position, 0, o->real_size - o->real_view_size);

	o->real_position_frac_above = real_pos % frac_below;
    } else
	o->real_position_frac_above = 0;
}
static void null_AddShapeI_progressbar(ttattr *attr, ttshort pitch, ttshort w, ttshort h,
				       ttattr *tattr, ttshort tw, ttshort th,
				       ttshort b_left, ttshort b_up, ttshort b_right, ttshort b_down,
				       ttprogressbar o) {
    
    ttshort ti, i, tj, j;
    ttint above = o->real_position_frac_above;
    ttshort max = o->real_position_frac_below - 1;
    
    
    if (o->orientation == ttanyscroll_orientation_x) {
	/* set horizontal progressbar inside */
	for (j = tj = b_up; j < h - b_down; j++, tj++) {
	    if (tj >= th - b_down)
		tj = b_up;
	    for (i = b_left; i < w - b_right; i++) {
		ti = i - b_left;
		if (ti < o->real_position)
		    attr[i+j*pitch] = tattr[max + tj*tw];
		else if (ti == o->real_position)
		    attr[i+j*pitch] = tattr[above + tj*tw];
		else
		    attr[i+j*pitch] = tattr[0 + tj*tw];
	    }
	}
    } else {
	/* set vertical progressbar inside */
	for (j = b_up; j < h - b_down; j++) {
	    tj = j - b_up;
	    for (i = ti = b_left; i < w - b_right; i++, ti++) {
		if (ti >= tw - b_right)
		    ti = b_left;
		if (tj < o->real_position)
		    attr[i+j*pitch] = tattr[ti + max*tw];
		else if (tj == o->real_position)
		    attr[i+j*pitch] = tattr[ti + above*tw];
		else
		    attr[i+j*pitch] = tattr[ti + 0*tw];
	    }
	}
    }
}
static ttuint null_CacheThemeShape_ttprogressbar(ttprogressbar o) {
    static TT_CONST ttuint case_flags_x[][3] = {
	{ ttprogressbar_vflags_normal,   ttprogressbar_shape_normal,   tttheme_shape_ttprogressbar_x_normal,   },
	{ ttprogressbar_vflags_disabled, ttprogressbar_shape_disabled, tttheme_shape_ttprogressbar_x_disabled, },
    };
    static TT_CONST ttuint case_flags_y[][3] = {
	{ ttprogressbar_vflags_normal,   ttprogressbar_shape_normal,   tttheme_shape_ttprogressbar_y_normal,   },
	{ ttprogressbar_vflags_disabled, ttprogressbar_shape_disabled, tttheme_shape_ttprogressbar_y_disabled, },
    };

    /* this is almost equal to null_CacheThemeShape_anyscroll(), except where noted below */
    
    ttuint statemask = ttprogressbar_vflags_statemask;
    ttuint case_n = sizeof(case_flags_x)/sizeof(case_flags_x[0]);
    TT_CONST ttuint (*case_flags)[3] = o->orientation == ttanyscroll_orientation_x ? case_flags_x : case_flags_y;
    ttuint i, vflags;
    
    vflags = o->vflags & statemask;
    
    for (i = 0; i < case_n; i++) {
	if (case_flags[i][0] == vflags)
	    break;
    }
    if (i >= case_n)
	return (ttuint)-1;
    
    if (o->theme_shape[case_flags[i][1]].attr)
	return case_flags[i][1];

    o->Class->Recalculate(o, TT_MIN_ttint, TT_MIN_ttint, TT_MIN_ttint, TT_MIN_ttint);

    {
	ttbyte x_or_y = o->orientation == ttanyscroll_orientation_x;
	ttbyte scroll_i = i;
	ttbyte theme_i = case_flags[i][2];
	tttheme t = myTheme(o);
	ttshape t_shape = &t->shape[theme_i][0], s_shape =  &t->shape[theme_i][1];
	ttattr *shape;
	ttshort w, h;
	ttshort b_left = t_shape->border[tt_x_left], b_up = t_shape->border[tt_y_up];
	ttshort b_right = t_shape->border[tt_x_right], b_down = t_shape->border[tt_y_down];
	ttint real_pos = o->real_position;
	
	if (x_or_y)
	    w = o->real_size, h = t_shape->height;
	else
	    w = t_shape->width, h = o->real_size;
	    
	if ((shape = (ttattr *)TTAllocMem((w += b_left+b_right) * (h += b_up+b_down) * sizeof(ttattr) ))) {
	    
	    TTWriteMem(shape, '\0', w * h * sizeof(ttattr));
	    
	    o->theme_shape[scroll_i].width = w;
	    o->theme_shape[scroll_i].height = h;
	    TTCopyMem(t_shape->border, o->theme_shape[scroll_i].border, 4*sizeof(ttshort));
	    
	    /* this differs from null_CacheThemeShape_anyscroll() : */
#if 1
	    null_AddShapeN(shape, w, w, h, t_shape->attr, t_shape->width, t_shape->height,
			   b_left, b_up, b_right, b_down);
	    null_AddShapeI_progressbar(shape, w, w, h, t_shape->attr, t_shape->width, t_shape->height,
				       b_left, b_up, b_right, b_down, o);
#else
	    null_AddShapeI(shape, w, w, h, t_shape->attr, t_shape->width, t_shape->height,
			   b_left, b_up, b_right, b_down);
#endif
	    
	    if (x_or_y) {
		null_AddShapeI(shape + real_pos, w, o->real_view_size, h,
			       s_shape->attr, s_shape->width, s_shape->height,
			       s_shape->border[tt_x_left], s_shape->border[tt_y_up],
			       s_shape->border[tt_x_right], s_shape->border[tt_y_down]);
	    } else {
		null_AddShapeI(shape + w * real_pos, w, w, o->real_view_size,
			       s_shape->attr, s_shape->width, s_shape->height,
			       s_shape->border[tt_x_left], s_shape->border[tt_y_up],
			       s_shape->border[tt_x_right], s_shape->border[tt_y_down]);
	    }

	    o->theme_shape[case_flags[i][1]].attr = shape;
	    
	    if (o->w != w || o->h != h)
		/* could FIRE_EVENT() here, but this is more compact */
		TTSetWH_ttwidget((tt_obj)o->id, w, h);

	    return case_flags[i][1];
	}

    }
    return (ttuint)-1;    
}

/* ttscrollpane */
static ttscrollpane null_New_ttscrollpane(ttclass_ttscrollpane Class, ttscrollpane o) {
    ttscrollbar sx, sy;
    
    if ((o = (ttscrollpane)TClass(ttwidget)->New((ttclass_ttwidget)Class, (ttwidget)o))) {
	if ((sx = Create1_ttscrollbar(ttanyscroll_orientation_x)) &&
	    (sy = Create1_ttscrollbar(ttanyscroll_orientation_y))) {
	    
	    o->Class->SetBarX(o, sx);
	    o->Class->SetBarY(o, sy);
	    
	    return o;
	}
	TDEL(o);
    }
    return (ttscrollpane)0;
}
static void null_Del_ttscrollpane(ttscrollpane o) {
    if (o) {
	if (o->bar_x) {
	    TDEL(o->bar_x);
	    o->bar_x = NULL;
	}
	if (o->bar_y) {
	    TDEL(o->bar_y);
	    o->bar_y = NULL;
	}
	TClass(ttwidget)->Del((ttwidget)o);
    }
}
static ttbyte null_SetWH_ttscrollpane(ttscrollpane o, ttshort w, ttshort h) {
    ttshort ow = o->w, oh = o->h;
    ttscrollbar sx = o->bar_x, sy = o->bar_y;
    
    
    TClass(ttwidget)->SetWH((ttwidget)o, w, h);
    
    w = o->w; h = o->h;
    
    /* a scrollpane must have logic size at least equal to real size */
    /* and must keep in sync with its scrollbars */
    
    /*
     * must call these FIRST, or scrollbars size will be wrong and will
     * incorrectly bound also their view_size
     */
    if (o->wl < w)
	TTSetWl_ttwidget((tt_obj)o->id, w);
    if (o->hl < h)
	TTSetHl_ttwidget((tt_obj)o->id, h);
    
    if (w != ow) {
	if (sx) {
	    TTSetRealSize_ttanyscroll((tt_obj)sx->id, w - 2);
	    TTSetViewSize_ttanyscroll((tt_obj)sx->id, w);
	}
	if (sy)
	    TTSetXY_ttwidget((tt_obj)sy->id, w, 0);
    }
    if (h != oh) {
	if (sy) {
	    TTSetRealSize_ttanyscroll((tt_obj)sy->id, h - 2);
	    TTSetViewSize_ttanyscroll((tt_obj)sy->id, h);
	}
	if (sx)
	    TTSetXY_ttwidget((tt_obj)sx->id, 0, h);
    }
    return TT_TRUE;
}
static ttbyte null_SetXl_ttscrollpane(ttscrollpane o, ttint xl) {
    ttint oxl = o->xl;
    
    TClass(ttwidget)->SetXl((ttwidget)o, xl);
    
    xl = o->xl;
    
    /* a scrollpane must keep in sync with its scrollbars */
    if (xl != oxl && o->bar_x)
	TTSetPosition_ttanyscroll((tt_obj)o->bar_x->id, xl);
    
    return TT_TRUE;
}
static ttbyte null_SetYl_ttscrollpane(ttscrollpane o, ttint yl) {
    ttint oyl = o->yl;
    
    TClass(ttwidget)->SetYl((ttwidget)o, yl);
    
    yl = o->yl;
    
    /* a scrollpane must keep in sync with its scrollbars */
    if (yl != oyl && o->bar_y)
	TTSetPosition_ttanyscroll((tt_obj)o->bar_y->id, yl);

    return TT_TRUE;
}
static ttbyte null_SetWl_ttscrollpane(ttscrollpane o, ttint wl) {
    ttint owl = o->wl;
    
    TClass(ttwidget)->SetWl((ttwidget)o, wl);
    
    wl = o->wl;
    
    /* a scrollpane must keep in sync with its scrollbars */
    if (wl != owl && o->bar_x)
	TTSetSize_ttanyscroll((tt_obj)o->bar_x->id, wl);

    return TT_TRUE;
}
static ttbyte null_SetHl_ttscrollpane(ttscrollpane o, ttint hl) {
    ttint ohl = o->hl;
    
    TClass(ttwidget)->SetHl((ttwidget)o, hl);
    
    hl = o->hl;
    
    /* a scrollpane must keep in sync with its scrollbars */
    if (hl != ohl && o->bar_y)
	TTSetSize_ttanyscroll((tt_obj)o->bar_y->id, hl);

    return TT_TRUE;
}
static void null_AddTo_ttscrollpane(ttscrollpane o, ttvisible parent, ttany constraint) {
    ttscrollbar sx, sy;
    
    TClass(ttwidget)->AddTo((ttwidget)o, parent, constraint);
    
    if ((sx = o->bar_x))
	TTAdd_ttvisible((tt_obj)parent->id, (tt_obj)sx->id, ttborderlayout_constraint_south);
    if ((sy = o->bar_y))
	TTAdd_ttvisible((tt_obj)parent->id, (tt_obj)sy->id, ttborderlayout_constraint_east);
}
static void null_Remove_ttscrollpane(ttscrollpane o) {
    ttscrollbar sx, sy;
    
    TClass(ttwidget)->Remove((ttwidget)o);
    
    if ((sx = o->bar_x))
	TTRemove_ttvisible((tt_obj)sx->id);
    if ((sy = o->bar_y))
	TTRemove_ttvisible((tt_obj)sy->id);
}
    
    
/* FIXME: scrollpanes must listen for children add/remove and recalculate wl,hl */

static ttbyte null_SetBarX_ttscrollpane(ttscrollpane o, ttscrollbar b) {
    ttscrollbar ob = o->bar_x;
    
    if (b != ob && (!b || (b->orientation == ttanyscroll_orientation_x))) {

	if (ob)
	    ob->scrollpane = (ttscrollpane)0;
	    
	if (b) {
	    if (b->scrollpane)
		TTSetBarX_ttscrollpane((tt_obj)b->scrollpane->id, NULL);
	    b->scrollpane = o;
	}

	FIRE_EVENT(o->bar_x = b, o, ttscrollpane_bar_x, (ttany)(opaque)b, (ttany)(opaque)ob);
	
	if (b) {
	    /* FIXME: we should fire events too */
	    if (b->parent != o->parent) {
		b->Class->Remove(b);
		b->Class->AddTo(b, o->parent, ttborderlayout_constraint_east);
	    }
	    b->Class->SetXY(b, 0, o->h);
	    b->Class->Recalculate(b, o->wl, TT_MIN_ttint, o->w, o->xl);
	}
    }
    return TT_TRUE;
}
static ttbyte null_SetBarY_ttscrollpane(ttscrollpane o, ttscrollbar b) {
    ttscrollbar ob = o->bar_y;
    
    if (b != ob && (!b || (b->orientation == ttanyscroll_orientation_y))) {

	if (ob)
	    ob->scrollpane = (ttscrollpane)0;
	    
	if (b) {
	    if (b->scrollpane)
		TTSetBarY_ttscrollpane((tt_obj)b->scrollpane->id, NULL);
	    b->scrollpane = o;
	}

	FIRE_EVENT(o->bar_y = b, o, ttscrollpane_bar_y, (ttany)(opaque)b, (ttany)(opaque)ob);
	
	if (b) {
	    /* FIXME: we should fire events too */
	    if (b->parent != o->parent) {
		b->Class->Remove(b);
		b->Class->AddTo(b, o->parent, ttborderlayout_constraint_south);
	    }
	    b->Class->SetXY(b, o->w, 0);
	    b->Class->Recalculate(b, o->hl, TT_MIN_ttint, o->h, o->yl);
	}
    }
    return TT_TRUE;
}

/* ttscroller */
static ttscroller null_New_ttscroller(ttclass_ttscroller Class, ttscroller o) {
    ttscrollpane sp;
    
    if ((o = (ttscroller)TClass(ttwindow)->New((ttclass_ttwindow)Class, (ttwindow)o))) {
	o->Class->SetLayout(o, (ttlayout)TNEW(ttborderlayout));
	
	if (o->layout && (o->scrollpane = sp = Create_ttscrollpane(o->w - 1, o->h - 1))) {
	    
	    sp->Class->AddTo(sp, (ttvisible)o, ttborderlayout_constraint_center);

	    return o;
	}
	TDEL(o);
    }
    return NULL;
}
static void null_Del_ttscroller(ttscroller o) {
    if (o) {
	if (o->scrollpane) {
	    TDEL(o->scrollpane);
	    o->scrollpane = (ttscrollpane)0;
	}
	TClass(ttwindow)->Del((ttwindow)o);
    }
}
static ttbyte null_SetWH_ttscroller(ttscroller o, ttshort w, ttshort h) {
    ttscrollpane sp;
    ttshort ow = o->w, oh = o->h;
    
    TClass(ttwindow)->SetWH((ttwindow)o, w, h);
    
    w = o->w; h = o->h;
    
    /* a scroller must keep its scrollpane in sync */
    if (w != ow || h != oh) {
	if ((sp = o->scrollpane))
	    TTSetWH_ttwidget((tt_obj)sp->id, w - 1, h - 1);
    }
    return TT_TRUE;
}

/* ttwindow */
static ttwindow null_New_ttwindow(ttclass_ttwindow Class, ttwindow o) {
    if ((o = (ttwindow)TClass(ttwidget)->New((ttclass_ttwidget)Class, (ttwidget)o))) {
	o->title_len = 0;
	o->title = NULL;
    }
    return o;
}
static void null_Del_ttwindow(ttwindow o) {
    if (o) {
	if (o->title) {
	    TTFreeMem(o->title);
	    o->title = NULL;
	}
	TClass(ttwidget)->Del((ttwidget)o);
    }
}
static ttbyte null_SetTitle_ttwindow(ttwindow o, TT_ARG_READ ttbyte *title) {
    ttbyte *_title = NULL;
    ttopaque title_len;
    
    if (!title || ((title_len = TTLenStr(title)),
		   (_title = TTCloneStrL(title, title_len + 1)))) {
	if (o->title)
	    TTFreeMem(o->title);
	o->title = _title;
	o->title_len = title_len;
	
	return TT_TRUE;
    }
    return TT_FALSE;
}

/* ttframe */
static ttframe null_New_ttframe(ttclass_ttframe Class, ttframe o) {
    if ((o = (ttframe)TClass(ttwindow)->New((ttclass_ttwindow)Class, (ttwindow)o))) {
	o->menubar = NULL;
    }
    return o;
}
static ttframe null_Build_ttframe(ttframe o) {
    /* ttframe are non-visible when created */
    o->vflags &= ~ttvisible_vflags_visible;
    o->Class->AddTo(o, (ttvisible)TClass_ttnative->GetRoot(), ttlayout_constraint_root);
    return o;
}
static ttbyte null_SetMenubar_ttframe(ttframe o, ttmenubar m) {
    o->menubar = m;
    return TT_TRUE;
}

/* ttmenuitem */


/* ttcheckmenuitem */


/* ttradiomenuitem */


/* ttmenuwindow */


/* ttmenu */

static ttmenu null_New_ttmenu(ttclass_ttmenu Class, ttmenu o) {
    if ((o = (ttmenu)TClass(ttmenuitem)->New((ttclass_ttmenuitem)Class, (ttmenuitem)o))) {
	o->menubar = (ttmenubar)0;
    }
    return o;
}

/* ttmenubar */

static void null_Del_ttmenubar(ttmenubar o) {
    if (o) {
	if (TTD.Menubar == o)
	    TTD.Menubar = (ttmenubar)0;
	TClass(ttvisible)->Del((ttvisible)o);
    }
}

/* ttanytext */

static ttanytext null_New_ttanytext(ttclass_ttanytext Class, ttanytext o) {
    if ((o = (ttanytext)TClass(ttwindow)->New((ttclass_ttwindow)Class, (ttwindow)o))) {
	o->text = NULL;
	o->text_len = 0;
    }
    return o;
}


/* tttextfield */


/* tttextarea */


/* tttheme */

static tttheme null_Build_tttheme(tttheme o) {
    /* FIXME finish this */
    return o;
}
static void null_Del_tttheme(tttheme o) {
    if (o) {
	if (TTD.Theme == o)
	    TTD.Theme = (tttheme)0;
	TClass(ttcomponent)->Del((ttcomponent)o);
    }
}


/* ttapplication */
static ttapplication null_New_ttapplication(ttclass_ttapplication Class, ttapplication o) {
    if ((o = (ttapplication)TClass(ttcomponent)->New((ttclass_ttcomponent)Class, (ttcomponent)o))) {
	o->name = (ttbyte *)0;
    }
    return o;
}

static void null_Del_ttapplication(ttapplication o) {
    if (o) {
	if (TTD.Application == o)
	    TTD.Application = (ttapplication)0;
	if (o->name) {
	    TTFreeMem(o->name);
	    o->name = (ttbyte *)0;
	}
	TClass(ttcomponent)->Del((ttcomponent)o);
    }
}


/* common stuff: */


static byte null_Sync(void) {
    return TT_TRUE;
}
static byte null_Flush(void) {
    return TT_TRUE;
}
static byte null_TimidFlush(void) {
    return TT_TRUE;
}
static byte null_MainLoopOnce(ttbyte wait) {
    return TT_TRUE;
}
static void null_DeleteCallback(ttcallback o) {
}
static void null_Close(void) {
}
static int null_ConnectionFd(void) {
    return TT_NOFD;
}
static ttuint null_GetErrno(void) {
    return 0;
}
static ttuint null_GetErrnoDetail(void) {
    return 0;
}
static TT_CONST byte *null_StrError(ttuint E) {
    return "";
}
static TT_CONST byte *null_StrErrorDetail(ttuint E, ttuint S) {
    return "";
}


ttclasses _TT_null_InitHW(tthw *HW) {
    *HW = &null_TTClasses.HW;
    return &null_TTClasses;
}






/*
 * use default values for methods not implemented in null_c
 *
 * null display target MUST IMPLEMENT ALL METHODS, even if as stubs.
 */











static s_ttclasses null_TTClasses = {

 
  {
    Class_ttobj,
    id_ttobj,
    magicmask_ttobj,
    magic_ttobj,
    size_ttobj,

  },
 
  {
    Class_ttclass,
    id_ttclass,
    magicmask_ttclass,
    magic_ttclass,
    size_ttclass,

    /* extends ttclass_ttobj */

  },
 
  {
    Class_ttobject,
    id_ttobject,
    magicmask_ttobject,
    magic_ttobject,
    size_ttobject,

    /* extends ttclass_ttobj */
    null_New_ttobject,
    null_Build_ttobject,
    null_Break_ttobject,
    null_Del_ttobject,

  },
 
  {
    Class_ttvector,
    id_ttvector,
    magicmask_ttvector,
    magic_ttvector,
    size_ttvector,

    /* extends ttclass_ttobj */
    null_New_ttvector,
    (void *)NULL /* inherits:  Build_ttvector >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttvector >> Break_ttobject */,
    null_Del_ttvector,

    /* extends ttclass_ttobject */
    null_GetAt_ttvector,
    null_SetAt_ttvector,
    null_AddY_ttvector,
    null_ContainsValue_ttvector,
    null_RemoveRange_ttvector,

  },
 
  {
    Class_ttfield,
    id_ttfield,
    magicmask_ttfield,
    magic_ttfield,
    size_ttfield,

    /* extends ttclass_ttobj */

  },
 
  {
    Class_ttmethod,
    id_ttmethod,
    magicmask_ttmethod,
    magic_ttmethod,
    size_ttmethod,

    /* extends ttclass_ttobj */

  },
 
  {
    Class_ttevent,
    id_ttevent,
    magicmask_ttevent,
    magic_ttevent,
    size_ttevent,

    /* extends ttclass_ttobj */
    null_New_ttevent,
    (void *)NULL /* inherits:  Build_ttevent >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttevent >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttevent >> Del_ttobject */,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_tteventbig,
    id_tteventbig,
    magicmask_tteventbig,
    magic_tteventbig,
    size_tteventbig,

    /* extends ttclass_ttobj */
    null_New_tteventbig,
    (void *)NULL /* inherits:  Build_tteventbig >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_tteventbig >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_tteventbig >> Del_ttobject */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttevent */

  },
 
  {
    Class_ttbitmask,
    id_ttbitmask,
    magicmask_ttbitmask,
    magic_ttbitmask,
    size_ttbitmask,

    /* extends ttclass_ttobj */
    null_New_ttbitmask,
    (void *)NULL /* inherits:  Build_ttbitmask >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttbitmask >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttbitmask >> Del_ttobject */,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_tteventmask,
    id_tteventmask,
    magicmask_tteventmask,
    magic_tteventmask,
    size_tteventmask,

    /* extends ttclass_ttobj */
    null_New_tteventmask,
    (void *)NULL /* inherits:  Build_tteventmask >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_tteventmask >> Break_ttobject */,
    null_Del_tteventmask,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_ttcallback,
    id_ttcallback,
    magicmask_ttcallback,
    magic_ttcallback,
    size_ttcallback,

    /* extends ttclass_ttobj */
    null_New_ttcallback,
    (void *)NULL /* inherits:  Build_ttcallback >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttcallback >> Break_ttobject */,
    null_Del_ttcallback,

    /* extends ttclass_ttobject */
    null_Create_ttcallback,

  },
 
  {
    Class_ttlistener,
    id_ttlistener,
    magicmask_ttlistener,
    magic_ttlistener,
    size_ttlistener,

    /* extends ttclass_ttobj */
    null_New_ttlistener,
    (void *)NULL /* inherits:  Build_ttlistener >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttlistener >> Break_ttobject */,
    null_Del_ttlistener,

    /* extends ttclass_ttobject */
    (void *)NULL /* inherits:  Create_ttlistener >> Create_ttcallback */,

    /* extends ttclass_ttcallback */
    null_AddTo_ttlistener,
    null_Remove_ttlistener,

  },
 
  {
    Class_tttimer,
    id_tttimer,
    magicmask_tttimer,
    magic_tttimer,
    size_tttimer,

    /* extends ttclass_ttobj */
    null_New_tttimer,
    null_Build_tttimer,
    null_Break_tttimer,
    null_Del_tttimer,

    /* extends ttclass_ttobject */
    (void *)NULL /* inherits:  Create_tttimer >> Create_ttcallback */,

    /* extends ttclass_ttcallback */
    null_AddTo_tttimer,
    null_Remove_tttimer,

    /* extends ttclass_ttlistener */
    null_SetEnabled_tttimer,
    null_IsEnabled_tttimer,

  },
 
  {
    Class_ttcomponent,
    id_ttcomponent,
    magicmask_ttcomponent,
    magic_ttcomponent,
    size_ttcomponent,

    /* extends ttclass_ttobj */
    null_New_ttcomponent,
    (void *)NULL /* inherits:  Build_ttcomponent >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttcomponent >> Break_ttobject */,
    null_Del_ttcomponent,

    /* extends ttclass_ttobject */

  },
 
  {
    Class_ttdata,
    id_ttdata,
    magicmask_ttdata,
    magic_ttdata,
    size_ttdata,

    /* extends ttclass_ttobj */
    null_New_ttdata,
    (void *)NULL /* inherits:  Build_ttdata >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttdata >> Break_ttobject */,
    null_Del_ttdata,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    null_AddTo_ttdata,
    null_Remove_ttdata,

  },
 
  {
    Class_ttvisible,
    id_ttvisible,
    magicmask_ttvisible,
    magic_ttvisible,
    size_ttvisible,

    /* extends ttclass_ttobj */
    null_New_ttvisible,
    (void *)NULL /* inherits:  Build_ttvisible >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttvisible >> Break_ttobject */,
    null_Del_ttvisible,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    null_AddTo_ttvisible,
    null_Validate_ttvisible,
    null_Remove_ttvisible,
    null_Invalidate_ttvisible,
    null_SetVisible_ttvisible,
    null_SetTheme_ttvisible,
    null_Draw_ttvisible,
    null_BuiltinRepaint_ttvisible,


  },
 
  {
    Class_ttlayout,
    id_ttlayout,
    magicmask_ttlayout,
    magic_ttlayout,
    size_ttlayout,

    /* extends ttclass_ttobj */
    null_New_ttlayout,
    (void *)NULL /* inherits:  Build_ttlayout >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttlayout >> Break_ttobject */,
    null_Del_ttlayout,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    null_SetWidget_ttlayout,
    null_DoLayout_ttlayout,

  },
 
  {
    Class_ttboxlayout,
    id_ttboxlayout,
    magicmask_ttboxlayout,
    magic_ttboxlayout,
    size_ttboxlayout,

    /* extends ttclass_ttobj */
    null_New_ttboxlayout,
    (void *)NULL /* inherits:  Build_ttboxlayout >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttboxlayout >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttboxlayout >> Del_ttlayout */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  SetWidget_ttboxlayout >> SetWidget_ttlayout */,
    (void *)NULL /* inherits:  DoLayout_ttboxlayout >> DoLayout_ttlayout */,

    /* extends ttclass_ttlayout */

  },
 
  {
    Class_ttborderlayout,
    id_ttborderlayout,
    magicmask_ttborderlayout,
    magic_ttborderlayout,
    size_ttborderlayout,

    /* extends ttclass_ttobj */
    (void *)NULL /* inherits:  New_ttborderlayout >> New_ttlayout */,
    (void *)NULL /* inherits:  Build_ttborderlayout >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttborderlayout >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttborderlayout >> Del_ttlayout */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  SetWidget_ttborderlayout >> SetWidget_ttlayout */,
    (void *)NULL /* inherits:  DoLayout_ttborderlayout >> DoLayout_ttlayout */,

    /* extends ttclass_ttlayout */

  },
 
  {
    Class_ttnative,
    id_ttnative,
    magicmask_ttnative,
    magic_ttnative,
    size_ttnative,

    /* extends ttclass_ttobj */
    (void *)NULL /* inherits:  New_ttnative >> New_ttvisible */,
    (void *)NULL /* inherits:  Build_ttnative >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttnative >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttnative >> Del_ttvisible */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttnative >> AddTo_ttvisible */,
    (void *)NULL /* inherits:  Validate_ttnative >> Validate_ttvisible */,
    (void *)NULL /* inherits:  Remove_ttnative >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttnative >> Invalidate_ttvisible */,
    (void *)NULL /* inherits:  SetVisible_ttnative >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttnative >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttnative >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttnative >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    null_GetRoot_ttnative,
    null_GetW_ttnative,
    null_GetH_ttnative,

  },
 
  {
    Class_ttwidget,
    id_ttwidget,
    magicmask_ttwidget,
    magic_ttwidget,
    size_ttwidget,

    /* extends ttclass_ttobj */
    null_New_ttwidget,
    (void *)NULL /* inherits:  Build_ttwidget >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttwidget >> Break_ttobject */,
    null_Del_ttwidget,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    null_AddTo_ttwidget,
    null_Validate_ttwidget,
    (void *)NULL /* inherits:  Remove_ttwidget >> Remove_ttvisible */,
    null_Invalidate_ttwidget,
    (void *)NULL /* inherits:  SetVisible_ttwidget >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttwidget >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttwidget >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttwidget >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    null_SetLayout_ttwidget,
    null_SetXY_ttwidget,
    null_SetWH_ttwidget,
    null_SetXl_ttwidget,
    null_SetYl_ttwidget,
    null_SetWl_ttwidget,
    null_SetHl_ttwidget,
    null_SetTooltip_ttwidget,

  },
 
  {
    Class_ttlabel,
    id_ttlabel,
    magicmask_ttlabel,
    magic_ttlabel,
    size_ttlabel,

    /* extends ttclass_ttobj */
    null_New_ttlabel,
    (void *)NULL /* inherits:  Build_ttlabel >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttlabel >> Break_ttobject */,
    null_Del_ttlabel,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttlabel >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttlabel >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttlabel >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttlabel >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttlabel >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttlabel >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttlabel >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttlabel >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttlabel >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttlabel >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttlabel >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttlabel >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttlabel >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttlabel >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttlabel >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttlabel >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    null_SetFontD_ttlabel,

  },
 
  {
    Class_tttooltip,
    id_tttooltip,
    magicmask_tttooltip,
    magic_tttooltip,
    size_tttooltip,

    /* extends ttclass_ttobj */
    null_New_tttooltip,
    (void *)NULL /* inherits:  Build_tttooltip >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_tttooltip >> Break_ttobject */,
    null_Del_tttooltip,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttooltip >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_tttooltip >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_tttooltip >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_tttooltip >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_tttooltip >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_tttooltip >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_tttooltip >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_tttooltip >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_tttooltip >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_tttooltip >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_tttooltip >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_tttooltip >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_tttooltip >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_tttooltip >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_tttooltip >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_tttooltip >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* inherits:  SetFontD_tttooltip >> SetFontD_ttlabel */,

    /* extends ttclass_ttlabel */
    null_SetWidget_tttooltip,

  },
 
  {
    Class_ttbuttongroup,
    id_ttbuttongroup,
    magicmask_ttbuttongroup,
    magic_ttbuttongroup,
    size_ttbuttongroup,

    /* extends ttclass_ttobj */
    null_New_ttbuttongroup,
    (void *)NULL /* inherits:  Build_ttbuttongroup >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttbuttongroup >> Break_ttobject */,
    null_Del_ttbuttongroup,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    null_Add_ttbuttongroup,
    null_Remove_ttbuttongroup,
    null_SetChecked_ttbuttongroup,

  },
 
  {
    Class_ttanybutton,
    id_ttanybutton,
    magicmask_ttanybutton,
    magic_ttanybutton,
    size_ttanybutton,

    /* extends ttclass_ttobj */
    null_New_ttanybutton,
    (void *)NULL /* inherits:  Build_ttanybutton >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttanybutton >> Break_ttobject */,
    null_Del_ttanybutton,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanybutton >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttanybutton >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttanybutton >> Remove_ttvisible */,
    null_Invalidate_ttanybutton,
    (void *)NULL /* inherits:  SetVisible_ttanybutton >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttanybutton >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttanybutton >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttanybutton >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttanybutton >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttanybutton >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttanybutton >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttanybutton >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttanybutton >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttanybutton >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttanybutton >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttanybutton >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    null_CacheThemeShape_ttanybutton,
    null_SetAttrD_ttanybutton,

  },
 
  {
    Class_ttbutton,
    id_ttbutton,
    magicmask_ttbutton,
    magic_ttbutton,
    size_ttbutton,

    /* extends ttclass_ttobj */
    null_New_ttbutton,
    (void *)NULL /* inherits:  Build_ttbutton >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttbutton >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttbutton >> Del_ttanybutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttbutton >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttbutton >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttbutton >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttbutton >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttbutton >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttbutton >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttbutton >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttbutton >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttbutton >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttbutton >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttbutton >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttbutton >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttbutton >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttbutton >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttbutton >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttbutton >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    null_CacheThemeShape_ttbutton,
    (void *)NULL /* inherits:  SetAttrD_ttbutton >> SetAttrD_ttanybutton */,

    /* extends ttclass_ttanybutton */
    null_SetPressed_ttbutton,

  },
 
  {
    Class_ttcheckbutton,
    id_ttcheckbutton,
    magicmask_ttcheckbutton,
    magic_ttcheckbutton,
    size_ttcheckbutton,

    /* extends ttclass_ttobj */
    null_New_ttcheckbutton,
    (void *)NULL /* inherits:  Build_ttcheckbutton >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttcheckbutton >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttcheckbutton >> Del_ttanybutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttcheckbutton >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttcheckbutton >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttcheckbutton >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttcheckbutton >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttcheckbutton >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttcheckbutton >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttcheckbutton >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttcheckbutton >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttcheckbutton >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttcheckbutton >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttcheckbutton >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttcheckbutton >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttcheckbutton >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttcheckbutton >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttcheckbutton >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttcheckbutton >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    null_CacheThemeShape_ttcheckbutton,
    (void *)NULL /* inherits:  SetAttrD_ttcheckbutton >> SetAttrD_ttanybutton */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* inherits:  SetPressed_ttcheckbutton >> SetPressed_ttbutton */,

    /* extends ttclass_ttbutton */
    null_SetChecked_ttcheckbutton,

  },
 
  {
    Class_ttradiobutton,
    id_ttradiobutton,
    magicmask_ttradiobutton,
    magic_ttradiobutton,
    size_ttradiobutton,

    /* extends ttclass_ttobj */
    null_New_ttradiobutton,
    (void *)NULL /* inherits:  Build_ttradiobutton >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttradiobutton >> Break_ttobject */,
    null_Del_ttradiobutton,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttradiobutton >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttradiobutton >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttradiobutton >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttradiobutton >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttradiobutton >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttradiobutton >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttradiobutton >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttradiobutton >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttradiobutton >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttradiobutton >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttradiobutton >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttradiobutton >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttradiobutton >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttradiobutton >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttradiobutton >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttradiobutton >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    null_CacheThemeShape_ttradiobutton,
    (void *)NULL /* inherits:  SetAttrD_ttradiobutton >> SetAttrD_ttanybutton */,

    /* extends ttclass_ttanybutton */
    (void *)NULL /* inherits:  SetPressed_ttradiobutton >> SetPressed_ttbutton */,

    /* extends ttclass_ttbutton */
    null_SetChecked_ttradiobutton,

    /* extends ttclass_ttcheckbutton */

  },
 
  {
    Class_ttanyscroll,
    id_ttanyscroll,
    magicmask_ttanyscroll,
    magic_ttanyscroll,
    size_ttanyscroll,

    /* extends ttclass_ttobj */
    null_New_ttanyscroll,
    (void *)NULL /* inherits:  Build_ttanyscroll >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttanyscroll >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttanyscroll >> Del_ttanybutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanyscroll >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttanyscroll >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttanyscroll >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttanyscroll >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttanyscroll >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttanyscroll >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttanyscroll >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttanyscroll >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttanyscroll >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttanyscroll >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttanyscroll >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttanyscroll >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttanyscroll >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttanyscroll >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttanyscroll >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttanyscroll >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* inherits:  CacheThemeShape_ttanyscroll >> CacheThemeShape_ttanybutton */,
    (void *)NULL /* inherits:  SetAttrD_ttanyscroll >> SetAttrD_ttanybutton */,

    /* extends ttclass_ttanybutton */
    null_Recalculate_ttanyscroll,
    null_SetState_ttanyscroll,
    null_SetOrientation_ttanyscroll,
    null_SetSize_ttanyscroll,
    null_SetRealSize_ttanyscroll,
    null_SetViewSize_ttanyscroll,
    null_SetPosition_ttanyscroll,

  },
 
  {
    Class_ttscrollbar,
    id_ttscrollbar,
    magicmask_ttscrollbar,
    magic_ttscrollbar,
    size_ttscrollbar,

    /* extends ttclass_ttobj */
    null_New_ttscrollbar,
    (void *)NULL /* inherits:  Build_ttscrollbar >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttscrollbar >> Break_ttobject */,
    null_Del_ttscrollbar,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscrollbar >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttscrollbar >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttscrollbar >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttscrollbar >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttscrollbar >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttscrollbar >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttscrollbar >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttscrollbar >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttscrollbar >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttscrollbar >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttscrollbar >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttscrollbar >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttscrollbar >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttscrollbar >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttscrollbar >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttscrollbar >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    null_CacheThemeShape_ttscrollbar,
    (void *)NULL /* inherits:  SetAttrD_ttscrollbar >> SetAttrD_ttanybutton */,

    /* extends ttclass_ttanybutton */
    null_Recalculate_ttscrollbar,
    (void *)NULL /* inherits:  SetState_ttscrollbar >> SetState_ttanyscroll */,
    (void *)NULL /* inherits:  SetOrientation_ttscrollbar >> SetOrientation_ttanyscroll */,
    (void *)NULL /* inherits:  SetSize_ttscrollbar >> SetSize_ttanyscroll */,
    (void *)NULL /* inherits:  SetRealSize_ttscrollbar >> SetRealSize_ttanyscroll */,
    (void *)NULL /* inherits:  SetViewSize_ttscrollbar >> SetViewSize_ttanyscroll */,
    (void *)NULL /* inherits:  SetPosition_ttscrollbar >> SetPosition_ttanyscroll */,

    /* extends ttclass_ttanyscroll */

  },
 
  {
    Class_ttslider,
    id_ttslider,
    magicmask_ttslider,
    magic_ttslider,
    size_ttslider,

    /* extends ttclass_ttobj */
    null_New_ttslider,
    (void *)NULL /* inherits:  Build_ttslider >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttslider >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttslider >> Del_ttanybutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttslider >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttslider >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttslider >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttslider >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttslider >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttslider >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttslider >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttslider >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttslider >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttslider >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttslider >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttslider >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttslider >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttslider >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttslider >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttslider >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    null_CacheThemeShape_ttslider,
    (void *)NULL /* inherits:  SetAttrD_ttslider >> SetAttrD_ttanybutton */,

    /* extends ttclass_ttanybutton */
    null_Recalculate_ttslider,
    (void *)NULL /* inherits:  SetState_ttslider >> SetState_ttanyscroll */,
    (void *)NULL /* inherits:  SetOrientation_ttslider >> SetOrientation_ttanyscroll */,
    null_SetSize_ttslider,
    (void *)NULL /* inherits:  SetRealSize_ttslider >> SetRealSize_ttanyscroll */,
    (void *)NULL /* inherits:  SetViewSize_ttslider >> SetViewSize_ttanyscroll */,
    null_SetPosition_ttslider,

    /* extends ttclass_ttanyscroll */
    null_SetSlideMin_ttslider,
    null_SetSlideMax_ttslider,
    null_SetSlideValue_ttslider,

  },
 
  {
    Class_ttprogressbar,
    id_ttprogressbar,
    magicmask_ttprogressbar,
    magic_ttprogressbar,
    size_ttprogressbar,

    /* extends ttclass_ttobj */
    null_New_ttprogressbar,
    (void *)NULL /* inherits:  Build_ttprogressbar >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttprogressbar >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttprogressbar >> Del_ttanybutton */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttprogressbar >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttprogressbar >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttprogressbar >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttprogressbar >> Invalidate_ttanybutton */,
    (void *)NULL /* inherits:  SetVisible_ttprogressbar >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttprogressbar >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttprogressbar >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttprogressbar >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttprogressbar >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttprogressbar >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttprogressbar >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttprogressbar >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttprogressbar >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttprogressbar >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttprogressbar >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttprogressbar >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    null_CacheThemeShape_ttprogressbar,
    (void *)NULL /* inherits:  SetAttrD_ttprogressbar >> SetAttrD_ttanybutton */,

    /* extends ttclass_ttanybutton */
    null_Recalculate_ttprogressbar,
    (void *)NULL /* inherits:  SetState_ttprogressbar >> SetState_ttanyscroll */,
    (void *)NULL /* inherits:  SetOrientation_ttprogressbar >> SetOrientation_ttanyscroll */,
    (void *)NULL /* inherits:  SetSize_ttprogressbar >> SetSize_ttanyscroll */,
    (void *)NULL /* inherits:  SetRealSize_ttprogressbar >> SetRealSize_ttanyscroll */,
    (void *)NULL /* inherits:  SetViewSize_ttprogressbar >> SetViewSize_ttanyscroll */,
    (void *)NULL /* inherits:  SetPosition_ttprogressbar >> SetPosition_ttanyscroll */,

    /* extends ttclass_ttanyscroll */

  },
 
  {
    Class_ttscrollpane,
    id_ttscrollpane,
    magicmask_ttscrollpane,
    magic_ttscrollpane,
    size_ttscrollpane,

    /* extends ttclass_ttobj */
    null_New_ttscrollpane,
    (void *)NULL /* inherits:  Build_ttscrollpane >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttscrollpane >> Break_ttobject */,
    null_Del_ttscrollpane,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    null_AddTo_ttscrollpane,
    (void *)NULL /* inherits:  Validate_ttscrollpane >> Validate_ttwidget */,
    null_Remove_ttscrollpane,
    (void *)NULL /* inherits:  Invalidate_ttscrollpane >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttscrollpane >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttscrollpane >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttscrollpane >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttscrollpane >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttscrollpane >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttscrollpane >> SetXY_ttwidget */,
    null_SetWH_ttscrollpane,
    null_SetXl_ttscrollpane,
    null_SetYl_ttscrollpane,
    null_SetWl_ttscrollpane,
    null_SetHl_ttscrollpane,
    (void *)NULL /* inherits:  SetTooltip_ttscrollpane >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    null_SetBarX_ttscrollpane,
    null_SetBarY_ttscrollpane,

  },
 
  {
    Class_ttwindow,
    id_ttwindow,
    magicmask_ttwindow,
    magic_ttwindow,
    size_ttwindow,

    /* extends ttclass_ttobj */
    null_New_ttwindow,
    (void *)NULL /* inherits:  Build_ttwindow >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttwindow >> Break_ttobject */,
    null_Del_ttwindow,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttwindow >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttwindow >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttwindow >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttwindow >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttwindow >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttwindow >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttwindow >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttwindow >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttwindow >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttwindow >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttwindow >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttwindow >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttwindow >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttwindow >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttwindow >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttwindow >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    null_SetTitle_ttwindow,

  },
 
  {
    Class_ttframe,
    id_ttframe,
    magicmask_ttframe,
    magic_ttframe,
    size_ttframe,

    /* extends ttclass_ttobj */
    null_New_ttframe,
    null_Build_ttframe,
    (void *)NULL /* inherits:  Break_ttframe >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttframe >> Del_ttwindow */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttframe >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttframe >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttframe >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttframe >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttframe >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttframe >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttframe >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttframe >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttframe >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttframe >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttframe >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttframe >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttframe >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttframe >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttframe >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttframe >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* inherits:  SetTitle_ttframe >> SetTitle_ttwindow */,

    /* extends ttclass_ttwindow */
    null_SetMenubar_ttframe,

  },
 
  {
    Class_ttscroller,
    id_ttscroller,
    magicmask_ttscroller,
    magic_ttscroller,
    size_ttscroller,

    /* extends ttclass_ttobj */
    null_New_ttscroller,
    (void *)NULL /* inherits:  Build_ttscroller >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttscroller >> Break_ttobject */,
    null_Del_ttscroller,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscroller >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttscroller >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttscroller >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttscroller >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttscroller >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttscroller >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttscroller >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttscroller >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttscroller >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttscroller >> SetXY_ttwidget */,
    null_SetWH_ttscroller,
    (void *)NULL /* inherits:  SetXl_ttscroller >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttscroller >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttscroller >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttscroller >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttscroller >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* inherits:  SetTitle_ttscroller >> SetTitle_ttwindow */,

    /* extends ttclass_ttwindow */

  },
 
  {
    Class_ttmenuitem,
    id_ttmenuitem,
    magicmask_ttmenuitem,
    magic_ttmenuitem,
    size_ttmenuitem,

    /* extends ttclass_ttobj */
    (void *)NULL /* inherits:  New_ttmenuitem >> New_ttvisible */,
    (void *)NULL /* inherits:  Build_ttmenuitem >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttmenuitem >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttmenuitem >> Del_ttvisible */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttmenuitem >> AddTo_ttvisible */,
    (void *)NULL /* inherits:  Validate_ttmenuitem >> Validate_ttvisible */,
    (void *)NULL /* inherits:  Remove_ttmenuitem >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttmenuitem >> Invalidate_ttvisible */,
    (void *)NULL /* inherits:  SetVisible_ttmenuitem >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttmenuitem >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttmenuitem >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttmenuitem >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */

  },
 
  {
    Class_ttcheckmenuitem,
    id_ttcheckmenuitem,
    magicmask_ttcheckmenuitem,
    magic_ttcheckmenuitem,
    size_ttcheckmenuitem,

    /* extends ttclass_ttobj */
    (void *)NULL /* inherits:  New_ttcheckmenuitem >> New_ttvisible */,
    (void *)NULL /* inherits:  Build_ttcheckmenuitem >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttcheckmenuitem >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttcheckmenuitem >> Del_ttvisible */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttcheckmenuitem >> AddTo_ttvisible */,
    (void *)NULL /* inherits:  Validate_ttcheckmenuitem >> Validate_ttvisible */,
    (void *)NULL /* inherits:  Remove_ttcheckmenuitem >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttcheckmenuitem >> Invalidate_ttvisible */,
    (void *)NULL /* inherits:  SetVisible_ttcheckmenuitem >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttcheckmenuitem >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttcheckmenuitem >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttcheckmenuitem >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

  },
 
  {
    Class_ttradiomenuitem,
    id_ttradiomenuitem,
    magicmask_ttradiomenuitem,
    magic_ttradiomenuitem,
    size_ttradiomenuitem,

    /* extends ttclass_ttobj */
    (void *)NULL /* inherits:  New_ttradiomenuitem >> New_ttvisible */,
    (void *)NULL /* inherits:  Build_ttradiomenuitem >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttradiomenuitem >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttradiomenuitem >> Del_ttvisible */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttradiomenuitem >> AddTo_ttvisible */,
    (void *)NULL /* inherits:  Validate_ttradiomenuitem >> Validate_ttvisible */,
    (void *)NULL /* inherits:  Remove_ttradiomenuitem >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttradiomenuitem >> Invalidate_ttvisible */,
    (void *)NULL /* inherits:  SetVisible_ttradiomenuitem >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttradiomenuitem >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttradiomenuitem >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttradiomenuitem >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

    /* extends ttclass_ttcheckmenuitem */

  },
 
  {
    Class_ttmenu,
    id_ttmenu,
    magicmask_ttmenu,
    magic_ttmenu,
    size_ttmenu,

    /* extends ttclass_ttobj */
    null_New_ttmenu,
    (void *)NULL /* inherits:  Build_ttmenu >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttmenu >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttmenu >> Del_ttvisible */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttmenu >> AddTo_ttvisible */,
    (void *)NULL /* inherits:  Validate_ttmenu >> Validate_ttvisible */,
    (void *)NULL /* inherits:  Remove_ttmenu >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttmenu >> Invalidate_ttvisible */,
    (void *)NULL /* inherits:  SetVisible_ttmenu >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttmenu >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttmenu >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttmenu >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */

    /* extends ttclass_ttmenuitem */

  },
 
  {
    Class_ttmenubar,
    id_ttmenubar,
    magicmask_ttmenubar,
    magic_ttmenubar,
    size_ttmenubar,

    /* extends ttclass_ttobj */
    (void *)NULL /* inherits:  New_ttmenubar >> New_ttvisible */,
    (void *)NULL /* inherits:  Build_ttmenubar >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttmenubar >> Break_ttobject */,
    null_Del_ttmenubar,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttmenubar >> AddTo_ttvisible */,
    (void *)NULL /* inherits:  Validate_ttmenubar >> Validate_ttvisible */,
    (void *)NULL /* inherits:  Remove_ttmenubar >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttmenubar >> Invalidate_ttvisible */,
    (void *)NULL /* inherits:  SetVisible_ttmenubar >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttmenubar >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttmenubar >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttmenubar >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */

  },
 
  {
    Class_ttanytext,
    id_ttanytext,
    magicmask_ttanytext,
    magic_ttanytext,
    size_ttanytext,

    /* extends ttclass_ttobj */
    null_New_ttanytext,
    (void *)NULL /* inherits:  Build_ttanytext >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttanytext >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_ttanytext >> Del_ttwindow */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanytext >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_ttanytext >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttanytext >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_ttanytext >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_ttanytext >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_ttanytext >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_ttanytext >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_ttanytext >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_ttanytext >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_ttanytext >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_ttanytext >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_ttanytext >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_ttanytext >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_ttanytext >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_ttanytext >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_ttanytext >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* inherits:  SetTitle_ttanytext >> SetTitle_ttwindow */,

    /* extends ttclass_ttwindow */

  },
 
  {
    Class_tttextfield,
    id_tttextfield,
    magicmask_tttextfield,
    magic_tttextfield,
    size_tttextfield,

    /* extends ttclass_ttobj */
    (void *)NULL /* inherits:  New_tttextfield >> New_ttanytext */,
    (void *)NULL /* inherits:  Build_tttextfield >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_tttextfield >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_tttextfield >> Del_ttwindow */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttextfield >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_tttextfield >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_tttextfield >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_tttextfield >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_tttextfield >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_tttextfield >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_tttextfield >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_tttextfield >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_tttextfield >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_tttextfield >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_tttextfield >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_tttextfield >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_tttextfield >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_tttextfield >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_tttextfield >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_tttextfield >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* inherits:  SetTitle_tttextfield >> SetTitle_ttwindow */,

    /* extends ttclass_ttwindow */

    /* extends ttclass_ttanytext */

  },
 
  {
    Class_tttextarea,
    id_tttextarea,
    magicmask_tttextarea,
    magic_tttextarea,
    size_tttextarea,

    /* extends ttclass_ttobj */
    (void *)NULL /* inherits:  New_tttextarea >> New_ttanytext */,
    (void *)NULL /* inherits:  Build_tttextarea >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_tttextarea >> Break_ttobject */,
    (void *)NULL /* inherits:  Del_tttextarea >> Del_ttwindow */,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttextarea >> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Validate_tttextarea >> Validate_ttwidget */,
    (void *)NULL /* inherits:  Remove_tttextarea >> Remove_ttvisible */,
    (void *)NULL /* inherits:  Invalidate_tttextarea >> Invalidate_ttwidget */,
    (void *)NULL /* inherits:  SetVisible_tttextarea >> SetVisible_ttvisible */,
    (void *)NULL /* inherits:  SetTheme_tttextarea >> SetTheme_ttvisible */,
    (void *)NULL /* inherits:  Draw_tttextarea >> Draw_ttvisible */,
    (void *)NULL /* inherits:  BuiltinRepaint_tttextarea >> BuiltinRepaint_ttvisible */,


    /* extends ttclass_ttvisible */
    (void *)NULL /* inherits:  SetLayout_tttextarea >> SetLayout_ttwidget */,
    (void *)NULL /* inherits:  SetXY_tttextarea >> SetXY_ttwidget */,
    (void *)NULL /* inherits:  SetWH_tttextarea >> SetWH_ttwidget */,
    (void *)NULL /* inherits:  SetXl_tttextarea >> SetXl_ttwidget */,
    (void *)NULL /* inherits:  SetYl_tttextarea >> SetYl_ttwidget */,
    (void *)NULL /* inherits:  SetWl_tttextarea >> SetWl_ttwidget */,
    (void *)NULL /* inherits:  SetHl_tttextarea >> SetHl_ttwidget */,
    (void *)NULL /* inherits:  SetTooltip_tttextarea >> SetTooltip_ttwidget */,

    /* extends ttclass_ttwidget */
    (void *)NULL /* inherits:  SetTitle_tttextarea >> SetTitle_ttwindow */,

    /* extends ttclass_ttwindow */

    /* extends ttclass_ttanytext */

    /* extends ttclass_tttextfield */

  },
 
  {
    Class_tttheme,
    id_tttheme,
    magicmask_tttheme,
    magic_tttheme,
    size_tttheme,

    /* extends ttclass_ttobj */
    (void *)NULL /* inherits:  New_tttheme >> New_ttcomponent */,
    null_Build_tttheme,
    (void *)NULL /* inherits:  Break_tttheme >> Break_ttobject */,
    null_Del_tttheme,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */

  },
 
  {
    Class_ttapplication,
    id_ttapplication,
    magicmask_ttapplication,
    magic_ttapplication,
    size_ttapplication,

    /* extends ttclass_ttobj */
    null_New_ttapplication,
    (void *)NULL /* inherits:  Build_ttapplication >> Build_ttobject */,
    (void *)NULL /* inherits:  Break_ttapplication >> Break_ttobject */,
    null_Del_ttapplication,

    /* extends ttclass_ttobject */

    /* extends ttclass_ttcomponent */

  },



  {

    null_Sync, 
    null_Flush, 
    null_TimidFlush, 
    null_MainLoopOnce, 
    null_DeleteCallback,    
    null_Close, 
    null_ConnectionFd, 
    null_GetErrno, 
    null_GetErrnoDetail,    
    null_StrError, 
    null_StrErrorDetail,   
  },
};






