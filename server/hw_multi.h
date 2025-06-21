/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_HW_MULTI_H
#define TWIN_HW_MULTI_H

/*
 * this file exports functions and variables defined in hw_multi.c
 * that are not exported from display.c
 */

extern byte StrategyFlag;
extern tany StrategyDelay;
/* strategy */
#define HW_UNSET 0
#define HW_ACCEL 1
#define HW_BUFFER 2
#define HW_DELAY 3

void StrategyReset(void) NOTHROW;
byte Strategy4Video(dat Xstart, dat Ystart, dat Xend, dat Yend) NOTHROW;

byte InitHW(void);
void QuitHW(void);

byte RestartHW(byte verbose);
void SuspendHW(byte verbose);

Tdisplay AttachDisplayHW(Chars arg, uldat slot, byte flags);
bool DetachDisplayHW(Chars arg, byte flags);

void FillVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, tcell Attr);
void RefreshVideo(void);
byte ResizeDisplay(void);

void RunNoHW(byte print_info);
void UpdateFlagsHW(void) NOTHROW;

void EnableMouseMotionEvents(byte enable);

byte StdAddMouseEvent(Tdisplay hw, udat Code, dat MouseX, dat MouseY);
void SyntheticKey(Twidget widget, udat Code, udat ShiftFlags, byte Len, const char *Seq);

#endif /* TWIN_HW_MULTI_H */
