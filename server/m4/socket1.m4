



dnl This file is `m4/socket1.m4' : m4 macros to autogenerate socket1m4.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/socket1.m4', do not edit! */

define(`TRIM', `translit(`$1', ` ')')

define(`NSHIFT', `ifelse($1, 0, `shift($@)', `NSHIFT(decr($1), shift(shift($@)))')')

define(`tv', v)
define(`t_', _)
define(`tx', x)
define(`tV', V)
define(`tW', V)

define(`T_', uldat)
define(`Tx', obj)

define(`DECL0', `ifelse($2, v, `', `a[0].$2 = (T$2)')')

define(`ARG', `ifelse($1, 1, `', ``, '')`'ifelse($3, V, `($2 CONST *)a[$1].V', `($2)a[$1].$3')')

define(`PARSE', `ifelse($#, 2, `', `ARG($1,$2,t$3)`'PARSE(incr($1), NSHIFT(3, $@))')')

define(`CALL', `ifelse($4, 0, $2`'$3($6), $4, 1, Do($2,$3)(Fn$3, $6), Act($2,($5)a[1].x)($6));')

define(`PROTO', `case order_`'TRIM($3)`'TRIM($4):
    DECL0($1, t$2)`'CALL($1,TRIM($3),TRIM($4),$5,$6,PARSE(1, NSHIFT(5, $@)))
    break;
')

define(`PROTOSyncSocket', `PROTO($@)')
define(`PROTOFindFunction', `PROTO($@)')

include(`m4/m4_sockproto.m4')

