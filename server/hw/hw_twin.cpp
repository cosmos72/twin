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

struct tw_data {
  tdisplay Td;
  twindow Twin;
  tmsgport Tmsgport;
  byte SelCount;
  byte TSelCount;
  sel_req SelReq[TSELMAX];  /* buffers to hold selection request data while waiting from twin */
  sel_req TSelReq[TSELMAX]; /* buffers to hold selection request data while waiting from libtw */
};

#define twdata ((tw_data *)HW->Private)
#define Td (twdata->Td)
#define Twin (twdata->Twin)
#define Tmsgport (twdata->Tmsgport)
#define SelCount (twdata->SelCount)
#define TSelCount (twdata->TSelCount)
#define SelReq (twdata->SelReq)
#define TSelReq (twdata->TSelReq)

static void TW_SelectionRequest_up(uldat Requestor, uldat ReqPrivate);
static void TW_SelectionNotify_up(uldat ReqPrivate, e_id Magic, const char MIME[MAX_MIMELEN],
                                  Chars Data);

static void TW_Beep(void) {
  Tw_WriteCharsetWindow(Td, Twin, 1, "\007");
  setFlush();
}

static void TW_Configure(udat resource, byte todefault, udat value) {
  switch (resource) {
  case HW_KBDAPPLIC:
    Tw_WriteCharsetWindow(Td, Twin, 2, todefault || !value ? "\033>" : "\033=");
    setFlush();
    break;
  case HW_ALTCURSKEYS:
    Tw_WriteCharsetWindow(Td, Twin, 5, todefault || !value ? "\033[?1l" : "\033[?1h");
    setFlush();
    break;
  case HW_BELLPITCH:
    if (todefault)
      Tw_WriteCharsetWindow(Td, Twin, 5, "\033[10]");
    else {
      char buf[10];
      sprintf(buf, "\033[10;%.3hd]", (unsigned)value);
      Tw_WriteCharsetWindow(Td, Twin, strlen(buf), buf);
    }
    setFlush();
    break;
  case HW_BELLDURATION:
    if (todefault)
      Tw_WriteCharsetWindow(Td, Twin, 5, "\033[11]");
    else {
      char buf[10];
      sprintf(buf, "\033[11;%.3hd]", (unsigned)value);
      Tw_WriteCharsetWindow(Td, Twin, strlen(buf), buf);
    }
    setFlush();
    break;
  case HW_MOUSEMOTIONEVENTS:
    if (todefault)
      value = 0;
    Tw_ChangeField(Td, Twin, TWS_window_Attr, TW_WINDOW_WANT_MOUSE_MOTION,
                   value ? TW_WINDOW_WANT_MOUSE_MOTION : 0);
    setFlush();
    break;
  default:
    break;
  }
}

static void TW_HandleMsg(tmsg Msg) {
  tevent_any Event;
  dat x, y, dx, dy;
  udat keys;

  Event = &Msg->Event;

  switch (Msg->Type) {
  case TW_MSG_SELECTIONCLEAR:
    HW->HWSelectionPrivate = 0; /* selection now owned by some other libtw client */
    TwinSelectionSetOwner((obj)HW, SEL_CURRENTTIME, SEL_CURRENTTIME);
    return;
  case TW_MSG_SELECTIONREQUEST:
    TW_SelectionRequest_up(Event->EventSelectionRequest.Requestor,
                           Event->EventSelectionRequest.ReqPrivate);
    return;
  case TW_MSG_SELECTIONNOTIFY:
    TW_SelectionNotify_up(Event->EventSelectionNotify.ReqPrivate,  //
                          e_id(Event->EventSelectionNotify.Magic), //
                          Event->EventSelectionNotify.MIME,        //
                          Chars(Event->EventSelectionNotify.Data, Event->EventSelectionNotify.Len));
    return;
  default:
    break;
  }

  if (Event->EventCommon.W == Twin) {
    switch (Msg->Type) {
    case TW_MSG_WIDGET_KEY:
      KeyboardEventCommon(Event->EventKeyboard.Code, Event->EventKeyboard.ShiftFlags,
                          Event->EventKeyboard.SeqLen, Event->EventKeyboard.AsciiSeq);
      break;
    case TW_MSG_WIDGET_MOUSE:
      x = Event->EventMouse.X;
      y = Event->EventMouse.Y;
      dx = x == 0 ? -1 : x == DisplayWidth - 1 ? 1 : 0;
      dy = y == 0 ? -1 : y == DisplayHeight - 1 ? 1 : 0;
      keys = Event->EventMouse.Code;
      keys = (keys & HOLD_ANY) | (isPRESS(keys) ? HOLD_CODE(PRESS_N(keys)) : 0);

      MouseEventCommon(x, y, dx, dy, keys);
      break;
    case TW_MSG_WIDGET_CHANGE:
      if (HW->X != Event->EventWidget.XWidth || HW->Y != Event->EventWidget.YWidth) {

        HW->X = Event->EventWidget.XWidth;
        HW->Y = Event->EventWidget.YWidth;
        ResizeDisplayPrefer(HW);
      }
      break;
    case TW_MSG_WIDGET_GADGET:
      if (!Event->EventGadget.Code)
        /* 0 == Close Code */
        HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
      break;
    default:
      break;
    }
  }
}

static void TW_KeyboardEvent(int fd, display_hw hw) {
  tmsg Msg;
  byte firstloop = ttrue;
  SaveHW;
  SetHW(hw);

  /*
   * All other parts of twin read and parse data from fds in big chunks,
   * while Tw_ReadMsg() returns only a single event at time.
   * To compensate this and avoid to lag behind, we do a small loop checking
   * for all messages received while reading the first one.
   */
  while ((firstloop || Tw_PendingMsg(Td)) && (Msg = Tw_ReadMsg(Td, tfalse))) {
    TW_HandleMsg(Msg);
    firstloop = tfalse;
  }

  if (Tw_InPanic(Td))
    HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
  RestoreHW;
}

inline void TW_Mogrify(dat x, dat y, uldat len) {
  tcell *V, *oV;
  uldat buflen = 0;
  tcell *buf;
  dat xbegin = x, ybegin = y;

  V = Video + x + y * (ldat)DisplayWidth;
  oV = OldVideo + x + y * (ldat)DisplayWidth;

  for (; len; x++, V++, oV++, len--) {
    if (buflen && ValidOldVideo && *V == *oV) {
      Tw_WriteTCellWindow(Td, Twin, xbegin, ybegin, buflen, buf);
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
    Tw_WriteTCellWindow(Td, Twin, xbegin, ybegin, buflen, buf);
}

static void TW_FlushVideo(void) {
  dat start, end;
  udat i;

  /* first burst all changes */
  if (ChangedVideoFlag) {
    for (i = 0; i < DisplayHeight * 2; i++) {
      start = ChangedVideo[i >> 1][i & 1][0];
      end = ChangedVideo[i >> 1][i & 1][1];

      if (start != -1)
        TW_Mogrify(start, i >> 1, end - start + 1);
    }
    setFlush();
  }

  /* update the cursor */
  if (!ValidOldVideo ||
      (CursorType != NOCURSOR && (CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
    Tw_GotoXYWindow(Td, Twin, HW->XY[0] = CursorX, HW->XY[1] = CursorY);
    setFlush();
  }
  if (!ValidOldVideo || CursorType != HW->TT) {
    /* Tw_SetCursorWindow(Twin, CursorType); */
    char buff[16];
    sprintf(buff, "\033[?%d;%d;%dc", (int)(CursorType & 0xFF), (int)((CursorType >> 8) & 0xFF),
            (int)((CursorType >> 16) & 0xFF));
    Tw_WriteCharsetWindow(Td, Twin, strlen(buff), buff);
    HW->TT = CursorType;
    setFlush();
  }

  HW->FlagsHW &= ~FlHWChangedMouseFlag;
  HW->RedrawVideo = tfalse;
}

static void TW_FlushHW(void) {
  byte ret = Tw_TimidFlush(Td);
  if (ret == tfalse)
    HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
  else if (ret == ttrue) {
    if (HW->NeedHW & NEEDFromPreviousFlushHW) {
      HW->NeedHW &= ~NEEDFromPreviousFlushHW;
      RemoteCouldWrite(HW->keyboard_slot);
    }
    clrFlush();
  } else { /* ret == ttrue+ttrue */
    if (!(HW->NeedHW & NEEDFromPreviousFlushHW)) {
      HW->NeedHW |= NEEDFromPreviousFlushHW;
      RemoteCouldntWrite(HW->keyboard_slot);
    }
  }
}

static void TW_DetectSize(dat *x, dat *y) {
  *x = HW->X;
  *y = HW->Y;
}

static void TW_CheckResize(dat *x, dat *y) { /* always ok */
}

static void TW_Resize(dat x, dat y) {
  if (x != HW->X || y != HW->Y) {
    Tw_ResizeWindow(Td, Twin, HW->X = x, HW->Y = y);
    setFlush();
  }
}

#if 0
static byte TW_canDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return (ldat)(Rgt-Left+1) * (Dwn-Up+1) > 20;
}

static void TW_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    Tw_DragAreaWindow(Td, Twin, Twin, Left, Up, Rgt, Dwn, DstLeft, DstUp);
    setFlush();
}
#endif

/*
 * import Selection from libtw
 */
static byte TW_SelectionImport_TW(void) {
  return !HW->HWSelectionPrivate;
}

/*
 * export our Selection to libtw
 */
static void TW_SelectionExport_TW(void) {
  if (!HW->HWSelectionPrivate) {
#ifdef DEBUG_HW_TWIN
    printf("exporting selection to libtw server\n");
#endif
    HW->HWSelectionPrivate = Tmsgport;
    Tw_SetOwnerSelection(Td, SEL_CURRENTTIME, SEL_CURRENTTIME);
    setFlush();
  }
}

/*
 * request Selection from libtw
 */
static void TW_SelectionRequest_TW(obj Requestor, uldat ReqPrivate) {
  if (!HW->HWSelectionPrivate) {
    if (TSelCount < TSELMAX) {
#ifdef DEBUG_HW_TWIN
      printf("requesting selection (%d) from libtw server\n", TSelCount);
#endif
      /*
       * we exploit the ReqPrivate field of libtw Selection Request/Notify
       */
      TSelReq[TSelCount].Requestor = (topaque)Requestor;
      TSelReq[TSelCount].ReqPrivate = ReqPrivate;
      Tw_RequestSelection(Td, Tw_GetOwnerSelection(Td), TSelCount++);
      setFlush();
      /* we will get a TW_MSG_SELECTIONNOTIFY event, i.e.
       * a TW_SelectionNotify_up() call */
    } else {
      TSelCount = 0;
      log(WARNING,
          "hw_twin.c: TW_SelectionRequest_TW1(): too many nested Twin Selection Request events!\n");
    }
  }
  /* else race! someone else became Selection owner in the meanwhile... */
}

/*
 * request twin Selection from upper layer
 */
static void TW_SelectionRequest_up(uldat Requestor, uldat ReqPrivate) {
  if (SelCount < TSELMAX) {
#ifdef DEBUG_HW_TWIN
    printf("requesting selection (%d) from twin core\n", SelCount);
#endif
    /*
     * we exploit the ReqPrivate field of libtw Selection Request/Notify
     */
    SelReq[SelCount].Requestor = Requestor;
    SelReq[SelCount].ReqPrivate = ReqPrivate;
    TwinSelectionRequest((obj)HW, SelCount++, TwinSelectionGetOwner());
    /* we will get a HW->HWSelectionNotify(), i.e. TW_SelectionNotify_TW() call */
    /* the call **CAN** arrive while we are still inside TwinSelectionRequest() !!! */
  } else {
    SelCount = 0;
    log(WARNING,
        "hw_twin.c: TW_SelectionRequest_up(): too many nested libtw Selection Request events!\n");
  }
}

/*
 * notify our Selection to libtw
 */
static void TW_SelectionNotify_TW(uldat ReqPrivate, e_id Magic, const char MIME[MAX_MIMELEN],
                                  Chars Data) {
#ifdef DEBUG_HW_TWIN
  printf("notifying selection (%d/%d) to libtw server\n", ReqPrivate, SelCount - 1);
#endif
  if (ReqPrivate + 1 == SelCount) {
    SelCount--;
    Tw_NotifySelection(Td, SelReq[SelCount].Requestor, SelReq[SelCount].ReqPrivate, Magic, MIME,
                       Data.size(), Data.data());
    setFlush();
  }
}

/*
 * notify the libtw Selection to twin upper layer
 */
static void TW_SelectionNotify_up(uldat ReqPrivate, e_id Magic, const char MIME[MAX_MIMELEN],
                                  Chars Data) {
#ifdef DEBUG_HW_TWIN
  printf("notifying selection (%d/%d) to twin core\n", ReqPrivate, TSelCount - 1);
#endif
  if (ReqPrivate + 1 == TSelCount) {
    TSelCount--;
    TwinSelectionNotify((obj)(topaque)TSelReq[TSelCount].Requestor, TSelReq[TSelCount].ReqPrivate,
                        Magic, MIME, Data);
  }
}

static void TW_QuitHW(void) {
  /* not necessary, and Tmsgport, Twin could be undefined */
  /*
   * Tw_UnMapWidget(Td, Twin);
   * Tw_DeleteObj(Td, Twin);
   * Tw_DeleteObj(Td, Tmsgport);
   */
  Tw_Close(Td);

  UnRegisterRemote(HW->keyboard_slot);
  HW->keyboard_slot = NOSLOT;

  HW->KeyboardEvent = (void (*)(int, display_hw))NoOp;

  HW->QuitHW = NoOp;
}

TW_DECL_MAGIC(hw_twin_magic);

static bool TW_InitHW(void) {
  Chars arg = HW->Name;
  char name[] = "twin :??? on twin";
  uldat len;
  tmenu Tmenu;
  tscreen Tscreen;

  if (arg.size() > 4) {
    arg = arg.view(4, arg.size());
    if (!arg.starts_with("twin")) {
      return false; /* user said "use <arg> as display, not libtw" */
    }
    // skip "twin"
    arg = arg.view(4, arg.size());

    if (arg.contains(Chars(",noinput"))) {
      HW->FlagsHW |= FlHWNoInput;
    }
    if (arg.contains(Chars(",slow"))) {
      HW->FlagsHW |= FlHWExpensiveFlushVideo;
    }
    if (arg.contains(Chars(",help"))) {
      log(INFO, "   --hw=twin options:\n"
                "      @TWDPY      connect to TWDPY instead of $TWDISPLAY (must be first option)\n"
                "      ,help       show this help\n"
                "      ,noinput    open a view-only window - ignore input\n"
                "      ,slow       assume connection is slow\n");
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
    log(ERROR, "      TW_InitHW() failed: TWDISPLAY is not set\n");
    return false;
  }

  if (!(HW->Private = (tw_data *)AllocMem(sizeof(tw_data)))) {
    log(ERROR, "      TW_InitHW(): Out of memory!\n");
    return false;
  }

#ifdef CONF__ALLOC
  Tw_ConfigMalloc(AllocMem, ReAllocMem, free);
#endif

  Td = NULL;

  if (Tw_CheckMagic(hw_twin_magic) && (Td = Tw_Open(arg.data())) &&

      /*
       * check if the server supports the functions we need and store their IDs
       * to avoid deadlocking later when we call them.
       */
      Tw_FindLFunction(Td, Tw_MapWidget, Tw_WriteCharsetWindow, Tw_WriteTCellWindow,
                       Tw_GotoXYWindow, Tw_ResizeWindow,
                       /* Tw_DragAreaWindow, */ NULL) &&

      (Tscreen = Tw_FirstScreen(Td)) && (Tmsgport = Tw_CreateMsgPort(Td, 12, "Twin on Twin")) &&
      (Tmenu = Tw_CreateMenu(Td, TCOL(tblack, twhite), TCOL(tblack, tgreen),
                             TCOL(thigh | tblack, twhite), TCOL(thigh | tblack, tblack),
                             TCOL(tred, twhite), TCOL(tred, tgreen), (byte)0)) &&
      Tw_Item4MenuCommon(Td, Tmenu)) {
    do {

      Tw_Info4Menu(Td, Tmenu, TW_ROW_ACTIVE, (uldat)14, " Twin on Twin ",
                   (const tcolor *)"ptppppppptpppp");

      sprintf(name + 5, "%s on twin", TWDisplay);
      len = strlen(name);

      Twin = Tw_CreateWindow(Td, len, name, NULL, Tmenu, TCOL(twhite, tblack), TW_LINECURSOR,
                             TW_WINDOW_WANT_KEYS | TW_WINDOW_WANT_MOUSE | TW_WINDOW_WANT_CHANGES |
                                 TW_WINDOW_DRAG | TW_WINDOW_RESIZE | TW_WINDOW_CLOSE,
                             TW_WINDOWFL_USECONTENTS | TW_WINDOWFL_CURSOR_ON,
                             (HW->X = GetDisplayWidth()), (HW->Y = GetDisplayHeight()), (uldat)0);

      if (!Twin)
        break;

      Tw_SetColorsWindow(Td, Twin, 0x1FF, TCOL(thigh | tyellow, tcyan),
                         TCOL(thigh | tgreen, thigh | tblue), TCOL(twhite, thigh | tblue),
                         TCOL(thigh | twhite, thigh | tblue), TCOL(thigh | twhite, thigh | tblue),
                         TCOL(twhite, tblack), TCOL(twhite, thigh | tblack),
                         TCOL(thigh | tblack, tblack), TCOL(tblack, thigh | tblack));
      Tw_MapWidget(Td, Twin, Tscreen);

      /*
       * NOT Tw_Sync(Td) as it might deadlock when
       * twin:x displays on twin:y which displays on twin:x
       */
      Tw_Flush(Td);

      TSelCount = SelCount = 0;

      HW->mouse_slot = NOSLOT;
      HW->keyboard_slot =
          RegisterRemote(Tw_ConnectionFd(Td), (obj)HW, (void (*)(int, obj))TW_KeyboardEvent);
      if (HW->keyboard_slot == NOSLOT)
        break;

      HW->FlushVideo = TW_FlushVideo;
      HW->FlushHW = TW_FlushHW;

      HW->KeyboardEvent = TW_KeyboardEvent;
      /* mouse events handled by TW_KeyboardEvent */
      HW->MouseEvent = (void (*)(int, display_hw))NoOp;

      HW->XY[0] = HW->XY[1] = 0;
      HW->TT = (uldat)-1; /* force updating cursor */

      HW->ShowMouse = NoOp;
      HW->HideMouse = NoOp;
      HW->UpdateMouseAndCursor = NoOp;
      HW->MouseState.x = HW->MouseState.y = HW->MouseState.keys = 0;

      HW->DetectSize = TW_DetectSize;
      HW->CheckResize = TW_CheckResize;
      HW->Resize = TW_Resize;

      HW->HWSelectionImport = TW_SelectionImport_TW;
      HW->HWSelectionExport = TW_SelectionExport_TW;
      HW->HWSelectionRequest = TW_SelectionRequest_TW;
      HW->HWSelectionNotify = TW_SelectionNotify_TW;
      HW->HWSelectionPrivate = 0;

      HW->CanDragArea = NULL;

      HW->Beep = TW_Beep;
      HW->Configure = TW_Configure;
      HW->SetPalette = (void (*)(udat, udat, udat, udat))NoOp;
      HW->ResetPalette = NoOp;

      HW->QuitHW = TW_QuitHW;
      HW->QuitKeyboard = NoOp;
      HW->QuitMouse = NoOp;
      HW->QuitVideo = NoOp;

      HW->DisplayIsCTTY = tfalse;
      HW->FlagsHW &= ~FlHWSoftMouse; /* mouse pointer handled by X11 server */

      HW->FlagsHW |= FlHWNeedOldVideo;
      HW->FlagsHW &= ~FlHWExpensiveFlushVideo;
      HW->NeedHW = 0;
      HW->CanResize = ttrue;
      HW->merge_Threshold = 0;

      /*
       * we must draw everything on our new shiny window
       * without forcing all other displays
       * to redraw everything too.
       */
      HW->RedrawVideo = tfalse;
      NeedRedrawVideo(0, 0, HW->X - 1, HW->Y - 1);

      return true;

    } while (0);
  } else {
    /* TwErrno(NULL) is valid... used when Tw_Open fails */
    if ((len = Tw_Errno(Td)))
      log(ERROR, "      TW_InitHW() failed: ", Chars::from_c(Tw_StrError(Td, len)), " ",
          Chars::from_c(Tw_StrErrorDetail(Td, len, Tw_ErrnoDetail(Td))), "\n");
    else
      log(ERROR, "      TW_InitHW() failed.\n");
  }

  if (Td && Tw_ConnectionFd(Td) >= 0)
    TW_QuitHW();

  return false;
}

EXTERN_C byte InitModule(module Module) {
  Module->DoInit = TW_InitHW;
  return ttrue;
}

/* this MUST be defined, or it seems that a bug in dlsym() gets triggered */
EXTERN_C void QuitModule(module Module) {
}
