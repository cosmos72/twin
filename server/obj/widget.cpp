/*
 *  widget.cpp  --  define methods of server class Swidget
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/widget.h"

#include "algo.h"    // Max2()
#include "alloc.h"   // AllocMem0()
#include "methods.h" // IncMouseMotionN(), DecMouseMotionN()
#include "extreg.h"  // Ext()
#include "resize.h"  // UpdateCursor()
#include "twin.h"    // IS_WIDGET()
#include "draw.h"    // DrawAreaWidget()

#include <Tw/Twstat_defs.h> // TWS_widget_*

#include <new>
#include <cstring> // memset()

Twidget Swidget::Create(Tmsgport owner, dat xwidth, dat ywidth, uldat attr, uldat flags, dat left,
                        dat up, tcell fill) {
  Twidget w = NULL;
  if (owner) {
    void *addr = AllocMem0(sizeof(Swidget));
    if (addr) {
      w = new (addr) Swidget();
      if (!w->Init(owner, xwidth, ywidth, attr, flags, left, up, fill, Twidget_class_id)) {
        w->Delete();
        w = NULL;
      }
    }
  }
  return w;
}

Twidget Swidget::Init(Tmsgport owner, dat xwidth, dat ywidth, uldat attr, uldat flags, dat left,
                      dat up, tcell fill, e_id class_id) {
  if (!owner || !Sobj::Init(class_id)) {
    return NULL;
  }
  Left = left;
  Up = up;
  XWidth = xwidth;
  YWidth = ywidth;
  Attr = attr;
  Flags = flags;
  USE_Fill = fill;
  /*
  if (w_USE(this, USEEXPOSE))
    memset(&USE.E, '\0', sizeof(USE.E));
  */
  Own(owner);
  return this;
}

void Swidget::Delete() {
  UnMap();
  if (Hook && WhereHook) {
    RemoveHook(Hook, WhereHook);
  }
  if (ShutDownHook) {
    ShutDownHook(this);
  }
  DisOwn();
  while (Widgets.First) {
    Widgets.First->UnMap();
  }
  Sobj::Delete();
}

void Swidget::InsertWidget(Tobj parent, Twidget prev, Twidget next) {
  if (parent && IS_WIDGET(parent)) {
    /*
     * don't check this->Parent here, because Raise() and Lower() call Swidget::Remove()
     * then calls Swidget::Insert() but Swidget::Remove() does not reset w->Parent
     */
    ((Twidget)parent)->Widgets.Insert(this, prev, next);
  }
}

void Swidget::Insert(Twidget parent, Twidget prev, Twidget next) {
  // 'this' may be a subclass of Swidget
  InsertWidget(parent, prev, next);
}

void Swidget::Remove() {
  if (Parent) {
    Parent->Widgets.Remove(this);
  }
}

void Swidget::ChangeField(udat field, uldat clear_mask, uldat xor_mask) {
  switch (field) {
  case TWS_widget_Left:
  case TWS_widget_Up:
  case TWS_widget_Width:
  case TWS_widget_Height:
    break;
#ifdef FIXME_TCELL
  case TWS_widget_USE_Fill: {
    uldat i = tcell((USE_Fill & ~clear_mask) ^ xor_mask);
    SetFill(i);
    break;
  }
#endif // 0
  case TWS_widget_XLogic:
  case TWS_widget_YLogic:
    break;
  default:
    Sobj::ChangeField(field, clear_mask, xor_mask);
    break;
  }
}

Tgadget Swidget::FindGadgetByCode(udat Code) {
  for (Twidget w = Widgets.First; w; w = w->Next) {
    if (IS_GADGET(w) && ((Tgadget)w)->Code == Code) {
      return (Tgadget)w;
    }
  }
  return (Tgadget)0;
}

void Swidget::SetXY(dat x, dat y) {
  Twidget parent = Parent, prev = NULL, next = NULL;
  if (parent) {
    prev = Prev;
    next = Next;
    Remove();
    DrawAreaWidget(this);
  }
  Left = x;
  Up = y;
  if (parent) {
    Insert(parent, prev, next);
    DrawAreaWidget(this);
  }
}

void Swidget::SetFill(tcell fill) {
  if (USE_Fill != fill) {
    USE_Fill = fill;
    if (Parent) {
      DrawAreaWidget(this);
    }
  }
}

Twidget Swidget::Focus() {
  return Focus(this);
}

Twidget Swidget::Focus(Twidget w) {
  Twidget old = KbdFocus(w);

  if (old != w && (!w || w->Parent == ::All->Screens.First)) {
    if (w && IS_WINDOW(w))
      DrawBorderWindow((Twindow)w, BORDER_ANY);
    if (old && IS_WINDOW(old))
      DrawBorderWindow((Twindow)old, BORDER_ANY);
    if ((w && IS_WINDOW(w)) || (old && IS_WINDOW(old))) {
      UpdateCursor();
      if (!w || !IS_WINDOW(w) || !(((Twindow)w)->Flags & WINDOWFL_MENU)) {
        ::All->Screens.First->DrawMenu(0, TW_MAXDAT);
      }
    }
  }
  return old;
}

void Swidget::UnFocus() {
  if (Parent == ::All->Screens.First && this == ::All->Screens.First->FocusW()) {
    if (IS_WINDOW(this)) {
      KbdFocus((Twidget)0);
      DrawBorderWindow((Twindow)this, BORDER_ANY);
      ((Tscreen)Parent)->DrawMenu(0, TW_MAXDAT);
      UpdateCursor();
    } else {
      ::All->Screens.First->FocusW((Twidget)0);
    }
  }
}

Twidget Swidget::KbdFocus() {
  return Fn_Twidget->KbdFocus(this);
}

Twidget Swidget::KbdFocus(Twidget w) {
  return Fn_Twidget->KbdFocus(w);
}

void Swidget::Map(Twidget parent) {
  Tmsg msg;
  Twidget w = this;

  if (!w || IS_SCREEN(w) || !parent || w->Parent || w->MapQueueMsg) {
    return;
  } else if (IS_SCREEN(parent)) {
    if (Ext(WM, MsgPort) && (msg = Smsg::Create(msg_map, 0))) {
      msg->Event.EventMap.W = w;
      msg->Event.EventMap.Code = 0;
      msg->Event.EventMap.Screen = (Tscreen)parent;
      w->MapQueueMsg = msg;
      SendMsg(Ext(WM, MsgPort), msg);
    } else
      w->MapTopReal((Tscreen)parent);
  } else if (IS_WIDGET(parent)) {
    if (w->Up == TW_MAXDAT) {
      w->Left = parent->XLogic;
      w->Up = parent->YLogic;
    }
    InsertFirst(Widgets, w, parent);
    w->Parent = parent;

    DrawAreaWidget(w);

    if (w->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
      IncMouseMotionN();

    if (w->MapUnMapHook)
      w->MapUnMapHook(w);
  }
}

void Swidget::MapTopReal(Tscreen screen) {
  Twidget w = this, oldw;

  if (IS_SCREEN(w)) {
    return;
  } else if (!screen || w->Parent || (IS_WINDOW(w) && !((Twindow)w)->Menu)) {
    return;
  }
  if (w->MapQueueMsg) {
    // let the upper layer do this:
    // w->MapQueueMsg->Delete();
    w->MapQueueMsg = (Tmsg)0;
  }
  if (w->Up == TW_MAXDAT) {
    w->Left = screen->XLogic;
    w->Up = Max2(screen->Up + 1, 0) + screen->YLogic;
  } else {
    w->Left += screen->XLogic;
    w->Up += screen->YLogic;
  }

  InsertFirst(Widgets, w, (Twidget)screen);
  w->Parent = (Twidget)screen;

  /* top-level widgets must be visible */
  w->Flags &= ~WINDOWFL_NOTVISIBLE;

  if (w->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS)) {
    IncMouseMotionN();
  }
  if (screen == All->Screens.First) {
    oldw = w->KbdFocus();
    if (oldw && IS_WINDOW(oldw))
      DrawBorderWindow((Twindow)oldw, BORDER_ANY);
    UpdateCursor();
  }
  if (IS_WINDOW(w)) {
    DrawAreaWindow((Twindow)w);
  } else {
    DrawAreaWidget(w);
  }
  if (!(w->Flags & WINDOWFL_MENU)) {
    screen->DrawMenu(0, TW_MAXDAT);
  }
  if (w->MapUnMapHook) {
    w->MapUnMapHook(w);
  }
  screen->HookMap();
}

void Swidget::UnMap() {
  Twidget w = this, parent;

  if (!w || IS_SCREEN(w)) {
    return;
  } else if ((parent = w->Parent)) {
    if (IS_SCREEN(parent)) {
      Tscreen screen = (Tscreen)parent;
      if (screen == All->Screens.First && w == screen->MenuWindow) {
        /*
         * ! DANGER !
         * Trying to UnMap() the Tmenu owner.
         * shutdown the Tmenu first!
         */
        CloseMenu();
      }

      if (w->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        DecMouseMotionN();

      if (screen->ClickWindow == (Twindow)w)
        screen->ClickWindow = NULL;

      Twindow next = NULL;
      const bool wasFocus = w == screen->FocusW();
      if (wasFocus) {
        if (w->Flags & WINDOWFL_MENU) {
          next = screen->MenuWindow;
        } else {
          if ((Twidget)w == screen->Widgets.First) {
            next = (Twindow)w->Next;
          } else {
            next = (Twindow)screen->Widgets.First;
          }
          while (next && !IS_WINDOW(next)) {
            next = (Twindow)next->Next;
          }
        }
      }

      w->Remove();
      if (IS_WINDOW(w))
        DrawAreaWindow((Twindow)w);
      else
        DrawAreaWidget(w);

      if (IS_SCREEN(parent)) {
        w->Left = 0;
        w->Up = TW_MAXDAT;
      }
      w->Parent = (Twidget)0;

      if (wasFocus) {
        if (screen == All->Screens.First) {
          /*
           * in case the user was dragging this Twindow...
           */
          if ((All->State & state_any) < state_menu) {
            All->State &= ~state_any;
          }
          if (next) {
            (void)next->KbdFocus();
            DrawBorderWindow(next, BORDER_ANY);
          } else {
            Swindow::KbdFocus(NULL);
          }
          if (!(w->Flags & WINDOWFL_MENU)) {
            screen->DrawMenu(0, TW_MAXDAT);
          }
          UpdateCursor();
        } else
          screen->FocusW(next);
      }

      if (w->MapUnMapHook)
        w->MapUnMapHook(w);

      screen->HookMap();

    } else {
      /* UnMap() a sub-window */
      if (w == parent->SelectW)
        parent->SelectW = (Twidget)0;

      w->Remove();
      DrawAreaWidget(w);
      w->Parent = (Twidget)0;

      if (w->Attr & (WIDGET_WANT_MOUSE_MOTION | WIDGET_AUTO_FOCUS))
        DecMouseMotionN();

      if (w->MapUnMapHook)
        w->MapUnMapHook(w);
    }
  } else if (w->MapQueueMsg) {
    /* the window was still waiting to be mapped! */
    w->MapQueueMsg->Delete();
    w->MapQueueMsg = (Tmsg)0;
  }
}

void Swidget::Raise() {
  RaiseWidget(this, false);
}

void Swidget::Lower() {
  LowerWidget(this, false);
}

void Swidget::Own(Tmsgport owner) {
  Twidget w = this;
  if (!w->Owner) {
    if ((w->O_Next = owner->Widgets.First)) {
      owner->Widgets.First->O_Prev = w;
    } else {
      owner->Widgets.Last = w;
    }
    w->O_Prev = (Twidget)0;
    owner->Widgets.First = w;
    w->Owner = owner;
  }
}

void Swidget::DisOwn() {
  Twidget w = this;
  Tmsgport owner = w->Owner;
  if (!owner) {
    return;
  }
  if (w->O_Prev)
    w->O_Prev->O_Next = w->O_Next;
  else if (owner->Widgets.First == w)
    owner->Widgets.First = w->O_Next;

  if (w->O_Next) {
    w->O_Next->O_Prev = w->O_Prev;
  } else if (owner->Widgets.Last == w) {
    owner->Widgets.Last = w->O_Prev;
  }
  w->O_Prev = w->O_Next = (Twidget)0;
  w->Owner = (Tmsgport)0;
}

void Swidget::RecursiveDelete(Tmsgport maybeOwner) {
  Twidget w = this;
  while (w->Widgets.First) {
    w->Widgets.First->RecursiveDelete(maybeOwner);
  }
  if (w->Owner == maybeOwner) {
    w->Delete();
  } else {
    w->UnMap();
  }
}

void Swidget::Expose(dat xwidth, dat ywidth, dat left, dat up, dat pitch, const char *ascii,
                     const trune *runes, const tcell *cells) {
  ExposeWidget2(this, xwidth, ywidth, left, up, pitch, ascii, runes, cells);
}

bool Swidget::InstallHook(HookFn hook, HookData *where) {
  if (hook && where && !where->Fn && !where->W && !Hook && !WhereHook) {
    Hook = where->Fn = hook;
    WhereHook = where;
    where->W = this;
    return true;
  }
  return false;
}

void Swidget::RemoveHook(HookFn hook, HookData *where) {
  if (hook && where && Hook == hook && WhereHook == where && where->Fn == hook &&
      where->W == this) {

    Hook = where->Fn = (HookFn)0;
    WhereHook = (HookData *)0;
    where->W = (Twidget)0;
  }
}
