



dnl This file is `m4/paranoiamacros.m4' : m4 macros to autogenerate paranoiamacros.h
dnl Tell the user about this.
/* This file was automatically generated with `m4' from `m4/paranoiamacros.m4', do not edit! */

/*
 *  paranoiamacros.m4  --  check libTw functions prototypes from sockproto.h
 *			   against twin.h or against libTw.h include files.
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#if defined(_TW_H)
# define CONST		TW_CONST
# define PREFIX_VOID	tdisplay
# define PREFIX		tdisplay,
#elif defined(_TWIN_H)
# define PREFIX_VOID	void
# define PREFIX
#else
# error you must #include at least one of "twin.h" or "Tw/Tw.h"
#endif

define(`Act')

define(`TRIM', `translit(`$1', ` ')')

define(`CHAIN', `TRIM($1)`'TRIM($2)')

define(`NAME', `Tw_`'CHAIN($1,$2)')

define(`NSHIFT', `ifelse($1, 0, `shift($@)', `NSHIFT(decr($1), shift(shift($@)))')')

define(`PREFIX_ANY', `ifelse($1, 5, PREFIX_VOID, PREFIX)')

define(`tv', v)
define(`t_', _)
define(`tx', x)
define(`tV', V)
define(`tW', W)

define(`Tv', `void ')
define(`T_', `$1 ')
define(`Tx', `t$1 ')
define(`TV', `CONST $1 *')
define(`TW', `CONST $1 *')

define(`TYPE', `T$2($1)')

define(`A', `a$1')

define(`ARG', `ifelse($3, v, `', `TYPE($2, $3)`'A($1)')')

define(`_ARGS', `ifelse($#, 2, `', `, ARG($1,$2,t$3)`'_ARGS(incr($1), NSHIFT(3, $@))')')
define(`ARGS', `ifelse($#, 2, `', `ARG($1,$2,t$3)`'ifelse($#, 4, `', `_ARGS(incr($1), NSHIFT(3, $@))')')')


define(`PROTO', `TYPE($1,$2) NAME($3, $4)(PREFIX_ANY($#) ARGS(1, NSHIFT(5, $@))) Act(fn,funct,name);')
define(`PROTOFindFunction', defn(`PROTO'))
define(`PROTOSyncSocket', defn(`PROTO'))

include(`m4/sockproto.m4')

