/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_COMMON_H
#define TWIN_COMMON_H

#include "stl/view.h"

/*
 * this file exports functions and variables common to both
 * display.c and the whole twin sources, in particular hw_multi.c
 */

char *CloneStr(const char *s);
char *CloneStrL(const char *s, uldat len);

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
void TwinSelectionNotify(obj Requestor, uldat ReqPrivate, uldat Magic, const char MIME[MAX_MIMELEN],
                         Chars Data);
void TwinSelectionRequest(obj Requestor, uldat ReqPrivate, obj Owner);

void SelectionExport(void);
void SelectionImport(void);
#ifndef SelectionAppend
#define SelectionAppend(Data) SelectionStore(SEL_APPEND, NULL, Data)
#endif
byte SelectionStore(uldat Magic, const char MIME[MAX_MIMELEN], Chars Data);

byte KeyboardEventCommon(udat Code, udat ShiftFlags, udat Len, const char *Seq);
byte MouseEventCommon(dat x, dat y, dat dx, dat dy, udat IdButtons);

dat GetDisplayWidth(void);
dat GetDisplayHeight(void);

byte InitTransUser(void);

/*
 * these comes either from display.c or from main.c/util.c
 */
void Quit(int status);
void GainPrivileges(void);

#ifndef SS
#define SS "%." STR(TW_SMALLBUFF) "s"
#endif

int printk(const char *format, ...);
int flushk(void);

/*
 * this comes either from display.c or from remote.c
 */
void RemotePidIsDead(pid_t pid);

#endif /* TWIN_HW_COMMON_H */
