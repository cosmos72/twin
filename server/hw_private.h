
#ifndef _TW_HW_PRIVATE_H
#define _TW_HW_PRIVATE_H

/*
 * this file exports "private" stuff from hw.c,
 * i.e. intended to be used only by HW/hw_*.c drivers
 */

extern dat (*ChangedVideo)[2][2];
extern byte ChangedVideoFlag;

extern udat CursorX, CursorY;
extern uldat CursorType;

extern udat ScreenWidth, ScreenHeight;

extern struct termios ttysave;

void VideoFlipMouse(void);
void FillOldVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, hwattr Attr);
void saveDisplaySize(void);

void NeedRedrawVideo(udat Left, udat Up, udat Right, udat Down);

extern display_hw *HW;
	    
#define SaveHW	display_hw *s_hw = HW

#define SetHW(hw)	(HW = (hw))

#define RestoreHW	(HW = s_hw)
	
#endif /* _TW_HW_PRIVATE_H */

