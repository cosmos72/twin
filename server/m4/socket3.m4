



dnl This file is `m4/socket3.m4' : m4 macros to autogenerate socket3_m4.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/socket3.m4', do not edit! */

define(`TRIM', `translit(`$1', ` ')')

define(`NSHIFT', `ifelse($1, 0, `shift($@)', `NSHIFT(decr($1), shift(shift($@)))')')

define(`A', `a[$1]._')

define(`tV', `V')
define(`tW', `V')
define(`tX', `V')
define(`tY', `V')
define(`lV', `$1')
define(`lW', `$1')
define(`lX', `$1')
define(`lY', `$1')

define(`VEC', `ifelse($3, V, `NAME`'      case $1: L = $4; break;
`'define(`NAME')`'define(`ENDNAME', `    }
    break;')')')

define(`CASE', `ifelse($#, 2, `', `VEC(`$1', `$2', t`'$3, l`'$3)'`CASE(incr($1), NSHIFT(3, $@))')')

define(`PROTO', `define(`NAME', `  case order_`'TRIM($3)`'TRIM($4):
    switch (n) {
')`'define(`ENDNAME', `')`'CASE(1, NSHIFT(5, $@))`'ENDNAME')

define(`PROTOSyncSocket', `PROTO($@)')
define(`PROTOFindFunction', `PROTO($@)')

include(`m4/sockproto.m4h')

