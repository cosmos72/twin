

dnl This file is `m4/handy.m4' : m4 macros to autogenerate handy_m4.h
dnl Tell the user about this.


/* This file was automatically generated from `m4/handy.m4', do not edit! */


/*
 *  handy_m4.h  --  wrappers around handy libTT methods
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

divert

/* mixed-internal {Get,Set,Change}Value(ttopaque id, ttuint which, ...) */

static ttbyte GetValue_ttobj(opaque id, ttuint which, ttany *value) {
    ttobj o;
    if ((o = ID2OBJ(id)))
	return o->FN->GetValue(o, which, value);
    return FALSE;
}    

static ttbyte SetValue_ttobj(opaque id, ttuint which, ttany value) {
    ttobj o;
    if ((o = ID2OBJ(id)))
	return o->FN->SetValue(o, which, value);
    return FALSE;
}    

static ttbyte ChangeValue_ttobj(opaque id, ttuint which, ttany nand_value, ttany xor_value) {
    ttobj o;
    if ((o = ID2OBJ(id)))
	return o->FN->ChangeValue(o, which, nand_value, xor_value);
    return FALSE;
}    

/* handy `public' and `exported' methods */


define(`decl_name', `ifelse(`$1', `', `Toupper($2)`'Toupper($3)', `Toupper($1)')')
define(`decl_args', `arg_decl_decay(this) o`'decl_fn_$3(NSHIFT(3, $@))')

define(`decl_fn_get', `')
define(`decl_fn_set', `, $2 $1')
define(`decl_fn_fixedchange', `')

define(`body', `body_fn_$3($@)')
define(`body_fn_get', `ttany a0;
    return GetValue_ttobj((opaque)o, this`'_$4, &a0) ? arg_val_decay($1)a0 : (ttany)0;')
define(`body_fn_set', `SetValue_ttobj((opaque)o, this`'_$4, (ttany)opaque_ifpointer($3)$4);')
define(`body_fn_fixedchange', `ChangeValue_ttobj((opaque)o, this`'_$4, this`'_$4_$5, this`'_$4_$6);')

define(`def_handy', `
arg_decl_decay($1) TT`'decl_name($2,$3,$4)_`'this`'(decl_args($@)) {
    body($@)
}')

define(`wrap_export', `ifelse(index(`$3', `r'), -1, `', `exported($1,,get,$2,$1)')`'dnl
`'ifelse(index(`$3', `w'), -1, `', `exported(void,,set,$2,$1)')')

define(`el', `

/* handy $1 methods */
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


