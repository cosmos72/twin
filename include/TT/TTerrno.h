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

#define TT_EXLIB_SIZES		1
#define TT_EALREADY_CONN	2
#define TT_ENO_MEM		3
#define TT_EDLERROR_TARGET	4
#define TT_EUNKNOWN_TARGET	5
#define TT_EFAILED_TARGET	6
#define TT_EBAD_TARGET		7
#define TT_EVERSION_TARGET	8


#define TT_EBAD_ARG		20
#define TT_EEXIST		21


typedef struct tt_errno {
    ttuint E;
    ttuint S;
    /* there may actually be more fields */
} tt_errno;

#ifdef __cplusplus
 extern "C" {
#endif

tt_errno *TTErrnoLocation(void);

#define TTErrno			(TTErrnoLocation()->E)
#define TTErrnoDetail		(TTErrnoLocation()->S)

TT_FN_ATTR_CONST TT_CONST byte *TTStrError(ttuint e);
TT_FN_ATTR_CONST TT_CONST byte *TTStrErrorDetail(ttuint e, ttuint s);

#ifdef __cplusplus
 }
#endif

#endif /* _TT_ERRNO_H */
