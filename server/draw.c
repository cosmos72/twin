/*
 *  draw.c  --  functions to draw screens, menus, windows, gadgets, etc.
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

#include "main.h"

#include "data.h"
#include "methods.h"
#include "hw.h"
#include "util.h"
#include "draw.h"

INLINE hwcol DoShadowColor(hwcol Color, byte Fg, byte Bg) {
    return
	(
	 Bg ? (Color & COL(0,MAXCOL)) > COL(0,HIGH|BLACK) ? COL(0,HIGH|BLACK) : COL(0,BLACK) :
	 Fg ? Color & COL(0,WHITE) : Color & COL(0,MAXCOL)
	 )
	|
	(
	 Fg ? (Color & COL(MAXCOL,0)) > COL(HIGH|BLACK,0) ? COL(WHITE,0) : COL(HIGH|BLACK,0) : Color & COL(MAXCOL,0)
	 );
}

void SearchFontMenuItem(menu *Menu, menuitem *MenuItem, dat i, byte Select, byte *PtrFont, byte *PtrColor) {
    byte Color, ShortCutFound;
    
    ShortCutFound = (udat)i==MenuItem->Left+MenuItem->ShortCut;
    if (MenuItem->FlagActive) {
	if (ShortCutFound) {
	    if (Select) 
		Color=Menu->ColSelShtCut;
	    else
		Color=Menu->ColShtCut;
	} else if (Select)
	    Color=Menu->ColSelect;
	else
	    Color=Menu->ColItem;
    } else if (Select)
	Color=Menu->ColSelectDisabled;
    else
	Color=Menu->ColDisabled;
    
    *PtrFont=MenuItem->Name[(udat)i-MenuItem->Left];
    *PtrColor=Color;
}

void SearchFontInfo(menu *Menu, dat i, byte Select, byte *PtrFont, byte *PtrColor) {
    row *Info;
    byte Color;
    
    Info=Menu->Info;
    if (Select)
	Color = Info->Flags & ROW_ACTIVE ? Menu->ColSelect : Menu->ColSelectDisabled;
    else if (!(Info->Flags & ROW_ACTIVE))
	Color = Menu->ColDisabled;
    else if (Menu->FlagDefColInfo)
	Color = Menu->ColItem;
    else
	Color = Info->ColText[i];
    
    *PtrFont=Info->Text[i];
    *PtrColor=Color;
}

#define XNumLogicMax 1024
#define YNumLogicMax (Window->MaxNumRow)
#define XBarSize     (Window->XWidth-(udat)5)
#define YBarSize     (Window->YWidth-(udat)4)


udat TabStart(window *Window, num isX) {
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

udat TabLen(window *Window, num isX) {
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

byte SearchFontBorderWin(window *Window, udat u, udat v, byte Border, byte MovWin, byte *PtrFont, byte *PtrColor) {
    byte Font, Found = (byte)0, FlagNewFont;
    hwcol Color;
    byte LastRow, LastColumn;
    uldat Attrib;
    byte Close, Resize, PlaceBack, BarX, BarY;
    num Back_Fwd;
    udat k, rev_u, rev_v;
    udat XWidth, YWidth;
    
    if (!Window)
	return Found;
    
    FlagNewFont=!!(All->SetUp->Flags & SETUP_NEW_FONT);
    Attrib=Window->Attrib;
    Close=!!(Attrib & WINDOW_CLOSE);
    Resize=!!(Attrib & WINDOW_RESIZE);
    PlaceBack=!(Attrib & WINDOW_MENU);
    BarX=!!(Attrib & WINDOW_X_BAR);
    BarY=!!(Attrib & WINDOW_Y_BAR);
    XWidth=Window->XWidth;
    YWidth=Window->YWidth;
    rev_u=XWidth-u-(udat)1;
    rev_v=YWidth-v-(udat)1;
    LastRow= v ? rev_v ? (byte)1 : (byte)2 : (byte)0;
    LastColumn= u ? rev_u ? (byte)1 : (byte)2 : (byte)0;
    
    if (!LastRow)
	if (Close && u<(udat)2) {
	    Font=GadgetClose[FlagNewFont][u];
	    Found=POS_GADGET_CLOSE;
	}
    else if (PlaceBack && rev_u<(udat)2) {
	Font=GadgetBack[FlagNewFont][(udat)1-rev_u];
	Found=POS_GADGET_BACK;
    }
    else if (((ldat)u*2+1 >= (ldat)XWidth - (ldat)Window->LenTitle) && ((ldat)u*2+1 < (ldat)XWidth + (ldat)Window->LenTitle)) {
	k=(udat)((ldat)u-((ldat)XWidth - (ldat)Window->LenTitle)/2);
	Font=Window->Title[k];
	Found=POS_TITLE;
    }
    else {
	Font=StdBorder[FlagNewFont][Border][0][LastColumn];
	Found=(byte)0;
    }
    else if (!u)
	Font=StdBorder[FlagNewFont][Border][LastRow][0];
    else if (LastRow==(byte)2)
	if (rev_u<(udat)2)
	if (Resize) {
	    Font=GadgetResize[FlagNewFont][(udat)1-rev_u];
	    Found=POS_GADGET_RESIZE;
	}
    else
	Font=StdBorder[FlagNewFont][Border][2][(udat)2-rev_u];
    else if (!BarX)
	Font=StdBorder[FlagNewFont][Border][2][1];
    else if (rev_u<(udat)4) {
	Font=ScrollBarX[FlagNewFont][(udat)4-rev_u];
	Found= rev_u==(udat)3 ? POS_ARROW_BACK : POS_ARROW_FWD;
    }
    else if (!(Back_Fwd=IsTabPosition(Window, u-(udat)1, TRUE))) {
	Font=TabX[FlagNewFont];
	Found=POS_TAB;
    }
    else {
	Font=ScrollBarX[FlagNewFont][0];
	Found=Back_Fwd>(num)0 ? POS_BAR_FWD : POS_BAR_BACK;
    }
    else
	if (!LastRow || !BarY)
	Font=StdBorder[FlagNewFont][Border][LastRow][2];
    else if (rev_v<(udat)3) {
	Font=ScrollBarY[FlagNewFont][(udat)3-rev_v];
	Found= rev_v==(udat)2 ? POS_ARROW_BACK : POS_ARROW_FWD;
    }
    else if (!(Back_Fwd=IsTabPosition(Window, v-(udat)1, FALSE))) {
	Font=TabY[FlagNewFont];
	Found=POS_TAB;
    }
    else {
	Font=ScrollBarY[FlagNewFont][0];
	Found=Back_Fwd>(num)0 ? POS_BAR_FWD : POS_BAR_BACK;
    }
    
    if (MovWin && (!Found || (Found==POS_TITLE && !Window->ColTitle))) {
	if (LastRow==(byte)1)
	    Color=COL( COLFG(Window->ColGadgets), COLBG(Window->ColBorder) );
	else
	    Color=Window->ColGadgets;
    } else if (MovWin && (Found==POS_GADGET_CLOSE || Found==POS_GADGET_BACK || Found==POS_GADGET_RESIZE))
	    Color=COL( COLBG(Window->ColGadgets), COLFG(Window->ColGadgets) );
    else
	switch (Found) {
	  case POS_GADGET_CLOSE:
	  case POS_GADGET_BACK:
	    if (((Found==POS_GADGET_CLOSE && Attrib & GADGET_CLOSE_SELECT) ||
		 (Found==POS_GADGET_BACK && Attrib & GADGET_BACK_SELECT)) &&
		Attrib & GADGET_PRESSED)
		Color=COL( COLBG(Window->ColGadgets), COLFG(Window->ColGadgets) );
	    else
		Color=Window->ColGadgets;
	    break;
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
	    if(Window->ColTitle)
		Color=Window->ColTitle[k];
	    else
		Color=Window->ColBorder;
	    break;
	  default:
	    Color=Window->ColBorder;
	    break;
	}
    
    *PtrColor=Color;
    *PtrFont=Font;
    return Found;
}

void DrawDesktop(screen *Screen, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded) {
    hwattr *Attr, attr = HWATTR(COL(WHITE,BLACK),' ');
    dat ScreenWidth, ScreenHeight;
    udat YLimit = 0;
    
    if (Xstart>Xend || Ystart>Yend)
	return;
    
    if (Screen) {
	Attr=Screen->Bg;
	attr=*Attr;
	YLimit=Screen->YLimit;
	ScreenWidth=Screen->ScreenWidth;
	ScreenHeight=Screen->ScreenHeight;
    } else {
	ScreenWidth=All->FirstScreen->ScreenWidth;
	ScreenHeight=All->FirstScreen->ScreenHeight;
    }
    
    if (Xstart>=ScreenWidth || Ystart>=ScreenHeight || Xend<(dat)0 || Yend<(Screen ? (dat)YLimit : (dat)0))
	return;
    
    Xstart=Max2(Xstart, (dat)0);
    Ystart = Max2(Ystart, (dat)YLimit);
    Xend=Min2(Xend, ScreenWidth-(dat)1);
    Yend=Min2(Yend, ScreenHeight-(dat)1);

    if (Screen && (Screen->BgWidth > 1 || Screen->BgHeight > 1)) {
	/* use tiling... easier than other solutions */
	ldat X, Y, x, max = Screen->BgWidth * Screen->BgHeight;
	ldat y = ((ldat)Ystart + Screen->Up - Screen->YLimit) % Screen->BgHeight;
	if (y < 0) y += Screen->BgHeight;
	y *= Screen->BgWidth;
	
	DirtyVideo(Xstart, Ystart, Xend, Yend);
	
	for (; Ystart <= Yend; Ystart++, y+=Screen->BgWidth) {
	    Y = Ystart * ScreenWidth;

	    if (y >= max) y -= max;
	    x = ((ldat)Xstart + Screen->Left) % Screen->BgWidth;
	    if (x < 0) x += Screen->BgWidth;
		
	    if (Shaded) {
		for (X = Xstart; X <= Xend; X++, x++) {
		    if (x >= Screen->BgWidth) x -= Screen->BgWidth;
		    
		    attr=Attr[x + y];
		    Video[X + Y] = HWATTR(DoShadowColor(HWCOL(attr), Shaded, Shaded), HWFONT(attr));
		}
	    } else {
		for (X = Xstart; X <= Xend; X++, x++) {
		    if (x >= Screen->BgWidth) x -= Screen->BgWidth;
		    Video[X + Y] = Attr[x + y];
		}
	    }
	}
    } else {
	if (Shaded)
	    attr=HWATTR(DoShadowColor(HWCOL(attr), Shaded, Shaded), HWFONT(attr));
	FillVideo(Xstart, Ystart, Xend, Yend, attr);
    }
}

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

INLINE void _DrawWindow_(area_win_parent *AreaWinParent, window *Window, gadget *FirstGadget, gadget *OnlyThisGadget, byte NoGadgets, uldat XLogic, uldat YLogic, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded, byte *lError) {
    area_win *AreaWin;
    if (!(AreaWin=Do(Create,AreaWin)(FnAreaWin, AreaWinParent, Window, FirstGadget, OnlyThisGadget, NoGadgets, XLogic, YLogic, Xstart, Ystart, Xend, Yend))) {
	DrawDesktop(((screen *)0), Xstart, Ystart, Xend, Yend, Shaded);
	*lError=TRUE;
    }
}

void DrawWindow(window *Window, gadget *FirstGadget, gadget *OnlyThisGadget, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded) {
    area_win_parent AreaWinParent = { (area_win *)0, (area_win *)0 };
    area_win *AreaWin = (area_win *)0;
    gadget *GadgetNext;
    ldat shLeft, shUp, shRgt, shDwn;
    ldat NWinDiMenu;
    byte Border, WinActive, MovWin, Absent, NoGadgets;
    byte Font, Select, RowDisabled;
    hwcol Color;
    byte GadgetFound=FALSE, lError=FALSE, FirstCicle=TRUE;
    screen *Screen;
    dat ScreenWidth, ScreenHeight, Offset;
    udat YLimit, i, j, k, l, u, v, i_max, j_max, i_min, j_min;
    row *CurrRow;
    udat *Contents, *CurrCont;
    uldat XLogic, YLogic, Row, PosInRow;
    byte *Text, **GadgetContents;
    hwcol *ColText;
    
    if (!Window || !(Screen=Window->Screen) || Xstart>Xend || Ystart>Yend)
	return;
    
    INIT;
    
    if (shLeft>=(ldat)ScreenWidth || shUp>=(ldat)ScreenHeight || shRgt<(ldat)0 || shDwn<(ldat)YLimit)
	return;

    if (Xstart>=ScreenWidth || Ystart>=ScreenHeight || Xend<(dat)0 || Yend<(dat)0)
	return;
    
    Xstart=Max2(Xstart, (dat)0);
    Ystart=Max2(Ystart, (dat)YLimit);
    Xend=Min2(Xend,ScreenWidth-(dat)1);
    Yend=Min2(Yend,ScreenHeight-(dat)1);
    
    if (shLeft>(ldat)Xend || shUp>(ldat)Yend || shRgt<(ldat)Xstart || shDwn<(ldat)Ystart)
	return;
    
    Xstart=(dat)Max2((ldat)Xstart, shLeft);
    Ystart=(dat)Max2((ldat)Ystart, shUp);
    Xend=(dat)Min2((ldat)Xend, shRgt);
    Yend=(dat)Min2((ldat)Yend, shDwn);
    
    WinActive= Window==All->FirstScreen->FocusWindow || Window==All->FirstScreen->MenuWindow;
    MovWin = WinActive && All->FlagsMove & GLMOVE_ANY_1stWIN;
    Border = Window->Attrib & WINDOW_MENU || !WinActive;
    if ((ldat)Ystart==shUp) {
	if (!OnlyThisGadget) {
	    j=Ystart*ScreenWidth;
	    for (i=Xstart; i<=Xend; i++) {
		u=(udat)((ldat)i-shLeft);
		SearchFontBorderWin(Window, u, (udat)0, Border, MovWin, &Font, &Color);
		Color=DoShadowColor(Color, Shaded || !WinActive, Shaded);
		Video[i+j]=HWATTR(Color, Font);
	    }
	    DirtyVideo(Xstart, Ystart, Xend, Ystart);
	}
	Ystart++;
	if (Ystart > Yend)
	    return;
    }
    
    if ((ldat)Yend==shDwn) {
	if (!OnlyThisGadget) {
	    v=(udat)((ldat)Yend-shUp);
	    j=Yend*ScreenWidth;
	    for (i=Xstart; i<=Xend; i++) {
		u=(udat)((ldat)i-shLeft);
		SearchFontBorderWin(Window, u, v, Border, MovWin, &Font, &Color);
		Color=DoShadowColor(Color, Shaded || !WinActive, Shaded);
		Video[i+j]=HWATTR(Color, Font);
	    }
	    DirtyVideo(Xstart, Yend, Xend, Yend);
	}
	Yend--;
	if (Ystart > Yend)
	    return;
    }
    
    if ((ldat)Xstart==shLeft) {
	if (!OnlyThisGadget) {
	    for (j=Ystart; j<=Yend; j++) {
		v=(udat)((ldat)j-shUp);
		SearchFontBorderWin(Window, (udat)0, v, Border, MovWin, &Font, &Color);
		Color=DoShadowColor(Color, Shaded || !WinActive, Shaded);
		Video[Xstart+j*ScreenWidth]=HWATTR(Color, Font);
	    }
	    DirtyVideo(Xstart, Ystart, Xstart, Yend);
	}
	Xstart++;
	if (Xstart > Xend)
	    return;
    }
    
    if ((ldat)Xend==shRgt) {
	if (!OnlyThisGadget) {
	    u=(udat)((ldat)Xend-shLeft);
	    for (j=Ystart; j<=Yend; j++) {
		v=(udat)((ldat)j-shUp);
		SearchFontBorderWin(Window, u, v, Border, MovWin, &Font, &Color);
		Color=DoShadowColor(Color, Shaded || !WinActive, Shaded);
		Video[Xend+j*ScreenWidth]=HWATTR(Color, Font);
	    }
	    DirtyVideo(Xend, Ystart, Xend, Yend);
	}
	Xend--;
	if (Xstart > Xend)
	    return;
    }
    
    if (Xstart>Xend || Ystart>Yend)
	return;
    
    YLogic = Window->YLogic;
    XLogic = Window->XLogic;
    
    if (!(Window->Flags & WINFL_USEANY)
	/* we are using normal Rows */
	&& (Window->NumRowOne>YLogic
	    || (YLogic>=(uldat)ScreenHeight
		&& Window->NumRowOne<=YLogic-(uldat)ScreenHeight))) {
	Window->RowOne=Act(SearchRow,Window)(Window, YLogic);
	Window->NumRowOne=YLogic;
    }
    
    FirstGadget=Window->FirstGadget;
    XLogic = (uldat)((ldat)Xstart-shLeft) - (uldat)1 + XLogic;
    YLogic = (uldat)((ldat)Ystart-shUp) - (uldat)1 + YLogic;
    
    do {
	if (FirstCicle)
	    NoGadgets=FirstCicle=FALSE;
	else {
	    FirstGadget=AreaWin->FirstGadget;
	    OnlyThisGadget=AreaWin->OnlyThisGadget;
	    NoGadgets=AreaWin->NoGadgets;
	    XLogic=AreaWin->XLogic;
	    YLogic=AreaWin->YLogic;
	    Xstart=AreaWin->Xstart;
	    Ystart=AreaWin->Ystart;
	    Xend=AreaWin->Xend;
	    Yend=AreaWin->Yend;
	    Delete(AreaWin);
	}
	
	i=(udat)XLogic+Xend-Xstart;
	j=(udat)YLogic+Yend-Ystart;
	
	GadgetFound=FALSE;
	GadgetNext=(gadget *)0;
	if (!NoGadgets && FirstGadget && XLogic<=MAXUDAT && YLogic<=MAXUDAT) {
	    while (FirstGadget && !GadgetFound) {
		u=FirstGadget->Left;
		v=FirstGadget->Up;
		k=FirstGadget->XWidth-(udat)1;
		l=FirstGadget->YWidth-(udat)1;
		GadgetNext=FirstGadget->Next;
		if ((u>i || v>j || k+u<(udat)XLogic || l+v<(udat)YLogic) ||
		    (OnlyThisGadget && OnlyThisGadget!=FirstGadget)) {
		    FirstGadget=GadgetNext;
		}
		else
		    GadgetFound=TRUE;
	    }
	}
	if (!GadgetNext)
	    NoGadgets=TRUE;
	
	
	if (GadgetFound && (!OnlyThisGadget || FirstGadget==OnlyThisGadget)) {
	    Select=(FirstGadget==Window->GadgetSelect && FirstGadget->Flags & GADGET_PRESSED);
	    RowDisabled=(FirstGadget->Flags & GADGET_DISABLED);
	    Absent=FirstGadget->Flags & GADGET_USE_DEFCOL;
	    
	    GadgetContents=FirstGadget->Contents;
	    
	    Text = Select
		? RowDisabled && GadgetContents[3]
		? GadgetContents[3]
		: GadgetContents[1]
		? GadgetContents[1]
		: GadgetContents[0]
		: RowDisabled && GadgetContents[2]
		? GadgetContents[2]
		: GadgetContents[0];
	    
	    if (Absent)
		Color = Select
		? RowDisabled
		? FirstGadget->ColSelectDisabled
		: FirstGadget->ColSelect
		: RowDisabled
		? FirstGadget->ColDisabled
		: FirstGadget->ColText;
	    else
		ColText = Select
		? RowDisabled && GadgetContents[7]
		? GadgetContents[7]
		: GadgetContents[5]
		? GadgetContents[5]
		: GadgetContents[4]
		: RowDisabled && GadgetContents[6]
		? GadgetContents[6]
		: GadgetContents[4];
	    
	    i_min = (uldat)u<XLogic ? (udat)(XLogic-(uldat)u) : (udat)0;
	    i_max = u+k<=i ? k : i-u;
	    
	    j_min = (uldat)v<YLogic ? (udat)(YLogic-(uldat)v) : (udat)0;
	    j_max = v+l<=j ? l : j-v;
	    
	    Offset=(dat)(Xstart+u-XLogic+(Ystart+v-YLogic)*ScreenWidth);
	    
	    k++;
	    
	    for (j=j_min; j<=j_max; j++) {
		for (i=i_min; i<=i_max; i++) {
		    Font=Text[j*k+i];
		    if (!Absent)
			Color=ColText[j*k+i];
		    Video[i+j*ScreenWidth+Offset] =
			HWATTR(DoShadowColor(Color, Shaded, Shaded), Font);
		}
	    }
	    DirtyVideo(i_min+(Xstart+u-XLogic), j_min+(Ystart+v-YLogic),
			i_max+(Xstart+u-XLogic), j_max+(Ystart+v-YLogic));
	    k--;
	    
	    if ((uldat)v>YLogic) {
		Offset=v-(udat)YLogic;
		_DrawWindow_(&AreaWinParent, Window, GadgetNext, OnlyThisGadget, NoGadgets, XLogic, YLogic, Xstart, Ystart, Xend, Ystart+Offset-(dat)1, Shaded, &lError);
		YLogic+=Offset;
		Ystart+=Offset;
	    }
	    
	    if ((uldat)v+(uldat)l<YLogic+(uldat)Yend-(uldat)Ystart) {
		Offset=(dat)((uldat)v+(uldat)l-YLogic);
		_DrawWindow_(&AreaWinParent, Window, GadgetNext, OnlyThisGadget, NoGadgets, XLogic, YLogic+Offset+(dat)1, Xstart, Ystart+Offset+(dat)1, Xend, Yend, Shaded, &lError);
		Yend=Ystart+Offset;
	    }
	    
	    if ((uldat)u>XLogic) {
		Offset=u-(udat)XLogic;
		_DrawWindow_(&AreaWinParent, Window, GadgetNext, OnlyThisGadget, NoGadgets, XLogic, YLogic, Xstart, Ystart, Xstart+Offset-(dat)1, Yend, Shaded, &lError);
		XLogic+=Offset;
		Xstart+=Offset;
	    }
	    
	    if ((uldat)u+(uldat)l<XLogic+(uldat)Xend-(uldat)Xstart) {
		Offset=(dat)((uldat)u+(uldat)k-XLogic);
		_DrawWindow_(&AreaWinParent, Window, GadgetNext, OnlyThisGadget, NoGadgets, XLogic+Offset+(dat)1, YLogic, Xstart+Offset+(dat)1, Ystart, Xend, Yend, Shaded, &lError);
		Xend=Xstart+Offset;
	    }
	}
	else if (!OnlyThisGadget) {
	    if (Window->Flags & WINFL_USECONTENTS && (Contents = Window->Contents)) {
	        /*
		 * For xterm-like windows, Contents is a buffer of (x=NumRowOne) * (y=MaxNumRow)
		 * udat:s, in the same format as Video: HWATTR(Color, Ascii).
		 * Room in excess is used as scrollback.
		 * 
		 * To avoid frequent mem-mem copies, VGA-like splitline method is used:
		 * NumRowSplit is the first used line of Contents, and after the end
		 * of Contents we restart from zero.
		 * 
		 * MaxNumRow also has the usual meaning:
		 * number of total lines (visible + scrollback)
		 */
		
		if (Yend - Ystart + YLogic >= Window->MaxNumRow) {
		    /* the ->Contents buffer is smaller than the window size... pad with SPACEs */
		    dat Ynew = Ystart + Window->MaxNumRow;
		    if ((uldat)Ynew > YLogic + YLimit)
			Ynew -= (dat)YLogic;
		    else
			Ynew = YLimit;
		    if (Ynew < Ystart)
			Ynew = Ystart;
		    FillVideo(Xstart, Ynew, Xend, Yend, HWATTR(Window->ColText, ' '));
		    Yend = Ynew - 1;
		}
		
		if (Xend - Xstart + XLogic >= Window->NumRowOne) {
		    /* the ->Contents buffer is smaller than the window size... pad with SPACEs */
		    dat Xnew = Xstart + Window->NumRowOne;
		    if ((uldat)Xnew > XLogic)
			Xnew -= XLogic;
		    else
			Xnew = 0;
		    if (Xnew < Xstart)
			Xnew = Xstart;
		    FillVideo(Xnew, Ystart, Xend, Yend, HWATTR(Window->ColText, ' '));
		    Xend = Xnew - 1;
		}
		    
		if (Xstart <= Xend && Ystart <= Yend) {
		    Row = YLogic + Window->NumRowSplit; /* row number in Contents */
		    if (Row >= Window->MaxNumRow)
			Row -= Window->MaxNumRow;
		    CurrCont = Contents + XLogic + Row*Window->NumRowOne;
		
		    Row = Window->MaxNumRow - 1 - Row;	/* rows still readable */
		
		    if (!Shaded) {
			for (j=Ystart, u=YLogic; j<=Yend; j++, u++, Row--) {
			    if (!(Window->Attrib & WINDOW_DO_SEL) ||
				u < Window->YstSel || u > Window->YendSel) {
				
				CopyMem(CurrCont, &Video[j*ScreenWidth+Xstart],
					sizeof(hwattr)*(Xend-Xstart+1));
				
			    } else {
				for (i=Xstart, v=0; i<=Xend; i++, v++) {
				    Select = (Window->Attrib & WINDOW_DO_SEL) &&
					((u>Window->YstSel ||
					  (u==Window->YstSel && v+XLogic>=Window->XstSel)) &&
					 (u<Window->YendSel ||
					  (u==Window->YendSel && v+XLogic<=Window->XendSel)));
				    if (Select)
					Color = Window->ColSelect;
				    else
					Color = HWCOL(CurrCont[v]);
				    Video[i+j*ScreenWidth] = HWATTR(Color, HWFONT(CurrCont[v]));
				}
			    }
			    CurrCont += Window->NumRowOne;
			    if (!Row)
				CurrCont -= (Row = Window->MaxNumRow) * Window->NumRowOne;
			}
		    } else {
			for (j=Ystart, u=YLogic; j<=Yend; j++, u++, Row--) {
			    for (i=Xstart, v=0; i<=Xend; i++, v++) {
				
				Select = (Window->Attrib & WINDOW_DO_SEL) &&
				    ((u>Window->YstSel ||
				      (u==Window->YstSel && v+XLogic>=Window->XstSel)) &&
				     (u<Window->YendSel ||
				      (u==Window->YendSel && v+XLogic<=Window->XendSel)));
				
				if (Select)
				    Color = Window->ColSelect;
				else
				    Color = HWCOL(CurrCont[v]);
				Color=DoShadowColor(Color, Shaded, Shaded);
				
				Video[i+j*ScreenWidth] = HWATTR(Color, HWFONT(CurrCont[v]));
			    }
			    CurrCont += Window->NumRowOne;
			    if (!Row)
				CurrCont -= (Row = Window->MaxNumRow) * Window->NumRowOne;
			}
		    }
		    DirtyVideo(Xstart, Ystart, Xend, Yend);
		}
	    } else if (!(Window->Flags & WINFL_USEANY) && Window->FirstRow) {
		/*
		 * editor-like windows. no length limit in the rows and in XLogic.
		 */
		Row=YLogic;
		
		if (Row>=Window->MaxNumRow)
		    CurrRow = (row *)0;
		else if (Window->NumRowSplit && Window->RowSplit && Row==Window->NumRowSplit)
		    CurrRow = Window->RowSplit;
		else if (Window->NumRowSplit && Window->RowSplit && Window->RowSplit->Next && Row==Window->NumRowSplit+(uldat)1)
		    CurrRow = Window->RowSplit->Next;
		else if (Window->NumRowOne && Window->RowOne && Row==Window->NumRowOne)
		    CurrRow = Window->RowOne;
		else if (Row==(uldat)0)
		    CurrRow = Window->FirstRow;
		else if (Row+(uldat)1==Window->MaxNumRow)
		    CurrRow = Window->LastRow;
		else
		    CurrRow = Act(SearchRow,Window)(Window, Row);
		
		for (j=Ystart, v=(udat)0; j<=Yend; j++, Row++, v++) {
		    
		    RowDisabled = (Window->Flags & WINFL_DISABLED)
			|| (CurrRow && !(CurrRow->Flags & (ROW_ACTIVE|ROW_IGNORE)));
		    
		    if (CurrRow) {
			Text=CurrRow->Text;
			ColText=CurrRow->ColText;
		    }
		    
		    PosInRow=XLogic;
		    if (CurrRow && PosInRow>=CurrRow->Gap)
			PosInRow+=CurrRow->LenGap;
		    
		    k=j*ScreenWidth;
		    for (i=Xstart, u=(udat)0; i<=Xend; i++, PosInRow++, u++) {
			
			if ((Absent=(!CurrRow || PosInRow>=CurrRow->Len)))
			    Font=' ';
			else
			    Font=*((byte *)Text+PosInRow);
			
			if (Window->Flags & WINFL_SEL_ROWCURR)
			    Select = Row==Window->CurY;
			else {
			    Select = (Window->Attrib & WINDOW_DO_SEL) &&
				((Row>Window->YstSel ||
				  (Row==Window->YstSel && PosInRow>=Window->XstSel)) &&
				 (Row<Window->YendSel ||
				  (Row==Window->YendSel && PosInRow<=Window->XendSel)));
			}
			if (CurrRow)
			    Select &= !(CurrRow->Flags & ROW_IGNORE);
			
			if (Select)
			    Color = RowDisabled ? Window->ColSelectDisabled : Window->ColSelect;
			else if (RowDisabled)
			    Color = Window->ColDisabled;
			else if ((Window->Flags & WINFL_USE_DEFCOL) || Absent)
			    Color = Window->ColText;
			else
			    Color = *((byte *)ColText+PosInRow);
			
			Color=DoShadowColor(Color, Shaded, Shaded);
			Video[i+k]=HWATTR(Color, Font);
		    }
		    if (CurrRow) {
			Window->RowSplit = CurrRow;
			Window->NumRowSplit = Row;
			CurrRow=CurrRow->Next;
		    }
		}
		DirtyVideo(Xstart, Ystart, Xend, Yend);
	    } else {
		/* either an unknown window type or just one of the above, but empty */
		Color = Window->ColText;
		Color=DoShadowColor(Color, Shaded, Shaded);
		FillVideo(Xstart, Ystart, Xend, Yend, HWATTR(Color, ' '));
	    }
	}
    } while ((AreaWin=AreaWinParent.FirstAreaWin));
    
    if (lError)
	Error(lError);
}
    
void DrawMenuBar(screen *Screen, dat Xstart, dat Xend) {
    screen *fScreen;
    menu *Menu;
    menuitem *CurrMenuItem;
    dat ScreenWidth, ScreenHeight, i, j;
    byte Color, Font, Select;
    
    if (!Screen || !Screen->All || Xstart>Xend)
	return;
    
    j=(dat)Screen->YLimit;
    ScreenWidth=Screen->ScreenWidth;
    ScreenHeight=Screen->ScreenHeight;
    
    if (!j-- || j>=ScreenHeight || Xstart>=ScreenWidth || Xend<(dat)0 || Xstart > Xend)
	return;
    
    for (fScreen = All->FirstScreen; fScreen && fScreen != Screen; fScreen = fScreen->Next) {
	if (fScreen->YLimit <= j)
	    return;
    }
    if (fScreen != Screen)
	return;
	
    Menu=Act(SearchMenu,Screen)(Screen);
    
    Xstart=Max2(Xstart, (dat)0);
    Xend=Min2(Xend, ScreenWidth-(dat)1);
    
    if (Menu)
	for (i=Xstart; i<=Xend; i++) {
	    if (ScreenWidth-i<=(dat)2) {
		Color = All->FlagsMove & GLMOVE_1stSCREEN ? Menu->ColSelShtCut : Menu->ColShtCut;
		if (Screen->Attrib & GADGET_BACK_SELECT && Screen->Attrib & GADGET_PRESSED)
		    Color = COL( COLBG(Color), COLFG(Color) );
		Font = GadgetBack[!!(All->SetUp->Flags & SETUP_NEW_FONT)][(udat)2-(udat)(ScreenWidth-(dat)i)];
	    }
	    else if (ScreenWidth-i<=(dat)3+lenTWDisplay) {
		Color = All->FlagsMove & GLMOVE_1stSCREEN ? Menu->ColSelShtCut : Menu->ColShtCut;
		Font = TWDisplay[(udat)3 + lenTWDisplay - (udat)(ScreenWidth - i)];
		if (!Font) Font = ' ';
	    }
	    else if (All->FlagsMove & GLMOVE_1stMENU && (CurrMenuItem=Act(SearchMenuItem,Menu)(Menu, i))) {
		Select = (All->FlagsMove & GLMOVE_1stSCREEN || Menu->MenuItemSelect==CurrMenuItem);
		SearchFontMenuItem(Menu, CurrMenuItem, i, Select, &Font, &Color);
	    }
	    else if (!(All->FlagsMove & GLMOVE_1stMENU) && SearchInfo(Menu, i)) {
		Select = All->FlagsMove & GLMOVE_1stSCREEN;
		SearchFontInfo(Menu, i, Select, &Font, &Color);
	    }
	    else {
		Color = (All->FlagsMove & GLMOVE_1stSCREEN) ? Menu->ColSelect : Menu->ColItem;
		Font = ' ';
	    }
	    if (Screen != All->FirstScreen)
		Color = Menu->ColDisabled;
	    Video[i+j*ScreenWidth]=HWATTR(Color, Font);
	}
    else {
	i = Min2(Xend,ScreenWidth-2);
	if (Xstart < i)
	    FillVideo(Xstart, j, i-1, j, HWATTR(COL(BLACK,WHITE),' '));
	if (i <= Xend) {
	    Color = COL(RED,WHITE);
	    if (Screen->Attrib & GADGET_BACK_SELECT && Screen->Attrib & GADGET_PRESSED)
		Color = COL( COLBG(Color), COLFG(Color) );
	    for (; i <= Xend; i++) {
		Font = GadgetBack[!!(All->SetUp->Flags & SETUP_NEW_FONT)][(udat)2-(udat)(ScreenWidth-(dat)i)];
		Video[i+j*ScreenWidth]=HWATTR(Color, Font);
	    }
	}
    }
    
    DirtyVideo(Xstart, j, Xend, j);    
}


#define ONLY_SHADE (byte)0x02
#define ORIZ  (byte)0x01
#define VERT (byte)0x02

INLINE void _DrawArea_(area_parent *AreaParent, screen *FirstScreen, window *FirstWindow, window *OnlyThisWindow, gadget *FirstGadget, gadget *OnlyThisGadget, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded, byte *lError) {
    area *Area;
    if (!(Area=Do(Create,Area)(FnArea, AreaParent, FirstScreen, FirstWindow, OnlyThisWindow, FirstGadget, OnlyThisGadget, Xstart, Ystart, Xend, Yend, Shaded))) {
	DrawDesktop(((screen *)0), Xstart, Ystart, Xend, Yend, Shaded);
	*lError=TRUE;
    }
}

void DrawArea(screen *FirstScreen, window *FirstWindow, window *OnlyThisWindow, gadget *FirstGadget, gadget *OnlyThisGadget, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded) {
    area_parent AreaParent = { (area *)0, (area *)0 };
    area *Area;
    dat ScreenWidth, ScreenHeight;
    screen *CurrScreen;
    window *NextWin, *Window;
    setup *SetUp;
    byte WinFound, Shade, lError=FALSE, FirstCicle=TRUE;
    byte DeltaXShade, DeltaYShade;
    ldat shLeft = 0, shUp = 0, shRgt = 0, shDwn = 0;
    dat u;
    ldat temp_Orizz, temp_Vert;
    dat Left, Up, Rgt, Dwn;
    dat Left_O, Rgt_O, Up_O, Dwn_O, Rgt_1, Dwn_1;
    
    ScreenWidth = All->FirstScreen->ScreenWidth;
    ScreenHeight = All->FirstScreen->ScreenHeight;
    
    do {
	if (FirstCicle)
	    FirstCicle=FALSE;
	else {
	    FirstScreen=Area->FirstScreen;
	    FirstWindow=Area->FirstWindow;
	    OnlyThisWindow=Area->OnlyThisWindow;
	    Xstart=Area->Xstart;
	    Ystart=Area->Ystart;
	    Xend=Area->Xend;
	    Yend=Area->Yend;
	    Shaded=Area->Shaded;
	    Delete(Area);
	}
	
	if (Xstart>Xend || Ystart>Yend || Xstart>=ScreenWidth || Ystart>=ScreenHeight || Xend<(dat)0 || Yend<(dat)0)
	    continue;
	
	Xstart=Max2(Xstart, (dat)0);
	Ystart=Max2(Ystart, (dat)0);
	Xend=Min2(Xend,ScreenWidth-(dat)1);
	Yend=Min2(Yend,ScreenHeight-(dat)1);
	
	if (FirstWindow)
	    FirstScreen=FirstWindow->Screen;
	
	if (!FirstScreen)
	    FirstScreen=All->FirstScreen;
	
	u=(dat)FirstScreen->YLimit-(dat)1;
	CurrScreen=FirstScreen->Next;
	if (!OnlyThisWindow || (OnlyThisWindow && OnlyThisWindow->Screen!=FirstScreen)) {
	    if (Ystart<u) {
		if (CurrScreen)
		    _DrawArea_(&AreaParent, CurrScreen, (window *)0, OnlyThisWindow, FirstGadget, OnlyThisGadget, Xstart, Ystart, Xend, Min2(Yend, u-(dat)1), Shaded, &lError);
		else if (!OnlyThisWindow)
		    DrawDesktop((screen *)0, Xstart, Ystart, Xend, Yend, Shaded);
	    }
	    if (OnlyThisWindow)
		continue;
	}
	
	if (Yend<u)
	    continue;
	
	if (Ystart<u)
	    Ystart=u;
	
	if (!FirstWindow) {
	    FirstWindow=FirstScreen->FirstWindow;
	    if (Ystart==u && !OnlyThisWindow) {
		DrawMenuBar(FirstScreen, Xstart, Xend);
		if (++Ystart>Yend)
		    continue;
	    }
	}
	
	SetUp=All->SetUp;
	Shade=!!(SetUp->Flags & SETUP_DO_SHADE) && !Shaded;
	DeltaXShade=Shade ? SetUp->DeltaXShade : (byte)0;
	DeltaYShade=Shade ? SetUp->DeltaYShade : (byte)0;
	
	WinFound=FALSE;
	while (FirstWindow && !WinFound) {
	    shLeft=(ldat)FirstWindow->Left;
	    shUp = (ldat)FirstWindow->Up + (ldat)FirstScreen->YLimit;
	    if (!(FirstWindow->Attrib & WINDOW_MENU)) {
		shLeft -= (ldat)FirstScreen->Left;
		shUp   -= (ldat)FirstScreen->Up;
	    }
	    shRgt=shLeft+(ldat)FirstWindow->XWidth-(ldat)1;
	    shDwn=shUp+(ldat)FirstWindow->YWidth-(ldat)1;
	    
	    if (shLeft<=(ldat)Xend && shRgt>=(ldat)Xstart && shUp<=(ldat)Yend && shDwn>=(ldat)Ystart)
		WinFound=TRUE;
	    else if (Shade && shLeft+(ldat)DeltaXShade<=(ldat)Xend && shRgt+(ldat)DeltaXShade>=(ldat)Xstart &&
		     shUp+(ldat)DeltaYShade<=(ldat)Yend && shDwn+(ldat)DeltaYShade>=(ldat)Ystart)
		WinFound=ONLY_SHADE;
	    else
		FirstWindow=FirstWindow->Next;
	}
	
	if (!WinFound && !OnlyThisWindow) {
	    DrawDesktop(FirstScreen, Xstart, Ystart, Xend, Yend, Shaded);
	    continue;
	}
	
	if (WinFound && (!OnlyThisWindow || FirstWindow==OnlyThisWindow))
	    DrawWindow(FirstWindow, FirstGadget, OnlyThisGadget, Xstart, Ystart, Xend, Yend, Shaded);
	
	/* Draw thw window's shadow : */
	
	if (WinFound && Shade && OnlyThisWindow!=FirstWindow) {
	    if (!(shLeft+(ldat)DeltaXShade>(ldat)Xend || shUp+(ldat)DeltaYShade>(ldat)Yend || shRgt+(ldat)DeltaXShade<(ldat)Xstart || shDwn+(ldat)DeltaYShade<(ldat)Ystart)) {
		Shade=FALSE;
		if (shLeft+(ldat)DeltaXShade<=(ldat)Xend && shRgt>=(ldat)Xstart &&
		    shDwn+(ldat)DeltaYShade>=(ldat)Ystart && shDwn<Yend)
		    Shade|=ORIZ;
		
		if (shRgt<(ldat)Xend && shRgt+(ldat)DeltaXShade>=(ldat)Xstart &&
		    shUp+(ldat)DeltaYShade<=(ldat)Yend && shDwn+(ldat)DeltaYShade>=(ldat)Ystart)
		    Shade|=VERT;
		
		Left_O=(dat)Max2(shLeft+(ldat)DeltaXShade, (ldat)Xstart);
		Up_O=(dat)Max2(shUp+(ldat)DeltaYShade, (ldat)Ystart);
		Rgt_O=(dat)Min2(shRgt+(ldat)DeltaXShade, (ldat)Xend);
		Dwn_O=(dat)Min2(shDwn+(ldat)DeltaYShade, (ldat)Yend);
		
		Rgt_1=(dat)Max2(shRgt+(ldat)1, (ldat)Xstart);
		Dwn_1=(dat)Max2(shDwn+(ldat)1, (ldat)Ystart);
		
		if (shRgt>(ldat)Xend)
		    shRgt=(ldat)Xend;
		
		Window = FirstWindow ? FirstWindow->Next : FirstWindow;
		if (Shade & ORIZ) {
		    if (Window)
			_DrawArea_(&AreaParent, (screen *)0, Window, OnlyThisWindow, FirstGadget, OnlyThisGadget, Left_O, Dwn_1, (dat)shRgt, Dwn_O, TRUE, &lError);
		    else
			DrawDesktop(FirstScreen, Left_O, Dwn_1, (dat)shRgt, Dwn_O, TRUE);
		}
		if (Shade & VERT) {
		    if (Window)
			_DrawArea_(&AreaParent, (screen *)0, Window, OnlyThisWindow, FirstGadget, OnlyThisGadget, Rgt_1, Up_O, Rgt_O, Dwn_O, TRUE, &lError);
		    else
			DrawDesktop(FirstScreen, Rgt_1, Up_O, Rgt_O, Dwn_O, TRUE);
		}
	    }
	}
	
	NextWin=FirstWindow ? FirstWindow->Next : FirstWindow;
	
	if (OnlyThisWindow==FirstWindow || (OnlyThisWindow && !NextWin))
	    continue;
	
	
	/* Draw the visible area above the window : */
	
	if (shUp>(ldat)Ystart) {
	    Up=(dat)shUp;
	    if (NextWin)
		_DrawArea_(&AreaParent, (screen *)0, NextWin, OnlyThisWindow, FirstGadget, OnlyThisGadget, Xstart, Ystart, Xend, Up-(dat)1, Shaded, &lError);
	    else
		DrawDesktop(FirstScreen, Xstart, Ystart, Xend, Up-(dat)1, Shaded);
	}
	else
	    Up=Ystart;
	
	/* Draw the visible area below the window : */
	
	if (shDwn+(ldat)DeltaYShade<(ldat)Yend) {
	    Dwn=(dat)shDwn+(dat)DeltaYShade;
	    if (NextWin)
		_DrawArea_(&AreaParent, (screen *)0, NextWin, OnlyThisWindow, FirstGadget, OnlyThisGadget, Xstart, Dwn+(dat)1, Xend, Yend, Shaded, &lError);
	    else
		DrawDesktop(FirstScreen, Xstart, Dwn+(dat)1, Xend, Yend, Shaded);
	}
	else
	    Dwn=Yend;
	
	/* Draw the visible area right of the window : */
	
	if (shLeft>(ldat)Xstart) {
	    Left=(dat)shLeft;
	    if (NextWin)
		_DrawArea_(&AreaParent, (screen *)0, NextWin, OnlyThisWindow, FirstGadget, OnlyThisGadget, Xstart, Up, Left-(dat)1, Dwn, Shaded, &lError);
	    else
		DrawDesktop(FirstScreen, Xstart, Up, Left-(dat)1, Dwn, Shaded);
	}
	else
	    Left=Xstart;
	
	/* Draw the visible area left of the window : */
	
	if (shRgt+(ldat)DeltaXShade<(ldat)Xend) {
	    Rgt=(dat)shRgt+(dat)DeltaXShade;
	    if (NextWin)
		_DrawArea_(&AreaParent, (screen *)0, NextWin, OnlyThisWindow, FirstGadget, OnlyThisGadget, Rgt+(dat)1, Up, Xend, Dwn, Shaded, &lError);
	    else
		DrawDesktop(FirstScreen, Rgt+(dat)1, Up, Xend, Dwn, Shaded);
	}
	else
	    Rgt=Xend;
	
	/* Draw the visible area below the window, left of the shadow : */
	
	if (Shade && shLeft<=(ldat)Xend && (temp_Orizz=shLeft+(ldat)DeltaXShade-(ldat)1)>=(ldat)Xstart &&
	    (temp_Vert=shDwn+(ldat)1)<=(ldat)Yend && shDwn+(ldat)DeltaYShade>=(ldat)Ystart) {
	    
	    if (temp_Orizz>(ldat)Xend)
		temp_Orizz=Xend;
	    if (temp_Vert<(ldat)Ystart)
		temp_Vert=Ystart;
	    
	    if (NextWin)
		_DrawArea_(&AreaParent, (screen *)0, NextWin, OnlyThisWindow, FirstGadget, OnlyThisGadget, Left, (dat)temp_Vert, (dat)temp_Orizz, Dwn, Shaded, &lError);
	    else
		DrawDesktop(FirstScreen, Left, (dat)temp_Vert, (dat)temp_Orizz, Dwn, Shaded);
	}
	
	/* Draw the visible area right of the window, above the shadow : */
	
	if (Shade && shRgt+(ldat)DeltaXShade>=Xstart && (temp_Orizz=shRgt+(ldat)1)<=(ldat)Xend &&
	    (temp_Vert=shUp+(dat)DeltaYShade-(ldat)1)>=(ldat)Ystart && shUp<=(ldat)Yend) {
	    
	    if (temp_Orizz<(ldat)Xstart)
		temp_Orizz=Xstart;
	    if (temp_Vert>(ldat)Yend)
		temp_Vert=Yend;
	    
	    if (NextWin)
		_DrawArea_(&AreaParent, (screen *)0, NextWin, OnlyThisWindow, FirstGadget, OnlyThisGadget, (dat)temp_Orizz, Up, Rgt, (dat)temp_Vert, Shaded, &lError);
	    else
		DrawDesktop(FirstScreen, (dat)temp_Orizz, Up, Rgt, (dat)temp_Vert, Shaded);
	}
    } while ((Area=AreaParent.FirstArea));
    
    if (lError)
	Error(lError);
}

void DrawBorderWindow(window *Window, byte Flags) {
    ldat shLeft, shUp, shRgt, shDwn;
    dat Left, Up, Rgt, Dwn;
    ldat NWinDiMenu;
    dat ScreenWidth, ScreenHeight;
    udat YLimit;
    window *FirstWindow;
    screen *Screen, *FirstScreen;

    if (!Window || !(Screen=Window->Screen))
	return;

    INIT;
    
    if (shLeft>=(ldat)ScreenWidth || shUp>=(ldat)ScreenHeight || shRgt<(ldat)0 || shDwn<(ldat)YLimit)
	return;

    FirstScreen= Screen==All->FirstScreen ? Screen : (screen *)0;
    FirstWindow= FirstScreen && Window==Screen->FirstWindow ? Window : (window *)0;
    
    Left=(dat)Max2(shLeft, (ldat)0);
    Up=(dat)Max2(shUp, (ldat)YLimit);
    Rgt=(dat)Min2(shRgt, (ldat)ScreenWidth-(ldat)1);
    Dwn=(dat)Min2(shDwn, (ldat)ScreenHeight-(ldat)1);
    
    if ((Flags&BORDER_LEFT)   && shLeft>=0)
	DrawArea(FirstScreen, FirstWindow, Window, (gadget *)0, (gadget *)0, Left, Up, Left, Dwn, FALSE);
    if ((Flags&BORDER_UP)     && shUp>=YLimit)
	DrawArea(FirstScreen, FirstWindow, Window, (gadget *)0, (gadget *)0, Left, Up, Rgt, Up, FALSE);
    if ((Flags&BORDER_RIGHT)  && shRgt<(ldat)ScreenWidth)
	DrawArea(FirstScreen, FirstWindow, Window, (gadget *)0, (gadget *)0, Rgt, Up, Rgt, Dwn, FALSE);
    if ((Flags&BORDER_DOWN)   && shDwn<(ldat)ScreenHeight)
	DrawArea(FirstScreen, FirstWindow, Window, (gadget *)0, (gadget *)0, Left, Dwn, Rgt, Dwn, FALSE);
}

void DrawShadeWindow(window *Window, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Internal) {
    ldat shLeft, shUp, shRgt, shDwn;
    dat ScreenHeight, ScreenWidth;
    dat Left_O, Rgt_O, Up_O, Dwn_O, Rgt_1, Dwn_1;
    udat YLimit;
    byte DeltaXShade, DeltaYShade, Shade;
    ldat NWinDiMenu;
    setup *SetUp;
    screen *Screen;
    

    SetUp=All->SetUp;
    if (!Window || !(Screen=Window->Screen) ||
	Xstart>Xend || Ystart>Yend || !(SetUp->Flags & SETUP_DO_SHADE))
	return;
    
    DeltaXShade=SetUp->DeltaXShade;
    DeltaYShade=SetUp->DeltaYShade;

    INIT;
    
    Xstart=Max2(Xstart, (dat)0);
    Ystart=Max2(Ystart, (dat)0);
    Xend=Min2(Xend, ScreenWidth-(dat)1);
    Yend=Min2(Yend, ScreenHeight-(dat)1);
    
    if (shLeft+(ldat)DeltaXShade>(ldat)Xend || shUp+(ldat)DeltaYShade>(ldat)Yend || shRgt+(ldat)DeltaXShade<(ldat)Xstart || shDwn+(ldat)DeltaYShade<(ldat)Ystart)
	return;
    
    Shade=FALSE;
    if (shLeft+(ldat)DeltaXShade<=(ldat)Xend && shRgt>=(ldat)Xstart &&
	shDwn+(ldat)DeltaYShade>=(ldat)Ystart && shDwn<Yend)
	Shade|=ORIZ;
    
    if (shRgt<(ldat)Xend && shRgt+(ldat)DeltaXShade>=(ldat)Xstart &&
	shUp+(ldat)DeltaYShade<=(ldat)Yend && shDwn+(ldat)DeltaYShade>=(ldat)Ystart)
	Shade|=VERT;
    
    Left_O=(dat)Max2(shLeft+(ldat)DeltaXShade, (ldat)Xstart);
    Up_O=(dat)Max2(shUp+(ldat)DeltaYShade, (ldat)Ystart);
    Rgt_O=(dat)Min2(shRgt+(ldat)DeltaXShade, (ldat)Xend);
    Dwn_O=(dat)Min2(shDwn+(ldat)DeltaYShade, (ldat)Yend);
    
    Rgt_1=(dat)Max2(shRgt+(ldat)1, (ldat)Xstart);
    Dwn_1=(dat)Max2(shDwn+(ldat)1, (ldat)Ystart);
    
    if (shRgt>(ldat)Xend)
	shRgt=(ldat)Xend;
    
    Window=Window->Next;
    if (Shade & ORIZ) {
	if (!Internal)
	    DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, Left_O, Dwn_1, (dat)shRgt, Dwn_O, FALSE);
	else if (Window)
	    DrawArea((screen *)0, Window, (window *)0, (gadget *)0, (gadget *)0, Left_O, Dwn_1, (dat)shRgt, Dwn_O, TRUE);
	else
	    DrawDesktop(Screen, Left_O, Dwn_1, (dat)shRgt, Dwn_O, TRUE);
    }
    if (Shade & VERT) {
	if (!Internal)
	    DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, Rgt_1, Up_O, Rgt_O, Dwn_O, FALSE);
	else if (Window)
	    DrawArea((screen *)0, Window, (window *)0, (gadget *)0, (gadget *)0, Rgt_1, Up_O, Rgt_O, Dwn_O, TRUE);
	else
	    DrawDesktop(Screen, Rgt_1, Up_O, Rgt_O, Dwn_O, TRUE);
    }
}

void DrawAreaWindow(window *Window, byte Shaded) {
    ldat shLeft, shUp, shRgt, shDwn;
    ldat NWinDiMenu;
    byte Shade, DeltaXShade, DeltaYShade;
    screen *Screen;
    dat ScreenWidth, ScreenHeight;
    udat YLimit;
    
    if (!Window || !(Screen=Window->Screen))
	return;

    Shade = All->SetUp->Flags & SETUP_DO_SHADE;

    if (Window == All->FirstScreen->FirstWindow)
	DrawWindow(Window, (gadget *)0, (gadget *)0, 0, 0, MAXDAT, MAXDAT, Shaded);
    else {
	DeltaXShade = Shade ? All->SetUp->DeltaXShade : (byte)0;
	DeltaYShade = Shade ? All->SetUp->DeltaYShade : (byte)0;
	
	INIT;
	
	if (shLeft>=(ldat)ScreenWidth || shUp>=(ldat)ScreenHeight || shRgt<-(ldat)DeltaXShade || shDwn<(ldat)YLimit-(ldat)DeltaYShade)
	    return;
    
	shLeft=Max2((ldat)0, shLeft);
	shUp=Max2((ldat)0, shUp);
	shRgt=Min2((ldat)ScreenWidth-(ldat)1, shRgt);
	shDwn=Min2((ldat)ScreenHeight-(ldat)1, shDwn);
    
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, Shaded);
    }

    if (Shade)
	DrawShadeWindow(Window, MINDAT, MINDAT, MAXDAT, MAXDAT, FALSE);
}

void DrawAbsoluteWindow(window *Window, udat Xstart, udat Ystart, udat Xend, udat Yend) {
    ldat shLeft, shUp, shRgt, shDwn;
    dat ScreenWidth, ScreenHeight;
    dat Left, Up, Rgt, Dwn;
    udat YLimit;
    ldat NWinDiMenu;
    screen *Screen;
    
    if (!Window || !(Screen=Window->Screen) || Xend<Xstart || Yend<Ystart)
	return;
    
    INIT;
    
    if (shLeft>=(ldat)ScreenWidth || shUp>=(ldat)ScreenHeight || shRgt<(ldat)0 || shDwn<(ldat)YLimit)
	return;
    
    shRgt=shLeft+(ldat)Xend;
    shDwn=shUp+(ldat)Yend;
    shLeft+=Xstart;
    shUp+=Ystart;
    
    Left=(dat)Max2(shLeft, (ldat)0);
    Up=(dat)Max2(shUp, (ldat)YLimit);
    Rgt=(dat)Min2(shRgt, (ldat)ScreenWidth-(ldat)1);
    Dwn=(dat)Min2(shDwn, (ldat)ScreenHeight-(ldat)1);
    
    DrawArea((screen *)0, (window *)0, Window, (gadget *)0, (gadget *)0, Left, Up, Rgt, Dwn, FALSE);
}

void DrawTextWindow(window *Window, uldat Xstart, uldat NumRowStart, uldat Xend, uldat NumRowEnd) {
    uldat XLogic, YLogic;
    
    if (!Window || Xend<Xstart || NumRowEnd<NumRowStart)
	return;
    
    XLogic=Window->XLogic;
    YLogic=Window->YLogic;
    
    if (Xend<XLogic || NumRowEnd<YLogic ||
	Xstart>XLogic+(uldat)Window->XWidth-(uldat)3 ||
	NumRowStart>YLogic+(uldat)Window->YWidth-(uldat)3)
	return;
    
    if (NumRowEnd<MAXULDAT) {
	Xstart = Xstart<XLogic ? (uldat)0 : Xstart-XLogic;
	Xend-=XLogic;
	NumRowStart = NumRowStart<YLogic ? (uldat)0 : NumRowStart-YLogic;
	NumRowEnd-=YLogic;
	DrawAbsoluteWindow(Window, (udat)++Xstart, (udat)++NumRowStart, (udat)++Xend, (udat)++NumRowEnd);
    }
}

void DrawScreen(screen *Screen) {
    screen *FirstScreen;
    
    FirstScreen= Screen==All->FirstScreen ? Screen : (screen *)0;
    DrawArea(FirstScreen, (window *)0, (window *)0, (gadget *)0, (gadget *)0, MINDAT, (dat)Screen->YLimit, MAXDAT, MAXDAT, FALSE);
}

void ClearSelection(window *Window) {
    if (Window->Attrib & WINDOW_DO_SEL) {
	Window->Attrib &= ~WINDOW_DO_SEL;
	if (Window->YendSel > Window->YstSel)
	    DrawTextWindow(Window, Window->XLogic, Window->YstSel, Window->XLogic+Window->XWidth-2, Window->YendSel);
	else
	    DrawTextWindow(Window, Window->XstSel, Window->YstSel, Window->XendSel, Window->YstSel);
    }
}

void StartSelection(window *Window, uldat XSel, uldat YSel) {
    ClearSelection(Window);
    Window->Attrib |= WINDOW_DO_SEL|WINDOW_FWDSEL;
    Window->XstSel = Window->XendSel = XSel;
    Window->YstSel = Window->YendSel = YSel;
    DrawTextWindow(Window, XSel, YSel, XSel, YSel);
}

void ExtendSelection(window *Window, uldat XSel, uldat YSel) {
    uldat oX, oY;
    
    if (!(Window->Attrib & WINDOW_DO_SEL)) {
	Window->Attrib |= WINDOW_DO_SEL;
	if (Window->YstSel == Window->YendSel)
	    DrawTextWindow(Window, Window->XstSel, Window->YstSel, Window->XendSel, Window->YstSel);
	else
	    DrawTextWindow(Window, Window->XLogic, Window->YstSel, Window->XLogic+Window->XWidth-2, Window->YendSel);
    }
    
    if (Window->Attrib & WINDOW_FWDSEL) {
	oX = Window->XendSel;
	oY = Window->YendSel;
    } else {
	oX = Window->XstSel;
	oY = Window->YstSel;
    }
    
    if (Window->Attrib & WINDOW_FWDSEL &&
	 (YSel < Window->YstSel || (YSel == Window->YstSel && XSel <= Window->XstSel))) {

	Window->Attrib ^= WINDOW_ANYSEL;
	Window->XendSel = Window->XstSel;
	Window->YendSel = Window->YstSel;
    } else if (Window->Attrib & WINDOW_REVSEL &&
	 (YSel > Window->YstSel || (YSel == Window->YstSel && XSel > Window->XstSel))) {

	Window->Attrib ^= WINDOW_ANYSEL;
	Window->XstSel = Window->XendSel;
	Window->YstSel = Window->YendSel;
    }

    if (Window->Attrib & WINDOW_FWDSEL) {
	Window->XendSel = XSel;
	Window->YendSel = YSel;
    } else {
	Window->XstSel = XSel;
	Window->YstSel = YSel;
    }
	
    if (YSel == oY)
	DrawTextWindow(Window, Min2(XSel, oX), YSel, Max2(XSel, oX), YSel);
    else
	DrawTextWindow(Window, Window->XLogic, Min2(YSel, oY), Window->XLogic+Window->XWidth-2, Max2(YSel, oY));
}
