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
extern byte ValidOldVideo, ChangedVideoFlagAgain;
extern byte ExpensiveFlushVideo, NeedHW;

extern byte StrategyFlag;
extern frac_t StrategyDelay;
/* strategy */
#define HW_UNSET  0
#define HW_ACCEL  1
#define HW_BUFFER 2
#define HW_DELAY  3

void StrategyReset(void);
byte Strategy4Video(dat Xstart, dat Ystart, dat Xend, dat Yend);


void DirtyVideo(dat Xstart, dat Ystart, dat Xend, dat Yend);
void FillVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, udat Attrib);
void DragArea(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);
void RefreshVideo(void);
void ResizeDisplay(void);
void ResizeDisplayPrefer(display_hw *);

void MoveToXY(udat x, udat y);
void SetCursorType(uldat type);
void ImportClipBoard(byte Wait);
void ExportClipBoard(void);
void PanicHW(void);
void FlushHW(void);
void BeepHW(void);
void ConfigureHW(udat resource, byte todefault, udat value);
void SetPaletteHW(udat N, udat R, udat G, udat B);
void ResetPaletteHW(void);
void UpdateFlagsHW(void);
void RunNoHW(void);

void InitSignals(void);
void InitTtysave(void);

byte InitDisplayHW(display_hw *);
void QuitDisplayHW(display_hw *);

display_hw *AttachDisplayHW(uldat len, byte *arg, uldat slot);
byte DetachDisplayHW(uldat len, byte *arg);

byte InitHW(void);
void QuitHW(void);

byte RestartHW(byte verbose);
void SuspendHW(byte verbose);

void SendHotKey(window *Window);

byte StdAddEventMouse(udat CodeMsg, udat ShiftFlags, udat Code, dat MouseX, dat MouseY);
#if 0
byte GetShortShiftFlags(void);
#endif

#endif /* _TW_HW_H */
