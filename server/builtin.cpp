/*
 *  builtin.c  --  create default menu, Clock and About windows.
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
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
#include "main.h"

#include "builtin.h"
#include "extreg.h"
#include "dl.h"
#include "draw.h"
#include "common.h"
#include "hw.h"
#include "log.h"
#include "hw_multi.h"
#include "obj/id.h"
#include "resize.h"
#include "log.h"
#include "util.h"
#include "version.h"

#include <Tw/Twkeys.h>
#include <Tw/autoconf.h>

#include <Tutf/Tutf.h>
#include <Tutf/Tutf_defs.h>

#ifdef TW_HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef TW_HAVE_SIGNAL_H
#include <signal.h>
#endif

enum {
  _CHECK = T_UTF_32_CHECK_MARK,
  _FULL = T_UTF_32_FULL_BLOCK,
  _LOWER = T_UTF_32_LOWER_HALF_BLOCK,
  _UPPER = T_UTF_32_UPPER_HALF_BLOCK,
  _MEDIUM_SHADE = T_UTF_32_MEDIUM_SHADE,
};

enum {
  COD_QUIT = 1,  /* as in term.c */
  COD_SPAWN = 3, /* as in term.c */

  COD_EXECUTE = 5,
  COD_SUSPEND = 10,
  COD_DETACH = 11,
  COD_RELOAD_RC = 12,
  COD_COPY = 13,
  COD_PASTE = 14,

  COD_CLOCK_WIN = 20,
  COD_OPTION_WIN = 21,
  COD_BUTTONS_WIN = 22,
  COD_DISPLAY_WIN = 23,
  COD_MESSAGES_WIN = 24,
  COD_ABOUT_WIN = 25,

  COD_TERM_ON = 30,
  COD_TERM_OFF = 31,
  COD_SOCKET_ON = 32,
  COD_SOCKET_OFF = 33,

  COD_O_SHADOWS = 40,
  COD_O_Xp_SHADE = 41,
  COD_O_Xn_SHADE = 42,
  COD_O_Yp_SHADE = 43,
  COD_O_Yn_SHADE = 44,
  COD_O_CURSOR_ALWAYS = 46,
  COD_O_MENU_HIDE = 47,
  COD_O_MENU_INFO = 48,
  COD_O_MENU_RELAX = 49,
  COD_O_SCREEN_SCROLL = 50,
  COD_O_TERMINALS_UTF8 = 51,

  COD_D_REMOVE = 60,
  COD_D_THIS = 61,

  COD_E_TTY = 70,
};

Tmsgport Builtin_MsgPort;

static Tmenu Builtin_Menu;
static Tmenuitem Builtin_File;
static Tmenuitem Builtin_Modules;

static Twindow AboutWin, ClockWin, OptionWin, ButtonWin, DisplayWin, DisplaySubWin, ExecuteWin;

Twindow WinList, MessagesWin;

static Tgadget ButtonOK_About, ButtonRemove, ButtonThis;

static void Clock_Update(void) {
  time_t Time = (time_t)All->Now.Seconds;
  struct tm *Date;
  char Buffer[30];

  ClockWin->CurX = ClockWin->CurY = (uldat)0;
  Date = localtime(&Time);

  snprintf(Buffer, sizeof(Buffer), "%02hu/%02hu/%04hu\n %02hu:%02hu:%02hu", (udat)Date->tm_mday,
           (udat)(Date->tm_mon + 1), (udat)(Date->tm_year + 1900), (udat)Date->tm_hour,
           (udat)Date->tm_min, (udat)Date->tm_sec);
  ClockWin->RowWriteCharset(strlen(Buffer), Buffer);

  Builtin_MsgPort->PauseDuration.Fraction = 1 FullSECs - All->Now.Fraction;
  Builtin_MsgPort->PauseDuration.Seconds = 0;
}

static void TweakMenuRows(Tmenuitem item, udat code, byte flag) {
  Twindow win;
  Trow row;

  if ((win = item->Window) && (row = win->FindRowByCode(code, (ldat *)0)))
    row->Flags = flag;
}

static void UpdateMenuRows(Twidget dummy) {
  (void)dummy;
  if (DlIsLoaded(TermSo)) {
    TweakMenuRows(Builtin_Modules, COD_TERM_ON, ROW_INACTIVE);
    TweakMenuRows(Builtin_Modules, COD_TERM_OFF, ROW_ACTIVE);
  } else {
    TweakMenuRows(Builtin_Modules, COD_TERM_ON, ROW_ACTIVE);
    TweakMenuRows(Builtin_Modules, COD_TERM_OFF, ROW_INACTIVE);
  }
  if (DlIsLoaded(SocketSo)) {
    TweakMenuRows(Builtin_Modules, COD_SOCKET_ON, ROW_INACTIVE);
    TweakMenuRows(Builtin_Modules, COD_SOCKET_OFF, ROW_ACTIVE);
  } else {
    TweakMenuRows(Builtin_Modules, COD_SOCKET_ON, ROW_ACTIVE);
    TweakMenuRows(Builtin_Modules, COD_SOCKET_OFF, ROW_INACTIVE);
  }
}

static void SelectWinList(void) {
  Tscreen screen = All->Screens.First;
  uldat n = WinList->CurY;
  Twidget w;
  Twindow win = (Twindow)0;

  for (w = screen->Widgets.First; w; w = w->Next) {
    if (w == (Twidget)WinList || !IS_WINDOW(w) ||
        (((Twindow)w)->Flags & (WINDOWFL_NOTVISIBLE | WINDOWFL_MENU))) {
      continue;
    }
    win = (Twindow)w;
    if (!n) {
      break;
    }
    n--;
  }
  if (!n && win) {
    RaiseWidget(win, true);
    CenterWindow(win);
  }
}

static void ExecuteGadgetH(event_gadget *event_g) {
  Tgadget g;

  if (event_g->Code == COD_E_TTY && (g = ExecuteWin->FindGadgetByCode(COD_E_TTY))) {

    if (g->USE.T.Text[0][1] == ' ')
      g->USE.T.Text[0][1] = _CHECK;
    else
      g->USE.T.Text[0][1] = ' ';

    DrawAreaWidget(g);
  }
}

static void ExecuteWinRun(void) {
  char **argv, *arg0;
  Trow row;
  Tgadget G;

  ExecuteWin->UnMap();

  if (flag_secure) {
    log(ERROR) << flag_secure_msg;
    return;
  }

  if ((row = ExecuteWin->FindRow(ExecuteWin->CurY)) && !row->LenGap) {

    argv = TokenizeTRuneVec(row->Len, row->Text);
    arg0 = argv ? argv[0] : NULL;

    if ((G = ExecuteWin->FindGadgetByCode(COD_E_TTY)) && G->USE.T.Text[0][1] != ' ') {
      /* run in a tty */
      Ext(Term, Open)(arg0, (const char *const *)argv);
    } else if (argv)
      switch (fork()) {
        /* do not run in a tty */
      case -1: /* error */
        break;
      case 0: { /* child */
        int fd = open("/dev/null", O_RDWR);
        closeAllFds(fd >= 0 ? fd : 1);
        (void)setsid();
        AllDefaultSignals();
        execvp(arg0, argv);
        exit(1);
        break;
      }
      default: /* parent */
        break;
      }
    if (argv)
      FreeStringVec(argv);
  }
}

void UpdateOptionWin(void) {
  Tgadget G;
  udat list[] = {COD_O_Xp_SHADE, COD_O_Xn_SHADE, COD_O_Yp_SHADE, COD_O_Yn_SHADE, 0};
  byte i, Flags = All->SetUp->Flags;
  char ch;

  for (i = 0; list[i]; i++) {
    if ((G = OptionWin->FindGadgetByCode(list[i]))) {
      if (Flags & setup_shadows)
        G->Flags &= ~GADGETFL_DISABLED;
      else
        G->Flags |= GADGETFL_DISABLED;
    }
  }
  if ((G = OptionWin->FindGadgetByCode(COD_O_SHADOWS)))
    G->USE.T.Text[0][1] = Flags & setup_shadows ? _CHECK : ' ';
  if ((G = OptionWin->FindGadgetByCode(COD_O_CURSOR_ALWAYS)))
    G->USE.T.Text[0][1] = Flags & setup_cursor_always ? _CHECK : ' ';
  if ((G = OptionWin->FindGadgetByCode(COD_O_MENU_HIDE)))
    G->USE.T.Text[0][1] = Flags & setup_menu_hide ? _CHECK : ' ';
  if ((G = OptionWin->FindGadgetByCode(COD_O_MENU_INFO)))
    G->USE.T.Text[0][1] = Flags & setup_menu_info ? _CHECK : ' ';
  if ((G = OptionWin->FindGadgetByCode(COD_O_MENU_RELAX)))
    G->USE.T.Text[0][1] = Flags & setup_menu_relax ? _CHECK : ' ';
  if ((G = OptionWin->FindGadgetByCode(COD_O_SCREEN_SCROLL)))
    G->USE.T.Text[0][1] = Flags & setup_screen_scroll ? _CHECK : ' ';
  if ((G = OptionWin->FindGadgetByCode(COD_O_TERMINALS_UTF8)))
    G->USE.T.Text[0][1] = Flags & setup_terminals_utf8 ? _CHECK : ' ';

  OptionWin->CurX = 25;
  OptionWin->CurY = 1;
  ch = (Flags & setup_shadows ? All->SetUp->DeltaXShade : 0) + '0';
  OptionWin->RowWriteCharset(1, &ch);
  OptionWin->CurX = 25;
  OptionWin->CurY = 2;
  ch = (Flags & setup_shadows ? All->SetUp->DeltaYShade : 0) + '0';
  OptionWin->RowWriteCharset(1, &ch);
}

static void OptionH(Tmsg msg) {
  byte Flags = All->SetUp->Flags, XShade = All->SetUp->DeltaXShade,
       YShade = All->SetUp->DeltaYShade;
  byte redraw = ttrue;

  switch (msg->Event.EventGadget.Code) {
  case COD_O_SHADOWS:
    Flags ^= setup_shadows;
    break;
  case COD_O_Xp_SHADE:
    if (XShade < max_xshade)
      XShade++;
    break;
  case COD_O_Xn_SHADE:
    if (XShade > 1)
      XShade--;
    break;
  case COD_O_Yp_SHADE:
    if (YShade < max_yshade)
      YShade++;
    break;
  case COD_O_Yn_SHADE:
    if (YShade > 1)
      YShade--;
    break;
  case COD_O_CURSOR_ALWAYS:
    Flags ^= setup_cursor_always;
    redraw = tfalse;
    break;
  case COD_O_MENU_HIDE:
    Flags ^= setup_menu_hide;
    HideMenu(!!(Flags & setup_menu_hide));
    redraw = tfalse;
    break;
  case COD_O_MENU_INFO:
    Flags ^= setup_menu_info;
    break;
  case COD_O_MENU_RELAX:
    Flags ^= setup_menu_relax;
    break;
  case COD_O_SCREEN_SCROLL:
    Flags ^= setup_screen_scroll;
    redraw = tfalse;
    break;
  case COD_O_TERMINALS_UTF8:
    Flags ^= setup_terminals_utf8;
    redraw = tfalse;
    break;
  default:
    redraw = tfalse;
    break;
  }
  if (Flags != All->SetUp->Flags || XShade != All->SetUp->DeltaXShade ||
      YShade != All->SetUp->DeltaYShade) {

    All->SetUp->Flags = Flags;
    All->SetUp->DeltaXShade = XShade;
    All->SetUp->DeltaYShade = YShade;

    UpdateOptionWin();
    if (redraw == ttrue)
      QueuedDrawArea2FullScreen = true;
    else {
      DrawFullWindow(OptionWin);
      UpdateCursor();
    }
  }
}

void FillButtonWin(void) {
  dat i, j;
  char b[] = "      ";
  const char *s;

  DeleteList(ButtonWin->Widgets.First);

  for (i = j = 0; j < BUTTON_MAX; j++) {
    if (All->ButtonVec[j].exists)
      i++;
  }
  ResizeRelWindow(ButtonWin, 0, (dat)(3 + i * 2) - (dat)ButtonWin->YWidth);

  /* clear the Twindow: */
  ButtonWin->TtyWriteCharset(4, "\033[2J");

  for (j = BUTTON_MAX - 1; j >= 0; j--) {
    if (!All->ButtonVec[j].exists)
      continue;
    i--;

    ButtonWin->GotoXY(2, 1 + i * 2);
    if (j)
      b[2] = j + '0', s = b;
    else
      s = "Close ";
    ButtonWin->TtyWriteCharset(7, "Button ");
    ButtonWin->TtyWriteCharset(6, s);
    {
      trune *f = All->ButtonVec[j].shape;
      tcell h[2] = {
          TCELL(ButtonWin->ColGadgets, f[0]),
          TCELL(ButtonWin->ColGadgets, f[1]),
      };

      ButtonWin->TtyWriteTCell(15, 1 + i * 2, 2, h);
    }
    Sgadget::Create(Builtin_MsgPort, ButtonWin, 3, 1, "[+]", 0, GADGETFL_TEXT_DEFCOL, 3 | (j << 2),
                    TCOL(tblack, twhite), TCOL(tWHITE, tgreen), TCOL(tBLACK, twhite),
                    TCOL(tBLACK, tblack), 22, 1 + i * 2);
    Sgadget::Create(Builtin_MsgPort, ButtonWin, 3, 1, "[-]", 0, GADGETFL_TEXT_DEFCOL, 2 | (j << 2),
                    TCOL(tblack, twhite), TCOL(tWHITE, tgreen), TCOL(tBLACK, twhite),
                    TCOL(tBLACK, tblack), 19, 1 + i * 2);
  }
}

void UpdateButtonWin(void) {
  dat i, j;
  char s[5];
  sbyte pos;

  for (i = j = 0; j < BUTTON_MAX; j++) {
    if (All->ButtonVec[j].exists)
      i++;
  }
  for (j = BUTTON_MAX - 1; j >= 0; j--) {
    if (!All->ButtonVec[j].exists) {
      continue;
    }
    i--;

    ButtonWin->GotoXY(26, 1 + i * 2);

    pos = All->ButtonVec[j].pos;
    if (pos >= 0) {
      ButtonWin->TtyWriteCharset(5, "Left ");
    } else if (pos == -1)
      ButtonWin->TtyWriteCharset(9, "Disabled ");
    else {
      ButtonWin->TtyWriteCharset(5, "Right");
      pos = -pos - 2;
    }
    if (pos >= 0) {
      snprintf(s, sizeof(s), " %3d", pos);
      ButtonWin->TtyWriteCharset(strlen(s), s);
    }
  }
}

static void BordersH(Tmsg msg) {
  udat Code = msg->Event.EventGadget.Code;
  sbyte op = -1;

  if (!(Code & 2))
    return;

  if (Code & 1)
    op = +1;

  All->ButtonVec[Code >> 2].pos += op;

  QueuedDrawArea2FullScreen = true;
  UpdateButtonWin();
}

static void UpdateDisplayWin(Twidget displayWin) {
  Tdisplay hw;
  uldat x = 12, y = 0;

  if (displayWin == (Twidget)DisplayWin) {
    DeleteList(DisplayWin->USE.R.Rows.First);

    for (hw = All->Displays.First; hw; hw = hw->Next) {
      DisplayWin->GotoXY(x, y++);
      if (!hw->Name)
        DisplayWin->RowWriteCharset(9, "(no name)");
      else
        DisplayWin->RowWriteCharset(hw->Name.size(), hw->Name.data());
    }
    if (DisplayWin->Parent)
      DrawFullWindow(DisplayWin);
  }
}

static void SelectRowWindow(Twindow CurrWin, ldat newCurY) {
  ldat oldCurY = CurrWin->CurY;

  CurrWin->CurY = newCurY;

  if (oldCurY != newCurY) {
    DrawLogicWidget((Twidget)CurrWin, 0, oldCurY, CurrWin->XWidth + CurrWin->XLogic, oldCurY);
    DrawLogicWidget((Twidget)CurrWin, 0, newCurY, CurrWin->XWidth + CurrWin->XLogic, newCurY);
  }
}

static void DisplayGadgetH(Tmsg msg) {
  Tdisplay hw;
  ldat i;

  switch (msg->Event.EventGadget.Code) {
  case COD_D_REMOVE:
    if ((i = DisplayWin->CurY) < DisplayWin->HLogic) {
      for (hw = All->Displays.First; hw && i; hw = hw->Next, i--) {
      }
      if (hw && !i) {
        hw->Delete();
      }
    }
    break;
  case COD_D_THIS:
    if (All->MouseDisplay) {
      for (i = 0, hw = All->Displays.First; hw; hw = hw->Next, i++) {
        if (hw == All->MouseDisplay)
          break;
      }
      if (hw)
        SelectRowWindow(DisplayWin, i);
    }
    break;
  default:
    break;
  }
}

static void BuiltinH(Tmsgport msgport) {
  Tmsg msg;
  event_any *Event;
  Tscreen screen = All->Screens.First;
  Twindow newWin = (Twindow)0, tempWin;
  Trow row;
  udat Code;

  (void)msgport;

  while ((msg = Builtin_MsgPort->Msgs.First)) {
    msg->Remove();
    Event = &msg->Event;

    switch (msg->Type) {
    case msg_widget_gadget:
      tempWin = (Twindow)Event->EventGadget.W;
      Code = Event->EventGadget.Code;
      /*0 == Code Chiusura */
      if (!Code || Code == COD_CANCEL || Code == COD_OK) {

        tempWin->UnMap();
        /* no Twindow needs Delete() here */

        if (tempWin == ClockWin)
          Builtin_MsgPort->WakeUp = tfalse;

      } else if (tempWin == OptionWin)
        OptionH(msg);
      else if (tempWin == ButtonWin)
        BordersH(msg);
      else if (tempWin == DisplaySubWin)
        DisplayGadgetH(msg);
      else if (tempWin == ExecuteWin)
        ExecuteGadgetH(&Event->EventGadget);
      break;

    case msg_menu_row:
      if (Event->EventMenu.Menu == Builtin_Menu) {
        Code = Event->EventMenu.Code;
        switch (Code) {
        case COD_EXECUTE:
        case COD_CLOCK_WIN:
        case COD_OPTION_WIN:
        case COD_BUTTONS_WIN:
        case COD_DISPLAY_WIN:
        case COD_MESSAGES_WIN:
        case COD_ABOUT_WIN:
          switch (Code) {
          case COD_EXECUTE:
            newWin = ExecuteWin;
            break;
          case COD_CLOCK_WIN:
            Builtin_MsgPort->WakeUp = TIMER_ALWAYS;
            newWin = ClockWin;
            break;
          case COD_OPTION_WIN:
            UpdateOptionWin();
            newWin = OptionWin;
            break;
          case COD_BUTTONS_WIN:
            UpdateButtonWin();
            newWin = ButtonWin;
            break;
          case COD_DISPLAY_WIN:
            UpdateDisplayWin((Twidget)DisplayWin);
            newWin = DisplayWin;
            break;
          case COD_MESSAGES_WIN:
            newWin = MessagesWin;
            break;
          case COD_ABOUT_WIN:
            newWin = AboutWin;
            break;
          default:
            break;
          }
          if (newWin->Parent)
            newWin->UnMap();
          newWin->Map((Twidget)screen);
          break;

        case COD_QUIT:
          Quit(0);
          break;

        case COD_SUSPEND:
          SuspendHW(true);
          flushk();

          kill(getpid(), SIGSTOP);

          (void)RestartHW(true);
          break;

        case COD_DETACH:
          QuitHW();
          break;

        case COD_COPY:
          All->Clipboard->dup(All->Selection);
          break;

        case COD_PASTE:
          All->Clipboard->paste();
          break;

        case COD_RELOAD_RC:
          SendControlMsg(Ext(WM, MsgPort), MSG_CONTROL_RESTART, 0, NULL);
          break;

        case COD_TERM_ON:
          if (!DlLoad(TermSo))
            break;
          /* FALLTHROUGH */

        case COD_SPAWN:
          Ext(Term, Open)(NULL, NULL);
          break;

        case COD_TERM_OFF:
          DlUnload(TermSo);
          break;

        case COD_SOCKET_OFF:
          DlUnload(SocketSo);
          if (All->Displays.First)
            break;
          /* hmm... better to fire it up again */
          /* FALLTHROUGH */
        case COD_SOCKET_ON:
          if (!DlLoad(SocketSo))
            break;
          break;

        default:
          break;
        }
      }
      break;

    case msg_widget_key:
      tempWin = (Twindow)msg->Event.EventCommon.W;
      if (tempWin == WinList) {
        switch (msg->Event.EventKeyboard.Code) {
        case TW_Escape:
          WinList->UnMap();
          break;
        case TW_Return:
          SelectWinList();
          break;
        default:
          FallBackKeyAction(WinList, &msg->Event.EventKeyboard);
          break;
        }
      } else if (tempWin == ExecuteWin) {
        switch ((Code = msg->Event.EventKeyboard.Code)) {
        case TW_Escape:
          ExecuteWin->UnMap();
          break;
        case TW_BackSpace:
          if (ExecuteWin->CurX) {
            ExecuteWin->CurX--;
            UpdateCursor();
            if ((row = ExecuteWin->FindRow(ExecuteWin->CurY)) && row->Len) {
              row->Len--;
              DrawLogicWidget((Twidget)ExecuteWin, row->Len, ExecuteWin->CurY, row->Len + 1,
                              ExecuteWin->CurY);
            }
          }
          break;
        default:
          if (Code == TW_Return || Code == TW_KP_Enter ||
              (msg->Event.EventKeyboard.SeqLen == 1 &&
               ((Code = msg->Event.EventKeyboard.AsciiSeq[0]) == 10 /* CTRL+J */ ||
                Code == 13 /* CTRL+M */)))
            ExecuteWinRun();
          else if (msg->Event.EventKeyboard.SeqLen)
            ExecuteWin->RowWriteCharset(msg->Event.EventKeyboard.SeqLen,
                                        msg->Event.EventKeyboard.AsciiSeq);
          break;
        }
      }
      break;

    case msg_widget_mouse:
      tempWin = (Twindow)msg->Event.EventCommon.W;

      if (tempWin == WinList || tempWin == DisplayWin) {
        dat EventMouseX, EventMouseY;
        byte temp;

        EventMouseX = msg->Event.EventMouse.X, EventMouseY = msg->Event.EventMouse.Y;
        temp = EventMouseX >= 0 && EventMouseX <= tempWin->XWidth - 2 && EventMouseY >= 0 &&
               EventMouseY <= tempWin->YWidth - 2 &&
               (uldat)EventMouseY + tempWin->YLogic < (uldat)tempWin->HLogic;

        SelectRowWindow(tempWin, temp ? (uldat)EventMouseY + tempWin->YLogic : TW_MAXLDAT);

        if (tempWin == WinList && isRELEASE(msg->Event.EventMouse.Code)) {
          if (temp)
            SelectWinList();
        }
      }
      break;

    case msg_selection:
      /* user wants to paste. ask for selection contents */
      if (msg->Event.EventSelection.W == (Twidget)ExecuteWin)
        TwinSelectionRequest((Tobj)Builtin_MsgPort, ExecuteWin->Id, TwinSelectionGetOwner());
      break;

    case msg_selection_notify:
      tempWin = (Twindow)Id2Obj(Twindow_class_byte, msg->Event.EventSelectionNotify.ReqPrivate);
      if (tempWin && tempWin == ExecuteWin) {
        switch (msg->Event.EventSelectionNotify.Magic) {
        case SEL_UTF8MAGIC: {
          const Chars utf8_bytes = msg->Event.EventSelectionNotify.Data();
          tempWin->RowWriteUtf8(utf8_bytes.size(), utf8_bytes.data());
          break;
        }
        default:
          break;
        }
      }
      break;

    case msg_user_control:
      switch (Event->EventControl.Code) {
      case MSG_CONTROL_OPEN: {
        char **cmd = TokenizeStringVec(Event->EventControl.Len, Event->EventControl.Data);
        if (cmd) {
          Ext(Term, Open)(cmd[0], (const char *const *)cmd);
          FreeStringVec(cmd);
        } else
          Ext(Term, Open)(NULL, NULL);
        break;
      }
      case MSG_CONTROL_QUIT:
        Quit(0);
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
  if (Builtin_MsgPort->WakeUp)
    Clock_Update();
}

void FullUpdateWinList(Twidget listWin);

void InstallRemoveWinListHook(Twidget listWin) {
  if (listWin == (Twidget)WinList) {
    if (WinList->Parent && IS_SCREEN(WinList->Parent))
      WinList->InstallHook(FullUpdateWinList, &((Tscreen)WinList->Parent)->HookMap);
    else
      WinList->RemoveHook(FullUpdateWinList, WinList->WhereHook);
  }
}

void UpdateWinList(void) {
  Tscreen screen = All->Screens.First;
  Twidget w;

  DeleteList(WinList->USE.R.Rows.First);
  WinList->CurX = WinList->CurY = 0;

  WinList->XLogic = WinList->YLogic = 0;
  WinList->XWidth = WinList->MinXWidth;
  WinList->YWidth = WinList->MinYWidth;

  for (w = screen->Widgets.First; w; w = w->Next) {
    if (w == (Twidget)WinList || !IS_WINDOW(w) ||
        (((Twindow)w)->Flags & (WINDOWFL_NOTVISIBLE | WINDOWFL_MENU)))
      continue;
    (void)Row4Menu(WinList, (udat)0, ROW_ACTIVE, ((Twindow)w)->NameLen, ((Twindow)w)->Name);
  }
}

void FullUpdateWinList(Twidget listWin) {
  if (listWin == (Twidget)WinList && WinList->Parent) {
    ResizeRelWindow(WinList, WinList->MinXWidth - WinList->XWidth,
                    WinList->MinYWidth - WinList->YWidth);

    UpdateWinList();

    DrawAreaWindow(WinList);
  }
}

#ifdef CONF_PRINTK
static byte InitMessagesWin(void) {
  MessagesWin = Swindow::Create(
      Builtin_MsgPort, 8, "Messages", NULL, Builtin_Menu, TCOL(tblack, twhite), LINECURSOR,
      WINDOW_DRAG | WINDOW_RESIZE | WINDOW_X_BAR | WINDOW_Y_BAR | WINDOW_CLOSE, WINDOWFL_CURSOR_ON,
      60, 20, 200);
  if (MessagesWin) {
    MessagesWin->SetColors(0x1F1, TCOL(tGREEN, twhite), TCOL0, TCOL0, TCOL0, TCOL(tWHITE, twhite),
                           TCOL(tblack, twhite), TCOL(tblack, tgreen), TCOL(tBLACK, twhite),
                           TCOL(tBLACK, tblack));
  }
  return !!MessagesWin;
}
#endif

static bool InitScreens(void) {
  Tscreen oneScreen;

  if ((oneScreen = Sscreen::CreateSimple(1, "1", TCELL(TCOL(tBLACK, tblue), _MEDIUM_SHADE)))) {

    InsertLast(Screens, oneScreen, All);
    return true;
  }
  Error(NOMEMORY);
  log(ERROR) << "twin: InitScreens(): " << Errstr << "\n";
  return false;
}

const tcolor *ColorFill(tcolor *array, uldat len, tcolor fill) {
  for (uldat i = 0; i < len; i++) {
    array[i] = fill;
  }
  return array;
}

bool InitBuiltin(void) {
  Twindow w;
  const char *greeting = "\n"
                         "                TWIN              \n"
                         "     Text WINdows manager " TWIN_VERSION_STR TWIN_VERSION_EXTRA_STR "\n"
                         "        (UTF-32, truecolor)       \n\n"
                         "      by Massimiliano Ghilardi    \n\n"
                         "  https://github.com/cosmos72/twin";
  uldat grlen = strlen(greeting);

  tcolor color_array[42];

  if ((Builtin_MsgPort = Smsgport::Create(4, "twin", 0, 0, 0, BuiltinH)) &&

      InitScreens() && /* Sscreen::Create() requires Builtin_MsgPort ! */

      (All->BuiltinRow = Srow::Create(0, ROW_ACTIVE | ROW_DEFCOL)) &&

      (Builtin_Menu = Smenu::Create(Builtin_MsgPort, TCOL(tblack, twhite), TCOL(tblack, tgreen),
                                    TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), TCOL(tred, twhite),
                                    TCOL(tred, tgreen), TCOL0)) &&
      Builtin_Menu->SetInfo(ROW_ACTIVE, (uldat)42, " Hit PAUSE or Mouse Right Button for Menu ",
                            ColorFill(color_array, 42, TCOL(tred, twhite))) &&

      (w = Swindow::Create4Menu(Builtin_Menu)) &&
      Row4Menu(w, COD_CLOCK_WIN, ROW_ACTIVE, 9, " Clock   ") &&
      Row4Menu(w, COD_OPTION_WIN, ROW_ACTIVE, 9, " Options ") &&
      Row4Menu(w, COD_BUTTONS_WIN, ROW_ACTIVE, 9, " Buttons ") &&
      Row4Menu(w, COD_DISPLAY_WIN, ROW_ACTIVE, 9, " Display ") &&
#ifdef CONF_PRINTK
      Row4Menu(w, COD_MESSAGES_WIN, ROW_ACTIVE, 10, " Messages ") &&
#endif
      Row4Menu(w, COD_ABOUT_WIN, ROW_ACTIVE, 9, " About   ") &&
      Smenuitem::Create4Menu(Builtin_Menu, w, 0, ttrue, 3, " \xF0 ") &&

      (w = Swindow::Create4Menu(Builtin_Menu)) &&
      Row4Menu(w, COD_SPAWN, ROW_ACTIVE, 10, " New Term ") &&
      Row4Menu(w, COD_EXECUTE, ROW_ACTIVE, 10, " Execute  ") &&
      Row4Menu(w, COD_RELOAD_RC, ROW_ACTIVE, 11, " Reload RC ") &&
      Row4Menu(w, (udat)0, ROW_IGNORE, 11, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4") &&
      Row4Menu(w, COD_DETACH, ROW_ACTIVE, 10, " Detach   ") &&
      Row4Menu(w, COD_SUSPEND, ROW_ACTIVE, 10, " Suspend  ") &&
      Row4Menu(w, COD_QUIT, ROW_ACTIVE, 10, " Quit     ") &&
      (Builtin_File = Smenuitem::Create4Menu(Builtin_Menu, w, 0, ttrue, 6, " File ")) &&

      (w = Swindow::Create4Menu(Builtin_Menu)) &&
      (w->InstallHook(UpdateMenuRows, &All->HookModule), ttrue) &&
      Row4Menu(w, COD_TERM_ON, ROW_ACTIVE, 20, " Run Twin Term      ") &&
      Row4Menu(w, COD_TERM_OFF, ROW_INACTIVE, 20, " Stop Twin Term     ") &&
      Row4Menu(
          w, (udat)0, ROW_IGNORE, 20,
          "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4") &&
      Row4Menu(w, COD_SOCKET_ON, ROW_ACTIVE, 20, " Run Socket Server  ") &&
      Row4Menu(w, COD_SOCKET_OFF, ROW_INACTIVE, 20, " Stop Socket Server ") &&
      (Builtin_Modules = Smenuitem::Create4Menu(Builtin_Menu, w, 0, ttrue, 9, " Modules ")) &&

      Smenuitem::Create4MenuCommon(Builtin_Menu) &&

      (AboutWin = Swindow::Create(Builtin_MsgPort, 5, "About",
                                  ColorFill(color_array, 5, TCOL(tWHITE, twhite)), Builtin_Menu,
                                  TCOL(tblack, twhite), NOCURSOR,
                                  WINDOW_AUTO_KEYS | WINDOW_WANT_MOUSE | WINDOW_DRAG | WINDOW_CLOSE,
                                  WINDOWFL_USEROWS | WINDOWFL_ROWS_DEFCOL, 36, 13, 0)) &&

      (ClockWin = Swindow::Create(Builtin_MsgPort, 5, "Clock", NULL, Builtin_Menu,
                                  TCOL(tyellow, tblue), NOCURSOR, WINDOW_DRAG | WINDOW_CLOSE,
                                  WINDOWFL_USEROWS | WINDOWFL_ROWS_DEFCOL, 10, 2, 0)) &&

      (OptionWin = Swindow::Create(
           Builtin_MsgPort, 7, "Options", NULL, Builtin_Menu, TCOL(tBLACK, tblack), NOCURSOR,
           WINDOW_AUTO_KEYS | WINDOW_WANT_MOUSE | WINDOW_DRAG | WINDOW_CLOSE,
           WINDOWFL_USEROWS | WINDOWFL_ROWS_DEFCOL, 40, 16, 0)) &&

      (ButtonWin = Swindow::Create(
           Builtin_MsgPort, 7, "Buttons", NULL, Builtin_Menu, TCOL(tWHITE, twhite), NOCURSOR,
           WINDOW_AUTO_KEYS | WINDOW_WANT_MOUSE | WINDOW_DRAG | WINDOW_CLOSE, WINDOWFL_USECONTENTS,
           37, 19, 0)) &&

      (DisplayWin = Swindow::Create(
           Builtin_MsgPort, 7, "Display", NULL, Builtin_Menu, TCOL(tBLACK, twhite), NOCURSOR,
           WINDOW_WANT_MOUSE | WINDOW_AUTO_KEYS | WINDOW_DRAG | WINDOW_RESIZE | WINDOW_CLOSE |
               WINDOW_X_BAR | WINDOW_Y_BAR,
           WINDOWFL_USEROWS | WINDOWFL_ROWS_SELCURRENT | WINDOWFL_ROWS_DEFCOL, 31, 10, 0)) &&

      (DisplaySubWin = Swindow::Create(
           Builtin_MsgPort, 0, NULL, NULL, Builtin_Menu, TCOL(tBLACK, twhite), NOCURSOR,
           WINDOW_AUTO_KEYS, WINDOWFL_USEROWS | WINDOWFL_ROWS_DEFCOL, 10, TW_MAXDAT, 0)) &&

      (WinList = Swindow::Create(
           Builtin_MsgPort, 11, "Window List", NULL, Builtin_Menu, TCOL(twhite, tblue), NOCURSOR,
           WINDOW_WANT_KEYS | WINDOW_WANT_MOUSE | WINDOW_DRAG | WINDOW_CLOSE /*|WINDOW_RESIZE*/ |
               WINDOW_X_BAR | WINDOW_Y_BAR,
           WINDOWFL_USEROWS | WINDOWFL_ROWS_SELCURRENT | WINDOWFL_ROWS_DEFCOL, 14, 2, 0)) &&

      (ExecuteWin = Swindow::Create(
           Builtin_MsgPort, 10, "Execute...", NULL, Builtin_Menu, TCOL(twhite, tblue), LINECURSOR,
           WINDOW_WANT_KEYS | WINDOW_CLOSE | WINDOW_DRAG | WINDOW_X_BAR,
           WINDOWFL_USEROWS | WINDOWFL_ROWS_DEFCOL | WINDOWFL_CURSOR_ON, 38, 2, 0)) &&

#ifdef CONF_PRINTK
      InitMessagesWin() &&
#endif
      AboutWin->RowWriteCharset(grlen, greeting) &&

      (ButtonOK_About = Sgadget::CreateEmptyButton(Builtin_MsgPort, 8, 1, TCOL(tblack, twhite))) &&

      (ButtonRemove = Sgadget::CreateEmptyButton(Builtin_MsgPort, 8, 1, TCOL(tblack, twhite))) &&
      (ButtonThis = Sgadget::CreateEmptyButton(Builtin_MsgPort, 8, 1, TCOL(tblack, twhite))) &&

      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 27, 1, "[ ] Enable screen Scrolling", 0,
                      GADGETFL_TEXT_DEFCOL, COD_O_SCREEN_SCROLL, TCOL(tblack, twhite),
                      TCOL(tWHITE, tgreen), TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 2, 14) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 23, 1, "[ ] Menu Relaxed Arrows", 0,
                      GADGETFL_TEXT_DEFCOL, COD_O_MENU_RELAX, TCOL(tblack, twhite),
                      TCOL(tWHITE, tgreen), TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 2, 12) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 25, 1, "[ ] Menu Information Line", 0,
                      GADGETFL_TEXT_DEFCOL, COD_O_MENU_INFO, TCOL(tblack, twhite),
                      TCOL(tWHITE, tgreen), TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 2, 10) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 15, 1, "[ ] Hidden Menu", 0,
                      GADGETFL_TEXT_DEFCOL, COD_O_MENU_HIDE, TCOL(tblack, twhite),
                      TCOL(tWHITE, tgreen), TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 2, 8) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 22, 1, "[ ] Always Show Cursor", 0,
                      GADGETFL_TEXT_DEFCOL, COD_O_CURSOR_ALWAYS, TCOL(tblack, twhite),
                      TCOL(tWHITE, tgreen), TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 2, 6) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 37, 1,
                      "[ ] New terminals start in UTF-8 mode", 0, GADGETFL_TEXT_DEFCOL,
                      COD_O_TERMINALS_UTF8, TCOL(tblack, twhite), TCOL(tWHITE, tgreen),
                      TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 2, 4) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 3, 1, "[+]", 0, GADGETFL_TEXT_DEFCOL,
                      COD_O_Yp_SHADE, TCOL(tblack, twhite), TCOL(tWHITE, tgreen),
                      TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 21, 2) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 3, 1, "[-]", 0, GADGETFL_TEXT_DEFCOL,
                      COD_O_Yn_SHADE, TCOL(tblack, twhite), TCOL(tWHITE, tgreen),
                      TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 18, 2) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 3, 1, "[+]", 0, GADGETFL_TEXT_DEFCOL,
                      COD_O_Xp_SHADE, TCOL(tblack, twhite), TCOL(tWHITE, tgreen),
                      TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 21, 1) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 3, 1, "[-]", 0, GADGETFL_TEXT_DEFCOL,
                      COD_O_Xn_SHADE, TCOL(tblack, twhite), TCOL(tWHITE, tgreen),
                      TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 18, 1) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)OptionWin, 11, 1, "[ ] Shadows", 0,
                      GADGETFL_TEXT_DEFCOL, COD_O_SHADOWS, TCOL(tblack, twhite),
                      TCOL(tWHITE, tgreen), TCOL(tBLACK, twhite), TCOL(tBLACK, tblack), 2, 1) &&
      Sgadget::Create(Builtin_MsgPort, (Twidget)ExecuteWin, 19, 1, "[ ] Run in Terminal", 0,
                      GADGETFL_TEXT_DEFCOL, COD_E_TTY, TCOL(tYELLOW, tblue), TCOL(tWHITE, tgreen),
                      TCOL(tBLACK, tblue), TCOL(tBLACK, tblue), 10, 1)

  ) {
    AboutWin->SetColors(0x1FF, TCOL(tGREEN, twhite), TCOL0, TCOL0, TCOL0, TCOL(tWHITE, twhite),
                        TCOL(tblack, twhite), TCOL(tblack, tgreen), TCOL(tBLACK, twhite),
                        TCOL(tBLACK, tblack));

    ClockWin->SetColors(0x1FF, TCOL(tYELLOW, tcyan), TCOL0, TCOL0, TCOL0, TCOL(tWHITE, tBLUE),
                        TCOL(tYELLOW, tblue), TCOL(tYELLOW, tcyan), TCOL(tBLACK, tblue),
                        TCOL(tBLACK, tblack));

    OptionWin->SetColors(0x1FF, TCOL(tGREEN, twhite), TCOL0, TCOL0, TCOL0, TCOL(tWHITE, twhite),
                         TCOL(tBLACK, twhite), TCOL(tblack, tgreen), TCOL(tBLACK, twhite),
                         TCOL(tBLACK, tblack));

    ButtonWin->SetColors(0x1FF, TCOL(tGREEN, twhite), TCOL0, TCOL0, TCOL0, TCOL(tWHITE, twhite),
                         TCOL(tWHITE, twhite), TCOL(tblack, tgreen), TCOL(tBLACK, twhite),
                         TCOL(tBLACK, tblack));

    WinList->SetColors(0x1FF, TCOL(tYELLOW, tcyan), TCOL(tGREEN, tBLUE), TCOL(twhite, tBLUE),
                       TCOL(tWHITE, tBLUE), TCOL(tWHITE, tBLUE), TCOL(twhite, tblue),
                       TCOL(tBLUE, twhite), TCOL(tBLACK, tblue), TCOL(tBLACK, tblack));
    WinList->Configure(1 << 2 | 1 << 3, 0, 0, 15, 2, 0, 0);

    DisplayWin->SetColors(0x1FF, TCOL(tGREEN, twhite), TCOL(tWHITE, twhite), TCOL(tBLUE, twhite),
                          TCOL(tBLUE, tWHITE), TCOL(tWHITE, twhite), TCOL(tblack, twhite),
                          TCOL(tblack, tgreen), TCOL(tBLACK, twhite), TCOL(tBLACK, tblack));

    DisplaySubWin->SetColors(0x30, 0, 0, 0, 0, TCOL(tBLACK, twhite), TCOL(tBLACK, twhite), 0, 0, 0);

    DisplaySubWin->Configure(1 << 0 | 1 << 1, -1, -1, 0, 0, 0, 0);
    DisplaySubWin->Map(DisplayWin);

    DisplayWin->InstallHook(UpdateDisplayWin, &All->HookDisplay);
    WinList->MapUnMapHook = InstallRemoveWinListHook;

    ButtonOK_About->FillButton(AboutWin, COD_OK, 15, 11, 0, "   OK   ", TCOL(tWHITE, tgreen),
                               TCOL(tBLACK, tgreen));

    ButtonRemove->FillButton(DisplaySubWin, COD_D_REMOVE, 1, 2, 0, " Remove ", TCOL(tWHITE, tgreen),
                             TCOL(tBLACK, tgreen));
    ButtonThis->FillButton(DisplaySubWin, COD_D_THIS, 1, 5, 0, "  This  ", TCOL(tWHITE, tgreen),
                           TCOL(tBLACK, tgreen));

    OptionWin->CurX = 25;
    OptionWin->CurY = 1;
    OptionWin->RowWriteCharset(10, "  X Shadow");
    OptionWin->CurX = 25;
    OptionWin->CurY = 2;
    OptionWin->RowWriteCharset(10, "  Y Shadow");

    All->BuiltinMenu = Builtin_Menu;

    return ttrue;
  }
  Error(NOMEMORY);
  log(ERROR) << "twin: InitBuiltin(): " << Errstr << "\n";
  return tfalse;
}
