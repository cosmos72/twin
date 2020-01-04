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

#ifndef _TWIN_GADGET_H
#define _TWIN_GADGET_H

#include "obj/widget.h"

struct s_gT { /* for GADGETFL_USETEXT gadgets */
  trune *Text[4];
  tcolor *Color[4];
};

struct s_gadget {
  uldat Id;
  fn_gadget Fn;
  widget Prev, Next;
  widget Parent;
  /* widget */
  widget FirstW, LastW; /* list of children */
  widget SelectW;       /* selected child */
  dat Left, Up, XWidth, YWidth;
  uldat Attrib;
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
};

struct s_fn_gadget {
  uldat Magic, Size, Used;
  gadget (*Create)(fn_gadget, msgport Owner, widget Parent, dat XWidth, dat YWidth,
                   CONST char *TextNormal, uldat Attrib, uldat Flags, udat Code, tcolor ColText,
                   tcolor ColTextSelect, tcolor ColTextDisabled, tcolor ColTextSelectDisabled,
                   dat Left, dat Up);
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
  void (*SetFill)(widget, tcell Fill);
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
  void (*Expose)(gadget, dat XWidth, dat YWidth, dat Left, dat Up, CONST char *, CONST trune *,
                 CONST tcell *);
  byte (*InstallHook)(gadget, fn_hook, fn_hook *Where);
  void (*RemoveHook)(gadget, fn_hook, fn_hook *Where);
  /* gadget */
  fn_widget Fn_Widget;
  gadget (*CreateEmptyButton)(fn_gadget Fn_Gadget, msgport Owner, dat XWidth, dat YWidth,
                              tcolor BgCol);
  byte (*FillButton)(gadget Gadget, widget Parent, udat Code, dat Left, dat Up, udat Flags,
                     CONST char *Text, tcolor Color, tcolor ColorDisabled);
  gadget (*CreateButton)(fn_gadget Fn_Gadget, widget Parent, dat XWidth, dat YWidth,
                         CONST char *Text, uldat Flags, udat Code, tcolor BgCol, tcolor Col,
                         tcolor ColDisabled, dat Left, dat Up);
  void (*WriteTexts)(gadget Gadget, byte bitmap, dat XWidth, dat YWidth, CONST char *Text, dat Left,
                     dat Up);
  void (*WriteTRunes)(gadget Gadget, byte bitmap, dat XWidth, dat YWidth, CONST trune *TRune,
                      dat Left, dat Up);
};

/* Gadget->Attrib */
typedef enum e_gadget_attr {
  GADGET_WANT_MOUSE_MOTION = WIDGET_WANT_MOUSE_MOTION, /* 0x0001 */
  GADGET_WANT_KEYS = WIDGET_WANT_KEYS,                 /* 0x0002 */
  GADGET_WANT_MOUSE = WIDGET_WANT_MOUSE,               /* 0x0004 */
  GADGET_WANT_CHANGES = WIDGET_WANT_CHANGES,           /* 0x0008 */
  GADGET_AUTO_FOCUS = WIDGET_AUTO_FOCUS,               /* 0x0010 */
} gadget_attr;

/* Gadget->Flags */
typedef enum e_gadget_flag {
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
} gadget_flag;

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

#endif /* _TWIN_GADGET_H */
