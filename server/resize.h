/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_RESIZE_H
#define _TWIN_RESIZE_H

byte EnsureLenRow(row Row, ldat Len, byte DefaultCol);
byte RowWriteAscii(window Window, ldat Len, CONST byte * Text);
byte RowWriteHWFont(window Window, ldat Len, CONST hwfont * Text);

void ExposeWidget2(widget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, CONST byte *Text, CONST hwfont *Font, CONST hwattr *Attr);
void ExposeWindow2(window W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, CONST byte *Text, CONST hwfont *Font, CONST hwattr *Attr);

extern byte NeedUpdateCursor;
#define UpdateCursor() (NeedUpdateCursor = TRUE)
void FlushCursor(void);

byte CheckResizeWindowContents(window Window);
byte ResizeWindowContents(window Window);

/*
void SetNewFont(void);
void GetPalette(void);
void SetPalette(void);
 */

void ResizeFirstScreen(dat DeltaY);
void DragFirstScreen(ldat DeltaX, ldat DeltaY);

void HideMenu(byte on_off);
void CloseMenu(void);
void SetMenuState(menuitem Item, byte ByMouse);

void ResizeWidget(widget W, dat X, dat Y);
void ResizeGadget(gadget G, dat X, dat Y);

void DragFirstWindow(dat i, dat j);
void DragWindow(window Window, dat i, dat j);
void ResizeRelFirstWindow(dat i, dat j);
void ResizeRelWindow(window Window, dat i, dat j);
void CenterWindow(window Window);

byte ExecScrollFocusWindow(void);
void ScrollFirstWindowArea(dat X1, dat Y1, dat X2, dat Y2, ldat DeltaX, ldat DeltaY);
void ScrollFirstWindow(ldat DeltaX, ldat DeltaY, byte byXYLogic);
void ScrollWindow(window Window, ldat DeltaX, ldat DeltaY);

void ScrollWidget(widget W, ldat DeltaX, ldat DeltaY);

void UnFocusWidget(widget W);
void RollUpWindow(window W, byte on_off);

void SetVisibleWidget(widget W, byte on_off);

void RaiseWidget(widget W, byte alsoFocus);
void LowerWidget(widget W, byte alsoDeFocus);
void RestackWidgets(widget P, uldat N, CONST widget *arrayW);

void RestackRows(obj O, uldat N, CONST row *arrayR);

void PressGadget(gadget G);
void UnPressGadget(gadget G, byte maySendMsgIfNotToggle);
void SendMsgGadget(gadget G);
void WriteTextsGadget(gadget G, byte bitmap, dat XWidth, dat YWidth, CONST byte *Text, dat Left, dat Up);
void WriteHWFontsGadget(gadget G, byte bitmap, dat XWidth, dat YWidth, CONST hwfont *Text, dat Left, dat Up);

void SyncMenu(menu Menu);

#endif /* _TWIN_RESIZE_H */

