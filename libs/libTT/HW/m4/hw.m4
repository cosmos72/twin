




dnl This file is m4/hw.m4h : m4 macros to autogenerate *_m4.c
dnl Tell the user about this.
/* This file was automatically generated from target()_c by m4/hw.m4h, do not edit! */


/*
 *  target()_m4.c  --  implementation of libTT methods on "target()" `target' display
 *
 *  Copyright (C) 2001,2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */


include(`m4/TTclasses.m4')

divert(-1)

dnl
dnl                                 DESIGN PROBLEM
dnl
dnl we have to deal with a multipath inheritance, and there is no automatic way
dnl to determine which is the default implementation of methods that are not explicitly
dnl overridden by display targets.
dnl
dnl let's try to explain with an example and a picture:
dnl
dnl the base driver null_c defines method `SetXY' for ttwidget saying DEF(SetXY_ttwidget)
dnl then later overrides the same method `SetXY' for ttscrollpane saying DEF(SetXY_ttscrollpane)
dnl
dnl the derivate driver twin_c does exactly the same:
dnl says DEF(SetXY_ttwidget) and later DEF(SetXY_ttscrollpane); see the picture below:
dnl 
dnl null_c:  DEF(SetXY_ttwidget)       DEF(SetXY_ttscrollpane)
dnl 
dnl twin_c:  DEF(SetXY_ttwidget)       DEF(SetXY_ttscrollpane)
dnl
dnl now, calling FNSUPER(ttscrollpane)->SetXY() from twin_SetXY_ttscrollpane
dnl which function should invoke: twin_SetXY_ttwidget or null_SetXY_ttscrollpane?
dnl
dnl                                    SOLUTION
dnl
dnl solution: we implement a double inheritance:
dnl FNDEFAULT(ttscrollpane)->SetXY() calls null_SetXY_ttscrollpane, while
dnl FNSUPER(ttscrollpane)->SetXY() calls twin_SetXY_ttwidget.
dnl
dnl also, in case twin_SetXY_ttscrollpane was not defined,
dnl calling TTSetXY_ttwidget() on a ttscrollpane results in a call to
dnl null_SetXY_ttscrollpane, which if needed can call twin_SetXY_ttwidget
dnl simply as FNSUPER(ttscrollpane)->SetXY().
dnl

define(`HWDEF',`define(`hw_$1')target()_$1')

define(`DEF',`define(`mtd_$1')target()_$1')

define(`BUILD',`$1 DEF(Build_$1)($1 o)')
define(`BREAK',`void DEF(Break_$1)($1 o)')
define(`NEW',`$1 DEF(New_$1)(ttclass_$1 Class, $1 o)')
define(`DEL',`void DEF(Del_$1)($1 o)')

define(`SUPER',`m4_super_$1')

define(`FNDEFAULT',`TClass_default($1)')
define(`FNSUPER',`TClass(m4_super_$1)')

define(`NEWDEFAULT',`FNDEFAULT($1)->New(Class, o)')
define(`NEWSUPER',`($1)FNSUPER($1)->New((ttclass_`'m4_super_$1)Class, (m4_super_$1)o)')

define(`DELDEFAULT',`FNDEFAULT($1)->Del(o)')
define(`DELSUPER',`FNSUPER($1)->Del((m4_super_$1)o)')


define(`DECL_BEFORE', `

/* order is crucial here! */

#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

/* include our internal copy of TT.h instead of the `public' one */
#include "TT.h"
#include <TT/TTerrno.h>

#include "mutex.h"

#include "TTextern.h"
#include "TTassert.h"

#include "utils.h"
#include "inlines.h"
#include "seterrno.h"
#include "theme.h"
#include "common_m4.h"

static s_ttclasses target()_TTClasses;

')

dnl m4Class_*, m4id_*, m4magicmask_*, m4magic_*, m4size_* /* and m4_super_class_* */ must always be defined!
define(`el',`dnl
`'`'define(`m4Class_$1')`'dnl
`'`'define(`m4id_$1')`'dnl
`'`'define(`m4order_$1')`'dnl
`'`'define(`m4magicmask_$1')`'dnl
`'`'define(`m4magic_$1')`'dnl
`'`'define(`m4size_$1')`'dnl
')
TTlist()
undefine(`el')


define(`softfail', `NULL /* UNDEFINED: $1 */')

define(`hardfail', `NULL
`#'error: undefined target()_$1
')

define(`fail', `ifelse(target(), `null', `hardfail($1)', `softfail($1)')')


define(`DECL_AFTER', `

ifdef(`hwfail', `', `define(`hwfail', `hardfail')')

/*
 * use default values for methods not implemented in target()_c
 *
 * null display `target' MUST IMPLEMENT ALL METHODS, even if as stubs.
 */

define(`extends',`TTdef_ttclass_$'`1($'`2)
    /* `extends' ttclass_$'`1 */
')

define(`superfield', `ifdef(`mtd'_$'`1_$'`3, `NULL /* inherits:  '$'`1`_'$'`2` >> '$'`1`_'$'`3` */', dnl
`ifelse('$'`3`, `ttobj', `fail(''$'`1``_''$'`2``)', `superfield(''$'`1``, ''$'`2``, m4_super_''$'`3``)')')')

define(`protected', `ifdef(`m4'$'`1_$'`2, $'`1_$'`2, `ifdef(`mtd'_'$'`1`_'$'`2`, target()_'$'`1`_'$'`2`, dnl
`(void *)superfield(''$'`1``, ''$'`2``, m4_super_''$'`2``)')'),
')
define(`public_set',`protected($'`@)')
define(`public',`protected($'`@)')

define(`el',`
  {
TTdef_ttclass_$'`1($'`1)
  },
')

static s_ttclasses target()_TTClasses = {

TTlist()
undefine(`el')`'dnl
define(`public')`'dnl
define(`public_set')`'dnl
define(`protected')`'dnl
define(`extends')`'dnl
dnl
dnl
define(`TTHWlist',
  `el(Sync) el(Flush) el(TimidFlush) el(MainLoopOnce) el(DeleteCallback) dnl
   el(Close) el(ConnectionFd) el(GetErrno) el(GetErrnoDetail) dnl
   el(StrError) el(StrErrorDetail) dnl
  ')
dnl
define(`el',`ifdef(`hw'_$'`1, `
    target()_'$'`1`,', `
    hwfail('$'`1`),')')
  {
TTHWlist()
  },
};
undefine(`el')

')

divert

include(target()_c)

