



dnl This file is m4/libTw2.m4 : m4 macros to autogenerate libTw2m4.h
dnl Tell the user about this.
/* This file was automatically generated with m4 from m4/libTw2.m4, do not edit! */

divert(-1)

define(`TRIM', `translit(`$1', ` ')')

define(`CHAIN', `TRIM($1)`'TRIM($2)')

define(`NAME', `Tw_`'CHAIN($1,$2)')

define(`NSHIFT', `ifelse($1, 0, `shift($@)', `NSHIFT(decr($1), shift(shift($@)))')')

define(`tv', v)
define(`t_', _)
define(`tx', x)
define(`tV', V)
define(`tW', W)
define(`tX', X)
define(`tY', Y)

define(`iV', `$1')
define(`iW', `$1')
define(`iX', `$1')
define(`iY', `$1')

define(`Tv', `void ')
define(`T_', `$1 ')
define(`Tx', `t$1 ')
define(`TV', `TW_CONST $1 *')
define(`TW', `TW_CONST $1 *')
define(`TX', `TW_CONST t$1 *')
define(`TY', `TW_CONST t$1 *')

define(`TYPE', `T$2($1)')

define(`A', `a$1')

define(`NARGS', `eval(($1-5)/2)')
define(`NARGS1', `eval(1+($1-5)/2)')

define(`ARG', `ifelse($3, v, `', `TYPE($2,$3)`'A($1)')')

define(`_ARGS', `ifelse($#, 2, `', `, ARG($1,$2,t$3)`'_ARGS(incr($1), NSHIFT(3, $@))')')
define(`ARGS', `tw_d TwD`'ifelse($#, 2, `', `_ARGS($@)')')

define(`PARSE_', `, c(A($1))')
define(`PARSEx', `PARSE_($@)')
define(`PARSEV', `, c(($3) * sizeof($2)), (TW_CONST void *)A($1)')
define(`PARSEW', `PARSEV($@)')
define(`PARSEX', `, c(($3) * sizeof(tobj)), (TW_CONST void *)A($1)')
define(`PARSEY', `PARSEX($@)')

define(`PARSE', `PARSE$3($1,$2,$4)')
define(`SPACE', `SPACE$3($1,$2,$4)')

define(`PARSES', `ifelse($#, 2, `', `
    PARSE($1,$2,t$3,i$3)`'PARSES(incr($1), NSHIFT(3, $@))')')

define(`CALL', `EncodeCall($1, order_`'$2, TwD $3);')

define(`FL_NOLOCK', `ENCODE_FL_NOLOCK')
define(`FL_RETURN', `ifelse(`$2', v, ENCODE_FL_VOID, 0)')

define(`RETv', `')
define(`RET_', `return ($1)')
define(`RETx', `return (tobj)')
define(`RET',  `RET$2($1)')

define(`PROTO', `TYPE($1,t$2)`'NAME($3, $4)(ARGS(1, NSHIFT(5, $@))) {
    RET($1, $2)`'CALL(FL_RETURN($1, $2), CHAIN($3, $4), `PARSES(1, NSHIFT(5, $@))')
}
')

define(`PROTOSyncSocket', `static TYPE($1,t$2)_`'NAME($3, $4)(ARGS(1, NSHIFT(5, $@))) {
    RET($1, $2)`'CALL(FL_NOLOCK|FL_RETURN($1, $2), CHAIN($3, $4), `PARSES(1, NSHIFT(5, $@))')
}
')

define(`PROTOFindFunction', `PROTOSyncSocket($@)')

divert

#define c(a) (tlargest)a

include(`m4/sockproto.m4h')

#undef c

