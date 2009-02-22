



/* This file was automatically generated from m4/limits.m4, do not edit! */

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

/*
 *  limits_m4.h  --  defines for various libTT hardcoded values and limits
 */


/* this is a libTT internal value. Do not use it in clients! */
#define TT_MAX_ERROR	256


#define TT_MAXID	0x00FFFFFF
#define TT_MAGIC_SHIFT	24 /* keep at least 8 bits available for order_* magic types */
#define TT_MAGIC_MASK	TT_MAXID

#define TT_SMALLBUFF	TW_SMALLBUFF
#define TT_BIGBUFF	TW_BIGBUFF

#define TTSB		TT_STR(TT_SMALLBUFF)
#define TTBB		TT_STR(TT_BIGBUFF)

#define TT_NOID		((opaque)0)
#define TT_BADID	TT_MAX_ttopaque
#define TT_NOFD		((int)-1)

#define TT_FALSE	FALSE
#define TT_TRUE		TRUE

#define TT_MIN2(a,b) ((a)<(b) ? (a) : (b))
#define TT_MAX2(a,b) ((a)>(b) ? (a) : (b))








/* max number of arguments a libTT function can have */
#define TT_MAX_ARG_N	11

