



dnl This file is `m4/socket2macros.m4' : m4 macros to autogenerate socket2macros.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/socket2macros.m4', do not edit! */

define(`TRIM', `translit(`$1', ` ')')

define(`NSHIFT', `ifelse($1, 0, `shift($@)', `NSHIFT(decr($1), shift(shift($@)))')')

define(`CHAIN', `TRIM($1)`'TRIM($2)')

define(`tv', v)
define(`t_', _)
define(`tx', x)
define(`tV', V)
define(`tW', W)

define(`ARG', `"$3"ifelse($3, x, TRIM($2)_magic_STR, TWS_`'TRIM($2)_STR)')

define(`PARSE', `ifelse($#, 2, `', `ARG($1,$2,t$3)`'PARSE(incr($1), NSHIFT(3, $@))')')

define(`PROTO', `{ 0, 0, "CHAIN($3, $4)",
    "$5"ARG(0, $1, $2)`'PARSE(1, NSHIFT(5, $@)) },')

define(`PROTOSyncSocket', defn(`PROTO'))
define(`PROTOFindFunction', defn(`PROTO'))

include(`m4/sockproto.m4')

