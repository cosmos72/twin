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

const char *TWDisplay, *origTWDisplay, *origTERM;

char printk_buf[TW_BIGBUFF];

String HOME;

#define L 0x55 /*  85 */
#define M 0xAA /* 170 */
#define H 0xFF /* 255 */

#define A 0x40 /*  64 */
#define B 0x68 /* 104 */
#define C 0xA0 /* 144 */
#define D 0xC4 /* 196 */
#define E 0xFF /* 255 */

#define TGRAY(n) TRGB((n), (n), (n))

#ifdef TWIN_PALETTE_SOLARIZED
#define TDARK TRGB(0x04, 0x08, 0x10)
#define TLIGHT TRGB(0xBB, 0xBB, 0xBB)
#else
#define TDARK TRGB(0, 0, 0)
#define TLIGHT TRGB(M, M, M)
#endif

/* the 256-color palette, compatible with xterm-256color palette */
const trgb Palette[tpalette_n] = {
    TDARK,         TRGB(M, 0, 0), TRGB(0, M, 0), TRGB(M, M, 0), TRGB(0, 0, M), TRGB(M, 0, M),
    TRGB(0, M, M), TLIGHT, /**/
    TRGB(L, L, L), TRGB(H, L, L), TRGB(L, H, L), TRGB(H, H, L), TRGB(L, L, H), TRGB(H, L, H),
    TRGB(L, H, H), TRGB(H, H, H), /**/

    TRGB(0, 0, 0), TRGB(0, 0, A), TRGB(0, 0, B), TRGB(0, 0, C), TRGB(0, 0, D), TRGB(0, 0, E),
    TRGB(0, A, 0), TRGB(0, A, A), TRGB(0, A, B), TRGB(0, A, C), TRGB(0, A, D), TRGB(0, A, E),
    TRGB(0, B, 0), TRGB(0, B, A), TRGB(0, B, B), TRGB(0, B, C), TRGB(0, B, D), TRGB(0, B, E),
    TRGB(0, C, 0), TRGB(0, C, A), TRGB(0, C, B), TRGB(0, C, C), TRGB(0, C, D), TRGB(0, C, E),
    TRGB(0, D, 0), TRGB(0, D, A), TRGB(0, D, B), TRGB(0, D, C), TRGB(0, D, D), TRGB(0, D, E),
    TRGB(0, E, 0), TRGB(0, E, A), TRGB(0, E, B), TRGB(0, E, C), TRGB(0, E, D), TRGB(0, E, E),

    TRGB(A, 0, 0), TRGB(A, 0, A), TRGB(A, 0, B), TRGB(A, 0, C), TRGB(A, 0, D), TRGB(A, 0, E),
    TRGB(A, A, 0), TRGB(A, A, A), TRGB(A, A, B), TRGB(A, A, C), TRGB(A, A, D), TRGB(A, A, E),
    TRGB(A, B, 0), TRGB(A, B, A), TRGB(A, B, B), TRGB(A, B, C), TRGB(A, B, D), TRGB(A, B, E),
    TRGB(A, C, 0), TRGB(A, C, A), TRGB(A, C, B), TRGB(A, C, C), TRGB(A, C, D), TRGB(A, C, E),
    TRGB(A, D, 0), TRGB(A, D, A), TRGB(A, D, B), TRGB(A, D, C), TRGB(A, D, D), TRGB(A, D, E),
    TRGB(A, E, 0), TRGB(A, E, A), TRGB(A, E, B), TRGB(A, E, C), TRGB(A, E, D), TRGB(A, E, E),

    TRGB(B, 0, 0), TRGB(B, 0, A), TRGB(B, 0, B), TRGB(B, 0, C), TRGB(B, 0, D), TRGB(B, 0, E),
    TRGB(B, A, 0), TRGB(B, A, A), TRGB(B, A, B), TRGB(B, A, C), TRGB(B, A, D), TRGB(B, A, E),
    TRGB(B, B, 0), TRGB(B, B, A), TRGB(B, B, B), TRGB(B, B, C), TRGB(B, B, D), TRGB(B, B, E),
    TRGB(B, C, 0), TRGB(B, C, A), TRGB(B, C, B), TRGB(B, C, C), TRGB(B, C, D), TRGB(B, C, E),
    TRGB(B, D, 0), TRGB(B, D, A), TRGB(B, D, B), TRGB(B, D, C), TRGB(B, D, D), TRGB(B, D, E),
    TRGB(B, E, 0), TRGB(B, E, A), TRGB(B, E, B), TRGB(B, E, C), TRGB(B, E, D), TRGB(B, E, E),

    TRGB(C, 0, 0), TRGB(C, 0, A), TRGB(C, 0, B), TRGB(C, 0, C), TRGB(C, 0, D), TRGB(C, 0, E),
    TRGB(C, A, 0), TRGB(C, A, A), TRGB(C, A, B), TRGB(C, A, C), TRGB(C, A, D), TRGB(C, A, E),
    TRGB(C, B, 0), TRGB(C, B, A), TRGB(C, B, B), TRGB(C, B, C), TRGB(C, B, D), TRGB(C, B, E),
    TRGB(C, C, 0), TRGB(C, C, A), TRGB(C, C, B), TRGB(C, C, C), TRGB(C, C, D), TRGB(C, C, E),
    TRGB(C, D, 0), TRGB(C, D, A), TRGB(C, D, B), TRGB(C, D, C), TRGB(C, D, D), TRGB(C, D, E),
    TRGB(C, E, 0), TRGB(C, E, A), TRGB(C, E, B), TRGB(C, E, C), TRGB(C, E, D), TRGB(C, E, E),

    TRGB(D, 0, 0), TRGB(D, 0, A), TRGB(D, 0, B), TRGB(D, 0, C), TRGB(D, 0, D), TRGB(D, 0, E),
    TRGB(D, A, 0), TRGB(D, A, A), TRGB(D, A, B), TRGB(D, A, C), TRGB(D, A, D), TRGB(D, A, E),
    TRGB(D, B, 0), TRGB(D, B, A), TRGB(D, B, B), TRGB(D, B, C), TRGB(D, B, D), TRGB(D, B, E),
    TRGB(D, C, 0), TRGB(D, C, A), TRGB(D, C, B), TRGB(D, C, C), TRGB(D, C, D), TRGB(D, C, E),
    TRGB(D, D, 0), TRGB(D, D, A), TRGB(D, D, B), TRGB(D, D, C), TRGB(D, D, D), TRGB(D, D, E),
    TRGB(D, E, 0), TRGB(D, E, A), TRGB(D, E, B), TRGB(D, E, C), TRGB(D, E, D), TRGB(D, E, E),

    TRGB(E, 0, 0), TRGB(E, 0, A), TRGB(E, 0, B), TRGB(E, 0, C), TRGB(E, 0, D), TRGB(E, 0, E),
    TRGB(E, A, 0), TRGB(E, A, A), TRGB(E, A, B), TRGB(E, A, C), TRGB(E, A, D), TRGB(E, A, E),
    TRGB(E, B, 0), TRGB(E, B, A), TRGB(E, B, B), TRGB(E, B, C), TRGB(E, B, D), TRGB(E, B, E),
    TRGB(E, C, 0), TRGB(E, C, A), TRGB(E, C, B), TRGB(E, C, C), TRGB(E, C, D), TRGB(E, C, E),
    TRGB(E, D, 0), TRGB(E, D, A), TRGB(E, D, B), TRGB(E, D, C), TRGB(E, D, D), TRGB(E, D, E),
    TRGB(E, E, 0), TRGB(E, E, A), TRGB(E, E, B), TRGB(E, E, C), TRGB(E, E, D), TRGB(E, E, E),

    TGRAY(15),     TGRAY(25),     TGRAY(35),     TGRAY(45),     TGRAY(55),     TGRAY(65),
    TGRAY(75),     TGRAY(85),     TGRAY(95),     TGRAY(105),    TGRAY(115),    TGRAY(125),
    TGRAY(135),    TGRAY(145),    TGRAY(155),    TGRAY(165),    TGRAY(175),    TGRAY(185),
    TGRAY(195),    TGRAY(205),    TGRAY(215),    TGRAY(225),    TGRAY(235),    TGRAY(245),
};

#undef H
#undef M
#undef L

#undef A
#undef B
#undef C
#undef D
#undef E

#undef TDARK
#undef TLIGHT

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
byte AlwaysFalse(void) {
  return tfalse;
}
byte AlwaysTrue(void) {
  return ttrue;
}
void *AlwaysNull(void) {
  return NULL;
}

void gainPrivileges(void) {
}
void RemotePidIsDead(pid_t pid) {
}

int printk(const char *format, ...) {
  int i = 0;
#ifdef TW_HAVE_VPRINTF
  va_list ap;
  va_start(ap, format);
  i = vfprintf(stderr, format, ap);
  va_end(ap);
#endif
  return i;
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

uldat RegisterRemote(int Fd, Tobj HandlerData, handler_io_d HandlerIO) {
  uldat Slot, j;

  if ((Slot = FdListGet()) == NOSLOT)
    return Slot;

  memset(&LS, 0, sizeof(fdlist));

  LS.Fd = Fd;
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

  if (Fd >= 0) {
    FD_SET(Fd, &save_rfds);
    if (max_fds < Fd)
      max_fds = Fd;
  }
  return Slot;
}

void UnRegisterRemote(uldat Slot) {
  /* not needed, we are going to quit anyway if this gets called */
}

void RemoteCouldntWrite(uldat Slot) {
  if (Slot == NOSLOT || Slot >= FdTop || LS.Fd == NOFD)
    return;
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

static struct SmoduleFn _FnModule = {
    (bool (*)(Tmodule))NoOp, /* DlOpen       */
    (void (*)(Tmodule))NoOp, /* DlClose      */
};

Tmodule Smodule::Init(Chars /*name*/) {
  Fn = &_FnModule;
  Sobj::Init(Tmodule_class_id);
  return this;
}

void Smodule::Delete() {
}

void Smodule::Remove() {
}

static Smodule _Module;
static Tmodule const GModule = (_Module.Fn = &_FnModule, _Module.Init(Chars()));

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
  } else if (!(init_func = (byte(*)(Tmodule))dlsym((dlhandle)m->Handle, "InitModule"))) {
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

static bool module_InitHW(Chars arg) {
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

    bool (*InitD)(void);
    if (!(InitD = m->DoInit) || !InitD()) {
      log(ERROR) << "twdisplay: ...module `" << name << "' failed to start.\n";
    } else {
      log(INFO) << "twdisplay: ...module `" << name << "' successfully started.\n";
      HW->Module = m;
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

static struct SobjFn _FnDisplay = {};

Tdisplay Sdisplay::Init(Chars name) {
  Fn = &_FnDisplay;
  if (!Sobj::Init(Tdisplay_class_id) || !Name.format(name)) {
    return NULL;
  }
  Module = NULL;
  /*
   * ->Quitted will be set to tfalse only
   * after DisplayHW->InitHW() has succeeded.
   */
  Quitted = ttrue;
  AttachSlot = NOSLOT;
  return this;
}

void Sdisplay::Delete() {
}

void Sdisplay::Remove() {
}

static Sdisplay _HW;

void warn_NoHW(uldat len, const char *arg, uldat tried) {
  log(ERROR) << "twdisplay: All display drivers failed";
  if (arg) {
    log(ERROR) << " for `" << Chars(arg, len) << "'";
  }
  log(ERROR) << "\n";
}

static void UpdateFlagsHW(void) NOTHROW {
  if (!HW->Quitted) {
    NeedOldVideo = HW->FlagsHW & FlHWNeedOldVideo;
    ExpensiveFlushVideo = HW->FlagsHW & FlHWExpensiveFlushVideo;
    CanDragArea = !!HW->CanDragArea;
  }
}

/*
 * InitDisplay runs HW specific InitXXX() functions, starting from best setup
 * and falling back in case some of them fails.
 */
static bool InitDisplay(Tdisplay display) {
  if (!display) {
    return false;
  }
  Chars arg = display->Name;
  uldat tried = 0;
  bool success;

  SaveHW;
  SetHW(display);

  display->DisplayIsCTTY = display->NeedHW = display->FlagsHW = tfalse;

  if (arg.starts_with(Chars("-hw="))) {
    arg = arg.view(4, arg.size());
  } else {
    arg = Chars();
  }
#define TRY4(hw_name) (tried++, module_InitHW(Chars(hw_name)))

  if (arg) {
    success = module_InitHW(display->Name);
  } else {
    success = TRY4("-hw=xft") || TRY4("-hw=X11") || TRY4("-hw=twin") ||
#if 0 /* cannot use `--hw=display' inside twdisplay! */
        TRY4("-hw=display") ||
#endif
              TRY4("-hw=tty");
  }
  if (success) {
    display->Quitted = tfalse;
    if (!DisplayHWCTTY && display->DisplayIsCTTY)
      DisplayHWCTTY = display;
    UpdateFlagsHW();
  } else
    warn_NoHW(display->Name.size(), HW->Name.data(), tried);

  RestoreHW;

  return success;
}

static void QuitDisplay(Tdisplay display) {
  if (display && display->QuitHW) {
    HW = display;
    display->QuitHW();
  }
}

bool Sdisplay::DoInit() {
  return InitDisplay(this);
}

void Sdisplay::DoQuit() {
  QuitDisplay(this);
}

static Tdisplay CreateDisplayHW(Chars name) {
  return HW = _HW.Init(name);
}

static byte IsValidHW(Chars carg) NOTHROW {
  const char *arg = carg.data();
  size_t len = carg.size();
  uldat i;
  byte b;
  if (len >= 4 && !memcmp(arg, "-hw=", 4))
    arg += 4, len -= 4;

  for (i = 0; i < len; i++) {
    b = arg[i];
    if (b == '@' || b == ',')
      /* the rest are options - validated by each display HW */
      break;
    if ((b < '0' || b > '9') && (b < 'A' || b > 'Z') && (b < 'a' || b > 'z') && b != '_') {
      log(ERROR) << "twdisplay: invalid non-alphanumeric character 0x" << hex(b)
                 << " in display HW name: `" << Chars(arg, len) << "'\n";
      return tfalse;
    }
  }
  return ttrue;
}

static Tdisplay AttachDisplayHW(Chars arg, uldat slot, byte flags) {
  if (arg && !arg.starts_with(Chars("-hw="))) {
    log(ERROR) << "twdisplay: specified `" << arg << "' is not `--hw=<display>'\n";
    return NULL;
  }
  if (IsValidHW(arg) && CreateDisplayHW(arg)) {
    HW->AttachSlot = slot;
    if (HW->DoInit())
      return HW;
  }
  return NULL;
}

#if 0
/* not needed, server-side hw_display already does optimization for us */
inline void OptimizeChangedVideo(void) {
    uldat _start, start, _end, end;
    ldat i;

    for (i=0; i<(ldat)DisplayHeight*2; i++) {
        start = (uldat)ChangedVideo[i>>1][!(i&1)][0];

        if (start != (uldat)-1) {

            start += (i>>1) * (ldat)DisplayWidth;
            _start = start;

            _end = end = (uldat)ChangedVideo[i>>1][!(i&1)][1] + (i>>1) * (ldat)DisplayWidth;

            while (start <= end && Video[start] == OldVideo[start])
                start++;
            while (start <= end && Video[end] == OldVideo[end])
                end--;

            if (start > end) {
                if (i&1) {
                    /*
                     * this is the first area, to make it empty
                     * copy the second on this.
                     */
                    if (ChangedVideo[i>>1][1][0] != -1) {
                        ChangedVideo[i>>1][0][0] = ChangedVideo[i>>1][1][0];
                        ChangedVideo[i>>1][0][1] = ChangedVideo[i>>1][1][1];
                        ChangedVideo[i>>1][1][0] = -1;
                    } else
                        ChangedVideo[i>>1][0][0] = -1;
                } else
                    ChangedVideo[i>>1][1][0] = -1;
                continue;
            } else if (start > _start || end < _end) {
                ChangedVideo[i>>1][!(i&1)][0] += start - _start;
                ChangedVideo[i>>1][!(i&1)][1] -= _end - end;
            }
        }
    }
}
#else
#define OptimizeChangedVideo()                                                                     \
  do {                                                                                             \
  } while (0)
#endif

inline void SyncOldVideo(void) NOTHROW {
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

  if (!ValidVideo)
    return;

  if (QueuedDrawArea2FullScreen) {
    QueuedDrawArea2FullScreen = false;
    DirtyVideo(0, 0, DisplayWidth - 1, DisplayHeight - 1);
    ValidOldVideo = tfalse;
  } else if (HW->RedrawVideo) {
    DirtyVideo(HW->RedrawLeft, HW->RedrawUp, HW->RedrawRight, HW->RedrawDown);
    ValidOldVideo = tfalse;
  } else if (NeedOldVideo && ValidOldVideo)
    OptimizeChangedVideo();

  HW->FlushVideo();

  HW->RedrawVideo = tfalse;

  if (HW->NeedHW & NEEDFlushHW)
    HW->FlushHW();

  if (NeedHW & NEEDFlushStdout)
    fflush(stdout), NeedHW &= ~NEEDFlushStdout;

  SyncOldVideo();

  ChangedVideoFlag = tfalse;
  ValidOldVideo = ttrue;
}

void ResizeDisplayPrefer(Tdisplay display) {
  SaveHW;
  SetHW(display);
  display->DetectSize(&TryDisplayWidth, &TryDisplayHeight);
  NeedHW |= NEEDResizeDisplay;
  RestoreHW;
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
  byte change = tfalse;
  tmsg tw_msg;

  if (!TryDisplayWidth || !TryDisplayHeight)
    HW->DetectSize(&TryDisplayWidth, &TryDisplayHeight);

  HW->CheckResize(&TryDisplayWidth, &TryDisplayHeight);
  HW->Resize(TryDisplayWidth, TryDisplayHeight);

  change = ReAllocVideo(TryDisplayWidth, TryDisplayHeight);

  NeedHW &= ~NEEDResizeDisplay;

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
  return (CanDragArea && HW->CanDragArea && HW->CanDragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp));
}

void DragAreaHW(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  HW->DragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp);
}

void SetPaletteHW(udat N, udat R, udat G, udat B) {
  /* nop */
}

void ResetPaletteHW(void) {
  HW->ResetPalette();
}

static void HandleMsg(tmsg msg) {
  tevent_display EventD;

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
    HW->HWSelectionRequest((Tobj)(topaque)msg->Event.EventSelectionRequest.Requestor,
                           msg->Event.EventSelectionRequest.ReqPrivate);
    break;
  case TW_MSG_SELECTIONNOTIFY:
#if 0
    log(INFO) << "twdisplay: Selection Notify to underlying HW\n";
#endif
    /* notify selection to underlying HW */

    HW->HWSelectionNotify(
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
      if (EventD->Len == sizeof(uldat))
        SetCursorType(deserialize<uldat>(EventD->Data));
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
        HW->Resize(EventD->X, EventD->Y);
        ReAllocVideo(EventD->X, EventD->Y);
      }
      break;
    case TW_EV_DPY_SelectionExport:
      NeedHW |= NEEDSelectionExport;
      break;
    case TW_EV_DPY_DragArea:
#define c(index) (deserialize<udat>(EventD->Data, (index) * sizeof(udat)))
      if (EventD->Len == 4 * sizeof(udat))
        DragArea(EventD->X, EventD->Y, c(0), c(1), c(2), c(3));
#undef c
      break;
    case TW_EV_DPY_Beep:
      HW->Beep();
      break;
    case TW_EV_DPY_Configure:
      if (EventD->X == HW_MOUSEMOTIONEVENTS)
        MouseMotionN = EventD->Y > 0;
      HW->Configure(EventD->X, EventD->Y == -1, EventD->Y);
      break;
    case TW_EV_DPY_SetPalette:
#define c(index) (deserialize<udat>(EventD->Data, (index) * sizeof(udat)))
      if (EventD->Len == 3 * sizeof(udat))
        HW->SetPalette(EventD->X, c(0), c(1), c(2));
#undef c
      break;
    case TW_EV_DPY_ResetPalette:
      HW->ResetPalette();
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
  while ((msg = TwReadMsg(tfalse)))
    HandleMsg(msg);
}

void SelectionExport(void) {
  HW->HWSelectionExport();
  NeedHW &= ~NEEDSelectionExport;
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
#if 0
  log(INFO) << "twdisplay: Selection Request from 0x" << hex((topaque)Requestor) //
            << ", Owner is 0x" << hex((topaque)Owner);
#endif
  /* cast back Owner from the fake (Tobj) to (uldat) */
  TwRequestSelection((topaque)Owner, ReqPrivate);
}

static byte StdAddMouseEvent(udat Code, dat MouseX, dat MouseY) {
  tevent_mouse Event;
  tmsg msg;

  if (HW->FlagsHW & FlHWNoInput)
    return ttrue;

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

byte MouseEventCommon(dat x, dat y, dat dx, dat dy, udat Buttons) {
  dat prev_x, prev_y;
  udat OldButtons, i;
  mouse_state *OldState;
  udat result;
  byte ret = ttrue;

  OldState = &HW->MouseState;
  OldButtons = OldState->keys;
  prev_x = OldState->x;
  prev_y = OldState->y;

  x = Max2(x, 0);
  x = Min2(x, DisplayWidth - 1);
  OldState->delta_x = x == 0 ? Min2(dx, 0) : x == DisplayWidth - 1 ? Max2(dx, 0) : 0;

  y = Max2(y, 0);
  y = Min2(y, DisplayHeight - 1);
  OldState->delta_y = y == 0 ? Min2(dy, 0) : y == DisplayHeight - 1 ? Max2(dy, 0) : 0;

  if (x != prev_x || y != prev_y)
    HW->FlagsHW |= FlHWChangedMouseFlag;

  OldState->x = x;
  OldState->y = y;

  OldState->keys = Buttons;

  if (Buttons != OldButtons || ((MouseMotionN || OldButtons) && (x != prev_x || y != prev_y))) {

    if ((MouseMotionN || OldButtons) && (x != prev_x || y != prev_y)) {
      ret = StdAddMouseEvent(MOVE_MOUSE | OldButtons, x, y);
    }
    for (i = 0; i < BUTTON_N_MAX && ret; i++) {
      if ((Buttons & HOLD_CODE(i)) != (OldButtons & HOLD_CODE(i))) {
        result = (Buttons & HOLD_CODE(i) ? PRESS_CODE(i) : RELEASE_CODE(i)) |
                 (OldButtons &= ~HOLD_CODE(i));
        OldButtons |= Buttons & HOLD_CODE(i);
        ret = StdAddMouseEvent(result, x, y);
      }
    }
  }
  return ret;
}

byte KeyboardEventCommon(udat Code, udat ShiftFlags, udat Len, const char *Seq) {
  tevent_keyboard Event;
  tmsg msg;

  if (HW->FlagsHW & FlHWNoInput)
    return ttrue;

  if ((msg = TwCreateMsg(TW_MSG_WIDGET_KEY, Len + 1 + sizeof(s_tevent_keyboard)))) {
    Event = &msg->Event.EventKeyboard;

    Event->Code = Code;
    Event->ShiftFlags = ShiftFlags;
    Event->SeqLen = Len;
    CopyMem(Seq, Event->AsciiSeq, Len);
    Event->AsciiSeq[Len] = '\0'; /* terminate string with \0 */

    TwBlindSendMsg(tw_helper, msg);
    return ttrue;
  }
  return tfalse;
}

static void MainLoop(int Fd) {
  struct timeval sel_timeout;
  fd_set read_fds, write_fds, *pwrite_fds;
  uldat err, detail;
  int sys_errno, num_fds = 0;

  for (;;) {
    if (GotSignals)
      HandleSignals();

    if (NeedHW & NEEDResizeDisplay)
      ResizeDisplay();

    if (NeedHW & NEEDSelectionExport)
      SelectionExport();

    /*
     * don't FlushHW() unconditionalluy here,
     * as it gets called syncronously
     * with the main twin from HandleEvent()...
     * just update what is needed.
     */
    if (HW->RedrawVideo)
      FlushHW();
    else {
      HW->UpdateMouseAndCursor();

      if (HW->NeedHW & NEEDFlushHW)
        HW->FlushHW();
      if (NeedHW & NEEDFlushStdout)
        fflush(stdout), NeedHW &= ~NEEDFlushStdout;
    }

    read_fds = save_rfds;

    if (!FdWQueued)
      pwrite_fds = NULL;
    else {
      write_fds = save_wfds;
      pwrite_fds = &write_fds;
    }

    if (!TwFlush())
      break;

    /*
     * messages can arrive during Tw* function calls,
     * so the FD_ISSET() test alone does not suffice.
     * also use an inactivity timeout: 2min...
     */
    sel_timeout.tv_sec = TwPendingMsg() ? 0 : 120;
    sel_timeout.tv_usec = 0;

    if (NeedHW & NEEDPanicHW)
      break;

    num_fds = OverrideSelect(max_fds + 1, &read_fds, pwrite_fds, NULL, &sel_timeout);

    if (num_fds < 0 && errno != EINTR)
      /* ach, problem. */
      break;

    if (num_fds == 0 && !TwPendingMsg()) {
      /*
       * no activity during timeout delay && no pending messages..
       * just ping twin to keep connection active..
       * however, if width <= 0 : means we lost connection, so exit..
       */
      if (TwGetDisplayWidth() <= 0) {
        QuitDisplay(HW);
        log(ERROR) << "twdisplay: lost connection to TWIN.. \n";
        exit(1);
      }
    }

    if ((num_fds > 0 && FD_ISSET(Fd, &read_fds)) || TwPendingMsg()) {
      /*
       * messages can arrive during Tw* function calls,
       * so the FD_ISSET() test alone does not suffice.
       */
      if (FD_ISSET(Fd, &read_fds)) {
        num_fds--;
        FD_CLR(Fd, &read_fds);
      }
      SocketIO();
    }
    if (num_fds > 0)
      RemoteEvent(num_fds, &read_fds);
  }

  if (NeedHW & NEEDPanicHW)
    Quit(0);

  if (num_fds < 0 && errno != EINTR) {
    QuitDisplay(HW);
    log(ERROR) << "twdisplay: select(): " << Chars::from_c(strerror(errno)) << "\n";
    exit(1);
  }
  if (TwInPanic()) {
    err = TwErrno;
    detail = TwErrnoDetail;
    QuitDisplay(HW);
    printk("" SS ": libtw error: " SS "" SS "\n", MYname, TwStrError(err),
           TwStrErrorDetail(err, detail));
    exit(1);
  }
  err = TwErrno;
  detail = TwErrnoDetail;
  sys_errno = errno;
  QuitDisplay(HW);
  printk("" SS ": shouldn't happen! Please report:\n"
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
        "\tX[@<XDISPLAY>]\n"
        "\ttwin[@<TWDISPLAY>]\n"
        "\ttty[@<tty device>]\n",
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
  char *arg_hw = NULL;
  int Fd;
  byte flags = TW_ATTACH_HW_REDIRECT;
  byte ret = 0, ourtty = 0;
  bool force = false;

  MergeHyphensArgv(argc, argv);

  MYname = argv[0];

  while (*++argv) {
    char *argi = *argv;
    if (!strcmp(argi, "-V") || !strcmp(argi, "-version")) {
      ShowVersion();
      return 0;
    } else if (!strcmp(argi, "-h") || !strcmp(argi, "-help")) {
      Usage();
      return 0;
    } else if (!strcmp(argi, "-x") || !strcmp(argi, "-excl"))
      flags |= TW_ATTACH_HW_EXCLUSIVE;
    else if (!strcmp(argi, "-s") || !strcmp(argi, "-share"))
      flags &= ~TW_ATTACH_HW_EXCLUSIVE;
    else if (!strcmp(argi, "-v") || !strcmp(argi, "-verbose"))
      flags |= TW_ATTACH_HW_REDIRECT;
    else if (!strcmp(argi, "-q") || !strcmp(argi, "-quiet"))
      flags &= ~TW_ATTACH_HW_REDIRECT;
    else if (!strcmp(argi, "-f") || !strcmp(argi, "-force"))
      force = true;
    else if (!strncmp(argi, "-plugindir=", 11)) {
      plugindir = Chars::from_c(argi + 11);
    } else if (!strncmp(argi, "-twin@", 6))
      dpy = argi + 6;
    else if (!strncmp(argi, "-hw=", 4)) {
      if (!strncmp(argi + 4, "display", 7)) {
        printk("" SS ": argument `--hw=display' is for internal use only.\n", MYname);
        TryUsage(NULL);
        return 1;
      }
      if (!strncmp(argi + 4, "tty", 3)) {
        const char *cs = "";
        char *opt = argi + 7;
        char *s = strchr(opt, ',');
        if (s)
          *s = '\0';

        if (!*opt)
          /* attach twin to our tty */
          ourtty = 1;
        else if (opt[0] == '@' && opt[1]) {
          if (opt[1] == '-') {
            /*
             * using server controlling tty makes no sense for twdisplay
             */
            printk("" SS ": `" SS "' makes sense only with twattach.\n", MYname, argi);
            return 1;
          } else if (tty) {
            if (!strcmp(opt + 1, tty))
              /* attach twin to our tty */
              ourtty = 1;
          } else {
            printk("" SS ": ttyname() failed: cannot find controlling tty!\n", MYname);
            return 1;
          }
        } else {
          printk("" SS ": malformed display hw `" SS "'\n", MYname, argi);
          return 1;
        }

        if (s) {
          *s = ',';
          cs = s;
        }

        if (ourtty) {
          const char *term = getenv("TERM");
          if (term && !*term)
            term = NULL;

          arg_hw = (char *)malloc(strlen(tty) + 9 + strlen(cs) + (term ? 6 + strlen(term) : 0));

          sprintf(arg_hw, "-hw=tty%s%s%s", (term ? ",TERM=" : term), term, cs);
        } else
          arg_hw = argi;
      } else if ((argi)[4]) {
        arg_hw = argi;
        if (!strncmp(argi + 4, "twin@", 5)) {
          client_dpy = argi + 9;
        } else if (!strcmp(argi + 4, "twin")) {
          client_dpy = "";
        }
      } else {
        TryUsage(argi);
        return 1;
      }
    } else {
      TryUsage(argi);
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
  TWDisplay = dpy ? CloneStr(dpy) : origTWDisplay;
  origTERM = CloneStr(getenv("TERM"));
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
      printk("twdisplay: refusing to display twin inside itself. Use option `-f' to override.\n");
      TwClose();
      return 1;
    }
  }

  InitSignals();
  InitTtysave();

#ifdef CONF__ALLOC
  TwConfigMalloc(AllocMem, ReAllocMem, free);
#endif

  if (TwCheckMagic(display_magic) && TwOpen(TWDisplay))
    do {
      char *buf;

      if (!VersionsMatch(force)) {
        if (!force) {
          printk("twdisplay: Aborting. Use option `-f' to ignore versions check.\n");
          TwClose();
          return 1;
        }
      }

      if (RegisterRemote(Fd = TwConnectionFd(), NULL, (handler_io_d)NoOp) == NOSLOT) {
        TwClose();
        OutOfMemory();
        return 1;
      }

      if (!(TMsgPort = TwCreateMsgPort(9, "twdisplay")))
        break;

      DisplayWidth = TryDisplayWidth = TwGetDisplayWidth();
      DisplayHeight = TryDisplayHeight = TwGetDisplayHeight();

      /* if user did not specify any `--hw=<dpy>', autoprobe */
      if (!(HW = AttachDisplayHW(Chars::from_c(arg_hw), NOSLOT, 0))) {
        TwClose();
        return 1;
      }
      if (!(buf = (char *)AllocMem(HW->Name.size() + 80))) {
        QuitDisplay(HW);
        TwClose();
        OutOfMemory();
        return 1;
      }

      sprintf(buf, "-hw=display@(%.*s),x=%d,y=%d%s%s%s", (int)HW->Name.size(), HW->Name.data(),
              (int)HW->X, (int)HW->Y, HW->CanResize ? ",resize" : "",
              /* CanDragArea */ ttrue ? ",drag" : "", ExpensiveFlushVideo ? ",slow" : "");

      TwAttachHW(strlen(buf), buf, flags);
      TwFlush();

      flags &= TW_ATTACH_HW_REDIRECT;

      if (flags)
        printk("messages reported by twin server...\n");

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
        MainLoop(Fd);
      else if (ret)
        log(WARNING) << Chars::from_c(MYname) << ": twin said we can quit... strange!\n";

      Quit(!ret);
    } while (0);

  log(ERROR) << Chars::from_c(MYname) << ": libtw error: " << Chars::from_c(TwStrError(TwErrno))
             << " " << Chars::from_c(TwStrErrorDetail(TwErrno, TwErrnoDetail)) << "\n";
  return 1;
}

void Quit(int status) {
  QuitDisplay(HW);
  if (status < 0)
    return; /* give control back to signal handler */
  exit(status);
}
