/*
 *  hw_display.c  --  functions to let twin use `twdisplay' as display
 *
 *  Copyright (C) 2000-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "algo.h"
#include "alloc.h"
#include "main.h"
#include "data.h"
#include "extreg.h"
#include "log.h"
#include "methods.h"
#include "remote.h"

#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "common.h"

class dpy_driver {
public:
  Tmsgport display, Helper;
};

#define dpydriver(hw) ((dpy_driver *)(hw)->Private)

static Tmsg gmsg;
static event_display *ev;
static uldat Used;

inline void display_CreateMsg(Tdisplay hw, udat Code, udat Len) {
  gmsg->Event.EventDisplay.Code = Code;
  gmsg->Event.EventDisplay.Len = Len;
}

static void display_Beep(Tdisplay hw) {
  display_CreateMsg(hw, ev_dpy_Beep, 0);
  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
  hw->setFlush();
}

static void display_Configure(Tdisplay hw, udat resource, byte todefault, udat value) {
  display_CreateMsg(hw, ev_dpy_Configure, 0);

  ev->X = resource;
  if (todefault) {
    ev->Y = -1;
  } else {
    ev->Y = (dat)value;
  }
  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
  hw->setFlush();
}

/* handle messages from twdisplay */
static void display_HandleEvent(Tdisplay hw) {
  Tmsg msg;
  Tmsgport helper = dpydriver(hw)->Helper;
  event_any *event;
  dat x, y, dx, dy;
  udat keys;

  while ((msg = helper->Msgs.First)) {

    msg->Remove();
    event = &msg->Event;

    switch (msg->Type) {
    case msg_widget_key:
      KeyboardEventCommon(hw, event->EventKeyboard.Code, event->EventKeyboard.ShiftFlags,
                          event->EventKeyboard.SeqLen, event->EventKeyboard.AsciiSeq);
      break;
    case msg_widget_mouse:
      x = event->EventMouse.X;
      y = event->EventMouse.Y;
      dx = x == 0 ? -1 : x == DisplayWidth - 1 ? 1 : 0;
      dy = y == 0 ? -1 : y == DisplayHeight - 1 ? 1 : 0;
      keys = event->EventMouse.Code;
      keys = (keys & HOLD_ANY) | (isPRESS(keys) ? HOLD_CODE(PRESS_N(keys)) : 0);

      MouseEventCommon(hw, x, y, dx, dy, keys);
      break;
    case msg_widget_gadget:
      if (!event->EventGadget.Code)
        /* 0 == Close Code */
        hw->NeedHW |= NeedPanicHW, NeedHW |= NeedPanicHW;
      break;
    case msg_selection_clear:
      /* selection now owned by some other client on the same display hw as twdisplay */
      hw->SelectionPrivate = (tany)0;
      /*
       * DO NOT use (Tobj)display here instead of (Tobj)hw, as it is a Tmsgport
       * and would bypass the OwnerOnce mechanism, often resulting in an infinite loop.
       */
      TwinSelectionSetOwner((Tobj)hw, SEL_CURRENTTIME, SEL_CURRENTTIME);
      break;
    case msg_selection_request:
      /*
       * should never happen, twdisplay uses libtw calls to manage Selection Requests
       */
      log(ERROR)
          << "\ntwin: display_HandleEvent(): unexpected SelectionRequest Message from twdisplay!\n";
#if 0
            TwinSelectionRequest(event->EventSelectionRequest.Requestor,
                                 event->EventSelectionRequest.ReqPrivate,
                                 TwinSelectionGetOwner());
#endif
      break;
    case msg_selection_notify:
      /*
       * should never happen, twdisplay uses libtw calls to manage Selection Notifies
       */
      log(ERROR)
          << "\ntwin: display_HandleEvent(): unexpected SelectionNotify Message from twdisplay!\n";
#if 0
      TwinSelectionNotify(
          dRequestor, dReqPrivate, event->EventSelectionNotify.Magic,
          event->EventSelectionNotify.MIME(),
          event->EventSelectionNotify.Data());
#endif
      break;

    case msg_display:
      switch (event->EventDisplay.Code) {
      case ev_dpy_RedrawVideo:
        /*
         * Not needed, twdisplay keeps its own copy of Video[]
         * and never generates ev_dpy_RedrawVideo events
         */
        log(ERROR) << "\ntwin: display_HandleEvent(): unexpected Display.RedrawVideo Message from "
                      "twdisplay!\n";
#if 0
                if (event->EventDisplay.Len == sizeof(dat) * 2)
                    NeedRedrawVideo(hw, event->EventDisplay.X, event->EventDisplay.Y,
                                    ((udat *)event->EventDisplay.Data)[0],
                                    ((udat *)event->EventDisplay.Data)[1]);
                break;
#endif
      case ev_dpy_Resize:
        if (hw->X != event->EventDisplay.X || hw->Y != event->EventDisplay.Y) {

          hw->X = event->EventDisplay.X;
          hw->Y = event->EventDisplay.Y;
          ResizeDisplayPrefer(hw);
        }
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
    msg->Delete();
  }
}

static void display_HelperH(Tmsgport Port) {
  display_HandleEvent(Port->AttachHW);
}

inline void display_DrawTCell(Tdisplay hw, dat x, dat y, udat buflen, tcell *buf) {
  display_CreateMsg(hw, ev_dpy_DrawTCell, buflen * sizeof(tcell));
  ev->X = x;
  ev->Y = y;
  ev->Data = buf;
  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
}

inline void display_DrawSome(Tdisplay hw, dat x, dat y, uldat len) {
  tcell *V, *oV;
  uldat buflen = 0;
  tcell *buf;
  dat startx = x, starty = y;

  V = Video + x + y * (ldat)DisplayWidth;
  oV = OldVideo + x + y * (ldat)DisplayWidth;

  for (; len; x++, V++, oV++, len--) {
    if (buflen && ValidOldVideo && *V == *oV) {
      display_DrawTCell(hw, startx, starty, buflen, buf);
      buflen = 0;
    }
    if (!ValidOldVideo || *V != *oV) {
      if (!buflen++) {
        startx = x;
        starty = y;
        buf = V;
      }
    }
  }
  if (buflen) {
    display_DrawTCell(hw, startx, starty, buflen, buf);
  }
}

inline void display_MoveToXY(Tdisplay hw, udat x, udat y) {
  display_CreateMsg(hw, ev_dpy_MoveToXY, 0);
  ev->X = x;
  ev->Y = y;
  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
}

inline void display_SetCursorType(Tdisplay hw, uldat type) {
  display_CreateMsg(hw, ev_dpy_SetCursorType, sizeof(uldat));
  ev->Data = &type;
  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
}

static void display_FlushVideo(Tdisplay hw) {
  dat start, end;
  udat i;

  /* first burst all changes */
  if (ChangedVideoFlag) {
    for (i = 0; i < DisplayHeight * 2; i++) {
      start = ChangedVideo[i >> 1][i & 1][0];
      end = ChangedVideo[i >> 1][i & 1][1];

      if (start != -1)
        display_DrawSome(hw, start, i >> 1, end - start + 1);
    }
    hw->setFlush();
  }

  /* update the cursor */
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR && (CursorX != hw->XY[0] || CursorY != hw->XY[1]))) {
    display_MoveToXY(hw, hw->XY[0] = CursorX, hw->XY[1] = CursorY);
    hw->setFlush();
  }
  if (!ValidOldVideo || CursorType != hw->TT) {
    display_SetCursorType(hw, hw->TT = CursorType);
    hw->setFlush();
  }
  hw->FlagsHW &= ~FlagChangedMouseFlagHW;
}

static void display_FlushHW(Tdisplay hw) {
  display_CreateMsg(hw, ev_dpy_FlushHW, 0);
  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
  if (RemoteFlush(hw->AttachSlot)) {
    hw->clrFlush();
  }
}

static void display_DetectSize(Tdisplay hw, dat *x, dat *y) {
  *x = hw->X;
  *y = hw->Y;
}

static void display_CheckResize(Tdisplay hw, dat *x, dat *y) {
  if (!hw->CanResize) {
    *x = Min2(*x, hw->X);
    *y = Min2(*y, hw->Y);
  }
}

static void display_Resize(Tdisplay hw, dat x, dat y) {
  /*
   * when !hw->CanResize we send the Resize message even if
   * x == hw->X && y == hw->Y as twdisplay might be using a smaller area
   */
  if (!hw->CanResize || x != hw->X || y != hw->Y) {
    display_CreateMsg(hw, ev_dpy_Resize, 0);
    ev->X = x;
    ev->Y = y;

    if (hw->CanResize) {
      hw->X = x;
      hw->Y = y;
    }

    Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
    hw->setFlush();
  }
}

static bool display_CanDragArea(Tdisplay /*hw*/, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft,
                                dat DstUp) {
  return (Rgt - Left + 1) * (Dwn - Up + 1) > 20;
}

static void display_DragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft,
                             dat DstUp) {
  udat data[4];

  data[0] = Rgt;
  data[1] = Dwn;
  data[2] = DstLeft;
  data[3] = DstUp;

  display_CreateMsg(hw, ev_dpy_DragArea, 4 * sizeof(dat));

  ev->X = Left;
  ev->Y = Up;
  ev->Data = data;

  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
  hw->setFlush();
}

static void display_SetPalette(Tdisplay hw, udat N, udat R, udat G, udat B) {
  udat data[3];
  data[0] = R;
  data[1] = G;
  data[2] = B;

  display_CreateMsg(hw, ev_dpy_SetPalette, 4 * sizeof(dat));

  ev->X = N;
  ev->Data = data;

  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
  hw->setFlush();
}

static void display_ResetPalette(Tdisplay hw) {
  display_CreateMsg(hw, ev_dpy_ResetPalette, 0);
  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
  hw->setFlush();
}

/*
 * import Selection from twdisplay
 */
static bool display_SelectionImport_display(Tdisplay hw) {
  return !hw->SelectionPrivate;
}

/*
 * export our Selection to twdisplay
 */
static void display_SelectionExport_display(Tdisplay hw) {
  if (!hw->SelectionPrivate) {
    hw->SelectionPrivate = (tany)dpydriver(hw)->display;
    display_CreateMsg(hw, ev_dpy_SelectionExport, 0);
    Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
    hw->setFlush();
  }
}

/*
 * request Selection from twdisplay
 */
static void display_SelectionRequest_display(Tdisplay hw, Tobj requestor, uldat reqprivate) {
  if (!hw->SelectionPrivate) {
    /*
     * shortcut: since (display) is a Tmsgport, use fail-safe TwinSelectionRequest()
     * to send message to twdisplay.
     */
    TwinSelectionRequest(requestor, reqprivate, (Tobj)dpydriver(hw)->display);
  }
  /* else race! someone else became Selection owner in the meanwhile... */
}

/*
 * notify our Selection to twdisplay
 */
static void display_SelectionNotify_display(Tdisplay hw, uldat reqprivate, e_id magic, Chars mime,
                                            Chars data) {
  /*
   * shortcut: since (display) is a Tmsgport, use fail-safe TwinSelectionNotify()
   * to send message to twdisplay.
   */
  TwinSelectionNotify((Tobj)dpydriver(hw)->display, reqprivate, magic, mime, data);
}

static void display_QuitHW(Tdisplay hw) {
  /* tell twdisplay to cleanly quit */
  display_CreateMsg(hw, ev_dpy_Quit, 4 * sizeof(dat));
  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
  RemoteFlush(hw->AttachSlot);

  /* then cleanup */

  dpydriver(hw)->Helper->AttachHW = (Tdisplay)0; /* to avoid infinite loop */
  dpydriver(hw)->Helper->Delete();

  if (!--Used && gmsg) {
    gmsg->Delete();
    gmsg = NULL;
  }

  /*
   * the rest is done by hw->DoQuit(hw) by the upper layers,
   * including KillSlot(hw->AttachSlot) which forces twdisplay
   * to shutdown its display and quit
   */

  hw->fnKeyboardEvent = NULL;
  hw->fnQuitHW = NULL;
}

static void display_FixName(Tdisplay hw) {
  Chars name = hw->Name;
  if (name.starts_with(Chars("-hw=display@(-hw="))) {
    size_t pos = name.find(")");

    // the following requires String::format to never shrink String's capacity
    // and to use memmove() to fill String
    hw->Name.format("-display=", name.view(17, pos != size_t(-1) ? pos : name.size()));
  }
}

static bool display_InitHW(Tdisplay hw) {
  Chars arg = hw->Name;
  Tmsgport Port;

  if (arg.size() > 4) {
    arg = arg.view(4, arg.size());
    if (arg.size() < 7 || memcmp(arg.data(), "display", 7)) {
      return false; /* user said "use <arg> as display, not twdisplay" */
    }
    arg = arg.view(7, arg.size());
  } else
    arg = Chars();

  if (hw->AttachSlot == NOSLOT) {
    /*
     * we can't start unless we have a connected twdisplay...
     */
    log(ERROR) << "      display_InitHW() failed: not connected to twdisplay.\n"
                  "      (you cannot use -hw=display from command line or twattach)\n";
    return false;
  }

  if (!(Port = RemoteGetMsgPort(hw->AttachSlot))) {
    log(ERROR) << "      display_InitHW() failed: twdisplay did not create a MsgPort.\n";
    return false;
  }

  if (!Ext(Socket, SendMsg)) {
    log(ERROR) << "      display_InitHW() failed: SocketSendMsg() not available.\n"
                  "      (maybe you should load Socket Server module?)\n";
    return false;
  }

  if (!(hw->Private = (dpy_driver *)AllocMem0(sizeof(dpy_driver))) ||
      !(dpydriver(hw)->Helper =
            Smsgport::Create(16, "twdisplay Helper", 0, 0, 0, display_HelperH)) ||
      (!gmsg && !(gmsg = Smsg::Create(msg_display, sizeof(uldat))))) {

    if (hw->Private) {
      if (dpydriver(hw)->Helper) {
        dpydriver(hw)->Helper->Delete();
      }
      FreeMem(hw->Private);
    }
    log(ERROR) << "      display_InitHW(): Out of memory!\n";
    return false;
  }

  ev = &gmsg->Event.EventDisplay;
  dpydriver(hw)->display = Port;
  dpydriver(hw)->Helper->AttachHW = hw;

  hw->mouse_slot = NOSLOT;
  hw->keyboard_slot = NOSLOT;

  hw->fnFlushVideo = display_FlushVideo;
  hw->fnFlushHW = display_FlushHW;

  /* we must go through display_HelperH */
  hw->fnKeyboardEvent = NULL;
  /* mouse events handled by display_HelperH */
  hw->fnMouseEvent = NULL;

  hw->XY[0] = hw->XY[1] = 0;
  hw->TT = (uldat)-1; /* force updating cursor */

  hw->fnShowMouse = NULL;
  hw->fnHideMouse = NULL;
  hw->fnUpdateMouseAndCursor = NULL;
  hw->MouseState.x = hw->MouseState.y = hw->MouseState.keys = 0;

  hw->fnDetectSize = display_DetectSize;
  hw->fnCheckResize = display_CheckResize;
  hw->fnResize = display_Resize;

  hw->fnSelectionImport = display_SelectionImport_display;
  hw->fnSelectionExport = display_SelectionExport_display;
  hw->fnSelectionRequest = display_SelectionRequest_display;
  hw->fnSelectionNotify = display_SelectionNotify_display;
  hw->SelectionPrivate = (tany)0;

  if (arg.contains(Chars(",drag"))) {
    hw->fnCanDragArea = display_CanDragArea;
    hw->fnDragArea = display_DragArea;
  } else
    hw->fnCanDragArea = NULL;

  hw->fnBeep = display_Beep;
  hw->fnConfigure = display_Configure;
  hw->fnSetPalette = display_SetPalette;
  hw->fnResetPalette = display_ResetPalette;

  hw->fnQuitHW = display_QuitHW;
  hw->fnQuitKeyboard = NULL;
  hw->fnQuitMouse = NULL;
  hw->fnQuitVideo = NULL;

  hw->DisplayIsCTTY = false;
  hw->FlagsHW &= ~FlagSoftMouseHW;

  hw->FlagsHW |= FlagNeedOldVideoHW;
  if (arg.contains(Chars(",slow"))) {
    hw->FlagsHW |= FlagExpensiveFlushVideoHW;
  } else {
    hw->FlagsHW &= ~FlagExpensiveFlushVideoHW;
  }
  hw->NeedHW = NeedPersistentSlot;
  hw->CanResize = arg.contains(Chars(",resize"));

  display_CreateMsg(hw, ev_dpy_Helper, sizeof(dpydriver(hw)->Helper->Id));
  ev->Len = sizeof(uldat);
  ev->Data = &dpydriver(hw)->Helper->Id;
  Ext(Socket, SendMsg)(dpydriver(hw)->display, gmsg);
  /* don't flush now, twdisplay waits for attach messages */

  size_t pos;
  if ((pos = arg.find(Chars(",x="))) != size_t(-1)) {
    hw->X = atoi(&arg[pos + 3]); // safe, arg is '\0' terminated
  } else {
    hw->X = GetDisplayWidth();
  }
  if ((pos = arg.find(Chars(",y="))) != size_t(-1)) {
    hw->Y = atoi(&arg[pos + 3]); // safe, arg is '\0' terminated
  } else {
    hw->Y = GetDisplayHeight();
  }
  /*
   * we must draw everything on our new shiny window
   * without forcing all other displays
   * to redraw everything too.
   */
  hw->RedrawVideo = false;
  NeedRedrawVideo(hw, 0, 0, hw->X - 1, hw->Y - 1);

  hw->setFlush();

  Used++;

  display_FixName(hw);

  return true;
}

EXTERN_C byte InitModule(Tmodule Module) {
  Module->DoInit = display_InitHW;
  return ttrue;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
EXTERN_C void QuitModule(Tmodule Module) {
}
