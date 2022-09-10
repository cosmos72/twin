/*
 *  window.h  --  declare server class s_window
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

#include "twautoconf.h"

#include "obj/widget.h"

#ifdef TW_HAVE_SYS_TYPES_H
#include <sys/types.h> /* pid_t */
#endif

/* values returned by Fn_window->FindBorder (modeled after STATE_*) */
enum tpos : byte {
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

struct s_remotedata {
  int Fd;
  pid_t ChildPid;
  uldat FdSlot; /* index in the FdList array (remote.c) */
};

struct s_WR { /* for WINDOWFL_USEROWS windows */
  row FirstRow, LastRow;
  row RowOne, RowSplit;        /*RESERVED: used to optimize the drawing on screen */
  ldat NumRowOne, NumRowSplit; /*RESERVED: updated automatically by WriteRow. To insert */
                               /*or remove manually rows, you must zero out NumRowOne */
                               /*and NumRowSplit forcing twin to recalculate them */
};

struct s_WC { /* for WINDOWFL_USECONTENTS windows */
  tcell *Contents;
  ttydata *TtyData;
  ldat HSplit;
};

struct s_fn_window {
  uldat Magic;
  void (*Insert)(window, widget Parent, widget Prev, widget Next);
  void (*Remove)(window);
  void (*Delete)(window);
  void (*ChangeField)(window, udat field, uldat CLEARMask, uldat XORMask);
  /* widget */
  fn_obj Fn_Obj;
  void (*DrawSelf)(draw_ctx *D);
  widget (*FindWidgetAt)(window Parent, dat X, dat Y);
  gadget (*FindGadgetByCode)(window Parent, udat Code);
  void (*SetXY)(window, dat X, dat Y);
  void (*SetFill)(window, tcell Fill);
  widget (*Focus)(window);
  widget (*KbdFocus)(window);
  void (*Map)(window, widget Parent);
  void (*UnMap)(window);
  void (*MapTopReal)(window, screen);
  void (*Raise)(window);
  void (*Lower)(window);
  void (*Own)(window, msgport);
  void (*DisOwn)(window);
  void (*RecursiveDelete)(window, msgport);
  void (*Expose)(window, dat XWidth, dat YWidth, dat Left, dat Up, const char *, const trune *,
                 const tcell *);
  byte (*InstallHook)(window, fn_hook, fn_hook *Where);
  void (*RemoveHook)(window, fn_hook, fn_hook *Where);
  /* window */
  fn_widget Fn_Widget;
  byte (*TtyWriteCharset)(window, uldat Len, const char *charset_bytes);
  byte (*TtyWriteUtf8)(window, uldat Len, const char *utf8_bytes);
  byte (*TtyWriteTRune)(window, uldat Len, const trune *runes);
  byte (*TtyWriteTCell)(window, dat x, dat y, uldat Len, const tcell *cells);

  byte (*RowWriteCharset)(window, uldat Len, const char *charset_bytes);
  byte (*RowWriteUtf8)(window, uldat Len, const char *utf8_bytes);
  byte (*RowWriteTRune)(window, uldat Len, const trune *runes);
  byte (*RowWriteTCell)(window, dat x, dat y, uldat Len, const tcell *cells);

  void (*GotoXY)(window, ldat X, ldat Y);
  void (*SetTitle)(window, dat titlelen, char *title);
  void (*SetColText)(window, tcolor ColText);
  void (*SetColors)(window, udat Bitmap, tcolor ColGadgets, tcolor ColArrows, tcolor ColBars,
                    tcolor ColTabs, tcolor ColBorder, tcolor ColText, tcolor ColSelect,
                    tcolor ColDisabled, tcolor ColSelectDisabled);
  void (*Configure)(window, byte Bitmap, dat Left, dat Up, dat MinXWidth, dat MinYWidth,
                    dat MaxXWidth, dat MaxYWidth);
  window (*Create4Menu)(menu);
  tpos (*FindBorder)(window, dat u, dat v, byte Border, tcell *PtrAttr);
  row (*FindRow)(window, ldat RowN);
  row (*FindRowByCode)(window, udat Code, ldat *NumRow);
};

struct s_window : public s_obj {
  fn_window Fn;
  widget Prev, Next; /* list in the same parent */
  widget Parent;     /* where this window sits */
  /* widget */
  widget FirstW, LastW; /* list of children */
  widget SelectW;       /* selected child */
  dat Left, Up, XWidth, YWidth;
  uldat Attr;
  uldat Flags;
  ldat XLogic, YLogic;
  widget O_Prev, O_Next; /* list with the same msgport (owner) */
  msgport Owner;
  fn_hook ShutDownHook; /* hooks for this widget */
  fn_hook Hook, *WhereHook;
  fn_hook MapUnMapHook;
  msg MapQueueMsg;
  tcell USE_Fill;
  union {
    struct s_WR R;
    struct s_WC C;
    struct s_wE E;
  } USE;
  /* window */
  menu Menu;
  menuitem MenuItem; /* from which the window depends */
  dat NameLen;
  char *Name;
  tcolor *ColName;
  trune *BorderPattern[2];
  remotedata RemoteData;
  ldat CurX, CurY;
  ldat XstSel, YstSel, XendSel, YendSel;
  tcolor ColGadgets, ColArrows, ColBars, ColTabs, ColBorder, ColText, ColSelect, ColDisabled,
      ColSelectDisabled;
  uldat State;
  uldat CursorType;
  dat MinXWidth, MinYWidth;
  dat MaxXWidth, MaxYWidth;
  ldat WLogic, HLogic;  /* window interior logic size */
  trune const *Charset; /* the byte -> trune translation to use */

  /* obj */
  static window Create(msgport owner, dat titlelen, const char *title, const tcolor *coltitle,
                       menu menu, tcolor coltext, uldat cursortype, uldat attr, uldat flags,
                       dat xwidth, dat ywidth, dat scrollbacklines);
  static window Create4Menu(menu);
  window Init(msgport owner, dat titlelen, const char *title, const tcolor *coltitle, menu menu,
              tcolor coltext, uldat cursortype, uldat attr, uldat flags, dat xwidth, dat ywidth,
              dat scrollbacklines);
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(widget parent, widget prev, widget next) {
    Fn->Insert(this, parent, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  void ChangeField(udat field, uldat clear_mask, uldat xor_mask) {
    Fn->ChangeField(this, field, clear_mask, xor_mask);
  }
  /* widget */
  void DrawSelf(draw_ctx *D) {
    Fn->DrawSelf(D);
  }
  widget FindWidgetAt(dat x, dat y) {
    return Fn->FindWidgetAt(this, x, y);
  }
  gadget FindGadgetByCode(udat code) {
    return Fn->FindGadgetByCode(this, code);
  }
  void SetXY(dat x, dat y) {
    Fn->SetXY(this, x, y);
  }
  void SetFill(tcell fill) {
    Fn->SetFill(this, fill);
  }
  widget Focus() {
    return Fn->Focus(this);
  }
  widget KbdFocus() {
    return Fn->KbdFocus(this);
  }
  void Map(widget parent) {
    Fn->Map(this, parent);
  }
  void UnMap() {
    Fn->UnMap(this);
  }
  void MapTopReal(screen scr) {
    Fn->MapTopReal(this, scr);
  }
  void Raise() {
    Fn->Raise(this);
  }
  void Lower() {
    Fn->Lower(this);
  }
  void Own(msgport port) {
    Fn->Own(this, port);
  }
  void DisOwn() {
    Fn->DisOwn(this);
  }
  void RecursiveDelete(msgport port) {
    Fn->RecursiveDelete(this, port);
  }
  void Expose(dat xwidth, dat ywidth, dat left, dat up, const char *ascii, const trune *runes,
              const tcell *cells) {
    Fn->Expose(this, xwidth, ywidth, left, up, ascii, runes, cells);
  }
  byte InstallHook(fn_hook hook, fn_hook *where) {
    return Fn->InstallHook(this, hook, where);
  }
  void RemoveHook(fn_hook hook, fn_hook *where) {
    Fn->RemoveHook(this, hook, where);
  }
  /* window */
  byte TtyWriteCharset(uldat len, const char *charset_bytes) {
    return Fn->TtyWriteCharset(this, len, charset_bytes);
  }
  byte TtyWriteUtf8(uldat len, const char *utf8_bytes) {
    return Fn->TtyWriteUtf8(this, len, utf8_bytes);
  }
  byte TtyWriteTRune(uldat len, const trune *runes) {
    return Fn->TtyWriteTRune(this, len, runes);
  }
  byte TtyWriteTCell(dat x, dat y, uldat len, const tcell *cells) {
    return Fn->TtyWriteTCell(this, x, y, len, cells);
  }

  byte RowWriteCharset(uldat len, const char *charset_bytes) {
    return Fn->RowWriteCharset(this, len, charset_bytes);
  }
  byte RowWriteUtf8(uldat len, const char *utf8_bytes) {
    return Fn->RowWriteUtf8(this, len, utf8_bytes);
  }
  byte RowWriteTRune(uldat len, const trune *runes) {
    return Fn->RowWriteTRune(this, len, runes);
  }
  byte RowWriteTCell(dat x, dat y, uldat len, const tcell *attr) {
    return Fn->RowWriteTCell(this, x, y, len, attr);
  }

  void GotoXY(ldat x, ldat y) {
    Fn->GotoXY(this, x, y);
  }
  void SetTitle(dat titlelen, char *title) {
    Fn->SetTitle(this, titlelen, title);
  }
  void SetColText(tcolor coltext) {
    Fn->SetColText(this, coltext);
  }
  void SetColors(udat bitmap, tcolor colgadgets, tcolor colarrows, tcolor colbars, tcolor coltabs,
                 tcolor colborder, tcolor coltext, tcolor colselect, tcolor coldisabled,
                 tcolor colselectdisabled) {
    Fn->SetColors(this, bitmap, colgadgets, colarrows, colbars, coltabs, colborder, coltext,
                  colselect, coldisabled, colselectdisabled);
  }
  void Configure(byte bitmap, dat left, dat up, dat minxwidth, dat minywidth, dat maxxwidth,
                 dat maxywidth) {
    Fn->Configure(this, bitmap, left, up, minxwidth, minywidth, maxxwidth, maxywidth);
  }
  tpos FindBorder(dat u, dat v, byte border, tcell *ptrattr) {
    return Fn->FindBorder(this, u, v, border, ptrattr);
  }
  row FindRow(ldat rown) {
    return Fn->FindRow(this, rown);
  }
  row FindRowByCode(udat code, ldat *numrow) {
    return Fn->FindRowByCode(this, code, numrow);
  }
};

/* Window->Attr */
enum window_attr : udat {
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
enum window_flag : udat {
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

#define W_USE(W, USExxx) (((W)->Flags & WINDOWFL_USEANY) == CAT(WINDOWFL_, USExxx))

/* Window->State */
enum window_state : uldat {
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
enum window_cursor_type : byte {
  NOCURSOR = 1,
  LINECURSOR = 2,
  SOLIDCURSOR = 8,
};

/* window size limits */
#define MIN_XWIN 5
#define MIN_YWIN 2

#endif /* TWIN_WINDOW_H */
