/*
 *  wm.c  --  builtin window manager for twin
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
#include "data.h"
#include "methods.h"
#include "builtin.h"
#include "main.h"
#include "draw.h"
#include "resize.h"
#include "util.h"
#include "extreg.h"
#include "scroller.h"
#include "wm.h"

#include "hw.h"
#include "hw_multi.h"
#include "common.h"
#include <Tw/Twkeys.h>	/* for TW_* key defines */

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
#endif

#include "rctypes.h"
#include "rcrun.h"

static void DetailCtx(wm_ctx *C);

byte ClickWindowPos = MAXBYTE;

static msgport WM_MsgPort;
static msgport MapQueue;

#define XBarSize     (Window->XWidth-(udat)5)
#define YBarSize     (Window->YWidth-(udat)4)

static udat TabStart(window Window, sbyte isX) {
    ldat NumLogicMax;
    udat ret;
    
    if (isX) {
	NumLogicMax = Max2(Window->WLogic, Window->XLogic+Window->XWidth-2);
	ret = Window->XLogic * (ldat)XBarSize / NumLogicMax;
    }
    else {
	NumLogicMax = Max2(Window->HLogic, Window->YLogic+(ldat)Window->YWidth-2);
	ret = Window->YLogic * (ldat)YBarSize / NumLogicMax;
    }
    return ret;
}

static udat TabLen(window Window, sbyte isX) {
    ldat NumLogicMax;
    udat ret;
    
    if (isX) {
	NumLogicMax=Max2(Window->WLogic, Window->XLogic+Window->XWidth-2);
	ret = ((Window->XWidth-2)*(ldat)XBarSize + NumLogicMax - 1) / NumLogicMax;
    }
    else {
	NumLogicMax=Max2(Window->HLogic, Window->YLogic+(uldat)Window->YWidth-2);
	ret = ((Window->YWidth-2)*(ldat)YBarSize + NumLogicMax - 1) / NumLogicMax;
    }
    return ret ? ret : 1;
}

/* this returns -1 before the tab, 0 on the tab, 1 after */
INLINE sbyte IsTabPosition(window Window, udat pos, sbyte isX) {
    udat start;
    return pos >= (start = TabStart(Window, isX)) ? pos - start < TabLen(Window, isX) ? 0 : 1 : -1;
}

#ifdef THIS_MODULE
static
#endif
byte WMFindBorderWindow(window W, dat u, dat v, byte Border, hwattr *PtrAttr) {
    hwfont *BorderFont, Font;
    ldat k;
    uldat Attrib;
    hwcol Color;
    dat rev_u, rev_v;
    dat XWidth, YWidth;
    byte Found = POS_SIDE_UP, FlDrag, FlResize, FlScroll, FlPressed;
    byte Horiz, Vert;
    byte Close, Resize, NMenuWin, BarX, BarY;
    sbyte Back_Fwd, i;
    byte extra_u;
    
    if (!W)
	return Found;
    
    XWidth=W->XWidth;
    YWidth=W->YWidth;
    rev_u=XWidth-u-1;
    rev_v=YWidth-v-1;
    
    if (u < 0 || v < 0 || rev_u < 0 || rev_v < 0)
	return POS_ROOT;
    
    if ((W->Flags & WINDOWFL_BORDERLESS) || (u && v && rev_u && rev_v))
	return POS_INSIDE;
    
    FlDrag = FlResize = FlScroll = FlPressed = FALSE;
    
    if (W == (window)All->FirstScreen->FocusW) {
	switch (All->State & STATE_ANY) {
	  case STATE_DRAG:
	    FlDrag = TRUE;
	    break;
	  case STATE_RESIZE:
	    FlResize = TRUE;
	    break;
	  case STATE_SCROLL:
	    FlScroll = TRUE;
	    break;
	  default:
	    break;
	}
    }
    
    Attrib=W->Attrib;
    Close=!!(Attrib & WINDOW_CLOSE);
    Resize=!!(Attrib & WINDOW_RESIZE);
    BarX=!!(Attrib & WINDOW_X_BAR);
    BarY=!!(Attrib & WINDOW_Y_BAR);
    NMenuWin=!(W->Flags & WINDOWFL_MENU);

    Horiz = extra_u = u ? rev_u ? (byte)1 : (byte)2 : (byte)0;
    Vert  =           v ? rev_v ? (byte)1 : (byte)2 : (byte)0;

    
    if (!(BorderFont = W->BorderPattern[Border]) &&
	!(BorderFont = RCFindBorderPattern(W, Border)))
	
	BorderFont = W->BorderPattern[Border] = StdBorder[Border];
    
    if (W->Parent && IS_SCREEN(W->Parent)) switch (Vert) {
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
	else if (NMenuWin)
	    for (i=1; i<BUTTON_MAX; i++)
		if (All->ButtonVec[i].exists && is_u(All->ButtonVec[i].pos))
		    break;

	if (i < BUTTON_MAX) {
	    Font = All->ButtonVec[i].shape[extra_u = delta_u(All->ButtonVec[i].pos)];
	    Found = i;
	} else if (W->NameLen) {

	    k = 2*(ldat)u - ( (ldat)XWidth-(ldat)W->NameLen-(ldat)3 );
	    if (k > 0) k /= 2;
	    if (k > 0 && k <= W->NameLen) {
#ifdef CONF__UNICODE
		Font = Tutf_CP437_to_UTF_16[W->Name[--k]];
#else
		Font = W->Name[--k];
#endif
		Found = POS_TITLE;
	    } else if (k == 0 || k == W->NameLen + 1) {
		Font = ' ';
		Found = POS_TITLE;
	    } else
		Font = BorderFont[Horiz];
	} else
	    Font = BorderFont[Horiz];
	break;
	
#undef is_u
#undef delta_u

      case 1:
	if (Horiz == 0) {
	    Font = BorderFont[(extra_u = Vert) * 3];
	    Found = POS_SIDE_LEFT;
	} else if (Horiz == 2) {
	    if (BarY) {
		if (rev_v<(udat)3) {
		    Font = ScrollBarY[extra_u = (udat)3-rev_v];
		    Found =  rev_v == (udat)2 ? POS_Y_ARROW_BACK : POS_Y_ARROW_FWD;
		} else if (!(Back_Fwd = IsTabPosition(W, v-(udat)1, FALSE))) {
		    Font = TabY;
		    Found = POS_Y_TAB;
		} else {
		    Font = ScrollBarY[0];
		    Found = Back_Fwd>(sbyte)0 ? POS_Y_BAR_FWD : POS_Y_BAR_BACK;
		}
	    } else {
		Font = BorderFont[(extra_u = Vert) * 3 + 2];
		Found = POS_SIDE_RIGHT;
	    }
	}
	break;
      case 2:
	Found = POS_SIDE_DOWN;
	
	if (rev_u<(udat)2) {
	    if (Resize) {
		Font = GadgetResize[extra_u = (udat)1-rev_u];
		Found = POS_BUTTON_RESIZE;
	    } else
		Font = BorderFont[6 + (extra_u = (udat)2-rev_u)];
	} else if (!BarX || !Horiz) {
	    Font = BorderFont[6 + Horiz];
	} else if (rev_u<(udat)4) {
	    Font = ScrollBarX[extra_u = (udat)4-rev_u];
	    Found =  rev_u==(udat)3 ? POS_X_ARROW_BACK : POS_X_ARROW_FWD;
	} else if (!(Back_Fwd=IsTabPosition(W, u-(udat)1, TRUE))) {
	    Font = TabX;
	    Found = POS_X_TAB;
	    extra_u = 0;
	} else {
	    Font = ScrollBarX[0];
	    Found = Back_Fwd>(sbyte)0 ? POS_X_BAR_FWD : POS_X_BAR_BACK;
	    extra_u = 0;
	}
	break;
      default:
	break;
	
    } else switch (Vert) {
      case 0:
	if (W->NameLen) {
	    k = 2*(ldat)u - ( (ldat)XWidth-(ldat)W->NameLen-(ldat)3);
	    if (k > 0) k /= 2;
	    if (k > 0 && k <= W->NameLen) {
#ifdef CONF__UNICODE
		Font = Tutf_CP437_to_UTF_16[W->Name[--k]];
#else
		Font = W->Name[--k];
#endif
		Found = POS_TITLE;
	    } else if (k == 0 || k == W->NameLen + 1) {
		Font = ' ';
		Found = POS_TITLE;
	    } else
		Font = BorderFont[Horiz];
	} else
	    Font = BorderFont[Horiz];
	break;
	
      default:
	Font = BorderFont[Vert*3 + Horiz];
	Found = Vert == 1 ? (Horiz ? POS_SIDE_RIGHT : POS_SIDE_LEFT) : POS_SIDE_DOWN;
	break;
    }

    if (!PtrAttr)
	return Found;
    
    if (FlDrag && Found >= POS_TITLE && Found <= POS_SIDE_DOWN) {
	FlPressed = TRUE;
	if (Found == POS_TITLE && W->ColName && k >= 0 && k < W->NameLen)
	    Color = W->ColName[k];
	else
	    Color = W->ColGadgets;
    } else switch (Found) {
      case POS_TITLE:
	if (W->ColName && k >= 0 && k < W->NameLen)
	    Color = W->ColName[k];
	else
	    Color = W->ColBorder;
	break;
      case POS_SIDE_LEFT:
      case POS_SIDE_UP:
      case POS_SIDE_RIGHT:
      case POS_SIDE_DOWN:
	Color = W->ColBorder;
	break;
      case POS_BUTTON_RESIZE:
	Color = W->ColGadgets;
	if (FlResize) {
	    Color = COL( COLBG(Color), COLFG(Color) );
	    FlPressed = TRUE;
	}
	break;
      case POS_X_ARROW_BACK:
      case POS_X_ARROW_FWD:
      case POS_Y_ARROW_FWD:
      case POS_Y_ARROW_BACK:
	Color = W->ColArrows;
	break;
      case POS_X_TAB:
      case POS_Y_TAB:
	Color = W->ColTabs;
	break;
      case POS_X_BAR_BACK:
      case POS_X_BAR_FWD:
      case POS_Y_BAR_BACK:
      case POS_Y_BAR_FWD:
	Color = W->ColBars;
	break;
      default:
	Color = Found < BUTTON_MAX
	    ? (FlDrag ? COL(COLBG(W->ColGadgets),COLFG(W->ColGadgets)) : W->ColGadgets)
	    : W->ColBorder;
	break;
    }
    if (FlScroll && Found >= POS_X_BAR_BACK && Found <= POS_Y_ARROW_FWD) {
	Color ^= COL(HIGH,HIGH);
	FlPressed = TRUE;
    } else if (Found < BUTTON_MAX && (W->State & WINDOW_GADGET_PRESSED) &&
	 (W->State & (BUTTON_FIRST_SELECT << Found))) {
	
	Color = COL( COLBG(Color), COLFG(Color) );
	FlPressed = TRUE;
    }
    
#ifdef CONF__UNICODE
    extra_u = EncodeToHWAttrExtra(Found, extra_u, !Border, FlPressed);
    *PtrAttr = HWATTR_EXTRA32(HWATTR(Color,Font), extra_u);
#else
    *PtrAttr = HWATTR(Color,Font);
#endif

    return Found;
}
    

static void SmartPlace(widget W, screen Screen);
    
void Check4Resize(window W) {
    msg Msg;
    event_any *Event;
    byte HasBorder;
    
    if (!W)
	return;
    
    HasBorder = !(W->Flags & WINDOWFL_BORDERLESS) * 2;
    
    if (W->Attrib & WINDOW_WANT_CHANGES &&
	(!W_USE(W, USECONTENTS) ||
	 W->XWidth != W->USE.C.TtyData->SizeX+HasBorder ||
	 W->YWidth != W->USE.C.TtyData->SizeY+HasBorder)) {
				
	if ((Msg=Do(Create,Msg)(FnMsg, MSG_WIDGET_CHANGE, 0))) {
	    Event=&Msg->Event;
	    Event->EventWidget.W = (widget)W;
	    Event->EventWidget.Code   = MSG_WIDGET_RESIZE;
	    Event->EventWidget.XWidth = W->XWidth - HasBorder;
	    Event->EventWidget.YWidth = W->YWidth - HasBorder;
	    SendMsg(W->Owner, Msg);
	}
    }
    if (W_USE(W, USECONTENTS))
	CheckResizeWindowContents(W);
}

void AskCloseWidget(widget W) {
    msg Msg;

    if (W && (!IS_WINDOW(W) || (W->Attrib & WINDOW_CLOSE))) {

	if ((Msg=Do(Create,Msg)(FnMsg, MSG_WIDGET_GADGET, 0))) {
	    Msg->Event.EventGadget.W = W;
	    Msg->Event.EventGadget.Code = (udat)0; /* COD_CLOSE */
	    SendMsg(W->Owner, Msg);
	}
    }
}

void MaximizeWindow(window W, byte full_screen) {
    screen Screen;
    
    if (W && IS_WINDOW(W) && (W->Attrib & WINDOW_RESIZE) &&
	(Screen = (screen)W->Parent) && IS_SCREEN(Screen)) {
	    
	if (full_screen) {
	    if (Screen->YLogic == MINDAT) Screen->YLogic++;
	    W->Left   = Screen->XLogic - 1;
	    W->Up     = Screen->YLogic;
	    W->XWidth = All->DisplayWidth + 2;
	    W->YWidth = All->DisplayHeight + 1 - Screen->YLimit;
	} else {
	    if (Screen->YLogic == MAXDAT) Screen->YLogic--;
	    W->Left = Screen->XLogic;
	    W->Up   = Screen->YLogic + 1;
	    W->XWidth = All->DisplayWidth;
	    W->YWidth = All->DisplayHeight - 1 - Screen->YLimit;
	} 
	QueuedDrawArea2FullScreen = TRUE;
	Check4Resize(W);
    }
}

void ShowWinList(wm_ctx *C) {
    if (!C->Screen)
	C->Screen = All->FirstScreen;
    if (WinList->Parent)
	Act(UnMap,WinList)(WinList);
    if (C->ByMouse) {
	WinList->Left = C->i - 5;
	WinList->Up = C->j - C->Screen->YLimit;
    } else {
	WinList->Left=0;
	WinList->Up=MAXDAT;
    }
    Act(Map,WinList)(WinList, (widget)C->Screen);
}

static void RecursiveFocusWidget(widget W) {
    widget P;
    W->SelectW = NULL;
    while ((P = W->Parent)) {
	P->SelectW = W;
	if (IS_SCREEN(P))
	    break;
	W = P;
    }
    if (ContainsCursor((widget)WindowParent(W)))
	UpdateCursor();
}

static widget RecursiveFindFocusWidget(widget W) {
    if (W) while (W->SelectW)
	W = W->SelectW;
    return W;
}

static void CleanupLastW(widget LastW, udat LastKeys, byte LastInside) {
    msg NewMsg;
    event_any *Event;
    udat i;
    
    if (LastW) {
	if (LastInside) {
	    if ((NewMsg=Do(Create,Msg)(FnMsg, MSG_WIDGET_MOUSE, 0))) {
		Event=&NewMsg->Event;
		Event->EventMouse.W = LastW;
		Event->EventMouse.ShiftFlags = (udat)0;
		Event->EventMouse.Code = MOVE_MOUSE | (LastKeys & HOLD_ANY);
		Event->EventMouse.X = MINDAT;
		Event->EventMouse.Y = MINDAT;
		SendMsg(LastW->Owner, NewMsg);
	    }
	}
	while (LastKeys & HOLD_ANY) {
	    if ((NewMsg=Do(Create,Msg)(FnMsg, MSG_WIDGET_MOUSE, 0))) {
		Event=&NewMsg->Event;
		Event->EventMouse.W = LastW;
		Event->EventMouse.ShiftFlags = (udat)0;
		
		i = HOLD_N(LastKeys & HOLD_ANY);
		
		Event->EventMouse.Code = RELEASE_N(i) | (LastKeys &= ~HOLD_CODE(i));
		Event->EventMouse.X = MINDAT;
		Event->EventMouse.Y = MINDAT;
		SendMsg(LastW->Owner, NewMsg);
	    } else
		LastKeys = 0;
	}
    } else
	LastKeys = 0;
}
			    
static void HandleHilightAndSelection(widget W, udat Code, dat X, dat Y, byte Inside) {
    udat _Code;
    byte IsHoldButtonSelection = (Code & HOLD_ANY) == All->SetUp->ButtonSelection;
    
    if (isSINGLE_PRESS(Code) && HOLD_CODE(PRESS_N(Code)) == All->SetUp->ButtonSelection)
	StartHilight((window)W, (ldat)X+W->XLogic, (ldat)Y+W->YLogic);
    else if (IsHoldButtonSelection && isMOVE(Code))
	ExtendHilight((window)W, (ldat)X+W->XLogic, (ldat)Y+W->YLogic);
    else if (isSINGLE_RELEASE(Code)) {
	
	_Code = HOLD_CODE(RELEASE_N(Code));
	
	if (_Code == All->SetUp->ButtonSelection && IS_WINDOW(W))
	    SetSelectionFromWindow((window)W);
	else if (_Code == All->SetUp->ButtonPaste && Inside) {
	    /* send Selection Paste msg */
	    msg NewMsg;
	    
	    /* store selection owner */
	    SelectionImport();
	    
	    if ((NewMsg = Do(Create,Msg)(FnMsg, MSG_SELECTION, 0))) {
		event_any *Event = &NewMsg->Event;
		Event->EventSelection.W = W;
		Event->EventSelection.Code = 0;
		Event->EventSelection.pad = (udat)0;
		Event->EventSelection.X = X;
		Event->EventSelection.Y = Y;
		SendMsg(W->Owner, NewMsg);
	    }
	}
    }
}

static byte CheckForwardMsg(wm_ctx *C, msg Msg, byte WasUsed) {
    static uldat LastWId = NOID;
    static byte LastInside = FALSE;
    static udat LastKeys = 0;
    widget LastW, W, P;
    event_any *Event;
    udat Code;
    dat X, Y;
    byte Inside, inUse = FALSE;
    
    if (Msg->Type != MSG_KEY && Msg->Type != MSG_MOUSE)
	return inUse;

    LastW = (widget)Id2Obj(widget_magic_id, LastWId);

    W = All->FirstScreen->FocusW;

    if ((All->State & STATE_ANY) == STATE_MENU) {
	if (!W)
	    /* the menu is being used, but no menu windows opened yet. continue. */
	    W = (widget)All->FirstScreen->MenuWindow;
	else
	    /* the menu is being used. leave LastW. */
	    W = NULL;
    } else {
	if (All->FirstScreen->ClickWindow && W != (widget)All->FirstScreen->ClickWindow) {
	    /* cannot send messages to focused window while user clicked on another window */
	    W = NULL;
	}
    }

    if (W)
	W = RecursiveFindFocusWidget(W);
    else
	return inUse;

    Event = &Msg->Event;
    if (Msg->Type == MSG_KEY) {
	if (!WasUsed && All->State == STATE_DEFAULT) {
	    
	    /* back up to first parent that WANT_KEYS or has AUTO_KEYS */
	    while (W && !(W->Attrib & WIDGET_WANT_KEYS || (IS_WINDOW(W) && ((window)W)->Attrib & WINDOW_AUTO_KEYS)))
		W = W->Parent;
	    if (W) {
		if (W->Attrib & WIDGET_WANT_KEYS) {
		    Msg->Type = MSG_WIDGET_KEY;
		    Event->EventKeyboard.W = (widget)W;
		    SendMsg(W->Owner, Msg);
		    return TRUE;
		} else if (IS_WINDOW(W) && ((window)W)->Attrib & WINDOW_AUTO_KEYS)
		    FallBackKeyAction((window)W, &Event->EventKeyboard);
	    }
	}
	return inUse;
    }
    
    if (W != LastW && (LastKeys || LastInside)) {
	/* try to leave LastW with a clean status */
	CleanupLastW(LastW, LastKeys, LastInside);
	
	LastW = W;
	LastKeys = 0;
	LastInside = FALSE;
    }

    if (!W->Owner || (IS_WINDOW(W) && ((window)W)->Flags & WINDOWFL_MENU))
	return inUse;

    C->Code = Code = Event->EventMouse.Code;
    C->i = X = Event->EventMouse.X;
    C->j = Y = Event->EventMouse.Y;
    Inside = FALSE;

    if (LastKeys) {
	TranslateCoordsWidget(NULL, W, &X, &Y, &Inside);
	if (C->Pos == POS_ROOT || C->Pos == POS_MENU)
	    Inside = FALSE;
	
    } else {
	
	C->W = NonScreenParent(W);
	C->Screen = (screen)C->W->Parent;
	
	if (C->Screen && IS_SCREEN(C->Screen)) {
	
	    DetailCtx(C);
	
	    if (W == C->W) {
		X -= C->Left + !(IS_WINDOW(W) && W->Flags & WINDOWFL_BORDERLESS);
		Y -= C->Up + !(IS_WINDOW(W) && W->Flags & WINDOWFL_BORDERLESS);
		Inside = C->Pos == POS_INSIDE;
	    } else {
		TranslateCoordsWidget(NULL, W, &X, &Y, &Inside);
		if (C->DW != W || C->Pos == POS_ROOT || C->Pos == POS_MENU)
		    /* W may be obscured by something else */
		    Inside = FALSE;
		while (Inside == TRUE+TRUE && (P = W->Parent)) {
		    /* on the border, must report to the parent */
		    W = P;
		    X = C->i; Y = C->j;
		    TranslateCoordsWidget(NULL, W, &X, &Y, &Inside);
		}
	    }
	}
	if (Inside == TRUE+TRUE)
	    Inside = FALSE;
    }
    
    /* manage window hilight and Selection */
    if (IS_WINDOW(W) && Inside && Code && !LastKeys &&
	(All->State & STATE_ANY) == STATE_DEFAULT && !(W->Attrib & WIDGET_WANT_MOUSE)) {

	HandleHilightAndSelection(W, Code, X, Y, Inside);

	LastWId = W->Id;
	
	return inUse;
    }
    /* finished with Selection stuff */

    
    /* forward the message */
    if (Inside || LastKeys || LastInside) {
	if (Code == MOVE_MOUSE
	    ? (Inside || LastInside) && (W->Attrib & WIDGET_WANT_MOUSE_MOTION)
	    : (Inside || LastKeys) && (W->Attrib & WIDGET_WANT_MOUSE)) {

	    if (Code == MOVE_MOUSE && !Inside)
		X = Y = MINDAT;
	    
	    Msg->Type=MSG_WIDGET_MOUSE;
	    Event->EventMouse.W = (widget)W;
	    Event->EventMouse.X = X;
	    Event->EventMouse.Y = Y;
	    SendMsg(W->Owner, Msg);
	    
	    LastInside = (W->Attrib & WIDGET_WANT_MOUSE_MOTION) ? Inside : 0;
	    LastKeys = (W->Attrib & WIDGET_WANT_MOUSE) ? Code & HOLD_ANY : 0;
	    if (isPRESS(Code))
		LastKeys |= HOLD_CODE(PRESS_N(Code));
	    
	    inUse = TRUE;
	}
    }
    
    LastWId = W->Id;
    
    return inUse;
}



static ldat DragPosition[2];


static void InitCtx(CONST msg Msg, wm_ctx *C) {

    C->Code = Msg->Event.EventCommon.Code;
    C->ShiftFlags = Msg->Event.EventKeyboard.ShiftFlags; /* ok for mouse too */
    
    if ((C->ByMouse = (Msg->Type == MSG_MOUSE))) {
	C->i = Msg->Event.EventMouse.X;
	C->j = Msg->Event.EventMouse.Y;
    
	if ((C->Screen = Do(Find,Screen)(C->j)) && C->Screen == All->FirstScreen
	    && C->Screen->YLimit < C->j) {
	    C->W = Act(FindWidgetAt,C->Screen)(C->Screen, C->i, C->j - C->Screen->YLimit);
	} else
	    C->W = NULL;
    } else {
	C->Screen = All->FirstScreen;
	C->W = C->Screen->FocusW;
    }
}


static void DetailCtx(wm_ctx *C) {
    if (C->W) {
	/* ensure IS_SCREEN(C->W->Parent) is true. */
	C->Screen = (screen)C->W->Parent;
	if (!C->Screen || !IS_SCREEN(C->Screen)) {
	    printk("twin: wm.c: DetailCtx(): internal error: C->W is a subwidget!\n");
	    return;
	}
    }
    
    C->DW = NULL;
	    
    if (C->Screen)
	C->Menu = Act(FindMenu,C->Screen)(C->Screen);

    if (C->ByMouse) {
	C->Pos = POS_ROOT;
	
	if (C->W) {
	    ldat HasBorder = IS_WINDOW(C->W) && !(C->W->Flags & WINDOWFL_BORDERLESS);
	    
	    C->Up=(ldat)C->W->Up - C->Screen->YLogic + (ldat)C->Screen->YLimit;
	    C->Left=(ldat)C->W->Left - C->Screen->XLogic;
	    C->Rgt=C->Left + (ldat)C->W->XWidth - 1;
	    C->Dwn=C->Up+(C->W->Attrib & WINDOW_ROLLED_UP ? 0 : (ldat)C->W->YWidth-(ldat)1);
		
	    if (C->i >= C->Left + HasBorder && C->i <= C->Rgt - HasBorder &&
		C->j >= C->Up + HasBorder && C->j <= C->Dwn - HasBorder) {
		
		C->Pos = POS_INSIDE;
		C->DW = RecursiveFindWidgetAt((widget)C->W, C->i - C->Left, C->j - C->Up);
	    } else if (HasBorder) {
		/*
		 * (i,j) _may_ be outside the window... see ContinueMenu()
		 * and CheckForwardMsg()
		 */
		if (IS_WINDOW(C->W) && (C->i == C->Left || C->i == C->Rgt || C->j == C->Up || C->j == C->Dwn))
		    C->Pos = Act(FindBorder,(window)C->W)((window)C->W, C->i - C->Left, C->j - C->Up, 0, NULL);
	    }
	}
	/*
	 * allow code above to calculate C->Left, C->Up
	 * (used for Interactive Drag/Resize/Scroll)
	 * but return the correct C->Pos :
	 */
	if (C->Screen && C->j <= C->Screen->YLimit) {
	    C->Pos = POS_ROOT;
	    if (C->j == C->Screen->YLimit) {
		if (C->i > All->DisplayWidth - (dat)3)
		    C->Pos = POS_BUTTON_SCREEN;
		else {
		    C->Pos = POS_MENU;
		    C->W = C->Screen->FocusW;
		    C->Item = Act(FindItem,C->Menu)(C->Menu, C->i);
		}
	    }
	}
    }
}

INLINE void Fill4RC_VM(wm_ctx *C, widget W, udat Type, byte Pos, udat Code) {
    C->W = W;
    C->Type = Type;
    C->Pos = Pos;
    C->Code = Code;
}

void FocusCtx(wm_ctx *C) {
    if (C->W)
	C->Screen = ScreenParent(C->W);
    
    if (C->Screen && C->Screen != All->FirstScreen)
	Act(Focus,C->Screen)(C->Screen);
    else
	C->Screen = All->FirstScreen;
	
    if (C->W && C->W != C->Screen->FocusW)
	Act(Focus,C->W)(C->W);
    else
	C->W = C->Screen->FocusW;
}

static byte ActivateScreen(wm_ctx *C) {
    if (C->Screen && C->Screen != All->FirstScreen)
	Act(Focus,C->Screen)(C->Screen);
    C->Screen = All->FirstScreen;
    All->State = STATE_SCREEN | (C->ByMouse ? STATE_FL_BYMOUSE : 0);
    Act(DrawMenu,C->Screen)(C->Screen, 0, MAXDAT);
    return TRUE;
}

/* this is mouse-only */
static void ContinueScreen(wm_ctx *C) {
    ResizeFirstScreen(C->j - All->FirstScreen->YLimit);
}

static void ReleaseScreen(wm_ctx *C) {
    All->State = STATE_DEFAULT;
    Act(DrawMenu,All->FirstScreen)(All->FirstScreen, 0, MAXDAT);
}

/* this is mouse-only */
static byte ActivateScreenButton(wm_ctx *C) {
    if (C->Screen == All->FirstScreen && (All->State & STATE_ANY) == STATE_DEFAULT) {
	All->State = STATE_BUTTON_SCREEN | STATE_FL_BYMOUSE;
	C->Screen->Flags |= SCREENFL_BACK_SELECT | SCREENFL_BACK_PRESSED;
	Act(DrawMenu,C->Screen)(C->Screen, All->DisplayWidth-(dat)2, All->DisplayWidth-(dat)1);
	return TRUE;
    }
    return FALSE;
}

#if 0
/* this is mouse-only */
static void ContinueScreenButton(wm_ctx *C) {
    udat temp = C->Screen->State;
    
    DetailCtx(C);
    
    if (C->Pos == POS_BUTTON_SCREEN)
	C->Screen->State |= GADGET_PRESSED;
    else
	C->Screen->State &= ~GADGET_PRESSED;
    if (temp != C->Screen->State)
	Act(DrawMenu,C->Screen)(C->Screen, All->DisplayWidth-(dat)2, All->DisplayWidth-(dat)1);
}
#endif

/* this is mouse-only */
static void ReleaseScreenButton(wm_ctx *C) {
    
    All->State = STATE_DEFAULT;
    C->Screen->Flags &= ~(SCREENFL_BACK_SELECT|SCREENFL_BACK_PRESSED);
    
    if (C->Screen != All->LastScreen && (DetailCtx(C), C->Pos == POS_BUTTON_SCREEN)) {
	MoveLast(Screen, All, C->Screen);
	DrawArea2(NULL, NULL, NULL,
		 0, Min2(C->Screen->YLimit, All->FirstScreen->YLimit),
		 MAXDAT, MAXDAT, FALSE);
	UpdateCursor();
    } else
	Act(DrawMenu,C->Screen)(C->Screen, All->DisplayWidth-(dat)2, All->DisplayWidth-(dat)1);
}

static byte ActivateMenu(wm_ctx *C) {
    if (C->Screen && C->Screen != All->FirstScreen)
	Act(Focus,C->Screen)(C->Screen);
    C->Screen = All->FirstScreen;
    C->W = C->Screen->FocusW;
    C->Menu = Act(FindMenu,C->Screen)(C->Screen); 
	
    if (C->ByMouse) {
	if (C->j == C->Screen->YLimit)
	    C->Item = Act(FindItem,C->Menu)(C->Menu, C->i);
	else
	    C->Item = (menuitem)0;
    } else {
	if (!(C->Item = Act(GetSelectedItem,C->Menu)(C->Menu)) &&
	    !(C->Item = C->Menu->FirstI))
	    
	    C->Item = All->CommonMenu->FirstI;
	
    }
    Act(ActivateMenu,C->Screen)(C->Screen, C->Item, C->ByMouse);
    
    return TRUE;
}

/* this is mouse-only */
static void ContinueMenu(wm_ctx *C) {
    window W;
    uldat y;
    
    DetailCtx(C);

    if (C->Pos == POS_MENU && C->Item && C->Item != Act(GetSelectedItem,C->Menu)(C->Menu))
	SetMenuState(C->Item, TRUE);
    else if (C->Pos == POS_INSIDE && (W = (window)C->W) && (W->Flags & WINDOWFL_MENU)) {
	y = (ldat)C->j - C->Up - (ldat)1 + W->YLogic;
	if ((C->Item = (menuitem)Act(FindRow,W)(W, y))) {
	    SetMenuState(C->Item, TRUE);
	    return;
	}
    } else if ((W = (window)All->FirstScreen->FocusW) &&
	       (W->Flags & WINDOWFL_MENU) &&
	       (C->Item = (menuitem)Act(FindRow,W)(W, W->CurY)) &&
	       IS_MENUITEM(C->Item) && !C->Item->Window) {

	if ((W = (window)C->Item->Parent) && IS_WINDOW(W))
	    SetMenuState(C->Item = W->MenuItem, TRUE);
	else
	    SetMenuState(C->Item = Act(GetSelectedItem,C->Menu)(C->Menu), TRUE);
    }
}

static void ReleaseMenu(wm_ctx *C) {
    window MW = All->FirstScreen->MenuWindow;
    window FW = (window)All->FirstScreen->FocusW;
    menu Menu;
    menuitem Item;
    row Row;
    msg Msg;
    event_menu *Event;
    udat Code;
    
    
    if (FW && IS_WINDOW(FW) && FW->CurY < MAXLDAT && (Menu = FW->Menu) &&
	(Item = Act(GetSelectedItem,Menu)(Menu)) && (Item->Flags & ROW_ACTIVE) &&
	(Row = Act(FindRow,FW)(FW, FW->CurY)) &&
	(Row->Flags & ROW_ACTIVE) && Row->Code)
	
	Code = Row->Code;
    else
	Code = (udat)0;

    /* disable the menu BEFORE processing the code! */
    CloseMenu();

    if (Code >= COD_RESERVED) {
	/* handle COD_RESERVED codes internally */
	Fill4RC_VM(C, (widget)MW, MSG_MENU_ROW, POS_MENU, Row->Code);
	(void)RC_VMQueue(C);
    } else if (Code) {
	if ((Msg=Do(Create,Msg)(FnMsg, MSG_MENU_ROW, 0))) {
	    Event=&Msg->Event.EventMenu;
	    Event->W = MW;
	    Event->Code = Code;
	    Event->Menu = Menu;
	    Event->Row = Row;
	    if (MW)
		SendMsg(MW->Owner, Msg);
	    else
		SendMsg(Menu->MsgPort, Msg);
	}
    }
}

static void ShowResize(window W) {
    static byte buf[40];
    dat x = W->XWidth;
    dat y = W->YWidth;
    
    if (!(W->Flags & WINDOWFL_BORDERLESS))
	x -= 2, y -= 2;
	
    sprintf(buf, "%hdx%hd", x, y);
    Act(SetText,All->BuiltinRow)(All->BuiltinRow, (x = LenStr(buf)), buf, 0);
    Act(DrawMenu,All->FirstScreen)(All->FirstScreen, All->DisplayWidth - 20, All->DisplayWidth - 10);
}

static void HideResize(void) {
    Act(SetText,All->BuiltinRow)(All->BuiltinRow, 0, NULL, 0);
    Act(DrawMenu,All->FirstScreen)(All->FirstScreen, All->DisplayWidth - 20, All->DisplayWidth - 10);
}

static byte ActivateDrag(wm_ctx *C) {
    if (C->Screen == All->FirstScreen && C->W && IS_WINDOW(C->W) &&
	C->W->Attrib & WINDOW_DRAG) {
	
	All->FirstScreen->ClickWindow = (window)C->W;
	All->State = STATE_DRAG;
	if (C->ByMouse) {
	    All->State |= STATE_FL_BYMOUSE;

	    DetailCtx(C);
	    DragPosition[0] = (ldat)C->i - C->Left;
	    DragPosition[1] = (ldat)C->j - C->Up;
	}
	DrawBorderWindow((window)C->W, BORDER_ANY);
	return TRUE;
    }
    return FALSE;
}

static byte ActivateResize(wm_ctx *C) {
    if (C->Screen == All->FirstScreen && C->W && IS_WINDOW(C->W) &&
	C->W->Attrib & WINDOW_RESIZE) {

	All->FirstScreen->ClickWindow = (window)C->W;
	All->State = STATE_RESIZE;
	if (C->ByMouse) {
	    All->State |= STATE_FL_BYMOUSE;
	    
	    DetailCtx(C);
	    DragPosition[0] = (ldat)C->i - C->Rgt;
	    DragPosition[1] = (ldat)C->j - C->Dwn;
	}
	DrawBorderWindow((window)C->W, BORDER_ANY);
	ShowResize((window)C->W);
	return TRUE;
    }
    return FALSE;
}

static byte ActivateScroll(wm_ctx *C) {
    window W = (window)C->W;
    
    if (C->Screen == All->FirstScreen && W && IS_WINDOW(W) &&
	C->W->Attrib & (WINDOW_X_BAR | WINDOW_Y_BAR)) {

	/*
	 * paranoia: SneakSetupMouse() mail call us even for a mouse event
	 * OUTSIDE or INSIDE C->W
	 */
	if (C->ByMouse) {
	    DetailCtx(C);
	    if ((ldat)C->j == C->Dwn) {
		if (C->Pos == POS_X_ARROW_BACK)
		    W->State |= X_BAR_SELECT|ARROW_BACK_SELECT;
		else if (C->Pos == POS_X_ARROW_FWD)
		    W->State |= X_BAR_SELECT|ARROW_FWD_SELECT;
		else if (C->Pos == POS_X_BAR_BACK)
		    W->State |= X_BAR_SELECT|PAGE_BACK_SELECT;
		else if (C->Pos == POS_X_BAR_FWD)
		    W->State |= X_BAR_SELECT|PAGE_FWD_SELECT;
		else {
		    W->State |= X_BAR_SELECT|TAB_SELECT;
		    DragPosition[0] = (ldat)C->i - C->Left - 1 - TabStart(W, (sbyte)1);
		    DragPosition[1] = 0;
		}
	    } else if ((ldat)C->i == C->Rgt) {
		if (C->Pos == POS_Y_ARROW_BACK)
		    W->State |= Y_BAR_SELECT|ARROW_BACK_SELECT;
		else if (C->Pos == POS_Y_ARROW_FWD)
		    W->State |= Y_BAR_SELECT|ARROW_FWD_SELECT;
		else if (C->Pos == POS_Y_BAR_BACK)
		    W->State |= Y_BAR_SELECT|PAGE_BACK_SELECT;
		else if (C->Pos == POS_Y_BAR_FWD)
		    W->State |= Y_BAR_SELECT|PAGE_FWD_SELECT;
		else {
		    W->State |= Y_BAR_SELECT|TAB_SELECT;
		    DragPosition[0] = 0;
		    DragPosition[1] = (ldat)C->j - C->Up - 1 - TabStart(W, (sbyte)0);
		}
	    }
	    if (W->State & SCROLL_ANY_SELECT)
		All->State = STATE_SCROLL | STATE_FL_BYMOUSE;
	} else
	    All->State = STATE_SCROLL;
	    
	if ((All->State & STATE_ANY) == STATE_SCROLL && IS_WINDOW(W)) {
	    All->FirstScreen->ClickWindow = W;
	    DrawBorderWindow(W, BORDER_ANY);
	    return TRUE;
	}
    }
    return FALSE;
}

/* this is mouse only */
static void ContinueDrag(wm_ctx *C) {
    if ((C->W = (widget)All->FirstScreen->ClickWindow)) {
	DetailCtx(C);
	if (C->W == All->FirstScreen->FirstW)
	    DragFirstWindow(C->i - C->Left - DragPosition[0],
			    Max2(C->j, All->FirstScreen->YLimit+1) - C->Up - DragPosition[1]);
	else
	    DragWindow((window)C->W, C->i - C->Left - DragPosition[0],
		       Max2(C->j, All->FirstScreen->YLimit+1) - C->Up - DragPosition[1]);
    }
}

/* this is mouse only */
static void ContinueResize(wm_ctx *C) {
    
    if ((C->W = (widget)All->FirstScreen->ClickWindow)) {
	DetailCtx(C);
	if (C->W == All->FirstScreen->FirstW)
	    ResizeRelFirstWindow(C->i - C->Rgt - DragPosition[0],
				 Max2(C->j, All->FirstScreen->YLimit+1) - C->Dwn - DragPosition[1]);
	else
	    ResizeRelWindow((window)C->W, C->i - C->Rgt - DragPosition[0],
			    Max2(C->j, All->FirstScreen->YLimit+1) - C->Dwn - DragPosition[1]);
	ShowResize((window)C->W);
    }
}

/* this is mouse only */
static void ContinueScroll(wm_ctx *C) {
    window W;
    uldat NumLogicMax;
    ldat i;
    
    if ((C->W = (widget)(W = All->FirstScreen->ClickWindow))) {
	DetailCtx(C);
    
	if (W->State & X_BAR_SELECT) {
	    NumLogicMax=Max2(W->WLogic, W->XLogic + (ldat)W->XWidth - 2);
	    i = C->i;
	    if (i + 4 > C->Rgt + DragPosition[0])
		i = C->Rgt + DragPosition[0] - 4;
	    ScrollWindow(W, (i - C->Left - 1 - DragPosition[0] - TabStart(W, (sbyte)1)) * 
			 (NumLogicMax / (W->XWidth - 5)), 0);

	} else if (W->State & Y_BAR_SELECT) {
	    NumLogicMax=Max2(W->HLogic, W->YLogic + (ldat)W->YWidth - 2);
	    i = Max2(C->j, All->FirstScreen->YLimit+1);
	    if (i + 3 > C->Dwn + DragPosition[1])
		i = C->Dwn + DragPosition[1] - 3;
	    ScrollWindow(W, 0, (i - C->Up - 1 - DragPosition[1] - TabStart(W, (sbyte)0)) * 
			 (NumLogicMax / (W->YWidth - (udat)4)));
	}
    }
}

static void ReleaseDragResizeScroll(CONST wm_ctx *C) {
    window FW = All->FirstScreen->ClickWindow;
    udat wasResize;
    
    wasResize = (All->State & STATE_ANY) == STATE_RESIZE;
    All->State = STATE_DEFAULT;
    
    if (FW) {
	FW->State &= ~(BUTTON_ANY_SELECT | SCROLL_ANY_SELECT | XY_BAR_SELECT);
	DrawBorderWindow(FW, BORDER_ANY);
	
	if (wasResize) {
	    Check4Resize(FW);
	    HideResize();
	}
    }
}

/* this is mouse only */
static byte ActivateButton(wm_ctx *C) {
    if (C->W && IS_WINDOW(C->W)) {
	All->State = C->Pos | STATE_FL_BYMOUSE;
	((window)C->W)->State |= (BUTTON_FIRST_SELECT << C->Pos) | WINDOW_GADGET_PRESSED;

	C->Type = MSG_MOUSE;
	(void)RC_VMQueue(C);
    
	DrawBorderWindow((window)C->W, BORDER_UP);
	return TRUE;
    }
    return FALSE;
}

#if 0
/* this is mouse only */
/*
 * this would release window border buttons when moving away from them...
 * I prefer them to stay pressed, so this is disabled
 */
static void ContinueButton(wm_ctx *C) {
    window FW = All->FirstScreen->ClickWindow;
    uldat ltemp;
    byte found = FALSE;
    
    if (!FW)
	return;
    
    if (FW == C->W && (ltemp = FW->State) & BUTTON_ANY_SELECT) {
	DetailCtx(C);
	if (C->Pos < BUTTON_MAX &&
	    (ltemp & BUTTON_ANY_SELECT) == (BUTTON_FIRST_SELECT << C->Pos))
		
	    found = TRUE;
    }
    
    if (found)
	FW->State |= GADGET_PRESSED;
    else
	FW->State &= ~GADGET_PRESSED;
    if (ltemp != FW->State)
	DrawBorderWindow(FW, BORDER_UP);
}
#endif

/* this is mouse only */
static void ReleaseButton(wm_ctx *C) {
    window FW = All->FirstScreen->ClickWindow;
    
    All->State = STATE_DEFAULT;
    if (FW) {
	if (FW == (window)C->W && FW->State & BUTTON_ANY_SELECT) {
	    DetailCtx(C);
    
	    if (C->Pos < BUTTON_MAX &&
		(FW->State & BUTTON_ANY_SELECT) == (BUTTON_FIRST_SELECT << C->Pos)) {
	    
		C->W = (widget)FW;
		C->Type = MSG_MOUSE;
		(void)RC_VMQueue(C);
	    }
	}
	FW->State &= ~(BUTTON_ANY_SELECT|WINDOW_GADGET_PRESSED);
	DrawBorderWindow(FW, BORDER_UP);
    }
}

/* this is mouse only */
static byte ActivateGadget(wm_ctx *C) {
    gadget G = (gadget)C->DW;
    All->State = STATE_GADGET | STATE_FL_BYMOUSE;
    if (!G->Group && (G->Flags & (GADGETFL_TOGGLE|GADGETFL_PRESSED))
	== (GADGETFL_TOGGLE|GADGETFL_PRESSED))
	UnPressGadget(G, TRUE);
    else
	PressGadget(G);
    if (!(G->Flags & GADGETFL_TOGGLE))
	RecursiveFocusWidget((widget)G);
    return TRUE;
}

/* this is mouse only */
static void ContinueGadget(wm_ctx *C) {
    window FW = All->FirstScreen->ClickWindow;
    gadget FG;
    udat temp;
    
    if (FW && (FG = (gadget)RecursiveFindFocusWidget((widget)FW))) {
	temp = FG->Flags;

	if (!(temp & GADGETFL_TOGGLE)) {
	    if (FW == (window)C->W && FG && (DetailCtx(C), (widget)FG == C->DW))
		FG->Flags |= GADGETFL_PRESSED;
	    else
		FG->Flags &= ~GADGETFL_PRESSED;
	
	    if (temp != FG->Flags) {
		if ((widget)FW == All->FirstScreen->FirstW)
		    DrawWidget((widget)FG, 0, 0, MAXDAT, MAXDAT, FALSE);
		else
		    DrawAreaWidget((widget)FG);
	    }
	}
    }
}

/* this is mouse only */
static void ReleaseGadget(wm_ctx *C) {
    window FW = All->FirstScreen->ClickWindow;
    gadget FG;

    All->State = STATE_DEFAULT;
    if (!FW)
	return;
    
    DetailCtx(C);

    FG = (gadget)RecursiveFindFocusWidget((widget)FW);
    
    if (!FG || !IS_GADGET(FG) || FG->Flags & GADGETFL_TOGGLE)
	return;
    
    UnPressGadget(FG, (widget)FW == C->W && FG && (widget)FG == C->DW);
    /* FW->SelectW=NULL; */
}

/* the only Activate*() that make sense from within RC_VM() */
byte ActivateCtx(wm_ctx *C, byte State) {
    switch (State) {
      case STATE_RESIZE: return ActivateResize(C);
      case STATE_DRAG:   return ActivateDrag(C);  
      case STATE_SCROLL: return ActivateScroll(C);
      case STATE_MENU:	 return ActivateMenu(C);  
      case STATE_SCREEN: return ActivateScreen(C);
      default: break;
    }
    return FALSE;
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
	    window FW;
	    gadget FG;

	    if ((FW = All->FirstScreen->ClickWindow) &&
		(FG = (gadget)RecursiveFindFocusWidget((widget)FW))
		&& IS_GADGET(FG) && !(FG->Flags & GADGETFL_TOGGLE))
		
		UnPressGadget(FG, FALSE);
	}
	break;
      case STATE_MENU:
	CloseMenu();
	break;
      case STATE_SCREEN:
	break;
      case STATE_BUTTON_SCREEN:
	All->FirstScreen->Flags &= ~(SCREENFL_BACK_SELECT|SCREENFL_BACK_PRESSED);
	break;
      default:
	if ((All->State & STATE_ANY) < BUTTON_MAX) {
	    window FW;
	    if ((FW = All->FirstScreen->ClickWindow))
		FW->State &= ~(BUTTON_ANY_SELECT|WINDOW_GADGET_PRESSED);
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
    byte isPressButtonSelection;
    
    if (isSINGLE_PRESS(C->Code)) {
	isPressButtonSelection = HOLD_CODE(PRESS_N(C->Code)) == All->SetUp->ButtonSelection;
	
	switch (C->Pos) {
	  case POS_BUTTON_SCREEN:
	    if (isPressButtonSelection)
		used = TRUE, ActivateScreenButton(C);
	    break;
	  case POS_INSIDE:
	    if (isPressButtonSelection && C->DW && IS_GADGET(C->DW) &&
		!(((gadget)C->DW)->Flags & GADGETFL_DISABLED))
		used = TRUE, ActivateGadget(C);
	    break;
	  default:
	    if (C->Pos < BUTTON_MAX)
		used = TRUE, ActivateButton(C);
	    break;
	}
    }
    return used;
}

/*
 * check if the mouse (C->i, C->j) is in a position suitable for the State.
 * setup DragPosition[] as if the current State was initiated with the mouse,
 * or return FALSE if (C->i, C->j) is in a non-appropriate position.
 */
static byte SneakSetupMouse(wm_ctx *C) {
    /* State was set with keyboard */
    window W = All->FirstScreen->ClickWindow;
    byte ok = TRUE;
    
    switch (All->State & STATE_ANY) {
      case STATE_RESIZE:
	DetailCtx(C);
	if (IS_WINDOW(W) && W == (window)C->W) {
	    DragPosition[0] = (ldat)C->i - C->Rgt;
	    DragPosition[1] = (ldat)C->j - C->Dwn;
	} else
	    ok = FALSE;
	break;
      case STATE_DRAG:
	DetailCtx(C);
	if (IS_WINDOW(W) && W == (window)C->W) {
	    DragPosition[0] = (ldat)C->i - C->Left;
	    DragPosition[1] = (ldat)C->j - C->Up;
	} else
	    ok = FALSE;
	break;
      case STATE_SCROLL:
	/*
	 * this is complex... we must setup W->State with what you clicked upon.
	 * do the dirty way and reuse the function ActivateScroll().
	 * since it returns (void), check for (W->State & SCROLL_ANY_SELECT).
	 */
	if (IS_WINDOW(W) && W == (window)C->W) {
	    ActivateScroll(C);
	    ok = !!(W->State & SCROLL_ANY_SELECT);
	} else
	    ok = FALSE;
	break;
      case STATE_GADGET:
      case STATE_MENU:
      case STATE_SCREEN:
      case STATE_BUTTON_SCREEN:
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
	  case STATE_BUTTON_SCREEN: ReleaseScreenButton(C); break;
	  case STATE_ROOT:
	  case STATE_DEFAULT:
	  default:
	    if (State < BUTTON_MAX)
		ReleaseButton(C);
	    else
		/* paranoid... */
		All->State = STATE_DEFAULT;
	    break;
	}
    } else if (isSINGLE_PRESS(C->Code) || isSINGLE_MOVE(C->Code) || isRELEASE(C->Code)) {
	switch (State) {
	  case STATE_RESIZE: ContinueResize(C); break;
	  case STATE_DRAG:   ContinueDrag(C);   break;
	  case STATE_SCROLL: ContinueScroll(C); break;
	  case STATE_GADGET: ContinueGadget(C); break;
	  case STATE_MENU:   ContinueMenu(C);   break;
	  case STATE_SCREEN: ContinueScreen(C); break;
	  /*case STATE_BUTTON_SCREEN: ContinueScreenButton(C); break; */
	  case STATE_ROOT: case STATE_DEFAULT:  break;
	  default:
	    /* if (State < BUTTON_MAX)
	     *  ContinueButton(C); */
	     break;
	}
    }
}

static menuitem PrevItem(menuitem Item, menu Menu) {
    menuitem Prev;
    
    if (!(Prev = Item->Prev)) {
	if (Item->Parent == (obj)Menu) {
	    if (Menu->CommonItems && All->CommonMenu)
		Prev = All->CommonMenu->LastI;
	} else
	    Prev = Menu->LastI;
    }
    
    if (Prev)
	return Prev;
    return Item;
}

static menuitem NextItem(menuitem Item, menu Menu) {
    menuitem Next;
    
    if (!(Next = Item->Next)) {
	if (Item->Parent == (obj)Menu) {
	    if (Menu->CommonItems && All->CommonMenu)
		Next = All->CommonMenu->FirstI;
	} else
	    Next = Menu->FirstI;
    }
    
    if (Next)
	return Next;
    return Item;
}

static void EnterItem(menuitem Item) {
    window W = Item->Window;
    if (!(Item = (menuitem)Act(FindRow,W)(W,W->CurY)))
	Item = (menuitem)Act(FindRow,W)(W,0);
    if (Item && IS_MENUITEM(Item))
	SetMenuState(Item, FALSE);
}

/* handle keyboard during various STATE_* */
/* this is keyboard only */
static byte ActivateKeyState(wm_ctx *C, byte State) {
    window W = (window)All->FirstScreen->FocusW;
    ldat NumRow;
    dat XDelta = 0, YDelta = 0, depth;
    udat Key = C->Code;
    byte used = FALSE;
    menuitem M;
    
    if (!W || !IS_WINDOW(W))
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
	else if (State == STATE_RESIZE && (W->Attrib & WINDOW_RESIZE)) {
	    used = TRUE;
	    ResizeRelWindow(W, XDelta, YDelta);
	    ShowResize(W);
	}
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
	C->Menu = Act(FindMenu,C->Screen)(C->Screen);
	C->Item = Act(GetSelectedItem,C->Menu)(C->Menu);
	M = Act(RecursiveGetSelectedItem,C->Menu)(C->Menu, &depth);
	switch (Key) {
	  case TW_Escape:
	    if (depth <= 1)
		CloseMenu();
	    else
		SetMenuState(W->MenuItem, FALSE);
	    used = TRUE;
	    break;
	  case TW_Return:
	    if (M && IS_MENUITEM(M) && M->Window)
		EnterItem(M);
	    else
		ReleaseMenu(C);
	    used = TRUE;
	    break;
	  case TW_Left:
	    if (depth > 0 &&
		(All->SetUp->Flags & SETUP_MENU_RELAX) &&
		(W = (window)M->Parent) && IS_WINDOW(W) &&
		(M = W->MenuItem) &&
		(W = (window)M->Parent) && IS_WINDOW(W))
		
		SetMenuState(M, FALSE);
	    else if (depth <= 1)
		SetMenuState(C->Item = PrevItem(C->Item, C->Menu), FALSE);
	    used = TRUE;
	    break;
	  case TW_Right:
	    if (depth > 0 &&
		(All->SetUp->Flags & SETUP_MENU_RELAX) && M->Window)
		EnterItem(M);
	    else if (depth <= 1)
		SetMenuState(C->Item = NextItem(C->Item, C->Menu), FALSE);
	    used = TRUE;
	    break;
	  case TW_Up:
	    if (!W->HLogic || (All->State & STATE_FL_BYMOUSE))
		break;
	    NumRow = W->CurY;
	    if (NumRow && NumRow < W->HLogic)
		NumRow--;
	    else
		NumRow = W->HLogic - 1;
	    M = (menuitem)Act(FindRow,W)(W,NumRow);
	    if (M && IS_MENUITEM(M))
		SetMenuState(M, FALSE);
	    used = TRUE;
	    break;
	  case TW_Down:
	    if (!W->HLogic || (All->State & STATE_FL_BYMOUSE))
		break;
	    NumRow = W->CurY;
	    if (NumRow < W->HLogic - 1)
		NumRow++;
	    else
		NumRow = 0;
	    M = (menuitem)Act(FindRow,W)(W,NumRow);
	    if (M && IS_MENUITEM(M))
		SetMenuState(M, FALSE);
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

/*
 * automatically recursively focus a subwidget on mouse motion
 * if it and all its non-screen parents have WIDGET_AUTO_FOCUS flag set
 */
static void TryAutoFocus(wm_ctx *C) {
    widget W, DeepW, OldW, FocusW = All->FirstScreen->FocusW;

    if (!FocusW)
	FocusW = (widget)All->FirstScreen->MenuWindow;

    if (!FocusW)
	return;
    
    OldW = RecursiveFindFocusWidget((widget)All->FirstScreen);

    if ((W = C->W) && W == FocusW && (DeepW = C->DW) && DeepW != OldW) {

	/*
	 * must have AUTO_FOCUS to be autofocused...
	 * and focusing top-level widgets is handled elsewhere
	 */
	if (!(DeepW->Attrib & WIDGET_AUTO_FOCUS) ||
	    (W == DeepW && W->Parent->SelectW != W))
	    return;
	
	FocusW = DeepW;
	
	/* climb through all AUTO_FOCUS widgets */
	while (DeepW != W) {
	    if (!(DeepW = DeepW->Parent))
		return;
	    if (!(DeepW->Attrib & WIDGET_AUTO_FOCUS))
		break;
	}
	/* climb through all already focused widgets */
	while (DeepW != W && DeepW->Parent && DeepW->Parent->SelectW == DeepW) {
	    DeepW = DeepW->Parent;
	}
	if (DeepW == W) {
	    RecursiveFocusWidget(FocusW);
#ifdef DEBUG_WM
	    printk("autofocus: 0x%x -> 0x%x\n", OldW ? OldW->Id : NOID, FocusW->Id);
#endif
	}
    }
}

/* the Window Manager built into Twin */
static void WManagerH(msgport MsgPort) {
    static wm_ctx _C;
    wm_ctx *C = &_C;
    msg Msg;
    widget W;
    byte used;
    
    while ((Msg=WM_MsgPort->FirstMsg)) {
	
	Remove(Msg);
	
	if (Msg->Type==MSG_MAP) {
	    SendMsg(MapQueue, Msg);
	    continue;
	} else if (Msg->Type != MSG_MOUSE && Msg->Type != MSG_KEY) {
	    if (Msg->Type == MSG_CONTROL) {
		/*
		 * for now, don't allow starting a different WM (MSG_CONTROL_RESTART)
		 * but just restart this one (MSG_CONTROL_OPEN)
		 */
		if ((Msg->Event.EventControl.Code == MSG_CONTROL_RESTART ||
		     Msg->Event.EventControl.Code == MSG_CONTROL_OPEN)) {
		
		    Fill4RC_VM(C, NULL, MSG_CONTROL, POS_ROOT, MSG_CONTROL_OPEN);
		    (void)RC_VMQueue(C);
		}
	    }
	    Delete(Msg);
	    continue;
	}
	
	InitCtx(Msg, C);

	if (All->State == STATE_DEFAULT) {
	    if (C->ByMouse && isSINGLE_PRESS(C->Code)) {
		if (C->Screen && C->Screen != All->FirstScreen) {
		    Act(Focus,C->Screen)(C->Screen);
		    InitCtx(Msg, C);
		}
		if (HOLD_CODE(PRESS_N(C->Code)) == All->SetUp->ButtonSelection) {
		    
		    if (C->W && C->W != C->Screen->FocusW)
			Act(Focus,C->W)(C->W);
		    
		    DetailCtx(C);
		    W = RecursiveFindFocusWidget((widget)All->FirstScreen);
		    if (C->DW && C->DW != W)
			RecursiveFocusWidget(C->DW);
		    
		    All->State |= STATE_FL_BYMOUSE;
		} else
		    DetailCtx(C);
		
		/* mouse action, setup ClickWindow */
		ClickWindowPos = C->Pos;
		All->FirstScreen->ClickWindow = (window)C->W;

		used = ActivateMouseState(C);
	    } else {
		used = FALSE, DetailCtx(C);

		if (C->ByMouse) {
		    if (!isPRESS(C->Code) && !(C->Code & HOLD_ANY))
			TryAutoFocus(C);
		} else
		    /* for keyboard actions, ClickWindow == FocusW */
		    All->FirstScreen->ClickWindow = (window)C->W;
	    }

	    if (!used) {
		C->Type = C->ByMouse ? MSG_MOUSE : MSG_KEY;
		used = RC_VMQueue(C);
	    }
	    	    
	} else if (C->ByMouse) {
	
	    /*
	     * if you use the mouse during a keyboard-activated STATE_xxx,
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

	} else /* (!C->ByMouse) */ {
	    used = ActivateKeyState(C, All->State);
	}

	/* cleanup ClickWindow if not needed anymore */
	if ((All->State & STATE_ANY) == STATE_DEFAULT &&
	    (!C->ByMouse || !isPRESS(C->Code) || !(C->Code & HOLD_ANY))) {
	    
	    ClickWindowPos = MAXBYTE;
	    All->FirstScreen->ClickWindow = NULL;
	}

	/* must we send the event to the focused window too ? */
	if (C->ByMouse || (All->State == STATE_DEFAULT && !used)) {

	    if (CheckForwardMsg(C, Msg, used))
		/* don't Delete(Msg) ! */
		continue;
	}

	Delete(Msg);	
    }

    if (All->State == STATE_DEFAULT) {
	for (used = 30, Msg = MapQueue->FirstMsg; Msg && used; Msg = Msg->Next, used--)
	    ;
	if (!used)
	    QueuedDrawArea2FullScreen = TRUE;
	while ((Msg = MapQueue->FirstMsg)) {
	    C->W = Msg->Event.EventMap.W;
	    SmartPlace((widget)C->W, Msg->Event.EventMap.Screen);

	    Fill4RC_VM(C, C->W, MSG_MAP, POS_ROOT, 0);
	    (void)RC_VMQueue(C);

	    Delete(Msg);
	}
    }
    
    if (All->MouseHW && All->MouseHW->MouseState.keys && Scroller_MsgPort->WakeUp != TIMER_ALWAYS) {
	extern msg Do_Scroll;
	Scroller_MsgPort->WakeUp = TIMER_ALWAYS;
	SendMsg(Scroller_MsgPort, Do_Scroll);
    } else if ((!All->MouseHW || !All->MouseHW->MouseState.keys) && Scroller_MsgPort->WakeUp == TIMER_ALWAYS) {
	extern msg Dont_Scroll;
	SendMsg(Scroller_MsgPort, Dont_Scroll);
    }

    
    if (RC_VM(&WM_MsgPort->PauseDuration)) {
	/* sleep specified amount of time */
	WM_MsgPort->WakeUp |= TIMER_ONCE;
    }
}

static dat XWidth, YWidth;

static byte doSmartPlace(widget W, dat *X, udat *Y) {
    dat WLeft, WRgt, TryX[2];
    dat WUp, WDwn, TryY[2];
    byte OK = FALSE;

    if (XWidth > X[1] - X[0] + 1 || YWidth > Y[1] - Y[0] + 1)
	return FALSE;
    
    if (!W)
	return TRUE;
	
    WRgt = (WLeft = W->Left) + W->XWidth;
    WDwn = (WUp = W->Up) + (IS_WINDOW(W) && (((window)W)->Attrib & WINDOW_ROLLED_UP)
			    ? 1 : W->YWidth);
    W = W->Next;
	
    if (X[0] >= WRgt || X[1] < WLeft || Y[0] >= WDwn || Y[1] < WUp)
	return W ? doSmartPlace(W, X, Y) : TRUE;
    
    if (Y[0] < WUp) {
	TryX[0] = X[0]; TryX[1] = X[1];
	TryY[0] = Y[0]; TryY[1] = WUp - 1;
	OK = doSmartPlace(W, TryX, TryY);
    }
    if (!OK && X[0] < WLeft) {
	TryX[0] = X[0]; TryX[1] = WLeft - 1;
	TryY[0] = Y[0]; TryY[1] = Y[1];
	OK = doSmartPlace(W, TryX, TryY);
    }
    if (!OK && X[1] >= WRgt) {
	TryX[0] = WRgt; TryX[1] = X[1];
	TryY[0] = Y[0]; TryY[1] = Y[1];
	OK = doSmartPlace(W, TryX, TryY);
    }
    if (!OK && Y[1] >= WDwn) {
	TryX[0] = X[0]; TryX[1] = X[1];
	TryY[0] = WDwn; TryY[1] = Y[1];
	OK = doSmartPlace(W, TryX, TryY);
    }
    if (OK) {
	X[0] = TryX[0]; X[1] = TryX[1];
	Y[0] = TryY[0]; Y[1] = TryY[1];
    }
    return OK;
}

#define MAXLRAND48 0x80000000l

static void SmartPlace(widget W, screen Screen) {
    dat X[2];
    dat Y[2];
    
    if (!W || W->Parent)
	return;

    if (W->Up == MAXDAT) {
	X[1] = (X[0] = Screen->XLogic) + All->DisplayWidth - 1;
	Y[1] = (Y[0] = Screen->YLogic + 1) + All->DisplayHeight - Screen->YLimit - 2;
    
	XWidth = W->XWidth;
	YWidth = IS_WINDOW(W) && W->Attrib & WINDOW_ROLLED_UP ? 1 : W->YWidth;
    
	if (!doSmartPlace(Screen->FirstW, X, Y)) {
	    /* can't be smart... be random */
	    if (XWidth <= X[1] - X[0])
		X[0] += lrand48() / (MAXLRAND48 / (X[1] - X[0] + 2 - XWidth));
	    if (YWidth <= Y[1] - Y[0])
		Y[0] += lrand48() / (MAXLRAND48 / (Y[1] - Y[0] + 2 - YWidth));
	}
	if (XWidth > X[1] - X[0] + 1 && X[0] > MINDAT)
	    X[0]--;
	if (YWidth > Y[1] - Y[0] + 1 && Y[0] > MINDAT)
	    Y[0]--;
    
	W->Left = X[0] - Screen->XLogic;
	W->Up = Y[0] - Screen->YLogic;
    }
    Act(MapTopReal,W)(W, Screen);
}


#ifdef THIS_MODULE
static void OverrideMethods(byte enter) {
    if (enter)
	OverrideMethod(Window,FindBorder, FakeFindBorderWindow, WMFindBorderWindow);
    else
	OverrideMethod(Window,FindBorder, WMFindBorderWindow, FakeFindBorderWindow);
}


byte InitModule(module Module)
#else
byte InitWM(void)
#endif
{
    byte sent = FALSE;
    
    srand48(time(NULL));
    if ((WM_MsgPort=Do(Create,MsgPort)(FnMsgPort, 2, "WM", 0, 0, 0, WManagerH)) &&
	/* this will later be sent to rcrun.c, it forces loading .twinrc */
	SendControlMsg(WM_MsgPort, MSG_CONTROL_OPEN, 0, NULL)) {
	
	if (RegisterExt(WM,MsgPort,WM_MsgPort)) {
	    
	    if ((MapQueue=Do(Create,MsgPort)
		 (FnMsgPort, 11, "WM MapQueue", 0, 0, 0, (void (*)(msgport))NoOp))) {
		
		Remove(MapQueue);
		
		if (InitRC()) {

#ifdef THIS_MODULE
		    OverrideMethods(TRUE);
#endif
		    return TRUE;
		} else {
		    sent = TRUE;
		    printk("twin: RC: %."STR(SMALLBUFF)"s\n", ErrStr);
		}
	    }
	    UnRegisterExt(WM,MsgPort,WM_MsgPort);
	} else {
	    sent = TRUE;
	    printk("twin: WM: RegisterExt(WM,MsgPort) failed! Another WM is running?\n");
	}
    }
    if (WM_MsgPort)
	Delete(WM_MsgPort);
    if (!sent) {
	printk("twin: WM: %."STR(SMALLBUFF)"s\n", ErrStr);
    }
    return FALSE;
}

#ifdef THIS_MODULE
void QuitModule(module Module) {
    QuitRC();
    OverrideMethods(FALSE);
    UnRegisterExt(WM,MsgPort,WM_MsgPort);
    Delete(WM_MsgPort);
    Delete(MapQueue);
}
#endif /* THIS_MODULE */
