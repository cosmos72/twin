/*
 *  widget.h  --  declare server class s_widget
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

struct s_wE { /* for WIDGET_USEEXPOSE widgets */
  union {
    const char *Text;
    const trune *TRune;
    const tcell *TCell;
  } E;
  dat Flags, Pitch;
  ldat X1, Y1, X2, Y2;
};

struct s_draw_ctx {
  draw_ctx *Next;
  screen Screen;
  widget TopW;
  widget W;
  widget OnlyW;
  ldat Left, Up, Rgt, Dwn; /* widget corners position on Screen */
  ldat X1, Y1, X2, Y2;     /* screen area to draw */
  dat DWidth;
  dat DHeight;
  bool NoChildren;
  bool BorderDone;
  bool Shaded;

  bool Init(widget w, dat x1, dat y1, dat x2, dat y2, bool shaded);
  bool InitAbsolute(widget w, dat x1, dat y1, dat x2, dat y2, bool shaded);

  void Draw();
  void DrawArea();
};

struct s_fn_widget {
  uldat Magic;
  void (*Insert)(widget self, widget parent, widget Prev, widget Next);
  void (*Remove)(widget self);
  void (*Delete)(widget self);
  void (*ChangeField)(widget self, udat field, uldat CLEARMask, uldat XORMask);
  /* widget */
  fn_obj Fn_Obj; /* backup of overloaded functions */
  void (*DrawSelf)(draw_ctx *d);
  widget (*FindWidgetAt)(widget self, dat x, dat y);
  gadget (*FindGadgetByCode)(widget self, udat code);
  void (*SetXY)(widget self, dat x, dat y);
  void (*SetFill)(widget self, tcell fill);
  widget (*Focus)(widget self);
  widget (*KbdFocus)(widget self);
  void (*Map)(widget self, widget parent);
  void (*UnMap)(widget self);
  void (*MapTopReal)(widget self, screen scr);
  void (*Raise)(widget self);
  void (*Lower)(widget self);
  void (*Own)(widget self, msgport port);
  void (*DisOwn)(widget self);
  void (*RecursiveDelete)(widget self, msgport port);
  void (*Expose)(widget self, dat xwidth, dat ywidth, dat left, dat up, dat pitch, const char *,
                 const trune *, const tcell *);
  byte (*InstallHook)(widget, fn_hook, fn_hook *where);
  void (*RemoveHook)(widget, fn_hook, fn_hook *where);
};

struct s_widget : public s_obj {
  fn_widget Fn;
  widget Prev, Next; /* list in the same parent */
  widget Parent;     /* where this widget sits */
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
    struct s_wE E;
  } USE;

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  static widget Create(msgport Owner, dat XWidth, dat YWidth, uldat Attr, uldat Flags, dat Left,
                       dat Up, tcell USE_Fill);
  widget Init(msgport Owner, dat XWidth, dat YWidth, uldat Attr, uldat Flags, dat Left, dat Up,
              tcell USE_Fill);
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
  void Expose(dat xwidth, dat ywidth, dat left, dat up, dat pitch, const char *ascii,
              const trune *runes, const tcell *cells) {
    Fn->Expose(this, xwidth, ywidth, left, up, pitch, ascii, runes, cells);
  }
  byte InstallHook(fn_hook hook, fn_hook *where) {
    return Fn->InstallHook(this, hook, where);
  }
  void RemoveHook(fn_hook hook, fn_hook *where) {
    Fn->RemoveHook(this, hook, where);
  }
};

#endif /* TWIN_WIDGET_H */
