




dnl This file is `m4/tt.m4hh' : m4 macros to autogenerate various target.h headers
dnl Tell the user about this.
/* This file was automatically generated from `m4/tt.m4hh', do not edit! */


/*
 *  target.h  --  public header for libTT declarations related to "target"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

define(`TARGET', `translit(target, `a-z', `A-Z')')

`#'ifndef _TT_`'TARGET`'_H
`#'define _TT_`'TARGET`'_H

divert(-1)

define(`c_doxygen', `
$@')dnl
define(`c_comment', `
$@')dnl

include(`m4/TTclasses.m4')
divert
divert(-1)
include(`m4/TTclient.m4')
include(`m4/TTdecay.m4')
include(`m4/TTfield.m4')
divert

/* object structure */
`#'ifndef s_`'target
define(`el', `TTdef_$1($1)
        struct s_$1;
typedef struct s_$1 *	$1;')
el(target)
undefine(`el')
`#'endif /* s_`'target */

define(`c_comment')

/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of target */
extern ttclass	TTClass_`'target;


dnl /* #defines for objects hierarchy */
dnl define(`extends',`
dnl /** the superclass of $2 */
dnl `#'define TT_super_$2	$1')dnl
dnl define(`field')dnl
dnl define(`el', `TTdef_$1($1)')dnl
dnl el(target)dnl
dnl undefine(`el')dnl
dnl define(`field')dnl
dnl define(`extends')dnl

divert(-1)

define(`_G0', `ifelse(`$1', `', `', `$1 ')`'$2')
define(`_G',  `ifelse(`$1', `', `', `$1 ')`'$2 $3')

define(`m4TTdeclare',`define(`m4TT$1_$2',`TT$1_$2')
$4 TT$1_$2(ifelse($3, 0, `void', `NSHIFT(4, $@)'));
')
define(`extends')
define(`public',`m4TTdeclare($@)')
define(`public_set',`public($@)')
define(`final',`public($@)')
define(`final_var',`public($@)')
define(`el', `TTdef_ttclass_$1($1)')

divert

/* target methods */
el(target)

divert(-1)

undefine(`el')
define(`final_var')
define(`final')
define(`public_set')
define(`public')
define(`extends')


define(`_G0', `$@')
define(`_G',  `$@')

define(`getset_method', `
`'/** $1 ``$2'' of target ``o'' */
`'c_arg_decl($5,$6)	TT`'Toupper2($1,$2)_`'this`'(c_args_decl(NSHIFT(7, $@)));
')

divert

define(`el', `

/* TTGet*_$1 and TTSet*_$1 methods */
define(`this', `$1')`'TTdef_$1($1)')
define(`extends')
define(`field', `getset_field($@)')
el(target)
define(`final')
define(`extends')
define(`field')
undefine(`el')



divert(-1)



dnl 
dnl define(`m4TTbasesuper',`ifdef(`m4TT$2_$3',`m4TT$2_$3',`m4TTbasesuper(,`$2',m4_super_$3)')')
dnl 
dnl /* methods inherited by derivate classes */
dnl /* set #defines pointing to the most primitive class implementing the method */
dnl define(`extends')
dnl define(`public',`
dnl `#'define TT$2_$3	m4TTbasesuper(,$2,$3)')
dnl define(`public_set',`public($@)')
dnl define(`final',`public($@)')
dnl define(`mysuper',`ifelse($1,ttobj,`',`mysuper(m4_super_$1,$2)')`'TTdef_ttclass_$1($2)')
dnl define(`el',`
dnl 
dnl /* methods inherited by $1 */dnl
dnl mysuper(m4_super_$1,$1)')
dnl 
dnl TTnlist()
dnl undefine(`el')
dnl undefine(`mysuper')
dnl define(`final')
dnl define(`public_set')
dnl define(`public')
dnl define(`extends')
dnl 

divert

`#'endif /* _TT_`'TARGET`'_H */

