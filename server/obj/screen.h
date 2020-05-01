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

#ifndef _TWIN_SCREEN_H
#define _TWIN_SCREEN_H

#include "obj/widget.h"

struct s_sB { /* for SCREENFL_USEBG screens */
  dat BgWidth, BgHeight;
  tcell *Bg;
};

struct s_screen {
  uldat Id;
  fn_screen Fn;
  screen Prev, Next;  /* list in the same All */
  widget dummyParent; /* NULL */
  /* widget */
  widget FirstW, LastW; /* list of children */
  widget FocusW;        /* same as SelectW : focused child */
  dat dummyLeft, YLimit, dummyXWidth, dummyYWidth;
  uldat Attrib;
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
    struct s_sB B;
    struct s_wE E;
  } USE;
  /* screen */
  dat NameLen;
  char *Name;
  window MenuWindow, ClickWindow;
  all All;
  fn_hook FnHookW; /* allow hooks on children Map()/UnMap() inside this widget */
  widget HookW;
};
struct s_fn_screen {
  uldat Magic, Size, Used;
  screen (*Create)(fn_screen, dat NameLen, const char *Name, dat BgWidth, dat BgHeight,
                   const tcell *Bg);
  void (*Insert)(screen, all, screen Prev, screen Next);
  void (*Remove)(screen);
  void (*Delete)(screen);
  void (*ChangeField)(screen, udat field, uldat CLEARMask, uldat XORMask);
  /* widget */
  fn_obj Fn_Obj;
  void (*DrawSelf)(draw_ctx *D);
  widget (*FindWidgetAt)(screen Parent, dat X, dat Y);
  gadget (*FindGadgetByCode)(screen Parent, udat Code);
  void (*SetXY)(screen, dat X, dat Y);
  void (*SetFill)(screen, tcell Fill);
  widget (*Focus)(screen);
  widget (*KbdFocus)(screen);
  void (*Map)(screen, widget Parent);
  void (*UnMap)(screen);
  void (*MapTopReal)(screen, screen);
  void (*Raise)(screen);
  void (*Lower)(screen);
  void (*Own)(screen, msgport);
  void (*DisOwn)(screen);
  void (*RecursiveDelete)(screen, msgport);
  void (*Expose)(screen, dat XWidth, dat YWidth, dat Left, dat Up, const char *, const trune *,
                 const tcell *);
  byte (*InstallHook)(screen, fn_hook, fn_hook *Where);
  void (*RemoveHook)(screen, fn_hook, fn_hook *Where);
  /* screen */
  fn_widget Fn_Widget;
  menu (*FindMenu)(screen);
  screen (*Find)(dat j);
  screen (*CreateSimple)(fn_screen, dat NameLen, const char *Name, tcell Bg);
  void (*BgImage)(screen, dat BgWidth, dat BgHeight, const tcell *Bg);
  void (*DrawMenu)(screen, dat Xstart, dat Xend);
  void (*ActivateMenu)(screen, menuitem, byte ByMouse);
  void (*DeActivateMenu)(screen);
};

/* Screen->Attrib */
/* not used */

/* Screen->Flags */
#define SCREENFL_USEBG 0x00                   /* it's the default */
#define SCREENFL_USEEXPOSE WIDGETFL_USEEXPOSE /* 0x02 */
#define SCREENFL_USEFILL WIDGETFL_USEFILL     /* 0x03 */
#define SCREENFL_USEANY WIDGETFL_USEANY       /* 0x07 */

#define S_USE(s, USExxx) (((s)->Flags & SCREENFL_USEANY) == CAT(SCREENFL_, USExxx))

#define SCREENFL_BACK_SELECT 0x0010
#define SCREENFL_BACK_PRESSED 0x0020
#define SCREENFL_NOTVISIBLE 0x8000

#endif /* _TWIN_SCREEN_H */
