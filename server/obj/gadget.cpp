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
#include "alloc.h"    // CloneStr2TRune
#include "menuitem.h" // COD_RESERVED

gadget s_gadget::Create(fn_gadget Fn, msgport Owner, widget Parent, dat XWidth, dat YWidth,
                        const char *TextNormal, uldat Attrib, uldat Flags, udat Code,
                        tcolor ColText, tcolor ColTextSelect, tcolor ColTextDisabled,
                        tcolor ColTextSelectDisabled, dat Left, dat Up) {
  gadget G = NULL;
  ldat Size;

  if (Owner && Code < COD_RESERVED && XWidth > 0 && YWidth > 0 &&
      (G = (gadget)s_widget::Create((fn_widget)Fn, Owner, XWidth, YWidth, Attrib, Flags, Left, Up,
                                    TCELL(ColText, ' ')))) {

    G->ColText = ColText;
    G->ColSelect = ColTextSelect;
    G->ColDisabled = ColTextDisabled;
    G->ColSelectDisabled = ColTextSelectDisabled;
    G->Code = Code;

    G->G_Prev = G->G_Next = NULL;
    G->Group = (ggroup)0;

    if (G_USE(G, USETEXT)) {
      Size = (ldat)XWidth * YWidth;
      if (TextNormal)
        G->USE.T.Text[0] = CloneStr2TRune(TextNormal, Size);
      else
        G->USE.T.Text[0] = NULL;

      G->USE.T.Text[1] = G->USE.T.Text[2] = G->USE.T.Text[3] = NULL;
      G->USE.T.Color[0] = G->USE.T.Color[1] = G->USE.T.Color[2] = G->USE.T.Color[3] = NULL;
    }

    /* G->Flags |= GADGETFL_TEXT_DEFCOL; */

    if (Parent)
      G->Map(Parent);
  }
  return G;
}
