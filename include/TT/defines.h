

#ifndef _TT_DEFINES_H
#define _TT_DEFINES_H

#define TT_FALSE	0
#define TT_TRUE		1

/** value meaning `no object' */
#define TT_NOID		((ttopaque)0)
/** value meaning `invalid object' */
#define TT_BADID	TW_MAXTOPAQUE

/** the obvious meaning */
#define TT_MIN2(a,b) ((a)<(b) ? (a) : (b))
/** the obvious meaning */
#define TT_MAX2(a,b) ((a)>(b) ? (a) : (b))




/** define some magic data to use with TTCheckMagic() */
#define TT_DECL_MAGIC(id) TW_DECL_MAGIC(id)


/** create a new object given its type name (i.e TTNEW(ttlabel)) */
#define TTNEW(type)		((type)TTNew(TT_CAT(TTClass_,type)))
/** delete given object */
#define TTDEL(o)		TTDel((ttobj)(o))

/** return 1 if `o' can be cast to type name `t' (i.e TTINSTANCEOF(tttheme, o)) */
#define TTINSTANCEOF(t, o)	TTInstanceOf(TT_CAT(TTClass_,t), (ttobj)(o))
/** alias for TTINSTANCEOF(t, o) */
#define TTIS(t, o)		TTInstanceOf(TT_CAT(TTClass_,t), (ttobj)(o))
/** alias for TTInstanceOf(t, o) */
#define TTIs(Class, o)		TTInstanceOf(Class, o)
/** alias for TTIsInstance_ttclass(FN, t) */
#define TTIs_ttclass(Class, t)	TTIsInstance_ttclass(Class, t)

/** return the class (runtime type) of `o' (will be one of the `TTClass_*' types) */
#define TTCLASSOF(o)		TTClassOf((ttobj)(o))


#endif /* _TT_DEFINES_H */

