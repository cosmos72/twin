#ifndef _TW_COMMON_H
#define _TW_COMMON_H

/*
 * this file exports functions and variables common to both
 * display.c and the whole twin sources, in particular hw_multi.c
 */

byte *CloneStr(byte *s);
byte *CloneStrL(byte *s, uldat len);

byte AllHWCanDragAreaNow(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
void DragAreaHW(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);

void ResizeDisplayPrefer(display_hw *);

void PanicHW(void);
void FlushHW(void);
void BeepHW(void);
void ConfigureHW(udat resource, byte todefault, udat value);
void SetPaletteHW(udat N, udat R, udat G, udat B);
void ResetPaletteHW(void);

void ImportClipBoard(byte Wait);
void ExportClipBoard(void);
byte *GetClipData(void);
uldat GetClipLen(void);

#ifndef AddToClipBoard
# define AddToClipBoard(Len, Data) SetClipBoard(CLIP_APPEND, Len, Data)
#endif
byte SetClipBoard(uldat Magic, uldat Len, byte *Data);

byte KeyboardEventCommon(udat Code, udat Len, byte *Seq);
byte MouseEventCommon(dat x, dat y, dat dx, dat dy, udat IdButtons);

byte InitTransUser(void);

/*
 * this comes either from display.c or from main.c
 */
void Quit(int status);

#endif /* _TW_HW_COMMON_H */

