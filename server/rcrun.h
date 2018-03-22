/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_RCRUN_H
#define _TWIN_RCRUN_H

#define GLOBAL_MAX 7
extern node Globals[GLOBAL_MAX];
extern byte GlobalsAreStatic;

#define CallList	Globals[0]
#define FuncList	Globals[1]
#define MenuList	Globals[2]

#define ScreenList	Globals[3]
#define ScreenIndex	3

#define BorderList	Globals[4]
#define KeyList		Globals[5]
#define MouseList	Globals[6]

extern node *MenuBinds; /* array of pointers to nodes inside MenuList */
extern udat MenuBindsMax;

byte InitRC(void);
void QuitRC(void);

node LookupNodeName(str name, node head);

hwfont *RCFindBorderPattern(window W, byte Border);

byte RC_VM(timevalue *t);
byte RC_VMQueue(CONST wm_ctx *C);


#endif /* _TWIN_RCRUN_H */

