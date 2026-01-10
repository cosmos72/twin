/*
 *  display.c  -- connect to a running twin and register as a display
 *
 *  Copyright (C) 2000-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

/*
 * this is basically a chopped down version of twin server,
 * with the minimum required features to startup a display driver,
 * plus libtw code to talk to twin, register on it as a special display,
 * and forward messages and commands between the display driver and twin.
 */

#include "twin.h"
#include "algo.h"
#include "alloc.h"
#include "common.h"
#include "data.h"
#include "palette.h"
#include "dl_helper.h"
#include "fdlist.h"
#include "hw.h"
#include "hw_private.h"
#include "log.h"
#include "obj/id.h" // T*_class_id
#include "stl/string.h"
#include "version.h"
#include "unaligned.h"

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>

#include <stdio.h>
#include <stdarg.h>
#include <cstring>
#include <signal.h>

#ifndef PLUGINDIR
#warning PLUGINDIR is not #defined, assuming "/usr/local/lib/twin"
#define PLUGINDIR "/usr/local/lib/twin"
#endif

Chars plugindir = PLUGINDIR;

static const char *MYname;

static dat TryDisplayWidth, TryDisplayHeight;
static byte ValidVideo;

const char *TWDisplay, *origTWDisplay, *origTERM, *origCOLORTERM;

char printk_buf[TW_BIGBUFF];

String HOME;

static fdlist FdList[5];
static uldat FdSize = 5, FdTop, FdBottom, FdWQueued;
#define LS FdList[Slot]
#define ls FdList[slot]

static fd_set save_rfds, save_wfds;
static int max_fds;

static tmsgport TMsgPort = NOID, tw_helper = NOID;
static byte MouseMotionN; /* non-zero to report also mouse motion events */

int (*OverrideSelect)(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                      struct timeval *timeout) = select;

/*
 * first, utility functions
 */

void Quit(int status);

void NoOp(void) {
}
bool AlwaysFalse(void) {
  return false;
}
bool AlwaysTrue(void) {
  return true;
}
void *AlwaysNull(void) {
  return NULL;
}

void gainPrivileges(void) {
}
void RemotePidIsDead(pid_t pid) {
  (void)pid;
}

void printk_str(const char *s, size_t len) {
  (void)fwrite(s, len, 1, stderr);
}

int flushk(void) {
  return fflush(stderr);
}

static void OutOfMemory(void) {
  log(ERROR) << "twdisplay: Out of memory!\n";
}

inline uldat FdListGet(void) {
  if (FdBottom < FdSize)
    return FdBottom;
  return NOSLOT;
}

uldat RegisterRemote(int fd, Tobj HandlerData, handler_io_d HandlerIO) {
  uldat Slot, j;

  if ((Slot = FdListGet()) == NOSLOT)
    return Slot;

  memset(&LS, 0, sizeof(fdlist));

  LS.Fd = fd;
  LS.pairSlot = NOSLOT;
  if ((LS.HandlerData = HandlerData))
    LS.HandlerIO.D = HandlerIO;
  else
    LS.HandlerIO.S = (handler_io_s)HandlerIO;
  LS.extern_couldntwrite = tfalse;

  if (FdTop <= Slot)
    FdTop = Slot + 1;
  for (j = FdBottom + 1; j < FdTop; j++)
    if (FdList[j].Fd == NOFD)
      break;
  FdBottom = j;

  if (fd >= 0) {
    FD_SET(fd, &save_rfds);
    if (max_fds < fd)
      max_fds = fd;
  }
  return Slot;
}

void UnRegisterRemote(uldat Slot) {
  /* not needed, we are going to quit anyway if this gets called */
  (void)Slot;
}

void RemoteCouldntWrite(uldat Slot) {
  if (Slot == NOSLOT || Slot >= FdTop || LS.Fd == NOFD) {
    return;
  }
  if (LS.extern_couldntwrite == tfalse) {
    LS.extern_couldntwrite = ttrue;
    FdWQueued++;
  }
  FD_SET(LS.Fd, &save_wfds);
}

void RemoteCouldWrite(uldat Slot) {
  if (Slot == NOSLOT || Slot >= FdTop || LS.Fd == NOFD)
    return;
  if (LS.extern_couldntwrite == ttrue) {
    LS.extern_couldntwrite = tfalse;
    FdWQueued--;
  }
  FD_CLR(LS.Fd, &save_wfds);
}

static void RemoteEvent(int FdCount, fd_set *FdSet) {
  uldat Slot;
  int fd;
  for (Slot = 0; Slot < FdTop && FdCount; Slot++) {
    if ((fd = LS.Fd) >= 0) {
      if (FD_ISSET(fd, FdSet)) {
        FdCount--;
        if (LS.HandlerData)
          LS.HandlerIO.D(fd, LS.HandlerData);
        else
          LS.HandlerIO.S(fd, Slot);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Sobj methods

Tobj Sobj::Init(e_id class_id) {
  Id = class_id;
  return this;
}

void Sobj::Delete() {
}

////////////////////////////////////////////////////////////////////////////////
/// Smodule methods

Tmodule Smodule::Init(Chars /*name*/) {
  Sobj::Init(Tmodule_class_id);
  return this;
}

void Smodule::Delete() {
}

void Smodule::Remove() {
}

static Smodule _Module;
static Tmodule const GModule = _Module.Init(Chars());

static Tmodule DlLoadAny(Chars name) {
  byte (*init_func)(Tmodule);
  Tmodule m = GModule;
  String path;

  if (!dlinit_once()) {
    return NULL;
  } else if (!m->Name.format(name) || !path.format(plugindir, "/" DL_PREFIX, name, DL_SUFFIX)) {
    Error(NOMEMORY);
  } else if (!(m->Handle = (void *)dlopen(path.data()))) {
    Error(DLERROR);
    /*
     * a Module MUST have a InitModule function, as it needs to set
     * Module->Private to its xxx_InitHW() startup code.
     */
  } else if (!(init_func = (byte (*)(Tmodule))dlsym((dlhandle)m->Handle, "InitModule"))) {
    Error(DLERROR);
    Errstr = Chars("InitModule() not found in Tmodule");
  } else if (!init_func(m)) {
    if (!Errstr) {
      Error(DLERROR);
      Errstr = Chars("InitModule() failed");
    }
  } else {
    return m;
  }
  return NULL;
}

static bool module_InitHW(Tdisplay hw, Chars arg) {
  Tmodule m = NULL;

  if (arg.size() <= 4) {
    return false;
  }
  arg = arg.view(4, arg.size()); // skip "-hw="

  const size_t at = arg.find(Chars("@"));
  const size_t comma = arg.find(Chars(","));
  const size_t separator = at < comma ? at : comma;

  Chars name = arg.view(0, separator != size_t(-1) ? separator : arg.size());
  if (name == Chars("X")) {
    name = Chars("X11");
  }

  String alloc_name;

  if (!alloc_name.format("hw_", name)) {
    Errstr = "Out of memory!";
  } else if (!(m = DlLoadAny(name = alloc_name))) {
    log(ERROR) << "twdisplay: unable to load display driver module `" << name //
               << "' :\n      " << Errstr << "\n";
  } else {
    log(INFO) << "twdisplay: starting display driver module `" << name << "'...\n";

    bool (*fnInit)(Tdisplay hw);
    if (!(fnInit = m->DoInit) || !fnInit(hw)) {
      log(ERROR) << "twdisplay: ...module `" << name << "' failed to start.\n";
    } else {
      log(INFO) << "twdisplay: ...module `" << name << "' successfully started.\n";
      hw->Module = m;
      m->Used++;
      return true;
    }
    /*m->Delete();*/
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// Sdisplay methods

static Tdisplay CreateDisplayHW(Chars name);

Tdisplay Sdisplay::Init(Chars name) {
  if (!Sobj::Init(Tdisplay_class_id) || !Name.format(name)) {
    return NULL;
  }
  Module = NULL;
  /*
   * ->Quitted will be set to false only
   * after DisplayHW->InitHW() has succeeded.
   */
  Quitted = true;
  AttachSlot = NOSLOT;
  return this;
}

void Sdisplay::Delete() {
}

void Sdisplay::Remove() {
}

static Sdisplay _HW;

void warn_NoHW(uldat len, const char *arg, uldat tried) {
  (void)tried;
  log(ERROR) << "twdisplay: All display drivers failed";
  if (arg) {
    log(ERROR) << " for `" << Chars(arg, len) << "'";
  }
  log(ERROR) << "\n";
}

static void UpdateFlagsHW(Tdisplay hw) NOTHROW {
  if (!hw->Quitted) {
    NeedOldVideo = (hw->FlagsHW & FlagNeedOldVideoHW) != 0;
    ExpensiveFlushVideo = (hw->FlagsHW & FlagExpensiveFlushVideoHW) != 0;
    CanDragArea = hw->fnCanDragArea != 0;
  }
}

/*
 * InitDisplay runs HW specific InitXXX() functions, starting from best setup
 * and falling back in case some of them fails.
 */
static bool InitDisplay(Tdisplay hw) {
  if (!hw) {
    return false;
  }
  Chars arg = hw->Name;
  uldat tried = 0;
  bool success;

  hw->DisplayIsCTTY = false;
  hw->NeedHW = hw->FlagsHW = 0;

  if (arg.starts_with(Chars("-hw="))) {
    arg = arg.view(4, arg.size());
  } else {
    arg = Chars();
  }
#define TRY4(hw_name) (tried++, module_InitHW(hw, Chars(hw_name)))

  if (arg) {
    success = module_InitHW(hw, hw->Name);
  } else {
    success = TRY4("-hw=xft") || TRY4("-hw=X11") || TRY4("-hw=twin") ||
#if 0 /* cannot use `--hw=display' inside twdisplay! */
        TRY4("-hw=display") ||
#endif
              TRY4("-hw=tty");
  }
  if (success) {
    hw->Quitted = false;
    if (!DisplayHWCTTY && hw->DisplayIsCTTY) {
      DisplayHWCTTY = hw;
    }
    UpdateFlagsHW(hw);
  } else {
    warn_NoHW(hw->Name.size(), hw->Name.data(), tried);
  }
  return success;
}

static void QuitDisplay(Tdisplay hw) {
  if (hw && hw->fnQuitHW) {
    hw->QuitHW();
  }
}

bool Sdisplay::DoInit() {
  return InitDisplay(this);
}

void Sdisplay::DoQuit() {
  QuitDisplay(this);
}

static Tdisplay CreateDisplayHW(Chars name) {
  return _HW.Init(name);
}

static bool IsValidNameHW(Chars carg) NOTHROW {
  const char *arg = carg.data();
  size_t len = carg.size();
  uldat i;
  byte b;
  if (len >= 4 && !memcmp(arg, "-hw=", 4)) {
    arg += 4, len -= 4;
  }
  for (i = 0; i < len; i++) {
    b = arg[i];
    if (b == '@' || b == ',')
      /* the rest are options - validated by each display HW */
      break;
    if ((b < '0' || b > '9') && (b < 'A' || b > 'Z') && (b < 'a' || b > 'z') && b != '_') {
      log(ERROR) << "twdisplay: invalid non-alphanumeric character 0x" << hex(b)
                 << " in display HW name: `" << Chars(arg, len) << "'\n";
      return false;
    }
  }
  return true;
}

static Tdisplay AttachDisplayHW(Chars arg, uldat slot, byte flags) {
  (void)flags;
  if (arg && !arg.starts_with(Chars("-hw="))) {
    log(ERROR) << "twdisplay: specified `" << arg << "' is not `--hw=<display>'\n";
    return NULL;
  }
  Tdisplay hw;
  if (IsValidNameHW(arg) && (hw = CreateDisplayHW(arg))) {
    hw->AttachSlot = slot;
    if (hw->DoInit()) {
      return hw;
    }
  }
  return NULL;
}

/* server-side hw_display already does optimization for us */
static inline void OptimizeChangedVideo(void) {
}

static inline void SyncOldVideo(void) NOTHROW {
  ldat start, len;
  ldat i;

  if (ChangedVideoFlag) {
    for (i = 0; i < (ldat)DisplayHeight * 2; i++) {
      start = ChangedVideo[i >> 1][i & 1][0];

      if (start >= 0) {
        len = ChangedVideo[i >> 1][i & 1][1] + 1 - start;
        start += (i >> 1) * (ldat)DisplayWidth;

        ChangedVideo[i >> 1][i & 1][0] = -1;

        CopyMem(Video + start, OldVideo + start, len * sizeof(tcell));
      }
    }
  }
}

void FlushHW(void) {
  if (!ValidVideo) {
    return;
  }
  Tdisplay hw = &_HW;

  if (QueuedDrawArea2FullScreen) {
    QueuedDrawArea2FullScreen = false;
    DirtyVideo(0, 0, DisplayWidth - 1, DisplayHeight - 1);
    ValidOldVideo = false;
  } else if (hw->RedrawVideo) {
    DirtyVideo(hw->RedrawLeft, hw->RedrawUp, hw->RedrawRight, hw->RedrawDown);
    ValidOldVideo = false;
  } else if (NeedOldVideo && ValidOldVideo) {
    OptimizeChangedVideo();
  }
  hw->FlushVideo();

  hw->RedrawVideo = false;

  if (hw->NeedHW & NeedFlushHW) {
    hw->FlushHW();
  }
  if (NeedHW & NeedFlushStdout) {
    fflush(stdout), NeedHW &= ~NeedFlushStdout;
  }
  SyncOldVideo();

  ChangedVideoFlag = false;
  ValidOldVideo = true;
}

void ResizeDisplayPrefer(Tdisplay hw) {
  hw->DetectSize(&TryDisplayWidth, &TryDisplayHeight);
  NeedHW |= NeedResizeDisplay;
}

static byte ReAllocVideo(dat Width, dat Height) {
  byte change = DisplayWidth != Width || DisplayHeight != Height;

  if (!NeedOldVideo && OldVideo) {
    FreeMem(OldVideo);
    OldVideo = NULL;
  } else if ((NeedOldVideo && !OldVideo) || change) {
    if (!(OldVideo = (tcell *)ReAllocMem(OldVideo, (ldat)Width * Height * sizeof(tcell))) &&
        Width && Height) {
      OutOfMemory();
      Quit(1);
    }
    ValidOldVideo = tfalse;
  }

  if (!Video || change) {
    DisplayWidth = Width;
    DisplayHeight = Height;

    if ((!(Video =
               (tcell *)ReAllocMem(Video, (ldat)DisplayWidth * DisplayHeight * sizeof(tcell))) ||
         !(ChangedVideo =
               (dat(*)[2][2])ReAllocMem(ChangedVideo, (ldat)DisplayHeight * sizeof(dat) * 4))) &&
        DisplayWidth && DisplayHeight) {

      OutOfMemory();
      Quit(1);
    }
    ValidVideo = tfalse;
    std::memset(ChangedVideo, 0xff, (ldat)DisplayHeight * sizeof(dat) * 4);
  }
  return change;
}

/*
 * return ttrue if DisplayWidth or DisplayHeight were changed
 */
static byte ResizeDisplay(void) {
  Tdisplay hw = &_HW;
  tmsg tw_msg;
  byte change = tfalse;

  if (!TryDisplayWidth || !TryDisplayHeight)
    hw->DetectSize(&TryDisplayWidth, &TryDisplayHeight);

  hw->CheckResize(&TryDisplayWidth, &TryDisplayHeight);
  hw->Resize(TryDisplayWidth, TryDisplayHeight);

  change = ReAllocVideo(TryDisplayWidth, TryDisplayHeight);

  NeedHW &= ~NeedResizeDisplay;

  TryDisplayWidth = TryDisplayHeight = 0;

  if (change && (tw_msg = TwCreateMsg(TW_MSG_DISPLAY, TW_SIZEOF_TEVENT_DISPLAY))) {
    tw_msg->Event.EventDisplay.Code = TW_EV_DPY_Resize;
    tw_msg->Event.EventDisplay.Len = 0;
    tw_msg->Event.EventDisplay.X = DisplayWidth;
    tw_msg->Event.EventDisplay.Y = DisplayHeight;
    TwBlindSendMsg(tw_helper, tw_msg);
  }
  return change;
}

byte AllHWCanDragAreaNow(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  Tdisplay hw = &_HW;
  return (CanDragArea && hw->fnCanDragArea && hw->CanDragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp));
}

void DragAreaHW(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  Tdisplay hw = &_HW;
  hw->DragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp);
}

void SetPaletteHW(udat N, udat R, udat G, udat B) {
  /* nop */
  (void)N;
  (void)R;
  (void)G;
  (void)B;
}

void ResetPaletteHW(void) {
  Tdisplay hw = &_HW;
  hw->ResetPalette();
}

static void HandleMsg(tmsg msg) {
  tevent_display EventD;
  Tdisplay hw = &_HW;

  switch (msg->Type) {
  case TW_MSG_SELECTION:
    /* should never happen */
    log(WARNING) << "\ntwdisplay: HandleMsg(): unexpected Selection Message from twin!\n";
    break;
  case TW_MSG_SELECTIONREQUEST:
#if 0
    log(INFO) << "twdisplay: Selection Request from 0x"
              << hex(msg->Event.EventSelectionRequest.Requestor) << ", owner is underlying HW\n";
#endif
    /* request selection from underlying HW */

    /*
     * Just like in TwinSelectionGetOwner() : normally Requestor
     * is a meaningful pointer; here it is just a libtw Id.
     * Cast it to (Tobj) as expected by HW displays...
     * we will cast it back when needed
     */
    hw->SelectionRequest((Tobj)(topaque)msg->Event.EventSelectionRequest.Requestor,
                         msg->Event.EventSelectionRequest.ReqPrivate);
    break;
  case TW_MSG_SELECTIONNOTIFY:
#if 0
    log(INFO) << "twdisplay: Selection Notify to underlying HW\n";
#endif
    /* notify selection to underlying HW */

    hw->SelectionNotify(
        msg->Event.EventSelectionNotify.ReqPrivate,                                 //
        e_id(msg->Event.EventSelectionNotify.Magic),                                //
        Chars::from_c_maxlen(msg->Event.EventSelectionNotify.MIME, TW_MAX_MIMELEN), //
        Chars(msg->Event.EventSelectionNotify.Data, msg->Event.EventSelectionNotify.Len));
    break;
  case TW_MSG_DISPLAY:
    EventD = &msg->Event.EventDisplay;
    switch (EventD->Code) {
    case TW_EV_DPY_DrawTCell:
      if (EventD->Len /= sizeof(tcell)) {
        if (EventD->X + EventD->Len > DisplayWidth || EventD->Y >= DisplayHeight) {
          /*
           * in a perfect world this should not happen,
           * but with our asyncronous display size negotiation,
           * it actually can.
           */

          if (EventD->Y < DisplayHeight && EventD->X < DisplayWidth)
            EventD->Len = DisplayWidth - EventD->X;
          else
            break;
        }
        DirtyVideo(EventD->X, EventD->Y, EventD->X + EventD->Len - 1, EventD->Y);
        CopyMem(EventD->Data, &Video[EventD->X + EventD->Y * (ldat)DisplayWidth],
                (uldat)EventD->Len * sizeof(tcell));
      }
      break;
    case TW_EV_DPY_FlushHW:
      ValidVideo = ttrue;
      FlushHW();
      break;
    case TW_EV_DPY_SetCursorType:
      if (EventD->Len == sizeof(uldat)) {
        SetCursorType(deserialize<uldat>(EventD->Data));
      }
      break;
    case TW_EV_DPY_MoveToXY:
      MoveToXY(EventD->X, EventD->Y);
      break;
    case TW_EV_DPY_Resize:
      /*
       * twin told us the new display size.
       * don't detect/check anything, just apply it
       * (if it is meaningful and different from current)
       */
      if (EventD->X != DisplayWidth || EventD->Y != DisplayHeight) {
        hw->Resize(EventD->X, EventD->Y);
        ReAllocVideo(EventD->X, EventD->Y);
      }
      break;
    case TW_EV_DPY_SelectionExport:
      NeedHW |= NeedSelectionExport;
      break;
    case TW_EV_DPY_DragArea:
#define c(index) (deserialize<udat>(EventD->Data, (index) * sizeof(udat)))
      if (EventD->Len == 4 * sizeof(udat))
        DragArea(EventD->X, EventD->Y, c(0), c(1), c(2), c(3));
#undef c
      break;
    case TW_EV_DPY_Beep:
      hw->Beep();
      break;
    case TW_EV_DPY_Configure:
      if (EventD->X == HW_MOUSEMOTIONEVENTS)
        MouseMotionN = EventD->Y > 0;
      hw->Configure(EventD->X, EventD->Y == -1, EventD->Y);
      break;
    case TW_EV_DPY_SetPalette:
#define c(index) (deserialize<udat>(EventD->Data, (index) * sizeof(udat)))
      if (EventD->Len == 3 * sizeof(udat))
        hw->SetPalette(EventD->X, c(0), c(1), c(2));
#undef c
      break;
    case TW_EV_DPY_ResetPalette:
      hw->ResetPalette();
      break;
    case TW_EV_DPY_Helper:
      tw_helper = deserialize<uldat>(EventD->Data);
      break;
    case TW_EV_DPY_Quit:
      Quit(0);
      break;
    default:
      break;
    }
  }
}

static void SocketIO(void) {
  tmsg msg;
  while ((msg = TwReadMsg(tfalse))) {
    HandleMsg(msg);
  }
}

void SelectionExport(void) {
  Tdisplay hw = &_HW;
  hw->SelectionExport();
  NeedHW &= ~NeedSelectionExport;
}

/* HW back-end function: get selection owner */

/*
 * In the same function in twin server, this returns a meaningful pointer.
 * Here, it returns just an Id coming from libtw.
 * Cheat and cast to to (Tobj), since the underlying display HW code
 * treats it as opaque. We will cast it back to (uldat) when needed.
 */
Tobj TwinSelectionGetOwner(void) {
  return (Tobj)(topaque)TwGetOwnerSelection();
}

/* HW back-end function: set selection owner */
void TwinSelectionSetOwner(Tobj Owner, tany Time, tany Frac) {
  tmsg msg;
  (void)Owner;
  (void)Time;
  (void)Frac;
  if ((msg = TwCreateMsg(TW_MSG_SELECTIONCLEAR, sizeof(tevent_common)))) {
    TwBlindSendMsg(tw_helper, msg);
  }
}

/* HW back-end function: notify selection */
void TwinSelectionNotify(Tobj Requestor, uldat ReqPrivate, e_id Magic, Chars mime, Chars data) {
#if 0
  log(INFO) << "twdisplay: Selection Notify to 0x" << hex((topaque)Requestor) << "\n";
#endif
  char mimeBuf[TW_MAX_MIMELEN] = {};
  CopyMem(mime.data(), mimeBuf, Min2u(mime.size(), TW_MAX_MIMELEN));

  /* cast back Requestor from fake (Tobj) to its original (uldat) */
  TwNotifySelection((topaque)Requestor, ReqPrivate, Magic, mimeBuf, data.size(), data.data());
}

/* HW back-end function: request selection */
void TwinSelectionRequest(Tobj Requestor, uldat ReqPrivate, Tobj Owner) {
#if 1
  (void)Requestor;
#else
  log(INFO) << "twdisplay: Selection Request from 0x" << hex((topaque)Requestor) //
            << ", Owner is 0x" << hex((topaque)Owner);
#endif
  /* cast back Owner from the fake (Tobj) to (uldat) */
  TwRequestSelection((topaque)Owner, ReqPrivate);
}

static byte StdAddMouseEvent(Tdisplay hw, udat Code, dat MouseX, dat MouseY) {
  tevent_mouse Event;
  tmsg msg;

  if (hw && (hw->FlagsHW & FlagNoInputHW)) {
    return ttrue;
  }
  if ((msg = TwCreateMsg(TW_MSG_WIDGET_MOUSE, sizeof(event_mouse)))) {
    Event = &msg->Event.EventMouse;

    Event->Code = Code;
    Event->ShiftFlags = (udat)0;
    Event->X = MouseX;
    Event->Y = MouseY;

    TwBlindSendMsg(tw_helper, msg);
    return ttrue;
  }
  return tfalse;
}

byte MouseEventCommon(Tdisplay hw, dat x, dat y, dat dx, dat dy, udat Buttons) {
  dat prev_x, prev_y;
  udat OldButtons, i;
  mouse_state *OldState;
  udat result;
  byte ret = ttrue;

  OldState = &hw->MouseState;
  OldButtons = OldState->keys;
  prev_x = OldState->x;
  prev_y = OldState->y;

  x = Max2(x, 0);
  x = Min2(x, DisplayWidth - 1);
  OldState->delta_x = x == 0 ? Min2(dx, 0) : x == DisplayWidth - 1 ? Max2(dx, 0) : 0;

  y = Max2(y, 0);
  y = Min2(y, DisplayHeight - 1);
  OldState->delta_y = y == 0 ? Min2(dy, 0) : y == DisplayHeight - 1 ? Max2(dy, 0) : 0;

  if (x != prev_x || y != prev_y) {
    hw->FlagsHW |= FlagChangedMouseFlagHW;
  }
  OldState->x = x;
  OldState->y = y;

  OldState->keys = Buttons;

  if (Buttons != OldButtons || ((MouseMotionN || OldButtons) && (x != prev_x || y != prev_y))) {

    if ((MouseMotionN || OldButtons) && (x != prev_x || y != prev_y)) {
      ret = StdAddMouseEvent(hw, MOVE_MOUSE | OldButtons, x, y);
    }
    for (i = 0; i < BUTTON_N_MAX && ret; i++) {
      if ((Buttons & HOLD_CODE(i)) != (OldButtons & HOLD_CODE(i))) {
        result = (Buttons & HOLD_CODE(i) ? PRESS_CODE(i) : RELEASE_CODE(i)) |
                 (OldButtons &= ~HOLD_CODE(i));
        OldButtons |= Buttons & HOLD_CODE(i);
        ret = StdAddMouseEvent(hw, result, x, y);
      }
    }
  }
  return ret;
}

bool KeyboardEventCommon(Tdisplay hw, udat Code, udat ShiftFlags, udat Len, const char *Seq) {
  tevent_keyboard Event;
  tmsg msg;

  if (hw->FlagsHW & FlagNoInputHW) {
    return true;
  }
  if ((msg = TwCreateMsg(TW_MSG_WIDGET_KEY, Len + 1 + sizeof(s_tevent_keyboard)))) {
    Event = &msg->Event.EventKeyboard;

    Event->Code = Code;
    Event->ShiftFlags = ShiftFlags;
    Event->SeqLen = Len;
    CopyMem(Seq, Event->AsciiSeq, Len);
    Event->AsciiSeq[Len] = '\0'; /* terminate string with \0 */

    TwBlindSendMsg(tw_helper, msg);
    return true;
  }
  return false;
}

static void MainLoop(Tdisplay hw, int fd) {
  struct timeval sel_timeout;
  fd_set read_fds, write_fds, *pwrite_fds;
  uldat err, detail;
  int sys_errno, num_fds = 0;

  for (;;) {
    if (GotSignals) {
      HandleSignals();
    }
    if (NeedHW & NeedResizeDisplay) {
      ResizeDisplay();
    }
    if (NeedHW & NeedSelectionExport) {
      SelectionExport();
    }
    /*
     * don't FlushHW() unconditionalluy here,
     * as it gets called syncronously
     * with the main twin from HandleEvent()...
     * just update what is needed.
     */
    if (hw->RedrawVideo) {
      FlushHW();
    } else {
      hw->UpdateMouseAndCursor();
      if (hw->NeedHW & NeedFlushHW) {
        hw->FlushHW();
      }
      if (NeedHW & NeedFlushStdout) {
        fflush(stdout), NeedHW &= ~NeedFlushStdout;
      }
    }

    read_fds = save_rfds;

    if (!FdWQueued) {
      pwrite_fds = NULL;
    } else {
      write_fds = save_wfds;
      pwrite_fds = &write_fds;
    }

    if (!TwFlush()) {
      break;
    }
    /*
     * messages can arrive during Tw* function calls,
     * so the FD_ISSET() test alone does not suffice.
     * also use an inactivity timeout: 2min...
     */
    sel_timeout.tv_sec = TwPendingMsg() ? 0 : 120;
    sel_timeout.tv_usec = 0;

    if (NeedHW & NeedPanicHW) {
      break;
    }
    num_fds = OverrideSelect(max_fds + 1, &read_fds, pwrite_fds, NULL, &sel_timeout);

    if (num_fds < 0 && errno != EINTR) {
      /* ach, problem. */
      break;
    }
    if (num_fds == 0 && !TwPendingMsg()) {
      /*
       * no activity during timeout delay && no pending messages..
       * just ping twin to keep connection active..
       * however, if width <= 0 : means we lost connection, so exit..
       */
      if (TwGetDisplayWidth() <= 0) {
        QuitDisplay(hw);
        log(ERROR) << "twdisplay: lost connection to TWIN.. \n";
        exit(1);
      }
    }

    if ((num_fds > 0 && FD_ISSET(fd, &read_fds)) || TwPendingMsg()) {
      /*
       * messages can arrive during Tw* function calls,
       * so the FD_ISSET() test alone does not suffice.
       */
      if (FD_ISSET(fd, &read_fds)) {
        num_fds--;
        FD_CLR(fd, &read_fds);
      }
      SocketIO();
    }
    if (num_fds > 0) {
      RemoteEvent(num_fds, &read_fds);
    }
  }

  if (NeedHW & NeedPanicHW) {
    Quit(0);
  }
  if (num_fds < 0 && errno != EINTR) {
    QuitDisplay(hw);
    log(ERROR) << "twdisplay: select(): " << Chars::from_c(strerror(errno)) << "\n";
    exit(1);
  }
  if (TwInPanic()) {
    err = TwErrno;
    detail = TwErrnoDetail;
    QuitDisplay(hw);
    fprintf(stderr, "" SS ": libtw error: " SS "" SS "\n", MYname, TwStrError(err),
            TwStrErrorDetail(err, detail));
    exit(1);
  }
  err = TwErrno;
  detail = TwErrnoDetail;
  sys_errno = errno;
  QuitDisplay(hw);
  fprintf(stderr,
          "" SS ": shouldn't happen! Please report:\n"
          "\tlibtw TwErrno: %d(%d),\t" SS "" SS "\n"
          "\tsystem  errno: %d,\t" SS "\n",
          MYname, err, detail, TwStrError(err), TwStrErrorDetail(err, detail), sys_errno,
          strerror(sys_errno));
  exit(1);
}

dat GetDisplayWidth(void) NOTHROW {
  return DisplayWidth;
}
dat GetDisplayHeight(void) NOTHROW {
  return DisplayHeight;
}

static void Usage(void) NOTHROW {
  fputs("Usage: twdisplay [OPTIONS]\n"
        "Currently known options: \n"
        " -h, --help               display this help and exit\n"
        " -V, --version            output version information and exit\n"
        " -s, --share              allow multiple simultaneous displays (default)\n"
        " -x, --excl               request exclusive display - detach all others\n"
        " -v, --verbose            verbose output (default)\n"
        " -q, --quiet              quiet - don't report messages from twin server\n"
        " -f, --force              force running even with wrong protocol version\n"
        " --twin@<TWDISPLAY>       specify server to contact instead of $TWDISPLAY\n"
        " --hw=<display>[,options] start the given display (only one --hw=... allowed)\n"
        "                          (default: autoprobe all displays until one succeeds)\n"
        " --plugindir=DIRECTORY    set directory where to look for --hw=... libraries\n"
        "Currently known display drivers: \n"
        "\txft[@<XDISPLAY>]\n"
        "\tx11[@<XDISPLAY>]\n"
        "\ttty[@<tty device>]\n"
        "\ttwin[@<TWDISPLAY>]\n",
        stdout);
}

static void TryUsage(const char *opt) NOTHROW {
  if (opt) {
    fprintf(stdout, "twdisplay: unknown option `" SS "'\n", opt);
  }
  fputs("           try `twdisplay --help' for usage summary.\n", stdout);
}

static void ShowVersion(void) NOTHROW {
  fputs("twdisplay " TWIN_VERSION_STR " with socket protocol " TW_PROTOCOL_VERSION_STR "\n",
        stdout);
}

static bool VersionsMatch(bool force) {
  uldat cv = TW_PROTOCOL_VERSION, lv = TwLibraryVersion(), sv = TwServerVersion();

  if (lv != sv || lv != cv) {
    String lib_verstr;
    String srv_verstr;
    lib_verstr.format(TW_VER_MAJOR(lv), ".", TW_VER_MINOR(lv), ".", TW_VER_PATCH(lv));
    srv_verstr.format(TW_VER_MAJOR(sv), ".", TW_VER_MINOR(sv), ".", TW_VER_PATCH(sv));

    if (force) {
      log(WARNING) << "twdisplay: warning: socket protocol version mismatch!  (ignored)"
                      "\n           client is " TW_PROTOCOL_VERSION_STR ", library is "
                   << lib_verstr << ", server is " << srv_verstr << "\n";
      return false;
    } else {
      log(ERROR) << "twdisplay: fatal: socket protocol version mismatch!"
                    "\n           client is " TW_PROTOCOL_VERSION_STR ", library is "
                 << lib_verstr << ", server is " << srv_verstr << "\n";
    }
  }
  return true;
}

static void MergeHyphensArgv(int argc, char **argv) NOTHROW {
  const char *s;
  while (argc) {
    if ((s = *argv) && s[0] == '-' && s[1] == '-' && s[2] && s[3])
      (*argv)++;
    argv++, argc--;
  }
}

TW_DECL_MAGIC(display_magic);

int main(int argc, char *argv[]) {
  const char *tty = ttyname(0);
  const char *dpy = NULL, *client_dpy = NULL;
  const char *arg_hw = NULL;
  int fd;
  byte flags = TW_ATTACH_HW_REDIRECT;
  byte ret = 0, ourtty = 0;
  bool force = false;

  MergeHyphensArgv(argc, argv);

  MYname = argv[0];

  while (*++argv) {
    Chars argi = Chars::from_c(*argv);
    if (argi == Chars("-V") || argi == Chars("-version")) {
      ShowVersion();
      return 0;
    } else if (argi == Chars("-h") || argi == Chars("-help")) {
      Usage();
      return 0;
    } else if (argi == Chars("-x") || argi == Chars("-excl")) {
      flags |= TW_ATTACH_HW_EXCLUSIVE;
    } else if (argi == Chars("-s") || argi == Chars("-share")) {
      flags &= ~TW_ATTACH_HW_EXCLUSIVE;
    } else if (argi == Chars("-v") || argi == Chars("-verbose")) {
      flags |= TW_ATTACH_HW_REDIRECT;
    } else if (argi == Chars("-q") || argi == Chars("-quiet")) {
      flags &= ~TW_ATTACH_HW_REDIRECT;
    } else if (argi == Chars("-f") || argi == Chars("-force")) {
      force = true;
    } else if (argi.starts_with(Chars("-plugindir="))) {
      plugindir = argi.view(11, argi.size());
    } else if (argi.starts_with(Chars("-twin@"))) {
      dpy = argi.data() + 6;
    } else if (argi.starts_with(Chars("-hw="))) {
      if (argi.size() >= 11 && argi.view(4, 11) == Chars("display")) {
        fprintf(stderr, SS ": argument `--hw=display' is for internal use only.\n", MYname);
        TryUsage(NULL);
        return 1;
      }
      if (argi.size() >= 7 && argi.view(4, 7) == Chars("tty")) {
        const char *cs = "";
        const char *opt = argi.data() + 7;
        char *s = const_cast<char *>(strchr(opt, ','));
        if (s) {
          *s = '\0';
        }
        if (!*opt) {
          /* attach twin to our tty */
          ourtty = 1;
        } else if (opt[0] == '@' && opt[1]) {
          if (opt[1] == '-') {
            /* using server controlling tty makes no sense for twattach */
            fprintf(stderr, SS ": `" SS "' makes sense only with twattach.\n", MYname, argi.data());
            return 1;
          } else if (tty) {
            if (!strcmp(opt + 1, tty)) {
              /* attach twin to our tty */
              ourtty = 1;
            }
          } else {
            fprintf(stderr, SS ": ttyname() failed: cannot find controlling tty!\n", MYname);
            return 1;
          }
        } else {
          fprintf(stderr, SS ": malformed display hw `" SS "'\n", MYname, argi.data());
          return 1;
        }

        if (s) {
          *s = ',';
          cs = s;
        }

        if (ourtty) {
          const char *term = getenv("TERM");
          if (term && !*term) {
            term = NULL;
          }
          const char *colorterm = getenv("COLORTERM");
          bool truecolor = colorterm && !strcmp(colorterm, "truecolor");
          size_t arg_hw_len =
              strlen(tty) + 9 + strlen(cs) + (term ? 6 + strlen(term) : 0) + (truecolor ? 9 : 0);
          char *arg_alloc = (char *)malloc(arg_hw_len);
          if (!arg_alloc) {
            fputs("twdisplay: out of memory!\n", stderr);
            return 1;
          }
          snprintf(arg_alloc, arg_hw_len, "-hw=tty%s%s%s%s", (term ? ",TERM=" : term),
                   (term ? term : ""), (truecolor ? ",colors=16m" : ""), cs);
          arg_hw = arg_alloc;
        } else {
          arg_hw = argi.data();
        }
      } else if (argi.size() > 4) {
        arg_hw = argi.data();
        if (argi.size() >= 8 && argi.view(4, 8) == Chars("twin")) {
          if (arg_hw[8] == '@') {
            client_dpy = arg_hw + 9;
          } else {
            client_dpy = "";
          }
        }
      } else {
        TryUsage(argi.data());
        return 1;
      }
    } else {
      TryUsage(argi.data());
      return 1;
    }
  }

#ifdef CONF__ALLOC
  /* do this as soon as possible */
  if (!InitAlloc()) {
    fputs("twdisplay: InitAlloc() failed: internal error!\n", stderr);
    return 1;
  }
#endif

  origTWDisplay = CloneStr(getenv("TWDISPLAY"));
  origTERM = CloneStr(getenv("TERM"));
  origCOLORTERM = CloneStr(getenv("COLORTERM"));

  TWDisplay = dpy ? CloneStr(dpy) : origTWDisplay;
  {
    const char *home = getenv("HOME");
    if (!home) {
      fputs("twdisplay: required environment variable HOME is not set. Aborting.\n", stderr);
      return 1;
    }
    if (!HOME.format(Chars::from_c(home))) { // also append final '\0' but do not count it
      fputs("twdisplay: out of memory! Aborting.\n", stderr);
      return 1;
    }
  }

  if (((ourtty || (client_dpy && !*client_dpy)) && origTWDisplay && TWDisplay &&
       !strcmp(origTWDisplay, TWDisplay)) ||
      (client_dpy && *client_dpy && TWDisplay && !strcmp(client_dpy, TWDisplay))) {

    if (!force) {
      fprintf(stderr,
              "twdisplay: refusing to display twin inside itself. Use option `-f' to override.\n");
      TwClose();
      return 1;
    }
  }

  InitSignals();
  InitTtysave();

#ifdef CONF__ALLOC
  TwConfigMalloc(AllocMem, ReAllocMem, free);
#endif

  if (TwCheckMagic(display_magic) && TwOpen(TWDisplay)) {
    do {
      char *buf;

      if (!VersionsMatch(force)) {
        if (!force) {
          fprintf(stderr, "twdisplay: Aborting. Use option `-f' to ignore versions check.\n");
          TwClose();
          return 1;
        }
      }

      if (RegisterRemote(fd = TwConnectionFd(), NULL, (handler_io_d)NoOp) == NOSLOT) {
        TwClose();
        OutOfMemory();
        return 1;
      }

      if (!(TMsgPort = TwCreateMsgPort(9, "twdisplay"))) {
        break;
      }

      DisplayWidth = TryDisplayWidth = TwGetDisplayWidth();
      DisplayHeight = TryDisplayHeight = TwGetDisplayHeight();

      Tdisplay hw;

      /* if user did not specify any `--hw=<dpy>', autoprobe */
      if (!(hw = AttachDisplayHW(Chars::from_c(arg_hw), NOSLOT, 0))) {
        TwClose();
        return 1;
      }
      size_t buf_len = hw->Name.size() + 80;
      if (!(buf = (char *)AllocMem(buf_len))) {
        QuitDisplay(hw);
        TwClose();
        OutOfMemory();
        return 1;
      }

      snprintf(buf, buf_len, "-hw=display@(%.*s),x=%d,y=%d%s%s%s", (int)hw->Name.size(),
               hw->Name.data(), (int)hw->X, (int)hw->Y, hw->CanResize ? ",resize" : "",
               /* CanDragArea */ ttrue ? ",drag" : "", ExpensiveFlushVideo ? ",slow" : "");

      TwAttachHW(strlen(buf), buf, flags);
      TwFlush();

      flags &= TW_ATTACH_HW_REDIRECT;

      if (flags) {
        fprintf(stderr, "messages reported by twin server...\n");
      }
      for (;;) {
        uldat len;
        const char *reply = TwAttachGetReply(&len);
        if (reply <= (char *)2) {
          ret = (byte)(size_t)reply;
          break;
        } else if (reply == (char *)-1)
          /* libtw panic */
          break;

        log(INFO) << "  " << Chars(reply, len);
      }
      flushk();

      if (TwInPanic())
        break;

      if (ourtty) {
        fputs("\033[2J", stdout); // clear screen
        fflush(stdout);
      }

      /*
       * twin waits this before grabbing the display...
       * so we can fflush(stdout) to show all messages
       * *BEFORE* twin draws on (eventually) the same tty
       */
      TwAttachConfirm();

      ResizeDisplay();

      if (flags && !ourtty) {
        if (ret)
          log(INFO) << "... ok, twin successfully attached.\n";
        else
          log(ERROR) << "... ach, twin failed to attach.\n";
        flushk();
      }
      if (ret == 2)
        /*
         * twin told us to stay and sit on the display
         * until it is quitted.
         */
        MainLoop(hw, fd);
      else if (ret)
        log(WARNING) << Chars::from_c(MYname) << ": twin said we can quit... strange!\n";

      Quit(!ret);
    } while (0);
  }
  log(ERROR) << Chars::from_c(MYname) << ": libtw error: " << Chars::from_c(TwStrError(TwErrno))
             << " " << Chars::from_c(TwStrErrorDetail(TwErrno, TwErrnoDetail)) << "\n";
  return 1;
}

void Quit(int status) {
  Tdisplay hw = &_HW;
  QuitDisplay(hw);
  if (status < 0) {
    return; /* give control back to signal handler */
  }
  exit(status);
}
