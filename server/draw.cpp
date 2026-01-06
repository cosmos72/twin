/*
 *  draw.cpp  --  functions to draw screens, menus, windows, gadgets, etc.
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "alloc.h"
#include "algo.h"
#include "main.h"
#include "data.h"
#include "log.h"
#include "methods.h"
#include "hw.h"
#include "hw_multi.h"
#include "util.h"
#include "draw.h"

#include <Tutf/Tutf.h>
#include <Tutf/Tutf_defs.h>

inline tcolor DoShadowColor(tcolor color, bool Fg, bool Bg) NOTHROW {
  return (Bg   ? (color & TCOL(0, tWHITE)) > TCOL(0, tBLACK) ? TCOL(0, tBLACK) : TCOL(0, tblack)
            : Fg ? color & TCOL(0, twhite)
               : color & TCOL(0, tWHITE)) |
         (Fg ? (color & TCOL(tWHITE, 0)) > TCOL(tBLACK, 0) ? TCOL(twhite, 0) : TCOL(tBLACK, 0)
             : color & TCOL(tWHITE, 0));
}

/*
 * warning: DrawMenu() can cheat and give us a user Menu
 * while MenuItem is from All->CommonMenu
 */
static void FindFontMenuItem(Tmenu Menu, Tmenuitem MenuItem, dat i, bool select,
                             tcell *PtrAttr) NOTHROW {
  tcolor color;
  byte ShortCutFound;

  if (Menu && MenuItem && i >= MenuItem->Left && i < (ldat)(MenuItem->Left + MenuItem->Len)) {
    ShortCutFound = i == MenuItem->Left + MenuItem->ShortCut;
    if (MenuItem->Flags & ROW_ACTIVE) {
      if (ShortCutFound) {
        if (select)
          color = Menu->ColSelShtCut;
        else
          color = Menu->ColShtCut;
      } else if (select)
        color = Menu->ColSelect;
      else
        color = Menu->ColItem;
    } else if (select)
      color = Menu->ColSelectDisabled;
    else
      color = Menu->ColDisabled;

    *PtrAttr = TCELL(color, MenuItem->Text[i - MenuItem->Left]);
  }
}

static void FindFontInfo(Tmenu Menu, dat i, bool select, tcell *PtrAttr) NOTHROW {
  Trow Info;
  tcolor color;

  if (Menu && (Info = Menu->Info) && i >= 0 && (uldat)i < Info->Len) {
    if (select)
      color = Info->Flags & ROW_ACTIVE ? Menu->ColSelect : Menu->ColSelectDisabled;
    else if (!(Info->Flags & ROW_ACTIVE))
      color = Menu->ColDisabled;
    else if (Menu->FlagDefColInfo || !Info->ColText)
      color = Menu->ColItem;
    else
      color = Info->ColText[i];

    *PtrAttr = TCELL(color, Info->Text[i]);
  }
}

void DrawDesktop(Tscreen screen, dat X1, dat Y1, dat X2, dat Y2, bool shaded) {
  tcell *p_attr, attr;
  tcolor col;
  ldat dwidth, dheight, BgWidth, BgHeight; /* (ldat) to avoid multiplication overflows */
  dat ylimit = -1;

  if (QueuedDrawArea2FullScreen || X1 > X2 || Y1 > Y2 || X2 < 0 || Y2 < 0)
    return;

  dwidth = All->DisplayWidth;
  dheight = All->DisplayHeight;

  if (screen)
    ylimit = screen->Up;

  if (X1 >= dwidth || Y1 >= dheight || Y2 < ylimit)
    return;

  X1 = Max2(X1, 0);
  Y1 = Max2(Y1, ylimit + 1);
  X2 = Min2(X2, dwidth - 1);
  Y2 = Min2(Y2, dheight - 1);

  if (screen && S_USE(screen, USEBG)) {
    p_attr = screen->USE.B.Bg;
    BgWidth = screen->USE.B.BgWidth;
    BgHeight = screen->USE.B.BgHeight;
  }

  if (screen && S_USE(screen, USEBG) && (BgWidth > 1 || BgHeight > 1)) {
    /* use tiling... easier than other solutions */
    ldat X, Y, x, max = BgWidth * BgHeight;
    ldat y = ((ldat)Y1 + screen->YLogic - screen->Up) % BgHeight;
    if (y < 0)
      y += BgHeight;
    y *= BgWidth;

    DirtyVideo(X1, Y1, X2, Y2);

    for (; Y1 <= Y2; Y1++, y += BgWidth) {
      Y = Y1 * (ldat)dwidth;

      if (y >= max)
        y -= max;
      x = ((ldat)X1 + screen->XLogic) % BgWidth;
      if (x < 0)
        x += BgWidth;

      if (shaded) {
        for (X = X1; X <= X2; X++, x++) {
          if (x >= BgWidth)
            x -= BgWidth;
          attr = p_attr[x + y];
          col = DoShadowColor(TCOLOR(attr), shaded, shaded);

          Video[X + Y] = TCELL(col, TRUNE(attr));
        }
      } else {
        for (X = X1; X <= X2; X++, x++) {
          if (x >= BgWidth)
            x -= BgWidth;

          Video[X + Y] = p_attr[x + y];
        }
      }
    }
  } else {
    if (screen) {
      if (S_USE(screen, USEBG))
        attr = *screen->USE.B.Bg;
      else
        attr = screen->USE_Fill;
    } else
      attr = TCELL(TCOL(twhite, tblack), ' ');

    if (shaded) {
      col = DoShadowColor(TCOLOR(attr), shaded, shaded);
      attr = TCELL(col, TRUNE(attr));
    }
    FillVideo(X1, Y1, X2, Y2, attr);
  }
}

/*
 * initialize a Sdraw given Twidget-relative coords
 * (0,0 is the Twidget top-left corner)
 * return false if given Twindow area is not visible
 */
bool Sdraw::Init(Twidget w, dat x1, dat y1, dat x2, dat y2, bool shaded) {
  Sdraw *d = this;
  ldat xl, yl, height;
  uldat cycle = 0;
  Twidget c = NULL, cc = NULL;
  byte HasTopBar, HasBorder;

  if (!w || !d) {
    return false;
  }
  d->Next = NULL;
  d->W = w;
  d->NoChildren = d->BorderDone = false;
  d->Shaded = shaded;

  d->Left = 0;
  d->Rgt = w->XWidth - 1;
  d->X1 = x1;
  d->X2 = x2;

  d->Up = 0;
  if (IS_WINDOW(w) && (((Twindow)w)->Attr & WINDOW_ROLLED_UP))
    d->Dwn = 0;
  else
    d->Dwn = w->YWidth - 1;
  d->Y1 = y1;
  d->Y2 = y2;

  while (w) {
    if (w->Flags & WIDGETFL_NOTVISIBLE) {
      return false;
    }
    HasTopBar = HasBorder = 0;

    if (!IS_WINDOW(w) || !(((Twindow)w)->Attr & WINDOW_ROLLED_UP)) {
      height = w->YWidth;
    } else {
      height = 1;
    }
    if (cycle++) {
      xl = w->XLogic;
      yl = w->YLogic;

      if (IS_SCREEN(w))
        HasTopBar = 1;
      else if (IS_WINDOW(w) && !(((Twindow)w)->Flags & WINDOWFL_BORDERLESS))
        /* count border thickness */
        HasBorder = 1;
    } else {
      xl = yl = 0;
    }
    d->Left += (ldat)w->Left - xl + HasBorder;
    d->Rgt += (ldat)w->Left - xl + HasBorder;
    d->X1 = Max2(d->X1 - xl, 0) + HasBorder + w->Left;
    d->X2 = Min2(d->X2 - xl + HasBorder, w->XWidth - 1 - HasBorder) + w->Left;

    d->Up += (ldat)w->Up - yl + HasBorder;
    d->Dwn += (ldat)w->Up - yl + HasBorder;
    d->Y1 = Max2(d->Y1 - yl, HasTopBar) + HasBorder + w->Up;
    d->Y2 = Min2(d->Y2 - yl + HasBorder, height - 1 - HasBorder) + w->Up;

    cc = c;
    c = w;
    w = w->Parent;
  }

  d->DWidth = All->DisplayWidth;
  d->DHeight = All->DisplayHeight;

  d->X1 = Max2(d->X1, 0);
  d->X2 = Min2(d->X2, d->DWidth - 1);
  d->Y1 = Max2(d->Y1, 0);
  d->Y2 = Min2(d->Y2, d->DHeight - 1);

  d->TopW = cc;
  if (d->W == cc) {
    d->W = NULL;
  }
  if (c && IS_SCREEN(c)) {
    d->Screen = (Tscreen)c;
    return d->X1 <= d->X2 && d->Y1 <= d->Y2;
  }
  d->Screen = NULL;
  return false;
}

/*
 * same as s_draw_cts::Init(), but now x1,y1,x2,y2 are
 * absolute display coords. (0,0 is the display top-left corner)
 * return false if given area is not visible
 */
bool Sdraw::InitAbsolute(Twidget w, dat x1, dat y1, dat x2, dat y2, bool shaded) {
  Sdraw *d = this;
  if (d->Init(w, 0, 0, TW_MAXDAT, TW_MAXDAT, shaded)) {

    d->X1 = Max2(d->X1, x1);
    d->X2 = Min2(d->X2, x2);

    d->Y1 = Max2(d->Y1, y1);
    d->Y2 = Min2(d->Y2, y2);

    return d->X1 <= d->X2 && d->Y1 <= d->Y2;
  }
  return false;
}

/*
 * translate X,Y coords from `relative to W1' to `relative to W2'.
 * If W1 is NULL X,Y coords are taken as `absolute'.
 * If W2 is NULL X,Y coords are translated to `absolute'.
 * *Inside will be == tfalse if X,Y is outside W2,
 * == ttrue if inside, == ttrue+ttrue if on the border.
 */
void TranslateCoordsWidget(Twidget W1, Twidget W2, dat *X, dat *Y, byte *Inside) {
  Sdraw d;
  if (W1 || W2) {
    if (W1) {
      d.Init(W1, 0, 0, TW_MAXDAT, TW_MAXDAT, false);
      if (IS_WINDOW(W1) && !(((Twindow)W1)->Flags & WINDOWFL_BORDERLESS)) {
        (*X)++, (*Y)++;
      }
      *X += d.Left;
      *Y += d.Up;
    }
    if (W2) {
      d.Init(W2, 0, 0, TW_MAXDAT, TW_MAXDAT, false);
      if (Inside) {
        if (*X >= d.Left && *X <= d.Rgt && *Y >= d.Up && *Y <= d.Dwn) {
          *Inside = ttrue;

          if (IS_WINDOW(W2) && !(((Twindow)W2)->Flags & WINDOWFL_BORDERLESS) &&
              (*X == d.Left || *X == d.Rgt || *Y == d.Up || *Y == d.Dwn))

            *Inside += ttrue;
        } else
          *Inside = tfalse;
      }
      if (IS_WINDOW(W2) && !(((Twindow)W2)->Flags & WINDOWFL_BORDERLESS)) {
        (*X)--, (*Y)--;
      }
      *X -= d.Left;
      *Y -= d.Up;
    }
  } else
    *Inside = tfalse;
}

/**
 * Find the Twidget at given coordinates inside this Twidget.
 * (0,0) is this Twidget's top-left corner
 */
Twidget Swidget::FindWidgetAt(dat x, dat y) {
  Twidget w;
  ldat i, j;
  dat height;

  if (IS_WINDOW(this) && !(((Twindow)this)->Flags & WINDOWFL_BORDERLESS))
    x--, y--;
  else if (IS_SCREEN(this) && y <= 0) {
    /* got nothing, or the Tmenu... */
    return (Twidget)0;
  }

  for (w = Widgets.First; w; w = w->Next) {

    if (w->Flags & WIDGETFL_NOTVISIBLE)
      continue;

    i = x + XLogic;
    j = y + YLogic;

    if (!IS_WINDOW(w) || !(((Twindow)w)->Attr & WINDOW_ROLLED_UP))
      height = w->YWidth;
    else
      height = 1;

    if ((ldat)w->Left <= i && (ldat)w->Left + w->XWidth > i && (ldat)w->Up <= j &&
        (ldat)w->Up + height > j)
      return w;
  }
  return (Twidget)0;
}

Twidget RecursiveFindWidgetAt(Twidget parent, dat x, dat y) {
  Twidget w;
  byte HasBorder;

  while (parent) {
    HasBorder = IS_WINDOW(parent) && !(((Twindow)parent)->Flags & WINDOWFL_BORDERLESS);

    if (x >= HasBorder && y >= HasBorder && x < parent->XWidth - HasBorder &&
        y < parent->YWidth - HasBorder && (w = parent->FindWidgetAt(x, y))) {

      x += parent->XLogic - w->Left - HasBorder;
      y += parent->YLogic - w->Up - HasBorder;
      parent = w;
    } else
      break;
  }
  return parent;
}

void Swidget::DrawSelf(Sdraw *d) {
  Twidget w = d->TopW;

  if (!w || QueuedDrawArea2FullScreen || (w->Flags & WIDGETFL_NOTVISIBLE))
    return;

  if (!w_USE(w, USEEXPOSE)) {
    FillVideo(d->X1, d->Y1, d->X2, d->Y2, w->USE_Fill);
    return;
  }

  const char *text = NULL;
  const trune *runes = NULL;
  const tcell *cells = NULL;

  switch (w->USE.E.Flags) {
  case WIDGET_USEEXPOSE_TEXT:
    text = w->USE.E.E.Text;
    break;
  case WIDGET_USEEXPOSE_TRUNE:
    runes = w->USE.E.E.TRune;
    break;
  case WIDGET_USEEXPOSE_TCELL:
    cells = w->USE.E.E.TCell;
    break;
  default:
    break;
  }
  ldat left = d->Left, up = d->Up;
  ldat x1 = d->X1, y1 = d->Y1, x2 = d->X2, y2 = d->Y2;
  ldat dwidth = d->DWidth; // (ldat) to avoid multiplication overflows
  bool shaded = d->Shaded;

#if 0
  /*
    * this would suppress EXPOSE messages during resize (good)
    * but also clears the whole Twidget during non-top resize (bad)
    */
  if ((All->State & state_any) == state_resize && (Twidget)All->Screens.First->ClickWindow == w) {
      /* user is interactively resizing this Twindow... pad with spaces */
      FillVideo(x1, y1, x2, y2, w->USE_Fill);
      return;
  }
#endif

  if (!text && !runes && !cells) {
    /* ask the client to draw */
    Tmsg msg;
    if ((msg = Smsg::Create(msg_widget_change, 0))) {
      event_widget *event = &msg->Event.EventWidget;
      event->W = w;
      event->Code = MSG_WIDGET_EXPOSE;
      event->Flags = shaded ? MSG_WIDGETFL_SHADED : 0;
      event->XWidth = x2 - x1 + 1;
      event->YWidth = y2 - y1 + 1;
      event->X = x1 - left;
      event->Y = y1 - up;
      SendMsg(w->Owner, msg);
    }
    return;
  }

  // clipped region
  ldat x1c = Max2(w->USE.E.X1 + left, x1);
  ldat x2c = Min2(w->USE.E.X2 + left, x2);
  ldat y1c = Max2(w->USE.E.Y1 + up, y1);
  ldat y2c = Min2(w->USE.E.Y2 + up, y2);

  ldat pitch = w->USE.E.Pitch;

  tcell h = w->USE_Fill;
  if (x1c > x2c || y1c > y2c) {
    /* no valid ->USE.E, fill with spaces */
    FillVideo(x1, y1, x2, y2, h);
    return;
  }

  DirtyVideo(x1, y1, x2, y2);

  /*
   * check if the ->USE.E is smaller than the Twidget size...
   * pad with SPACEs as needed
   */
  if (y1 < y1c) {
    FillVideo(x1, y1, x2, y1c - 1, h);
    y1 = y1c;
  }
  if (y2 > y2c) {
    FillVideo(x1, y2c + 1, x2, y2, h);
    y2 = y2c;
  }
  if (x1 < x1c) {
    FillVideo(x1, y1, x1c - 1, y2, h);
    x1 = x1c;
  }
  if (x2 > x2c) {
    FillVideo(x2c + 1, y1, x2, y2, h);
    x2 = x2c;
  }

  ldat dx = x1c - w->USE.E.X1 - left;
  ldat dy = y1c - w->USE.E.Y1 - up;

  if (text) {
    tcolor color = TCOLOR(w->USE_Fill);
    if (shaded)
      color = DoShadowColor(color, shaded, shaded);
    text += dy * pitch;
    for (ldat j = y1; j <= y2; j++) {
      text += dx;
      for (ldat i = x1, v = 0; i <= x2; i++, v++)
        Video[i + j * (ldat)dwidth] = TCELL(color, text[v]);
      text += pitch - dx;
    }
  } else if (runes) {
    tcolor color = TCOLOR(w->USE_Fill);
    if (shaded)
      color = DoShadowColor(color, shaded, shaded);
    runes += dy * pitch;
    for (ldat j = y1; j <= y2; j++) {
      runes += dx;
      for (ldat i = x1, v = 0; i <= x2; i++, v++)
        Video[i + j * (ldat)dwidth] = TCELL(color, runes[v]);
      runes += pitch - dx;
    }
  } else if (cells && !shaded) {
    cells += dx + dy * pitch;
    for (ldat j = y1; j <= y2; j++) {
      CopyMem(cells, &Video[x1 + j * (ldat)dwidth], (uldat)sizeof(tcell) * (x2 - x1 + 1));
      cells += pitch;
    }
  } else if (cells) {
    /* shaded == true */
    cells += dy * pitch;
    for (ldat j = y1; j <= y2; j++) {
      cells += dx;
      for (ldat i = x1, v = 0; i <= x2; i++, v++) {
        tcolor color = DoShadowColor(TCOLOR(cells[v]), shaded, shaded);
        Video[i + j * (ldat)dwidth] = TCELL(color, 0) | TCELL_RUNEMASK(cells[v]);
      }
      cells += pitch - dx;
    }
  }
}

void Sgadget::DrawSelf(Sdraw *d) {

  if (!d->TopW || !IS_GADGET(d->TopW) || QueuedDrawArea2FullScreen) {
    return;
  }
  Tgadget g = (Tgadget)d->TopW;
  if (g->Flags & WIDGETFL_NOTVISIBLE) {
    return;
  } else if (!G_USE(g, USETEXT)) {
    Swidget::DrawSelf(d);
    return;
  }

  ldat width, dwidth, offset; /* (ldat) to avoid multiplication overflows */
  dat i, i_min, i_max, j, j_min, j_max;
  bool select, disabled, absent;
  trune font, *text, **gadgetText;
  tcolor *colText, **gadgetColor;
  tcolor color = 0;

  select = !!(g->Flags & GADGETFL_PRESSED);
  disabled = !!(g->Flags & GADGETFL_DISABLED);
  absent = !!(g->Flags & GADGETFL_TEXT_DEFCOL);

  i_min = d->X1 - d->Left;
  i_max = d->X2 - d->Left;

  j_min = d->Y1 - d->Up;
  j_max = d->Y2 - d->Up;

  width = g->XWidth;
  dwidth = d->DWidth;

  offset = d->Left + d->Up * (ldat)dwidth;

  gadgetText = g->USE.T.Text;
  gadgetColor = g->USE.T.Color;

  text = select ? (disabled && gadgetText[3] ? gadgetText[3]
                                             : (gadgetText[1] ? gadgetText[1] : gadgetText[0]))
                : (disabled && gadgetText[2] ? gadgetText[2] : gadgetText[0]);

  if (!text)
    font = ' ';

  colText = absent ? NULL
            : select
                ? (disabled && gadgetColor[3] ? gadgetColor[3]
                                              : (gadgetColor[1] ? gadgetColor[1] : gadgetColor[0]))
                : (disabled && gadgetColor[2] ? gadgetColor[2] : gadgetColor[0]);

  if (!colText) {
    absent = true;
    color = select ? (disabled ? g->ColSelectDisabled : g->ColSelect)
                   : (disabled ? g->ColDisabled : g->ColText);
    color = DoShadowColor(color, d->Shaded, d->Shaded);
  }

  for (j = j_min; j <= j_max; j++) {
    for (i = i_min; i <= i_max; i++) {
      if (text)
        font = text[i + j * width];
      if (!absent)
        color = DoShadowColor(colText[i + j * width], d->Shaded, d->Shaded);
      Video[i + j * (ldat)dwidth + offset] = TCELL(color, font);
    }
  }
  DirtyVideo(d->X1, d->Y1, d->X2, d->Y2);
}

static void DrawSelfBorder(Twindow w, ldat left, ldat up, ldat rgt, ldat dwn, dat x1, dat y1,
                           dat x2, dat Y2, byte border, bool winActive, bool shaded) {

  ldat i, j, u, v, dwidth = All->DisplayWidth; /* (ldat) to avoid multiplication overflows */
  tcell attr;
  tcolor color;

  if (QueuedDrawArea2FullScreen || (w->Flags & WIDGETFL_NOTVISIBLE)) {
    return;
  }

  if ((ldat)y1 == up) {
    j = y1 * dwidth;
    for (i = x1, u = i - left; i <= x2; i++, u++) {
      w->FindBorder(u, 0, border, &attr);
      color = DoShadowColor(TCOLOR(attr), shaded || !winActive, shaded);
      Video[i + j] = TCELL(color, 0) | (attr & ~TCELL(TW_MAXWCOL, 0));
    }
    DirtyVideo(x1, y1, x2, y1);
    y1++;
    if (y1 > Y2)
      return;
  }

  if ((ldat)Y2 == dwn) {
    v = (ldat)Y2 - up;
    j = Y2 * dwidth;
    for (i = x1, u = i - left; i <= x2; i++, u++) {
      w->FindBorder(u, v, border, &attr);
      color = DoShadowColor(TCOLOR(attr), shaded || !winActive, shaded);
      Video[i + j] = TCELL(color, 0) | (attr & ~TCELL(TW_MAXWCOL, 0));
    }
    DirtyVideo(x1, Y2, x2, Y2);
    Y2--;
    if (y1 > Y2)
      return;
  }

  if ((ldat)x1 == left) {
    for (j = y1, v = j - up; j <= Y2; j++, v++) {
      w->FindBorder(0, v, border, &attr);
      color = DoShadowColor(TCOLOR(attr), shaded || !winActive, shaded);
      Video[x1 + j * dwidth] = TCELL(color, 0) | (attr & ~TCELL(TW_MAXWCOL, 0));
    }
    DirtyVideo(x1, y1, x1, Y2);
    x1++;
    if (x1 > x2)
      return;
  }

  if ((ldat)x2 == rgt) {
    u = (ldat)x2 - left;
    for (j = y1, v = j - up; j <= Y2; j++, v++) {
      w->FindBorder(u, v, border, &attr);
      color = DoShadowColor(TCOLOR(attr), shaded || !winActive, shaded);
      Video[x2 + j * dwidth] = TCELL(color, 0) | (attr & ~TCELL(TW_MAXWCOL, 0));
    }
    DirtyVideo(x2, y1, x2, Y2);
    x2--;
    if (x1 > x2)
      return;
  }
}

void Swindow::DrawSelf(Sdraw *d) {
  if (!d->TopW || !IS_WINDOW(d->TopW) || QueuedDrawArea2FullScreen) {
    return;
  }
  Twindow w = (Twindow)d->TopW;
  if ((w->Flags & WIDGETFL_NOTVISIBLE)) {
    return;
  } else if (!W_USE(w, USEROWS) && !W_USE(w, USECONTENTS)) {
    Swidget::DrawSelf(d);
    return;
  }

  const tcell *contents = NULL, *currCont = NULL;
  const trune *runes = NULL;
  tcolor *colText = NULL;
  ldat row, posInRow;
  trune font;
  Trow curr_row;

  ldat left = d->Left, up = d->Up, rgt = d->Rgt;
  ldat x1 = d->X1, y1 = d->Y1, x2 = d->X2, Y2 = d->Y2;
  ldat dwidth = d->DWidth;

  tcolor color;
  bool shaded = d->Shaded, absent, select, rowDisabled;

  ldat i, j, u, v; /* (ldat) to avoid multiplication overflows */

  /* not here... already done in Sdraw.Draw() */
  /*
   * left -= w->XLogic; rgt -= w->XLogic;
   * up   -= w->YLogic; dwn -= w->YLogic;
   */

  if (W_USE(w, USECONTENTS) && (contents = w->USE.C.Contents)) {
    /*
     * For xterm-like windows, Contents is a buffer of (x=WLogic) * (y=HLogic)
     * tcell:screen, in the same format as Video: TCELL(color, Ascii).
     * Room in excess is used as scrollback.
     *
     * To avoid frequent mem-mem copies, VGA-like splitline method is used:
     * USE.C.Split is the first used line of Contents, and after the end
     * of Contents we restart from zero.
     *
     * HLogic also has the usual meaning:
     * number of total lines (visible + scrollback)
     */
    if (Y2 - up >= w->HLogic) {
      /* the ->Contents buffer is smaller than the Twindow size... pad with SPACEs */
      dat Ynew = up + w->HLogic;
      FillVideo(x1, Ynew, x2, Y2, TCELL(w->ColText, ' '));
      Y2 = Ynew - 1;
    }
    if (x2 - left >= w->WLogic) {
      /* the ->Contents buffer is smaller than the Twindow size... pad with SPACEs */
      dat Xnew = left + w->WLogic;
      FillVideo(Xnew, y1, x2, Y2, TCELL(w->ColText, ' '));
      x2 = Xnew - 1;
    }
    if (x1 <= x2 && y1 <= Y2) {
      row = y1 - up + w->USE.C.HSplit; /* row number in Contents */
      if (row >= w->HLogic)
        row -= w->HLogic;
      currCont = contents + row * w->WLogic;

      row = w->HLogic - 1 - row; /* rows still readable */

      if (!shaded) {
        for (j = y1, u = y1 - up; j <= Y2; j++, u++, row--) {
          if (!(w->State & WINDOW_DO_SEL) || u < w->YstSel || u > w->YendSel) {

            CopyMem(currCont + x1 - left, &Video[x1 + j * (ldat)dwidth],
                    sizeof(tcell) * (x2 - x1 + 1));

          } else {
            for (i = x1, v = x1 - left; i <= x2; i++, v++) {
              select = (w->State & WINDOW_DO_SEL) &&
                       ((u > w->YstSel || (u == w->YstSel && v >= w->XstSel)) &&
                        (u < w->YendSel || (u == w->YendSel && v <= w->XendSel)));
              if (select)
                color = w->ColSelect;
              else
                color = TCOLOR(currCont[v]);

              Video[i + j * (ldat)dwidth] = TCELL(color, TRUNE(currCont[v]));
            }
          }
          currCont += w->WLogic;
          if (!row)
            currCont -= (row = w->HLogic) * w->WLogic;
        }
      } else {
        for (j = y1, u = y1 - up; j <= Y2; j++, u++, row--) {
          for (i = x1, v = x1 - left; i <= x2; i++, v++) {

            select = (w->State & WINDOW_DO_SEL) &&
                     ((u > w->YstSel || (u == w->YstSel && v >= w->XstSel)) &&
                      (u < w->YendSel || (u == w->YendSel && v <= w->XendSel)));

            if (select)
              color = w->ColSelect;
            else
              color = TCOLOR(currCont[v]);
            color = DoShadowColor(color, shaded, shaded);

            Video[i + j * (ldat)dwidth] = TCELL(color, TRUNE(currCont[v]));
          }
          currCont += w->WLogic;
          if (!row)
            currCont -= (row = w->HLogic) * w->WLogic;
        }
      }
      DirtyVideo(x1, y1, x2, Y2);
    }
  } else if (W_USE(w, USEROWS) && w->USE.R.Rows.First) {
    /*
     * editor-like windows. no length limit in the rows and in XLogic.
     */
    row = y1 - up;

    if (row >= w->HLogic)
      curr_row = (Trow)0;
    else if (w->USE.R.NumRowSplit && w->USE.R.RowSplit && row == w->USE.R.NumRowSplit)
      curr_row = w->USE.R.RowSplit;
    else if (w->USE.R.NumRowSplit && w->USE.R.RowSplit && w->USE.R.RowSplit->Next &&
             row == w->USE.R.NumRowSplit + (ldat)1)
      curr_row = w->USE.R.RowSplit->Next;
    else if (w->USE.R.NumRowOne && w->USE.R.RowOne && row == w->USE.R.NumRowOne)
      curr_row = w->USE.R.RowOne;
    else if (row == (ldat)0)
      curr_row = w->USE.R.Rows.First;
    else if (row + (ldat)1 == w->HLogic)
      curr_row = w->USE.R.Rows.Last;
    else
      curr_row = w->FindRow(row);

    for (j = y1; j <= Y2; j++, row++) {

      rowDisabled = (w->Flags & WINDOWFL_DISABLED) ||
                    (curr_row && !(curr_row->Flags & (ROW_ACTIVE | ROW_IGNORE)));

      if (curr_row) {
        runes = curr_row->Text;
        colText = curr_row->ColText;
      }

      posInRow = x1 - left;
      if (curr_row && posInRow >= 0 && (uldat)posInRow >= curr_row->Gap)
        posInRow += curr_row->LenGap;

      for (i = x1; i <= x2; i++, posInRow++) {

        absent = !curr_row || (posInRow >= 0 && (uldat)posInRow >= curr_row->Len);

        if (curr_row && IS_MENUITEM(curr_row) && ((Tmenuitem)curr_row)->Window && i == rgt) {
          font = T_UTF_32_BLACK_RIGHT_POINTING_TRIANGLE;
        } else if (absent)
          font = ' ';
        else
          font = runes[posInRow];

        if (w->Flags & WINDOWFL_ROWS_SELCURRENT)
          select = row == w->CurY;
        else {
          select = (w->State & WINDOW_DO_SEL) &&
                   ((row > w->YstSel || (row == w->YstSel && posInRow >= w->XstSel)) &&
                    (row < w->YendSel || (row == w->YendSel && posInRow <= w->XendSel)));
        }
        if (curr_row)
          select &= !(curr_row->Flags & ROW_IGNORE);

        if (select)
          color = rowDisabled ? w->ColSelectDisabled : w->ColSelect;
        else if (rowDisabled)
          color = w->ColDisabled;
        else if ((w->Flags & WINDOWFL_ROWS_DEFCOL) || absent)
          color = w->ColText;
        else
          color = colText[posInRow];

        color = DoShadowColor(color, shaded, shaded);
        Video[i + j * (ldat)dwidth] = TCELL(color, font);
      }
      if (curr_row) {
        w->USE.R.RowSplit = curr_row;
        w->USE.R.NumRowSplit = row;
        curr_row = curr_row->Next;
      }
    }
    DirtyVideo(x1, y1, x2, Y2);
  } else {
    /* either an unknown Twindow type or just one of the above, but empty */
    color = w->ColText;
    color = DoShadowColor(color, shaded, shaded);
    FillVideo(x1, y1, x2, Y2, TCELL(color, ' '));
  }
}

void Sscreen::DrawSelf(Sdraw *d) {
  (void)d;
  /* should never be called */
  log(ERROR) << "twin: Sscreen::DrawSelf() called! This should not happen.\n";
}

static void _DrawWCtx_(Sdraw **FirstD, Twidget w, Twidget child_next, Twidget only_child, ldat left,
                       ldat up, ldat rgt, ldat dwn, dat x1, dat y1, dat x2, dat Y2,
                       bool no_children, bool borderDone, byte shaded, e_errnum *lError) {
  Sdraw *d;
  if (!QueuedDrawArea2FullScreen) {
    if ((d = (Sdraw *)AllocMem(sizeof(Sdraw)))) {
      d->TopW = w;
      d->W = child_next;
      d->OnlyW = only_child;
      d->Screen = (Tscreen)0;
      d->X1 = x1;
      d->Y1 = y1;
      d->X2 = x2;
      d->Y2 = Y2;
      d->Left = left;
      d->Up = up;
      d->Rgt = rgt;
      d->Dwn = dwn;
      d->DWidth = All->DisplayWidth;
      d->DHeight = All->DisplayHeight;
      d->NoChildren = no_children;
      d->BorderDone = borderDone;
      d->Shaded = shaded;

      d->Next = *FirstD;
      *FirstD = d;
    } else {
      DrawDesktop((Tscreen)0, x1, y1, x2, Y2, shaded);
      *lError = NOMEMORY;
    }
  }
}

void Sdraw::Draw() {
  Sdraw *d = this;
  Sdraw *FirstD = d;
  Twidget w;
  Twidget only_child, child_next;
  Twindow window;
  ldat left, up, rgt, dwn;
  dat x1, y1, x2, y2;
  ldat cL, cU, cR, cD;
  dat dwidth, dheight;
  e_errnum lError = SUCCESS;
  byte border;
  bool shaded, winActive, no_children;
  byte child_found = false, first_cycle = true;

  if (QueuedDrawArea2FullScreen)
    return;

  do {
    w = d->TopW;
    child_next = d->W;
    only_child = d->OnlyW;
    x1 = d->X1;
    y1 = d->Y1;
    x2 = d->X2;
    y2 = d->Y2;
    left = d->Left;
    up = d->Up;
    rgt = d->Rgt;
    dwn = d->Dwn;
    dwidth = d->DWidth;
    dheight = d->DHeight;
    shaded = d->Shaded;
    no_children = d->NoChildren;

    if (only_child == w) /* got it */
      only_child = NULL;

#ifdef DEBUG_DRAW
    FillVideo(x1, y1, x2, y2, TCELL(TCOL(twhite, tyellow), ' '));
    FlushHW();
    usleep(300000);
    FillVideo(x1, y1, x2, y2, TCELL(TCOL(twhite, tblack), ' '));
#endif

    FirstD = d->Next;

    /* take care of borders and XLogic,YLogic */
    if (!d->BorderDone) {
      d->BorderDone = true;

      if (IS_WINDOW(w) && !((window = (Twindow)w)->Flags & WINDOWFL_BORDERLESS)) {

        if (!only_child) {
          winActive =
              window == All->Screens.First->MenuWindow || window == All->Screens.First->FocusW();
          border = (window->Flags & WINDOWFL_MENU) || !winActive;
          DrawSelfBorder(window, left, up, rgt, dwn, x1, y1, x2, y2, border, winActive, shaded);
        }

        if ((ldat)x1 == left++)
          x1++;
        if ((ldat)x2 == rgt--)
          x2--;
        if ((ldat)y1 == up++)
          y1++;
        if ((ldat)y2 == dwn--)
          y2--;
        d->X1 = x1;
        d->Y1 = y1;
        d->X2 = x2;
        d->Y2 = y2;
      }
      /*
       * track displacement between real display coordinates
       * and Twidget interior coordinates.
       */
      d->Left = left -= w->XLogic;
      d->Up = up -= w->YLogic;
      d->Rgt = rgt -= w->XLogic;
      d->Dwn = dwn -= w->YLogic;
    }

    if (x1 > x2 || y1 > y2 || x1 >= dwidth || y1 >= dheight || x2 < 0 || y2 < 0) {
      if (first_cycle) {
        first_cycle = false;
      } else {
        FreeMem(d);
      }
      continue;
    }

    child_found = false;
    if (no_children)
      child_next = NULL;
    else {
      if (!child_next) {
        child_next = w->Widgets.First;
      }
      if (!no_children && child_next) {
        while (child_next && !child_found) {
          cL = left + child_next->Left;
          cR = cL + child_next->XWidth - 1;
          cU = up + child_next->Up;
          cD = cU + child_next->YWidth - 1;
          if (cL > x2 || cU > y2 || cR < x1 || cD < y1 || (child_next->Flags & WIDGETFL_NOTVISIBLE))
            child_next = child_next->Next;
          else
            child_found = true;
        }
      }
      if (!child_next)
        no_children = true;
    }

    if (!child_found) {
      /* no children... draw this Twidget */
      w->DrawSelf(d);
    }

    if (!first_cycle)
      FreeMem(d);
    else
      first_cycle = false;

    if (child_found) {

      /* recursively draw the area below child_next */
      if (y2 > cD) {
        _DrawWCtx_(&FirstD, w, child_next, only_child, left, up, rgt, dwn, x1, cD + 1, x2, y2,
                   no_children, ttrue, shaded, &lError);
        y2 = cD;
      }

      /* recursively draw the area right of child_next */
      if (x2 > cR) {
        _DrawWCtx_(&FirstD, w, child_next, only_child, left, up, rgt, dwn, cR + 1, y1, x2, y2,
                   no_children, ttrue, shaded, &lError);
        x2 = cR;
      }

      /* recursively draw the area left of child_next */
      if (x1 < cL) {
        _DrawWCtx_(&FirstD, w, child_next, only_child, left, up, rgt, dwn, x1, y1, cL - 1, y2,
                   no_children, ttrue, shaded, &lError);
        x1 = cL;
      }

      /* recursively draw the area above child_next */
      if (y1 < cU) {
        _DrawWCtx_(&FirstD, w, child_next, only_child, left, up, rgt, dwn, x1, y1, x2, cU - 1,
                   no_children, ttrue, shaded, &lError);
        y1 = cU;
      }

      /* ok, now actually draw child_next */
      if (x1 <= x2 && y1 <= y2) {
        _DrawWCtx_(&FirstD, child_next, NULL, only_child, cL, cU, cR, cD, x1, y1, x2, y2, tfalse,
                   tfalse, shaded, &lError);
      }
    }
  } while ((d = FirstD));

  if (lError)
    Error(lError);
}

/*
 * ASSUMES the specified part of the Twidget is unobscured.
 * x1,y1,x2,y2 are absolute Tscreen coordinates.
 */
void DrawUnobscuredWidget(Twidget w, dat x1, dat y1, dat x2, dat y2, bool shaded) {
  Sdraw d;

  if (!QueuedDrawArea2FullScreen && w && d.InitAbsolute(w, x1, y1, x2, y2, shaded)) {
    d.TopW = w;
    d.W = d.OnlyW = NULL;
    d.Draw();
  }
}

/*
 * does NOT assume that the specified part of the Twidget is unobscured.
 *
 * does NOT draw shadow.
 */
void DrawAreaWidget(Twidget w) {
  Sdraw d;

  if (!QueuedDrawArea2FullScreen && w && d.InitAbsolute(w, 0, 0, TW_MAXDAT, TW_MAXDAT, false)) {
    d.TopW = d.W = d.OnlyW = NULL;
    d.DrawArea();
  }
}

static void _DrawAreaCtx_(Sdraw **FirstD, Tscreen screen, Twidget w, Twidget onlyW, dat x1, dat Y1,
                          dat X2, dat Y2, bool shaded, e_errnum *lError) {
  Sdraw *d;
  if (!QueuedDrawArea2FullScreen) {
    if ((d = (Sdraw *)AllocMem(sizeof(Sdraw)))) {
      d->TopW = w;
      d->W = onlyW;
      d->Screen = screen;
      d->Left = d->X1 = x1;
      d->Up = d->Y1 = Y1;
      d->Rgt = d->X2 = X2;
      d->Dwn = d->Y2 = Y2;
      d->DWidth = All->DisplayWidth;
      d->DHeight = All->DisplayHeight;
      d->Shaded = shaded;

      d->Next = *FirstD;
      *FirstD = d;

    } else {
      DrawDesktop((Tscreen)0, x1, Y1, X2, Y2, shaded);
      *lError = NOMEMORY;
    }
  }
}

Twindow WindowParent(Twidget w) {
  if (w)
    do {
      if (IS_WINDOW(w))
        return (Twindow)w;
    } while ((w = w->Parent));
  return (Twindow)0;
}

Tscreen ScreenParent(Twidget w) {
  if (w)
    do {
      if (IS_SCREEN(w))
        return (Tscreen)w;
    } while ((w = w->Parent));
  return (Tscreen)0;
}

Twidget NonScreenParent(Twidget w) {
  if (w)
    while (w->Parent) {
      if (IS_SCREEN(w->Parent))
        return w;
      w = w->Parent;
    }
  return w;
}

Twindow FindCursorWindow(void) {
  Tscreen s = All->Screens.First;
  Twidget p = s->FocusW();
  Twidget w;
  if (!p) {
    p = s->MenuWindow;
  }
  if (p) {
    while ((w = p->SelectW)) {
      p = w;
    }
  }
  return WindowParent(p);
}

bool ContainsCursor(Twidget w) {
  Twidget c = (Twidget)FindCursorWindow();
  while (c) {
    if (c == w) {
      return ttrue;
    }
    c = c->Parent;
  }
  return tfalse;
}

void Sdraw::DrawArea() {
  Sdraw *d = this;
  Sdraw *FirstD = d;
  Tscreen first_screen, screen;
  Twidget w, onlyW, TopOnlyW, NextW;
  Ssetup *SetUp;
  ldat dwidth, dheight, ylimit;
  ldat sh_left = 0, sh_up = 0, sh_right = 0, sh_down = 0;
  /* position of Horizontal Shadow */
  ldat hs_x1, hs_x2, hs_y1, s_y2;
  /* position of Vertical Shadow */
  ldat vs_x1, vs_x2, vs_y1;
  dat left, up, rgt, dwn;
  dat x1, y1, x2, y2;
  e_errnum lError = SUCCESS;
  byte WidgetFound, shade;
  byte deltaXShade, deltaYShade;
  bool first_cycle = true, shaded;

  if (QueuedDrawArea2FullScreen)
    return;

  SetUp = All->SetUp;

  do {
    w = d->TopW;
    onlyW = d->W;
    first_screen = d->Screen;
    x1 = d->X1;
    y1 = d->Y1;
    x2 = d->X2;
    y2 = d->Y2;
    dwidth = d->DWidth;
    dheight = d->DHeight;
    shaded = d->Shaded;

    FirstD = d->Next;

    if (!first_cycle)
      FreeMem(d);
    else
      first_cycle = false;

    if (x1 > x2 || y1 > y2 || x1 >= dwidth || y1 >= dheight || x2 < 0 || y2 < 0)
      continue;

    x1 = Max2(x1, 0);
    y1 = Max2(y1, 0);
    x2 = Min2(x2, dwidth - 1);
    y2 = Min2(y2, dheight - 1);

    if (w) {
      if (IS_SCREEN(w->Parent)) {
        first_screen = (Tscreen)w->Parent;
      } else {
        continue;
      }
    }
    if (!first_screen) {
      first_screen = All->Screens.First;
    }
    /* calculate visible part of this Tscreen */
    for (screen = All->Screens.First; screen && screen != first_screen;
         screen = screen->NextScreen()) {
      ylimit = screen->Up;
      if (y2 >= ylimit)
        y2 = ylimit - 1;
    }
    if (y1 > y2) {
      continue;
    }
    screen = first_screen->NextScreen();
    ylimit = first_screen->Up; /* FIXED +1 */

    if (!onlyW || (onlyW && ScreenParent(onlyW->Parent) != first_screen)) {
      if (y1 < ylimit) {
        if (screen) {
          _DrawAreaCtx_(&FirstD, screen, w, onlyW, x1, y1, x2, Min2(y2, ylimit - 1), shaded,
                        &lError);
        } else if (!onlyW) {
          DrawDesktop((Tscreen)0, x1, y1, x2, y2, shaded);
        }
      }
      if (onlyW) {
        continue;
      }
    }

    if (y2 < ylimit)
      continue;

    if (y1 < ylimit)
      y1 = ylimit;

    if (!w) {
      w = first_screen->Widgets.First;
      if (y1 == ylimit && !onlyW) {
        first_screen->DrawMenu(x1, x2);
        if (++y1 > y2)
          continue;
      }
    }

    shade = (SetUp->Flags & setup_shadows) && !shaded;
    deltaXShade = shade ? SetUp->DeltaXShade : (byte)0;
    deltaYShade = shade ? SetUp->DeltaYShade : (byte)0;

    WidgetFound = tfalse;
    while (w && !WidgetFound) {
      if (w->Flags & WIDGETFL_NOTVISIBLE) {
        w = w->Next;
        continue;
      }
      sh_left = (ldat)w->Left + (ldat)first_screen->Left - first_screen->XLogic;
      sh_up = (ldat)w->Up + (ldat)ylimit - first_screen->YLogic;
      sh_right = sh_left + (ldat)w->XWidth - 1;
      sh_down = sh_up +
                (IS_WINDOW(w) && (((Twindow)w)->Attr & WINDOW_ROLLED_UP) ? 0 : (ldat)w->YWidth - 1);

      if (shade && IS_WINDOW(w)) {
        /* only windows have shadows */
        hs_x1 = Max2(sh_left + (ldat)deltaXShade, (ldat)x1);
        hs_x2 = Min2(sh_right, (ldat)x2);
        hs_y1 = Max2(sh_up + (ldat)deltaYShade, sh_down + (ldat)1);
        hs_y1 = Max2(hs_y1, (ldat)y1);
        s_y2 = Min2(sh_down + (ldat)deltaYShade, (ldat)y2);

        vs_x1 = Max2(sh_left + (ldat)deltaXShade, sh_right + (ldat)1);
        vs_x1 = Max2(vs_x1, (ldat)x1);
        vs_x2 = Min2(sh_right + (ldat)deltaXShade, (ldat)x2);
        vs_y1 = Max2(sh_up + (ldat)deltaYShade, (ldat)y1);
      }

      if (sh_left <= (ldat)x2 && sh_right >= (ldat)x1 && sh_up <= (ldat)y2 && sh_down >= (ldat)y1)
        WidgetFound = ttrue;
      else if (shade && IS_WINDOW(w) &&
               ((hs_x1 <= hs_x2 && hs_y1 <= s_y2) || (vs_x1 <= vs_x2 && vs_y1 <= s_y2)))
        WidgetFound = ttrue + ttrue;
      else
        w = w->Next;
    }

    /* again, only windows have shadows */
    if (w && !IS_WINDOW(w)) {
      shade = tfalse;
      deltaXShade = deltaYShade = (byte)0;
    }

    if (!WidgetFound && !onlyW) {
      DrawDesktop(first_screen, x1, y1, x2, y2, shaded);
      continue;
    }

    TopOnlyW = onlyW ? NonScreenParent(onlyW) : NULL;

    if (WidgetFound == ttrue && (!onlyW || TopOnlyW == w)) {
      Sdraw *fd = NULL;
      _DrawWCtx_(&fd, w, NULL, onlyW, sh_left, sh_up, sh_right, sh_down, Max2(x1, sh_left),
                 Max2(y1, sh_up), Min2(x2, sh_right), Min2(y2, sh_down), tfalse, tfalse, shaded,
                 &lError);
      if (fd) {
        fd->Draw();
        FreeMem(fd);
      }
    }

    /* Draw the Twindow's shadow : */

    NextW = w ? w->Next : w;

    if (WidgetFound && shade && TopOnlyW != w) {

      if (hs_x1 <= hs_x2 && hs_y1 <= s_y2) {
        if (NextW)
          _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, (dat)hs_x1, (dat)hs_y1, (dat)hs_x2,
                        (dat)s_y2, ttrue, &lError);
        else
          DrawDesktop(first_screen, (dat)hs_x1, (dat)hs_y1, (dat)hs_x2, (dat)s_y2, true);
      }
      if (vs_x1 <= vs_x2 && vs_y1 <= s_y2) {
        if (NextW)
          _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, (dat)vs_x1, (dat)vs_y1, (dat)vs_x2,
                        (dat)s_y2, ttrue, &lError);
        else
          DrawDesktop(first_screen, (dat)vs_x1, (dat)vs_y1, (dat)vs_x2, (dat)s_y2, true);
      }
    }

    if (TopOnlyW == w || (TopOnlyW && !NextW))
      continue;

    /* Draw the visible area below the Twindow : */

    if (sh_down + (ldat)deltaYShade < (ldat)y2) {
      dwn = (dat)sh_down + (dat)deltaYShade;
      if (NextW)
        _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, x1, dwn + (dat)1, x2, y2, shaded, &lError);
      else
        DrawDesktop(first_screen, x1, dwn + (dat)1, x2, y2, shaded);
    } else
      dwn = y2;

    /* Draw the visible area right of the Twindow : */

    if (sh_right + (ldat)deltaXShade < (ldat)x2) {
      rgt = (dat)sh_right + (dat)deltaXShade;
      if (NextW)
        _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, rgt + (dat)1, y1, x2, dwn, shaded,
                      &lError);
      else
        DrawDesktop(first_screen, rgt + (dat)1, y1, x2, dwn, shaded);
    } else
      rgt = x2;

    /* Draw the visible area left of the Twindow : */

    if (sh_left > (ldat)x1) {
      left = (dat)sh_left;
      if (NextW)
        _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, x1, y1, left - (dat)1, dwn, shaded,
                      &lError);
      else
        DrawDesktop(first_screen, x1, y1, left - (dat)1, dwn, shaded);
    } else
      left = x1;

    /* Draw the visible area above the Twindow : */

    if (sh_up > (ldat)y1) {
      up = (dat)sh_up;
      if (NextW)
        _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, left, y1, rgt, up - (dat)1, shaded,
                      &lError);
      else
        DrawDesktop(first_screen, left, y1, rgt, up - (dat)1, shaded);
    } else
      up = y1;

    if (shade) {

      ldat X_1, Y_1, X_2, Y_2;

      X_1 = Max2(sh_left, (ldat)x1);
      X_2 = Min2(sh_left + (ldat)deltaXShade - (ldat)1, (ldat)x2);
      Y_1 = Max2(sh_down + (ldat)1, (ldat)y1);
      Y_2 = Min2(sh_down + (ldat)deltaYShade, (ldat)y2);

      /* Draw the visible area below the Twindow, left of the shadow : */

      if (X_1 <= X_2 && Y_1 <= Y_2) {
        if (NextW)
          _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2,
                        shaded, &lError);
        else
          DrawDesktop(first_screen, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, shaded);
      }

      /* Draw the visible area right of the Twindow, above the shadow : */

      X_1 = Max2(sh_right + (ldat)1, (ldat)x1);
      X_2 = Min2(sh_right + (ldat)deltaXShade, (ldat)x2);
      Y_1 = Max2(sh_up, (ldat)y1);
      Y_2 = Min2(sh_up + (ldat)deltaYShade - (ldat)1, sh_down);
      Y_2 = Min2(Y_2, (ldat)y2);

      if (X_1 <= X_2 && Y_1 <= y2) {
        if (NextW)
          _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2,
                        shaded, &lError);
        else
          DrawDesktop(first_screen, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, shaded);
      }

      /* Draw the visible area between the Twindow and a floating shadow : */

      if (sh_left + (ldat)deltaXShade > sh_right + (ldat)1 ||
          sh_up + (ldat)deltaYShade > sh_down + (ldat)1) {

        if (sh_up + (ldat)deltaYShade <= sh_down + (ldat)1) {
          /*
           * the shadow is like
           *
           *  +--+BBBBBB
           *  |  |CC+--+
           *  +--+CC|  |
           *  AAAAAA+--+
           *
           *  parts A and B are already drawn, now draw c
           */
          X_1 = Max2(sh_right + (ldat)1, (ldat)x1);
          X_2 = Min2(sh_left + (ldat)deltaXShade - (ldat)1, (ldat)x2);
          Y_1 = Max2(sh_up + (ldat)deltaYShade, (ldat)y1);
          Y_2 = Min2(sh_down, (ldat)y2);
        } else {
          /*
           * the shadow is like
           *
           *  +--+BBB        +--+BBBBB
           *  |  |BBB               |  |BBBBB
           *  +--+BBB               +--+BBBBB
           *  AAACCCC          or   AAAAACCCC
           *  AAA+--+               AAAAA+--+
           *  AAA|  |               AAAAA|  |
           *  AAA+--+               AAAAA+--+
           *
           * and we now draw c
           */
          X_1 = Max2(sh_left + (ldat)deltaXShade, (ldat)x1);
          X_2 = Min2(sh_right + (ldat)deltaXShade, (ldat)x2);
          Y_1 = Max2(sh_down + (ldat)1, (ldat)y1);
          Y_2 = Min2(sh_up + (ldat)deltaYShade - (ldat)1, (ldat)y2);
        }

        if (X_1 <= X_2 && Y_1 <= y2) {
          if (NextW)
            _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2,
                          shaded, &lError);
          else
            DrawDesktop(first_screen, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, shaded);
        }
      }
    }
  } while ((d = FirstD));

  if (lError)
    Error(lError);
}

void DrawArea2(Tscreen first_screen, Twidget w, Twidget onlyW, dat x1, dat y1, dat x2, dat y2,
               bool shaded) {
  Sdraw d;

  if (QueuedDrawArea2FullScreen)
    return;

  d.DWidth = All->DisplayWidth;
  d.DHeight = All->DisplayHeight;

  d.X1 = Max2(x1, 0);
  d.Y1 = Max2(y1, 0);
  d.X2 = Min2(x2, d.DWidth - 1);
  d.Y2 = Min2(y2, d.DHeight - 1);

  if (d.X1 <= d.X2 && d.Y1 <= d.Y2) {
    d.Next = NULL;
    d.W = onlyW;
    d.TopW = w;
    d.Screen = first_screen;
    d.Shaded = shaded;

    d.DrawArea();
  }
}

void DrawBorderWindow(Twindow w, byte Flags) {
  ldat sh_left, sh_up, sh_right, sh_down;
  dat left, up, rgt, dwn;
  dat dwidth, dheight;
  dat ylimit;
  Twidget first_w;
  Twidget Parent;
  Tscreen screen, first_screen;

  if (QueuedDrawArea2FullScreen || !w || (w->Flags & (WINDOWFL_BORDERLESS | WINDOWFL_NOTVISIBLE)) ||
      !(Parent = w->Parent) || !IS_SCREEN(Parent))
    return;

  screen = (Tscreen)Parent;
  dwidth = All->DisplayWidth;
  dheight = All->DisplayHeight;
  ylimit = screen->Up; /* FIXED +1 */

  sh_up = (ldat)w->Up - screen->YLogic + (ldat)ylimit;
  sh_left = (ldat)w->Left - screen->XLogic;
  sh_right = sh_left + (ldat)w->XWidth - (ldat)1;
  sh_down = sh_up + (w->Attr & WINDOW_ROLLED_UP ? 0 : (ldat)w->YWidth - (ldat)1);

  if (sh_left >= (ldat)dwidth || sh_up >= (ldat)dheight || sh_right < (ldat)0 ||
      sh_down <= (ldat)ylimit)
    return;

  first_screen = screen == All->Screens.First ? screen : (Tscreen)0;
  first_w = first_screen && (Twidget)w == screen->Widgets.First ? (Twidget)w : (Twidget)0;

  left = (dat)Max2(sh_left, (ldat)0);
  up = (dat)Max2(sh_up, (ldat)ylimit + 1);
  rgt = (dat)Min2(sh_right, (ldat)dwidth - (ldat)1);
  dwn = (dat)Min2(sh_down, (ldat)dheight - (ldat)1);

  if ((Flags & BORDER_UP) && sh_up >= ylimit)
    DrawArea2(first_screen, first_w, (Twidget)w, left, up, rgt, up, tfalse);
  if (!(w->Flags & WINDOW_ROLLED_UP)) {
    if ((Flags & BORDER_LEFT) && sh_left >= 0)
      DrawArea2(first_screen, first_w, (Twidget)w, left, up, left, dwn, tfalse);
    if ((Flags & BORDER_RIGHT) && sh_right < (ldat)dwidth)
      DrawArea2(first_screen, first_w, (Twidget)w, rgt, up, rgt, dwn, tfalse);
    if ((Flags & BORDER_DOWN) && sh_down < (ldat)dheight)
      DrawArea2(first_screen, first_w, (Twidget)w, left, dwn, rgt, dwn, tfalse);
  }
}

void DrawAreaShadeWindow(Tscreen screen, Twindow w, dat x1, dat y1, dat x2, dat y2, ldat sh_left,
                         ldat sh_up, ldat sh_right, ldat sh_down, byte Internal) {
  dat dheight, dwidth, ylimit;
  byte deltaXShade, deltaYShade;
  Ssetup *SetUp;

  SetUp = All->SetUp;
  if (QueuedDrawArea2FullScreen || !w || (w->Flags & WINDOWFL_NOTVISIBLE) || !screen || x1 > x2 ||
      y1 > y2 || !(SetUp->Flags & setup_shadows))
    return;

  deltaXShade = SetUp->DeltaXShade;
  deltaYShade = SetUp->DeltaYShade;

  dwidth = All->DisplayWidth;
  dheight = All->DisplayHeight;
  ylimit = screen->Up;

  x1 = Max2(x1, 0);
  y1 = Max2(y1, ylimit + 1);
  x2 = Min2(x2, dwidth - 1);
  y2 = Min2(y2, dheight - 1);

  if (sh_left + (ldat)deltaXShade > (ldat)x2 || sh_up + (ldat)deltaYShade > (ldat)y2 ||
      sh_right + (ldat)deltaXShade < (ldat)x1 || sh_down + (ldat)deltaYShade < (ldat)y1) {
    return;
  }
  w = (Twindow)w->Next;

  /* position of Horizontal Shadow */
  ldat hs_x1 = Max2(sh_left + (ldat)deltaXShade, (ldat)x1);
  ldat hs_x2 = Min2(sh_right, (ldat)x2);
  ldat hs_y1 = Max3(sh_up + (ldat)deltaYShade, sh_down + (ldat)1, (ldat)y1);

  /* position of Vertical Shadow */
  ldat vs_x1 = Max3(sh_left + (ldat)deltaXShade, sh_right + (ldat)1, (ldat)x1);
  ldat vs_x2 = Min2(sh_right + (ldat)deltaXShade, (ldat)x2);
  ldat vs_y1 = Max2(sh_up + (ldat)deltaYShade, (ldat)y1);

  /* end of Horizontal and Vertical Shadow */
  ldat s_y2 = Min2(sh_down + (ldat)deltaYShade, (ldat)y2);

  if (hs_x1 <= hs_x2 && hs_y1 <= s_y2) {
    if (!Internal)
      DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, (dat)hs_x1, (dat)hs_y1, (dat)hs_x2, (dat)s_y2,
                tfalse);
    else if (w)
      DrawArea2((Tscreen)0, (Twidget)w, (Twidget)0, (dat)hs_x1, (dat)hs_y1, (dat)hs_x2, (dat)s_y2,
                ttrue);
    else
      DrawDesktop(screen, (dat)hs_x1, (dat)hs_y1, (dat)hs_x2, (dat)s_y2, true);
  }

  if (vs_x1 <= vs_x2 && vs_y1 <= s_y2) {
    if (!Internal)
      DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, (dat)vs_x1, (dat)vs_y1, (dat)vs_x2, (dat)s_y2,
                tfalse);
    else if (w)
      DrawArea2((Tscreen)0, (Twidget)w, (Twidget)0, (dat)vs_x1, (dat)vs_y1, (dat)vs_x2, (dat)s_y2,
                ttrue);
    else
      DrawDesktop(screen, (dat)vs_x1, (dat)vs_y1, (dat)vs_x2, (dat)s_y2, true);
  }
}

void DrawShadeWindow(Twindow w, dat x1, dat y1, dat x2, dat y2, byte internal) {
  Tscreen screen;
  dat ylimit;
  ldat sh_left, sh_up, sh_right, sh_down;

  if (!QueuedDrawArea2FullScreen && w && (screen = (Tscreen)w->Parent) && IS_SCREEN(screen)) {

    ylimit = screen->Up;
    sh_up = (ldat)w->Up - screen->YLogic + (ldat)ylimit;
    sh_left = (ldat)w->Left - screen->XLogic;
    sh_right = sh_left + (ldat)w->XWidth - (ldat)1;
    sh_down = sh_up + (w->Attr & WINDOW_ROLLED_UP ? 0 : (ldat)w->YWidth - 1);

    DrawAreaShadeWindow(screen, w, x1, y1, x2, y2, sh_left, sh_up, sh_right, sh_down, internal);
  }
}

void DrawAreaWindow(Twindow w) {
  Sdraw d;
  byte Dvalid = tfalse;
  if (!QueuedDrawArea2FullScreen && w && w->Parent && IS_SCREEN(w->Parent)) {
    if ((Twidget)w == All->Screens.First->Widgets.First) {
      DrawUnobscuredWidget((Twidget)w, 0, 0, TW_MAXDAT, TW_MAXDAT, tfalse);

    } else if (d.Init((Twidget)w, 0, 0, TW_MAXDAT, TW_MAXDAT, false)) {
      Dvalid = ttrue;
      d.W = d.TopW = NULL;
      d.DrawArea();
    }
    if (All->SetUp->Flags & setup_shadows) {
      if (!Dvalid)
        d.Init((Twidget)w, 0, 0, TW_MAXDAT, TW_MAXDAT, false);
      DrawAreaShadeWindow((Tscreen)w->Parent, w, 0, 0, TW_MAXDAT, TW_MAXDAT, d.Left, d.Up, d.Rgt,
                          d.Dwn, tfalse);
    }
  }
}

void DrawFullWindow(Twindow w) {
  DrawAreaWidget(w);
}

void DrawPartialWidget(Twidget w, dat X1, dat Y1, dat X2, dat Y2) {
  Sdraw d;
  if (!QueuedDrawArea2FullScreen && w && d.Init(w, X1, Y1, X2, Y2, false)) {
    /*
     * DO NOT assume w is completely visible...
     * might be obscured by another Twindow! So reset d.TopW
     * Assume instead that who called us wants to draw the visible part of w,
     * not whatever covers it. So set d.W (used as onlyW)
     */
    d.TopW = NULL;
    d.W = w;
    d.DrawArea();
  }
}

void DrawLogicWidget(Twidget w, ldat X1, ldat Y1, ldat X2, ldat Y2) {
  ldat xl, yl;
  byte HasBorder;

  if (!QueuedDrawArea2FullScreen && w && !(w->Flags & WIDGETFL_NOTVISIBLE) &&
      (!IS_WINDOW(w) || !(((Twindow)w)->Attr & WINDOW_ROLLED_UP)) && X2 >= X1 && Y2 >= Y1) {
    xl = w->XLogic;
    yl = w->YLogic;

    HasBorder = IS_WINDOW(w) && !(((Twindow)w)->Flags & WINDOWFL_BORDERLESS);

    if (X2 >= xl && Y2 >= yl && Y1 < TW_MAXLDAT && X1 < xl + (ldat)w->XWidth - 2 * HasBorder &&
        Y1 < yl + (ldat)w->YWidth - 2 * HasBorder) {

      X1 = Max2(X1 - xl, 0);
      X1 = Min2(X1 + HasBorder, TW_MAXDAT);
      X2 = Min2(X2, xl + (ldat)w->XWidth - 1 - 2 * HasBorder);
      X2 = Max2(X2 - xl, 0);
      X2 = Min2(X2 + HasBorder, TW_MAXDAT);

      Y1 = Max2(Y1 - yl, 0);
      Y1 = Min2(Y1 + HasBorder, TW_MAXDAT);
      Y2 = Min2(Y2, yl + (ldat)w->YWidth - 1 - 2 * HasBorder);
      Y2 = Max2(Y2 - yl, 0);
      Y2 = Min2(Y2 + HasBorder, TW_MAXDAT);

      DrawPartialWidget(w, (dat)X1, (dat)Y1, (dat)X2, (dat)Y2);
    }
  }
}

void ReDrawRolledUpAreaWindow(Twindow w, bool shaded) {
  ldat sh_left, sh_up, sh_right, sh_down;
  byte shade, deltaXShade, deltaYShade;
  Tscreen screen;
  dat dwidth, dheight;
  dat ylimit;

  if (QueuedDrawArea2FullScreen || !w || (w->Flags & WINDOWFL_NOTVISIBLE) || !w->Parent ||
      !IS_SCREEN(w->Parent))
    return;

  screen = (Tscreen)w->Parent;

  shade = All->SetUp->Flags & setup_shadows;
  deltaXShade = shade ? All->SetUp->DeltaXShade : (byte)0;
  deltaYShade = shade ? All->SetUp->DeltaYShade : (byte)0;

  dwidth = All->DisplayWidth;
  dheight = All->DisplayHeight;
  ylimit = screen->Up;
  sh_up = (ldat)w->Up - screen->YLogic + (ldat)ylimit;
  sh_left = (ldat)w->Left - screen->XLogic;
  sh_right = sh_left + (ldat)w->XWidth - (ldat)1;
  sh_down = sh_up + (ldat)w->YWidth - (ldat)1;
  /*sh_down=sh_up+(w->Attr & WINDOW_ROLLED_UP ? 0 : (ldat)w->YWidth-(ldat)1);*/

  if (sh_left >= (ldat)dwidth || sh_up >= (ldat)dheight || sh_right < -(ldat)deltaXShade ||
      sh_down < (ldat)ylimit - (ldat)deltaYShade)
    return;

  sh_left = Max2((ldat)0, sh_left);
  sh_up = Max2((ldat)0, sh_up + 1);
  sh_right = Min2((ldat)dwidth - (ldat)1, sh_right + (ldat)deltaXShade);
  sh_down = Min2((ldat)dheight - (ldat)1, sh_down + (ldat)deltaYShade);

  DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, (dat)sh_left, (dat)sh_up, (dat)sh_right,
            (dat)sh_down, shaded);
}

void DrawMenuScreen(Tscreen screen, dat Xstart, dat Xend) {
  Tscreen fScreen;
  Tmenu Menu;
  Tmenuitem item;
  dat dwidth, dheight, i, j, x;
  tcell attr = 0;
  trune font;
  tcolor color;
  byte select, State, MenuInfo;

  if (QueuedDrawArea2FullScreen || !screen || !screen->All || Xstart > Xend)
    return;

  j = screen->Up;
  dwidth = All->DisplayWidth;
  dheight = All->DisplayHeight;

  if (j < 0 || j >= dheight || Xstart >= dwidth || Xstart > Xend)
    return;

  for (fScreen = All->Screens.First; fScreen && fScreen != screen;
       fScreen = fScreen->NextScreen()) {
    if (fScreen->Up <= j) {
      return;
    }
  }
  if (fScreen != screen) {
    return;
  }
  State = All->State & state_any;
  Menu = screen->FindMenu();

  MenuInfo = State != state_menu && (All->SetUp->Flags & setup_menu_info);

  Xstart = Max2(Xstart, 0);
  Xend = Min2(Xend, dwidth - 1);

  for (i = Xstart; i <= Xend; i++) {
    if (i + 2 >= dwidth) {
      color = State == state_screen ? Menu->ColSelShtCut : Menu->ColShtCut;
      if ((screen->Flags & (SCREENFL_BACK_SELECT | SCREENFL_BACK_PRESSED)) ==
          (SCREENFL_BACK_SELECT | SCREENFL_BACK_PRESSED)) {
        color = TCOL(TCOLBG(color), TCOLFG(color));
      }
      font = Screen_Back[2 - (dwidth - i)];
    } else if (dwidth - i <= (dat)3 + lenTWDisplay) {
      color = State == state_screen ? Menu->ColSelShtCut : Menu->ColShtCut;
      font = TWDisplay[3 + lenTWDisplay - (dwidth - i)];
      if (!font)
        font = ' ';
    } else if (dwidth - i > 9 && (uldat)(dwidth - i) <= 9 + All->BuiltinRow->Len) {
      color = State == state_screen ? Menu->ColSelect : Menu->ColItem;
      font = All->BuiltinRow->Text[9 + All->BuiltinRow->Len - (dwidth - i)];
      if (!font)
        font = ' ';
    } else if (MenuInfo && FindInfo(Menu, i)) {
      select = State == state_screen;
      FindFontInfo(Menu, i, select, &attr);
      font = TRUNE(attr);
      color = TCOLOR(attr);
    } else {
      if (!MenuInfo) {
        item = Menu->FindItem(i);

        if (item) {
          /* check if item is from All->CommonMenu */
          if ((Tmenu)item->Parent == All->CommonMenu && Menu->Items.Last)
            x = Menu->Items.Last->Left + Menu->Items.Last->Len;
          else
            x = 0;

          select = State == state_screen ||
                   (State == state_menu && ((Tmenu)item->Parent)->SelectI == item);
          /*
           * CHEAT: item may be in CommonMenu, not in Menu...
           * steal item color from Menu.
           */
          FindFontMenuItem(Menu, item, i - x, select, &attr);
          font = TRUNE(attr);
          color = TCOLOR(attr);
        }
      }
      if (MenuInfo || !item) {
        color = State == state_screen ? Menu->ColSelect : Menu->ColItem;
        font = ' ';
      }
    }
    if (screen != All->Screens.First) {
      color = Menu->ColDisabled;
    }
    Video[i + j * (ldat)dwidth] = TCELL(color, font);
  }
  DirtyVideo(Xstart, j, Xend, j);
}

void DrawScreen(Tscreen screen) {
  Tscreen first_screen;
  if (!QueuedDrawArea2FullScreen) {
    first_screen = screen == All->Screens.First ? screen : (Tscreen)0;
    DrawArea2(first_screen, (Twidget)0, (Twidget)0, 0, (dat)screen->Up, TW_MAXDAT, TW_MAXDAT,
              tfalse);
  }
}

void ClearHilight(Twindow w) {
  if (w->State & WINDOW_DO_SEL) {
    w->State &= ~WINDOW_DO_SEL;
    if (w->YendSel > w->YstSel)
      DrawLogicWidget((Twidget)w, w->XLogic, w->YstSel, w->XLogic + w->XWidth, w->YendSel);
    else
      DrawLogicWidget((Twidget)w, w->XstSel, w->YstSel, w->XendSel, w->YstSel);
  }
}

void StartHilight(Twindow w, ldat XSel, ldat YSel) {
  if (w) {
    ClearHilight(w);
    w->State &= ~WINDOW_ANYSEL;
    w->State |= WINDOW_FWDSEL;
    w->XstSel = w->XendSel = XSel;
    w->YstSel = w->YendSel = YSel;
  }
}

void ExtendHilight(Twindow w, ldat XSel, ldat YSel) {
  ldat oX, oY;
  uldat oState;

  if (!w)
    return;

  if (!(w->State & WINDOW_DO_SEL)) {
    if (w->State & WINDOW_ANYSEL) {
      if (w->State & WINDOW_FWDSEL)
        StartHilight(w, w->XstSel, w->YstSel);
      else if (w->State & WINDOW_REVSEL)
        StartHilight(w, w->XendSel, w->YendSel);
      w->State |= WINDOW_DO_SEL;
      DrawLogicWidget((Twidget)w, w->XstSel, w->YstSel, w->XendSel, w->YstSel);
    } else
      return;
  }

  if (w->State & WINDOW_FWDSEL) {
    oX = w->XendSel;
    oY = w->YendSel;
  } else {
    oX = w->XstSel;
    oY = w->YstSel;
  }
  oState = w->State;

  if ((w->State & WINDOW_FWDSEL) && (YSel < w->YstSel || (YSel == w->YstSel && XSel < w->XstSel))) {

    w->State ^= WINDOW_ANYSEL;
    w->XendSel = w->XstSel;
    w->YendSel = w->YstSel;
  } else if ((w->State & WINDOW_REVSEL) &&
             (YSel > w->YendSel || (YSel == w->YendSel && XSel > w->XendSel))) {

    w->State ^= WINDOW_ANYSEL;
    w->XstSel = w->XendSel;
    w->YstSel = w->YendSel;
  }

  if (w->State & WINDOW_FWDSEL) {
    w->XendSel = XSel;
    w->YendSel = YSel;
  } else {
    w->XstSel = XSel;
    w->YstSel = YSel;
  }

  /*
  printf("%c%c %d %d %d %d\n", (w->State & WINDOW_DO_SEL) ? '*' : ' ',
         (w->State & WINDOW_FWDSEL) ? '+' : '-',
         w->XstSel, w->YstSel, w->XendSel, w->YendSel);
  */

  if (YSel == oY && ((oState & WINDOW_ANYSEL) == (w->State & WINDOW_ANYSEL)))
    DrawLogicWidget((Twidget)w, Min2(XSel, oX), YSel, Max2(XSel, oX), YSel);
  else
    DrawLogicWidget((Twidget)w, w->XLogic, Min2(YSel, oY), w->XLogic + w->XWidth, Max2(YSel, oY));
}
