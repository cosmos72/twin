/*
 *  resize.c  --  code to drag and resize screens and windows
 *
 *  Copyright (C) 1993-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "twin.h"
#include "data.h"
#include "methods.h"

#include "draw.h"
#include "resize.h"
#include "hw.h"

#define INIT do { \
    ScreenWidth = All->FirstScreen->ScreenWidth;	\
    ScreenHeight = All->FirstScreen->ScreenHeight;	\
    YLimit=Screen->YLimit;				\
    NWinDiMenu=(Window->Attrib & WINDOW_MENU) ? 0 : (ldat)~(ldat)0; \
    shUp=(ldat)Window->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)YLimit; \
    shLeft=(ldat)Window->Left-((ldat)Screen->Left & NWinDiMenu); \
    shRgt=shLeft+(ldat)Window->XWidth-(ldat)1;		\
    shDwn=shUp+(ldat)Window->YWidth-(ldat)1;		\
    } while (0)

/***************/

void UpdateCursor(void) {
    uldat type = NOCURSOR;
    screen *Screen;
    window *Window;
    ldat NWinDiMenu;
    dat ScreenWidth, ScreenHeight;
    udat YLimit, XWidth, YWidth;
    ldat Left, Up;
    uldat CurX, CurY, XLogic, YLogic;
    ldat XCursor, YCursor;
    
    if ((Screen = All->FirstScreen) && (Window = Screen->FocusWindow)
	&& ((All->SetUp->Flags & SETUP_ALWAYSCURSOR) || (Window->Flags & WINFL_CURSOR_ON))) {

	ScreenWidth=Screen->ScreenWidth;
	ScreenHeight=Screen->ScreenHeight;
	YLimit=Screen->YLimit;
	NWinDiMenu=(Window->Attrib & WINDOW_MENU) ? 0 : (ldat)~(ldat)0;
	Left=(ldat)Window->Left-((ldat)Screen->Left & NWinDiMenu);
	Up=(ldat)Window->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)YLimit;
	XLogic=Window->XLogic;
	YLogic=Window->YLogic;
	CurX=Window->CurX;
	CurY=Window->CurY;
	XWidth=Window->XWidth;
	YWidth=Window->YWidth;
	XCursor=Left+((ldat)CurX-XLogic)+(ldat)1;
	YCursor=Up+((ldat)CurY-YLogic)+(ldat)1;
	
	if (CurX>=XLogic && CurY>=YLogic &&
	    XWidth>(udat)2 && YWidth>(udat)2 &&
	    CurX<XLogic+XWidth-2 &&
	    CurY<YLogic+YWidth-2 &&
	    XCursor>=(ldat)0 && XCursor<(ldat)ScreenWidth &&
	    YCursor>=(ldat)YLimit && YCursor<(ldat)ScreenHeight &&
	    (Window == Screen->FirstWindow ||
	     Window == Act(SearchWindow,Screen)(Screen, (dat)XCursor, (dat)YCursor, NULL))) {
		
	    type = Window->CursorType;
	    if ((All->SetUp->Flags & SETUP_ALWAYSCURSOR) && type == NOCURSOR)
		type = LINECURSOR;
	    
	    MoveToXY((udat)XCursor, (udat)YCursor);
	}
    }
	
    SetCursorType(type);    
}

static int SendResizeSignal(window *Window) {
    struct winsize wsiz;
    
    if (!Window->TtyData)
	return -1;
	
    wsiz.ws_col = Window->TtyData->SizeX;
    wsiz.ws_row = Window->TtyData->SizeY;
    wsiz.ws_xpixel = 0;
    wsiz.ws_ypixel = 0;
    
    if (Window->RemoteData.FdSlot != NOSLOT) {
	if (ioctl(Window->RemoteData.Fd, TIOCSWINSZ, &wsiz) == 0)
	    return kill(-Window->RemoteData.ChildPid, SIGWINCH);
	return -1;
    }
    return 0;
}

byte CheckResizeWindowContents(window *Window) {
    if ((Window->Flags & WINFL_USECONTENTS) &&
	(Window->TtyData->SizeY != Window->YWidth - 2 ||
	 Window->TtyData->SizeX != Window->XWidth - 2)) {
	return ResizeWindowContents(Window);
    }
    return TRUE;
}

byte ResizeWindowContents(window *Window) {
    hwattr *NewCont, *saveNewCont, *OldCont, *max;
    uldat count, common, left;
    ttydata *Data = Window->TtyData;
    udat x = Window->XWidth - 2, y = Window->YWidth - 2 + Data->ScrollBack;
    
    if (!(saveNewCont = NewCont = (udat *)AllocMem(x*y*sizeof(hwattr))))
	return FALSE;
    
    /*
     * copy the Contents. Quite non-trivial for two reasons:
     * 1. always preserve the cursor line
     * 2. copy as much scrollback as possible
     */
    if ((max = OldCont = Window->Contents)) {
	max += Window->NumRowOne * Window->MaxNumRow;
	common = Min2(Window->NumRowOne, x);
    	count = Min2(Window->MaxNumRow, y);
	OldCont += Window->NumRowSplit * Window->NumRowOne;
	
	/* ok, now check for the cursor line */
	if (Window->CurY >= y) {
	    OldCont += (Window->CurY + 1 - y) * Window->NumRowOne;
	    if (OldCont > max)
		OldCont -= Window->NumRowOne * Window->MaxNumRow;
	}
	
	while (count--) {
	    CopyMem(OldCont, NewCont, common*sizeof(hwattr));
	    
	    if ((OldCont += Window->NumRowOne) >= max) 
		OldCont = Window->Contents;
	    NewCont += common;
	    for (left = x - common; left; left--)
		*NewCont++ = HWATTR(Window->ColText, ' ');
	}
	FreeMem(Window->Contents);
    }
    
    left = (saveNewCont + x*y) - NewCont;
    while (left--)
	*NewCont++ = HWATTR(Window->ColText, ' ');

    Window->XLogic = 0;
    Window->YLogic = Data->ScrollBack;
    Window->NumRowOne = x;	/* Contents width */
    Window->MaxNumRow = y;	/* Y visible + scrollback */
    Window->NumRowSplit = 0;	/* splitline == 0 */
    Window->Contents = saveNewCont;

    if (Window->CurY >= y)
	Window->CurY = y - 1;
    else if (Window->CurY < Window->YLogic)
	Window->CurY = Window->YLogic;

    Data->SizeX = x;
    Data->SizeY = Window->YWidth - 2;
    Data->Top = 0;
    Data->Bottom = Data->SizeY;
    
    Data->Start = Window->Contents + x * Data->ScrollBack;
    Data->Split = Window->Contents + x * y;
    Data->saveX = Data->X = Window->CurX;
    Data->saveY = Data->Y = Window->CurY - Data->ScrollBack;    
    Data->Pos = Window->Contents + Window->CurY * x + Window->CurX;
    
    (void)SendResizeSignal(Window);
    
    if (Window->Screen) {
	DrawBorderWindow(Window, BORDER_RIGHT);
    
	DrawTextWindow(Window, 0, 0, Window->NumRowOne-1, Window->MaxNumRow-1);
    
	if (Window == All->FirstScreen->FocusWindow)
	    UpdateCursor();
    }
    
    return TRUE;
}

static row *InsertRowsWindow(window *Window, uldat NumRows) {
    row *CurrRow;
    
    while (NumRows--) {
	if ((CurrRow=Do(Create,Row)(FnRow, 0, ROW_ACTIVE))) {
	    InsertLast(Row, CurrRow, Window);
	} else
	    break;
    }
    DrawBorderWindow(Window, BORDER_RIGHT);
    return CurrRow;
}

byte WriteRow(window *Window, uldat Len, byte *Text) {
    row *CurrRow;
    uldat x, y, max, RowLen, NewLen;
    byte *tempText;
    hwcol *tempColText;
    byte FlagNewRows, ModeInsert;
    
    x=Window->CurX;
    y=Window->CurY;
    max=Window->MaxNumRow;
    CurrRow=Window->LastRow;
    ModeInsert=Window->Flags & WINFL_INSERT;
    
    if (Window->Attrib & WINDOW_ANYSEL)
	ClearHilight(Window);
    
    while (Len) {
	if (max<=y || (max==y+1 && (*Text==(byte)13 || *Text==(byte)10))) {
	    if (InsertRowsWindow(Window, Max2(y+1-max,1))) {
		FlagNewRows=TRUE;
		max=Window->MaxNumRow;
		CurrRow=Window->LastRow;
	    } else
		return FALSE;
	} else {
	    FlagNewRows=FALSE;
	    CurrRow=Act(SearchRow,Window)(Window, y);
	}
	
	RowLen=(uldat)0;
	tempText=Text;
	while (RowLen < Len && *tempText!=(byte)13 && *tempText!=(byte)10)
	    ++RowLen, ++tempText;
	
	/*	WINFL_INSERT non implementato */
	/*  Gap non implementato				 */
	
	if (RowLen) {
	    if (ModeInsert || (CurrRow && CurrRow->LenGap))
		return FALSE;
	
	    if (CurrRow->MaxLen<x+RowLen) {
		NewLen=((x+RowLen) + ((x+RowLen) >> 1)) | All->SetUp->BloccoMemMin;
		if ((tempText=ReAllocMem(CurrRow->Text, NewLen))) {
		    if (!(Window->Flags & WINFL_USE_DEFCOL)) {
			if ((tempColText=(hwcol *)ReAllocMem(CurrRow->ColText, sizeof(hwcol)*NewLen)))
			    CurrRow->ColText=tempColText;
			else
			    return FALSE;
		    }
		    CurrRow->Text=tempText;
		    CurrRow->MaxLen=NewLen;
		    
		    if (Window->NumRowOne==y)
			Window->RowOne=CurrRow;
		    if (Window->NumRowSplit==y)
			Window->RowSplit=CurrRow;
		} else
		    return FALSE;
	    }
	    CurrRow->Flags=ROW_ACTIVE;
	
	    CopyMem(Text, CurrRow->Text+x, RowLen);
	    if (CurrRow->Len<x)
		WriteMem(CurrRow->Text+CurrRow->Len, ' ', x-CurrRow->Len);
	
	    if (!(Window->Flags & WINFL_USE_DEFCOL)) {
		WriteMem(CurrRow->ColText+x, Window->ColText, sizeof(hwcol)*RowLen);
		if (CurrRow->Len<x)
		    WriteMem(CurrRow->ColText+CurrRow->Len, Window->ColText,
			     sizeof(hwcol)*(x-CurrRow->Len));
	    }
	    
	    if (CurrRow->Len<x+RowLen)
		CurrRow->Len=x+RowLen;

	    DrawTextWindow(Window, x, y, x+RowLen-(uldat)1, y);
	
	    Text+=RowLen;
	    Len -=RowLen;
	}
	
	if (Len && (*Text=='\r' || *Text=='\n')) {
	    Window->CurX=x=(uldat)0;
	    Window->CurY=++y;
	    Text++, Len--;
	} else
	    Window->CurX=x+=RowLen;
    }
    
    if (Window==All->FirstScreen->FocusWindow)
	UpdateCursor();
    
    return TRUE;
}

/***************/

void DragFirstScreen(ldat DeltaX, ldat DeltaY) {
    screen *Screen;
    dat ScreenHeight, ScreenWidth;
    udat YLimit, minY;
    ldat Left, Up, Rgt, Dwn;
    
    Screen=All->FirstScreen;
    YLimit=Screen->YLimit;
    minY = !!YLimit;
    ScreenWidth=Screen->ScreenWidth;
    ScreenHeight=Screen->ScreenHeight;
    
    DeltaX = -DeltaX;
    DeltaY = -DeltaY;
    
    if (DeltaY < 0 && (udat)-DeltaY + minY > Screen->Up)
	DeltaY = (dat)(minY - Screen->Up);
    else if (DeltaY > 0 && (udat)DeltaY + Screen->ScreenHeight - 1 > MAXUDAT - Screen->Up)
	DeltaY = (dat)(MAXUDAT - Screen->Up - Screen->ScreenHeight + 1);
    if (Screen->Left < 0 && -DeltaX > Screen->Left - MINDAT)
	DeltaX = -(Screen->Left - MINDAT);
    else if (Screen->Left > 0 && DeltaX + Screen->ScreenWidth - 1 > MAXDAT - Screen->Left)
	DeltaX = MAXDAT - Screen->Left - Screen->ScreenWidth + 1;
    
    if (!DeltaX && !DeltaY)
	return;
    
    Left=Max2(DeltaX, (ldat)0);
    Up=(ldat)YLimit+Max2(DeltaY, (ldat)0);
    Rgt=(ldat)ScreenWidth-(ldat)1+Min2(DeltaX, (ldat)0);
    Dwn=(ldat)ScreenHeight-(ldat)1+Min2(DeltaY, (ldat)0);
    Screen->Left+=DeltaX;
    Screen->Up+=DeltaY;
    
    if (Up<=Dwn && Left<=Rgt)
	DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)(Left-DeltaX), (dat)(Up-DeltaY));

    if (Abs(DeltaX) >= ScreenWidth || Abs(DeltaY) >= ScreenHeight - YLimit)
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, 0, (dat)YLimit, ScreenWidth-1, ScreenHeight-1, FALSE);
    else if (DeltaY<0) {
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)0, (dat)YLimit, ScreenWidth-1, (dat)(YLimit-DeltaY), FALSE);
	if (DeltaX<=0)
	    DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)0, (dat)(YLimit-DeltaY), -(dat)(DeltaX+1), ScreenHeight-1, FALSE);
	else if (DeltaX>0)
	    DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)(ScreenWidth-DeltaX), (dat)(YLimit-DeltaY), ScreenWidth-1, ScreenHeight-1, FALSE);
    }
    else {
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)0, (dat)(ScreenHeight-DeltaY), ScreenWidth-1, ScreenHeight-1, FALSE);
	if (DeltaX<=0)
	    DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)0, (dat)YLimit, -(dat)(DeltaX+1), (dat)(ScreenHeight-DeltaY), FALSE);
	else if (DeltaX>=0)
	    DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)(ScreenWidth-DeltaX), (dat)YLimit, ScreenWidth-1, (dat)(ScreenHeight-DeltaY), FALSE);
    }
    UpdateCursor();
}

void ResizeFirstScreen(dat DeltaY) {
    screen *Screen;
    dat ScreenHeight, ScreenWidth;
    udat YLimit;
    ldat Left, Up, Rgt, Dwn;
    
    Screen=All->FirstScreen;
    YLimit=Screen->YLimit;
    ScreenWidth=Screen->ScreenWidth;
    ScreenHeight=Screen->ScreenHeight;
    
    if ((dat)YLimit+DeltaY>ScreenHeight)
	DeltaY=ScreenHeight-(dat)YLimit;
    else if ((dat)YLimit+DeltaY<(dat)0)
	DeltaY=-(dat)YLimit;
    
    Left=(ldat)0;
    Up=Max2((ldat)YLimit-1, (ldat)0);
    Rgt=(ldat)ScreenWidth-(ldat)1;
    Dwn=(ldat)ScreenHeight-(ldat)1-Max2(DeltaY, (ldat)0);
    Screen->YLimit+=DeltaY;
    
    if (DeltaY<(dat)0) {
	if (Up<=Dwn)
	    DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)Left, (dat)Up+DeltaY);
	DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Left, (dat)Dwn+1+DeltaY, (dat)Rgt, (dat)Dwn, FALSE);
    }
    else if (DeltaY>(dat)0) {
	if (Up<=Dwn)
	    DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)Left, (dat)Up+(dat)DeltaY);
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Left, (dat)Up, (dat)Rgt, (dat)Up+DeltaY-1, FALSE);
    }
    UpdateCursor();
}

void CenterWindow(window *Window) {
    screen *Screen;
    ldat parz1, parz2;
    ldat DeltaX, DeltaY;
    dat ScreenWidth, ScreenHeight;
    udat YLimit;
    
    if (!Window || !(Screen=Window->Screen) ||
	(Screen!=All->FirstScreen) || (Window->Attrib & WINDOW_MENU))
	return;
    
    ScreenWidth=Screen->ScreenWidth;
    ScreenHeight=Screen->ScreenHeight;
    YLimit=Screen->YLimit;
    parz1=(ldat)Window->Left-(ldat)Screen->Left;
    parz2=parz1+(ldat)Window->XWidth;
    if (parz1>=(ldat)0 && parz2<=(ldat)ScreenWidth)
	DeltaX=(ldat)0;
    else
	DeltaX=-parz1;
    
    if (Window->XWidth<=(udat)ScreenWidth) {
	parz1=DeltaX;
	parz2=DeltaX+(ldat)ScreenWidth-(ldat)Window->XWidth;
	DeltaX=Abs(parz1)<Abs(parz2) ? parz1 : parz2;
    } else
	DeltaX--;
	
    
    parz1=(ldat)Window->Up-(ldat)Screen->Up+(ldat)YLimit;
    parz2=parz1+(ldat)Window->YWidth;
    if (parz1>=(ldat)YLimit && parz2<=(ldat)ScreenHeight)
	DeltaY=(ldat)0;
    else
	DeltaY=-parz1+YLimit;
    
    if ((ldat)Window->YWidth<=(ldat)ScreenHeight-(ldat)YLimit) {
	parz1=DeltaY;
	parz2=DeltaY+(ldat)ScreenHeight-(ldat)YLimit-(ldat)Window->YWidth;
	DeltaY=Abs(parz1)<Abs(parz2) ? parz1 : parz2;
    } else
	DeltaY--;
	
    DragFirstScreen(DeltaX, DeltaY);
}

void DragFirstWindow(dat i, dat j) {
    ldat Left, Up, Rgt, Dwn;
    ldat Xstart, Ystart, Xend, Yend;
    ldat shLeft, shUp, shRgt, shDwn;
    screen *Screen;
    setup *SetUp;
    window *Window;
    dat Delta;
    udat YLimit;
    dat ScreenWidth, ScreenHeight;
    ldat NWinDiMenu;
    byte Shade, DeltaXShade, DeltaYShade, isFocus;
    
    Screen=All->FirstScreen;
    Window=Screen->FirstWindow;
    
    if (!(Window->Attrib & WINDOW_DRAG))
	return;
    
    isFocus = Window == Screen->FocusWindow;
    
    ScreenWidth=Screen->ScreenWidth;
    ScreenHeight=Screen->ScreenHeight;
    NWinDiMenu=(Window->Attrib & WINDOW_MENU) ? 0 : (ldat)~(ldat)0;
    SetUp=All->SetUp;
    Shade=!!(SetUp->Flags & SETUP_DO_SHADE);
    DeltaXShade=Shade ? SetUp->DeltaXShade : (byte)0;
    DeltaYShade=Shade ? SetUp->DeltaYShade : (byte)0;
    YLimit=Screen->YLimit;
    
    Up=(ldat)Window->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)YLimit;
    Left=(ldat)Window->Left-((ldat)Screen->Left & NWinDiMenu);
    Rgt=Left+(ldat)Window->XWidth-(ldat)1;
    Dwn=Up+(ldat)Window->YWidth-(ldat)1;
    
    if ((udat)ABS(i)>=Window->XWidth+DeltaXShade || (udat)ABS(j)>=Window->YWidth+DeltaYShade) {
	if (i<(dat)0) {
	    if (Window->Left<MINDAT-i)
		i=MINDAT-Window->Left;
	}
	else if (i>(dat)0) {
	    if (Window->Left>MAXDAT-i)
		i=MAXDAT-Window->Left;
	}
	if (j<(dat)0) {
	    if (Window->Up<(udat)-j)
		j=(dat)Window->Up;
	}
	else if (j>(dat)0) {
	    if (Window->Up>MAXUDAT-(udat)j)
		j=(dat)(MAXUDAT-Window->Up);
	}
	Window->Left+=i;
	Window->Up+=j;
	DrawAreaWindow(Window, FALSE);
	
	if (Left<(ldat)ScreenWidth && Up<(ldat)ScreenHeight && Rgt>=(ldat)0 && Dwn>=(ldat)0) {
	    Left=Max2((ldat)0, Left);
	    Up=Max2((ldat)0, Up);
	    Rgt=Min2((ldat)ScreenWidth-(ldat)1, Rgt);
	    Dwn=Min2((ldat)ScreenHeight-(ldat)1, Dwn);
	    
	    DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Left, (dat)Up, (dat)Rgt+(dat)DeltaXShade, (dat)Dwn+(dat)DeltaYShade, FALSE);
	}
	if (isFocus) UpdateCursor();
	return;
    }
    
    if ((Delta=-i)>(dat)0 && Window->Left>MINDAT) {
	if (Window->Left<MINDAT+Delta)
	    Delta=Window->Left-MINDAT;
	Window->Left-=Delta;
	if (Left-(ldat)Delta<(ldat)ScreenWidth && Up<(ldat)ScreenHeight && Rgt+(ldat)DeltaXShade>=-(ldat)Delta && Dwn+(ldat)DeltaYShade>=(ldat)YLimit) {
	    Xstart=Max2(Left,(ldat)Delta);
	    Ystart=Max2(Up,(ldat)YLimit);
	    Xend=Min2(Rgt,(ldat)ScreenWidth-(ldat)1);
	    Yend=Min2(Dwn,(ldat)ScreenHeight-(ldat)1);
	    if (Xstart<=Xend && Xend >= 0 && Xstart < ScreenWidth &&
		Ystart<=Yend && Yend >= YLimit && Ystart < ScreenHeight)
		DragArea((dat)Xstart, (dat)Ystart, (dat)Xend, (dat)Yend, (dat)Xstart-Delta, (dat)Ystart);
	    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Min2(Xend+(ldat)1, (ldat)ScreenWidth)-Delta, (dat)Max2((ldat)YLimit, Ystart), (dat)Xend, (dat)Yend, FALSE);
	    if (Shade) {
		if ((shLeft=Rgt+(ldat)1+(ldat)Max2((dat)DeltaXShade-Delta, (dat)0))>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		if ((shUp=Max2((ldat)YLimit, Up+(ldat)DeltaYShade))>(ldat)MAXDAT) shUp=MAXDAT;
		if ((shRgt=Rgt+(ldat)DeltaXShade)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		if ((shDwn=Dwn+(ldat)DeltaYShade)>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		
		if ((shLeft=Left+(ldat)DeltaXShade-(ldat)Delta)>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		if ((shUp=Max2((ldat)YLimit, Dwn+(ldat)1))>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		if ((shRgt=Left+(ldat)DeltaXShade-(ldat)1)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		if ((shDwn=Dwn+(dat)DeltaYShade)>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		
		if (Delta>(dat)DeltaXShade) {
		    if ((shLeft=Rgt+(dat)DeltaXShade-Delta+1)>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		    if ((shUp=Max2((ldat)YLimit, Dwn+(ldat)1))>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		    if ((shRgt=Rgt)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		    if ((shDwn=Dwn+(dat)DeltaYShade)>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		}
	    }
	}
	Left-=Delta;
	Rgt-=Delta;
    }
    else if ((Delta=i)>(dat)0 && Window->Left<MAXDAT) {
	if (Window->Left>MAXDAT-Delta)
	    Delta=MAXDAT-Window->Left;
	Window->Left+=Delta;
	if (Left<(ldat)ScreenWidth && Up<(ldat)ScreenHeight && Rgt+(ldat)DeltaXShade>=-(ldat)Delta && Dwn+(ldat)DeltaYShade>=(ldat)YLimit) {
	    Xstart=Max2(Left,(ldat)0);
	    Ystart=Max2(Up,(ldat)YLimit);
	    Xend=Min2(Rgt,(ldat)ScreenWidth-(ldat)1-(ldat)Delta);
	    Yend=Min2(Dwn,(ldat)ScreenHeight-(ldat)1);
	    if (Xstart<=Xend && Xend >= 0 && Xstart < ScreenWidth &&
		Ystart<=Yend && Yend >= YLimit && Ystart < ScreenHeight)
		DragArea((dat)Xstart, (dat)Ystart, (dat)Xend, (dat)Yend, (dat)Xstart+(dat)Delta, (dat)Ystart);
	    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Xstart, (dat)Max2((ldat)YLimit, Ystart), (dat)Xstart-1+Delta, (dat)Yend, FALSE);
	    if (Shade) {
		if ((shLeft=Rgt+1+Max2(Delta, (dat)DeltaXShade))>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		if ((shUp=Max2((ldat)YLimit, Up+(ldat)DeltaYShade))>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		if ((shRgt=Rgt+(ldat)DeltaXShade+(ldat)Delta)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		if ((shDwn=Max2((ldat)YLimit, Dwn+(ldat)DeltaYShade))>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		
		if ((shLeft=Left+(ldat)DeltaXShade)>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		if ((shUp=Max2((ldat)YLimit, Dwn+(ldat)1))>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		if ((shRgt=Left+(ldat)DeltaXShade+(ldat)Delta-(ldat)1)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		if ((shDwn=Dwn+(ldat)DeltaYShade)>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		
		if (Delta>(dat)DeltaXShade) {
		    if ((shLeft=Max2(Rgt+(ldat)1, Left+(ldat)Delta)+(ldat)DeltaXShade)>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		    if ((shUp=Max2((ldat)YLimit, Dwn+(ldat)1))>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		    if ((shRgt=Rgt+(ldat)Delta)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		    if ((shDwn=Dwn+(ldat)DeltaYShade)>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		}
	    }
	}
	Left+=Delta;
	Rgt+=Delta;
    }
    if ((Delta=-j)>(dat)0 && Window->Up>(udat)0) {
	if (Window->Up<(udat)Delta)
	    Delta=(dat)Window->Up;
	Window->Up-=Delta;
	if (Left<(ldat)ScreenWidth && Up<(ldat)ScreenHeight+(ldat)Delta && Rgt+(ldat)DeltaXShade>=-(ldat)Delta && Dwn+(ldat)DeltaYShade>=(ldat)YLimit) {
	    Xstart=Max2(Left,(ldat)0);
	    Ystart=Max2(Up,(ldat)YLimit+(ldat)Delta);
	    Xend=Min2(Rgt,(ldat)ScreenWidth-(ldat)1);
	    Yend=Min2(Dwn,(ldat)ScreenHeight-(ldat)1);
	    if (Xstart<=Xend && Xend >= 0 && Xstart < ScreenWidth &&
		Ystart<=Yend && Yend >= YLimit && Ystart < ScreenHeight)
		DragArea((dat)Xstart, (dat)Ystart, (dat)Xend, (dat)Yend, (dat)Xstart, (dat)Ystart-(dat)Delta);
	    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Xstart, (dat)Yend+1-Delta, (dat)Xend, (dat)Yend, FALSE);
	    if (Shade) {
		if ((shLeft=Left+(ldat)DeltaXShade)>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		if ((shUp=Dwn+(ldat)1+(ldat)Max2((dat)DeltaYShade-Delta, (dat)0))>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		if ((shRgt=Rgt+(ldat)DeltaXShade)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		if ((shDwn=Dwn+(ldat)DeltaYShade)>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		
		if ((shLeft=Rgt+(ldat)1)>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		if ((shUp=Max2((ldat)YLimit, Up+(ldat)DeltaYShade-(ldat)Delta))>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		if ((shRgt=Rgt+(ldat)DeltaXShade)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		if ((shDwn=Up+(ldat)DeltaYShade-(ldat)1)>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		
		if (Delta>(dat)DeltaYShade) {
		    if ((shLeft=Rgt+1)>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		    if ((shUp=Max2((ldat)YLimit, Dwn+(ldat)DeltaYShade-(ldat)Delta+(ldat)1))>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		    if ((shRgt=(dat)Rgt+(dat)DeltaXShade)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		    if ((shDwn=Max2((ldat)YLimit, Dwn))>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		}
	    }
	}
	Up-=Delta;
	Dwn-=Delta;
    }
    else if ((Delta=j)>(dat)0 && Window->Up<MAXUDAT) {
	if (Window->Up>MAXUDAT-(udat)Delta)
	    Delta=(dat)(MAXUDAT-Window->Up);
	Window->Up+=Delta;
	if (Left<(ldat)ScreenWidth && Up<(ldat)ScreenHeight && Rgt+(ldat)DeltaXShade>=(ldat)0 && Dwn+(ldat)DeltaYShade>=-(ldat)Delta+(ldat)YLimit) {
	    Xstart=Max2(Left, (ldat)0);
	    Ystart=Max2(Up, (ldat)YLimit);
	    Xend=Min2(Rgt, (ldat)ScreenWidth-(ldat)1);
	    Yend=Min2(Dwn, (ldat)ScreenHeight-(ldat)1-(ldat)Delta);
	    if (Xstart<=Xend && Xend >= 0 && Xstart < ScreenWidth &&
		Ystart<=Yend && Yend >= YLimit && Ystart < ScreenHeight)
		DragArea((dat)Xstart, (dat)Ystart, (dat)Xend, (dat)Yend, (dat)Xstart, (dat)Ystart+(dat)Delta);
	    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Xstart, (dat)Ystart, (dat)Xend, (dat)Ystart-1+Delta, FALSE);
	    if (Shade) {
		if ((shLeft=Left+(ldat)DeltaXShade)>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		if ((shUp=Dwn+(ldat)1+(ldat)Max2(Delta, (dat)DeltaYShade))>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		if ((shRgt=Rgt+(ldat)DeltaXShade)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		if ((shDwn=Dwn+(ldat)DeltaYShade+(ldat)Delta)>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		
		if ((shLeft=Rgt+(ldat)1)>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		if ((shUp=Max2((ldat)YLimit, Up+(ldat)DeltaYShade))>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		if ((shRgt=Rgt+(ldat)DeltaXShade)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		if ((shDwn=Max2((ldat)YLimit, Up+(ldat)Delta+(ldat)DeltaYShade-(ldat)1))>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		
		if (Delta>(dat)DeltaYShade) {
		    if ((shLeft=Rgt+(ldat)1)>(ldat)MAXDAT) shLeft=(ldat)MAXDAT;
		    if ((shUp=Max2((ldat)Dwn+(ldat)1, (ldat)Up+(ldat)Delta)+(ldat)DeltaYShade)>(ldat)MAXDAT) shUp=(ldat)MAXDAT;
		    if ((shRgt=Rgt+(ldat)DeltaXShade)>(ldat)MAXDAT) shRgt=(ldat)MAXDAT;
		    if ((shDwn=Max2((ldat)YLimit, Dwn+Delta))>(ldat)MAXDAT) shDwn=(ldat)MAXDAT;
		    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, FALSE);
		}
	    }
	}
	Up+=Delta;
	Dwn+=Delta;
    }
    if (isFocus)
	UpdateCursor();
}

void DragWindow(window *Window, dat i, dat j) {
    ldat Left, Up, Rgt, Dwn;
    ldat Left1, Rgt1;
    screen *Screen;
    setup *SetUp;
    udat YLimit;
    dat ScreenWidth, ScreenHeight;
    ldat NWinDiMenu;
    byte Shade, DeltaXShade, DeltaYShade;

    if (!Window || !(Window->Attrib & WINDOW_DRAG))
	return;

    if (Window == All->FirstScreen->FirstWindow) {
	DragFirstWindow(i, j);
	return;
    }
	
    Screen=Window->Screen;

    ScreenWidth=Screen->ScreenWidth;
    ScreenHeight=Screen->ScreenHeight;
    NWinDiMenu=(Window->Attrib & WINDOW_MENU) ? 0 : (ldat)~(ldat)0;
    SetUp=All->SetUp;
    Shade=!!(SetUp->Flags & SETUP_DO_SHADE);
    DeltaXShade=Shade ? SetUp->DeltaXShade : (byte)0;
    DeltaYShade=Shade ? SetUp->DeltaYShade : (byte)0;
    YLimit=Screen->YLimit;

    Up=(ldat)Window->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)YLimit;
    Left=(ldat)Window->Left-((ldat)Screen->Left & NWinDiMenu);
    Rgt=Left+(ldat)Window->XWidth-(ldat)1;
    Dwn=Up+(ldat)Window->YWidth-(ldat)1;

    if (i<(dat)0) {
	if (Window->Left<MINDAT-i)
	    i=MINDAT-Window->Left;
    }
    else if (i>(dat)0) {
	if (Window->Left>MAXDAT-i)
	    i=MAXDAT-Window->Left;
    }
    if (j<(dat)0) {
	if (Window->Up<(udat)-j)
	    j=(dat)Window->Up;
    }
    else if (j>(dat)0) {
	if (Window->Up>MAXUDAT-(udat)j)
	    j=(dat)(MAXUDAT-Window->Up);
    }
    Window->Left+=i;
    Window->Up+=j;
    DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0,
	     Left+i, Up+j, Rgt+i+DeltaXShade, Dwn+j+DeltaYShade, FALSE);
    
    if (Left<(ldat)ScreenWidth && Up<(ldat)ScreenHeight && Rgt+DeltaXShade>=(ldat)0 && Dwn+DeltaYShade>=(ldat)YLimit) {
	if (i > 0) {
	    Rgt1=Min2(Rgt+DeltaXShade, Left+i-1);
	    Left1=Max2(0, Left);
	} else {
	    Left1=Max2(Rgt+DeltaXShade+i, Left);
	    Rgt1=Min2((ldat)ScreenWidth-(ldat)1, Rgt+DeltaXShade);
	}
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Left1, (dat)Up, (dat)Rgt1, (dat)Dwn+DeltaYShade, FALSE);
	
	if (j > 0) {
	    Dwn=Min2(Dwn+DeltaYShade, Up+j-1);
	    Up=Max2(YLimit, Up);
	} else {
	    Up=Max2(Dwn+DeltaYShade+j, Up);
	    Dwn=Min2((ldat)ScreenHeight-(ldat)1, Dwn+DeltaYShade);
	}
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Left, (dat)Up, (dat)Rgt+DeltaXShade, (dat)Dwn, FALSE);
    }
    if (Window == All->FirstScreen->FocusWindow)
	UpdateCursor();
}

void ResizeRelFirstWindow(dat i, dat j) {
    ldat Left, Up, Rgt, Dwn;
    screen *Screen;
    setup *SetUp;
    window *Window;
    dat DeltaX, DeltaY;
    udat YLimit, XWidth, YWidth;
    udat MinXWidth, MinYWidth, MaxXWidth, MaxYWidth;
    dat ScreenWidth, ScreenHeight;
    ldat NWinDiMenu;
    byte Shade, DeltaXShade, DeltaYShade;
    
    Screen=All->FirstScreen;
    Window=Screen->FirstWindow;
    
    if (!(Window->Attrib & WINDOW_RESIZE))
	return;
    
    ScreenWidth=Screen->ScreenWidth;
    ScreenHeight=Screen->ScreenHeight;
    NWinDiMenu=(Window->Attrib & WINDOW_MENU) ? 0 : (ldat)~(ldat)0;
    SetUp=All->SetUp;
    Shade=!!(SetUp->Flags & SETUP_DO_SHADE);
    DeltaXShade=Shade ? SetUp->DeltaXShade : (byte)0;
    DeltaYShade=Shade ? SetUp->DeltaYShade : (byte)0;
    YLimit=Screen->YLimit;
    
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;
    MinXWidth=Window->MinXWidth;
    MinYWidth=Window->MinYWidth;
    MaxXWidth=Window->MaxXWidth;
    MaxYWidth=Window->MaxYWidth;
    
    Up=(ldat)Window->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)YLimit;
    Left=(ldat)Window->Left-((ldat)Screen->Left & NWinDiMenu);
    Rgt=Left+(ldat)XWidth-(ldat)1;
    Dwn=Up+(ldat)YWidth-(ldat)1;
    
    if ((DeltaX=-i)>(dat)0 && XWidth>MinXWidth) {
	if (MinXWidth+DeltaX>XWidth)
	    DeltaX=XWidth-MinXWidth;
	XWidth=Window->XWidth-=DeltaX;
	if (Left<(ldat)ScreenWidth && Up<(ldat)ScreenHeight && Rgt+(ldat)DeltaXShade>=(ldat)0 && Dwn+(ldat)DeltaYShade>=(ldat)YLimit) {
	    DrawArea((screen *)0, Window, (window *)0, (gadget *)0, (gadget *)0, (dat)Rgt-DeltaX+1, (dat)Max2(Up, (ldat)YLimit), (dat)Rgt, (dat)Max2((ldat)YLimit, Dwn), FALSE);
	    if (Shade) {
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Rgt+Max2((dat)DeltaXShade-DeltaX-1, 1), (dat)Max2((ldat)YLimit, Up), (dat)Rgt+(dat)DeltaXShade, (dat)Dwn+(dat)DeltaYShade, FALSE);
		if (DeltaX>(dat)DeltaXShade)
		    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Rgt+(dat)DeltaXShade-DeltaX+1, (dat)Max2((ldat)YLimit, Dwn+1), (dat)Rgt, (dat)Dwn+(dat)DeltaYShade, FALSE);
	    }
	}
	Rgt-=DeltaX;
    }
    else if ((DeltaX=i)>(dat)0 && XWidth<MaxXWidth) {
	if (XWidth>MaxXWidth-DeltaX)
	    DeltaX=MaxXWidth-XWidth;
	XWidth=Window->XWidth+=DeltaX;
	if (Left<(ldat)ScreenWidth && Up<(ldat)ScreenHeight && Rgt+(ldat)DeltaXShade>=-(ldat)DeltaX && Dwn+(ldat)DeltaYShade>=(ldat)YLimit) {
	    DrawArea((screen *)0, Window, (window *)0, (gadget *)0, (gadget *)0, (dat)Rgt, (dat)Max2(Up+(ldat)1, (ldat)YLimit), (dat)Rgt+DeltaX-1, (dat)Max2((ldat)YLimit, Dwn-1), FALSE);
	    if (Shade) {
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Rgt+1+Max2((dat)DeltaXShade, DeltaX), (dat)Max2((ldat)YLimit, Up+(dat)DeltaYShade), (dat)Rgt+DeltaX+(dat)DeltaXShade, (dat)Dwn+(dat)DeltaYShade, FALSE);
		if (DeltaX>(dat)DeltaXShade)
		    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Rgt+(dat)DeltaXShade+1, (dat)Max2((ldat)YLimit, Dwn+1), (dat)Rgt+DeltaX, (dat)Dwn+(dat)DeltaYShade, FALSE);
	    }
	}
	Rgt+=DeltaX;
    }
    if ((DeltaY=-j)>(dat)0 && YWidth>MinYWidth) {
	if (MinYWidth+DeltaY>YWidth)
	    DeltaY=YWidth-MinYWidth;
	YWidth=Window->YWidth-=DeltaY;
	if (Left<(ldat)ScreenWidth && Up<(ldat)ScreenHeight && Rgt+(ldat)DeltaXShade>=(ldat)0 && Dwn+(ldat)DeltaYShade>=(ldat)YLimit) {
	    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Left, (dat)Max2(Dwn-(ldat)DeltaY+(ldat)1, (ldat)YLimit), (dat)Rgt, (dat)Max2((ldat)YLimit, Dwn), FALSE);
	    if (Shade) {
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Left, (dat)Max2((ldat)YLimit, Dwn+1+Max2((dat)DeltaYShade-DeltaY, (dat)0)), (dat)Rgt+(dat)DeltaXShade, (dat)Dwn+(dat)DeltaYShade, FALSE);
		if (DeltaY>(dat)DeltaYShade)
		    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Rgt+1, (dat)Max2((ldat)YLimit, Dwn+1+(dat)DeltaYShade-DeltaY), (dat)Rgt+(dat)DeltaXShade, (dat)Max2((ldat)YLimit, Dwn), FALSE);
	    }
	}
	Dwn-=DeltaY;
    }
    else if ((DeltaY=j)>(dat)0 && YWidth<MaxYWidth) {
	if (YWidth>MaxYWidth-DeltaY)
	    DeltaY=MaxYWidth-YWidth;
	YWidth=Window->YWidth+=DeltaY;
	if (Left<(ldat)ScreenWidth && Up<(ldat)ScreenHeight && Rgt+(ldat)DeltaXShade>=(ldat)0 && Dwn+(ldat)DeltaYShade>=-(ldat)DeltaY+(ldat)YLimit) {
	    DrawArea((screen *)0, Window, (window *)0, (gadget *)0, (gadget *)0, (dat)Left, (dat)Max2((ldat)YLimit, Dwn), (dat)Rgt-1, (dat)Max2((ldat)YLimit, Dwn+DeltaY-1), FALSE);
	    if (Shade) {
		DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Left+(dat)DeltaXShade, (dat)Dwn+1+Max2(DeltaY, (dat)DeltaYShade), (dat)Rgt+(dat)DeltaXShade, (dat)Dwn+(dat)DeltaYShade+DeltaY, FALSE);
		if (DeltaY>(dat)DeltaYShade)
		    DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Rgt+1, (dat)Dwn+(dat)DeltaYShade+1, (dat)Rgt+(dat)DeltaXShade, (dat)Max2((ldat)YLimit, Dwn+DeltaY), FALSE);
	    }
	}
	Dwn+=DeltaY;
    }
    if (DeltaX || DeltaY) {
	DrawBorderWindow(Window, BORDER_ANY);
	if (Window == Screen->FocusWindow)
	    UpdateCursor();
    }
}


void ResizeRelWindow(window *Window, dat i, dat j) {
    ldat Left, Up, Rgt, Dwn;
    screen *Screen;
    setup *SetUp;
    dat DeltaX, DeltaY;
    udat YLimit, XWidth, YWidth;
    udat MinXWidth, MinYWidth, MaxXWidth, MaxYWidth;
    dat ScreenWidth, ScreenHeight;
    ldat NWinDiMenu;
    byte Shade, DeltaXShade, DeltaYShade;
    
    if (!Window || !(Window->Attrib & WINDOW_RESIZE) || (!i && !j))
	return;

    if (Window == All->FirstScreen->FirstWindow) {
	ResizeRelFirstWindow(i, j);
	return;
    }
    
    if ((Screen=Window->Screen)) {
	ScreenWidth=Screen->ScreenWidth;
	ScreenHeight=Screen->ScreenHeight;
	NWinDiMenu=(Window->Attrib & WINDOW_MENU) ? 0 : (ldat)~(ldat)0;
	SetUp=All->SetUp;
	Shade=!!(SetUp->Flags & SETUP_DO_SHADE);
	DeltaXShade=Shade ? SetUp->DeltaXShade : (byte)0;
	DeltaYShade=Shade ? SetUp->DeltaYShade : (byte)0;
	YLimit=Screen->YLimit;
	
	XWidth=Window->XWidth;
	YWidth=Window->YWidth;
	MinXWidth=Window->MinXWidth;
	MinYWidth=Window->MinYWidth;
	MaxXWidth=Window->MaxXWidth;
	MaxYWidth=Window->MaxYWidth;
	
	Up=(ldat)Window->Up-((ldat)Screen->Up & NWinDiMenu)+(ldat)YLimit;
	Left=(ldat)Window->Left-((ldat)Screen->Left & NWinDiMenu);
	Rgt=Left+(ldat)XWidth-(ldat)1 + DeltaXShade;
	Dwn=Up+(ldat)YWidth-(ldat)1 + DeltaYShade;
    }
    
    if ((DeltaX=-i)>(dat)0 && XWidth>MinXWidth) {
	if (MinXWidth+DeltaX>XWidth)
	    DeltaX=XWidth-MinXWidth;
	XWidth=Window->XWidth-=DeltaX;
    } else if ((DeltaX=i)>(dat)0 && XWidth<MaxXWidth) {
	if (XWidth>MaxXWidth-DeltaX)
	    DeltaX=MaxXWidth-XWidth;
	XWidth=Window->XWidth+=DeltaX;
	Rgt+=DeltaX;
    }
    if ((DeltaY=-j)>(dat)0 && YWidth>MinYWidth) {
	if (MinYWidth+DeltaY>YWidth)
	    DeltaY=YWidth-MinYWidth;
	YWidth=Window->YWidth-=DeltaY;
    }
    else if ((DeltaY=j)>(dat)0 && YWidth<MaxYWidth) {
	if (YWidth>MaxYWidth-DeltaY)
	    DeltaY=MaxYWidth-YWidth;
	YWidth=Window->YWidth+=DeltaY;
	Dwn+=DeltaY;
    }
    if (Screen) {
	Up = (dat)Max2(Up, (ldat)YLimit);
	DrawArea(Screen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, FALSE);
	if (Shade)
	    DrawShadeWindow(Window, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
    }
    
    if (Window == All->FirstScreen->FocusWindow)
	UpdateCursor();
}
	
/*
 * coordinates are special here:
 * (0,0) is the (XLogic,YLogic) cell of the Window,
 * i.e. the first visible one.
 */
void ScrollFirstWindowArea(dat X1, dat Y1, dat X2, dat Y2, dat DeltaX, dat DeltaY) {
    screen *Screen;
    dat ScreenWidth, ScreenHeight;
    window *Window;
    ldat shLeft, shUp, shRgt, shDwn;
    dat Left, Up, Rgt, Dwn;
    dat Xstart, Ystart, Xend, Yend;
    udat XWidth, YWidth, YLimit;
    ldat NWinDiMenu;
    
    Screen=All->FirstScreen;
    Window=Screen->FirstWindow;
    
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;

    if (X1 > X2 || X1 > XWidth-3 || X2 < 0 || Y1 > Y2 || Y1 > YWidth-3 || Y2 < 0)
	return;
    
    INIT;
    
    X1=Max2(X1, 0);
    Y1=Max2(Y1, 0);
    X2=Min2(X2, XWidth-3);
    Y2=Min2(Y2, YWidth-3);
    
    Left=(dat)(shLeft+(ldat)1+(ldat)X1);
    if (Left < 0)
	X1 += -Left, Left = 0;
    else if (Left >= ScreenWidth)
	return;
    
    Up=(dat)(shUp+(ldat)1+(ldat)Y1);
    if (Up < (dat)YLimit)
	Y1 += (dat)YLimit - Up, Up = (dat)YLimit;
    else if (Up >= ScreenHeight)
	return;
    
    Rgt=(dat)(shLeft+(ldat)1+(ldat)X2);
    if (Rgt >= ScreenWidth)
	X2 -= Rgt - ScreenWidth, Rgt = ScreenWidth - 1;
    else if (Rgt < 0)
	return;

    Dwn=(dat)(shUp+(ldat)1+(ldat)Y2);
    if (Dwn >= ScreenHeight)
	Y2 -= Dwn - ScreenHeight, Dwn = ScreenHeight - 1;
    else if (Dwn < 0)
	return;
    
    Xstart=Left+Max2(-DeltaX, (dat)0);
    Ystart=Up+Max2(-DeltaY, (dat)0);
    Xend=Rgt-Max2(DeltaX, (dat)0);
    Yend=Dwn-Max2(DeltaY, (dat)0);
    
    if (DeltaX) {
	if (Xend>=Xstart)
	    DragArea(Xstart, Ystart, Xend, Yend, Xstart+DeltaX, Ystart);
	if (DeltaX>(dat)0)
	    DrawWindow(Window, (gadget *)0, (gadget *)0, Left, Up, Left+DeltaX-1, Dwn, FALSE);
	else
	    DrawWindow(Window, (gadget *)0, (gadget *)0, Rgt+DeltaX+1, Up, Rgt, Dwn, FALSE);
    }
    else if (DeltaY) {
	if (Yend>=Ystart)
	    DragArea(Xstart, Ystart, Xend, Yend, Xstart, Ystart+DeltaY);
	if (DeltaY>(dat)0)
	    DrawWindow(Window, (gadget *)0, (gadget *)0, Left, Up, Rgt, Up+DeltaY-1, FALSE);
	else
	    DrawWindow(Window, (gadget *)0, (gadget *)0, Left, Dwn+DeltaY+1, Rgt, Dwn, FALSE);
    }
}

void ScrollFirstWindow(dat DeltaX, dat DeltaY, byte byXYLogic) {
    window *Window;
    uldat XLogic, YLogic;
    udat XWidth, YWidth;
    
    if (!(Window=All->FirstScreen->FirstWindow))
	return;
    
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;
    
    if (byXYLogic) {
	XLogic=Window->XLogic;
	YLogic=Window->YLogic;
	
	if (DeltaX>(dat)0 && XLogic>=MAXULDAT-(uldat)DeltaX)
	    DeltaX=(dat)(MAXULDAT-XLogic-(uldat)1);
	else if (DeltaX<(dat)0 && XLogic<(uldat)-DeltaX)
	    DeltaX=-(dat)XLogic;
	if (Window->Flags & WINFL_USEANY) {
	    /*
	     * WARNING: Window->Contents and other methods
	     * may be unable to handle out-of-bound rows
	     */
	    if (DeltaY>(dat)0 && (uldat)YLogic+YWidth-2+DeltaY>=Window->MaxNumRow)
		DeltaY=(dat)(Window->MaxNumRow-YLogic-YWidth+2);
	    else if (DeltaY<(dat)0 && YLogic<(uldat)-DeltaY)
		DeltaY=-(dat)YLogic;
	} else {
	    if (DeltaY>(dat)0 && YLogic+DeltaY>=Window->MaxNumRow)
		DeltaY=(dat)(Window->MaxNumRow-YLogic-(uldat)1);
	    else if (DeltaY<(dat)0 && YLogic<(uldat)-DeltaY)
		DeltaY=-(dat)YLogic;
	}
	if (DeltaX)
	    Window->XLogic = (XLogic += DeltaX);
	if (DeltaY)
	    Window->YLogic = (YLogic += DeltaY);
	if (DeltaX || DeltaY)
	    DrawBorderWindow(Window, (DeltaX ? BORDER_DOWN : 0) | (DeltaY ? BORDER_RIGHT : 0));
    }

    ScrollFirstWindowArea(0, 0, XWidth-3, YWidth-3, -DeltaX, -DeltaY);
    
    if (byXYLogic && Window == All->FirstScreen->FocusWindow)
	UpdateCursor();
}

void ScrollWindow(window *Window, dat DeltaX, dat DeltaY) {
    uldat XLogic, YLogic;
    udat YWidth;
    
    if (!Window || (!DeltaX && !DeltaY))
	return;
    
    if (Window == All->FirstScreen->FirstWindow) {
	ScrollFirstWindow(DeltaX, DeltaY, TRUE);
	return;
    }
    
    YWidth=Window->YWidth;
    XLogic=Window->XLogic;
    YLogic=Window->YLogic;
	
    if (DeltaX>(dat)0 && XLogic>=MAXULDAT-(uldat)DeltaX)
	DeltaX=(dat)(MAXULDAT-XLogic-(uldat)1);
    else if (DeltaX<(dat)0 && XLogic<(uldat)-DeltaX)
	DeltaX=-(dat)XLogic;
    if (Window->Flags & WINFL_USEANY) {
	/*
	 * WARNING: Window->Contents and other methods
	 * may be unable to handle out-of-bound rows
	 */
	if (DeltaY>(dat)0 && (uldat)YLogic+YWidth-2+DeltaY>=Window->MaxNumRow)
	    DeltaY=(dat)(Window->MaxNumRow-YLogic-YWidth+2);
	else if (DeltaY<(dat)0 && YLogic<(uldat)-DeltaY)
	    DeltaY=-(dat)YLogic;
    } else {
	if (DeltaY>(dat)0 && YLogic+DeltaY>=Window->MaxNumRow)
	    DeltaY=(dat)(Window->MaxNumRow-YLogic-(uldat)1);
	else if (DeltaY<(dat)0 && YLogic<(uldat)-DeltaY)
	    DeltaY=-(dat)YLogic;
    }
    if (DeltaX)
	Window->XLogic = (XLogic += DeltaX);
    if (DeltaY)
	Window->YLogic = (YLogic += DeltaY);
    if (DeltaX || DeltaY)
	DrawBorderWindow(Window, (DeltaX ? BORDER_DOWN : 0) | (DeltaY ? BORDER_RIGHT : 0));

    DrawAreaWindow(Window, FALSE);

    if (Window == All->FirstScreen->FocusWindow)
	UpdateCursor();
}

byte ExecScrollFocusWindow(void) {
    screen *Screen;
    dat ScreenWidth, ScreenHeight;
    window *Window;
    uldat Attrib, Scroll;
    udat XWidth, YWidth;
    dat DeltaX, DeltaY;
    
    if (!(All->FlagsMove & GLMOVE_SCROLL_1stWIN))
	return FALSE;
    
    if (!(Screen=All->FirstScreen) || !(Window=Screen->FocusWindow))
	return FALSE;
    
    Attrib=Window->Attrib;
    DeltaX=DeltaY=(num)0;
    
    if (Attrib & WINDOW_X_BAR && Attrib & X_BAR_SELECT)
	DeltaX=1;
    else if (Attrib & WINDOW_Y_BAR && Attrib & Y_BAR_SELECT)
	DeltaY=1;
    else
	return FALSE;
    
    ScreenWidth=Screen->ScreenWidth;
    ScreenHeight=Screen->ScreenHeight;
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;
    
    XWidth=Min2(XWidth, ScreenWidth);
    YWidth=Min2(YWidth, ScreenHeight);
    Scroll=Attrib & SCROLL_ANY_SELECT;
    if (Scroll!=ARROW_BACK_SELECT && Scroll!=ARROW_FWD_SELECT &&
	Scroll!=PAGE_BACK_SELECT && Scroll!=PAGE_FWD_SELECT &&
	Scroll!=TAB_SELECT)
	return FALSE;
    
    if (Scroll==ARROW_BACK_SELECT) {
	DeltaX=-DeltaX;
	DeltaY=-DeltaY;
    }
    else if (Scroll==ARROW_FWD_SELECT)
	;
    else if (Scroll==PAGE_BACK_SELECT) {
	DeltaX*=-(XWidth-3);
	DeltaY*=-(YWidth-3);
    }
    else if (Scroll==PAGE_FWD_SELECT) {
	DeltaX*=(XWidth-3);
	DeltaY*=(YWidth-3);
    }
    else if (Scroll==TAB_SELECT)
	return FALSE;
	
    if (Window == Screen->FirstWindow)
	ScrollFirstWindow(DeltaX, DeltaY, TRUE);
    else
	ScrollWindow(Window, DeltaX, DeltaY);
    return TRUE;
}

void HideMenu(byte on_off) {
    screen *Screen=All->FirstScreen;
    
    if (on_off) {
	if (Screen->YLimit == 1) {
	    if (Screen->Up) {
		Screen->Up--;
		Screen->YLimit--;
		DrawArea(Screen, NULL, NULL, NULL, NULL,
			 0, Screen->YLimit, Screen->ScreenWidth-1, Screen->YLimit, FALSE);
		UpdateCursor();
	    } else
		ResizeFirstScreen(-1);
	}
    } else {
	if (Screen->YLimit == 0) {
	    if (Screen->Up < MAXUDAT) {
		Screen->Up++;
		Screen->YLimit++;
		DrawMenuBar(Screen, MINDAT, MAXDAT);
		UpdateCursor();
	    } else
		ResizeFirstScreen(1);
	}
    }
}

void ChangeMenuFirstScreen(menuitem *NewItem, byte ByMouse, byte Flag) {
    screen *Screen;
    menu *Menu;
    menuitem *CurrItem;
    window *NewWin, *CurrWin;
    
    Screen=All->FirstScreen;
    Menu=Act(SearchMenu,Screen)(Screen);
    
    if (Menu) {

	if (All->SetUp->Flags & SETUP_HIDEMENU && Flag==ACTIVATE_MENU_FLAG)
	    HideMenu(FALSE);
	
	CurrWin = Screen->FocusWindow;
	CurrItem = Flag==ACTIVATE_MENU_FLAG ? (menuitem *)0 : Menu->MenuItemSelect;
    
	All->FlagsMove =
	    (
	     ByMouse && Flag != DISABLE_MENU_FLAG
	     ? GLMOVE_BY_MOUSE
	     : (byte)0
	     )
	    |
	    (
	     Flag == DISABLE_MENU_FLAG
	     ? (byte)0
	     : GLMOVE_1stMENU
	     );
	
	if (Flag!=DISABLE_MENU_FLAG) {
	    
	    if (Flag==ACTIVATE_MENU_FLAG)
		Screen->MenuWindow = CurrWin; /* so that it keeps `active' borders */

	    if (NewItem) {
		NewWin=(Menu->MenuItemSelect=NewItem)->Window;
	    
		if (ByMouse)
		    NewWin->CurY=MAXULDAT;
		else if (NewWin->CurY==MAXULDAT)
		    NewWin->CurY=(uldat)0;
	    
		if (NewItem->FlagActive)
		    NewWin->Flags &= ~WINFL_DISABLED;
		else
		    NewWin->Flags |= WINFL_DISABLED;
		
		Act(RealMap,NewWin)(NewWin, Screen);
	    } else
		Menu->MenuItemSelect=(menuitem *)0;
	}
	if (Flag!=ACTIVATE_MENU_FLAG) {
	    if (Flag==DISABLE_MENU_FLAG) {
		if ((NewWin = Screen->MenuWindow)) {
		    Act(Focus,NewWin)(NewWin);
		    Screen->MenuWindow = (window *)0;
		} else
		    Do(Focus,Window)((window *)0);
	    }
	    if (CurrItem && CurrWin && (CurrWin->Attrib & WINDOW_MENU))
		Act(UnMap,CurrWin)(CurrWin);
	    UpdateCursor();
	}
	if (All->SetUp->Flags & SETUP_HIDEMENU && Flag==DISABLE_MENU_FLAG)
	    HideMenu(TRUE);
    }
    DrawMenuBar(Screen, MINDAT, MAXDAT);
}




void MakeFirstWindow(window *Window, byte alsoFocus) {
    screen *Screen;
    window *firstWin, *focusWin;
    
    if (Window && (Screen=Window->Screen)) {
	
	firstWin = Screen->FirstWindow;
	MoveFirst(Window, Screen, Window);
	
	if (alsoFocus) {
	    focusWin = Act(Focus,Window)(Window);
	    if (focusWin != Window) {
		DrawBorderWindow(focusWin, BORDER_ANY);
		DrawMenuBar(Screen, MINDAT, MAXDAT);
	    }
	}
	if (firstWin != Window)
	    DrawAreaWindow(Window, FALSE);
	else if (focusWin != Window && alsoFocus)
	    DrawBorderWindow(Window, BORDER_ANY);

	if (Screen->FnHookWindow)
	    Screen->FnHookWindow(Screen->HookWindow);

	if (Screen == All->FirstScreen)
	    UpdateCursor();
    }
}

void MakeLastWindow(window *Window, byte alsoDeFocus) {
    screen *Screen;
    window *focusWin, *firstWin, *lastWin;
    
    if (Window && (Screen=Window->Screen)) {
	
	lastWin = Screen->LastWindow;
	MoveLast(Window, Screen, Window);
	
	if (alsoDeFocus) {
	    firstWin = Screen->FirstWindow;
	    focusWin = Act(Focus,firstWin)(firstWin);
	    if (firstWin != focusWin) {
		DrawBorderWindow(firstWin, BORDER_ANY);
		DrawMenuBar(Screen, MINDAT, MAXDAT);
	    }
	}
	if (lastWin != Window)
	    DrawAreaWindow(Window, FALSE);
	else if (alsoDeFocus)
	    DrawBorderWindow(Window, BORDER_ANY);
	
	if (Screen->FnHookWindow)
	    Screen->FnHookWindow(Screen->HookWindow);

	if (Screen == All->FirstScreen)
	    UpdateCursor();
    }
}







#if 0
void SetNewFont(void) {
    struct REGPACK regs;
    byte FontHeight;
    font *RamFont;
    udat i;
    
    FontHeight=HEIGHT_FONT;
    if (FontHeight==(byte)8)
	RamFont=NewFont8;
    else if (FontHeight==(byte)14)
	RamFont=NewFont14;
    else if (FontHeight==(byte)16)
	RamFont=NewFont16;
    else
	return;
    
    for (i=(udat)0; i<(udat)256 && RamFont[i].AsciiCode; i++) {
	regs.r_ax=0x1100;
	regs.r_bx=(FontHeight<<8) | 0x0000;
	regs.r_cx=0x0001;
	regs.r_dx=RamFont[i].AsciiCode;
	regs.r_es=(udat)FP_SEG((void *)RamFont[i].Bitmap);
	regs.r_bp=(udat)FP_OFF((void *)RamFont[i].Bitmap);
	intr(0x10, &regs);
    }
    All->SetUp->Flags |= NEW_FONT;
    
    /*Alcune schede richiedono anche : */
    
    regs.r_ax=0x1110;
    regs.r_bx=(FontHeight<<8) | 0x0000;
    regs.r_cx=0x0001;
    regs.r_dx=RamFont[0].AsciiCode;
    regs.r_es=(udat)FP_SEG((void *)RamFont[0].Bitmap);
    regs.r_bp=(udat)FP_OFF((void *)RamFont[0].Bitmap);
    intr(0x10, &regs);
}

void GetPalette(void) {
    struct REGPACK regs;
    udat i;
    palette *Palette;
    
    Palette=All->Palette;
    
    for (i=(udat)0; i<(udat)16; i++) {
	regs.r_ax=0x1007;
	regs.r_bx=i;
	intr(0x10, &regs);
	
	regs.r_ax=0x1015;
	regs.r_bx>>=8;
	intr(0x10, &regs);
	Palette[i].Red=regs.r_dx>>8;
	Palette[i].Green=regs.r_cx>>8;
	Palette[i].Blue=regs.r_cx & 0xFF;
    }
}

void SetPalette(void) {
    struct REGPACK regs;
    udat i;
    
    regs.r_ax=0x1012;
    regs.r_bx=0;
    regs.r_cx=16;
    regs.r_es=(udat)FP_SEG((void *)All->Palette);
    regs.r_dx=(udat)FP_OFF((void *)All->Palette);
    intr(0x10, &regs);
    
    for (i=(udat)0; i<(udat)16; i++) {
	regs.r_ax=0x1000;
	regs.r_bx=i<<8 | i;
	intr(0x10, &regs);
    }
}
#endif /* 0 */

