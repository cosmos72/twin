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

#define TW_EBAD_SIZES			1
#define TW_EBAD_STRUCTS			2
#define TW_ENO_DISPLAY	 		4
#define TW_EBAD_DISPLAY	 		5
#define TW_ENO_HOST			6
#define TW_ENO_AUTH			7
#define TW_ESYS_NO_MEM	 		8
#define TW_ESYS_NO_SOCKET		9
#define TW_ESYS_CANNOT_CONNECT		10
#define TW_ESYS_CANNOT_WRITE		11
#define TW_ESERVER_BAD_VERSION		12
#define TW_ESERVER_BAD_PROTOCOL		13
#define TW_ESERVER_BAD_ENDIAN		14
#define TW_ESERVER_BAD_SIZES		15
#define TW_ESERVER_LOST_CONNECT		16
#define TW_ESERVER_ALREADY_CONNECT	17
#define TW_ESERVER_DENIED_CONNECT	18
#define TW_ESERVER_NO_FUNCTION		19
#define TW_ESERVER_BAD_FUNCTION		20
#define TW_ESERVER_BAD_RETURN		21
#define TW_EGZIP_BAD_PROTOCOL		22
#define TW_EGZIP_INTERNAL		23
#define TW_ECALL_BAD			24
#define TW_ECALL_BAD_ARG		25


#define TW_EDETAIL_NO_MODULE	1

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
