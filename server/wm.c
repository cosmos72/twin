/*
 *  wm.c  --  builtin window manager for twin
 *
 *  Copyright (C) 1993-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twinsys.h"
#include "twin.h"
#include "data.h"
#include "methods.h"

#include "resize.h"
#include "draw.h"
#include "main.h"
#include "util.h"
#include "scroller.h"
#include "builtin.h"
#include "hw.h"
#include "hw_multi.h"
#include "common.h"
#include "libTwkeys.h"

msgport *WM_MsgPort;
static msgport *MapQueue;

static void SmartPlace(window *Window, screen *Screen);

static void CommonKeyAction(window *CurrWin, event_keyboard *EventK) {
    uldat NumRow, OldNumRow;
	
    switch (EventK->Code) {
      case TW_Up:
	if (!CurrWin->MaxNumRow)
	    break;
	OldNumRow=CurrWin->CurY;
	if (OldNumRow<MAXULDAT) {
	    if (!OldNumRow)
		NumRow=CurrWin->MaxNumRow-(uldat)1;
	    else
		NumRow=OldNumRow-(uldat)1;
	    CurrWin->CurY=NumRow;
	    if (CurrWin->Flags & WINFL_SEL_ROWCURR)
		DrawTextWindow(CurrWin, (uldat)0, OldNumRow, (uldat)MAXUDAT-(uldat)2, OldNumRow);
	}
	else
	    CurrWin->CurY=NumRow=CurrWin->MaxNumRow-(uldat)1;
	if (CurrWin->Flags & WINFL_SEL_ROWCURR)
	    DrawTextWindow(CurrWin, (uldat)0, NumRow, (uldat)MAXUDAT-(uldat)2, NumRow);
	UpdateCursor();
	break;
      case TW_Down:
	if (!CurrWin->MaxNumRow)
	    break;
	OldNumRow=CurrWin->CurY;
	if (OldNumRow<MAXULDAT) {
	    if (OldNumRow>=CurrWin->MaxNumRow-(uldat)1)
		NumRow=(uldat)0;
	    else
		NumRow=OldNumRow+(uldat)1;
	    CurrWin->CurY=NumRow;
	    if (CurrWin->Flags & WINFL_SEL_ROWCURR)
		DrawTextWindow(CurrWin, (uldat)0, OldNumRow, (uldat)MAXUDAT-(uldat)2, OldNumRow);
	}
	else
	    CurrWin->CurY=NumRow=(uldat)0;
	if (CurrWin->Flags & WINFL_SEL_ROWCURR)
	    DrawTextWindow(CurrWin, (uldat)0, NumRow, (uldat)MAXUDAT-(uldat)2, NumRow);
	UpdateCursor();
	break;
      case TW_Left:
	if (CurrWin->CurX) {
	    CurrWin->CurX--;
	    UpdateCursor();
	}
	break;
      case TW_Right:
	if (((CurrWin->Flags & WINFL_USECONTENTS) && CurrWin->CurX < CurrWin->XWidth - 3) ||
	    (!(CurrWin->Flags & WINFL_USEANY) && CurrWin->CurX < MAXULDAT - 1)) {
	    CurrWin->CurX++;
	    UpdateCursor();
	}
	break;
      default:
	break;
    }
}

	    
static byte CheckForwardMsg(msg *Msg, byte WasUsed) {
    static window *CurrWin = (window *)0;
    static byte ClickedInside = FALSE, LastKeys = 0;
    screen *Screen=All->FirstScreen;
    window *FocusWin=Screen->FocusWindow;
    event_any *Event;
    
    if ((All->FlagsMove & GLMOVE_1stWIN_FREEZE) ||
	(Msg->Type != MSG_KEY && Msg->Type != MSG_MOUSE)) {
	return FALSE;
    }
    
    if (FocusWin != CurrWin) {
	/* try to leave CurrWin with a clean status */
	msg *NewMsg;
	udat Code;
	if (ClickedInside && CurrWin && (CurrWin->Attrib & WINDOW_WANT_MOUSE)) {
	    
	    while (LastKeys) {
		if (!(NewMsg=Do(Create,Msg)(FnMsg, MSG_WINDOW_MOUSE, sizeof(event_mouse))))
		    LastKeys = 0, Error(NOMEMORY);
		else {
		    Event=&NewMsg->Event;
		    Event->EventMouse.Window = CurrWin;
		    Event->EventMouse.ShiftFlags = (udat)0;
		    Code = LastKeys & HOLD_LEFT ? (LastKeys &= ~HOLD_LEFT, RELEASE_LEFT) :
			LastKeys & HOLD_MIDDLE ? (LastKeys &= ~HOLD_MIDDLE, RELEASE_MIDDLE) :
			LastKeys & HOLD_RIGHT ? (LastKeys &= ~HOLD_RIGHT, RELEASE_RIGHT) : 0;
		    Event->EventMouse.Code = Code | LastKeys;
		    Event->EventMouse.X = -1;
		    Event->EventMouse.Y = -1;
		    SendMsg(CurrWin->Menu->MsgPort, NewMsg);
		}
	    }
	}
	ClickedInside = FALSE;
    }
    
    CurrWin = !FocusWin || (FocusWin->Attrib & WINDOW_MENU) || !FocusWin->Menu ?
	    (window *)0 : FocusWin;
	
    if (!CurrWin || !CurrWin->Screen || (CurrWin->Attrib & WINDOW_MENU)
	|| !CurrWin->Menu)
	return FALSE;
    
    Event = &Msg->Event;
	
    if (Msg->Type == MSG_KEY) {
	if (!WasUsed && !All->FlagsMove) {
	    if (CurrWin->Attrib & WINDOW_WANT_KEYS) {
		Msg->Type = MSG_WINDOW_KEY;
		Event->EventKeyboard.Window = CurrWin;
		SendMsg(CurrWin->Menu->MsgPort, Msg);
		return TRUE;
	    } else
		CommonKeyAction(CurrWin, &Event->EventKeyboard);
	}
	return FALSE;
    }

    {
	ldat Left, Up, Rgt, Dwn, NWinDiMenu;
	dat x, y;
	udat Code;
	byte Inside;
	
	NWinDiMenu=CurrWin->Attrib & WINDOW_MENU ? 0 : (ldat)~(ldat)0;
	Up=(ldat)CurrWin->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)Screen->YLimit;
	Left=(ldat)CurrWin->Left-((ldat)Screen->Left & NWinDiMenu);
	Rgt=Left+(ldat)CurrWin->XWidth-(ldat)1;
	Dwn=Up+(ldat)CurrWin->YWidth-(ldat)1;
	x = Event->EventMouse.X;
	y = Event->EventMouse.Y;
    
	Inside = x>Left && x<Rgt && y>Up && y>=Screen->YLimit && y<Dwn;
	Code = Event->EventMouse.Code;
	
	if (isSINGLE_PRESS(Code))
	    ClickedInside = Inside;
	
	if (ClickedInside && (Inside || !isDRAG(Code))) {

	    if (isSINGLE_RELEASE(Code))
		ClickedInside = FALSE;

	    /* manage window hilight and Selection */
	    if (!(CurrWin->Attrib & WINDOW_WANT_MOUSE)) {
		if (isPRESS(Code) && Code == DOWN_LEFT)
		    StartHilight(CurrWin, (ldat)x-Left-1+CurrWin->XLogic, (ldat)y-Up-1+CurrWin->YLogic);
		else if (isDRAG(Code) && Code == (DRAG_MOUSE|HOLD_LEFT)) {
		    ExtendHilight(CurrWin, (ldat)x-Left-1+CurrWin->XLogic, (ldat)y-Up-1+CurrWin->YLogic);
		} else if (isRELEASE(Code) && Code == RELEASE_LEFT) {
		    SetSelectionFromWindow(CurrWin);
		} else if (isRELEASE(Code) && Code == RELEASE_MIDDLE) {
		    /* send Selection Paste msg */
		    msg *NewMsg;
		    
		    /* store selection owner */
		    SelectionImport();
		    
		    if ((NewMsg = Do(Create,Msg)(FnMsg, MSG_SELECTION, sizeof(event_selection)))) {
			Event = &NewMsg->Event;
			Event->EventSelection.Window = CurrWin;
			Event->EventSelection.Code = 0;
			Event->EventSelection.pad = (udat)0;
			Event->EventSelection.X = x;
			Event->EventSelection.Y = y;
			SendMsg(CurrWin->Menu->MsgPort, NewMsg);
		    }
		}
	    }
	    /* finished with Selection stuff */
	    
	    if (CurrWin->Attrib & WINDOW_WANT_MOUSE) {
		Msg->Type=MSG_WINDOW_MOUSE;
		Event->EventMouse.Window = CurrWin;
		Event->EventMouse.X -= Left+1;
		Event->EventMouse.Y -= Up+1;
		SendMsg(CurrWin->Menu->MsgPort, Msg);
		LastKeys = Code & HOLD_ANY;
		return TRUE;
	    }
	}
    }
    return FALSE;
}


static void Check4Resize(window *CurrWin) {
    msg *Msg;
    event_any *Event;
    
    if ((CurrWin->Attrib & WINDOW_WANT_CHANGE) &&
	(!(CurrWin->Flags & WINFL_USECONTENTS) ||
	 CurrWin->XWidth != CurrWin->TtyData->SizeX+2 || CurrWin->YWidth != CurrWin->TtyData->SizeY+2)) {
				
	if ((Msg=Do(Create,Msg)(FnMsg, MSG_WINDOW_CHANGE, sizeof(event_window)))) {
	    Event=&Msg->Event;
	    Event->EventWindow.Window = CurrWin;
	    Event->EventWindow.Code   = (udat)0; /* not used */
	    Event->EventWindow.XWidth = CurrWin->XWidth;
	    Event->EventWindow.YWidth = CurrWin->YWidth;
	    SendMsg(CurrWin->Menu->MsgPort, Msg);
	} else
	    Error(NOMEMORY);
    }
    if ((CurrWin->Flags & WINFL_USECONTENTS))
	CheckResizeWindowContents(CurrWin);
}

static void CommonMenuAction(msg *Msg) {
    window *CurrWin=Msg->Event.EventCommon.Window;
    screen *Screen;
    udat Code=Msg->Event.EventCommon.Code;
    msg *NewMsg;

    if (Code != COD_COMMON_WINLIST &&
	(!CurrWin || !(Screen=CurrWin->Screen) ||
	 !Screen->FocusWindow || CurrWin != Screen->FocusWindow))
	return;
    
    switch (Code) {
	
      case COD_COMMON_CLOSE:
	if ((NewMsg=Do(Create,Msg)(FnMsg, MSG_WINDOW_GADGET, sizeof(event_gadget)))) {
	    NewMsg->Event.EventGadget.Window = CurrWin;
	    NewMsg->Event.EventGadget.Code = (udat)0; /* COD_CLOSE */
	    SendMsg(CurrWin->Menu->MsgPort, NewMsg);
	} else
	    Error(NOMEMORY);
	break;
		
      case COD_COMMON_RESIZE:
	if (CurrWin->Attrib & (WINDOW_DRAG|WINDOW_RESIZE)) {
	    All->FlagsMove = GLMOVE_RESIZE_1stWIN;
	    /* not GLMOVE_1stWIN or window will miss MSG_WINDOW_CHANGE events. */
	    DrawBorderWindow(CurrWin, BORDER_ANY);
	}
	break;

      case COD_COMMON_SCROLL:
	if (CurrWin->Attrib & (WINDOW_X_BAR | WINDOW_Y_BAR)) {
	    All->FlagsMove = GLMOVE_SCROLL_1stWIN;
	    DrawBorderWindow(CurrWin, BORDER_ANY);
	}
	break;

      case COD_COMMON_CENTER:
      case COD_COMMON_ZOOM:
      case COD_COMMON_MAXZOOM:
	if (CurrWin->Attrib & (WINDOW_DRAG))
	    CenterWindow(CurrWin);
	
	if (Code == COD_COMMON_CENTER || !(CurrWin->Attrib & WINDOW_RESIZE))
	    break;
	
	if (Code == COD_COMMON_ZOOM) {
	    CurrWin->Left = Screen->Left;
	    CurrWin->Up   = Screen->Up;
	    CurrWin->XWidth = Screen->ScreenWidth;
	    CurrWin->YWidth = Screen->ScreenHeight - Screen->YLimit;
	} else {
	    if (!Screen->Up)  Screen->Up++;
	    CurrWin->Left   = Screen->Left - 1;
	    CurrWin->Up     = Screen->Up - 1;
	    CurrWin->XWidth = Screen->ScreenWidth + 2;
	    CurrWin->YWidth = Screen->ScreenHeight + 2 - Screen->YLimit;
	} 
	Check4Resize(CurrWin);
	DrawArea(FULLSCREEN);
	UpdateCursor();
	break;
		
      case COD_COMMON_REFRESH:
	RefreshVideo();
	break;

      case COD_COMMON_HOTKEY:
	SendHotKey(CurrWin);
	break;

      case COD_COMMON_NEXT:
	MakeLastWindow(CurrWin, TRUE);
	break;

      case COD_COMMON_WINLIST:
	if (ListWin->Screen)
	    Act(UnMap,ListWin)(ListWin);
	if (CurrWin && CurrWin->Screen)
	    Screen = CurrWin->Screen;
	else
	    Screen = All->FirstScreen;
	SmartPlace(ListWin, Screen);
	break;

      case COD_COMMON_RAISELOWER:
	if (CurrWin == Screen->FirstWindow)
	    MakeLastWindow(CurrWin, FALSE);
	else
	    MakeFirstWindow(CurrWin, FALSE);
	break;

      case COD_COMMON_UNFOCUS:
	Act(Focus,CurrWin)((window *)0);
	DrawBorderWindow(CurrWin, BORDER_ANY);
	DrawMenuBar(Screen, MINDAT, MAXDAT);
	UpdateCursor();
	break;

      default:
	break;
    }
}


static void DeActivateMenu(screen *Screen, menu *Menu, menuitem *CurrMenuItem, dat i, dat j, window *CurrWin) {
    window *tempWin;
    byte FlagsMove = All->FlagsMove;
    row *tempRow;
    msg *NewMsg;
    event_any *tempEvent;
    byte dummyShade;
    
    if (FlagsMove & GLMOVE_BY_MOUSE) {
	if ((tempWin = CurrMenuItem
	     && Act(SearchWindow,Screen)(Screen, i, j, &dummyShade)==CurrWin
	     ? CurrWin : (window *)0))
	    tempRow=Act(SearchRow,tempWin)(tempWin, CurrWin->CurY);
    } else {
	if ((tempWin = CurrMenuItem ? CurrWin : (window *)0))
	    tempRow=Act(SearchRow,CurrWin)(CurrWin, CurrWin->CurY);
    }
    
    if (tempWin && tempWin->CurY<MAXULDAT && CurrMenuItem->FlagActive
	&& tempRow && tempRow->Flags & ROW_ACTIVE && tempRow->Code) {
	
	if (!(NewMsg=Do(Create,Msg)(FnMsg, MSG_MENU_ROW, sizeof(event_menu))))
	    Error(NOMEMORY);
	else {
	    tempEvent=&NewMsg->Event;
	    tempEvent->EventMenu.Window = Screen->MenuWindow;
	    tempEvent->EventMenu.Code = tempRow->Code;
	    tempEvent->EventMenu.Menu = Menu;
	    if (tempRow->Code < COD_COMMON)
		SendMsg(Menu->MsgPort, NewMsg);
	    else
		/* handle COD_COMMON_* codes internally */
		SendMsg(WM_MsgPort, NewMsg);
	}
    }
}

static void DeActivateScreen(screen *Screen) {
    Screen->Attrib &= ~(GADGET_BACK_SELECT|GADGET_PRESSED);
    if (Screen != All->LastScreen) {
	MoveLast(Screen, All, Screen);
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0,
		 0, (dat)Min2(Screen->YLimit, All->FirstScreen->YLimit)-(dat)1,
		 MAXDAT, MAXDAT, FALSE);
	UpdateCursor();
    }
}

/* the Window Manager built into Twin */
static void WManagerH(msgport *MsgPort) {
    msg *Msg, *NewMsg;
    event_any *Event, *tempEvent;
    window *CurrWin, *tempWin;
    udat Funct = 0, CallKey;
    dat EventMouseX, EventMouseY;
    static ldat DragPosition;
    udat temp;
    udat Repeat;
    uldat ltemp;
    setup *SetUp;
    screen *Screen, *tempScreen;
    menu *Menu;
    menuitem *CurrMenuItem, *tempMenuItem;
    gadget *Gadget;
    ldat Left, Up, Rgt, Dwn, NWinDiMenu;
    uldat NumRow, NumLogicMax, OldNumRow, Delta;
    dat i, j, XDelta, YDelta;
    byte FlagsMove, State, CharFound;
    byte Shade;
    dat ScreenWidth;
    udat YLimit;
    
    while ((Msg=WM_MsgPort->FirstMsg)) {
	
	Remove(Msg);
	
	if (Msg->Type==MSG_MAP) {
	    SendMsg(MapQueue, Msg);
	    continue;
	} else if (Msg->Type==MSG_MENU_ROW && (temp=Msg->Event.EventMenu.Code) >= COD_COMMON) {
	    CommonMenuAction(Msg);
	    Delete(Msg);
	    continue;
	} else if (Msg->Type!=MSG_MOUSE && Msg->Type!=MSG_KEY) {
	    Delete(Msg);
	    continue;
	}
	
	Event=&Msg->Event;
	CallKey=Event->EventKeyboard.Code;
	FlagsMove=All->FlagsMove;
	
	if (FlagsMove & GLMOVE_1stSCREEN)
	    State=STATE_SCREEN;
	else if (FlagsMove & GLMOVE_1stMENU)
	    State=STATE_MENU;
	else if (FlagsMove & GLMOVE_SCROLL_1stWIN && Msg->Type==MSG_KEY) 
	    State=STATE_SCROLL;
	else if (FlagsMove & GLMOVE_ANY_1stWIN)
	    State=STATE_WINDOW;
	else
	    State=STATE_DEFAULT;
	
	/*
	 Funziona solo se GlobalKeyCodes e GlobalMouseCodes
	 hanno formato normale (non compresso)
	 */
	
	if (Msg->Type==MSG_MOUSE) {
	    if (All->GlobalMouseCodes[State][0])		/* Compressed Format : not implemented */
		Funct = (udat)0;
	    else
		Funct = FnM(All->GlobalMouseCodes[State],CallKey);
	} else if (CallKey < ' ' || CallKey > '~') {
	    if (All->GlobalKeyCodes[State][0])			/* Compressed Format */
		switch (All->GlobalKeyCodes[State][0]) {
		  case 1:					/* parallel list */
		    for (temp = 1; (Repeat = All->GlobalKeyCodes[State][temp]); temp += 2)
			if (Repeat == CallKey) {
			    Funct = All->GlobalKeyCodes[State][temp+1];
			    break;
			}
		    break;
		  default:
		    break;
		}
	    else
		Funct = FnK(All->GlobalKeyCodes[State],CallKey);
	}
	
	if (Funct) {
	    Screen=All->FirstScreen;
	    ScreenWidth=Screen->ScreenWidth;
	    YLimit=Screen->YLimit;
	    if ((CurrWin=Screen->FocusWindow)) {
		NWinDiMenu=CurrWin->Attrib & WINDOW_MENU ? 0 : (ldat)~(ldat)0;

		Up=(ldat)CurrWin->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)YLimit;
		Left=(ldat)CurrWin->Left-((ldat)Screen->Left & NWinDiMenu);
		Rgt=Left+(ldat)CurrWin->XWidth-(ldat)1;
		Dwn=Up+(ldat)CurrWin->YWidth-(ldat)1;

	    } else
		NWinDiMenu=TRUE;
	    
	    if ((Menu=Act(SearchMenu,Screen)(Screen)))
		CurrMenuItem=Menu->MenuItemSelect;
	    
	    FlagsMove=All->FlagsMove;
	    SetUp=All->SetUp;
	    Shade=!!(SetUp->Flags & SETUP_DO_SHADE);
	    if (Msg->Type==MSG_MOUSE) {
		EventMouseX=Event->EventMouse.X;
		EventMouseY=Event->EventMouse.Y;
	    }
	    else {
		Repeat=1;
		EventMouseX=EventMouseY=(dat)0;
	    }
	    
	    if (State==STATE_WINDOW && EventMouseY<(dat)YLimit)
		EventMouseY=(dat)YLimit;
	    
	    
	    i = (dat)EventMouseX;
	    j = (dat)EventMouseY;
	    
	    
	    switch (Funct) {
	      case STMENU_ACT_BACKTO_DEF:
	      case STWIN_BACKTO_DEF:
	      case STWIN_BACKTO_DEF_FREEZE:
	      case STWIN_BACKTO_NULL:
		if (FlagsMove & GLMOVE_ANY && ((FlagsMove & GLMOVE_BY_MOUSE) ? Msg->Type != MSG_MOUSE : Msg->Type != MSG_KEY))
		    break;
		
		if (Funct==STMENU_ACT_BACKTO_DEF && State==STATE_MENU) {
		    
		    DeActivateMenu(Screen, Menu, CurrMenuItem, i, j, CurrWin);
		    
		    FlagsMove=All->FlagsMove;
		    Funct=STWIN_BACKTO_DEF;
		}
		
		if (Funct==STWIN_BACKTO_DEF || Funct==STWIN_BACKTO_DEF_FREEZE) {
		    
		    if (All->FlagsMove & GLMOVE_RESIZE_1stWIN)
			temp = TRUE;
		    else
			temp = FALSE;
		    
		    if (Funct==STWIN_BACKTO_DEF_FREEZE)
			All->FlagsMove=GLMOVE_1stWIN_FREEZE;
		    else
			All->FlagsMove=(byte)0;
		    
		    if (State==STATE_SCREEN)
			DrawMenuBar(Screen, MINDAT, MAXDAT);
		    else if (State==STATE_MENU)
			ChangeMenuFirstScreen((menuitem *)0, GLMOVE_BY_MOUSE, DISABLE_MENU_FLAG);
		    else if (State==STATE_SCROLL)
			DrawBorderWindow(CurrWin, BORDER_ANY);
		    else if (State==STATE_WINDOW) {
			CurrWin->Attrib &= ~(GADGET_ANY_SELECT | SCROLL_ANY_SELECT);
			DrawBorderWindow(CurrWin, BORDER_ANY);

			if (temp)
			    Check4Resize(CurrWin);
		    }
		    else {
			if (j+1==(dat)YLimit && i>ScreenWidth-(dat)3 && Screen->Attrib & GADGET_BACK_SELECT)
			    DeActivateScreen(Screen);

			else if (CurrWin && (ldat)j==Up && (ldat)i>=Left && (ldat)i<Left+(ldat)2 && CurrWin->Attrib & WINDOW_CLOSE && CurrWin->Attrib & GADGET_CLOSE_SELECT) {
			    if (!(NewMsg=Do(Create,Msg)(FnMsg, MSG_WINDOW_GADGET, sizeof(event_gadget))))
				Error(NOMEMORY);
			    else if (CurrWin) {
				CurrWin->Attrib &= ~(GADGET_CLOSE_SELECT|GADGET_PRESSED);
				DrawBorderWindow(CurrWin, BORDER_ANY);
				tempEvent=&NewMsg->Event;
				tempEvent->EventGadget.Window = CurrWin;
				tempEvent->EventGadget.Code = (udat)0; /* COD_CLOSE */
				SendMsg(CurrWin->Menu->MsgPort, NewMsg);
			    }
			}
			else if (CurrWin && (ldat)j==Up && Rgt-(ldat)i<(ldat)2 && (ldat)i<=Rgt && CurrWin && !(CurrWin->Attrib & WINDOW_MENU) && CurrWin->Attrib & GADGET_BACK_SELECT) {
			    CurrWin->Attrib &= ~(GADGET_BACK_SELECT|GADGET_PRESSED);
			    
			    if (CurrWin == Screen->FirstWindow && CurrWin->Next)
				MakeLastWindow(CurrWin, TRUE);
			    else if (CurrWin->Prev)
				MakeFirstWindow(CurrWin, TRUE);
			    else
				DrawArea((screen *)0, (window *)0, CurrWin, (gadget *)0, (gadget *)0,
					 Rgt-1, Up, Rgt, Up, FALSE);
			}
			else if (Screen->Attrib & GADGET_BACK_SELECT) {
			    Screen->Attrib &= ~(GADGET_BACK_SELECT|GADGET_PRESSED);
			    DrawMenuBar(Screen, MINDAT, MAXDAT);
			}
			else if (CurrWin && CurrWin->Attrib & (GADGET_CLOSE_SELECT | GADGET_BACK_SELECT)) {
			    CurrWin->Attrib &= ~(GADGET_CLOSE_SELECT | GADGET_BACK_SELECT|GADGET_PRESSED);
			    DrawBorderWindow(CurrWin, BORDER_ANY);
			}
			else if (CurrWin && (Gadget=CurrWin->GadgetSelect)) {
			    if (Gadget==Act(SearchGadget,CurrWin)(CurrWin, i, j) &&
				!(Gadget->Flags & GADGET_DISABLED) && Gadget->Flags & GADGET_PRESSED
				&& Gadget->Code) {
				if (!(NewMsg=Do(Create,Msg)(FnMsg, MSG_WINDOW_GADGET, sizeof(event_gadget))))
				    Error(NOMEMORY);
				else {
				    tempEvent=&NewMsg->Event;
				    tempEvent->EventGadget.Window = CurrWin;
				    tempEvent->EventGadget.Code = Gadget->Code;
				    SendMsg(CurrWin->Menu->MsgPort, NewMsg);
				}
			    }
			    /* CurrWin->GadgetSelect=(gadget *)0; */
			    Gadget->Flags &= ~GADGET_PRESSED;
			    if (Gadget) {
				if (CurrWin == All->FirstScreen->FirstWindow)
				    DrawWindow(CurrWin, (gadget *)0, Gadget, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
				else
				    DrawArea((screen *)0, (window *)0, CurrWin, (gadget *)0, Gadget, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
			    }
			}
		    }
		}
		break;
		
	      case STDEF_MOUSE_ACT_SOME:
	      case STDEF_MOUSE_ACT_MENU:
	      case STDEF_MOUSE_ACT_NULL:
		if (!(tempScreen=Do(Search,Screen)(j)))
		    break;
		else if (tempScreen!=Screen) {
		    MoveFirst(Screen, All, tempScreen);
		    DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0,
			     0, (dat)Min2(Screen->YLimit, tempScreen->YLimit)-(dat)1,
			     MAXDAT, MAXDAT, FALSE);
		    UpdateCursor();
		    Screen=tempScreen;
		    ScreenWidth=Screen->ScreenWidth;
		    YLimit=Screen->YLimit;
		    if ((CurrWin=Screen->FocusWindow)) {
			NWinDiMenu=CurrWin->Attrib & WINDOW_MENU ? 0 : (ldat)~(ldat)0;
		    }
		    
		    if ((Menu=Act(SearchMenu,Screen)(Screen)))
			CurrMenuItem=Menu->MenuItemSelect;

		    State=STATE_DEFAULT;
		    All->FlagsMove=FlagsMove=GLMOVE_1stWIN_FREEZE;
		}
		
		if (FlagsMove & GLMOVE_1stSCREEN) {
		    All->FlagsMove &= ~GLMOVE_1stSCREEN;
		    DrawMenuBar(Screen, MINDAT, MAXDAT);
		}
		if (FlagsMove & GLMOVE_ANY_1stWIN) {
		    All->FlagsMove &= ~GLMOVE_ANY_1stWIN;
		    DrawBorderWindow(CurrWin, BORDER_ANY);
		}
		if (FlagsMove & GLMOVE_1stMENU) {
		    ChangeMenuFirstScreen((menuitem *)0, FALSE, DISABLE_MENU_FLAG);
		    CurrWin=Screen->FocusWindow;
		}
		FlagsMove=All->FlagsMove;

		tempWin=Act(SearchWindow,Screen)(Screen, i, j, &Shade);
		
		if (Funct==STDEF_MOUSE_ACT_MENU) {
		    if (Menu) {
			if (j+1==(dat)YLimit)
			    CurrMenuItem=Act(SearchMenuItem,Menu)(Menu, i);
			else
			    CurrMenuItem=(menuitem *)0;
			ChangeMenuFirstScreen(CurrMenuItem, TRUE, ACTIVATE_MENU_FLAG);
		    }
		}
		else if (Funct==STDEF_MOUSE_ACT_NULL) {
		    if (!tempWin && j >= (dat)YLimit) {
			/* open the Window List */
			Act(UnMap,ListWin)(ListWin);
			ListWin->Left = Screen->Left + i - ListWin->XWidth / 2;
			ListWin->Up = Screen->Up + (udat)j - YLimit;
			Act(RealMap,ListWin)(ListWin,Screen);
		    }
		}
		else if (j+1==(dat)YLimit) {
		    if (i>ScreenWidth-(dat)3) {
			tempScreen->Attrib |= GADGET_BACK_SELECT|GADGET_PRESSED;
			All->FlagsMove=FlagsMove=GLMOVE_1stWIN_FREEZE;
		    }
		    else
			All->FlagsMove=FlagsMove=GLMOVE_1stSCREEN | GLMOVE_BY_MOUSE;
		    DrawMenuBar(Screen, (dat)0, ScreenWidth-(dat)1);
		}
		else if (tempWin) {
		    if (tempWin!=CurrWin) {
			CurrWin=Act(Focus,tempWin)(tempWin);
			if (Screen->FocusWindow != tempWin)
			    /* failed for some reason */
			    break;
			DrawBorderWindow(tempWin, BORDER_ANY);
			DrawBorderWindow(CurrWin, BORDER_ANY);
			DrawMenuBar(Screen, MINDAT, MAXDAT);
			UpdateCursor();
			CurrWin = tempWin;
			All->FlagsMove=FlagsMove=GLMOVE_1stWIN_FREEZE;
			NWinDiMenu=CurrWin->Attrib & WINDOW_MENU ? 0 : (ldat)~(ldat)0;
		    }
		    
		    Up=(ldat)CurrWin->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)YLimit;
		    Left=(ldat)CurrWin->Left-((ldat)Screen->Left & NWinDiMenu);
		    Rgt=Left+(ldat)CurrWin->XWidth-(ldat)1;
		    Dwn=Up+(ldat)CurrWin->YWidth-(ldat)1;
		    
		    if ((ldat)i==Left || (ldat)i==Rgt || (ldat)j==Up || (ldat)j==Dwn) {
			if ((ldat)i>Rgt-(ldat)2 && (ldat)j==Dwn && CurrWin->Attrib & WINDOW_RESIZE) {
			    All->FlagsMove=FlagsMove=GLMOVE_RESIZE_1stWIN | GLMOVE_BY_MOUSE;
			    DrawBorderWindow(CurrWin, BORDER_ANY);
			}
			else if ((ldat)j==Up) {
			    if ((ldat)i<Left+(ldat)2 && CurrWin->Attrib & WINDOW_CLOSE) {
				CurrWin->Attrib|=GADGET_CLOSE_SELECT|GADGET_PRESSED;
				All->FlagsMove=FlagsMove=GLMOVE_1stWIN_FREEZE;
			    }
			    else if (Rgt-(ldat)i<(ldat)2 && !(CurrWin->Attrib & WINDOW_MENU)) {
				CurrWin->Attrib|=GADGET_BACK_SELECT|GADGET_PRESSED;
				All->FlagsMove=FlagsMove=GLMOVE_1stWIN_FREEZE;
			    }
			    else if (CurrWin->Attrib & WINDOW_DRAG) {
				All->FlagsMove=FlagsMove=GLMOVE_1stWIN | GLMOVE_BY_MOUSE;
				DragPosition=(ldat)i-Left;
			    }
			    DrawBorderWindow(CurrWin, BORDER_ANY);
			}
			else if ((CurrWin->Attrib & WINDOW_X_BAR && (ldat)j==Dwn && (ldat)i<=Rgt-(ldat)2 && (ldat)i>=Left+(ldat)1)
				 || (CurrWin->Attrib & WINDOW_Y_BAR && (ldat)i==Rgt && (ldat)j<=Dwn-(ldat)1 && (ldat)j>=Up+(ldat)1)) {
			    
			    CharFound=SearchFontBorderWin(CurrWin, (udat)((ldat)i-Left), (udat)((ldat)j-Up), (byte)0, (byte)0, &FlagsMove, &FlagsMove);
			    All->FlagsMove=FlagsMove=GLMOVE_SCROLL_1stWIN | GLMOVE_BY_MOUSE;
			    
			    if ((ldat)j==Dwn) {
				if (CharFound==POS_ARROW_BACK)
				    CurrWin->Attrib|=X_BAR_SELECT|ARROW_BACK_SELECT;
				else if (CharFound==POS_ARROW_FWD)
				    CurrWin->Attrib|=X_BAR_SELECT|ARROW_FWD_SELECT;
				else if (CharFound==POS_BAR_BACK)
				    CurrWin->Attrib|=X_BAR_SELECT|PAGE_BACK_SELECT;
				else if (CharFound==POS_BAR_FWD)
				    CurrWin->Attrib|=X_BAR_SELECT|PAGE_FWD_SELECT;
				else {
				    CurrWin->Attrib|=X_BAR_SELECT|TAB_SELECT;
				    DragPosition=i-Left-1-TabStart(CurrWin, (num)1);
				}
			    }
			    else {
				if (CharFound==POS_ARROW_BACK)
				    CurrWin->Attrib|=Y_BAR_SELECT|ARROW_BACK_SELECT;
				else if (CharFound==POS_ARROW_FWD)
				    CurrWin->Attrib|=Y_BAR_SELECT|ARROW_FWD_SELECT;
				else if (CharFound==POS_BAR_BACK)
				    CurrWin->Attrib|=Y_BAR_SELECT|PAGE_BACK_SELECT;
				else if (CharFound==POS_BAR_FWD)
				    CurrWin->Attrib|=Y_BAR_SELECT|PAGE_FWD_SELECT;
				else {
				    CurrWin->Attrib|=Y_BAR_SELECT|TAB_SELECT;
				    DragPosition=j-Up-1-TabStart(CurrWin, (num)0);
				}
			    }
			    DrawBorderWindow(CurrWin, BORDER_ANY);
			}
		    }
		    else if (!(FlagsMove & GLMOVE_1stWIN_FREEZE)) {
			if ((Gadget=Act(SearchGadget,CurrWin)(CurrWin, i, j))
			    && !(Gadget->Flags & GADGET_DISABLED)) {
			    
			    All->FlagsMove|=GLMOVE_1stWIN_FREEZE;
			    CurrWin->GadgetSelect=Gadget;
			    Gadget->Flags|=GADGET_PRESSED;
			    if (CurrWin == All->FirstScreen->FirstWindow)
				DrawWindow(CurrWin, (gadget *)0, Gadget, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
			    else
				DrawArea((screen *)0, (window *)0, CurrWin, (gadget *)0, Gadget, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
			}
		    }
		}
		else {
		    if (CurrWin) {
			Act(Focus,CurrWin)((window *)0);
			DrawBorderWindow(CurrWin, BORDER_ANY);
			DrawMenuBar(Screen, MINDAT, MAXDAT);
			UpdateCursor();
		    }
		    All->FlagsMove|=GLMOVE_1stWIN_FREEZE;
		}
		break;
	      case STMENU_MOUSE_DRAG:
		if (EventMouseY+1==YLimit && (tempMenuItem=Act(SearchMenuItem,Menu)(Menu, EventMouseX)) && tempMenuItem!=CurrMenuItem)
		    ChangeMenuFirstScreen(tempMenuItem, TRUE, KEEP_ACTIVE_MENU_FLAG);
		else if (CurrMenuItem) {
		    if (CurrWin)
			Delta=CurrWin->CurY;
		    else
			Delta = MAXULDAT;
		    
		    if (Act(SearchWindow,Screen)(Screen, EventMouseX, EventMouseY ,&Shade)==CurrWin
			&& (ldat)EventMouseX>Left && (ldat)EventMouseX<Rgt && (ldat)EventMouseY>Up && (ldat)EventMouseY<Dwn)
			CurrWin->CurY=(uldat)((ldat)EventMouseY-Up-(ldat)1)+CurrWin->YLogic;
		    else
			CurrWin->CurY=MAXULDAT;
		    
		    if (Delta!=MAXULDAT) {
			Delta+=(dat)(Up-CurrWin->YLogic)+(dat)1;
			DrawWindow(CurrWin, (gadget *)0, (gadget *)0, (dat)Left+(dat)1, (dat)Delta, (dat)Rgt-(dat)1, (dat)Delta, FALSE);
		    }
		    if ((Delta=CurrWin->CurY)!=MAXULDAT) {
			Delta+=(dat)(Up-CurrWin->YLogic)+(dat)1;
			DrawWindow(CurrWin, (gadget *)0, (gadget *)0, (dat)Left+(dat)1, (dat)Delta, (dat)Rgt-(dat)1, (dat)Delta, FALSE);
		    }
		    UpdateCursor();
		}
		break;
	      case STDEF_MOUSE_DRAG_SOME:
		if (FlagsMove & GLMOVE_1stWIN_FREEZE && CurrWin) {
		    ltemp=CurrWin->Attrib;
		    temp=Screen->Attrib;
		    if (temp & GADGET_ANY_SELECT) {
			if (j+1==(dat)YLimit && i>ScreenWidth-(dat)3)
			    Screen->Attrib|=GADGET_PRESSED;
			else
			    Screen->Attrib&=~GADGET_PRESSED;
			if (temp!=Screen->Attrib)
			    DrawMenuBar(Screen, ScreenWidth-(dat)2, ScreenWidth-(dat)1);
		    }
		    else if (ltemp & GADGET_BACK_SELECT && !(ltemp & WINDOW_MENU)) {
			if ((ldat)j==Up && Rgt-(ldat)i<(ldat)2 && (ldat)i<=Rgt)
			    CurrWin->Attrib|=GADGET_PRESSED;
			else
			    CurrWin->Attrib&=~GADGET_PRESSED;
			if (ltemp!=CurrWin->Attrib)
			    DrawBorderWindow(CurrWin, BORDER_ANY);
		    }
		    else if (ltemp & GADGET_CLOSE_SELECT && ltemp & WINDOW_CLOSE) {
			if ((ldat)j==Up && (ldat)i>=Left && (ldat)i<Left+(ldat)2)
			    CurrWin->Attrib|=GADGET_PRESSED;
			else
			    CurrWin->Attrib&=~GADGET_PRESSED;
			if (ltemp!=CurrWin->Attrib)
			    DrawBorderWindow(CurrWin, BORDER_ANY);
		    }
		    else if ((Gadget=CurrWin->GadgetSelect)) {
			temp=Gadget->Flags;
			if (Gadget==Act(SearchGadget,CurrWin)(CurrWin, i, j))
			    Gadget->Flags|=GADGET_PRESSED;
			else
			    Gadget->Flags&=~GADGET_PRESSED;
			if (temp!=Gadget->Flags) {
			    if (CurrWin == All->FirstScreen->FirstWindow)
				DrawWindow(CurrWin, (gadget *)0, Gadget, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
			    else
				DrawArea((screen *)0, (window *)0, CurrWin, (gadget *)0, Gadget, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
			}
		    }
		}
		else if (FlagsMove & GLMOVE_BY_MOUSE && CurrWin && CurrWin->Attrib & TAB_SELECT) {
		    if (CurrWin->Attrib & X_BAR_SELECT) {
			NumLogicMax=Max2((uldat)1024, CurrWin->XLogic+(uldat)CurrWin->XWidth-(uldat)2);
			if (i-DragPosition>Rgt-(ldat)4)
			    i=Rgt+DragPosition-(ldat)4;
			ScrollWindow(CurrWin, (i-Left-1-DragPosition-TabStart(CurrWin, (num)1))*(dat)(NumLogicMax/(CurrWin->XWidth-(udat)5)), 0);
		    }
		    else if (CurrWin->Attrib & Y_BAR_SELECT) {
			NumLogicMax=Max2(CurrWin->MaxNumRow, CurrWin->YLogic+(uldat)CurrWin->YWidth-(uldat)2);
			if (j-DragPosition>Dwn-(ldat)3)
			    i=Dwn+DragPosition-(ldat)3;
			ScrollWindow(CurrWin, 0, (j-Up-1-DragPosition-TabStart(CurrWin, (num)0))*(dat)(NumLogicMax/(CurrWin->YWidth-(udat)4)));
		    }
		}
		else if (FlagsMove & GLMOVE_BY_MOUSE && FlagsMove & GLMOVE_1stSCREEN) {
		    YDelta=(dat)((ldat)EventMouseY-(ldat)YLimit+(ldat)1);
		    if (YDelta)
			ResizeFirstScreen(YDelta);
		}
		else if (FlagsMove & GLMOVE_BY_MOUSE && FlagsMove & GLMOVE_1stWIN) {
		    i=(dat)((ldat)EventMouseX-Left-DragPosition);
		    j=(dat)((ldat)EventMouseY-Up);
		    if (i || j)
			DragWindow(CurrWin, i, j);
		}
		else if (FlagsMove & GLMOVE_BY_MOUSE && FlagsMove & GLMOVE_RESIZE_1stWIN) {
		    i=(dat)((ldat)EventMouseX-Rgt);
		    j=(dat)((ldat)EventMouseY-Dwn);
		    if (i || j)
			ResizeRelWindow(CurrWin, i, j);
		}
		break;

	      case STDEF_MOUSE_DRAG_NULL:
		break;
		
	      case STDEF_QUIT:
		Quit(0);
		break;
	      case SCREEN_Xn_SCROLL:
		DragFirstScreen(Repeat, 0);
		break;
	      case SCREEN_Xp_SCROLL:
		DragFirstScreen(-Repeat, 0);
		break;
	      case SCREEN_Yn_SCROLL:
		DragFirstScreen(0, Repeat);
		break;
	      case SCREEN_Yp_SCROLL:
		DragFirstScreen(0, -Repeat);
		break;
	      case STWIN_X_DECR:
	      case STWIN_X_INCR:
	      case STWIN_Y_DECR:
	      case STWIN_Y_INCR:
	      case STWIN_Xn_DRAG:
	      case STWIN_Xp_DRAG:
	      case STWIN_Yn_DRAG:
	      case STWIN_Yp_DRAG:
		if (CurrWin->Attrib & WINDOW_RESIZE && (Funct==STWIN_X_DECR || Funct==STWIN_X_INCR || Funct==STWIN_Y_DECR || Funct==STWIN_Y_INCR))
		    temp = GLMOVE_RESIZE_1stWIN;
		else if (CurrWin->Attrib & WINDOW_DRAG && (Funct==STWIN_Xn_DRAG|| Funct==STWIN_Xp_DRAG|| Funct==STWIN_Yn_DRAG|| Funct==STWIN_Yp_DRAG))
		    temp = GLMOVE_1stWIN;
		else
		    break;
		
		Funct==STWIN_X_INCR || Funct==STWIN_Xp_DRAG ? (XDelta= Repeat, YDelta= (dat)0) :
		Funct==STWIN_X_DECR || Funct==STWIN_Xn_DRAG ? (XDelta=-Repeat, YDelta= (dat)0) :
		Funct==STWIN_Y_INCR || Funct==STWIN_Yp_DRAG ? (XDelta= (dat)0, YDelta= Repeat) :
							      (XDelta= (dat)0, YDelta=-Repeat);

		if (temp == GLMOVE_RESIZE_1stWIN)
		    ResizeRelWindow(CurrWin, XDelta, YDelta);
		else
		    DragWindow(CurrWin, XDelta, YDelta);
		
		break;
	      case SCROLL_Xn_CHAR:
	      case SCROLL_Xp_CHAR:
	      case SCROLL_Yn_CHAR:
	      case SCROLL_Yp_CHAR:
	      case SCROLL_Xp_PAGE:
	      case SCROLL_Xn_PAGE:
	      case SCROLL_Yn_PAGE:
	      case SCROLL_Yp_PAGE:
		if ((!(CurrWin->Attrib & WINDOW_X_BAR) && (Funct == SCROLL_Xp_CHAR || Funct == SCROLL_Xn_CHAR ||
							   Funct == SCROLL_Xp_PAGE || Funct == SCROLL_Xn_PAGE)) ||
		    (!(CurrWin->Attrib & WINDOW_Y_BAR) && (Funct == SCROLL_Yp_CHAR || Funct == SCROLL_Yn_CHAR ||
							   Funct == SCROLL_Yp_PAGE || Funct == SCROLL_Yn_PAGE)))
		    break;
		
		
		Funct==SCROLL_Xp_CHAR ? (XDelta= Repeat, YDelta= (dat)0) :
		Funct==SCROLL_Xn_CHAR ? (XDelta=-Repeat, YDelta= (dat)0) :
		Funct==SCROLL_Yp_CHAR ? (XDelta= (dat)0, YDelta= Repeat) :
		Funct==SCROLL_Yn_CHAR ? (XDelta= (dat)0, YDelta=-Repeat) :
		Funct==SCROLL_Xp_PAGE ? (XDelta= Repeat*(CurrWin->XWidth-3), YDelta= (dat)0) :
		Funct==SCROLL_Xn_PAGE ? (XDelta=-Repeat*(CurrWin->XWidth-3), YDelta= (dat)0) :
		Funct==SCROLL_Yp_PAGE ? (XDelta= (dat)0, YDelta= Repeat*(CurrWin->YWidth-3)) :
					(XDelta= (dat)0, YDelta=-Repeat*(CurrWin->YWidth-3));
		
		ScrollWindow(CurrWin, XDelta, YDelta);
		break;
#if 0
	      case STDEF_NEXT_WINDOW:
		MakeLastWindow(CurrWin);
		break;
#endif
	      case STDEF_CLOSE_WINDOW:
		if (!(CurrWin->Attrib & WINDOW_CLOSE))
		    break;
		
		if ((NewMsg=Do(Create,Msg)(FnMsg, MSG_WINDOW_GADGET, sizeof(event_gadget)))) {
		    tempEvent=&NewMsg->Event;
		    tempEvent->EventGadget.Window = CurrWin;
		    tempEvent->EventGadget.Code = (udat)0; /* COD_CLOSE */
		    SendMsg(CurrWin->Menu->MsgPort, NewMsg);
		} else
		    Error(NOMEMORY);
		break;
	      case STDEF_DRAGorRESIZE_WINDOW:
		if (!(CurrWin->Attrib & (WINDOW_DRAG | WINDOW_RESIZE)))
		    break;
		All->FlagsMove=FlagsMove=GLMOVE_1stWIN;
		DrawBorderWindow(CurrWin, BORDER_ANY);
		break;
	      case STDEF_CENTER_WINDOW:
		if (NWinDiMenu)
		    CenterWindow(CurrWin);
		break;
	      case STDEF_NEXT_SCREEN:
		if ((tempScreen=All->LastScreen)==Screen)
		    break;
		MoveFirst(Screen, All, tempScreen);
		DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
		UpdateCursor();
		break;
	      case STDEF_DRAGorRESIZE_SCREEN:
		All->FlagsMove = GLMOVE_1stSCREEN;
		DrawMenuBar(Screen, MINDAT, MAXDAT);
		break;
	      case STDEF_ACT_MENU:
		if (!Menu->FirstMenuItem)
		    break;
		if (!Menu->MenuItemSelect)
		    CurrMenuItem=Menu->FirstMenuItem;
		ChangeMenuFirstScreen(CurrMenuItem, FALSE, ACTIVATE_MENU_FLAG);
		break;
	      case SCREEN_Ln_SCROLL:
		ResizeFirstScreen(-Repeat);
		break;
	      case SCREEN_Lp_SCROLL:
		ResizeFirstScreen(Repeat);
		break;
	      case STMENU_PREV_ITEM:
		if (!(tempMenuItem=CurrMenuItem->Prev))
		    tempMenuItem=Menu->LastMenuItem;
		ChangeMenuFirstScreen(tempMenuItem, FALSE, KEEP_ACTIVE_MENU_FLAG);
		break;
	      case STMENU_NEXT_ITEM:
		if (!(tempMenuItem=CurrMenuItem->Next))
		    tempMenuItem=Menu->FirstMenuItem;
		ChangeMenuFirstScreen(tempMenuItem, FALSE, KEEP_ACTIVE_MENU_FLAG);
		break;
	      case STMENU_PREV_ROW:
		if (!CurrWin->MaxNumRow || FlagsMove & GLMOVE_BY_MOUSE)
		    break;
		OldNumRow=CurrWin->CurY;
		if (OldNumRow<MAXULDAT) {
		    if (!OldNumRow)
			NumRow=CurrWin->MaxNumRow-(uldat)1;
		    else
			NumRow=OldNumRow-(uldat)1;
		    CurrWin->CurY=NumRow;
		    DrawTextWindow(CurrWin, (uldat)0, OldNumRow, (uldat)MAXUDAT-(uldat)2, OldNumRow);
		}
		else
		    CurrWin->CurY=NumRow=CurrWin->MaxNumRow-(uldat)1;
		DrawTextWindow(CurrWin, (uldat)0, NumRow, (uldat)MAXUDAT-(uldat)2, NumRow);
		UpdateCursor();
		break;
	      case STMENU_NEXT_ROW:
		if (!CurrWin->MaxNumRow || FlagsMove & GLMOVE_BY_MOUSE)
		    break;
		OldNumRow=CurrWin->CurY;
		if (OldNumRow<MAXULDAT) {
		    if (OldNumRow>=CurrWin->MaxNumRow-(uldat)1)
			NumRow=(uldat)0;
		    else
			NumRow=OldNumRow+(uldat)1;
		    CurrWin->CurY=NumRow;
		    DrawTextWindow(CurrWin, (uldat)0, OldNumRow, (uldat)MAXUDAT-(uldat)2, OldNumRow);
		}
		else
		    CurrWin->CurY=NumRow=(uldat)0;
		DrawTextWindow(CurrWin, (uldat)0, NumRow, (uldat)MAXUDAT-(uldat)2, NumRow);
		UpdateCursor();
		break;
	      default:
		break;
	    }
	}
	
	if (!CheckForwardMsg(Msg, !!Funct))
	    Delete(Msg);	
    }
    
    if (!(All->FlagsMove & GLMOVE_ANY)) {
	while ((Msg=MapQueue->FirstMsg)) {
	    tempWin=Msg->Event.EventMap.Window;
	    SmartPlace(tempWin, Msg->Event.EventMap.Screen);
	    Delete(Msg);
	}
    }
    
    if (All->MouseHW && All->MouseHW->MouseState.keys && Scroller_MsgPort->WakeUp != TIMER_ALWAYS) {
	extern msg *Do_Scroll;
	Scroller_MsgPort->WakeUp = TIMER_ALWAYS;
	SendMsg(Scroller_MsgPort, Do_Scroll);
    } else if ((!All->MouseHW || !All->MouseHW->MouseState.keys) && Scroller_MsgPort->WakeUp == TIMER_ALWAYS) {
	extern msg *Dont_Scroll;
	SendMsg(Scroller_MsgPort, Dont_Scroll);
    }
}

byte InitWM(void) {
    srand48(time(NULL));
    return (WM_MsgPort=Do(Create,MsgPort)
	    (FnMsgPort, 2, "WM", (uldat)0, (udat)0, (byte)0, WManagerH))
	&& ((MapQueue=Do(Create,MsgPort)
	    (FnMsgPort, 8, "MapQueue", (uldat)0, (udat)0, (byte)0, (void (*)(msgport *))NoOp))
	    ? (Remove(MapQueue), TRUE) : FALSE);
}


static dat SLeft;
static udat SUp, XWidth, YWidth;

static byte doSmartPlace(window *Window, dat *X, udat *Y) {
    dat WLeft, WRgt, TryX[2];
    udat WUp, WDwn, TryY[2];
    byte OK = FALSE;

    if (XWidth > X[1] - X[0] + 1 || YWidth > Y[1] - Y[0] + 1)
	return FALSE;
    
    if (!Window)
	return TRUE;
	
    WRgt = (WLeft = Window->Left) + Window->XWidth;
    WDwn = (WUp = Window->Up) + Window->YWidth;
    Window = Window->Next;
	
    if (X[0] >= WRgt || X[1] < WLeft || Y[0] >= WDwn || Y[1] < WUp)
	return Window ? doSmartPlace(Window, X, Y) : TRUE;
    
    if (X[0] < WLeft) {
	TryX[0] = X[0]; TryX[1] = WLeft - 1;
	TryY[0] = Y[0]; TryY[1] = Y[1];
	OK = doSmartPlace(Window, TryX, TryY);
    }
    if (!OK && X[1] >= WRgt) {
	TryX[0] = WRgt; TryX[1] = X[1];
	TryY[0] = Y[0]; TryY[1] = Y[1];
	OK = doSmartPlace(Window, TryX, TryY);
    }
    if (!OK && Y[0] < WUp) {
	TryX[0] = X[0]; TryX[1] = X[1];
	TryY[0] = Y[0]; TryY[1] = WUp - 1;
	OK = doSmartPlace(Window, TryX, TryY);
    }
    if (!OK && Y[1] >= WDwn) {
	TryX[0] = X[0]; TryX[1] = X[1];
	TryY[0] = WDwn; TryY[1] = Y[1];
	OK = doSmartPlace(Window, TryX, TryY);
    }
    if (OK) {
	X[0] = TryX[0]; X[1] = TryX[1];
	Y[0] = TryY[0]; Y[1] = TryY[1];
    }
    return OK;
}

#define MAXLRAND48 0x80000000l

static void SmartPlace(window *Window, screen *Screen) {
    dat X[2];
    udat Y[2];
    
    X[1] = (SLeft = X[0] = Screen->Left) + Screen->ScreenWidth - 1;
    Y[1] = (SUp = Y[0] = Screen->Up) + Screen->ScreenHeight - Screen->YLimit - 1;
    
    XWidth = Window->XWidth;
    YWidth = Window->YWidth;
    
    if (!doSmartPlace(Screen->FirstWindow, X, Y)) {
	/* can't be smart... be random */
	if (XWidth <= X[1] - X[0])
	    X[0] += lrand48() / (MAXLRAND48 / (X[1] - X[0] + 2 - XWidth));
	if (YWidth <= Y[1] - Y[0])
	    Y[0] += lrand48() / (MAXLRAND48 / (Y[1] - Y[0] + 2 - YWidth));
    }
    if (XWidth > X[1] - X[0] + 1 && X[0] > MINDAT)
	X[0]--;
    if (YWidth > Y[1] - Y[0] + 1 && Y[0] > 0)
	Y[0]--;
    
    Window->Left = X[0];
    Window->Up = Y[0];
    Act(RealMap,Window)(Window,Screen);
}

