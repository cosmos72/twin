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

struct display_data {
  msgport display, Helper;
};

#define displaydata ((struct display_data *)HW->Private)
#define display (displaydata->display)
#define Helper (displaydata->Helper)

static msg Msg;
static event_display *ev;
static uldat Used;

inline void display_CreateMsg(udat Code, udat Len) {
  Msg->Event.EventDisplay.Code = Code;
  Msg->Event.EventDisplay.Len = Len;
}

static void display_Beep(void) {
  display_CreateMsg(ev_dpy_Beep, 0);
  Ext(Socket, SendMsg)(display, Msg);
  setFlush();
}

static void display_Configure(udat resource, byte todefault, udat value) {
  display_CreateMsg(ev_dpy_Configure, 0);

  ev->X = resource;
  if (todefault)
    ev->Y = -1;
  else
    ev->Y = (dat)value;
  Ext(Socket, SendMsg)(display, Msg);
  setFlush();
}

/* handle messages from twdisplay */
static void display_HandleEvent(display_hw hw) {
  msg hMsg;
  event_any *Event;
  dat x, y, dx, dy;
  udat keys;

  SaveHW;
  SetHW(hw);

  while ((hMsg = Helper->FirstMsg)) {

    hMsg->Remove();
    Event = &hMsg->Event;

    switch (hMsg->Type) {
    case msg_widget_key:
      KeyboardEventCommon(Event->EventKeyboard.Code, Event->EventKeyboard.ShiftFlags,
                          Event->EventKeyboard.SeqLen, Event->EventKeyboard.AsciiSeq);
      break;
    case msg_widget_mouse:
      x = Event->EventMouse.X;
      y = Event->EventMouse.Y;
      dx = x == 0 ? -1 : x == DisplayWidth - 1 ? 1 : 0;
      dy = y == 0 ? -1 : y == DisplayHeight - 1 ? 1 : 0;
      keys = Event->EventMouse.Code;
      keys = (keys & HOLD_ANY) | (isPRESS(keys) ? HOLD_CODE(PRESS_N(keys)) : 0);

      MouseEventCommon(x, y, dx, dy, keys);
      break;
    case msg_widget_gadget:
      if (!Event->EventGadget.Code)
        /* 0 == Close Code */
        HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
      break;
    case msg_selection_clear:
      /* selection now owned by some other client on the same display HW as twdisplay */
      HW->HWSelectionPrivate = (tany)0;
      /*
       * DO NOT use (obj)display here instead of (obj)HW, as it is a msgport
       * and would bypass the OwnerOnce mechanism, often resulting in an infinite loop.
       */
      TwinSelectionSetOwner((obj)HW, SEL_CURRENTTIME, SEL_CURRENTTIME);
      break;
    case msg_selection_request:
      /*
       * should never happen, twdisplay uses libtw calls to manage Selection Requests
       */
      printk(
          "\ntwin: display_HandleEvent(): unexpected SelectionRequest Message from twdisplay!\n");
#if 0
            TwinSelectionRequest(Event->EventSelectionRequest.Requestor,
                                 Event->EventSelectionRequest.ReqPrivate,
                                 TwinSelectionGetOwner());
#endif
      break;
    case msg_selection_notify:
      /*
       * should never happen, twdisplay uses libtw calls to manage Selection Notifies
       */
      printk("\ntwin: display_HandleEvent(): unexpected SelectionNotify Message from twdisplay!\n");
#if 0
      TwinSelectionNotify(
          dRequestor, dReqPrivate, Event->EventSelectionNotify.Magic,
          Event->EventSelectionNotify.MIME,
          Chars(Event->EventSelectionNotify.Data, Event->EventSelectionNotify.Len));
#endif
      break;

    case msg_display:
      switch (Event->EventDisplay.Code) {
      case ev_dpy_RedrawVideo:
        /*
         * Not needed, twdisplay keeps its own copy of Video[]
         * and never generates ev_dpy_RedrawVideo events
         */
        printk("\ntwin: display_HandleEvent(): unexpected Display.RedrawVideo Message from "
               "twdisplay!\n");
#if 0
                if (Event->EventDisplay.Len == sizeof(dat) * 2)
                    NeedRedrawVideo(Event->EventDisplay.X, Event->EventDisplay.Y,
                                    ((udat *)Event->EventDisplay.Data)[0],
                                    ((udat *)Event->EventDisplay.Data)[1]);
                break;
#endif
      case ev_dpy_Resize:
        if (HW->X != Event->EventDisplay.X || HW->Y != Event->EventDisplay.Y) {

          HW->X = Event->EventDisplay.X;
          HW->Y = Event->EventDisplay.Y;
          ResizeDisplayPrefer(HW);
        }
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
    hMsg->Delete();
  }
  RestoreHW;
}

static void display_HelperH(msgport Port) {
  display_HandleEvent(Port->AttachHW);
}

inline void display_DrawTCell(dat x, dat y, udat buflen, tcell *buf) {
  display_CreateMsg(ev_dpy_DrawTCell, buflen * sizeof(tcell));
  ev->X = x;
  ev->Y = y;
  ev->Data = buf;
  Ext(Socket, SendMsg)(display, Msg);
}

inline void display_Mogrify(dat x, dat y, uldat len) {
  tcell *V, *oV;
  uldat buflen = 0;
  tcell *buf;
  dat xbegin = x, ybegin = y;

  V = Video + x + y * (ldat)DisplayWidth;
  oV = OldVideo + x + y * (ldat)DisplayWidth;

  for (; len; x++, V++, oV++, len--) {
    if (buflen && ValidOldVideo && *V == *oV) {
      display_DrawTCell(xbegin, ybegin, buflen, buf);
      buflen = 0;
    }
    if (!ValidOldVideo || *V != *oV) {
      if (!buflen++) {
        xbegin = x;
        ybegin = y;
        buf = V;
      }
    }
  }
  if (buflen)
    display_DrawTCell(xbegin, ybegin, buflen, buf);
}

inline void display_MoveToXY(udat x, udat y) {
  display_CreateMsg(ev_dpy_MoveToXY, 0);
  ev->X = x;
  ev->Y = y;
  Ext(Socket, SendMsg)(display, Msg);
}

inline void display_SetCursorType(uldat type) {
  display_CreateMsg(ev_dpy_SetCursorType, sizeof(uldat));
  ev->Data = &type;
  Ext(Socket, SendMsg)(display, Msg);
}

static void display_FlushVideo(void) {
  dat start, end;
  udat i;

  /* first burst all changes */
  if (ChangedVideoFlag) {
    for (i = 0; i < DisplayHeight * 2; i++) {
      start = ChangedVideo[i >> 1][i & 1][0];
      end = ChangedVideo[i >> 1][i & 1][1];

      if (start != -1)
        display_Mogrify(start, i >> 1, end - start + 1);
    }
    setFlush();
  }

  /* update the cursor */
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR && (CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
    display_MoveToXY(HW->XY[0] = CursorX, HW->XY[1] = CursorY);
    setFlush();
  }
  if (!ValidOldVideo || CursorType != HW->TT) {
    display_SetCursorType(HW->TT = CursorType);
    setFlush();
  }
  HW->FlagsHW &= ~FlHWChangedMouseFlag;
}

static void display_FlushHW(void) {
  display_CreateMsg(ev_dpy_FlushHW, 0);
  Ext(Socket, SendMsg)(display, Msg);
  if (RemoteFlush(HW->AttachSlot))
    clrFlush();
}

static void display_DetectSize(dat *x, dat *y) {
  *x = HW->X;
  *y = HW->Y;
}

static void display_CheckResize(dat *x, dat *y) {
  if (!HW->CanResize) {
    *x = Min2(*x, HW->X);
    *y = Min2(*y, HW->Y);
  }
}

static void display_Resize(dat x, dat y) {
  /*
   * when !HW->CanResize we send the Resize message even if
   * x == HW->X && y == HW->Y as twdisplay might be using a smaller area
   */
  if (!HW->CanResize || x != HW->X || y != HW->Y) {
    display_CreateMsg(ev_dpy_Resize, 0);
    ev->X = x;
    ev->Y = y;

    if (HW->CanResize) {
      HW->X = x;
      HW->Y = y;
    }

    Ext(Socket, SendMsg)(display, Msg);
    setFlush();
  }
}

static byte display_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  return (Rgt - Left + 1) * (Dwn - Up + 1) > 20;
}

static void display_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  udat data[4];

  data[0] = Rgt;
  data[1] = Dwn;
  data[2] = DstLeft;
  data[3] = DstUp;

  display_CreateMsg(ev_dpy_DragArea, 4 * sizeof(dat));

  ev->X = Left;
  ev->Y = Up;
  ev->Data = data;

  Ext(Socket, SendMsg)(display, Msg);
  setFlush();
}

static void display_SetPalette(udat N, udat R, udat G, udat B) {
  udat data[3];
  data[0] = R;
  data[1] = G;
  data[2] = B;

  display_CreateMsg(ev_dpy_SetPalette, 4 * sizeof(dat));

  ev->X = N;
  ev->Data = data;

  Ext(Socket, SendMsg)(display, Msg);
  setFlush();
}

static void display_ResetPalette(void) {
  display_CreateMsg(ev_dpy_ResetPalette, 0);
  Ext(Socket, SendMsg)(display, Msg);
  setFlush();
}

/*
 * import Selection from twdisplay
 */
static byte display_SelectionImport_display(void) {
  return !HW->HWSelectionPrivate;
}

/*
 * export our Selection to twdisplay
 */
static void display_SelectionExport_display(void) {
  if (!HW->HWSelectionPrivate) {
    HW->HWSelectionPrivate = (tany)display;
    display_CreateMsg(ev_dpy_SelectionExport, 0);
    Ext(Socket, SendMsg)(display, Msg);
    setFlush();
  }
}

/*
 * request Selection from twdisplay
 */
static void display_SelectionRequest_display(obj Requestor, uldat ReqPrivate) {
  if (!HW->HWSelectionPrivate) {
    /*
     * shortcut: since (display) is a msgport, use fail-safe TwinSelectionRequest()
     * to send message to twdisplay.
     */
    TwinSelectionRequest(Requestor, ReqPrivate, (obj)display);
  }
  /* else race! someone else became Selection owner in the meanwhile... */
}

/*
 * notify our Selection to twdisplay
 */
static void display_SelectionNotify_display(uldat ReqPrivate, e_id Magic,
                                            const char MIME[MAX_MIMELEN], Chars Data) {
  /*
   * shortcut: since (display) is a msgport, use fail-safe TwinSelectionNotify()
   * to send message to twdisplay.
   */
  TwinSelectionNotify((obj)display, ReqPrivate, Magic, MIME, Data);
}

static void display_QuitHW(void) {
  /* tell twdisplay to cleanly quit */
  display_CreateMsg(ev_dpy_Quit, 4 * sizeof(dat));
  Ext(Socket, SendMsg)(display, Msg);
  RemoteFlush(HW->AttachSlot);

  /* then cleanup */

  Helper->AttachHW = (display_hw)0; /* to avoid infinite loop */
  Helper->Delete();

  if (!--Used && Msg) {
    Msg->Delete();
    Msg = NULL;
  }

  /*
   * the rest is done by HW->DoQuit(HW) by the upper layers,
   * including KillSlot(HW->AttachSlot) which forces twdisplay
   * to shutdown its display and quit
   */

  HW->KeyboardEvent = (void (*)(int, display_hw))NoOp;
  HW->QuitHW = NoOp;
}

static void fix4display(void) {
  Chars name = HW->Name;
  if (name.starts_with(Chars("-hw=display@(-hw="))) {
    size_t pos = name.find(")");

    // the following requires String::format to never shrink String's capacity
    // and to use memmove() to fill String
    HW->Name.format("-display=", name.view(17, pos != size_t(-1) ? pos : name.size()));
  }
}

static bool display_InitHW(void) {
  Chars arg = HW->Name;
  msgport Port;

  if (arg.size() > 4) {
    arg = arg.view(4, arg.size());
    if (arg.size() < 7 || memcmp(arg.data(), "display", 7)) {
      return false; /* user said "use <arg> as display, not twdisplay" */
    }
    arg = arg.view(7, arg.size());
  } else
    arg = Chars();

  if (HW->AttachSlot == NOSLOT) {
    /*
     * we can't start unless we have a connected twdisplay...
     */
    log(ERROR, "      display_InitHW() failed: not connected to twdisplay.\n"
               "      (you cannot use -hw=display from command line or twattach)\n");
    return false;
  }

  if (!(Port = RemoteGetMsgPort(HW->AttachSlot))) {
    log(ERROR, "      display_InitHW() failed: twdisplay did not create a MsgPort.\n");
    return false;
  }

  if (!Ext(Socket, SendMsg)) {
    log(ERROR, "      display_InitHW() failed: SocketSendMsg() not available.\n"
               "      (maybe you should load Socket Server module?)\n");
    return false;
  }

  if (!(HW->Private = (struct display_data *)AllocMem(sizeof(struct display_data))) ||
      !(Helper = New(msgport)(16, "twdisplay Helper", 0, 0, 0, display_HelperH)) ||
      (!Msg && !(Msg = New(msg)(msg_display, sizeof(uldat))))) {

    if (HW->Private) {
      if (Helper) {
        Helper->Delete();
      }
      FreeMem(HW->Private);
    }
    log(ERROR, "      display_InitHW(): Out of memory!\n");
    return false;
  }

  ev = &Msg->Event.EventDisplay;
  display = Port;
  Helper->AttachHW = HW;

  HW->mouse_slot = NOSLOT;
  HW->keyboard_slot = NOSLOT;

  HW->FlushVideo = display_FlushVideo;
  HW->FlushHW = display_FlushHW;

  /* we must go through display_HelperH */
  HW->KeyboardEvent = (void (*)(int, display_hw))NoOp;
  /* mouse events handled by display_HelperH */
  HW->MouseEvent = (void (*)(int, display_hw))NoOp;

  HW->XY[0] = HW->XY[1] = 0;
  HW->TT = (uldat)-1; /* force updating cursor */

  HW->ShowMouse = NoOp;
  HW->HideMouse = NoOp;
  HW->UpdateMouseAndCursor = NoOp;
  HW->MouseState.x = HW->MouseState.y = HW->MouseState.keys = 0;

  HW->DetectSize = display_DetectSize;
  HW->CheckResize = display_CheckResize;
  HW->Resize = display_Resize;

  HW->HWSelectionImport = display_SelectionImport_display;
  HW->HWSelectionExport = display_SelectionExport_display;
  HW->HWSelectionRequest = display_SelectionRequest_display;
  HW->HWSelectionNotify = display_SelectionNotify_display;
  HW->HWSelectionPrivate = (tany)0;

  if (arg.contains(Chars(",drag"))) {
    HW->CanDragArea = display_CanDragArea;
    HW->DragArea = display_DragArea;
  } else
    HW->CanDragArea = NULL;

  HW->Beep = display_Beep;
  HW->Configure = display_Configure;
  HW->SetPalette = display_SetPalette;
  HW->ResetPalette = display_ResetPalette;

  HW->QuitHW = display_QuitHW;
  HW->QuitKeyboard = NoOp;
  HW->QuitMouse = NoOp;
  HW->QuitVideo = NoOp;

  HW->DisplayIsCTTY = tfalse;
  HW->FlagsHW &= ~FlHWSoftMouse;

  HW->FlagsHW |= FlHWNeedOldVideo;
  if (arg.contains(Chars(",slow")))
    HW->FlagsHW |= FlHWExpensiveFlushVideo;
  else
    HW->FlagsHW &= ~FlHWExpensiveFlushVideo;

  HW->NeedHW = NEEDPersistentSlot;
  HW->CanResize = arg.contains(Chars(",resize"));
  HW->merge_Threshold = 0;

  display_CreateMsg(ev_dpy_Helper, sizeof(Helper->Id));
  ev->Len = sizeof(uldat);
  ev->Data = &Helper->Id;
  Ext(Socket, SendMsg)(display, Msg);
  /* don't flush now, twdisplay waits for attach messages */

  size_t pos;
  if ((pos = arg.find(Chars(",x="))) != size_t(-1))
    HW->X = atoi(&arg[pos + 3]); // safe, arg is '\0' terminated
  else
    HW->X = GetDisplayWidth();
  if ((pos = arg.find(Chars(",y="))) != size_t(-1))
    HW->Y = atoi(&arg[pos + 3]); // safe, arg is '\0' terminated
  else
    HW->Y = GetDisplayHeight();

  /*
   * we must draw everything on our new shiny window
   * without forcing all other displays
   * to redraw everything too.
   */
  HW->RedrawVideo = tfalse;
  NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);

  setFlush();

  Used++;

  fix4display();

  return true;
}

EXTERN_C byte InitModule(module Module) {
  Module->DoInit = display_InitHW;
  return ttrue;
}

/* this MUST be included, or it seems that a bug in dlsym() gets triggered */
EXTERN_C void QuitModule(module Module) {
}
