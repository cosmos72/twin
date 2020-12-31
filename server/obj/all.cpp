/*
 *  all.cpp  --  define server class s_all
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
#include "Tw/mouse.h"  /* HOLD_* */
#include "Tutf/Tutf.h" /* Tutf_*_to_UTF_32[] */

static trune GtransUser[0x100];

static setup _SetUp = {
    (dat)1,              /* MaxMouseSnap */
    (udat)0x7,           /* MinAllocSize */
    (udat)0,             /* Flags */
    HOLD_LEFT,           /* SelectionButton */
    HOLD_MIDDLE,         /* PasteButton */
    (byte)3,     (byte)2 /* DeltaXShade, DeltaYShade */
};

static selection _Selection = {{(tany)0, (tany)0}, (msgport)0, (display_hw)0,
                               SEL_TEXTMAGIC,      "",         {}};

all s_all::Init() {
  State = state_default;
  Selection = &_Selection;
  SetUp = &_SetUp;
  Gtranslations[VT100GR_MAP] = Tutf_VT100GR_to_UTF_32;
  Gtranslations[LATIN1_MAP] = NULL; /* it's the identity */
  Gtranslations[IBMPC_MAP] = Tutf_CP437_to_UTF_32;
  Gtranslations[USER_MAP] = GtransUser;
  return this;
};

static struct s_all _All;

all const All = _All.Init();
