/*
 *  resize.c  --  code to drag and resize screens and windows
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <sys/stat.h>
#include <signal.h>

#include <Tw/autoconf.h>

#ifdef TW_HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef TW_HAVE_TERMIOS_H
#include <termios.h>
#else
#ifdef TW_HAVE_TERMIO_H
#include <termio.h>
#endif
#endif

#include "twin.h"
#include "alloc.h"
#include "algo.h"
#include "data.h"
#include "draw.h"
#include "log.h"
#include "methods.h"
#include "resize.h"
#include "util.h"
#include "hw.h"
#include "stl/utf8.h"
#include "stl/vector.h"

#include <Tw/Tw.h>
#include <Tw/Twstat.h>
#include <Tutf/Tutf.h>

/***************/

bool NeedUpdateCursor;

void FlushCursor(void) {
  uldat type = NOCURSOR;

  if (NeedUpdateCursor) {
    NeedUpdateCursor = false;

    const Tscreen screen = All->FirstScreen;
    const Twindow window = FindCursorWindow();

    if (window &&
        ((window->Flags & WINDOWFL_CURSOR_ON) || (All->SetUp->Flags & setup_cursor_always))) {

      const byte hasBorder = !(window->Flags & WINDOWFL_BORDERLESS);

      ldat curX = window->CurX - window->XLogic + hasBorder;
      ldat curY = window->CurY - window->YLogic + hasBorder;

      Sdraw d;
      if (curX >= 0 && curX < (window->XWidth - hasBorder) && curY >= 0 &&
          curY < (window->YWidth - hasBorder) &&
          d.Init(window, (dat)curX, (dat)curY, (dat)curX, (dat)curY, false) &&
          ((window == (Twindow)screen->FirstW && !window->FirstW) ||
#if 1
           /* widgets and gadgets cannot contain cursor, but they can obscure it */
           window == RecursiveFindWidgetAt(screen, (dat)d.X1, (dat)d.Y1 - screen->Up)
#else
           window == WindowParent(RecursiveFindWidgetAt(screen, (dat)d.X1, (dat)d.Y1 - screen->Up))
#endif
               )) {

        MoveToXY((dat)d.X1, (dat)d.Y1);
        if ((type = window->CursorType) == NOCURSOR && All->SetUp->Flags & setup_cursor_always)
          type = LINECURSOR;
      }
    }
    SetCursorType(type);
  }
}

static byte SendResizeSignal(Twindow w) {
  struct winsize wsiz;

  wsiz.ws_col = w->USE.C.TtyData->SizeX;
  wsiz.ws_row = w->USE.C.TtyData->SizeY;
  wsiz.ws_xpixel = 0;
  wsiz.ws_ypixel = 0;

#if 0
    /* is this enough on every OS ? */
    return ioctl(w->RemoteData.Fd, TIOCSWINSZ, &wsiz) == 0;
#else
  return ioctl(w->RemoteData.Fd, TIOCSWINSZ, &wsiz) == 0 &&
         kill(-w->RemoteData.ChildPid, SIGWINCH) == 0;
#endif
}

bool CheckResizeWindowContents(Twindow w) {
  if (W_USE(w, USECONTENTS) &&
      (w->USE.C.TtyData->SizeY != w->YWidth - 2 || w->USE.C.TtyData->SizeX != w->XWidth - 2)) {
    return ResizeWindowContents(w);
  }
  return true;
}

bool ResizeWindowContents(Twindow w) {
  tcell *NewCont, *saveNewCont, *OldCont, *max, h;
  ldat count, common, left;
  ttydata *Data = w->USE.C.TtyData;
  dat x = w->XWidth, y = w->YWidth + Data->ScrollBack;

  if (!(w->Flags & WINDOWFL_BORDERLESS))
    x -= 2, y -= 2;

  h = TCELL(w->ColText, ' ');

  /* safety check: */
  if (x > 0 && y > 0) {
    if (!(saveNewCont = NewCont = (tcell *)AllocMem(x * y * sizeof(tcell))))
      return false;

    /*
     * copy the Contents. Quite non-trivial for two reasons:
     * 1. always preserve the cursor line
     * 2. copy as much scrollback as possible
     */
    if ((max = OldCont = w->USE.C.Contents)) {
      max += w->WLogic * w->HLogic;
      common = Min2(w->WLogic, x);
      count = Min2(w->HLogic, y);
      OldCont += w->USE.C.HSplit * w->WLogic;

      /* ok, now check for the cursor line */
      if (w->CurY >= y) {
        OldCont += (w->CurY + 1 - y) * w->WLogic;
        if (OldCont > max)
          OldCont -= w->WLogic * w->HLogic;
      }

      while (count--) {
        CopyMem(OldCont, NewCont, common * sizeof(tcell));

        if ((OldCont += w->WLogic) >= max)
          OldCont = w->USE.C.Contents;
        NewCont += common;
        for (left = x - common; left; left--)
          *NewCont++ = h;
      }
      FreeMem(w->USE.C.Contents);
    }

    left = (saveNewCont + x * y) - NewCont;
    while (left--)
      *NewCont++ = h;
  } else {
    x = y = 0;
    if (w->USE.C.Contents)
      FreeMem(w->USE.C.Contents);
    saveNewCont = NULL;
  }

  w->XLogic = 0;
  w->YLogic = Data->ScrollBack;
  w->WLogic = x;       /* Contents width */
  w->HLogic = y;       /* Y visible + scrollback */
  w->USE.C.HSplit = 0; /* splitline == 0 */
  w->USE.C.Contents = saveNewCont;

  if (w->CurY >= y)
    w->CurY = y - 1;
  else if (w->CurY < w->YLogic)
    w->CurY = w->YLogic;

  Data->SizeX = x;
  Data->SizeY = w->YWidth - 2;
  Data->Top = 0;
  Data->Bottom = Data->SizeY;

  Data->Start = w->USE.C.Contents + x * Data->ScrollBack;
  Data->Split = w->USE.C.Contents + x * y;
  Data->saveX = Data->X = w->CurX;
  Data->saveY = Data->Y = w->CurY - Data->ScrollBack;
  Data->Pos = w->USE.C.Contents + w->CurY * x + w->CurX;

  if (!(w->Attr & WINDOW_WANT_CHANGES) && w->USE.C.TtyData && w->RemoteData.FdSlot != NOSLOT)
    /* the MsgPort will not be informed of the resize...
     * we must send SIGWINCH manually */
    (void)SendResizeSignal(w);

  if (w->Parent) {
    DrawBorderWindow(w, BORDER_RIGHT);

    DrawLogicWidget((Twidget)w, 0, 0, w->WLogic - 1, w->HLogic - 1);

    if (ContainsCursor((Twidget)w))
      UpdateCursor();
  }

  return true;
}

static Trow InsertRowsWindow(Twindow w, ldat NumRows) {
  Trow row;

  while (NumRows--) {
    if ((row = New(row)(0, ROW_ACTIVE))) {
      Act(Insert, row)(row, w, w->USE.R.LastRow, NULL);
    } else
      break;
  }
  DrawBorderWindow(w, BORDER_RIGHT);
  return row;
}

bool EnsureLenRow(Trow row, uldat len, bool default_color) {
  trune *temp_text;
  tcolor *temp_color;
  uldat new_len;

  if (len > row->MaxLen) {
    new_len = (len + (len >> 1)) | All->SetUp->MinAllocSize;
    if ((temp_text = (trune *)ReAllocMem(row->Text, new_len * sizeof(trune)))) {
      if (!(row->Flags & ROW_DEFCOL) && !default_color) {
        if ((temp_color = (tcolor *)ReAllocMem(row->ColText, new_len * sizeof(tcolor))))
          row->ColText = temp_color;
        else
          return false;
      }
      row->Text = temp_text;
      row->MaxLen = new_len;
    } else
      return false;
  }
  return true;
}

bool RowWriteCharsetWindow(Twindow w, uldat len, const char *charset_bytes) {

  if (!w || (len && !charset_bytes) || !W_USE(w, USEROWS))
    return false;

  Trow row = w->USE.R.LastRow;
  ldat x = w->CurX;
  ldat y = w->CurY;
  ldat max = w->HLogic;
  bool isInsert = !!(w->Flags & WINDOWFL_ROWS_INSERT);

  if (w->State & WINDOW_ANYSEL)
    ClearHilight(w);

  while (len) {
    if (max <= y || (max == y + 1 && (*charset_bytes == '\n' || *charset_bytes == '\r'))) {
      if (InsertRowsWindow(w, Max2(y + 1 - max, 1))) {
        max = w->HLogic;
        row = w->USE.R.LastRow;
      } else
        return false;
    } else {
      row = Act(FindRow, w)(w, y);
    }

    uldat row_len = 0;
    const char *text = charset_bytes;
    while (row_len < len && *text != '\n' && *text != '\r')
      ++row_len, ++text;

    /*        WINDOWFL_INSERT non implementato */
    /*  Gap non implementato                                 */

    if (row_len) {
      if (isInsert || (row && row->LenGap))
        return tfalse;

      if (!EnsureLenRow(row, x + row_len, !!(w->Flags & WINDOWFL_ROWS_DEFCOL)))
        return tfalse;

      if (w->USE.R.NumRowOne == y)
        w->USE.R.RowOne = row;
      if (w->USE.R.NumRowSplit == y)
        w->USE.R.RowSplit = row;
      row->Flags = ROW_ACTIVE;

      trune const *to_UTF_32 = w->Charset;

      for (uldat i = (uldat)Max2(0, -x); i < row_len; i++) {
        row->Text[x + i] = to_UTF_32[(byte)charset_bytes[i]];
      }
      if (x >= 0 && (uldat)x > row->Len)
        for (uldat i = row->Len; i < (uldat)x; i++) {
          row->Text[i] = (trune)' ';
        }

      if (!(w->Flags & WINDOWFL_ROWS_DEFCOL)) {
        memset(row->ColText + x, w->ColText, sizeof(tcolor) * row_len);
        if (x >= 0 && (uldat)x > row->Len)
          memset(row->ColText + row->Len, w->ColText, sizeof(tcolor) * (x - row->Len));
      }

      if (row->Len < x + row_len)
        row->Len = x + row_len;

      DrawLogicWidget(w, x, y, x + row_len - (ldat)1, y);

      charset_bytes += row_len;
      len -= row_len;
    }

    if (len && (*charset_bytes == '\n' || *charset_bytes == '\r')) {
      w->CurX = x = (ldat)0;
      w->CurY = ++y;
      charset_bytes++, len--;
    } else
      w->CurX = x += row_len;
  }

  if (w == FindCursorWindow())
    UpdateCursor();

  return true;
}

bool RowWriteUtf8Window(Twindow w, uldat len, const char *utf8_bytes) {
  Chars chars(utf8_bytes, len);
  Vector<trune> runes;
  Utf8 seq;

  runes.reserve(len);
  while (chars) {
    seq.parse(chars, &chars);
    runes.append(seq.rune());
  }
  if (!runes) {
    return false;
  }
  return RowWriteTRuneWindow(w, runes.size(), runes.data());
}

bool RowWriteTRuneWindow(Twindow w, uldat len, const trune *runes) {

  if (!w || !len || !W_USE(w, USEROWS))
    return false;

  Trow row = w->USE.R.LastRow;
  const trune *text;
  ldat x = w->CurX, y = w->CurY, max = w->HLogic;
  uldat i, row_len;
  bool isInsert = !!(w->Flags & WINDOWFL_ROWS_INSERT);

  if (w->State & WINDOW_ANYSEL)
    ClearHilight(w);

  while (len) {
    if (max <= y || (max == y + 1 && (*runes == '\n' || *runes == '\r'))) {
      if (InsertRowsWindow(w, Max2(y + 1 - max, 1))) {
        max = w->HLogic;
        row = w->USE.R.LastRow;
      } else
        return tfalse;
    } else {
      row = Act(FindRow, w)(w, y);
    }

    row_len = (ldat)0;
    text = runes;
    while (row_len < len && *text != '\n' && *text != '\r')
      ++row_len, ++text;

    /*        WINDOWFL_INSERT non implementato */
    /*  Gap non implementato                                 */

    if (row_len) {
      if (isInsert || (row && row->LenGap))
        return tfalse;

      if (!EnsureLenRow(row, x + row_len, !!(w->Flags & WINDOWFL_ROWS_DEFCOL)))
        return tfalse;

      if (w->USE.R.NumRowOne == y)
        w->USE.R.RowOne = row;
      if (w->USE.R.NumRowSplit == y)
        w->USE.R.RowSplit = row;
      row->Flags = ROW_ACTIVE;

      if (x >= 0) {
        CopyMem(runes, row->Text + x, sizeof(trune) * row_len);
      } else if ((uldat)-x < row_len) {
        CopyMem(runes - x, row->Text, sizeof(trune) * (row_len + x));
      }
      if (x >= 0 && (uldat)x > row->Len) {
        for (i = row->Len; i < (uldat)x; i++)
          row->Text[i] = (trune)' ';
      }

      if (!(w->Flags & WINDOWFL_ROWS_DEFCOL)) {
        if (x >= 0) {
          memset(row->ColText + x, w->ColText, sizeof(tcolor) * row_len);
        } else if ((uldat)-x < row_len) {
          memset(row->ColText, w->ColText - x, sizeof(tcolor) * (row_len + x));
        }
        if (x >= 0 && (uldat)x > row->Len) {
          memset(row->ColText + row->Len, w->ColText, sizeof(tcolor) * (x - row->Len));
        }
      }

      if (row->Len < x + row_len)
        row->Len = x + row_len;

      DrawLogicWidget((Twidget)w, x, y, x + row_len - (ldat)1, y);

      runes += row_len;
      len -= row_len;
    }

    if (len && (*runes == '\n' || *runes == '\r')) {
      w->CurX = x = (ldat)0;
      w->CurY = ++y;
      runes++, len--;
    } else
      w->CurX = x += row_len;
  }

  if (w == FindCursorWindow())
    UpdateCursor();

  return true;
}

// unimplemented!
bool RowWriteTCellWindow(Twindow w, dat x, dat y, uldat len, const tcell *cells) {
  (void)w;
  (void)x;
  (void)y;
  (void)len;
  (void)cells;
  return false;
}

void ExposeWidget2(Twidget w, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                   const char *utf8_bytes, const trune *runes, const tcell *cells) {
  if (w_USE(w, USEEXPOSE)) {
    if (utf8_bytes || runes || cells) {
      if (utf8_bytes) {
        w->USE.E.E.Text = utf8_bytes;
        w->USE.E.Flags = WIDGET_USEEXPOSE_TEXT;
      } else if (runes) {
        w->USE.E.E.TRune = runes;
        w->USE.E.Flags = WIDGET_USEEXPOSE_TRUNE;
      } else {
        w->USE.E.E.TCell = cells;
        w->USE.E.Flags = WIDGET_USEEXPOSE_TCELL;
      }
      w->USE.E.X1 = Left;
      w->USE.E.X2 = Left + XWidth - 1;
      w->USE.E.Y1 = Up;
      w->USE.E.Y2 = Up + YWidth - 1;
      w->USE.E.Pitch = Pitch;
    } else {
      w->USE.E.E.Text = (char *)1;
      w->USE.E.Flags = WIDGET_USEEXPOSE_TEXT;
      w->USE.E.X1 = w->USE.E.Y1 = w->USE.E.X2 = w->USE.E.Y2 = -1;
      w->USE.E.Pitch = 0;
    }

    DrawLogicWidget(w, Left, Up, Left + XWidth - 1, Up + YWidth - 1);

    memset(&w->USE.E.E, '\0', sizeof(w->USE.E.E));
    w->USE.E.Flags = 0;
  }
}

void ExposeWindow2(Twindow w, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                   const char *utf8_bytes, const trune *runes, const tcell *cells) {
  ldat curX, curY;

  if (W_USE(w, USEEXPOSE)) {
    ExposeWidget2((Twidget)w, XWidth, YWidth, Left, Up, Pitch, utf8_bytes, runes, cells);
    return;
  }

  if (!W_USE(w, USECONTENTS) && !W_USE(w, USEROWS))
    return;

  /* handle negative (Left,Up) by clipping */
  if (Left < 0) {
    XWidth += Left;
    if (utf8_bytes)
      utf8_bytes -= Left;
    if (runes)
      runes -= Left;
    if (cells)
      cells -= Left;
    Left = 0;
  }
  if (Up < 0) {
    YWidth += Up;
    if (utf8_bytes)
      utf8_bytes -= (ldat)Up * Pitch;
    if (runes)
      runes -= (ldat)Up * Pitch;
    if (cells)
      cells -= (ldat)Up * Pitch;
    Up = 0;
  }

  if (W_USE(w, USECONTENTS)) {
    /* clip to window size */
    curX = w->USE.C.TtyData->SizeX;
    curY = w->USE.C.TtyData->SizeY;
    if (Left >= curY || Up >= curY)
      return;
    if ((ldat)XWidth + Left > curX)
      XWidth = curX - Left;
    if ((ldat)YWidth + Up > curY)
      YWidth = curY - Up;
  }
  if (XWidth <= 0 || YWidth <= 0)
    return;

  if (utf8_bytes) {
    bool (*writeUtf8)(Twindow, uldat, const char *);
    if (W_USE(w, USECONTENTS)) {
      writeUtf8 = w->fn()->TtyWriteUtf8;
    } else
      writeUtf8 = RowWriteUtf8Window;

    curX = w->CurX;
    curY = w->CurY;
    for (; YWidth; YWidth--, Up++, utf8_bytes += Pitch) {
      Act(GotoXY, w)(w, Left, Up);
      writeUtf8(w, (uldat)XWidth, utf8_bytes);
    }
    Act(GotoXY, w)(w, curX, curY);

  } else if (runes) {
    bool (*writeTRune)(Twindow, uldat, const trune *);
    if (W_USE(w, USECONTENTS))
      writeTRune = w->fn()->TtyWriteTRune;
    else
      writeTRune = RowWriteTRuneWindow;

    curX = w->CurX;
    curY = w->CurY;
    for (; YWidth; YWidth--, Up++, runes += Pitch) {
      w->GotoXY(Left, Up);
      writeTRune(w, (uldat)XWidth, runes);
    }
    w->GotoXY(curX, curY);

  } else if (cells) {
    bool (*writeTCell)(Twindow, dat, dat, uldat, const tcell *);
    if (W_USE(w, USECONTENTS))
      writeTCell = w->fn()->TtyWriteTCell;
    else
      writeTCell = RowWriteTCellWindow;

    for (; YWidth; YWidth--, Up++, cells += Pitch)
      writeTCell(w, Left, Up, (uldat)XWidth, cells);
  }
}

/***************/

void ResizeWidget(Twidget w, dat X, dat Y) {
  if (w) {
    X = Max2(1, X);
    Y = Max2(1, Y);
    /* mX = Max2(X, w->XWidth); */
    /* mY = Max2(Y, w->YWidth); */
    w->XWidth = X;
    w->YWidth = Y;

    if (!(w->Flags & WIDGETFL_NOTVISIBLE))
      /* FIXME: use mX and mY */
      DrawAreaWidget(w);
  }
}

void ResizeGadget(Tgadget G, dat X, dat Y) {
  if (G) {
    if (G_USE(G, USETEXT)) {
      /* FIXME: finish this */
    } else {
      ResizeWidget((Twidget)G, X, Y);
    }
  }
}

/***************/

void DragFirstScreen(ldat DeltaX, ldat DeltaY) {
  Tscreen screen;
  dat DHeight, DWidth;
  dat ylimit;
  ldat Left, Up, Rgt, Dwn;

  screen = All->FirstScreen;
  ylimit = screen->Up;
  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;

  DeltaX = -DeltaX;
  DeltaY = -DeltaY;

  if (DeltaY < TW_MINDAT - screen->YLogic)
    DeltaY = TW_MINDAT - screen->YLogic;
  else if (DeltaY > TW_MAXDAT - screen->YLogic - (ldat)DHeight + 1)
    DeltaY = TW_MAXDAT - screen->YLogic - (ldat)DHeight + 1;
  if (DeltaX < TW_MINDAT - screen->XLogic)
    DeltaX = TW_MINDAT - screen->XLogic;
  else if (DeltaX > TW_MAXDAT - screen->XLogic - (ldat)DWidth + 1)
    DeltaX = TW_MAXDAT - screen->XLogic - (ldat)DWidth + 1;

  if (!DeltaX && !DeltaY)
    return;

  Left = Max2(DeltaX, (ldat)0);
  Up = (ldat)ylimit + 1 + Max2(DeltaY, (ldat)0);
  Rgt = (ldat)DWidth - (ldat)1 + Min2(DeltaX, (ldat)0);
  Dwn = (ldat)DHeight - (ldat)1 + Min2(DeltaY, (ldat)0);
  screen->XLogic += DeltaX;
  screen->YLogic += DeltaY;

  if (Up <= Dwn && Left <= Rgt)
    DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)(Left - DeltaX), (dat)(Up - DeltaY));

  ylimit++;

  if (Abs(DeltaX) >= DWidth || Abs(DeltaY) >= DHeight - ylimit)
    DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, 0, ylimit, DWidth - 1, DHeight - 1, tfalse);
  else if (DeltaY < 0) {
    DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, 0, ylimit, DWidth - 1, ylimit - DeltaY, tfalse);
    if (DeltaX <= 0)
      DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, 0, ylimit - DeltaY, -DeltaX - 1, DHeight - 1,
                tfalse);
    else if (DeltaX > 0)
      DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, DWidth - DeltaX, ylimit - DeltaY, DWidth - 1,
                DHeight - 1, tfalse);
  } else {
    DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, 0, DHeight - DeltaY, DWidth - 1, DHeight - 1,
              tfalse);
    if (DeltaX <= 0)
      DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, 0, ylimit, -DeltaX - 1, DHeight - DeltaY,
                tfalse);
    else if (DeltaX >= 0)
      DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, DWidth - DeltaX, ylimit, DWidth - 1,
                DHeight - DeltaY, tfalse);
  }
  UpdateCursor();
}

void ResizeFirstScreen(dat DeltaY) {
  Tscreen screen;
  dat DHeight, DWidth;
  dat ylimit;
  ldat Left, Up, Rgt, Dwn;

  screen = All->FirstScreen;
  ylimit = screen->Up;
  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;

  if (ylimit + DeltaY >= DHeight)
    DeltaY = DHeight - ylimit - 1;
  else if (ylimit + DeltaY < TW_MINDAT)
    DeltaY = TW_MINDAT - ylimit;

  Left = (ldat)0;
  Up = Max2(ylimit, 0);
  Rgt = DWidth - 1;
  Dwn = DHeight - 1 - Max2((ldat)DeltaY, 0);
  screen->Up += DeltaY;

  if (DeltaY < 0) {
    if (Up <= Dwn)
      DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)Left, (dat)Up + DeltaY);
    DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)Left, (dat)Dwn + 1 + DeltaY, (dat)Rgt, (dat)Dwn,
              tfalse);
  } else if (DeltaY > (dat)0) {
    if (Up <= Dwn)
      DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)Left, (dat)Up + (dat)DeltaY);
    DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, (dat)Left, (dat)Up, (dat)Rgt,
              (dat)Up + DeltaY - 1, tfalse);
  }
  UpdateCursor();
}

void CenterWindow(Twindow w) {
  Tscreen screen;
  ldat parz1, parz2;
  ldat DeltaX, DeltaY;
  dat DWidth, DHeight;
  dat ylimit;

  if (!w || !(screen = (Tscreen)w->Parent) || (screen != All->FirstScreen) ||
      (w->Flags & WINDOWFL_MENU))
    return;

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  ylimit = screen->Up;
  parz1 = (ldat)w->Left - (ldat)screen->XLogic;
  parz2 = parz1 + (ldat)w->XWidth;
  if (parz1 >= (ldat)0 && parz2 <= (ldat)DWidth)
    DeltaX = (ldat)0;
  else
    DeltaX = -parz1;

  if (w->XWidth <= (dat)DWidth) {
    parz1 = DeltaX;
    parz2 = DeltaX + (ldat)DWidth - (ldat)w->XWidth;
    DeltaX = Abs(parz1) < Abs(parz2) ? parz1 : parz2;
  } else
    DeltaX--;

  parz1 = (ldat)w->Up - (ldat)screen->YLogic + (ldat)ylimit;
  parz2 = parz1 + (ldat)w->YWidth;
  if (parz1 > (ldat)ylimit && parz2 <= (ldat)DHeight)
    DeltaY = (ldat)0;
  else
    DeltaY = -parz1 + ylimit + 1;

  if ((ldat)w->YWidth <= (ldat)DHeight - (ldat)ylimit - 1) {
    parz1 = DeltaY;
    parz2 = DeltaY + (ldat)DHeight - (ldat)ylimit - 1 - (ldat)w->YWidth;
    DeltaY = Abs(parz1) < Abs(parz2) ? parz1 : parz2;
  } else
    DeltaY--;

  DragFirstScreen(DeltaX, DeltaY);
}

inline void DrawDeltaShadeFirstWindow(dat i, dat j) {
  ldat _Left, _Up, _Rgt, _Dwn;
  ldat Left_, Up_, Rgt_, Dwn_;
  Tscreen screen;
  Twindow w;
  dat ylimit;

  screen = All->FirstScreen;
  if (!(w = (Twindow)screen->FirstW) || !IS_WINDOW(w))
    return;

  ylimit = screen->Up;

  Left_ = (ldat)w->Left - screen->XLogic;
  _Left = Left_ - i;
  Rgt_ = Left_ + (ldat)w->XWidth - (ldat)1;
  _Rgt = Rgt_ - i;
  Up_ = (ldat)w->Up - screen->YLogic + (ldat)ylimit;
  _Up = Up_ - j;
  Dwn_ = Up_ + (w->Attr & WINDOW_ROLLED_UP ? (ldat)0 : (ldat)w->YWidth - (ldat)1);
  _Dwn = Dwn_ - j;

  DrawAreaShadeWindow(screen, w, 0, 0, TW_MAXDAT, TW_MAXDAT, _Left, _Up, _Rgt, _Dwn, tfalse);
  DrawAreaShadeWindow(screen, w, 0, 0, TW_MAXDAT, TW_MAXDAT, Left_, Up_, Rgt_, Dwn_, ttrue);
}

void DragFirstWindow(dat i, dat j) {
  ldat Left, Up, Rgt, Dwn;
  ldat _Left, _Up, _Rgt, _Dwn;
  ldat Left_, Up_, Rgt_, Dwn_;
  ldat xLeft, xRgt, xUp, xDwn;
  Tscreen screen;
  Ssetup *SetUp;
  Twindow w;
  dat ylimit;
  dat DWidth, DHeight;
  byte Shade;

  screen = All->FirstScreen;
  if (!(w = (Twindow)screen->FirstW) || !IS_WINDOW(w) || !(w->Attr & WINDOW_DRAG))
    return;

  ylimit = screen->Up;
  if (i < 0 && w->Left < TW_MINDAT - i)
    i = TW_MINDAT - w->Left;
  else if (i > 0 && w->Left > TW_MAXDAT - i)
    i = TW_MAXDAT - w->Left;

  if (j < 0 && w->Up < TW_MINDAT - j)
    j = TW_MINDAT - w->Up;
  else if (j > 0 && w->Up > TW_MAXDAT - j)
    j = TW_MAXDAT - w->Up;

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  SetUp = All->SetUp;
  Shade = !!(SetUp->Flags & setup_shadows);

  Left = (ldat)w->Left - screen->XLogic;
  Rgt = Left + (ldat)w->XWidth - (ldat)1;
  Up = (ldat)w->Up - screen->YLogic + (ldat)ylimit;
  Dwn = Up + (w->Attr & WINDOW_ROLLED_UP ? (ldat)0 : (ldat)w->YWidth - (ldat)1);

  /* calculate the visible part of the window for direct DragArea() */

  Left_ = Max2(Left, -Min2(i, 0));
  Up_ = Max2(Up, (ldat)ylimit + 1 - Min2(j, 0));
  Rgt_ = Min2(Rgt, (ldat)DWidth - (ldat)1 - Max2(i, 0));
  Dwn_ = Min2(Dwn, (ldat)DHeight - (ldat)1 - Max2(j, 0));

  /* copy the visible part */
  if (Left_ <= Rgt_ && Up_ <= Dwn_)
    DragArea(Left_, Up_, Rgt_, Dwn_, Left_ + i, Up_ + j);

  w->Left += i;
  w->Up += j;

  if (Shade)
    /* update the window's shadow */
    DrawDeltaShadeFirstWindow(i, j);

  /* redraw the old window location */

  _Left = Max2(Left, (ldat)0);
  _Up = Max2(Up, (ldat)ylimit + 1);
  _Rgt = Min2(Rgt, (ldat)DWidth - (ldat)1);
  _Dwn = Min2(Dwn, (ldat)DHeight - (ldat)1);

  if (_Left <= _Rgt && _Up <= _Dwn) {
    xLeft = _Left;
    xRgt = _Rgt;
    if (i) {
      if (i > 0)
        xRgt = Min2(_Left + i - 1, _Rgt);
      else
        xLeft = Max2(_Left, _Rgt + i + 1);
      DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)xLeft, (dat)_Up, (dat)xRgt, (dat)_Dwn, tfalse);
    }
    xUp = _Up;
    xDwn = _Dwn;
    if (j && (!i || xRgt < _Rgt || xLeft > _Left)) {
      if (j > 0)
        xDwn = Min2(_Up + j - 1, _Dwn);
      else
        xUp = Max2(_Up, _Dwn + j + 1);

      if (xRgt < _Rgt) {
        xLeft = xRgt + (ldat)1;
        xRgt = _Rgt;
      } else if (xLeft > _Left) {
        xRgt = xLeft - (ldat)1;
        xLeft = _Left;
      }

      DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)xLeft, (dat)xUp, (dat)xRgt, (dat)xDwn, tfalse);
    }
  }

  /* draw the new window location */

  /* xxx_ : final location of copied area */
  Left_ += (ldat)i;
  Up_ += (ldat)j;
  Rgt_ += (ldat)i;
  Dwn_ += (ldat)j;

  /* _xxx : final location of window */
  _Left = Max2(Left + (ldat)i, (ldat)0);
  _Up = Max2(Up + (ldat)j, (ldat)ylimit + 1);
  _Rgt = Min2(Rgt + (ldat)i, (ldat)DWidth - (ldat)1);
  _Dwn = Min2(Dwn + (ldat)j, (ldat)DHeight - (ldat)1);

  if (_Left <= _Rgt && _Up <= _Dwn) {
    if (Left_ > _Left) {
      xLeft = Min2(Left_ - (ldat)1, _Rgt);
      DrawWidget((Twidget)w, (dat)_Left, (dat)_Up, (dat)xLeft, (dat)_Dwn, tfalse);
    } else
      xLeft = _Left;
    if (Rgt_ < _Rgt) {
      xRgt = Max2(Rgt_ + (ldat)1, _Left);
      DrawWidget((Twidget)w, (dat)xRgt, (dat)_Up, (dat)_Rgt, (dat)_Dwn, tfalse);
    } else
      xRgt = _Rgt;
    if (Up_ > _Up) {
      xUp = Min2(Up_ - (ldat)1, _Dwn);
      DrawWidget((Twidget)w, (dat)xLeft, (dat)_Up, (dat)xRgt, (dat)xUp, tfalse);
    }
    if (Dwn_ < _Dwn) {
      xDwn = Max2(Dwn_ + (ldat)1, _Up);
      DrawWidget((Twidget)w, (dat)xLeft, (dat)xDwn, (dat)xRgt, (dat)_Dwn, tfalse);
    }
  }

  if (ContainsCursor((Twidget)w))
    UpdateCursor();
}

void DragWindow(Twindow w, dat i, dat j) {
  ldat Left, Up, Rgt, Dwn;
  ldat Left1, Rgt1;
  Tscreen screen;
  Ssetup *SetUp;
  dat ylimit;
  dat DWidth, DHeight;
  byte Shade, DeltaXShade, DeltaYShade;

  if (!w || !(w->Attr & WINDOW_DRAG))
    return;

  if (w == (Twindow)All->FirstScreen->FirstW) {
    DragFirstWindow(i, j);
    return;
  }

  if (!(screen = (Tscreen)w->Parent) || !IS_SCREEN(screen))
    return;

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  SetUp = All->SetUp;
  Shade = !!(SetUp->Flags & setup_shadows);
  DeltaXShade = Shade ? SetUp->DeltaXShade : (byte)0;
  DeltaYShade = Shade ? SetUp->DeltaYShade : (byte)0;
  ylimit = screen->Up;

  Up = (ldat)w->Up - screen->YLogic + (ldat)ylimit;
  Left = (ldat)w->Left - screen->XLogic;
  Rgt = Left + (ldat)w->XWidth - (ldat)1;
  Dwn = Up + (w->Attr & WINDOW_ROLLED_UP ? (ldat)0 : (ldat)w->YWidth - (ldat)1);

  if (i < (dat)0) {
    if (w->Left < TW_MINDAT - i)
      i = TW_MINDAT - w->Left;
  } else if (i > (dat)0) {
    if (w->Left > TW_MAXDAT - i)
      i = TW_MAXDAT - w->Left;
  }
  if (j < (dat)0) {
    if (w->Up < TW_MINDAT - j)
      j = TW_MINDAT - w->Up;
  } else if (j > (dat)0) {
    if (w->Up > TW_MAXDAT - j)
      j = TW_MAXDAT - w->Up;
  }
  w->Left += i;
  w->Up += j;
  DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, Left + i, Up + j, Rgt + i + DeltaXShade,
            Dwn + j + DeltaYShade, tfalse);

  if (Left < (ldat)DWidth && Up < (ldat)DHeight && Rgt + DeltaXShade >= (ldat)0 &&
      Dwn + DeltaYShade > (ldat)ylimit) {
    if (i > 0) {
      Rgt1 = Min2(Rgt + DeltaXShade, Left + i - 1);
      Left1 = Max2(0, Left);
    } else {
      Left1 = Max2(Rgt + DeltaXShade + i, Left);
      Rgt1 = Min2((ldat)DWidth - (ldat)1, Rgt + DeltaXShade);
    }
    DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, (dat)Left1, (dat)Up, (dat)Rgt1,
              (dat)Dwn + DeltaYShade, tfalse);

    if (j > 0) {
      Dwn = Min2(Dwn + DeltaYShade, Up + j - 1);
      Up = Max2(ylimit + 1, Up);
    } else {
      Up = Max2(Dwn + DeltaYShade + j, Up);
      Dwn = Min2((ldat)DHeight - (ldat)1, Dwn + DeltaYShade);
    }
    DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, (dat)Left, (dat)Up, (dat)Rgt + DeltaXShade,
              (dat)Dwn, tfalse);
  }
  if (w == (Twindow)All->FirstScreen->FocusW())
    UpdateCursor();
}

void ResizeRelFirstWindow(dat i, dat j) {
  ldat Left, Up, Rgt, Dwn;
  Tscreen screen;
  Ssetup *SetUp;
  Twindow w;
  dat DeltaX, DeltaY;
  dat ylimit, XWidth, YWidth;
  dat MinXWidth, MinYWidth, MaxXWidth, MaxYWidth;
  dat DWidth, DHeight;
  byte Shade, DeltaXShade, DeltaYShade, hasBorder;

  screen = All->FirstScreen;
  if (!(w = (Twindow)screen->FirstW) || !IS_WINDOW(w) ||
      (!i && !j)) /* || !(w->Attr & WINDOW_RESIZE)) */
    return;

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  SetUp = All->SetUp;
  Shade = !!(SetUp->Flags & setup_shadows);
  DeltaXShade = Shade ? SetUp->DeltaXShade : (byte)0;
  DeltaYShade = Shade ? SetUp->DeltaYShade : (byte)0;
  hasBorder = !(w->Flags & WINDOWFL_BORDERLESS);
  ylimit = screen->Up + 1;

  XWidth = w->XWidth;
  YWidth = w->YWidth;
  MinXWidth = w->MinXWidth;
  MinYWidth = w->MinYWidth;
  MaxXWidth = w->MaxXWidth;
  MaxYWidth = w->MaxYWidth;

  Up = (ldat)w->Up - screen->YLogic + (ldat)ylimit - 1;
  Left = (ldat)w->Left - screen->XLogic;
  Rgt = Left + (ldat)XWidth - (ldat)1;
  Dwn = Up + (ldat)YWidth - (ldat)1;

  if ((DeltaX = -i) > (dat)0 && XWidth > MinXWidth) {
    if (MinXWidth + DeltaX > XWidth)
      DeltaX = XWidth - MinXWidth;
    XWidth = w->XWidth -= DeltaX;
    if (Left < (ldat)DWidth && Up < (ldat)DHeight && Rgt + (ldat)DeltaXShade >= (ldat)0 &&
        Dwn + (ldat)DeltaYShade >= (ldat)ylimit) {

      DrawArea2((Tscreen)0, (Twidget)w, (Twidget)0, (dat)Rgt - DeltaX + 1,
                (dat)Max2(Up, (ldat)ylimit), (dat)Rgt, (dat)Max2((ldat)ylimit, Dwn), tfalse);
      if (Shade) {
        DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)Rgt + Max2((dat)DeltaXShade - DeltaX - 1, 1),
                  (dat)Max2((ldat)ylimit, Up), (dat)Rgt + (dat)DeltaXShade,
                  (dat)Dwn + (dat)DeltaYShade, tfalse);
        if (DeltaX > (dat)DeltaXShade)
          DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)Rgt + (dat)DeltaXShade - DeltaX + 1,
                    (dat)Max2((ldat)ylimit, Dwn + 1), (dat)Rgt, (dat)Dwn + (dat)DeltaYShade,
                    tfalse);
      }
    }
    Rgt -= DeltaX;
  } else if ((DeltaX = i) > (dat)0 && XWidth < MaxXWidth) {
    if (XWidth > MaxXWidth - DeltaX)
      DeltaX = MaxXWidth - XWidth;
    XWidth = w->XWidth += DeltaX;
    if (Left < (ldat)DWidth && Up < (ldat)DHeight && Rgt + (ldat)DeltaXShade >= -(ldat)DeltaX &&
        Dwn + (ldat)DeltaYShade >= (ldat)ylimit) {

      DrawArea2((Tscreen)0, (Twidget)w, (Twidget)0, (dat)Rgt,
                (dat)Max2(Up + hasBorder, (ldat)ylimit), (dat)Rgt + DeltaX - hasBorder,
                (dat)Max2((ldat)ylimit, Dwn - hasBorder), tfalse);
      if (Shade) {
        DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)Rgt + 1 + Max2((dat)DeltaXShade, DeltaX),
                  (dat)Max2((ldat)ylimit, Up + (dat)DeltaYShade),
                  (dat)Rgt + DeltaX + (dat)DeltaXShade, (dat)Dwn + (dat)DeltaYShade, tfalse);
        if (DeltaX > (dat)DeltaXShade)
          DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)Rgt + (dat)DeltaXShade + 1,
                    (dat)Max2((ldat)ylimit, Dwn + 1), (dat)Rgt + DeltaX,
                    (dat)Dwn + (dat)DeltaYShade, tfalse);
      }
    }
    Rgt += DeltaX;
  }
  if ((DeltaY = -j) > (dat)0 && YWidth > MinYWidth) {
    if (MinYWidth + DeltaY > YWidth)
      DeltaY = YWidth - MinYWidth;
    YWidth = w->YWidth -= DeltaY;
    if (Left < (ldat)DWidth && Up < (ldat)DHeight && Rgt + (ldat)DeltaXShade >= (ldat)0 &&
        Dwn + (ldat)DeltaYShade >= (ldat)ylimit) {

      DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)Left,
                (dat)Max2(Dwn - (ldat)DeltaY + (ldat)1, (ldat)ylimit), (dat)Rgt,
                (dat)Max2((ldat)ylimit, Dwn), tfalse);
      if (Shade) {
        DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)Left,
                  (dat)Max2((ldat)ylimit, Dwn + 1 + Max2((dat)DeltaYShade - DeltaY, (dat)0)),
                  (dat)Rgt + (dat)DeltaXShade, (dat)Dwn + (dat)DeltaYShade, tfalse);
        if (DeltaY > (dat)DeltaYShade)
          DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)Rgt + 1,
                    (dat)Max2((ldat)ylimit, Dwn + 1 + (dat)DeltaYShade - DeltaY),
                    (dat)Rgt + (dat)DeltaXShade, (dat)Max2((ldat)ylimit, Dwn), tfalse);
      }
    }
    Dwn -= DeltaY;
  } else if ((DeltaY = j) > (dat)0 && YWidth < MaxYWidth) {
    if (YWidth > MaxYWidth - DeltaY)
      DeltaY = MaxYWidth - YWidth;
    YWidth = w->YWidth += DeltaY;
    if (Left < (ldat)DWidth && Up < (ldat)DHeight && Rgt + (ldat)DeltaXShade >= (ldat)0 &&
        Dwn + (ldat)DeltaYShade >= -(ldat)DeltaY + (ldat)ylimit) {

      DrawArea2((Tscreen)0, (Twidget)w, (Twidget)0, (dat)Left, (dat)Max2((ldat)ylimit, Dwn),
                (dat)Rgt - hasBorder, (dat)Max2((ldat)ylimit, Dwn + DeltaY - hasBorder), tfalse);
      if (Shade) {
        DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)Left + (dat)DeltaXShade,
                  (dat)Dwn + 1 + Max2(DeltaY, (dat)DeltaYShade), (dat)Rgt + (dat)DeltaXShade,
                  (dat)Dwn + (dat)DeltaYShade + DeltaY, tfalse);
        if (DeltaY > (dat)DeltaYShade)
          DrawArea2(screen, (Twidget)0, (Twidget)0, (dat)Rgt + 1, (dat)Dwn + (dat)DeltaYShade + 1,
                    (dat)Rgt + (dat)DeltaXShade, (dat)Max2((ldat)ylimit, Dwn + DeltaY), tfalse);
      }
    }
    Dwn += DeltaY;
  }
  if (DeltaX || DeltaY) {
    DrawBorderWindow(w, BORDER_ANY);
    if (w == (Twindow)screen->FocusW())
      UpdateCursor();

    /* resize contents? for Interactive Resize, let the WM resize it
     * when Interactive Resize finishes. otherwise, do it now */
    if (W_USE(w, USECONTENTS) && w->USE.C.Contents &&
        (w != screen->ClickWindow || (All->State & state_any) != state_resize))

      CheckResizeWindowContents(w);
  }
}

void ResizeRelWindow(Twindow w, dat i, dat j) {
  ldat Left, Up, Rgt, Dwn;
  Twidget Parent;
  Ssetup *SetUp;
  dat DeltaX, DeltaY;
  dat ylimit, XWidth, YWidth;
  dat MinXWidth, MinYWidth, MaxXWidth, MaxYWidth;
  byte Shade, DeltaXShade, DeltaYShade, visible;

  if (!w || (!i && !j)) /* || !(w->Attr & WINDOW_RESIZE) */
    return;

  visible = !(w->Flags & WIDGETFL_NOTVISIBLE);

  if (visible && (Twidget)w == All->FirstScreen->FirstW) {
    ResizeRelFirstWindow(i, j);
    return;
  }

  XWidth = w->XWidth;
  YWidth = w->YWidth;
  MinXWidth = w->MinXWidth;
  MinYWidth = w->MinYWidth;
  MaxXWidth = w->MaxXWidth;
  MaxYWidth = w->MaxYWidth;

  if (visible && (Parent = w->Parent) && IS_SCREEN(Parent)) {
    SetUp = All->SetUp;
    Shade = !!(SetUp->Flags & setup_shadows);
    DeltaXShade = Shade ? SetUp->DeltaXShade : (byte)0;
    DeltaYShade = Shade ? SetUp->DeltaYShade : (byte)0;
    ylimit = Parent->Up;

    Up = (ldat)w->Up - Parent->YLogic + (ldat)ylimit;
    Left = (ldat)w->Left - Parent->XLogic;
    Rgt = Left + (ldat)XWidth - (ldat)1 + DeltaXShade;
    Dwn = Up + (ldat)YWidth - (ldat)1 + DeltaYShade;
  }

  if ((DeltaX = -i) > (dat)0 && XWidth > MinXWidth) {
    if (MinXWidth + DeltaX > XWidth)
      DeltaX = XWidth - MinXWidth;
    XWidth = w->XWidth -= DeltaX;
  } else if ((DeltaX = i) > (dat)0 && XWidth < MaxXWidth) {
    if (XWidth > MaxXWidth - DeltaX)
      DeltaX = MaxXWidth - XWidth;
    XWidth = w->XWidth += DeltaX;
    Rgt += DeltaX;
  }
  if ((DeltaY = -j) > (dat)0 && YWidth > MinYWidth) {
    if (MinYWidth + DeltaY > YWidth)
      DeltaY = YWidth - MinYWidth;
    YWidth = w->YWidth -= DeltaY;
  } else if ((DeltaY = j) > (dat)0 && YWidth < MaxYWidth) {
    if (YWidth > MaxYWidth - DeltaY)
      DeltaY = MaxYWidth - YWidth;
    YWidth = w->YWidth += DeltaY;
    Dwn += DeltaY;
  }
  if (DeltaX || DeltaY) {
    if (visible && Parent && IS_SCREEN(Parent)) {
      Up = (dat)Max2(Up, (ldat)ylimit);
      DrawArea2((Tscreen)Parent, (Twidget)0, (Twidget)0, (dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn,
                tfalse);
      if (Shade)
        DrawShadeWindow(w, 0, 0, TW_MAXDAT, TW_MAXDAT, tfalse);
    }

    if (visible && w == (Twindow)All->FirstScreen->FocusW())
      UpdateCursor();

    /* resize contents? for Interactive Resize, let the WM resize it
     * when Interactive Resize finishes. otherwise, do it now */
    if (W_USE(w, USECONTENTS) && w->USE.C.Contents &&
        (w != All->FirstScreen->ClickWindow || (All->State & state_any) != state_resize))

      CheckResizeWindowContents(w);
  }
}

/*
 * coordinates are special here:
 * (0,0) is the (XLogic,YLogic) cell of the w,
 * i.e. the first visible one.
 */
void ScrollFirstWindowArea(dat X1, dat Y1, dat X2, dat Y2, ldat DeltaX, ldat DeltaY) {
  Tscreen screen;
  dat DWidth, DHeight;
  Twindow w;
  ldat shLeft, shUp;
  dat Left, Up, Rgt, Dwn;
  dat Xstart, Ystart, Xend, Yend;
  dat XWidth, YWidth, ylimit;

  screen = All->FirstScreen;
  w = (Twindow)screen->FirstW;

  if (!w || !IS_WINDOW(w) || (w->Attr & WINDOW_ROLLED_UP))
    return;

  XWidth = w->XWidth;
  YWidth = w->YWidth;

  if (X1 > X2 || X1 > XWidth - 3 || X2 < 0 || Y1 > Y2 || Y1 > YWidth - 3 || Y2 < 0)
    return;

  if (DeltaX >= XWidth || -DeltaX >= XWidth || DeltaY >= YWidth || -DeltaY >= YWidth) {
    DrawWidget((Twidget)w, (dat)0, (dat)0, TW_MAXDAT, TW_MAXDAT, tfalse);
    return;
  }

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  ylimit = screen->Up;
  shUp = (ldat)w->Up - screen->YLogic + (ldat)ylimit;
  shLeft = (ldat)w->Left - screen->XLogic;

  X1 = Max2(X1, 0);
  Y1 = Max2(Y1, 0);
  X2 = Min2(X2, XWidth - 3);
  Y2 = Min2(Y2, YWidth - 3);

  Left = (dat)(shLeft + (ldat)1 + (ldat)X1);
  if (Left < 0)
    X1 += -Left, Left = 0;
  else if (Left >= DWidth)
    return;

  Up = (dat)(shUp + (ldat)1 + (ldat)Y1);
  if (Up <= (dat)ylimit)
    Y1 += (dat)ylimit + 1 - Up, Up = (dat)ylimit + 1;
  else if (Up >= DHeight)
    return;

  Rgt = (dat)(shLeft + (ldat)1 + (ldat)X2);
  if (Rgt >= DWidth)
    X2 -= Rgt - DWidth, Rgt = DWidth - 1;
  else if (Rgt < 0)
    return;

  Dwn = (dat)(shUp + (ldat)1 + (ldat)Y2);
  if (Dwn >= DHeight)
    Y2 -= Dwn - DHeight, Dwn = DHeight - 1;
  else if (Dwn < 0)
    return;

  Xstart = Left + Max2(-DeltaX, (dat)0);
  Ystart = Up + Max2(-DeltaY, (dat)0);
  Xend = Rgt - Max2(DeltaX, (dat)0);
  Yend = Dwn - Max2(DeltaY, (dat)0);

  if (DeltaX) {
    if (Xend >= Xstart)
      DragArea(Xstart, Ystart, Xend, Yend, Xstart + DeltaX, Ystart);
    if (DeltaX > (dat)0)
      DrawWidget((Twidget)w, Left, Up, Left + DeltaX - 1, Dwn, tfalse);
    else
      DrawWidget((Twidget)w, Rgt + DeltaX + 1, Up, Rgt, Dwn, tfalse);
  } else if (DeltaY) {
    if (Yend >= Ystart)
      DragArea(Xstart, Ystart, Xend, Yend, Xstart, Ystart + DeltaY);
    if (DeltaY > (dat)0)
      DrawWidget((Twidget)w, Left, Up, Rgt, Up + DeltaY - 1, tfalse);
    else
      DrawWidget((Twidget)w, Left, Dwn + DeltaY + 1, Rgt, Dwn, tfalse);
  }
}

void ScrollFirstWindow(ldat DeltaX, ldat DeltaY, byte byXYLogic) {
  Twindow w;
  ldat XLogic, YLogic;
  dat XWidth, YWidth;

  w = (Twindow)All->FirstScreen->FirstW;
  if (!w || !IS_WINDOW(w))
    return;

  XWidth = w->XWidth;
  YWidth = w->YWidth;

  if (byXYLogic) {
    XLogic = w->XLogic;
    YLogic = w->YLogic;

    if (DeltaX > 0 && XLogic >= TW_MAXLDAT - DeltaX)
      DeltaX = TW_MAXLDAT - XLogic - 1;
    else if (DeltaX < 0 && XLogic < -DeltaX)
      DeltaX = -XLogic;
    if (!W_USE(w, USEROWS)) {
      /*
       * WARNING: w->USE.C.Contents and other methods
       * may be unable to handle out-of-bound rows
       */
      if (DeltaY > 0 && YLogic + YWidth - 2 + DeltaY >= w->HLogic)
        DeltaY = w->HLogic - YLogic - YWidth + 2;
      else if (DeltaY < 0 && YLogic < -DeltaY)
        DeltaY = -YLogic;
    } else {
      if (DeltaY > 0 && YLogic + DeltaY >= w->HLogic)
        DeltaY = w->HLogic - YLogic - 1;
      else if (DeltaY < 0 && YLogic < -DeltaY)
        DeltaY = -YLogic;
    }
    if (DeltaX)
      w->XLogic = (XLogic += DeltaX);
    if (DeltaY)
      w->YLogic = (YLogic += DeltaY);
    if (DeltaX || DeltaY)
      DrawBorderWindow(w, (DeltaX ? BORDER_DOWN : 0) | (DeltaY ? BORDER_RIGHT : 0));
  }

  ScrollFirstWindowArea(0, 0, XWidth - 3, YWidth - 3, -DeltaX, -DeltaY);

  if (byXYLogic && ContainsCursor((Twidget)w))
    UpdateCursor();
}

void ScrollWindow(Twindow w, ldat DeltaX, ldat DeltaY) {
  ldat XLogic, YLogic;
  dat YWidth;

  if (!w || !IS_WINDOW(w) || (!DeltaX && !DeltaY))
    return;

  if (!(w->Attr & WINDOW_X_BAR))
    DeltaX = 0;

  if (!(w->Attr & WINDOW_Y_BAR))
    DeltaY = 0;

  if (!DeltaX && !DeltaY)
    return;

  if ((Twidget)w == All->FirstScreen->FirstW) {
    ScrollFirstWindow(DeltaX, DeltaY, ttrue);
    return;
  }

  YWidth = w->YWidth;
  XLogic = w->XLogic;
  YLogic = w->YLogic;

  if (DeltaX > 0 && XLogic >= TW_MAXLDAT - DeltaX)
    DeltaX = TW_MAXLDAT - XLogic - 1;
  else if (DeltaX < 0 && XLogic < -DeltaX)
    DeltaX = -XLogic;
  if (!W_USE(w, USEROWS)) {
    /*
     * WARNING: w->USE.C.Contents and other methods
     * may be unable to handle out-of-bound rows
     */
    if (DeltaY > 0 && YLogic + YWidth - 2 + DeltaY >= w->HLogic)
      DeltaY = w->HLogic - YLogic - YWidth + 2;
    else if (DeltaY < 0 && YLogic < -DeltaY)
      DeltaY = -YLogic;
  } else {
    if (DeltaY > 0 && YLogic + DeltaY >= w->HLogic)
      DeltaY = w->HLogic - YLogic - 1;
    else if (DeltaY < 0 && YLogic < -DeltaY)
      DeltaY = -YLogic;
  }
  if (DeltaX)
    w->XLogic = (XLogic += DeltaX);
  if (DeltaY)
    w->YLogic = (YLogic += DeltaY);

  DrawFullWindow2(w);

  if (ContainsCursor((Twidget)w))
    UpdateCursor();
}

void ScrollWidget(Twidget w, ldat DeltaX, ldat DeltaY) {
  ldat XLogic, YLogic;
  dat YWidth;

  if (!w || !(DeltaX || DeltaY))
    return;

  if (IS_WINDOW(w)) {
    ScrollWindow((Twindow)w, DeltaX, DeltaY);
    return;
  }

  YWidth = w->YWidth;
  XLogic = w->XLogic;
  YLogic = w->YLogic;

  if (DeltaX > 0 && XLogic >= TW_MAXLDAT - DeltaX)
    DeltaX = TW_MAXLDAT - XLogic - 1;
  else if (DeltaX < 0 && XLogic < -DeltaX)
    DeltaX = -XLogic;
  /*
   * WARNING: content methods may be
   * unable to handle out-of-bound areas
   */
  if (DeltaY > 0 && YLogic + YWidth - 2 >= TW_MAXLDAT - DeltaY)
    DeltaY = TW_MAXLDAT - YLogic - YWidth + 2;
  else if (DeltaY < 0 && YLogic < -DeltaY)
    DeltaY = -YLogic;

  if (DeltaX)
    w->XLogic = (XLogic += DeltaX);
  if (DeltaY)
    w->YLogic = (YLogic += DeltaY);

  DrawAreaWidget(w);

  if (ContainsCursor(w))
    UpdateCursor();
}

byte ExecScrollFocusWindow(void) {
  Tscreen screen;
  dat DWidth, DHeight;
  Twindow w;
  uldat Attr, State, Scroll;
  dat XWidth, YWidth;
  dat DeltaX, DeltaY;

  if ((All->State & state_any) != state_scroll)
    return tfalse;

  if (!(screen = All->FirstScreen) || !(w = (Twindow)screen->FocusW()) || !IS_WINDOW(w))
    return tfalse;

  Attr = w->Attr;
  State = w->State;
  DeltaX = DeltaY = (sbyte)0;

  if (Attr & WINDOW_X_BAR && State & X_BAR_SELECT)
    DeltaX = 1;
  else if (Attr & WINDOW_Y_BAR && State & Y_BAR_SELECT)
    DeltaY = 1;
  else
    return tfalse;

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  XWidth = w->XWidth;
  YWidth = w->YWidth;

  XWidth = Min2(XWidth, DWidth);
  YWidth = Min2(YWidth, DHeight);
  Scroll = State & SCROLL_ANY_SELECT;
  if (Scroll != ARROW_BACK_SELECT && Scroll != ARROW_FWD_SELECT && Scroll != PAGE_BACK_SELECT &&
      Scroll != PAGE_FWD_SELECT && Scroll != TAB_SELECT)
    return tfalse;

  if (Scroll == ARROW_BACK_SELECT) {
    DeltaX = -DeltaX;
    DeltaY = -DeltaY;
  } else if (Scroll == ARROW_FWD_SELECT)
    ;
  else if (Scroll == PAGE_BACK_SELECT) {
    DeltaX *= -(XWidth - 3);
    DeltaY *= -(YWidth - 3);
  } else if (Scroll == PAGE_FWD_SELECT) {
    DeltaX *= (XWidth - 3);
    DeltaY *= (YWidth - 3);
  } else if (Scroll == TAB_SELECT)
    return tfalse;

  if ((Twidget)w == screen->FirstW)
    ScrollFirstWindow(DeltaX, DeltaY, ttrue);
  else
    ScrollWindow(w, DeltaX, DeltaY);
  return ttrue;
}

/* Tmenu traversing functions */

void HideMenu(byte on_off) {
  Tscreen screen = All->FirstScreen;

  if (on_off) {
    if (screen->Up == 0) {
      if (screen->YLogic > TW_MINDAT) {
        screen->YLogic--;
        screen->Up--;
        DrawArea2(screen, (Twidget)0, (Twidget)0, 0, 0, TW_MAXDAT, 0, tfalse);
        UpdateCursor();
      } else
        ResizeFirstScreen(-1);
    }
  } else {
    if (screen->Up == -1) {
      if (screen->YLogic < TW_MAXDAT) {
        screen->YLogic++;
        screen->Up++;
        Act(DrawMenu, screen)(screen, 0, TW_MAXDAT);
        UpdateCursor();
      } else
        ResizeFirstScreen(1);
    }
  }
}

static void OpenSubMenuItem(Tmenu M, Tmenuitem item, bool by_mouse) {
  Twindow P = (Twindow)item->Parent;
  Twindow w = item->Window;
  Tscreen S = All->FirstScreen;
  ldat y = P->CurY;

  P->CurY = item->WCurY;
  if (y != TW_MAXLDAT)
    DrawLogicWidget((Twidget)P, 0, y, TW_MAXLDAT, y);
  if ((y = P->CurY) == TW_MAXLDAT)
    (void)Act(FindRowByCode, P)(P, item->Code, &P->CurY);
  if ((y = P->CurY) != TW_MAXLDAT)
    DrawLogicWidget((Twidget)P, 0, y, TW_MAXLDAT, y);

  if (w) {
    if (!w->Parent) {
      w->Left = P->Left + P->XWidth - S->XLogic;
      w->Up = P->Up + P->CurY - P->YLogic - S->YLogic + 1;
    }
    if (by_mouse)
      w->CurY = TW_MAXLDAT;
    else if (w->CurY == TW_MAXLDAT)
      w->CurY = 0;
    Act(MapTopReal, w)(w, S);
  }
  if ((Twidget)P != S->FocusW())
    Act(Focus, P)(P);
}

static void OpenTopMenuItem(Tmenu M, Tmenuitem item, bool by_mouse) {
  Tmenu _M = (Tmenu)item->Parent; /* may either be M or All->CommonMenu */
  Twindow w = item->Window;

  if (!w->Parent) {
    w->Up = 1;
    w->Left = item->Left;

    if (M != _M && M->LastI)
      /* adjust common Tmenu w->Left to the item position in this Tmenu */
      w->Left += M->LastI->Left + M->LastI->Len;
  }

  Act(SetSelectedItem, M)(M, item);

  if (by_mouse)
    w->CurY = TW_MAXLDAT;
  else if (w->CurY == TW_MAXLDAT)
    w->CurY = (ldat)0;

  if (item->Flags & ROW_ACTIVE)
    w->Flags &= ~WINDOWFL_DISABLED;
  else
    w->Flags |= WINDOWFL_DISABLED;

  Act(MapTopReal, w)(w, All->FirstScreen);

  if (w->CurY != TW_MAXLDAT && (item = (Tmenuitem)Act(FindRow, w)(w, w->CurY)))
    OpenSubMenuItem(M, item, by_mouse);
}

static void OpenMenuItem(Tmenu M, Tmenuitem item, bool by_mouse) {
  if (item) {
    Tobj O = item->Parent;
    if (O && IS_WINDOW(O))
      OpenSubMenuItem(M, item, by_mouse);
    else
      OpenTopMenuItem(M, item, by_mouse);
  } else
    Act(SetSelectedItem, M)(M, (Tmenuitem)0);
}

/* this activates the Tmenu bar */
static void OpenMenu(Tmenuitem item, bool by_mouse) {
  Tscreen S = All->FirstScreen;
  Twidget w = S->FocusW();
  Tmenu M = Act(FindMenu, S)(S);

  if ((All->State & state_any) == state_default) {

    All->State = state_menu | (by_mouse ? state_fl_bymouse : 0);

    if (All->SetUp->Flags & setup_menu_hide)
      HideMenu(tfalse);

    if (!S->MenuWindow && w) {
      S->MenuWindow = (Twindow)w; /* so that it keeps `active' borders */
      S->FocusW((Twidget)0);
    }
  }
  OpenMenuItem(M, item, by_mouse);
}

/*
 * up one level; return new selected item;
 * do NOT use to close the Tmenu, CloseMenu() does that
 */
static Tmenuitem CloseMenuItem(Tmenu M, Tmenuitem item, bool by_mouse) {
  Twindow P = (Twindow)item->Parent, w = item->Window;

  if (w)
    Act(UnMap, w)(w);

  if (P && IS_WINDOW(P)) {
    if (by_mouse) {
      ldat y = P->CurY;
      P->CurY = TW_MAXLDAT;

      if (y != TW_MAXLDAT)
        DrawLogicWidget((Twidget)P, 0, y, TW_MAXLDAT, y);
    }
    item = P->MenuItem;
    if (item) {
      w = (Twindow)item->Parent;
      if (w && IS_WINDOW(w))
        w->Focus();
      else
        P->Focus();
    }
    return item;
  } else {
    item = (Tmenuitem)0;
    Act(SetSelectedItem, M)(M, item);
    return item;
  }
}

static dat DepthOfMenuItem(Tmenuitem I) {
  Twindow w;
  dat d = 0;

  while (I && (w = (Twindow)I->Parent) && IS_WINDOW(w)) {
    I = w->MenuItem;
    d++;
  }
  return d;
}

/* this traverses the Tmenu bar as needed */
static void TraverseMenu(Tmenu M, Tmenuitem OldItem, dat Odepth, Tmenuitem NewItem, dat Ndepth,
                         bool by_mouse) {
  while (Odepth > Ndepth && OldItem) {
    Odepth--;
    OldItem = CloseMenuItem(M, OldItem, by_mouse);
  }
  /* paranoia */
  Odepth = DepthOfMenuItem(OldItem);

  if (Ndepth == Odepth) {
    if (OldItem != NewItem)
      CloseMenuItem(M, OldItem, by_mouse);
    OpenMenuItem(M, NewItem, by_mouse);
  } else if (Ndepth == Odepth + 1) {
    OpenMenuItem(M, NewItem, by_mouse);
  } else
    log(ERROR) << "twin: internal error: unsupported Tmenu traversing.\n";
}

/* close the Tmenu bar */
void CloseMenu(void) {
  Tscreen S = All->FirstScreen;
  Tmenu M = Act(FindMenu, S)(S);
  Tmenuitem item;
  Twindow w;

  if (M) {
    if ((w = S->MenuWindow)) {
      w->KbdFocus();
      S->MenuWindow = NULL;
    } else
      Swindow::KbdFocus(NULL);

    /* close whole currently open Tmenu tree */
    item = Act(GetSelectedItem, M)(M);
    while (item && IS_MENUITEM(item) && (w = (Twindow)item->Window) && IS_WINDOW(w)) {
      item = (Tmenuitem)Act(FindRow, w)(w, w->CurY);
      Act(UnMap, w)(w);
    }
  }
  All->State = state_default;
  if (All->SetUp->Flags & setup_menu_hide)
    HideMenu(ttrue);
  else
    Act(DrawMenu, S)(S, 0, TW_MAXDAT);
}

/*
 * exported interface to open and interact with the Tmenu.
 * do NOT use to close the Tmenu, CloseMenu() does that
 */
void SetMenuState(Tmenuitem item, bool by_mouse) {
  Tscreen S = All->FirstScreen;
  Tmenu M = Act(FindMenu, S)(S);
  Tmenuitem OldItem = (Tmenuitem)0;
  dat Odepth = 0;

  if (M && (item || by_mouse)) {
    if ((All->State & state_any) != state_default)
      OldItem = Act(RecursiveGetSelectedItem, M)(M, &Odepth);
    if (!OldItem)
      OpenMenu(item, by_mouse);
    else if (OldItem != item)
      TraverseMenu(M, OldItem, Odepth, item, DepthOfMenuItem(item), by_mouse);
    UpdateCursor();
  }
}

/* ---------------- */

void RollUpWindow(Twindow w, byte on_off) {
  if (w && !(w->Flags & WINDOWFL_BORDERLESS)) {
    /*
     * you cannot roll-up borderless windows...
     * without a top border you cannot collapse them
     * to their top border :/
     */
    if (on_off && !(w->Attr & WINDOW_ROLLED_UP)) {
      w->Attr |= WINDOW_ROLLED_UP;
      ReDrawRolledUpAreaWindow(w, tfalse);
    } else if (!on_off && (w->Attr & WINDOW_ROLLED_UP)) {
      w->Attr &= ~WINDOW_ROLLED_UP;
      DrawAreaWindow2(w);
    }
    if (w->Parent == (Twidget)All->FirstScreen)
      UpdateCursor();
  }
}

/* ---------------- */

void SetVisibleWidget(Twidget w, byte on_off) {
  byte visible;
  if (w) {
    on_off = !!on_off;
    visible = !(w->Flags & WIDGETFL_NOTVISIBLE);

    if (on_off != visible) {
      w->Flags ^= WIDGETFL_NOTVISIBLE;
      if (IS_WINDOW(w))
        DrawAreaWindow2((Twindow)w);
      else
        DrawAreaWidget(w);
    }
  }
}

void RaiseWidget(Twidget w, bool alsoFocus) {

  if (!w || !w->Parent || !IS_SCREEN(w->Parent)) {
    return;
  }
  Tscreen screen = (Tscreen)w->Parent;

  if (screen->FirstW != w) {
    MoveFirst(W, (Twidget)screen, w);
    if (IS_WINDOW(w))
      DrawAreaWindow2((Twindow)w);
    else
      DrawAreaWidget(w);
  }
  if (screen == All->FirstScreen) {
    if (alsoFocus)
      Act(Focus, w)(w);
    UpdateCursor();
  }

  screen->HookMap();
}

void LowerWidget(Twidget w, bool alsoUnFocus) {
  Tscreen screen;
  Twidget oldw;

  if (w && (screen = (Tscreen)w->Parent) && IS_SCREEN(screen)) {

    if (screen->LastW != w) {
      MoveLast(W, (Twidget)screen, w);
      if (IS_WINDOW(w))
        DrawAreaWindow2((Twindow)w);
      else
        DrawAreaWidget(w);
    }
    if (screen == All->FirstScreen) {
      if (alsoUnFocus) {
        oldw = screen->FirstW;
        if (oldw && IS_WINDOW(oldw) && oldw != w)
          oldw->Focus();
        else
          Swindow::Focus(NULL);
      } else
        UpdateCursor();
    }

    screen->HookMap();
  }
}

void RestackWidgets(Twidget w, uldat N, const Twidget *arrayW) {
  Twidget FW, CW;
  byte need_redraw = tfalse;

  if (w && N && arrayW) {
    for (FW = (Twidget)0; N; N--, arrayW++) {
      /*
       * Allow only children that really have the given parent.
       * Also deny WINDOFL_MENU windows
       */
      if ((CW = *arrayW) && CW->Parent == w && !(CW->Flags & WINDOWFL_MENU)) {
        if (FW && CW != FW->Next) {
          /* restack after arrayW[0] */
          CW->Remove();
          Act(Insert, CW)(CW, w, FW, FW->Next);
          need_redraw = ttrue;
        }
        FW = CW;
      }
    }
    /* FIXME: this is gross */
    if (need_redraw)
      DrawAreaWidget(w);
  }
}

/* ---------------- */

void RestackRows(Tobj O, uldat N, const Trow *arrayR) {
  Trow FR, CR;
  byte need_redraw = tfalse;

  if (O && (IS_MENU(O) || IS_WINDOW(O)) && N && arrayR) {
    for (FR = (Trow)0; N; N--, arrayR++) {
      /*
       * Allow only children that really have the given parent.
       */
      if ((CR = *arrayR) && (Tobj)CR->Window() == O) {
        if (FR && CR != FR->Next) {
          /* restack after arrayR[0] */
          CR->Remove();
          Act(Insert, CR)(CR, (Twindow)O, FR, FR->Next);
          need_redraw = ttrue;
        }
        FR = CR;
      }
    }
    /* FIXME: this is gross */
    if (need_redraw) {
      if (IS_MENU(O))
        SyncMenu((Tmenu)O);
      else
        DrawAreaWidget((Twidget)O);
    }
  }
}

/* ---------------- */

void SendMsgGadget(Tgadget G) {
  Tmsg msg;
  event_gadget *Event;
  if (G->Code && !(G->Flags & GADGETFL_DISABLED)) {
    if ((msg = New(msg)(msg_widget_gadget, 0))) {
      Event = &msg->Event.EventGadget;
      Event->W = G->Parent;
      Event->Code = G->Code;
      Event->Flags = G->Flags;
      SendMsg(G->Owner, msg);
    }
  }
}

static void realUnPressGadget(Tgadget G) {
  G->Flags &= ~GADGETFL_PRESSED;
  if (G->Group && G->Group->SelectG == G)
    G->Group->SelectG = (Tgadget)0;
  if ((Twidget)G == All->FirstScreen->FirstW)
    DrawWidget((Twidget)G, 0, 0, TW_MAXDAT, TW_MAXDAT, tfalse);
  else
    DrawAreaWidget((Twidget)G);
}

static void realPressGadget(Tgadget G) {
  G->Flags |= GADGETFL_PRESSED;
  if (G->Group)
    G->Group->SelectG = G;
  if ((Twidget)G == All->FirstScreen->FirstW)
    DrawWidget((Twidget)G, 0, 0, TW_MAXDAT, TW_MAXDAT, tfalse);
  else
    DrawAreaWidget((Twidget)G);
}

void PressGadget(Tgadget G) {
  if (!(G->Flags & GADGETFL_DISABLED)) {
    /* honour groups */
    if (G->Group && G->Group->SelectG && G->Group->SelectG != G)
      UnPressGadget(G->Group->SelectG, ttrue);

    realPressGadget(G);
    if (G->Flags & GADGETFL_TOGGLE)
      SendMsgGadget(G);
  }
}

void UnPressGadget(Tgadget G, byte maySendMsgIfNotToggle) {
  if (!(G->Flags & GADGETFL_DISABLED)) {
    realUnPressGadget(G);
    if (maySendMsgIfNotToggle || (G->Flags & GADGETFL_TOGGLE))
      SendMsgGadget(G);
  }
}

/* Left < 0 means right-align leaving (-Left-1) margin */
/* Up   < 0 means down-align  leaving (-Up-1)   margin */
void Sgadget::WriteTexts(byte bitmap, dat t_width, dat t_height, const char *charset_bytes,
                         dat left, dat up) {
  Tgadget g = this;
  dat g_width = g->XWidth, g_height = g->YWidth;
  dat t_left = 0, t_up = 0, w, h;

  if (g->Flags & GADGETFL_BUTTON) {
    g_width--;
    g_height--;
  }

  if (!G_USE(g, USETEXT) || left >= g_width || up >= g_height)
    return;

  if (left < 0) {
    left += g_width - t_width + 1;
    if (left < 0) {
      t_left = -left;
      left = 0;
    }
  }
  if (up < 0) {
    up += g_height - t_height + 1;
    if (up < 0) {
      t_up = -up;
      up = 0;
    }
  }
  w = Min2(t_width - t_left, g_width - left);
  h = Min2(t_height - t_up, g_height - up);

  if (w <= 0) {
    return;
  } else if (g->Flags & GADGETFL_BUTTON) {
    g_width++;
    g_height++;
  }
  for (byte i = 0; i < 4; i++, bitmap >>= 1) {
    if ((bitmap & 1) && g->USE.T.Text[i]) {
      trune *g_runes = g->USE.T.Text[i] + left + up * g_width;
      if (charset_bytes) {
        const char *text = charset_bytes + t_left + t_up * t_width;
        /* update the specified part, do not touch the rest */
        while (h-- > 0) {
          dat temp_w = w;
          while (temp_w-- > 0) {
            *g_runes++ = Tutf_CP437_to_UTF_32[(byte)(*text++)];
          }
          g_runes += g_width - w;
          text += t_width - w;
        }
      } else {
        /* clear the specified part of Tgadget contents */
        while (h-- > 0) {
          dat temp_w = w;
          while (temp_w-- > 0)
            *g_runes++ = ' ';
          g_runes += g_width - w;
        }
      }
    }
  }
  DrawAreaWidget(g);
}

/* Left < 0 means right-align leaving (-Left-1) margin */
/* Up   < 0 means down-align  leaving (-Up-1)   margin */
void Sgadget::WriteTRunes(byte bitmap, dat t_width, dat t_height, //
                          const trune *runes, dat left, dat up) {
  Tgadget g = this;
  dat g_width = g->XWidth, g_height = g->YWidth;
  dat t_left = 0, t_up = 0, w, h;

  byte i;

  if (g->Flags & GADGETFL_BUTTON) {
    g_width--;
    g_height--;
  }

  if (!G_USE(g, USETEXT) || left >= g_width || up >= g_height)
    return;

  if (left < 0) {
    left += g_width - t_width + 1;
    if (left < 0) {
      t_left = -left;
      left = 0;
    }
  }
  if (up < 0) {
    up += g_height - t_height + 1;
    if (up < 0) {
      t_up = -up;
      up = 0;
    }
  }
  w = Min2(t_width - t_left, g_width - left);
  h = Min2(t_height - t_up, g_height - up);

  if (w <= 0) {
    return;
  } else if (g->Flags & GADGETFL_BUTTON) {
    g_width++;
    g_height++;
  }

  for (i = 0; i < 4; i++, bitmap >>= 1) {
    if ((bitmap & 1) && g->USE.T.Text[i]) {
      trune *g_runes = g->USE.T.Text[i] + left + up * g_width;
      if (runes) {
        const trune *runes_ptr = runes + t_left + t_up * t_width;
        /* update the specified part, do not touch the rest */
        while (h-- > 0) {
          dat temp_w = w;
          while (temp_w-- > 0) {
            *g_runes++ = *runes_ptr++;
          }
          g_runes += g_width - w;
          runes_ptr += t_width - w;
        }
      } else {
        /* clear the specified part of Tgadget contents */
        while (h-- > 0) {
          dat temp_w = w;
          while (temp_w-- > 0)
            *g_runes++ = ' ';
          g_runes += g_width - w;
        }
      }
    }
  }
  DrawAreaWidget(g);
}

void SyncMenu(Tmenu Menu) {
  Tmenuitem I, PrevI = (Tmenuitem)0;
  Tscreen screen;

  if (Menu) {
    for (I = Menu->FirstI; I; I = I->Next()) {
      if (PrevI)
        I->Left = PrevI->Left + PrevI->Len;
      else
        I->Left = 1;
      PrevI = I;
    }
    for (screen = All->FirstScreen; screen; screen = screen->Next()) {
      if (Act(FindMenu, screen)(screen) == Menu)
        Act(DrawMenu, screen)(screen, 0, TW_MAXDAT);
    }
  }
}
