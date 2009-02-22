/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_DRAW_H
#define _TWIN_DRAW_H

byte InitDraw(void);

widget FindWidgetAt(widget Parent, dat X, dat Y);

void DrawSelfWidget(draw_ctx *D);
void DrawSelfGadget(draw_ctx *D);
void DrawSelfWindow(draw_ctx *D);
void DrawSelfScreen(draw_ctx *D);

byte InitDrawCtx(widget W, dat X1, dat Y1, dat X2, dat Y2, byte Shaded, draw_ctx *D);
byte InitAbsoluteDrawCtx(widget W, dat X1, dat Y1, dat X2, dat Y2, byte Shaded, draw_ctx *D);
void TranslateCoordsWidget(widget W1, widget W2, dat *X, dat *Y, byte *Inside);

void DrawDesktop(screen Screen, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded);

void DrawArea2(screen FirstScreen, widget Top, widget OnlyW,
	       dat X1, dat Y1, dat X2, dat Y2, byte Shaded);
#define FULL_SCREEN (screen)0, (widget)0, (widget)0, (dat)0, (dat)0, MAXDAT, MAXDAT, FALSE

/* DrawWidget() assumes W is not obscured */
void DrawWidget(widget W, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded);
#define DrawFirstWidget(W) DrawWidget((W), 0, 0, MAXDAT, MAXDAT, FALSE)
void DrawAreaWidget(widget W);

/* like DrawAreaWindow2, but does not draw the shadow: */
#define DrawFullWindow2(W) DrawAreaWidget((widget)W)


void DrawScreen(screen Screen);

void DrawBorderWindow(window Window, byte Flags);
#define BORDER_LEFT	(0x1)
#define BORDER_UP	(0x2)
#define BORDER_RIGHT	(0x4)
#define BORDER_DOWN	(0x8)
#define BORDER_ANY	(0xF)

void DrawShadeWindow(window Window, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Internal);
void DrawAreaShadeWindow(screen Screen, window Window, dat Xstart, dat Ystart, dat Xend, dat Yend,
			 ldat shLeft, ldat shUp, ldat shRgt, ldat shDwn, byte Internal);
/* this also draws the shadow: */
void DrawAreaWindow2(window Window);

void DrawPartialWidget(widget W, dat X1, dat Y1, dat X2, dat Y2);
void DrawLogicWidget(widget W, ldat X1, ldat Y1, ldat X2, ldat Y2);

void ReDrawRolledUpAreaWindow(window Window, byte Shaded);

void DrawMenuScreen(screen Screen, dat Xstart, dat Xend);

void ClearHilight(window Window);
void StartHilight(window Window, ldat XSel, ldat YSel);
void ExtendHilight(window Window, ldat XSel, ldat YSel);

screen ScreenParent(widget W);
window WindowParent(widget W);
widget NonScreenParent(widget W);
window FindCursorWindow(void);
byte ContainsCursor(widget W);
widget RecursiveFindWidgetAt(widget Parent, dat X, dat Y);

#endif /* _TWIN_DRAW_H */
