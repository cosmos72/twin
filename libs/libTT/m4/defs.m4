




dnl This file is `m4/defs.m4' : m4 macros to autogenerate defs_m4.h
dnl Tell the user about this.
/* This file was automatically generated from `m4/defs.m4', do not edit! */


/*
 *  defs_m4.h  --  INTERNAL (!!) header for libTT types declarations
 *
 *  Note: you cannot include both this file and <TT/defs_m4.h>, as they conflict!
 */

#ifndef _TT_DEFSM4_H
#define _TT_DEFSM4_H

include(`m4/TTclasses.m4h')

/* many useful macros */

#define Min2(a,b) ((a) < (b) ? (a) : (b))

#define TT_OFFSETOF(type,field)	((size_t)&((type)0)->field)

/* #defines for objects hierarchy */
define(`extends',`
`#'define super_$2	$1')
define(`field')
define(`el',`TTdef_$1($1)')
TTlist()
undefine(`el')
define(`field')
define(`extends')


typedef struct s_ttobj TT_CONST *	ttobj_c;

/* forward declarations for objects structures */
define(`el',`
typedef struct s_$1 *   $1;')
TTlist()
undefine(`el')


/* forward declarations for methods structures */
define(`el',`
typedef struct s_ttfn_$1 *	ttfn_$1;')
TTlist()
undefine(`el')

typedef ttfn_ttobj ttfn;


/* declarations for objects structures */
define(`extends',`TTdef_$1($2)`'dnl
    /* `extends' $1 */
')
define(`field',`$1 $2;
')
define(`el',`
typedef struct s_$1 {
    TTdef_$1($1)} s_$1;
')
TTlist()
undefine(`el')
define(`field')
define(`extends')


/* declarations for methods structures */
define(`extends',`TTFNdef_$1($2)
    ttfn_$1 FN_$1;
')
define(`protected',`$1;
')
define(`public',`protected($@)')
define(`el',`
typedef struct s_ttfn_$1 {
    TTFNdef_$1($1)
} s_ttfn_$1;
')
TTlist()
undefine(`el')
define(`public')
define(`protected')
define(`extends')

typedef struct s_tthw *tthw;
typedef struct s_tthw {
    byte (*Sync)(void);
    byte (*Flush)(void);
    byte (*TimidFlush)(void);
    byte (*MainLoop)(void);
    byte (*MainLoopOnce)(byte);
    void (*ExitMainLoop)(void);
    void (*DeleteListener)(ttlistener);

    void (*Close)(void);
    int (*ConnectionFd)(void);
    
    ttuint (*GetErrno)(void);
    ttuint (*GetErrnoDetail)(void);
    TT_CONST byte *(*StrError)(ttuint E);
    TT_CONST byte *(*StrErrorDetail)(ttuint E, ttuint S);

} s_tthw;

define(`el',`
    s_ttfn_$1 fn_$1;')
typedef struct s_ttfns *ttfns;
typedef struct s_ttfns {
TTlist()
    s_tthw HW;
} s_ttfns;
undefine(`el')

define(`el',`
extern tt_fn TTFN_$1;')

/* client-visible global types */
TTtypelist()
#define TTFN_ttsbyte	TTFN_ttbyte
#define TTFN_ttushort	TTFN_ttshort
#define TTFN_ttuint	TTFN_ttint

/* client-visible global methods structures */
TTlist()

undefine(`el')

/* #defines for global methods structures */
#define TFNs	(TTD.FN_)

#define THW	(TTD.FN.HW)

define(`el',`
`#'define TFN_$1	(&TTD.FN.fn_$1)')
TTlist()
undefine(`el')

#endif /* _TT_DEFSM4_H */

