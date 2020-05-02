/*
 *  gadget.cpp  --  define methods of server class s_gadget
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
#include "alloc.h"    // AllocMem0(), CloneStr2TRune()
#include "fn.h"       // Fn_gadget
#include "id.h"       // AssignId()
#include "menuitem.h" // COD_RESERVED

gadget s_gadget::Create(msgport owner, widget parent, dat xwidth, dat ywidth,
                        const char *textnormal, uldat attr, uldat flags, udat code, tcolor coltext,
                        tcolor coltextselect, tcolor coltextdisabled, tcolor coltextselectdisabled,
                        dat left, dat up) {
  gadget g = NULL;
  if (owner) {
    g = (gadget)AllocMem0(sizeof(s_gadget), 1);
    if (g) {
      g->Fn = Fn_gadget;
      if (!g->Init(owner, parent, xwidth, ywidth, textnormal, attr, flags, code, coltext,
                   coltextselect, coltextdisabled, coltextselectdisabled, left, up)) {
        g->Delete();
        g = NULL;
      }
    }
  }
  return g;
}

gadget s_gadget::Init(msgport owner, widget parent, dat xwidth, dat ywidth, const char *textnormal,
                      uldat attr, uldat flags, udat code, tcolor coltext, tcolor coltextselect,
                      tcolor coltextdisabled, tcolor coltextselectdisabled, dat left, dat up) {
  ldat Size;

  if (!((widget)this)->Init(owner, xwidth, ywidth, attr, flags, left, up, TCELL(coltext, ' '))) {
    return NULL;
  }
  if (code >= COD_RESERVED || xwidth <= 0 || ywidth <= 0) {
    return NULL;
  }

  this->ColText = coltext;
  this->ColSelect = coltextselect;
  this->ColDisabled = coltextdisabled;
  this->ColSelectDisabled = coltextselectdisabled;
  this->Code = code;

  // this->G_Prev = this->G_Next = NULL;
  // this->Group = NULL;

  if (G_USE(this, USETEXT)) {
    Size = (ldat)xwidth * ywidth;
    if (textnormal)
      this->USE.T.Text[0] = CloneStr2TRune(textnormal, Size);
    else {
      // this->USE.T.Text[0] = NULL;
    }
    // this->USE.T.Text[1] = this->USE.T.Text[2] = this->USE.T.Text[3] = NULL;
    // this->USE.T.Color[0] = this->USE.T.Color[1] = NULL;
    // this->USE.T.Color[2] = this->USE.T.Color[3] = NULL;
  }

  /* G->flags |= GADGETFL_TEXT_DEFCOL; */

  if (parent) {
    this->Map(parent);
  }
  return this;
}
