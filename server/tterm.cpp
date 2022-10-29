/*
 *  tterm.c  --  create and manage multiple terminal emulator windows on twin
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "alloc.h"
#include "data.h"
#include "methods.h"
#include "obj/id.h" // Id2Obj()

#include "common.h"
#include "extreg.h"
#include "log.h"
#include "pty.h"
#include "remote.h"
#include "util.h"

#define COD_QUIT (udat)1
#define COD_SPAWN (udat)3

static Tmenu Term_Menu;

static char *default_args[3];

static Tmsgport Term_MsgPort;

static void TwinTermH(Tmsgport MsgPort);
static void TwinTermIO(int Fd, Twindow Window);

static void termShutDown(Twidget W) {
  Twindow Window;
  if (IS_WINDOW(W)) {
    Window = (Twindow)W;
    if (Window->RemoteData.Fd != NOFD)
      close(Window->RemoteData.Fd);
    UnRegisterWindowFdIO(Window);
  }
}

static Twindow newTermWindow(const char *title) {
  Twindow Window;

  Window = New(window)(Term_MsgPort, strlen(title), title, NULL, Term_Menu, TCOL(twhite, tblack),
                       LINECURSOR,
                       WINDOW_WANT_KEYS | WINDOW_DRAG | WINDOW_RESIZE | WINDOW_Y_BAR | WINDOW_CLOSE,
                       WINDOWFL_CURSOR_ON | WINDOWFL_USECONTENTS,
                       /*width*/ 80, /*height*/ 25, /*scrollbacklines*/ 1000);

  if (Window) {
    Window->SetColors(0x1FF, TCOL(thigh | tyellow, tcyan), TCOL(thigh | tgreen, thigh | tblue),
                      TCOL(twhite, thigh | tblue), TCOL(thigh | twhite, thigh | tblue),
                      TCOL(thigh | twhite, thigh | tblue), TCOL(twhite, tblack),
                      TCOL(thigh | tblack, thigh | twhite), TCOL(thigh | tblack, tblack),
                      TCOL(tblack, thigh | tblack));

    Window->Configure((1 << 2) | (1 << 3), 0, 0, 7, 3, 0, 0);
  }
  return Window;
}

static Twindow OpenTerm(const char *arg0, const char *const *argv) {
  Twindow Window;
  const char *title;

  /* if {arg0, argv} is {NULL, ...} or {"", ... } then start user's shell */
  if (arg0 && *arg0 && argv && argv[0]) {
    if ((title = strrchr(argv[0], '/')))
      title++;
    else
      title = argv[0];
  } else {
    arg0 = default_args[0];
    argv = (const char *const *)default_args + 1;

    title = "Twin Term";
  }

  if ((Window = newTermWindow(title))) {
    if (spawnInWindow(Window, arg0, argv)) {
      if (RegisterWindowFdIO(Window, TwinTermIO)) {
        Window->ShutDownHook = termShutDown;
        Window->Map((Twidget)All->FirstScreen);
        return Window;
      }
      close(Window->RemoteData.Fd);
    }
    Window->Delete();
  }
  return NULL;
}

static void TwinTermH(Tmsgport MsgPort) {
  Tmsg Msg;
  event_any *Event;
  udat Code /*, Repeat*/;
  Twindow Win;

  while ((Msg = Term_MsgPort->FirstMsg)) {
    Msg->Remove();

    Event = &Msg->Event;
    Win = (Twindow)Event->EventSelection.W;
    if (Win && !IS_WINDOW(Win))
      Win = NULL;

    if (Msg->Type == msg_widget_key) {
      Code = Event->EventKeyboard.Code;
      /* send keypresses */
      if (Win)
        (void)RemoteWindowWriteQueue(Win, Event->EventKeyboard.SeqLen,
                                     Event->EventKeyboard.AsciiSeq);
    } else if (Msg->Type == msg_selection) {

      TwinSelectionRequest((Tobj)Term_MsgPort, Win->Id, TwinSelectionGetOwner());

    } else if (Msg->Type == msg_selection_notify) {

      if ((Win = (Twindow)Id2Obj(Twindow_magic_byte, Event->EventSelectionNotify.ReqPrivate))) {
        (void)RemoteWindowWriteQueue(Win, Event->EventSelectionNotify.Len,
                                     Event->EventSelectionNotify.Data);
      }
    } else if (Msg->Type == msg_widget_mouse) {
      if (Win) {
        char buf[10];
        byte len = CreateXTermMouseEvent(&Event->EventMouse, 10, buf);

        /* send mouse movements using xterm mouse protocol */
        if (len)
          (void)RemoteWindowWriteQueue(Win, len, buf);
      }
    } else if (Msg->Type == msg_widget_gadget) {
      if (Win && Event->EventGadget.Code == 0 /* Close Code */) {
        Win->Delete();
      }
    } else if (Msg->Type == msg_menu_row) {
      if (Event->EventMenu.Menu == Term_Menu) {
        Code = Event->EventMenu.Code;
        switch (Code) {
        case COD_SPAWN:
          OpenTerm(NULL, NULL);
          break;
        default:
          break;
        }
      }
    } else if (Msg->Type == msg_user_control) {
      /* this duplicates the same functionality of builtin.c */
      if (Event->EventControl.Code == MSG_CONTROL_OPEN) {
        char **cmd = TokenizeStringVec(Event->EventControl.Len, Event->EventControl.Data);
        if (cmd) {
          OpenTerm(cmd[0], (const char *const *)cmd);
          FreeStringVec(cmd);
        } else
          OpenTerm(NULL, NULL);
      }
    }
    Msg->Delete();
  }
}

static void TwinTermIO(int Fd, Twindow Window) {
  static char buf[TW_BIGBUFF];
  uldat got = 0, chunk = 0;

  do {
    /*
     * TW_BIGBUFF - 1 to avoid silly windows...
     * linux ttys buffer up to 4095 bytes.
     */
    chunk = read(Fd, buf + got, TW_BIGBUFF - 1 - got);
  } while (chunk && chunk != (uldat)-1 && (got += chunk) < TW_BIGBUFF - 1);

  if (got) {
    Window->TtyWriteCharset(got, buf);
  } else if (chunk == (uldat)-1 && errno != EINTR && errno != EWOULDBLOCK) {
    /* something bad happened to our child :( */
    Window->Delete();
  }
}

#include "tty.h"

static void OverrideMethods(bool enter) {
  if (enter) {
    OverrideMethod(widget, KbdFocus, FakeKbdFocus, TtyKbdFocus);
    OverrideMethod(gadget, KbdFocus, FakeKbdFocus, TtyKbdFocus);
    OverrideMethod(window, KbdFocus, FakeKbdFocus, TtyKbdFocus);
    OverrideMethod(window, TtyWriteCharset, FakeWriteCharset, TtyWriteCharset);
    OverrideMethod(window, TtyWriteUtf8, FakeWriteUtf8, TtyWriteUtf8);
    OverrideMethod(window, TtyWriteTRune, FakeWriteTRune, TtyWriteTRune);
    OverrideMethod(window, TtyWriteTCell, FakeWriteTCell, TtyWriteTCell);
    ForceKbdFocus();
  } else {
    OverrideMethod(window, TtyWriteTCell, TtyWriteTCell, FakeWriteTCell);
    OverrideMethod(window, TtyWriteTRune, TtyWriteTRune, FakeWriteTRune);
    OverrideMethod(window, TtyWriteUtf8, TtyWriteUtf8, FakeWriteUtf8);
    OverrideMethod(window, TtyWriteCharset, TtyWriteCharset, FakeWriteCharset);
    OverrideMethod(window, KbdFocus, TtyKbdFocus, FakeKbdFocus);
    OverrideMethod(gadget, KbdFocus, TtyKbdFocus, FakeKbdFocus);
    OverrideMethod(widget, KbdFocus, TtyKbdFocus, FakeKbdFocus);
  }
}

EXTERN_C byte InitModule(Tmodule Module) {
  Twindow Window;
  const char *shellpath, *shell;

  if (((shellpath = getenv("SHELL")) || (shellpath = "/bin/sh")) &&
      (default_args[0] = CloneStr(shellpath)) &&
      (default_args[1] =
           (shell = strrchr(shellpath, '/')) ? CloneStr(shell) : CloneStr(shellpath)) &&

      (Term_MsgPort = New(msgport)(14, "builtin twterm", (uldat)0, (udat)0, (byte)0, TwinTermH)) &&
      (Term_Menu = New(menu)(Term_MsgPort, TCOL(tblack, twhite), TCOL(tblack, tgreen),
                             TCOL(thigh | tblack, twhite), TCOL(thigh | tblack, tblack),
                             TCOL(tred, twhite), TCOL(tred, tgreen), (byte)0)) &&
      Info4Menu(Term_Menu, ROW_ACTIVE, (uldat)19, " Builtin Twin Term ",
                (const tcolor *)"ptppppppptpppptpppp") &&

      (Window = Win4Menu(Term_Menu)) && Row4Menu(Window, COD_SPAWN, ROW_ACTIVE, 10, " New Term ") &&
      Row4Menu(Window, COD_QUIT, tfalse, 6, " Exit ") &&
      Item4Menu(Term_Menu, Window, ttrue, 6, " File ") &&

      Item4MenuCommon(Term_Menu)) {

    RegisterExt(Term, Open, OpenTerm);
    OverrideMethods(true);

    if (default_args[1][0] == '/')
      default_args[1][0] = '-';
    return ttrue;
  }
  if (shellpath)
    log(ERROR) << "twin: InitTerm(): " << Errstr << "\n";
  else
    log(ERROR) << "twin: required environment variable $SHELL not set!\n";
  return tfalse;
}

EXTERN_C void QuitModule(Tmodule Module) {
  UnRegisterExt(Term, Open, OpenTerm);
  OverrideMethods(false);
  if (Term_MsgPort) {
    Term_MsgPort->Delete();
  }
}
