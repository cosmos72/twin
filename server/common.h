/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_COMMON_H
#define _TWIN_COMMON_H

/*
 * this file exports functions and variables common to both
 * display.c and the whole twin sources, in particular hw_multi.c
 */

byte *CloneStr(CONST byte *s);
byte *CloneStrL(CONST byte *s, uldat len);

byte AllHWCanDragAreaNow(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
void DragAreaHW(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);

void ResizeDisplayPrefer(display_hw);

void PanicHW(void);
void FlushHW(void);
void BeepHW(void);
void ConfigureHW(udat resource, byte todefault, udat value);
void SetPaletteHW(udat N, udat R, udat G, udat B);
void ResetPaletteHW(void);

obj TwinSelectionGetOwner(void);
void TwinSelectionSetOwner(obj Owner, tany Time, tany Frac);
#define SEL_CURRENTTIME ((tany)0)
void TwinSelectionNotify(obj Requestor, uldat ReqPrivate, uldat Magic, CONST byte MIME[MAX_MIMELEN],
			    uldat Len, byte CONST * Data);
void TwinSelectionRequest(obj Requestor, uldat ReqPrivate, obj Owner);

void SelectionExport(void);
void SelectionImport(void);
#ifndef SelectionAppend
# define SelectionAppend(Len, Data) SelectionStore(SEL_APPEND, NULL, Len, Data)
#endif
byte SelectionStore(uldat Magic, CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data);

byte KeyboardEventCommon(udat Code, udat ShiftFlags, udat Len, CONST byte *Seq);
byte MouseEventCommon(dat x, dat y, dat dx, dat dy, udat IdButtons);

dat GetDisplayWidth(void);
dat GetDisplayHeight(void);

byte InitTransUser(void);

/*
 * these comes either from display.c or from main.c/util.c
 */
void Quit(int status);
void GainPrivileges(void);

int printk(CONST byte *format, ...);
int flushk(void);


/*
 * this comes either from display.c or from remote.c
 */
void RemotePidIsDead(pid_t pid);

#endif /* _TWIN_HW_COMMON_H */

