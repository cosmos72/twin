/*
 *  window.h  --  declare server class Swindow
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_SCREEN_H
#define TWIN_SCREEN_H

#include "obj/widget.h"

struct SscreenFn {
  /* Twidget */
  TobjFn Fn_Obj;
  Twidget (*KbdFocus)(Tscreen);
  /* Tscreen */
  TwidgetFn Fn_Widget;
  Tmenu (*FindMenu)(Tscreen);
  void (*BgImage)(Tscreen, dat BgWidth, dat BgHeight, const tcell *Bg);
  void (*DrawMenu)(Tscreen, dat Xstart, dat Xend);
  void (*ActivateMenu)(Tscreen, Tmenuitem, bool by_mouse);
  void (*DeActivateMenu)(Tscreen);
};

struct Sscreen : public Swidget {
  dat NameLen;
  char *Name;
  Twindow MenuWindow, ClickWindow;
  Tall All;
  HookData HookMap; /* allow hooks on children Map()/UnMap() inside this Tscreen */

private:
  Tscreen Init(dat namelen, const char *name, dat bgwidth, dat bgheight, const tcell *bg);

public:
  static Tscreen Create(dat namelen, const char *name, dat bgwidth, dat bgheight, const tcell *bg);
  static Tscreen CreateSimple(dat namelen, const char *name, tcell bg);

  /* Tobj */
  virtual void Delete() OVERRIDE;
  virtual void Remove() OVERRIDE;
  virtual void ChangeField(udat field, uldat clear_mask, uldat xor_mask) OVERRIDE;

  /* Twidget */
  virtual void DrawSelf(Sdraw *d) OVERRIDE; // defined in draw.cpp
  virtual void SetXY(dat x, dat y) OVERRIDE;
  virtual Twidget Focus() OVERRIDE;

  /* Tscreen */
  void Insert(Tall parent, Tscreen prev, Tscreen next);

  const TscreenFn fn() const {
    return (TscreenFn)Fn;
  }
  const TwidgetFn widget_fn() const {
    return ((TscreenFn)Fn)->Fn_Widget;
  }

  Twidget FocusW() const {
    return SelectW;
  }
  void FocusW(Twidget focusW) {
    SelectW = focusW;
  }

  Tscreen Prev() const {
    return (Tscreen)Swidget::Prev;
  }
  void Prev(Tscreen prev) {
    Swidget::Prev = prev;
  }

  Tscreen Next() const {
    return (Tscreen)Swidget::Next;
  }
  void Next(Tscreen next) {
    Swidget::Next = next;
  }

  Tmenu FindMenu() {
    return fn()->FindMenu(this);
  }
  static Tscreen Find(dat j);

  void BgImage(dat bgwidth, dat bgheight, const tcell *bg) {
    fn()->BgImage(this, bgwidth, bgheight, bg);
  }
  void DrawMenu(dat xstart, dat xend) {
    fn()->DrawMenu(this, xstart, xend);
  }
  void ActivateMenu(Tmenuitem item, byte bymouse) {
    fn()->ActivateMenu(this, item, bymouse);
  }
  void DeActivateMenu() {
    fn()->DeActivateMenu(this);
  }

protected:
  virtual void InsertWidget(Tobj parent, Twidget prev, Twidget next) OVERRIDE;
};

/* screen->Attr */
/* not used */

/* screen->Flags */
#define SCREENFL_USEBG 0x00                   /* it's the default */
#define SCREENFL_USEEXPOSE WIDGETFL_USEEXPOSE /* 0x02 */
#define SCREENFL_USEFILL WIDGETFL_USEFILL     /* 0x03 */
#define SCREENFL_USEANY WIDGETFL_USEANY       /* 0x07 */

#define S_USE(s, USExxx) (((s)->Flags & SCREENFL_USEANY) == CAT(SCREENFL_, USExxx))

#define SCREENFL_BACK_SELECT 0x0010
#define SCREENFL_BACK_PRESSED 0x0020
#define SCREENFL_NOTVISIBLE 0x8000

#endif /* TWIN_SCREEN_H */
