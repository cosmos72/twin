



dnl This file is `m4/socklist.m4' : m4 macros to autogenerate socklist_m4.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/socklist.m4', do not edit! */

define(`TRIM', `translit(`$1', ` ')')


define(`PROTO', `EL(TRIM($3)`'TRIM($4))')

define(`PROTOFindFunction', `PROTO($@)')
define(`PROTOSyncSocket', `PROTO($@)')

#define OK_MAGIC	((uldat)0x3E4B4F3Cul)
#define FAIL_MAGIC	((uldat)0xFFFFFFFFul)

#define FIND_MAGIC		((uldat)0x646E6946ul) /* i.e. "Find" */
#define MSG_MAGIC		((uldat)0x2167734dul) /* i.e. "Msg!" */

include(`m4/sockproto.m4h')

