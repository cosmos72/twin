#ifndef _TW_HW_MULTI_H
#define _TW_HW_MULTI_H

/*
 * this file exports functions and variables defined in hw_multi.c
 * that are not exported from display.c
 */

extern byte StrategyFlag;
extern frac_t StrategyDelay;
/* strategy */
#define HW_UNSET  0
#define HW_ACCEL  1
#define HW_BUFFER 2
#define HW_DELAY  3

void StrategyReset(void);
byte Strategy4Video(dat Xstart, dat Ystart, dat Xend, dat Yend);

byte InitDisplayHW(display_hw *);
void QuitDisplayHW(display_hw *);

byte InitHW(void);
void QuitHW(void);

byte RestartHW(byte verbose);
void SuspendHW(byte verbose);

display_hw *AttachDisplayHW(uldat len, CONST byte *arg, uldat slot, byte flags);
byte DetachDisplayHW(uldat len, CONST byte *arg, byte flags);

void FillVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, udat Attrib);
void RefreshVideo(void);
byte ResizeDisplay(void);

void RunNoHW(void);
void UpdateFlagsHW(void);

byte StdAddEventMouse(udat CodeMsg, udat Code, dat MouseX, dat MouseY);
void SyntheticKey(window *Window, udat Code, udat ShiftFlags, byte Len, byte *Seq);

#endif /* _TW_HW_MULTI_H */

