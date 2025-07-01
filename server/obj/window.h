/*
 *  window.h  --  declare server class Swindow
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_WINDOW_H
#define TWIN_WINDOW_H

#include <Tw/autoconf.h>

#include "obj/widget.h"
#include "obj/fn.h"
#include "resize.h" // RowWrite*Window()

#ifdef TW_HAVE_SYS_TYPES_H
#include <sys/types.h> /* pid_t */
#endif

/* values returned by Twindow::FindBorder (modeled after STATE_*) */
enum tpos /*: byte*/ {
  POS_TITLE = 10,
  POS_SIDE_LEFT = 11,
  POS_SIDE_UP = 12,
  POS_SIDE_RIGHT = 13,
  POS_SIDE_DOWN = 14,
  POS_BUTTON_RESIZE = 15,
  POS_X_BAR_BACK = 16,
  POS_X_BAR_FWD = 17,
  POS_X_TAB = 18,
  POS_X_ARROW_BACK = 19,
  POS_X_ARROW_FWD = 20,
  POS_Y_BAR_BACK = 21,
  POS_Y_BAR_FWD = 22,
  POS_Y_TAB = 23,
  POS_Y_ARROW_BACK = 24,
  POS_Y_ARROW_FWD = 25,
  POS_INSIDE = 26,
  POS_MENU = 27,
  POS_BUTTON_SCREEN = 29,
  POS_ROOT = 30,
};

class Sremotedata {
public:
  int Fd;
  pid_t ChildPid;
  uldat FdSlot; /* index in the FdList array (remote.c) */
};

class SwindowFn {
public:
  /* Twidget */
  Twidget (*KbdFocus)(Twidget);
  /* Twindow */
  bool (*TtyWriteCharset)(Twindow, uldat len, const char *charset_bytes);
  bool (*TtyWriteUtf8)(Twindow, uldat len, const char *utf8_bytes);
  bool (*TtyWriteTRune)(Twindow, uldat len, const trune *runes);
  bool (*TtyWriteTCell)(Twindow, dat x, dat y, uldat len, const tcell *cells);
  tpos (*FindBorder)(Twindow w, dat u, dat v, byte border, tcell *ptrattr);
};

class Swindow : public Swidget {
public:
  /* Twindow */
  Tmenu Menu;
  Tmenuitem MenuItem; /* from which the Twindow depends */
  dat NameLen;
  char *Name;
  tcolor *ColName;
  trune *BorderPattern[2];
  Tremotedata RemoteData;
  ldat CurX, CurY;
  ldat XstSel, YstSel, XendSel, YendSel;
  tcolor ColGadgets, ColArrows, ColBars, ColTabs, ColBorder, ColText, ColSelect, ColDisabled,
      ColSelectDisabled;
  uldat State;
  uldat CursorType;
  dat MinXWidth, MinYWidth;
  dat MaxXWidth, MaxYWidth;
  ldat WLogic, HLogic;  /* Twindow interior logic size */
  const trune *Charset; /* the byte -> trune translation to use */

private:
  Twindow Init(Tmsgport owner, dat titlelen, const char *title, const tcolor *coltitle, Tmenu menu,
               tcolor coltext, uldat cursortype, uldat attr, uldat flags, dat xwidth, dat ywidth,
               dat scrollbacklines);

public:
  static Twindow Create(Tmsgport owner, dat titlelen, const char *title, const tcolor *coltitle,
                        Tmenu menu, tcolor coltext, uldat cursortype, uldat attr, uldat flags,
                        dat xwidth, dat ywidth, dat scrollbacklines);
  static Twindow Create4Menu(Tmenu menu);

  /* Tobj */
  virtual void Delete() OVERRIDE;
  virtual void ChangeField(udat field, uldat clear_mask, uldat xor_mask) OVERRIDE;

  /* Twidget */
  virtual void DrawSelf(Sdraw *d) OVERRIDE; // defined in draw.cpp
  virtual void SetXY(dat x, dat y) OVERRIDE;

  virtual void Expose(dat xwidth, dat ywidth, dat left, dat up, dat pitch, const char *ascii,
                      const trune *runes, const tcell *cells) OVERRIDE;

  /* Twindow */

  bool TtyWriteCharset(uldat len, const char *charset_bytes) {
    return Fn_Twindow->TtyWriteCharset(this, len, charset_bytes);
  }
  bool TtyWriteUtf8(uldat len, const char *utf8_bytes) {
    return Fn_Twindow->TtyWriteUtf8(this, len, utf8_bytes);
  }
  bool TtyWriteTRune(uldat len, const trune *runes) {
    return Fn_Twindow->TtyWriteTRune(this, len, runes);
  }
  bool TtyWriteTCell(dat x, dat y, uldat len, const tcell *cells) {
    return Fn_Twindow->TtyWriteTCell(this, x, y, len, cells);
  }

  bool RowWriteCharset(uldat len, const char *charset_bytes) {
    return RowWriteCharsetWindow(this, len, charset_bytes);
  }
  bool RowWriteUtf8(uldat len, const char *utf8_bytes) {
    return RowWriteUtf8Window(this, len, utf8_bytes);
  }
  bool RowWriteTRune(uldat len, const trune *runes) {
    return RowWriteTRuneWindow(this, len, runes);
  }
  bool RowWriteTCell(dat x, dat y, uldat len, const tcell *attr) {
    return RowWriteTCellWindow(this, x, y, len, attr);
  }

  void GotoXY(ldat x, ldat y);
  void SetTitle(dat titlelen, char *title); // title must be allocated mem, it will be retained
  void SetColText(tcolor coltext);
  void SetColors(udat bitmap, tcolor colgadgets, tcolor colarrows, tcolor colbars, tcolor coltabs,
                 tcolor colborder, tcolor coltext, tcolor colselect, tcolor coldisabled,
                 tcolor colselectdisabled);
  void Configure(byte bitmap, dat left, dat up, dat minxwidth, dat minywidth, dat maxxwidth,
                 dat maxywidth);
  tpos FindBorder(dat u, dat v, byte border, tcell *ptrattr) {
    return Fn_Twindow->FindBorder(this, u, v, border, ptrattr);
  }
  Trow FindRow(ldat row_i) const;
  Trow FindRowByCode(udat code, ldat *row_i) const;
};

/* Window->Attr */
enum window_attr /*: udat*/ {
  WINDOW_WANT_MOUSE_MOTION = WIDGET_WANT_MOUSE_MOTION, /* 0x0001 */
  WINDOW_WANT_KEYS = WIDGET_WANT_KEYS,                 /* 0x0002 */
  WINDOW_WANT_MOUSE = WIDGET_WANT_MOUSE,               /* 0x0004 */
  WINDOW_WANT_CHANGES = WIDGET_WANT_CHANGES,           /* 0x0008 */
  WINDOW_AUTO_FOCUS = WIDGET_AUTO_FOCUS,               /* 0x0010 */
  WINDOW_DRAG = 0x0100,
  WINDOW_RESIZE = 0x0200,
  WINDOW_CLOSE = 0x0400,
  WINDOW_ROLLED_UP = 0x0800,
  WINDOW_X_BAR = 0x1000,
  WINDOW_Y_BAR = 0x2000,
  WINDOW_AUTO_KEYS = 0x4000,
};

/* Window->Flags */
enum window_flag /*: udat*/ {
  WINDOWFL_USEROWS = 0x00, /* default */
  WINDOWFL_USECONTENTS = 0x01,
  WINDOWFL_USEEXPOSE = WIDGETFL_USEEXPOSE, /* 0x02 */
  WINDOWFL_USEFILL = WIDGETFL_USEFILL,     /* 0x03 */
  WINDOWFL_USEANY = WIDGETFL_USEANY,       /* 0x07 */
  WINDOWFL_CURSOR_ON = 0x10,
  WINDOWFL_MENU = 0x20,
  WINDOWFL_DISABLED = 0x40,
  WINDOWFL_BORDERLESS = 0x80,
  WINDOWFL_ROWS_INSERT = 0x0100,
  WINDOWFL_ROWS_DEFCOL = 0x0200,
  WINDOWFL_ROWS_SELCURRENT = 0x0400,
  WINDOWFL_NOTVISIBLE = 0x8000,
};

#define W_USE(w, USExxx) (((w)->Flags & WINDOWFL_USEANY) == CAT(WINDOWFL_, USExxx))

/* Window->State */
enum window_state /*: uldat*/ {
  X_BAR_SELECT = 0x0001,
  Y_BAR_SELECT = 0x0002,
  XY_BAR_SELECT = (X_BAR_SELECT | Y_BAR_SELECT),
  TAB_SELECT = 0x0004,
  PAGE_BACK_SELECT = 0x0008,
  PAGE_FWD_SELECT = 0x0010,
  ARROW_BACK_SELECT = 0x0020,
  ARROW_FWD_SELECT = 0x0040,
  SCROLL_ANY_SELECT =
      ARROW_BACK_SELECT | ARROW_FWD_SELECT | PAGE_BACK_SELECT | PAGE_FWD_SELECT | TAB_SELECT,
  WINDOW_FWDSEL = 0x0080,
  WINDOW_REVSEL = 0x0100,
  WINDOW_ANYSEL = (WINDOW_FWDSEL | WINDOW_REVSEL),
  WINDOW_DO_SEL = 0x0200,
  WINDOW_GADGET_PRESSED = 0x0400,
  BUTTON_FIRST_SELECT = 0x00400000u,
  BUTTON_LAST_SELECT = 0x80000000u,
  BUTTON_ANY_SELECT = 0xFFC00000u,
};

/*#define BUTTON_FIRST                0 */
/*#define BUTTON_CLOSE                0 */
/*#define BUTTON_LAST                9 */

#define BUTTON_MAX 10

/* Window->CursorType */
/* These come from linux/drivers/char/console.c */
enum window_cursor_type /*: byte*/ {
  NOCURSOR = 1,
  LINECURSOR = 2,
  SOLIDCURSOR = 8,
};

/* Twindow size limits */
#define MIN_XWIN 5
#define MIN_YWIN 2

#endif /* TWIN_WINDOW_H */
