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

#ifndef _TT_INLINES_H
#define _TT_INLINES_H


/* #define ISClass(Class1,Class2) macro */
#define ISClass(ClassT,Class) (((Class)->magic & (ClassT)->magicmask) == (ClassT)->magic)

/* #define ISCLASS(type,Class) macro */
#define ISCLASS(type,Class) (((Class)->magic & magicmask_(type)) == magic_(type))

/* #define IS(type,o) macros */
#define IS(type,o) ISCLASS(type,(o)->Class)

/* #define Is(Class,o) macros */
#define Is(Class,o) ISClass(Class,(o)->Class)



/* ttobj <-> tt_obj conversion */

TT_INLINE opaque Obj2Id(ttobj Obj) {
    return Obj ? Obj->id : TT_NOID;
}
#define OBJ2ID(o) Obj2Id((ttobj)(o))

#define ID2OBJ(id)   Id2Obj(id_ttclass_ttobj, (opaque)(id))
#define ID2(type,id) ((type)Id2Obj(id_ttclass_(type), (topaque)(id)))


/* ttclass <-> tt_obj conversion */

#define ORDER2ID_CLASS(Class) ((Class) | (order_ttclass<<TT_MAGIC_SHIFT))

TT_INLINE ttclass Id2Class(opaque Class) {
    if (Class >= id_ttclass_first && Class < id_ttclass_last)
	return TClasses[Class & TT_MAGIC_MASK];
    return NULL;
}
#define ID2CLASS(Class) Id2Class((opaque)(Class))



/* constructors */

#define TNEW_NOBUILD(type)	(TClass(type)->New(TClass(type), (type)0))
#define TNEW2(type, o)		((type)New((ttclass)TClass(type), (ttobject)o))
#define TNEW(type)		TNEW2(type, (ttobject)0)
#define TDEL(o)			Del((ttobject)o)

/* destructors */
/* unused *//*
TT_INLINE void Break(ttobj o) {
    if (o)
	o->Class->Break(o);
}
*/
TT_INLINE void Del(ttobject o) {
    if (o && IS(ttobject,o) &&
	!(o->oflags & ttobject_oflags_const) &&
	!(o->refcount &= ~ttobject_refcount_alive) &&
	!(o->events_inprogress & ttmask(ttevent_evtype_del))) {
	
	/*
	 * WARNING: delete ONLY if not already doing it:
	 * object deletion is not reentrant!
	 */
	o->events_inprogress |= ttmask(ttevent_evtype_del);

	o->Class->Del(o);
    }
}
/* constructors */
TT_INLINE ttobject Build(ttobject o) {
    if (o) {
	if (o->Class->Build(o))
	    return o;
	TDEL(o);
    }
    return (ttobject)0;
}
TT_INLINE ttobject New(ttclass _Class, ttobject o) {
    ttclass_ttobject Class = (ttclass_ttobject)_Class;
    
    if (Class && ISCLASS(ttobject,Class) && (o = Class->New(Class, o)))
	return Build(o);
    return (ttobject)0;
}

/* ttlistener */


/* ttmenubar */
TT_INLINE ttmenubar GetDefault_ttmenubar(void) {
    if (!TTD.Menubar)
	TTD.Menubar = TNEW(ttmenubar);
    return TTD.Menubar;
}


/* tttheme */
TT_INLINE tttheme GetDefault_tttheme(void) {
    if (!TTD.Theme && !TTD.DummyTheme)
	TTD.Theme = TNEW(tttheme);
    return TTD.Theme ? TTD.Theme : TTD.DummyTheme;
}

TT_INLINE ttapplication Get_ttapplication(void) {
    return TTD.Application;
}

#endif /* _TT_INLINES_H */

