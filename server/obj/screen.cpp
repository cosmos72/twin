/*
 *  screen.cpp  --  define methods of server class Swidget
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/screen.h"

#include "algo.h"    // Max2()
#include "alloc.h"   // AllocMem0(), AllocMem(), CloneStrL(), CopyMem()
#include "builtin.h" // Builtin_MsgPort
#include "draw.h"    // DrawArea2()
#include "fn.h"      // Fn_Tscreen
#include "methods.h" // MoveFirst()
#include "obj/all.h" // All
#include "resize.h"  // ResizeFirstScreen()
#include "twin.h"    // IS_ALL(), IS_SCREEN()

#include <new>
#include <Tw/datasizes.h> // TW_MAXDAT

Tscreen Sscreen::Create(dat namelen, const char *name, dat bgwidth, dat bgheight, const tcell *bg) {
  Tscreen screen = NULL;
  if (bgwidth && bgheight) {
    void *addr = AllocMem0(sizeof(Sscreen));
    if (addr) {
      screen = new (addr) Sscreen();
      screen->Fn = (TwidgetFn)Fn_Tscreen;
      if (!screen->Init(namelen, name, bgwidth, bgheight, bg)) {
        screen->Delete();
        screen = NULL;
      }
    }
  }
  return screen;
}

Tscreen Sscreen::CreateSimple(dat namelen, const char *name, tcell bg) {
  return New(screen)(namelen, name, 1, 1, &bg);
}

Tscreen Sscreen::Init(dat namelen, const char *name, dat bgwidth, dat bgheight, const tcell *bg) {
  size_t size = (size_t)bgwidth * bgheight * sizeof(tcell);

  if (!size || !Swidget::Init(Builtin_MsgPort, TW_MAXDAT, TW_MAXDAT, 0, SCREENFL_USEBG, 0, 0, bg[0],
                              Tscreen_class_id)) {
    return NULL;
  }
  if (name && !(this->Name = CloneStrL(name, namelen))) {
    return NULL;
  }
  if (!(this->USE.B.Bg = (tcell *)AllocMem(size))) {
    return NULL;
  }
  this->NameLen = namelen;
  // this->MenuWindow = this->ClickWindow = NULL;
  // this->HookMap.clear();
  this->USE.B.BgWidth = bgwidth;
  this->USE.B.BgHeight = bgheight;
  CopyMem(bg, this->USE.B.Bg, size);
  this->All = NULL;
  return this;
}

void Sscreen::Delete() {
  while (Widgets.First) {
    Widgets.First->UnMap();
  }
  Remove();
  if (S_USE(this, USEBG) && USE.B.Bg) {
    FreeMem(USE.B.Bg);
    USE.B.Bg = NULL;
  }
  Swidget::Delete();
}

void Sscreen::InsertWidget(Tobj parent, Twidget prev, Twidget next) {
  if ((parent && IS_ALL(parent)) && (!prev || IS_SCREEN(prev)) && (!next || IS_SCREEN(next))) {
    Insert((Tall)parent, (Tscreen)prev, (Tscreen)next);
  }
}

void Sscreen::Insert(Tall parent, Tscreen prev, Tscreen next) {
  if (parent && !All) {
    parent->Screens.Insert(this, prev, next);
    All = parent;
  }
}

void Sscreen::Remove() {
  if (All) {
    All->Screens.Remove(this);
    All = (Tall)0;
  }
}

void Sscreen::ChangeField(udat field, uldat clear_mask, uldat xor_mask) {
  switch (field) {
  default:
    Swidget::ChangeField(field, clear_mask, xor_mask);
    break;
  }
}

void Sscreen::SetXY(dat x, dat y) {
  if (this == ::All->Screens.First) {
    y = Max2(y, -1);
    y = Min2(y, ::All->DisplayHeight - 1);
    ResizeFirstScreen(y - Up);
  }
}

Twidget Sscreen::Focus() {
  Tscreen old = ::All->Screens.First;
  if (old != this) {
    MoveFirst(Screens, ::All, this);
    DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, 0, Min2(old->Up, this->Up), //
              TW_MAXDAT, TW_MAXDAT, false);
    UpdateCursor();
  }
  return (Twidget)old;
}

Tscreen Sscreen::Find(dat j) {
  for (Tscreen screen = ::All->Screens.First; screen; screen = screen->NextScreen()) {
    if (j >= (dat)screen->Up) {
      return screen;
    }
  }
  return (Tscreen)0;
}

void Sscreen::BgImage(dat bgwidth, dat bgheight, const tcell *bg) {
  Tscreen screen = this;
  size_t size;

  if (screen && S_USE(screen, USEBG) && bg && (size = (size_t)bgwidth * bgheight * sizeof(tcell)) &&
      (screen->USE.B.Bg = (tcell *)ReAllocMem(screen->USE.B.Bg, size))) {

    screen->USE.B.BgWidth = bgwidth;
    screen->USE.B.BgHeight = bgheight;
    CopyMem(bg, screen->USE.B.Bg, size);
    DrawArea2((Tscreen)0, (Twidget)0, (Twidget)0, 0, screen->Up + 1, TW_MAXDAT, TW_MAXDAT, tfalse);
  }
}

Tmenu Sscreen::FindMenu() const {

  if (MenuWindow && IS_WINDOW(MenuWindow))
    /* menu activated from MenuWindow, return its Tmenu */
    return MenuWindow->Menu;

  /* no Twindow activated the Tmenu... either the Tmenu is inactive
   * or it is activated from the builtin Tmenu */

  if (FocusW() && IS_WINDOW(FocusW()) && ((Twindow)FocusW())->Menu != ::All->CommonMenu)
    /* Tmenu inactive... return the focus Twindow's one */
    return ((Twindow)FocusW())->Menu;

  /* last case: Tmenu activated from builtin Tmenu */
  return ::All->BuiltinMenu;
}

void Sscreen::DrawMenu(dat xstart, dat xend) {
  DrawMenuScreen(this, xstart, xend);
}

void Sscreen::ActivateMenu(Tmenuitem item, bool by_mouse) {
  if ((::All->State & state_any) != state_default) {
    return;
  } else if (this != ::All->Screens.First) {
    Focus();
  }
  SetMenuState(item, by_mouse);
}

void Sscreen::DeActivateMenu() {
  if (this == ::All->Screens.First && (::All->State & state_any) == state_menu) {
    CloseMenu();
  }
}
