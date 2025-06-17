/*
 *  gadget.h  --  declare server class Sgadget
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_GADGET_H
#define TWIN_GADGET_H

#include "obj/widget.h"

class Sgadget : public Swidget {
public:
  /* Tgadget */
  tcolor ColText, ColSelect, ColDisabled, ColSelectDisabled;
  udat Code;
  Tgadget G_Prev, G_Next; /* list in the same Tgroup */
  Tgroup Group;

  static Tgadget Create(Tmsgport owner, Twidget Parent, dat xwidth, dat ywidth,
                        const char *textnormal, uldat attr, uldat flags, udat code, tcolor coltext,
                        tcolor coltextselect, tcolor coltextdisabled, tcolor coltextselectdisabled,
                        dat left, dat up);
  static Tgadget CreateButton(Twidget parent, dat xwidth, dat ywidth, const char *text, uldat flags,
                              udat code, tcolor bgcol, tcolor col, tcolor coldisabled, dat left,
                              dat up);
  static Tgadget CreateEmptyButton(Tmsgport owner, dat xwidth, dat ywidth, tcolor bgcol);

  Tgadget Init(Tmsgport owner, Twidget Parent, dat xwidth, dat ywidth, const char *TextNormal,
               uldat Attr, uldat Flags, udat Code, tcolor ColText, tcolor ColTextSelect,
               tcolor ColTextDisabled, tcolor ColTextSelectDisabled, dat Left, dat Up);

  /* Tobj */
  virtual void Delete() OVERRIDE;
  virtual void ChangeField(udat field, uldat clear_mask, uldat xor_mask) OVERRIDE;

  /* Twidget */
  virtual void DrawSelf(Sdraw *d) OVERRIDE; // defined in draw.cpp

  /* Tgadget */
  bool FillButton(Twidget parent, udat code, dat left, dat up, udat flags, const char *text,
                  tcolor color, tcolor colordisabled);
  // defined in resize.cpp
  void WriteTexts(byte bitmap, dat xwidth, dat ywidth, const char *text, dat left, dat up);
  // defined in resize.cpp
  void WriteTRunes(byte bitmap, dat xwidth, dat ywidth, const trune *runes, dat left, dat up);
};

/* Gadget->Attr */
enum gadget_attr /*: udat*/ {
  GADGET_WANT_MOUSE_MOTION = WIDGET_WANT_MOUSE_MOTION, /* 0x0001 */
  GADGET_WANT_KEYS = WIDGET_WANT_KEYS,                 /* 0x0002 */
  GADGET_WANT_MOUSE = WIDGET_WANT_MOUSE,               /* 0x0004 */
  GADGET_WANT_CHANGES = WIDGET_WANT_CHANGES,           /* 0x0008 */
  GADGET_AUTO_FOCUS = WIDGET_AUTO_FOCUS,               /* 0x0010 */
};

/* Gadget->Flags */
enum gadget_flag /*: udat*/ {
  GADGETFL_USETEXT = 0x00,                 /* default */
  GADGETFL_USEEXPOSE = WIDGETFL_USEEXPOSE, /* 0x02 */
  GADGETFL_USEFILL = WIDGETFL_USEFILL,     /* 0x03 */
  GADGETFL_USEANY = WIDGETFL_USEANY,       /* 0x07 */

  /* remember this Tgadget is a button, so that SetText() does not ruin the shadow */
  GADGETFL_BUTTON = 0x0010,
  GADGETFL_DISABLED = 0x0020,
  GADGETFL_TEXT_DEFCOL = 0x0040,
  /* this makes the Tgadget 'checkable' : can be in 'checked' or 'unchecked' state.
   * also necessary to put the Tgadget in a Tgroup */
  GADGETFL_TOGGLE = 0x0080,
  GADGETFL_PRESSED = 0x0100,

  GADGETFL_NOTVISIBLE = 0x8000,
};

#define G_USE(g, USExxx) (((g)->Flags & GADGETFL_USEANY) == CAT(GADGETFL_, USExxx))

/*              NOTE :
 *
 * the USE.T fields (Contents) of a Tgadget are used as follows:
 * Text[0]==TextNormal;      (mandatory)
 * Text[1]==TextSelect;        (if not present, use ...[0])
 * Text[2]==TextDisabled;      (if not present, use ...[0])
 * Text[3]==TextSelectDisabled; (if not present, use ...[1]; if ...[1] not present too, use ...[0])
 *
 * Color[0]==ColorNormal;      (mandatory unless GADGET_USE_DEFCOL is set)
 * Color[1]==ColorSelect;        (if not present, use ...[0])
 * Color[2]==ColorDisabled;      (if not present, use ...[0])
 * Color[3]==ColorSelectDisabled;(if not present, use ...[1]; if ...[1] not present too, use ...[0])
 *
 * If GADGET_USE_DEFCOL is set,
 * Color[0], [1], [2] and [3] are ignored.
 *
 */

#endif /* TWIN_GADGET_H */
