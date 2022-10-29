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
  Twidget TopW;
  Twidget W;
  Twidget OnlyW;
  ldat Left, Up, Rgt, Dwn; /* Twidget corners position on Screen */
  ldat X1, Y1, X2, Y2;     /* screen area to draw */
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
  uldat Magic;
  void (*Insert)(Twidget self, Twidget parent, Twidget Prev, Twidget Next);
  void (*Remove)(Twidget self);
  void (*Delete)(Twidget self);
  void (*ChangeField)(Twidget self, udat field, uldat CLEARMask, uldat XORMask);
  /* Twidget */
  TobjFn Fn_Obj; /* backup of overloaded functions */
  void (*DrawSelf)(draw_ctx *d);
  Twidget (*FindWidgetAt)(Twidget self, dat x, dat y);
  Tgadget (*FindGadgetByCode)(Twidget self, udat code);
  void (*SetXY)(Twidget self, dat x, dat y);
  void (*SetFill)(Twidget self, tcell fill);
  Twidget (*Focus)(Twidget self);
  Twidget (*KbdFocus)(Twidget self);
  void (*Map)(Twidget self, Twidget parent);
  void (*UnMap)(Twidget self);
  void (*MapTopReal)(Twidget self, screen scr);
  void (*Raise)(Twidget self);
  void (*Lower)(Twidget self);
  void (*Own)(Twidget self, Tmsgport port);
  void (*DisOwn)(Twidget self);
  void (*RecursiveDelete)(Twidget self, Tmsgport port);
  void (*Expose)(Twidget self, dat xwidth, dat ywidth, dat left, dat up, dat pitch, const char *,
                 const trune *, const tcell *);
  byte (*InstallHook)(Twidget, HookFn, HookFn *where);
  void (*RemoveHook)(Twidget, HookFn, HookFn *where);
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
  HookFn Hook, *WhereHook;
  HookFn MapUnMapHook;
  msg MapQueueMsg;
  tcell USE_Fill;
  union {
    struct s_wE E;
  } USE;

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  static Twidget Create(Tmsgport Owner, dat XWidth, dat YWidth, uldat Attr, uldat Flags, dat Left,
                        dat Up, tcell USE_Fill);
  Twidget Init(Tmsgport Owner, dat XWidth, dat YWidth, uldat Attr, uldat Flags, dat Left, dat Up,
               tcell USE_Fill);
  void Insert(Twidget parent, Twidget prev, Twidget next) {
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
  /* Twidget */
  void DrawSelf(draw_ctx *D) {
    Fn->DrawSelf(D);
  }
  Twidget FindWidgetAt(dat x, dat y) {
    return Fn->FindWidgetAt(this, x, y);
  }
  Tgadget FindGadgetByCode(udat code) {
    return Fn->FindGadgetByCode(this, code);
  }
  void SetXY(dat x, dat y) {
    Fn->SetXY(this, x, y);
  }
  void SetFill(tcell fill) {
    Fn->SetFill(this, fill);
  }
  Twidget Focus() {
    return Fn->Focus(this);
  }
  Twidget KbdFocus() {
    return Fn->KbdFocus(this);
  }
  void Map(Twidget parent) {
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
  byte InstallHook(HookFn hook, HookFn *where) {
    return Fn->InstallHook(this, hook, where);
  }
  void RemoveHook(HookFn hook, HookFn *where) {
    Fn->RemoveHook(this, hook, where);
  }
};

#endif /* TWIN_WIDGET_H */
