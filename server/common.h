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

obj *TwinSelectionGetOwner(void);
void TwinSelectionSetOwner(obj *Owner, time_t Time, frac_t Frac);
#define SEL_CURRENTTIME ((time_t)0)
void TwinSelectionNotify(obj *Requestor, uldat ReqPrivate, uldat Magic, byte MIME[MAX_MIMELEN],
			    uldat Len, byte *Data);
void TwinInternalSelectionNotify(obj *Requestor, uldat ReqPrivate);
void TwinSelectionRequest(obj *Requestor, uldat ReqPrivate, obj *Owner);

void SelectionExport(void);
void SelectionImport(void);
#ifndef SelectionAppend
# define SelectionAppend(Len, Data) SelectionStore(SEL_APPEND, NULL, Len, Data)
#endif
byte SelectionStore(uldat Magic, byte MIME[MAX_MIMELEN], uldat Len, byte *Data);

byte KeyboardEventCommon(udat Code, udat Len, byte *Seq);
byte MouseEventCommon(dat x, dat y, dat dx, dat dy, udat IdButtons);

udat GetDisplayWidth(void);
udat GetDisplayHeight(void);

byte InitTransUser(void);

/*
 * this comes either from display.c or from main.c
 */
void Quit(int status);

#endif /* _TW_HW_COMMON_H */

