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

#include <sys/stat.h>
#include <signal.h>

#include "autoconf.h"

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ifdef HAVE_TERMIOS_H
# include <termios.h>
#else
# ifdef HAVE_TERMIO_H
#  include <termio.h>
# endif
#endif

#include "twin.h"
#include "data.h"
#include "methods.h"
#include "draw.h"
#include "printk.h"
#include "resize.h"
#include "util.h"

#include "hw.h"

#include <Tw/Tw.h>
#include <Tw/Twstat.h>

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
#endif

/***************/

#ifdef CONF__UNICODE
extern hwattr extra_POS_INSIDE;
#else
# define extra_POS_INSIDE 0
#endif


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
		       (All->SetUp->Flags & SETUP_CURSOR_ALWAYS))) {

	    W = (widget)Window;
	    
	    HasBorder = !(Window->Flags & WINDOWFL_BORDERLESS);
		
	    CurX = Window->CurX - Window->XLogic + HasBorder;
	    CurY = Window->CurY - Window->YLogic + HasBorder;
	    
	    if (CurX >= 0 && CurX < (Window->XWidth - HasBorder) &&
		CurY >= 0 && CurY < (Window->YWidth - HasBorder) &&
		InitDrawCtx(W, (dat)CurX, (dat)CurY, (dat)CurX, (dat)CurY, FALSE, &D) &&
		((Window == (window)Screen->FirstW && !Window->FirstW) ||
#if 1
		 /* widgets and gadgets cannot contain cursor, but they can obscure it */
		 W == RecursiveFindWidgetAt((widget)Screen, (dat)D.X1, (dat)D.Y1 - Screen->YLimit)
#else
		 Window == WindowParent(RecursiveFindWidgetAt
					((widget)Screen, (dat)D.X1, (dat)D.Y1 - Screen->YLimit))
#endif
		 )) {
		
		MoveToXY((dat)D.X1, (dat)D.Y1);
		if ((type = Window->CursorType) == NOCURSOR && All->SetUp->Flags & SETUP_CURSOR_ALWAYS)
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
    hwattr *NewCont, *saveNewCont, *OldCont, *max, h;
    ldat count, common, left;
    ttydata *Data = Window->USE.C.TtyData;
    dat x = Window->XWidth, y = Window->YWidth + Data->ScrollBack;
    
    if (!(Window->Flags & WINDOWFL_BORDERLESS))
	x -= 2, y -= 2;

    h = HWATTR(Window->ColText, ' ') | extra_POS_INSIDE;

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
		    *NewCont++ = h;
	    }
	    FreeMem(Window->USE.C.Contents);
	}
	
	left = (saveNewCont + x*y) - NewCont;
	while (left--)
	    *NewCont++ = h;
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

byte RowWriteAscii(window Window, ldat Len, CONST byte *Text) {
    row CurrRow;
    byte CONST * _Text;
    byte FlagNewRows, ModeInsert;
#if TW_SIZEOFHWFONT != 1
    hwfont CONST * to_UTF_16;
    ldat i;
#endif
    ldat x, y, max, RowLen;
    
    if (!Window || (Len && !Text) || !W_USE(Window, USEROWS))
	return FALSE;
    
    x=Window->CurX;
    y=Window->CurY;
    max=Window->HLogic;
    CurrRow=Window->USE.R.LastRow;
    ModeInsert=Window->Flags & WINDOWFL_ROWS_INSERT;
    
    if (Window->State & WINDOW_ANYSEL)
	ClearHilight(Window);
    
    while (Len) {
	if (max<=y || (max==y+1 && (*Text=='\n' || *Text=='\r'))) {
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
	while (RowLen < Len && *_Text!='\n' && *_Text!='\r')
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
	
	if (Len && (*Text=='\n' || *Text=='\r')) {
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


#if TW_SIZEOFHWFONT == 1
byte RowWriteHWFont(window Window, ldat Len, CONST hwfont *Text) {
    return RowWriteAscii(Window, Len, (CONST byte *)Text);
}
#else
byte RowWriteHWFont(window Window, ldat Len, CONST hwfont *Text) {
    row CurrRow;
    CONST hwfont * _Text;
    byte FlagNewRows, ModeInsert;
    ldat i;
    ldat x, y, max, RowLen;
    
    if (!Window || !Len || !W_USE(Window, USEROWS))
	return FALSE;
    
    x=Window->CurX;
    y=Window->CurY;
    max=Window->HLogic;
    CurrRow=Window->USE.R.LastRow;
    ModeInsert=Window->Flags & WINDOWFL_ROWS_INSERT;
    
    if (Window->State & WINDOW_ANYSEL)
	ClearHilight(Window);
    
    while (Len) {
	if (max<=y || (max==y+1 && (*Text=='\n' || *Text=='\r'))) {
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
	while (RowLen < Len && *_Text!='\n' && *_Text!='\r')
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

	    CopyMem(Text, CurrRow->Text+x, sizeof(hwfont)*RowLen);
	    if (CurrRow->Len<x)
		for (i = CurrRow->Len; i < x; i++)
		    CurrRow->Text[i] = (hwfont)' ';

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
	
	if (Len && (*Text=='\n' || *Text=='\r')) {
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
#endif /* CONF__UNICODE */



	    

void ExposeWidget2(widget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, CONST byte *Text, CONST hwfont *Font, CONST hwattr *Attr) {
    if (w_USE(W, USEEXPOSE)) {
	if (Text || Font || Attr) {
	    if (Text) {
		W->USE.E.E.Text = Text;
		W->USE.E.Flags = WIDGET_USEEXPOSE_TEXT;
	    } else if (Font) {
		W->USE.E.E.HWFont = Font;
		W->USE.E.Flags = WIDGET_USEEXPOSE_HWFONT;
	    } else {
		W->USE.E.E.HWAttr = Attr;
		W->USE.E.Flags = WIDGET_USEEXPOSE_HWATTR;
	    }
	    W->USE.E.X1 = Left;
	    W->USE.E.X2 = Left + XWidth - 1;
	    W->USE.E.Y1 = Up;
	    W->USE.E.Y2 = Up + YWidth - 1;
	    W->USE.E.Pitch = Pitch;
	} else {
	    W->USE.E.E.Text = (byte *)1;
	    W->USE.E.Flags = WIDGET_USEEXPOSE_TEXT;
	    W->USE.E.X1 = W->USE.E.Y1 =
		W->USE.E.X2 = W->USE.E.Y2 = -1;
	    W->USE.E.Pitch = 0;
	}
	
	DrawLogicWidget(W, Left, Up, Left + XWidth - 1, Up + YWidth - 1);

	WriteMem(&W->USE.E.E, '\0', sizeof(W->USE.E.E));
	W->USE.E.Flags = 0;
    }
}


void ExposeWindow2(window W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, CONST byte *Text, CONST hwfont *Font, CONST hwattr *Attr) {
    ldat CurX, CurY;
    
    if (W_USE(W, USEEXPOSE)) {
	ExposeWidget2((widget)W, XWidth, YWidth, Left, Up, Pitch, Text, Font, Attr);
	return;
    }

    if (!W_USE(W, USECONTENTS) && !W_USE(W, USEROWS))
	return;

    /* handle negative (Left,Up) by clipping */
    if (Left < 0) {
	XWidth += Left;
	if (Text) Text -= Left;
	if (Font) Font -= Left;
	if (Attr) Attr -= Left;
	Left = 0;
    }
    if (Up < 0) {
	YWidth += Up;
	if (Text) Text -= (ldat)Up * Pitch;
	if (Font) Font -= (ldat)Up * Pitch;
	if (Attr) Attr -= (ldat)Up * Pitch;
	Up = 0;
    }
    
    if (W_USE(W, USECONTENTS)) {
	/* clip to window size */
	CurX = W->USE.C.TtyData->SizeX;
	CurY = W->USE.C.TtyData->SizeY;
	if (Left >= CurY || Up >= CurY)
	    return;
	if ((ldat)XWidth + Left > CurX)
	    XWidth = CurX - Left;
	if ((ldat)YWidth + Up > CurY)
	    YWidth = CurY - Up;
    }
    if (XWidth <= 0 || YWidth <= 0)
	return;
    
    if (Text) {
	void (*WriteString)(window, ldat, CONST byte *);
	if (W_USE(W, USECONTENTS)) {
	    WriteString = W->Fn->TtyWriteString;
	} else
	    WriteString = (void (*)(window, ldat, CONST byte *))W->Fn->RowWriteString;
	
	CurX = W->CurX;
	CurY = W->CurY;
	for (; YWidth; YWidth--, Up++, Text += Pitch) {
	    Act(GotoXY,W)(W, Left, Up);
	    WriteString(W, XWidth, Text);
	}
	Act(GotoXY,W)(W, CurX, CurY);
	
    } else if (Font) {
	void (*WriteHWFont)(window, ldat, CONST hwfont *);
	if (W_USE(W, USECONTENTS))
	    WriteHWFont = W->Fn->TtyWriteHWFont;
	else
	    WriteHWFont = (void (*)(window, ldat, CONST hwfont *))W->Fn->RowWriteHWFont;
	
	CurX = W->CurX;
	CurY = W->CurY;
	for (; YWidth; YWidth--, Up++, Font += Pitch) {
	    Act(GotoXY,W)(W, Left, Up);
	    WriteHWFont(W, XWidth, Font);
	}
	Act(GotoXY,W)(W, CurX, CurY);
	
    } else if (Attr) {
	void (*WriteHWAttr)(window, dat, dat, ldat, CONST hwattr *);
	if (W_USE(W, USECONTENTS))
	    WriteHWAttr = W->Fn->TtyWriteHWAttr;
	else
	    WriteHWAttr = (void (*)(window, dat, dat, ldat, CONST hwattr *))W->Fn->RowWriteHWAttr;
			   
	for (; YWidth; YWidth--, Up++, Attr += Pitch)
	    WriteHWAttr(W, Left, Up, XWidth, Attr);
    }
}

/***************/


void ResizeWidget(widget W, dat X, dat Y) {
    dat mX, mY;

    if (W) {
	X = Max2(1, X);
	Y = Max2(1, Y);
	mX = Max2(X, W->XWidth);
	mY = Max2(Y, W->YWidth);
	W->XWidth = X;
	W->YWidth = Y;

	if (!(W->Flags & WIDGETFL_NOTVISIBLE))
	    DrawAreaWidget(W);
    }
}

void ResizeGadget(gadget G, dat X, dat Y) {
    if (G) {
	if (G_USE(G, USETEXT)) {
	    /* FIXME: finish this */
	} else {
	    ResizeWidget((widget)G, X, Y);
	}
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
    byte Shade, DeltaXShade, DeltaYShade, visible;
    
    if (!Window || (!i && !j)) /* || !(Window->Attrib & WINDOW_RESIZE) */
	return;

    visible = !(Window->Flags & WIDGETFL_NOTVISIBLE);
    
    if (visible && (widget)Window == All->FirstScreen->FirstW) {
	ResizeRelFirstWindow(i, j);
	return;
    }
    
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;
    MinXWidth=Window->MinXWidth;
    MinYWidth=Window->MinYWidth;
    MaxXWidth=Window->MaxXWidth;
    MaxYWidth=Window->MaxYWidth;

    if (visible && (Parent=Window->Parent) && IS_SCREEN(Parent)) {
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
	if (visible && Parent && IS_SCREEN(Parent)) {
	    Up = (dat)Max2(Up, (ldat)YLimit);
	    DrawArea2((screen)Parent, (widget)0, (widget)0,
		      (dat)Left, (dat)Up, (dat)Rgt, (dat)Dwn, FALSE);
	    if (Shade)
		DrawShadeWindow(Window, 0, 0, MAXDAT, MAXDAT, FALSE);
	}
    
	if (visible && Window == (window)All->FirstScreen->FocusW)
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
void ScrollFirstWindowArea(dat X1, dat Y1, dat X2, dat Y2, ldat DeltaX, ldat DeltaY) {
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

    if (DeltaX >= XWidth || -DeltaX >= XWidth || DeltaY >= YWidth || -DeltaY >= YWidth) {
	DrawWidget((widget)Window, (dat)0, (dat)0, MAXDAT, MAXDAT, FALSE);
	return;
    }

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

void ScrollFirstWindow(ldat DeltaX, ldat DeltaY, byte byXYLogic) {
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
	
	if (DeltaX>0 && XLogic>=MAXLDAT-DeltaX)
	    DeltaX=MAXLDAT-XLogic-1;
	else if (DeltaX<0 && XLogic<-DeltaX)
	    DeltaX=-XLogic;
	if (!W_USE(Window, USEROWS)) {
	    /*
	     * WARNING: Window->USE.C.Contents and other methods
	     * may be unable to handle out-of-bound rows
	     */
	    if (DeltaY>0 && YLogic+YWidth-2+DeltaY>=Window->HLogic)
		DeltaY=Window->HLogic-YLogic-YWidth+2;
	    else if (DeltaY<0 && YLogic<-DeltaY)
		DeltaY=-YLogic;
	} else {
	    if (DeltaY>0 && YLogic+DeltaY>=Window->HLogic)
		DeltaY=Window->HLogic-YLogic-1;
	    else if (DeltaY<0 && YLogic<-DeltaY)
		DeltaY=-YLogic;
	}
	if (DeltaX)
	    Window->XLogic = (XLogic += DeltaX);
	if (DeltaY)
	    Window->YLogic = (YLogic += DeltaY);
	if (DeltaX || DeltaY)
	    DrawBorderWindow(Window, (DeltaX ? BORDER_DOWN : 0) | (DeltaY ? BORDER_RIGHT : 0));
    }

    ScrollFirstWindowArea(0, 0, XWidth-3, YWidth-3, -DeltaX, -DeltaY);
    
    if (byXYLogic && ContainsCursor((widget)Window))
	UpdateCursor();
}

void ScrollWindow(window Window, ldat DeltaX, ldat DeltaY) {
    ldat XLogic, YLogic;
    dat YWidth;
    
    if (!Window || !IS_WINDOW(Window) || (!DeltaX && !DeltaY))
	return;
    
    if (!(Window->Attrib & WINDOW_X_BAR))
	DeltaX = 0;

    if (!(Window->Attrib & WINDOW_Y_BAR))
	DeltaY = 0;

    if (!DeltaX && !DeltaY)
	return;

    if ((widget)Window == All->FirstScreen->FirstW) {
	ScrollFirstWindow(DeltaX, DeltaY, TRUE);
	return;
    }
    
    YWidth=Window->YWidth;
    XLogic=Window->XLogic;
    YLogic=Window->YLogic;
	
    if (DeltaX>0 && XLogic>=MAXLDAT-DeltaX)
	DeltaX=MAXLDAT-XLogic-1;
    else if (DeltaX<0 && XLogic<-DeltaX)
	DeltaX=-XLogic;
    if (!W_USE(Window, USEROWS)) {
	/*
	 * WARNING: Window->USE.C.Contents and other methods
	 * may be unable to handle out-of-bound rows
	 */
	if (DeltaY>0 && YLogic+YWidth-2+DeltaY>=Window->HLogic)
	    DeltaY=Window->HLogic-YLogic-YWidth+2;
	else if (DeltaY<0 && YLogic<-DeltaY)
	    DeltaY=-YLogic;
    } else {
	if (DeltaY>0 && YLogic+DeltaY>=Window->HLogic)
	    DeltaY=Window->HLogic-YLogic-1;
	else if (DeltaY<0 && YLogic<-DeltaY)
	    DeltaY=-YLogic;
    }
    if (DeltaX)
	Window->XLogic = (XLogic += DeltaX);
    if (DeltaY)
	Window->YLogic = (YLogic += DeltaY);

    DrawFullWindow2(Window);

    if (ContainsCursor((widget)Window))
	UpdateCursor();
}

void ScrollWidget(widget W, ldat DeltaX, ldat DeltaY) {
    ldat XLogic, YLogic;
    dat YWidth;

    if (!W || !(DeltaX || DeltaY))
	return;
    
    if (IS_WINDOW(W)) {
	ScrollWindow((window)W, DeltaX, DeltaY);
	return;
    }
    
    YWidth=W->YWidth;
    XLogic=W->XLogic;
    YLogic=W->YLogic;
	
    if (DeltaX>0 && XLogic>=MAXLDAT-DeltaX)
	DeltaX=MAXLDAT-XLogic-1;
    else if (DeltaX<0 && XLogic<-DeltaX)
	DeltaX=-XLogic;
    /*
     * WARNING: content methods may be
     * unable to handle out-of-bound areas
     */
    if (DeltaY>0 && YLogic+YWidth-2>=MAXLDAT-DeltaY)
	DeltaY=MAXLDAT-YLogic-YWidth+2;
    else if (DeltaY<0 && YLogic<-DeltaY)
	DeltaY=-YLogic;

    if (DeltaX)
	W->XLogic = (XLogic += DeltaX);
    if (DeltaY)
	W->YLogic = (YLogic += DeltaY);

    DrawAreaWidget(W);

    if (ContainsCursor(W))
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
    DeltaX=DeltaY=(sbyte)0;
    
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


/* menu traversing functions */

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

static void OpenSubMenuItem(menu M, menuitem Item, byte ByMouse) {
    window P = (window)Item->Parent;
    window W = Item->Window;
    screen S = All->FirstScreen;
    ldat y = P->CurY;
    
    P->CurY = Item->WCurY;
    if (y != MAXLDAT)
	DrawLogicWidget((widget)P, 0, y, MAXLDAT, y);
    if ((y = P->CurY) == MAXLDAT)
	(void)Act(FindRowByCode,P)(P, Item->Code, &P->CurY);
    if ((y = P->CurY) != MAXLDAT)
	DrawLogicWidget((widget)P, 0, y, MAXLDAT, y);
    
    if (W) {
	if (!W->Parent) {
	    W->Left = P->Left + P->XWidth - S->XLogic;
	    W->Up = P->Up + P->CurY - P->YLogic - S->YLogic + 1;
	}
	if (ByMouse)
	    W->CurY = MAXLDAT;
	else if (W->CurY == MAXLDAT)
	    W->CurY = 0;
	Act(MapTopReal,W)(W, S);
    }
    if ((widget)P != S->FocusW)
	Act(Focus,P)(P);
}

static void OpenTopMenuItem(menu M, menuitem Item, byte ByMouse) {
    menu _M = (menu)Item->Parent; /* may either be M or All->CommonMenu */
    window W = Item->Window;
    
    if (!W->Parent) {
	W->Up = 1;
	W->Left = Item->Left;
    
	if (M != _M && M->LastI)
	    /* adjust common menu W->Left to the Item position in this menu */
	    W->Left += M->LastI->Left + M->LastI->Len;
    }
    
    Act(SetSelectedItem,M)(M, Item);
    
    if (ByMouse)
	W->CurY = MAXLDAT;
    else if (W->CurY == MAXLDAT)
	W->CurY = (ldat)0;
    
    if (Item->Flags & ROW_ACTIVE)
	W->Flags &= ~WINDOWFL_DISABLED;
    else
	W->Flags |= WINDOWFL_DISABLED;
    
    Act(MapTopReal,W)(W, All->FirstScreen);
    
    if (W->CurY != MAXLDAT && (Item = (menuitem)Act(FindRow,W)(W, W->CurY)))
	OpenSubMenuItem(M, Item, ByMouse);
}

static void OpenMenuItem(menu M, menuitem Item, byte ByMouse) {
    if (Item) {
	obj O = Item->Parent;
	if (O && IS_WINDOW(O))
	    OpenSubMenuItem(M, Item, ByMouse);
	else
	    OpenTopMenuItem(M, Item, ByMouse);
    } else
	Act(SetSelectedItem,M)(M, (menuitem)0);
}

/* this activates the menu bar */
static void OpenMenu(menuitem Item, byte ByMouse) {
    screen S = All->FirstScreen;
    widget W = S->FocusW;
    menu M = Act(FindMenu,S)(S);
    
    if ((All->State & STATE_ANY) == STATE_DEFAULT) {
	
	All->State = STATE_MENU | (ByMouse ? STATE_FL_BYMOUSE : 0);

	if (All->SetUp->Flags & SETUP_MENU_HIDE)
	    HideMenu(FALSE);

	if (!S->MenuWindow && W) {
	    S->MenuWindow = (window)W; /* so that it keeps `active' borders */
	    S->FocusW = (widget)0;
	}
    }
    OpenMenuItem(M, Item, ByMouse);
}


/*
 * up one level; return new selected item;
 * do NOT use to close the menu, CloseMenu() does that
 */
static menuitem CloseMenuItem(menu M, menuitem Item, byte ByMouse) {
    window P = (window)Item->Parent, W = Item->Window;
    
    if (W)
	Act(UnMap,W)(W);

    if (P && IS_WINDOW(P)) {
	if (ByMouse) {
	    ldat y = P->CurY;
	    P->CurY = MAXLDAT;
	
	    if (y != MAXLDAT)
		DrawLogicWidget((widget)P, 0, y, MAXLDAT, y);
	}
	Item = P->MenuItem;
	if (Item) {
	    W = (window)Item->Parent;
	    if (W && IS_WINDOW(W))
		Act(Focus,W)(W);
	    else
		Act(Focus,P)(P);
	}
	return Item;
    } else {
	Item = (menuitem)0;
	Act(SetSelectedItem,M)(M, Item);
	return Item;
    }
}

static dat DepthOfMenuItem(menuitem I) {
    window W;
    dat d = 0;
    
    while (I && (W = (window)I->Parent) && IS_WINDOW(W)) {
	I = W->MenuItem;
	d++;
    }
    return d;
}

/* this traverses the menu bar as needed */
static void TraverseMenu(menu M, menuitem OldItem, dat Odepth, menuitem NewItem, dat Ndepth, byte ByMouse) {
    while (Odepth > Ndepth && OldItem) {
	Odepth--;
	OldItem = CloseMenuItem(M, OldItem, ByMouse);
    }
    /* paranoia */
    Odepth = DepthOfMenuItem(OldItem);
    
    if (Ndepth == Odepth) {
	if (OldItem != NewItem)
	    CloseMenuItem(M, OldItem, ByMouse);
	OpenMenuItem(M, NewItem, ByMouse);
    } else if (Ndepth == Odepth + 1) {
	OpenMenuItem(M, NewItem, ByMouse);
    } else
	printk("twin: internal error: unsupported menu traversing.\n");
}

/* close the menu bar */
void CloseMenu(void) {
    screen S = All->FirstScreen;
    menu M = Act(FindMenu,S)(S);
    menuitem Item;
    window W;
    
    if (M) {
	if ((W = S->MenuWindow)) {
	    Act(KbdFocus,W)(W);
	    S->MenuWindow = (window)0;
	} else
	    Do(KbdFocus,Window)((window)0);
	
	/* close whole currently open menu tree */
	Item = Act(GetSelectedItem,M)(M);
	while (Item && IS_MENUITEM(Item) && (W = (window)Item->Window) && IS_WINDOW(W)) {
	    Item = (menuitem)Act(FindRow,W)(W,W->CurY);
	    Act(UnMap,W)(W);
	}
    }
    All->State = STATE_DEFAULT;
    if (All->SetUp->Flags & SETUP_MENU_HIDE)
	HideMenu(TRUE);
    else
	Act(DrawMenu, S)(S, 0, MAXDAT);
}

/*
 * exported interface to open and interact with the menu.
 * do NOT use to close the menu, CloseMenu() does that
 */
void SetMenuState(menuitem Item, byte ByMouse) {
    screen S = All->FirstScreen;
    menu M = Act(FindMenu,S)(S);
    menuitem OldItem = (menuitem)0;
    dat Odepth = 0;
    
    if (M && (Item || ByMouse)) {
	if ((All->State & STATE_ANY) != STATE_DEFAULT)
	    OldItem = Act(RecursiveGetSelectedItem,M)(M, &Odepth);
	if (!OldItem)
	    OpenMenu(Item, ByMouse);
	else if (OldItem != Item)
	    TraverseMenu(M, OldItem, Odepth, Item, DepthOfMenuItem(Item), ByMouse);
	UpdateCursor();
    }
}


/* ---------------- */

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

/* ---------------- */


void SetVisibleWidget(widget W, byte on_off) {
    byte visible;
    if (W) {
	on_off = !!on_off;
	visible = !(W->Flags & WIDGETFL_NOTVISIBLE);
	
	if (on_off != visible) {
	    W->Flags ^= WIDGETFL_NOTVISIBLE;
	    if (IS_WINDOW(W))
		DrawAreaWindow2((window)W);
	    else
		DrawAreaWidget(W);
	}
    }
}

void RaiseWidget(widget W, byte alsoFocus) {
    screen Screen;
    
    if (W && (Screen = (screen)W->Parent) && IS_SCREEN(Screen)) {
	
	if (Screen->FirstW != W) {
	    MoveFirst(W, (widget)Screen, W);
	    if (IS_WINDOW(W))
		DrawAreaWindow2((window)W);
	    else
		DrawAreaWidget(W);
	}
	if (Screen == All->FirstScreen) {
	    if (alsoFocus)
		Act(Focus,W)(W);
	    UpdateCursor();
	}

	if (Screen->FnHookW)
	    Screen->FnHookW(Screen->HookW);
    }
}

void LowerWidget(widget W, byte alsoUnFocus) {
    screen Screen;
    widget _W;
    
    if (W && (Screen = (screen)W->Parent) && IS_SCREEN(Screen)) {
	
	if (Screen->LastW != W) {
	    MoveLast(W, (widget)Screen, W);
	    if (IS_WINDOW(W))
		DrawAreaWindow2((window)W);
	    else
		DrawAreaWidget(W);
	}
	if (Screen == All->FirstScreen) {
	    if (alsoUnFocus) {
		_W = Screen->FirstW;
		if (_W && IS_WINDOW(_W) && _W != W)
		    Act(Focus,_W)(_W);
		else
		    Do(Focus,Window)((window)0);
	    } else
		UpdateCursor();
	}
	
	if (Screen->FnHookW)
	    Screen->FnHookW(Screen->HookW);
    }
}


void RestackWidgets(widget W, uldat N, CONST widget *arrayW) {
    widget FW, CW;
    byte need_redraw = FALSE;
    
    if (W && N && arrayW) {
	for (FW = (widget)0; N; N--, arrayW++) {
	    /*
	     * Allow only children that really have the given parent.
	     * Also deny WINDOFL_MENU windows
	     */
	    if ((CW = *arrayW) && CW->Parent == W && !(CW->Flags & WINDOWFL_MENU)) {
		if (FW && CW != FW->Next) {
		    /* restack after arrayW[0] */
		    Remove(CW);
		    Act(Insert,CW)(CW, W, FW, FW->Next);
		    need_redraw = TRUE;
		}
		FW = CW;
	    }
	}
	/* FIXME: this is gross */
	if (need_redraw)
	    DrawAreaWidget(W);
    }
}



/* ---------------- */


void RestackRows(obj O, uldat N, CONST row *arrayR) {
    row FR, CR;
    byte need_redraw = FALSE;
    
    if (O && (IS_MENU(O) || IS_WINDOW(O)) && N && arrayR) {
	for (FR = (row)0; N; N--, arrayR++) {
	    /*
	     * Allow only children that really have the given parent.
	     */
	    if ((CR = *arrayR) && (obj)CR->Window == O) {
		if (FR && CR != FR->Next) {
		    /* restack after arrayR[0] */
		    Remove(CR);
		    Act(Insert,CR)(CR, (window)O, FR, FR->Next);
		    need_redraw = TRUE;
		}
		FR = CR;
	    }
	}
	/* FIXME: this is gross */
	if (need_redraw) {
	    if (IS_MENU(O))
		SyncMenu((menu)O);
	    else
		DrawAreaWidget((widget)O);
	}
    }
}



/* ---------------- */


void SendMsgGadget(gadget G) {
    msg Msg;
    event_gadget *Event;
    if (G->Code && !(G->Flags & GADGETFL_DISABLED)) {
	if ((Msg=Do(Create,Msg)(FnMsg, MSG_WIDGET_GADGET, 0))) {
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
			    *GT++ = Tutf_CP437_to_UTF_16[*TT++];
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
			    *GT++ = Tutf_CP437_to_UTF_16[*TT++];
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

void SyncMenu(menu Menu) {
    menuitem I, PrevI = (menuitem)0;
    screen Screen;
    
    if (Menu) {
	for (I = Menu->FirstI; I; I = I->Next) {
	    if (PrevI)
		I->Left = PrevI->Left + PrevI->Len;
	    else
		I->Left = 1;
	    PrevI = I;
	}
	for (Screen = All->FirstScreen; Screen; Screen = Screen->Next) {
	    if (Act(FindMenu,Screen)(Screen) == Menu)
		Act(DrawMenu,Screen)(Screen, 0, MAXDAT);
	}
    }
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

