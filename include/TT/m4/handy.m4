
dnl This file is `m4/handy.m4' : m4 macros to autogenerate handy_m4.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/handy.m4', do not edit! */



/*
 *  handy_m4.h  --  public header for handy libTT functions
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 */

#ifndef _TT_HANDY_M4_H
#define _TT_HANDY_M4_H

include(`m4/TThandy.m4h')

divert(-1)

define(`tolower', `translit(`$1', `A-Z', `a-z')')
define(`toupper', `translit(`$1', `a-z', `A-Z')')
define(`Toupper', `toupper(substr(`$1', 0, 1))`'patsubst(substr(`$1', 1), `_\(\w\)',``'toupper(`\1')')')

define(`decl_name', `ifelse(`$1', `', `Toupper($2)`'Toupper($3)', `Toupper($1)')')
define(`decl_args', `this o`'decl_fn_$3(NSHIFT(3, $@))')

define(`decl_fn_get', `')
define(`decl_fn_set', `, $2 $1')
define(`decl_fn_fixedchange', `')

define(`decl_handy', `
$1	TT`'decl_name($2,$3,$4)_`'this`'(decl_args($@));')

define(`wrap_export', `ifelse(index(`$3', `r'), -1, `', `exported($1,,get,$2,$1)')`'dnl
`'ifelse(index(`$3', `w'), -1, `', `exported(void,,set,$2,$1)')')

divert

define(`el', `

/* handy $1 methods */
define(`this', `$1')`'TThandy_$1($1,$1)')
define(`extends')
define(`exported', `decl_handy($@)')
define(`field', `wrap_export($@)')
define(`exported_fields', `TTdef_$1($1,$1)')
TTlist()
define(`exported_fields')
define(`exported')
define(`extends')
define(`field')
undefine(`el')


#endif /* _TT_HANDY_M4_H */
