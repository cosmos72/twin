

dnl This file is `m4/wrap.m4' : m4 macros to autogenerate wrap_m4.h
dnl Tell the user about this.


/* This file was automatically generated from `m4/wrap.m4', do not edit! */


/*
 *  wrap_m4.h  --  wrappers around exported libTT methods
 *
 */

include(`m4/TTclasses.m4h')

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
arg_decl_decay($5) TT$2_$3`'(ifelse($4, 0, void, `args_decl(1, $4, NSHIFT(5, $@))')) {
    $6 o;
    decl_a0_ifnotvoid($5)`'LOCK;
    if ((o = ID2($6,a1)))
	a0_eq_ifnotvoid($5) obj2id_ifnotvoid_ifdecay($5, `o->FN->$2`'(o`'_args_call(2, decr($4), NSHIFT(6, $@)))');
    UNLK;
    return a0_ifnotvoid($5);
}', `
arg_decl_decay($5) TT$2_$3`'(ifelse($4, 0, void, `args_decl(1, $4, NSHIFT(5, $@))')) {
    decl_a0_ifnotvoid($5)`'LOCK;
    a0_eq_ifnotvoid($5) obj2id_ifnotvoid_ifdecay($5, TFN_$3->$2`'(args_call(1, $4, NSHIFT(5, $@))));
    UNLK;
    return a0_ifnotvoid($5);
}')')
define(`exported',`ifdef(`m4super_$6', `
arg_decl_decay($5) TT$2_$3`'(ifelse($4, 0, void, `args_decl(1, $4, NSHIFT(5, $@))')) {
    $6 o;
    decl_a0_ifnotvoid($5)`'LOCK;
    if ((o = ID2($6,a1)))
	a0_eq_ifnotvoid($5) obj2id_ifnotvoid_ifdecay($5, `$2_$3`'(o`'_args_call(2, decr($4), NSHIFT(6, $@)))');
    UNLK;
    return a0_ifnotvoid($5);
}', `
arg_decl_decay($5) TT$2_$3`'(ifelse($4, 0, void, `args_decl(1, $4, NSHIFT(5, $@))')) {
    decl_a0_ifnotvoid($5)`'LOCK;
    a0_eq_ifnotvoid($5) obj2id_ifnotvoid_ifdecay($5, `$2_$3`'(args_call(1, $4, NSHIFT(5, $@)))');
    UNLK;
    return a0_ifnotvoid($5);
}')')
define(`public_set',`exported($@)')


divert

/* `public', `public_set' and `exported' methods */

define(`el',`
/* $1 methods */
TTFNdef_$1($1)
')
TTlist()
undefine(`el')
define(`exported')
define(`public')
define(`public_set')
define(`extends')


