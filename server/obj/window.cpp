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
#include "alloc.h" // CloneStrL()
#include "data.h"  // DEFAULT_Col*
#include "twin.h"  // NOFD, NOPID, NOSLOT

#include <string.h>    // memset()
#include <Tutf/Tutf.h> // Tutf_CP437_to_UTF_32[]

static byte InitTtyData(window w, dat ScrollBackLines);

window s_window::Create(fn_window Fn, msgport Owner, dat TitleLen, const char *Title,
                        const tcolor *ColTitle, menu Menu, tcolor ColText, uldat CursorType,
                        uldat Attrib, uldat Flags, dat XWidth, dat YWidth, dat ScrollBackLines) {

  window w = NULL;
  char *_Title = NULL;
  tcolor *_ColTitle = NULL;
  byte HasBorder = 2 * !(Flags & WINDOWFL_BORDERLESS);

  /* overflow safety */
  if (HasBorder) {
    if ((dat)(XWidth + HasBorder) > 0)
      XWidth += HasBorder;
    if ((dat)(YWidth + HasBorder) > 0)
      YWidth += HasBorder;
  }

  if ((!Title || (_Title = CloneStrL(Title, TitleLen))) &&
      (!ColTitle || (_ColTitle = (tcolor *)CloneMem(ColTitle, TitleLen * sizeof(tcolor)))) &&
      Owner &&
      (w = (window)Fn->Fn_Widget->Create((fn_widget)Fn, Owner, XWidth, YWidth, Attrib, Flags, 0,
                                         TW_MAXDAT, TCELL(ColText, ' ')))) {
    w->Menu = Menu;
    w->MenuItem = (menuitem)0;
    w->NameLen = TitleLen;
    w->Name = _Title;
    w->ColName = _ColTitle;
    w->BorderPattern[0] = w->BorderPattern[1] = (trune *)0;
    w->RemoteData.Fd = NOFD;
    w->RemoteData.ChildPid = NOPID;
    w->RemoteData.FdSlot = NOSLOT;
    w->CurX = w->CurY = 0;
    w->XstSel = w->YstSel = w->XendSel = w->YendSel = 0;
    w->ColGadgets = DEFAULT_ColGadgets;
    w->ColArrows = DEFAULT_ColArrows;
    w->ColBars = DEFAULT_ColBars;
    w->ColTabs = DEFAULT_ColTabs;
    w->ColBorder = DEFAULT_ColBorder;
    w->ColText = ColText;
    w->ColSelect = TCOL(TCOLBG(ColText), TCOLFG(ColText));
    w->ColDisabled = DEFAULT_ColDisabled;
    w->ColSelectDisabled = DEFAULT_ColSelectDisabled;
    /* sanity: */
    if (w->Flags & WINDOWFL_BORDERLESS)
      w->Attrib &= ~WINDOW_ROLLED_UP;
    if (w->Attrib & WINDOW_WANT_KEYS)
      w->Attrib &= ~WINDOW_AUTO_KEYS;

    w->State = (uldat)0;
    w->CursorType = CursorType;

    w->MinXWidth = MIN_XWIN;
    w->MinYWidth = MIN_YWIN;
    w->XWidth = XWidth = Max2(MIN_XWIN, XWidth);
    w->YWidth = YWidth = Max2(MIN_YWIN, YWidth);
    w->MaxXWidth = TW_MAXDAT;
    w->MaxYWidth = TW_MAXDAT;

    w->Charset = Tutf_CP437_to_UTF_32;
    memset(&w->USE, '\0', sizeof(w->USE));

    if (W_USE(w, USECONTENTS)) {
      if (TW_MAXDAT - ScrollBackLines < YWidth - HasBorder)
        ScrollBackLines = TW_MAXDAT - YWidth + HasBorder;
      w->CurY = w->YLogic = ScrollBackLines;
      w->WLogic = XWidth - HasBorder;
      w->HLogic = ScrollBackLines + YWidth - HasBorder;
      if (!InitTtyData(w, ScrollBackLines)) {
        w->Delete();
        w = NULL;
      }
    } else if (W_USE(w, USEROWS)) {
      w->WLogic = 1024; /* just an arbitrary value... */
      w->HLogic = 0;    /* no rows */
    } else {
      w->WLogic = XWidth - HasBorder;
      w->HLogic = ScrollBackLines + YWidth - HasBorder;
    }
    return w;
  }
  if (_Title)
    FreeMem(_Title);
  if (_ColTitle)
    FreeMem(_ColTitle);
  return w;
}

/* ttydata */

static byte InitTtyData(window w, dat ScrollBackLines) {
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
  w->YLogic = w->CurY = Data->ScrollBack = ScrollBackLines;
  w->USE.C.HSplit = 0;
  Data->SizeX = w->WLogic;
  Data->SizeY = w->HLogic - ScrollBackLines;
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
