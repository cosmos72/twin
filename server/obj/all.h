/*
 *  all.h  --  declare server class Sall
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
#include "obj/event.h"  /* TW_MAX_MIMELEN */
#include "obj/window.h" /* BUTTON_MAX */
#include "tty.h"        /* USER_MAP */
#include "stl_types.h"  /* String */

#include <Tw/Tw.h> /* TW_MAX_MIMELEN */

class Ssetup {
public:
  dat MaxMouseSnap;
  udat MinAllocSize;
  byte Flags;
  byte ButtonSelection, ButtonPaste;
  byte DeltaXShade, DeltaYShade;
};

/* All->Setup->Flags */
enum e_setup_flag {
  setup_shadows = 0x01,
  /* setup_blink = 0x02, no longer used */
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

class Sselection {
public:
  timevalue Time;
  Tmsgport Owner;
  Tdisplay OwnerOnce;
  uldat Magic;
  String Data;
  char MIME[TW_MAX_MIMELEN];

  void dup(const Tselection other);
  void paste();
};

class Sall : public Sobj {
public:
  TobjFn Fn;
  Tobj Prev, Next, Parent;

  /* Tall */
  Tscreen FirstScreen, LastScreen;
  Tmsgport FirstMsgPort, LastMsgPort, RunMsgPort;
  Tmutex FirstMutex, LastMutex;

  Tmodule FirstModule, LastModule;
  HookData HookModule;

  Tdisplay FirstDisplay, LastDisplay, MouseDisplay, ExclusiveDisplay;
  HookData HookDisplay;

  dat DisplayWidth, DisplayHeight;
  byte State;
  timevalue Now;
  Tselection Selection;
  Tselection Clipboard;
  Ssetup *SetUp;
  void (*AtQuit)(void);

  Tmenu BuiltinMenu, CommonMenu;
  Trow BuiltinRow;
  uldat MouseMotionN; /* number of mapped windows wanting mouse motion events */

  button_vec ButtonVec[BUTTON_MAX + 1]; /* +1 for window corner */

  trune *Gtranslations[USER_MAP + 1];

  Tall Init();

  // Sall::Delete() does nothing
  virtual void Delete() OVERRIDE;

  // Sall::Remove() does nothing
  virtual void Remove() OVERRIDE;
};

extern Tall const All;

#endif /* TWIN_ALL_H */
