



dnl This file is `m4/socklist.m4' : m4 macros to autogenerate socklistm4.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/socklist.m4', do not edit! */

define(`TRIM', `translit(`$1', ` ')')

define(`PROTO', `EL(TRIM($3)`'TRIM($4))')

define(`PROTOFindFunction', `PROTO($@)')
define(`PROTOSyncSocket', `PROTO($@)')

include(`m4/m4_sockproto.m4')

