/*
 *  fn.h  --  declare server class s_fn
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_ALL_H
#define _TWIN_ALL_H

#include "obj/obj.h"
#include "tty.h" // USER_MAP

struct s_setup {
  dat MaxMouseSnap;
  udat MinAllocSize;
  byte Flags;
  byte ButtonSelection, ButtonPaste;
  byte DeltaXShade, DeltaYShade;
};
/* All->Setup->Flags */
#define SETUP_SHADOWS 0x01
#define SETUP_BLINK 0x02
#define SETUP_CURSOR_ALWAYS 0x04
#define SETUP_MENU_HIDE 0x08
#define SETUP_MENU_INFO 0x10
#define SETUP_MENU_RELAX 0x20
#define SETUP_SCREEN_SCROLL 0x40
#define SETUP_TERMINALS_UTF8 0x80

#define MAX_XSHADE 9
#define MAX_YSHADE 9

/*
 * values of All->State.
 * This order is hardcoded in methods.c:UnMapWindow(),
 * in resize.c:ExecScrollFocusWindow(),
 * in scroller.c:ScrollerH(),
 * do not change it!
 */
typedef enum e_state {
  STATE_DRAG = 10,
  STATE_RESIZE = 15,
  STATE_SCROLL = 16,
  STATE_GADGET = 26,
  STATE_MENU = 27,
  STATE_SCREEN = 28,
  STATE_BUTTON_SCREEN = 29,
  STATE_ROOT = 30,
  STATE_DEFAULT = 31,

  /* mask for all the above */
  STATE_ANY = 0x1F,

  /* further All->State flags */
  STATE_FL_BYMOUSE = 0x40,
} tstate;

typedef struct s_button_vec {
  hwfont shape[2];
  sbyte pos;
  byte exists;
  byte changed;
} button_vec;

typedef struct s_selection {
  timevalue Time;
  msgport Owner;
  display_hw OwnerOnce;
  uldat Magic;
  char MIME[MAX_MIMELEN];
  uldat Len, Max;
  char *Data;
} selection;

struct s_all {
  /* for compatibility with s_obj */
  uldat Id;
  fn_obj Fn;
  obj Prev, Next;
  obj Parent;

  screen FirstScreen, LastScreen;
  msgport FirstMsgPort, LastMsgPort, RunMsgPort;
  mutex FirstMutex, LastMutex;

  module FirstModule, LastModule;
  fn_hook FnHookModule;
  widget HookModule;

  display_hw FirstDisplayHW, LastDisplayHW, MouseHW, ExclusiveHW;
  fn_hook FnHookDisplayHW;
  widget HookDisplayHW;

  dat DisplayWidth, DisplayHeight;
  byte State;
  timevalue Now;
  selection *Selection;
  setup *SetUp;
  void (*AtQuit)(void);

  menu BuiltinMenu, CommonMenu;
  row BuiltinRow;
  uldat MouseMotionN; /* number of mapped windows wanting mouse motion events */

  button_vec ButtonVec[BUTTON_MAX + 1]; /* +1 for window corner */

  hwfont *Gtranslations[USER_MAP + 1];
};

#endif /* _TWIN_ALL_H */
