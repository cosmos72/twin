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
/* ttclass_ttobj <-> tt_obj conversion */


TT_INLINE ttopaque Class2Id(ttclass Class) {
    return Class ? Class->id : TT_NOID;
}
#define CLASS2ID(Class) Class2Id((ttclass)Class)



/* type query helpers */


TT_INLINE ttbyte IsInstance_ttclass(ttclass Class, ttclass t) {
    return Class && t && (t->magic & Class->magicmask) == Class->magic;
}

TT_INLINE ttbyte InstanceOf(ttclass Class, ttobj o) {
    return o && IsInstance_ttclass(Class, (ttclass)o->Class);
}



/* ttobj <-> tobj conversion */


static ttobj Id2Obj(ttopaque i, ttopaque Id) {
    ttopaque j = Id >> TT_MAGIC_SHIFT;
    ttobj o;
    if (i >= id_ttclass_first && i < id_ttclass_last && j >= order_first && j < order_last) {
	Id &= TT_MAGIC_MASK;
	if (Id < IdTop[j] && (o = IdList[j][Id]))
	    if (InstanceOf(Id2Class(i), o))
		return o;
    }
    return (ttobj)0;
}


/* type querying functions */

ttbyte TTIsInstance_ttclass(tt_obj _Class, tt_obj t) {
    opaque Class = (opaque)_Class;
    ttbyte ret;
    if (Class >= id_ttclass_type_first && Class < id_ttclass_type_last)
	/* handle the case Class and t are one of TTClass_void...TTClass_ttarg */
	ret = Class == (opaque)t;
    else
	ret = IsInstance_ttclass(ID2CLASS(Class), ID2CLASS(t));
    return ret;
}
ttbyte TTInstanceOf(tt_obj _Class, tt_obj o) {
    opaque Class = (opaque)_Class;
    ttbyte ret;
    if (Class >= id_ttclass_type_first && Class < id_ttclass_type_last) {
	/* a primitive type can have any value, including values that are also objects */
	ret = TT_TRUE;
    } else {
	LOCK;
	ret = InstanceOf(ID2CLASS(Class), ID2OBJ(o));
	UNLK;
    }
    return ret;
}

#if 0 /* not currently used */
TT_INLINE ttclass ClassOf(ttobj o) {
    ttclass ret;
    LOCK;
    ret = o ? o->Class : (ttclass)o;
    UNLK;
    return ret;
}
#endif

tt_obj TTClassOf(tt_obj o) {
    tt_obj ret;

    LOCK;
    /* shortcut for CLASS2ID(ClassOf(ID2OBJ(o))) */
    ret = (tt_obj) ( ID2OBJ(o) ? ORDER2ID_CLASS((opaque)o >> TT_MAGIC_SHIFT) : TT_NOID );
    UNLK;
    return ret;
}

ttopaque TTGetSize_ttclass(tt_obj _Class) {
    opaque Class = (opaque)_Class;
    
    if (Class >= id_ttclass_type_first && Class < id_ttclass_type_last) {
	/* handle the case Class is one of TTClass_void...TTClass_tttype */
	switch (Class & TT_MAGIC_MASK) {
#define el(t) case TT_CAT(order_,t): return TT_CAT(order_,t) == order_void ? 0 : sizeof(t);
	    TT_TYPELIST(el)
#undef el
	  default:
	    return 0;
	}
    } else if (Class >= id_ttclass_first && Class < id_ttclass_last) {
	return sizeof(tt_obj);
    } else
	return 0;
}

ttopaque TTSizeOf(tt_obj o) {
#if 1
    return ID2OBJ(o) ? sizeof(tt_obj) : 0;
#else
    return TTGetSize_ttclass((tt_obj)TTClassOf(o));
#endif
}


/*
 * return the superclass of the class fn
 */
static TT_ARG_READ ttclass GetSuper_ttclass(TT_ARG_READ ttclass Class) {
    ttclass super_Class;
    opaque i;
    
    /*
     * this could be *MUCH* smarter than a dumb linear search,
     * but hardcoding the class tree here would make it impossible
     * to create more classes at runtime.
     */
    if (Class) {
	for (i = Class->id - 1; i != (opaque)-1; i--) {
	    if ((super_Class = ID2CLASS(i)) && ISClass(super_Class, Class))
		return super_Class;
	}
    }
    return (TT_ARG_READ ttclass)0;
}

TT_ARG_READ tt_obj TTGetSuper_ttclass(TT_ARG_READ tt_obj Class) {
    return (tt_obj)CLASS2ID(GetSuper_ttclass(ID2CLASS(Class)));
}

tt_obj TTSuperClassOf(tt_obj o) {
    return TTGetSuper_ttclass((tt_obj)TTClassOf(o));
}


static TT_CONST ttbyte *ttclass_names[] = {
#define el(t) TT_STR(t),
    TT_LIST(el)
};
static TT_CONST ttbyte *ttclass_type_names[] = {
    TT_TYPELIST(el)
#undef el
};

/*
 * return the name of the class Class
 */
TT_ARG_ARRAY_Z TT_CONST ttbyte *TTGetName_ttclass(tt_obj _Class) {
    opaque Class = (opaque)_Class;
    
    if (Class >= id_ttclass_first && Class < id_ttclass_last)
	return ttclass_names[Class - id_ttclass_first];

    if (Class >= id_ttclass_type_first && Class < id_ttclass_type_last)
	return ttclass_type_names[Class - id_ttclass_type_first];
    
    return NULL;
}

/*
 * return the name of the class of o
 */
TT_CONST ttbyte *TTClassNameOf(tt_obj o) {
    return TTGetName_ttclass((tt_obj)TTClassOf(o));
}












/* return the first existing object of type `Class' */
tt_obj TTGetFirst(tt_obj _Class) {
    opaque Class = (opaque)_Class;
    opaque ret = (opaque)0;
    
    if (Class == id_ttclass_ttclass) {
	/* handle the case Class is TTClass_ttclass */
	ret = id_ttclass_first; /* TTClass_ttobj */
    } else if (Class >= id_ttclass_first && Class < id_ttclass_last) {
	/* handle the case Class is one of TTClass_* */
	ret = OBJ2ID( IdList[Class & TT_MAGIC_MASK][0] );
    }
    return (tt_obj)ret;
}

/* return the last existing object of type `Class' */
tt_obj TTGetLast(tt_obj _Class) {
    opaque Class = (opaque)_Class;
    opaque ret = (opaque)0;
    
    if (Class == id_ttclass_ttclass) {
	/* handle the case Class is TTClass_ttclass */
	ret = id_ttclass_last - 1; /* TTClass_ttapplication */
    } else if (Class >= id_ttclass_first && Class < id_ttclass_last) {
	/* handle the case Class is one of TTClass_* */
	Class &= TT_MAGIC_MASK;
	if (IdTop[Class])
	    ret = OBJ2ID( IdList[Class][IdTop[Class] - 1] );
    }
    return (tt_obj)ret;
}

