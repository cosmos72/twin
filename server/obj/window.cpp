/*
 *  widget.cpp  --  define methods of server class s_window
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/window.h"
#include "algo.h"  // Max2()
#include "alloc.h" // AllocMem0(), CloneStrL()
#include "data.h"  // DEFAULT_Col*
#include "fn.h"    // Fn_window
#include "id.h"    // AssignId()
#include "twin.h"  // NOFD, NOPID, NOSLOT

#include <new>
#include <string.h>    // memset()
#include <Tutf/Tutf.h> // Tutf_CP437_to_UTF_32[]

static byte InitTtyData(window w, dat scrollbacklines);

window s_window::Create(msgport owner, dat titlelen, const char *title, const tcolor *coltitle,
                        menu m, tcolor coltext, uldat cursortype, uldat attr, uldat flags,
                        dat xwidth, dat ywidth, dat scrollbacklines) {

  window w = NULL;
  if (owner) {
    void *addr = AllocMem0(sizeof(s_window));
    if (addr) {
      w = new (addr) s_window();
      w->Fn = Fn_window;
      if (!w->Init(owner, titlelen, title, coltitle, m, coltext, cursortype, attr, flags, xwidth,
                   ywidth, scrollbacklines)) {
        w->Delete();
        w = NULL;
      }
    }
  }
  return w;
}

window s_window::Init(msgport owner, dat titlelen, const char *title, const tcolor *coltitle,
                      menu m, tcolor coltext, uldat cursortype, uldat attr, uldat flags, dat xwidth,
                      dat ywidth, dat scrollbacklines) {

  byte hasborder = 2 * !(flags & WINDOWFL_BORDERLESS);
  /* overflow safety */
  if (hasborder) {
    if ((dat)(xwidth + hasborder) > 0)
      xwidth += hasborder;
    if ((dat)(ywidth + hasborder) > 0)
      ywidth += hasborder;
  }
  if (!((widget)this)
           ->Init(owner, xwidth, ywidth, attr, flags, 0, TW_MAXDAT, TCELL(coltext, ' '))) {
    return NULL;
  }
  if (title && !(Name = CloneStrL(title, titlelen))) {
    return NULL;
  }
  if (coltitle && !(ColName = (tcolor *)CloneMem(coltitle, titlelen * sizeof(tcolor)))) {
    return NULL;
  }

  Menu = m;
  // MenuItem = NULL;
  NameLen = titlelen;
  //  BorderPattern[0] = BorderPattern[1] = NULL;
  RemoteData.Fd = NOFD;
  RemoteData.ChildPid = NOPID;
  RemoteData.FdSlot = NOSLOT;
  // CurX = CurY = 0;
  // XstSel = YstSel = XendSel = YendSel = 0;
  ColGadgets = DEFAULT_ColGadgets;
  ColArrows = DEFAULT_ColArrows;
  ColBars = DEFAULT_ColBars;
  ColTabs = DEFAULT_ColTabs;
  ColBorder = DEFAULT_ColBorder;
  ColText = ColText;
  ColSelect = TCOL(TCOLBG(ColText), TCOLFG(ColText));
  ColDisabled = DEFAULT_ColDisabled;
  ColSelectDisabled = DEFAULT_ColSelectDisabled;
  /* sanity: */
  if (Flags & WINDOWFL_BORDERLESS)
    Attr &= ~WINDOW_ROLLED_UP;
  if (Attr & WINDOW_WANT_KEYS)
    Attr &= ~WINDOW_AUTO_KEYS;

  // State = (uldat)0;
  CursorType = cursortype;

  MinXWidth = MIN_XWIN;
  MinYWidth = MIN_YWIN;
  XWidth = xwidth = Max2(MIN_XWIN, xwidth);
  YWidth = ywidth = Max2(MIN_YWIN, ywidth);
  MaxXWidth = TW_MAXDAT;
  MaxYWidth = TW_MAXDAT;

  Charset = Tutf_CP437_to_UTF_32;
  // memset(&USE, '\0', sizeof(USE));

  if (W_USE(this, USECONTENTS)) {
    if (TW_MAXDAT - scrollbacklines < ywidth - hasborder)
      scrollbacklines = TW_MAXDAT - YWidth + hasborder;
    CurY = YLogic = scrollbacklines;
    WLogic = xwidth - hasborder;
    HLogic = scrollbacklines + ywidth - hasborder;
    if (!InitTtyData(this, scrollbacklines)) {
      return NULL;
    }
  } else if (W_USE(this, USEROWS)) {
    WLogic = 1024; /* just an arbitrary value... */
    HLogic = 0;    /* no rows */
  } else {
    WLogic = xwidth - hasborder;
    HLogic = scrollbacklines + ywidth - hasborder;
  }
  return this;
}

/* ttydata */

static byte InitTtyData(window w, dat scrollbacklines) {
  ttydata *Data = w->USE.C.TtyData;
  ldat count = w->WLogic * w->HLogic;
  tcell *p = w->USE.C.Contents, h;

  if (!Data && !(w->USE.C.TtyData = Data = (ttydata *)AllocMem(sizeof(ttydata))))
    return tfalse;

  if (!p && !(w->USE.C.Contents = p = (tcell *)AllocMem(count * sizeof(tcell))))
    return tfalse;

  h = TCELL(TCOL(twhite, tblack), ' ');
  while (count--)
    *p++ = h;

  /*
   * this is a superset of reset_tty(),
   * but we don't want to call it from here
   */
  Data->State = ESnormal;
  Data->Flags = TTY_AUTOWRAP;
  Data->Effects = 0;
  w->YLogic = w->CurY = Data->ScrollBack = scrollbacklines;
  w->USE.C.HSplit = 0;
  Data->SizeX = w->WLogic;
  Data->SizeY = w->HLogic - scrollbacklines;
  Data->Top = 0;
  Data->Bottom = Data->SizeY;
  Data->saveX = Data->X = w->CurX = 0;
  Data->saveY = Data->Y = 0;
  Data->Pos = Data->Start = w->USE.C.Contents + Data->ScrollBack * w->WLogic;
  Data->Split = w->USE.C.Contents + w->WLogic * w->HLogic;

  w->CursorType = LINECURSOR;
  /* respect the WINDOWFL_CURSOR_ON set by the client and don't force it on */
#if 0
  w->Flags |= WINDOWFL_CURSOR_ON;
#endif
  w->ColText = Data->Color = Data->DefColor = Data->saveColor = TCOL(twhite, tblack);
  Data->Underline = TCOL(thigh | twhite, tblack);
  Data->HalfInten = TCOL(thigh | tblack, tblack);
  Data->TabStop[0] = 0x01010100;
  Data->TabStop[1] = Data->TabStop[2] = Data->TabStop[3] = Data->TabStop[4] = 0x01010101;
  Data->nPar = 0;

  Data->G = Data->saveG = 0;
  /* default to latin1 charset */
  Data->currG = Data->G0 = Data->saveG0 = LATIN1_MAP;
  Data->G1 = Data->saveG1 = VT100GR_MAP;

  Data->utf8 = Data->utf8_count = Data->utf8_char = 0;
  Data->InvCharset = Tutf_UTF_32_to_ISO_8859_1;
  Data->newLen = Data->newMax = 0;
  Data->newName = NULL;

  return ttrue;
}
