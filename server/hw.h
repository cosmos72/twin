#ifndef _TW_HW_H
#define _TW_HW_H


#define setFlush() (All->NeedHW |= NEEDFlushStdout)
#define clrFlush() (All->NeedHW &= ~NEEDFlushStdout)


void SendHotKey(window *Window);


byte StdAddEventMouse(udat CodeMsg, udat ShiftFlags, udat Code, dat MouseX, dat MouseY);
#if 0
byte GetShortShiftFlags(void);
#endif

void StrategyReset(void);
byte Strategy4Video(dat Xstart, dat Ystart, dat Xend, dat Yend);
extern byte StrategyFlag;
extern frac_t StrategyDelay;
/* strategy */
#define HW_UNSET  0
#define HW_ACCEL  1
#define HW_BUFFER 2
#define HW_DELAY  3

void DirtyVideo(dat Xstart, dat Ystart, dat Xend, dat Yend);
void FillVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, udat Attrib);
void DragArea(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);
void ResizeDisplay(void);

void InitSignals(void);

byte doAttachHW(byte len, byte *arg, byte tostderr);
void doDetachHW(void); /* cannot fail */


byte InitHW(byte *arg);
void QuitHW(void);
void DevNullStderr(void);

extern hwattr *Video;
extern byte ChangedVideoFlagAgain;


typedef struct display_hw display_hw;

struct display_hw {
    void (*FlushVideo)(void);
    void (*FlushHW)(void);

    void (*KeyboardEvent)(int fd, uldat slot);
    void (*MouseEvent)(int fd, uldat slot);
    
    void (*SetCursorType)(uldat CursorType);
    void (*MoveToXY)(udat x, udat y);
    void (*Beep)(void);

    byte SoftMouse;
    void (*ShowMouse)(void);
    void (*HideMouse)(void);

    void (*DetectSize)(udat *x, udat *y);
    
    void (*ImportClipBoard)(byte Wait);
    void (*ExportClipBoard)(void);
    void *PrivateClipBoard;
	
    byte (*canDragArea)(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);
    void (*DragArea)(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);
    /*
     * if the display HW is capable of doing BLiTs (BLock image Transfers) under
     * certain conditions (tipically X11's XCopyArea()), set canDragArea to a
     * function that checks if the HW can do the BLiT with the given coordinates,
     * and set DragArea to a function that unconditionally does the BLiT
     * (it will very probably need to FlushVideo() first)
     */

    byte NeedOldVideo;
    /*
     * set NeedOldVideo to TRUE if FlushVideo() is a bit expensive,
     * and it's better to cache the actual display contents in OldVideo[]
     * and send only what effectively changed, instead of all the dirty areas.
     * 
     * Note that there is no automatic updating scheme for OldVideo[]
     * (except for HW accelerated operations - currently only DragArea())
     * and that FlushVideo() is responsible of keeping it up to date.
     */
    
    byte ExpensiveFlushVideo;
    /*
     * set ExpensiveFlushVideo to TRUE if FlushVideo() is SO expensive
     * that it's better to sleep a little before flushing,
     * hoping to receive more data in the meantime,
     * in order to merge the flush operations.
     */

    uldat merge_Threshold;
    /*
     * if sending many small draw commands to the HW is more expensive
     * than sending fewer, bigger ones even considering you will also send
     * intermediate, clean areas, set merge_Threshold to a reasonable value
     * for merge: dirty areas less far than this will be considered as merged
     * by Threshold_isDirtyVideo().
     * Anyway, it's up to FlushVideo() to actually merge them.
     * 
     * Otherwise, set this to zero.
     */
    
    void (*QuitHW)(void);
    void (*QuitKeyboard)(void);
    void (*QuitMouse)(void);
    void (*QuitVideo)(void);
};

extern display_hw *HW;


#endif /* _TW_HW_H */
