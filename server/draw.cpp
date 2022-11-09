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

inline tcolor DoShadowColor(tcolor Color, bool Fg, bool Bg) NOTHROW {
  return (Bg   ? (Color & TCOL(0, tmaxcol)) > TCOL(0, thigh | tblack) ? TCOL(0, thigh | tblack)
                                                                      : TCOL(0, tblack)
            : Fg ? Color & TCOL(0, twhite)
               : Color & TCOL(0, tmaxcol)) |
         (Fg ? (Color & TCOL(tmaxcol, 0)) > TCOL(thigh | tblack, 0) ? TCOL(twhite, 0)
                                                                    : TCOL(thigh | tblack, 0)
             : Color & TCOL(tmaxcol, 0));
}

/*
 * warning: DrawMenu() can cheat and give us a user Menu
 * while MenuItem is from All->CommonMenu
 */
static void FindFontMenuItem(Tmenu Menu, Tmenuitem MenuItem, dat i, bool select,
                             tcell *PtrAttr) NOTHROW {
  tcolor Color;
  byte ShortCutFound;

  if (Menu && MenuItem && i >= MenuItem->Left && i < (ldat)(MenuItem->Left + MenuItem->Len)) {
    ShortCutFound = i == MenuItem->Left + MenuItem->ShortCut;
    if (MenuItem->Flags & ROW_ACTIVE) {
      if (ShortCutFound) {
        if (select)
          Color = Menu->ColSelShtCut;
        else
          Color = Menu->ColShtCut;
      } else if (select)
        Color = Menu->ColSelect;
      else
        Color = Menu->ColItem;
    } else if (select)
      Color = Menu->ColSelectDisabled;
    else
      Color = Menu->ColDisabled;

    *PtrAttr = TCELL(Color, MenuItem->Text[i - MenuItem->Left]);
  }
}

static void FindFontInfo(Tmenu Menu, dat i, bool select, tcell *PtrAttr) NOTHROW {
  Trow Info;
  tcolor Color;

  if (Menu && (Info = Menu->Info) && i >= 0 && (uldat)i < Info->Len) {
    if (select)
      Color = Info->Flags & ROW_ACTIVE ? Menu->ColSelect : Menu->ColSelectDisabled;
    else if (!(Info->Flags & ROW_ACTIVE))
      Color = Menu->ColDisabled;
    else if (Menu->FlagDefColInfo || !Info->ColText)
      Color = Menu->ColItem;
    else
      Color = Info->ColText[i];

    *PtrAttr = TCELL(Color, Info->Text[i]);
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

          Video[X + Y] = TCELL(col, TRUNEEXTRA(attr));
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
      attr = TCELL(col, TRUNEEXTRA(attr));
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

  if (!w || !d)
    return false;

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

    if (w->Flags & WIDGETFL_NOTVISIBLE)
      return false;

    HasTopBar = HasBorder = 0;

    if (!IS_WINDOW(w) || !(((Twindow)w)->Attr & WINDOW_ROLLED_UP))
      height = w->YWidth;
    else
      height = 1;

    if (cycle++) {
      xl = w->XLogic;
      yl = w->YLogic;

      if (IS_SCREEN(w))
        HasTopBar = 1;
      else if (IS_WINDOW(w) && !(((Twindow)w)->Flags & WINDOWFL_BORDERLESS))
        /* count border thickness */
        HasBorder = 1;
    } else
      xl = yl = 0;

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

  d->Screen = (Tscreen)c;
  d->TopW = cc;
  if (d->W == cc)
    d->W = NULL;

  if (c && IS_SCREEN(c))
    return d->X1 <= d->X2 && d->Y1 <= d->Y2;

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

  for (w = FirstW; w; w = w->Next) {

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

void DrawSelfWidget(Sdraw *d) {
  Twidget w = d->TopW;

  if (QueuedDrawArea2FullScreen || (w->Flags & WIDGETFL_NOTVISIBLE))
    return;

  if (w_USE(w, USEEXPOSE)) {
    const char *Text = NULL;
    const trune *TRune = NULL;
    const tcell *TCell = NULL;
    ldat Left, up, _X1, _X2, _Y1, _Y2;
    ldat Pitch, dwidth, i, j, v; /* (ldat) to avoid multiplication overflows */
    dat X1, X2, Y1, Y2, dX, dY;
    tcell h;
    tcolor Color;
    bool shaded;

    switch (w->USE.E.Flags) {
    case WIDGET_USEEXPOSE_TEXT:
      Text = w->USE.E.E.Text;
      break;
    case WIDGET_USEEXPOSE_TRUNE:
      TRune = w->USE.E.E.TRune;
      break;
    case WIDGET_USEEXPOSE_TCELL:
      TCell = w->USE.E.E.TCell;
      break;
    default:
      break;
    }
    Left = d->Left;
    up = d->Up;
    X1 = d->X1;
    Y1 = d->Y1;
    X2 = d->X2;
    Y2 = d->Y2;
    shaded = d->Shaded;
    dwidth = d->DWidth;

#if 0
        /*
         * this would suppress EXPOSE messages during resize (good)
         * but also clears the whole Twidget during non-top resize (bad)
         */
        if ((All->State & state_any) == state_resize && (Twidget)All->FirstScreen->ClickWindow == w) {
            /* user is interactively resizing this Twindow... pad with spaces */
            FillVideo(X1, Y1, X2, Y2, w->USE_Fill);
            return;
        }
#endif

    if (Text || TRune || TCell) {
      Pitch = w->USE.E.Pitch;

      _X1 = Max2(w->USE.E.X1 + Left, X1);
      _X2 = Min2(w->USE.E.X2 + Left, X2);
      _Y1 = Max2(w->USE.E.Y1 + up, Y1);
      _Y2 = Min2(w->USE.E.Y2 + up, Y2);

      dX = _X1 - w->USE.E.X1 - Left;
      dY = _Y1 - w->USE.E.Y1 - up;

      h = w->USE_Fill;
      if (_X1 > _X2 || _Y1 > _Y2) {
        /* no valid ->USE.E, fill with spaces */
        FillVideo(X1, Y1, X2, Y2, h);
        return;
      }

      DirtyVideo(X1, Y1, X2, Y2);

      /*
       * check if the ->USE.E is smaller than the Twidget size...
       * pad with SPACEs as needed
       */
      if (Y1 < _Y1) {
        FillVideo(X1, Y1, X2, _Y1 - 1, h);
        Y1 = _Y1;
      }
      if (Y2 > _Y2) {
        FillVideo(X1, _Y2 + 1, X2, Y2, h);
        Y2 = _Y2;
      }
      if (X1 < _X1) {
        FillVideo(X1, Y1, _X1 - 1, Y2, h);
        X1 = _X1;
      }
      if (X2 > _X2) {
        FillVideo(_X2 + 1, Y1, X2, Y2, h);
        X2 = _X2;
      }

      if (Text) {
        Color = TCOLOR(w->USE_Fill);
        if (shaded)
          Color = DoShadowColor(Color, shaded, shaded);
        Text += dY * Pitch;
        for (j = Y1; j <= Y2; j++) {
          Text += dX;
          for (i = X1, v = 0; i <= X2; i++, v++)
            Video[i + j * (ldat)dwidth] = TCELL(Color, Text[v]);
          Text += Pitch - dX;
        }
      } else if (TRune) {
        Color = TCOLOR(w->USE_Fill);
        if (shaded)
          Color = DoShadowColor(Color, shaded, shaded);
        TRune += dY * Pitch;
        for (j = Y1; j <= Y2; j++) {
          TRune += dX;
          for (i = X1, v = 0; i <= X2; i++, v++)
            Video[i + j * (ldat)dwidth] = TCELL(Color, TRune[v]);
          TRune += Pitch - dX;
        }
      } else if (TCell && !shaded) {
        TCell += dX + dY * Pitch;
        for (j = Y1; j <= Y2; j++) {
          CopyMem(TCell, &Video[X1 + j * (ldat)dwidth], (uldat)sizeof(tcell) * (X2 - X1 + 1));
          TCell += Pitch;
        }
      } else if (TCell) {
        /* shaded == true */
        TCell += dY * Pitch;
        for (j = Y1; j <= Y2; j++) {
          TCell += dX;
          for (i = X1, v = 0; i <= X2; i++, v++) {
            Color = DoShadowColor(TCOLOR(TCell[v]), shaded, shaded);
            Video[i + j * (ldat)dwidth] = TCELL(Color, 0) | TCELL_FONTMASK(TCell[v]);
          }
          TCell += Pitch - dX;
        }
      }
    } else {
      /* ask the client to draw */
      Tmsg msg;
      event_widget *EventW;

      if ((msg = New(msg)(msg_widget_change, 0))) {
        EventW = &msg->Event.EventWidget;
        EventW->W = w;
        EventW->Code = MSG_WIDGET_EXPOSE;
        EventW->Flags = shaded ? MSG_WIDGETFL_SHADED : 0;
        EventW->XWidth = X2 - X1 + 1;
        EventW->YWidth = Y2 - Y1 + 1;
        EventW->X = X1 - Left;
        EventW->Y = Y1 - up;
        SendMsg(w->Owner, msg);
      }
    }
  } else
    FillVideo(d->X1, d->Y1, d->X2, d->Y2, w->USE_Fill);
}

void DrawSelfGadget(Sdraw *d) {
  Tgadget g = (Tgadget)d->TopW;

  if (QueuedDrawArea2FullScreen || (g->Flags & WIDGETFL_NOTVISIBLE))
    return;

  if (!G_USE(g, USETEXT)) {
    g->widget_fn()->DrawSelf(d);
    return;
  }

  {
    ldat width, dwidth, Offset; /* (ldat) to avoid multiplication overflows */
    dat i, i_min, i_max, j, j_min, j_max;
    bool select, disabled, absent;
    trune Font, *Text, **GadgetText;
    tcolor *ColText, **GadgetColor;
    tcolor Color;

    select = !!(g->Flags & GADGETFL_PRESSED);
    disabled = !!(g->Flags & GADGETFL_DISABLED);
    absent = !!(g->Flags & GADGETFL_TEXT_DEFCOL);

    i_min = d->X1 - d->Left;
    i_max = d->X2 - d->Left;

    j_min = d->Y1 - d->Up;
    j_max = d->Y2 - d->Up;

    width = g->XWidth;
    dwidth = d->DWidth;

    Offset = d->Left + d->Up * (ldat)dwidth;

    GadgetText = g->USE.T.Text;
    GadgetColor = g->USE.T.Color;

    Text = select                      ? disabled && GadgetText[3] ? GadgetText[3]
                                         : GadgetText[1]           ? GadgetText[1]
                                                                   : GadgetText[0]
                                : disabled && GadgetText[2] ? GadgetText[2]
                                       : GadgetText[0];

    if (!Text)
      Font = ' ';

    ColText = absent                       ? NULL
              : select                     ? disabled && GadgetColor[3] ? GadgetColor[3]
                                             : GadgetColor[1]           ? GadgetColor[1]
                                                                        : GadgetColor[0]
                                  : disabled && GadgetColor[2] ? GadgetColor[2]
                                           : GadgetColor[0];

    if (!ColText) {
      absent = true;
      Color = select     ? disabled ? g->ColSelectDisabled : g->ColSelect
              : disabled ? g->ColDisabled
                         : g->ColText;
      Color = DoShadowColor(Color, d->Shaded, d->Shaded);
    }

    for (j = j_min; j <= j_max; j++) {
      for (i = i_min; i <= i_max; i++) {
        if (Text)
          Font = Text[i + j * width];
        if (!absent)
          Color = DoShadowColor(ColText[i + j * width], d->Shaded, d->Shaded);
        Video[i + j * (ldat)dwidth + Offset] = TCELL(Color, Font);
      }
    }
    DirtyVideo(d->X1, d->Y1, d->X2, d->Y2);
  }
}

static void DrawSelfBorder(Twindow w, ldat Left, ldat up, ldat rgt, ldat dwn, dat X1, dat Y1,
                           dat X2, dat Y2, byte Border, bool winActive, bool shaded) {

  ldat i, j, u, v, dwidth = All->DisplayWidth; /* (ldat) to avoid multiplication overflows */
  tcell attr;
  tcolor Color;

  if (QueuedDrawArea2FullScreen || (w->Flags & WIDGETFL_NOTVISIBLE))
    return;

  if ((ldat)Y1 == up) {
    j = Y1 * dwidth;
    for (i = X1, u = i - Left; i <= X2; i++, u++) {
      w->FindBorder(u, 0, Border, &attr);
      Color = DoShadowColor(TCOLOR(attr), shaded || !winActive, shaded);
      Video[i + j] = TCELL(Color, 0) | (attr & ~TCELL(TW_MAXWCOL, 0));
    }
    DirtyVideo(X1, Y1, X2, Y1);
    Y1++;
    if (Y1 > Y2)
      return;
  }

  if ((ldat)Y2 == dwn) {
    v = (ldat)Y2 - up;
    j = Y2 * dwidth;
    for (i = X1, u = i - Left; i <= X2; i++, u++) {
      w->FindBorder(u, v, Border, &attr);
      Color = DoShadowColor(TCOLOR(attr), shaded || !winActive, shaded);
      Video[i + j] = TCELL(Color, 0) | (attr & ~TCELL(TW_MAXWCOL, 0));
    }
    DirtyVideo(X1, Y2, X2, Y2);
    Y2--;
    if (Y1 > Y2)
      return;
  }

  if ((ldat)X1 == Left) {
    for (j = Y1, v = j - up; j <= Y2; j++, v++) {
      w->FindBorder(0, v, Border, &attr);
      Color = DoShadowColor(TCOLOR(attr), shaded || !winActive, shaded);
      Video[X1 + j * dwidth] = TCELL(Color, 0) | (attr & ~TCELL(TW_MAXWCOL, 0));
    }
    DirtyVideo(X1, Y1, X1, Y2);
    X1++;
    if (X1 > X2)
      return;
  }

  if ((ldat)X2 == rgt) {
    u = (ldat)X2 - Left;
    for (j = Y1, v = j - up; j <= Y2; j++, v++) {
      w->FindBorder(u, v, Border, &attr);
      Color = DoShadowColor(TCOLOR(attr), shaded || !winActive, shaded);
      Video[X2 + j * dwidth] = TCELL(Color, 0) | (attr & ~TCELL(TW_MAXWCOL, 0));
    }
    DirtyVideo(X2, Y1, X2, Y2);
    X2--;
    if (X1 > X2)
      return;
  }
}

void DrawSelfWindow(Sdraw *d) {
  Twindow w = (Twindow)d->TopW;

  if (QueuedDrawArea2FullScreen || (w->Flags & WIDGETFL_NOTVISIBLE))
    return;

  if (!W_USE(w, USEROWS) && !W_USE(w, USECONTENTS)) {
    w->widget_fn()->DrawSelf(d);
    return;
  }

  {
    const tcell *Contents, *CurrCont;
    const trune *TRune;
    tcolor *ColText;
    ldat Left, up, rgt;
    ldat dwidth, i, j, u, v; /* (ldat) to avoid multiplication overflows */
    ldat row, PosInRow;
    trune Font;
    Trow CurrRow;
    dat X1, Y1, X2, Y2;
    bool shaded, absent, select, rowDisabled;
    tcolor Color;

    X1 = d->X1;
    Y1 = d->Y1;
    X2 = d->X2;
    Y2 = d->Y2;
    Left = d->Left;
    up = d->Up;
    rgt = d->Rgt;
    shaded = d->Shaded;
    dwidth = d->DWidth;

    /* not here... already done in Sdraw.Draw() */
    /*
     * Left -= w->XLogic; rgt -= w->XLogic;
     * up   -= w->YLogic; dwn -= w->YLogic;
     */

    if (W_USE(w, USECONTENTS) && (Contents = w->USE.C.Contents)) {
      /*
       * For xterm-like windows, Contents is a buffer of (x=WLogic) * (y=HLogic)
       * tcell:screen, in the same format as Video: TCELL(Color, Ascii).
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
        FillVideo(X1, Ynew, X2, Y2, TCELL(w->ColText, ' '));
        Y2 = Ynew - 1;
      }
      if (X2 - Left >= w->WLogic) {
        /* the ->Contents buffer is smaller than the Twindow size... pad with SPACEs */
        dat Xnew = Left + w->WLogic;
        FillVideo(Xnew, Y1, X2, Y2, TCELL(w->ColText, ' '));
        X2 = Xnew - 1;
      }
      if (X1 <= X2 && Y1 <= Y2) {
        row = Y1 - up + w->USE.C.HSplit; /* row number in Contents */
        if (row >= w->HLogic)
          row -= w->HLogic;
        CurrCont = Contents + row * w->WLogic;

        row = w->HLogic - 1 - row; /* rows still readable */

        if (!shaded) {
          for (j = Y1, u = Y1 - up; j <= Y2; j++, u++, row--) {
            if (!(w->State & WINDOW_DO_SEL) || u < w->YstSel || u > w->YendSel) {

              CopyMem(CurrCont + X1 - Left, &Video[X1 + j * (ldat)dwidth],
                      sizeof(tcell) * (X2 - X1 + 1));

            } else {
              for (i = X1, v = X1 - Left; i <= X2; i++, v++) {
                select = (w->State & WINDOW_DO_SEL) &&
                         ((u > w->YstSel || (u == w->YstSel && v >= w->XstSel)) &&
                          (u < w->YendSel || (u == w->YendSel && v <= w->XendSel)));
                if (select)
                  Color = w->ColSelect;
                else
                  Color = TCOLOR(CurrCont[v]);

                Video[i + j * (ldat)dwidth] = TCELL(Color, TRUNEEXTRA(CurrCont[v]));
              }
            }
            CurrCont += w->WLogic;
            if (!row)
              CurrCont -= (row = w->HLogic) * w->WLogic;
          }
        } else {
          for (j = Y1, u = Y1 - up; j <= Y2; j++, u++, row--) {
            for (i = X1, v = X1 - Left; i <= X2; i++, v++) {

              select = (w->State & WINDOW_DO_SEL) &&
                       ((u > w->YstSel || (u == w->YstSel && v >= w->XstSel)) &&
                        (u < w->YendSel || (u == w->YendSel && v <= w->XendSel)));

              if (select)
                Color = w->ColSelect;
              else
                Color = TCOLOR(CurrCont[v]);
              Color = DoShadowColor(Color, shaded, shaded);

              Video[i + j * (ldat)dwidth] = TCELL(Color, TRUNEEXTRA(CurrCont[v]));
            }
            CurrCont += w->WLogic;
            if (!row)
              CurrCont -= (row = w->HLogic) * w->WLogic;
          }
        }
        DirtyVideo(X1, Y1, X2, Y2);
      }
    } else if (W_USE(w, USEROWS) && w->USE.R.FirstRow) {
      /*
       * editor-like windows. no length limit in the rows and in XLogic.
       */
      row = Y1 - up;

      if (row >= w->HLogic)
        CurrRow = (Trow)0;
      else if (w->USE.R.NumRowSplit && w->USE.R.RowSplit && row == w->USE.R.NumRowSplit)
        CurrRow = w->USE.R.RowSplit;
      else if (w->USE.R.NumRowSplit && w->USE.R.RowSplit && w->USE.R.RowSplit->Next &&
               row == w->USE.R.NumRowSplit + (ldat)1)
        CurrRow = w->USE.R.RowSplit->Next;
      else if (w->USE.R.NumRowOne && w->USE.R.RowOne && row == w->USE.R.NumRowOne)
        CurrRow = w->USE.R.RowOne;
      else if (row == (ldat)0)
        CurrRow = w->USE.R.FirstRow;
      else if (row + (ldat)1 == w->HLogic)
        CurrRow = w->USE.R.LastRow;
      else
        CurrRow = w->FindRow(row);

      for (j = Y1; j <= Y2; j++, row++) {

        rowDisabled = (w->Flags & WINDOWFL_DISABLED) ||
                      (CurrRow && !(CurrRow->Flags & (ROW_ACTIVE | ROW_IGNORE)));

        if (CurrRow) {
          TRune = CurrRow->Text;
          ColText = CurrRow->ColText;
        }

        PosInRow = X1 - Left;
        if (CurrRow && PosInRow >= 0 && (uldat)PosInRow >= CurrRow->Gap)
          PosInRow += CurrRow->LenGap;

        for (i = X1; i <= X2; i++, PosInRow++) {

          absent = !CurrRow || (PosInRow >= 0 && (uldat)PosInRow >= CurrRow->Len);

          if (CurrRow && IS_MENUITEM(CurrRow) && ((Tmenuitem)CurrRow)->Window && i == rgt) {
            Font = T_UTF_32_BLACK_RIGHT_POINTING_TRIANGLE;
          } else if (absent)
            Font = ' ';
          else
            Font = TRune[PosInRow];

          if (w->Flags & WINDOWFL_ROWS_SELCURRENT)
            select = row == w->CurY;
          else {
            select = (w->State & WINDOW_DO_SEL) &&
                     ((row > w->YstSel || (row == w->YstSel && PosInRow >= w->XstSel)) &&
                      (row < w->YendSel || (row == w->YendSel && PosInRow <= w->XendSel)));
          }
          if (CurrRow)
            select &= !(CurrRow->Flags & ROW_IGNORE);

          if (select)
            Color = rowDisabled ? w->ColSelectDisabled : w->ColSelect;
          else if (rowDisabled)
            Color = w->ColDisabled;
          else if ((w->Flags & WINDOWFL_ROWS_DEFCOL) || absent)
            Color = w->ColText;
          else
            Color = ColText[PosInRow];

          Color = DoShadowColor(Color, shaded, shaded);
          Video[i + j * (ldat)dwidth] = TCELL(Color, Font);
        }
        if (CurrRow) {
          w->USE.R.RowSplit = CurrRow;
          w->USE.R.NumRowSplit = row;
          CurrRow = CurrRow->Next;
        }
      }
      DirtyVideo(X1, Y1, X2, Y2);
    } else {
      /* either an unknown Twindow type or just one of the above, but empty */
      Color = w->ColText;
      Color = DoShadowColor(Color, shaded, shaded);
      FillVideo(X1, Y1, X2, Y2, TCELL(Color, ' '));
    }
  }
}

void DrawSelfScreen(Sdraw *d) {
  /* should never be called */
  log(ERROR) << "twin: DrawSelfScreen() called! This should not happen.\n";
}

static void _DrawWCtx_(Sdraw **FirstD, Twidget w, Twidget childNext, Twidget onlyChild, ldat left,
                       ldat up, ldat rgt, ldat dwn, dat X1, dat Y1, dat X2, dat Y2, bool noChildren,
                       bool borderDone, byte shaded, errnum *lError) {
  Sdraw *d;
  if (!QueuedDrawArea2FullScreen) {
    if ((d = (Sdraw *)AllocMem(sizeof(Sdraw)))) {
      d->TopW = w;
      d->W = childNext;
      d->OnlyW = onlyChild;
      d->Screen = (Tscreen)0;
      d->X1 = X1;
      d->Y1 = Y1;
      d->X2 = X2;
      d->Y2 = Y2;
      d->Left = left;
      d->Up = up;
      d->Rgt = rgt;
      d->Dwn = dwn;
      d->DWidth = All->DisplayWidth;
      d->DHeight = All->DisplayHeight;
      d->NoChildren = noChildren;
      d->BorderDone = borderDone;
      d->Shaded = shaded;

      d->Next = *FirstD;
      *FirstD = d;
    } else {
      DrawDesktop((Tscreen)0, X1, Y1, X2, Y2, shaded);
      *lError = NOMEMORY;
    }
  }
}

void Sdraw::Draw() {
  Sdraw *d = this;
  Sdraw *FirstD = d;
  Twidget w;
  Twidget onlyChild, childNext;
  Twindow Window;
  ldat left, up, rgt, dwn;
  dat x1, y1, x2, y2;
  ldat cL, cU, cR, cD;
  dat dwidth, dheight;
  errnum lError = SUCCESS;
  byte Border;
  bool shaded, winActive, noChildren;
  byte ChildFound = tfalse, FirstCycle = ttrue;

  if (QueuedDrawArea2FullScreen)
    return;

  do {
    w = d->TopW;
    childNext = d->W;
    onlyChild = d->OnlyW;
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
    noChildren = d->NoChildren;

    if (onlyChild == w) /* got it */
      onlyChild = NULL;

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

      if (IS_WINDOW(w) && !((Window = (Twindow)w)->Flags & WINDOWFL_BORDERLESS)) {

        if (!onlyChild) {
          winActive = Window == (Twindow)All->FirstScreen->FocusW() ||
                      Window == All->FirstScreen->MenuWindow;
          Border = (Window->Flags & WINDOWFL_MENU) || !winActive;
          DrawSelfBorder(Window, left, up, rgt, dwn, x1, y1, x2, y2, Border, winActive, shaded);
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
      if (!FirstCycle)
        FreeMem(d);
      else
        FirstCycle = tfalse;
      continue;
    }

    ChildFound = tfalse;
    if (noChildren)
      childNext = NULL;
    else {
      if (!childNext)
        childNext = w->FirstW;

      if (!noChildren && childNext) {
        while (childNext && !ChildFound) {
          cL = left + childNext->Left;
          cR = cL + childNext->XWidth - 1;
          cU = up + childNext->Up;
          cD = cU + childNext->YWidth - 1;
          if (cL > x2 || cU > y2 || cR < x1 || cD < y1 || (childNext->Flags & WIDGETFL_NOTVISIBLE))
            childNext = childNext->Next;
          else
            ChildFound = ttrue;
        }
      }
      if (!childNext)
        noChildren = true;
    }

    if (!ChildFound) {
      /* no children... draw this Twidget */
      w->DrawSelf(d);
    }

    if (!FirstCycle)
      FreeMem(d);
    else
      FirstCycle = tfalse;

    if (ChildFound) {

      /* recursively draw the area below childNext */
      if (y2 > cD) {
        _DrawWCtx_(&FirstD, w, childNext, onlyChild, left, up, rgt, dwn, x1, cD + 1, x2, y2,
                   noChildren, ttrue, shaded, &lError);
        y2 = cD;
      }

      /* recursively draw the area right of childNext */
      if (x2 > cR) {
        _DrawWCtx_(&FirstD, w, childNext, onlyChild, left, up, rgt, dwn, cR + 1, y1, x2, y2,
                   noChildren, ttrue, shaded, &lError);
        x2 = cR;
      }

      /* recursively draw the area left of childNext */
      if (x1 < cL) {
        _DrawWCtx_(&FirstD, w, childNext, onlyChild, left, up, rgt, dwn, x1, y1, cL - 1, y2,
                   noChildren, ttrue, shaded, &lError);
        x1 = cL;
      }

      /* recursively draw the area above childNext */
      if (y1 < cU) {
        _DrawWCtx_(&FirstD, w, childNext, onlyChild, left, up, rgt, dwn, x1, y1, x2, cU - 1,
                   noChildren, ttrue, shaded, &lError);
        y1 = cU;
      }

      /* ok, now actually draw childNext */
      if (x1 <= x2 && y1 <= y2) {
        _DrawWCtx_(&FirstD, childNext, NULL, onlyChild, cL, cU, cR, cD, x1, y1, x2, y2, tfalse,
                   tfalse, shaded, &lError);
      }
    }
  } while ((d = FirstD));

  if (lError)
    Error(lError);
}

/*
 * this ASSUMES the specified part of the Twidget is unobscured.
 * x1,y1,x2,y2 are absolute Tscreen coordinates.
 */
void DrawWidget(Twidget w, dat x1, dat y1, dat x2, dat y2, bool shaded) {
  Sdraw d;

  if (!QueuedDrawArea2FullScreen && w && d.InitAbsolute(w, x1, y1, x2, y2, shaded)) {
    d.TopW = w;
    d.W = d.OnlyW = NULL;
    d.Draw();
  }
}

/*
 * this DOES NOT assume that the specified part of the Twidget is unobscured
 */
/* partially replaces DrawAreaWindow() -- does not redraw shadow */
void DrawAreaWidget(Twidget w) {
  Sdraw d;

  if (!QueuedDrawArea2FullScreen && w && d.InitAbsolute(w, 0, 0, TW_MAXDAT, TW_MAXDAT, false)) {
    d.TopW = d.W = d.OnlyW = NULL;
    d.DrawArea();
  }
}

static void _DrawAreaCtx_(Sdraw **FirstD, Tscreen screen, Twidget w, Twidget onlyW, dat X1, dat Y1,
                          dat X2, dat Y2, bool shaded, errnum *lError) {
  Sdraw *d;
  if (!QueuedDrawArea2FullScreen) {
    if ((d = (Sdraw *)AllocMem(sizeof(Sdraw)))) {
      d->TopW = w;
      d->W = onlyW;
      d->Screen = screen;
      d->Left = d->X1 = X1;
      d->Up = d->Y1 = Y1;
      d->Rgt = d->X2 = X2;
      d->Dwn = d->Y2 = Y2;
      d->DWidth = All->DisplayWidth;
      d->DHeight = All->DisplayHeight;
      d->Shaded = shaded;

      d->Next = *FirstD;
      *FirstD = d;

    } else {
      DrawDesktop((Tscreen)0, X1, Y1, X2, Y2, shaded);
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
  Twidget P, w;
  if (!(P = All->FirstScreen->FocusW()))
    P = (Twidget)All->FirstScreen->MenuWindow;
  if (P)
    while ((w = P->SelectW))
      P = w;
  return WindowParent(P);
}

byte ContainsCursor(Twidget w) {
  Twidget c = (Twidget)FindCursorWindow();
  while (c) {
    if (c == w)
      return ttrue;
    c = c->Parent;
  }
  return tfalse;
}

void Sdraw::DrawArea() {
  Sdraw *d = this;
  Sdraw *FirstD = d;
  Tscreen FirstScreen, screen;
  Twidget w, onlyW, TopOnlyW, NextW;
  setup *SetUp;
  ldat dwidth, dheight, ylimit;
  ldat shLeft = 0, shUp = 0, shRgt = 0, shDwn = 0;
  /* position of Horizontal Shadow */
  ldat HS_X1, HS_X2, HS_Y1, S_Y2;
  /* position of Vertical Shadow */
  ldat VS_X1, VS_X2, VS_Y1;
  dat left, up, rgt, dwn;
  dat x1, y1, x2, y2;
  errnum lError = SUCCESS;
  byte WidgetFound, Shade;
  byte DeltaXShade, DeltaYShade;
  bool FirstCycle = true, shaded;

  if (QueuedDrawArea2FullScreen)
    return;

  SetUp = All->SetUp;

  do {
    w = d->TopW;
    onlyW = d->W;
    FirstScreen = d->Screen;
    x1 = d->X1;
    y1 = d->Y1;
    x2 = d->X2;
    y2 = d->Y2;
    dwidth = d->DWidth;
    dheight = d->DHeight;
    shaded = d->Shaded;

    FirstD = d->Next;

    if (!FirstCycle)
      FreeMem(d);
    else
      FirstCycle = tfalse;

    if (x1 > x2 || y1 > y2 || x1 >= dwidth || y1 >= dheight || x2 < 0 || y2 < 0)
      continue;

    x1 = Max2(x1, 0);
    y1 = Max2(y1, 0);
    x2 = Min2(x2, dwidth - 1);
    y2 = Min2(y2, dheight - 1);

    if (w) {
      if (IS_SCREEN(w->Parent))
        FirstScreen = (Tscreen)w->Parent;
      else
        continue;
    }
    if (!FirstScreen)
      FirstScreen = All->FirstScreen;

    /* calculate visible part of this Tscreen */
    for (screen = All->FirstScreen; screen && screen != FirstScreen; screen = screen->Next()) {
      ylimit = screen->Up;
      if (y2 >= ylimit)
        y2 = ylimit - 1;
    }
    if (y1 > y2)
      continue;

    screen = FirstScreen->Next();
    ylimit = FirstScreen->Up; /* FIXED +1 */

    if (!onlyW || (onlyW && ScreenParent(onlyW->Parent) != FirstScreen)) {
      if (y1 < ylimit) {
        if (screen)
          _DrawAreaCtx_(&FirstD, screen, w, onlyW, x1, y1, x2, Min2(y2, ylimit - 1), shaded,
                        &lError);
        else if (!onlyW)
          DrawDesktop((Tscreen)0, x1, y1, x2, y2, shaded);
      }
      if (onlyW)
        continue;
    }

    if (y2 < ylimit)
      continue;

    if (y1 < ylimit)
      y1 = ylimit;

    if (!w) {
      w = FirstScreen->FirstW;
      if (y1 == ylimit && !onlyW) {
        FirstScreen->DrawMenu(x1, x2);
        if (++y1 > y2)
          continue;
      }
    }

    Shade = (SetUp->Flags & setup_shadows) && !shaded;
    DeltaXShade = Shade ? SetUp->DeltaXShade : (byte)0;
    DeltaYShade = Shade ? SetUp->DeltaYShade : (byte)0;

    WidgetFound = tfalse;
    while (w && !WidgetFound) {
      if (w->Flags & WIDGETFL_NOTVISIBLE) {
        w = w->Next;
        continue;
      }
      shLeft = (ldat)w->Left + (ldat)FirstScreen->Left - FirstScreen->XLogic;
      shUp = (ldat)w->Up + (ldat)ylimit - FirstScreen->YLogic;
      shRgt = shLeft + (ldat)w->XWidth - 1;
      shDwn = shUp +
              (IS_WINDOW(w) && (((Twindow)w)->Attr & WINDOW_ROLLED_UP) ? 0 : (ldat)w->YWidth - 1);

      if (Shade && IS_WINDOW(w)) {
        /* only windows have shadows */
        HS_X1 = Max2(shLeft + (ldat)DeltaXShade, (ldat)x1);
        HS_X2 = Min2(shRgt, (ldat)x2);
        HS_Y1 = Max2(shUp + (ldat)DeltaYShade, shDwn + (ldat)1);
        HS_Y1 = Max2(HS_Y1, (ldat)y1);
        S_Y2 = Min2(shDwn + (ldat)DeltaYShade, (ldat)y2);

        VS_X1 = Max2(shLeft + (ldat)DeltaXShade, shRgt + (ldat)1);
        VS_X1 = Max2(VS_X1, (ldat)x1);
        VS_X2 = Min2(shRgt + (ldat)DeltaXShade, (ldat)x2);
        VS_Y1 = Max2(shUp + (ldat)DeltaYShade, (ldat)y1);
      }

      if (shLeft <= (ldat)x2 && shRgt >= (ldat)x1 && shUp <= (ldat)y2 && shDwn >= (ldat)y1)
        WidgetFound = ttrue;
      else if (Shade && IS_WINDOW(w) &&
               ((HS_X1 <= HS_X2 && HS_Y1 <= S_Y2) || (VS_X1 <= VS_X2 && VS_Y1 <= S_Y2)))
        WidgetFound = ttrue + ttrue;
      else
        w = w->Next;
    }

    /* again, only windows have shadows */
    if (w && !IS_WINDOW(w)) {
      Shade = tfalse;
      DeltaXShade = DeltaYShade = (byte)0;
    }

    if (!WidgetFound && !onlyW) {
      DrawDesktop(FirstScreen, x1, y1, x2, y2, shaded);
      continue;
    }

    TopOnlyW = onlyW ? NonScreenParent(onlyW) : NULL;

    if (WidgetFound == ttrue && (!onlyW || TopOnlyW == w)) {
      Sdraw *fd = NULL;
      _DrawWCtx_(&fd, w, NULL, onlyW, shLeft, shUp, shRgt, shDwn, Max2(x1, shLeft), Max2(y1, shUp),
                 Min2(x2, shRgt), Min2(y2, shDwn), tfalse, tfalse, shaded, &lError);
      if (fd) {
        fd->Draw();
        FreeMem(fd);
      }
    }

    /* Draw the Twindow's shadow : */

    NextW = w ? w->Next : w;

    if (WidgetFound && Shade && TopOnlyW != w) {

      if (HS_X1 <= HS_X2 && HS_Y1 <= S_Y2) {
        if (NextW)
          _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, (dat)HS_X1, (dat)HS_Y1, (dat)HS_X2,
                        (dat)S_Y2, ttrue, &lError);
        else
          DrawDesktop(FirstScreen, (dat)HS_X1, (dat)HS_Y1, (dat)HS_X2, (dat)S_Y2, ttrue);
      }
      if (VS_X1 <= VS_X2 && VS_Y1 <= S_Y2) {
        if (NextW)
          _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, (dat)VS_X1, (dat)VS_Y1, (dat)VS_X2,
                        (dat)S_Y2, ttrue, &lError);
        else
          DrawDesktop(FirstScreen, (dat)VS_X1, (dat)VS_Y1, (dat)VS_X2, (dat)S_Y2, ttrue);
      }
    }

    if (TopOnlyW == w || (TopOnlyW && !NextW))
      continue;

    /* Draw the visible area below the Twindow : */

    if (shDwn + (ldat)DeltaYShade < (ldat)y2) {
      dwn = (dat)shDwn + (dat)DeltaYShade;
      if (NextW)
        _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, x1, dwn + (dat)1, x2, y2, shaded, &lError);
      else
        DrawDesktop(FirstScreen, x1, dwn + (dat)1, x2, y2, shaded);
    } else
      dwn = y2;

    /* Draw the visible area right of the Twindow : */

    if (shRgt + (ldat)DeltaXShade < (ldat)x2) {
      rgt = (dat)shRgt + (dat)DeltaXShade;
      if (NextW)
        _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, rgt + (dat)1, y1, x2, dwn, shaded,
                      &lError);
      else
        DrawDesktop(FirstScreen, rgt + (dat)1, y1, x2, dwn, shaded);
    } else
      rgt = x2;

    /* Draw the visible area left of the Twindow : */

    if (shLeft > (ldat)x1) {
      left = (dat)shLeft;
      if (NextW)
        _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, x1, y1, left - (dat)1, dwn, shaded,
                      &lError);
      else
        DrawDesktop(FirstScreen, x1, y1, left - (dat)1, dwn, shaded);
    } else
      left = x1;

    /* Draw the visible area above the Twindow : */

    if (shUp > (ldat)y1) {
      up = (dat)shUp;
      if (NextW)
        _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, left, y1, rgt, up - (dat)1, shaded,
                      &lError);
      else
        DrawDesktop(FirstScreen, left, y1, rgt, up - (dat)1, shaded);
    } else
      up = y1;

    if (Shade) {

      ldat X_1, Y_1, X_2, Y_2;

      X_1 = Max2(shLeft, (ldat)x1);
      X_2 = Min2(shLeft + (ldat)DeltaXShade - (ldat)1, (ldat)x2);
      Y_1 = Max2(shDwn + (ldat)1, (ldat)y1);
      Y_2 = Min2(shDwn + (ldat)DeltaYShade, (ldat)y2);

      /* Draw the visible area below the Twindow, left of the shadow : */

      if (X_1 <= X_2 && Y_1 <= Y_2) {
        if (NextW)
          _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2,
                        shaded, &lError);
        else
          DrawDesktop(FirstScreen, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, shaded);
      }

      /* Draw the visible area right of the Twindow, above the shadow : */

      X_1 = Max2(shRgt + (ldat)1, (ldat)x1);
      X_2 = Min2(shRgt + (ldat)DeltaXShade, (ldat)x2);
      Y_1 = Max2(shUp, (ldat)y1);
      Y_2 = Min2(shUp + (ldat)DeltaYShade - (ldat)1, shDwn);
      Y_2 = Min2(Y_2, (ldat)y2);

      if (X_1 <= X_2 && Y_1 <= y2) {
        if (NextW)
          _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2,
                        shaded, &lError);
        else
          DrawDesktop(FirstScreen, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, shaded);
      }

      /* Draw the visible area between the Twindow and a floating shadow : */

      if (shLeft + (ldat)DeltaXShade > shRgt + (ldat)1 ||
          shUp + (ldat)DeltaYShade > shDwn + (ldat)1) {

        if (shUp + (ldat)DeltaYShade <= shDwn + (ldat)1) {
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
          X_1 = Max2(shRgt + (ldat)1, (ldat)x1);
          X_2 = Min2(shLeft + (ldat)DeltaXShade - (ldat)1, (ldat)x2);
          Y_1 = Max2(shUp + (ldat)DeltaYShade, (ldat)y1);
          Y_2 = Min2(shDwn, (ldat)y2);
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
          X_1 = Max2(shLeft + (ldat)DeltaXShade, (ldat)x1);
          X_2 = Min2(shRgt + (ldat)DeltaXShade, (ldat)x2);
          Y_1 = Max2(shDwn + (ldat)1, (ldat)y1);
          Y_2 = Min2(shUp + (ldat)DeltaYShade - (ldat)1, (ldat)y2);
        }

        if (X_1 <= X_2 && Y_1 <= y2) {
          if (NextW)
            _DrawAreaCtx_(&FirstD, (Tscreen)0, NextW, onlyW, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2,
                          shaded, &lError);
          else
            DrawDesktop(FirstScreen, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, shaded);
        }
      }
    }
  } while ((d = FirstD));

  if (lError)
    Error(lError);
}

void DrawArea2(Tscreen FirstScreen, Twidget w, Twidget onlyW, dat X1, dat Y1, dat X2, dat Y2,
               bool shaded) {
  Sdraw d;

  if (QueuedDrawArea2FullScreen)
    return;

  d.DWidth = All->DisplayWidth;
  d.DHeight = All->DisplayHeight;

  d.X1 = Max2(X1, 0);
  d.Y1 = Max2(Y1, 0);
  d.X2 = Min2(X2, d.DWidth - 1);
  d.Y2 = Min2(Y2, d.DHeight - 1);

  if (d.X1 <= d.X2 && d.Y1 <= d.Y2) {
    d.Next = NULL;
    d.W = onlyW;
    d.TopW = w;
    d.Screen = FirstScreen;
    d.Shaded = shaded;

    d.DrawArea();
  }
}

void DrawBorderWindow(Twindow w, byte Flags) {
  ldat shLeft, shUp, shRgt, shDwn;
  dat left, up, rgt, dwn;
  dat dwidth, dheight;
  dat ylimit;
  Twidget FirstW;
  Twidget Parent;
  Tscreen screen, FirstScreen;

  if (QueuedDrawArea2FullScreen || !w || (w->Flags & (WINDOWFL_BORDERLESS | WINDOWFL_NOTVISIBLE)) ||
      !(Parent = w->Parent) || !IS_SCREEN(Parent))
    return;

  screen = (Tscreen)Parent;
  dwidth = All->DisplayWidth;
  dheight = All->DisplayHeight;
  ylimit = screen->Up; /* FIXED +1 */

  shUp = (ldat)w->Up - screen->YLogic + (ldat)ylimit;
  shLeft = (ldat)w->Left - screen->XLogic;
  shRgt = shLeft + (ldat)w->XWidth - (ldat)1;
  shDwn = shUp + (w->Attr & WINDOW_ROLLED_UP ? 0 : (ldat)w->YWidth - (ldat)1);

  if (shLeft >= (ldat)dwidth || shUp >= (ldat)dheight || shRgt < (ldat)0 || shDwn <= (ldat)ylimit)
    return;

  FirstScreen = screen == All->FirstScreen ? screen : (Tscreen)0;
  FirstW = FirstScreen && (Twidget)w == screen->FirstW ? (Twidget)w : (Twidget)0;

  left = (dat)Max2(shLeft, (ldat)0);
  up = (dat)Max2(shUp, (ldat)ylimit + 1);
  rgt = (dat)Min2(shRgt, (ldat)dwidth - (ldat)1);
  dwn = (dat)Min2(shDwn, (ldat)dheight - (ldat)1);

  if ((Flags & BORDER_UP) && shUp >= ylimit)
    DrawArea2(FirstScreen, FirstW, (Twidget)w, left, up, rgt, up, tfalse);
  if (!(w->Flags & WINDOW_ROLLED_UP)) {
    if ((Flags & BORDER_LEFT) && shLeft >= 0)
      DrawArea2(FirstScreen, FirstW, (Twidget)w, left, up, left, dwn, tfalse);
    if ((Flags & BORDER_RIGHT) && shRgt < (ldat)dwidth)
      DrawArea2(FirstScreen, FirstW, (Twidget)w, rgt, up, rgt, dwn, tfalse);
    if ((Flags & BORDER_DOWN) && shDwn < (ldat)dheight)
      DrawArea2(FirstScreen, FirstW, (Twidget)w, left, dwn, rgt, dwn, tfalse);
  }
}

void DrawAreaShadeWindow(Tscreen screen, Twindow w, dat X1, dat Y1, dat X2, dat Y2, ldat shLeft,
                         ldat shUp, ldat shRgt, ldat shDwn, byte Internal) {
  dat dheight, dwidth, ylimit;
  /* position of Horizontal Shadow */
  ldat HS_X1, HS_X2, HS_Y1, S_Y2;
  /* position of Vertical Shadow */
  ldat VS_X1, VS_X2, VS_Y1;
  byte DeltaXShade, DeltaYShade;
  setup *SetUp;

  SetUp = All->SetUp;
  if (QueuedDrawArea2FullScreen || !w || (w->Flags & WINDOWFL_NOTVISIBLE) || !screen || X1 > X2 ||
      Y1 > Y2 || !(SetUp->Flags & setup_shadows))
    return;

  DeltaXShade = SetUp->DeltaXShade;
  DeltaYShade = SetUp->DeltaYShade;

  dwidth = All->DisplayWidth;
  dheight = All->DisplayHeight;
  ylimit = screen->Up;

  X1 = Max2(X1, 0);
  Y1 = Max2(Y1, ylimit + 1);
  X2 = Min2(X2, dwidth - 1);
  Y2 = Min2(Y2, dheight - 1);

  if (shLeft + (ldat)DeltaXShade > (ldat)X2 || shUp + (ldat)DeltaYShade > (ldat)Y2 ||
      shRgt + (ldat)DeltaXShade < (ldat)X1 || shDwn + (ldat)DeltaYShade < (ldat)Y1)
    return;

  w = (Twindow)w->Next;

  HS_X1 = Max2(shLeft + (ldat)DeltaXShade, (ldat)X1);
  HS_X2 = Min2(shRgt, (ldat)X2);
  HS_Y1 = Max2(shUp + (ldat)DeltaYShade, shDwn + (ldat)1);
  HS_Y1 = Max2(HS_Y1, (ldat)Y1);
  S_Y2 = Min2(shDwn + (ldat)DeltaYShade, (ldat)Y2);

  VS_X1 = Max2(shLeft + (ldat)DeltaXShade, shRgt + (ldat)1);
  VS_X1 = Max2(VS_X1, (ldat)X1);
  VS_X2 = Min2(shRgt + (ldat)DeltaXShade, (ldat)X2);
  VS_Y1 = Max2(shUp + (ldat)DeltaYShade, (ldat)Y1);

  if (HS_X1 <= HS_X2 && HS_Y1 <= S_Y2) {
    if (!Internal)
      DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, (dat)HS_X1, (dat)HS_Y1, (dat)HS_X2, (dat)S_Y2,
                tfalse);
    else if (w)
      DrawArea2((Tscreen)0, (Twidget)w, (Twidget)0, (dat)HS_X1, (dat)HS_Y1, (dat)HS_X2, (dat)S_Y2,
                ttrue);
    else
      DrawDesktop(screen, (dat)HS_X1, (dat)HS_Y1, (dat)HS_X2, (dat)S_Y2, ttrue);
  }

  if (VS_X1 <= VS_X2 && VS_Y1 <= S_Y2) {
    if (!Internal)
      DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, (dat)VS_X1, (dat)VS_Y1, (dat)VS_X2, (dat)S_Y2,
                tfalse);
    else if (w)
      DrawArea2((Tscreen)0, (Twidget)w, (Twidget)0, (dat)VS_X1, (dat)VS_Y1, (dat)VS_X2, (dat)S_Y2,
                ttrue);
    else
      DrawDesktop(screen, (dat)VS_X1, (dat)VS_Y1, (dat)VS_X2, (dat)S_Y2, ttrue);
  }
}

void DrawShadeWindow(Twindow w, dat X1, dat Y1, dat X2, dat Y2, byte Internal) {
  Tscreen screen;
  dat ylimit;
  ldat shLeft, shUp, shRgt, shDwn;

  if (!QueuedDrawArea2FullScreen && w && (screen = (Tscreen)w->Parent) && IS_SCREEN(screen)) {

    ylimit = screen->Up;
    shUp = (ldat)w->Up - screen->YLogic + (ldat)ylimit;
    shLeft = (ldat)w->Left - screen->XLogic;
    shRgt = shLeft + (ldat)w->XWidth - (ldat)1;
    shDwn = shUp + (w->Attr & WINDOW_ROLLED_UP ? 0 : (ldat)w->YWidth - 1);

    DrawAreaShadeWindow(screen, w, X1, Y1, X2, Y2, shLeft, shUp, shRgt, shDwn, Internal);
  }
}

/* replaces DrawAreaWindow() */
void DrawAreaWindow2(Twindow w) {
  Sdraw d;
  byte Dvalid = tfalse;
  if (!QueuedDrawArea2FullScreen && w && w->Parent && IS_SCREEN(w->Parent)) {
    if ((Twidget)w == All->FirstScreen->FirstW) {
      DrawWidget((Twidget)w, 0, 0, TW_MAXDAT, TW_MAXDAT, tfalse);

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

/* replaces DrawAbsoluteWindow() */
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

/* replaces DrawTextWindow() */
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
  ldat shLeft, shUp, shRgt, shDwn;
  byte Shade, DeltaXShade, DeltaYShade;
  Tscreen screen;
  dat dwidth, dheight;
  dat ylimit;

  if (QueuedDrawArea2FullScreen || !w || (w->Flags & WINDOWFL_NOTVISIBLE) || !w->Parent ||
      !IS_SCREEN(w->Parent))
    return;

  screen = (Tscreen)w->Parent;

  Shade = All->SetUp->Flags & setup_shadows;
  DeltaXShade = Shade ? All->SetUp->DeltaXShade : (byte)0;
  DeltaYShade = Shade ? All->SetUp->DeltaYShade : (byte)0;

  dwidth = All->DisplayWidth;
  dheight = All->DisplayHeight;
  ylimit = screen->Up;
  shUp = (ldat)w->Up - screen->YLogic + (ldat)ylimit;
  shLeft = (ldat)w->Left - screen->XLogic;
  shRgt = shLeft + (ldat)w->XWidth - (ldat)1;
  shDwn = shUp + (ldat)w->YWidth - (ldat)1;
  /*shDwn=shUp+(w->Attr & WINDOW_ROLLED_UP ? 0 : (ldat)w->YWidth-(ldat)1);*/

  if (shLeft >= (ldat)dwidth || shUp >= (ldat)dheight || shRgt < -(ldat)DeltaXShade ||
      shDwn < (ldat)ylimit - (ldat)DeltaYShade)
    return;

  shLeft = Max2((ldat)0, shLeft);
  shUp = Max2((ldat)0, shUp + 1);
  shRgt = Min2((ldat)dwidth - (ldat)1, shRgt + (ldat)DeltaXShade);
  shDwn = Min2((ldat)dheight - (ldat)1, shDwn + (ldat)DeltaYShade);

  DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn,
            shaded);
}

void DrawMenuScreen(Tscreen screen, dat Xstart, dat Xend) {
  Tscreen fScreen;
  Tmenu Menu;
  Tmenuitem item;
  dat dwidth, dheight, i, j, x;
  tcell attr;
  trune Font;
  tcolor Color;
  byte select, State, MenuInfo;

  if (QueuedDrawArea2FullScreen || !screen || !screen->All || Xstart > Xend)
    return;

  j = screen->Up;
  dwidth = All->DisplayWidth;
  dheight = All->DisplayHeight;

  if (j < 0 || j >= dheight || Xstart >= dwidth || Xstart > Xend)
    return;

  for (fScreen = All->FirstScreen; fScreen && fScreen != screen; fScreen = fScreen->Next()) {
    if (fScreen->Up <= j)
      return;
  }
  if (fScreen != screen)
    return;

  State = All->State & state_any;
  Menu = screen->FindMenu();

  MenuInfo = State != state_menu && (All->SetUp->Flags & setup_menu_info);

  Xstart = Max2(Xstart, 0);
  Xend = Min2(Xend, dwidth - 1);

  for (i = Xstart; i <= Xend; i++) {
    if (i + 2 >= dwidth) {
      Color = State == state_screen ? Menu->ColSelShtCut : Menu->ColShtCut;
      if ((screen->Flags & (SCREENFL_BACK_SELECT | SCREENFL_BACK_PRESSED)) ==
          (SCREENFL_BACK_SELECT | SCREENFL_BACK_PRESSED)) {
        Color = TCOL(TCOLBG(Color), TCOLFG(Color));
      }
      Font = Screen_Back[2 - (dwidth - i)];
    } else if (dwidth - i <= (dat)3 + lenTWDisplay) {
      Color = State == state_screen ? Menu->ColSelShtCut : Menu->ColShtCut;
      Font = TWDisplay[3 + lenTWDisplay - (dwidth - i)];
      if (!Font)
        Font = ' ';
    } else if (dwidth - i > 9 && (uldat)(dwidth - i) <= 9 + All->BuiltinRow->Len) {
      Color = State == state_screen ? Menu->ColSelect : Menu->ColItem;
      Font = All->BuiltinRow->Text[9 + All->BuiltinRow->Len - (dwidth - i)];
      if (!Font)
        Font = ' ';
    } else if (MenuInfo && FindInfo(Menu, i)) {
      select = State == state_screen;
      FindFontInfo(Menu, i, select, &attr);
      Font = TRUNE(attr);
      Color = TCOLOR(attr);
    } else {
      if (!MenuInfo) {
        item = Menu->FindItem(i);

        if (item) {
          /* check if item is from All->CommonMenu */
          if ((Tmenu)item->Parent == All->CommonMenu && Menu->LastI)
            x = Menu->LastI->Left + Menu->LastI->Len;
          else
            x = 0;

          select = State == state_screen ||
                   (State == state_menu && ((Tmenu)item->Parent)->SelectI == item);
          /*
           * CHEAT: item may be in CommonMenu, not in Menu...
           * steal item color from Menu.
           */
          FindFontMenuItem(Menu, item, i - x, select, &attr);
          Font = TRUNE(attr);
          Color = TCOLOR(attr);
        }
      }
      if (MenuInfo || !item) {
        Color = State == state_screen ? Menu->ColSelect : Menu->ColItem;
        Font = ' ';
      }
    }
    if (screen != All->FirstScreen)
      Color = Menu->ColDisabled;
    Video[i + j * (ldat)dwidth] = TCELL(Color, Font);
  }
  DirtyVideo(Xstart, j, Xend, j);
}

void DrawScreen(Tscreen screen) {
  Tscreen FirstScreen;
  if (!QueuedDrawArea2FullScreen) {
    FirstScreen = screen == All->FirstScreen ? screen : (Tscreen)0;
    DrawArea2(FirstScreen, (Twidget)0, (Twidget)0, 0, (dat)screen->Up, TW_MAXDAT, TW_MAXDAT,
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
