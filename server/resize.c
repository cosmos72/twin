/*
 *  resize.c  --  code to drag and resize screens and windows
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
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
#include "util.h"

#include "hw.h"

#include "Tw/Tw.h"
#include "Tw/Twstat.h"

#ifdef CONF__UNICODE
# include "Tutf/Tutf.h"
#endif

/***************/

byte NeedUpdateCursor;

void FlushCursor(void) {
    draw_ctx D;
    uldat type = NOCURSOR;
    screen Screen;
    window Window;
    widget W;
    ldat CurX, CurY;
    byte HasBorder;
    
    if (NeedUpdateCursor) {
	NeedUpdateCursor = FALSE;
    
	Screen = All->FirstScreen;
	Window = FindCursorWindow();
	
	if (Window && ((Window->Flags & WINDOWFL_CURSOR_ON) ||
		       (All->SetUp->Flags & SETUP_ALWAYSCURSOR))) {

	    W = (widget)Window;
	    
	    HasBorder = !(Window->Flags & WINDOWFL_BORDERLESS);
		
	    CurX = Window->CurX - Window->XLogic + HasBorder;
	    CurY = Window->CurY - Window->YLogic + HasBorder;
	    
	    if (CurX >= 0 && CurX < (Window->XWidth - HasBorder) &&
		CurY >= 0 && CurY < (Window->YWidth - HasBorder) &&
		InitDrawCtx(W, (dat)CurX, (dat)CurY, (dat)CurX, (dat)CurY, FALSE, &D) &&
		((Window == (window)Screen->FirstW && !Window->FirstW) ||
		 Window == WindowParent(RecursiveFindWidgetAt
					((widget)Screen, (dat)D.X1, (dat)D.Y1 - Screen->YLimit)))) {
		
		MoveToXY((dat)D.X1, (dat)D.Y1);
		if ((type = Window->CursorType) == NOCURSOR && All->SetUp->Flags & SETUP_ALWAYSCURSOR)
		    type = LINECURSOR;
	    }
	}
	SetCursorType(type);    
    }
}

static byte SendResizeSignal(window Window) {
    struct winsize wsiz;
    
    wsiz.ws_col = Window->USE.C.TtyData->SizeX;
    wsiz.ws_row = Window->USE.C.TtyData->SizeY;
    wsiz.ws_xpixel = 0;
    wsiz.ws_ypixel = 0;
    
#if 0
    /* is this enough on every OS ? */
    return ioctl(Window->RemoteData.Fd, TIOCSWINSZ, &wsiz) == 0;
#else
    return ioctl(Window->RemoteData.Fd, TIOCSWINSZ, &wsiz) == 0
	&& kill(-Window->RemoteData.ChildPid, SIGWINCH) == 0;
#endif
}

byte CheckResizeWindowContents(window Window) {
    if (W_USE(Window, USECONTENTS) &&
	(Window->USE.C.TtyData->SizeY != Window->YWidth - 2 ||
	 Window->USE.C.TtyData->SizeX != Window->XWidth - 2)) {
	return ResizeWindowContents(Window);
    }
    return TRUE;
}

byte ResizeWindowContents(window Window) {
    hwattr *NewCont, *saveNewCont, *OldCont, *max;
    ldat count, common, left;
    ttydata *Data = Window->USE.C.TtyData;
    dat x = Window->XWidth, y = Window->YWidth + Data->ScrollBack;
    
    if (!(Window->Flags & WINDOWFL_BORDERLESS))
	x -= 2, y -= 2;
    
    /* safety check: */
    if (x > 0 && y > 0) {
	if (!(saveNewCont = NewCont = (hwattr *)AllocMem(x*y*sizeof(hwattr))))
	    return FALSE;
    
	/*
	 * copy the Contents. Quite non-trivial for two reasons:
	 * 1. always preserve the cursor line
	 * 2. copy as much scrollback as possible
	 */
	if ((max = OldCont = Window->USE.C.Contents)) {
	    max += Window->WLogic * Window->HLogic;
	    common = Min2(Window->WLogic, x);
	    count = Min2(Window->HLogic, y);
	    OldCont += Window->USE.C.HSplit * Window->WLogic;
	
	    /* ok, now check for the cursor line */
	    if (Window->CurY >= y) {
		OldCont += (Window->CurY + 1 - y) * Window->WLogic;
		if (OldCont > max)
		    OldCont -= Window->WLogic * Window->HLogic;
	    }
	    
	    while (count--) {
		CopyMem(OldCont, NewCont, common*sizeof(hwattr));
		
		if ((OldCont += Window->WLogic) >= max) 
		    OldCont = Window->USE.C.Contents;
		NewCont += common;
		for (left = x - common; left; left--)
		    *NewCont++ = HWATTR(Window->ColText, ' ');
	    }
	    FreeMem(Window->USE.C.Contents);
	}
	
	left = (saveNewCont + x*y) - NewCont;
	while (left--)
	    *NewCont++ = HWATTR(Window->ColText, ' ');
    } else {
	x = y = 0;
	if (Window->USE.C.Contents)
	    FreeMem(Window->USE.C.Contents);
	saveNewCont = NULL;
    }
    
    Window->XLogic = 0;
    Window->YLogic = Data->ScrollBack;
    Window->WLogic = x;	/* Contents width */
    Window->HLogic = y;	/* Y visible + scrollback */
    Window->USE.C.HSplit = 0;	/* splitline == 0 */
    Window->USE.C.Contents = saveNewCont;

    if (Window->CurY >= y)
	Window->CurY = y - 1;
    else if (Window->CurY < Window->YLogic)
	Window->CurY = Window->YLogic;

    Data->SizeX = x;
    Data->SizeY = Window->YWidth - 2;
    Data->Top = 0;
    Data->Bottom = Data->SizeY;
    
    Data->Start = Window->USE.C.Contents + x * Data->ScrollBack;
    Data->Split = Window->USE.C.Contents + x * y;
    Data->saveX = Data->X = Window->CurX;
    Data->saveY = Data->Y = Window->CurY - Data->ScrollBack;    
    Data->Pos = Window->USE.C.Contents + Window->CurY * x + Window->CurX;
    
    if (!(Window->Attrib & WINDOW_WANT_CHANGES)
	&& Window->USE.C.TtyData && Window->RemoteData.FdSlot != NOSLOT)
	/* the MsgPort will not be informed of the resize...
	 * we must send SIGWINCH manually */
	(void)SendResizeSignal(Window);
    
    if (Window->Parent) {
	DrawBorderWindow(Window, BORDER_RIGHT);
    
	DrawLogicWidget((widget)Window, 0, 0, Window->WLogic-1, Window->HLogic-1);
    
	if (ContainsCursor((widget)Window))
	    UpdateCursor();
    }
    
    return TRUE;
}

static row InsertRowsWindow(window Window, ldat NumRows) {
    row CurrRow;
    
    while (NumRows--) {
	if ((CurrRow=Do(Create,Row)(FnRow, 0, ROW_ACTIVE))) {
	    Act(Insert,CurrRow)(CurrRow, Window, Window->USE.R.LastRow, NULL);
	} else
	    break;
    }
    DrawBorderWindow(Window, BORDER_RIGHT);
    return CurrRow;
}

byte EnsureLenRow(row Row, ldat Len, byte DefaultCol) {
    hwfont *tempText;
    hwcol *tempColText;
    ldat NewLen;
    
    if (Row->MaxLen<Len) {
	NewLen=(Len + (Len >> 1)) | All->SetUp->MinAllocSize;
	if ((tempText=(hwfont *)ReAllocMem(Row->Text, NewLen * sizeof(hwfont)))) {
	    if (!(Row->Flags & ROW_DEFCOL) && !DefaultCol) {
		if ((tempColText=(hwcol *)ReAllocMem(Row->ColText, NewLen * sizeof(hwcol))))
		    Row->ColText=tempColText;
		else
		    return FALSE;
	    }
	    Row->Text=tempText;
	    Row->MaxLen=NewLen;
	} else
	    return FALSE;
    }
    return TRUE;
}

byte WriteRow(window Window, ldat Len, CONST byte *Text) {
    row CurrRow;
    byte CONST * _Text;
    byte FlagNewRows, ModeInsert;
#if TW_SIZEOFHWFONT != 1
    hwfont CONST * to_UTF_16;
    ldat i;
#endif
    ldat x, y, max, RowLen;
    
    if (!W_USE(Window, USEROWS))
	return FALSE;
    
    x=Window->CurX;
    y=Window->CurY;
    max=Window->HLogic;
    CurrRow=Window->USE.R.LastRow;
    ModeInsert=Window->Flags & WINDOWFL_ROWS_INSERT;
    
    if (Window->State & WINDOW_ANYSEL)
	ClearHilight(Window);
    
    while (Len) {
	if (max<=y || (max==y+1 && (*Text==(byte)13 || *Text==(byte)10))) {
	    if (InsertRowsWindow(Window, Max2(y+1-max,1))) {
		FlagNewRows=TRUE;
		max=Window->HLogic;
		CurrRow=Window->USE.R.LastRow;
	    } else
		return FALSE;
	} else {
	    FlagNewRows=FALSE;
	    CurrRow=Act(FindRow,Window)(Window, y);
	}
	
	RowLen=(ldat)0;
	_Text=Text;
	while (RowLen < Len && *_Text!=(byte)13 && *_Text!=(byte)10)
	    ++RowLen, ++_Text;
	
	/*	WINDOWFL_INSERT non implementato */
	/*  Gap non implementato				 */
	
	if (RowLen) {
	    if (ModeInsert || (CurrRow && CurrRow->LenGap))
		return FALSE;

	    if (!EnsureLenRow(CurrRow, x+RowLen, (Window->Flags & WINDOWFL_ROWS_DEFCOL)))
		return FALSE;
	    
	    if (Window->USE.R.NumRowOne==y)
		Window->USE.R.RowOne=CurrRow;
	    if (Window->USE.R.NumRowSplit==y)
		Window->USE.R.RowSplit=CurrRow;
	    CurrRow->Flags=ROW_ACTIVE;

#if TW_SIZEOFHWFONT != 1
	    to_UTF_16 = Window->Charset;
	    
	    for (i = 0; i < RowLen; i++)
		CurrRow->Text[x+i] = to_UTF_16[Text[i]];
	    if (CurrRow->Len < x)
		for (i = CurrRow->Len; i < x; i++)
		    CurrRow->Text[i] = (hwfont)' ';
#else
	    CopyMem(Text, CurrRow->Text+x, RowLen);
	    if (CurrRow->Len<x)
		WriteMem(CurrRow->Text+CurrRow->Len, ' ', x-CurrRow->Len);
#endif

	    if (!(Window->Flags & WINDOWFL_ROWS_DEFCOL)) {
		WriteMem(CurrRow->ColText+x, Window->ColText, sizeof(hwcol)*RowLen);
		if (CurrRow->Len<x)
		    WriteMem(CurrRow->ColText+CurrRow->Len, Window->ColText,
			     sizeof(hwcol)*(x-CurrRow->Len));
	    }
	    
	    if (CurrRow->Len<x+RowLen)
		CurrRow->Len=x+RowLen;

	    DrawLogicWidget((widget)Window, x, y, x+RowLen-(ldat)1, y);
	
	    Text+=RowLen;
	    Len -=RowLen;
	}
	
	if (Len && (*Text=='\r' || *Text=='\n')) {
	    Window->CurX=x=(ldat)0;
	    Window->CurY=++y;
	    Text++, Len--;
	} else
	    Window->CurX=x+=RowLen;
    }
    
    if (Window == FindCursorWindow())
	UpdateCursor();
    
    return TRUE;
}




void ExposeWidget(widget W, dat XWidth, dat YWidth, dat Left, dat Up, CONST byte *Text, CONST hwfont *Font, CONST hwattr *Attr) {
    if (w_USE(W, USEEXPOSE)) {
	if (Text || Font || Attr) {
	    W->USE.E.Text = Text;
	    W->USE.E.HWFont = Font;
	    W->USE.E.HWAttr = Attr;
	    W->USE.E.X1 = Left;
	    W->USE.E.X2 = Left + XWidth - 1;
	    W->USE.E.Y1 = Up;
	    W->USE.E.Y2 = Up + YWidth - 1;
	} else {
	    W->USE.E.Text = (byte *)1;
	    W->USE.E.X1 = W->USE.E.Y1 =
		W->USE.E.X2 = W->USE.E.Y2 = -1;
	}
	
	DrawLogicWidget(W, Left, Up, Left + XWidth - 1, Up + YWidth - 1);
	W->USE.E.Text = NULL;
	W->USE.E.HWFont = NULL;
	W->USE.E.HWAttr = NULL;
    }
}


/***************/

void DragFirstScreen(ldat DeltaX, ldat DeltaY) {
    screen Screen;
    dat DHeight, DWidth;
    dat YLimit;
    ldat Left, Up, Rgt, Dwn;
    
    Screen=All->FirstScreen;
    YLimit=Screen->YLimit;
    DWidth=All->DisplayWidth;
    DHeight=All->DisplayHeight;
    
    DeltaX = -DeltaX;
    DeltaY = -DeltaY;
    
    if (DeltaY < MINDAT - Screen->YLogic)
	DeltaY = MINDAT - Screen->YLogic;
    else if (DeltaY > MAXDAT - Screen->YLogic - (ldat)DHeight + 1)
	DeltaY = MAXDAT - Screen->YLogic - (ldat)DHeight + 1;
    if (DeltaX < MINDAT - Screen->XLogic)
	DeltaX = MINDAT - Screen->XLogic;
    else if (DeltaX > MAXDAT - Screen->XLogic - (ldat)DWidth + 1)
	DeltaX = MAXDAT - Screen->XLogic - (ldat)DWidth + 1;
    
    if (!DeltaX && !DeltaY)
	return;
    
    Left=Max2(DeltaX, (ldat)0);
    Up=(ldat)YLimit+1+Max2(DeltaY, (ldat)0);
    Rgt=(ldat)DWidth-(ldat)1+Min2(DeltaX, (ldat)0);
    Dwn=(ldat)DHeight-(ldat)1+Min2(DeltaY, (ldat)0);
    Screen->XLogic+=DeltaX;
    Screen->YLogic+=DeltaY;
    
    if (Up<=Dwn && Left<=Rgt)
	DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)(Left-DeltaX), (dat)(Up-DeltaY));

    YLimit++;
    
    if (Abs(DeltaX) >= DWidth || Abs(DeltaY) >= DHeight - YLimit)
	DrawArea2((screen)0, (widget)0, (widget)0, 0, YLimit, DWidth-1, DHeight-1, FALSE);
    else if (DeltaY<0) {
	DrawArea2((screen)0, (widget)0, (widget)0, 0, YLimit, DWidth-1, YLimit-DeltaY, FALSE);
	if (DeltaX<=0)
	    DrawArea2((screen)0, (widget)0, (widget)0, 0, YLimit-DeltaY, -DeltaX-1, DHeight-1, FALSE);
	else if (DeltaX>0)
	    DrawArea2((screen)0, (widget)0, (widget)0, DWidth-DeltaX, YLimit-DeltaY, DWidth-1, DHeight-1, FALSE);
    }
    else {
	DrawArea2((screen)0, (widget)0, (widget)0, 0, DHeight-DeltaY, DWidth-1, DHeight-1, FALSE);
	if (DeltaX<=0)
	    DrawArea2((screen)0, (widget)0, (widget)0, 0, YLimit, -DeltaX-1, DHeight-DeltaY, FALSE);
	else if (DeltaX>=0)
	    DrawArea2((screen)0, (widget)0, (widget)0, DWidth-DeltaX, YLimit, DWidth-1, DHeight-DeltaY, FALSE);
    }
    UpdateCursor();
}

void ResizeFirstScreen(dat DeltaY) {
    screen Screen;
    dat DHeight, DWidth;
    dat YLimit;
    ldat Left, Up, Rgt, Dwn;
    
    Screen=All->FirstScreen;
    YLimit=Screen->YLimit;
    DWidth=All->DisplayWidth;
    DHeight=All->DisplayHeight;
    
    if (YLimit+DeltaY>=DHeight)
	DeltaY=DHeight-YLimit-1;
    else if (YLimit+DeltaY<MINDAT)
	DeltaY=MINDAT-YLimit;
    
    Left=(ldat)0;
    Up=Max2(YLimit, 0);
    Rgt=DWidth-1;
    Dwn=DHeight-1-Max2((ldat)DeltaY, 0);
    Screen->YLimit+=DeltaY;
    
    if (DeltaY<0) {
	if (Up<=Dwn)
	    DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)Left, (dat)Up+DeltaY);
	DrawArea2(Screen, (widget)0, (widget)0, (dat)Left, (dat)Dwn+1+DeltaY, (dat)Rgt, (dat)Dwn, FALSE);
    }
    else if (DeltaY>(dat)0) {
	if (Up<=Dwn)
	    DragArea((dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, (dat)Left, (dat)Up+(dat)DeltaY);
	DrawArea2((screen)0, (widget)0, (widget)0, (dat)Left, (dat)Up, (dat)Rgt, (dat)Up+DeltaY-1, FALSE);
    }
    UpdateCursor();
}

void CenterWindow(window Window) {
    screen Screen;
    ldat parz1, parz2;
    ldat DeltaX, DeltaY;
    dat DWidth, DHeight;
    dat YLimit;
    
    if (!Window || !(Screen=(screen)Window->Parent) || (Screen!=All->FirstScreen) ||
	(Window->Flags & WINDOWFL_MENU))
	return;
    
    DWidth=All->DisplayWidth;
    DHeight=All->DisplayHeight;
    YLimit=Screen->YLimit;
    parz1=(ldat)Window->Left-(ldat)Screen->XLogic;
    parz2=parz1+(ldat)Window->XWidth;
    if (parz1>=(ldat)0 && parz2<=(ldat)DWidth)
	DeltaX=(ldat)0;
    else
	DeltaX=-parz1;
    
    if (Window->XWidth<=(dat)DWidth) {
	parz1=DeltaX;
	parz2=DeltaX+(ldat)DWidth-(ldat)Window->XWidth;
	DeltaX=Abs(parz1)<Abs(parz2) ? parz1 : parz2;
    } else
	DeltaX--;
	
    
    parz1=(ldat)Window->Up-(ldat)Screen->YLogic+(ldat)YLimit;
    parz2=parz1+(ldat)Window->YWidth;
    if (parz1>(ldat)YLimit && parz2<=(ldat)DHeight)
	DeltaY=(ldat)0;
    else
	DeltaY=-parz1+YLimit+1;
    
    if ((ldat)Window->YWidth<=(ldat)DHeight-(ldat)YLimit-1) {
	parz1=DeltaY;
	parz2=DeltaY+(ldat)DHeight-(ldat)YLimit-1-(ldat)Window->YWidth;
	DeltaY=Abs(parz1)<Abs(parz2) ? parz1 : parz2;
    } else
	DeltaY--;
	
    DragFirstScreen(DeltaX, DeltaY);
}

INLINE void DrawDeltaShadeFirstWindow(dat i, dat j) {
    ldat _Left, _Up, _Rgt, _Dwn;
    ldat Left_, Up_, Rgt_, Dwn_;
    screen Screen;
    window Window;
    dat YLimit;
    dat DWidth, DHeight;
    byte DeltaXShade, DeltaYShade;

    Screen=All->FirstScreen;
    if (!(Window=(window)Screen->FirstW) || !IS_WINDOW(Window))
	return;

    DWidth=All->DisplayWidth;
    DHeight=All->DisplayHeight;
    DeltaXShade=All->SetUp->DeltaXShade;
    DeltaYShade=All->SetUp->DeltaYShade;
    YLimit=Screen->YLimit;
    
     Left_= (ldat)Window->Left - Screen->XLogic;
    _Left = Left_ - i;
     Rgt_ = Left_ + (ldat)Window->XWidth-(ldat)1;
    _Rgt  = Rgt_ - i;
     Up_  = (ldat)Window->Up - Screen->YLogic + (ldat)YLimit;
    _Up   = Up_ - j;
     Dwn_ = Up_ + (Window->Attrib & WINDOW_ROLLED_UP ? (ldat)0 : (ldat)Window->YWidth-(ldat)1);
    _Dwn  = Dwn_ - j;
 
    DrawAreaShadeWindow(Screen, Window, 0, 0, MAXDAT, MAXDAT, _Left, _Up, _Rgt, _Dwn, FALSE);
    DrawAreaShadeWindow(Screen, Window, 0, 0, MAXDAT, MAXDAT, Left_, Up_, Rgt_, Dwn_, TRUE);
}

void DragFirstWindow(dat i, dat j) {
    ldat Left, Up, Rgt, Dwn;
    ldat _Left, _Up, _Rgt, _Dwn;
    ldat Left_, Up_, Rgt_, Dwn_;
    ldat xLeft, xRgt, xUp, xDwn;
    screen Screen;
    setup *SetUp;
    window Window;
    dat YLimit;
    dat DWidth, DHeight;
    byte Shade, DeltaXShade, DeltaYShade;
    
    Screen=All->FirstScreen;
    if (!(Window=(window)Screen->FirstW) || !IS_WINDOW(Window) || !(Window->Attrib & WINDOW_DRAG))
	return;

    YLimit=Screen->YLimit;
    if (i<0 && Window->Left<MINDAT-i)
	i=MINDAT-Window->Left;
    else if (i>0 && Window->Left>MAXDAT-i)
	i=MAXDAT-Window->Left;
    
    if (j<0 && Window->Up<MINDAT-j)
	j=MINDAT-Window->Up;
    else if (j>0 && Window->Up>MAXDAT-j)
	j=MAXDAT-Window->Up;

    DWidth=All->DisplayWidth;
    DHeight=All->DisplayHeight;
    SetUp=All->SetUp;
    Shade=!!(SetUp->Flags & SETUP_SHADOWS);
    DeltaXShade=Shade ? SetUp->DeltaXShade : (byte)0;
    DeltaYShade=Shade ? SetUp->DeltaYShade : (byte)0;
   
    Left = (ldat)Window->Left - Screen->XLogic;
    Rgt  = Left+(ldat)Window->XWidth-(ldat)1;
    Up   = (ldat)Window->Up - Screen->YLogic + (ldat)YLimit;
    Dwn  = Up+(Window->Attrib & WINDOW_ROLLED_UP ? (ldat)0 : (ldat)Window->YWidth-(ldat)1);

    /* calculate the visible part of the window for direct DragArea() */
    
    Left_= Max2(Left, - Min2(i, 0));
    Up_  = Max2(Up,  (ldat)YLimit+1 - Min2(j, 0));
    Rgt_ = Min2(Rgt, (ldat)DWidth -(ldat)1 - Max2(i, 0));
    Dwn_ = Min2(Dwn, (ldat)DHeight-(ldat)1 - Max2(j, 0));

    /* copy the visible part */
    if (Left_ <= Rgt_ && Up_ <= Dwn_)
	DragArea(Left_, Up_, Rgt_, Dwn_, Left_+i, Up_+j);

    Window->Left += i;
    Window->Up += j;

    if (Shade)
	/* update the window's shadow */
	DrawDeltaShadeFirstWindow(i, j);
    

    /* redraw the old window location */
    
    _Left = Max2(Left,(ldat)0);
    _Up   = Max2(Up,  (ldat)YLimit+1);
    _Rgt  = Min2(Rgt, (ldat)DWidth-(ldat)1);
    _Dwn  = Min2(Dwn, (ldat)DHeight-(ldat)1);

    if (_Left <= _Rgt && _Up <= _Dwn) {
	xLeft = _Left;
	xRgt = _Rgt;
	if (i) {
	    if (i > 0)
		xRgt = Min2(_Left + i - 1, _Rgt); 
	    else
		xLeft = Max2(_Left, _Rgt + i + 1); 
	    DrawArea2(Screen, (widget)0, (widget)0,
		     (dat)xLeft, (dat)_Up, (dat)xRgt, (dat)_Dwn, FALSE);
	}
	xUp = _Up;
	xDwn = _Dwn;
	if (j && (!i || xRgt < _Rgt || xLeft > _Left)) {
	    if (j > 0)
		xDwn = Min2(_Up + j - 1, _Dwn);
	    else
		xUp = Max2(_Up, _Dwn + j + 1); 
	    
	    if (xRgt < _Rgt) {
		xLeft = xRgt + (ldat)1;
		xRgt = _Rgt;
	    } else if (xLeft > _Left) {
		xRgt = xLeft - (ldat)1;
		xLeft = _Left;
	    }
		
	    DrawArea2(Screen, (widget)0, (widget)0,
		     (dat)xLeft, (dat)xUp, (dat)xRgt, (dat)xDwn, FALSE);
	}
    }

    /* draw the new window location */

    /* xxx_ : final location of copied area */
    Left_ += (ldat)i;
    Up_   += (ldat)j;
    Rgt_  += (ldat)i;
    Dwn_  += (ldat)j;
    
    /* _xxx : final location of window */
    _Left = Max2(Left+ (ldat)i, (ldat)0);
    _Up   = Max2(Up  + (ldat)j, (ldat)YLimit+1);
    _Rgt  = Min2(Rgt + (ldat)i, (ldat)DWidth-(ldat)1);
    _Dwn  = Min2(Dwn + (ldat)j, (ldat)DHeight-(ldat)1);

    if (_Left <= _Rgt && _Up <= _Dwn) {
	if (Left_ > _Left) {
	    xLeft = Min2(Left_ - (ldat)1, _Rgt);
	    DrawWidget((widget)Window, (dat)_Left, (dat)_Up, (dat)xLeft, (dat)_Dwn, FALSE);
	} else
	    xLeft = _Left;
	if (Rgt_ < _Rgt) {
	    xRgt = Max2(Rgt_ + (ldat)1, _Left);
	    DrawWidget((widget)Window, (dat)xRgt, (dat)_Up, (dat)_Rgt, (dat)_Dwn, FALSE);
	} else
	    xRgt = _Rgt;
	if (Up_ > _Up) {
	    xUp = Min2(Up_ - (ldat)1, _Dwn);
	    DrawWidget((widget)Window, (dat)xLeft, (dat)_Up, (dat)xRgt, (dat)xUp, FALSE);
	}
	if (Dwn_ < _Dwn) {
	    xDwn = Max2(Dwn_ + (ldat)1, _Up);
	    DrawWidget((widget)Window, (dat)xLeft, (dat)xDwn, (dat)xRgt, (dat)_Dwn, FALSE);
	}
    }
    
    if (ContainsCursor((widget)Window))
	UpdateCursor();
}

void DragWindow(window Window, dat i, dat j) {
    ldat Left, Up, Rgt, Dwn;
    ldat Left1, Rgt1;
    screen Screen;
    setup *SetUp;
    dat YLimit;
    dat DWidth, DHeight;
    byte Shade, DeltaXShade, DeltaYShade;

    if (!Window || !(Window->Attrib & WINDOW_DRAG))
	return;

    if (Window == (window)All->FirstScreen->FirstW) {
	DragFirstWindow(i, j);
	return;
    }

    if (!(Screen=(screen)Window->Parent) || !IS_SCREEN(Screen))
	return;

    DWidth=All->DisplayWidth;
    DHeight=All->DisplayHeight;
    SetUp=All->SetUp;
    Shade=!!(SetUp->Flags & SETUP_SHADOWS);
    DeltaXShade=Shade ? SetUp->DeltaXShade : (byte)0;
    DeltaYShade=Shade ? SetUp->DeltaYShade : (byte)0;
    YLimit=Screen->YLimit;

    Up=(ldat)Window->Up - Screen->YLogic + (ldat)YLimit;
    Left=(ldat)Window->Left - Screen->XLogic;
    Rgt=Left+(ldat)Window->XWidth-(ldat)1;
    Dwn=Up+(Window->Attrib & WINDOW_ROLLED_UP ? (ldat)0 : (ldat)Window->YWidth-(ldat)1);

    if (i<(dat)0) {
	if (Window->Left < MINDAT - i)
	    i = MINDAT - Window->Left;
    }
    else if (i>(dat)0) {
	if (Window->Left>MAXDAT-i)
	    i = MAXDAT - Window->Left;
    }
    if (j < (dat)0) {
	if (Window->Up < MINDAT - j)
	    j = MINDAT - Window->Up;
    }
    else if (j > (dat)0) {
	if (Window->Up > MAXDAT - j)
	    j = MAXDAT-Window->Up;
    }
    Window->Left+=i;
    Window->Up+=j;
    DrawArea2((screen)0, (widget)0, (widget)0,
	     Left+i, Up+j, Rgt+i+DeltaXShade, Dwn+j+DeltaYShade, FALSE);
    
    if (Left<(ldat)DWidth && Up<(ldat)DHeight && Rgt+DeltaXShade>=(ldat)0 && Dwn+DeltaYShade>(ldat)YLimit) {
	if (i > 0) {
	    Rgt1=Min2(Rgt+DeltaXShade, Left+i-1);
	    Left1=Max2(0, Left);
	} else {
	    Left1=Max2(Rgt+DeltaXShade+i, Left);
	    Rgt1=Min2((ldat)DWidth-(ldat)1, Rgt+DeltaXShade);
	}
	DrawArea2((screen)0, (widget)0, (widget)0,
		  (dat)Left1, (dat)Up, (dat)Rgt1, (dat)Dwn+DeltaYShade, FALSE);
	
	if (j > 0) {
	    Dwn=Min2(Dwn+DeltaYShade, Up+j-1);
	    Up=Max2(YLimit+1, Up);
	} else {
	    Up=Max2(Dwn+DeltaYShade+j, Up);
	    Dwn=Min2((ldat)DHeight-(ldat)1, Dwn+DeltaYShade);
	}
	DrawArea2((screen)0, (widget)0, (widget)0,
		  (dat)Left, (dat)Up, (dat)Rgt+DeltaXShade, (dat)Dwn, FALSE);
    }
    if (Window == (window)All->FirstScreen->FocusW)
	UpdateCursor();
}

void ResizeRelFirstWindow(dat i, dat j) {
    ldat Left, Up, Rgt, Dwn;
    screen Screen;
    setup *SetUp;
    window Window;
    dat DeltaX, DeltaY;
    dat YLimit, XWidth, YWidth;
    dat MinXWidth, MinYWidth, MaxXWidth, MaxYWidth;
    dat DWidth, DHeight;
    byte Shade, DeltaXShade, DeltaYShade, HasBorder;
    
    Screen=All->FirstScreen;
    if (!(Window=(window)Screen->FirstW) || !IS_WINDOW(Window) || (!i && !j)) /* || !(Window->Attrib & WINDOW_RESIZE)) */
	return;
    
    DWidth = All->DisplayWidth;
    DHeight = All->DisplayHeight;
    SetUp = All->SetUp;
    Shade = !!(SetUp->Flags & SETUP_SHADOWS);
    DeltaXShade = Shade ? SetUp->DeltaXShade : (byte)0;
    DeltaYShade = Shade ? SetUp->DeltaYShade : (byte)0;
    HasBorder = !(Window->Flags & WINDOWFL_BORDERLESS);
    YLimit = Screen->YLimit + 1;
    
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;
    MinXWidth=Window->MinXWidth;
    MinYWidth=Window->MinYWidth;
    MaxXWidth=Window->MaxXWidth;
    MaxYWidth=Window->MaxYWidth;
    
    Up=(ldat)Window->Up - Screen->YLogic + (ldat)YLimit - 1;
    Left=(ldat)Window->Left - Screen->XLogic;
    Rgt=Left+(ldat)XWidth-(ldat)1;
    Dwn=Up+(ldat)YWidth-(ldat)1;

    if ((DeltaX=-i)>(dat)0 && XWidth>MinXWidth) {
	if (MinXWidth+DeltaX>XWidth)
	    DeltaX=XWidth-MinXWidth;
	XWidth=Window->XWidth-=DeltaX;
	if (Left<(ldat)DWidth && Up<(ldat)DHeight &&
	    Rgt+(ldat)DeltaXShade>=(ldat)0 && Dwn+(ldat)DeltaYShade>=(ldat)YLimit) {
	    
	    DrawArea2((screen)0, (widget)Window, (widget)0, (dat)Rgt-DeltaX+1,
		      (dat)Max2(Up, (ldat)YLimit), (dat)Rgt, (dat)Max2((ldat)YLimit, Dwn), FALSE);
	    if (Shade) {
		DrawArea2(Screen, (widget)0, (widget)0, (dat)Rgt+Max2((dat)DeltaXShade-DeltaX-1, 1),
			  (dat)Max2((ldat)YLimit, Up), (dat)Rgt+(dat)DeltaXShade, (dat)Dwn+(dat)DeltaYShade, FALSE);
		if (DeltaX>(dat)DeltaXShade)
		    DrawArea2(Screen, (widget)0, (widget)0, (dat)Rgt+(dat)DeltaXShade-DeltaX+1,
			      (dat)Max2((ldat)YLimit, Dwn+1), (dat)Rgt, (dat)Dwn+(dat)DeltaYShade, FALSE);
	    }
	}
	Rgt-=DeltaX;
    }
    else if ((DeltaX=i)>(dat)0 && XWidth<MaxXWidth) {
	if (XWidth>MaxXWidth-DeltaX)
	    DeltaX=MaxXWidth-XWidth;
	XWidth=Window->XWidth+=DeltaX;
	if (Left<(ldat)DWidth && Up<(ldat)DHeight &&
	    Rgt+(ldat)DeltaXShade>=-(ldat)DeltaX && Dwn+(ldat)DeltaYShade>=(ldat)YLimit) {
	    
	    DrawArea2((screen)0, (widget)Window, (widget)0, (dat)Rgt, (dat)Max2(Up+HasBorder, (ldat)YLimit),
		      (dat)Rgt+DeltaX-HasBorder, (dat)Max2((ldat)YLimit, Dwn-HasBorder), FALSE);
	    if (Shade) {
		DrawArea2(Screen, (widget)0, (widget)0, (dat)Rgt+1+Max2((dat)DeltaXShade, DeltaX),
			  (dat)Max2((ldat)YLimit, Up+(dat)DeltaYShade), (dat)Rgt+DeltaX+(dat)DeltaXShade, (dat)Dwn+(dat)DeltaYShade, FALSE);
		if (DeltaX>(dat)DeltaXShade)
		    DrawArea2(Screen, (widget)0, (widget)0, (dat)Rgt+(dat)DeltaXShade+1,
			      (dat)Max2((ldat)YLimit, Dwn+1), (dat)Rgt+DeltaX, (dat)Dwn+(dat)DeltaYShade, FALSE);
	    }
	}
	Rgt+=DeltaX;
    }
    if ((DeltaY=-j)>(dat)0 && YWidth>MinYWidth) {
	if (MinYWidth+DeltaY>YWidth)
	    DeltaY=YWidth-MinYWidth;
	YWidth=Window->YWidth-=DeltaY;
	if (Left<(ldat)DWidth && Up<(ldat)DHeight &&
	    Rgt+(ldat)DeltaXShade>=(ldat)0 && Dwn+(ldat)DeltaYShade>=(ldat)YLimit) {
	    
	    DrawArea2(Screen, (widget)0, (widget)0, (dat)Left,
		      (dat)Max2(Dwn-(ldat)DeltaY+(ldat)1, (ldat)YLimit),
		      (dat)Rgt, (dat)Max2((ldat)YLimit, Dwn), FALSE);
	    if (Shade) {
		DrawArea2(Screen, (widget)0, (widget)0, (dat)Left,
			  (dat)Max2((ldat)YLimit, Dwn+1+Max2((dat)DeltaYShade-DeltaY, (dat)0)),
			  (dat)Rgt+(dat)DeltaXShade, (dat)Dwn+(dat)DeltaYShade, FALSE);
		if (DeltaY>(dat)DeltaYShade)
		    DrawArea2(Screen, (widget)0, (widget)0, (dat)Rgt+1,
			      (dat)Max2((ldat)YLimit, Dwn+1+(dat)DeltaYShade-DeltaY),
			      (dat)Rgt+(dat)DeltaXShade, (dat)Max2((ldat)YLimit, Dwn), FALSE);
	    }
	}
	Dwn-=DeltaY;
    }
    else if ((DeltaY=j)>(dat)0 && YWidth<MaxYWidth) {
	if (YWidth>MaxYWidth-DeltaY)
	    DeltaY=MaxYWidth-YWidth;
	YWidth=Window->YWidth+=DeltaY;
	if (Left<(ldat)DWidth && Up<(ldat)DHeight &&
	    Rgt+(ldat)DeltaXShade>=(ldat)0 && Dwn+(ldat)DeltaYShade>=-(ldat)DeltaY+(ldat)YLimit) {
	    
	    DrawArea2((screen)0, (widget)Window, (widget)0, (dat)Left, (dat)Max2((ldat)YLimit, Dwn),
		      (dat)Rgt-HasBorder, (dat)Max2((ldat)YLimit, Dwn+DeltaY-HasBorder), FALSE);
	    if (Shade) {
		DrawArea2(Screen, (widget)0, (widget)0, (dat)Left+(dat)DeltaXShade,
			  (dat)Dwn+1+Max2(DeltaY, (dat)DeltaYShade), (dat)Rgt+(dat)DeltaXShade,
			  (dat)Dwn+(dat)DeltaYShade+DeltaY, FALSE);
		if (DeltaY>(dat)DeltaYShade)
		    DrawArea2(Screen, (widget)0, (widget)0, (dat)Rgt+1, (dat)Dwn+(dat)DeltaYShade+1,
			      (dat)Rgt+(dat)DeltaXShade, (dat)Max2((ldat)YLimit, Dwn+DeltaY), FALSE);
	    }
	}
	Dwn+=DeltaY;
    }
    if (DeltaX || DeltaY) {
	DrawBorderWindow(Window, BORDER_ANY);
	if (Window == (window)Screen->FocusW)
	    UpdateCursor();
	
	/* resize contents? for Interactive Resize, let the WM resize it
	 * when Interactive Resize finishes. otherwise, do it now */
	if (W_USE(Window, USECONTENTS) && Window->USE.C.Contents &&
	    (Window != Screen->ClickWindow || (All->State & STATE_ANY) != STATE_RESIZE))
	    
	    CheckResizeWindowContents(Window);
    }
}


void ResizeRelWindow(window Window, dat i, dat j) {
    ldat Left, Up, Rgt, Dwn;
    widget Parent;
    setup *SetUp;
    dat DeltaX, DeltaY;
    dat YLimit, XWidth, YWidth;
    dat MinXWidth, MinYWidth, MaxXWidth, MaxYWidth;
    dat DWidth, DHeight;
    byte Shade, DeltaXShade, DeltaYShade;
    
    if (!Window || (!i && !j)) /* || !(Window->Attrib & WINDOW_RESIZE) */
	return;

    if ((widget)Window == All->FirstScreen->FirstW) {
	ResizeRelFirstWindow(i, j);
	return;
    }
    
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;
    MinXWidth=Window->MinXWidth;
    MinYWidth=Window->MinYWidth;
    MaxXWidth=Window->MaxXWidth;
    MaxYWidth=Window->MaxYWidth;

    if ((Parent=Window->Parent) && IS_SCREEN(Parent)) {
	DWidth=All->DisplayWidth;
	DHeight=All->DisplayHeight;
	SetUp=All->SetUp;
	Shade=!!(SetUp->Flags & SETUP_SHADOWS);
	DeltaXShade=Shade ? SetUp->DeltaXShade : (byte)0;
	DeltaYShade=Shade ? SetUp->DeltaYShade : (byte)0;
	YLimit=Parent->Up;
	
	Up=(ldat)Window->Up- Parent->YLogic + (ldat)YLimit;
	Left=(ldat)Window->Left - Parent->XLogic;
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
    if (DeltaX || DeltaY) {
	if (Parent && IS_SCREEN(Parent)) {
	    Up = (dat)Max2(Up, (ldat)YLimit);
	    DrawArea2((screen)Parent, (widget)0, (widget)0,
		      (dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, FALSE);
	    if (Shade)
		DrawShadeWindow(Window, 0, 0, MAXDAT, MAXDAT, FALSE);
	}
    
	if (Window == (window)All->FirstScreen->FocusW)
	    UpdateCursor();
	
	/* resize contents? for Interactive Resize, let the WM resize it
	 * when Interactive Resize finishes. otherwise, do it now */
	if (W_USE(Window, USECONTENTS) && Window->USE.C.Contents &&
	    (Window != All->FirstScreen->ClickWindow || (All->State & STATE_ANY) != STATE_RESIZE))
	    
	    CheckResizeWindowContents(Window);
    }
}
	
/*
 * coordinates are special here:
 * (0,0) is the (XLogic,YLogic) cell of the Window,
 * i.e. the first visible one.
 */
void ScrollFirstWindowArea(dat X1, dat Y1, dat X2, dat Y2, dat DeltaX, dat DeltaY) {
    screen Screen;
    dat DWidth, DHeight;
    window Window;
    ldat shLeft, shUp, shRgt, shDwn;
    dat Left, Up, Rgt, Dwn;
    dat Xstart, Ystart, Xend, Yend;
    dat XWidth, YWidth, YLimit;
    
    Screen=All->FirstScreen;
    Window=(window)Screen->FirstW;

    if (!Window || !IS_WINDOW(Window) || (Window->Attrib & WINDOW_ROLLED_UP))
	return;
    
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;

    if (X1 > X2 || X1 > XWidth-3 || X2 < 0 || Y1 > Y2 || Y1 > YWidth-3 || Y2 < 0)
	return;
    
    DWidth = All->DisplayWidth;
    DHeight = All->DisplayHeight;
    YLimit=Screen->YLimit;
    shUp=(ldat)Window->Up - Screen->YLogic + (ldat)YLimit;
    shLeft=(ldat)Window->Left - Screen->XLogic;
    shRgt=shLeft+(ldat)Window->XWidth-(ldat)1;
    shDwn=shUp+(ldat)Window->YWidth-(ldat)1;
    
    X1=Max2(X1, 0);
    Y1=Max2(Y1, 0);
    X2=Min2(X2, XWidth-3);
    Y2=Min2(Y2, YWidth-3);
    
    Left=(dat)(shLeft+(ldat)1+(ldat)X1);
    if (Left < 0)
	X1 += -Left, Left = 0;
    else if (Left >= DWidth)
	return;
    
    Up=(dat)(shUp+(ldat)1+(ldat)Y1);
    if (Up <= (dat)YLimit)
	Y1 += (dat)YLimit + 1 - Up, Up = (dat)YLimit + 1;
    else if (Up >= DHeight)
	return;
    
    Rgt=(dat)(shLeft+(ldat)1+(ldat)X2);
    if (Rgt >= DWidth)
	X2 -= Rgt - DWidth, Rgt = DWidth - 1;
    else if (Rgt < 0)
	return;

    Dwn=(dat)(shUp+(ldat)1+(ldat)Y2);
    if (Dwn >= DHeight)
	Y2 -= Dwn - DHeight, Dwn = DHeight - 1;
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
	    DrawWidget((widget)Window, Left, Up, Left+DeltaX-1, Dwn, FALSE);
	else
	    DrawWidget((widget)Window, Rgt+DeltaX+1, Up, Rgt, Dwn, FALSE);
    }
    else if (DeltaY) {
	if (Yend>=Ystart)
	    DragArea(Xstart, Ystart, Xend, Yend, Xstart, Ystart+DeltaY);
	if (DeltaY>(dat)0)
	    DrawWidget((widget)Window, Left, Up, Rgt, Up+DeltaY-1, FALSE);
	else
	    DrawWidget((widget)Window, Left, Dwn+DeltaY+1, Rgt, Dwn, FALSE);
    }
}

void ScrollFirstWindow(dat DeltaX, dat DeltaY, byte byXYLogic) {
    window Window;
    ldat XLogic, YLogic;
    dat XWidth, YWidth;
    
    Window=(window)All->FirstScreen->FirstW;
    if (!Window || !IS_WINDOW(Window))
	return;
    
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;
    
    if (byXYLogic) {
	XLogic=Window->XLogic;
	YLogic=Window->YLogic;
	
	if (DeltaX>(dat)0 && XLogic>=MAXLDAT-(ldat)DeltaX)
	    DeltaX=(dat)(MAXLDAT-XLogic-(ldat)1);
	else if (DeltaX<(dat)0 && XLogic<(ldat)-DeltaX)
	    DeltaX=-(dat)XLogic;
	if (!W_USE(Window, USEROWS)) {
	    /*
	     * WARNING: Window->USE.C.Contents and other methods
	     * may be unable to handle out-of-bound rows
	     */
	    if (DeltaY>(dat)0 && (ldat)YLogic+YWidth-2+DeltaY>=Window->HLogic)
		DeltaY=(dat)(Window->HLogic-YLogic-YWidth+2);
	    else if (DeltaY<(dat)0 && YLogic<(ldat)-DeltaY)
		DeltaY=-(dat)YLogic;
	} else {
	    if (DeltaY>(dat)0 && YLogic+DeltaY>=Window->HLogic)
		DeltaY=(dat)(Window->HLogic-YLogic-(ldat)1);
	    else if (DeltaY<(dat)0 && YLogic<(ldat)-DeltaY)
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
    
    if (byXYLogic && Window == (window)All->FirstScreen->FocusW)
	UpdateCursor();
}

void ScrollWindow(window Window, dat DeltaX, dat DeltaY) {
    ldat XLogic, YLogic;
    dat YWidth;
    
    if (!Window || !IS_WINDOW(Window) || (!DeltaX && !DeltaY))
	return;
    
    if ((widget)Window == All->FirstScreen->FirstW) {
	ScrollFirstWindow(DeltaX, DeltaY, TRUE);
	return;
    }
    
    YWidth=Window->YWidth;
    XLogic=Window->XLogic;
    YLogic=Window->YLogic;
	
    if (DeltaX>(dat)0 && XLogic>=MAXLDAT-(ldat)DeltaX)
	DeltaX=(dat)(MAXLDAT-XLogic-(ldat)1);
    else if (DeltaX<(dat)0 && XLogic<(ldat)-DeltaX)
	DeltaX=-(dat)XLogic;
    if (!W_USE(Window, USEROWS)) {
	/*
	 * WARNING: Window->USE.C.Contents and other methods
	 * may be unable to handle out-of-bound rows
	 */
	if (DeltaY>(dat)0 && (ldat)YLogic+YWidth-2+DeltaY>=Window->HLogic)
	    DeltaY=(dat)(Window->HLogic-YLogic-YWidth+2);
	else if (DeltaY<(dat)0 && YLogic<(ldat)-DeltaY)
	    DeltaY=-(dat)YLogic;
    } else {
	if (DeltaY>(dat)0 && YLogic+DeltaY>=Window->HLogic)
	    DeltaY=(dat)(Window->HLogic-YLogic-(ldat)1);
	else if (DeltaY<(dat)0 && YLogic<(ldat)-DeltaY)
	    DeltaY=-(dat)YLogic;
    }
    if (DeltaX)
	Window->XLogic = (XLogic += DeltaX);
    if (DeltaY)
	Window->YLogic = (YLogic += DeltaY);

    DrawFullWindow2(Window);

    if (Window == (window)All->FirstScreen->FocusW)
	UpdateCursor();
}

byte ExecScrollFocusWindow(void) {
    screen Screen;
    dat DWidth, DHeight;
    window Window;
    uldat Attrib, State, Scroll;
    dat XWidth, YWidth;
    dat DeltaX, DeltaY;
    
    if ((All->State & STATE_ANY) != STATE_SCROLL)
	return FALSE;
    
    if (!(Screen = All->FirstScreen) || !(Window = (window)Screen->FocusW) ||
	!IS_WINDOW(Window))
	return FALSE;
    
    Attrib=Window->Attrib;
    State=Window->State;
    DeltaX=DeltaY=(num)0;
    
    if (Attrib & WINDOW_X_BAR && State & X_BAR_SELECT)
	DeltaX=1;
    else if (Attrib & WINDOW_Y_BAR && State & Y_BAR_SELECT)
	DeltaY=1;
    else
	return FALSE;
    
    DWidth=All->DisplayWidth;
    DHeight=All->DisplayHeight;
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;
    
    XWidth=Min2(XWidth, DWidth);
    YWidth=Min2(YWidth, DHeight);
    Scroll=State & SCROLL_ANY_SELECT;
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
	
    if ((widget)Window == Screen->FirstW)
	ScrollFirstWindow(DeltaX, DeltaY, TRUE);
    else
	ScrollWindow(Window, DeltaX, DeltaY);
    return TRUE;
}

void HideMenu(byte on_off) {
    screen Screen=All->FirstScreen;
    
    if (on_off) {
	if (Screen->YLimit == 0) {
	    if (Screen->YLogic > MINDAT) {
		Screen->YLogic--;
		Screen->YLimit--;
		DrawArea2(Screen, (widget)0, (widget)0, 0, 0, MAXDAT, 0, FALSE);
		UpdateCursor();
	    } else
		ResizeFirstScreen(-1);
	}
    } else {
	if (Screen->YLimit == -1) {
	    if (Screen->YLogic < MAXDAT) {
		Screen->YLogic++;
		Screen->YLimit++;
		Act(DrawMenu,Screen)(Screen, 0, MAXDAT);
		UpdateCursor();
	    } else
		ResizeFirstScreen(1);
	}
    }
}

void ChangeMenuFirstScreen(menuitem NewItem, byte ByMouse, byte Flag) {
    screen Screen;
    menu Menu, _Menu;
    menuitem CurrItem;
    window NewWin, CurrWin;
    
    Screen=All->FirstScreen;
    Menu=Act(FindMenu,Screen)(Screen);
    
    if (Menu) {

	if (All->SetUp->Flags & SETUP_HIDEMENU && Flag==ACTIVATE_MENU_FLAG)
	    HideMenu(FALSE);
	
	CurrWin = (window)Screen->FocusW;
	if (CurrWin && !IS_WINDOW(CurrWin))
	    CurrWin = (window)0;
	
	
	if (Flag == ACTIVATE_MENU_FLAG)
	    CurrItem = (menuitem)0;
	else
	    CurrItem = Act(GetSelectItem,Menu)(Menu);

	if (CurrItem == NewItem && Flag == KEEP_ACTIVE_MENU_FLAG)
	    /* this can happen... on an empty menu with common items enabled */
	    return;
    
	/*
	 * WARNING:
	 * UnMapWindow() calls us if unmapping a menu window or a menu owner
	 * while the menu is active (All->State & STATE_ANY) == STATE_MENU
	 * 
	 * when shutting down the menu, disable (All->State & STATE_ANY) == STATE_MENU
	 * _BEFORE_ calling UnMap() or we will enter an infinite loop
	 */
	All->State =
	    (ByMouse && Flag != DISABLE_MENU_FLAG ? STATE_FL_BYMOUSE : (byte)0) |
	    (Flag != DISABLE_MENU_FLAG ? STATE_MENU : STATE_DEFAULT);
	
	if (Flag!=DISABLE_MENU_FLAG) {
	    
	    if (Flag==ACTIVATE_MENU_FLAG) {
		Screen->MenuWindow = CurrWin; /* so that it keeps `active' borders */
		Screen->FocusW = (widget)0;
	    }
	    
	    if (NewItem) {
		_Menu = NewItem->Menu; /* may either be Menu or All->CommonMenu */
		NewWin = NewItem->Window;
		
		/* take into account Screen->XLogic,YLogic scrolling */
		NewWin->Up = Screen->YLogic + 1;
		NewWin->Left = Screen->XLogic + NewItem->Left;
		
		if (Menu != _Menu && Menu->LastI)
		    /* adjust common menu NewWin->Left to the Item position in this menu */
		    NewWin->Left += Menu->LastI->Left + Menu->LastI->NameLen;
	    
		Act(SetSelectItem,Menu)(Menu, NewItem);
		
		if (ByMouse)
		    NewWin->CurY = MAXLDAT;
		else if (NewWin->CurY == MAXLDAT)
		    NewWin->CurY = (ldat)0;
	    
		if (NewItem->FlagActive)
		    NewWin->Flags &= ~WINDOWFL_DISABLED;
		else
		    NewWin->Flags |= WINDOWFL_DISABLED;
		
		Act(MapTopReal,NewWin)(NewWin, Screen);
	    } else
		Act(SetSelectItem,Menu)(Menu, (menuitem)0);
	}
	if (Flag!=ACTIVATE_MENU_FLAG) {
	    if (Flag==DISABLE_MENU_FLAG) {
		if ((NewWin = Screen->MenuWindow)) {
		    Act(KbdFocus,NewWin)(NewWin);
		    Screen->MenuWindow = (window)0;
		} else
		    Do(KbdFocus,Window)((window)0);
	    }
	    if (CurrItem && CurrWin && (CurrWin->Flags & WINDOWFL_MENU))
		Act(UnMap,CurrWin)(CurrWin);
	    UpdateCursor();
	}
	if (All->SetUp->Flags & SETUP_HIDEMENU && Flag==DISABLE_MENU_FLAG)
	    HideMenu(TRUE);
    }
    Act(DrawMenu,Screen)(Screen, 0, MAXDAT);
}

void UnFocusWidget(widget W) {
    if (W && W->Parent == (widget)All->FirstScreen && W == All->FirstScreen->FocusW) {
	if (IS_WINDOW(W)) {
	    Act(KbdFocus,W)((widget)0);
	    DrawBorderWindow((window)W, BORDER_ANY);
	    Act(DrawMenu,(screen)W->Parent)((screen)W->Parent, 0, MAXDAT);
	    UpdateCursor();
	} else
	    All->FirstScreen->FocusW = (widget)0;
    }
}

void RollUpWindow(window W, byte on_off) {
    if (W && !(W->Flags & WINDOWFL_BORDERLESS)) {
	/*
	 * you cannot roll-up borderless windows...
	 * without a top border you cannot collapse them
	 * to their top border :/
	 */
	if (on_off && !(W->Attrib & WINDOW_ROLLED_UP)) {
	    W->Attrib |= WINDOW_ROLLED_UP;
	    ReDrawRolledUpAreaWindow(W, FALSE);
	} else if (!on_off && (W->Attrib & WINDOW_ROLLED_UP)) {
	    W->Attrib &= ~WINDOW_ROLLED_UP;
	    DrawAreaWindow2(W);
	}
	if (W->Parent == (widget)All->FirstScreen)
	    UpdateCursor();
    }
}

void MakeFirstWindow(window W, byte alsoFocus) {
    screen Screen;
    
    if (W && (Screen = (screen)W->Parent) && IS_SCREEN(Screen)) {
	
	if (Screen->FirstW != (widget)W) {
	    MoveFirst(W, (widget)Screen, (widget)W);
	    DrawAreaWindow2(W);
	}
	if (Screen == All->FirstScreen) {
	    if (alsoFocus)
		Act(Focus,W)(W);
	    UpdateCursor();
	}

	if (Screen->FnHookWindow)
	    Screen->FnHookWindow(Screen->HookWindow);
    }
}

void MakeLastWindow(window W, byte alsoUnFocus) {
    screen Screen;
    window _W;
    
    if (W && (Screen = (screen)W->Parent) && IS_SCREEN(Screen)) {
	
	if (Screen->LastW != (widget)W) {
	    MoveLast(W, (widget)Screen, (widget)W);
	    DrawAreaWindow2(W);
	}
	if (Screen == All->FirstScreen) {
	    if (alsoUnFocus) {
		_W = (window)Screen->FirstW;
		if (_W && IS_WINDOW(_W) && _W != W)
		    Act(Focus,_W)(_W);
		else
		    Do(Focus,Window)((window)0);
	    } else
		UpdateCursor();
	}
	
	if (Screen->FnHookWindow)
	    Screen->FnHookWindow(Screen->HookWindow);
    }
}





void SendMsgGadget(gadget G) {
    msg Msg;
    event_gadget *Event;
    if (G->Code && !(G->Flags & GADGETFL_DISABLED)) {
	if ((Msg=Do(Create,Msg)(FnMsg, MSG_WIDGET_GADGET, sizeof(event_gadget)))) {
	    Event = &Msg->Event.EventGadget;
	    Event->W = G->Parent;
	    Event->Code = G->Code;
	    Event->Flags = G->Flags;
	    SendMsg(G->Owner, Msg);
	}
    }
}

static void realUnPressGadget(gadget G) {
    G->Flags &= ~GADGETFL_PRESSED;
    if (G->Group && G->Group->SelectG == G)
	G->Group->SelectG = (gadget)0;
    if ((widget)G == All->FirstScreen->FirstW)
	DrawWidget((widget)G, 0, 0, MAXDAT, MAXDAT, FALSE);
    else
	DrawAreaWidget((widget)G);
}

static void realPressGadget(gadget G) {
    G->Flags |= GADGETFL_PRESSED;
    if (G->Group)
	G->Group->SelectG = G;
    if ((widget)G == All->FirstScreen->FirstW)
	DrawWidget((widget)G, 0, 0, MAXDAT, MAXDAT, FALSE);
    else
	DrawAreaWidget((widget)G);
}


void PressGadget(gadget G) {
    if (!(G->Flags & GADGETFL_DISABLED)) {
	/* honour groups */
	if (G->Group && G->Group->SelectG && G->Group->SelectG != G)
	    UnPressGadget(G->Group->SelectG, TRUE);
    
	realPressGadget(G);
	if (G->Flags & GADGETFL_TOGGLE)
	    SendMsgGadget(G);
    }
}

void UnPressGadget(gadget G, byte maySendMsgIfNotToggle) {
    if (!(G->Flags & GADGETFL_DISABLED)) {
	realUnPressGadget(G);
	if (maySendMsgIfNotToggle || (G->Flags & GADGETFL_TOGGLE))
	    SendMsgGadget(G);
    }
}

/* Left < 0 means right-align leaving (-Left-1) margin */
/* Up   < 0 means down-align  leaving (-Up-1)   margin */
void WriteTextsGadget(gadget G, byte bitmap, dat TW, dat TH, CONST byte *Text, dat L, dat U) {
    dat GW = G->XWidth, GH = G->YWidth;
    dat TL = 0, TU = 0, W, H;
#if TW_SIZEOFHWFONT != 1
    dat _W;
#endif
    hwfont *GT;
    CONST byte *TT;
    byte i;
    
    if (G->Flags & GADGETFL_BUTTON) {
	GW--; GH--;
    }

    if (!G_USE(G, USETEXT) || L >= GW || U >= GH)
	return;
    
    if (L < 0) {
	L += GW - TW + 1;
	if (L < 0) {
	    TL = -L;
	    L = 0;
	}
    }
    if (U < 0) {
	U += GH - TH + 1;
	if (U < 0) {
	    TU = -U;
	    U = 0;
	}
    }
    W = Min2(TW - TL, GW - L);
    H = Min2(TH - TU, GH - U);

    if (G->Flags & GADGETFL_BUTTON) {
	GW++; GH++;
    }
    
    if (W > 0) {
	for (i = 0; i < 4; i++, bitmap>>=1) {
	    if ((bitmap & 1) && G->USE.T.Text[i]) {
		GT = G->USE.T.Text[i] + L + U * GW;
		if (Text) {
		    TT = Text + TL + TU * TW;
		    /* update the specified part, do not touch the rest */
		    while (H-- > 0) {
#if TW_SIZEOFHWFONT == 1
			CopyMem(TT, GT, W);
			GT += GW;
			TT += TW;
#else
			_W = W;
			while (_W-- > 0) {
# ifdef CONF__UNICODE
			    *GT++ = Tutf_IBM437_to_UTF_16[*TT++];
# else
			    *GT++ = *TT++;
# endif
			}
			GT += GW - W;
			TT += TW - W;
#endif
		    }
		} else {
		    /* clear the specified part of gadget contents */
		    while (H-- > 0) {
#if TW_SIZEOFHWFONT == 1
			WriteMem(GT, ' ', W);
			GT += GW;
#else
			_W = W;
			while (_W-- > 0)
			    *GT++ = ' ';
			GT += GW - W;
#endif
		    }
		}
	    }
	}
	DrawAreaWidget((widget)G);
    }
}


/* Left < 0 means right-align leaving (-Left-1) margin */
/* Up   < 0 means down-align  leaving (-Up-1)   margin */
void WriteHWFontsGadget(gadget G, byte bitmap, dat TW, dat TH, CONST hwfont *HWFont, dat L, dat U) {
#if TW_SIZEOFHWFONT == 1
    Act(WriteTexts,G)(G,bitmap,TW,TH,HWFont,L,U);
#else
    dat GW = G->XWidth, GH = G->YWidth;
    dat TL = 0, TU = 0, W, H;
    dat _W;
    hwfont *GT;
    CONST hwfont *TT;
    byte i;
    
    if (G->Flags & GADGETFL_BUTTON) {
	GW--; GH--;
    }

    if (!G_USE(G, USETEXT) || L >= GW || U >= GH)
	return;
    
    if (L < 0) {
	L += GW - TW + 1;
	if (L < 0) {
	    TL = -L;
	    L = 0;
	}
    }
    if (U < 0) {
	U += GH - TH + 1;
	if (U < 0) {
	    TU = -U;
	    U = 0;
	}
    }
    W = Min2(TW - TL, GW - L);
    H = Min2(TH - TU, GH - U);

    if (G->Flags & GADGETFL_BUTTON) {
	GW++; GH++;
    }
    
    if (W > 0) {
	for (i = 0; i < 4; i++, bitmap>>=1) {
	    if ((bitmap & 1) && G->USE.T.Text[i]) {
		GT = G->USE.T.Text[i] + L + U * GW;
		if (HWFont) {
		    TT = HWFont + TL + TU * TW;
		    /* update the specified part, do not touch the rest */
		    while (H-- > 0) {
			_W = W;
			while (_W-- > 0) {
# ifdef CONF__UNICODE   
			    *GT++ = Tutf_IBM437_to_UTF_16[*TT++];
# else
			    *GT++ = *TT++;
# endif
			}
			GT += GW - W;
			TT += TW - W;
		    }
		} else {
		    /* clear the specified part of gadget contents */
		    while (H-- > 0) {
			_W = W;
			while (_W-- > 0)
			    *GT++ = ' ';
			GT += GW - W;
		    }
		}
	    }
	}
	DrawAreaWidget((widget)G);
    }
#endif
}


#if 0
void SetNewFont(void) {
    struct REGPACK regs;
    byte FontHeight;
    font *RamFont;
    dat i;
    
    FontHeight=HEIGHT_FONT;
    if (FontHeight==(byte)8)
	RamFont=NewFont8;
    else if (FontHeight==(byte)14)
	RamFont=NewFont14;
    else if (FontHeight==(byte)16)
	RamFont=NewFont16;
    else
	return;
    
    for (i=(dat)0; i<(dat)256 && RamFont[i].AsciiCode; i++) {
	regs.r_ax=0x1100;
	regs.r_bx=(FontHeight<<8) | 0x0000;
	regs.r_cx=0x0001;
	regs.r_dx=RamFont[i].AsciiCode;
	regs.r_es=(dat)FP_SEG((void *)RamFont[i].Bitmap);
	regs.r_bp=(dat)FP_OFF((void *)RamFont[i].Bitmap);
	intr(0x10, &regs);
    }
    All->SetUp->Flags |= NEW_FONT;
    
    /*Alcune schede richiedono anche : */
    
    regs.r_ax=0x1110;
    regs.r_bx=(FontHeight<<8) | 0x0000;
    regs.r_cx=0x0001;
    regs.r_dx=RamFont[0].AsciiCode;
    regs.r_es=(dat)FP_SEG((void *)RamFont[0].Bitmap);
    regs.r_bp=(dat)FP_OFF((void *)RamFont[0].Bitmap);
    intr(0x10, &regs);
}

void GetPalette(void) {
    struct REGPACK regs;
    dat i;
    palette *Palette;
    
    Palette=All->Palette;
    
    for (i=(dat)0; i<(dat)16; i++) {
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
    dat i;
    
    regs.r_ax=0x1012;
    regs.r_bx=0;
    regs.r_cx=16;
    regs.r_es=(dat)FP_SEG((void *)All->Palette);
    regs.r_dx=(dat)FP_OFF((void *)All->Palette);
    intr(0x10, &regs);
    
    for (i=(dat)0; i<(dat)16; i++) {
	regs.r_ax=0x1000;
	regs.r_bx=i<<8 | i;
	intr(0x10, &regs);
    }
}
#endif /* 0 */

