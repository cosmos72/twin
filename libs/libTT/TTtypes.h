
#ifndef _TT_TYPES_H
#define _TT_TYPES_H


#define Min2(a,b) ((a) < (b) ? (a) : (b))


/* set magic_xxx enums */
typedef enum e_order_ttobjs {
#define el(obj) TT_CAT(order_,obj),
TT_LIST(el)
#undef el
} order_ttobjs;

typedef enum e_magicbit_ttobjs {
#define el(obj) TT_CAT(magicbit_,obj) = 1<<TT_CAT(order_,obj),
TT_LIST(el)
#undef el
} magicbit_ttobjs;

typedef enum e_magic_ttobjs {
    magic_ttobj = magicbit_ttobj,
#define el(obj) TT_CAT(magic_,obj) = TT_CAT(magicbit_,obj) | TT_CAT(magic_,TT_CAT(TTsuper_,obj)),
TT_NLIST(el)
#undef el
} magic_ttobjs;


/* #define IS(xxx,o) macros */
#define IS(obj,o) ((o)->FN->magic & TT_CAT(magicbit_,obj))


typedef struct s_ttobj TT_CONST * ttobj_c;

typedef struct s_ttlistener *ttlistener;
struct s_ttlistener {
    ttlistener prev, next;
    tlistener t;
};


typedef struct s_ttshape *ttshape;
struct s_ttshape {
    dat width, height, border[4];
    hwattr *attr;
};


#endif /* _TT_TYPES_H */

