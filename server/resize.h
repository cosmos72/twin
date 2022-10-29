/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_RESIZE_H
#define TWIN_RESIZE_H

byte EnsureLenRow(row r, uldat len, byte default_color);
byte RowWriteCharset(window w, uldat Len, const char *charset_bytes);
byte RowWriteUtf8(window w, uldat len, const char *utf8_bytes);
byte RowWriteTRune(window w, uldat len, const trune *runes);

void ExposeWidget2(Twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                   const char *utf8_bytes, const trune *runes, const tcell *cells);
void ExposeWindow2(window W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                   const char *utf8_bytes, const trune *runes, const tcell *cells);

extern byte NeedUpdateCursor;
#define UpdateCursor() (NeedUpdateCursor = ttrue)
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
void SetMenuState(Tmenuitem Item, byte ByMouse);

void ResizeWidget(Twidget W, dat X, dat Y);
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

void ScrollWidget(Twidget W, ldat DeltaX, ldat DeltaY);

void UnFocusWidget(Twidget W);
void RollUpWindow(window W, byte on_off);

void SetVisibleWidget(Twidget W, byte on_off);

void RaiseWidget(Twidget W, byte alsoFocus);
void LowerWidget(Twidget W, byte alsoDeFocus);
void RestackWidgets(Twidget P, uldat N, const Twidget *arrayW);

void RestackRows(obj O, uldat N, const row *arrayR);

void PressGadget(gadget G);
void UnPressGadget(gadget G, byte maySendMsgIfNotToggle);
void SendMsgGadget(gadget G);
void WriteTextsGadget(gadget G, byte bitmap, dat XWidth, dat YWidth, const char *Text, dat Left,
                      dat Up);
void WriteTRunesGadget(gadget G, byte bitmap, dat XWidth, dat YWidth, const trune *Text, dat Left,
                       dat Up);

void SyncMenu(menu Menu);

#endif /* TWIN_RESIZE_H */
