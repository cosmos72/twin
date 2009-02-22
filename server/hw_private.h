/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef _TWIN_HW_PRIVATE_H
#define _TWIN_HW_PRIVATE_H

/*
 * this file exports "private" stuff from hw.c,
 * i.e. intended to be used only by HW/hw_*.c drivers
 */

extern dat (*ChangedVideo)[2][2];
extern byte ChangedVideoFlag;

extern dat CursorX, CursorY;
extern uldat CursorType;

extern dat DisplayWidth, DisplayHeight;

extern struct termios ttysave;

void FillOldVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, hwattr Attr);
void saveDisplaySize(void);

void NeedRedrawVideo(dat Left, dat Up, dat Right, dat Down);

extern display_hw HW;
	    
#define SaveHW	display_hw s_hw = HW

#define SetHW(hw)	(HW = (hw))

#define RestoreHW	(HW = s_hw)
	
#endif /* _TWIN_HW_PRIVATE_H */

