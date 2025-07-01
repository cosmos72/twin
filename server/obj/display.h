/*
 *  module.h  --  declare server class Smodule
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_DISPLAY_H
#define TWIN_DISPLAY_H

#include "obj/fwd.h"
#include "obj/event.h" // TW_MAX_MIMELEN
#include "obj/obj.h"
#include "stl_types.h"

/* Sdisplay::FlagsHW */
enum FlagHW {
  FlagSoftMouseHW = 0x01,
  FlagChangedMouseFlagHW = 0x02,
  FlagNeedOldVideoHW = 0x04,
  FlagExpensiveFlushVideoHW = 0x08,
  FlagNoInputHW = 0x10,
};

/* Sdisplay::NeedHW and global NeedHW */
enum NeedHW {
  NeedFlushStdout = 0x01,
  NeedFlushHW = 0x02,
  NeedResizeDisplay = 0x04,
  NeedSelectionExport = 0x08,
  NeedPanicHW = 0x10,
  NeedPersistentSlot = 0x20,
  NeedFromPreviousFlushHW = 0x40,
  NeedBeepHW = 0x80,
};

class mouse_state {
public:
  dat x, y;
  dat delta_x, delta_y;
  byte keys;
};

class Sdisplay : public Sobj {
public:
  Tdisplay Prev, Next; /* in the same All */
  Tall All;

  /* Tdisplay */
  String Name;
  Tmodule Module;

  void *Private; /* used to store HW-specific data */

  void (*fnFlushVideo)(Tdisplay hw);
  void (*fnFlushHW)(Tdisplay hw);

  void (*fnKeyboardEvent)(int fd, Tdisplay hw);
  void (*fnMouseEvent)(int fd, Tdisplay hw);

  void (*fnShowMouse)(Tdisplay hw);
  void (*fnHideMouse)(Tdisplay hw);
  void (*fnUpdateMouseAndCursor)(Tdisplay hw);

  /* just detect size */
  void (*fnDetectSize)(Tdisplay hw, dat *x, dat *y);

  /* check if size (x,y) is possible. if not, decrease (x,y) to the nearest possible size */
  void (*fnCheckResize)(Tdisplay hw, dat *x, dat *y);

  /* unconditionally resize to (x,y). it is guaranteed that CheckResize returned this (x,y) */
  void (*fnResize)(Tdisplay hw, dat x, dat y);

  bool (*fnSelectionImport)(Tdisplay hw);
  void (*fnSelectionExport)(Tdisplay hw);
  void (*fnSelectionRequest)(Tdisplay hw, Tobj requestor, uldat reqprivate);
  void (*fnSelectionNotify)(Tdisplay hw, uldat reqprivate, e_id magic, Chars mime, Chars data);
  tany SelectionPrivate;

  bool (*fnCanDragArea)(Tdisplay hw, dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart,
                        dat DstYstart);
  void (*fnDragArea)(Tdisplay hw, dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart,
                     dat DstYstart);
  /*
   * if the display HW is capable of doing BLiTs (BLock image Transfers) under
   * certain conditions (tipically X11's XCopyArea()), set canDragArea to a
   * function that checks if the HW can do the BLiT with the given coordinates,
   * and set DragArea to a function that unconditionally does the BLiT
   * (it will very probably need to FlushVideo() first)
   */

  void (*fnBeep)(Tdisplay hw);
  void (*fnConfigure)(Tdisplay hw, udat resource, byte todefault, udat value);
  void (*fnConfigureKeyboard)(Tdisplay hw, udat resource, byte todefault, udat value);
  void (*fnConfigureMouse)(Tdisplay hw, udat resource, byte todefault, udat value);
  void (*fnSetPalette)(Tdisplay hw, udat N, udat R, udat G, udat B);
  void (*fnResetPalette)(Tdisplay hw);

  void (*fnQuitHW)(Tdisplay hw);
  void (*fnQuitKeyboard)(Tdisplay hw);
  void (*fnQuitMouse)(Tdisplay hw);
  void (*fnQuitVideo)(Tdisplay hw);

  void setFlush() {
    NeedHW |= NeedFlushHW;
  }

  void clrFlush() {
    NeedHW &= ~NeedFlushHW;
  }

  void FlushVideo() {
    if (fnFlushVideo) {
      fnFlushVideo(this);
    }
  }
  void FlushHW() {
    if (fnFlushHW) {
      fnFlushHW(this);
    }
  }

  void KeyboardEvent(int fd) {
    if (fnKeyboardEvent) {
      fnKeyboardEvent(fd, this);
    }
  }
  void MouseEvent(int fd) {
    if (fnMouseEvent) {
      fnMouseEvent(fd, this);
    }
  }

  void ShowMouse() {
    if (fnShowMouse) {
      fnShowMouse(this);
    }
  }
  void HideMouse() {
    if (fnHideMouse) {
      fnHideMouse(this);
    }
  }
  void UpdateMouseAndCursor() {
    if (fnUpdateMouseAndCursor) {
      fnUpdateMouseAndCursor(this);
    }
  }

  /* just detect size */
  void DetectSize(dat *x, dat *y) {
    if (fnDetectSize) {
      fnDetectSize(this, x, y);
    }
  }

  /* check if size (x,y) is possible. if not, decrease (x,y) to the nearest possible size */
  void CheckResize(dat *x, dat *y) {
    if (fnCheckResize) {
      fnCheckResize(this, x, y);
    }
  }

  /* unconditionally resize to (x,y). it is guaranteed that CheckResize returned this (x,y) */
  void Resize(dat x, dat y) {
    if (fnResize) {
      fnResize(this, x, y);
    }
  }

  bool SelectionImport() {
    if (fnSelectionImport) {
      return fnSelectionImport(this);
    }
    return false;
  }
  void SelectionExport() {
    if (fnSelectionExport) {
      fnSelectionExport(this);
    }
  }
  void SelectionRequest(Tobj requestor, uldat reqprivate) {
    if (fnSelectionRequest) {
      fnSelectionRequest(this, requestor, reqprivate);
    }
  }
  void SelectionNotify(uldat reqprivate, e_id magic, Chars mime, Chars data) {
    if (fnSelectionNotify) {
      fnSelectionNotify(this, reqprivate, magic, mime, data);
    }
  }

  byte CanDragArea(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart) {
    if (fnCanDragArea) {
      return fnCanDragArea(this, Xstart, Ystart, Xend, Yend, DstXstart, DstYstart);
    }
    return false;
  }
  void DragArea(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart) {
    if (fnDragArea) {
      fnDragArea(this, Xstart, Ystart, Xend, Yend, DstXstart, DstYstart);
    }
  }
  /*
   * if the display HW is capable of doing BLiTs (BLock image Transfers) under
   * certain conditions (tipically X11's XCopyArea()), set canDragArea to a
   * function that checks if the HW can do the BLiT with the given coordinates,
   * and set DragArea to a function that unconditionally does the BLiT
   * (it will very probably need to FlushVideo() first)
   */

  void Beep() {
    if (fnBeep) {
      fnBeep(this);
    }
  }
  void Configure(udat resource, byte todefault, udat value) {
    if (fnConfigure) {
      fnConfigure(this, resource, todefault, value);
    }
  }
  void ConfigureKeyboard(udat resource, byte todefault, udat value) {
    if (fnConfigureKeyboard) {
      fnConfigureKeyboard(this, resource, todefault, value);
    }
  }
  void ConfigureMouse(udat resource, byte todefault, udat value) {
    if (fnConfigureMouse) {
      fnConfigureMouse(this, resource, todefault, value);
    }
  }
  void SetPalette(udat N, udat R, udat G, udat B) {
    if (fnSetPalette) {
      fnSetPalette(this, N, R, G, B);
    }
  }
  void ResetPalette() {
    if (fnResetPalette) {
      fnResetPalette(this);
    }
  }

  void QuitHW() {
    if (fnQuitHW) {
      fnQuitHW(this);
    }
  }
  void QuitKeyboard() {
    if (fnQuitKeyboard) {
      fnQuitKeyboard(this);
    }
  }
  void QuitMouse() {
    if (fnQuitMouse) {
      fnQuitMouse(this);
    }
  }
  void QuitVideo() {
    if (fnQuitVideo) {
      fnQuitVideo(this);
    }
  }

  bool DisplayIsCTTY; /* set to true if display is the controlling terminal */
  bool Quitted;       /* used internally: set to true before InitHW() and after QuitHW() */

  /*
   * various display HW flags:
   *
   * FlagSoftMouseHW         : set if display HW has to manually hide/show the mouse pointer
   * FlagChangedMouseFlagHW  : set after a mouse event that requires redrawing mouse pointer
   * FlagNeedOldVideoHW      : set if FlushVideo() is a bit expensive, and it's better to cache the
   *                         actual display contents in OldVideo[] and send only what effectively
   *                         changed, instead of all the dirty areas.
   * FlagExpensiveFlushVideoHW : set if FlushVideo() is SO expensive that it's better to sleep
   *                         a little before flushing, hoping to receive more data in the meantime,
   *                         in order to merge the flush operations.
   * FlagNoInputHW           : set if the display HW should be used as view-only, ignoring all input
   *                         from it.
   */
  byte FlagsHW;

  byte NeedHW; /* various runtime flags */

  /*
   * set to ttrue if the display can actually resize itself (example: X11)
   * set to tfalse if it can only live with the externally set size (example: ttys)
   */
  bool CanResize;

  /*
   * set to ttrue if the display was corrupted by some external event
   * example: hw_X11.c sets this when its window gets Expose events
   */
  bool RedrawVideo;

  /*
   * the dirty area that needs to be redrawn.
   *
   * the upper layer (i.e. hw.c) automagically updates
   * ChangedVideoFlag and ChangedVideo[] to include this area
   * for your display.
   */
  dat RedrawLeft, RedrawUp, RedrawRight, RedrawDown;
  uldat keyboard_slot, mouse_slot;
  mouse_state MouseState;

  dat X, Y;           /* real display size, in character cells. */
  dat usedX, usedY;   /* used display size (i.e. ScreenWidth, ScreenHeight) */
  dat Last_x, Last_y; /* position of last mouse event */

  uldat AttachSlot; /* slot of client that told us to attach to this display */

  dat XY[2]; /* hw-dependent cursor position */
  uldat TT;  /* hw-dependent cursor type */

  Tdisplay Init(Chars name);
  static Tdisplay Create(Chars name);

  /* Tobj */

  virtual void Delete() OVERRIDE;
  void Insert(Tall a, Tdisplay prev, Tdisplay next);
  virtual void Remove() OVERRIDE;

  /* Tdisplay */
  bool DoInit(); // defined in hw_multi.cpp
  void DoQuit(); // defined in hw_multi.cpp
};

#endif /* TWIN_DISPLAY_H */
