/*
 *  draw.c  --  functions to draw screens, menus, windows, gadgets, etc.
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
#include "main.h"
#include "data.h"
#include "methods.h"
#include "hw.h"
#include "hw_multi.h"
#include "printk.h"
#include "util.h"
#include "draw.h"

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
# include <Tutf/Tutf_defs.h>
#endif

#ifdef CONF__UNICODE
hwattr extra_POS_INSIDE;
hwattr extra_POS_ROOT;
#else
# define extra_POS_INSIDE 0
# define extra_POS_ROOT 0
#endif


byte InitDraw(void) {
#ifdef CONF__UNICODE
    extra_POS_INSIDE = HWATTR_EXTRA32(0, EncodeToHWAttrExtra(POS_INSIDE, 0, 0, 0));
    extra_POS_ROOT   = HWATTR_EXTRA32(0, EncodeToHWAttrExtra(POS_ROOT, 0, 0, 0));
#endif
    return TRUE;
}

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

/*
 * warning: DrawMenu() can cheat and give us a user Menu
 * while MenuItem is from All->CommonMenu
 */
static void FindFontMenuItem(menu Menu, menuitem MenuItem, dat i, byte Select, hwattr *PtrAttr) {
    hwcol Color;
    byte ShortCutFound;
    
    if (Menu && MenuItem && i >= MenuItem->Left && i < MenuItem->Left + MenuItem->Len) {
	ShortCutFound = i==MenuItem->Left+MenuItem->ShortCut;
	if (MenuItem->Flags & ROW_ACTIVE) {
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

	*PtrAttr = HWATTR(Color, MenuItem->Text[i - MenuItem->Left]);
    }
}

static void FindFontInfo(menu Menu, dat i, byte Select, hwattr *PtrAttr) {
    row Info;
    hwcol Color;
    
    if (Menu && (Info = Menu->Info) && i >= 0 && i < Info->Len) {
	if (Select)
	    Color = Info->Flags & ROW_ACTIVE ? Menu->ColSelect : Menu->ColSelectDisabled;
	else if (!(Info->Flags & ROW_ACTIVE))
	    Color = Menu->ColDisabled;
	else if (Menu->FlagDefColInfo || !Info->ColText)
	    Color = Menu->ColItem;
	else
	    Color = Info->ColText[i];
	
	*PtrAttr = HWATTR(Color, Info->Text[i]);
    }
}


void DrawDesktop(screen Screen, dat X1, dat Y1, dat X2, dat Y2, byte Shaded) {
    hwattr *Attr, attr;
    hwcol col;
    dat DWidth, DHeight;
    dat YLimit = -1;
    dat BgWidth, BgHeight;
    
    if (QueuedDrawArea2FullScreen || X1>X2 || Y1>Y2 || X2<0 || Y2<0)
	return;
    
    DWidth=All->DisplayWidth;
    DHeight=All->DisplayHeight;
    
    if (Screen)
	YLimit = Screen->YLimit;

    if (X1>=DWidth || Y1>=DHeight || Y2<YLimit)
	return;

    X1 = Max2(X1, 0);
    Y1 = Max2(Y1, YLimit+1);
    X2 = Min2(X2, DWidth-1);
    Y2 = Min2(Y2, DHeight-1);

    if (Screen && S_USE(Screen, USEBG)) {
	Attr = Screen->USE.B.Bg;
	BgWidth = Screen->USE.B.BgWidth;
	BgHeight = Screen->USE.B.BgHeight;
    }
    
    if (Screen && S_USE(Screen, USEBG) && (BgWidth > 1 || BgHeight > 1)) {
	/* use tiling... easier than other solutions */
	ldat X, Y, x, max = BgWidth * BgHeight;
	ldat y = ((ldat)Y1 + Screen->YLogic - Screen->YLimit) % BgHeight;
	if (y < 0) y += BgHeight;
	y *= BgWidth;
	
	DirtyVideo(X1, Y1, X2, Y2);
	
	for (; Y1 <= Y2; Y1++, y+=BgWidth) {
	    Y = Y1 * DWidth;

	    if (y >= max) y -= max;
	    x = ((ldat)X1 + Screen->XLogic) % BgWidth;
	    if (x < 0) x += BgWidth;
		
	    if (Shaded) {
		for (X = X1; X <= X2; X++, x++) {
		    if (x >= BgWidth)
			x -= BgWidth;
		    attr = Attr[x + y];
		    col = DoShadowColor(HWCOL(attr), Shaded, Shaded);

		    Video[X + Y] = HWATTR(col, HWFONT(attr)) | extra_POS_ROOT;
		}
	    } else {
		for (X = X1; X <= X2; X++, x++) {
		    if (x >= BgWidth)
			x -= BgWidth;
		    
		    Video[X + Y] = Attr[x + y] | extra_POS_ROOT;
		}
	    }
	}
    } else {
	if (Screen) {
	    if (S_USE(Screen, USEBG))
		attr = *Screen->USE.B.Bg;
	    else
		attr = Screen->USE_Fill;
	} else
	    attr = HWATTR(COL(WHITE,BLACK),' ');

	if (Shaded) {
	    col = DoShadowColor(HWCOL(attr), Shaded, Shaded);
	    attr = HWATTR(col, 0) | HWATTR_FONTMASK(attr);
	}
	FillVideo(X1, Y1, X2, Y2, attr | extra_POS_ROOT);
    }
}

/*
 * initialize a draw_ctx given widget-relative coords
 * (0,0 is the widget top-left corner)
 * return FALSE if given window area is not visible
 */
byte InitDrawCtx(widget W, dat X1, dat Y1, dat X2, dat Y2, byte Shaded, draw_ctx *D) {
    ldat XL, YL, height;
    uldat Cycle = 0;
    widget C = NULL, CC = NULL;
    byte HasTopBar, HasBorder;
    
    if (!W || !D)
	return FALSE;

    D->Next = (draw_ctx *)0;
    D->W = W;
    D->NoChildren = D->BorderDone = FALSE;
    D->Shaded = Shaded;
    
    D->Left= 0;
    D->Rgt = W->XWidth - 1;
    D->X1  = X1;
    D->X2  = X2;
    
    D->Up  = 0;
    if (IS_WINDOW(W) && (((window)W)->Attrib & WINDOW_ROLLED_UP))
	D->Dwn = 0;
    else
	D->Dwn = W->YWidth - 1;
    D->Y1  = Y1;
    D->Y2  = Y2;

    while (W) {
	
	if (W->Flags & WIDGETFL_NOTVISIBLE)
	    return FALSE;
	
	HasTopBar = HasBorder = 0;

	if (!IS_WINDOW(W) || !(((window)W)->Attrib & WINDOW_ROLLED_UP))
	    height = W->YWidth;
	else
	    height = 1;

	if (Cycle++) {
	    XL = W->XLogic;
	    YL = W->YLogic;

	    if (IS_SCREEN(W))
		HasTopBar = 1;
	    else if (IS_WINDOW(W) && !(((window)W)->Flags & WINDOWFL_BORDERLESS))
		/* count border thickness */
		HasBorder = 1;
	} else
	    XL = YL = 0;

	D->Left+= (ldat)W->Left - XL + HasBorder;
	D->Rgt += (ldat)W->Left - XL + HasBorder;
	D->X1   = Max2(D->X1 - XL, 0) + HasBorder + W->Left;
	D->X2   = Min2(D->X2 - XL + HasBorder, W->XWidth - 1 - HasBorder) + W->Left;
	
	D->Up  += (ldat)W->Up - YL + HasBorder;
	D->Dwn += (ldat)W->Up - YL + HasBorder;
	D->Y1   = Max2(D->Y1 - YL, HasTopBar) + HasBorder + W->Up;
	D->Y2   = Min2(D->Y2 - YL + HasBorder, height - 1 - HasBorder) + W->Up;

	CC = C;
	C = W;
	W = W->Parent;
    }
    
    D->DWidth = All->DisplayWidth;
    D->DHeight = All->DisplayHeight;

    D->X1 = Max2(D->X1, 0);
    D->X2 = Min2(D->X2, D->DWidth - 1);
    D->Y1 = Max2(D->Y1, 0);
    D->Y2 = Min2(D->Y2, D->DHeight - 1);

    D->Screen = (screen)C;
    D->TopW = CC;
    if (D->W == CC)
	D->W = NULL;
    
    if (C && IS_SCREEN(C))
	return D->X1 <= D->X2 && D->Y1 <= D->Y2;
    
    return FALSE;
}

/*
 * same as InitDrawCtx, but now X1,Y1,X2,Y2 are
 * absolute display coords. (0,0 is the display top-left corner)
 * return FALSE if given area is not visible 
 */
byte InitAbsoluteDrawCtx(widget W, dat X1, dat Y1, dat X2, dat Y2, byte Shaded, draw_ctx *D) {

    if (InitDrawCtx(W, 0, 0, MAXDAT, MAXDAT, Shaded, D)) {
	
	D->X1 = Max2(D->X1, X1);
	D->X2 = Min2(D->X2, X2);
    
	D->Y1 = Max2(D->Y1, Y1);
	D->Y2 = Min2(D->Y2, Y2);
	
	return D->X1 <= D->X2 && D->Y1 <= D->Y2;
    }
    return FALSE;
}

/*
 * translate X,Y coords from `relative to W1' to `relative to W2'.
 * If W1 is NULL X,Y coords are taken as `absolute'.
 * If W2 is NULL X,Y coords are translated to `absolute'.
 * *Inside will be == FALSE if X,Y is outside W2,
 * == TRUE if inside, == TRUE+TRUE if on the border.
 */
void TranslateCoordsWidget(widget W1, widget W2, dat *X, dat *Y, byte *Inside) {
    draw_ctx D;
    if (W1 || W2) {
	if (W1) {
	    InitDrawCtx(W1, 0, 0, MAXDAT, MAXDAT, FALSE, &D);
	    if (IS_WINDOW(W1) && !(((window)W1)->Flags & WINDOWFL_BORDERLESS)) {
		(*X)++, (*Y)++;
	    }
	    *X += D.Left;
	    *Y += D.Up;
	}
	if (W2) {
	    InitDrawCtx(W2, 0, 0, MAXDAT, MAXDAT, FALSE, &D);
	    if (Inside) {
		if (*X >= D.Left && *X <= D.Rgt && *Y >= D.Up && *Y <= D.Dwn) {
		    *Inside = TRUE;
	    
		    if (IS_WINDOW(W2) && !(((window)W2)->Flags & WINDOWFL_BORDERLESS) &&
			(*X == D.Left || *X == D.Rgt || *Y == D.Up || *Y == D.Dwn))
		
			*Inside += TRUE;
		} else
		    *Inside = FALSE;
	    }
	    if (IS_WINDOW(W2) && !(((window)W2)->Flags & WINDOWFL_BORDERLESS)) {
		(*X)--, (*Y)--;
	    }
	    *X -= D.Left;
	    *Y -= D.Up;
	}
    } else
	*Inside = FALSE;
}


/*
 * find the widget at given coordinates inside Parent
 * --- (0,0) is the Parent top-left corner
 */
widget FindWidgetAt(widget Parent, dat X, dat Y) {
    widget W;
    ldat i, j;
    dat height;
    
    if (IS_WINDOW(Parent) && !(((window)Parent)->Flags & WINDOWFL_BORDERLESS))
	X--, Y--;
    else if (IS_SCREEN(Parent) && Y <= 0) {
	/* got nothing, or the menu... */
	return (widget)0;
    }
    
    for (W = Parent->FirstW; W; W = W->Next) {
	
	if (W->Flags & WIDGETFL_NOTVISIBLE)
	    continue;
	
	i = X + Parent->XLogic;
	j = Y + Parent->YLogic;

	if (!IS_WINDOW(W) || !(((window)W)->Attrib & WINDOW_ROLLED_UP))
	    height = W->YWidth;
	else
	    height = 1;
	
	if ((ldat)W->Left <= i && (ldat)W->Left + W->XWidth > i &&
	    (ldat)W->Up   <= j && (ldat)W->Up   + height    > j)
	    return W;
    }
    return (widget)0;
}


widget RecursiveFindWidgetAt(widget Parent, dat X, dat Y) {
    widget W;
    byte HasBorder;
    
    while (Parent) {
	HasBorder = IS_WINDOW(Parent) && !(((window)Parent)->Flags & WINDOWFL_BORDERLESS);

	if (X >= HasBorder && Y >= HasBorder && X < Parent->XWidth - HasBorder && Y < Parent->YWidth - HasBorder &&
	    (W = Act(FindWidgetAt,Parent)(Parent, X, Y))) {

	    X += Parent->XLogic - W->Left - HasBorder;
	    Y += Parent->YLogic - W->Up - HasBorder;
	    Parent = W;
	} else
	    break;
    }
    return Parent;
}



void DrawSelfWidget(draw_ctx *D) {
    widget W = D->TopW;
    
    if (QueuedDrawArea2FullScreen || (W->Flags & WIDGETFL_NOTVISIBLE))
	return;
    
    if (w_USE(W, USEEXPOSE)) {
	CONST byte *Text = NULL;
	CONST hwfont *HWFont = NULL;
	CONST hwattr *HWAttr = NULL;
	ldat Left, Up, Rgt, Dwn;
	ldat v;
	dat Pitch, X1, X2, Y1, Y2, dX, dY;
	ldat _X1, _X2, _Y1, _Y2;
	dat DWidth, i, j;
	hwattr h;
	hwcol Color;
	byte Shaded;

	switch (W->USE.E.Flags) {
	  case WIDGET_USEEXPOSE_TEXT:
	    Text = W->USE.E.E.Text;
	    break;
	  case WIDGET_USEEXPOSE_HWFONT:
	    HWFont = W->USE.E.E.HWFont;
	    break;
	  case WIDGET_USEEXPOSE_HWATTR:
	    HWAttr = W->USE.E.E.HWAttr;
	    break;
	  default:
	    break;
	}
	Left = D->Left; Up = D->Up;
	Rgt = D->Rgt; Dwn = D->Dwn;
	X1 = D->X1; Y1 = D->Y1;
	X2 = D->X2; Y2 = D->Y2;
	Shaded = D->Shaded;
	DWidth = D->DWidth;

#if 0
	/*
	 * this would suppress EXPOSE messages during resize (good)
	 * but also clears the whole widget during non-top resize (bad)
	 */
	if ((All->State & STATE_ANY) == STATE_RESIZE && (widget)All->FirstScreen->ClickWindow == W) {
	    /* user is interactively resizing this window... pad with spaces */
	    FillVideo(X1, Y1, X2, Y2, W->USE_Fill);
	    return;
	}
#endif

	if (Text || HWFont || HWAttr) {
	    Pitch = W->USE.E.Pitch;
	    
	    _X1 = Max2(W->USE.E.X1 + Left, X1);
	    _X2 = Min2(W->USE.E.X2 + Left, X2);
	    _Y1 = Max2(W->USE.E.Y1 + Up, Y1);
	    _Y2 = Min2(W->USE.E.Y2 + Up, Y2);

	    dX = _X1 - W->USE.E.X1 - Left;
	    dY = _Y1 - W->USE.E.Y1 - Up;
	    
	    h = W->USE_Fill | extra_POS_INSIDE;
	    if (_X1 > _X2 || _Y1 > _Y2) {
		/* no valid ->USE.E, fill with spaces */
		FillVideo(X1, Y1, X2, Y2, h);
		return;
	    }

	    DirtyVideo(X1, Y1, X2, Y2);
	    
	    /*
	     * check if the ->USE.E is smaller than the widget size...
	     * pad with SPACEs as needed
	     */
	    if (Y1 < _Y1) {
		FillVideo(X1, Y1, X2, _Y1 - 1, h);
		Y1 = _Y1;
	    }
	    if (Y2 > _Y2) {
		FillVideo(X1, _Y2 + 1, X2, Y2, h);
		Y2 = _Y2;
	    }
	    if (X1 < _X1) {
		FillVideo(X1, Y1, _X1 - 1, Y2, h);
		X1 = _X1;
	    }
	    if (X2 > _X2) {
		FillVideo(_X2 + 1, Y1, X2, Y2, h);
		X2 = _X2;
	    }
	    
	    if (Text) {
		Color = HWCOL(W->USE_Fill);
		if (Shaded)
		    Color = DoShadowColor(Color, Shaded, Shaded);
		Text += dY * Pitch;
		for (j=Y1; j<=Y2; j++) {
		    Text += dX;
		    for (i=X1, v=0; i<=X2; i++, v++)
			Video[i+j*DWidth] = HWATTR(Color, Text[v]);
		    Text += Pitch - dX;
		}
	    } else if (HWFont) {
		Color = HWCOL(W->USE_Fill);
		if (Shaded)
		    Color = DoShadowColor(Color, Shaded, Shaded);
		HWFont += dY * Pitch;
		for (j=Y1; j<=Y2; j++) {
		    HWFont += dX;
		    for (i=X1, v=0; i<=X2; i++, v++)
			Video[i+j*DWidth] = HWATTR(Color, HWFont[v]);
		    HWFont += Pitch - dX;
		}
	    } else if (HWAttr && !Shaded) {
		HWAttr += dX + dY * Pitch;
		for (j=Y1; j<=Y2; j++) {
		    CopyMem(HWAttr, &Video[j*DWidth+X1], sizeof(hwattr)*(X2-X1+1));
		    HWAttr += Pitch;
		}
	    } else if (HWAttr) {
		/* Shaded == TRUE */
		HWAttr += dY * Pitch;
		for (j=Y1; j<=Y2; j++) {
		    HWAttr += dX;
		    for (i=X1, v=0; i<=X2; i++, v++) {
			Color = DoShadowColor(HWCOL(HWAttr[v]), Shaded, Shaded);
			Video[i+j*DWidth] = HWATTR(Color, 0) | HWATTR_FONTMASK(HWAttr[v]);
		    }
		    HWAttr += Pitch - dX;
		}
	    }
	} else {
	    /* ask the client to draw */
	    msg Msg;
	    event_widget *EventW;

	    if ((Msg=Do(Create,Msg)(FnMsg, MSG_WIDGET_CHANGE, 0))) {
		EventW = &Msg->Event.EventWidget;
		EventW->W      = W;
		EventW->Code   = MSG_WIDGET_EXPOSE;
		EventW->Flags  = Shaded ? MSG_WIDGETFL_SHADED : 0;
		EventW->XWidth = X2 - X1 + 1;
		EventW->YWidth = Y2 - Y1 + 1;
		EventW->X      = X1 - Left;
		EventW->Y      = Y1 - Up;
		SendMsg(W->Owner, Msg);
	    }
	}
    } else
	FillVideo(D->X1, D->Y1, D->X2, D->Y2, W->USE_Fill | extra_POS_INSIDE);
}


void DrawSelfGadget(draw_ctx *D) {
    gadget G = (gadget)D->TopW;

    if (QueuedDrawArea2FullScreen || (G->Flags & WIDGETFL_NOTVISIBLE))
	return;
    
    if (!G_USE(G, USETEXT)) {
	G->Fn->Fn_Widget->DrawSelf(D);
	return;
    }
    
    {
	widget Parent;
	byte Select, Disabled, Absent;
	dat width, DWidth, i, i_min, i_max, j, j_min, j_max;
	ldat Offset;
	hwfont Font, *Text, **GadgetText;
	hwcol *ColText, **GadgetColor;
	hwcol Color;

	Parent = G->Parent;
	Select = !!(G->Flags & GADGETFL_PRESSED);
	Disabled = !!(G->Flags & GADGETFL_DISABLED);
	Absent = !!(G->Flags & GADGETFL_TEXT_DEFCOL);
	    
	i_min = D->X1 - D->Left;
	i_max = D->X2 - D->Left;
	    
	j_min = D->Y1 - D->Up;
	j_max = D->Y2 - D->Up;

	width = G->XWidth;
	DWidth = D->DWidth;
    
	Offset = D->Left + D->Up * DWidth;
	    
	GadgetText = G->USE.T.Text;
	GadgetColor = G->USE.T.Color;

	Text = Select
	    ? Disabled && GadgetText[3]
	    ? GadgetText[3]
	    : GadgetText[1]
	    ? GadgetText[1]
	    : GadgetText[0]
	    : Disabled && GadgetText[2]
	    ? GadgetText[2]
	    : GadgetText[0];
	    
	if (!Text)
	    Font = ' ';
	
	ColText = Absent
	    ? NULL
	    : Select
	    ? Disabled && GadgetColor[3]
	    ? GadgetColor[3]
	    : GadgetColor[1]
	    ? GadgetColor[1]
	    : GadgetColor[0]
	    : Disabled && GadgetColor[2]
	    ? GadgetColor[2]
	    : GadgetColor[0];
	
	if (!ColText) {
	    Absent = TRUE;
	    Color = Select
	    ? Disabled
	    ? G->ColSelectDisabled
	    : G->ColSelect
	    : Disabled
	    ? G->ColDisabled
	    : G->ColText;
	    Color = DoShadowColor(Color, D->Shaded, D->Shaded);
	}

	for (j=j_min; j<=j_max; j++) {
	    for (i=i_min; i<=i_max; i++) {
		if (Text)
		    Font = Text[i + j * width];
		if (!Absent)
		    Color = DoShadowColor(ColText[i + j * width], D->Shaded, D->Shaded);
		Video[i + j * DWidth + Offset] =
		    HWATTR(Color, Font) | extra_POS_INSIDE;
	    }
	}
	DirtyVideo(D->X1, D->Y1, D->X2, D->Y2);
    }
}

static void DrawSelfBorder(window Window, ldat Left, ldat Up, ldat Rgt, ldat Dwn,
			   dat X1, dat Y1, dat X2, dat Y2,
			   byte Border, byte WinActive, byte Shaded) {

    dat i, j, u, v;
    dat DWidth = All->DisplayWidth;
    hwattr Attr;
    hwcol Color;
    
    if (QueuedDrawArea2FullScreen || (Window->Flags & WIDGETFL_NOTVISIBLE))
	return;
    
    if ((ldat)Y1==Up) {
	j=Y1*DWidth;
	for (i=X1, u=(ldat)i-Left; i<=X2; i++, u++) {
	    Act(FindBorder,Window)(Window, u, 0, Border, &Attr);
	    Color = DoShadowColor(HWCOL(Attr), Shaded || !WinActive, Shaded);
	    Video[i+j] = HWATTR(Color, 0) | (Attr & ~HWATTR(MAXHWCOL, 0));
	}
	DirtyVideo(X1, Y1, X2, Y1);
	Y1++;
	if (Y1 > Y2)
	    return;
    }
    
    if ((ldat)Y2==Dwn) {
	v=((ldat)Y2-Up);
	j=Y2*DWidth;
	for (i=X1, u=(ldat)i-Left; i<=X2; i++, u++) {
	    Act(FindBorder,Window)(Window, u, v, Border, &Attr);
	    Color = DoShadowColor(HWCOL(Attr), Shaded || !WinActive, Shaded);
	    Video[i+j] = HWATTR(Color, 0) | (Attr & ~HWATTR(MAXHWCOL, 0));
	}
	DirtyVideo(X1, Y2, X2, Y2);
	Y2--;
	if (Y1 > Y2)
	    return;
    }
    
    if ((ldat)X1==Left) {
	for (j=Y1, v=(ldat)j-Up; j<=Y2; j++, v++) {
	    Act(FindBorder,Window)(Window, 0, v, Border, &Attr);
	    Color = DoShadowColor(HWCOL(Attr), Shaded || !WinActive, Shaded);
	    Video[X1+j*DWidth] = HWATTR(Color, 0) | (Attr & ~HWATTR(MAXHWCOL, 0));
	}
	DirtyVideo(X1, Y1, X1, Y2);
	X1++;
	if (X1 > X2)
	    return;
    }
    
    if ((ldat)X2==Rgt) {
	u=((ldat)X2-Left);
	for (j=Y1, v=(ldat)j-Up; j<=Y2; j++, v++) {
	    Act(FindBorder,Window)(Window, u, v, Border, &Attr);
	    Color = DoShadowColor(HWCOL(Attr), Shaded || !WinActive, Shaded);
	    Video[X2+j*DWidth] = HWATTR(Color, 0) | (Attr & ~HWATTR(MAXHWCOL, 0));
	}
	DirtyVideo(X2, Y1, X2, Y2);
	X2--;
	if (X1 > X2)
	    return;
    }
}

void DrawSelfWindow(draw_ctx *D) {
    window W = (window)D->TopW;

    if (QueuedDrawArea2FullScreen || (W->Flags & WIDGETFL_NOTVISIBLE))
	return;

    if (!W_USE(W, USEROWS) && !W_USE(W, USECONTENTS)) {
	W->Fn->Fn_Widget->DrawSelf(D);
	return;
    }
	
    {
	ldat Left, Up, Rgt, Dwn;
	dat X1, Y1, X2, Y2;
	dat DWidth, i, j;
	ldat u, v;
	byte Shaded, Absent;
	byte Select, RowDisabled;
	row CurrRow;
	CONST hwattr *Contents, *CurrCont;
	ldat Row, PosInRow;
	CONST hwfont *HWFont;
	hwcol *ColText;
	hwcol Color;
	hwfont Font;
	
	X1 = D->X1; Y1 = D->Y1;
	X2 = D->X2; Y2 = D->Y2;
	Left = D->Left; Up = D->Up;
	Rgt = D->Rgt; Dwn = D->Dwn;
	Shaded = D->Shaded;
	DWidth = D->DWidth;
    
	/* not here... already done in DrawWCtx() */
	/*
	 * Left -= W->XLogic; Rgt -= W->XLogic;
	 * Up   -= W->YLogic; Dwn -= W->YLogic;
	 */
	
	if (W_USE(W, USECONTENTS) && (Contents = W->USE.C.Contents)) {
	    /*
	     * For xterm-like windows, Contents is a buffer of (x=WLogic) * (y=HLogic)
	     * hwattr:s, in the same format as Video: HWATTR(Color, Ascii).
	     * Room in excess is used as scrollback.
	     * 
	     * To avoid frequent mem-mem copies, VGA-like splitline method is used:
	     * USE.C.Split is the first used line of Contents, and after the end
	     * of Contents we restart from zero.
	     * 
	     * HLogic also has the usual meaning:
	     * number of total lines (visible + scrollback)
	     */
	    if (Y2 - Up >= W->HLogic) {
		/* the ->Contents buffer is smaller than the window size... pad with SPACEs */
		dat Ynew = Up + W->HLogic;
		FillVideo(X1, Ynew, X2, Y2, HWATTR(W->ColText, ' ') | extra_POS_INSIDE);
		Y2 = Ynew - 1;
	    }
	    if (X2 - Left >= W->WLogic) {
		/* the ->Contents buffer is smaller than the window size... pad with SPACEs */
		dat Xnew = Left + W->WLogic;
		FillVideo(Xnew, Y1, X2, Y2, HWATTR(W->ColText, ' ') | extra_POS_INSIDE);
		X2 = Xnew - 1;
	    }
	    if (X1 <= X2 && Y1 <= Y2) {
		Row = Y1 - Up + W->USE.C.HSplit; /* row number in Contents */
		if (Row >= W->HLogic)
		    Row -= W->HLogic;
		CurrCont = Contents + Row * W->WLogic;
		
		Row = W->HLogic - 1 - Row;	/* rows still readable */
		
		if (!Shaded) {
		    for (j=Y1, u=Y1-Up; j<=Y2; j++, u++, Row--) {
			if (!(W->State & WINDOW_DO_SEL) ||
			    u < W->YstSel || u > W->YendSel) {
			    
			    CopyMem(CurrCont + X1-Left, &Video[j*DWidth+X1],
				    sizeof(hwattr)*(X2-X1+1));
			    
			} else {
			    for (i=X1, v=X1-Left; i<=X2; i++, v++) {
				Select = (W->State & WINDOW_DO_SEL) &&
				    ((u>W->YstSel ||
				      (u==W->YstSel && v>=W->XstSel)) &&
				     (u<W->YendSel ||
				      (u==W->YendSel && v<=W->XendSel)));
				if (Select)
				    Color = W->ColSelect;
				else
				    Color = HWCOL(CurrCont[v]);
				
				Video[i+j*DWidth] = HWATTR(Color, HWFONT(CurrCont[v])) | extra_POS_INSIDE;
			    }
			}
			CurrCont += W->WLogic;
			if (!Row)
			    CurrCont -= (Row = W->HLogic) * W->WLogic;
		    }
		} else {
		    for (j=Y1, u=Y1-Up; j<=Y2; j++, u++, Row--) {
			for (i=X1, v=X1-Left; i<=X2; i++, v++) {
			    
			    Select = (W->State & WINDOW_DO_SEL) &&
				((u>W->YstSel ||
				  (u==W->YstSel && v>=W->XstSel)) &&
				 (u<W->YendSel ||
				  (u==W->YendSel && v<=W->XendSel)));
			    
			    if (Select)
				Color = W->ColSelect;
			    else
				Color = HWCOL(CurrCont[v]);
			    Color = DoShadowColor(Color, Shaded, Shaded);
			    
			    Video[i+j*DWidth] = HWATTR(Color, HWFONT(CurrCont[v])) | extra_POS_INSIDE;
			}
			CurrCont += W->WLogic;
			if (!Row)
			    CurrCont -= (Row = W->HLogic) * W->WLogic;
		    }
		}
		DirtyVideo(X1, Y1, X2, Y2);
	    }
	} else if (W_USE(W, USEROWS) && W->USE.R.FirstRow) {
	    /*
	     * editor-like windows. no length limit in the rows and in XLogic.
	     */
	    Row=Y1-Up;
	    
	    if (Row>=W->HLogic)
		CurrRow = (row)0;
	    else if (W->USE.R.NumRowSplit && W->USE.R.RowSplit && Row==W->USE.R.NumRowSplit)
		CurrRow = W->USE.R.RowSplit;
	    else if (W->USE.R.NumRowSplit && W->USE.R.RowSplit && W->USE.R.RowSplit->Next && Row==W->USE.R.NumRowSplit+(ldat)1)
		CurrRow = W->USE.R.RowSplit->Next;
	    else if (W->USE.R.NumRowOne && W->USE.R.RowOne && Row==W->USE.R.NumRowOne)
		CurrRow = W->USE.R.RowOne;
	    else if (Row==(ldat)0)
		CurrRow = W->USE.R.FirstRow;
	    else if (Row+(ldat)1==W->HLogic)
		CurrRow = W->USE.R.LastRow;
	    else
		CurrRow = Act(FindRow,W)(W, Row);
	    
	    for (j=Y1; j<=Y2; j++, Row++) {
		
		RowDisabled = (W->Flags & WINDOWFL_DISABLED)
		    || (CurrRow && !(CurrRow->Flags & (ROW_ACTIVE|ROW_IGNORE)));
		
		if (CurrRow) {
		    HWFont=CurrRow->Text;
		    ColText=CurrRow->ColText;
		}
		
		PosInRow=X1-Left;
		if (CurrRow && PosInRow>=CurrRow->Gap)
		    PosInRow+=CurrRow->LenGap;
		
		for (i=X1; i<=X2; i++, PosInRow++) {
		    
		    Absent = (!CurrRow || PosInRow>=CurrRow->Len);
		    
		    if (CurrRow && IS_MENUITEM(CurrRow) && ((menuitem)CurrRow)->Window && i == Rgt) {
#ifdef CONF__UNICODE
			Font = T_UTF_16_BLACK_RIGHT_POINTING_TRIANGLE;
#else
			Font = '\x10'; /* T_IBM437_BLACK_RIGHT_POINTING_TRIANGLE */
#endif
		    } else if (Absent)
			Font = ' ';
		    else
			Font = HWFont[PosInRow];
		    
		    if (W->Flags & WINDOWFL_ROWS_SELCURRENT)
			Select = Row==W->CurY;
		    else {
			Select = (W->State & WINDOW_DO_SEL) &&
			    ((Row>W->YstSel ||
			      (Row==W->YstSel && PosInRow>=W->XstSel)) &&
			     (Row<W->YendSel ||
			      (Row==W->YendSel && PosInRow<=W->XendSel)));
		    }
		    if (CurrRow)
			Select &= !(CurrRow->Flags & ROW_IGNORE);
		    
		    if (Select)
			Color = RowDisabled ? W->ColSelectDisabled : W->ColSelect;
		    else if (RowDisabled)
			Color = W->ColDisabled;
		    else if ((W->Flags & WINDOWFL_ROWS_DEFCOL) || Absent)
			Color = W->ColText;
		    else
			Color = ColText[PosInRow];
		    
		    Color=DoShadowColor(Color, Shaded, Shaded);
		    Video[i+j*DWidth] = HWATTR(Color, Font) | extra_POS_INSIDE;
		}
		if (CurrRow) {
		    W->USE.R.RowSplit = CurrRow;
		    W->USE.R.NumRowSplit = Row;
		    CurrRow=CurrRow->Next;
		}
	    }
	    DirtyVideo(X1, Y1, X2, Y2);
	} else {
	    /* either an unknown window type or just one of the above, but empty */
	    Color = W->ColText;
	    Color = DoShadowColor(Color, Shaded, Shaded);
	    FillVideo(X1, Y1, X2, Y2, HWATTR(Color, ' ') | extra_POS_INSIDE);
	}
    }
}

void DrawSelfScreen(draw_ctx *D) {
    /* should never be called */
    printk("twin: DrawSelfScreen() called! This is not good...\n");
}

static void _DrawWCtx_(draw_ctx **FirstD, widget W, widget ChildNext, widget OnlyChild,
		       ldat Left, ldat Up, ldat Rgt, ldat Dwn, dat X1, dat Y1, dat X2, dat Y2,
		       byte NoChildren, byte BorderDone,
		       byte Shaded, byte *lError) {
    draw_ctx *D;
    if (!QueuedDrawArea2FullScreen) {
	if ((D = (draw_ctx *)AllocMem(sizeof(draw_ctx)))) {
	    D->TopW = W;
	    D->W = ChildNext;
	    D->OnlyW = OnlyChild;
	    D->Screen = (screen)0;
	    D->X1 = X1; D->Y1 = Y1;
	    D->X2 = X2; D->Y2 = Y2;
	    D->Left = Left; D->Up = Up;
	    D->Rgt = Rgt; D->Dwn = Dwn;
	    D->DWidth = All->DisplayWidth;
	    D->DHeight = All->DisplayHeight;
	    D->NoChildren = NoChildren;
	    D->BorderDone = BorderDone;
	    D->Shaded = Shaded;
	    
	    D->Next = *FirstD;
	    *FirstD = D;
	} else {
	    DrawDesktop((screen)0, X1, Y1, X2, Y2, Shaded);
	    *lError=TRUE;
	}
    }
}

static void DrawWCtx(draw_ctx *D) {
    draw_ctx *FirstD = D;
    widget W;
    widget OnlyChild, ChildNext;
    ldat Left, Up, Rgt, Dwn;
    dat X1, Y1, X2, Y2;
    window Window;
    byte Shaded, Border, WinActive, NoChildren;
    byte ChildFound=FALSE, lError=FALSE, FirstCycle=TRUE;
    screen Screen;
    dat DWidth, DHeight;
    ldat cL, cU, cR, cD;

    if (QueuedDrawArea2FullScreen)
	return;
    
    do {
	W = D->TopW;
	ChildNext = D->W;
	OnlyChild = D->OnlyW;
	X1 = D->X1; Y1 = D->Y1;
	X2 = D->X2; Y2 = D->Y2;
	Left = D->Left; Up = D->Up;
	Rgt  = D->Rgt; Dwn = D->Dwn;
	Screen = D->Screen;
	DWidth = D->DWidth;
	DHeight = D->DHeight;
	Shaded = D->Shaded;
	NoChildren = D->NoChildren;

	if (OnlyChild == W) /* got it */
	    OnlyChild = NULL;

#ifdef DEBUG_DRAW
	FillVideo(X1, Y1, X2, Y2, HWATTR(COL(WHITE,YELLOW), ' '));
	FlushHW();
	usleep(300000);
	FillVideo(X1, Y1, X2, Y2, HWATTR(COL(WHITE,BLACK), ' '));
#endif
	
	FirstD = D->Next;

	/* take care of borders and XLogic,YLogic */
	if (!D->BorderDone) {
	    D->BorderDone = TRUE;
	    
	    if (IS_WINDOW(W) && !((Window = (window)W)->Flags & WINDOWFL_BORDERLESS)) {
		
		if (!OnlyChild) {
		    WinActive =
			Window==(window)All->FirstScreen->FocusW ||
			Window==All->FirstScreen->MenuWindow;
		    Border = (Window->Flags & WINDOWFL_MENU) || !WinActive;
		    DrawSelfBorder(Window, Left, Up, Rgt, Dwn, X1, Y1, X2, Y2, Border, WinActive, Shaded);
		}
		
		if ((ldat)X1==Left++) X1++;
		if ((ldat)X2==Rgt--)  X2--;
		if ((ldat)Y1==Up++)   Y1++;
		if ((ldat)Y2==Dwn--)  Y2--;
		D->X1 = X1; D->Y1 = Y1;
		D->X2 = X2; D->Y2 = Y2;
	    }
	    /*
	     * track displacement between real display coordinates
	     * and widget interior coordinates.
	     */
	    D->Left = Left -= W->XLogic;
	    D->Up   = Up   -= W->YLogic;
	    D->Rgt  = Rgt  -= W->XLogic;
	    D->Dwn  = Dwn  -= W->YLogic;
	}
	
	if (X1>X2 || Y1>Y2 || X1>=DWidth || Y1>=DHeight || X2<0 || Y2<0) {
	    if (!FirstCycle) FreeMem(D);
	    else FirstCycle = FALSE;
	    continue;
	}

	ChildFound=FALSE;
	if (NoChildren)
	    ChildNext=NULL;
	else {
	    if (!ChildNext)
		ChildNext=W->FirstW;
    
	    if (!NoChildren && ChildNext) {
		while (ChildNext && !ChildFound) {
		    cL = Left + ChildNext->Left;
		    cR =       cL + ChildNext->XWidth-1;
		    cU = Up   + ChildNext->Up;
		    cD =       cU + ChildNext->YWidth-1;
		    if (cL>X2 || cU>Y2 || cR<X1 || cD<Y1 || (ChildNext->Flags & WIDGETFL_NOTVISIBLE))
			ChildNext=ChildNext->Next;
		    else
			ChildFound=TRUE;
		}
	    }
	    if (!ChildNext)
		NoChildren=TRUE;
	}
	
	if (!ChildFound) {
	    /* no children... draw this widget */
	    Act(DrawSelf,W)(D);
	}
	    
	if (!FirstCycle)
	    FreeMem(D);
	else
	    FirstCycle = FALSE;

	if (ChildFound) {
	    	    
	    /* recursively draw the area below ChildNext */
	    if (Y2 > cD) {
		_DrawWCtx_(&FirstD, W, ChildNext, OnlyChild,
			   Left, Up, Rgt, Dwn, X1, cD+1, X2, Y2,
			   NoChildren, TRUE, Shaded, &lError);
		Y2 = cD;
	    }

	    /* recursively draw the area right of ChildNext */
	    if (X2 > cR) {
		_DrawWCtx_(&FirstD, W, ChildNext, OnlyChild,
			   Left, Up, Rgt, Dwn, cR+1, Y1, X2, Y2,
			   NoChildren, TRUE, Shaded, &lError);
		X2 = cR;
	    }
	    
	    /* recursively draw the area left of ChildNext */
	    if (X1 < cL) {
		_DrawWCtx_(&FirstD, W, ChildNext, OnlyChild,
			   Left, Up, Rgt, Dwn, X1, Y1, cL-1, Y2,
			   NoChildren, TRUE, Shaded, &lError);
		X1 = cL;
	    }

	    /* recursively draw the area above ChildNext */
	    if (Y1 < cU) {
		_DrawWCtx_(&FirstD, W, ChildNext, OnlyChild,
			   Left, Up, Rgt, Dwn, X1, Y1, X2, cU-1,
			   NoChildren, TRUE, Shaded, &lError);
		Y1 = cU;
	    }
	    
	    /* ok, now actually draw ChildNext */
	    if (X1 <= X2 && Y1 <= Y2) {
		_DrawWCtx_(&FirstD, ChildNext, NULL, OnlyChild,
			   cL, cU, cR, cD, X1, Y1, X2, Y2,
			   FALSE, FALSE, Shaded, &lError);
	    }
	    

	}
    } while ((D = FirstD));

    if (lError)
	Error(lError);
}


/*
 * this ASSUMES the specified part of the widget is unobscured
 */
void DrawWidget(widget W, dat X1, dat Y1, dat X2, dat Y2, byte Shaded) {
    draw_ctx D;
    
    if (!QueuedDrawArea2FullScreen && W && InitAbsoluteDrawCtx(W, X1, Y1, X2, Y2, Shaded, &D)) {
	D.TopW = W; D.W = D.OnlyW = NULL;
	DrawWCtx(&D);
    }
}

static void DrawAreaCtx(draw_ctx *D);

/*
 * this DOES NOT assume that the specified part of the widget is unobscured
 */
/* partially replaces DrawAreaWindow() -- does not redraw shadow */
void DrawAreaWidget(widget W) {
    draw_ctx D;
    
    if (!QueuedDrawArea2FullScreen && W && InitAbsoluteDrawCtx(W, 0, 0, MAXDAT, MAXDAT, FALSE, &D)) {
	D.TopW = D.W = D.OnlyW = NULL;
	DrawAreaCtx(&D);
    }
}



static void _DrawAreaCtx_(draw_ctx **FirstD, screen Screen, widget W, widget OnlyW,
		      dat X1, dat Y1, dat X2, dat Y2, byte Shaded, byte *lError) {
    draw_ctx *D;
    if (!QueuedDrawArea2FullScreen) {
	if ((D = (draw_ctx *)AllocMem(sizeof(draw_ctx)))) {
	    D->TopW = W;
	    D->W = OnlyW;
	    D->Screen = Screen;
	    D->Left = D->X1 = X1; D->Up  = D->Y1 = Y1;
	    D->Rgt  = D->X2 = X2; D->Dwn = D->Y2 = Y2;
	    D->DWidth = All->DisplayWidth;
	    D->DHeight = All->DisplayHeight;
	    D->Shaded = Shaded;
	    
	    D->Next = *FirstD;
	    *FirstD = D;
	    
	} else {
	    DrawDesktop((screen)0, X1, Y1, X2, Y2, Shaded);
	    *lError=TRUE;
	}
    }
}

window WindowParent(widget W) {
    if (W) do {
	if (IS_WINDOW(W))
	    return (window)W;
    } while ((W = W->Parent));
    return (window)0;
}

screen ScreenParent(widget W) {
    if (W) do {
	if (IS_SCREEN(W))
	    return (screen)W;
    } while ((W = W->Parent));
    return (screen)0;
}

widget NonScreenParent(widget W) {
    if (W) while (W->Parent) {
	if (IS_SCREEN(W->Parent))
	    return W;
	W = W->Parent;
    }
    return W;
}

window FindCursorWindow(void) {
    widget P, W;
    if (!(P = All->FirstScreen->FocusW))
	P = (widget)All->FirstScreen->MenuWindow;
    if (P) while ((W = P->SelectW))
	P = W;
    return WindowParent(P);
}

byte ContainsCursor(widget W) {
    widget C = (widget)FindCursorWindow();
    while (C) {
	if (C == W)
	    return TRUE;
	C = C->Parent;
    }
    return FALSE;
}

static void DrawAreaCtx(draw_ctx *D) {
    draw_ctx *FirstD = D;
    dat DWidth, DHeight, YLimit;
    screen FirstScreen, Screen;
    widget W, OnlyW, TopOnlyW, NextW;
    setup *SetUp;
    byte WidgetFound, Shade, lError=FALSE, FirstCycle=TRUE;
    byte DeltaXShade, DeltaYShade;
    ldat shLeft = 0, shUp = 0, shRgt = 0, shDwn = 0;
    /* position of Horizontal Shadow */
    ldat HS_X1, HS_X2, HS_Y1, S_Y2;
    /* position of Vertical Shadow */
    ldat VS_X1, VS_X2, VS_Y1;
    dat Left, Up, Rgt, Dwn;
    dat X1, Y1, X2, Y2;
    byte Shaded;
    
    if (QueuedDrawArea2FullScreen)
	return;
    
    SetUp=All->SetUp;
    
    do {
	W = D->TopW;
	OnlyW = D->W;
	FirstScreen = D->Screen;
	X1 = D->X1; Y1 = D->Y1;
	X2 = D->X2; Y2 = D->Y2;
	DWidth = D->DWidth;
	DHeight = D->DHeight;
	Shaded = D->Shaded;
	
	FirstD = D->Next;
	
	if (!FirstCycle)
	    FreeMem(D);
	else
	    FirstCycle = FALSE;
	
	if (X1>X2 || Y1>Y2 || X1>=DWidth || Y1>=DHeight || X2<0 || Y2<0)
	    continue;
	
	X1=Max2(X1, 0);
	Y1=Max2(Y1, 0);
	X2=Min2(X2,DWidth-1);
	Y2=Min2(Y2,DHeight-1);
	
	if (W) {
	    if (IS_SCREEN(W->Parent))
		FirstScreen = (screen)W->Parent;
	    else
		continue;
	}
	if (!FirstScreen)
	    FirstScreen = All->FirstScreen;
	
	/* calculate visible part of this screen */
	for (Screen = All->FirstScreen; Screen && Screen != FirstScreen; Screen = Screen->Next) {
	    YLimit = Screen->YLimit;
	    if (Y2 >= YLimit)
		Y2 = YLimit - 1;
	}
	if (Y1 > Y2)
	    continue;
	
	Screen = FirstScreen->Next;
	YLimit = FirstScreen->YLimit; /* FIXED +1 */
	
	if (!OnlyW || (OnlyW && ScreenParent(OnlyW->Parent)!=FirstScreen)) {
	    if (Y1<YLimit) {
		if (Screen)
		    _DrawAreaCtx_(&FirstD, Screen, W, OnlyW,
				  X1, Y1, X2, Min2(Y2, YLimit-1), Shaded, &lError);
		else if (!OnlyW)
		    DrawDesktop((screen)0, X1, Y1, X2, Y2, Shaded);
	    }
	    if (OnlyW)
		continue;
	}
	
	if (Y2<YLimit)
	    continue;
	
	if (Y1<YLimit)
	    Y1=YLimit;
	
	if (!W) {
	    W = FirstScreen->FirstW;
	    if (Y1==YLimit && !OnlyW) {
		Act(DrawMenu,FirstScreen)(FirstScreen, X1, X2);
		if (++Y1>Y2)
		    continue;
	    }
	}
		
	Shade=(SetUp->Flags & SETUP_SHADOWS) && !Shaded;
	DeltaXShade=Shade ? SetUp->DeltaXShade : (byte)0;
	DeltaYShade=Shade ? SetUp->DeltaYShade : (byte)0;

	WidgetFound=FALSE;
	while (W && !WidgetFound) {
	    if (W->Flags & WIDGETFL_NOTVISIBLE) {
		W=W->Next;
		continue;
	    }
	    shLeft = (ldat)W->Left + (ldat)FirstScreen->dummyLeft - FirstScreen->XLogic;
	    shUp   = (ldat)W->Up + (ldat)YLimit - FirstScreen->YLogic;
	    shRgt=shLeft + (ldat)W->XWidth-1;
	    shDwn=shUp + (IS_WINDOW(W) && (((window)W)->Attrib & WINDOW_ROLLED_UP)
		? 0 : (ldat)W->YWidth-1);
	    
	    if (Shade && IS_WINDOW(W)) {
		/* only windows have shadows */
		HS_X1 = Max2(shLeft + (ldat)DeltaXShade, (ldat)X1);
		HS_X2   = Min2(shRgt, (ldat)X2);
		HS_Y1 = Max2(shUp + (ldat)DeltaYShade, shDwn + (ldat)1);
		HS_Y1 = Max2(HS_Y1, (ldat)Y1);
		S_Y2    = Min2(shDwn + (ldat)DeltaYShade, (ldat)Y2);

		VS_X1 = Max2(shLeft + (ldat)DeltaXShade, shRgt + (ldat)1);
		VS_X1 = Max2(VS_X1, (ldat)X1);
		VS_X2   = Min2(shRgt + (ldat)DeltaXShade, (ldat)X2);
		VS_Y1 = Max2(shUp + (ldat)DeltaYShade, (ldat)Y1);
	    }
	    
	    if (shLeft<=(ldat)X2 && shRgt>=(ldat)X1 && shUp<=(ldat)Y2 && shDwn>=(ldat)Y1)
		WidgetFound=TRUE;
	    else if (Shade && IS_WINDOW(W) && ((HS_X1<=HS_X2 && HS_Y1 <= S_Y2)
					       || (VS_X1<=VS_X2 && VS_Y1 <= S_Y2)))
		WidgetFound=TRUE+TRUE;
	    else
		W=W->Next;
	}
	
	/* again, only windows have shadows */
	if (W && !IS_WINDOW(W)) {
	    Shade = FALSE;
	    DeltaXShade =  DeltaYShade = (byte)0;
	}
	
	if (!WidgetFound && !OnlyW) {
	    DrawDesktop(FirstScreen, X1, Y1, X2, Y2, Shaded);
	    continue;
	}
	
	TopOnlyW = OnlyW ? NonScreenParent(OnlyW) : NULL;
	
	if (WidgetFound==TRUE && (!OnlyW || TopOnlyW==W)) {
	    draw_ctx *FD = NULL;
	    _DrawWCtx_(&FD, W, NULL, OnlyW, shLeft, shUp, shRgt, shDwn,
		       Max2(X1, shLeft), Max2(Y1, shUp), Min2(X2, shRgt), Min2(Y2, shDwn),
		       FALSE, FALSE, Shaded, &lError);
	    if (FD) {
		DrawWCtx(FD);
		FreeMem(FD);
	    }
	}
	
	/* Draw the window's shadow : */
	
	NextW = W ? W->Next : W;
	
	if (WidgetFound && Shade && TopOnlyW!=W) {

	    if (HS_X1<=HS_X2 && HS_Y1 <= S_Y2) {
		if (NextW)
		    _DrawAreaCtx_(&FirstD, (screen)0, NextW, OnlyW, 
			       (dat)HS_X1, (dat)HS_Y1, (dat)HS_X2, (dat)S_Y2, TRUE, &lError);
		else
		    DrawDesktop(FirstScreen, (dat)HS_X1, (dat)HS_Y1, (dat)HS_X2, (dat)S_Y2, TRUE);
	    }
	    if (VS_X1<=VS_X2 && VS_Y1 <= S_Y2) {
		if (NextW)
		    _DrawAreaCtx_(&FirstD, (screen)0, NextW, OnlyW, 
			       (dat)VS_X1, (dat)VS_Y1, (dat)VS_X2, (dat)S_Y2, TRUE, &lError);
		else
		    DrawDesktop(FirstScreen, (dat)VS_X1, (dat)VS_Y1, (dat)VS_X2, (dat)S_Y2, TRUE);
	    }
	}
	
	
	if (TopOnlyW==W || (TopOnlyW && !NextW))
	    continue;
	

	/* Draw the visible area below the window : */
	
	if (shDwn+(ldat)DeltaYShade<(ldat)Y2) {
	    Dwn=(dat)shDwn+(dat)DeltaYShade;
	    if (NextW)
		_DrawAreaCtx_(&FirstD, (screen)0, NextW, OnlyW, X1, Dwn+(dat)1, X2, Y2, Shaded, &lError);
	    else
		DrawDesktop(FirstScreen, X1, Dwn+(dat)1, X2, Y2, Shaded);
	} else
	    Dwn=Y2;
	
	/* Draw the visible area right of the window : */
	
	if (shRgt+(ldat)DeltaXShade<(ldat)X2) {
	    Rgt=(dat)shRgt+(dat)DeltaXShade;
	    if (NextW)
		_DrawAreaCtx_(&FirstD, (screen)0, NextW, OnlyW, Rgt+(dat)1, Y1, X2, Dwn, Shaded, &lError);
	    else
		DrawDesktop(FirstScreen, Rgt+(dat)1, Y1, X2, Dwn, Shaded);
	} else
	    Rgt=X2;
	
	/* Draw the visible area left of the window : */
	
	if (shLeft>(ldat)X1) {
	    Left=(dat)shLeft;
	    if (NextW)
		_DrawAreaCtx_(&FirstD, (screen)0, NextW, OnlyW, X1, Y1, Left-(dat)1, Dwn, Shaded, &lError);
	    else
		DrawDesktop(FirstScreen, X1, Y1, Left-(dat)1, Dwn, Shaded);
	} else
	    Left=X1;
	
	/* Draw the visible area above the window : */
	
	if (shUp>(ldat)Y1) {
	    Up=(dat)shUp;
	    if (NextW)
		_DrawAreaCtx_(&FirstD, (screen)0, NextW, OnlyW, Left, Y1, Rgt, Up-(dat)1, Shaded, &lError);
	    else
		DrawDesktop(FirstScreen, Left, Y1, Rgt, Up-(dat)1, Shaded);
	} else
	    Up=Y1;
	
	if (Shade) {
	    
	    ldat X_1, Y_1, X_2, Y_2;
	    
	    X_1 = Max2(shLeft, (ldat)X1);
	    X_2 = Min2(shLeft+(ldat)DeltaXShade-(ldat)1, (ldat)X2);
	    Y_1 = Max2(shDwn+(ldat)1, (ldat)Y1);
	    Y_2 = Min2(shDwn+(ldat)DeltaYShade, (ldat)Y2);

	    /* Draw the visible area below the window, left of the shadow : */

	    if (X_1 <= X_2 && Y_1 <= Y_2) {
		if (NextW)
		    _DrawAreaCtx_(&FirstD, (screen)0, NextW, OnlyW, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, Shaded, &lError);
		else
		    DrawDesktop(FirstScreen, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, Shaded);
	    }
	    
	    /* Draw the visible area right of the window, above the shadow : */
	    
	    X_1 = Max2(shRgt+(ldat)1, (ldat)X1);
	    X_2 = Min2(shRgt+(ldat)DeltaXShade, (ldat)X2);
	    Y_1 = Max2(shUp, (ldat)Y1);
	    Y_2 = Min2(shUp+(ldat)DeltaYShade-(ldat)1, shDwn);
	    Y_2 = Min2(Y_2, (ldat)Y2);

	    if (X_1 <= X_2 && Y_1 <= Y2) {
		if (NextW)
		    _DrawAreaCtx_(&FirstD, (screen)0, NextW, OnlyW, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, Shaded, &lError);
		else
		    DrawDesktop(FirstScreen, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, Shaded);
	    }

	    /* Draw the visible area between the window and a floating shadow : */

	    if (shLeft+(ldat)DeltaXShade > shRgt+(ldat)1 || shUp+(ldat)DeltaYShade > shDwn+(ldat)1) {
		
		if (shUp+(ldat)DeltaYShade <= shDwn+(ldat)1) {
		    /*
		     * the shadow is like
		     * 
		     *  +--+BBBBBB
		     *  |  |CC+--+
		     *  +--+CC|  |
		     *  AAAAAA+--+
		     * 
		     *  parts A and B are already drawn, now draw C
		     */
		    X_1 = Max2(shRgt+(ldat)1, (ldat)X1);
		    X_2 = Min2(shLeft+(ldat)DeltaXShade-(ldat)1, (ldat)X2);
		    Y_1 = Max2(shUp+(ldat)DeltaYShade, (ldat)Y1);
		    Y_2 = Min2(shDwn, (ldat)Y2);
		} else {
		    /*
		     * the shadow is like
		     * 
		     *  +--+BBB        +--+BBBBB
		     *  |  |BBB	       |  |BBBBB
		     *  +--+BBB	       +--+BBBBB
		     *  AAACCCC	  or   AAAAACCCC
		     *  AAA+--+	       AAAAA+--+
		     *  AAA|  |	       AAAAA|  |
		     *  AAA+--+	       AAAAA+--+
		     *
		     * and we now draw C
		     */
		    X_1 = Max2(shLeft+(ldat)DeltaXShade, (ldat)X1);
		    X_2 = Min2(shRgt+(ldat)DeltaXShade, (ldat)X2);
		    Y_1 = Max2(shDwn+(ldat)1, (ldat)Y1);
		    Y_2 = Min2(shUp+(ldat)DeltaYShade-(ldat)1, (ldat)Y2);
		}
		
		if (X_1 <= X_2 && Y_1 <= Y2) {
		    if (NextW)
			_DrawAreaCtx_(&FirstD, (screen)0, NextW, OnlyW, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, Shaded, &lError);
		    else
			DrawDesktop(FirstScreen, (dat)X_1, (dat)Y_1, (dat)X_2, (dat)Y_2, Shaded);
		}
	    }
	}
    } while ((D = FirstD));
    
    if (lError)
	Error(lError);
}

void DrawArea2(screen FirstScreen, widget W, widget OnlyW,
	      dat X1, dat Y1, dat X2, dat Y2, byte Shaded) {
    draw_ctx D;
    
    if (QueuedDrawArea2FullScreen)
	return;
    
    D.DWidth = All->DisplayWidth;
    D.DHeight = All->DisplayHeight;
    
    D.X1 = Max2(X1, 0);
    D.Y1 = Max2(Y1, 0);
    D.X2 = Min2(X2, D.DWidth  - 1);
    D.Y2 = Min2(Y2, D.DHeight - 1);

    if (D.X1 <= D.X2 && D.Y1 <= D.Y2) {
	D.Next = (draw_ctx *)0;
	D.W = OnlyW; D.TopW = W;
	D.Screen = FirstScreen;
	D.Shaded = Shaded;
	
        DrawAreaCtx(&D);
    }
}

void DrawBorderWindow(window Window, byte Flags) {
    ldat shLeft, shUp, shRgt, shDwn;
    dat Left, Up, Rgt, Dwn;
    dat DWidth, DHeight;
    dat YLimit;
    widget FirstW;
    widget Parent;
    screen Screen, FirstScreen;

    if (QueuedDrawArea2FullScreen || !Window ||
	(Window->Flags & (WINDOWFL_BORDERLESS|WINDOWFL_NOTVISIBLE)) ||
	!(Parent=Window->Parent) || !IS_SCREEN(Parent))
	return;

    Screen = (screen)Parent;
    DWidth = All->DisplayWidth;
    DHeight = All->DisplayHeight;
    YLimit = Screen->YLimit; /* FIXED +1 */

    shUp = (ldat)Window->Up - Screen->YLogic + (ldat)YLimit;
    shLeft = (ldat)Window->Left - Screen->XLogic;
    shRgt = shLeft+(ldat)Window->XWidth-(ldat)1;
    shDwn = shUp+(Window->Attrib & WINDOW_ROLLED_UP ? 0 : (ldat)Window->YWidth-(ldat)1);
    
    
    if (shLeft>=(ldat)DWidth || shUp>=(ldat)DHeight || shRgt<(ldat)0 || shDwn<=(ldat)YLimit)
	return;

    FirstScreen= Screen==All->FirstScreen ? Screen : (screen)0;
    FirstW= FirstScreen && (widget)Window==Screen->FirstW ? (widget)Window : (widget)0;
    
    Left=(dat)Max2(shLeft, (ldat)0);
    Up=(dat)Max2(shUp, (ldat)YLimit+1);
    Rgt=(dat)Min2(shRgt, (ldat)DWidth-(ldat)1);
    Dwn=(dat)Min2(shDwn, (ldat)DHeight-(ldat)1);
    
    if ((Flags&BORDER_UP)     && shUp>=YLimit)
	DrawArea2(FirstScreen, FirstW, (widget)Window, Left, Up, Rgt, Up, FALSE);
    if (!(Window->Flags & WINDOW_ROLLED_UP)) {
	if ((Flags&BORDER_LEFT)   && shLeft>=0)
	    DrawArea2(FirstScreen, FirstW, (widget)Window, Left, Up, Left, Dwn, FALSE);
	if ((Flags&BORDER_RIGHT)  && shRgt<(ldat)DWidth)
	    DrawArea2(FirstScreen, FirstW, (widget)Window, Rgt, Up, Rgt, Dwn, FALSE);
	if ((Flags&BORDER_DOWN)   && shDwn<(ldat)DHeight)
	    DrawArea2(FirstScreen, FirstW, (widget)Window, Left, Dwn, Rgt, Dwn, FALSE);
    }
}

void DrawAreaShadeWindow(screen Screen, window Window, dat X1, dat Y1, dat X2, dat Y2,
			 ldat shLeft, ldat shUp, ldat shRgt, ldat shDwn, byte Internal) {
    dat DHeight, DWidth, YLimit;
    /* position of Horizontal Shadow */
    ldat HS_X1, HS_X2, HS_Y1, S_Y2;
    /* position of Vertical Shadow */
    ldat VS_X1, VS_X2, VS_Y1;
    byte DeltaXShade, DeltaYShade;
    setup *SetUp;
    
    SetUp=All->SetUp;
    if (QueuedDrawArea2FullScreen || !Window ||
	(Window->Flags & WINDOWFL_NOTVISIBLE) || !Screen ||
	X1>X2 || Y1>Y2 || !(SetUp->Flags & SETUP_SHADOWS))
	return;
    
    DeltaXShade=SetUp->DeltaXShade;
    DeltaYShade=SetUp->DeltaYShade;

    DWidth = All->DisplayWidth;
    DHeight = All->DisplayHeight;
    YLimit = Screen->YLimit;
    
    X1=Max2(X1, 0);
    Y1=Max2(Y1, YLimit + 1);
    X2=Min2(X2, DWidth  - 1);
    Y2=Min2(Y2, DHeight - 1);
    
    if (shLeft+(ldat)DeltaXShade>(ldat)X2 || shUp+(ldat)DeltaYShade>(ldat)Y2 || shRgt+(ldat)DeltaXShade<(ldat)X1 || shDwn+(ldat)DeltaYShade<(ldat)Y1)
	return;

    Window=(window)Window->Next;

    HS_X1 = Max2(shLeft + (ldat)DeltaXShade, (ldat)X1);
    HS_X2 = Min2(shRgt, (ldat)X2);
    HS_Y1 = Max2(shUp + (ldat)DeltaYShade, shDwn + (ldat)1);
    HS_Y1 = Max2(HS_Y1, (ldat)Y1);
    S_Y2  = Min2(shDwn + (ldat)DeltaYShade, (ldat)Y2);

    VS_X1 = Max2(shLeft + (ldat)DeltaXShade, shRgt + (ldat)1);
    VS_X1 = Max2(VS_X1, (ldat)X1);
    VS_X2 = Min2(shRgt + (ldat)DeltaXShade, (ldat)X2);
    VS_Y1 = Max2(shUp + (ldat)DeltaYShade, (ldat)Y1);

    if (HS_X1<=HS_X2 && HS_Y1 <= S_Y2) {
	if (!Internal)
	    DrawArea2((screen)0, (widget)0, (widget)0, (dat)HS_X1, (dat)HS_Y1, (dat)HS_X2, (dat)S_Y2, FALSE);
	else if (Window)
	    DrawArea2((screen)0, (widget)Window, (widget)0, (dat)HS_X1, (dat)HS_Y1, (dat)HS_X2, (dat)S_Y2, TRUE);
	else
	    DrawDesktop(Screen, (dat)HS_X1, (dat)HS_Y1, (dat)HS_X2, (dat)S_Y2, TRUE);
    }
    
    if (VS_X1<=VS_X2 && VS_Y1 <= S_Y2) {
	if (!Internal)
	    DrawArea2((screen)0, (widget)0, (widget)0, (dat)VS_X1, (dat)VS_Y1, (dat)VS_X2, (dat)S_Y2, FALSE);
	else if (Window)
	    DrawArea2((screen)0, (widget)Window, (widget)0, (dat)VS_X1, (dat)VS_Y1, (dat)VS_X2, (dat)S_Y2, TRUE);
	else
	    DrawDesktop(Screen, (dat)VS_X1, (dat)VS_Y1, (dat)VS_X2, (dat)S_Y2, TRUE);
    }
}

void DrawShadeWindow(window Window, dat X1, dat Y1, dat X2, dat Y2, byte Internal) {
    screen Screen;
    dat YLimit;
    ldat shLeft, shUp, shRgt, shDwn;
    
    if (!QueuedDrawArea2FullScreen && Window && (Screen=(screen)Window->Parent) && IS_SCREEN(Screen)) {
	
	YLimit=Screen->YLimit;
	shUp=(ldat)Window->Up - Screen->YLogic + (ldat)YLimit; 
	shLeft=(ldat)Window->Left - Screen->XLogic;
	shRgt=shLeft+(ldat)Window->XWidth-(ldat)1;
	shDwn=shUp+(Window->Attrib & WINDOW_ROLLED_UP ? 0 : (ldat)Window->YWidth-(ldat)1);
	
	DrawAreaShadeWindow(Screen, Window, X1, Y1, X2, Y2, shLeft, shUp, shRgt, shDwn, Internal);
    }
}



/* replaces DrawAreaWindow() */
void DrawAreaWindow2(window W) {
    draw_ctx D;
    byte Dvalid = FALSE;
    if (!QueuedDrawArea2FullScreen && W && W->Parent && IS_SCREEN(W->Parent)) {
	if ((widget)W == All->FirstScreen->FirstW) {
	    DrawWidget((widget)W, 0, 0, MAXDAT, MAXDAT, FALSE);
	    
	} else if (InitDrawCtx((widget)W, 0, 0, MAXDAT, MAXDAT, FALSE, &D)){
	    Dvalid = TRUE;
	    D.W = D.TopW = NULL;
	    DrawAreaCtx(&D);
	}
	if (All->SetUp->Flags & SETUP_SHADOWS) {
	    if (!Dvalid)
		InitDrawCtx((widget)W, 0, 0, MAXDAT, MAXDAT, FALSE, &D);
	    DrawAreaShadeWindow((screen)W->Parent, W, 0, 0, MAXDAT, MAXDAT,
				D.Left, D.Up, D.Rgt, D.Dwn, FALSE);
	}
    }
}

/* replaces DrawAbsoluteWindow() */
void DrawPartialWidget(widget W, dat X1, dat Y1, dat X2, dat Y2) {
    draw_ctx D;
    if (!QueuedDrawArea2FullScreen && W && InitDrawCtx(W, X1, Y1, X2, Y2, FALSE, &D)) {
	/*
	 * DO NOT assume W is completely visible...
	 * might be obscured by another window! So reset D.TopW
	 * Assume instead that who called us wants to draw the visible part of W,
	 * not whatever covers it. So set D.W (used as OnlyW)
	 */
	D.TopW = NULL; D.W = W;
	DrawAreaCtx(&D);
    }
}

/* replaces DrawTextWindow() */
void DrawLogicWidget(widget W, ldat X1, ldat Y1, ldat X2, ldat Y2) {
    ldat XL, YL;
    byte HasBorder;
    
    if (!QueuedDrawArea2FullScreen && W && !(W->Flags & WIDGETFL_NOTVISIBLE) &&
	(!IS_WINDOW(W) || !(((window)W)->Attrib & WINDOW_ROLLED_UP)) && X2>=X1 && Y2>=Y1) {
	XL = W->XLogic;
	YL = W->YLogic;
    
	HasBorder = IS_WINDOW(W) && !(((window)W)->Flags & WINDOWFL_BORDERLESS);
	
	if (X2>=XL && Y2>=YL && Y1<MAXLDAT &&
	    X1<XL+(ldat)W->XWidth-2*HasBorder &&
	    Y1<YL+(ldat)W->YWidth-2*HasBorder) {

	    X1 = Max2(X1-XL, 0); X1 = Min2(X1+HasBorder, MAXDAT);
	    X2 = Min2(X2, XL+(ldat)W->XWidth-1-2*HasBorder);
	    X2 = Max2(X2-XL, 0); X2 = Min2(X2+HasBorder, MAXDAT);
	    
	    Y1 = Max2(Y1-YL, 0); Y1 = Min2(Y1+HasBorder, MAXDAT);
	    Y2 = Min2(Y2, YL+(ldat)W->YWidth-1-2*HasBorder);
	    Y2 = Max2(Y2-YL, 0); Y2 = Min2(Y2+HasBorder, MAXDAT);
	    
	    DrawPartialWidget(W, (dat)X1, (dat)Y1, (dat)X2, (dat)Y2);
	}
    }
}


void ReDrawRolledUpAreaWindow(window Window, byte Shaded) {
    ldat shLeft, shUp, shRgt, shDwn;
    byte Shade, DeltaXShade, DeltaYShade;
    screen Screen;
    dat DWidth, DHeight;
    dat YLimit;
    
    if (QueuedDrawArea2FullScreen || !Window ||
	(Window->Flags & WINDOWFL_NOTVISIBLE) ||
	!Window->Parent || !IS_SCREEN(Window->Parent))
	return;

    Screen = (screen)Window->Parent;

    Shade = All->SetUp->Flags & SETUP_SHADOWS;
    DeltaXShade = Shade ? All->SetUp->DeltaXShade : (byte)0;
    DeltaYShade = Shade ? All->SetUp->DeltaYShade : (byte)0;
	
    DWidth = All->DisplayWidth;
    DHeight = All->DisplayHeight;
    YLimit=Screen->YLimit;
    shUp=(ldat)Window->Up - Screen->YLogic + (ldat)YLimit;
    shLeft=(ldat)Window->Left - Screen->XLogic;
    shRgt=shLeft+(ldat)Window->XWidth-(ldat)1;
    shDwn=shUp+(ldat)Window->YWidth-(ldat)1;
    /*shDwn=shUp+(Window->Attrib & WINDOW_ROLLED_UP ? 0 : (ldat)Window->YWidth-(ldat)1);*/
	
    if (shLeft>=(ldat)DWidth || shUp>=(ldat)DHeight || shRgt<-(ldat)DeltaXShade || shDwn<(ldat)YLimit-(ldat)DeltaYShade)
	return;
    
    shLeft=Max2((ldat)0, shLeft);
    shUp=Max2((ldat)0, shUp + 1);
    shRgt=Min2((ldat)DWidth-(ldat)1, shRgt+(ldat)DeltaXShade);
    shDwn=Min2((ldat)DHeight-(ldat)1, shDwn+(ldat)DeltaYShade);
    
    DrawArea2((screen)0, (widget)0, (widget)0, (dat)shLeft, (dat)shUp, (dat)shRgt, (dat)shDwn, Shaded);
}


void DrawMenuScreen(screen Screen, dat Xstart, dat Xend) {
#ifdef CONF__UNICODE
    static hwattr extra;
#endif
    screen fScreen;
    menu Menu;
    menuitem Item;
    dat DWidth, DHeight, i, j, x;
    hwattr Attr;
    hwfont Font;
    hwcol Color;
    byte Select, State, MenuInfo;
    
    if (QueuedDrawArea2FullScreen || !Screen || !Screen->All || Xstart>Xend)
	return;
    
    j = Screen->YLimit;
    DWidth = All->DisplayWidth;
    DHeight = All->DisplayHeight;
    
    if (j<0 || j>=DHeight || Xstart>=DWidth || Xstart > Xend)
	return;
    
    for (fScreen = All->FirstScreen; fScreen && fScreen != Screen; fScreen = fScreen->Next) {
	if (fScreen->YLimit <= j)
	    return;
    }
    if (fScreen != Screen)
	return;
	
    State = All->State & STATE_ANY;
    Menu = Act(FindMenu,Screen)(Screen);
    
    MenuInfo = State != STATE_MENU && (All->SetUp->Flags & SETUP_MENU_INFO);
    
    Xstart = Max2(Xstart, 0);
    Xend   = Min2(Xend, DWidth-1);
    

#ifdef CONF__UNICODE	
    if (!extra)
	extra = EncodeToHWAttrExtra(POS_MENU, 0, 0, 0);
#endif

    for (i=Xstart; i<=Xend; i++) {
	if (i+2>=DWidth) {
	    Color = State == STATE_SCREEN ? Menu->ColSelShtCut : Menu->ColShtCut;
	    if (XAND(Screen->Flags, SCREENFL_BACK_SELECT|SCREENFL_BACK_PRESSED))
		Color = COL( COLBG(Color), COLFG(Color));
	    Font = Screen_Back[2-(DWidth-i)];
	}
	else if (DWidth-i<=(dat)3+lenTWDisplay) {
	    Color = State == STATE_SCREEN ? Menu->ColSelShtCut : Menu->ColShtCut;
	    Font = TWDisplay[3 + lenTWDisplay - (DWidth - i)];
	    if (!Font) Font = ' ';
	}
	else if (DWidth-i>(dat)9 && DWidth-i<=(dat)9+All->BuiltinRow->Len) {
	    Color = State == STATE_SCREEN ? Menu->ColSelect : Menu->ColItem;
	    Font = All->BuiltinRow->Text[9+All->BuiltinRow->Len - (DWidth-i)];
	    if (!Font) Font = ' ';
	}
	else if (MenuInfo && FindInfo(Menu, i)) {
	    Select = State == STATE_SCREEN;
	    FindFontInfo(Menu, i, Select, &Attr);
	    Font = HWFONT(Attr);
	    Color = HWCOL(Attr);
	}
	else {
	    if (!MenuInfo) {
		Item = Act(FindItem,Menu)(Menu, i);
		
		if (Item) {
		    /* check if Item is from All->CommonMenu */
		    if ((menu)Item->Parent == All->CommonMenu && Menu->LastI)
			x = Menu->LastI->Left + Menu->LastI->Len;
		    else
			x = 0;

		    Select = State == STATE_SCREEN ||
			(State == STATE_MENU && ((menu)Item->Parent)->SelectI == Item);
		    /*
		     * CHEAT: Item may be in CommonMenu, not in Menu...
		     * steal Item color from Menu.
		     */
		    FindFontMenuItem(Menu, Item, i - x, Select, &Attr);
		    Font = HWFONT(Attr);
		    Color = HWCOL(Attr);
		}
	    }
	    if (MenuInfo || !Item) {
		Color = State == STATE_SCREEN ? Menu->ColSelect : Menu->ColItem;
		Font = ' ';
	    }
	}
	if (Screen != All->FirstScreen)
	    Color = Menu->ColDisabled;
#ifdef CONF__UNICODE
	Video[i+j*DWidth]=HWATTR_EXTRA32(HWATTR(Color, Font), extra);
#else
	Video[i+j*DWidth]=HWATTR(Color, Font);
#endif
    }
    DirtyVideo(Xstart, j, Xend, j);    
}

void DrawScreen(screen Screen) {
    screen FirstScreen;
    if (!QueuedDrawArea2FullScreen) {
	FirstScreen= Screen==All->FirstScreen ? Screen : (screen)0;
	DrawArea2(FirstScreen, (widget)0, (widget)0, 0, (dat)Screen->YLimit, MAXDAT, MAXDAT, FALSE);
    }
}

void ClearHilight(window Window) {
    if (Window->State & WINDOW_DO_SEL) {
	Window->State &= ~WINDOW_DO_SEL;
	if (Window->YendSel > Window->YstSel)
	    DrawLogicWidget((widget)Window, Window->XLogic, Window->YstSel, Window->XLogic+Window->XWidth, Window->YendSel);
	else
	    DrawLogicWidget((widget)Window, Window->XstSel, Window->YstSel, Window->XendSel, Window->YstSel);
    }
}

void StartHilight(window Window, ldat XSel, ldat YSel) {
    if (Window) {
	ClearHilight(Window);
	Window->State &= ~WINDOW_ANYSEL;
	Window->State |= WINDOW_FWDSEL;
	Window->XstSel = Window->XendSel = XSel;
	Window->YstSel = Window->YendSel = YSel;
    }
}

void ExtendHilight(window Window, ldat XSel, ldat YSel) {
    ldat oX, oY;
    uldat oState;
    
    if (!Window)
	return;

    if (!(Window->State & WINDOW_DO_SEL)) {
	if (Window->State & WINDOW_ANYSEL) {
	    if (Window->State & WINDOW_FWDSEL)
		StartHilight(Window, Window->XstSel, Window->YstSel);
	    else if (Window->State & WINDOW_REVSEL)
		StartHilight(Window, Window->XendSel, Window->YendSel);
	    Window->State |= WINDOW_DO_SEL;
	    DrawLogicWidget((widget)Window, Window->XstSel, Window->YstSel, Window->XendSel, Window->YstSel);
	} else
	    return;
    }
	
    if (Window->State & WINDOW_FWDSEL) {
	oX = Window->XendSel;
	oY = Window->YendSel;
    } else {
	oX = Window->XstSel;
	oY = Window->YstSel;
    }
    oState = Window->State;
    
    if ((Window->State & WINDOW_FWDSEL) &&
	 (YSel < Window->YstSel || (YSel == Window->YstSel && XSel < Window->XstSel))) {

	Window->State ^= WINDOW_ANYSEL;
	Window->XendSel = Window->XstSel;
	Window->YendSel = Window->YstSel;
    } else if ((Window->State & WINDOW_REVSEL) &&
	 (YSel > Window->YendSel || (YSel == Window->YendSel && XSel > Window->XendSel))) {

	Window->State ^= WINDOW_ANYSEL;
	Window->XstSel = Window->XendSel;
	Window->YstSel = Window->YendSel;
    }

    if (Window->State & WINDOW_FWDSEL) {
	Window->XendSel = XSel;
	Window->YendSel = YSel;
    } else {
	Window->XstSel = XSel;
	Window->YstSel = YSel;
    }

    /*
    printf("%c%c %d %d %d %d\n", (Window->State & WINDOW_DO_SEL) ? '*' : ' ',
	   (Window->State & WINDOW_FWDSEL) ? '+' : '-',
	   Window->XstSel, Window->YstSel, Window->XendSel, Window->YendSel);
    */
    
    if (YSel == oY && ((oState & WINDOW_ANYSEL) == (Window->State & WINDOW_ANYSEL)))
	DrawLogicWidget((widget)Window, Min2(XSel, oX), YSel, Max2(XSel, oX), YSel);
    else
	DrawLogicWidget((widget)Window, Window->XLogic, Min2(YSel, oY), Window->XLogic+Window->XWidth, Max2(YSel, oY));
}
