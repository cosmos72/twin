
#ifndef _TT_DATATYPES_H
#define _TT_DATATYPES_H

/* typedefs for objects and method structures */
typedef  byte    ttbyte;
typedef sbyte    ttchar;
typedef   dat    ttshort;
typedef  udat    ttushort;
typedef  ldat    ttint;
typedef uldat    ttuint;

typedef hwcol    ttcol;
typedef hwfont   ttfont;
typedef hwattr   ttattr;

typedef topaque ttopaque;
typedef topaque   opaque;
typedef tany    ttany;
typedef tany      any;

typedef void *	 ttpointer;
typedef TT_CONST ttbyte * ttstring;
typedef void (*ttfunction)(void);

/*
 * these are the types to interface with clients.
 * Data of this types actually contains (ttopaque) integers.
 */
typedef struct s_tt_obj *tt_obj;
typedef struct s_tt_fn  *tt_fn;


typedef void (*ttlistener_fn)(ttany arg0);
typedef void (*ttlistener_fn_array)(ttuint nargs, ttany *args);
typedef void (*ttvisible_repaint_fn)(tt_obj,ttshort,ttshort,ttshort,ttshort);


typedef struct s_ttshape *ttshape;
typedef struct s_ttshape {
    ttshort width, height, border[4];
    ttattr *attr;
} s_ttshape;


/* AVL stuff imported from <Tw/Twavl.h> */
typedef struct s_tavl s_ttavl;
typedef tavl ttavl;
typedef tavl_c ttavl_c;
typedef tavl_compare ttavl_compare;



#endif /* _TT_DATATYPES_H */

