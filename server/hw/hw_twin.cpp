/*
 *  hw_twin.h  --  functions to let twin display on another twin
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
#include "alloc.h"
#include "algo.h" // Min2u()
#include "main.h"
#include "data.h"
#include "remote.h"

#include "common.h"
#include "hw.h"
#include "hw_private.h"
#include "hw_dirty.h"
#include "log.h"
#include "stl/view.h"

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>
#include <Tw/Twkeys.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>

struct sel_req {
  tany Requestor;
  uldat ReqPrivate;
};

#define TSELMAX 2

class tw_driver {
public:
  Tdisplay hw;
  tdisplay dpy;
  twindow win;
  tmsgport port;
  byte SelCount;
  byte TSelCount;
  sel_req SelReq[TSELMAX];  /* buffers to hold selection request data while waiting from twin */
  sel_req TSelReq[TSELMAX]; /* buffers to hold selection request data while waiting from libtw */

  static bool InitHW(Tdisplay hw);
  static void QuitHW(Tdisplay hw);

  static void Beep(Tdisplay hw);
  static void Configure(Tdisplay hw, udat resource, byte todefault, udat value);
  static void CheckResize(Tdisplay hw, dat *x, dat *y);
  static void DetectSize(Tdisplay hw, dat *x, dat *y);
  static void DrawSome(Tdisplay hw, dat x, dat y, uldat len);
  static void FlushHW(Tdisplay hw);
  static void FlushVideo(Tdisplay hw);
  static void HandleMsg(Tdisplay hw, tmsg msg);
  static void KeyboardEvent(int fd, Tdisplay hw);
  static void Resize(Tdisplay hw, dat x, dat y);

  static void SelectionExport_TW(Tdisplay hw);
  static bool SelectionImport_TW(Tdisplay hw);
  static void SelectionNotify_TW(Tdisplay hw, uldat reqprivate, e_id magic, Chars mime, Chars data);
  static void SelectionRequest_TW(Tdisplay hw, Tobj requestor, uldat reqprivate);

  static void SelectionNotify_up(Tdisplay hw, uldat reqprivate, e_id magic, Chars mime, Chars data);
  static void SelectionRequest_up(Tdisplay hw, uldat requestor, uldat reqprivate);
};

#define twdriver(hw) ((tw_driver *)(hw)->Private)

#define twdata(hw) (twdriver(hw)->TSelReq)

TW_ATTR_HIDDEN void tw_driver::Beep(Tdisplay hw) {
  tw_driver *self = twdriver(hw);
  Tw_WriteCharsetWindow(self->dpy, self->win, 1, "\007");
  hw->setFlush();
}

TW_ATTR_HIDDEN void tw_driver::Configure(Tdisplay hw, udat resource, byte todefault, udat value) {
  tw_driver *self = twdriver(hw);
  switch (resource) {
  case HW_KBDAPPLIC:
    Tw_WriteCharsetWindow(self->dpy, self->win, 2, todefault || !value ? "\033>" : "\033=");
    hw->setFlush();
    break;
  case HW_ALTCURSKEYS:
    Tw_WriteCharsetWindow(self->dpy, self->win, 5, todefault || !value ? "\033[?1l" : "\033[?1h");
    hw->setFlush();
    break;
  case HW_BELLPITCH:
    if (todefault)
      Tw_WriteCharsetWindow(self->dpy, self->win, 5, "\033[10]");
    else {
      char buf[17];
      sprintf(buf, "\033[10;%u]", (unsigned)value);
      Tw_WriteCharsetWindow(self->dpy, self->win, strlen(buf), buf);
    }
    hw->setFlush();
    break;
  case HW_BELLDURATION:
    if (todefault)
      Tw_WriteCharsetWindow(self->dpy, self->win, 5, "\033[11]");
    else {
      char buf[17];
      sprintf(buf, "\033[11;%u]", (unsigned)value);
      Tw_WriteCharsetWindow(self->dpy, self->win, strlen(buf), buf);
    }
    hw->setFlush();
    break;
  case HW_MOUSEMOTIONEVENTS:
    if (todefault)
      value = 0;
    Tw_ChangeField(self->dpy, self->win, TWS_window_Attr, TW_WINDOW_WANT_MOUSE_MOTION,
                   value ? TW_WINDOW_WANT_MOUSE_MOTION : 0);
    hw->setFlush();
    break;
  default:
    break;
  }
}

TW_ATTR_HIDDEN void tw_driver::HandleMsg(Tdisplay hw, tmsg msg) {
  tevent_any event = &msg->Event;
  tw_driver *self = twdriver(hw);
  dat x, y, dx, dy;
  udat keys;

  switch (msg->Type) {
  case TW_MSG_SELECTIONCLEAR:
    hw->SelectionPrivate = 0; /* selection now owned by some other libtw client */
    TwinSelectionSetOwner((Tobj)hw, SEL_CURRENTTIME, SEL_CURRENTTIME);
    return;
  case TW_MSG_SELECTIONREQUEST:
    SelectionRequest_up(hw, event->EventSelectionRequest.Requestor,
                        event->EventSelectionRequest.ReqPrivate);
    return;
  case TW_MSG_SELECTIONNOTIFY:
    SelectionNotify_up(hw, event->EventSelectionNotify.ReqPrivate,                             //
                       e_id(event->EventSelectionNotify.Magic),                                //
                       Chars::from_c_maxlen(event->EventSelectionNotify.MIME, TW_MAX_MIMELEN), //
                       Chars(event->EventSelectionNotify.Data, event->EventSelectionNotify.Len));
    return;
  default:
    break;
  }

  if (event->EventCommon.W == self->win) {
    switch (msg->Type) {
    case TW_MSG_WIDGET_KEY:
      KeyboardEventCommon(hw, event->EventKeyboard.Code, event->EventKeyboard.ShiftFlags,
                          event->EventKeyboard.SeqLen, event->EventKeyboard.AsciiSeq);
      break;
    case TW_MSG_WIDGET_MOUSE:
      x = event->EventMouse.X;
      y = event->EventMouse.Y;
      dx = x == 0 ? -1 : x == DisplayWidth - 1 ? 1 : 0;
      dy = y == 0 ? -1 : y == DisplayHeight - 1 ? 1 : 0;
      keys = event->EventMouse.Code;
      keys = (keys & HOLD_ANY) | (isPRESS(keys) ? HOLD_CODE(PRESS_N(keys)) : 0);

      MouseEventCommon(hw, x, y, dx, dy, keys);
      break;
    case TW_MSG_WIDGET_CHANGE:
      if (hw->X != event->EventWidget.XWidth || hw->Y != event->EventWidget.YWidth) {

        hw->X = event->EventWidget.XWidth;
        hw->Y = event->EventWidget.YWidth;
        ResizeDisplayPrefer(hw);
      }
      break;
    case TW_MSG_WIDGET_GADGET:
      if (!event->EventGadget.Code)
        /* 0 == Close Code */
        hw->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
      break;
    default:
      break;
    }
  }
}

TW_ATTR_HIDDEN void tw_driver::KeyboardEvent(int fd, Tdisplay hw) {
  tw_driver *self = twdriver(hw);
  tmsg msg;
  SaveHW;
  SetHW(hw);
  bool firstloop = true;

  /*
   * All other parts of twin read and parse data from fds in big chunks,
   * while Tw_ReadMsg() returns only a single event at time.
   * To compensate this and avoid to lag behind, we do a small loop checking
   * for all messages received while reading the first one.
   */
  while ((firstloop || Tw_PendingMsg(self->dpy)) && (msg = Tw_ReadMsg(self->dpy, tfalse))) {
    HandleMsg(hw, msg);
    firstloop = false;
  }

  if (Tw_InPanic(self->dpy)) {
    hw->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
  }
  RestoreHW;
}

TW_ATTR_HIDDEN void tw_driver::DrawSome(Tdisplay hw, dat x, dat y, uldat len) {
  tw_driver *self = twdriver(hw);
  tcell *V, *oV;
  uldat buflen = 0;
  tcell *buf;
  dat xbegin = x, ybegin = y;

  V = Video + x + y * (ldat)DisplayWidth;
  oV = OldVideo + x + y * (ldat)DisplayWidth;

  for (; len; x++, V++, oV++, len--) {
    if (buflen && ValidOldVideo && *V == *oV) {
      Tw_WriteTCellWindow(self->dpy, self->win, xbegin, ybegin, buflen, buf);
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
    Tw_WriteTCellWindow(self->dpy, self->win, xbegin, ybegin, buflen, buf);
}

TW_ATTR_HIDDEN void tw_driver::FlushVideo(Tdisplay hw) {
  tw_driver *self = twdriver(hw);
  dat start, end;
  udat i;

  /* first burst all changes */
  if (ChangedVideoFlag) {
    for (i = 0; i < DisplayHeight * 2; i++) {
      start = ChangedVideo[i >> 1][i & 1][0];
      end = ChangedVideo[i >> 1][i & 1][1];
      if (start != -1) {
        DrawSome(hw, start, i >> 1, end - start + 1);
      }
    }
    hw->setFlush();
  }

  /* update the cursor */
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR && (CursorX != hw->XY[0] || CursorY != hw->XY[1]))) {
    Tw_GotoXYWindow(self->dpy, self->win, hw->XY[0] = CursorX, hw->XY[1] = CursorY);
    hw->setFlush();
  }
  if (!ValidOldVideo || CursorType != hw->TT) {
    /* Tw_SetCursorWindow(self->win, CursorType); */
    char buff[16];
    sprintf(buff, "\033[?%d;%d;%dc", (int)(CursorType & 0xFF), (int)((CursorType >> 8) & 0xFF),
            (int)((CursorType >> 16) & 0xFF));
    Tw_WriteCharsetWindow(self->dpy, self->win, strlen(buff), buff);
    hw->TT = CursorType;
    hw->setFlush();
  }

  hw->FlagsHW &= ~FlHWChangedMouseFlag;
  hw->RedrawVideo = false;
}

TW_ATTR_HIDDEN void tw_driver::FlushHW(Tdisplay hw) {
  tw_driver *self = twdriver(hw);
  byte ret = Tw_TimidFlush(self->dpy);
  if (ret == tfalse) {
    hw->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
  } else if (ret == ttrue) {
    if (hw->NeedHW & NEEDFromPreviousFlushHW) {
      hw->NeedHW &= ~NEEDFromPreviousFlushHW;
      RemoteCouldWrite(hw->keyboard_slot);
    }
    hw->clrFlush();
  } else { /* ret == ttrue+ttrue */
    if (!(hw->NeedHW & NEEDFromPreviousFlushHW)) {
      hw->NeedHW |= NEEDFromPreviousFlushHW;
      RemoteCouldntWrite(hw->keyboard_slot);
    }
  }
}

TW_ATTR_HIDDEN void tw_driver::DetectSize(Tdisplay hw, dat *x, dat *y) {
  *x = hw->X;
  *y = hw->Y;
}

TW_ATTR_HIDDEN void tw_driver::CheckResize(Tdisplay hw, dat *x, dat *y) { /* always ok */
}

TW_ATTR_HIDDEN void tw_driver::Resize(Tdisplay hw, dat x, dat y) {
  tw_driver *self = twdriver(hw);
  if (x != hw->X || y != hw->Y) {
    Tw_ResizeWindow(self->dpy, self->win, hw->X = x, hw->Y = y);
    hw->setFlush();
  }
}

#if 0
TW_ATTR_HIDDEN bool tw_driver::CanDragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft,
                            dat DstUp) {
  return (ldat)(Rgt - Left + 1) * (Dwn - Up + 1) > 20;
}

TW_ATTR_HIDDEN void tw_driver::DragArea(Tdisplay hw, dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
  Tw_DragAreaWindow(self->dpy, self->win, self->win, Left, Up, Rgt, Dwn, DstLeft, DstUp);
  hw->setFlush();
}
#endif

/*
 * import Selection from libtw
 */
TW_ATTR_HIDDEN bool tw_driver::SelectionImport_TW(Tdisplay hw) {
  return !hw->SelectionPrivate;
}

/*
 * export our Selection to libtw
 */
TW_ATTR_HIDDEN void tw_driver::SelectionExport_TW(Tdisplay hw) {
  if (!hw->SelectionPrivate) {
    tw_driver *self = twdriver(hw);
#ifdef DEBUG_HW_TWIN
    printf("exporting selection to libtw server\n");
#endif
    hw->SelectionPrivate = self->port;
    Tw_SetOwnerSelection(self->dpy, SEL_CURRENTTIME, SEL_CURRENTTIME);
    hw->setFlush();
  }
}

/*
 * request Selection from libtw
 */
TW_ATTR_HIDDEN void tw_driver::SelectionRequest_TW(Tdisplay hw, Tobj requestor, uldat reqprivate) {
  if (!hw->SelectionPrivate) {
    tw_driver *self = twdriver(hw);
    if (self->TSelCount < TSELMAX) {
#ifdef DEBUG_HW_TWIN
      printf("requesting selection (%d) from libtw server\n", self->TSelCount);
#endif
      /*
       * we exploit the ReqPrivate field of libtw Selection Request/Notify
       */
      self->TSelReq[self->TSelCount].Requestor = (topaque)requestor;
      self->TSelReq[self->TSelCount].ReqPrivate = reqprivate;
      Tw_RequestSelection(self->dpy, Tw_GetOwnerSelection(self->dpy), self->TSelCount++);
      hw->setFlush();
      /* we will get a TW_MSG_SELECTIONNOTIFY event, i.e.
       * a TW_SelectionNotify_up() call */
    } else {
      self->TSelCount = 0;
      log(WARNING) << "hw_twin.c: TW_SelectionRequest_TW1(): too many nested self->win Selection "
                      "Request events!\n";
    }
  }
  /* else race! someone else became Selection owner in the meanwhile... */
}

/*
 * request twin Selection from upper layer
 */
TW_ATTR_HIDDEN void tw_driver::SelectionRequest_up(Tdisplay hw, uldat requestor, uldat reqprivate) {
  tw_driver *self = twdriver(hw);
  if (self->SelCount < TSELMAX) {
#ifdef DEBUG_HW_TWIN
    printf("requesting selection (%d) from twin core\n", self->SelCount);
#endif
    /*
     * we exploit the ReqPrivate field of libtw Selection Request/Notify
     */
    self->SelReq[self->SelCount].Requestor = requestor;
    self->SelReq[self->SelCount].ReqPrivate = reqprivate;
    TwinSelectionRequest((Tobj)hw, self->SelCount++, TwinSelectionGetOwner());
    /* we will get a hw->SelectionNotify(), i.e. SelectionNotify_TW() call */
    /* the call **CAN** arrive while we are still inside TwinSelectionRequest() !!! */
  } else {
    self->SelCount = 0;
    log(WARNING)
        << "hw_twin.c: TW_SelectionRequest_up(): too many nested libtw Selection Request events!\n";
  }
}

/*
 * notify our Selection to libtw
 */
TW_ATTR_HIDDEN void tw_driver::SelectionNotify_TW(Tdisplay hw, uldat reqprivate, e_id magic,
                                                  Chars mime, Chars data) {
  tw_driver *self = twdriver(hw);
#ifdef DEBUG_HW_TWIN
  printf("notifying selection (%d/%d) to libtw server\n", reqprivate, self->SelCount - 1);
#endif
  if (reqprivate + 1 != self->SelCount) {
    return;
  }
  self->SelCount--;
  char mimeBuf[TW_MAX_MIMELEN] = {};
  CopyMem(mime.data(), mimeBuf, Min2u(mime.size(), TW_MAX_MIMELEN));
  Tw_NotifySelection(self->dpy, self->SelReq[self->SelCount].Requestor,
                     self->SelReq[self->SelCount].ReqPrivate, magic, mimeBuf, data.size(),
                     data.data());
  hw->setFlush();
}

/*
 * notify the libtw Selection to twin upper layer
 */
TW_ATTR_HIDDEN void tw_driver::SelectionNotify_up(Tdisplay hw, uldat reqprivate, e_id magic,
                                                  Chars mime, Chars data) {
  tw_driver *self = twdriver(hw);
#ifdef DEBUG_HW_TWIN
  printf("notifying selection (%d/%d) to twin core\n", reqprivate, self->TSelCount - 1);
#endif
  if (reqprivate + 1 == self->TSelCount) {
    self->TSelCount--;
    TwinSelectionNotify((Tobj)(topaque)self->TSelReq[self->TSelCount].Requestor,
                        self->TSelReq[self->TSelCount].ReqPrivate, magic, mime, data);
  }
}

TW_ATTR_HIDDEN void tw_driver::QuitHW(Tdisplay hw) {
  tw_driver *self = twdriver(hw);

  /*
   *redundant, and self->port, self->win could be unset:
   *
   * Tw_UnMapWidget(self->dpy, self->win);
   * Tw_DeleteObj(self->dpy, self->win);
   * Tw_DeleteObj(self->dpy, self->port);
   */
  Tw_Close(self->dpy);

  UnRegisterRemote(hw->keyboard_slot);
  hw->keyboard_slot = NOSLOT;
  hw->fnKeyboardEvent = NULL;
  hw->fnQuitHW = NULL;
}

TW_DECL_MAGIC(hw_twin_magic);

TW_ATTR_HIDDEN bool tw_driver::InitHW(Tdisplay hw) {
  Chars arg = hw->Name;
  char name[] = "twin :??? on twin";
  uldat len;
  tmenu tw_menu;
  tscreen tw_screen;

  if (arg.size() > 4) {
    arg = arg.view(4, arg.size());
    if (!arg.starts_with("twin")) {
      return false; /* user said "use <arg> as display, not libtw" */
    }
    // skip "twin"
    arg = arg.view(4, arg.size());

    if (arg.contains(Chars(",noinput"))) {
      hw->FlagsHW |= FlHWNoInput;
    }
    if (arg.contains(Chars(",slow"))) {
      hw->FlagsHW |= FlHWExpensiveFlushVideo;
    }
    if (arg.contains(Chars(",help"))) {
      log(INFO)
          << "   --hw=twin options:\n"
             "      @TWDPY      connect to TWDPY instead of $TWDISPLAY (must be first option)\n"
             "      ,help       show this help\n"
             "      ,noinput    open a view-only window - ignore input\n"
             "      ,slow       assume connection is slow\n";
      return false;
    }
    /* if '@' is present, it is followed by the TWDISPLAY to use */
    const size_t at = arg.find(Chars("@"));
    if (at != size_t(-1)) {
      arg = arg.view(at + 1, arg.size());
      const size_t comma = arg.find(Chars(","));
      if (comma != size_t(-1)) {
        arg = arg.view(0, comma);
      }
    } else {
      arg = Chars();
    }
  } else {
    arg = Chars();
  }

  if (!arg && !(arg = Chars::from_c(origTWDisplay))) {
    /*
     * we can't call Tw_Open(NULL) since getenv("TWDISPLAY")
     * returns OUR socket... and using ourself as display isn't
     * exactly a bright idea.
     */
    log(ERROR) << "      TW_InitHW() failed: TWDISPLAY is not set\n";
    return false;
  }
  tw_driver *self;
  if (!(hw->Private = self = (tw_driver *)AllocMem0(sizeof(tw_driver)))) {
    log(ERROR) << "      TW_InitHW(): Out of memory!\n";
    return false;
  }

#ifdef CONF__ALLOC
  Tw_ConfigMalloc(AllocMem, ReAllocMem, free);
#endif

  self->dpy = NULL;

  if (Tw_CheckMagic(hw_twin_magic) && (self->dpy = Tw_Open(arg.data())) &&

      /*
       * check if the server supports the functions we need and store their IDs
       * to avoid deadlocking later when we call them.
       */
      Tw_FindLFunction(self->dpy, Tw_MapWidget, Tw_WriteCharsetWindow, Tw_WriteTCellWindow,
                       Tw_GotoXYWindow, Tw_ResizeWindow,
                       /* Tw_DragAreaWindow, */ NULL) &&

      (tw_screen = Tw_FirstScreen(self->dpy)) &&
      (self->port = Tw_CreateMsgPort(self->dpy, 12, "Twin on Twin")) &&
      (tw_menu = Tw_CreateMenu(self->dpy, TCOL(tblack, twhite), TCOL(tblack, tgreen),
                               TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), TCOL(tred, twhite),
                               TCOL(tred, tgreen), (byte)0)) &&
      Tw_Item4MenuCommon(self->dpy, tw_menu)) {
    do {
      const tcolor b = TCOL(tblack, twhite), r = TCOL(tred, twhite);
      const tcolor color_array[14] = {b, r, b, b, b, b, b, b, b, r, b, b, b, b};
      Tw_Info4Menu(self->dpy, tw_menu, TW_ROW_ACTIVE, (uldat)14, " Twin on Twin ", color_array);

      sprintf(name + 5, "%s on twin", TWDisplay);
      len = strlen(name);

      self->win =
          Tw_CreateWindow(self->dpy, len, name, NULL, tw_menu, TCOL(twhite, tblack), TW_LINECURSOR,
                          TW_WINDOW_WANT_KEYS | TW_WINDOW_WANT_MOUSE | TW_WINDOW_WANT_CHANGES |
                              TW_WINDOW_DRAG | TW_WINDOW_RESIZE | TW_WINDOW_CLOSE,
                          TW_WINDOWFL_USECONTENTS | TW_WINDOWFL_CURSOR_ON,
                          (hw->X = GetDisplayWidth()), (hw->Y = GetDisplayHeight()), (uldat)0);
      if (!self->win) {
        break;
      }
      Tw_SetColorsWindow(self->dpy, self->win, 0x1FF, TCOL(tYELLOW, tcyan), TCOL(tGREEN, tBLUE),
                         TCOL(twhite, tBLUE), TCOL(tWHITE, tBLUE), TCOL(tWHITE, tBLUE),
                         TCOL(twhite, tblack), TCOL(twhite, tBLACK), TCOL(tBLACK, tblack),
                         TCOL(tblack, tBLACK));
      Tw_MapWidget(self->dpy, self->win, tw_screen);

      /*
       * NOT Tw_Sync(self->dpy) as it might deadlock when
       * twin:x displays on twin:y which displays on twin:x
       */
      Tw_Flush(self->dpy);

      self->TSelCount = self->SelCount = 0;

      hw->mouse_slot = NOSLOT;
      hw->keyboard_slot = RegisterRemote(Tw_ConnectionFd(self->dpy), (Tobj)hw,
                                         (void (*)(int, Tobj))&tw_driver::KeyboardEvent);
      if (hw->keyboard_slot == NOSLOT)
        break;

      hw->fnFlushVideo = &tw_driver::FlushVideo;
      hw->fnFlushHW = &tw_driver::FlushHW;

      hw->fnKeyboardEvent = &tw_driver::KeyboardEvent;
      /* mouse events handled by KeyboardEvent() */
      hw->fnMouseEvent = NULL;

      hw->XY[0] = hw->XY[1] = 0;
      hw->TT = (uldat)-1; /* force updating cursor */

      hw->fnShowMouse = NULL;
      hw->fnHideMouse = NULL;
      hw->fnUpdateMouseAndCursor = NULL;
      hw->MouseState.x = hw->MouseState.y = hw->MouseState.keys = 0;

      hw->fnDetectSize = &tw_driver::DetectSize;
      hw->fnCheckResize = &tw_driver::CheckResize;
      hw->fnResize = &tw_driver::Resize;

      hw->fnSelectionImport = &tw_driver::SelectionImport_TW;
      hw->fnSelectionExport = &tw_driver::SelectionExport_TW;
      hw->fnSelectionRequest = &tw_driver::SelectionRequest_TW;
      hw->fnSelectionNotify = &tw_driver::SelectionNotify_TW;
      hw->SelectionPrivate = 0;

      hw->fnCanDragArea = NULL;

      hw->fnBeep = &tw_driver::Beep;
      hw->fnConfigure = &tw_driver::Configure;
      hw->fnSetPalette = NULL;
      hw->fnResetPalette = NULL;

      hw->fnQuitHW = &tw_driver::QuitHW;
      hw->fnQuitKeyboard = NULL;
      hw->fnQuitMouse = NULL;
      hw->fnQuitVideo = NULL;

      hw->DisplayIsCTTY = false;
      hw->FlagsHW &= ~FlHWSoftMouse; /* mouse pointer handled by X11 server */

      hw->FlagsHW |= FlHWNeedOldVideo;
      hw->FlagsHW &= ~FlHWExpensiveFlushVideo;
      hw->NeedHW = 0;
      hw->CanResize = true;

      /*
       * we must draw everything on our new shiny window
       * without forcing all other displays
       * to redraw everything too.
       */
      hw->RedrawVideo = false;
      NeedRedrawVideo(hw, 0, 0, hw->X - 1, hw->Y - 1);

      return true;

    } while (0);
  } else {
    /* TwErrno(NULL) is valid... used when Tw_Open fails */
    uldat err;
    if ((err = Tw_Errno(self->dpy)))
      log(ERROR) << "      TW_InitHW() failed: " << Chars::from_c(Tw_StrError(self->dpy, err))
                 << " "
                 << Chars::from_c(Tw_StrErrorDetail(self->dpy, err, Tw_ErrnoDetail(self->dpy)))
                 << "\n";
    else {
      log(ERROR) << "      TW_InitHW() failed.\n";
    }
  }

  if (self->dpy && Tw_ConnectionFd(self->dpy) >= 0) {
    QuitHW(hw);
  }
  return false;
}

EXTERN_C byte InitModule(Tmodule Module) {
  Module->DoInit = &tw_driver::InitHW;
  return ttrue;
}

/* this MUST be defined, or it seems that a bug in dlsym() gets triggered */
EXTERN_C void QuitModule(Tmodule Module) {
}
