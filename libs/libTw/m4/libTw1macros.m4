



dnl This file is `m4/libTw2macros.m4' : m4 macros to autogenerate libTw2macros.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/libTw2macros.m4', do not edit! */


/* To avoid troubles with strlen(), you should not use '\0' or "\0" */
/* Also, using TWS_obj is really a good idea */
#define magic_id_STR(x)	"\xFF"

define(`TRIM', `translit(`$1', ` ')')

define(`NSHIFT', `ifelse($1, 0, `shift($@)', `NSHIFT(decr($1), shift(shift($@)))')')

define(`CHAIN', `TRIM($1)`'TRIM($2)')

define(`tv', v)
define(`t_', _)
define(`tx', x)
define(`tV', V)
define(`tW', W)

define(`TWS_void', `0')

define(`ARG', `"$3"ifelse($3, x, magic_id_STR(TRIM($2)), TWS_`'TRIM($2)_STR)')

define(`PARSE', `ifelse($#, 2, `', `ARG($1,$2,t$3)`'PARSE(incr($1), NSHIFT(3, $@))')')

define(`PROTO', `{ Tw_`'CHAIN($3, $4), 0, 0, "Tw_`'CHAIN($3, $4)", "$5"ARG(0, $1, $2)`'PARSE(1, NSHIFT(5, $@)) },')

define(`PROTOSyncSocket', defn(`PROTO'))
define(`PROTOFindFunction', defn(`PROTO'))

include(`m4/sockproto.m4')

