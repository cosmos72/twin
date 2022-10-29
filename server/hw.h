/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_HW_H
#define TWIN_HW_H

#define setFlush() (HW->NeedHW |= NEEDFlushHW)
#define clrFlush() (HW->NeedHW &= ~NEEDFlushHW)

#define HW_KBDAPPLIC 0x0001
#define HW_ALTCURSKEYS 0x0002
#define HW_BELLPITCH 0x0003
#define HW_BELLDURATION 0x0004
#define HW_MOUSEMOTIONEVENTS 0x0005
#define HW_CONFIGURE_MAX 0x0006

extern display_hw DisplayHWCTTY;
#define HWCTTY_DETACHED ((display_hw)1)

extern tcell *Video, *OldVideo;
extern bool NeedOldVideo, ValidOldVideo;
extern bool ExpensiveFlushVideo;
extern bool CanDragArea, ChangedVideoFlagAgain;
extern bool QueuedDrawArea2FullScreen;
extern byte NeedHW; // various flags

extern VOLATILE bool GotSignals;
bool InitSignals(void) NOTHROW;
void HandleSignals(void);
void QuitSignals(void) NOTHROW;
void AllDefaultSignals(void) NOTHROW; // set all signal handlers to SIG_DFL
bool InitTtysave(void) NOTHROW;

struct termios;
void InitTtyStruct(int fd, termios &ttyb) NOTHROW;

void DirtyVideo(dat Xstart, dat Ystart, dat Xend, dat Yend) NOTHROW;
void DragArea(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);

void MoveToXY(dat x, dat y) NOTHROW;
void SetCursorType(uldat type) NOTHROW;

#endif /* TWIN_HW_H */
