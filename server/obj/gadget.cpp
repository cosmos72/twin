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
#include "obj/id.h"    // Tgadget_class_id
#include "obj/group.h" // Sgroup
#include "menuitem.h"  // COD_RESERVED
#include "resize.h"    // PressGadget(), UnPressGadget()
#include "draw.h"      // DrawAreaWidget()
#include "builtin.h"   // ColorFill()

#include <Tw/datasizes.h>   // TW_SIZEOF_TCOLOR
#include <Tw/Tw_defs.h>     // tpalette_n
#include <Tw/Twstat_defs.h> // TWS_gadget_*
#include <Tutf/utf_32.h>    // T_UTF_32_*_BLOCK
#include <Tutf/Tutf.h>      // Tutf_CP437_to_UTF_32

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
      if (!g->Init(owner, parent, xwidth, ywidth, textnormal, attr, flags, code, coltext,
                   coltextselect, coltextdisabled, coltextselectdisabled, left, up)) {
        g->Delete();
        g = NULL;
      }
    }
  }
  return g;
}

Tgadget Sgadget::CreateButton(Twidget parent, dat xwidth, dat ywidth, const char *text, uldat flags,
                              udat code, tcolor bgcol, tcolor col, tcolor coldisabled, dat left,
                              dat up) {
  Tgadget g = NULL;
  if (parent && (g = CreateEmptyButton(parent->Owner, xwidth, ywidth, bgcol))) {
    if (g->FillButton(parent, code, left, up, flags, text, col, coldisabled))
      return g;
    g->Delete();
    g = NULL;
  }
  return g;
}

Tgadget Sgadget::CreateEmptyButton(Tmsgport owner, dat xwidth, dat ywidth, tcolor bgcol) {
  Tgadget g = NULL;
  ldat size;
  byte i;
  dat j, k;
#define FULL_ T_UTF_32_FULL_BLOCK
#define LOWER_ T_UTF_32_LOWER_HALF_BLOCK
#define UPPER_ T_UTF_32_UPPER_HALF_BLOCK
  void *addr = AllocMem0(sizeof(Sgadget));
  if (addr) {
    g = new (addr) Sgadget();
    if (!((Twidget)g)
             ->Init(owner, ++xwidth, ++ywidth, 0, GADGETFL_USETEXT | GADGETFL_BUTTON, 0, 0, TCELL0,
                    Tgadget_class_id)) {
      g->Delete();
      return NULL;
    }

    size = (ldat)xwidth * ywidth;

    for (i = 0; i < 4; i++) {
      g->USE.T.Text[i] = NULL;
      g->USE.T.Color[i] = NULL;
    }
    bgcol.fg &= TRGB_MAX;
    bgcol.bg &= TRGB_MAX;
    for (i = 0; i < 4; i++) {
      if (!(g->USE.T.Text[i] = (trune *)AllocMem(size * sizeof(trune))) ||
          !(g->USE.T.Color[i] = (tcolor *)AllocMem(size * sizeof(tcolor)))) {

        g->Delete();
        return NULL;
      }
      ColorFill(g->USE.T.Color[i], size, bgcol);
    }
    size = (ldat)--xwidth * --ywidth;

    for (i = 0; i < 4; i++) {
      for (j = k = 0; j < ywidth; j++, k += xwidth + 1) {
        g->USE.T.Text[i][k + (i & 1 ? 0 : xwidth)] = i & 1 ? ' ' : k ? FULL_ : LOWER_;
      }
      g->USE.T.Text[i][k] = ' ';
      for (j = 0; j < xwidth; j++) {
        g->USE.T.Text[i][k + 1 + j] = i & 1 ? ' ' : UPPER_;
      }
    }

    g->G_Prev = g->G_Next = (Tgadget)0;
    g->Group = (Tgroup)0;
  }
  return g;
#undef FULL_
#undef UPPER_
#undef LOWER_
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

bool Sgadget::FillButton(Twidget parent, udat code, dat left, dat up, udat flags, const char *text,
                         tcolor color, tcolor colordisabled) {

  if (code >= COD_RESERVED) {
    return false;
  }
  Code = code;
  Left = left;
  Up = up;
  Flags = (flags & ~GADGETFL_USEANY) | GADGETFL_USETEXT | GADGETFL_BUTTON;

  dat xwidth = XWidth, ywidth = YWidth;
  for (dat j = 0; j < (ywidth - 1) * xwidth; j += xwidth) {
    for (dat i = 0; i < xwidth - 1; i++) {
      USE.T.Text[0][i + j] = USE.T.Text[1][i + j + 1] = USE.T.Text[2][i + j] =
          USE.T.Text[3][i + j + 1] = Tutf_CP437_to_UTF_32[(byte) * (text++)];

      USE.T.Color[0][i + j] = USE.T.Color[1][i + j + 1] = color;
      USE.T.Color[2][i + j] = USE.T.Color[3][i + j + 1] = colordisabled;
    }
  }
  if (parent) {
    Map(parent);
  }
  return true;
}
