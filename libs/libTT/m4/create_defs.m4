

dnl This file is `m4/exports_util.m4' : m4 macros to autogenerate exports_util_m4.h
dnl Tell the user about this.


/* This file was automatically generated from `m4/exports_util.m4', do not edit! */


/*
 *  exports_util_m4.h  --  exported wrappers around libTT methods
 *
 */

include(`m4/TTclasses.m4h')

define(`arg_decl', `$3')
define(`args_decl', `ifelse($2, 0, `', $2, 1, `arg_decl($@)', `arg_decl($@), args_decl(incr($1), decr($2), NSHIFT(3, $@))')')

define(`extends')
define(`exported',`ifelse(substr(`$2', 0, 6), `Create', `
`#'define $2_$3		TTD.CREATE.$2_$3')')
define(`protected',`ifelse(substr(`$2', 0, 6), `Create', `
`#'define $2_$3		TFN_$3->$2')')
define(`el',`TTFNdef_$1($1)')
TTlist()
undefine(`el')
define(`protected')
define(`exported')
define(`extends')

