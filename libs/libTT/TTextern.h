
#ifndef _TT_EXTERN_H
#define _TT_EXTERN_H


/* typedef s_tt_errno */

#ifdef CONF_SOCKET_PTHREADS

typedef struct {
    ttuint E;
    ttuint S;
    th_self T;
} s_tt_errno;

typedef struct {
    s_tt_errno *vec;
    ttuint max, last;
} s_tt_errno_vec;

#else

typedef struct {
    ttuint E;
    ttuint S;
} s_tt_errno;

typedef s_tt_errno s_tt_errno_vec;

#endif


typedef struct s_ttutil *ttutil;
typedef struct s_ttutil {
#ifdef CONF_SOCKET_PTHREADS
    s_tt_errno *(*GetErrnoLocation)(void);
#endif
    ttfont *(*CloneStr2TTFont)(TT_CONST byte * s, size_t len);
    byte (*AssignId)(TT_CONST ttfn_ttobj FN, ttobj Obj);
    void (*DropId)(ttobj Obj);
    ttobj (*Id2Obj)(byte i, opaque Id);
    ttobj (*FindNative)(ttany id);
    
    void (*DispatchEvent)(ttcomponent o, ttevent ev, ttbyte dflags);
    void (*DispatchSimpleEvent)(ttcomponent o, ttuint evtype);
    
    void (*DelAllListeners_ttcomponent)(ttcomponent o);
    void (*DelAllCallbacks_ttcomponent)(ttcomponent o);
    
    ttcallback (*AddTo_ttcallback)(ttcallback c, ttcomponent o);
    void (*Remove_ttcallback)(ttcallback c);
    
    void (*Expose_ttvisible)(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h);
} s_ttutil;

typedef struct s_ttcreates {
#include "create_decl_m4.h"
} s_ttcreates;

typedef struct s_tt_d {
#ifdef CONF_SOCKET_PTHREADS
    th_r_mutex mutex;
#endif
    tttheme Theme, DummyTheme;
    ttmenubar Menubar;
    ttapplication Application;
    
    ttuint HWArgsN;
    byte **HWArgs;
    void *DlHandle;
    
    byte OpenFlag, ExitMainLoopFlag, PanicFlag;
    s_tt_errno rCommonErrno_;
    s_tt_errno_vec rErrno_;
    TT_CONST byte *str_dlerror;
    
    s_ttcreates CREATE;
    s_ttutil UTIL;
    
    ttfn_ttobj FN_[order_n];
    
    ttfns null_FNs;
    s_ttfns FN;
} s_tt_d;


extern s_tt_d TTD;

#define mutex			(TTD.mutex)
#define rErrno			(TTD.rErrno_)
#define rCommonErrno		(TTD.rCommonErrno_)
#define CommonErrno		(TTD.rCommonErrno_.E)
#define CommonErrnoDetail	(TTD.rCommonErrno_.S)

#define LOCK th_r_mutex_lock(mutex)
#define UNLK th_r_mutex_unlock(mutex)

#define FAIL(E, S)		(CommonErrno = TT_MAX_ERROR+(E), CommonErrnoDetail = (S), FALSE)
#define FAIL_PRINT(E, S, name)	(FAIL((E), (S)), _TTPrintInitError(name), FALSE)

#endif /* _TT_EXTERN_H */

