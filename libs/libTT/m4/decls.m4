



dnl This file is `m4/decls.m4' : m4 macros to autogenerate decls_m4.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/decls.m4', do not edit! */


/*
 *  decls_m4.h  --  INTERNAL (!!) header for libTT types declarations
 *
 *  Note: you cannot include both this file and <TT/decls_m4.h>, as they conflict!
 */

#ifndef _TT_DECLSM4_H
#define _TT_DECLSM4_H

include(`m4/TTclasses.m4h')

/* many useful macros */

#define Min2(a,b) ((a) < (b) ? (a) : (b))

#define TT_OFFSETOF(type,field)	((size_t)&((type)0)->field)

/* #defines for objects hierarchy */
define(`extends',`
`#'define super_$2	$1')
define(`field')
define(`el',`TTdef_$1($1)')
TTlist()
undefine(`el')
define(`field')
define(`extends')


typedef struct s_ttobj TT_CONST *	ttobj_c;

/* forward declarations for objects structures */
define(`el',`
typedef struct s_$1 *   $1;')
TTlist()
undefine(`el')


/* forward declarations for methods structures */
define(`el',`
typedef struct s_ttfn_$1 *	ttfn_$1;')
TTlist()
undefine(`el')

typedef ttfn_ttobj ttfn;


/* declarations for objects structures */
define(`extends',`TTdef_$1($2)`'dnl
    /* `extends' $1 */
')
define(`field',`$1 $2;
')
define(`el',`
typedef struct s_$1 {
`'TTdef_$1($1)} s_$1;
')
TTlist()
undefine(`el')
define(`field')
define(`extends')


/* declarations for methods structures */
define(`extends',`TTFNdef_$1($2)
    /* `extends' ttfn_$1 */
')
define(`protected',`$1;
')
define(`public',`protected($@)')
define(`public_set',`protected($@)')
define(`el',`
typedef struct s_ttfn_$1 {
`'TTFNdef_$1($1)
} s_ttfn_$1;
')
TTlist()
undefine(`el')
define(`public')
define(`public_set')
define(`protected')
define(`extends')

typedef struct s_tthw *tthw;
typedef struct s_tthw {
    byte (*Sync)(void);
    byte (*Flush)(void);
    byte (*TimidFlush)(void);
    byte (*MainLoopOnce)(byte);
    void (*DeleteCallback)(ttcallback);

    void (*Close)(void);
    int (*ConnectionFd)(void);
    
    ttuint (*GetErrno)(void);
    ttuint (*GetErrnoDetail)(void);
    TT_CONST byte *(*StrError)(ttuint E);
    TT_CONST byte *(*StrErrorDetail)(ttuint E, ttuint S);

} s_tthw;

define(`el',`
    s_ttfn_$1 fn_$1;')
typedef struct s_ttfns *ttfns;
typedef struct s_ttfns {
TTlist()
    s_tthw HW;
} s_ttfns;
undefine(`el')

define(`el',`
extern tt_fn TTFN_$1;')

/* client-visible global types */
TTtypelist()
#define TTFN_ttsbyte	TTFN_ttbyte
#define TTFN_ttushort	TTFN_ttshort
#define TTFN_ttuint	TTFN_ttint

/* client-visible global methods structures */
TTlist()

undefine(`el')

/* #defines for global methods structures */
#define TFNs	(TTD.FNs)

#define THW	(TTD.FN.HW)

define(`el',`
`#'define TFN_$1	(&TTD.FN.fn_$1)')
TTlist()
undefine(`el')

#define TFN(type) TT_CAT(TFN_,type)


define(`el',`
`#'define TFNdefault_$1	(&TTD.FNdefault.fn_$1)')
TTlist()
undefine(`el')
#define TFNdefault(type) TT_CAT(TFNdefault_,type)



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
define(`public', `
arg_decl_decay($5) TT$2_$3`'(ifelse($4, 0, void, `args_decl(1, $4, NSHIFT(5, $@))'));
')
define(`public_set',`public($@)')
define(`exported',`public($@)')


divert

/* prototypes for `public', `public_set' and `exported' methods */

define(`el',`
/* $1 methods */
`'TTFNdef_$1($1)
')
TTlist()
undefine(`el')
define(`exported')
define(`public_set')
define(`public')
define(`extends')

/* prototypes for handy `public', `public_set' and `exported' methods */

define(`decl_name', `ifelse(`$1', `', `Toupper($2)`'Toupper($3)', `Toupper($1)')')
define(`decl_args', `arg_decl_decay(this) o`'decl_fn_$3(NSHIFT(3, $@))')

define(`decl_fn_get', `')
define(`decl_fn_set', `, $2 $1')
define(`decl_fn_fixedchange', `')

define(`def_handy', `
arg_decl_decay($1) TT`'decl_name($2,$3,$4)_`'this`'(decl_args($@));
')

define(`wrap_export', `ifelse(index(`$3', `r'), -1, `', `exported($1,,get,$2,$1)')`'dnl
`'ifelse(index(`$3', `w'), -1, `', `exported(tt_fn,,set,$2,$1)')')

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


#endif /* _TT_DECLSM4_H */

