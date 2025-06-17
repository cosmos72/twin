/*
 *  all.cpp  --  define server class Sall
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/id.h" /* NOID */
#include "obj/all.h"
#include "obj/msgport.h"
#include "obj/screen.h"
#include "common.h" /* TwinSelectionNotify */

#include <Tw/mouse.h>  /* HOLD_* */
#include <Tutf/Tutf.h> /* Tutf_*_to_UTF_32[] */

static trune GtransUser[0x100];

static Ssetup _SetUp = {
    (dat)1,              /* MaxMouseSnap */
    (udat)0x7,           /* MinAllocSize */
    (udat)0,             /* Flags */
    HOLD_LEFT,           /* SelectionButton */
    HOLD_MIDDLE,         /* PasteButton */
    (byte)3,     (byte)2 /* DeltaXShade, DeltaYShade */
};

static Sselection _Selection = {{(tany)0, (tany)0}, (Tmsgport)0, (Tdisplay)0,
                                SEL_UTF8MAGIC,      String(),    ""};

static Sselection _Clipboard = {{(tany)0, (tany)0}, (Tmsgport)0, (Tdisplay)0,
                                SEL_UTF8MAGIC,      String(),    ""};

void Sselection::dup(const Tselection other) {
  Time = other->Time;
  Owner = other->Owner;
  OwnerOnce = other->OwnerOnce;
  Magic = other->Magic;
  Data.assign(other->Data);
  memcpy(MIME, other->MIME, sizeof(MIME));
}

void Sselection::paste() {
  Twidget w;
  if (Data && (w = All->FirstScreen->FocusW()) && w->Owner) {
    TwinSelectionNotify(w->Owner, w->Id, e_id(Magic), MIME, Data);
  }
}

Tall Sall::Init() {
  State = state_default;
  Selection = &_Selection;
  Clipboard = &_Clipboard;
  SetUp = &_SetUp;
  Gtranslations[VT100GR_MAP] = const_cast<trune *>(Tutf_VT100GR_to_UTF_32);
  Gtranslations[LATIN1_MAP] = NULL; /* it's the identity */
  Gtranslations[IBMPC_MAP] = const_cast<trune *>(Tutf_CP437_to_UTF_32);
  Gtranslations[USER_MAP] = GtransUser;
  return this;
};

void Sall::Remove() {
}

void Sall::Delete() {
}

static Sall _All;

Tall const All = _All.Init();
