/*
 *  libTwerrno.h  --  error handling stuff for libTw
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _LIB_TW_ERRNO_H
#define _LIB_TW_ERRNO_H

extern uldat TwErrno;
#define TW_EX_ENDIAN	((uldat)1)
#define TW_EX_SIZES	((uldat)2)
#define TW_ELOST_CONN	((uldat)3)
#define TW_EALREADY_CONN ((uldat)4)
#define TW_ENO_DISPLAY	((uldat)5)
#define TW_ECANT_CONN	((uldat)6)
#define TW_ENO_MEM	((uldat)7)
#define TW_ECANT_WRITE	((uldat)8)
#define TW_ENO_FUNCTION	((uldat)9)
#define TW_EWEIRD	((uldat)10)
#define TW_ENO_AUTH	((uldat)11)
#define TW_EDENIED	((uldat)12)
#define TW_EBAD_GZIP	((uldat)13)
#define TW_EINTERNAL_GZIP ((uldat)14)

byte *TwStrError(uldat e);
byte TwInPanic(void);

#endif /* _LIB_TW_ERRNO_H */
