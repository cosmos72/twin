




dnl This file is `m4/methods.m4' : m4 macros to autogenerate methodsm4.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/methods.m4', do not edit! */


/*
 *  methodsm4.h  --  actual methods implementation for libTT types
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TT_METHODSM4_H
#define _TT_METHODSM4_H

include(`m4/m4_TTtypes.m4')

define(`DEF',`define(`m4$1')$1')

define(`NEW',`$1 DEF(New_$1)(ttfn_$1 FN)')
define(`SUPER',`($1)FN->FN_`'m4super_$1`'->New((ttfn_`'m4super_$1)FN)')
define(`REALIZE',`$1 DEF(Realize_$1)($1 o)')
define(`DEL',`void DEF(Del_$1)($1 o)')
define(`DELSUPER',`o->FN->FN_`'m4super_$1`'->Del((m4super_$1) o)')


include(`methods_base.h')


dnl m4magicbit_*, m4magic_* and m4size_* must always be defined!
dnl define sizeof(object) for all objects
define(`el',`define(`m4magicbit_$1')define(`m4magic_$1')define(`m4size_$1')define(`size_$1', `sizeof(struct s_$1)')')
TTlist()
undefine(`el')

/* use default values for methods not implemented in methods_base.h */
define(`extends',`TTFNdef_$1($2)
    &s_TTFN_$1,')
define(`superfield',`ifdef(`m4$2_$3',`$2_$3',`ifelse(`$3',`ttobj',`/*WARNING: undefined:*/$2_$3',`superfield(`',`$2',m4super_$3)')')')
define(`protected',`ifdef(`m4$2_$3',`$2_$3',`(void *)superfield(,`$2',m4super_$3)'),')
define(`public',`protected($@)')
define(`el',`
static struct s_ttfn_$1 s_TTFN_$1 = {
    TTFNdef_$1($1)
};
ttfn_$1 TTFN_$1 = &s_TTFN_$1;
')
TTlist()
undefine(`el')
define(`public')
define(`protected')
define(`extends')




#endif /* _TT_METHODSM4_H */

