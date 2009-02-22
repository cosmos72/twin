/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

#ifndef _TT_EXTERN_H
#define _TT_EXTERN_H


#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

#include "mutex.h"


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
    ttobj **IdList;
    ttopaque *IdSize, *IdTop, *IdBottom;
    
#ifdef CONF_SOCKET_PTHREADS
    s_tt_errno *(*GetErrnoLocation)(void);
#endif
    ttbyte	(*AssignId)(TT_CONST ttclass Class, ttobj Obj);
    void	(*DropId)(ttobj Obj);
    ttobj	(*Id2Obj)(ttopaque ClassId, ttopaque Id);
    ttbyte	(*FixedAssignId)(TT_CONST ttclass Class, ttobj Obj);
    ttbyte	(*FixedAssignIdArray)(ttopaque ClassId, ttopaque o_n, TT_CONST ttobj *o_array);
    ttobject	(*FindNative)(ttany id);
    void	(*GetNow)(void);
    
    void	(*FireEvent)(ttevent ev, ttcomponent o);
    void	(*FireSimpleEvent)(ttcomponent o, ttuint evtype);
    void	(*FireChangeEvent)(ttcomponent o, ttuint which, ttany value, ttany old_value, ttopaque len);
    
    void	(*AddTo_ttlistener)(ttlistener c, ttcomponent o);
    void	(*Remove_ttlistener)(ttlistener c);
    void	(*Activate_tttimer)(tttimer o, ttbyte active);
    
    void	(*AddTo_ttdata)(ttdata d, ttcomponent o, ttbyte quickndirty);
    void	(*Remove_ttdata)(ttdata d);
    ttbyte	(*SetData_ttdata)(ttdata o, ttany data);
    ttdata	(*FindByKey_ttdata)(ttdata base, TT_CONST ttbyte *key, ttopaque len);
    ttdata	(*protected_Create_ttdata)(ttcomponent c, TT_ARG_READ ttbyte *key, ttopaque key_len, ttany data);
    
    void	(*DelAllExtras_ttcomponent)(ttcomponent o);
    
    void	(*Expose_ttvisible)(ttvisible o, ttshort x, ttshort y, ttshort w, ttshort h);
    
    void	(*RealClose)(void);
    
} s_ttutil;

typedef struct s_ttcreates {
#include "create_decl_m4.h"
} s_ttcreates;

typedef struct s_tt_d {
#ifdef CONF_SOCKET_PTHREADS
    th_r_mutex mutex;
#endif
    ttuint refcount;
    
    tttheme Theme, DummyTheme;
    ttmenubar Menubar;
    ttapplication Application;

    tteventmask InstalledEM, DefaultEM;

    /* events queued */
    ttevent FirstE, LastE;
    /* listeners in progress */
    ttlistener FirstL, LastL;
    
    /* active timers */
    tttimer FirstT, LastT;
    ttany TNow;
    ttany FNow;
    
    ttbyte *HWTarget, *HWOptions;
    void *DlHandle;
    
    ttbyte OpenFlag, PanicFlag, ExitMainLoopFlag, QuitArg;
    
    s_tt_errno rCommonErrno_;
    s_tt_errno_vec rErrno_;
    TT_CONST byte *str_dlerror;
    
    s_ttcreates CREATE;
    s_ttutil UTIL;
    
    ttclasses Class_hw_null;
    
    ttclass Classes[order_last];
    
    s_ttclasses Class;
    s_ttclasses Class_default;
} s_tt_d;


extern s_tt_d TTD;

#define mutex			(TTD.mutex)
#define rErrno			(TTD.rErrno_)
#define rCommonErrno		(TTD.rCommonErrno_)
#define CommonErrno		(TTD.rCommonErrno_.E)
#define CommonErrnoDetail	(TTD.rCommonErrno_.S)

#define TT_MUTEX_HELPER_DEFS(attr) \
    attr void Lock(void) { \
	th_r_mutex_lock(mutex); \
	TTD.refcount++; \
    } \
    attr void Unlock(void) { \
	/* do not automatically call RealClose if in panic */ \
	if (!--TTD.refcount && TTD.OpenFlag && !TTD.PanicFlag) \
	    RealClose(); \
	th_r_mutex_unlock(mutex); \
    } \
    TH_R_MUTEX_HELPER_DEFS(attr)


#define LOCK Lock()
#define UNLK Unlock()

#define FAIL_TT(E, S)		(CommonErrno = (E), CommonErrnoDetail = (S), FALSE)
#define FAIL(E, S)		FAIL_TT(TT_MAX_ERROR+(E), (S))
#define FAIL_PRINT(E, S, name)	(FAIL((E), (S)), _TTPrintInitError(name), FALSE)

#endif /* _TT_EXTERN_H */

