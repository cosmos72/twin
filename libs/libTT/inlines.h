
#ifndef _TT_INLINES_H
#define _TT_INLINES_H


/* ttobj <-> tt_obj conversion */

TT_INLINE opaque Obj2Id(ttobj Obj) {
    return Obj ? Obj->id : NOID;
}
#define OBJ2ID(o) Obj2Id((ttobj)(o))

#define ID2OBJ(id)   Id2Obj(order_ttobj, (opaque)(id))
#define ID2(type,id) ((type)Id2Obj(order_(type), (topaque)(id)))


#define TNEW_NOBUILD(type)	(TFN(type)->New(TFN(type), (type)0))
#define TNEW2(type, o)		((type)New((ttfn_ttobj)TFN(type), (ttobj)o))
#define TNEW(type)		TNEW2(type, (ttobj)0)
#define TDEL(o)			Del((ttobj)o)

/* destructors */
/* unused *//*
TT_INLINE void Break(ttobj o) {
    if (o)
	o->FN->Break(o);
}
*/
TT_INLINE void Del(ttobj o) {
    if (o && !(o->refcount &= ~ttobj_refcount_alive) &&
	!(o->events_inprogress & ttmask(ttevent_evtype_del))) {
	
	/*
	 * WARNING: delete ONLY if not already doing it:
	 * object deletion is not reentrant!
	 */
	o->events_inprogress |= ttmask(ttevent_evtype_del);

	o->FN->Del(o);
    }
}
/* constructors */
TT_INLINE ttobj Build(ttobj o) {
    if (o) {
	if (o->FN->Build(o))
	    return o;
	TDEL(o);
    }
    return (ttobj)0;
}
TT_INLINE ttobj New(ttfn_ttobj FN, ttobj o) {
    if (FN && (o = FN->New(FN, o)))
	return Build(o);
    return (ttobj)0;
}

/* ttlistener */


/* ttmenubar */
TT_INLINE ttmenubar GetDefault_ttmenubar(void) {
    if (!TTD.Menubar && (TTD.Menubar = TFN_ttmenubar->New(TFN_ttmenubar, NULL)))
	return (ttmenubar)Build((ttobj)TTD.Menubar);
    return TTD.Menubar;
}


/* tttheme */
TT_INLINE tttheme GetDefault_tttheme(void) {
    if (!TTD.Theme && !TTD.DummyTheme && (TTD.Theme = TFN_tttheme->New(TFN_tttheme, NULL)))
	return (tttheme)Build((ttobj)TTD.Theme);
    return TTD.Theme ? TTD.Theme : TTD.DummyTheme;
}

TT_INLINE ttapplication Get_ttapplication(void) {
    return TTD.Application;
}

#endif /* _TT_INLINES_H */

