




dnl This file is `m4/TT.m4' : m4 macros to autogenerate TTm4.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/TT.m4', do not edit! */


/*
 *  TTm4.h  --  internal header for libTT types declarations
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TT_M4_H
#define _TT_M4_H

include(`m4/m4_TTtypes.m4')

/* declarations for objects structures */
define(`extends',`/* `extends' $1 */
    TTdef_$1($1)')
define(`field',`$1 $2;')
define(`el',`
struct s_`'$1 {
    ttfn_$1 FN;
    TTdef_$1($1)
};
')
TTlist()
undefine(`el')
define(`field')
define(`extends')


/* declarations for methods structures */
define(`extends',`TTFNdef_$1($2)
    ttfn_$1 FN_$1;')
define(`protected',`$1;')
define(`public',`protected($@)')
define(`el',`
struct s_ttfn_$1 {
    TTFNdef_$1($1)
};
')
TTlist()
undefine(`el')
define(`public')
define(`protected')
define(`extends')

#endif /* _TT_M4_H */
