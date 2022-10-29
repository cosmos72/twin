/*
 *  hw_multi.c  --  functions to use multiple
 *                  simultaneous HW/hw_* displays
 *
 *  Copyright (C) 2000-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <Tw/autoconf.h>

#include <stdio.h>

#ifdef TW_HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef TW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include "twin.h"
#include "alloc.h"
#include "algo.h"
#include "common.h"
#include "data.h"
#include "main.h"
#include "extreg.h"
#include "methods.h"
#include "draw.h"
#include "remote.h"

#include "dl.h"
#include "hw.h"
#include "hw_private.h"
#include "hw_multi.h"
#include "log.h"
#include "resize.h"
#include "stl/string.h"
#include "util.h"

#include <Tw/Tw.h>

#define forHW for (HW = All->FirstDisplayHW; HW; HW = HW->Next)

#define safeforHW(s_HW)                                                                            \
  for (HW = All->FirstDisplayHW; HW && (((s_HW) = HW->Next), ttrue); HW = (s_HW))

/* common data */

static dat (*saveChangedVideo)[2][2];

static dat savedDisplayWidth = 100, savedDisplayHeight = 30;
static dat TryDisplayWidth, TryDisplayHeight;

static dat AccelVideo[4] = {TW_MAXDAT, TW_MAXDAT, TW_MINDAT, TW_MINDAT};
byte StrategyFlag;
tany StrategyDelay = (tany)0;

static udat ConfigureHWValue[HW_CONFIGURE_MAX];
static byte ConfigureHWDefault[HW_CONFIGURE_MAX];

/* common functions */

dat GetDisplayWidth(void) NOTHROW {
  return All->FirstDisplayHW && !All->FirstDisplayHW->Quitted ? DisplayWidth : savedDisplayWidth;
}

dat GetDisplayHeight(void) NOTHROW {
  return All->FirstDisplayHW && !All->FirstDisplayHW->Quitted ? DisplayHeight : savedDisplayHeight;
}

void UpdateFlagsHW(void) NOTHROW {
  StrategyReset(); /* reset StrategyFlag */

  NeedOldVideo = ExpensiveFlushVideo = tfalse;
  CanDragArea = ttrue;

  forHW {
    if (!HW->Quitted) {
      NeedOldVideo |= HW->FlagsHW & FlHWNeedOldVideo;
      ExpensiveFlushVideo |= HW->FlagsHW & FlHWExpensiveFlushVideo;
      CanDragArea &= !!HW->CanDragArea;
    }
  }
}

void RunNoHW(byte print_info) {
  pid_t child;

  if (DisplayWidth && DisplayHeight) {
    savedDisplayWidth = DisplayWidth;
    savedDisplayHeight = DisplayHeight;
  }
  ResizeDisplay();

  if (!DisplayHWCTTY) {
    /*
     * we fork in the background
     * so that the shell we were started from
     * can realize we have finished with the tty
     */
    switch ((child = fork())) {
    case 0: /* child: continue */
      DisplayHWCTTY = HWCTTY_DETACHED;
      setsid();
      break;
    case -1:
      break;
    default: /* parent: exit */
      if (print_info) {
        struct stat s1, s2;

        /* if stderr != stdout, also print on stdout (exploited by twdm) */
        if (fstat(1, &s1) != fstat(2, &s2) || s1.st_ino != s2.st_ino)
          printf("twin: starting in background as %s (pid %d)\n", TWDisplay, (unsigned)child);

        fprintf(stderr, "twin: starting in background as %s (pid %d)\n", TWDisplay,
                (unsigned)child);
      }
      exit(0);
    }
  }

  /* try to fire up the Socket Server ... */
  (void)DlLoad(SocketSo);
}

static bool module_InitHW(Chars arg) {

  if (!arg) {
    return tfalse;
  } else if (arg.starts_with(Chars("-hw="))) {
    arg = arg.view(4, arg.size()); // skip "-hw="
  }

  const size_t at = arg.find(Chars("@"));
  const size_t comma = arg.find(Chars(","));
  const size_t separator = at < comma ? at : comma;

  Chars name = arg.view(0, separator != size_t(-1) ? separator : arg.size());
  if (name == Chars("X")) {
    name = Chars("X11");
  }

  String alloc_name;
  Tmodule Module = NULL;

  if (alloc_name.format("hw_", name)) {
    name = alloc_name;
    Module = DlLoadAny(name.size(), name.data());

    if (Module) {
      log(INFO) << "twin: starting display driver module `" << name << "'...\n";
      bool (*InitD)(void);
      if ((InitD = Module->DoInit) && InitD()) {
        log(INFO) << "twin: ...module `" << name << "' successfully started.\n";
        HW->Module = Module;
        Module->Used++;
        return true;
      }
      Module->Delete();
    }
  }

  if (Module) {
    log(ERROR) << "twin: ...module `" << name << "' failed to start.\n";
  } else {
    log(ERROR) << "twin: unable to load display driver module `" << name //
               << "' :\n      " << Errstr << "\n";
  }
  return false;
}

static byte set_hw_name(Tdisplay D_HW, const Chars name) {
  String alloc_name;
  if (D_HW && alloc_name.format(name)) {
    D_HW->Name.swap(alloc_name);
  }
  return ttrue;
}

static void warn_NoHW(const char *arg, uldat len) {
  log(ERROR) << "twin: all display drivers failed";
  if (arg)
    log(ERROR) << " for `" << Chars(arg, len) << "'\n";
  else
    log(ERROR) << ".\n";
}

/*
 * InitDisplayHW runs HW specific InitXXX() functions, starting from best setup
 * and falling back in case some of them fails.
 */
byte InitDisplayHW(Tdisplay D_HW) {
  Chars arg = D_HW->Name;
  byte success;

  SaveHW;
  SetHW(D_HW);

  D_HW->DisplayIsCTTY = D_HW->NeedHW = D_HW->FlagsHW = tfalse;

#define AUTOTRY4(hw_name) (module_InitHW(Chars(hw_name)) && set_hw_name(D_HW, Chars(hw_name)))

  if (!arg) {
    success =
        AUTOTRY4("-hw=xft") || AUTOTRY4("-hw=X11") || AUTOTRY4("-hw=twin") || AUTOTRY4("-hw=tty");
  } else {
    success = module_InitHW(arg);
  }

#undef AUTOTRY4

  if (success) {
    udat tried;

    D_HW->Quitted = tfalse;

    /* configure correctly the new HW */
    for (tried = 0; tried < HW_CONFIGURE_MAX; tried++) {
      if (!(ConfigureHWDefault[tried]))
        D_HW->Configure(tried, tfalse, ConfigureHWValue[tried]);
    }

    if (!DisplayHWCTTY && D_HW->DisplayIsCTTY)
      DisplayHWCTTY = D_HW;
    if (All->FnHookDisplayHW)
      All->FnHookDisplayHW(All->HookDisplayHW);
    UpdateFlagsHW(); /* this garbles HW... not a problem here */
  } else {
    warn_NoHW(arg.data(), arg.size());
  }

  RestoreHW;

  return success;
}

void QuitDisplayHW(Tdisplay D_HW) {
  Tmsgport MsgPort;
  uldat slot;
  SaveHW;

  if (D_HW) {
    if (D_HW->QuitHW)
      HW = D_HW, D_HW->QuitHW();

    D_HW->Quitted = ttrue;

    if ((slot = D_HW->AttachSlot) != NOSLOT) {
      /* avoid KillSlot <-> DeleteDisplayHW infinite recursion */
      if ((MsgPort = RemoteGetMsgPort(slot)))
        MsgPort->AttachHW = (Tdisplay)0;
      Ext(Remote, KillSlot)(slot);
    }

    if (D_HW->Module) {
      D_HW->Module->Used--;
      D_HW->Module->Delete();
      D_HW->Module = (Tmodule)0;
    }
    UpdateFlagsHW(); /* this garbles HW... not a problem here */
  }
  RestoreHW;
}

static byte IsValidHW(Chars carg) {
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
      log(ERROR) << "twin: invalid non-alphanumeric character 0x" << hex(b)
                 << " in display HW name `" << Chars(arg, len) << "'\n";
      return tfalse;
    }
  }
  return ttrue;
}

Tdisplay AttachDisplayHW(Chars arg, uldat slot, byte flags) {
  Tdisplay D_HW = NULL;

  if (arg && !arg.starts_with(Chars("-hw="))) {
    log(ERROR) << "twin: specified `" << arg
               << "' is not a known option.\n"
                  "      try `twin --help' for usage summary.\n";
    return D_HW;
  }

  if (All->ExclusiveHW) {
    log(ERROR) << "twin: exclusive display in use, permission to display denied!\n";
    return D_HW;
  }

  if (IsValidHW(arg) && (D_HW = New(display)(arg.size(), arg.data()))) {
    D_HW->AttachSlot = slot;
    if (D_HW->DoInit()) {

      if (flags & TW_ATTACH_HW_EXCLUSIVE) {
        /* started exclusive display, kill all others */
        Tdisplay s_HW, n_HW;

        All->ExclusiveHW = D_HW;

        for (s_HW = All->FirstDisplayHW; s_HW; s_HW = n_HW) {
          n_HW = s_HW->Next;
          if (s_HW != D_HW) {
            s_HW->Delete();
          }
        }
      }

      if (ResizeDisplay()) {
        QueuedDrawArea2FullScreen = ttrue;
      }
      return D_HW;
    }
    /* failed, clean up without calling RunNoHW() or KillSlot() */
    D_HW->Quitted = ttrue;
    D_HW->AttachSlot = NOSLOT;
    D_HW->QuitHW = NoOp;
    D_HW->Delete();
    D_HW = NULL;
  }
  return D_HW;
}

bool DetachDisplayHW(Chars arg, byte flags) {
  if (All->ExclusiveHW && !(flags & TW_ATTACH_HW_EXCLUSIVE))
    return false;

  Tdisplay s_HW;
  bool done = false;
  if (arg) {
    safeforHW(s_HW) {
      if (HW->Name == arg) {
        HW->Delete();
        done = true;
        break;
      }
    }
  } else {
    QuitHW();
    done = true;
  }
  return done;
}

/* initialize all required HW displays. Since we are at it, also parse command line */
byte InitHW(void) {
  char **arglist = orig_argv;
  char *arg;
  udat hwcount = 0;

  byte ret = tfalse, flags = 0, nohw = tfalse;

  memset(ConfigureHWDefault, '\1', HW_CONFIGURE_MAX); /* set everything to default (-1) */

  for (arglist = orig_argv; (arg = *arglist); arglist++) {
    if (!strcmp(arg, "-nohw"))
      nohw = ttrue;
    else if (!strcmp(arg, "-x") || !strcmp(arg, "-excl"))
      flags |= TW_ATTACH_HW_EXCLUSIVE;
    else if (!strcmp(arg, "-s") || !strcmp(arg, "-share"))
      flags &= ~TW_ATTACH_HW_EXCLUSIVE;
    else if (!strcmp(arg, "-secure"))
      flag_secure = ttrue;
    else if (!strcmp(arg, "-envrc"))
      flag_envrc = ttrue;
    else if (!strncmp(arg, "-hw=", 4))
      hwcount++;
    else if (!strncmp(arg, "-plugindir=", 11))
      ; // already processed, ignore option
    else
      log(WARNING) << "twin: ignoring unknown option `" << Chars::from_c(arg) << "'\n";
  }

  if (nohw && hwcount > 0) {
    log(ERROR) << "twin: `--hw=' and `--nohw' options cannot be used together.\n";
    return ret;
  }
  if (flags & TW_ATTACH_HW_EXCLUSIVE) {
    if (nohw) {
      log(ERROR) << "twin: `--excl' cannot be used with `--nohw'.\n";
      return ret;
    } else if (hwcount > 1) {
      log(ERROR) << "twin: `--excl' cannot be used with multiple `--hw'.\n";
      return ret;
    }
  }

  /*
   * execute ~/.config/twin/twenvrc.sh if needed and read its output to set
   * environment variables (mostly useful for twdm)
   */
  RunTwEnvRC();

  if (nohw)
    RunNoHW(ret = ttrue);
  else if (hwcount) {
    for (arglist = orig_argv; (arg = *arglist); arglist++) {
      if (!strncmp(arg, "-hw=", 4)) {
        ret |= !!AttachDisplayHW(Chars::from_c(arg), NOSLOT, flags);
      }
    }
  }
  if (hwcount == 0 && !ret)
    /* autoprobe */
    ret = !!AttachDisplayHW(Chars(), NOSLOT, flags);

  if (!ret)
    log(ERROR) << "\ntwin:  \033[1mALL  DISPLAY  DRIVERS  FAILED.  QUITTING.\033[0m\n";

  return ret;
}

void QuitHW(void) {
  DeleteList(All->FirstDisplayHW);
}

byte RestartHW(byte verbose) {
  Tdisplay s_HW;
  byte ret = tfalse;

  if (All->FirstDisplayHW) {
    safeforHW(s_HW) {
      if (HW->DoInit())
        ret = ttrue;
      else
        HW->Delete();
    }
    if (ret) {
      ResizeDisplay();
      QueuedDrawArea2FullScreen = ttrue;
    } else {
      log(WARNING) << "\ntwin:   \033[1mALL  DISPLAY  DRIVERS  FAILED.\033[0m\n"
                      "\ntwin: continuing in background with no display.\n";
      RunNoHW(tfalse);
    }
  } else if (verbose) {
    log(INFO) << "twin: RestartHW(): All display drivers removed by SuspendHW().\n"
                 "      No display available for restarting, use twattach or twdisplay.\n";
  }
  return ret;
}

void SuspendHW(byte verbose) {
  Tdisplay s_HW;
  safeforHW(s_HW) {
    if (HW->AttachSlot != NOSLOT && HW->NeedHW & NEEDPersistentSlot)
      /* we will not be able to restart it */
      HW->Delete();
    else
      HW->DoQuit();
  }
  if (verbose && !All->FirstDisplayHW) {
    log(INFO) << "twin: SuspendHW(): All display drivers had to be removed\n"
                 "      since they were attached to clients (twattach/twdisplay).\n"
                 "twin: --- STOPPED ---\n";
  }
}

void PanicHW(void) {
  Tdisplay s_HW;

  if (NeedHW & NEEDPanicHW) {
    safeforHW(s_HW) {
      if (HW->NeedHW & NEEDPanicHW)
        HW->Delete();
    }
    NeedHW &= ~NEEDPanicHW;
  }
}

void ResizeDisplayPrefer(Tdisplay D_HW) {
  SaveHW;
  SetHW(D_HW);
  D_HW->DetectSize(&TryDisplayWidth, &TryDisplayHeight);
  NeedHW |= NEEDResizeDisplay;
  RestoreHW;
}

/*
 * return ttrue if DisplayWidth or DisplayHeight were changed
 */
byte ResizeDisplay(void) {
  dat Width, Height;
  byte change = tfalse;

  if (All->FirstDisplayHW) {

    if (!TryDisplayWidth || !TryDisplayHeight) {
      /*
       * we are trying to come up with a fair display size
       * and have all HW agree on it.
       */
      TryDisplayWidth = TryDisplayHeight = TW_MAXDAT;
      forHW {
        if (!HW->Quitted) {
          HW->DetectSize(&Width, &Height);
          if (TryDisplayWidth > Width)
            TryDisplayWidth = Width;
          if (TryDisplayHeight > Height)
            TryDisplayHeight = Height;
        }
      }
    }

    /* now let's check how much we can resize all HW displays... */
    do {
      Width = TryDisplayWidth;
      Height = TryDisplayHeight;
      forHW {
        if (!HW->Quitted)
          HW->CheckResize(&TryDisplayWidth, &TryDisplayHeight);
      }
    } while (TryDisplayWidth < Width || TryDisplayHeight < Height);

    if (!TryDisplayWidth || TryDisplayWidth == TW_MAXDAT)
      TryDisplayWidth = DisplayWidth;
    if (!TryDisplayHeight || TryDisplayHeight == TW_MAXDAT)
      TryDisplayHeight = DisplayHeight;

    /* size seems reasonable, apply it to all HW displays */
    forHW {
      if (!HW->Quitted)
        HW->Resize(TryDisplayWidth, TryDisplayHeight);
    }
  } else
    TryDisplayWidth = TryDisplayHeight = 1;

  change = DisplayWidth != TryDisplayWidth || DisplayHeight != TryDisplayHeight;

  if (!NeedOldVideo && OldVideo) {
    FreeMem(OldVideo);
    OldVideo = NULL;
  } else if ((NeedOldVideo && !OldVideo) || change) {
    if (!(OldVideo = (tcell *)ReAllocMem(OldVideo, (ldat)TryDisplayWidth * TryDisplayHeight *
                                                       sizeof(tcell)))) {
      log(ERROR) << "twin: out of memory!\n";
      Quit(1);
    }
    ValidOldVideo = tfalse;
  }

  if (!Video || change) {
    All->DisplayWidth = DisplayWidth = TryDisplayWidth;
    All->DisplayHeight = DisplayHeight = TryDisplayHeight;

    if (!(Video = (tcell *)ReAllocMem(Video, (ldat)DisplayWidth * DisplayHeight * sizeof(tcell))) ||
        !(ChangedVideo =
              (dat(*)[2][2])ReAllocMem(ChangedVideo, (ldat)DisplayHeight * sizeof(dat) * 4)) ||
        !(saveChangedVideo =
              (dat(*)[2][2])ReAllocMem(saveChangedVideo, (ldat)DisplayHeight * sizeof(dat) * 4))) {

      log(ERROR) << "twin: out of memory!\n";
      Quit(1);
    }
    memset(ChangedVideo, 0xff, (ldat)DisplayHeight * sizeof(dat) * 4);
  }
  NeedHW &= ~NEEDResizeDisplay;

  TryDisplayWidth = TryDisplayHeight = 0;

  return change;
}

void BeepHW(void) {
  NeedHW |= NEEDBeepHW;
}

/* configure underlying HW */
void ConfigureHW(udat resource, byte todefault, udat value) {

  if (!(ConfigureHWDefault[resource] = todefault))
    ConfigureHWValue[resource] = value;

  forHW {
    HW->Configure(resource, todefault, value);
  }
}

void SetPaletteHW(udat N, udat R, udat G, udat B) {
  if (N <= tmaxcol) {
    rgb c;
    c.Red = R;
    c.Green = G;
    c.Blue = B;

    if (memcmp(&Palette[N], &c, sizeof(rgb))) {
      Palette[N] = c;
      forHW {
        HW->SetPalette(N, R, G, B);
      }
    }
  }
}

void ResetPaletteHW(void) {
  forHW {
    HW->ResetPalette();
  }
}

/* HW back-end function: get selection owner */
Tobj TwinSelectionGetOwner(void) {
  /*
   * this looks odd... but it's correct:
   * only libtw clients can persistently own the Selection;
   * a display HW can own the Selection only if you did a mouse click
   * on its display, and even in this case it will own the Selection
   * only for a single request.
   */
  Tobj Owner = (Tobj)All->Selection->OwnerOnce;
  if (Owner)
    All->Selection->OwnerOnce = NULL;
  else
    Owner = (Tobj)All->Selection->Owner;
  return Owner;
}

static void SelectionClear(Tmsgport Owner) {
  Tmsg Msg;

  if ((Msg = New(msg)(msg_selection_clear, 0)))
    SendMsg(Owner, Msg);
}

/* HW back-end function: set selection owner */
void TwinSelectionSetOwner(Tobj Owner, tany Time, tany Frac) {
  timevalue T;
  if (Time == SEL_CURRENTTIME)
    CopyMem(&All->Now, &T, sizeof(timevalue));
  else {
    T.Seconds = Time;
    T.Fraction = Frac;
  }

  if (CmpTime(&T, &All->Selection->Time) > 0) {
    if (!Owner || Owner->Id >> magic_shift == msgport_magic >> magic_shift) {
      if (All->Selection->Owner)
        SelectionClear(All->Selection->Owner);

      NeedHW |= NEEDSelectionExport;

      All->Selection->Owner = (Tmsgport)Owner;
      All->Selection->OwnerOnce = NULL;
      CopyMem(&T, &All->Selection->Time, sizeof(timevalue));
    } else if (Owner->Id >> magic_shift == display_hw_magic >> magic_shift) {
      /* don't NEEDSelectionExport here! */
      All->Selection->OwnerOnce = (Tdisplay)0;
    }
  }
}

void TwinSelectionNotify(Tobj Requestor, uldat ReqPrivate, e_id Magic, const char MIME[MAX_MIMELEN],
                         Chars Data) {
  Tmsg NewMsg;
  event_any *Event;
#if 0
  log(INFO) << "twin: Selection Notify to 0x" << hex(Requestor ? Requestor->Id : NOID) << "\n";
#endif
  if (!Requestor) {
    (void)SelectionStore(Magic, MIME, Data);
  } else if (Requestor->Id >> magic_shift == msgport_magic >> magic_shift) {

    const size_t len = Data.size();

    if ((NewMsg = New(msg)(msg_selection_notify, len))) {
      Event = &NewMsg->Event;
      Event->EventSelectionNotify.W = NULL;
      Event->EventSelectionNotify.Code = 0;
      Event->EventSelectionNotify.pad = 0;
      Event->EventSelectionNotify.ReqPrivate = ReqPrivate;
      Event->EventSelectionNotify.Magic = Magic;
      if (MIME)
        CopyMem(MIME, Event->EventSelectionNotify.MIME, MAX_MIMELEN);
      else
        memset(Event->EventSelectionNotify.MIME, '\0', MAX_MIMELEN);
      Event->EventSelectionNotify.Len = len;
      CopyMem(Data.data(), Event->EventSelectionNotify.Data, len);
      SendMsg((Tmsgport)Requestor, NewMsg);
    }
  } else if (Requestor->Id >> magic_shift == display_hw_magic >> magic_shift) {
    SaveHW;
    SetHW((Tdisplay)Requestor);
    HW->HWSelectionNotify(ReqPrivate, Magic, MIME, Data);
    RestoreHW;
  }
}

void TwinSelectionRequest(Tobj Requestor, uldat ReqPrivate, Tobj Owner) {
#if 0
  log(INFO) << "twin: Selection Request from 0x" << (Requestor ? Requestor->Id : NOID)
            << ", owner is 0x" << (Owner ? Owner->Id : NOID) << "\n";
#endif
  if (Owner) {
    if (Owner->Id >> magic_shift == msgport_magic >> magic_shift) {
      Tmsg NewMsg;
      event_any *Event;
      if ((NewMsg = New(msg)(msg_selection_request, 0))) {

        Event = &NewMsg->Event;
        Event->EventSelectionRequest.W = NULL;
        Event->EventSelectionRequest.Code = 0;
        Event->EventSelectionRequest.pad = 0;
        Event->EventSelectionRequest.Requestor = Requestor;
        Event->EventSelectionRequest.ReqPrivate = ReqPrivate;
        SendMsg((Tmsgport)Owner, NewMsg);
      }
    } else if (Owner->Id >> magic_shift == display_hw_magic >> magic_shift) {
      SaveHW;
      SetHW((Tdisplay)Owner);
      HW->HWSelectionRequest(Requestor, ReqPrivate);
      RestoreHW;
    }
  } else {
    selection Sel = All->Selection;
    TwinSelectionNotify(Requestor, ReqPrivate, e_id(Sel->Magic), Sel->MIME, Sel->Data);
  }
}

void SelectionExport(void) {
  forHW {
    HW->HWSelectionExport();
  }
  NeedHW &= ~NEEDSelectionExport;
}

void SelectionImport(void) {
  if ((HW = All->MouseHW)) {
    if (HW->HWSelectionImport())
      All->Selection->OwnerOnce = HW;
    else
      All->Selection->OwnerOnce = NULL;
  }
}

inline void DiscardBlinkVideo(void) {
  ldat i;
  uldat start, len;
  tcell *V;

  for (i = 0; i < (ldat)DisplayHeight * 2; i++) {
    start = (uldat)ChangedVideo[i >> 1][i & 1][0];

    if (start != (uldat)-1) {
      len = (uldat)ChangedVideo[i >> 1][i & 1][1] + 1 - start;
      start += (i >> 1) * (ldat)DisplayWidth;

      for (V = &Video[start]; len; V++, len--)
        *V &= ~TCELL(TCOL(0, thigh), (byte)0);
    }
  }
}

inline void OptimizeChangedVideo(void) {
  uldat _start, start, _end, end;
  ldat i;

  ChangedVideoFlag = tfalse;

  for (i = 0; i < (ldat)DisplayHeight * 2; i++) {
    start = (uldat)ChangedVideo[i >> 1][!(i & 1)][0];

    if (start != (uldat)-1) {

      start += (i >> 1) * (ldat)DisplayWidth;
      _start = start;

      _end = end = (uldat)ChangedVideo[i >> 1][!(i & 1)][1] + (i >> 1) * (ldat)DisplayWidth;

      while (start <= end && Video[start] == OldVideo[start])
        start++;
      while (start <= end && Video[end] == OldVideo[end])
        end--;

      if (start > end) {
        if (i & 1) {
          /*
           * this is the first area, to make it empty
           * copy the second on this.
           */
          if (ChangedVideo[i >> 1][1][0] != -1) {
            ChangedVideo[i >> 1][0][0] = ChangedVideo[i >> 1][1][0];
            ChangedVideo[i >> 1][0][1] = ChangedVideo[i >> 1][1][1];
            ChangedVideo[i >> 1][1][0] = -1;
          } else
            ChangedVideo[i >> 1][0][0] = -1;
        } else
          ChangedVideo[i >> 1][1][0] = -1;
        continue;
      } else {
        ChangedVideoFlag = ttrue;
        if (start > _start)
          ChangedVideo[i >> 1][!(i & 1)][0] += start - _start;
        if (end < _end)
          ChangedVideo[i >> 1][!(i & 1)][1] -= _end - end;
      }
    }
  }
}

inline void SyncOldVideo(void) {
  ldat start, len;
  ldat i;

  for (i = 0; i < (ldat)DisplayHeight * 2; i++) {
    start = ChangedVideo[i >> 1][i & 1][0];

    if (start != -1) {
      len = ChangedVideo[i >> 1][i & 1][1] + 1 - start;
      start += (i >> 1) * (ldat)DisplayWidth;

      ChangedVideo[i >> 1][i & 1][0] = -1;

      CopyMem(Video + start, OldVideo + start, len * sizeof(tcell));
    }
  }
}

#define MaxRecentBeepHW ((byte)30)

void FlushHW(void) {
  static timevalue LastBeep = {(tany)0, (tany)0};
  timevalue tmp = {(tany)0, 100 MilliSECs};
  byte doBeep = tfalse, saved = tfalse, mangled = tfalse;
  byte saveChangedVideoFlag, saveValidOldVideo;
  /*
   * we can NEVER get (saved == tfalse && mangled == ttrue)
   * as it would mean we have irreversibly lost ChangedVideo[]
   */

  /*
   * displaying on ourselves can cause infine beeping loops...
   * avoid it
   */
  if (NeedHW & NEEDBeepHW) {
    IncrTime(&tmp, &LastBeep);
    if (CmpTime(&All->Now, &tmp) >= 0) {
      CopyMem(&All->Now, &LastBeep, sizeof(timevalue));
      doBeep = ttrue;
    }
    NeedHW &= ~NEEDBeepHW;
  }

  if (QueuedDrawArea2FullScreen) {
    QueuedDrawArea2FullScreen = tfalse;
    DirtyVideo(0, 0, DisplayWidth - 1, DisplayHeight - 1);
    DrawArea2(FULL_SCREEN);
    UpdateCursor();
  }

  if (NeedUpdateCursor)
    FlushCursor();

  if (!(All->SetUp->Flags & setup_blink))
    DiscardBlinkVideo();

  if (NeedOldVideo && ValidOldVideo)
    OptimizeChangedVideo();

  forHW {
    /*
     * adjust ChangedVideoFlag and ChangedVideo[]
     * to include HW supplied area HW->Redraw*
     *
     * we do not save/restore ChangedVideoFlagAgain since it's used
     * to decide buffering strategies, so must reflect whether
     * some display had to draw something or not.
     */
    if (mangled) {
      ValidOldVideo = saveValidOldVideo;
      ChangedVideoFlag = saveChangedVideoFlag;
      CopyMem(saveChangedVideo, ChangedVideo, (ldat)DisplayHeight * sizeof(dat) * 4);
      mangled = tfalse;
    }
    if (HW->RedrawVideo ||
        ((HW->FlagsHW & FlHWSoftMouse) && (HW->FlagsHW & FlHWChangedMouseFlag))) {
      if (!saved) {
        saveValidOldVideo = ValidOldVideo;
        saveChangedVideoFlag = ChangedVideoFlag;
        CopyMem(ChangedVideo, saveChangedVideo, (ldat)DisplayHeight * sizeof(dat) * 4);
        saved = ttrue;
      }
      if (HW->RedrawVideo) {
        DirtyVideo(HW->RedrawLeft, HW->RedrawUp, HW->RedrawRight, HW->RedrawDown);
        ValidOldVideo = tfalse;
        /* the OldVideo[] caching would make all this stuff useless otherwise */
      }
      mangled = ttrue;
    }
    if (doBeep)
      HW->Beep();

    HW->FlushVideo();

    HW->RedrawVideo = tfalse;

    if (HW->NeedHW & NEEDFlushHW)
      HW->FlushHW();
  }
  if (NeedHW & NEEDFlushStdout)
    fflush(stdout), NeedHW &= ~NEEDFlushStdout;

  if (ChangedVideoFlag && NeedOldVideo)
    SyncOldVideo();

  ChangedVideoFlag = tfalse;
  ValidOldVideo = ttrue;
}

void SyntheticKey(Twidget W, udat Code, udat ShiftFlags, byte Len, const char *Seq) {
  event_keyboard *Event;
  Tmsg Msg;

  if (W && Len && Seq && (Msg = New(msg)(msg_widget_key, Len))) {

    Event = &Msg->Event.EventKeyboard;
    Event->W = W;
    Event->Code = Code;
    Event->ShiftFlags = ShiftFlags;
    Event->SeqLen = Len;
    CopyMem(Seq, Event->AsciiSeq, Len);
    Event->AsciiSeq[Len] = '\0'; /* terminate string with \0 */
    SendMsg(W->Owner, Msg);
  }
}

void FillVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, tcell Attr) {
  tcell *pos;
  udat _xc, xc, yc, delta;

  if (Xstart > Xend || Xstart >= DisplayWidth || Xend < 0 || Ystart > Yend ||
      Ystart >= DisplayHeight || Yend < 0)
    return;
  Xstart = Max2(Xstart, 0);
  Ystart = Max2(Ystart, 0);
  Xend = Min2(Xend, DisplayWidth - 1);
  Yend = Min2(Yend, DisplayHeight - 1);
  DirtyVideo(Xstart, Ystart, Xend, Yend);

  yc = Yend - Ystart + 1;
  _xc = Xend - Xstart + 1;
  delta = DisplayWidth - _xc;
  pos = Video + Xstart + Ystart * (ldat)DisplayWidth;

  while (yc--) {
    xc = _xc;
    while (xc--)
      *pos++ = Attr;
    pos += delta;
  }
}

void FillOldVideo(dat Xstart, dat Ystart, dat Xend, dat Yend, tcell Attr) {
  tcell *pos;
  udat _xc, xc, yc, delta;

  if (Xstart > Xend || Xstart >= DisplayWidth || Xend < 0 || Ystart > Yend ||
      Ystart >= DisplayHeight || Yend < 0)
    return;
  Xstart = Max2(Xstart, 0);
  Ystart = Max2(Ystart, 0);
  Xend = Min2(Xend, DisplayWidth - 1);
  Yend = Min2(Yend, DisplayHeight - 1);

  yc = Yend - Ystart + 1;
  _xc = Xend - Xstart + 1;
  delta = DisplayWidth - _xc;
  pos = OldVideo + Xstart + Ystart * (ldat)DisplayWidth;

  while (yc--) {
    xc = _xc;
    while (xc--)
      *pos++ = Attr;
    pos += delta;
  }
}

void RefreshVideo(void) {
  ValidOldVideo = tfalse;
  QueuedDrawArea2FullScreen = ttrue;
  /* safer than DirtyVideo(0, 0, DisplayWidth - 1, DisplayHeight - 1),
   * and also updates the cursor */
}

inline uldat Plain_countDirtyVideo(dat X1, dat Y1, dat X2, dat Y2) {
  uldat t = 0;
  dat a, b;

  for (; Y1 <= Y2; Y1++) {
    a = ChangedVideo[Y1][0][0];
    b = ChangedVideo[Y1][0][1];
    if (a != -1 && a <= X2 && b >= X1)
      t += Min2(b, X2) - Max2(a, X1) + 1;
    if (a != -1) {
      a = ChangedVideo[Y1][1][0];
      b = ChangedVideo[Y1][1][0];
      if (a != -1 && a <= X2 && b >= X1)
        t += Min2(b, X2) - Max2(a, X1) + 1;
    }
  }
  return t;
}

void StrategyReset(void) NOTHROW {
  AccelVideo[0] = AccelVideo[1] = TW_MAXDAT;
  AccelVideo[2] = AccelVideo[3] = TW_MINDAT;
  StrategyFlag = HW_UNSET;
}

byte Strategy4Video(dat Xstart, dat Ystart, dat Xend, dat Yend) NOTHROW {
  uldat Varea = 0, XYarea_2 = (Xend - Xstart + 1) * (Yend - Ystart + 1) / 2;
  dat x1, y1, x2, y2;

  if (QueuedDrawArea2FullScreen || (StrategyFlag != HW_UNSET && StrategyFlag != HW_ACCEL))
    return HW_BUFFER;

  /* find the intersection between the current area and AccelVideo[] */
  if (Xstart < AccelVideo[2] && Ystart < AccelVideo[3] && Xend >= AccelVideo[0] &&
      Yend >= AccelVideo[1]) {

    x1 = Max2(Xstart, AccelVideo[0]);
    y1 = Max2(Ystart, AccelVideo[1]);
    x2 = Min2(Xend, AccelVideo[2]);
    y2 = Min2(Yend, AccelVideo[3]);

    Varea = (uldat)(x2 - x1 + 1) * (y2 - y1 + 1);
  }
  if (Varea < XYarea_2)
    Varea += Plain_countDirtyVideo(Xstart, Ystart, Xend, Yend);

  if (Varea < XYarea_2) {
    AccelVideo[0] = Min2(Xstart, AccelVideo[0]);
    AccelVideo[1] = Min2(Ystart, AccelVideo[1]);
    AccelVideo[2] = Max2(Xend, AccelVideo[2]);
    AccelVideo[3] = Max2(Yend, AccelVideo[3]);
    return StrategyFlag = HW_ACCEL;
  }
  return StrategyFlag = HW_BUFFER;
}

byte AllHWCanDragAreaNow(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  dat DstRgt = DstLeft + (Rgt - Left), DstDwn = DstUp + (Dwn - Up);
  byte Accel;

  if (CanDragArea && Strategy4Video(DstLeft, DstUp, DstRgt, DstDwn) == HW_ACCEL) {
    Accel = ttrue;
    forHW {
      if (HW->CanDragArea && HW->CanDragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp))
        ;
      else {
        Accel = tfalse;
        break;
      }
    }
  } else
    Accel = tfalse;

  return Accel;
}

void DragAreaHW(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  forHW {
    HW->DragArea(Left, Up, Rgt, Dwn, DstLeft, DstUp);
  }
}

void EnableMouseMotionEvents(byte enable) {
  ConfigureHW(HW_MOUSEMOTIONEVENTS, tfalse, enable);
}

byte MouseEventCommon(dat x, dat y, dat dx, dat dy, udat Buttons) {
  dat prev_x, prev_y;
  udat OldButtons, i;
  mouse_state *OldState;
  udat result;
  byte ret = ttrue;
  byte alsoMotionEvents = All->MouseMotionN > 0;

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

  OldState->keys = Buttons &= HOLD_ANY;

  /* keep it available */
  All->MouseHW = HW;

  if (Buttons != OldButtons || ((alsoMotionEvents || OldButtons) && (x != prev_x || y != prev_y))) {

    if ((alsoMotionEvents || OldButtons) && (x != prev_x || y != prev_y)) {
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

byte StdAddMouseEvent(udat Code, dat MouseX, dat MouseY) {
  Tmsg Msg;
  event_mouse *Event;

  if (HW && HW == All->MouseHW && HW->FlagsHW & FlHWNoInput)
    return ttrue;

  if ((Code & MOUSE_ACTION_ANY) == MOVE_MOUSE && (Msg = Ext(WM, MsgPort)->LastMsg) &&
      Msg->Type == msg_mouse && (Event = &Msg->Event.EventMouse) && Event->Code == Code) {
    /* merge the two events */
    Event->X = MouseX;
    Event->Y = MouseY;
    return ttrue;
  }
  if ((Msg = New(msg)(msg_mouse, 0))) {
    Event = &Msg->Event.EventMouse;
    Event->Code = Code;
    Event->ShiftFlags = (udat)0;
    Event->X = MouseX;
    Event->Y = MouseY;
    SendMsg(Ext(WM, MsgPort), Msg);
    return ttrue;
  }
  return tfalse;
}

byte KeyboardEventCommon(udat Code, udat ShiftFlags, udat Len, const char *Seq) {
  event_keyboard *Event;
  Tmsg Msg;

  if (HW->FlagsHW & FlHWNoInput)
    return ttrue;

  if ((Msg = New(msg)(msg_key, Len))) {
    Event = &Msg->Event.EventKeyboard;

    Event->Code = Code;
    Event->ShiftFlags = ShiftFlags;
    Event->SeqLen = Len;
    CopyMem(Seq, Event->AsciiSeq, Len);
    Event->AsciiSeq[Len] = '\0'; /* terminate string with \0 */
    SendMsg(Ext(WM, MsgPort), Msg);
    return ttrue;
  }
  return tfalse;
}

#ifdef __linux__
#include <linux/kd.h>
#endif

byte InitTransUser(void) {
  udat c;
#ifdef __linux__

#define SCRNMAP_T unsigned short
#define SCRNMAP_IOCTL GIO_UNISCRNMAP

  SCRNMAP_T map[E_TABSZ];

  if (ioctl(0, SCRNMAP_IOCTL, map) == 0) {

    if (sizeof(SCRNMAP_T) == sizeof(trune))
      CopyMem(map + 0x80, All->Gtranslations[USER_MAP] + 0x80, sizeof(trune) * 0x80);
    else
      for (c = 0x80; c < 0x100; c++)
        All->Gtranslations[USER_MAP][c] = (trune)map[c];
  } else
#endif
  {
    /* if nothing better is available, initialize to direct-to-font translation */
    for (c = 0x80; c < 0x100; c++)
      All->Gtranslations[USER_MAP][c] = (trune)c | 0xf000;
  }
  for (c = 0; c < 0x80; c++)
    All->Gtranslations[USER_MAP][c] = (trune)c;
  return ttrue;
}
