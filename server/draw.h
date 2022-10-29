/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_DRAW_H
#define TWIN_DRAW_H

widget FindWidgetAt(widget Parent, dat X, dat Y);

void DrawSelfWidget(draw_ctx *d);
void DrawSelfGadget(draw_ctx *d);
void DrawSelfWindow(draw_ctx *d);
void DrawSelfScreen(draw_ctx *d);

void TranslateCoordsWidget(widget w1, widget w2, dat *x, dat *y, byte *inside);

void DrawDesktop(screen s, dat xstart, dat ystart, dat xend, dat yend, bool shaded);

void DrawArea2(screen FirstScreen, widget Top, widget OnlyW, dat X1, dat Y1, dat X2, dat Y2,
               bool Shaded);
#define FULL_SCREEN (screen)0, (widget)0, (widget)0, (dat)0, (dat)0, TW_MAXDAT, TW_MAXDAT, tfalse

/*
 * DrawWidget() ASSUMES the specified part of the widget is unobscured.
 * xstart,ystart,xend,yend are absolute screen coordinates.
 */
void DrawWidget(widget w, dat xstart, dat ystart, dat xend, dat yend, bool shaded);
#define DrawFirstWidget(w) DrawWidget((w), 0, 0, TW_MAXDAT, TW_MAXDAT, tfalse)
void DrawAreaWidget(widget w);

/* like DrawAreaWindow2, but does not draw the shadow: */
#define DrawFullWindow2(w) DrawAreaWidget((widget)w)

void DrawScreen(screen s);

void DrawBorderWindow(window w, byte Flags);
#define BORDER_LEFT (0x1)
#define BORDER_UP (0x2)
#define BORDER_RIGHT (0x4)
#define BORDER_DOWN (0x8)
#define BORDER_ANY (0xF)

void DrawShadeWindow(window w, dat xstart, dat ystart, dat xend, dat yend, byte internal);
void DrawAreaShadeWindow(screen s, window w, dat xstart, dat ystart, dat xend, dat yend,
                         ldat shleft, ldat shup, ldat shrgt, ldat shdwn, byte internal);
/* this also draws the shadow: */
void DrawAreaWindow2(window w);

void DrawPartialWidget(widget w, dat x1, dat y1, dat x2, dat y2);
void DrawLogicWidget(widget w, ldat x1, ldat y1, ldat x2, ldat y2);

void ReDrawRolledUpAreaWindow(window w, bool shaded);

void DrawMenuScreen(screen s, dat xstart, dat xend);

void ClearHilight(window w);
void StartHilight(window w, ldat xsel, ldat ysel);
void ExtendHilight(window w, ldat xsel, ldat ysel);

screen ScreenParent(widget w);
window WindowParent(widget w);
widget NonScreenParent(widget w);
window FindCursorWindow(void);
byte ContainsCursor(widget w);
widget RecursiveFindWidgetAt(widget Parent, dat X, dat Y);

#endif /* TWIN_DRAW_H */
