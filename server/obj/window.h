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

#ifdef TW_HAVE_SYS_TYPES_H
#include <sys/types.h> /* pid_t */
#endif

/* values returned by Fn_Twindow->FindBorder (modeled after STATE_*) */
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

struct Sremotedata {
  int Fd;
  pid_t ChildPid;
  uldat FdSlot; /* index in the FdList array (remote.c) */
};

struct SwindowFn {
  /* Twidget */
  TobjFn Fn_Obj;
  void (*DrawSelf)(Sdraw *D);
  Tgadget (*FindGadgetByCode)(Twindow Parent, udat Code);
  void (*SetXY)(Twindow, dat X, dat Y);
  void (*SetFill)(Twindow, tcell Fill);
  Twidget (*Focus)(Twindow);
  Twidget (*KbdFocus)(Twindow);
  void (*Map)(Twindow, Twidget Parent);
  void (*UnMap)(Twindow);
  void (*MapTopReal)(Twindow, Tscreen);
  void (*Raise)(Twindow);
  void (*Lower)(Twindow);
  void (*Own)(Twindow, Tmsgport);
  void (*DisOwn)(Twindow);
  void (*RecursiveDelete)(Twindow, Tmsgport);
  void (*Expose)(Twindow, dat XWidth, dat YWidth, dat Left, dat Up, const char *, const trune *,
                 const tcell *);
  /* Twindow */
  TwidgetFn Fn_Widget;
  byte (*TtyWriteCharset)(Twindow, uldat Len, const char *charset_bytes);
  byte (*TtyWriteUtf8)(Twindow, uldat Len, const char *utf8_bytes);
  byte (*TtyWriteTRune)(Twindow, uldat Len, const trune *runes);
  byte (*TtyWriteTCell)(Twindow, dat x, dat y, uldat Len, const tcell *cells);

  byte (*RowWriteCharset)(Twindow, uldat Len, const char *charset_bytes);
  byte (*RowWriteUtf8)(Twindow, uldat Len, const char *utf8_bytes);
  byte (*RowWriteTRune)(Twindow, uldat Len, const trune *runes);
  byte (*RowWriteTCell)(Twindow, dat x, dat y, uldat Len, const tcell *cells);

  void (*GotoXY)(Twindow, ldat X, ldat Y);
  void (*SetTitle)(Twindow, dat titlelen, char *title);
  void (*SetColText)(Twindow, tcolor ColText);
  void (*SetColors)(Twindow, udat Bitmap, tcolor ColGadgets, tcolor ColArrows, tcolor ColBars,
                    tcolor ColTabs, tcolor ColBorder, tcolor ColText, tcolor ColSelect,
                    tcolor ColDisabled, tcolor ColSelectDisabled);
  void (*Configure)(Twindow, byte Bitmap, dat Left, dat Up, dat MinXWidth, dat MinYWidth,
                    dat MaxXWidth, dat MaxYWidth);
  Twindow (*Create4Menu)(Tmenu);
  tpos (*FindBorder)(Twindow, dat u, dat v, byte Border, tcell *PtrAttr);
  Trow (*FindRow)(Twindow, ldat RowN);
  Trow (*FindRowByCode)(Twindow, udat Code, ldat *NumRow);
};

struct Swindow : public Swidget {
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
  trune const *Charset; /* the byte -> trune translation to use */

private:
  Twindow Init(Tmsgport owner, dat titlelen, const char *title, const tcolor *coltitle, Tmenu menu,
               tcolor coltext, uldat cursortype, uldat attr, uldat flags, dat xwidth, dat ywidth,
               dat scrollbacklines);

public:
  static Twindow Create(Tmsgport owner, dat titlelen, const char *title, const tcolor *coltitle,
                        Tmenu menu, tcolor coltext, uldat cursortype, uldat attr, uldat flags,
                        dat xwidth, dat ywidth, dat scrollbacklines);
  static Twindow Create4Menu(Tmenu);

  /* Tobj */
  virtual void Delete() OVERRIDE;
  virtual void ChangeField(udat field, uldat clear_mask, uldat xor_mask) OVERRIDE;

  /* Twindow */
  const TwindowFn fn() const {
    return (TwindowFn)Fn;
  }
  const TwidgetFn widget_fn() const {
    return fn()->Fn_Widget;
  }

  byte TtyWriteCharset(uldat len, const char *charset_bytes) {
    return fn()->TtyWriteCharset(this, len, charset_bytes);
  }
  byte TtyWriteUtf8(uldat len, const char *utf8_bytes) {
    return fn()->TtyWriteUtf8(this, len, utf8_bytes);
  }
  byte TtyWriteTRune(uldat len, const trune *runes) {
    return fn()->TtyWriteTRune(this, len, runes);
  }
  byte TtyWriteTCell(dat x, dat y, uldat len, const tcell *cells) {
    return fn()->TtyWriteTCell(this, x, y, len, cells);
  }

  byte RowWriteCharset(uldat len, const char *charset_bytes) {
    return fn()->RowWriteCharset(this, len, charset_bytes);
  }
  byte RowWriteUtf8(uldat len, const char *utf8_bytes) {
    return fn()->RowWriteUtf8(this, len, utf8_bytes);
  }
  byte RowWriteTRune(uldat len, const trune *runes) {
    return fn()->RowWriteTRune(this, len, runes);
  }
  byte RowWriteTCell(dat x, dat y, uldat len, const tcell *attr) {
    return fn()->RowWriteTCell(this, x, y, len, attr);
  }

  void GotoXY(ldat x, ldat y) {
    fn()->GotoXY(this, x, y);
  }
  void SetTitle(dat titlelen, char *title) {
    fn()->SetTitle(this, titlelen, title);
  }
  void SetColText(tcolor coltext) {
    fn()->SetColText(this, coltext);
  }
  void SetColors(udat bitmap, tcolor colgadgets, tcolor colarrows, tcolor colbars, tcolor coltabs,
                 tcolor colborder, tcolor coltext, tcolor colselect, tcolor coldisabled,
                 tcolor colselectdisabled) {
    fn()->SetColors(this, bitmap, colgadgets, colarrows, colbars, coltabs, colborder, coltext,
                    colselect, coldisabled, colselectdisabled);
  }
  void Configure(byte bitmap, dat left, dat up, dat minxwidth, dat minywidth, dat maxxwidth,
                 dat maxywidth) {
    fn()->Configure(this, bitmap, left, up, minxwidth, minywidth, maxxwidth, maxywidth);
  }
  tpos FindBorder(dat u, dat v, byte border, tcell *ptrattr) {
    return fn()->FindBorder(this, u, v, border, ptrattr);
  }
  Trow FindRow(ldat rown) {
    return fn()->FindRow(this, rown);
  }
  Trow FindRowByCode(udat code, ldat *numrow) {
    return fn()->FindRowByCode(this, code, numrow);
  }
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
