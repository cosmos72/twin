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

#include <signal.h>

#include "twin.h"
#include "data.h"
#include "methods.h"
#include "main.h"
#include "extreg.h"

#include "hw.h"
#include "hw_multi.h"
#include "common.h"
#include "resize.h"
#include "draw.h"
#include "printk.h"
#include "util.h"
#include "version.h"

#ifdef CONF__MODULES
# include "dl.h"
#endif

#include <Tw/Twkeys.h>

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
# include <Tutf/Tutf_defs.h>

# define _CHECK T_UTF_16_CHECK_MARK
# define _FULL	T_UTF_16_FULL_BLOCK
# define _LOWER T_UTF_16_LOWER_HALF_BLOCK
# define _UPPER T_UTF_16_UPPER_HALF_BLOCK
# define _MEDIUM_SHADE	T_UTF_16_MEDIUM_SHADE
#else
# define _CHECK 'û'
# define _FULL  'Û'
# define _LOWER 'Ü'
# define _UPPER 'ß'
# define _MEDIUM_SHADE	'±'
#endif

#define COD_QUIT	(udat)1 /* as in term.c */
#define COD_SPAWN	(udat)3 /* as COD_SPAWN in term.c */

#define COD_EXECUTE	(udat)5
#define COD_SUSPEND	(udat)10
#define COD_DETACH	(udat)11
#define COD_RELOAD_RC	(udat)12

#define COD_CLOCK_WIN   (udat)20
#define COD_OPTION_WIN	(udat)21
#define COD_BUTTONS_WIN	(udat)22
#define COD_DISPLAY_WIN	(udat)23
#define COD_MESSAGES_WIN (udat)24
#define COD_ABOUT_WIN	(udat)25


#define COD_TERM_ON	(udat)30
#define COD_TERM_OFF	(udat)31
#define COD_SOCKET_ON	(udat)32
#define COD_SOCKET_OFF	(udat)33

#define COD_O_SHADOWS	(udat)40
#define COD_O_Xp_SHADE	(udat)41
#define COD_O_Xn_SHADE	(udat)42
#define COD_O_Yp_SHADE	(udat)43
#define COD_O_Yn_SHADE	(udat)44
#define COD_O_BLINK	(udat)45
#define COD_O_CURSOR_ALWAYS	(udat)46
#define COD_O_MENU_HIDE	(udat)47
#define COD_O_MENU_INFO	(udat)48
#define COD_O_MENU_RELAX	(udat)49
#define COD_O_SCREEN_SCROLL	(udat)50

#define COD_D_REMOVE	(udat)60
#define COD_D_THIS	(udat)61

#define COD_E_TTY	(udat)70

msgport Builtin_MsgPort;

static menu Builtin_Menu;
static menuitem Builtin_File;
#if defined(CONF__MODULES) && !(defined(CONF_SOCKET) && defined(CONF_TERM))
static menuitem Builtin_Modules;
#endif

static window AboutWin, ClockWin, OptionWin, ButtonWin,
    DisplayWin, DisplaySubWin, ExecuteWin;

window WinList, MessagesWin;

static gadget ButtonOK_About, ButtonRemove, ButtonThis;

static void Clock_Update(void) {
    time_t Time = (time_t)All->Now.Seconds;
    struct tm *Date;
    byte Buffer[30];
    
    ClockWin->CurX=ClockWin->CurY=(uldat)0;
    Date = localtime(&Time);
    
    sprintf((char *)Buffer, "%02u/%02u/%04u\n %02u:%02u:%02u",
	    (udat)Date->tm_mday, (udat)Date->tm_mon+1, (udat)Date->tm_year + 1900,
	    (udat)Date->tm_hour, (udat)Date->tm_min,   (udat)Date->tm_sec);
    Act(RowWriteAscii,ClockWin)(ClockWin, strlen(Buffer), Buffer);
    
    Builtin_MsgPort->PauseDuration.Fraction = 1 FullSECs - All->Now.Fraction;
    Builtin_MsgPort->PauseDuration.Seconds = 0;
}

#if defined(CONF__MODULES) && !(defined(CONF_TERM) && defined(CONF_SOCKET))
static void TweakMenuRows(menuitem Item, udat code, byte flag) {
    window Win;
    row Row;
    
    if ((Win = Item->Window) &&
	(Row = Act(FindRowByCode,Win)(Win, code, (uldat *)0)))
	Row->Flags = flag;
}

static void UpdateMenuRows(widget dummy) {
#ifndef CONF_TERM
    if (DlIsLoaded(TermSo)) {
	TweakMenuRows(Builtin_Modules, COD_TERM_ON,    ROW_INACTIVE);
	TweakMenuRows(Builtin_Modules, COD_TERM_OFF,   ROW_ACTIVE);
    } else {
	TweakMenuRows(Builtin_Modules, COD_TERM_ON,    ROW_ACTIVE);
	TweakMenuRows(Builtin_Modules, COD_TERM_OFF,   ROW_INACTIVE);
    }
#endif
#ifndef CONF_SOCKET
    if (DlIsLoaded(SocketSo)) {
	TweakMenuRows(Builtin_Modules, COD_SOCKET_ON,  ROW_INACTIVE);
	TweakMenuRows(Builtin_Modules, COD_SOCKET_OFF, ROW_ACTIVE);
    } else {
	TweakMenuRows(Builtin_Modules, COD_SOCKET_ON,  ROW_ACTIVE);
	TweakMenuRows(Builtin_Modules, COD_SOCKET_OFF, ROW_INACTIVE);
    }
#endif
}

#endif


static void SelectWinList(void) {
    screen Screen = All->FirstScreen;
    uldat n = WinList->CurY;
    widget W;
    
    for (W = Screen->FirstW; W; W = W->Next) {
	if (W == (widget)WinList || !IS_WINDOW(W) ||
	    (((window)W)->Flags & (WINDOWFL_NOTVISIBLE|WINDOWFL_MENU)))
	    continue;
	if (!n)
	    break;
	n--;
    }
    if (!n && W) {
	RaiseWidget(W, TRUE);
	CenterWindow((window)W);
    }
}


static void ExecuteGadgetH(event_gadget *EventG) {
    gadget G;
    
    if (EventG->Code == COD_E_TTY &&
	(G = Act(FindGadgetByCode,ExecuteWin)(ExecuteWin, COD_E_TTY))) {
	
	if (G->USE.T.Text[0][1] == ' ')
	    G->USE.T.Text[0][1] = _CHECK;
	else
	    G->USE.T.Text[0][1] = ' ';
	
	DrawAreaWidget((widget)G);
    }
}

static void ExecuteWinRun(void) {
    byte **argv, *arg0;
    row Row;
    gadget G;
    
    Act(UnMap,ExecuteWin)(ExecuteWin);
    
    if (flag_secure) {
	printk(flag_secure_msg);
	return;
    }
    
    if ((Row = Act(FindRow,ExecuteWin)(ExecuteWin, ExecuteWin->CurY)) && !Row->LenGap) {
	
	argv = TokenizeHWFontVec(Row->Len, Row->Text);
	arg0 = argv ? argv[0] : NULL;

	if ((G = Act(FindGadgetByCode,ExecuteWin)(ExecuteWin, COD_E_TTY)) && G->USE.T.Text[0][1] != ' ') {
	    /* run in a tty */
	    Ext(Term,Open)(arg0, argv);
	} else if (argv) switch (fork()) {
	    /* do not run in a tty */
	  case -1: /* error */
	    break;
	  case 0:  /* child */
	    execvp((char *)arg0, (char **)argv);
	    exit(1);
	    break;
	  default: /* parent */
	    break;
	}
	if (argv)
	    FreeStringVec(argv);
    }
}

void UpdateOptionWin(void) {
    gadget G;
    byte i, Flags = All->SetUp->Flags;
    udat list[] = {COD_O_Xp_SHADE, COD_O_Xn_SHADE, COD_O_Yp_SHADE, COD_O_Yn_SHADE, 0 };

    for (i = 0; list[i]; i++) {
	if ((G = Act(FindGadgetByCode,OptionWin)(OptionWin, list[i]))) {
	    if (Flags & SETUP_SHADOWS)
		G->Flags &= ~GADGETFL_DISABLED;
	    else
		G->Flags |= GADGETFL_DISABLED;
	}
    }
    if ((G = Act(FindGadgetByCode,OptionWin)(OptionWin, COD_O_SHADOWS)))
	G->USE.T.Text[0][1] = Flags & SETUP_SHADOWS ? _CHECK : ' ';
    if ((G = Act(FindGadgetByCode,OptionWin)(OptionWin, COD_O_BLINK)))
	G->USE.T.Text[0][1] = Flags & SETUP_BLINK ? _CHECK : ' ';
    if ((G = Act(FindGadgetByCode,OptionWin)(OptionWin, COD_O_CURSOR_ALWAYS)))
	G->USE.T.Text[0][1] = Flags & SETUP_CURSOR_ALWAYS ? _CHECK : ' ';
    if ((G = Act(FindGadgetByCode,OptionWin)(OptionWin, COD_O_MENU_HIDE)))
	G->USE.T.Text[0][1] = Flags & SETUP_MENU_HIDE ? _CHECK : ' ';
    if ((G = Act(FindGadgetByCode,OptionWin)(OptionWin, COD_O_MENU_INFO)))
	G->USE.T.Text[0][1] = Flags & SETUP_MENU_INFO ? _CHECK : ' ';
    if ((G = Act(FindGadgetByCode,OptionWin)(OptionWin, COD_O_MENU_RELAX)))
	G->USE.T.Text[0][1] = Flags & SETUP_MENU_RELAX ? _CHECK : ' ';
    if ((G = Act(FindGadgetByCode,OptionWin)(OptionWin, COD_O_SCREEN_SCROLL)))
	G->USE.T.Text[0][1] = Flags & SETUP_SCREEN_SCROLL ? _CHECK : ' ';
    
    OptionWin->CurX = 25; OptionWin->CurY = 1;
    i = (Flags & SETUP_SHADOWS ? All->SetUp->DeltaXShade : 0) + '0';
    Act(RowWriteAscii,OptionWin)(OptionWin, 1, &i);
    OptionWin->CurX = 25; OptionWin->CurY = 2;
    i = (Flags & SETUP_SHADOWS ? All->SetUp->DeltaYShade : 0) + '0';
    Act(RowWriteAscii,OptionWin)(OptionWin, 1, &i);
}


static void OptionH(msg Msg) {
    byte Flags = All->SetUp->Flags, XShade = All->SetUp->DeltaXShade, YShade = All->SetUp->DeltaYShade;
    byte redraw = TRUE;
    
    switch (Msg->Event.EventGadget.Code) {
      case COD_O_SHADOWS:
	Flags ^= SETUP_SHADOWS;
	break;
      case COD_O_Xp_SHADE:
	if (XShade < MAX_XSHADE)
	    XShade++;
	break;
      case COD_O_Xn_SHADE:
	if (XShade > 1)
	    XShade--;
	break;
      case COD_O_Yp_SHADE:
	if (YShade < MAX_YSHADE)
	    YShade++;
	break;
      case COD_O_Yn_SHADE:
	if (YShade > 1)
	    YShade--;
	break;
      case COD_O_BLINK:
	Flags ^= SETUP_BLINK;
	break;
      case COD_O_CURSOR_ALWAYS:
	Flags ^= SETUP_CURSOR_ALWAYS;
	redraw = FALSE;
	break;
      case COD_O_MENU_HIDE:
	Flags ^= SETUP_MENU_HIDE;
	HideMenu(!!(Flags & SETUP_MENU_HIDE));
	redraw = FALSE;
	break;
      case COD_O_MENU_INFO:
	Flags ^= SETUP_MENU_INFO;
	break;
      case COD_O_MENU_RELAX:
	Flags ^= SETUP_MENU_RELAX;
	break;
      case COD_O_SCREEN_SCROLL:
	Flags ^= SETUP_SCREEN_SCROLL;
	redraw = FALSE;
	break;
      default:
	redraw = FALSE;
	break;
    }
    if (Flags != All->SetUp->Flags || XShade != All->SetUp->DeltaXShade
	|| YShade != All->SetUp->DeltaYShade) {
	
	All->SetUp->Flags = Flags;
	All->SetUp->DeltaXShade = XShade;
	All->SetUp->DeltaYShade = YShade;
	
	UpdateOptionWin();
	if (redraw == TRUE)
	    QueuedDrawArea2FullScreen = TRUE;
	else {
	    DrawFullWindow2(OptionWin);
	    UpdateCursor();
	}
    }
}

void FillButtonWin(void) {
    dat i, j;
    byte b[6] = "      ";
    CONST byte *s;
    
    DeleteList(ButtonWin->FirstW);
    
    for (i=j=0; j<BUTTON_MAX; j++) {
	if (All->ButtonVec[j].exists)
	    i++;
    }
    ResizeRelWindow(ButtonWin, 0, (dat)(3 + i*2) - (dat)ButtonWin->YWidth);
    
    /* clear the window: */
    Act(TtyWriteAscii,ButtonWin)(ButtonWin, 4, "\033[2J");

    for (j=BUTTON_MAX-1; j>=0; j--) {
	if (!All->ButtonVec[j].exists)
	    continue;
	i--;
	
	Act(GotoXY,ButtonWin)(ButtonWin, 2, 1 + i*2);
	if (j)
	    b[2] = j + '0', s = b;
	else
	    s = "Close ";
	Act(TtyWriteAscii,ButtonWin)(ButtonWin, 7, "Button ");
	Act(TtyWriteAscii,ButtonWin)(ButtonWin, 6, s);
#ifdef CONF__UNICODE
	{
	    hwattr h[2];
	    hwfont *f = All->ButtonVec[j].shape;
	    h[0] = HWATTR_EXTRA32(HWATTR(ButtonWin->ColGadgets, f[0]), EncodeToHWAttrExtra(j, 0, 0, 0));
	    h[1] = HWATTR_EXTRA32(HWATTR(ButtonWin->ColGadgets, f[1]), EncodeToHWAttrExtra(j, 1, 0, 0));
	    
	    Act(TtyWriteHWAttr,ButtonWin)(ButtonWin, 15, 1+i*2, 2, h);
	}
#else
	Act(TtyWriteHWFont,ButtonWin)(ButtonWin, 2, All->ButtonVec[j].shape);
#endif
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)ButtonWin, 3, 1, "[+]",
			  0, GADGETFL_TEXT_DEFCOL, 3 | (j<<2),
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  22, 1+i*2);
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)ButtonWin, 3, 1, "[-]",
			  0, GADGETFL_TEXT_DEFCOL, 2 | (j<<2),
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  19, 1+i*2);
    }
}
    
void UpdateButtonWin(void) {
    dat i, j;
    byte s[5];
    sbyte pos;
    
    for (i=j=0; j<BUTTON_MAX; j++) {
	if (All->ButtonVec[j].exists)
	    i++;
    }
    for (j=BUTTON_MAX-1; j>=0; j--) {
	if (!All->ButtonVec[j].exists)
	    continue;
	i--;
	
	Act(GotoXY,ButtonWin)(ButtonWin, 26, 1 + i*2);
	
	pos = All->ButtonVec[j].pos;
	if (pos >= 0) {
	    Act(TtyWriteAscii,OptionWin)(ButtonWin, 5, "Left ");
	} else if (pos == -1)
	    Act(TtyWriteAscii,OptionWin)(ButtonWin, 9, "Disabled ");
	else {
	    Act(TtyWriteAscii,OptionWin)(ButtonWin, 5, "Right");
	    pos = -pos - 2;
	}
	if (pos >= 0) {
	    sprintf(s, " %3d", pos);
	    Act(TtyWriteAscii,OptionWin)(ButtonWin, strlen(s), s);
	}
    }
}

static void BordersH(msg Msg) {
    udat Code = Msg->Event.EventGadget.Code;
    sbyte op = -1;

    if (!(Code & 2))
	return;
    
    if (Code & 1)
	op = +1;
    
    All->ButtonVec[Code >> 2].pos += op;
    
    QueuedDrawArea2FullScreen = TRUE;
    UpdateButtonWin();
}

static void UpdateDisplayWin(widget displayWin) {
    display_hw hw;
    uldat x = 12, y = 0;
    
    if (displayWin == (widget)DisplayWin) {
	DeleteList(DisplayWin->USE.R.FirstRow);
    
	for (hw = All->FirstDisplayHW; hw; hw = hw->Next) {
	    Act(GotoXY,DisplayWin)(DisplayWin, x, y++);
	    if (!hw->NameLen)
		Act(RowWriteAscii,DisplayWin)(DisplayWin, 9, "(no name)");
	    else
		Act(RowWriteAscii,DisplayWin)(DisplayWin, hw->NameLen, hw->Name);
	}
	if (DisplayWin->Parent)
	    DrawFullWindow2(DisplayWin);
    }
}

static void SelectRowWindow(window CurrWin, uldat newCurY) {
    uldat oldCurY=CurrWin->CurY;

    CurrWin->CurY=newCurY;

    if (oldCurY!=newCurY) {
	DrawLogicWidget((widget)CurrWin, 0, oldCurY, CurrWin->XWidth + CurrWin->XLogic, oldCurY);
	DrawLogicWidget((widget)CurrWin, 0, newCurY, CurrWin->XWidth + CurrWin->XLogic, newCurY);
    }
}

static void DisplayGadgetH(msg Msg) {
    display_hw hw;
    uldat i;

    switch (Msg->Event.EventGadget.Code) {
      case COD_D_REMOVE:
	if ((i = DisplayWin->CurY) < DisplayWin->HLogic) {
	    for (hw = All->FirstDisplayHW; hw && i; hw = hw->Next, i--)
		;
	    if (hw && !i)
		Delete(hw);
	}
	break;
      case COD_D_THIS:
	if (All->MouseHW) {
	    for (i = 0, hw = All->FirstDisplayHW; hw; hw = hw->Next, i++) {
		if (hw == All->MouseHW)
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
	    

static void BuiltinH(msgport MsgPort) {
    msg Msg;
    event_any *Event;
    screen Screen;
    window NewWindow, tempWin;
    row Row;
    udat Code;
    byte /*FontHeight,*/ Flags;
    
    Screen = All->FirstScreen;
    
    while ((Msg = Builtin_MsgPort->FirstMsg)) {
	Remove(Msg);
	Event = &Msg->Event;

	switch (Msg->Type) {
	  case MSG_WIDGET_GADGET:
	    tempWin = (window)Event->EventGadget.W;
	    Code = Event->EventGadget.Code;
	    /*0 == Code Chiusura */
	    if (!Code || Code==COD_CANCEL || Code==COD_OK) {
		
		Act(UnMap,tempWin)(tempWin);
		/* no window needs Delete() here */
		
		if (tempWin == ClockWin)
		    Builtin_MsgPort->WakeUp = FALSE;
		
	    } else if (tempWin == OptionWin)
		OptionH(Msg);
	    else if (tempWin == ButtonWin)
		BordersH(Msg);
	    else if (tempWin == DisplaySubWin)
		DisplayGadgetH(Msg);
	    else if (tempWin == ExecuteWin)
		ExecuteGadgetH(&Event->EventGadget);
	    break;

	  case MSG_MENU_ROW:
	    if (Event->EventMenu.Menu==Builtin_Menu) {
		Code=Event->EventMenu.Code;
		Flags=All->SetUp->Flags;
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
			NewWindow = ExecuteWin; break;
		      case COD_CLOCK_WIN:
			Builtin_MsgPort->WakeUp = TIMER_ALWAYS;
			NewWindow = ClockWin;   break;
		      case COD_OPTION_WIN:
			UpdateOptionWin();
			NewWindow = OptionWin;  break;
		      case COD_BUTTONS_WIN:
			UpdateButtonWin();
			NewWindow = ButtonWin;  break;
		      case COD_DISPLAY_WIN:
			UpdateDisplayWin((widget)DisplayWin);
			NewWindow = DisplayWin; break;
		      case COD_MESSAGES_WIN:
			NewWindow = MessagesWin;break;
		      case COD_ABOUT_WIN:
			NewWindow = AboutWin;   break;
		      default:
			break;
		    }
		    if (NewWindow->Parent)
			Act(UnMap,NewWindow)(NewWindow);
		    Act(Map,NewWindow)(NewWindow, (widget)Screen);
		    break;

		  case COD_QUIT:
		    Quit(0);
		    break;
		    
		  case COD_SUSPEND:
		    SuspendHW(TRUE);
		    flushk();
		    
		    kill(getpid(), SIGSTOP);
		    
		    (void)RestartHW(TRUE);
		    break;

		  case COD_DETACH:
		    QuitHW();
		    break;

		  case COD_RELOAD_RC:
		    SendControlMsg(Ext(WM,MsgPort), MSG_CONTROL_RESTART, 0, NULL);
		    break;

#if defined(CONF__MODULES) && !defined(CONF_TERM)
		  case COD_TERM_ON:
		    if (!DlLoad(TermSo))
			break;
		    /* FALLTHROUGH */
#endif
#if defined(CONF__MODULES) || defined(CONF_TERM)
		 case COD_SPAWN:
		    Ext(Term,Open)(NULL, NULL);
		    break;
#endif
#if defined(CONF__MODULES) && !defined(CONF_TERM)
		  case COD_TERM_OFF:
		    DlUnLoad(TermSo);
		    break;
#endif
#if defined(CONF__MODULES) && !defined(CONF_SOCKET)
		  case COD_SOCKET_OFF:
		    DlUnLoad(SocketSo);
		    if (All->FirstDisplayHW)
			break;
		    /* hmm... better to fire it up again */
		    /* FALLTHROUGH */
		  case COD_SOCKET_ON:
		    if (!DlLoad(SocketSo))
			break;
		    break;
#endif
		  default:
		    break;
		}
	    }
	    break;
	    
	  case MSG_WIDGET_KEY:
	    tempWin = (window)Msg->Event.EventCommon.W;
	    if (tempWin == WinList) {
		switch (Msg->Event.EventKeyboard.Code) {
		  case TW_Escape:
		    Act(UnMap,WinList)(WinList);
		    break;
		  case TW_Return:
		    SelectWinList();
		    break;
		  default:
		    FallBackKeyAction(WinList, &Msg->Event.EventKeyboard);
		    break;
		}
	    } else if (tempWin == ExecuteWin) {
		switch ((Code = Msg->Event.EventKeyboard.Code)) {
		  case TW_Escape:
		    Act(UnMap,ExecuteWin)(ExecuteWin);
		    break;
		  case TW_BackSpace:
		    if (ExecuteWin->CurX) {
			ExecuteWin->CurX--;
			UpdateCursor();
			if ((Row = Act(FindRow,ExecuteWin)(ExecuteWin, ExecuteWin->CurY)) && Row->Len) {
			    Row->Len--;
			    DrawLogicWidget((widget)ExecuteWin, Row->Len, ExecuteWin->CurY,
					   Row->Len + 1, ExecuteWin->CurY);
			}
		    }
		    break;
		  default:
		    if (Code == TW_Return || Code == TW_KP_Enter ||
			(Msg->Event.EventKeyboard.SeqLen == 1 &&
			 ((Code = Msg->Event.EventKeyboard.AsciiSeq[0]) == 10 /* CTRL+J */ ||
			  Code == 13 /* CTRL+M */)))
			ExecuteWinRun();
		    else if (Msg->Event.EventKeyboard.SeqLen)
			Act(RowWriteAscii,ExecuteWin)(ExecuteWin, Msg->Event.EventKeyboard.SeqLen, Msg->Event.EventKeyboard.AsciiSeq);
		    break;
		}
	    }
	    break;
	    
	  case MSG_WIDGET_MOUSE:
	    tempWin = (window)Msg->Event.EventCommon.W;
	    
	    if (tempWin == WinList || tempWin == DisplayWin) {
		dat EventMouseX, EventMouseY;
		byte temp;

		EventMouseX = Msg->Event.EventMouse.X, EventMouseY = Msg->Event.EventMouse.Y;
		temp = EventMouseX >= 0 && EventMouseX <= tempWin->XWidth-2
		    && EventMouseY >= 0 && EventMouseY <= tempWin->YWidth-2
		    && (uldat)EventMouseY+tempWin->YLogic < (uldat)tempWin->HLogic;

		SelectRowWindow(tempWin, temp ? (uldat)EventMouseY+tempWin->YLogic : MAXLDAT);
		
		if (tempWin == WinList &&
		    isRELEASE(Msg->Event.EventMouse.Code)) {
		    if (temp)
			SelectWinList();
		}
	    }
	    break;
	    
	  case MSG_SELECTION:
	    /* user wants to paste. ask for selection contents */
	    if (Msg->Event.EventSelection.W == (widget)ExecuteWin)
		TwinSelectionRequest((obj)Builtin_MsgPort, ExecuteWin->Id, TwinSelectionGetOwner());
	    break;
	    
	  case MSG_SELECTIONNOTIFY:
	    tempWin = (window)Id2Obj(window_magic_id, Msg->Event.EventSelectionNotify.ReqPrivate);
	    if (tempWin && tempWin == ExecuteWin) {
		switch (Msg->Event.EventSelectionNotify.Magic) {
		  case SEL_TEXTMAGIC:
		    Act(RowWriteAscii,tempWin)(tempWin, Msg->Event.EventSelectionNotify.Len, Msg->Event.EventSelectionNotify.Data);
		    break;
		  default:
		    break;
		}
	    }
	    break;
	    
	  case MSG_USER_CONTROL:
	    switch (Event->EventControl.Code) {
	      case MSG_CONTROL_OPEN:
		{
		    byte **cmd = TokenizeStringVec(Event->EventControl.Len, Event->EventControl.Data);
		    if (cmd) {
			Ext(Term,Open)(cmd[0], cmd);
			FreeStringVec(cmd);
		    } else
			Ext(Term,Open)(NULL, NULL);
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
	Delete(Msg);
    }
    if (Builtin_MsgPort->WakeUp)
	Clock_Update();
}

void FullUpdateWinList(widget listWin);

void InstallRemoveWinListHook(widget listWin) {    
    if (listWin == (widget)WinList) {
	if (WinList->Parent && IS_SCREEN(WinList->Parent))
	    Act(InstallHook,WinList)(WinList, FullUpdateWinList,
				     &((screen)WinList->Parent)->FnHookW);
	else
	    Act(RemoveHook,WinList)(WinList, FullUpdateWinList, WinList->WhereHook);
    }
}

void UpdateWinList(void) {
    screen Screen = All->FirstScreen;
    widget W;
    
    DeleteList(WinList->USE.R.FirstRow);
    WinList->CurX = WinList->CurY = 0;
    
    WinList->XLogic = WinList->YLogic = 0;
    WinList->XWidth = WinList->MinXWidth;
    WinList->YWidth = WinList->MinYWidth;
    
    for (W = Screen->FirstW; W; W = W->Next) {
	if (W == (widget)WinList || !IS_WINDOW(W) ||
	    (((window)W)->Flags & (WINDOWFL_NOTVISIBLE|WINDOWFL_MENU)))
	    continue;
	(void)Row4Menu(WinList, (udat)0, ROW_ACTIVE, ((window)W)->NameLen, ((window)W)->Name);
    }
}

void FullUpdateWinList(widget listWin) {
    if (listWin == (widget)WinList && WinList->Parent) {
	ResizeRelWindow(WinList, WinList->MinXWidth - WinList->XWidth, WinList->MinYWidth - WinList->YWidth);
    
	UpdateWinList();
	
	DrawAreaWindow2(WinList);
    }
}


#ifdef CONF_PRINTK
static byte InitMessagesWin(void) {
    MessagesWin = Do(Create,Window)
	(FnWindow, Builtin_MsgPort, 8, "Messages", NULL,
	 Builtin_Menu, COL(WHITE,BLACK), LINECURSOR,
	 WINDOW_DRAG|WINDOW_RESIZE|WINDOW_X_BAR|WINDOW_Y_BAR|WINDOW_CLOSE,
	 WINDOWFL_CURSOR_ON, 60, 20, 200);
    if (MessagesWin) {
	Act(SetColors,MessagesWin)
	    (MessagesWin, 0x1F1, COL(HIGH|GREEN,WHITE), 0, 0, 0, COL(HIGH|WHITE,WHITE),
	     COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK));
    }
    return !!MessagesWin;
}
#endif

static byte InitScreens(void) {
    screen OneScreen;

    if ((OneScreen = Do(CreateSimple,Screen)(FnScreen, 1, "1", HWATTR(COL(HIGH|BLACK,BLUE),_MEDIUM_SHADE)))) {
	
	InsertLast(Screen, OneScreen, All);
	return TRUE;
    }
    Error(NOMEMORY);
    printk("twin: InitScreens(): %."STR(SMALLBUFF)"s\n", ErrStr);
    return FALSE;
}

byte InitBuiltin(void) {
    window Window;
    CONST byte *greeting = "\n"
	"                TWIN             \n"
	"        Text WINdows manager     \n\n"
#ifdef CONF__UNICODE
	"     Version " TWIN_VERSION_STR TWIN_VERSION_EXTRA_STR " (Unicode) by  \n\n"
#else
	"          Version " TWIN_VERSION_STR TWIN_VERSION_EXTRA_STR " by       \n\n"
#endif
	"        Massimiliano Ghilardi    \n\n"
	"         <max@Linuz.sns.it>      ";
    uldat grlen = strlen(greeting);
    
    if ((Builtin_MsgPort=Do(Create,MsgPort)
	 (FnMsgPort, 4, "twin", 0, 0, 0, BuiltinH)) &&
	
	InitScreens() && /* Do(Create,Screen)() requires Builtin_MsgPort ! */
	
	(All->BuiltinRow = Do(Create,Row)(FnRow, 0, ROW_ACTIVE|ROW_DEFCOL))&&
	
	(Builtin_Menu=Do(Create,Menu)
	 (FnMenu, Builtin_MsgPort, (byte)0x70, (byte)0x20, (byte)0x78, (byte)0x08, (byte)0x74, (byte)0x24, (byte)0)) &&
	Info4Menu(Builtin_Menu, ROW_ACTIVE, (uldat)42, " Hit PAUSE or Mouse Right Button for Menu ", "tttttttttttttttttttttttttttttttttttttttttt") &&
	
	(Window=Win4Menu(Builtin_Menu)) &&
	Row4Menu(Window, COD_CLOCK_WIN,  ROW_ACTIVE, 9, " Clock   ") &&
	Row4Menu(Window, COD_OPTION_WIN, ROW_ACTIVE, 9, " Options ") &&
	Row4Menu(Window, COD_BUTTONS_WIN,ROW_ACTIVE, 9, " Buttons ") &&
	Row4Menu(Window, COD_DISPLAY_WIN,ROW_ACTIVE, 9, " Display ") &&
#ifdef CONF_PRINTK
	Row4Menu(Window,COD_MESSAGES_WIN,ROW_ACTIVE,10, " Messages ") &&
#endif
	Row4Menu(Window, COD_ABOUT_WIN,  ROW_ACTIVE, 9, " About   ") &&
	Item4Menu(Builtin_Menu, Window, TRUE, 3, " ð ") &&
	
	(Window=Win4Menu(Builtin_Menu)) &&
#if defined(CONF_TERM) || defined(CONF__MODULES)

	Row4Menu(Window, COD_SPAWN,  ROW_ACTIVE,10, " New Term ") &&
#endif
	Row4Menu(Window, COD_EXECUTE,ROW_ACTIVE,10, " Execute  ") &&
#if defined(CONF_WM_RC) || defined(CONF__MODULES)
	Row4Menu(Window, COD_RELOAD_RC,ROW_ACTIVE,11," Reload RC ") &&
	Row4Menu(Window, (udat)0,    ROW_IGNORE,11, "ÄÄÄÄÄÄÄÄÄÄÄ") &&
#else
	Row4Menu(Window, (udat)0,    ROW_IGNORE,10, "ÄÄÄÄÄÄÄÄÄÄ") &&
#endif
	Row4Menu(Window, COD_DETACH, ROW_ACTIVE,10, " Detach   ") &&
	Row4Menu(Window, COD_SUSPEND,ROW_ACTIVE,10, " Suspend  ") &&
	Row4Menu(Window, COD_QUIT,   ROW_ACTIVE,10, " Quit     ") &&
	(Builtin_File=Item4Menu(Builtin_Menu, Window, TRUE, 6, " File ")) &&
	
	(Window=Win4Menu(Builtin_Menu)) &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,11," Undo      ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,11," Redo      ") &&
	Row4Menu(Window, (udat)0, ROW_IGNORE,  11,"ÄÄÄÄÄÄÄÄÄÄÄ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,11," Cut       ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,11," Copy      ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,11," Paste     ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,11," Clear     ") &&
	Row4Menu(Window, (udat)0, ROW_IGNORE,  11,"ÄÄÄÄÄÄÄÄÄÄÄ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,11," Clipboard ") &&
	Item4Menu(Builtin_Menu, Window, TRUE, 6," Edit ") &&
	
#if defined(CONF__MODULES) && !(defined(CONF_TERM) && defined(CONF_SOCKET))
	(Window=Win4Menu(Builtin_Menu)) &&
	(Act(InstallHook,Window)(Window, UpdateMenuRows, &All->FnHookModule), TRUE) &&
	
#if !defined(CONF_TERM)
	Row4Menu(Window, COD_TERM_ON,	ROW_ACTIVE,  20, " Run Twin Term      ") &&
	Row4Menu(Window, COD_TERM_OFF,	ROW_INACTIVE,20, " Stop Twin Term     ") &&
#endif
#if !defined(CONF_SOCKET) && !defined(CONF_TERM)
	Row4Menu(Window, (udat)0,       ROW_IGNORE,  20, "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ") &&
#endif	
#if !defined(CONF_SOCKET)
	Row4Menu(Window, COD_SOCKET_ON,	ROW_ACTIVE,  20, " Run Socket Server  ") &&
	Row4Menu(Window, COD_SOCKET_OFF,ROW_INACTIVE,20, " Stop Socket Server ") &&
#endif
	(Builtin_Modules=Item4Menu(Builtin_Menu, Window, TRUE, 9," Modules ")) &&
#endif
	
	Item4MenuCommon(Builtin_Menu) &&
		
	(AboutWin = Do(Create,Window)
	 (FnWindow, Builtin_MsgPort, 5, "About", "\x7F\x7F\x7F\x7F\x7F", Builtin_Menu, COL(BLACK,WHITE),
	  NOCURSOR, WINDOW_AUTO_KEYS|WINDOW_WANT_MOUSE|WINDOW_DRAG|WINDOW_CLOSE, WINDOWFL_USEROWS|WINDOWFL_ROWS_DEFCOL,
	  36, 13, 0)) &&

	(ClockWin = Do(Create,Window)
	 (FnWindow, Builtin_MsgPort, 5, "Clock", NULL, Builtin_Menu, COL(YELLOW,BLUE),
	  NOCURSOR, WINDOW_DRAG|WINDOW_CLOSE, WINDOWFL_USEROWS|WINDOWFL_ROWS_DEFCOL,
	  10, 2, 0)) &&

	(OptionWin = Do(Create,Window)
	 (FnWindow, Builtin_MsgPort, 7, "Options", NULL, Builtin_Menu, COL(HIGH|BLACK,BLACK),
	  NOCURSOR, WINDOW_AUTO_KEYS|WINDOW_WANT_MOUSE|WINDOW_DRAG|WINDOW_CLOSE,WINDOWFL_USEROWS|WINDOWFL_ROWS_DEFCOL,
	  37, 16, 0)) &&

	(ButtonWin = Do(Create,Window)
	 (FnWindow, Builtin_MsgPort, 7, "Buttons", NULL, Builtin_Menu, COL(HIGH|WHITE,WHITE),
	  NOCURSOR, WINDOW_AUTO_KEYS|WINDOW_WANT_MOUSE|WINDOW_DRAG|WINDOW_CLOSE, WINDOWFL_USECONTENTS,
	  37, 19, 0)) &&

	(DisplayWin = Do(Create,Window)
	 (FnWindow, Builtin_MsgPort, 7, "Display", NULL, Builtin_Menu, COL(HIGH|BLACK,WHITE),
	  NOCURSOR,
	  WINDOW_WANT_MOUSE|WINDOW_AUTO_KEYS|WINDOW_DRAG|WINDOW_RESIZE|WINDOW_CLOSE|WINDOW_X_BAR|WINDOW_Y_BAR,
	  WINDOWFL_USEROWS|WINDOWFL_ROWS_SELCURRENT|WINDOWFL_ROWS_DEFCOL,
	  31, 10, 0)) &&

	(DisplaySubWin = Do(Create,Window)
	 (FnWindow, Builtin_MsgPort, 0, NULL, NULL, Builtin_Menu, COL(HIGH|BLACK,WHITE),
	  NOCURSOR, WINDOW_AUTO_KEYS, WINDOWFL_USEROWS|WINDOWFL_ROWS_DEFCOL,
	  10, MAXDAT, 0)) &&

	(WinList = Do(Create,Window)
	 (FnWindow, Builtin_MsgPort, 11, "Window List", NULL, Builtin_Menu, COL(WHITE,BLUE),
	  NOCURSOR,
	  WINDOW_WANT_KEYS|WINDOW_WANT_MOUSE|WINDOW_DRAG|WINDOW_CLOSE/*|WINDOW_RESIZE*/|WINDOW_X_BAR|WINDOW_Y_BAR,
	  WINDOWFL_USEROWS|WINDOWFL_ROWS_SELCURRENT|WINDOWFL_ROWS_DEFCOL,
	  14, 2, 0)) &&

	(ExecuteWin = Do(Create,Window)
	 (FnWindow, Builtin_MsgPort, 10, "Execute...", NULL, Builtin_Menu, COL(WHITE,BLUE),
	  LINECURSOR, WINDOW_WANT_KEYS|WINDOW_CLOSE|WINDOW_DRAG|WINDOW_X_BAR,
	  WINDOWFL_USEROWS|WINDOWFL_ROWS_DEFCOL|WINDOWFL_CURSOR_ON,
	  38, 2, 0)) &&

#ifdef CONF_PRINTK
	InitMessagesWin() &&
#endif
	Act(RowWriteAscii,AboutWin)(AboutWin, grlen, greeting) &&
	
	(ButtonOK_About=Do(CreateEmptyButton,Gadget)(FnGadget, Builtin_MsgPort, 8, 1, COL(BLACK,WHITE))) &&

	(ButtonRemove=Do(CreateEmptyButton,Gadget)(FnGadget, Builtin_MsgPort, 8, 1, COL(BLACK,WHITE))) &&
	(ButtonThis  =Do(CreateEmptyButton,Gadget)(FnGadget, Builtin_MsgPort, 8, 1, COL(BLACK,WHITE))) &&
	
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 27, 1, "[ ] Enable Screen Scrolling",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_SCREEN_SCROLL,
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 2, 14) &&
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 23, 1, "[ ] Menu Relaxed Arrows",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_MENU_RELAX,
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 2, 12) &&
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 25, 1, "[ ] Menu Information Line",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_MENU_INFO,
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 2, 10) &&
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 15, 1, "[ ] Hidden Menu",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_MENU_HIDE,
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 2, 8) &&
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 32, 1, "[ ] Enable Blink/High Background",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_BLINK,
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 2, 6) &&
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 22, 1, "[ ] Always Show Cursor",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_CURSOR_ALWAYS,
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 2, 4) &&
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 3, 1, "[+]",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_Yp_SHADE, 
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 21, 2) &&
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 3, 1, "[-]",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_Yn_SHADE,
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 18, 2) &&
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 3, 1, "[+]",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_Xp_SHADE,
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 21, 1) &&
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 3, 1, "[-]",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_Xn_SHADE,
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 18, 1) &&
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)OptionWin, 11, 1, "[ ] Shadows",
			  0, GADGETFL_TEXT_DEFCOL, COD_O_SHADOWS,
			  COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK), 2, 1) &&
	
	Do(Create,Gadget)(FnGadget, Builtin_MsgPort, (widget)ExecuteWin, 19, 1, "[ ] Run in Terminal",
			  0, GADGETFL_TEXT_DEFCOL, COD_E_TTY,
			  COL(HIGH|YELLOW,BLUE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,BLUE), COL(HIGH|BLACK,BLUE), 10, 1)

	)
    {
	Act(SetColors,AboutWin)(AboutWin, 0x1FF, (hwcol)0x7A, (hwcol)0, (hwcol)0, (hwcol)0, (hwcol)0x7F,
				(hwcol)0x70, (hwcol)0x20, (hwcol)0x78, (hwcol)0x08);
	
	Act(SetColors,ClockWin)(ClockWin, 0x1FF, (hwcol)0x3E, (hwcol)0, (hwcol)0, (hwcol)0, (hwcol)0x9F,
				(hwcol)0x1E, (hwcol)0x3E, (hwcol)0x18, (hwcol)0x08);

	Act(SetColors,OptionWin)(OptionWin, 0x1FF, (hwcol)0x7A, (hwcol)0, (hwcol)0, (hwcol)0, (hwcol)0x7F,
				 (hwcol)0x78, (hwcol)0x20, (hwcol)0x78, (hwcol)0x08);

	Act(SetColors,ButtonWin)(ButtonWin, 0x1FF, (hwcol)0x7A, (hwcol)0, (hwcol)0, (hwcol)0, (hwcol)0x7F,
				 (hwcol)0x7F, (hwcol)0x20, (hwcol)0x78, (hwcol)0x08);

	Act(SetColors,WinList)(WinList, 0x1FF,
			       COL(HIGH|YELLOW,CYAN), COL(HIGH|GREEN,HIGH|BLUE), COL(WHITE,HIGH|BLUE),
			       COL(HIGH|WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE),
			       COL(WHITE,BLUE), COL(HIGH|BLUE,WHITE), COL(HIGH|BLACK,BLUE), COL(HIGH|BLACK,BLACK));
	Act(Configure,WinList)(WinList, 1<<2 | 1<<3, 0, 0, 15, 2, 0, 0);

	Act(SetColors,DisplayWin)(DisplayWin, 0x1FF, (hwcol)0x7A, (hwcol)0x7F, (hwcol)0x79, (hwcol)0xF9, (hwcol)0x7F,
				  (hwcol)0x70, (hwcol)0x20, (hwcol)0x78, (hwcol)0x08);

	Act(SetColors,DisplaySubWin)(DisplaySubWin, 1<<4, 0, 0, 0, 0, COL(HIGH|BLACK,WHITE),
				     0, 0, 0, 0);

	Act(Configure,DisplaySubWin)(DisplaySubWin, 1<<0 | 1<<1, -1, -1, 0, 0, 0, 0);
	Act(Map,DisplaySubWin)(DisplaySubWin, (widget)DisplayWin);
	
	Act(InstallHook,DisplayWin)(DisplayWin, UpdateDisplayWin, &All->FnHookDisplayHW);
	WinList->MapUnMapHook = InstallRemoveWinListHook;
	
	Act(FillButton,ButtonOK_About)(ButtonOK_About, (widget)AboutWin, COD_OK,
				       15, 11, 0, "   OK   ", (byte)0x2F, (byte)0x28);

	Act(FillButton,ButtonRemove)(ButtonRemove, (widget)DisplaySubWin, COD_D_REMOVE,
				     1, 2, 0, " Remove ", (byte)0x2F, (byte)0x28);
	Act(FillButton,ButtonThis)  (ButtonThis,   (widget)DisplaySubWin, COD_D_THIS,
				     1, 5, 0, "  This  ", (byte)0x2F, (byte)0x28);

	OptionWin->CurX = 25; OptionWin->CurY = 1;
	Act(RowWriteAscii,OptionWin)(OptionWin, 10, "  X Shadow");
	OptionWin->CurX = 25; OptionWin->CurY = 2;
	Act(RowWriteAscii,OptionWin)(OptionWin, 10, "  Y Shadow");

	All->BuiltinMenu=Builtin_Menu;

	return TRUE;
    }
    Error(NOMEMORY);
    printk("twin: InitBuiltin(): %."STR(SMALLBUFF)"s\n", ErrStr);
    return FALSE;
}


