/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_WM_H
#define _TWIN_WM_H

typedef struct wm_ctx wm_ctx;

struct wm_ctx {
    widget W;  /* top level widget user is interacting with */
    screen Screen;
    menu Menu;
    menuitem Item;
    widget DW; /* deepest widget user is interacting with */
    udat Type; /* Msg->Type: MSG_* */
    byte Pos;  /* POS_* relative to (W) */
    byte ByMouse;
    udat Code; /* keyboard key / mouse buttons pressed */
    udat ShiftFlags;
    dat i, j;  /* mouse coordinates */
    ldat Left, Up, Rgt, Dwn; /* window (W) corners */
};


byte InitWM(void);

void AskCloseWidget(widget W);
void Check4Resize(window W);
void MaximizeWindow(window W, byte full_screen);
void ShowWinList(wm_ctx *C);

void FocusCtx(wm_ctx *C);
byte ActivateCtx(wm_ctx *C, byte State);

void ForceRelease(CONST wm_ctx *C);

#endif /* _TWIN_WM_H */
