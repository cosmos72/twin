/*
 *  all.h  --  declare server class s_all
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_ALL_H
#define TWIN_ALL_H

#include "obj/obj.h"
#include "obj/event.h"  /* MAX_MIMELEN */
#include "obj/window.h" /* BUTTON_MAX */
#include "tty.h"        /* USER_MAP */
#include "stl_types.h"  /* String */

struct s_setup {
  dat MaxMouseSnap;
  udat MinAllocSize;
  byte Flags;
  byte ButtonSelection, ButtonPaste;
  byte DeltaXShade, DeltaYShade;
};

/* All->Setup->Flags */
enum e_setup_flag {
  setup_shadows = 0x01,
  setup_blink = 0x02,
  setup_cursor_always = 0x04,
  setup_menu_hide = 0x08,
  setup_menu_info = 0x10,
  setup_menu_relax = 0x20,
  setup_screen_scroll = 0x40,
  setup_terminals_utf8 = 0x80,
};

enum {
  max_xshade = 9,
  max_yshade = 9,
};

/*
 * values of All->State.
 * This order is hardcoded in methods.c:UnMapWindow(),
 * in resize.c:ExecScrollFocusWindow(),
 * in scroller.c:ScrollerH(),
 * do not change it!
 */
enum e_all_state {
  state_drag = 10,
  state_resize = 15,
  state_scroll = 16,
  state_gadget = 26,
  state_menu = 27,
  state_screen = 28,
  state_button_screen = 29,
  state_root = 30,
  state_default = 31,

  /* mask for all the above */
  state_any = 0x1F,

  /* further All->State flags */
  state_fl_bymouse = 0x40,
};

typedef struct s_button_vec {
  trune shape[2];
  sbyte pos;
  byte exists;
  byte changed;
} button_vec;

struct s_selection {
  timevalue Time;
  msgport Owner;
  display_hw OwnerOnce;
  uldat Magic;
  String Data;
  char MIME[MAX_MIMELEN];

  void dup(const selection other);
  void paste();
};

struct s_all : public s_obj {
  fn_obj Fn;
  obj Prev, Next, Parent;

  /* all */
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
  selection Selection;
  selection Clipboard;
  setup *SetUp;
  void (*AtQuit)(void);

  menu BuiltinMenu, CommonMenu;
  row BuiltinRow;
  uldat MouseMotionN; /* number of mapped windows wanting mouse motion events */

  button_vec ButtonVec[BUTTON_MAX + 1]; /* +1 for window corner */

  trune *Gtranslations[USER_MAP + 1];

  all Init();
};

extern all const All;

#endif /* TWIN_ALL_H */
