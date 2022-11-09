/*
 *  widget.h  --  declare server class Swidget
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_WIDGET_H
#define TWIN_WIDGET_H

#include "obj/obj.h"
#include "stl/macros.h" // OVERRIDE

/* WIDGET_USEEXPOSE  USE.E.Flags: */
#define WIDGET_USEEXPOSE_TEXT 1
#define WIDGET_USEEXPOSE_TRUNE 2
#define WIDGET_USEEXPOSE_TCELL 4

/* Widget->Attr */
enum widget_attr /*: udat*/ {
  /*
   * ask the server to send events even for mouse motion without any pressed button.
   * works only if WIDGET_WANT_MOUSE is set too.
   */
  WIDGET_WANT_MOUSE_MOTION = 0x0001,
  WIDGET_WANT_KEYS = 0x0002,
  WIDGET_WANT_MOUSE = 0x0004,
  WIDGET_WANT_CHANGES = 0x0008,
  WIDGET_AUTO_FOCUS = 0x0010,
};

/* Widget->Flags */
enum widget_flag /*: udat*/ {
  WIDGETFL_USEEXPOSE = 0x02,
  WIDGETFL_USEFILL = 0x03,
  WIDGETFL_USEANY = 0x07, /* mask of all above ones */

  WIDGETFL_NOTVISIBLE = 0x8000,
};

#define w_USE(w, USExxx) (((w)->Flags & WIDGETFL_USEANY) == CAT(WIDGETFL_, USExxx))

struct s_SB { /* for SCREENFL_USEBG screens */
  dat BgWidth, BgHeight;
  tcell *Bg;
};

struct s_WC { /* for WINDOWFL_USECONTENTS windows */
  tcell *Contents;
  ttydata *TtyData;
  ldat HSplit;
};

struct s_WE { /* for WIDGET_USEEXPOSE widgets */
  union {
    const char *Text;
    const trune *TRune;
    const tcell *TCell;
  } E;
  dat Flags, Pitch;
  ldat X1, Y1, X2, Y2;
};

struct s_WR { /* for WINDOWFL_USEROWS windows */
  Trow FirstRow, LastRow;
  Trow RowOne, RowSplit;       /*RESERVED: used to optimize the drawing on Tscreen */
  ldat NumRowOne, NumRowSplit; /*RESERVED: updated automatically by WriteRow. To insert */
                               /*or remove manually rows, you must zero out NumRowOne */
                               /*and NumRowSplit forcing twin to recalculate them */
};

struct s_GT { /* for GADGETFL_USETEXT gadgets */
  trune *Text[4];
  tcolor *Color[4];
};

struct Sdraw {
  Sdraw *Next;
  Tscreen Screen;
  Twidget TopW;
  Twidget W;
  Twidget OnlyW;
  ldat Left, Up, Rgt, Dwn; /* Twidget corners position on screen */
  ldat X1, Y1, X2, Y2;     /* Tscreen area to draw */
  dat DWidth;
  dat DHeight;
  bool NoChildren;
  bool BorderDone;
  bool Shaded;

  bool Init(Twidget w, dat x1, dat y1, dat x2, dat y2, bool shaded);
  bool InitAbsolute(Twidget w, dat x1, dat y1, dat x2, dat y2, bool shaded);

  void Draw();
  void DrawArea();
};

struct SwidgetFn {
  /* Twidget */
  TobjFn Fn_Obj; /* backup of overloaded functions */
  Twidget (*KbdFocus)(Twidget);
  void (*Map)(Twidget self, Twidget parent);
  void (*UnMap)(Twidget self);
  void (*MapTopReal)(Twidget self, Tscreen screen);
  void (*Raise)(Twidget self);
  void (*Lower)(Twidget self);
  void (*Own)(Twidget self, Tmsgport port);
  void (*DisOwn)(Twidget self);
  void (*RecursiveDelete)(Twidget self, Tmsgport port);
  void (*Expose)(Twidget self, dat xwidth, dat ywidth, dat left, dat up, dat pitch, const char *,
                 const trune *, const tcell *);
};

struct Swidget : public Sobj {
  TwidgetFn Fn;
  Twidget Prev, Next; /* list in the same parent */
  Twidget Parent;     /* where this Twidget sits */
  /* Twidget */
  Twidget FirstW, LastW; /* list of children */
  Twidget SelectW;       /* selected child */
  dat Left, Up, XWidth, YWidth;
  uldat Attr;
  uldat Flags;
  ldat XLogic, YLogic;
  Twidget O_Prev, O_Next; /* list with the same Tmsgport (owner) */
  Tmsgport Owner;
  HookFn ShutDownHook; /* hooks for this Twidget */
  HookFn Hook;
  HookData *WhereHook;
  HookFn MapUnMapHook;
  Tmsg MapQueueMsg;
  tcell USE_Fill;
  union {
    s_SB B; // Sscreen only
    s_WE E;
    s_WC C; // Swindow only
    s_WR R; // Swindow only
    s_GT T; // Sgadget only
  } USE;

  Twidget Init(Tmsgport Owner, dat XWidth, dat YWidth, uldat Attr, uldat Flags, dat Left, dat Up,
               tcell USE_Fill, e_id class_id);
  static Twidget Create(Tmsgport Owner, dat XWidth, dat YWidth, uldat Attr, uldat Flags, dat Left,
                        dat Up, tcell USE_Fill);

  /* Tobj */
  virtual void Delete() OVERRIDE;
  virtual void Remove() OVERRIDE;
  virtual void ChangeField(udat field, uldat clear_mask, uldat xor_mask) OVERRIDE;

  /* Twidget */
  void Insert(Twidget parent, Twidget prev, Twidget next);
  virtual void DrawSelf(Sdraw *d);    // defined in draw.cpp
  Twidget FindWidgetAt(dat x, dat y); // defined in draw.cpp
  Tgadget FindGadgetByCode(udat code);
  virtual void SetXY(dat x, dat y);
  void SetFill(tcell fill);

  virtual Twidget Focus();         // return previously focused widget
  static Twidget Focus(Twidget w); // return previously focused widget
  void UnFocus();
  virtual Twidget KbdFocus();         // return previously focused widget
  static Twidget KbdFocus(Twidget w); // return previously focused widget

  void Map(Twidget parent) {
    Fn->Map(this, parent);
  }
  void UnMap() {
    Fn->UnMap(this);
  }
  void MapTopReal(Tscreen screen) {
    Fn->MapTopReal(this, screen);
  }
  void Raise() {
    Fn->Raise(this);
  }
  void Lower() {
    Fn->Lower(this);
  }
  void Own(Tmsgport port) {
    Fn->Own(this, port);
  }
  void DisOwn() {
    Fn->DisOwn(this);
  }
  void RecursiveDelete(Tmsgport port) {
    Fn->RecursiveDelete(this, port);
  }
  void Expose(dat xwidth, dat ywidth, dat left, dat up, dat pitch, const char *ascii,
              const trune *runes, const tcell *cells) {
    Fn->Expose(this, xwidth, ywidth, left, up, pitch, ascii, runes, cells);
  }
  bool InstallHook(HookFn hook, HookData *where);
  void RemoveHook(HookFn hook, HookData *where);

protected:
  virtual void InsertWidget(Tobj parent, Twidget prev, Twidget next);
};

#endif /* TWIN_WIDGET_H */
