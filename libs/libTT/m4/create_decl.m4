

dnl This file is `m4/exports_init.m4' : m4 macros to autogenerate exports_initm4.h
dnl Tell the user about this.


/* This file was automatically generated from `m4/exports_init.m4', do not edit! */


/*
 *  exports_initm4.h  --  exported wrappers around libTT methods
 *
 */

include(`m4/TTclasses.m4h')

define(`arg_decl', `$3')
define(`args_decl', `ifelse($2, 0, `', $2, 1, `arg_decl($@)', `arg_decl($@), args_decl(incr($1), decr($2), NSHIFT(3, $@))')')

define(`extends')
define(`exported',`ifelse(substr(`$2', 0, 6), `Create', `
    $5 (*$2_$3)(ifelse($4, 0, void, `args_decl(1, $4, NSHIFT(5, $@))'));')')
define(`el',`TTFNdef_$1($1)')
TTlist()
undefine(`el')
define(`exported')
define(`extends')

