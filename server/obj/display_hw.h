/*
 *  module.h  --  declare server class s_module
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_DISPLAY_HW_H
#define _TWIN_DISPLAY_HW_H

#include "obj/fwd.h"

typedef struct s_mouse_state {
  dat x, y;
  dat delta_x, delta_y;
  byte keys;
} mouse_state;

struct s_display_hw {
  uldat Id;
  fn_display_hw Fn;
  display_hw Prev, Next; /* in the same All */
  all All;

  /* display_hw */
  uldat NameLen;
  char *Name;
  module Module;

  void *Private; /* used to store HW-specific data */

  void (*FlushVideo)(void);
  void (*FlushHW)(void);

  void (*KeyboardEvent)(int fd, display_hw hw);
  void (*MouseEvent)(int fd, display_hw hw);

  void (*ShowMouse)(void);
  void (*HideMouse)(void);
  void (*UpdateMouseAndCursor)(void);

  /* just detect size */
  void (*DetectSize)(dat *x, dat *y);

  /* check if size (x,y) is possible. if not, decrease (x,y) to the nearest possible size */
  void (*CheckResize)(dat *x, dat *y);

  /* unconditionally resize to (x,y). it is guaranteed that CheckResize returned this (x,y) */
  void (*Resize)(dat x, dat y);

  byte (*HWSelectionImport)(void);
  void (*HWSelectionExport)(void);
  void (*HWSelectionRequest)(obj Requestor, uldat ReqPrivate);
  void (*HWSelectionNotify)(uldat ReqPrivate, uldat Magic, const char MIME[MAX_MIMELEN], uldat Len,
                            const char *Data);
  tany HWSelectionPrivate;

  byte (*CanDragArea)(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);
  void (*DragArea)(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);
  /*
   * if the display HW is capable of doing BLiTs (BLock image Transfers) under
   * certain conditions (tipically X11's XCopyArea()), set canDragArea to a
   * function that checks if the HW can do the BLiT with the given coordinates,
   * and set DragArea to a function that unconditionally does the BLiT
   * (it will very probably need to FlushVideo() first)
   */

  void (*Beep)(void);
  void (*Configure)(udat resource, byte todefault, udat value);
  void (*ConfigureKeyboard)(udat resource, byte todefault, udat value);
  void (*ConfigureMouse)(udat resource, byte todefault, udat value);
  void (*SetPalette)(udat N, udat R, udat G, udat B);
  void (*ResetPalette)(void);

  void (*QuitHW)(void);
  void (*QuitKeyboard)(void);
  void (*QuitMouse)(void);
  void (*QuitVideo)(void);

  byte DisplayIsCTTY;
  /*
   * set to ttrue if display is the controlling terminal
   */

  byte Quitted;
  /*
   * used internally... is set to ttrue before InitHW() and after QuitHW()
   */

  byte FlagsHW;
  /*
   * various display HW flags:
   *
   * FlHWSoftMouse		: set if display HW has to manually hide/show the mouse pointer
   * FlHWChangedMouseFlag	: set after a mouse event that requires redrawing mouse pointer
   * FlHWNeedOldVideo		: set if FlushVideo() is a bit expensive, and it's better to cache
   *				  the actual display contents in OldVideo[] and send only
   *				  what effectively changed, instead of all the dirty areas.
   * FlHWExpensiveFlushVideo	: set if FlushVideo() is SO expensive that it's better to sleep
   *				  a little before flushing, hoping to receive more data
   *				  in the meantime, in order to merge the flush operations.
   * FlHWNoInput		: set if the display HW should be used as view-only,
   * 				  ignoring all input from it.
   */

  byte NeedHW;
  /*
   * various runtime flags
   */

  byte CanResize;
  /*
   * set to ttrue if the display can actually resize itself (example: X11)
   * set to tfalse if it can only live with the externally set size (example: ttys)
   */

  byte RedrawVideo;
  /*
   * set to ttrue if the display was corrupted by some external event
   * example: hw_X11.c sets this when its window gets Expose events
   */
  dat RedrawLeft, RedrawUp, RedrawRight, RedrawDown;
  /*
   * the corrupted area that needs to be redrawn.
   *
   * the upper layer (i.e. hw.c) automagically updates
   * ChangedVideoFlag and ChangedVideo[] to include this area
   * for your display.
   */

  uldat keyboard_slot, mouse_slot;

  mouse_state MouseState;

  dat X, Y;
  /*
   * real display size, in character cells.
   */

  dat usedX, usedY;
  /*
   * used display size (i.e. ScreenWidth, ScreenHeight)
   */

  dat Last_x, Last_y;
  /*
   * position of last mouse event
   */

  dat merge_Threshold;
  /*
   * if sending many small draw commands to the HW is more expensive
   * than sending fewer, bigger ones even considering you will also send
   * intermediate, clean areas, set merge_Threshold to a reasonable value
   * for merge: dirty areas less far than this will be considered as merged
   * by Threshold_isDirtyVideo().
   * Anyway, it's up to your FlushVideo() to actually merge them.
   *
   * Otherwise, set this to zero.
   */

  uldat AttachSlot; /* slot of client that told us to attach to this display */

  dat XY[2]; /* hw-dependent cursor position */
  uldat TT;  /* hw-dependent cursor type */
};

struct s_fn_display_hw {
  uldat Magic, Size, Used;
  display_hw (*Create)(fn_display_hw, uldat NameLen, const char *Name);
  void (*Insert)(display_hw, all, display_hw Prev, display_hw Next);
  void (*Remove)(display_hw);
  void (*Delete)(display_hw);
  void (*ChangeField)(display_hw, udat field, uldat CLEARMask, uldat XORMask);
  /* display_hw */
  fn_obj Fn_Obj;
  byte (*Init)(display_hw);
  void (*Quit)(display_hw);
};

/* DisplayHW->FlagsHW */
#define FlHWSoftMouse ((byte)0x01)
#define FlHWChangedMouseFlag ((byte)0x02)
#define FlHWNeedOldVideo ((byte)0x04)
#define FlHWExpensiveFlushVideo ((byte)0x08)
#define FlHWNoInput ((byte)0x10)

/* DisplayHW->NeedHW */
#define NEEDFlushStdout ((byte)0x01)
#define NEEDFlushHW ((byte)0x02)
#define NEEDResizeDisplay ((byte)0x04)
#define NEEDSelectionExport ((byte)0x08)
#define NEEDPanicHW ((byte)0x10)
#define NEEDPersistentSlot ((byte)0x20)
#define NEEDFromPreviousFlushHW ((byte)0x40)
#define NEEDBeepHW ((byte)0x80)

#endif /* _TWIN_DISPLAY_HW_H */
