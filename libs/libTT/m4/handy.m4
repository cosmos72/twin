

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

/* handy `public', `public_set' and `exported' methods */


define(`decl_name', `ifelse(`$1', `', `Toupper($2)`'Toupper($3)', `Toupper($1)')')
define(`decl_args', `arg_decl_decay(this) o`'decl_fn_$3(NSHIFT(3, $@))')

define(`decl_fn_get', `')
define(`decl_fn_set', `, $2 $1')
define(`decl_fn_fixedchange', `')

define(`body', `body_fn_$3($@)')
define(`body_fn_get', `this a0;
    if ((a0 = ID2(this, o)))
	return obj2id_ifnotvoid_ifdecay($1, a0->$4);
    return arg_val_decay($1)0;
')
define(`body_fn_set', `TTSetValue_ttobj(o, this`'_$4, (ttany)opaque_ifpointer($3)$4);')
define(`body_fn_fixedchange', `TTChangeField_ttobj(o, this`'_$4, this`'_$4_$5, this`'_$4_$6);')

define(`def_handy', `
arg_decl_decay($1) TT`'decl_name($2,$3,$4)_`'this`'(decl_args($@)) {
    body($@)
}')

dnl define(`wrap_export', `ifelse(index(`$3', `r'), -1, `', `exported($1,,get,$2,$1)')`'dnl
dnl `'ifelse(index(`$3', `w'), -1, `', `exported(void,,set,$2,$1)')')
dnl
define(`wrap_export', `ifelse(index(`$3', `r'), -1, `', `exported($1,,get,$2,$1)')')


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

divert(-1)

undefine(`wrap_export')`'dnl
define(`wrap_export', `ifelse(index(`$3', `r'), -1, `', `exported($1,,get,$2,$1)')`'dnl
`'ifelse(index(`$3', `w'), -1, `', `exported(tt_fn,,set,$2,$1)')')

define(`type_ttfn', `ifelse(index(`$1', `*'), -1, ifelse(regexp(`$1', `_fn$'), -1, TTFN_$1, TTFN_ttfunction), index(`$1', ttbyte), -1, TTFN_ttpointer, TTFN_ttstring)')

define(`m4super_mtd', `ifdef(`mtd_$1_$2', `$2', `m4super_mtd($1, m4super_$2)')')

define(`reverse', `ifelse(`$1', `', `', `regexp(`$1', `\([^ ]+\)\(\( +[^ ]+\)*\)', `reverse(\2) \1')')')

define(`reverseTTlist', reverse(TTlist()))

divert

/* back-end functions for TTGetField_ttobj() */
define(`el', `
`'define(`this', $1)`'dnl
`'define(`extends')`'dnl
`'define(`this_count', 0)`'dnl
`'define(`field', `ifelse(index($'`3, r), -1, `', `define(`this_count', inc(this_count))')')`'dnl
`'TTdef_$1($1,$1)`'dnl
`'ifelse(this_count, 0, `', `
`'define(`mtd_GetField_$1')`'dnl
'static tt_fn handy_GetField_$1($1 o, ttuint which, ttany *value) {
    if (which && which < this()_field_last && o && TTAssert(IS($1,o))) {
	if (which > this()_field_first) switch (which) {
`'define(`exported', `ifelse($'`3, get, `
	  case this()_'$'`4`:
	    *value = (ttany)opaque_ifpointer('$'`1`)obj2id_ifnotvoid_ifdecay('$'`1`, o->'$'`4`); dnl
`'`'`'`'`'`'return type_ttfn('$'`1`);', `')')`'dnl
`'define(`field', `wrap_export($'`@)')`'dnl
`'TTdef_$1($1,$1)
	}
	ifelse($1, ttobj, `', `else
	    return handy_GetField_`'m4super_mtd(GetField,m4super_$1)((m4super_mtd(GetField,m4super_$1))o, which, value);')
    }
    return (tt_fn)0;
})`'dnl
`'define(`field')`'dnl
`'define(`exported')`'dnl
`'define(`extends')`'dnl
`'undefine(`this')`'dnl
`'undefine(`this_count')`'dnl
')
TTlist()
undefine(`el')

typedef tt_fn (*get_fn)(TT_ARG_READ ttobj, ttuint, ttany *);

static tt_fn GetField(TT_ARG_READ ttobj o, ttuint which, ttany *value) {
    static get_fn fn [] = {
	(get_fn)0,
define(`el', `
	(get_fn)handy_GetField_`'m4super_mtd(GetField,$1),')
TTlist()
undefine(`el')
    };
    ttuint i;
    
    if (TTAssert(o) && (i = o->id >> magic_shift) && i < sizeof(fn)/sizeof(fn[0])) {
	return fn[i]((ttobj)o, which, value);
    }
    return (tt_fn)0;
}

/* back-end function for TTSetField_ttobj() */
define(`el', `
`'define(`this', $1)`'dnl
`'define(`extends')`'dnl
`'define(`this_count', 0)`'dnl
`'define(`field', `ifelse(index($'`3, w), -1, `', `define(`this_count', inc(this_count))')')`'dnl
`'TTdef_$1($1,$1)`'dnl
`'ifelse(this_count, 0, `', `
`'define(`mtd_SetField_$1')`'dnl
'static tt_fn handy_SetField_$1($1 o, ttuint which, ttany value) {
    if (which && which < this()_field_last && o && TTAssert(IS($1,o))) {
	if (which > this()_field_first) switch (which) {
`'define(`exported', `ifelse($'`3, set, `
	  case this()_'$'`4`:
	    return TTSet`'Toupper('$'`4`)_`'this`'((tt_obj)o->id, value);', `')')`'dnl
`'define(`field', `wrap_export($'`@)')`'dnl
`'TTdef_$1($1,$1)
	}
	ifelse($1, ttobj, `', `else
	    return handy_SetField_`'m4super_mtd(SetField,m4super_$1)((m4super_mtd(SetField,m4super_$1))o, which, value);')
    }
    return (tt_fn)0;
})`'dnl
`'define(`field')`'dnl
`'define(`exported')`'dnl
`'define(`extends')`'dnl
`'undefine(`this')`'dnl
`'undefine(`this_count')`'dnl
')
TTlist()
undefine(`el')

typedef tt_fn (*set_fn)(TT_ARG_READ ttobj, ttuint, ttany);

static tt_fn SetField(TT_ARG_READ ttobj o, ttuint which, ttany value) {
    static set_fn fn [] = {
	(set_fn)0,
define(`el', `
	(set_fn)handy_SetField_`'m4super_mtd(SetField,$1),')
TTlist()
undefine(`el')
    };
    ttuint i;
    
    if (TTAssert(o) && (i = o->id >> magic_shift) && i < sizeof(fn)/sizeof(fn[0])) {
	return fn[i]((ttobj)o, which, value);
    }
    return (tt_fn)0;
}



