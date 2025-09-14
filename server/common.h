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

char *CloneStr(const char *s) NOTHROW;
char *CloneStrL(const char *s, uldat len) NOTHROW;

byte AllHWCanDragAreaNow(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
void DragAreaHW(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);

void ResizeDisplayPrefer(Tdisplay);

void PanicHW(void);
void FlushHW(void);
void BeepHW(void);
void ConfigureHW(udat resource, byte todefault, udat value);
void SetPaletteHW(udat N, udat R, udat G, udat B);
void ResetPaletteHW(void);

Tobj TwinSelectionGetOwner(void);
void TwinSelectionSetOwner(Tobj Owner, tany Time, tany Frac);
#define SEL_CURRENTTIME ((tany)0)
void TwinSelectionNotify(Tobj Requestor, uldat ReqPrivate, e_id Magic, Chars mime, Chars data);
void TwinSelectionRequest(Tobj Requestor, uldat ReqPrivate, Tobj Owner);

void SelectionExport(void);
void SelectionImport(void);
bool SelectionStore(e_id magic, Chars mime, Chars data);
byte SelectionAppend(Chars data);
byte SelectionAppendRune(trune rune);
byte SelectionAppendRunes(TRunes runes);

bool KeyboardEventCommon(Tdisplay hw, udat Code, udat ShiftFlags, udat Len, const char *Seq);
byte MouseEventCommon(Tdisplay hw, dat x, dat y, dat dx, dat dy, udat IdButtons);

dat GetDisplayWidth(void) NOTHROW;
dat GetDisplayHeight(void) NOTHROW;

byte InitTransUser(void);

/*
 * these comes either from display.c or from main.c/util.c
 */
void Quit(int status);
void gainPrivileges(void);

#ifndef SS
#define SS "%." STR(TW_SMALLBUFF) "s"
#endif

int flushk(void);

/*
 * this comes either from display.c or from remote.c
 */
void RemotePidIsDead(pid_t pid);

#endif /* TWIN_HW_COMMON_H */
