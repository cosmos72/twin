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

bool EnsureLenRow(Trow r, uldat len, bool default_color);
bool RowWriteCharsetWindow(Twindow window, uldat Len, const char *charset_bytes);
bool RowWriteUtf8Window(Twindow window, uldat len, const char *utf8_bytes);
bool RowWriteTRuneWindow(Twindow window, uldat len, const trune *runes);
// unimplemented!
bool RowWriteTCellWindow(Twindow window, dat x, dat y, uldat len, const tcell *cells);

void ExposeWidget2(Twidget w, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                   const char *utf8_bytes, const trune *runes, const tcell *cells);
void ExposeWindow2(Twindow window, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                   const char *utf8_bytes, const trune *runes, const tcell *cells);

extern bool NeedUpdateCursor;
#define UpdateCursor() (NeedUpdateCursor = true)
void FlushCursor(void);

bool CheckResizeWindowContents(Twindow window);
bool ResizeWindowContents(Twindow window);

/*
void SetNewFont(void);
void GetPalette(void);
void SetPalette(void);
 */

void ResizeFirstScreen(dat DeltaY);
void DragFirstScreen(ldat DeltaX, ldat DeltaY);

void HideMenu(byte on_off);
void CloseMenu(void);
void SetMenuState(Tmenuitem item, bool by_mouse);

void ResizeWidget(Twidget w, dat X, dat Y);
void ResizeGadget(Tgadget G, dat X, dat Y);

void DragFirstWindow(dat i, dat j);
void DragWindow(Twindow window, dat i, dat j);
void ResizeRelFirstWindow(dat i, dat j);
void ResizeRelWindow(Twindow window, dat i, dat j);
void CenterWindow(Twindow window);

byte ExecScrollFocusWindow(void);
void ScrollFirstWindowArea(dat X1, dat Y1, dat X2, dat Y2, ldat DeltaX, ldat DeltaY);
void ScrollFirstWindow(ldat DeltaX, ldat DeltaY, byte byXYLogic);
void ScrollWindow(Twindow window, ldat DeltaX, ldat DeltaY);

void ScrollWidget(Twidget w, ldat DeltaX, ldat DeltaY);

void RollUpWindow(Twindow window, byte on_off);

void SetVisibleWidget(Twidget w, byte on_off);

void RaiseWidget(Twidget w, bool alsoFocus);
void LowerWidget(Twidget w, bool alsoDeFocus);
void RestackWidgets(Twidget P, uldat N, const Twidget *arrayW);

void RestackRows(Tobj O, uldat N, const Trow *arrayR);

void PressGadget(Tgadget G);
void UnPressGadget(Tgadget G, byte maySendMsgIfNotToggle);
void SendMsgGadget(Tgadget G);

void SyncMenu(Tmenu Menu);

#endif /* TWIN_RESIZE_H */
