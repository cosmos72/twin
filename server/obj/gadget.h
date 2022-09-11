/*
 *  gadget.h  --  declare server class s_gadget
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_GADGET_H
#define TWIN_GADGET_H

#include "obj/widget.h"

struct s_gT { /* for GADGETFL_USETEXT gadgets */
  trune *Text[4];
  tcolor *Color[4];
};

struct s_fn_gadget {
  e_id Magic;
  void (*Insert)(gadget, widget Parent, widget Prev, widget Next);
  void (*Remove)(gadget);
  void (*Delete)(gadget);
  void (*ChangeField)(gadget, udat field, uldat CLEARMask, uldat XORMask);
  /* widget */
  fn_obj Fn_Obj;
  void (*DrawSelf)(draw_ctx *D);
  widget (*FindWidgetAt)(gadget Parent, dat X, dat Y);
  gadget (*FindGadgetByCode)(gadget Parent, udat Code);
  void (*SetXY)(gadget, dat X, dat Y);
  void (*SetFill)(gadget, tcell Fill);
  widget (*Focus)(gadget);
  widget (*KbdFocus)(gadget);
  void (*Map)(gadget, widget Parent);
  void (*UnMap)(gadget);
  void (*MapTopReal)(gadget, screen);
  void (*Raise)(gadget);
  void (*Lower)(gadget);
  void (*Own)(gadget, msgport);
  void (*DisOwn)(gadget);
  void (*RecursiveDelete)(gadget, msgport);
  void (*Expose)(gadget, dat xwidth, dat ywidth, dat Left, dat Up, const char *, const trune *,
                 const tcell *);
  byte (*InstallHook)(gadget, fn_hook, fn_hook *Where);
  void (*RemoveHook)(gadget, fn_hook, fn_hook *Where);
  /* gadget */
  fn_widget Fn_Widget;
  gadget (*CreateEmptyButton)(msgport owner, dat xwidth, dat ywidth, tcolor BgCol);
  byte (*FillButton)(gadget g, widget Parent, udat Code, dat Left, dat Up, udat Flags,
                     const char *Text, tcolor Color, tcolor ColorDisabled);
  gadget (*CreateButton)(widget Parent, dat xwidth, dat ywidth, const char *Text, uldat Flags,
                         udat Code, tcolor BgCol, tcolor Col, tcolor ColDisabled, dat Left, dat Up);
  void (*WriteTexts)(gadget g, byte bitmap, dat xwidth, dat ywidth, const char *Text, dat Left,
                     dat Up);
  void (*WriteTRunes)(gadget g, byte bitmap, dat xwidth, dat ywidth, const trune *TRune, dat Left,
                      dat Up);
};

struct s_gadget : public s_obj {
  fn_gadget Fn;
  widget Prev, Next;
  widget Parent;
  /* widget */
  widget FirstW, LastW; /* list of children */
  widget SelectW;       /* selected child */
  dat Left, Up, XWidth, YWidth;
  uldat Attr;
  uldat Flags;
  ldat XLogic, YLogic;
  widget O_Prev, O_Next; /* list in the same msgport (owner) */
  msgport Owner;
  fn_hook ShutDownHook; /* hooks for this widget */
  fn_hook Hook, *WhereHook;
  fn_hook MapUnMapHook;
  msg MapQueueMsg;
  tcell USE_Fill;
  union {
    struct s_gT T;
    struct s_wE E;
  } USE;
  /* gadget */
  tcolor ColText, ColSelect, ColDisabled, ColSelectDisabled;
  udat Code;
  gadget G_Prev, G_Next; /* list in the same ggroup */
  ggroup Group;

  static gadget Create(msgport owner, widget Parent, dat xwidth, dat ywidth, const char *TextNormal,
                       uldat Attr, uldat Flags, udat Code, tcolor ColText, tcolor ColTextSelect,
                       tcolor ColTextDisabled, tcolor ColTextSelectDisabled, dat Left, dat Up);
  gadget Init(msgport owner, widget Parent, dat xwidth, dat ywidth, const char *TextNormal,
              uldat Attr, uldat Flags, udat Code, tcolor ColText, tcolor ColTextSelect,
              tcolor ColTextDisabled, tcolor ColTextSelectDisabled, dat Left, dat Up);

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
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
  /* gadget */
  byte FillButton(widget parent, udat code, dat left, dat up, udat flags, const char *text,
                  tcolor color, tcolor colordisabled) {
    return Fn->FillButton(this, parent, code, left, up, flags, text, color, colordisabled);
  }
  gadget CreateEmptyButton(msgport owner, dat xwidth, dat ywidth, tcolor bgcol) {
    return Fn->CreateEmptyButton(owner, xwidth, ywidth, bgcol);
  }
  gadget CreateButton(widget parent, dat xwidth, dat ywidth, const char *text, uldat flags,
                      udat code, tcolor bgcol, tcolor col, tcolor coldisabled, dat left, dat up) {
    return Fn->CreateButton(parent, xwidth, ywidth, text, flags, code, bgcol, col, coldisabled,
                            left, up);
  }
  void WriteTexts(byte bitmap, dat xwidth, dat ywidth, const char *text, dat left, dat up) {
    Fn->WriteTexts(this, bitmap, xwidth, ywidth, text, left, up);
  }
  void WriteTRunes(byte bitmap, dat xwidth, dat ywidth, const trune *runes, dat left, dat up) {
    Fn->WriteTRunes(this, bitmap, xwidth, ywidth, runes, left, up);
  }
};

/* Gadget->Attr */
enum gadget_attr : udat {
  GADGET_WANT_MOUSE_MOTION = WIDGET_WANT_MOUSE_MOTION, /* 0x0001 */
  GADGET_WANT_KEYS = WIDGET_WANT_KEYS,                 /* 0x0002 */
  GADGET_WANT_MOUSE = WIDGET_WANT_MOUSE,               /* 0x0004 */
  GADGET_WANT_CHANGES = WIDGET_WANT_CHANGES,           /* 0x0008 */
  GADGET_AUTO_FOCUS = WIDGET_AUTO_FOCUS,               /* 0x0010 */
};

/* Gadget->Flags */
enum gadget_flag : udat {
  GADGETFL_USETEXT = 0x00,                 /* default */
  GADGETFL_USEEXPOSE = WIDGETFL_USEEXPOSE, /* 0x02 */
  GADGETFL_USEFILL = WIDGETFL_USEFILL,     /* 0x03 */
  GADGETFL_USEANY = WIDGETFL_USEANY,       /* 0x07 */

  /* remember this gadget is a button, so that SetText() does not ruin the shadow */
  GADGETFL_BUTTON = 0x0010,
  GADGETFL_DISABLED = 0x0020,
  GADGETFL_TEXT_DEFCOL = 0x0040,
  /* this makes the gadget 'checkable' : can be in 'checked' or 'unchecked' state.
   * also necessary to put the gadget in a ggroup */
  GADGETFL_TOGGLE = 0x0080,
  GADGETFL_PRESSED = 0x0100,

  GADGETFL_NOTVISIBLE = 0x8000,
};

#define G_USE(g, USExxx) (((g)->Flags & GADGETFL_USEANY) == CAT(GADGETFL_, USExxx))

/*              NOTE :
 *
 * the USE.T fields (Contents) of a gadget are used as follows:
 * Text[0]==TextNormal;      (mandatory)
 * Text[1]==TextSelect;        (if not present, use ...[0])
 * Text[2]==TextDisabled;      (if not present, use ...[0])
 * Text[3]==TextSelectDisabled; (if not present, use ...[1]; if ...[1] not present too, use ...[0])
 *
 * Color[0]==ColorNormal;      (mandatory unless GADGET_USE_DEFCOL is set)
 * Color[1]==ColorSelect;        (if not present, use ...[0])
 * Color[2]==ColorDisabled;      (if not present, use ...[0])
 * Color[3]==ColorSelectDisabled;(if not present, use ...[1]; if ...[1] not present too, use ...[0])
 *
 * If GADGET_USE_DEFCOL is set,
 * Color[0], [1], [2] and [3] are ignored.
 *
 */

#endif /* TWIN_GADGET_H */
