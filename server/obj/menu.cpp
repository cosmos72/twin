/*
 *  menu.cpp  --  define methods of server class Smenu
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "alloc.h"   // AllocMem0()
#include "fn.h"      // Fn_Tobj
#include "hw.h"      // QueuedDrawArea2FullScreen
#include "methods.h" // InsertLast()
#include "twin.h"    // IS_SCREEN(), IS_WINDOW()
#include "obj/menu.h"
#include "obj/msgport.h"

#include <new>

Tmenu Smenu::Create(Tmsgport owner, tcolor colitem, tcolor colselect, tcolor coldisabled,
                    tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut,
                    byte flagdefcolinfo) {
  Tmenu m = NULL;
  if (owner) {
    void *addr = AllocMem0(sizeof(Smenu));
    if (addr) {
      m = new (addr) Smenu();
      m->Fn = Fn_Tobj;
      if (!m->Init(owner, colitem, colselect, coldisabled, colselectdisabled, colshtcut,
                   colselshtcut, flagdefcolinfo)) {
        m->Delete();
        m = NULL;
      }
    }
  }
  return m;
}

Tmenu Smenu::Init(Tmsgport owner, tcolor colitem, tcolor colselect, tcolor coldisabled,
                  tcolor colselectdisabled, tcolor colshtcut, tcolor colselshtcut,
                  byte flagdefcolinfo) {
  if (!owner || !Sobj::Init(Tmenu_class_id)) {
    return NULL;
  }
  ColItem = colitem;
  ColSelect = colselect;
  ColDisabled = coldisabled;
  ColSelectDisabled = colselectdisabled;
  ColShtCut = colshtcut;
  ColSelShtCut = colselshtcut;
  // Items.First = Items.Last = SelectI = NULL;
  CommonItems = tfalse;
  FlagDefColInfo = flagdefcolinfo;
  // Info = NULL;
  InsertLast(Menus, this, owner);
  return this;
}

void Smenu::Delete() {
  uldat count = 30;

  Tmsgport msgport = MsgPort;
  Twidget w, wnext;

  /*
   * warning:
   * the only way to get the list of windows that
   * are using this Tmenu is to scan the whole MsgPort.
   * We must UnMap() all those windows
   * as a Twindow without Tmenu cannot be displayed.
   *
   * optimization: if we are going to UnMap() a lot of windows,
   * we set QueuedDrawArea2FullScreen = true, so that the UnMap()
   * calls do not have to redraw every time.
   */
  if (!QueuedDrawArea2FullScreen) {
    for (w = msgport->Widgets.First; w && count; w = wnext) {
      wnext = w->O_Next;
      if (IS_WINDOW(w) && ((Twindow)w)->Menu == this) {
        if (w->Parent && IS_SCREEN(w->Parent)) {
          count--;
        }
      }
    }
    if (!count) {
      QueuedDrawArea2FullScreen = true;
    }
  }
  for (w = msgport->Widgets.First; w; w = wnext) {
    wnext = w->O_Next;
    if (IS_WINDOW(w) && ((Twindow)w)->Menu == this) {
      if (w->Parent && IS_SCREEN(w->Parent)) {
        w->UnMap();
        ((Twindow)w)->Menu = NULL;
      }
    }
  }
  Remove();
  DeleteList(Items.First);
  if (Info) {
    Info->Delete();
  }
  Sobj::Delete();
}

void Smenu::Insert(Tmsgport parent, Tmenu prev, Tmenu next) {
  if (parent && !MsgPort) {
    parent->Menus.Insert(this, prev, next);
    MsgPort = parent;
  }
}

void Smenu::Remove() {
  if (MsgPort) {
    MsgPort->Menus.Remove(this);
    MsgPort = (Tmsgport)0;
  }
}

Trow Smenu::SetInfo(byte flags, ldat len, const char *text, const tcolor *coltext) {
  Tmenu menu = this;
  Trow row = New(row)(0, flags);
  if (row) {
    if ((!text || (row->Text = CloneStr2TRune(text, len))) &&
        (!coltext || (row->ColText = (tcolor *)CloneMem(coltext, len * sizeof(tcolor))))) {
      row->Len = row->MaxLen = len;
      if (menu->Info)
        menu->Info->Delete();
      return menu->Info = row;
    }
    row->Delete();
    row = (Trow)0;
  }
  return row;
}

Tmenuitem Smenu::FindItem(dat i) const {
  const Smenu *menu = this;
  Tmenuitem item = (Tmenuitem)0;

  if (menu) {
    for (item = menu->Items.First; item; item = item->NextItem()) {
      if (i >= item->Left && (uldat)(i - item->Left) < item->Len) {
        break;
      }
    }

    if (!item && menu->CommonItems && All->CommonMenu) {

      item = menu->Items.Last;

      if (!item || (i >= item->Left && (uldat)(i - item->Left) >= item->Len)) {
        /* search in All->CommonMenu */
        if (item)
          i -= item->Left + (dat)item->Len;
        for (item = All->CommonMenu->Items.First; item; item = item->NextItem()) {
          if (i >= item->Left && (uldat)(i - item->Left) < item->Len) {
            break;
          }
        }
      } else {
        item = (Tmenuitem)0;
      }
    }
  }
  return item;
}

Tmenuitem Smenu::GetSelectedItem() const {
  const Smenu *menu = this;
  if (menu) {
    if (menu->SelectI)
      return menu->SelectI;
    if (menu->CommonItems && All->CommonMenu)
      return All->CommonMenu->SelectI;
  }
  return (Tmenuitem)0;
}

Tmenuitem Smenu::RecursiveGetSelectedItem(dat *depth) const {
  const Smenu *menu = this;
  Tmenuitem item = NULL, prev = menu->GetSelectedItem();
  Twindow w = (Twindow)0;
  Twidget focus_w = All->Screens.First->FocusW();
  dat d = -1;

  while (prev && IS_MENUITEM(prev)) {
    item = prev;
    d++;
    if (w == focus_w)
      break;
    if ((w = item->Window) && w->Parent)
      prev = (Tmenuitem)w->FindRow(w->CurY);
    else
      break;
  }
  if (depth)
    *depth = d > 0 ? d : 0;
  return item;
}

void Smenu::SetSelectedItem(Tmenuitem item) {
  Tmenu menu = this;
  if (menu) {
    if (item) {
      if (item->Parent == (Tobj)menu) {
        menu->SelectI = item;
        if (menu->CommonItems && All->CommonMenu)
          All->CommonMenu->SelectI = (Tmenuitem)0;
      } else if (menu->CommonItems && item->Parent == (Tobj)All->CommonMenu) {
        menu->SelectI = (Tmenuitem)0;
        All->CommonMenu->SelectI = item;
      }
      /* else item is not a meaningful one! */
    } else {
      menu->SelectI = item;
      if (menu->CommonItems && All->CommonMenu)
        All->CommonMenu->SelectI = item;
    }

    All->Screens.First->DrawMenu(0, TW_MAXDAT);
  }
}
