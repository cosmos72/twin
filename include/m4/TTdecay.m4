/*  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */
dnl first, non-decay stuff

define(`c_arg_type', `addspace_ifnotempty($1)`'$2')

define(`c_arg_decl', `c_arg_type($1,$2) $3')
define(`c_args_decl', `ifelse(`$2', `', `', `$5', `', `c_arg_decl($1,$2,$3)', `c_arg_decl($1,$2,$3), c_args_decl(NSHIFT(3, $@))')')

define(`c_arg_call', `$3')
define(`_c_args_call', `ifelse($3, `', `', `, c_arg_call($1,$2,$3)`'_c_args_call(NSHIFT(3, $@))')')
define(`c_args_call', `ifelse($3, `', `', $6, `', `c_arg_call($1,$2,$3)', `c_arg_call($1,$2,$3)`'_c_args_call(NSHIFT(3, $@))')')


define(`isaddress', `ifelse(regexp(`$2', `\(^tt_class\|^tt_obj\|_fn$\|\*\)'), -1, `', `1')')
define(`opaque_ifaddress', `ifelse(isaddress(`$1',`$2'), `1', `(opaque)')')
define(`opaque_ifobject_or_address', `ifdef(`m4_super_$2', (opaque), `ifdef(`m4_super_ttclass_$2', (opaque), `opaque_ifaddress($@)')')')
define(`_R_ifaddress', `ifelse(index(`$1', _R), -1, `ifelse(`isaddress(`$1',`$2')', `1', `_R ')')')
define(`addspace_ifnotempty', `ifelse(`$1', `', `', `$1 ')')


dnl then, decay stuff

define(`c_arg_type_decay', `addspace_ifnotempty($1)`'ifdef(`m4_super_$2', tt_obj, `ifdef(`m4_super_ttclass_$2', tt_class, $2)')')
define(`c_arg_val_decay', `ifdef(`m4_super_$2', (tt_obj)(opaque), `ifdef(`m4_super_ttclass_$2', (tt_class)(opaque), `(c_arg_type($1,$2))opaque_ifaddress($1,$2)')')')
define(`c_arg_ret_decay', `opaque_ifaddress($1,$2)')
define(`c_arg_ret_undecay_ifnotvoid', `ifelse($2, void, `', `opaque_ifobject_or_address($@)')')

define(`obj2id_ifnotvoid_ifdecay', `ifelse($1, void, $2, `ifdef(`m4_super_$1', `OBJ2ID($2)', $2)')')
define(`tt_obj_obj2id_ifnotvoid_ifdecay', `ifelse($1, void, $2, `ifdef(`m4_super_$1', `(tt_obj)OBJ2ID($2)', $2)')')



define(`c_arg_decl_decay', `c_arg_type_decay($1,$2) $3')
define(`c_args_decl_decay', `ifelse($2, `', `', $5, `', `c_arg_decl_decay($1,$2,$3)', `c_arg_decl_decay($1,$2,$3), c_args_decl_decay(NSHIFT(3, $@))')')

define(`c_arg_call_decay', `ifdef(`m4_super_$2', `ID2($2,$3)', `ifdef(`m4_super_$2', `ID2CLASS($3)', $3)')')
define(`_c_args_call_decay', `ifelse($3, `', `', `, c_arg_call_decay($1,$2,$3)`'_c_args_call_decay(NSHIFT(3, $@))')')
define(`c_args_call_decay', `ifelse($3, `', `', $6, `', `c_arg_call_decay($1,$2,$3)', `c_arg_call_decay($1,$2,$3)`'_c_args_call_decay(NSHIFT(3, $@))')')


define(`c_decl_decay_a0_ifnotvoid', `ifelse($2, void, `', `c_arg_decl_decay($1,$2,a0) = c_arg_val_decay($1,$2)0;
    ')')
define(`a0_eq_ifnotvoid', `ifelse($2, void, `', `a0 = ')')
define(`a0_ifnotvoid', `ifelse($2, void, `', `a0')')


define(`type_function_decay', `ifelse(regexp(`$1', `_fn$'), -1, `$1', ttfunction_fn)')

define(`trim_star_n_space', `translit(`$1', ` *')')

dnl define(`type_attributes', `patsubst(`$1', `^ *\(\([^ ]+ +\)*\)\([a-zA-Z0-9_]+\) *\*? *$', `\1')')
dnl define(`trim_attributes', `patsubst(`$1', `^ *\(\([^ ]+ +\)*\)\([a-zA-Z0-9_]+\) *\*? *$', `\2')')

