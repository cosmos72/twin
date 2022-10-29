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
byte RowWriteCharset(Twindow w, uldat Len, const char *charset_bytes);
byte RowWriteUtf8(Twindow w, uldat len, const char *utf8_bytes);
byte RowWriteTRune(Twindow w, uldat len, const trune *runes);

void ExposeWidget2(Twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                   const char *utf8_bytes, const trune *runes, const tcell *cells);
void ExposeWindow2(Twindow W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                   const char *utf8_bytes, const trune *runes, const tcell *cells);

extern byte NeedUpdateCursor;
#define UpdateCursor() (NeedUpdateCursor = ttrue)
void FlushCursor(void);

byte CheckResizeWindowContents(Twindow Window);
byte ResizeWindowContents(Twindow Window);

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
void ResizeGadget(Tgadget G, dat X, dat Y);

void DragFirstWindow(dat i, dat j);
void DragWindow(Twindow Window, dat i, dat j);
void ResizeRelFirstWindow(dat i, dat j);
void ResizeRelWindow(Twindow Window, dat i, dat j);
void CenterWindow(Twindow Window);

byte ExecScrollFocusWindow(void);
void ScrollFirstWindowArea(dat X1, dat Y1, dat X2, dat Y2, ldat DeltaX, ldat DeltaY);
void ScrollFirstWindow(ldat DeltaX, ldat DeltaY, byte byXYLogic);
void ScrollWindow(Twindow Window, ldat DeltaX, ldat DeltaY);

void ScrollWidget(Twidget W, ldat DeltaX, ldat DeltaY);

void UnFocusWidget(Twidget W);
void RollUpWindow(Twindow W, byte on_off);

void SetVisibleWidget(Twidget W, byte on_off);

void RaiseWidget(Twidget W, byte alsoFocus);
void LowerWidget(Twidget W, byte alsoDeFocus);
void RestackWidgets(Twidget P, uldat N, const Twidget *arrayW);

void RestackRows(obj O, uldat N, const row *arrayR);

void PressGadget(Tgadget G);
void UnPressGadget(Tgadget G, byte maySendMsgIfNotToggle);
void SendMsgGadget(Tgadget G);
void WriteTextsGadget(Tgadget G, byte bitmap, dat XWidth, dat YWidth, const char *Text, dat Left,
                      dat Up);
void WriteTRunesGadget(Tgadget G, byte bitmap, dat XWidth, dat YWidth, const trune *Text, dat Left,
                       dat Up);

void SyncMenu(menu Menu);

#endif /* TWIN_RESIZE_H */
