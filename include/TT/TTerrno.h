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

#define TT_EUNKNOWN_TARGET	1
#define TT_EALREADY_CONN	2
#define TT_EXLIB_SIZES		3

/* this is a libTT internal value. Do not use it in clients! */
#define TT_MAX_ERROR	256

typedef struct tt_errno {
    uldat E;
    uldat S;
    /* there may actually be more fields */
} tt_errno;

#ifdef __cplusplus
 extern "C" {
#endif

tt_errno *TTErrnoLocation(void);

#define TTErrno			(TTErrnoLocation()->E)
#define TTErrnoDetail		(TTErrnoLocation()->S)

TW_CONST byte *TTStrError(uldat e) TW_FNATTR_CONST;
TW_CONST byte *TTStrErrorDetail(uldat e, uldat s) TW_FNATTR_CONST;

#ifdef __cplusplus
 }
#endif

#endif /* _TT_ERRNO_H */
