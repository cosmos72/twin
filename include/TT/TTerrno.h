/*
 *  TTerrno.h  --  error handling stuff for libTT
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TT_ERRNO_H
#define _TT_ERRNO_H

#include <TT/TTtypes.h>

#define TT_EBAD_SIZES		1
#define TT_ENO_MEM		2
#define TT_ETARGET_ALREADY_OPEN	20
#define TT_ETARGET_DLERROR	21
#define TT_ETARGET_FAILED	22
#define TT_ETARGET_BAD		23
#define TT_ETARGET_NO_EXTENSION	40
#define TT_ETARGET_BAD_EXTENSION_VERSION 41

#define TT_EBAD_ARG		60
#define TT_EEXIST		61

#define TT_EDETAIL_TARGET_TWIN_TUNNEL	1
#define TT_EDETAIL_TARGET_TWIN_DETUNNEL 2
#define TT_EDETAIL_TARGET_TWIN		3
#define TT_EDETAIL_TARGET_GTK		4
#define TT_EDETAIL_TARGET_X11		5
#define TT_EDETAIL_TARGET_XML		6
#define TT_EDETAIL_TARGET_NULL		7

typedef struct tt_errno {
    /** error number */
    ttuint E;
    /** error detail number */
    ttuint S;
    /* there may actually be more fields */
} tt_errno;

#ifdef __cplusplus
 extern "C" {
#endif

/* get address of struct `tt_errno' describing last error */
tt_errno *TTErrnoLocation(void);

/* get error number describing last error */
#define TTErrno			(TTErrnoLocation()->E)
/* get error detail number describing last error */
#define TTErrnoDetail		(TTErrnoLocation()->S)

/* get human-readable string describing error number `e' */
TT_FN_ATTR_CONST TT_CONST byte *TTStrError(ttuint e);
/* get human-readable string describing error number `e' and error detail number `s' */
TT_FN_ATTR_CONST TT_CONST byte *TTStrErrorDetail(ttuint e, ttuint s);

#ifdef __cplusplus
 }
#endif

#endif /* _TT_ERRNO_H */
