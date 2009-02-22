/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_HW_MULTI_H
#define _TWIN_HW_MULTI_H

/*
 * this file exports functions and variables defined in hw_multi.c
 * that are not exported from display.c
 */

extern byte StrategyFlag;
extern tany StrategyDelay;
/* strategy */
#define HW_UNSET  0
#define HW_ACCEL  1
#define HW_BUFFER 2
#define HW_DELAY  3

void StrategyReset(void);
byte Strategy4Video(dat Xstart, dat Ystart, dat Xend, dat Yend);

byte InitDisplayHW(display_hw);
void QuitDisplayHW(display_hw);

byte InitHW(void);
void QuitHW(void);

byte RestartHW(byte verbose);
void SuspendHW(byte verbose);

display_hw AttachDisplayHW(uldat len, CONST byte *arg, uldat slot, byte flags);
byte DetachDisplayHW(uldat len, CONST byte *arg, byte flags);

void FillVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, hwattr Attrib);
void RefreshVideo(void);
byte ResizeDisplay(void);

void RunNoHW(byte print_info);
void UpdateFlagsHW(void);

void EnableMouseMotionEvents(byte enable);

byte StdAddMouseEvent(udat Code, dat MouseX, dat MouseY);
void SyntheticKey(widget W, udat Code, udat ShiftFlags, byte Len, byte *Seq);

#endif /* _TWIN_HW_MULTI_H */

