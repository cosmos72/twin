#ifndef _TW_HW_H
#define _TW_HW_H

#define setFlush() (HW->NeedHW |= NEEDFlushHW)
#define clrFlush() (HW->NeedHW &= ~NEEDFlushHW)

#define HW_KBDAPPLIC	((udat)0x0001)
#define HW_ALTCURSKEYS	((udat)0x0002)
#define HW_BELLPITCH    ((udat)0x0003)
#define HW_BELLDURATION ((udat)0x0004)

extern display_hw *DisplayHWCTTY;
#define HWCTTY_DETACHED ((display_hw *)1)

extern hwattr *Video, *OldVideo;
extern byte NeedOldVideo, ValidOldVideo;
extern byte ExpensiveFlushVideo, NeedHW;
extern byte CanDragArea, ChangedVideoFlagAgain;

byte InitSignals(void);
void QuitSignals(void);
byte InitTtysave(void);


void DirtyVideo(dat Xstart, dat Ystart, dat Xend, dat Yend);
void DragArea(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);

void MoveToXY(udat x, udat y);
void SetCursorType(uldat type);

#endif /* _TW_HW_H */
