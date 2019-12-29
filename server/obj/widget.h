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

#ifndef _TWIN_WIDGET_H
#define _TWIN_WIDGET_H

#include "obj/obj.h"

/* WIDGET_USEEXPOSE  USE.E.Flags: */
#define WIDGET_USEEXPOSE_TEXT 1
#define WIDGET_USEEXPOSE_HWFONT 2
#define WIDGET_USEEXPOSE_HWATTR 4

/* Widget->Attrib */
typedef enum e_widget_attr {
  /*
   * ask the server to send events even for mouse motion without any pressed button.
   * works only if WIDGET_WANT_MOUSE is set too.
   */
  WIDGET_WANT_MOUSE_MOTION = 0x0001,
  WIDGET_WANT_KEYS = 0x0002,
  WIDGET_WANT_MOUSE = 0x0004,
  WIDGET_WANT_CHANGES = 0x0008,
  WIDGET_AUTO_FOCUS = 0x0010,
} widget_attr;

/* Widget->Flags */
typedef enum e_widget_flag {
  WIDGETFL_USEEXPOSE = 0x02,
  WIDGETFL_USEFILL = 0x03,
  WIDGETFL_USEANY = 0x07, /* mask of all above ones */

  WIDGETFL_NOTVISIBLE = 0x8000,
} widget_flag;

#define w_USE(w, USExxx) (((w)->Flags & WIDGETFL_USEANY) == CAT(WIDGETFL_, USExxx))

struct s_wE { /* for WIDGET_USEEXPOSE widgets */
  union {
    CONST char *Text;
    CONST hwfont *HWFont;
    CONST hwattr *HWAttr;
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
  byte NoChildren;
  byte BorderDone;
  byte Shaded;
};

struct s_widget {
  uldat Id;
  fn_widget Fn;
  widget Prev, Next; /* list in the same parent */
  widget Parent;     /* where this widget sits */
  /* widget */
  widget FirstW, LastW; /* list of children */
  widget SelectW;       /* selected child */
  dat Left, Up, XWidth, YWidth;
  uldat Attrib;
  uldat Flags;
  ldat XLogic, YLogic;
  widget O_Prev, O_Next; /* list with the same msgport (owner) */
  msgport Owner;
  fn_hook ShutDownHook; /* hooks for this widget */
  fn_hook Hook, *WhereHook;
  fn_hook MapUnMapHook;
  msg MapQueueMsg;
  hwattr USE_Fill;
  union {
    struct s_wE E;
  } USE;
};

struct s_fn_widget {
  uldat Magic, Size, Used;
  widget (*Create)(fn_widget, msgport Owner, dat XWidth, dat YWidth, uldat Attrib, uldat Flags,
                   dat Left, dat Up, hwattr USE_Fill);
  void (*Insert)(widget, widget Parent, widget Prev, widget Next);
  void (*Remove)(widget);
  void (*Delete)(widget);
  void (*ChangeField)(widget, udat field, uldat CLEARMask, uldat XORMask);
  /* widget */
  fn_obj Fn_Obj; /* backup of overloaded functions */
  void (*DrawSelf)(draw_ctx *D);
  widget (*FindWidgetAt)(widget Parent, dat X, dat Y);
  gadget (*FindGadgetByCode)(widget Parent, udat Code);
  void (*SetXY)(widget, dat X, dat Y);
  void (*SetFill)(widget, hwattr Fill);
  widget (*Focus)(widget);
  widget (*KbdFocus)(widget);
  void (*Map)(widget, widget Parent);
  void (*UnMap)(widget);
  void (*MapTopReal)(widget, screen);
  void (*Raise)(widget);
  void (*Lower)(widget);
  void (*Own)(widget, msgport);
  void (*DisOwn)(widget);
  void (*RecursiveDelete)(widget, msgport);
  void (*Expose)(widget, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, CONST char *,
                 CONST hwfont *, CONST hwattr *);
  byte (*InstallHook)(widget, fn_hook, fn_hook *Where);
  void (*RemoveHook)(widget, fn_hook, fn_hook *Where);
};

#endif /* _TWIN_WIDGET_H */
