/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_SCROLLER_H
#define TWIN_SCROLLER_H

byte InitScroller(void);

extern Tmsgport Scroller_MsgPort;
extern msg Do_Scroll, Dont_Scroll;

#endif /* TWIN_SCROLLER_H */
