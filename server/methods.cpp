/*
 *  methods.c  --  functions for OO-style calls on twin data structures
 *                 (windows, menus, ...)
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "alloc.h"
#include "algo.h"
#include "main.h"
#include "builtin.h"
#include "methods.h"
#include "obj/id.h"
#include "data.h"

#include "dl.h"
#include "draw.h"
#include "extreg.h"
#include "log.h"
#include "resize.h"
#include "util.h"
#include "hw.h"
#include "hw_multi.h"
#include "tty.h" // TtyWrite*()
#include "wm.h"

#include <new>
#include <Tw/Tw.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>
#include <Tutf/Tutf.h>
#include <Tutf/Tutf_defs.h>

void *OverrideMth(void **where, void *OrigMth, void *NewMth) {
  if (*where == OrigMth) {
    *where = NewMth;
    return NewMth;
  }
  return NULL;
}

/* Twidget */

void IncMouseMotionN(void) {
  if (!All->MouseMotionN++) {
    EnableMouseMotionEvents(ttrue);
  }
}

void DecMouseMotionN(void) {
  if (All->MouseMotionN && !--All->MouseMotionN) {
    EnableMouseMotionEvents(tfalse);
  }
}

static SwidgetFn _FnWidget = {
    TtyKbdFocus,
};

/* Tgadget */

/* Twindow */

static SwindowFn _FnWindow = {
    /* Twidget */
    TtyKbdFocus,
    /* Twindow */
    TtyWriteCharset,
    TtyWriteUtf8,
    TtyWriteTRune,
    TtyWriteTCell,
    WMFindBorderWindow,
};

/* Tscreen */

/* Tgroup */

/* Trow */

byte FindInfo(Tmenu Menu, dat i) {
  Trow Info;

  if (Menu && (Info = Menu->Info) && Info->Len > (udat)i)
    return ttrue;
  return tfalse;
}

/* Tmenu */
/* Tmsg */
/* Tmsgport */
/* Tmutex */

/* Tmodule */

static SmoduleFn _FnModule = {
    /* Tmodule */
    DlOpen,
    DlClose,
};

/* Tdisplay */

SstructFn FnStruct = {&_FnWidget, &_FnWindow, &_FnModule};
