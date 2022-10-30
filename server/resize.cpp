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

byte NeedUpdateCursor;

void FlushCursor(void) {
  Sdraw d;
  uldat type = NOCURSOR;
  screen Screen;
  Twindow Window;
  Twidget w;
  ldat CurX, CurY;
  byte HasBorder;

  if (NeedUpdateCursor) {
    NeedUpdateCursor = tfalse;

    Screen = All->FirstScreen;
    Window = FindCursorWindow();

    if (Window &&
        ((Window->Flags & WINDOWFL_CURSOR_ON) || (All->SetUp->Flags & setup_cursor_always))) {

      w = (Twidget)Window;

      HasBorder = !(Window->Flags & WINDOWFL_BORDERLESS);

      CurX = Window->CurX - Window->XLogic + HasBorder;
      CurY = Window->CurY - Window->YLogic + HasBorder;

      if (CurX >= 0 && CurX < (Window->XWidth - HasBorder) && CurY >= 0 &&
          CurY < (Window->YWidth - HasBorder) &&
          d.Init(w, (dat)CurX, (dat)CurY, (dat)CurX, (dat)CurY, false) &&
          ((Window == (Twindow)Screen->FirstW && !Window->FirstW) ||
#if 1
           /* widgets and gadgets cannot contain cursor, but they can obscure it */
           w == RecursiveFindWidgetAt((Twidget)Screen, (dat)d.X1, (dat)d.Y1 - Screen->YLimit)
#else
           Window == WindowParent(RecursiveFindWidgetAt((Twidget)Screen, (dat)d.X1,
                                                        (dat)d.Y1 - Screen->YLimit))
#endif
               )) {

        MoveToXY((dat)d.X1, (dat)d.Y1);
        if ((type = Window->CursorType) == NOCURSOR && All->SetUp->Flags & setup_cursor_always)
          type = LINECURSOR;
      }
    }
    SetCursorType(type);
  }
}

static byte SendResizeSignal(Twindow Window) {
  struct winsize wsiz;

  wsiz.ws_col = Window->USE.C.TtyData->SizeX;
  wsiz.ws_row = Window->USE.C.TtyData->SizeY;
  wsiz.ws_xpixel = 0;
  wsiz.ws_ypixel = 0;

#if 0
    /* is this enough on every OS ? */
    return ioctl(Window->RemoteData.Fd, TIOCSWINSZ, &wsiz) == 0;
#else
  return ioctl(Window->RemoteData.Fd, TIOCSWINSZ, &wsiz) == 0 &&
         kill(-Window->RemoteData.ChildPid, SIGWINCH) == 0;
#endif
}

byte CheckResizeWindowContents(Twindow Window) {
  if (W_USE(Window, USECONTENTS) && (Window->USE.C.TtyData->SizeY != Window->YWidth - 2 ||
                                     Window->USE.C.TtyData->SizeX != Window->XWidth - 2)) {
    return ResizeWindowContents(Window);
  }
  return ttrue;
}

byte ResizeWindowContents(Twindow Window) {
  tcell *NewCont, *saveNewCont, *OldCont, *max, h;
  ldat count, common, left;
  ttydata *Data = Window->USE.C.TtyData;
  dat x = Window->XWidth, y = Window->YWidth + Data->ScrollBack;

  if (!(Window->Flags & WINDOWFL_BORDERLESS))
    x -= 2, y -= 2;

  h = TCELL(Window->ColText, ' ');

  /* safety check: */
  if (x > 0 && y > 0) {
    if (!(saveNewCont = NewCont = (tcell *)AllocMem(x * y * sizeof(tcell))))
      return tfalse;

    /*
     * copy the Contents. Quite non-trivial for two reasons:
     * 1. always preserve the cursor line
     * 2. copy as much scrollback as possible
     */
    if ((max = OldCont = Window->USE.C.Contents)) {
      max += Window->WLogic * Window->HLogic;
      common = Min2(Window->WLogic, x);
      count = Min2(Window->HLogic, y);
      OldCont += Window->USE.C.HSplit * Window->WLogic;

      /* ok, now check for the cursor line */
      if (Window->CurY >= y) {
        OldCont += (Window->CurY + 1 - y) * Window->WLogic;
        if (OldCont > max)
          OldCont -= Window->WLogic * Window->HLogic;
      }

      while (count--) {
        CopyMem(OldCont, NewCont, common * sizeof(tcell));

        if ((OldCont += Window->WLogic) >= max)
          OldCont = Window->USE.C.Contents;
        NewCont += common;
        for (left = x - common; left; left--)
          *NewCont++ = h;
      }
      FreeMem(Window->USE.C.Contents);
    }

    left = (saveNewCont + x * y) - NewCont;
    while (left--)
      *NewCont++ = h;
  } else {
    x = y = 0;
    if (Window->USE.C.Contents)
      FreeMem(Window->USE.C.Contents);
    saveNewCont = NULL;
  }

  Window->XLogic = 0;
  Window->YLogic = Data->ScrollBack;
  Window->WLogic = x;       /* Contents width */
  Window->HLogic = y;       /* Y visible + scrollback */
  Window->USE.C.HSplit = 0; /* splitline == 0 */
  Window->USE.C.Contents = saveNewCont;

  if (Window->CurY >= y)
    Window->CurY = y - 1;
  else if (Window->CurY < Window->YLogic)
    Window->CurY = Window->YLogic;

  Data->SizeX = x;
  Data->SizeY = Window->YWidth - 2;
  Data->Top = 0;
  Data->Bottom = Data->SizeY;

  Data->Start = Window->USE.C.Contents + x * Data->ScrollBack;
  Data->Split = Window->USE.C.Contents + x * y;
  Data->saveX = Data->X = Window->CurX;
  Data->saveY = Data->Y = Window->CurY - Data->ScrollBack;
  Data->Pos = Window->USE.C.Contents + Window->CurY * x + Window->CurX;

  if (!(Window->Attr & WINDOW_WANT_CHANGES) && Window->USE.C.TtyData &&
      Window->RemoteData.FdSlot != NOSLOT)
    /* the MsgPort will not be informed of the resize...
     * we must send SIGWINCH manually */
    (void)SendResizeSignal(Window);

  if (Window->Parent) {
    DrawBorderWindow(Window, BORDER_RIGHT);

    DrawLogicWidget((Twidget)Window, 0, 0, Window->WLogic - 1, Window->HLogic - 1);

    if (ContainsCursor((Twidget)Window))
      UpdateCursor();
  }

  return ttrue;
}

static Trow InsertRowsWindow(Twindow Window, ldat NumRows) {
  Trow CurrRow;

  while (NumRows--) {
    if ((CurrRow = New(row)(0, ROW_ACTIVE))) {
      Act(Insert, CurrRow)(CurrRow, Window, Window->USE.R.LastRow, NULL);
    } else
      break;
  }
  DrawBorderWindow(Window, BORDER_RIGHT);
  return CurrRow;
}

byte EnsureLenRow(Trow Row, uldat Len, byte DefaultCol) {
  trune *tempText;
  tcolor *tempColText;
  uldat NewLen;

  if (Len > Row->MaxLen) {
    NewLen = (Len + (Len >> 1)) | All->SetUp->MinAllocSize;
    if ((tempText = (trune *)ReAllocMem(Row->Text, NewLen * sizeof(trune)))) {
      if (!(Row->Flags & ROW_DEFCOL) && !DefaultCol) {
        if ((tempColText = (tcolor *)ReAllocMem(Row->ColText, NewLen * sizeof(tcolor))))
          Row->ColText = tempColText;
        else
          return tfalse;
      }
      Row->Text = tempText;
      Row->MaxLen = NewLen;
    } else
      return tfalse;
  }
  return ttrue;
}

byte RowWriteCharset(Twindow Window, uldat Len, const char *charset_bytes) {
  Trow CurrRow;
  const char *_Text;
  byte ModeInsert;
  trune const *to_UTF_32;
  ldat x, y, max;
  uldat i, RowLen;

  if (!Window || (Len && !charset_bytes) || !W_USE(Window, USEROWS))
    return tfalse;

  x = Window->CurX;
  y = Window->CurY;
  max = Window->HLogic;
  CurrRow = Window->USE.R.LastRow;
  ModeInsert = Window->Flags & WINDOWFL_ROWS_INSERT;

  if (Window->State & WINDOW_ANYSEL)
    ClearHilight(Window);

  while (Len) {
    if (max <= y || (max == y + 1 && (*charset_bytes == '\n' || *charset_bytes == '\r'))) {
      if (InsertRowsWindow(Window, Max2(y + 1 - max, 1))) {
        max = Window->HLogic;
        CurrRow = Window->USE.R.LastRow;
      } else
        return tfalse;
    } else {
      CurrRow = Act(FindRow, Window)(Window, y);
    }

    RowLen = 0;
    _Text = charset_bytes;
    while (RowLen < Len && *_Text != '\n' && *_Text != '\r')
      ++RowLen, ++_Text;

    /*        WINDOWFL_INSERT non implementato */
    /*  Gap non implementato                                 */

    if (RowLen) {
      if (ModeInsert || (CurrRow && CurrRow->LenGap))
        return tfalse;

      if (!EnsureLenRow(CurrRow, x + RowLen, (Window->Flags & WINDOWFL_ROWS_DEFCOL)))
        return tfalse;

      if (Window->USE.R.NumRowOne == y)
        Window->USE.R.RowOne = CurrRow;
      if (Window->USE.R.NumRowSplit == y)
        Window->USE.R.RowSplit = CurrRow;
      CurrRow->Flags = ROW_ACTIVE;

      to_UTF_32 = Window->Charset;

      for (i = (uldat)Max2(0, -x); i < RowLen; i++)
        CurrRow->Text[x + i] = to_UTF_32[(byte)charset_bytes[i]];
      if (x >= 0 && (uldat)x > CurrRow->Len)
        for (i = CurrRow->Len; i < (uldat)x; i++)
          CurrRow->Text[i] = (trune)' ';

      if (!(Window->Flags & WINDOWFL_ROWS_DEFCOL)) {
        memset(CurrRow->ColText + x, Window->ColText, sizeof(tcolor) * RowLen);
        if (x >= 0 && (uldat)x > CurrRow->Len)
          memset(CurrRow->ColText + CurrRow->Len, Window->ColText,
                 sizeof(tcolor) * (x - CurrRow->Len));
      }

      if (CurrRow->Len < x + RowLen)
        CurrRow->Len = x + RowLen;

      DrawLogicWidget((Twidget)Window, x, y, x + RowLen - (ldat)1, y);

      charset_bytes += RowLen;
      Len -= RowLen;
    }

    if (Len && (*charset_bytes == '\n' || *charset_bytes == '\r')) {
      Window->CurX = x = (ldat)0;
      Window->CurY = ++y;
      charset_bytes++, Len--;
    } else
      Window->CurX = x += RowLen;
  }

  if (Window == FindCursorWindow())
    UpdateCursor();

  return ttrue;
}

byte RowWriteUtf8(Twindow w, uldat len, const char *utf8_bytes) {
  Chars chars(utf8_bytes, len);
  Vector<trune> runes;
  Utf8 seq;

  runes.reserve(len);
  while (chars) {
    seq.parse(chars, &chars);
    runes.append(seq.rune());
  }
  if (!runes) {
    return tfalse;
  }
  return RowWriteTRune(w, runes.size(), runes.data());
}

byte RowWriteTRune(Twindow Window, uldat Len, const trune *runes) {
  Trow CurrRow;
  const trune *_Text;
  byte ModeInsert;
  ldat x, y, max;
  uldat i, RowLen;

  if (!Window || !Len || !W_USE(Window, USEROWS))
    return tfalse;

  x = Window->CurX;
  y = Window->CurY;
  max = Window->HLogic;
  CurrRow = Window->USE.R.LastRow;
  ModeInsert = Window->Flags & WINDOWFL_ROWS_INSERT;

  if (Window->State & WINDOW_ANYSEL)
    ClearHilight(Window);

  while (Len) {
    if (max <= y || (max == y + 1 && (*runes == '\n' || *runes == '\r'))) {
      if (InsertRowsWindow(Window, Max2(y + 1 - max, 1))) {
        max = Window->HLogic;
        CurrRow = Window->USE.R.LastRow;
      } else
        return tfalse;
    } else {
      CurrRow = Act(FindRow, Window)(Window, y);
    }

    RowLen = (ldat)0;
    _Text = runes;
    while (RowLen < Len && *_Text != '\n' && *_Text != '\r')
      ++RowLen, ++_Text;

    /*        WINDOWFL_INSERT non implementato */
    /*  Gap non implementato                                 */

    if (RowLen) {
      if (ModeInsert || (CurrRow && CurrRow->LenGap))
        return tfalse;

      if (!EnsureLenRow(CurrRow, x + RowLen, (Window->Flags & WINDOWFL_ROWS_DEFCOL)))
        return tfalse;

      if (Window->USE.R.NumRowOne == y)
        Window->USE.R.RowOne = CurrRow;
      if (Window->USE.R.NumRowSplit == y)
        Window->USE.R.RowSplit = CurrRow;
      CurrRow->Flags = ROW_ACTIVE;

      if (x >= 0) {
        CopyMem(runes, CurrRow->Text + x, sizeof(trune) * RowLen);
      } else if ((uldat)-x < RowLen) {
        CopyMem(runes - x, CurrRow->Text, sizeof(trune) * (RowLen + x));
      }
      if (x >= 0 && (uldat)x > CurrRow->Len) {
        for (i = CurrRow->Len; i < (uldat)x; i++)
          CurrRow->Text[i] = (trune)' ';
      }

      if (!(Window->Flags & WINDOWFL_ROWS_DEFCOL)) {
        if (x >= 0) {
          memset(CurrRow->ColText + x, Window->ColText, sizeof(tcolor) * RowLen);
        } else if ((uldat)-x < RowLen) {
          memset(CurrRow->ColText, Window->ColText - x, sizeof(tcolor) * (RowLen + x));
        }
        if (x >= 0 && (uldat)x > CurrRow->Len) {
          memset(CurrRow->ColText + CurrRow->Len, Window->ColText,
                 sizeof(tcolor) * (x - CurrRow->Len));
        }
      }

      if (CurrRow->Len < x + RowLen)
        CurrRow->Len = x + RowLen;

      DrawLogicWidget((Twidget)Window, x, y, x + RowLen - (ldat)1, y);

      runes += RowLen;
      Len -= RowLen;
    }

    if (Len && (*runes == '\n' || *runes == '\r')) {
      Window->CurX = x = (ldat)0;
      Window->CurY = ++y;
      runes++, Len--;
    } else
      Window->CurX = x += RowLen;
  }

  if (Window == FindCursorWindow())
    UpdateCursor();

  return ttrue;
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
  ldat CurX, CurY;

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
    CurX = w->USE.C.TtyData->SizeX;
    CurY = w->USE.C.TtyData->SizeY;
    if (Left >= CurY || Up >= CurY)
      return;
    if ((ldat)XWidth + Left > CurX)
      XWidth = CurX - Left;
    if ((ldat)YWidth + Up > CurY)
      YWidth = CurY - Up;
  }
  if (XWidth <= 0 || YWidth <= 0)
    return;

  if (utf8_bytes) {
    byte (*WriteUtf8)(Twindow, uldat, const char *);
    if (W_USE(w, USECONTENTS)) {
      WriteUtf8 = w->Fn->TtyWriteUtf8;
    } else
      WriteUtf8 = w->Fn->RowWriteUtf8;

    CurX = w->CurX;
    CurY = w->CurY;
    for (; YWidth; YWidth--, Up++, utf8_bytes += Pitch) {
      Act(GotoXY, w)(w, Left, Up);
      WriteUtf8(w, (uldat)XWidth, utf8_bytes);
    }
    Act(GotoXY, w)(w, CurX, CurY);

  } else if (runes) {
    byte (*WriteTRune)(Twindow, uldat, const trune *);
    if (W_USE(w, USECONTENTS))
      WriteTRune = w->Fn->TtyWriteTRune;
    else
      WriteTRune = w->Fn->RowWriteTRune;

    CurX = w->CurX;
    CurY = w->CurY;
    for (; YWidth; YWidth--, Up++, runes += Pitch) {
      Act(GotoXY, w)(w, Left, Up);
      WriteTRune(w, (uldat)XWidth, runes);
    }
    Act(GotoXY, w)(w, CurX, CurY);

  } else if (cells) {
    byte (*WriteTCell)(Twindow, dat, dat, uldat, const tcell *);
    if (W_USE(w, USECONTENTS))
      WriteTCell = w->Fn->TtyWriteTCell;
    else
      WriteTCell = w->Fn->RowWriteTCell;

    for (; YWidth; YWidth--, Up++, cells += Pitch)
      WriteTCell(w, Left, Up, (uldat)XWidth, cells);
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
  screen Screen;
  dat DHeight, DWidth;
  dat YLimit;
  ldat Left, Up, Rgt, Dwn;

  Screen = All->FirstScreen;
  YLimit = Screen->YLimit;
  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;

  DeltaX = -DeltaX;
  DeltaY = -DeltaY;

  if (DeltaY < TW_MINDAT - Screen->YLogic)
    DeltaY = TW_MINDAT - Screen->YLogic;
  else if (DeltaY > TW_MAXDAT - Screen->YLogic - (ldat)DHeight + 1)
    DeltaY = TW_MAXDAT - Screen->YLogic - (ldat)DHeight + 1;
  if (DeltaX < TW_MINDAT - Screen->XLogic)
    DeltaX = TW_MINDAT - Screen->XLogic;
  else if (DeltaX > TW_MAXDAT - Screen->XLogic - (ldat)DWidth + 1)
    DeltaX = TW_MAXDAT - Screen->XLogic - (ldat)DWidth + 1;

  if (!DeltaX && !DeltaY)
    return;

  Left = Max2(DeltaX, (ldat)0);
  Up = (ldat)YLimit + 1 + Max2(DeltaY, (ldat)0);
  Rgt = (ldat)DWidth - (ldat)1 + Min2(DeltaX, (ldat)0);
  Dwn = (ldat)DHeight - (ldat)1 + Min2(DeltaY, (ldat)0);
  Screen->XLogic += DeltaX;
  Screen->YLogic += DeltaY;

  if (Up <= Dwn && Left <= Rgt)
    DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)(Left - DeltaX), (dat)(Up - DeltaY));

  YLimit++;

  if (Abs(DeltaX) >= DWidth || Abs(DeltaY) >= DHeight - YLimit)
    DrawArea2((screen)0, (Twidget)0, (Twidget)0, 0, YLimit, DWidth - 1, DHeight - 1, tfalse);
  else if (DeltaY < 0) {
    DrawArea2((screen)0, (Twidget)0, (Twidget)0, 0, YLimit, DWidth - 1, YLimit - DeltaY, tfalse);
    if (DeltaX <= 0)
      DrawArea2((screen)0, (Twidget)0, (Twidget)0, 0, YLimit - DeltaY, -DeltaX - 1, DHeight - 1,
                tfalse);
    else if (DeltaX > 0)
      DrawArea2((screen)0, (Twidget)0, (Twidget)0, DWidth - DeltaX, YLimit - DeltaY, DWidth - 1,
                DHeight - 1, tfalse);
  } else {
    DrawArea2((screen)0, (Twidget)0, (Twidget)0, 0, DHeight - DeltaY, DWidth - 1, DHeight - 1,
              tfalse);
    if (DeltaX <= 0)
      DrawArea2((screen)0, (Twidget)0, (Twidget)0, 0, YLimit, -DeltaX - 1, DHeight - DeltaY,
                tfalse);
    else if (DeltaX >= 0)
      DrawArea2((screen)0, (Twidget)0, (Twidget)0, DWidth - DeltaX, YLimit, DWidth - 1,
                DHeight - DeltaY, tfalse);
  }
  UpdateCursor();
}

void ResizeFirstScreen(dat DeltaY) {
  screen Screen;
  dat DHeight, DWidth;
  dat YLimit;
  ldat Left, Up, Rgt, Dwn;

  Screen = All->FirstScreen;
  YLimit = Screen->YLimit;
  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;

  if (YLimit + DeltaY >= DHeight)
    DeltaY = DHeight - YLimit - 1;
  else if (YLimit + DeltaY < TW_MINDAT)
    DeltaY = TW_MINDAT - YLimit;

  Left = (ldat)0;
  Up = Max2(YLimit, 0);
  Rgt = DWidth - 1;
  Dwn = DHeight - 1 - Max2((ldat)DeltaY, 0);
  Screen->YLimit += DeltaY;

  if (DeltaY < 0) {
    if (Up <= Dwn)
      DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)Left, (dat)Up + DeltaY);
    DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)Left, (dat)Dwn + 1 + DeltaY, (dat)Rgt, (dat)Dwn,
              tfalse);
  } else if (DeltaY > (dat)0) {
    if (Up <= Dwn)
      DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)Left, (dat)Up + (dat)DeltaY);
    DrawArea2((screen)0, (Twidget)0, (Twidget)0, (dat)Left, (dat)Up, (dat)Rgt, (dat)Up + DeltaY - 1,
              tfalse);
  }
  UpdateCursor();
}

void CenterWindow(Twindow Window) {
  screen Screen;
  ldat parz1, parz2;
  ldat DeltaX, DeltaY;
  dat DWidth, DHeight;
  dat YLimit;

  if (!Window || !(Screen = (screen)Window->Parent) || (Screen != All->FirstScreen) ||
      (Window->Flags & WINDOWFL_MENU))
    return;

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  YLimit = Screen->YLimit;
  parz1 = (ldat)Window->Left - (ldat)Screen->XLogic;
  parz2 = parz1 + (ldat)Window->XWidth;
  if (parz1 >= (ldat)0 && parz2 <= (ldat)DWidth)
    DeltaX = (ldat)0;
  else
    DeltaX = -parz1;

  if (Window->XWidth <= (dat)DWidth) {
    parz1 = DeltaX;
    parz2 = DeltaX + (ldat)DWidth - (ldat)Window->XWidth;
    DeltaX = Abs(parz1) < Abs(parz2) ? parz1 : parz2;
  } else
    DeltaX--;

  parz1 = (ldat)Window->Up - (ldat)Screen->YLogic + (ldat)YLimit;
  parz2 = parz1 + (ldat)Window->YWidth;
  if (parz1 > (ldat)YLimit && parz2 <= (ldat)DHeight)
    DeltaY = (ldat)0;
  else
    DeltaY = -parz1 + YLimit + 1;

  if ((ldat)Window->YWidth <= (ldat)DHeight - (ldat)YLimit - 1) {
    parz1 = DeltaY;
    parz2 = DeltaY + (ldat)DHeight - (ldat)YLimit - 1 - (ldat)Window->YWidth;
    DeltaY = Abs(parz1) < Abs(parz2) ? parz1 : parz2;
  } else
    DeltaY--;

  DragFirstScreen(DeltaX, DeltaY);
}

inline void DrawDeltaShadeFirstWindow(dat i, dat j) {
  ldat _Left, _Up, _Rgt, _Dwn;
  ldat Left_, Up_, Rgt_, Dwn_;
  screen Screen;
  Twindow Window;
  dat YLimit;

  Screen = All->FirstScreen;
  if (!(Window = (Twindow)Screen->FirstW) || !IS_WINDOW(Window))
    return;

  YLimit = Screen->YLimit;

  Left_ = (ldat)Window->Left - Screen->XLogic;
  _Left = Left_ - i;
  Rgt_ = Left_ + (ldat)Window->XWidth - (ldat)1;
  _Rgt = Rgt_ - i;
  Up_ = (ldat)Window->Up - Screen->YLogic + (ldat)YLimit;
  _Up = Up_ - j;
  Dwn_ = Up_ + (Window->Attr & WINDOW_ROLLED_UP ? (ldat)0 : (ldat)Window->YWidth - (ldat)1);
  _Dwn = Dwn_ - j;

  DrawAreaShadeWindow(Screen, Window, 0, 0, TW_MAXDAT, TW_MAXDAT, _Left, _Up, _Rgt, _Dwn, tfalse);
  DrawAreaShadeWindow(Screen, Window, 0, 0, TW_MAXDAT, TW_MAXDAT, Left_, Up_, Rgt_, Dwn_, ttrue);
}

void DragFirstWindow(dat i, dat j) {
  ldat Left, Up, Rgt, Dwn;
  ldat _Left, _Up, _Rgt, _Dwn;
  ldat Left_, Up_, Rgt_, Dwn_;
  ldat xLeft, xRgt, xUp, xDwn;
  screen Screen;
  setup *SetUp;
  Twindow Window;
  dat YLimit;
  dat DWidth, DHeight;
  byte Shade;

  Screen = All->FirstScreen;
  if (!(Window = (Twindow)Screen->FirstW) || !IS_WINDOW(Window) || !(Window->Attr & WINDOW_DRAG))
    return;

  YLimit = Screen->YLimit;
  if (i < 0 && Window->Left < TW_MINDAT - i)
    i = TW_MINDAT - Window->Left;
  else if (i > 0 && Window->Left > TW_MAXDAT - i)
    i = TW_MAXDAT - Window->Left;

  if (j < 0 && Window->Up < TW_MINDAT - j)
    j = TW_MINDAT - Window->Up;
  else if (j > 0 && Window->Up > TW_MAXDAT - j)
    j = TW_MAXDAT - Window->Up;

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  SetUp = All->SetUp;
  Shade = !!(SetUp->Flags & setup_shadows);

  Left = (ldat)Window->Left - Screen->XLogic;
  Rgt = Left + (ldat)Window->XWidth - (ldat)1;
  Up = (ldat)Window->Up - Screen->YLogic + (ldat)YLimit;
  Dwn = Up + (Window->Attr & WINDOW_ROLLED_UP ? (ldat)0 : (ldat)Window->YWidth - (ldat)1);

  /* calculate the visible part of the window for direct DragArea() */

  Left_ = Max2(Left, -Min2(i, 0));
  Up_ = Max2(Up, (ldat)YLimit + 1 - Min2(j, 0));
  Rgt_ = Min2(Rgt, (ldat)DWidth - (ldat)1 - Max2(i, 0));
  Dwn_ = Min2(Dwn, (ldat)DHeight - (ldat)1 - Max2(j, 0));

  /* copy the visible part */
  if (Left_ <= Rgt_ && Up_ <= Dwn_)
    DragArea(Left_, Up_, Rgt_, Dwn_, Left_ + i, Up_ + j);

  Window->Left += i;
  Window->Up += j;

  if (Shade)
    /* update the window's shadow */
    DrawDeltaShadeFirstWindow(i, j);

  /* redraw the old window location */

  _Left = Max2(Left, (ldat)0);
  _Up = Max2(Up, (ldat)YLimit + 1);
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
      DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)xLeft, (dat)_Up, (dat)xRgt, (dat)_Dwn, tfalse);
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

      DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)xLeft, (dat)xUp, (dat)xRgt, (dat)xDwn, tfalse);
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
  _Up = Max2(Up + (ldat)j, (ldat)YLimit + 1);
  _Rgt = Min2(Rgt + (ldat)i, (ldat)DWidth - (ldat)1);
  _Dwn = Min2(Dwn + (ldat)j, (ldat)DHeight - (ldat)1);

  if (_Left <= _Rgt && _Up <= _Dwn) {
    if (Left_ > _Left) {
      xLeft = Min2(Left_ - (ldat)1, _Rgt);
      DrawWidget((Twidget)Window, (dat)_Left, (dat)_Up, (dat)xLeft, (dat)_Dwn, tfalse);
    } else
      xLeft = _Left;
    if (Rgt_ < _Rgt) {
      xRgt = Max2(Rgt_ + (ldat)1, _Left);
      DrawWidget((Twidget)Window, (dat)xRgt, (dat)_Up, (dat)_Rgt, (dat)_Dwn, tfalse);
    } else
      xRgt = _Rgt;
    if (Up_ > _Up) {
      xUp = Min2(Up_ - (ldat)1, _Dwn);
      DrawWidget((Twidget)Window, (dat)xLeft, (dat)_Up, (dat)xRgt, (dat)xUp, tfalse);
    }
    if (Dwn_ < _Dwn) {
      xDwn = Max2(Dwn_ + (ldat)1, _Up);
      DrawWidget((Twidget)Window, (dat)xLeft, (dat)xDwn, (dat)xRgt, (dat)_Dwn, tfalse);
    }
  }

  if (ContainsCursor((Twidget)Window))
    UpdateCursor();
}

void DragWindow(Twindow Window, dat i, dat j) {
  ldat Left, Up, Rgt, Dwn;
  ldat Left1, Rgt1;
  screen Screen;
  setup *SetUp;
  dat YLimit;
  dat DWidth, DHeight;
  byte Shade, DeltaXShade, DeltaYShade;

  if (!Window || !(Window->Attr & WINDOW_DRAG))
    return;

  if (Window == (Twindow)All->FirstScreen->FirstW) {
    DragFirstWindow(i, j);
    return;
  }

  if (!(Screen = (screen)Window->Parent) || !IS_SCREEN(Screen))
    return;

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  SetUp = All->SetUp;
  Shade = !!(SetUp->Flags & setup_shadows);
  DeltaXShade = Shade ? SetUp->DeltaXShade : (byte)0;
  DeltaYShade = Shade ? SetUp->DeltaYShade : (byte)0;
  YLimit = Screen->YLimit;

  Up = (ldat)Window->Up - Screen->YLogic + (ldat)YLimit;
  Left = (ldat)Window->Left - Screen->XLogic;
  Rgt = Left + (ldat)Window->XWidth - (ldat)1;
  Dwn = Up + (Window->Attr & WINDOW_ROLLED_UP ? (ldat)0 : (ldat)Window->YWidth - (ldat)1);

  if (i < (dat)0) {
    if (Window->Left < TW_MINDAT - i)
      i = TW_MINDAT - Window->Left;
  } else if (i > (dat)0) {
    if (Window->Left > TW_MAXDAT - i)
      i = TW_MAXDAT - Window->Left;
  }
  if (j < (dat)0) {
    if (Window->Up < TW_MINDAT - j)
      j = TW_MINDAT - Window->Up;
  } else if (j > (dat)0) {
    if (Window->Up > TW_MAXDAT - j)
      j = TW_MAXDAT - Window->Up;
  }
  Window->Left += i;
  Window->Up += j;
  DrawArea2((screen)0, (Twidget)0, (Twidget)0, Left + i, Up + j, Rgt + i + DeltaXShade,
            Dwn + j + DeltaYShade, tfalse);

  if (Left < (ldat)DWidth && Up < (ldat)DHeight && Rgt + DeltaXShade >= (ldat)0 &&
      Dwn + DeltaYShade > (ldat)YLimit) {
    if (i > 0) {
      Rgt1 = Min2(Rgt + DeltaXShade, Left + i - 1);
      Left1 = Max2(0, Left);
    } else {
      Left1 = Max2(Rgt + DeltaXShade + i, Left);
      Rgt1 = Min2((ldat)DWidth - (ldat)1, Rgt + DeltaXShade);
    }
    DrawArea2((screen)0, (Twidget)0, (Twidget)0, (dat)Left1, (dat)Up, (dat)Rgt1,
              (dat)Dwn + DeltaYShade, tfalse);

    if (j > 0) {
      Dwn = Min2(Dwn + DeltaYShade, Up + j - 1);
      Up = Max2(YLimit + 1, Up);
    } else {
      Up = Max2(Dwn + DeltaYShade + j, Up);
      Dwn = Min2((ldat)DHeight - (ldat)1, Dwn + DeltaYShade);
    }
    DrawArea2((screen)0, (Twidget)0, (Twidget)0, (dat)Left, (dat)Up, (dat)Rgt + DeltaXShade,
              (dat)Dwn, tfalse);
  }
  if (Window == (Twindow)All->FirstScreen->FocusW)
    UpdateCursor();
}

void ResizeRelFirstWindow(dat i, dat j) {
  ldat Left, Up, Rgt, Dwn;
  screen Screen;
  setup *SetUp;
  Twindow Window;
  dat DeltaX, DeltaY;
  dat YLimit, XWidth, YWidth;
  dat MinXWidth, MinYWidth, MaxXWidth, MaxYWidth;
  dat DWidth, DHeight;
  byte Shade, DeltaXShade, DeltaYShade, HasBorder;

  Screen = All->FirstScreen;
  if (!(Window = (Twindow)Screen->FirstW) || !IS_WINDOW(Window) ||
      (!i && !j)) /* || !(Window->Attr & WINDOW_RESIZE)) */
    return;

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  SetUp = All->SetUp;
  Shade = !!(SetUp->Flags & setup_shadows);
  DeltaXShade = Shade ? SetUp->DeltaXShade : (byte)0;
  DeltaYShade = Shade ? SetUp->DeltaYShade : (byte)0;
  HasBorder = !(Window->Flags & WINDOWFL_BORDERLESS);
  YLimit = Screen->YLimit + 1;

  XWidth = Window->XWidth;
  YWidth = Window->YWidth;
  MinXWidth = Window->MinXWidth;
  MinYWidth = Window->MinYWidth;
  MaxXWidth = Window->MaxXWidth;
  MaxYWidth = Window->MaxYWidth;

  Up = (ldat)Window->Up - Screen->YLogic + (ldat)YLimit - 1;
  Left = (ldat)Window->Left - Screen->XLogic;
  Rgt = Left + (ldat)XWidth - (ldat)1;
  Dwn = Up + (ldat)YWidth - (ldat)1;

  if ((DeltaX = -i) > (dat)0 && XWidth > MinXWidth) {
    if (MinXWidth + DeltaX > XWidth)
      DeltaX = XWidth - MinXWidth;
    XWidth = Window->XWidth -= DeltaX;
    if (Left < (ldat)DWidth && Up < (ldat)DHeight && Rgt + (ldat)DeltaXShade >= (ldat)0 &&
        Dwn + (ldat)DeltaYShade >= (ldat)YLimit) {

      DrawArea2((screen)0, (Twidget)Window, (Twidget)0, (dat)Rgt - DeltaX + 1,
                (dat)Max2(Up, (ldat)YLimit), (dat)Rgt, (dat)Max2((ldat)YLimit, Dwn), tfalse);
      if (Shade) {
        DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)Rgt + Max2((dat)DeltaXShade - DeltaX - 1, 1),
                  (dat)Max2((ldat)YLimit, Up), (dat)Rgt + (dat)DeltaXShade,
                  (dat)Dwn + (dat)DeltaYShade, tfalse);
        if (DeltaX > (dat)DeltaXShade)
          DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)Rgt + (dat)DeltaXShade - DeltaX + 1,
                    (dat)Max2((ldat)YLimit, Dwn + 1), (dat)Rgt, (dat)Dwn + (dat)DeltaYShade,
                    tfalse);
      }
    }
    Rgt -= DeltaX;
  } else if ((DeltaX = i) > (dat)0 && XWidth < MaxXWidth) {
    if (XWidth > MaxXWidth - DeltaX)
      DeltaX = MaxXWidth - XWidth;
    XWidth = Window->XWidth += DeltaX;
    if (Left < (ldat)DWidth && Up < (ldat)DHeight && Rgt + (ldat)DeltaXShade >= -(ldat)DeltaX &&
        Dwn + (ldat)DeltaYShade >= (ldat)YLimit) {

      DrawArea2((screen)0, (Twidget)Window, (Twidget)0, (dat)Rgt,
                (dat)Max2(Up + HasBorder, (ldat)YLimit), (dat)Rgt + DeltaX - HasBorder,
                (dat)Max2((ldat)YLimit, Dwn - HasBorder), tfalse);
      if (Shade) {
        DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)Rgt + 1 + Max2((dat)DeltaXShade, DeltaX),
                  (dat)Max2((ldat)YLimit, Up + (dat)DeltaYShade),
                  (dat)Rgt + DeltaX + (dat)DeltaXShade, (dat)Dwn + (dat)DeltaYShade, tfalse);
        if (DeltaX > (dat)DeltaXShade)
          DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)Rgt + (dat)DeltaXShade + 1,
                    (dat)Max2((ldat)YLimit, Dwn + 1), (dat)Rgt + DeltaX,
                    (dat)Dwn + (dat)DeltaYShade, tfalse);
      }
    }
    Rgt += DeltaX;
  }
  if ((DeltaY = -j) > (dat)0 && YWidth > MinYWidth) {
    if (MinYWidth + DeltaY > YWidth)
      DeltaY = YWidth - MinYWidth;
    YWidth = Window->YWidth -= DeltaY;
    if (Left < (ldat)DWidth && Up < (ldat)DHeight && Rgt + (ldat)DeltaXShade >= (ldat)0 &&
        Dwn + (ldat)DeltaYShade >= (ldat)YLimit) {

      DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)Left,
                (dat)Max2(Dwn - (ldat)DeltaY + (ldat)1, (ldat)YLimit), (dat)Rgt,
                (dat)Max2((ldat)YLimit, Dwn), tfalse);
      if (Shade) {
        DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)Left,
                  (dat)Max2((ldat)YLimit, Dwn + 1 + Max2((dat)DeltaYShade - DeltaY, (dat)0)),
                  (dat)Rgt + (dat)DeltaXShade, (dat)Dwn + (dat)DeltaYShade, tfalse);
        if (DeltaY > (dat)DeltaYShade)
          DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)Rgt + 1,
                    (dat)Max2((ldat)YLimit, Dwn + 1 + (dat)DeltaYShade - DeltaY),
                    (dat)Rgt + (dat)DeltaXShade, (dat)Max2((ldat)YLimit, Dwn), tfalse);
      }
    }
    Dwn -= DeltaY;
  } else if ((DeltaY = j) > (dat)0 && YWidth < MaxYWidth) {
    if (YWidth > MaxYWidth - DeltaY)
      DeltaY = MaxYWidth - YWidth;
    YWidth = Window->YWidth += DeltaY;
    if (Left < (ldat)DWidth && Up < (ldat)DHeight && Rgt + (ldat)DeltaXShade >= (ldat)0 &&
        Dwn + (ldat)DeltaYShade >= -(ldat)DeltaY + (ldat)YLimit) {

      DrawArea2((screen)0, (Twidget)Window, (Twidget)0, (dat)Left, (dat)Max2((ldat)YLimit, Dwn),
                (dat)Rgt - HasBorder, (dat)Max2((ldat)YLimit, Dwn + DeltaY - HasBorder), tfalse);
      if (Shade) {
        DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)Left + (dat)DeltaXShade,
                  (dat)Dwn + 1 + Max2(DeltaY, (dat)DeltaYShade), (dat)Rgt + (dat)DeltaXShade,
                  (dat)Dwn + (dat)DeltaYShade + DeltaY, tfalse);
        if (DeltaY > (dat)DeltaYShade)
          DrawArea2(Screen, (Twidget)0, (Twidget)0, (dat)Rgt + 1, (dat)Dwn + (dat)DeltaYShade + 1,
                    (dat)Rgt + (dat)DeltaXShade, (dat)Max2((ldat)YLimit, Dwn + DeltaY), tfalse);
      }
    }
    Dwn += DeltaY;
  }
  if (DeltaX || DeltaY) {
    DrawBorderWindow(Window, BORDER_ANY);
    if (Window == (Twindow)Screen->FocusW)
      UpdateCursor();

    /* resize contents? for Interactive Resize, let the WM resize it
     * when Interactive Resize finishes. otherwise, do it now */
    if (W_USE(Window, USECONTENTS) && Window->USE.C.Contents &&
        (Window != Screen->ClickWindow || (All->State & state_any) != state_resize))

      CheckResizeWindowContents(Window);
  }
}

void ResizeRelWindow(Twindow Window, dat i, dat j) {
  ldat Left, Up, Rgt, Dwn;
  Twidget Parent;
  setup *SetUp;
  dat DeltaX, DeltaY;
  dat YLimit, XWidth, YWidth;
  dat MinXWidth, MinYWidth, MaxXWidth, MaxYWidth;
  byte Shade, DeltaXShade, DeltaYShade, visible;

  if (!Window || (!i && !j)) /* || !(Window->Attr & WINDOW_RESIZE) */
    return;

  visible = !(Window->Flags & WIDGETFL_NOTVISIBLE);

  if (visible && (Twidget)Window == All->FirstScreen->FirstW) {
    ResizeRelFirstWindow(i, j);
    return;
  }

  XWidth = Window->XWidth;
  YWidth = Window->YWidth;
  MinXWidth = Window->MinXWidth;
  MinYWidth = Window->MinYWidth;
  MaxXWidth = Window->MaxXWidth;
  MaxYWidth = Window->MaxYWidth;

  if (visible && (Parent = Window->Parent) && IS_SCREEN(Parent)) {
    SetUp = All->SetUp;
    Shade = !!(SetUp->Flags & setup_shadows);
    DeltaXShade = Shade ? SetUp->DeltaXShade : (byte)0;
    DeltaYShade = Shade ? SetUp->DeltaYShade : (byte)0;
    YLimit = Parent->Up;

    Up = (ldat)Window->Up - Parent->YLogic + (ldat)YLimit;
    Left = (ldat)Window->Left - Parent->XLogic;
    Rgt = Left + (ldat)XWidth - (ldat)1 + DeltaXShade;
    Dwn = Up + (ldat)YWidth - (ldat)1 + DeltaYShade;
  }

  if ((DeltaX = -i) > (dat)0 && XWidth > MinXWidth) {
    if (MinXWidth + DeltaX > XWidth)
      DeltaX = XWidth - MinXWidth;
    XWidth = Window->XWidth -= DeltaX;
  } else if ((DeltaX = i) > (dat)0 && XWidth < MaxXWidth) {
    if (XWidth > MaxXWidth - DeltaX)
      DeltaX = MaxXWidth - XWidth;
    XWidth = Window->XWidth += DeltaX;
    Rgt += DeltaX;
  }
  if ((DeltaY = -j) > (dat)0 && YWidth > MinYWidth) {
    if (MinYWidth + DeltaY > YWidth)
      DeltaY = YWidth - MinYWidth;
    YWidth = Window->YWidth -= DeltaY;
  } else if ((DeltaY = j) > (dat)0 && YWidth < MaxYWidth) {
    if (YWidth > MaxYWidth - DeltaY)
      DeltaY = MaxYWidth - YWidth;
    YWidth = Window->YWidth += DeltaY;
    Dwn += DeltaY;
  }
  if (DeltaX || DeltaY) {
    if (visible && Parent && IS_SCREEN(Parent)) {
      Up = (dat)Max2(Up, (ldat)YLimit);
      DrawArea2((screen)Parent, (Twidget)0, (Twidget)0, (dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn,
                tfalse);
      if (Shade)
        DrawShadeWindow(Window, 0, 0, TW_MAXDAT, TW_MAXDAT, tfalse);
    }

    if (visible && Window == (Twindow)All->FirstScreen->FocusW)
      UpdateCursor();

    /* resize contents? for Interactive Resize, let the WM resize it
     * when Interactive Resize finishes. otherwise, do it now */
    if (W_USE(Window, USECONTENTS) && Window->USE.C.Contents &&
        (Window != All->FirstScreen->ClickWindow || (All->State & state_any) != state_resize))

      CheckResizeWindowContents(Window);
  }
}

/*
 * coordinates are special here:
 * (0,0) is the (XLogic,YLogic) cell of the Window,
 * i.e. the first visible one.
 */
void ScrollFirstWindowArea(dat X1, dat Y1, dat X2, dat Y2, ldat DeltaX, ldat DeltaY) {
  screen Screen;
  dat DWidth, DHeight;
  Twindow Window;
  ldat shLeft, shUp;
  dat Left, Up, Rgt, Dwn;
  dat Xstart, Ystart, Xend, Yend;
  dat XWidth, YWidth, YLimit;

  Screen = All->FirstScreen;
  Window = (Twindow)Screen->FirstW;

  if (!Window || !IS_WINDOW(Window) || (Window->Attr & WINDOW_ROLLED_UP))
    return;

  XWidth = Window->XWidth;
  YWidth = Window->YWidth;

  if (X1 > X2 || X1 > XWidth - 3 || X2 < 0 || Y1 > Y2 || Y1 > YWidth - 3 || Y2 < 0)
    return;

  if (DeltaX >= XWidth || -DeltaX >= XWidth || DeltaY >= YWidth || -DeltaY >= YWidth) {
    DrawWidget((Twidget)Window, (dat)0, (dat)0, TW_MAXDAT, TW_MAXDAT, tfalse);
    return;
  }

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  YLimit = Screen->YLimit;
  shUp = (ldat)Window->Up - Screen->YLogic + (ldat)YLimit;
  shLeft = (ldat)Window->Left - Screen->XLogic;

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
  if (Up <= (dat)YLimit)
    Y1 += (dat)YLimit + 1 - Up, Up = (dat)YLimit + 1;
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
      DrawWidget((Twidget)Window, Left, Up, Left + DeltaX - 1, Dwn, tfalse);
    else
      DrawWidget((Twidget)Window, Rgt + DeltaX + 1, Up, Rgt, Dwn, tfalse);
  } else if (DeltaY) {
    if (Yend >= Ystart)
      DragArea(Xstart, Ystart, Xend, Yend, Xstart, Ystart + DeltaY);
    if (DeltaY > (dat)0)
      DrawWidget((Twidget)Window, Left, Up, Rgt, Up + DeltaY - 1, tfalse);
    else
      DrawWidget((Twidget)Window, Left, Dwn + DeltaY + 1, Rgt, Dwn, tfalse);
  }
}

void ScrollFirstWindow(ldat DeltaX, ldat DeltaY, byte byXYLogic) {
  Twindow Window;
  ldat XLogic, YLogic;
  dat XWidth, YWidth;

  Window = (Twindow)All->FirstScreen->FirstW;
  if (!Window || !IS_WINDOW(Window))
    return;

  XWidth = Window->XWidth;
  YWidth = Window->YWidth;

  if (byXYLogic) {
    XLogic = Window->XLogic;
    YLogic = Window->YLogic;

    if (DeltaX > 0 && XLogic >= TW_MAXLDAT - DeltaX)
      DeltaX = TW_MAXLDAT - XLogic - 1;
    else if (DeltaX < 0 && XLogic < -DeltaX)
      DeltaX = -XLogic;
    if (!W_USE(Window, USEROWS)) {
      /*
       * WARNING: Window->USE.C.Contents and other methods
       * may be unable to handle out-of-bound rows
       */
      if (DeltaY > 0 && YLogic + YWidth - 2 + DeltaY >= Window->HLogic)
        DeltaY = Window->HLogic - YLogic - YWidth + 2;
      else if (DeltaY < 0 && YLogic < -DeltaY)
        DeltaY = -YLogic;
    } else {
      if (DeltaY > 0 && YLogic + DeltaY >= Window->HLogic)
        DeltaY = Window->HLogic - YLogic - 1;
      else if (DeltaY < 0 && YLogic < -DeltaY)
        DeltaY = -YLogic;
    }
    if (DeltaX)
      Window->XLogic = (XLogic += DeltaX);
    if (DeltaY)
      Window->YLogic = (YLogic += DeltaY);
    if (DeltaX || DeltaY)
      DrawBorderWindow(Window, (DeltaX ? BORDER_DOWN : 0) | (DeltaY ? BORDER_RIGHT : 0));
  }

  ScrollFirstWindowArea(0, 0, XWidth - 3, YWidth - 3, -DeltaX, -DeltaY);

  if (byXYLogic && ContainsCursor((Twidget)Window))
    UpdateCursor();
}

void ScrollWindow(Twindow Window, ldat DeltaX, ldat DeltaY) {
  ldat XLogic, YLogic;
  dat YWidth;

  if (!Window || !IS_WINDOW(Window) || (!DeltaX && !DeltaY))
    return;

  if (!(Window->Attr & WINDOW_X_BAR))
    DeltaX = 0;

  if (!(Window->Attr & WINDOW_Y_BAR))
    DeltaY = 0;

  if (!DeltaX && !DeltaY)
    return;

  if ((Twidget)Window == All->FirstScreen->FirstW) {
    ScrollFirstWindow(DeltaX, DeltaY, ttrue);
    return;
  }

  YWidth = Window->YWidth;
  XLogic = Window->XLogic;
  YLogic = Window->YLogic;

  if (DeltaX > 0 && XLogic >= TW_MAXLDAT - DeltaX)
    DeltaX = TW_MAXLDAT - XLogic - 1;
  else if (DeltaX < 0 && XLogic < -DeltaX)
    DeltaX = -XLogic;
  if (!W_USE(Window, USEROWS)) {
    /*
     * WARNING: Window->USE.C.Contents and other methods
     * may be unable to handle out-of-bound rows
     */
    if (DeltaY > 0 && YLogic + YWidth - 2 + DeltaY >= Window->HLogic)
      DeltaY = Window->HLogic - YLogic - YWidth + 2;
    else if (DeltaY < 0 && YLogic < -DeltaY)
      DeltaY = -YLogic;
  } else {
    if (DeltaY > 0 && YLogic + DeltaY >= Window->HLogic)
      DeltaY = Window->HLogic - YLogic - 1;
    else if (DeltaY < 0 && YLogic < -DeltaY)
      DeltaY = -YLogic;
  }
  if (DeltaX)
    Window->XLogic = (XLogic += DeltaX);
  if (DeltaY)
    Window->YLogic = (YLogic += DeltaY);

  DrawFullWindow2(Window);

  if (ContainsCursor((Twidget)Window))
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
  screen Screen;
  dat DWidth, DHeight;
  Twindow Window;
  uldat Attr, State, Scroll;
  dat XWidth, YWidth;
  dat DeltaX, DeltaY;

  if ((All->State & state_any) != state_scroll)
    return tfalse;

  if (!(Screen = All->FirstScreen) || !(Window = (Twindow)Screen->FocusW) || !IS_WINDOW(Window))
    return tfalse;

  Attr = Window->Attr;
  State = Window->State;
  DeltaX = DeltaY = (sbyte)0;

  if (Attr & WINDOW_X_BAR && State & X_BAR_SELECT)
    DeltaX = 1;
  else if (Attr & WINDOW_Y_BAR && State & Y_BAR_SELECT)
    DeltaY = 1;
  else
    return tfalse;

  DWidth = All->DisplayWidth;
  DHeight = All->DisplayHeight;
  XWidth = Window->XWidth;
  YWidth = Window->YWidth;

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

  if ((Twidget)Window == Screen->FirstW)
    ScrollFirstWindow(DeltaX, DeltaY, ttrue);
  else
    ScrollWindow(Window, DeltaX, DeltaY);
  return ttrue;
}

/* Tmenu traversing functions */

void HideMenu(byte on_off) {
  screen Screen = All->FirstScreen;

  if (on_off) {
    if (Screen->YLimit == 0) {
      if (Screen->YLogic > TW_MINDAT) {
        Screen->YLogic--;
        Screen->YLimit--;
        DrawArea2(Screen, (Twidget)0, (Twidget)0, 0, 0, TW_MAXDAT, 0, tfalse);
        UpdateCursor();
      } else
        ResizeFirstScreen(-1);
    }
  } else {
    if (Screen->YLimit == -1) {
      if (Screen->YLogic < TW_MAXDAT) {
        Screen->YLogic++;
        Screen->YLimit++;
        Act(DrawMenu, Screen)(Screen, 0, TW_MAXDAT);
        UpdateCursor();
      } else
        ResizeFirstScreen(1);
    }
  }
}

static void OpenSubMenuItem(Tmenu M, Tmenuitem Item, byte ByMouse) {
  Twindow P = (Twindow)Item->Parent;
  Twindow w = Item->Window;
  screen S = All->FirstScreen;
  ldat y = P->CurY;

  P->CurY = Item->WCurY;
  if (y != TW_MAXLDAT)
    DrawLogicWidget((Twidget)P, 0, y, TW_MAXLDAT, y);
  if ((y = P->CurY) == TW_MAXLDAT)
    (void)Act(FindRowByCode, P)(P, Item->Code, &P->CurY);
  if ((y = P->CurY) != TW_MAXLDAT)
    DrawLogicWidget((Twidget)P, 0, y, TW_MAXLDAT, y);

  if (w) {
    if (!w->Parent) {
      w->Left = P->Left + P->XWidth - S->XLogic;
      w->Up = P->Up + P->CurY - P->YLogic - S->YLogic + 1;
    }
    if (ByMouse)
      w->CurY = TW_MAXLDAT;
    else if (w->CurY == TW_MAXLDAT)
      w->CurY = 0;
    Act(MapTopReal, w)(w, S);
  }
  if ((Twidget)P != S->FocusW)
    Act(Focus, P)(P);
}

static void OpenTopMenuItem(Tmenu M, Tmenuitem Item, byte ByMouse) {
  Tmenu _M = (Tmenu)Item->Parent; /* may either be M or All->CommonMenu */
  Twindow w = Item->Window;

  if (!w->Parent) {
    w->Up = 1;
    w->Left = Item->Left;

    if (M != _M && M->LastI)
      /* adjust common Tmenu w->Left to the Item position in this Tmenu */
      w->Left += M->LastI->Left + M->LastI->Len;
  }

  Act(SetSelectedItem, M)(M, Item);

  if (ByMouse)
    w->CurY = TW_MAXLDAT;
  else if (w->CurY == TW_MAXLDAT)
    w->CurY = (ldat)0;

  if (Item->Flags & ROW_ACTIVE)
    w->Flags &= ~WINDOWFL_DISABLED;
  else
    w->Flags |= WINDOWFL_DISABLED;

  Act(MapTopReal, w)(w, All->FirstScreen);

  if (w->CurY != TW_MAXLDAT && (Item = (Tmenuitem)Act(FindRow, w)(w, w->CurY)))
    OpenSubMenuItem(M, Item, ByMouse);
}

static void OpenMenuItem(Tmenu M, Tmenuitem Item, byte ByMouse) {
  if (Item) {
    Tobj O = Item->Parent;
    if (O && IS_WINDOW(O))
      OpenSubMenuItem(M, Item, ByMouse);
    else
      OpenTopMenuItem(M, Item, ByMouse);
  } else
    Act(SetSelectedItem, M)(M, (Tmenuitem)0);
}

/* this activates the Tmenu bar */
static void OpenMenu(Tmenuitem Item, byte ByMouse) {
  screen S = All->FirstScreen;
  Twidget w = S->FocusW;
  Tmenu M = Act(FindMenu, S)(S);

  if ((All->State & state_any) == state_default) {

    All->State = state_menu | (ByMouse ? state_fl_bymouse : 0);

    if (All->SetUp->Flags & setup_menu_hide)
      HideMenu(tfalse);

    if (!S->MenuWindow && w) {
      S->MenuWindow = (Twindow)w; /* so that it keeps `active' borders */
      S->FocusW = (Twidget)0;
    }
  }
  OpenMenuItem(M, Item, ByMouse);
}

/*
 * up one level; return new selected item;
 * do NOT use to close the Tmenu, CloseMenu() does that
 */
static Tmenuitem CloseMenuItem(Tmenu M, Tmenuitem Item, byte ByMouse) {
  Twindow P = (Twindow)Item->Parent, w = Item->Window;

  if (w)
    Act(UnMap, w)(w);

  if (P && IS_WINDOW(P)) {
    if (ByMouse) {
      ldat y = P->CurY;
      P->CurY = TW_MAXLDAT;

      if (y != TW_MAXLDAT)
        DrawLogicWidget((Twidget)P, 0, y, TW_MAXLDAT, y);
    }
    Item = P->MenuItem;
    if (Item) {
      w = (Twindow)Item->Parent;
      if (w && IS_WINDOW(w))
        w->Focus();
      else
        P->Focus();
    }
    return Item;
  } else {
    Item = (Tmenuitem)0;
    Act(SetSelectedItem, M)(M, Item);
    return Item;
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
                         byte ByMouse) {
  while (Odepth > Ndepth && OldItem) {
    Odepth--;
    OldItem = CloseMenuItem(M, OldItem, ByMouse);
  }
  /* paranoia */
  Odepth = DepthOfMenuItem(OldItem);

  if (Ndepth == Odepth) {
    if (OldItem != NewItem)
      CloseMenuItem(M, OldItem, ByMouse);
    OpenMenuItem(M, NewItem, ByMouse);
  } else if (Ndepth == Odepth + 1) {
    OpenMenuItem(M, NewItem, ByMouse);
  } else
    log(ERROR) << "twin: internal error: unsupported Tmenu traversing.\n";
}

/* close the Tmenu bar */
void CloseMenu(void) {
  screen S = All->FirstScreen;
  Tmenu M = Act(FindMenu, S)(S);
  Tmenuitem Item;
  Twindow w;

  if (M) {
    if ((w = S->MenuWindow)) {
      Act(KbdFocus, w)(w);
      S->MenuWindow = NULL;
    } else
      Do(KbdFocus, window)(NULL);

    /* close whole currently open Tmenu tree */
    Item = Act(GetSelectedItem, M)(M);
    while (Item && IS_MENUITEM(Item) && (w = (Twindow)Item->Window) && IS_WINDOW(w)) {
      Item = (Tmenuitem)Act(FindRow, w)(w, w->CurY);
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
void SetMenuState(Tmenuitem Item, byte ByMouse) {
  screen S = All->FirstScreen;
  Tmenu M = Act(FindMenu, S)(S);
  Tmenuitem OldItem = (Tmenuitem)0;
  dat Odepth = 0;

  if (M && (Item || ByMouse)) {
    if ((All->State & state_any) != state_default)
      OldItem = Act(RecursiveGetSelectedItem, M)(M, &Odepth);
    if (!OldItem)
      OpenMenu(Item, ByMouse);
    else if (OldItem != Item)
      TraverseMenu(M, OldItem, Odepth, Item, DepthOfMenuItem(Item), ByMouse);
    UpdateCursor();
  }
}

/* ---------------- */

void UnFocusWidget(Twidget w) {
  if (w && w->Parent == (Twidget)All->FirstScreen && w == All->FirstScreen->FocusW) {
    if (IS_WINDOW(w)) {
      Act(KbdFocus, w)((Twidget)0);
      DrawBorderWindow((Twindow)w, BORDER_ANY);
      Act(DrawMenu, (screen)w->Parent)((screen)w->Parent, 0, TW_MAXDAT);
      UpdateCursor();
    } else
      All->FirstScreen->FocusW = (Twidget)0;
  }
}

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

void RaiseWidget(Twidget w, byte alsoFocus) {
  screen Screen;

  if (w && (Screen = (screen)w->Parent) && IS_SCREEN(Screen)) {

    if (Screen->FirstW != w) {
      MoveFirst(W, (Twidget)Screen, w);
      if (IS_WINDOW(w))
        DrawAreaWindow2((Twindow)w);
      else
        DrawAreaWidget(w);
    }
    if (Screen == All->FirstScreen) {
      if (alsoFocus)
        Act(Focus, w)(w);
      UpdateCursor();
    }

    if (Screen->FnHookW)
      Screen->FnHookW(Screen->HookW);
  }
}

void LowerWidget(Twidget w, byte alsoUnFocus) {
  screen Screen;
  Twidget _W;

  if (w && (Screen = (screen)w->Parent) && IS_SCREEN(Screen)) {

    if (Screen->LastW != w) {
      MoveLast(W, (Twidget)Screen, w);
      if (IS_WINDOW(w))
        DrawAreaWindow2((Twindow)w);
      else
        DrawAreaWidget(w);
    }
    if (Screen == All->FirstScreen) {
      if (alsoUnFocus) {
        _W = Screen->FirstW;
        if (_W && IS_WINDOW(_W) && _W != w)
          _W->Focus();
        else
          Do(Focus, window)(NULL);
      } else
        UpdateCursor();
    }

    if (Screen->FnHookW)
      Screen->FnHookW(Screen->HookW);
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
      if ((CR = *arrayR) && (Tobj)CR->Window == O) {
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
void WriteTextsGadget(Tgadget G, byte bitmap, dat TW, dat TH, const char *charset_bytes, dat L,
                      dat U) {
  dat GW = G->XWidth, GH = G->YWidth;
  dat TL = 0, TU = 0, w, H;
  dat _W;
  trune *GT;
  const char *TT;
  byte i;

  if (G->Flags & GADGETFL_BUTTON) {
    GW--;
    GH--;
  }

  if (!G_USE(G, USETEXT) || L >= GW || U >= GH)
    return;

  if (L < 0) {
    L += GW - TW + 1;
    if (L < 0) {
      TL = -L;
      L = 0;
    }
  }
  if (U < 0) {
    U += GH - TH + 1;
    if (U < 0) {
      TU = -U;
      U = 0;
    }
  }
  w = Min2(TW - TL, GW - L);
  H = Min2(TH - TU, GH - U);

  if (G->Flags & GADGETFL_BUTTON) {
    GW++;
    GH++;
  }

  if (w > 0) {
    for (i = 0; i < 4; i++, bitmap >>= 1) {
      if ((bitmap & 1) && G->USE.T.Text[i]) {
        GT = G->USE.T.Text[i] + L + U * GW;
        if (charset_bytes) {
          TT = charset_bytes + TL + TU * TW;
          /* update the specified part, do not touch the rest */
          while (H-- > 0) {
            _W = w;
            while (_W-- > 0) {
              *GT++ = Tutf_CP437_to_UTF_32[(byte)(*TT++)];
            }
            GT += GW - w;
            TT += TW - w;
          }
        } else {
          /* clear the specified part of Tgadget contents */
          while (H-- > 0) {
            _W = w;
            while (_W-- > 0)
              *GT++ = ' ';
            GT += GW - w;
          }
        }
      }
    }
    DrawAreaWidget((Twidget)G);
  }
}

/* Left < 0 means right-align leaving (-Left-1) margin */
/* Up   < 0 means down-align  leaving (-Up-1)   margin */
void WriteTRunesGadget(Tgadget G, byte bitmap, dat TW, dat TH, const trune *TRune, dat L, dat U) {
  dat GW = G->XWidth, GH = G->YWidth;
  dat TL = 0, TU = 0, w, H;
  dat _W;
  trune *GT;
  const trune *TT;
  byte i;

  if (G->Flags & GADGETFL_BUTTON) {
    GW--;
    GH--;
  }

  if (!G_USE(G, USETEXT) || L >= GW || U >= GH)
    return;

  if (L < 0) {
    L += GW - TW + 1;
    if (L < 0) {
      TL = -L;
      L = 0;
    }
  }
  if (U < 0) {
    U += GH - TH + 1;
    if (U < 0) {
      TU = -U;
      U = 0;
    }
  }
  w = Min2(TW - TL, GW - L);
  H = Min2(TH - TU, GH - U);

  if (G->Flags & GADGETFL_BUTTON) {
    GW++;
    GH++;
  }

  if (w > 0) {
    for (i = 0; i < 4; i++, bitmap >>= 1) {
      if ((bitmap & 1) && G->USE.T.Text[i]) {
        GT = G->USE.T.Text[i] + L + U * GW;
        if (TRune) {
          TT = TRune + TL + TU * TW;
          /* update the specified part, do not touch the rest */
          while (H-- > 0) {
            _W = w;
            while (_W-- > 0) {
              *GT++ = Tutf_CP437_to_UTF_32[*TT++];
            }
            GT += GW - w;
            TT += TW - w;
          }
        } else {
          /* clear the specified part of Tgadget contents */
          while (H-- > 0) {
            _W = w;
            while (_W-- > 0)
              *GT++ = ' ';
            GT += GW - w;
          }
        }
      }
    }
    DrawAreaWidget((Twidget)G);
  }
}

void SyncMenu(Tmenu Menu) {
  Tmenuitem I, PrevI = (Tmenuitem)0;
  screen Screen;

  if (Menu) {
    for (I = Menu->FirstI; I; I = I->Next) {
      if (PrevI)
        I->Left = PrevI->Left + PrevI->Len;
      else
        I->Left = 1;
      PrevI = I;
    }
    for (Screen = All->FirstScreen; Screen; Screen = Screen->Next) {
      if (Act(FindMenu, Screen)(Screen) == Menu)
        Act(DrawMenu, Screen)(Screen, 0, TW_MAXDAT);
    }
  }
}
