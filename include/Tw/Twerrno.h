/*
 *  Twerrno.h  --  error handling stuff for libTw
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TW_ERRNO_H
#define _TW_ERRNO_H

#include <Tw/Twtypes.h>

#define TW_EX_ENDIAN	 1
#define TW_EX_SIZES	 2
#define TW_ELOST_CONN	 3
#define TW_EALREADY_CONN 4
#define TW_ENO_DISPLAY	 5
#define TW_EBAD_DISPLAY	 6
#define TW_ECANT_CONN	 7
#define TW_ENO_MEM	 8
#define TW_ECANT_WRITE	 9
#define TW_ENO_FUNCTION	10
#define TW_ESTRANGE	11
#define TW_ENO_AUTH	12
#define TW_EDENIED	13
#define TW_EBAD_GZIP	14
#define TW_EINTERNAL_GZIP	15
#define TW_ENO_HOST		16
#define TW_EBAD_FUNCTION	17
#define TW_EX_PROTOCOL		18
#define TW_ENO_SOCKET		19
#define TW_ESTRANGE_CALL	20
#define TW_EFAILED_CALL		21
#define TW_EFAILED_ARG_CALL	22
#define TW_EXLIB_SIZES		23
#define TW_EXLIB_STRUCT_SIZES	24

#define TW_ENO_MODULE_DETAIL	1

typedef struct tw_errno {
    uldat E;
    uldat S;
    /* there may actually be more fields */
} tw_errno;

#ifdef __cplusplus
 extern "C" {
#endif

tw_errno *Tw_ErrnoLocation(tdisplay TwD);

#define Tw_Errno(TwD)		(Tw_ErrnoLocation(TwD)->E)
#define Tw_ErrnoDetail(TwD)	(Tw_ErrnoLocation(TwD)->S)

TW_FN_ATTR_CONST TW_CONST byte *Tw_StrError(TW_CONST tdisplay TwD, uldat e);
TW_FN_ATTR_CONST TW_CONST byte *Tw_StrErrorDetail(TW_CONST tdisplay TwD, uldat e, uldat s);

#define TwErrnoLocation		Tw_ErrnoLocation(Tw_DefaultD)
#define TwErrno			Tw_Errno(Tw_DefaultD)
#define TwErrnoDetail		Tw_ErrnoDetail(Tw_DefaultD)
#define TwStrError(e)		Tw_StrError(Tw_DefaultD, (e))
#define TwStrErrorDetail(e, s)	Tw_StrErrorDetail(Tw_DefaultD, (e), (s))

#ifdef __cplusplus
 }
#endif

#endif /* _TW_ERRNO_H */
