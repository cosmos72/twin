/*
 *  Tw1.h  --  single-head compatibility defines and prototypes for libTw
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TW1_H
#define _TW1_H


/*
 * libTw.so.1 --- single head compatibility. these are too handy to drop them.
 */
extern tdisplay Tw_DefaultD;

#define TwOpen(d)			(!!(Tw_DefaultD = Tw_Open(d)))
#define TwClose()			(Tw_Close(Tw_DefaultD), Tw_DefaultD = (tdisplay)0)


#include <Tw/common1_m4.h>
#include <Tw/proto1_m4.h>
#include <Tw/alias1_m4.h>

#endif /* _TW1_H */

