

dnl This file is `m4/call.m4' : m4 macros to autogenerate call_m4.h
dnl Tell the user about this.


/* This file was automatically generated from `m4/call.m4', do not edit! */


/*
 *  call_m4.h  --  implementation of callbacks to libTT methods
 *
 */

include(`m4/TTclasses.m4h')
include(`m4/TThandy.m4h')

divert(-1)

define(`tolower', `translit(`$1', `A-Z', `a-z')')
define(`toupper', `translit(`$1', `a-z', `A-Z')')
define(`Toupper', `toupper(substr(`$1', 0, 1))`'patsubst(substr(`$1', 1), `_\(\w\)',``'toupper(`\1')')')

define(`opaque_ifpointer', `ifelse(regexp(`$1', `_fn$'), -1, ifelse(index(`$1', `*'), -1, `', (opaque)), (opaque))')

define(`arg_decl_decay', `ifdef(`m4super_$1', tt_obj, $1)')
define(`arg_val_decay', `ifdef(`m4super_$1', (tt_obj)(opaque), `($1)opaque_ifpointer($1)')')
define(`arg_ret_decay', `(ttany)ifdef(`m4super_$1', (opaque), `opaque_ifpointer($1)')')

define(`obj2id_ifnotvoid_ifdecay', `ifelse($1, void, $2, `ifdef(`m4super_$1', `(tt_obj)OBJ2ID($2)', $2)')')

define(`arg_decl', `arg_decl_decay($3) a$1')
define(`args_decl', `ifelse($2, 0, `', $2, 1, `arg_decl($@)', `arg_decl($@), args_decl(incr($1), decr($2), NSHIFT(3, $@))')')

define(`arg_call', `ifdef(`m4super_$3', `ID2($3,a$1)', a$1)')
define(`_args_call', `ifelse($2, 0, `', `, arg_call($@)`'_args_call(incr($1), decr($2), NSHIFT(3, $@))')')
define(`args_call', `ifelse($2, 0, `', $2, 1, `arg_call($@)', `arg_call($@)`'_args_call(incr($1), decr($2), NSHIFT(3, $@))')')


define(`decl_a0_ifnotvoid', `ifelse($1, void, `', `arg_decl(0, 0, $1) = arg_val_decay($1)0;
    ')')
define(`a0_eq_ifnotvoid', `ifelse($1, void, `', `a0 =')')
define(`a0_ifnotvoid', `ifelse($1, void, `', `a0')')

define(`extends')
define(`public', `ifdef(`m4super_$6', `
arg_decl_decay($5) TT$2_$3`'(ifelse($4, 0, void, `args_decl(1, $4, NSHIFT(5, $@))'));
', `
arg_decl_decay($5) TT$2_$3`'(ifelse($4, 0, void, `args_decl(1, $4, NSHIFT(5, $@))'));
')')
define(`exported',`ifdef(`m4super_$6', `
arg_decl_decay($5) TT$2_$3`'(ifelse($4, 0, void, `args_decl(1, $4, NSHIFT(5, $@))'));
', `
arg_decl_decay($5) TT$2_$3`'(ifelse($4, 0, void, `args_decl(1, $4, NSHIFT(5, $@))'));
')')


divert

/* prototypes for `public' and `exported' methods */

define(`el',`
/* $1 methods */
TTFNdef_$1($1)
')
TTlist()
undefine(`el')
define(`exported')
define(`public')
define(`extends')

/* prototypes for handy `public' and `exported' methods */

define(`decl_name', `ifelse(`$1', `', `Toupper($2)`'Toupper($3)', `Toupper($1)')')
define(`decl_args', `arg_decl_decay(this) o`'decl_fn_$3(NSHIFT(3, $@))')

define(`decl_fn_get', `')
define(`decl_fn_set', `, $2 $1')
define(`decl_fn_fixedchange', `')

define(`def_handy', `
arg_decl_decay($1) TT`'decl_name($2,$3,$4)_`'this`'(decl_args($@));
')

define(`wrap_export', `ifelse(index(`$3', `r'), -1, `', `exported($1,,get,$2,$1)')`'dnl
`'ifelse(index(`$3', `w'), -1, `', `exported(void,,set,$2,$1)')')

define(`el', `
/* prototypes for handy $1 methods */
define(`this', `$1')`'TThandy_$1($1,$1)')
define(`extends')
define(`exported', `def_handy($@)')
define(`field', `wrap_export($@)')
define(`exported_fields', `TTdef_$1($1,$1)')
TTlist()
define(`exported_fields')
define(`exported')
define(`extends')
define(`field')
undefine(`el')


/* enum for method callbacks */

typedef enum e_order_methods {
    /* generic functions */
    order_BAD = -1,
    order_ExitMainLoop,
    order_New,
    order_Del,dnl
define(`exported', `
    order_$2_$3,')
define(`public', `exported($@)')
define(`el',`
    /* $1 methods */TTFNdef_$1($1)')
TTlist()
undefine(`el')
define(`exported')
define(`public')

    order_FN_handy,

define(`el', `
    /* handy $1 methods */dnl
define(`this', `$1')`'TThandy_$1($1,$1)')
define(`extends')
define(`exported', `
    order_`'decl_name($2,$3,$4)_`'this`',')
define(`field', `wrap_export($@)')
define(`exported_fields', `TTdef_$1($1,$1)')
TTlist()
define(`exported_fields')
define(`exported')
define(`extends')
define(`field')
undefine(`el')

    order_FN_n
} e_order_methods;


typedef struct s_ttmethod s_ttmethod;
typedef struct s_ttmethod *ttmethod;
struct s_ttmethod {
    opaque mtho;
    void *mth;
};

/* array for method callbacks */

static struct s_ttmethod method_array[] = {
    /* generic functions */
    { order_ExitMainLoop, (void *)TTExitMainLoop, },
    { order_New, (void *)TTNew, },
    { order_Del, (void *)TTDel, }, dnl
define(`exported', `
    { order_$2_$3, (void *)TT$2_$3, },')
define(`public', `exported($@)')
define(`el',`
    /* $1 methods */TTFNdef_$1($1)')
TTlist()
undefine(`el')
define(`exported')
define(`public')

define(`el', `
    /* handy $1 methods */dnl
define(`this', `$1')`'TThandy_$1($1,$1)')
define(`extends')
define(`exported', `
    { order_`'decl_name($2,$3,$4)_`'this`', (void *)TT`'decl_name($2,$3,$4)_`'this`', },')
define(`field', `wrap_export($@)')
define(`exported_fields', `TTdef_$1($1,$1)')
TTlist()
define(`exported_fields')
define(`exported')
define(`extends')
define(`field')
undefine(`el')

};

static ttbyte method_needsort = TRUE;

static int CompareMethods(TT_CONST ttmethod m1, TT_CONST ttmethod m2) {
    return (int)((byte *)m1->mth - (byte *)m2->mth);
}

/*
 * use binary search for efficiency
 */
static opaque Method2Order(void *method) {
    s_ttmethod key, *m;
    
    if (method_needsort) {
	method_needsort = FALSE;
	qsort(method_array, order_FN_n, sizeof(struct s_ttmethod),
	      (int (*)(TT_CONST void *, TT_CONST void *))CompareMethods);
    }
    
    key.mth = method;
    
    if ((m = bsearch(&key, method_array, order_FN_n, sizeof(struct s_ttmethod),
	             (int (*)(TT_CONST void *, TT_CONST void *))CompareMethods))) {
	return m->mtho;
    }
    return (opaque)order_BAD;
}


define(`arg_call', `arg_val_decay($3)a[decr($1)]')
define(`_args_call', `ifelse($2, 0, `', `, arg_call($@)`'_args_call(incr($1), decr($2), NSHIFT(3, $@))')')
define(`args_call', `ifelse($2, 0, `', $2, 1, `arg_call($@)', `arg_call($@)`'_args_call(incr($1), decr($2), NSHIFT(3, $@))')')


/*
 * here and below, we do consider TTGet*() methods specially:
 * they have no side effects, and if called from within a callback they
 * cannot return a useful return value, so we can do without calling them.
 */
static void CallMethod(ttuint order, ttuint nargs, TT_CONST ttany *a) {

    switch (order) {
    
    /* generic functions */
    
      case order_New:
	if (nargs >= 1)
	    TTNew((tt_fn)(opaque)a[0]);
	break;
      case order_Del:
	if (nargs >= 1)
	    TTDel((tt_obj)(opaque)a[0]);
	break;
      case order_ExitMainLoop:
	if (nargs >= 0)
	    TTExitMainLoop();
	break;

define(`exported', `
      case order_$2_$3:
	if (nargs >= $4)
	    TT$2_$3(args_call(1, $4, NSHIFT(5, $@)));
	break;')
define(`public', `exported($@)')
define(`el',`
    /* $1 methods */
    TTFNdef_$1($1)
')
TTlist()
undefine(`el')
define(`exported')
define(`public')

define(`el', `
    /* handy $1 methods */dnl
define(`this', `$1')`'TThandy_$1($1,$1)')
define(`extends')
define(`exported', `ifelse(`$3', get, `', `
      case order_`'decl_name($2,$3,$4)_`'this`':
	if (nargs >= ifelse(`$3', set, 2, 1))
	    TT`'decl_name($2,$3,$4)_`'this`'((tt_obj)(opaque)a[0]`'dnl
`'`'`'`'`'`'ifelse(`$3', set, `, ($5)opaque_ifpointer($5)a[1]', `'));
	break;')')
define(`field', `wrap_export($@)')
define(`exported_fields', `TTdef_$1($1,$1)')
TTlist()
define(`exported_fields')
define(`exported')
define(`extends')
define(`field')
undefine(`el')

      default:
	break;
    }
}



