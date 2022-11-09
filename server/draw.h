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

void DrawSelfWidget(Sdraw *d);
void DrawSelfGadget(Sdraw *d);
void DrawSelfWindow(Sdraw *d);
void DrawSelfScreen(Sdraw *d);

void TranslateCoordsWidget(Twidget w1, Twidget w2, dat *x, dat *y, byte *inside);

void DrawDesktop(Tscreen s, dat xstart, dat ystart, dat xend, dat yend, bool shaded);

void DrawArea2(Tscreen FirstScreen, Twidget Top, Twidget OnlyW, dat X1, dat Y1, dat X2, dat Y2,
               bool Shaded);
#define FULL_SCREEN (Tscreen)0, (Twidget)0, (Twidget)0, (dat)0, (dat)0, TW_MAXDAT, TW_MAXDAT, tfalse

/*
 * DrawWidget() ASSUMES the specified part of the Twidget is unobscured.
 * xstart,ystart,xend,yend are absolute Tscreen coordinates.
 */
void DrawWidget(Twidget w, dat xstart, dat ystart, dat xend, dat yend, bool shaded);
#define DrawFirstWidget(w) DrawWidget((w), 0, 0, TW_MAXDAT, TW_MAXDAT, tfalse)
void DrawAreaWidget(Twidget w);

/* like DrawAreaWindow2, but does not draw the shadow: */
#define DrawFullWindow2(w) DrawAreaWidget((Twidget)w)

void DrawScreen(Tscreen s);

void DrawBorderWindow(Twindow w, byte Flags);
#define BORDER_LEFT (0x1)
#define BORDER_UP (0x2)
#define BORDER_RIGHT (0x4)
#define BORDER_DOWN (0x8)
#define BORDER_ANY (0xF)

void DrawShadeWindow(Twindow w, dat xstart, dat ystart, dat xend, dat yend, byte internal);
void DrawAreaShadeWindow(Tscreen s, Twindow w, dat xstart, dat ystart, dat xend, dat yend,
                         ldat shleft, ldat shup, ldat shrgt, ldat shdwn, byte internal);
/* this also draws the shadow: */
void DrawAreaWindow2(Twindow w);

void DrawPartialWidget(Twidget w, dat x1, dat y1, dat x2, dat y2);
void DrawLogicWidget(Twidget w, ldat x1, ldat y1, ldat x2, ldat y2);

void ReDrawRolledUpAreaWindow(Twindow w, bool shaded);

void DrawMenuScreen(Tscreen s, dat xstart, dat xend);

void ClearHilight(Twindow w);
void StartHilight(Twindow w, ldat xsel, ldat ysel);
void ExtendHilight(Twindow w, ldat xsel, ldat ysel);

Tscreen ScreenParent(Twidget w);
Twindow WindowParent(Twidget w);
Twidget NonScreenParent(Twidget w);
Twindow FindCursorWindow(void);
byte ContainsCursor(Twidget w);
Twidget RecursiveFindWidgetAt(Twidget Parent, dat X, dat Y);

#endif /* TWIN_DRAW_H */
