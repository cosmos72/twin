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

static selection _Selection = {{(tany)0, (tany)0}, (msgport)0, (display_hw)0, SEL_TEXTMAGIC, "",
                               (uldat)0,           (uldat)0,   NULL};

static struct s_all _All = {
    /*s_obj*/
    NOID,
    NULL,
    NULL,
    NULL,
    NULL,
    /*FirstScreen*/ NULL,
    /*LastScreen*/ NULL,
    /*FirstMsgPort*/ NULL,
    /*LastMsgPort*/ NULL,
    /*RunMsgPort*/ NULL,
    /*FirstMutex*/ NULL,
    /*LastMutex*/ NULL,
    /*FirstModule*/ NULL,
    /*LastModule*/ NULL,
    /*FnHookModule*/ NULL,
    /*HookModule*/ NULL,
    /*FirstDisplayHW*/ NULL,
    /*LastDisplayHW*/ NULL,
    /*MouseHW*/ NULL,
    /*ExclusiveHW*/ NULL,
    /*FnHookDisplayHW*/ NULL,
    /*HookDisplayHW*/ NULL,
    /*DisplayWidth*/ 1,
    /*DisplayHeight*/ 1,
    /*State*/ state_default,
    /*Now*/ {(tany)0, (tany)0},
    /*Selection*/ &_Selection,
    /*SetUp*/ &_SetUp,
    /*AtQuit*/ NULL,
    /*BuiltinMenu*/ NULL,
    /*CommonMenu*/ NULL,
    /*BuiltinRow*/ NULL,
    /*MouseMotionN*/ 0,
    /*ButtonVec*/
    {{{
        0,
    }}},
    /*Gtranslations*/
    /*VT100GR_MAP*/ Tutf_VT100GR_to_UTF_32,
    /*LATIN1_MAP*/ NULL, /* it's the identity */
    /*IBMPC_MAP*/ Tutf_CP437_to_UTF_32,
    /*USER_MAP*/ GtransUser,
};

all CONST All = &_All;
