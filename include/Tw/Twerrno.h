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

#define TW_EX_ENDIAN	((uldat)1)
#define TW_EX_SIZES	((uldat)2)
#define TW_ELOST_CONN	((uldat)3)
#define TW_EALREADY_CONN ((uldat)4)
#define TW_ENO_DISPLAY	((uldat)5)
#define TW_EBAD_DISPLAY	((uldat)6)
#define TW_ECANT_CONN	((uldat)7) /* see errno for details about this error */
#define TW_ENO_MEM	((uldat)8)
#define TW_ECANT_WRITE	((uldat)9)
#define TW_ENO_FUNCTION	((uldat)10)
#define TW_ESTRANGE	((uldat)11)
#define TW_ENO_AUTH	((uldat)12)
#define TW_EDENIED	((uldat)13)
#define TW_EBAD_GZIP	((uldat)14)
#define TW_EINTERNAL_GZIP	((uldat)15)
#define TW_ENO_HOST		((uldat)16)
#define TW_EBAD_FUNCTION	((uldat)17)
#define TW_EX_PROTOCOL		((uldat)18)
#define TW_ENO_SOCKET		((uldat)19)
#define TW_ESTRANGE_CALL	((uldat)20)
#define TW_EFAILED_CALL		((uldat)21)
#define TW_EFAILED_ARG_CALL	((uldat)22)
#define TW_EXLIB_SIZES		((uldat)23)

typedef struct tw_errno {
    uldat E;
    uldat S;
    /* there may actually be more fields */
} tw_errno;

#ifdef __cplusplus
 extern "C" {
#endif

uldat Tw_Errno(tdisplay TwD);
tw_errno *Tw_ErrnoLocation(tdisplay TwD);

/*
 * This is tricky. We want the function Tw_Errno() to return
 * the VALUE of error for binary compatibility,
 * yet new programs should use Tw_ErrnoLocation(), which returns
 * the ADDRESS of error variables, so that they can be assigned to.
 * Thus we hide the function Tw_Errno() with a macro.
 */
#define Tw_Errno(TwD)		(Tw_ErrnoLocation(TwD)->E)
#define Tw_ErrnoDetail(TwD)	(Tw_ErrnoLocation(TwD)->S)

TW_CONST byte *Tw_StrError(TW_CONST tdisplay TwD, uldat e) TW_FNATTR_CONST;
TW_CONST byte *Tw_StrErrorDetail(TW_CONST tdisplay TwD, uldat e, uldat s) TW_FNATTR_CONST;

#define TwErrnoLocation		Tw_ErrnoLocation(Tw_DefaultD)
#define TwErrno			Tw_Errno(Tw_DefaultD)
#define TwErrnoDetail		Tw_ErrnoDetail(Tw_DefaultD)
#define TwStrError(e)		Tw_StrError(Tw_DefaultD, (e))
#define TwStrErrorDetail(e, s)	Tw_StrErrorDetail(Tw_DefaultD, (e), (s))

#ifdef __cplusplus
 }
#endif

#endif /* _TW_ERRNO_H */
