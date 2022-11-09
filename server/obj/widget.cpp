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

#include "alloc.h"   // AllocMem0()
#include "fn.h"      // Fn_Twidget
#include "methods.h" // SetFillWidget()
#include "twin.h"    //IS_WIDGET()

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
      w->Fn = Fn_Twidget;
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
  while (FirstW) {
    FirstW->UnMap();
  }
  Sobj::Delete();
}

void Swidget::InsertWidget(Tobj parent, Twidget prev, Twidget next) {
  if (parent && IS_WIDGET(parent)) {
    /*
     * don't check this->Parent here, because Raise() and Lower() call Swidget::Remove()
     * then calls Swidget::Insert() but Swidget::Remove() does not reset w->Parent
     */
    InsertGeneric((TobjEntry)this, (TobjList) & ((Twidget)parent)->FirstW, //
                  (TobjEntry)prev, (TobjEntry)next, NULL);
  }
}

void Swidget::Insert(Twidget parent, Twidget prev, Twidget next) {
  // 'this' may be a subclass of Swidget
  InsertWidget(parent, prev, next);
}

void Swidget::Remove() {
  if (Parent) {
    RemoveGeneric((TobjEntry)this, (TobjList)&Parent->FirstW, NULL);
  }
}

void Swidget::ChangeField(udat field, uldat clear_mask, uldat xor_mask) {
  switch (field) {
  case TWS_widget_Left:
  case TWS_widget_Up:
  case TWS_widget_Width:
  case TWS_widget_Height:
    break;
  case TWS_widget_USE_Fill: {
    uldat i = tcell((USE_Fill & ~clear_mask) ^ xor_mask);
    SetFillWidget(this, i);
    break;
  }
  case TWS_widget_XLogic:
  case TWS_widget_YLogic:
    break;
  default:
    Sobj::ChangeField(field, clear_mask, xor_mask);
    break;
  }
}

Tgadget Swidget::FindGadgetByCode(udat Code) {
  for (Twidget w = FirstW; w; w = w->Next) {
    if (IS_GADGET(w) && ((Tgadget)w)->Code == Code) {
      return (Tgadget)w;
    }
  }
  return (Tgadget)0;
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
