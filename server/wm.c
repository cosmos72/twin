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

static msgport *MapQueue;


#define XNumLogicMax 1024
#define YNumLogicMax (Window->MaxNumRow)
#define XBarSize     (Window->XWidth-(udat)5)
#define YBarSize     (Window->YWidth-(udat)4)

static udat TabStart(window *Window, num isX) {
    uldat NumLogicMax;
    udat ret;
    
    if (isX) {
	NumLogicMax=Max2(XNumLogicMax, Window->XLogic+Window->XWidth-2);
	ret = Window->XLogic * (uldat)XBarSize / NumLogicMax;
    }
    else {
	NumLogicMax=Max2(Window->MaxNumRow, Window->YLogic+(uldat)Window->YWidth-(uldat)2);
	ret = Window->YLogic * (uldat)YBarSize / NumLogicMax;
    }
    return ret;
}

static udat TabLen(window *Window, num isX) {
    uldat NumLogicMax;
    udat ret;
    
    if (isX) {
	NumLogicMax=Max2(XNumLogicMax, Window->XLogic+Window->XWidth-2);
	ret = ((Window->XWidth-2)*(uldat)XBarSize + NumLogicMax - 1) / NumLogicMax;
    }
    else {
	NumLogicMax=Max2(Window->MaxNumRow, Window->YLogic+(uldat)Window->YWidth-(uldat)2);
	ret = ((Window->YWidth-2)*(uldat)YBarSize + NumLogicMax - 1) / NumLogicMax;
    }
    return ret ? ret : 1;
}

/* this returns -1 before the tab, 0 on the tab, 1 after */
INLINE num IsTabPosition(window *Window, udat pos, num isX) {
    udat start;
    return pos >= (start = TabStart(Window, isX)) ? pos - start < TabLen(Window, isX) ? 0 : 1 : -1;
}

#ifdef MODULE
static
#endif
byte FindBorderWindow(window *Window, udat u, udat v, byte Border, byte MovWin, byte *PtrChar, byte *PtrColor) {
    byte Char, Found = (byte)0, FlagNewChar;
    hwcol Color;
    byte Horiz, Vert;
    uldat Attrib;
    byte Close, Resize, NMenuWin, BarX, BarY;
    num Back_Fwd, i;
    ldat k;
    udat rev_u, rev_v;
    udat XWidth, YWidth;
    
    if (!Window)
	return Found;
    
    FlagNewChar=!!(All->SetUp->Flags & SETUP_NEW_FONT);
    Attrib=Window->Attrib;
    Close=!!(Attrib & WINDOW_CLOSE) && Button_Close != (num)-1;
    Resize=!!(Attrib & WINDOW_RESIZE);
    NMenuWin=!(Attrib & WINDOW_MENU);
    BarX=!!(Attrib & WINDOW_X_BAR);
    BarY=!!(Attrib & WINDOW_Y_BAR);
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;
    rev_u=XWidth-u-(udat)1;
    rev_v=YWidth-v-(udat)1;

    Vert  = v ? rev_v ? (byte)1 : (byte)2 : (byte)0;
    Horiz = u ? rev_u ? (byte)1 : (byte)2 : (byte)0;

    switch (Vert) {
      case 0:
	
#define is_u(pos) ((pos) >= 0 \
		   ? u == (udat)(pos) || u == (udat)(pos) + (udat)1 \
		   : rev_u == (udat)-(pos) || rev_u + (udat)1 == (udat)-(pos))
#define delta_u(pos) ((pos) >= 0 ? u - (udat)(pos) : (udat)-(pos) - rev_u)

	i = Button_N;
	if (Close && is_u(Button_Pos[Button_Close]))
	    i = Button_Close;
	else if (NMenuWin) {
	    for (i=0; i<Button_N; i++) {
		if (is_u(Button_Pos[i]))
		    break;
	    }
	}
	
	if (i < Button_N) {
	    Char=Button_Shape[i][FlagNewChar][delta_u(Button_Pos[i])];
	    Found=Button_Fn[i];
	} else if (Window->LenTitle) {

	    if (NMenuWin)
		rev_u = rev_v = 0;
	    else {
		/* titlebar length used by buttons at left */
		rev_u = Button_Delta_Left;
		/* titlebar length used by buttons at right */
		rev_v = Button_Delta_Right;
	    }
	    k = 2*(ldat)u - ( (ldat)XWidth-(ldat)Window->LenTitle+(ldat)rev_u-(ldat)rev_v-(ldat)4 );
	    if (k > 0) k /= 2;
	    if (k > 0 && k <= Window->LenTitle) {
		Char=Window->Title[--k];
		Found=POS_TITLE;
	    } else if (k == 0 || k == Window->LenTitle + 1)
		Char=' ';
	    else
		Char=StdBorder[FlagNewChar][Border][0][Horiz];
	} else
	    Char=StdBorder[FlagNewChar][Border][0][Horiz];
	break;
	
#undef is_u
#undef delta_u

      case 1:
	if (Horiz == 0)
	    Char=StdBorder[FlagNewChar][Border][Vert][0];
	else if (Horiz == 2) {
	    if (BarY) {
		if (rev_v<(udat)3) {
		    Char=ScrollBarY[FlagNewChar][(udat)3-rev_v];
		    Found= rev_v==(udat)2 ? POS_ARROW_BACK : POS_ARROW_FWD;
		} else if (!(Back_Fwd=IsTabPosition(Window, v-(udat)1, FALSE))) {
		    Char=TabY[FlagNewChar];
		    Found=POS_TAB;
		} else {
		    Char=ScrollBarY[FlagNewChar][0];
		    Found=Back_Fwd>(num)0 ? POS_BAR_FWD : POS_BAR_BACK;
		}
	    } else
		Char=StdBorder[FlagNewChar][Border][Vert][2];
	}
	break;
      case 2:
	if (rev_u<(udat)2) {
	    if (Resize) {
		Char=GadgetResize[FlagNewChar][(udat)1-rev_u];
		Found=POS_GADGET_RESIZE;
	    } else {
		Char=StdBorder[FlagNewChar][Border][2][(udat)2-rev_u];
	    }
	} else if (!BarX || !Horiz) {
	    Char=StdBorder[FlagNewChar][Border][2][Horiz];
	} else if (rev_u<(udat)4) {
	    Char=ScrollBarX[FlagNewChar][(udat)4-rev_u];
	    Found= rev_u==(udat)3 ? POS_ARROW_BACK : POS_ARROW_FWD;
	} else if (!(Back_Fwd=IsTabPosition(Window, u-(udat)1, TRUE))) {
	    Char=TabX[FlagNewChar];
	    Found=POS_TAB;
	} else {
	    Char=ScrollBarX[FlagNewChar][0];
	    Found=Back_Fwd>(num)0 ? POS_BAR_FWD : POS_BAR_BACK;
	}
	break;
      default:
	break;
    }
    
    if (PtrColor) {
	if (MovWin && (!Found || (Found==POS_TITLE && !Window->ColTitle))) {
	    if (Vert==(byte)1)
		Color=COL( COLFG(Window->ColGadgets), COLBG(Window->ColBorder) );
	    else
		Color=Window->ColGadgets;
	} else if (MovWin && ((Found >= BUTTON_FIRST && Found <= BUTTON_LAST) ||
			      Found==POS_GADGET_RESIZE))
	    Color=COL( COLBG(Window->ColGadgets), COLFG(Window->ColGadgets) );
	else switch (Found) {
	  case POS_GADGET_RESIZE:
	    Color=Window->ColGadgets;
	    break;
	  case POS_TAB:
	    Color=Window->ColTabs;
	    break;
	  case POS_ARROW_BACK:
	  case POS_ARROW_FWD:
	    Color=Window->ColArrows;
	    break;
	  case POS_BAR_BACK:
	  case POS_BAR_FWD:
	    Color=Window->ColBars;
	    break;
	  case POS_TITLE:
	    if (Window->ColTitle)
		Color=Window->ColTitle[k];
	    else
		Color=Window->ColBorder;
	    break;
	  default:
	    if (Found >= BUTTON_FIRST && Found <= BUTTON_LAST) {
		if (Attrib & GADGET_PRESSED && 
		    Attrib & (BUTTON_FIRST_SELECT << (Found - BUTTON_FIRST)))
		    
		    Color=COL( COLBG(Window->ColGadgets), COLFG(Window->ColGadgets) );
		else
		    Color=Window->ColGadgets;
	    } else
		Color=Window->ColBorder;
	    break;
	}
	*PtrColor=Color;
    }
    
    if (PtrChar)
	*PtrChar=Char;
    return Found;
}
    
    
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
	Dwn=Up+(CurrWin->Attrib & WINDOW_ROLLED_UP ? 0 : (ldat)CurrWin->YWidth-(ldat)1);
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
		
      case COD_COMMON_ROLLUP:
	if (CurrWin->Attrib & WINDOW_ROLLED_UP) {
	    CurrWin->Attrib &= ~WINDOW_ROLLED_UP;
	    DrawAreaWindow(CurrWin, FALSE);
	} else {
	    CurrWin->Attrib |= WINDOW_ROLLED_UP;
	    ReDrawRolledUpAreaWindow(CurrWin, FALSE);
	}
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

static screen *Screen;
static menu *Menu;
static menuitem *CurrMenuItem;
static window *CurrWin;
static gadget *Gadget;
static dat i, j;
static udat Funct;
static byte State, Shade;
static ldat Left, Up, Rgt, Dwn, NWinDiMenu;
static udat YLimit;
static dat ScreenWidth;
static setup *SetUp;
static ldat DragPosition;

static void DeActivateScreen(void) {
    Screen->Attrib &= ~(GADGET_BACK_SELECT|GADGET_PRESSED);
    if (Screen != All->LastScreen) {
	MoveLast(Screen, All, Screen);
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0,
		 0, (dat)Min2(Screen->YLimit, All->FirstScreen->YLimit)-(dat)1,
		 MAXDAT, MAXDAT, FALSE);
	UpdateCursor();
    }
}

static void SwitchToScreen(screen *tScreen) {
    MoveFirst(Screen, All, tScreen);
    DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0,
	     0, (dat)Min2(Screen->YLimit, tScreen->YLimit)-(dat)1,
	     MAXDAT, MAXDAT, FALSE);
    UpdateCursor();
    Screen=tScreen;
    ScreenWidth=Screen->ScreenWidth;
    YLimit=Screen->YLimit;
    if ((CurrWin=Screen->FocusWindow)) {
	NWinDiMenu=CurrWin->Attrib & WINDOW_MENU ? 0 : (ldat)~(ldat)0;
    }
		    
    if ((Menu=Act(SearchMenu,Screen)(Screen)))
	CurrMenuItem=Menu->MenuItemSelect;
}

static void ActivateMenu(void) {
    if (j+1==(dat)YLimit)
	CurrMenuItem=Act(SearchMenuItem,Menu)(Menu, i);
    else
	CurrMenuItem=(menuitem *)0;
    ChangeMenuFirstScreen(CurrMenuItem, TRUE, ACTIVATE_MENU_FLAG);
}

static void DeActivateMenu(void) {
    window *tempWin;
    row *tempRow;
    msg *NewMsg;
    event_any *tempEvent;

    if (All->FlagsMove & GLMOVE_BY_MOUSE) {
	if ((tempWin = CurrMenuItem
	     && Act(SearchWindow,Screen)(Screen, i, j)==CurrWin
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

static void DragMenu(void) {
    uldat Delta;
    
    if (CurrWin)
	Delta=CurrWin->CurY;
    else
	Delta = MAXULDAT;
		    
    if (Act(SearchWindow,Screen)(Screen, i, j)==CurrWin
	&& (ldat)i>Left && (ldat)i<Rgt && (ldat)j>Up && (ldat)j<Dwn)
	CurrWin->CurY=(uldat)((ldat)j-Up-(ldat)1)+CurrWin->YLogic;
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

static void ActivateTitleWindow(void) {
    byte found = Act(FindBorder,CurrWin)(CurrWin, (ldat)i-Left, 0, 0, 0, NULL, NULL);

    if (found >= BUTTON_FIRST && found <= BUTTON_LAST) {
	CurrWin->Attrib |= (BUTTON_FIRST_SELECT << (found-BUTTON_FIRST)) | GADGET_PRESSED;
	All->FlagsMove=GLMOVE_1stWIN_FREEZE;
    } else if (CurrWin->Attrib & WINDOW_DRAG) {
	All->FlagsMove=GLMOVE_1stWIN | GLMOVE_BY_MOUSE;
	DragPosition=(ldat)i-Left;
    }
    DrawBorderWindow(CurrWin, BORDER_ANY);
}

static void ActivateBarsWindow(void) {
    byte CharFound = Act(FindBorder,CurrWin)
	(CurrWin, (udat)((ldat)i-Left), (udat)((ldat)j-Up), (byte)0, (byte)0, NULL, NULL);

    All->FlagsMove=GLMOVE_SCROLL_1stWIN | GLMOVE_BY_MOUSE;
			    
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
    } else {
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

static void TryScrollWindow(void) {
    uldat NumLogicMax;
    
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

static void ActivateGadget(void) {
    All->FlagsMove|=GLMOVE_1stWIN_FREEZE;
    CurrWin->GadgetSelect=Gadget;
    Gadget->Flags|=GADGET_PRESSED;
    if (CurrWin == All->FirstScreen->FirstWindow)
	DrawWindow(CurrWin, (gadget *)0, Gadget, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
    else
	DrawArea((screen *)0, (window *)0, CurrWin, (gadget *)0, Gadget, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
}

static void DeActivateGadget(void) {
    msg *NewMsg;
    event_any *tempEvent;

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


static void DragGadget(void) {
    uldat temp, ltemp;
    byte found;
    
    ltemp=CurrWin->Attrib;
    temp=Screen->Attrib;
    if (temp & GADGET_BACK_SELECT) {
	if (j+1==(dat)YLimit && i>ScreenWidth-(dat)3)
	    Screen->Attrib|=GADGET_PRESSED;
	else
	    Screen->Attrib&=~GADGET_PRESSED;
	if (temp!=Screen->Attrib)
	    DrawMenuBar(Screen, ScreenWidth-(dat)2, ScreenWidth-(dat)1);
    } else if (ltemp & BUTTON_ANY_SELECT) {
	found = 0;
	if ((ldat)j==Up && (ldat)i>=Left && (ldat)i<=Rgt) {
	    found = Act(FindBorder,CurrWin)(CurrWin, (ldat)i-Left, 0, 0, 0, NULL, NULL);
	    
	    if (found < BUTTON_FIRST || found > BUTTON_LAST ||
		(ltemp & BUTTON_ANY_SELECT) != (BUTTON_FIRST_SELECT << (found - BUTTON_FIRST)))
		
		found = 0;
	}
	if (found)
	    CurrWin->Attrib |= GADGET_PRESSED;
	else
	    CurrWin->Attrib &= ~GADGET_PRESSED;
	    
	if (ltemp!=CurrWin->Attrib)
	    DrawBorderWindow(CurrWin, BORDER_UP);

    } else if ((Gadget=CurrWin->GadgetSelect)) {
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

static void DeActivate(void) {
    udat temp;
    msg *NewMsg;
    event_any *tempEvent;
    byte found;
    
    if (Funct==STMENU_ACT_BACKTO_DEF && State==STATE_MENU) {
	DeActivateMenu();
	Funct=STWIN_BACKTO_DEF;
    }
		
    if (Funct!=STWIN_BACKTO_DEF && Funct!=STWIN_BACKTO_DEF_FREEZE)
	return;

    temp = All->FlagsMove & GLMOVE_RESIZE_1stWIN;
    
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
	CurrWin->Attrib &= ~(BUTTON_ANY_SELECT | SCROLL_ANY_SELECT | XY_BAR_SELECT);
	DrawBorderWindow(CurrWin, BORDER_ANY);
	
	if (temp)
	    Check4Resize(CurrWin);
	
    } else {
	if (j+1==(dat)YLimit && i>ScreenWidth-(dat)3 && Screen->Attrib & GADGET_BACK_SELECT)
	    DeActivateScreen();
	else if (CurrWin && (ldat)j==Up && (ldat)i>=Left && (ldat)i<=Rgt &&
		 (CurrWin->Attrib & BUTTON_ANY_SELECT) &&
		 (found = Act(FindBorder,CurrWin)(CurrWin, (ldat)i-Left, 0, 0, 0, NULL, NULL)) &&
		 found >= BUTTON_FIRST && found <= BUTTON_LAST &&
		 (CurrWin->Attrib & BUTTON_ANY_SELECT) == BUTTON_FIRST_SELECT << (found-BUTTON_FIRST)) {
		
	    if (found == BUTTON_CLOSE && CurrWin->Attrib & WINDOW_CLOSE) {
		CurrWin->Attrib &= ~(BUTTON_ANY_SELECT|GADGET_PRESSED);
		DrawBorderWindow(CurrWin, BORDER_UP);
		if (!(NewMsg=Do(Create,Msg)(FnMsg, MSG_WINDOW_GADGET, sizeof(event_gadget))))
		    Error(NOMEMORY);
		else if (CurrWin) {
		    tempEvent=&NewMsg->Event;
		    tempEvent->EventGadget.Window = CurrWin;
		    tempEvent->EventGadget.Code = (udat)0; /* COD_CLOSE */
		    SendMsg(CurrWin->Menu->MsgPort, NewMsg);
		}
	    } else if (!(CurrWin->Attrib & WINDOW_MENU)) {
		if (found == BUTTON_BACK) {
		    CurrWin->Attrib &= ~(BUTTON_ANY_SELECT|GADGET_PRESSED);
		    DrawBorderWindow(CurrWin, BORDER_UP);
		    if (CurrWin == Screen->FirstWindow && CurrWin->Next)
			MakeLastWindow(CurrWin, TRUE);
		    else if (CurrWin->Prev)
			MakeFirstWindow(CurrWin, TRUE);
		    else
			DrawBorderWindow(CurrWin, BORDER_UP);
		} else if (found == BUTTON_ROLLUP) {
		    CurrWin->Attrib &= ~(BUTTON_ANY_SELECT|GADGET_PRESSED);		    
		    if (CurrWin->Attrib & WINDOW_ROLLED_UP) {
			CurrWin->Attrib &= ~WINDOW_ROLLED_UP;
			DrawAreaWindow(CurrWin, FALSE);
		    } else {
			CurrWin->Attrib |= WINDOW_ROLLED_UP;
			ReDrawRolledUpAreaWindow(CurrWin, FALSE);
		    }
		}
		if (CurrWin == All->FirstScreen->FocusWindow)
		    UpdateCursor();
	    }
	} else if (Screen->Attrib & GADGET_BACK_SELECT) {
	    Screen->Attrib &= ~(GADGET_BACK_SELECT|GADGET_PRESSED);
	    DrawMenuBar(Screen, MINDAT, MAXDAT);
	} else if (CurrWin && CurrWin->Attrib & BUTTON_ANY_SELECT) {
	    CurrWin->Attrib &= ~(BUTTON_ANY_SELECT|GADGET_PRESSED);
	    DrawBorderWindow(CurrWin, BORDER_ANY);
	} else if (CurrWin && (Gadget=CurrWin->GadgetSelect))
	    DeActivateGadget();
    }
}

INLINE void InitVars(void) {
    Screen=All->FirstScreen;
    ScreenWidth=Screen->ScreenWidth;
    YLimit=Screen->YLimit;
    if ((CurrWin=Screen->FocusWindow)) {
	NWinDiMenu=CurrWin->Attrib & WINDOW_MENU ? 0 : (ldat)~(ldat)0;
	
	Up=(ldat)CurrWin->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)YLimit;
	Left=(ldat)CurrWin->Left-((ldat)Screen->Left & NWinDiMenu);
	Rgt=Left+(ldat)CurrWin->XWidth-(ldat)1;
	Dwn=Up+(CurrWin->Attrib & WINDOW_ROLLED_UP ? 0 : (ldat)CurrWin->YWidth-(ldat)1);

    } else
	NWinDiMenu=TRUE;
	    
    if ((Menu=Act(SearchMenu,Screen)(Screen)))
	CurrMenuItem=Menu->MenuItemSelect;
    
    SetUp=All->SetUp;
    Shade=!!(SetUp->Flags & SETUP_DO_SHADE);
}


/* the Window Manager built into Twin */
static void WManagerH(msgport *MsgPort) {
    msg *Msg, *NewMsg;
    event_any *Event, *tempEvent;
    window *tWin;
    udat CallKey;
    dat EventMouseX, EventMouseY;
    udat temp;
    udat Repeat;
    screen *tScreen;
    menuitem *tempMenuItem;
    uldat NumRow, OldNumRow;
    dat XDelta, YDelta;
    
    Funct = 0;
    
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
	
	if (All->FlagsMove & GLMOVE_1stSCREEN)
	    State=STATE_SCREEN;
	else if (All->FlagsMove & GLMOVE_1stMENU)
	    State=STATE_MENU;
	else if (All->FlagsMove & GLMOVE_SCROLL_1stWIN && Msg->Type==MSG_KEY) 
	    State=STATE_SCROLL;
	else if (All->FlagsMove & GLMOVE_ANY_1stWIN)
	    State=STATE_WINDOW;
	else
	    State=STATE_DEFAULT;
	
	/*
	 Funziona solo se GlobalKeyCodes e GlobalMouseCodes
	 hanno formato normale (non compresso)
	 */
	
	if (Msg->Type==MSG_MOUSE) {
	    
	    i = EventMouseX = Event->EventMouse.X;
	    j = EventMouseY = Event->EventMouse.Y;

	    if ((tScreen = Do(Search,Screen)(j)))
		tWin = Act(SearchWindow,tScreen)(tScreen, i, j);
	    else
		tWin = (window *)0;

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
	    
	    Repeat=1;
	    EventMouseX=EventMouseY=(dat)0;
	}
	
	if (Funct) {
	    InitVars();
	    
	    if (State==STATE_WINDOW && j<(dat)YLimit)
		j=EventMouseY=(dat)YLimit;
	    
	    switch (Funct) {
	      case STMENU_ACT_BACKTO_DEF:
	      case STWIN_BACKTO_DEF:
	      case STWIN_BACKTO_DEF_FREEZE:
	      case STWIN_BACKTO_NULL:
		
		if (All->FlagsMove & GLMOVE_ANY && ((All->FlagsMove & GLMOVE_BY_MOUSE) ? Msg->Type != MSG_MOUSE : Msg->Type != MSG_KEY))
		    break;
		
		DeActivate();
		
		break;
		
	      case STDEF_MOUSE_ACT_SOME:
	      case STDEF_MOUSE_ACT_MENU:
	      case STDEF_MOUSE_ACT_NULL:
		if (!tScreen)
		    break;
		else if (tScreen!=Screen) {
		    SwitchToScreen(tScreen);
		    
		    State=STATE_DEFAULT;
		    All->FlagsMove=GLMOVE_1stWIN_FREEZE;
		}
		
		if (All->FlagsMove & GLMOVE_1stSCREEN) {
		    All->FlagsMove &= ~GLMOVE_1stSCREEN;
		    DrawMenuBar(Screen, MINDAT, MAXDAT);
		}
		if (All->FlagsMove & GLMOVE_ANY_1stWIN) {
		    All->FlagsMove &= ~GLMOVE_ANY_1stWIN;
		    DrawBorderWindow(CurrWin, BORDER_ANY);
		}
		if (All->FlagsMove & GLMOVE_1stMENU) {
		    ChangeMenuFirstScreen((menuitem *)0, FALSE, DISABLE_MENU_FLAG);
		    CurrWin=Screen->FocusWindow;
		}

		if (Funct==STDEF_MOUSE_ACT_MENU) {
		    if (Menu) {
			ActivateMenu();
		    }
		} else if (Funct==STDEF_MOUSE_ACT_NULL) {
		    if (!tWin && j >= (dat)YLimit) {
			/* open the Window List */
			Act(UnMap,ListWin)(ListWin);
			ListWin->Left = Screen->Left + i - ListWin->XWidth / 2;
			ListWin->Up = Screen->Up + (udat)j - YLimit;
			Act(RealMap,ListWin)(ListWin,Screen);
		    }
		}
		else if (j+1==(dat)YLimit) {
		    if (i>ScreenWidth-(dat)3) {
			tScreen->Attrib |= GADGET_BACK_SELECT|GADGET_PRESSED;
			All->FlagsMove=GLMOVE_1stWIN_FREEZE;
		    }
		    else
			All->FlagsMove=GLMOVE_1stSCREEN | GLMOVE_BY_MOUSE;
		    DrawMenuBar(Screen, (dat)0, ScreenWidth-(dat)1);
		}
		else if (tWin) {
		    if (tWin!=CurrWin) {
			CurrWin=Act(Focus,tWin)(tWin);
			if (Screen->FocusWindow != tWin)
			    /* failed for some reason */
			    break;
			DrawBorderWindow(tWin, BORDER_ANY);
			DrawBorderWindow(CurrWin, BORDER_ANY);
			DrawMenuBar(Screen, MINDAT, MAXDAT);
			UpdateCursor();
			CurrWin = tWin;
			All->FlagsMove=GLMOVE_1stWIN_FREEZE;
			NWinDiMenu=CurrWin->Attrib & WINDOW_MENU ? 0 : (ldat)~(ldat)0;
			
			Up=(ldat)CurrWin->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)YLimit;
			Left=(ldat)CurrWin->Left-((ldat)Screen->Left & NWinDiMenu);
			Rgt=Left+(ldat)CurrWin->XWidth-(ldat)1;
			Dwn=Up+(CurrWin->Attrib & WINDOW_ROLLED_UP ? 0 : (ldat)CurrWin->YWidth-(ldat)1);
		    }
		    
		    if ((ldat)i==Left || (ldat)i==Rgt || (ldat)j==Up || (ldat)j==Dwn) {
			if ((ldat)j==Up)
			    ActivateTitleWindow();
			else if ((ldat)i>Rgt-(ldat)2 && (ldat)j==Dwn && CurrWin->Attrib & WINDOW_RESIZE) {
			    All->FlagsMove=GLMOVE_RESIZE_1stWIN | GLMOVE_BY_MOUSE;
			    DrawBorderWindow(CurrWin, BORDER_ANY);
			} else if ((CurrWin->Attrib & WINDOW_X_BAR && (ldat)j==Dwn && (ldat)i<=Rgt-(ldat)2 && (ldat)i>=Left+(ldat)1)
				 || (CurrWin->Attrib & WINDOW_Y_BAR && (ldat)i==Rgt && (ldat)j<=Dwn-(ldat)1 && (ldat)j>=Up+(ldat)1)) {
			    
			    ActivateBarsWindow();
			}
		    } else if (!(All->FlagsMove & GLMOVE_1stWIN_FREEZE)) {
			if ((Gadget=Act(SearchGadget,CurrWin)(CurrWin, i, j))
			    && !(Gadget->Flags & GADGET_DISABLED)) {
			    
			    ActivateGadget();
			}
		    }
		} else {
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
		else if (CurrMenuItem)
		    DragMenu();
		break;
	      case STDEF_MOUSE_DRAG_SOME:
		if (All->FlagsMove & GLMOVE_1stWIN_FREEZE && CurrWin) {
		    DragGadget();
		}
		else if (All->FlagsMove & GLMOVE_BY_MOUSE && CurrWin && CurrWin->Attrib & TAB_SELECT) {
		    TryScrollWindow();
		}
		else if (All->FlagsMove & GLMOVE_BY_MOUSE && All->FlagsMove & GLMOVE_1stSCREEN) {
		    YDelta=(dat)((ldat)EventMouseY-(ldat)YLimit+(ldat)1);
		    if (YDelta)
			ResizeFirstScreen(YDelta);
		}
		else if (All->FlagsMove & GLMOVE_BY_MOUSE && All->FlagsMove & GLMOVE_1stWIN) {
		    i=(dat)((ldat)EventMouseX-Left-DragPosition);
		    j=(dat)((ldat)EventMouseY-Up);
		    if (i || j)
			DragWindow(CurrWin, i, j);
		}
		else if (All->FlagsMove & GLMOVE_BY_MOUSE && All->FlagsMove & GLMOVE_RESIZE_1stWIN) {
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
		All->FlagsMove=GLMOVE_1stWIN;
		DrawBorderWindow(CurrWin, BORDER_ANY);
		break;
	      case STDEF_CENTER_WINDOW:
		if (NWinDiMenu)
		    CenterWindow(CurrWin);
		break;
	      case STDEF_NEXT_SCREEN:
		if ((tScreen=All->LastScreen)==Screen)
		    break;
		MoveFirst(Screen, All, tScreen);
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
		if (!CurrWin->MaxNumRow || All->FlagsMove & GLMOVE_BY_MOUSE)
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
		if (!CurrWin->MaxNumRow || All->FlagsMove & GLMOVE_BY_MOUSE)
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
	    tWin=Msg->Event.EventMap.Window;
	    SmartPlace(tWin, Msg->Event.EventMap.Screen);
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

#ifdef MODULE
static
#endif
byte InitWM(void) {
    srand48(time(NULL));
    if (!WM_MsgPort && (WM_MsgPort=Do(Create,MsgPort)
			(FnMsgPort, 2, "WM", (uldat)0, (udat)0, (byte)0, WManagerH))) {
	if ((MapQueue=Do(Create,MsgPort)
	     (FnMsgPort, 8, "MapQueue", (uldat)0, (udat)0, (byte)0, (void (*)(msgport *))NoOp))) {
		
	    Remove(MapQueue);
	    
	    return TRUE;
	}
	Delete(WM_MsgPort);
	WM_MsgPort = (msgport *)0;
    }
    fprintf(stderr, "twin: Out of memory!\n");
    return FALSE;
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
    WDwn = (WUp = Window->Up) + (Window->Attrib & WINDOW_ROLLED_UP ? 1 : Window->YWidth);
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

static void SmartPlace(window *Window, screen *tScreen) {
    dat X[2];
    udat Y[2];
    
    X[1] = (SLeft = X[0] = tScreen->Left) + tScreen->ScreenWidth - 1;
    Y[1] = (SUp = Y[0] = tScreen->Up) + tScreen->ScreenHeight - tScreen->YLimit - 1;
    
    XWidth = Window->XWidth;
    YWidth = (Window->Attrib & WINDOW_ROLLED_UP ? 1 : Window->YWidth);
    
    if (!doSmartPlace(tScreen->FirstWindow, X, Y)) {
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
    Act(RealMap,Window)(Window, tScreen);
}

#ifdef MODULE
static byte (*orig_FindBorder)(window *, udat u, udat v, byte Border, byte MovWin, byte *PtrChar, byte *PtrColor);

INLINE byte InsertHook(void) {
    orig_FindBorder = FnWindow->FindBorder;
    FnWindow->FindBorder = FindBorderWindow;
    return TRUE;
}
INLINE byte RemoveHook(void) {
    FnWindow->FindBorder = orig_FindBorder;
}

byte InitModule(module *Module) {
    return InitWM() && InsertHook();
}

void QuitModule(module *Module) {
    RemoveHook();
    Delete(MapQueue);
    Delete(WM_MsgPort);
    WM_MsgPort = (msgport *)0;
}
#endif /* MODULE */
