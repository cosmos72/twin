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

#include "twin.h"
#include "data.h"
#include "methods.h"
#include "builtin.h"
#include "main.h"
#include "draw.h"
#include "resize.h"
#include "util.h"
#include "extensions.h"
#include "scroller.h"
#include "wm.h"

#include "hw.h"
#include "hw_multi.h"
#include "common.h"
#include "libTwkeys.h"	/* for TW_* key defines */

#include "rctypes.h"
#include "rcrun.h"


static void DetailCtx(wm_ctx *C);

static msgport *WM_MsgPort;
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
byte WMFindBorderWindow(window *W, udat u, udat v, byte Border, byte *PtrChar, hwcol *PtrColor) {
    byte *BorderFont;
    ldat k;
    uldat Attrib;
    hwcol Color;
    udat rev_u, rev_v;
    udat XWidth, YWidth;
    byte Char, Found = POS_SIDE, FlagAltFont, MovWin;
    byte Horiz, Vert;
    byte Close, Resize, NMenuWin, BarX, BarY;
    num Back_Fwd, i;
    
    if (!W)
	return Found;
    
    FlagAltFont = !!(All->SetUp->Flags & SETUP_ALTFONT);
    MovWin = W == All->FirstScreen->FocusWindow &&
	((All->State & STATE_ANY) == STATE_DRAG ||
	 (All->State & STATE_ANY) == STATE_RESIZE ||
	 (All->State & STATE_ANY) == STATE_SCROLL);
    Attrib=W->Attrib;
    Close=!!(Attrib & WINDOW_CLOSE);
    Resize=!!(Attrib & WINDOW_RESIZE);
    NMenuWin=!(Attrib & WINDOW_MENU);
    BarX=!!(Attrib & WINDOW_X_BAR);
    BarY=!!(Attrib & WINDOW_Y_BAR);
    XWidth=W->XWidth;
    YWidth=W->YWidth;
    rev_u=XWidth-u-(udat)1;
    rev_v=YWidth-v-(udat)1;

    Vert  = v ? rev_v ? (byte)1 : (byte)2 : (byte)0;
    Horiz = u ? rev_u ? (byte)1 : (byte)2 : (byte)0;

    
    if (!(BorderFont = W->BorderPattern[Border]) &&
	!(BorderFont = RCFindBorderPattern(W, Border)))
	
	BorderFont = W->BorderPattern[Border] = StdBorder[FlagAltFont][Border];
    

    switch (Vert) {
      case 0:
	
#define is_u(pos) ((pos) >= 0 \
		   ? u == (udat)(pos) || u == (udat)(pos) + (udat)1 \
		   : (pos) < -1 \
		   ? rev_u + (udat)1 == (udat)-(pos) || rev_u + (udat)2 == (udat)-(pos) \
		   : 0)
#define delta_u(pos) ((pos) >= 0 ? u - (udat)(pos) : (udat)-(pos) - rev_u - (udat)1)

	i = BUTTON_MAX;
	if (Close && All->ButtonVec[0].exists && is_u(All->ButtonVec[0].pos))
	    i = 0;
	else if (NMenuWin) {
	    for (i=1; i<BUTTON_MAX; i++) {
		if (All->ButtonVec[i].exists && is_u(All->ButtonVec[i].pos))
		    break;
	    }
	}

	Found = POS_TITLE;
	
	if (i < BUTTON_MAX) {
	    Char = All->ButtonVec[i].shape[delta_u(All->ButtonVec[i].pos)];
	    Found = i;
	} else if (W->LenTitle) {

	    rev_u = rev_v = 0;
	    k = 2*(ldat)u - ( (ldat)XWidth-(ldat)W->LenTitle+(ldat)rev_u-(ldat)rev_v-(ldat)4 );
	    if (k > 0) k /= 2;
	    if (k > 0 && k <= W->LenTitle)
		Char=W->Title[--k];
	    else if (k == 0 || k == W->LenTitle + 1)
		Char=' ';
	    else
		Char=BorderFont[Horiz];
	} else
	    Char=BorderFont[Horiz];
	break;
	
#undef is_u
#undef delta_u

      case 1:
	if (Horiz == 0)
	    Char=BorderFont[Vert*3];
	else if (Horiz == 2) {
	    if (BarY) {
		if (rev_v<(udat)3) {
		    Char=ScrollBarY[FlagAltFont][(udat)3-rev_v];
		    Found= rev_v==(udat)2 ? POS_ARROW_BACK : POS_ARROW_FWD;
		} else if (!(Back_Fwd=IsTabPosition(W, v-(udat)1, FALSE))) {
		    Char=TabY[FlagAltFont];
		    Found=POS_TAB;
		} else {
		    Char=ScrollBarY[FlagAltFont][0];
		    Found=Back_Fwd>(num)0 ? POS_BAR_FWD : POS_BAR_BACK;
		}
	    } else {
		Char=BorderFont[Vert*3+2];
	    }
	}
	break;
      case 2:
	if (rev_u<(udat)2) {
	    if (Resize) {
		Char=GadgetResize[FlagAltFont][(udat)1-rev_u];
		Found=POS_CORNER;
	    } else
		Char=BorderFont[6+(udat)2-rev_u];
	} else if (!BarX || !Horiz) {
	    Char=BorderFont[6+Horiz];
	} else if (rev_u<(udat)4) {
	    Char=ScrollBarX[FlagAltFont][(udat)4-rev_u];
	    Found= rev_u==(udat)3 ? POS_ARROW_BACK : POS_ARROW_FWD;
	} else if (!(Back_Fwd=IsTabPosition(W, u-(udat)1, TRUE))) {
	    Char=TabX[FlagAltFont];
	    Found=POS_TAB;
	} else {
	    Char=ScrollBarX[FlagAltFont][0];
	    Found=Back_Fwd>(num)0 ? POS_BAR_FWD : POS_BAR_BACK;
	}
	break;
      default:
	break;
    }
    
    if (PtrColor) {
	if (MovWin && (Found < BUTTON_MAX || Found == POS_CORNER ||
		       Found == POS_TITLE || Found == POS_SIDE)) {
	    if (Found < BUTTON_MAX || Found==POS_CORNER)
		Color=COL( COLBG(W->ColGadgets), COLFG(W->ColGadgets) );
	    else if (Found==POS_TITLE && W->ColTitle && k >= 0 && k < W->LenTitle)
		Color=W->ColTitle[k];
	    else
		Color=W->ColGadgets;
	} else switch (Found) {
	  case POS_CORNER:	Color=W->ColGadgets; break;
	  case POS_SIDE:	Color=W->ColBorder;  break;
	  case POS_ARROW_BACK:
	  case POS_ARROW_FWD:	Color=W->ColArrows;  break;
	  case POS_TAB:		Color=W->ColTabs;    break;
	  case POS_BAR_BACK:
	  case POS_BAR_FWD:	Color=W->ColBars;    break;
	  case POS_TITLE:
	    if (W->ColTitle && k >= 0 && k < W->LenTitle)
		Color=W->ColTitle[k];
	    else
		Color=W->ColBorder;
	    break;
	  default:
	    if (Found < BUTTON_MAX) {
		if (Attrib & GADGET_PRESSED && 
		    Attrib & (BUTTON_FIRST_SELECT << Found))
		    
		    Color=COL( COLBG(W->ColGadgets), COLFG(W->ColGadgets) );
		else
		    Color=W->ColGadgets;
	    } else
		Color=W->ColBorder;
	    break;
	}
	*PtrColor=Color;
    }
    
    if (PtrChar)
	*PtrChar=Char;
    return Found;
}
    

static void SmartPlace(window *W, screen *Screen);
    
void Check4Resize(window *W) {
    msg *Msg;
    event_any *Event;

    if (!W)
	return;
    
    if (W->Attrib & WINDOW_WANT_CHANGE &&
	(!(W->Flags & WINFL_USECONTENTS) ||
	 W->XWidth != W->TtyData->SizeX+2 || W->YWidth != W->TtyData->SizeY+2)) {
				
	if ((Msg=Do(Create,Msg)(FnMsg, MSG_WINDOW_CHANGE, sizeof(event_window)))) {
	    Event=&Msg->Event;
	    Event->EventWindow.Window = W;
	    Event->EventWindow.Code   = (udat)0; /* not used */
	    Event->EventWindow.XWidth = W->XWidth;
	    Event->EventWindow.YWidth = W->YWidth;
	    SendMsg(W->Menu->MsgPort, Msg);
	} else
	    Error(NOMEMORY);
    }
    if ((W->Flags & WINFL_USECONTENTS))
	CheckResizeWindowContents(W);
}

void AskCloseWindow(window *W) {
    msg *Msg;

    if (W && !(W->Attrib & WINDOW_MENU) && (W->Attrib & WINDOW_CLOSE)) {

	if ((Msg=Do(Create,Msg)(FnMsg, MSG_WINDOW_GADGET, sizeof(event_gadget)))) {
	    Msg->Event.EventGadget.Window = W;
	    Msg->Event.EventGadget.Code = (udat)0; /* COD_CLOSE */
	    SendMsg(W->Menu->MsgPort, Msg);
	} else
	    Error(NOMEMORY);
    }
}

void MaximizeWindow(window *W, byte full_screen) {
    screen *Screen;
    
    if (W && (Screen = W->Screen) &&
	!(W->Attrib & WINDOW_MENU) && (W->Attrib & WINDOW_RESIZE)) {
	    
	if (full_screen) {
	    if (!Screen->Up)  Screen->Up++;
	    W->Left   = Screen->Left - 1;
	    W->Up     = Screen->Up - 1;
	    W->XWidth = Screen->ScreenWidth + 2;
	    W->YWidth = Screen->ScreenHeight + 2 - Screen->YLimit;
	} else {
	    W->Left = Screen->Left;
	    W->Up   = Screen->Up;
	    W->XWidth = Screen->ScreenWidth;
	    W->YWidth = Screen->ScreenHeight - Screen->YLimit;
	} 
	Check4Resize(W);
	DrawArea(FULL_SCREEN);
	UpdateCursor();
    }
}

void ShowWinList(wm_ctx *C) {
    if (!C->Screen)
	C->Screen = All->FirstScreen;
    if (WinList->Screen)
	Act(UnMap,WinList)(WinList);
    if (C->ByMouse) {
	WinList->Left = C->Screen->Left + C->i - 5;
	WinList->Up = C->Screen->Up + C->j - C->Screen->YLimit;
    } else {
	WinList->Left=0;
	WinList->Up=MAXUDAT;
    }
    Act(Map,WinList)(WinList, C->Screen);
}

static byte CheckForwardMsg(wm_ctx *C, msg *Msg, byte WasUsed) {
    static uldat ClickWinId = NOID;
    static byte ClickedInside = FALSE, LastKeys = 0;
    window *ClickWin = (window *)Id2Obj(window_magic >> magic_shift, ClickWinId);
    window *W;
    event_any *Event;
    
    if ((All->State & STATE_ANY) == STATE_MENU &&
	(W = All->FirstScreen->FocusWindow) &&
	(W->Attrib & WINDOW_MENU))
	/* the menu is being used. leave ClickWin. */
	W = (window *)0;
    else
	W = C->W;
    
    if (W != ClickWin) {
	if (ClickedInside && ClickWin && (ClickWin->Attrib & WINDOW_WANT_MOUSE)) {
	/* try to leave ClickWin with a clean status */
	    msg *NewMsg;
	    udat Code;
	    
	    while (LastKeys) {
		if ((NewMsg=Do(Create,Msg)(FnMsg, MSG_WINDOW_MOUSE, sizeof(event_mouse)))) {
		    Event=&NewMsg->Event;
		    Event->EventMouse.Window = ClickWin;
		    Event->EventMouse.ShiftFlags = (udat)0;
		    Code = LastKeys & HOLD_LEFT ? (LastKeys &= ~HOLD_LEFT, RELEASE_LEFT) :
			LastKeys & HOLD_MIDDLE ? (LastKeys &= ~HOLD_MIDDLE, RELEASE_MIDDLE) :
			LastKeys & HOLD_RIGHT ? (LastKeys &= ~HOLD_RIGHT, RELEASE_RIGHT) : 0;
		    Event->EventMouse.Code = Code | LastKeys;
		    Event->EventMouse.X = -1;
		    Event->EventMouse.Y = -1;
		    SendMsg(ClickWin->Menu->MsgPort, NewMsg);
		} else
		    LastKeys = 0, Error(NOMEMORY);
	    }
	}
	ClickedInside = FALSE;
    }
    
    ClickWin = W && !(W->Attrib & WINDOW_MENU) && W->Menu ? W : (window *)0;
    ClickWinId = ClickWin ? ClickWin->Id : NOID;
    
    if (!ClickWin || !ClickWin->Screen || (ClickWin->Attrib & WINDOW_MENU)
	|| !ClickWin->Menu)
	return FALSE;
    
    Event = &Msg->Event;
	
    if (Msg->Type == MSG_KEY) {
	if (!WasUsed && All->State == STATE_DEFAULT) {
	    if (ClickWin->Attrib & WINDOW_WANT_KEYS) {
		Msg->Type = MSG_WINDOW_KEY;
		Event->EventKeyboard.Window = ClickWin;
		SendMsg(ClickWin->Menu->MsgPort, Msg);
		return TRUE;
	    } else
		FallBackKeyAction(ClickWin, &Event->EventKeyboard);
	}
	return FALSE;
    }

    {
	udat Code, _Code;
	byte Inside, IsSelection;
	
	C->W = ClickWin;
	C->Screen = ClickWin->Screen;
	C->Code = Code = Event->EventMouse.Code;
	C->i = Event->EventMouse.X;
	C->j = Event->EventMouse.Y;
	DetailCtx(C);
	
	Inside = C->Pos == POS_INSIDE && !C->G;
	
	if (isSINGLE_PRESS(Code))
	    ClickedInside = Inside;
	
	if (ClickedInside && (Inside || !isDRAG(Code))) {

	    if (isSINGLE_RELEASE(Code))
		ClickedInside = FALSE;

	    /* manage window hilight and Selection */
	    if (!(ClickWin->Attrib & WINDOW_WANT_MOUSE)) {
		
		IsSelection = (Code & HOLD_ANY) == All->SetUp->SelectionButton;
		
		if (IsSelection && isPRESS(Code))
		    StartHilight(ClickWin, (ldat)C->i-C->Left-1+ClickWin->XLogic, (ldat)C->j-C->Up-1+ClickWin->YLogic);
		else if (IsSelection && isDRAG(Code))
		    ExtendHilight(ClickWin, (ldat)C->i-C->Left-1+ClickWin->XLogic, (ldat)C->j-C->Up-1+ClickWin->YLogic);
		else if (isSINGLE_RELEASE(Code)) {
		    
		    _Code = HOLD_CODE(RELEASE_N(Code));
		    
		    if (_Code == All->SetUp->SelectionButton)
			SetSelectionFromWindow(ClickWin);
		    else if (_Code == All->SetUp->PasteButton) {
			/* send Selection Paste msg */
			msg *NewMsg;
			
			/* store selection owner */
			SelectionImport();
		    
			if ((NewMsg = Do(Create,Msg)(FnMsg, MSG_SELECTION, sizeof(event_selection)))) {
			    Event = &NewMsg->Event;
			    Event->EventSelection.Window = ClickWin;
			    Event->EventSelection.Code = 0;
			    Event->EventSelection.pad = (udat)0;
			    Event->EventSelection.X = C->i; /* ABSOLUTE coords here! */
			    Event->EventSelection.Y = C->j;
			    SendMsg(ClickWin->Menu->MsgPort, NewMsg);
			}
		    }
		}
	    }
	    /* finished with Selection stuff */
	    
	    if (ClickWin->Attrib & WINDOW_WANT_MOUSE) {
		Msg->Type=MSG_WINDOW_MOUSE;
		Event->EventMouse.Window = ClickWin;
		Event->EventMouse.X -= C->Left + 1; /* RELATIVE coords here */
		Event->EventMouse.Y -= C->Up + 1;
		SendMsg(ClickWin->Menu->MsgPort, Msg);
		LastKeys = Code & HOLD_ANY;
		return TRUE;
	    }
	}
    }
    return FALSE;
}

static ldat DragPosition[2];


static void InitCtx(udat Code, byte ByMouse, dat i, dat j, wm_ctx *C) {

    C->Code = Code;
    if ((C->ByMouse = ByMouse)) {
	C->i = i;
	C->j = j;
    
	if ((C->Screen = Do(Search,Screen)(C->j)) && C->Screen == All->FirstScreen
	    && C->Screen->YLimit <= C->j)
	    C->W = Act(SearchWindow,C->Screen)(C->Screen, C->i, C->j);
	else
	    C->W = (window *)0;
    } else {
	C->Screen = All->FirstScreen;
	C->W = C->Screen->FocusWindow;
    }
}


static void InitMsgCtx(CONST msg *Msg, wm_ctx *C) {
    InitCtx(Msg->Event.EventMouse.Code, Msg->Type == MSG_MOUSE,
	    Msg->Event.EventMouse.X, Msg->Event.EventMouse.Y, C);
}

static void DetailCtx(wm_ctx *C) {
    
    if (C->W)
	C->Screen = C->W->Screen;
    
    if (C->Screen)
	C->Menu = Act(SearchMenu,C->Screen)(C->Screen);

    if (C->ByMouse) {
	C->Pos = POS_ROOT;
	
	if (C->W) {
	    ldat NWinDiMenu = C->W->Attrib & WINDOW_MENU ? 0 : (ldat)~(ldat)0;
	    
	    C->Up=(ldat)C->W->Up-((ldat)C->Screen->Up & NWinDiMenu)+(ldat)C->Screen->YLimit;
	    C->Left=(ldat)C->W->Left-((ldat)C->Screen->Left & NWinDiMenu);
	    C->Rgt=C->Left+(ldat)C->W->XWidth-(ldat)1;
	    C->Dwn=C->Up+(C->W->Attrib & WINDOW_ROLLED_UP ? 0 : (ldat)C->W->YWidth-(ldat)1);
		
	    if (C->i > C->Left && C->i < C->Rgt && C->j > C->Up && C->j < C->Dwn) {
		C->Pos = POS_INSIDE;
		C->G = Act(SearchGadget,C->W)(C->W, C->i, C->j);
	    } else {
		/*
		 * (i,j) _may_ be outside the window... see ContinueMenu()
		 * and CheckForwardMsg()
		 */
		C->G = (gadget *)0;
		if (C->i == C->Left || C->i == C->Rgt || C->j == C->Up || C->j == C->Dwn)
		    C->Pos = Act(FindBorder,C->W)(C->W, C->i - C->Left, C->j - C->Up,
						  0, NULL, NULL);
	    }
	} else if (C->j + 1 == (dat)C->Screen->YLimit) {
	    if (C->i > C->Screen->ScreenWidth - (dat)3)
		C->Pos = POS_SCREENBUTTON;
	    else {
		C->Pos = POS_MENU;
		C->Item = Act(SearchItem,C->Menu)(C->Menu, C->i);
	    }
	}
    }
}

INLINE void Fill4RC_VM(wm_ctx *C, window *W, udat Type, byte Pos, udat Code) {
    C->W = W;
    C->Type = Type;
    C->Pos = Pos;
    C->Code = Code;
}

void FocusCtx(wm_ctx *C) {
    if (C->W)
	C->Screen = C->W->Screen;
    
    if (C->Screen && C->Screen != All->FirstScreen)
	Act(Focus,C->Screen)(C->Screen);
    else
	C->Screen = All->FirstScreen;
	
    if (C->W && C->W != C->Screen->FocusWindow)
	Act(Focus,C->W)(C->W);
    else
	C->W = C->Screen->FocusWindow;
}

static void ActivateScreen(wm_ctx *C) {
    if (C->Screen == All->FirstScreen) {
	All->State = STATE_SCREEN | (C->ByMouse ? STATE_FL_BYMOUSE : 0);
	Act(DrawMenu,C->Screen)(C->Screen, MINDAT, MAXDAT);
    }
}

/* this is mouse-only */
static void ContinueScreen(wm_ctx *C) {
    ResizeFirstScreen(C->j - All->FirstScreen->YLimit + 1);
}

static void ReleaseScreen(wm_ctx *C) {
    All->State = STATE_DEFAULT;
    Act(DrawMenu,All->FirstScreen)(All->FirstScreen, MINDAT, MAXDAT);
}

/* this is mouse-only */
static void ActivateScreenButton(wm_ctx *C) {
    if (C->Screen == All->FirstScreen && All->State == STATE_DEFAULT) {
	All->State = STATE_SCREENBUTTON | STATE_FL_BYMOUSE;
	C->Screen->Attrib |= GADGET_BACK_SELECT | GADGET_PRESSED;
	Act(DrawMenu,C->Screen)(C->Screen, C->Screen->ScreenWidth-(dat)2, C->Screen->ScreenWidth-(dat)1);
    }
}

#if 0
/* this is mouse-only */
static void ContinueScreenButton(wm_ctx *C) {
    udat temp = C->Screen->Attrib;
    
    DetailCtx(C);
    
    if (C->Pos == POS_SCREENBUTTON)
	C->Screen->Attrib |= GADGET_PRESSED;
    else
	C->Screen->Attrib &= ~GADGET_PRESSED;
    if (temp != C->Screen->Attrib)
	Act(DrawMenu,C->Screen)(C->Screen, C->Screen->ScreenWidth-(dat)2, C->Screen->ScreenWidth-(dat)1);
}
#endif

/* this is mouse-only */
static void ReleaseScreenButton(wm_ctx *C) {
    
    All->State = STATE_DEFAULT;
    C->Screen->Attrib &= ~(GADGET_BACK_SELECT|GADGET_PRESSED);
    
    if (C->Screen != All->LastScreen && (DetailCtx(C), C->Pos == POS_SCREENBUTTON)) {
	MoveLast(Screen, All, C->Screen);
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0,
		 0, (dat)Min2(C->Screen->YLimit, All->FirstScreen->YLimit)-(dat)1,
		 MAXDAT, MAXDAT, FALSE);
	UpdateCursor();
    } else
	Act(DrawMenu,C->Screen)(C->Screen, C->Screen->ScreenWidth-(dat)2, C->Screen->ScreenWidth-(dat)1);
}

static void ActivateMenu(wm_ctx *C) {
    C->Screen = All->FirstScreen;
    C->W = C->Screen->FocusWindow;
    C->Menu = Act(SearchMenu,C->Screen)(C->Screen); 
	
    if (C->ByMouse) {
	if (C->j + 1 == C->Screen->YLimit)
	    C->Item = Act(SearchItem,C->Menu)(C->Menu, C->i);
	else
	    C->Item = (menuitem *)0;
    } else {
	C->Item = Act(GetSelectItem,C->Menu)(C->Menu);
	if (!C->Item)
	    C->Item = C->Menu->FirstMenuItem;
    }
    Act(ActivateMenu,C->Screen)(C->Screen, C->Item, C->ByMouse);
}

/* this is mouse-only */
static void ContinueMenu(wm_ctx *C) {
    window *FW = All->FirstScreen->FocusWindow;
    
    DetailCtx(C);

    if (C->Pos == POS_MENU && C->Item && C->Item != Act(GetSelectItem,C->Menu)(C->Menu))
	ChangeMenuFirstScreen(C->Item, TRUE, KEEP_ACTIVE_MENU_FLAG);
    else if (FW && (FW->Attrib & WINDOW_MENU)) {
	uldat Delta = FW->CurY;
	
	if (FW == C->W && C->Pos == POS_INSIDE)
	    FW->CurY = (uldat)((ldat)C->j - C->Up - (ldat)1) + FW->YLogic;
	else {
	    FW->CurY = MAXULDAT;
	    if (FW != C->W) {
		C->W = FW;
		DetailCtx(C); /* re-calculate Left,Up,Rgt,Dwn */
	    }
	}
	
	if (FW->CurY != Delta) {
	    if (Delta != MAXULDAT) {
		Delta += C->Up - FW->YLogic + 1;
		DrawWindow(FW, (gadget *)0, (gadget *)0, (dat)C->Left+(dat)1, (dat)Delta, (dat)C->Rgt-(dat)1, (dat)Delta, FALSE);
	    }
	    if ((Delta = FW->CurY) != MAXULDAT) {
		Delta += C->Up - FW->YLogic + 1;
		DrawWindow(FW, (gadget *)0, (gadget *)0, (dat)C->Left+(dat)1, (dat)Delta, (dat)C->Rgt-(dat)1, (dat)Delta, FALSE);
	    }
	    UpdateCursor();
	}
    }
}

static void ReleaseMenu(wm_ctx *C) {
    window *MW = All->FirstScreen->MenuWindow;
    window *FW = All->FirstScreen->FocusWindow;
    menu *Menu;
    menuitem *Item;
    row *Row;
    msg *Msg;
    event_menu *Event;
    udat Code;
    
    if (FW && FW->CurY < MAXULDAT && (Menu = FW->Menu) &&
	(Item = Act(GetSelectItem,Menu)(Menu)) && Item->FlagActive &&
	(Row = Act(SearchRow,FW)(FW, FW->CurY)) &&
	(Row->Flags & ROW_ACTIVE) && Row->Code)
	
	Code = Row->Code;
    else
	Code = (udat)0;

    /* disable the menu BEFORE processing the code! */
    ChangeMenuFirstScreen((menuitem *)0, FALSE, DISABLE_MENU_FLAG);

    if (Code >= COD_RESERVED) {
	/* handle COD_RESERVED codes internally */
	Fill4RC_VM(C, MW, MSG_MENU_ROW, POS_MENU, Row->Code);
	(void)RC_VMQueue(C);
    } else if (Code) {
	if ((Msg=Do(Create,Msg)(FnMsg, MSG_MENU_ROW, sizeof(event_menu)))) {
	    Event=&Msg->Event.EventMenu;
	    Event->Window = MW;
	    Event->Code = Code;
	    Event->Menu = Menu;
	    SendMsg(Menu->MsgPort, Msg);
	} else
	    Error(NOMEMORY);
    }
}

static void ActivateDrag(wm_ctx *C) {
    if (C->Screen == All->FirstScreen && C->W &&
	C->W == C->Screen->FocusWindow && C->W->Attrib & WINDOW_DRAG) {
	
	All->State = STATE_DRAG;
	if (C->ByMouse) {
	    All->State |= STATE_FL_BYMOUSE;

	    DetailCtx(C);
	    DragPosition[0] = (ldat)C->i - C->Left;
	    DragPosition[1] = (ldat)C->j - C->Up;
	}
	DrawBorderWindow(C->W, BORDER_ANY);
    }
}

static void ActivateResize(wm_ctx *C) {
    if (C->Screen == All->FirstScreen && C->W &&
	C->W == C->Screen->FocusWindow && C->W->Attrib & WINDOW_RESIZE) {

	All->State = STATE_RESIZE;
	if (C->ByMouse) {
	    All->State |= STATE_FL_BYMOUSE;
	    
	    DetailCtx(C);
	    DragPosition[0] = (ldat)C->i - C->Rgt;
	    DragPosition[1] = (ldat)C->j - C->Dwn;
	}
	DrawBorderWindow(C->W, BORDER_ANY);
    }
}

static void ActivateScroll(wm_ctx *C) {
    if (C->Screen == All->FirstScreen && C->W &&
	C->W == C->Screen->FocusWindow &&
	C->W->Attrib & (WINDOW_X_BAR | WINDOW_Y_BAR)) {

	/*
	 * paranoia: SneakSetupMouse() mail call us even for a mouse event
	 * OUTSIDE or INSIDE All->FirstScreen->FocusWindow
	 */
	if (C->ByMouse) {
	    DetailCtx(C);
	    if ((ldat)C->j == C->Dwn) {
		if (C->Pos == POS_ARROW_BACK)
		    C->W->Attrib |= X_BAR_SELECT|ARROW_BACK_SELECT;
		else if (C->Pos == POS_ARROW_FWD)
		    C->W->Attrib |= X_BAR_SELECT|ARROW_FWD_SELECT;
		else if (C->Pos == POS_BAR_BACK)
		    C->W->Attrib |= X_BAR_SELECT|PAGE_BACK_SELECT;
		else if (C->Pos == POS_BAR_FWD)
		    C->W->Attrib |= X_BAR_SELECT|PAGE_FWD_SELECT;
		else {
		    C->W->Attrib |= X_BAR_SELECT|TAB_SELECT;
		    DragPosition[0] = (ldat)C->i - C->Left - 1 - TabStart(C->W, (num)1);
		    DragPosition[1] = 0;
		}
	    } else if ((ldat)C->i == C->Rgt) {
		if (C->Pos == POS_ARROW_BACK)
		    C->W->Attrib |= Y_BAR_SELECT|ARROW_BACK_SELECT;
		else if (C->Pos == POS_ARROW_FWD)
		    C->W->Attrib |= Y_BAR_SELECT|ARROW_FWD_SELECT;
		else if (C->Pos == POS_BAR_BACK)
		    C->W->Attrib |= Y_BAR_SELECT|PAGE_BACK_SELECT;
		else if (C->Pos == POS_BAR_FWD)
		    C->W->Attrib |= Y_BAR_SELECT|PAGE_FWD_SELECT;
		else {
		    C->W->Attrib |= Y_BAR_SELECT|TAB_SELECT;
		    DragPosition[0] = 0;
		    DragPosition[1] = (ldat)C->j - C->Up - 1 - TabStart(C->W, (num)0);
		}
	    }
	    if (C->W->Attrib & SCROLL_ANY_SELECT)
		All->State = STATE_SCROLL | STATE_FL_BYMOUSE;
	} else
	    All->State = STATE_SCROLL;
	    
	if ((All->State & STATE_ANY) == STATE_SCROLL)
	    DrawBorderWindow(C->W, BORDER_ANY);
    }
}

/* this is mouse only */
static void ContinueDrag(wm_ctx *C) {
    C->W = All->FirstScreen->FocusWindow;
    DetailCtx(C);
    if (C->W == All->FirstScreen->FirstWindow)
	DragFirstWindow(C->i - C->Left - DragPosition[0], C->j - C->Up- DragPosition[1]);
    else
	DragWindow(C->W, C->i - C->Left - DragPosition[0], C->j - C->Up - DragPosition[1]);
}

/* this is mouse only */
static void ContinueResize(wm_ctx *C) {
    C->W = All->FirstScreen->FocusWindow;
    DetailCtx(C);
    if (C->W == All->FirstScreen->FirstWindow)
	ResizeRelFirstWindow(C->i - C->Rgt - DragPosition[0], C->j - C->Dwn - DragPosition[1]);
    else
	ResizeRelWindow(C->W, C->i - C->Rgt - DragPosition[0], C->j - C->Dwn - DragPosition[1]);
}

/* this is mouse only */
static void ContinueScroll(wm_ctx *C) {
    uldat NumLogicMax;
    ldat i;
    
    C->W = All->FirstScreen->FocusWindow;
    DetailCtx(C);
    
    if (C->W->Attrib & X_BAR_SELECT) {
	NumLogicMax=Max2((uldat)1024, C->W->XLogic + (uldat)C->W->XWidth - (uldat)2);
	i = C->i;
	if (i + 4 > C->Rgt + DragPosition[0])
	    i = C->Rgt + DragPosition[0] - 4;
	ScrollWindow(C->W, (i - C->Left - 1 - DragPosition[0] - TabStart(C->W, (num)1)) * 
		     (NumLogicMax / (C->W->XWidth - (udat)5)), 0);

    } else if (C->W->Attrib & Y_BAR_SELECT) {
	NumLogicMax=Max2(C->W->MaxNumRow, C->W->YLogic + (uldat)C->W->YWidth - (uldat)2);
	i = C->j;
	if (i + 3 > C->Dwn + DragPosition[1])
	    i = C->Dwn + DragPosition[1] - 3;
	ScrollWindow(C->W, 0, (i - C->Up - 1 - DragPosition[1] - TabStart(C->W, (num)0)) * 
		     (NumLogicMax / (C->W->YWidth - (udat)4)));
    }
}

static void ReleaseDragResizeScroll(CONST wm_ctx *C) {
    window *FW = All->FirstScreen->FocusWindow;
    udat temp;
    
    temp = (All->State & STATE_ANY) == STATE_RESIZE;
    All->State = STATE_DEFAULT;
    
    if (FW) {
	FW->Attrib &= ~(BUTTON_ANY_SELECT | SCROLL_ANY_SELECT | XY_BAR_SELECT);
	DrawBorderWindow(FW, BORDER_ANY);
	
	if (temp)
	    Check4Resize(FW);
    }
}

/* this is mouse only */
static void ActivateButton(wm_ctx *C) {
    All->State = C->Pos | STATE_FL_BYMOUSE;
    C->W->Attrib |= (BUTTON_FIRST_SELECT << C->Pos) | GADGET_PRESSED;

    C->Type = MSG_MOUSE;
    (void)RC_VMQueue(C);
    
    DrawBorderWindow(C->W, BORDER_UP);
}

#if 0
/* this is mouse only */
static void ContinueButton(wm_ctx *C) {
    window *FW = All->FirstScreen->FocusWindow;
    uldat ltemp = FW->Attrib;
    byte found = FALSE;
    
    if (FW == C->W && ltemp & BUTTON_ANY_SELECT) {
	DetailCtx(C);
	if (C->Pos < BUTTON_MAX &&
	    (ltemp & BUTTON_ANY_SELECT) == (BUTTON_FIRST_SELECT << C->Pos))
		
	    found = TRUE;
    }
    
    if (found)
	FW->Attrib |= GADGET_PRESSED;
    else
	FW->Attrib &= ~GADGET_PRESSED;
    if (ltemp != FW->Attrib)
	DrawBorderWindow(FW, BORDER_UP);
}
#endif

/* this is mouse only */
static void ReleaseButton(wm_ctx *C) {
    window *FW = All->FirstScreen->FocusWindow;
    
    All->State = STATE_DEFAULT;
    if (FW) {
	if (FW == C->W && FW->Attrib & BUTTON_ANY_SELECT) {
	    DetailCtx(C);
    
	    if (C->Pos < BUTTON_MAX &&
		(FW->Attrib & BUTTON_ANY_SELECT) == (BUTTON_FIRST_SELECT << C->Pos)) {
	    
		C->W = FW;
		C->Type = MSG_MOUSE;
		(void)RC_VMQueue(C);
	    }
	}
	FW->Attrib &= ~(BUTTON_ANY_SELECT|GADGET_PRESSED);
	DrawBorderWindow(FW, BORDER_ANY);
    }
}

/* this is mouse only */
static void ActivateGadget(wm_ctx *C) {
    All->State = STATE_GADGET | STATE_FL_BYMOUSE;
    C->W->GadgetSelect = C->G;
    C->G->Flags |= GADGET_PRESSED;
    if (C->W == All->FirstScreen->FirstWindow)
	DrawWindow(C->W, (gadget *)0, C->G, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
    else
	DrawArea((screen *)0, (window *)0, C->W, (gadget *)0, C->G, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
}

/* this is mouse only */
static void ContinueGadget(wm_ctx *C) {
    window *FW = All->FirstScreen->FocusWindow;
    gadget *FG = FW->GadgetSelect;
    udat temp = FG->Flags;
    
    if (FW == C->W && FG && (DetailCtx(C), FG == C->G))
	FG->Flags |= GADGET_PRESSED;
    else
	FG->Flags &= ~GADGET_PRESSED;
	
    if (temp != FG->Flags) {
	if (FW == All->FirstScreen->FirstWindow)
	    DrawWindow(FW, (gadget *)0, FG, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
	else
	    DrawArea((screen *)0, (window *)0, FW, (gadget *)0, FG, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
    }
}

/* this is mouse only */
static void ReleaseGadget(wm_ctx *C) {
    window *FW = All->FirstScreen->FocusWindow;
    gadget *FG = FW->GadgetSelect;
    msg *Msg;
    event_gadget *Event;

    DetailCtx(C);
    
    if (FW == C->W && FG && FG == C->G && FG->Code &&
	!(FG->Flags & GADGET_DISABLED) && FG->Flags & GADGET_PRESSED) {

	if ((Msg=Do(Create,Msg)(FnMsg, MSG_WINDOW_GADGET, sizeof(event_gadget)))) {
	    Event=&Msg->Event.EventGadget;
	    Event->Window = FW;
	    Event->Code = FG->Code;
	    SendMsg(FW->Menu->MsgPort, Msg);
	} else
	    Error(NOMEMORY);
    }

    All->State = STATE_DEFAULT;
    /* FW->GadgetSelect=(gadget *)0; */
    FG->Flags &= ~GADGET_PRESSED;
    if (FW == All->FirstScreen->FirstWindow)
	DrawWindow(FW, (gadget *)0, FG, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
    else
	DrawArea((screen *)0, (window *)0, FW, (gadget *)0, FG, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
}

/* the only Activate*() that make sense from within RC_VM() */
void ActivateCtx(wm_ctx *C, byte State) {
    switch (State) {
      case STATE_RESIZE: ActivateResize(C); break;
      case STATE_DRAG:   ActivateDrag(C);   break;
      case STATE_SCROLL: ActivateScroll(C); break;
      case STATE_MENU:	 ActivateMenu(C);   break;
      case STATE_SCREEN: ActivateScreen(C); break;
      default: break;
    }
}

/* force returning to STATE_DEFAULT. used before RCReload() */
void ForceRelease(CONST wm_ctx *C) {
    switch (All->State & STATE_ANY) {
      case STATE_RESIZE:
      case STATE_DRAG:
      case STATE_SCROLL:
	ReleaseDragResizeScroll(C);
	break;
      case STATE_GADGET:
	{
	    window *FW;
	    gadget *FG;

	    if ((FW = All->FirstScreen->FocusWindow) && (FG = FW->GadgetSelect))
		FG->Flags &= ~GADGET_PRESSED;
	}
	break;
      case STATE_MENU:
	ChangeMenuFirstScreen((menuitem *)0, FALSE, DISABLE_MENU_FLAG);
	break;
      case STATE_SCREEN:
	break;
      case STATE_SCREENBUTTON:
	All->FirstScreen->Attrib &= ~(GADGET_BACK_SELECT|GADGET_PRESSED);
	break;
      default:
	if ((All->State & STATE_ANY) < BUTTON_MAX) {
	    window *FW;
	    if ((FW = All->FirstScreen->FocusWindow))
		FW->Attrib &= ~(BUTTON_ANY_SELECT|GADGET_PRESSED);
	}
	break;
    }
    All->State = STATE_DEFAULT;
}
    
/*
 * these must be handled manually as we want the gadgets/buttons
 * to immediately change color when selected
 * 
 * this is mouse only
 */
static byte ActivateMouseState(wm_ctx *C) {
    byte used = FALSE;
    
    switch (C->Pos) {
      case POS_SCREENBUTTON:
	if ((C->Code & HOLD_ANY) == All->SetUp->SelectionButton)
	    used = TRUE, ActivateScreenButton(C);
	break;
      case POS_INSIDE:
	if ((C->Code & HOLD_ANY) == All->SetUp->SelectionButton &&
	    C->G && !(C->G->Flags & GADGET_DISABLED))
	    used = TRUE, ActivateGadget(C);
	break;
      default:
	if (C->Pos < BUTTON_MAX)
	    used = TRUE, ActivateButton(C);
	break;
    }
    return used;
}

/*
 * check if the mouse (C->i, C->j) is in a position suitable for the State.
 * setup DragPosition[] as if the current State was initiated with the mouse,
 * or return FALSE if (C->i, C->j) is in a non-appropriate position.
 */
static byte SneakSetupMouse(wm_ctx *C) {
    window *W = All->FirstScreen->FocusWindow;
    byte ok = TRUE;
    
    switch (All->State & STATE_ANY) {
      case STATE_RESIZE:
	DetailCtx(C);
	if (W == C->W) {
	    DragPosition[0] = (ldat)C->i - C->Rgt;
	    DragPosition[1] = (ldat)C->j - C->Dwn;
	} else
	    ok = FALSE;
	break;
      case STATE_DRAG:
	DetailCtx(C);
	if (W == C->W) {
	    DragPosition[0] = (ldat)C->i - C->Left;
	    DragPosition[1] = (ldat)C->j - C->Up;
	} else
	    ok = FALSE;
	break;
      case STATE_SCROLL:
	/*
	 * this is complex... we must setup W->Attrib with what you clicked upon.
	 * do the dirty way and reuse the function ActivateScroll().
	 * since it returns (void), check for (W->Attrib & SCROLL_ANY_SELECT).
	 */
	ActivateScroll(C);
	ok = !!(W->Attrib & SCROLL_ANY_SELECT);
	break;
      case STATE_GADGET:
      case STATE_MENU:
      case STATE_SCREEN:
      case STATE_SCREENBUTTON:
      case STATE_ROOT:
      case STATE_DEFAULT:
      default:
	break;
    }
    return ok;
}


/* handle mouse during various STATE_* */
/* this is mouse only */
static void ContinueReleaseMouseState(wm_ctx *C, byte State) {
    if (isSINGLE_RELEASE(C->Code)) {
	switch (State) {
	  case STATE_RESIZE:
	  case STATE_DRAG:
	  case STATE_SCROLL: ReleaseDragResizeScroll(C); break;
	  case STATE_GADGET: ReleaseGadget(C);           break;
	  case STATE_MENU:   ReleaseMenu(C);             break;
	  case STATE_SCREEN: ReleaseScreen(C);           break;
	  case STATE_SCREENBUTTON: ReleaseScreenButton(C); break;
	  case STATE_ROOT:
	  case STATE_DEFAULT:
	  default:
	    if (State < BUTTON_MAX)
		ReleaseButton(C);
	    else
		/* paranoid... but necessary to cleanup STATE_FL_FREEZE */
		All->State = STATE_DEFAULT;
	    break;
	}
    } else if (isSINGLE_DRAG(C->Code) || isRELEASE(C->Code)) {
	switch (State) {
	  case STATE_RESIZE: ContinueResize(C); break;
	  case STATE_DRAG:   ContinueDrag(C);   break;
	  case STATE_SCROLL: ContinueScroll(C); break;
	  case STATE_GADGET: ContinueGadget(C); break;
	  case STATE_MENU:   ContinueMenu(C);   break;
	  case STATE_SCREEN: ContinueScreen(C); break;
	  /*case STATE_SCREENBUTTON: ContinueScreenButton(C); break; */
	  case STATE_ROOT: case STATE_DEFAULT:  break;
	  default:
	    /* if (State < BUTTON_MAX)
	     *  ContinueButton(C); */
	     break;
	}
    }
}

/* handle keyboard during various STATE_* */
/* this is keyboard only */
static byte ActivateKeyState(wm_ctx *C, byte State) {
    window *W = All->FirstScreen->FocusWindow;
    uldat NumRow, OldNumRow;
    udat XDelta = 0, YDelta = 0;
    udat Key = C->Code;
    byte used = FALSE;
    
    if (!W)
	return used;
    
    switch (Key) {
      case TW_Right: XDelta= 1; break;
      case TW_Left:  XDelta=-1; break;
      case TW_Down:  YDelta= 1; break;
      case TW_Up:    YDelta=-1; break;
      default: break;
    }

    switch (State & STATE_ANY) {
      case STATE_DRAG:
      case STATE_RESIZE:
	if (Key == TW_Escape || Key == TW_Return)
	    used = TRUE, ReleaseDragResizeScroll(C);
	else if (State == STATE_RESIZE && (W->Attrib & WINDOW_RESIZE))
	    used = TRUE, ResizeRelWindow(W, XDelta, YDelta);
	else if (State == STATE_DRAG && (W->Attrib & WINDOW_DRAG))
	    used = TRUE, DragWindow(W, XDelta, YDelta);
	break;
      case STATE_SCROLL:
	if (Key == TW_Escape || Key == TW_Return) {
	    ReleaseDragResizeScroll(C);
	    used = TRUE;
	    break;
	}
	if (W->Attrib & (WINDOW_X_BAR|WINDOW_Y_BAR)) {

	    switch (Key) {
	      case TW_Insert: XDelta= (W->XWidth-3); break;
	      case TW_Delete: XDelta=-(W->XWidth-3); break;
	      case TW_Next:   YDelta= (W->YWidth-3); break;
	      case TW_Prior:  YDelta=-(W->YWidth-3); break;
	      default: break;
	    }
	    if (!(W->Attrib & WINDOW_X_BAR))
		XDelta = 0;
	    if (!(W->Attrib & WINDOW_Y_BAR))
		YDelta = 0;
	    
	    if (XDelta || YDelta)
		used = TRUE, ScrollWindow(W, XDelta, YDelta);
	}
	break;
      case STATE_MENU:
	C->Menu = Act(SearchMenu,C->Screen)(C->Screen);
	C->Item = Act(GetSelectItem,C->Menu)(C->Menu);
	switch (Key) {
	  case TW_Escape:
	    ChangeMenuFirstScreen((menuitem *)0, FALSE, DISABLE_MENU_FLAG);
	    used = TRUE;
	    break;
	  case TW_Return:
	    ReleaseMenu(C);
	    used = TRUE;
	    break;
	  case TW_Left:
	    if (C->Item->Prev)
		C->Item = C->Item->Prev;
	    else {
		if (C->Item->Menu == C->Menu && C->Menu->CommonItems && All->CommonMenu)
		    C->Item = All->CommonMenu->LastMenuItem;
		else
		    C->Item = C->Menu->LastMenuItem;
	    }
	    ChangeMenuFirstScreen(C->Item, FALSE, KEEP_ACTIVE_MENU_FLAG);
	    used = TRUE;
	    break;
	  case TW_Right:
	    if (C->Item->Next)
		C->Item = C->Item->Next;
	    else {
		if (C->Item->Menu == C->Menu && C->Menu->CommonItems && All->CommonMenu)
		    C->Item = All->CommonMenu->FirstMenuItem;
		else
		    C->Item = C->Menu->FirstMenuItem;
	    }
	    ChangeMenuFirstScreen(C->Item, FALSE, KEEP_ACTIVE_MENU_FLAG);
	    used = TRUE;
	    break;
	  case TW_Up:
	    if (!W->MaxNumRow || (All->State & STATE_FL_BYMOUSE))
		break;
	    OldNumRow=W->CurY;
	    if (OldNumRow<MAXULDAT) {
		if (!OldNumRow)
		    NumRow=W->MaxNumRow-(uldat)1;
		else
		    NumRow=OldNumRow-(uldat)1;
		W->CurY=NumRow;
		DrawTextWindow(W, (uldat)0, OldNumRow, (uldat)MAXUDAT-(uldat)2, OldNumRow);
	    } else
		W->CurY=NumRow=W->MaxNumRow-(uldat)1;
	    DrawTextWindow(W, (uldat)0, NumRow, (uldat)MAXUDAT-(uldat)2, NumRow);
	    UpdateCursor();
	    used = TRUE;
	    break;
	  case TW_Down:
	    if (!W->MaxNumRow || (All->State & STATE_FL_BYMOUSE))
		break;
	    OldNumRow=W->CurY;
	    if (OldNumRow<MAXULDAT) {
		if (OldNumRow>=W->MaxNumRow-(uldat)1)
		    NumRow=(uldat)0;
		else
		    NumRow=OldNumRow+(uldat)1;
		W->CurY=NumRow;
		DrawTextWindow(W, (uldat)0, OldNumRow, (uldat)MAXUDAT-(uldat)2, OldNumRow);
	    } else
		W->CurY=NumRow=(uldat)0;
	    DrawTextWindow(W, (uldat)0, NumRow, (uldat)MAXUDAT-(uldat)2, NumRow);
	    UpdateCursor();
	    used = TRUE;
	    break;
	  default:
	    break;
	}
	break;
      case STATE_SCREEN:
	if (Key == TW_Escape || Key == TW_Return)
	    used = TRUE, ReleaseScreen(C);
	else if (YDelta)
	    used = TRUE, ResizeFirstScreen(-YDelta);
	break;
      case STATE_DEFAULT:
	/* ? is this needed ? */
	DragFirstScreen(XDelta, YDelta);
	break;
      default:
	break;
    }
    return used;
}

    
/* the Window Manager built into Twin */
static void WManagerH(msgport *MsgPort) {
    static wm_ctx _C;
    wm_ctx *C = &_C;
    msg *Msg;
    byte used;
    
    while ((Msg=WM_MsgPort->FirstMsg)) {
	
	Remove(Msg);
	
	if (Msg->Type==MSG_MAP) {
	    SendMsg(MapQueue, Msg);
	    continue;
	} else if (Msg->Type != MSG_MOUSE && Msg->Type != MSG_KEY) {
	    if (Msg->Type == MSG_CONTROL) {
		/*
		 * for now, don't allow starting a different WM (MSG_CTRL_RESTART)
		 * but just restart this one (MSG_CTRL_OPEN)
		 */
		if ((Msg->Event.EventControl.Code == MSG_CTRL_RESTART ||
		     Msg->Event.EventControl.Code == MSG_CTRL_OPEN)) {
		
		    Fill4RC_VM(C, (window *)0, MSG_CONTROL, POS_ROOT, MSG_CTRL_OPEN);
		    (void)RC_VMQueue(C);
		}
	    }
	    Delete(Msg);
	    continue;
	}
	
	InitMsgCtx(Msg, C);

	if (All->State == STATE_DEFAULT) {
	    if (C->ByMouse && isSINGLE_PRESS(C->Code)) {
		if (C->Screen && C->Screen != All->FirstScreen)
		    Act(Focus,C->Screen)(C->Screen);
		
		InitMsgCtx(Msg, C);
		
		if (C->W && C->W != C->Screen->FocusWindow &&
		    (C->Code & HOLD_ANY) == All->SetUp->SelectionButton) {

		    Act(Focus,C->W)(C->W);
		    All->State |= STATE_FL_BYMOUSE|STATE_FL_FREEZE;
		} 
		DetailCtx(C);

		used = ActivateMouseState(C);
	    } else
		used = FALSE, DetailCtx(C);
	    
	    if (!used) {
		C->Type = C->ByMouse ? MSG_MOUSE : MSG_KEY;
		used = RC_VMQueue(C);
	    }
	    	    
	} else if (C->ByMouse) {
	    /*
	     * if you use the mouse during a keyboard-only STATE_xxx,
	     * you will activate the STATE_FL_BYMOUSE flag.
	     * This means that the mouse "steals" to the keyboard
	     * the action you were doing (resize, scroll, ...)
	     * and the keyboard becomes non-functional until
	     * you return to STATE_DEFAULT.
	     * 
	     * Since the WM does not know which mouse button is used to
	     * activate the various STATE_xxx (it's specified in the RC Virtual Machine)
	     * this also means that you can Drag,Resize,Scroll,Activate Menu or Screen
	     * with the "wrong" mouse button with this trick:
	     * first you enter the State STATE_xxx using the Menu, then you
	     * click in an appropriate place with whatever mouse button you like.
	     * 
	     * clicking in a non-appropriate place (e.g. Menu during a Window Scroll)
	     * just forces a return to STATE_DEFAULT.
	     */
	    
	    if (!(All->State & STATE_FL_BYMOUSE)) {
		if (SneakSetupMouse(C))
		    /* ok, mouse is in a meaningful position */
		    All->State |= STATE_FL_BYMOUSE;
		else
		    /* clicked in a strange place? return to STATE_DEFAULT */
		    ForceRelease(C);
	    }
	    
	    if (All->State & STATE_FL_BYMOUSE)
		ContinueReleaseMouseState(C, All->State & STATE_ANY);
	    
	} else if (!C->ByMouse) {
	    used = ActivateKeyState(C, All->State);
	}
	
	/* must we send the event to the focused window too ? */
	if (!(All->State & STATE_FL_FREEZE) &&
	    (C->ByMouse || (All->State == STATE_DEFAULT && !used))) {

	    if ((All->State & STATE_ANY) == STATE_MENU)
		C->W = All->FirstScreen->MenuWindow;
	    else
		C->W = All->FirstScreen->FocusWindow;
	    if (C->W && CheckForwardMsg(C, Msg, used))
		/* don't Delete(Msg) ! */
		continue;
	}

	Delete(Msg);	
    }

    if (All->State == STATE_DEFAULT) {
	while ((Msg = MapQueue->FirstMsg)) {
	    C->W = Msg->Event.EventMap.Window;
	    SmartPlace(C->W, Msg->Event.EventMap.Screen);

	    Fill4RC_VM(C, C->W, MSG_MAP, POS_ROOT, 0);
	    (void)RC_VMQueue(C);

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

    
    if (RC_VM(&WM_MsgPort->PauseDuration)) {
	/* sleep specified amount of time */
	WM_MsgPort->WakeUp |= TIMER_ONCE;
    }
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
    
    if (!Window || Window->Screen)
	return;

    if (Window->Up == MAXUDAT && Window->Left == 0) {
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
    }
    Act(RealMap,Window)(Window, tScreen);
}


#ifdef MODULE
static void OverrideMethods(byte enter) {
    if (enter)
	OverrideMethod(Window,FindBorder, FakeFindBorderWindow, WMFindBorderWindow);
    else
	OverrideMethod(Window,FindBorder, WMFindBorderWindow, FakeFindBorderWindow);
}
#endif

#ifdef MODULE

# include "version.h"
MODULEVERSION;
    
byte InitModule(module *Module)
#else
byte InitWM(void)
#endif
{
    byte sent = FALSE;
    
    srand48(time(NULL));
    if ((WM_MsgPort=Do(Create,MsgPort)(FnMsgPort, 2, "WM", (uldat)0, (udat)0, (byte)0, WManagerH)) &&
	SendControlMsg(WM_MsgPort, MSG_CTRL_OPEN, 0, NULL)) {
	
	if (RegisterExtension(WM,MsgPort,WM_MsgPort)) {
	    
	    if ((MapQueue=Do(Create,MsgPort)
		 (FnMsgPort, 8, "MapQueue", (uldat)0, (udat)0, (byte)0, (void (*)(msgport *))NoOp))) {
		
		Remove(MapQueue);
#ifdef MODULE
		OverrideMethods(TRUE);
#endif
		return TRUE;
	    }
	    UnRegisterExtension(WM,MsgPort,WM_MsgPort);
	} else {
	    sent = TRUE;
	    fprintf(stderr, "twin: WM: RegisterExtension(WM,MsgPort) failed! Another WM is running?\n");
	}
    }
    if (WM_MsgPort)
	Delete(WM_MsgPort);
    if (!sent)
	fprintf(stderr, "twin: WM: Out of memory!\n");
    return FALSE;
}

#ifdef MODULE
void QuitModule(module *Module) {
    QuitRC();
    OverrideMethods(FALSE);
    UnRegisterExtension(WM,MsgPort,WM_MsgPort);
    Delete(WM_MsgPort);
    Delete(MapQueue);
}
#endif /* MODULE */
