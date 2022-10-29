/*
 *  gadget.h  --  declare server class Sgadget
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

struct SgadgetFn {
  uldat Magic;
  void (*Insert)(Tgadget, Twidget Parent, Twidget Prev, Twidget Next);
  void (*Remove)(Tgadget);
  void (*Delete)(Tgadget);
  void (*ChangeField)(Tgadget, udat field, uldat CLEARMask, uldat XORMask);
  /* Twidget */
  TobjFn Fn_Obj;
  void (*DrawSelf)(draw_ctx *D);
  Twidget (*FindWidgetAt)(Tgadget Parent, dat X, dat Y);
  Tgadget (*FindGadgetByCode)(Tgadget Parent, udat Code);
  void (*SetXY)(Tgadget, dat X, dat Y);
  void (*SetFill)(Tgadget, tcell Fill);
  Twidget (*Focus)(Tgadget);
  Twidget (*KbdFocus)(Tgadget);
  void (*Map)(Tgadget, Twidget Parent);
  void (*UnMap)(Tgadget);
  void (*MapTopReal)(Tgadget, screen);
  void (*Raise)(Tgadget);
  void (*Lower)(Tgadget);
  void (*Own)(Tgadget, Tmsgport);
  void (*DisOwn)(Tgadget);
  void (*RecursiveDelete)(Tgadget, Tmsgport);
  void (*Expose)(Tgadget, dat xwidth, dat ywidth, dat Left, dat Up, const char *, const trune *,
                 const tcell *);
  byte (*InstallHook)(Tgadget, HookFn, HookFn *Where);
  void (*RemoveHook)(Tgadget, HookFn, HookFn *Where);
  /* Tgadget */
  TwidgetFn Fn_Widget;
  Tgadget (*CreateEmptyButton)(Tmsgport owner, dat xwidth, dat ywidth, tcolor BgCol);
  byte (*FillButton)(Tgadget g, Twidget Parent, udat Code, dat Left, dat Up, udat Flags,
                     const char *Text, tcolor Color, tcolor ColorDisabled);
  Tgadget (*CreateButton)(Twidget Parent, dat xwidth, dat ywidth, const char *Text, uldat Flags,
                          udat Code, tcolor BgCol, tcolor Col, tcolor ColDisabled, dat Left,
                          dat Up);
  void (*WriteTexts)(Tgadget g, byte bitmap, dat xwidth, dat ywidth, const char *Text, dat Left,
                     dat Up);
  void (*WriteTRunes)(Tgadget g, byte bitmap, dat xwidth, dat ywidth, const trune *TRune, dat Left,
                      dat Up);
};

struct Sgadget : public Sobj {
  TgadgetFn Fn;
  Twidget Prev, Next;
  Twidget Parent;
  /* Twidget */
  Twidget FirstW, LastW; /* list of children */
  Twidget SelectW;       /* selected child */
  dat Left, Up, XWidth, YWidth;
  uldat Attr;
  uldat Flags;
  ldat XLogic, YLogic;
  Twidget O_Prev, O_Next; /* list in the same Tmsgport (owner) */
  Tmsgport Owner;
  HookFn ShutDownHook; /* hooks for this Twidget */
  HookFn Hook, *WhereHook;
  HookFn MapUnMapHook;
  Tmsg MapQueueMsg;
  tcell USE_Fill;
  union {
    struct s_gT T;
    struct s_wE E;
  } USE;
  /* Tgadget */
  tcolor ColText, ColSelect, ColDisabled, ColSelectDisabled;
  udat Code;
  Tgadget G_Prev, G_Next; /* list in the same Tgroup */
  Tgroup Group;

  static Tgadget Create(Tmsgport owner, Twidget Parent, dat xwidth, dat ywidth,
                        const char *TextNormal, uldat Attr, uldat Flags, udat Code, tcolor ColText,
                        tcolor ColTextSelect, tcolor ColTextDisabled, tcolor ColTextSelectDisabled,
                        dat Left, dat Up);
  Tgadget Init(Tmsgport owner, Twidget Parent, dat xwidth, dat ywidth, const char *TextNormal,
               uldat Attr, uldat Flags, udat Code, tcolor ColText, tcolor ColTextSelect,
               tcolor ColTextDisabled, tcolor ColTextSelectDisabled, dat Left, dat Up);

  /* Tobj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
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
  void Expose(dat xwidth, dat ywidth, dat left, dat up, const char *ascii, const trune *runes,
              const tcell *cells) {
    Fn->Expose(this, xwidth, ywidth, left, up, ascii, runes, cells);
  }
  byte InstallHook(HookFn hook, HookFn *where) {
    return Fn->InstallHook(this, hook, where);
  }
  void RemoveHook(HookFn hook, HookFn *where) {
    Fn->RemoveHook(this, hook, where);
  }
  /* Tgadget */
  byte FillButton(Twidget parent, udat code, dat left, dat up, udat flags, const char *text,
                  tcolor color, tcolor colordisabled) {
    return Fn->FillButton(this, parent, code, left, up, flags, text, color, colordisabled);
  }
  Tgadget CreateEmptyButton(Tmsgport owner, dat xwidth, dat ywidth, tcolor bgcol) {
    return Fn->CreateEmptyButton(owner, xwidth, ywidth, bgcol);
  }
  Tgadget CreateButton(Twidget parent, dat xwidth, dat ywidth, const char *text, uldat flags,
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
enum gadget_attr /*: udat*/ {
  GADGET_WANT_MOUSE_MOTION = WIDGET_WANT_MOUSE_MOTION, /* 0x0001 */
  GADGET_WANT_KEYS = WIDGET_WANT_KEYS,                 /* 0x0002 */
  GADGET_WANT_MOUSE = WIDGET_WANT_MOUSE,               /* 0x0004 */
  GADGET_WANT_CHANGES = WIDGET_WANT_CHANGES,           /* 0x0008 */
  GADGET_AUTO_FOCUS = WIDGET_AUTO_FOCUS,               /* 0x0010 */
};

/* Gadget->Flags */
enum gadget_flag /*: udat*/ {
  GADGETFL_USETEXT = 0x00,                 /* default */
  GADGETFL_USEEXPOSE = WIDGETFL_USEEXPOSE, /* 0x02 */
  GADGETFL_USEFILL = WIDGETFL_USEFILL,     /* 0x03 */
  GADGETFL_USEANY = WIDGETFL_USEANY,       /* 0x07 */

  /* remember this Tgadget is a button, so that SetText() does not ruin the shadow */
  GADGETFL_BUTTON = 0x0010,
  GADGETFL_DISABLED = 0x0020,
  GADGETFL_TEXT_DEFCOL = 0x0040,
  /* this makes the Tgadget 'checkable' : can be in 'checked' or 'unchecked' state.
   * also necessary to put the Tgadget in a Tgroup */
  GADGETFL_TOGGLE = 0x0080,
  GADGETFL_PRESSED = 0x0100,

  GADGETFL_NOTVISIBLE = 0x8000,
};

#define G_USE(g, USExxx) (((g)->Flags & GADGETFL_USEANY) == CAT(GADGETFL_, USExxx))

/*              NOTE :
 *
 * the USE.T fields (Contents) of a Tgadget are used as follows:
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
