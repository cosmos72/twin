/*
 *  builtin.c  --  create default menu, Clock and About windows.
 *
 *  Copyright (C) 1993-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <signal.h>
#include <unistd.h>

#include "twin.h"
#include "data.h"
#include "methods.h"
#include "main.h"
#include "extensions.h"

#include "hw.h"
#include "hw_multi.h"
#include "resize.h"
#include "draw.h"
#include "util.h"
#include "version.h"

#ifdef CONF__MODULES
# include "dl.h"
#endif

#include "libTwkeys.h"


#define COD_QUIT	(udat)1 /* as in term.c */
#define COD_SPAWN	(udat)3 /* as COD_SPAWN in term.c */

#define COD_SUSPEND	(udat)10
#define COD_DETACH	(udat)11
#define COD_WM_RESTART	(udat)12

#define COD_ABOUT_WIN	(udat)20
#define COD_CLOCK_WIN   (udat)21
#define COD_OPTION_WIN	(udat)22
#define COD_BUTTONS_WIN	(udat)23
#define COD_DISPLAY_WIN	(udat)24
#define COD_REFRESH	(udat)25

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
#define COD_O_ALWAYSCURSOR (udat)46
#define COD_O_HIDEMENU	(udat)47
#define COD_O_MENUINFO	(udat)48
#define COD_O_EDGESCROLL (udat)49
#define COD_O_ALTFONT	(udat)50

#define COD_D_REMOVE	(udat)60
#define COD_D_THIS	(udat)61

static msgport *Builtin_MsgPort;
static menu *Builtin_Menu;
static menuitem *Builtin_File;
#ifdef CONF__MODULES 
static menuitem *Builtin_Modules;
#endif

static window *AboutWin, *ClockWin, *OptionWin, *ButtonWin, *DisplayWin;
window *WinList;

static gadget *ButtonOK_About, *ButtonRemove, *ButtonThis;

static void Clock_Update(void) {
    struct timevalue *Time = &All->Now;
    struct tm *Date;
    byte Buffer[30];
    
    ClockWin->CurX=ClockWin->CurY=(uldat)0;
    Date = localtime(&Time->Seconds);
    
    sprintf((char *)Buffer, "%02u/%02u/%04u\n %02u:%02u:%02u",
	    (udat)Date->tm_mday, (udat)Date->tm_mon+1, (udat)Date->tm_year + 1900,
	    (udat)Date->tm_hour, (udat)Date->tm_min,   (udat)Date->tm_sec);
    if (!Act(WriteRow,ClockWin)(ClockWin, strlen(Buffer), Buffer))
	Error(NOMEMORY);
    
    Builtin_MsgPort->PauseDuration.Fraction = 1 FullSECs - Time->Fraction;
    Builtin_MsgPort->PauseDuration.Seconds = 0;
}

#if defined(CONF__MODULES) && !(defined(CONF_TERM) && defined(CONF_SOCKET))
static void TweakMenuRows(menuitem *Item, udat code, byte flag) {
    window *Win;
    row *Row;
    
    if ((Win = Item->Window) &&
	(Row = Act(SearchRowCode,Win)(Win, code, (uldat *)0)))
	Row->Flags = flag;
}

static void UpdateMenuRows(window *dummy) {
    module *Module = All->FirstModule;
    byte _TermSo = FALSE, _SocketSo = FALSE;

    while (Module) {
	if (Module->NameLen == 7 && !CmpMem("term.so", Module->Name, 7))
	    _TermSo = TRUE;
	else if (Module->NameLen == 9 && !CmpMem("socket.so", Module->Name, 9))
	    _SocketSo = TRUE;

	Module = Module->Next;
    }

#ifndef CONF_TERM
    if (_TermSo) {
	TweakMenuRows(Builtin_Modules, COD_TERM_ON,    ROW_INACTIVE);
	TweakMenuRows(Builtin_Modules, COD_TERM_OFF,   ROW_ACTIVE);
	TweakMenuRows(Builtin_File,    COD_SPAWN,      ROW_ACTIVE);
    } else {
	TweakMenuRows(Builtin_Modules, COD_TERM_ON,    ROW_ACTIVE);
	TweakMenuRows(Builtin_Modules, COD_TERM_OFF,   ROW_INACTIVE);
	TweakMenuRows(Builtin_File,    COD_SPAWN,      ROW_INACTIVE);
    }
#endif
#ifndef CONF_SOCKET
    if (_SocketSo) {
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
    screen *Screen = All->FirstScreen;
    window *Window = Screen->FirstWindow, *tempWin;
    uldat n = WinList->CurY;
    
    for (tempWin = Window; ; Window = Window->Next) {
	if (!Window)
	    break;
	if (Window == WinList || (Window->Attrib & WINDOW_MENU))
	    continue;
	if (!n)
	    break;
	n--;
    }
    if (!n && Window) {
	MakeFirstWindow(Window, TRUE);
	CenterWindow(Window);
    }
}



void UpdateOptionWin(void) {
    gadget *G;
    byte i, Flags = All->SetUp->Flags;
    udat list[] = {COD_O_Xp_SHADE, COD_O_Xn_SHADE, COD_O_Yp_SHADE, COD_O_Yn_SHADE, 0 };

    for (i = 0; list[i]; i++) {
	if ((G = Act(SearchGadgetCode,OptionWin)(OptionWin, list[i]))) {
	    if (Flags & SETUP_SHADOWS)
		G->Flags &= ~GADGET_DISABLED;
	    else
		G->Flags |= GADGET_DISABLED;
	}
    }
    if ((G = Act(SearchGadgetCode,OptionWin)(OptionWin, COD_O_SHADOWS)))
	G->Contents[0][1] = Flags & SETUP_SHADOWS ? 'û' : ' ';
    if ((G = Act(SearchGadgetCode,OptionWin)(OptionWin, COD_O_BLINK)))
	G->Contents[0][1] = Flags & SETUP_BLINK ? 'û' : ' ';
    if ((G = Act(SearchGadgetCode,OptionWin)(OptionWin, COD_O_ALWAYSCURSOR)))
	G->Contents[0][1] = Flags & SETUP_ALWAYSCURSOR ? 'û' : ' ';
    if ((G = Act(SearchGadgetCode,OptionWin)(OptionWin, COD_O_HIDEMENU)))
	G->Contents[0][1] = Flags & SETUP_HIDEMENU ? 'û' : ' ';
    if ((G = Act(SearchGadgetCode,OptionWin)(OptionWin, COD_O_MENUINFO)))
	G->Contents[0][1] = Flags & SETUP_MENUINFO ? 'û' : ' ';
    if ((G = Act(SearchGadgetCode,OptionWin)(OptionWin, COD_O_EDGESCROLL)))
	G->Contents[0][1] = Flags & SETUP_EDGESCROLL ? 'û' : ' ';
    if ((G = Act(SearchGadgetCode,OptionWin)(OptionWin, COD_O_ALTFONT)))
	G->Contents[0][1] = Flags & SETUP_ALTFONT ? 'û' : ' ';
    
    OptionWin->CurX = 25; OptionWin->CurY = 1;
    i = (Flags & SETUP_SHADOWS ? All->SetUp->DeltaXShade : 0) + '0';
    Act(WriteRow,OptionWin)(OptionWin, 1, &i);
    OptionWin->CurX = 25; OptionWin->CurY = 2;
    i = (Flags & SETUP_SHADOWS ? All->SetUp->DeltaYShade : 0) + '0';
    Act(WriteRow,OptionWin)(OptionWin, 1, &i);
}


static void OptionH(msg *Msg) {
    byte Flags = All->SetUp->Flags, XShade = All->SetUp->DeltaXShade, YShade = All->SetUp->DeltaYShade;
    byte redraw = TRUE;
    
    switch (Msg->Event.EventGadget.Code) {
      case COD_O_ALTFONT:
	Flags ^= SETUP_ALTFONT;
	ResetBorderPattern();
	break;
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
      case COD_O_ALWAYSCURSOR:
	Flags ^= SETUP_ALWAYSCURSOR;
	redraw = FALSE;
	break;
      case COD_O_HIDEMENU:
	Flags ^= SETUP_HIDEMENU;
	HideMenu(!!(Flags & SETUP_HIDEMENU));
	redraw = FALSE;
	break;
      case COD_O_MENUINFO:
	Flags ^= SETUP_MENUINFO;
	break;
      case COD_O_EDGESCROLL:
	Flags ^= SETUP_EDGESCROLL;
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
	    DrawArea(FULL_SCREEN);
	else {
	    DrawAreaWindow(OptionWin, FALSE);
	    UpdateCursor();
	}
    }
}

void FillButtonWin(void) {
    byte i, j;
    byte b[6] = "      ", *s;
    
    DeleteList(ButtonWin->FirstGadget);
    DeleteList(ButtonWin->FirstRow);
    
    for (i=j=0; j<BUTTON_MAX; j++) {
	if (!All->ButtonVec[j].exists)
	    continue;
	
	ButtonWin->CurX = 2; ButtonWin->CurY = 1 + i*2;
	if (j)
	    b[2] = j + '0', s = b;
	else
	    s = "Close ";
	Act(WriteRow,ButtonWin)(ButtonWin, 7, "Button ");
	Act(WriteRow,ButtonWin)(ButtonWin, 6, s);
	Act(WriteRow,ButtonWin)(ButtonWin, 2, All->ButtonVec[j].shape);

	Do(Create,Gadget)(FnGadget, ButtonWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  2 | (j<<2), GADGET_USE_DEFCOL, 21, 1+i*2, 3, 1,
			  "[-]", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	Do(Create,Gadget)(FnGadget, ButtonWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  3 | (j<<2), GADGET_USE_DEFCOL, 24, 1+i*2, 3, 1,
			  "[+]", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	i++;
    }
    
    ButtonWin->Attrib |= WINDOW_RESIZE;
    ResizeRelWindow(ButtonWin, 0, (dat)(3 + i*2) - (dat)ButtonWin->YWidth);
    ButtonWin->Attrib &= ~WINDOW_RESIZE;
}
    
void UpdateButtonWin(void) {
    byte i, j, s[5];
    num pos;
    
    for (i=j=0; j<BUTTON_MAX; j++) {
	if (!All->ButtonVec[j].exists)
	    continue;
	ButtonWin->CurX = 28; ButtonWin->CurY = 1 + i*2;
	
	pos = All->ButtonVec[j].pos;
	if (pos >= 0) {
	    Act(WriteRow,OptionWin)(ButtonWin, 5, "Left ");
	} else if (pos == -1)
	    Act(WriteRow,OptionWin)(ButtonWin, 9, "Disabled ");
	else {
	    Act(WriteRow,OptionWin)(ButtonWin, 5, "Right");
	    pos = -pos - 2;
	}
	if (pos >= 0) {
	    sprintf(s, " %3d", pos);
	    Act(WriteRow,OptionWin)(ButtonWin, strlen(s), s);
	}
	i++;
    }
}

static void BordersH(msg *Msg) {
    udat Code = Msg->Event.EventGadget.Code;
    num op = -1;

    if (!(Code & 2))
	return;
    
    if (Code & 1)
	op = +1;
    
    All->ButtonVec[Code >> 2].pos += op;
    
    UpdateButtonWin();
    DrawArea(FULL_SCREEN);
}

void UpdateDisplayWin(window *displayWin) {
    display_hw *hw;
    uldat x = 12, y = 0;
    
    if (displayWin == DisplayWin) {
	DeleteList(DisplayWin->FirstRow);
    
	for (hw = All->FirstDisplayHW; hw; hw = hw->Next) {
	    Act(GotoXY,DisplayWin)(DisplayWin, x, y++);
	    if (!hw->NameLen)
		Act(WriteRow,DisplayWin)(DisplayWin, 9, "(no name)");
	    else
		Act(WriteRow,DisplayWin)(DisplayWin, hw->NameLen, hw->Name);
	}
	if (DisplayWin->Screen)
	    DrawAreaWindow(DisplayWin, FALSE);
    }
}

static void SelectRowWindow(window *CurrWin, uldat newCurY) {
    uldat oldCurY=CurrWin->CurY;

    CurrWin->CurY=newCurY;

    if (oldCurY!=newCurY) {
	DrawTextWindow(CurrWin, 0, oldCurY, CurrWin->XWidth-2, oldCurY);
	DrawTextWindow(CurrWin, 0, newCurY, CurrWin->XWidth-2, newCurY);
    }
}

static void DisplayGadgetH(msg *Msg) {
    display_hw *hw;
    uldat i;

    switch (Msg->Event.EventGadget.Code) {
      case COD_D_REMOVE:
	if ((i = DisplayWin->CurY) < DisplayWin->MaxNumRow) {
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
	    
    

static void BuiltinH(msgport *MsgPort) {
    msg *Msg;
    event_any *Event;
    screen *Screen;
    window *NewWindow, *tempWin;
    udat Code;
    byte /*FontHeight,*/ Flags;
    
    Screen = All->FirstScreen;
    
    while ((Msg = Builtin_MsgPort->FirstMsg)) {
	Remove(Msg);

	Event = &Msg->Event;
	if (Msg->Type == MSG_WINDOW_GADGET) {
	    tempWin = Event->EventGadget.Window;
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
	    else if (tempWin == DisplayWin)
		DisplayGadgetH(Msg);
	}
	else if (Msg->Type==MSG_MENU_ROW) {
	    if (Event->EventMenu.Menu==Builtin_Menu) {
		Code=Event->EventMenu.Code;
		Flags=All->SetUp->Flags;
		switch (Code) {
		  case COD_OPTION_WIN:
		  case COD_BUTTONS_WIN:
		  case COD_ABOUT_WIN:
		  case COD_DISPLAY_WIN:
		    if (Code == COD_OPTION_WIN)
			UpdateOptionWin(), NewWindow = OptionWin;
		    else if (Code == COD_BUTTONS_WIN)
			UpdateButtonWin(), NewWindow = ButtonWin;
		    else if (Code == COD_ABOUT_WIN)
			NewWindow = AboutWin;
		    else if (Code == COD_DISPLAY_WIN)
			UpdateDisplayWin(DisplayWin), NewWindow = DisplayWin;
		    else
			break;
		    
		    if (NewWindow->Screen)
			Act(UnMap,NewWindow)(NewWindow);
		    NewWindow->Left=Screen->Left+(udat)20;
		    NewWindow->Up=Screen->Up+(udat)6;
		    Act(Map,NewWindow)(NewWindow, Screen);
		    break;
		  case COD_CLOCK_WIN:
		    NewWindow = ClockWin;
		    if (NewWindow->Screen)
			Act(UnMap,NewWindow)(NewWindow);
		    NewWindow->Left=Screen->Left;
		    NewWindow->Up=Screen->Up;
		    Act(Map,NewWindow)(NewWindow, Screen);
		    Builtin_MsgPort->WakeUp = TIMER_ALWAYS;
		    break;

		  case COD_QUIT:
		    Quit(0);
		    break;
		    
		  case COD_SUSPEND:
		    SuspendHW(TRUE);
		    
		    kill(getpid(), SIGSTOP);
		    
		    (void)RestartHW(TRUE);
		    break;

		  case COD_DETACH:
		    QuitHW();
		    break;

		  case COD_WM_RESTART:
		    SendControlMsg(Ext(WM,MsgPort), MSG_CTRL_RESTART, 0, NULL);
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
	} else if (Msg->Event.EventCommon.Window == WinList ||
		   Msg->Event.EventCommon.Window == DisplayWin) {
	    
	    if (Msg->Event.EventCommon.Window == WinList &&
		Msg->Type==MSG_WINDOW_KEY) {
		
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
	    } else if (Msg->Type==MSG_WINDOW_MOUSE) {
		dat EventMouseX = Msg->Event.EventMouse.X, EventMouseY = Msg->Event.EventMouse.Y;
		window *CurrWin = Msg->Event.EventCommon.Window;
		byte temp = EventMouseX >= 0 && EventMouseX <= CurrWin->XWidth-2
		    && EventMouseY >= 0 && EventMouseY <= CurrWin->YWidth-2
		    && (uldat)EventMouseY+CurrWin->YLogic < (uldat)CurrWin->MaxNumRow;

		SelectRowWindow(CurrWin, temp ? (uldat)EventMouseY+CurrWin->YLogic : MAXULDAT);
		
		if (CurrWin == WinList &&
		    isRELEASE(Msg->Event.EventMouse.Code)) {
		    if (temp)
			SelectWinList();
		}
	    }
	}
	Delete(Msg);
    }
    if (Builtin_MsgPort->WakeUp)
	Clock_Update();
}

void FullUpdateWinList(window *listWin);

void InstallRemoveWinListHook(window *listWin) {    
    if (listWin == WinList) {
	if (WinList->Screen)
	    Act(InstallHook,WinList)(WinList, FullUpdateWinList, &WinList->Screen->FnHookWindow);
	else
	    Act(RemoveHook,WinList)(WinList, FullUpdateWinList, WinList->WhereHook);
    }
}

void UpdateWinList(void) {
    screen *Screen = All->FirstScreen;
    window *Window = Screen->FirstWindow;
    
    DeleteList(WinList->FirstRow);
    WinList->CurX = WinList->CurY = 0;
    
    WinList->XWidth = WinList->MinXWidth;
    WinList->YWidth = WinList->MinYWidth;
    
    for (; Window; Window = Window->Next) {
	if (Window == WinList || (Window->Attrib & WINDOW_MENU))
	    continue;
	Row4Menu(WinList, (udat)0, ROW_ACTIVE, Window->LenTitle, Window->Title);
    }
}

void FullUpdateWinList(window *listWin) {
    if (listWin == WinList && WinList->Screen) {
	ResizeRelWindow(WinList, WinList->MinXWidth - WinList->XWidth, WinList->MinYWidth - WinList->YWidth);
    
	UpdateWinList();
	
	DrawAreaWindow(WinList, FALSE);
    }
}

byte InitBuiltin(void) {
    window *Window;
    byte *s, *greeting = "\n"
	"                TWIN             \n"
	"        Text WINdows manager     \n\n"
	"          Version " TWIN_VERSION_STR " by       \n\n"
	"        Massimiliano Ghilardi    \n\n"
	"         <max@Linuz.sns.it>      ";
    uldat grlen = strlen(greeting);
    
    if ((Builtin_MsgPort=Do(Create,MsgPort)
	 (FnMsgPort, 5, "Builtin", (time_t)0, (frac_t)0, 0, BuiltinH)) &&
	(Builtin_Menu=Do(Create,Menu)
	 (FnMenu, Builtin_MsgPort, (byte)0x70, (byte)0x20, (byte)0x78, (byte)0x08, (byte)0x74, (byte)0x24, (byte)0)) &&
	Info4Menu(Builtin_Menu, ROW_ACTIVE, (uldat)42, " Hit PAUSE or Mouse Right Button for Menu ", "tttttttttttttttttttttttttttttttttttttttttt") &&
	
	(Window=Win4Menu(Builtin_Menu)) &&
	Row4Menu(Window, COD_CLOCK_WIN,  ROW_ACTIVE, 9, " Clock   ") &&
	Row4Menu(Window, COD_OPTION_WIN, ROW_ACTIVE, 9, " Options ") &&
	Row4Menu(Window, COD_BUTTONS_WIN,ROW_ACTIVE, 9, " Buttons ") &&
	Row4Menu(Window, COD_DISPLAY_WIN,ROW_ACTIVE, 9, " Display ") &&
	Row4Menu(Window, COD_ABOUT_WIN,  ROW_ACTIVE, 9, " About   ") &&
	Item4Menu(Builtin_Menu, Window, TRUE, 3, " ð ") &&
	
	(Window=Win4Menu(Builtin_Menu)) &&
#if defined(CONF_TERM)
	Row4Menu(Window, COD_SPAWN,  ROW_ACTIVE,11, " New Term  ") &&
#elif defined(CONF__MODULES)
	Row4Menu(Window, COD_SPAWN,ROW_INACTIVE,11, " New Term  ") &&
#endif
#if defined(CONF_WM_RC) || defined(CONF__MODULES)
	Row4Menu(Window, COD_WM_RESTART,ROW_ACTIVE,11," Reload RC ") &&
#endif
	Row4Menu(Window, (udat)0,    ROW_IGNORE,11, "ÄÄÄÄÄÄÄÄÄÄÄ") &&
	Row4Menu(Window, COD_DETACH, ROW_ACTIVE,11, " Detach    ") &&
	Row4Menu(Window, COD_SUSPEND,ROW_ACTIVE,11, " Suspend   ") &&
	Row4Menu(Window, COD_QUIT,   ROW_ACTIVE,11, " Quit      ") &&
	(Builtin_File=Item4Menu(Builtin_Menu, Window, TRUE, 6, " File ")) &&
	
	(Window=Win4Menu(Builtin_Menu)) &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,16," Undo           ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,16," Redo           ") &&
	Row4Menu(Window, (udat)0, ROW_IGNORE,  16,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,16," Cut            ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,16," Copy           ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,16," Paste          ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,16," Clear          ") &&
	Row4Menu(Window, (udat)0, ROW_IGNORE,  16,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ") &&
	Row4Menu(Window, (udat)0, ROW_INACTIVE,16," Show Clipboard ") &&
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
	 (FnWindow, (udat)5, "About", "\x7F\x7F\x7F\x7F\x7F", Builtin_Menu, COL(BLACK,WHITE),
	  NOCURSOR, WINDOW_WANT_MOUSE|WINDOW_DRAG|WINDOW_CLOSE, WINFL_USE_DEFCOL,
	  (udat)40, (udat)15, (udat)0)) &&

	(ClockWin = Do(Create,Window)
	 (FnWindow, (udat)5, "Clock", NULL, Builtin_Menu, COL(YELLOW,BLUE),
	  NOCURSOR, WINDOW_DRAG|WINDOW_CLOSE, WINFL_USE_DEFCOL,
	  (udat)12, (udat)4, (udat)0)) &&

	(OptionWin = Do(Create,Window)
	 (FnWindow, (udat)7, "Options", NULL, Builtin_Menu, COL(HIGH|BLACK,BLACK),
	  NOCURSOR, WINDOW_WANT_MOUSE|WINDOW_DRAG|WINDOW_CLOSE,WINFL_USE_DEFCOL,
	  (udat)42, (udat)16,(udat)0)) &&

	(ButtonWin = Do(Create,Window)
	 (FnWindow, (udat)7, "Buttons", NULL, Builtin_Menu, COL(HIGH|BLACK,BLACK),
	  NOCURSOR, WINDOW_WANT_MOUSE|WINDOW_DRAG|WINDOW_CLOSE, WINFL_USE_DEFCOL,
	  (udat)42, (udat)11, (udat)0)) &&

	(DisplayWin = Do(Create,Window)
	 (FnWindow, (udat)7, "Display", NULL, Builtin_Menu, COL(HIGH|BLACK,WHITE),
	  NOCURSOR,
	  WINDOW_DRAG|WINDOW_RESIZE|WINDOW_CLOSE
	  |WINDOW_X_BAR|WINDOW_Y_BAR,
	  WINFL_USE_DEFCOL|WINFL_SEL_ROWCURR,
	  (udat)33, (udat)12,(udat)0)) &&

	(WinList = Do(Create,Window)
	 (FnWindow, (udat)11, "Window List", NULL, Builtin_Menu, COL(WHITE,BLUE),
	  NOCURSOR,
	  WINDOW_WANT_KEYS|WINDOW_WANT_MOUSE|WINDOW_DRAG|WINDOW_RESIZE|WINDOW_CLOSE
	  |WINDOW_X_BAR|WINDOW_Y_BAR,
	  WINFL_SEL_ROWCURR|WINFL_USE_DEFCOL,
	  (udat)17, (udat)9, (udat)0)) &&

	Act(WriteRow,AboutWin)(AboutWin, grlen, greeting) &&
	
	(ButtonOK_About=Do(CreateEmptyButton,Gadget)(FnGadget, AboutWin, (udat)8, (udat)1, (byte)0x70)) &&

	(ButtonRemove=Do(CreateEmptyButton,Gadget)(FnGadget, DisplayWin, (udat)8, (udat)1, (byte)0x70)) &&
	(ButtonThis  =Do(CreateEmptyButton,Gadget)(FnGadget, DisplayWin, (udat)8, (udat)1, (byte)0x70)) &&

	Do(Create,Gadget)(FnGadget, DisplayWin, COL(BLACK,WHITE), COL(BLACK,WHITE),
			  COL(BLACK,WHITE), COL(BLACK,WHITE),
			  0, GADGET_USE_DEFCOL|GADGET_DISABLED, 0, 0, 11, 8,
			  ((s = AllocMem(11*8)), WriteMem(s, ' ', 11*8), s),
			  NULL, NULL, NULL, NULL, NULL, NULL, NULL) &&
	
	Do(Create,Gadget)(FnGadget, OptionWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  COD_O_SHADOWS, GADGET_USE_DEFCOL, 2, 1, 11, 1,
			  "[ ] Shadows", NULL, NULL, NULL, NULL, NULL, NULL, NULL) &&

	Do(Create,Gadget)(FnGadget, OptionWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  COD_O_Xn_SHADE, GADGET_USE_DEFCOL, 18, 1, 3, 1,
			  "[-]", NULL, NULL, NULL, NULL, NULL, NULL, NULL) &&
	
	Do(Create,Gadget)(FnGadget, OptionWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  COD_O_Xp_SHADE, GADGET_USE_DEFCOL, 21, 1, 3, 1,
			  "[+]", NULL, NULL, NULL, NULL, NULL, NULL, NULL) &&

	Do(Create,Gadget)(FnGadget, OptionWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  COD_O_Yn_SHADE, GADGET_USE_DEFCOL, 18, 2, 3, 1,
			  "[-]", NULL, NULL, NULL, NULL, NULL, NULL, NULL) &&
	
	Do(Create,Gadget)(FnGadget, OptionWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  COD_O_Yp_SHADE, GADGET_USE_DEFCOL, 21, 2, 3, 1,
			  "[+]", NULL, NULL, NULL, NULL, NULL, NULL, NULL) &&
	
	Do(Create,Gadget)(FnGadget, OptionWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  COD_O_ALWAYSCURSOR, GADGET_USE_DEFCOL, 2, 4, 22, 1,
			  "[ ] Always Show Cursor", NULL, NULL, NULL, NULL, NULL, NULL, NULL) &&

	Do(Create,Gadget)(FnGadget, OptionWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  COD_O_BLINK, GADGET_USE_DEFCOL, 2, 6, 32, 1,
			  "[ ] Enable Blink/High Background", NULL, NULL, NULL, NULL, NULL, NULL, NULL) &&

	Do(Create,Gadget)(FnGadget, OptionWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  COD_O_HIDEMENU, GADGET_USE_DEFCOL, 2, 8, 15, 1,
			  "[ ] Hidden Menu", NULL, NULL, NULL, NULL, NULL, NULL, NULL) &&

	Do(Create,Gadget)(FnGadget, OptionWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  COD_O_EDGESCROLL, GADGET_USE_DEFCOL, 2, 10, 27, 1,
			  "[ ] Enable Screen Scrolling", NULL, NULL, NULL, NULL, NULL, NULL, NULL) &&

	Do(Create,Gadget)(FnGadget, OptionWin, COL(BLACK,WHITE), COL(HIGH|WHITE,GREEN),
			  COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
			  COD_O_ALTFONT, GADGET_USE_DEFCOL, 2, 12, 15, 1,
			  "[ ] Custom Font", NULL, NULL, NULL, NULL, NULL, NULL, NULL)
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
	Act(Configure,WinList)(WinList, 1<<2 | 1<<3, 0, 0, (udat)17, (udat)4, 0, 0);

	Act(SetColors,DisplayWin)(DisplayWin, 0x1FF, (hwcol)0x7A, (hwcol)0x7F, (hwcol)0x79, (hwcol)0xF9, (hwcol)0x7F,
				  (hwcol)0x70, (hwcol)0x20, (hwcol)0x78, (hwcol)0x08);

	Act(InstallHook,DisplayWin)(DisplayWin, UpdateDisplayWin, &All->FnHookDisplayHW);
	WinList->MapUnMapHook = InstallRemoveWinListHook;
	
	Act(FillButton,ButtonOK_About)(ButtonOK_About, COD_OK, (udat)15, (udat)11, (udat)0, "   OK   ", (byte)0x2F, (byte)0x28);

	Act(FillButton,ButtonRemove)(ButtonRemove, COD_D_REMOVE, (udat)1, (udat)2, (udat)0, " Remove ", (byte)0x2F, (byte)0x28);
	Act(FillButton,ButtonThis)  (ButtonThis,   COD_D_THIS,   (udat)1, (udat)5, (udat)0, "  This  ", (byte)0x2F, (byte)0x28);

	OptionWin->CurX = 25; OptionWin->CurY = 1;
	Act(WriteRow,OptionWin)(OptionWin, 10, "  X Shadow");
	OptionWin->CurX = 25; OptionWin->CurY = 2;
	Act(WriteRow,OptionWin)(OptionWin, 10, "  Y Shadow");

	All->BuiltinMenu=Builtin_Menu;

	return TRUE;
    }
    fprintf(stderr, "twin: InitBuiltin(): Out of memory!\n");
    return FALSE;
}

