/*
 *  gadget.cpp  --  define methods of server class Sgadget
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/gadget.h"

#include "alloc.h"     // AllocMem0(), CloneStr2TRune()
#include "fn.h"        // Fn_Tgadget
#include "obj/id.h"    // Tgadget_class_id
#include "obj/group.h" // Sgroup
#include "menuitem.h"  // COD_RESERVED
#include "resize.h"    // PressGadget(), UnPressGadget()
#include "draw.h"      // DrawAreaWidget()

#include <Tw/Twstat_defs.h> // TWS_gadget_*

#include <new>

Tgadget Sgadget::Create(Tmsgport owner, Twidget parent, dat xwidth, dat ywidth,
                        const char *textnormal, uldat attr, uldat flags, udat code, tcolor coltext,
                        tcolor coltextselect, tcolor coltextdisabled, tcolor coltextselectdisabled,
                        dat left, dat up) {
  Tgadget g = NULL;

  if (owner) {
    void *addr = AllocMem0(sizeof(Sgadget));
    if (addr) {
      g = new (addr) Sgadget();
      g->Fn = (TwidgetFn)Fn_Tgadget;
      if (!g->Init(owner, parent, xwidth, ywidth, textnormal, attr, flags, code, coltext,
                   coltextselect, coltextdisabled, coltextselectdisabled, left, up)) {
        g->Delete();
        g = NULL;
      }
    }
  }
  return g;
}

Tgadget Sgadget::Init(Tmsgport owner, Twidget parent, dat xwidth, dat ywidth,
                      const char *textnormal, uldat attr, uldat flags, udat code, tcolor coltext,
                      tcolor coltextselect, tcolor coltextdisabled, tcolor coltextselectdisabled,
                      dat left, dat up) {

  if (code >= COD_RESERVED || xwidth <= 0 || ywidth <= 0 ||
      !Swidget::Init(owner, xwidth, ywidth, attr, flags, left, up, TCELL(coltext, ' '),
                     Tgadget_class_id)) {
    return NULL;
  }

  ColText = coltext;
  ColSelect = coltextselect;
  ColDisabled = coltextdisabled;
  ColSelectDisabled = coltextselectdisabled;
  Code = code;

  // G_Prev = G_Next = NULL;
  // Group = NULL;

  if (G_USE(this, USETEXT)) {
    ldat size = (ldat)xwidth * ywidth;
    if (textnormal)
      USE.T.Text[0] = CloneStr2TRune(textnormal, size);
    else {
      // USE.T.Text[0] = NULL;
    }
    // USE.T.Text[1] = USE.T.Text[2] = USE.T.Text[3] = NULL;
    // USE.T.Color[0] = USE.T.Color[1] = NULL;
    // USE.T.Color[2] = USE.T.Color[3] = NULL;
  }

  /* G->flags |= GADGETFL_TEXT_DEFCOL; */

  if (parent) {
    Map(parent);
  }
  return this;
}

void Sgadget::Delete() {
  UnMap();
  if (G_USE(this, USETEXT)) {
    for (byte i = 0; i < 4; i++) {
      if (USE.T.Text[i])
        FreeMem(USE.T.Text[i]);
      if (USE.T.Color[i])
        FreeMem(USE.T.Color[i]);
    }
  }
  if (Group) {
    Group->RemoveGadget(this);
  }
  Swidget::Delete();
}

void Sgadget::ChangeField(udat field, uldat clear_mask, uldat xor_mask) {
  switch (field) {
  case TWS_gadget_ColText:
  case TWS_gadget_ColSelect:
  case TWS_gadget_ColDisabled:
  case TWS_gadget_ColSelectDisabled:
  case TWS_gadget_Code:
    break;
  case TWS_gadget_Flags: {
    uldat mask = GADGETFL_DISABLED | GADGETFL_TEXT_DEFCOL | GADGETFL_PRESSED | GADGETFL_TOGGLE;
    clear_mask &= mask;
    xor_mask &= mask;
    uldat i = (Flags & ~clear_mask) ^ xor_mask;
    if ((i & mask) != (Flags & mask)) {
      if ((i & GADGETFL_PRESSED) != (Flags & GADGETFL_PRESSED)) {
        if (i & GADGETFL_PRESSED)
          PressGadget(this);
        else
          UnPressGadget(this, ttrue);
      }
      mask = GADGETFL_DISABLED | GADGETFL_TEXT_DEFCOL;
      if ((i & mask) != (Flags & mask)) {
        Flags = i;
        DrawAreaWidget((Twidget)this);
      } else
        Flags = i;
    }
    break;
  }
  default:
    Swidget::ChangeField(field, clear_mask, xor_mask);
    break;
  }
}
