




dnl This file is `m4/TTdefs.m4' : m4 macros to autogenerate TTdefsm4.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/TTdefs.m4', do not edit! */


/*
 *  TTdefsm4.h  --  public header for libTT types declarations
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TT_DEFSM4_H
#define _TT_DEFSM4_H

include(`m4/m4_TTtypes.m4')

#define TT_LIST(el) \
TTlist()

#define TT_NLIST(el) \
TTnlist()

/* typedefs for objects structures */
define(`el',`
typedef struct s_$1	* $1;')
TTlist()
undefine(`el')

/* typedefs for methods structures */
define(`el',`
typedef struct s_ttfn_$1	* ttfn_$1;')
TTlist()
undefine(`el')

/* global methods structures */
define(`el',`
extern ttfn_$1	TTFN_$1;')
TTlist()
undefine(`el')

/* #defines for objects hierarchy */
define(`extends',`
`#'define TTsuper_$2	$1')
define(`field')
define(`el',`TTdef_$1($1)')
TTlist()
undefine(`el')
define(`field')
define(`extends')

/* costructors */
define(`el',`
`#'define TTNew_$1()	TT_NEW($1)')
TTlist()
undefine(`el')

/* and destructors */
define(`el',`
`#'define TTDel_$1(o)	TT_DEL(o)')
TTlist()
undefine(`el')

/* `public' and `exported' methods */
define(`n_SHIFT',`ifelse($1,0,`shift($@)',`n_SHIFT(decr($1), shift(shift($@)))')')
define(`comma_n_ARGS',`ifelse($1,0,,`, a$1`'comma_n_ARGS(decr($1))')')
define(`n_ARGS',`ifelse($1,0,,`a$1`'ifelse($1,1,,`comma_n_ARGS(decr($1))')')')

define(`comma_cast_n_ARGS',`ifelse($1,0,,`, (($2)a$1)`'comma_cast_n_ARGS(decr($1),n_SHIFT(2, $@))')')
define(`cast_n_ARGS',`ifelse($1,0,,`(($2)a$1)ifelse($1,1,,`comma_cast_n_ARGS(decr($1),n_SHIFT(2, $@))')')')

define(`m4TTdeclare',`define(`m4TT$2_$3',`TT$2_$3(cast_n_ARGS($4, n_SHIFT(5, $@)))')
$5 TT$2_$3(n_SHIFT(5, $@));')

define(`el',`

/* $1 `public' and `exported' methods */TTFNdef_$1($1)')
define(`extends')
define(`public',`m4TTdeclare($@)')
define(`exported',`public($@)')
TTlist()
define(`exported')
define(`public')
define(`extends')
undefine(`el')

define(`m4TTbasesuper',`ifdef(`m4TT$2_$3',`m4TT$2_$3',`m4TTbasesuper(,`$2',m4super_$3)')')

/* methods inherited by derivate classes */
/* set #defines pointing to the most primitive class implementing the method */
define(`extends')
define(`public',`
`#'define TT$2_$3(n_ARGS($4)) (($5) m4TTbasesuper(,$2,$3))')
define(`exported',`public($@)')
define(`mysuper',`ifelse($1,ttobj,`TTFNdef_$1($2)',`mysuper(m4super_$1,$2)TTFNdef_$1($2)')')
define(`el',`

/* methods inherited by $1 */mysuper(m4super_$1,$1)')
TTnlist()
undefine(`el')
undefine(`mysuper')
define(`exported')
define(`public')
define(`extends')

#endif /* _TT_DEFSM4_H */

