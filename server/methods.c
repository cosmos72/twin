/*
 *  methods.c  --  functions for OO-style calls on twin data structures
 *                 (windows, menus, ...)
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
#include "builtin.h"
#include "methods.h"
#include "data.h"
#include "extensions.h"

#include "draw.h"
#include "resize.h"
#include "util.h"
#include "hw.h"
#include "hw_multi.h"

#ifdef CONF_TERM
# include "tty.h"
#endif
#ifdef CONF__MODULES
# include "dl.h"
#endif
#ifdef CONF_WM
# include "wm.h"
#endif

/* some object-oriented ones not included in fn_obj */

void *OverrideMth(void **where, void *OrigMth, void *NewMth) {
    if (*where == OrigMth) {
	*where = NewMth;
	return NewMth;
    }
    return NULL;
}

#if 0 /* not used */
INLINE void DeletePartialList(obj Obj) {
    obj Next;
    while (Obj) {
	Next = Obj->Next;
	Delete(Obj);
	Obj = Next;
    }
}
#endif

INLINE void InsertGeneric(obj Obj, obj_parent Parent, obj Prev, obj Next, ldat *ObjCount) {
    if (Obj->Prev || Obj->Next)
	return;

    if (Prev)
	Next=Prev->Next;
    else if (Next)
	Prev=Next->Prev;
    
    if ((Obj->Prev=Prev))
	Prev->Next=Obj;
    else
	Parent->First=Obj;
    
    if ((Obj->Next=Next))
	Next->Prev=Obj;
    else
	Parent->Last=Obj;
    
    if (ObjCount)
	(*ObjCount)++;
}

INLINE void RemoveGeneric(obj Obj, obj_parent Parent, ldat *ObjCount) {
    if (Obj->Prev)
	Obj->Prev->Next=Obj->Next;
    else if (Parent->First == Obj)
	Parent->First=Obj->Next;
    
    if (Obj->Next)
	Obj->Next->Prev=Obj->Prev;
    else if (Parent->Last == Obj)
	Parent->Last=Obj->Prev;
    
    Obj->Prev=Obj->Next=(obj)0;
    if (ObjCount)
	(*ObjCount)--;
}

/* fn_obj and others fn_XXX functions */

/* obj */

static obj CreateObj(fn_obj Fn_Obj) {
    obj Obj;
    
    if ((Obj=(obj)AllocMem(Fn_Obj->Size))) {
	if (AssignId(Fn_Obj, Obj)) {
	    (Obj->Fn = Fn_Obj)->Used++;
	    Obj->Prev = Obj->Next = (obj)0;
	    Obj->Parent = (obj_parent)0;
	} else {
	    FreeMem(Obj);
	    Obj = (obj)0;
	}
    }
    return Obj;
}

static void InsertObj(obj Obj, obj_parent Parent, obj Prev, obj Next) {
    if (!Obj->Parent && Parent) {
	InsertGeneric(Obj, Parent, Prev, Next, (ldat *)0);
	Obj->Parent = Parent;
    }
}

static void RemoveObj(obj Obj) {
    if (Obj->Parent) {
	RemoveGeneric(Obj, Obj->Parent, (ldat *)0);
	Obj->Parent = (obj_parent)0;
    }
}

static void DeleteObj(obj Obj) {
    /* not a good idea to RemoveObj(Obj) here */
    DropId(Obj);
    if (!--Obj->Fn->Used)
	FreeMem(Obj->Fn);
    FreeMem(Obj);
}

static struct s_fn_obj _FnObj = {
    obj_magic, sizeof(struct s_obj), (uldat)1,
    CreateObj,
    InsertObj,
    RemoveObj,
    DeleteObj
};

/* widget */


static widget CreateWidget(fn_widget Fn_Widget, msgport Owner, dat XWidth, dat YWidth, hwattr Fill, dat Left, dat Up) {
    widget W = (widget)0;

    if (Owner && (W=(widget)Fn_Widget->Fn_Obj->Create((fn_obj)Fn_Widget))) {
	Fn_Widget->Fn_Obj->Used++;
	
	W->FirstW = W->LastW = W->SelectW = (widget)0;
	W->Left = Left;
	W->Up   = Up;
	W->XWidth = XWidth;
	W->YWidth = YWidth;
	W->XLogic = W->YLogic = 0;
	W->OPrev = W->ONext = (widget)0;
	W->Owner = (msgport)0;
	
	Act(Own,W)(W, Owner);
    }
    return W;
}
    
static void InsertWidget(widget W, widget Parent, widget Prev, widget Next) {
    if (Parent)
	/*
	 * don't check W->Parent here, as MakeFirst() and MakeLast() call
	 * RemoveWidget() then InsertWidget() but RemoveWidget() does not reset W->Parent
	 */
	InsertGeneric((obj)W, (obj_parent)&Parent->FirstW, (obj)Prev, (obj)Next, (ldat *)0);
}

static void RemoveWidget(widget W) {
    if (W->Parent)
	RemoveGeneric((obj)W, (obj_parent)&W->Parent->FirstW, (ldat *)0);
}

static void DeleteWidget(widget W) {
    fn_obj Fn_Obj = W->Fn->Fn_Obj;
	
    Act(UnMap,W)(W);
    Act(DisOwn,W)(W);
    while (W->FirstW)
	Act(UnMap,W->FirstW)(W->FirstW);

    (Fn_Obj->Delete)((obj)W);
    if (!--Fn_Obj->Used)
	FreeMem(Fn_Obj);
}

static void SetFillWidget(widget W, hwattr Fill) {
    if (W->Fill != Fill) {
	W->Fill = Fill;
	if (W->Parent)
	    DrawAreaWidget(W);
    }
}

static gadget FindGadgetByCode(widget Parent, udat Code) {
    widget W;
    
    for (W = Parent->FirstW; W; W = W->Next) {
	if (IS_GADGET(W) && ((gadget)W)->Code == Code)
	    return (gadget)W;
    }
    return (gadget)0;
}

static void MapWidget(widget W, widget Parent) {
    if (Parent && !W->Parent) {
	if (W->Up == MAXDAT) {
	    W->Left = Parent->XLogic;
	    W->Up = Parent->YLogic;
	}
	
	InsertLast(W, W, Parent);
	W->Parent = Parent;
	
	DrawAreaWidget(W);
    }
}

static void UnMapWidget(widget W) {
    widget Parent;
    
    if ((Parent = W->Parent)) {
	if (W == Parent->SelectW)
	    Parent->SelectW = (widget)0;
	
	Remove(W);
	DrawAreaWidget(W);
	W->Parent = (widget)0;
    }
}

static void SetXYWidget(widget W, dat X, dat Y) {
    widget Prev, Next;
    
    if (W->Parent) {
	Prev = W->Prev;
	Next = W->Next;
	Remove(W);
	DrawAreaWidget(W);
    }
    W->Left = X;
    W->Up = Y;
    if (W->Parent) {
	InsertMiddle(W, W, W->Parent, Prev, Next);
	DrawAreaWidget(W);
    }
}

static void OwnWidget(widget Widget, msgport Owner) {
    if (!Widget->Owner) {
	if ((Widget->ONext = Owner->FirstW))
	    Owner->FirstW->OPrev = Widget;
	else
	    Owner->LastW=Widget;	
    
	Widget->OPrev = (widget)0;
	Owner->FirstW = Widget;
	Widget->Owner = Owner;
    }
}

static void DisOwnWidget(widget W) {
    msgport Owner;
    if ((Owner = W->Owner)) {
	if (W->OPrev)
	    W->OPrev->ONext = W->ONext;
	else if (Owner->FirstW == W)
	    Owner->FirstW = W->ONext;
	
	if (W->ONext)
	    W->ONext->OPrev = W->OPrev;
	else if (Owner->LastW == W)
	    Owner->LastW = W->OPrev;
	
	W->OPrev = W->ONext = (widget)0;
	W->Owner = (msgport)0;
    }
}

static void RecursiveDeleteWidget(widget W, msgport maybeOwner) {
    while (W->FirstW)
	Act(RecursiveDelete,W->FirstW)(W->FirstW, maybeOwner);
    
    if (W->Owner == maybeOwner)
	Delete(W);
    else
	Act(UnMap,W)(W);
}


static struct s_fn_widget _FnWidget = {
    widget_magic, sizeof(struct s_widget), (uldat)1,
    CreateWidget,
    InsertWidget,
    RemoveWidget,
    DeleteWidget,
    &_FnObj,
    DrawSelfWidget, /* exported by draw.c */
    FindWidgetAt,   /* exported by draw.c */
    FindGadgetByCode,
    SetXYWidget,
    SetFillWidget,
    MapWidget,
    UnMapWidget,
    OwnWidget,
    DisOwnWidget,
    RecursiveDeleteWidget
};


/* gadget */

static gadget CreateGadget
    (fn_gadget Fn_Gadget, widget Parent, dat XWidth, dat YWidth, CONST byte *TextNormal,
     udat Code, udat Flags,
     hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
     dat Left, dat Up,       CONST byte *TextSelect, CONST byte *TextDisabled, CONST byte *TextSelectDisabled,
     CONST hwcol *ColNormal, CONST hwcol *ColSelect, CONST hwcol *ColDisabled, CONST hwcol *ColSelectDisabled)
{
    gadget G = (gadget)0;
    ldat Size;

    if (Parent && Parent->Owner && Code < COD_RESERVED && XWidth > 0 && YWidth > 0 && TextNormal &&
	(G=(gadget)Fn_Gadget->Fn_Widget->Create
	 ((fn_widget)Fn_Gadget, Parent->Owner, XWidth, YWidth, (hwattr)0, Left, Up))) {
	
	Fn_Gadget->Fn_Widget->Used++;

	G->ColText=ColText;
	G->ColSelect=ColTextSelect;
	G->ColDisabled=ColTextDisabled;
	G->ColSelectDisabled=ColTextSelectDisabled;
	G->Code=Code;
	G->Flags=Flags;

	G->GPrev = G->GNext = (gadget)0;
	G->Group = (group)0;
	
	Size = (ldat)XWidth*YWidth;
	G->Text[0] = CloneMem(TextNormal, Size);
		
	if (TextSelect)
	    G->Text[1] = CloneMem(TextSelect, Size);
	else
	    G->Text[1] = NULL;
	
	if (TextDisabled)
	    G->Text[2] = CloneMem(TextDisabled, Size);
	else
	    G->Text[2] = NULL;
	
	if (TextSelectDisabled)
	    G->Text[3] = CloneMem(TextSelectDisabled, Size);
	else
	    G->Text[3] = NULL;
	 
	if (ColNormal)
	    G->Color[0] = CloneMem(ColNormal, Size*sizeof(hwcol));
	else
	    G->Color[0] = NULL, G->Flags |= GADGET_USE_DEFCOL;
	
	if (ColSelect)
	    G->Color[1] = CloneMem(ColSelect, Size*sizeof(hwcol));
	else
	    G->Color[1] = NULL;
	    
	if (ColDisabled)
	    G->Color[2] = CloneMem(ColDisabled, Size*sizeof(hwcol));
	else
	    G->Color[2] = NULL;
	    
	if (ColSelectDisabled)
	    G->Color[3] = CloneMem(ColSelectDisabled, Size*sizeof(hwcol));
	else
	    G->Color[3] = NULL;
	
	Act(Map,G)(G, Parent);
    }
    return G;
}

static void DeleteGadget(gadget G) {
    fn_widget Fn_Widget = G->Fn->Fn_Widget;
    byte i;
    
    Act(UnMap,G)(G);
    for (i=0; i<4; i++) {
	if (G->Text[i])
	    FreeMem(G->Text[i]);
	if (G->Color[i])
	    FreeMem(G->Color[i]);
    }
    (Fn_Widget->Delete)((widget)G);
    if (!--Fn_Widget->Used)
	FreeMem(Fn_Widget);
}

static gadget CreateEmptyButton(fn_gadget Fn_Gadget, msgport Owner, dat XWidth, dat YWidth, hwcol BgCol) {
    gadget G;
    ldat Size;
    byte i;
    dat j, k;
    
    if ((G=(gadget)Fn_Gadget->Fn_Widget->Create
	 ((fn_widget)Fn_Gadget, Owner, ++XWidth, ++YWidth, (hwattr)0, 0, 0))) {
	
	Size = (ldat)XWidth * YWidth;
	
	for (i=0; i<4; i++)
	    G->Text[i] = G->Color[i] = NULL;
	
	for (i=0; i<4; i++)
	    if (!(G->Text[i]=(byte *)AllocMem(Size)) ||
		!(G->Color[i]=(byte *)AllocMem(Size * sizeof(hwcol)))) {
		
		Act(Delete,G)(G);
		return (gadget)0;
	    }

	Fn_Gadget->Fn_Widget->Used++;
	
	Size = (ldat)--XWidth * --YWidth;
	BgCol &= COL(0,MAXCOL);
	
	for (i=0; i<4; i++) {
	    for (j=k=(dat)0; j<YWidth; j++, k+=XWidth+1) {
		G->Text [i][k+(i & 1 ? 0 : XWidth)] = i & 1 ? ' ' : k ? 'Û' : 'Ü';
		G->Color[i][k+(i & 1 ? 0 : XWidth)] = BgCol;
	    }
	    G->Text[i][k] = ' ';
	    WriteMem((void *)(G->Text [i]+k+1), i & 1 ? ' ' : 'ß', XWidth);
	    WriteMem((void *)(G->Color[i]+k), BgCol, (XWidth+1) * sizeof(hwcol));
	}
	
	G->GPrev = G->GNext = (gadget)0;
	G->Group = (group)0;
    }
    return G;
}

byte FillButton(gadget G, widget Parent, udat Code, dat Left, dat Up,
		udat Flags, CONST byte *Text, hwcol Color, hwcol ColorDisabled) {
    dat i, j, XWidth, YWidth;
    byte CONST *T;
    
    if (Code >= COD_RESERVED)
	return FALSE;

    G->Code = Code;
    G->Left = Left;
    G->Up   = Up;
    G->Flags = Flags | GADGET_BUTTON;
    XWidth = G->XWidth;
    YWidth = G->YWidth;
    
    T = Text;
    for (j=(dat)0; j<(YWidth-(dat)1)*XWidth; j+=XWidth) {
	for (i=(dat)0; i<XWidth-(dat)1; i++) {
	    G->Text[0][i+j] = G->Text[1][i+j+1] = G->Text[2][i+j] = G->Text[3][i+j+1] = *(T++);
	    
	    G->Color[0][i+j] = G->Color[1][i+j+1] = Color;
	    G->Color[2][i+j] = G->Color[3][i+j+1] = ColorDisabled;
	}
    }
    if (Parent)
	Act(Map,G)(G, Parent);
    
    return TRUE;
}


static gadget CreateButton(fn_gadget Fn_Gadget, widget Parent, dat XWidth, dat YWidth, CONST byte *Text,
			    udat Code, udat Flags, hwcol BgCol, hwcol Col, hwcol ColDisabled,
			    dat Left, dat Up) {
    gadget G;
    if (Parent && (G = Fn_Gadget->CreateEmptyButton(Fn_Gadget, Parent->Owner, XWidth, YWidth, BgCol))) {
	if (Act(FillButton,G)(G, Parent, Code, Left, Up, Flags, Text, Col, ColDisabled))
	    return G;
	Act(Delete,G)(G);
	G = (gadget)0;
    }
    return G;
}


static struct s_fn_gadget _FnGadget = {
    gadget_magic, sizeof(struct s_gadget), (uldat)1,
    CreateGadget,
    (void *)InsertWidget,
    (void *)RemoveWidget,
    DeleteGadget,
    /* widget */
    &_FnObj,
    DrawSelfGadget,       /* exported by draw.c */
    (void *)FindWidgetAt, /* exported by draw.c */
    (void *)FindGadgetByCode,
    (void *)SetXYWidget,
    (void *)SetFillWidget,
    (void *)MapWidget,
    (void *)UnMapWidget,
    (void *)OwnWidget,
    (void *)DisOwnWidget,
    (void *)RecursiveDeleteWidget,
    /* gadget */
    &_FnWidget,
    CreateEmptyButton,
    FillButton,
    CreateButton,
    WriteTextGadget,	/* exported by resize.c */
};

/* ttydata */

static byte InitTtyData(window Window) {
    ttydata *Data = Window->TtyData;
    ldat count = Window->MaxNumRow * Window->NumRowOne;
    hwattr *p = Window->Contents;
    
    if (!Data && !(Window->TtyData = Data = AllocMem(sizeof(ttydata))))
	return FALSE;

    if (!p && !(Window->Contents = p = AllocMem(count * sizeof(hwattr))))
	return FALSE;
    
    while (count--)
	*p++ = HWATTR( COL(WHITE,BLACK), ' ');
    
    /*
     * this is a superset of reset_tty(),
     * but we don't want to call it from here
     */
    Data->State = ESnormal;
    Data->Flags = TTY_AUTOWRAP;
    Data->Effects = 0;
    Window->YLogic = Window->CurY = Data->ScrollBack = Window->MaxNumRow - (Window->YWidth-2);
    Window->NumRowSplit = 0;
    Data->SizeX = Window->NumRowOne;
    Data->SizeY = Window->YWidth - 2;
    Data->Top = 0;
    Data->Bottom = Data->SizeY;
    Data->saveX = Data->X = Window->CurX = 0;
    Data->saveY = Data->Y = 0;
    Data->Pos = Data->Start = Window->Contents + Data->ScrollBack * Window->NumRowOne;
    Data->Split = Window->Contents + Window->MaxNumRow * Window->NumRowOne;
    
    Window->CursorType = LINECURSOR;
    /* respect the WINFL_CURSOR_ON set by the client and don't force it on */
#if 0
    Window->Flags |= WINFL_CURSOR_ON;
#endif
    Window->ColText = Data->Color = Data->DefColor = Data->saveColor = COL(WHITE,BLACK);
    Data->Underline = COL(HIGH|WHITE,BLACK);
    Data->HalfInten = COL(HIGH|BLACK,BLACK);
    Data->TabStop[0] = 0x01010100;
    Data->TabStop[1] = Data->TabStop[2] = Data->TabStop[3] = Data->TabStop[4] = 0x01010101;
    Data->nPar = 0;

    Data->G = Data->saveG = 0;
    /*
     * this probably violates some standard, 
     * but starting with the identity mapping
     * seems the only reasonable choice to me
     */
    Data->currG = Data->G0 = Data->saveG0 = IBMPC_MAP;
    Data->G1 = Data->saveG1 = GRAF_MAP;

    Data->newLen = Data->newMax = 0;
    Data->newTitle = NULL;
    
    return TRUE;
}

/* window */

static window CreateWindow(fn_window Fn_Window, dat LenTitle, CONST byte *Title, CONST hwcol *ColTitle,
			    menu Menu, hwcol ColText, uldat CursorType, uldat Attrib, byte Flags,
			    dat XWidth, dat YWidth, dat ScrollBackLines) {

    window Window = (window)0;
    byte *_Title = NULL;
    hwcol *_ColTitle = NULL;
    
    if ((!Title || (_Title=CloneStrL(Title, LenTitle))) &&
	(!ColTitle || (_ColTitle=CloneMem(ColTitle, LenTitle*sizeof(hwcol)))) &&
	Menu && Menu->MsgPort &&
	(Window=(window)Fn_Window->Fn_Widget->Create
	 ((fn_widget)Fn_Window, Menu->MsgPort, XWidth, YWidth, (hwattr)0, 0, MAXDAT))) {
	Window->Menu = Menu;
	Window->LenTitle = LenTitle;
	Window->Title = _Title;
	Window->ColTitle = _ColTitle;
	Window->BorderPattern[0] = Window->BorderPattern[1] = (void *)0;
	Window->TtyData = (ttydata *)0;
	Window->RemoteData.Fd = NOFD;
	Window->RemoteData.ChildPid = NOPID;
	Window->RemoteData.FdSlot = NOSLOT;
	Window->CurX = Window->CurY = 0;
	Window->XstSel = Window->YstSel = Window->XendSel = Window->YendSel = 0;
        Window->ColGadgets = DEFAULT_ColGadgets;
	Window->ColArrows = DEFAULT_ColArrows;
	Window->ColBars = DEFAULT_ColBars;
	Window->ColTabs = DEFAULT_ColTabs;
	Window->ColBorder = DEFAULT_ColBorder;
	Window->ColText = ColText;
	Window->ColSelect = COL(COLBG(ColText),COLFG(ColText));
	Window->ColDisabled = DEFAULT_ColDisabled;
	Window->ColSelectDisabled = DEFAULT_ColSelectDisabled;
	/* sanity: */
	if ((Window->Flags = Flags) & WINFL_BORDERLESS)
	    Attrib &= ~WINDOW_ROLLED_UP;
	if (Attrib & WINDOW_WANT_KEYS)
	    Attrib &= ~WINDOW_AUTO_KEYS;
	
	Window->Attrib = Attrib;
	Window->CursorType = CursorType;

	Window->MinXWidth=MIN_XWIN;
	Window->MinYWidth=MIN_YWIN;
	Window->XWidth = XWidth = Max2(MIN_XWIN, XWidth);
	Window->YWidth = YWidth = Max2(MIN_YWIN, YWidth);
	Window->MaxXWidth = MAXDAT;
	Window->MaxYWidth = MAXDAT;

	Window->Contents=(hwattr *)0;
	Window->FirstRow=Window->LastRow=Window->RowOne=Window->RowSplit=(row)0;
	Window->MaxNumRow=Window->NumRowOne=Window->NumRowSplit=(ldat)0;

	Window->ShutDownHook = (fn_hook)0;
	Window->Hook = (fn_hook)0;
	Window->WhereHook = (fn_hook *)0;
	Window->MapUnMapHook = (fn_hook)0;
	Window->MapQueueMsg = (msg)0;

	if (Flags & WINFL_USECONTENTS) {
	    if (MAXDAT - ScrollBackLines < YWidth - 2)
		ScrollBackLines = MAXDAT - YWidth + 2;
	    Window->CurY = Window->YLogic = ScrollBackLines;
	    Window->MaxNumRow = ScrollBackLines + YWidth-2;
	    Window->NumRowOne = XWidth-2;
	    if (!InitTtyData(Window)) {
		Act(Delete,Window)(Window);
		Window = (window)0;
	    }
	}
	
	Fn_Window->Fn_Widget->Used++;
	return Window;
    }
    if (_Title)
	FreeMem(_Title);
    if (_ColTitle)
	FreeMem(_ColTitle);
    return Window;
}

static void DeleteWindow(window W) {
    fn_widget Fn_Widget = W->Fn->Fn_Widget;
    
    Act(UnMap,W)(W);
    if (W->Hook)
	Act(RemoveHook,W)(W, W->Hook, W->WhereHook);
    if (W->ShutDownHook)
	W->ShutDownHook(W);
    if (W->Title)
	FreeMem(W->Title);
    if (W->ColTitle)
	FreeMem(W->ColTitle);
    if (W->TtyData)
	FreeMem(W->TtyData);
    if (W->Contents)
	FreeMem(W->Contents);
    DeleteList(W->FirstRow);
	
    (Fn_Widget->Delete)((widget)W);
    if (!--Fn_Widget->Used)
	FreeMem(Fn_Widget);
}

static void SetColTextWindow(window W, hwcol ColText) {
    W->ColText = ColText;
}

static void SetColorsWindow(window W, udat Bitmap,
			    hwcol ColGadgets, hwcol ColArrows, hwcol ColBars, hwcol ColTabs, hwcol ColBorder,
			    hwcol ColText, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled) {
    if (Bitmap & 1)
	W->ColGadgets = ColGadgets;
    if (Bitmap & 2)
	W->ColArrows = ColArrows;
    if (Bitmap & 4)
	W->ColBars = ColBars;
    if (Bitmap & 8)
	W->ColTabs = ColTabs;
    if (Bitmap & 0x10)
	W->ColBorder = ColBorder;
    if (Bitmap & 0x20)
	W->ColText = ColText;
    if (Bitmap & 0x40)
	W->ColSelect = ColSelect;
    if (Bitmap & 0x80)
	W->ColDisabled = ColDisabled;
    if (Bitmap & 0x100)
	W->ColSelectDisabled = ColSelectDisabled;
    if (W->Parent)
	DrawBorderWindow(W, BORDER_ANY);
}

static void SetXYWindow(window W, dat X, dat Y) {
    widget Prev, Next;
    
    if (W->Parent) {
	Prev = W->Prev;
	Next = W->Next;
	Remove(W);
	DrawAreaWindow2(W);
    }
    W->Left = X;
    W->Up = Y;
    if (W->Parent) {
	InsertMiddle(W, W, W->Parent, Prev, Next);
	DrawAreaWindow2(W);
    }
}

static void ConfigureWindow(window W, byte Bitmap, dat Left, dat Up,
			    dat MinXWidth, dat MinYWidth, dat MaxXWidth, dat MaxYWidth) {
    widget Prev, Next;

    if (W->Parent) {
	Prev = W->Prev;
	Next = W->Next;
	Remove(W);
	DrawAreaWindow2(W);
    }
	
    if (Bitmap & 1)
	W->Left = Left;
    if (Bitmap & 2)
	W->Up = Up;
    if (Bitmap & 4) {
	W->MinXWidth = MinXWidth;
	W->XWidth = Max2(MinXWidth, W->XWidth);
    }
    if (Bitmap & 8) {
	W->MinYWidth = MinYWidth;
	W->YWidth = Max2(MinYWidth, W->YWidth);
    }
    if (Bitmap & 0x10) {
	W->MaxXWidth = Max2(W->MinXWidth, MaxXWidth);
	W->XWidth = Min2(MaxXWidth, W->XWidth);
    }
    if (Bitmap & 0x20) {
	W->MaxYWidth = Max2(W->MinYWidth, MaxYWidth);
	W->YWidth = Min2(MaxYWidth, W->YWidth);
    }
    if (W->Parent) {
	InsertMiddle(W, W, W->Parent, Prev, Next);
	DrawAreaWindow2(W);
    }
}

static void GotoXYWindow(window Window, ldat X, ldat Y) {
    if (Window->Flags & WINFL_USECONTENTS) {
	if (X >= Window->NumRowOne)
	    X = Window->NumRowOne - 1;
	if (Y >= Window->TtyData->SizeY)
	    Y = Window->TtyData->SizeY - 1;
	Window->TtyData->X = X;
	Window->TtyData->Y = Y;
	Window->TtyData->Pos = Window->TtyData->Start + X + (ldat)Y * Window->TtyData->SizeX;
	if (Window->TtyData->Pos >= Window->TtyData->Split)
	    Window->TtyData->Pos -= Window->TtyData->Split - Window->Contents;
	Y += Window->MaxNumRow - Window->TtyData->SizeY;
    }
    Window->CurX = X;
    Window->CurY = Y;
    if (All->FirstScreen->FocusWindow == Window)
	UpdateCursor();
}

window Create4MenuWindow(fn_window Fn_Window, menu Menu) {
    window Window = (window)0;
    if (Menu && (Window=Fn_Window->Create
		 (Fn_Window, 0, NULL, (hwcol *)0, Menu, COL(BLACK,WHITE),
		  NOCURSOR, WINDOW_MENU|WINDOW_AUTO_KEYS, WINFL_USE_DEFCOL|WINFL_SEL_ROWCURR,
		  MIN_XWIN, MIN_YWIN, 0))) {
	
	Act(SetColors,Window)(Window, 0x1FF, COL(0,0), COL(0,0), COL(0,0), COL(0,0), COL(HIGH|WHITE,WHITE),
			      COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK));
	Act(Configure,Window)(Window, 0x3F, 0, 1, MIN_XWIN, MIN_YWIN, MAXDAT, MAXDAT);
    }
    return Window;
}

static void MapTopRealWindow(window Window, screen Screen) {
    window OldWindow;
    
    if (Screen && Window->Menu && !Window->Parent) {
	if (Window->MapQueueMsg)
	    /*
	     * let the upper layer do this:
	     * Delete(Window->MapQueueMsg);
	     */
	    Window->MapQueueMsg = (msg)0;
	
	if (Window->Up == MAXDAT) {
	    Window->Left = Screen->XLogic;
	    Window->Up = Max2(Screen->YLimit+1, 0) + Screen->YLogic;
	}
	
	InsertFirst(W, Window, (widget)Screen);
	Window->Parent = (widget)Screen;
	
	if (Screen == All->FirstScreen) {
	    OldWindow = Act(KbdFocus,Window)(Window);
	    if (OldWindow)
		DrawBorderWindow(OldWindow, BORDER_ANY);
	    UpdateCursor();
	}
	DrawAreaWindow2(Window);
	if (!(Window->Attrib & WINDOW_MENU))
	    Act(DrawMenu,Screen)(Screen, 0, MAXDAT);

	if (Window->MapUnMapHook)
	    Window->MapUnMapHook(Window);
	
	if (Screen->FnHookWindow)
	    Screen->FnHookWindow(Screen->HookWindow);
    }
}

static void MapWindow(window W, widget Parent) {
    msg Msg;
    
    if (!W->Parent && !W->MapQueueMsg && Parent) {
	if (IS_SCREEN(Parent)) {
	    if (Ext(WM,MsgPort) && (Msg = Do(Create,Msg)(FnMsg, MSG_MAP, sizeof(event_map)))) {
		Msg->Event.EventMap.Window = W;
		Msg->Event.EventMap.Code   = 0;
		Msg->Event.EventMap.Screen = (screen)Parent;
		W->MapQueueMsg = Msg;
		SendMsg(Ext(WM,MsgPort), Msg);
	    } else
		Act(MapTopReal,W)(W, (screen)Parent);
	} else if (IS_WIDGET(Parent)) {
	    W->Fn->Fn_Widget->Map((widget)W, (widget)Parent);
	    if (W->MapUnMapHook)
		W->MapUnMapHook(W);
	}
    }
}

static void UnMapWindow(window W) {
    widget Parent;
    window Next;
    screen Screen;
    byte wasFocus;
    
    if ((Parent = W->Parent)) {
	if (IS_SCREEN(Parent)) {
	    if ((Screen = (screen)Parent) == All->FirstScreen &&
		(All->State & STATE_ANY) == STATE_MENU) {

		/* take care... menu is active */
		if (W == Screen->MenuWindow || 
		    (W == Screen->FocusWindow && (W->Attrib & WINDOW_MENU))) {
		
		    /*
		     * ! DANGER ! 
		     * Trying to UnMap() either the menu window or the menu owner.
		     * shutdown the menu first!
		     */
		    ChangeMenuFirstScreen((menuitem)0, FALSE, DISABLE_MENU_FLAG);
		    
		    if (W->Attrib & WINDOW_MENU)
			/* all done */
			return;
		    /* else we still must UnMap() the original window */
		}
	    }
	    
	    if (Screen->ClickWindow == W)
		Screen->ClickWindow = NULL;
	
	    if ((wasFocus = W == Screen->FocusWindow)) {
		if (W->Attrib & WINDOW_MENU)
		    Next = Screen->MenuWindow;
		else {
		    if ((widget)W == Screen->FirstW)
			Next = (window)W->Next;
		    else
			Next = (window)Screen->FirstW;
		    
		    while (Next && !IS_WINDOW(Next))
			Next = (window)Next->Next;
		}
	    }
	    
	    Remove(W);
	    DrawAreaWindow2(W);
	    W->Parent = (widget)0;
	    W->Left = 0;
	    W->Up = MAXDAT;

	    if (wasFocus) {
		if (Screen == All->FirstScreen) {
		    /*
		     * in case the user was dragging this window...
		     * or using it as menu :-P
		     */
		    if ((All->State & STATE_ANY) <= STATE_MENU)
			All->State &= ~STATE_ANY;
		    
		    if (Next) {
			(void)Act(KbdFocus,Next)(Next);
			DrawBorderWindow(Next, BORDER_ANY);
		    } else
			Do(KbdFocus,Window)((window)0);
		    if (!(W->Attrib & WINDOW_MENU))
			Act(DrawMenu,Screen)(Screen, 0, MAXDAT);
		    UpdateCursor();
		} else
		    Screen->FocusWindow = Next;
	    }

	    if (W->MapUnMapHook)
		W->MapUnMapHook(W);

	    if (Screen->FnHookWindow)
		Screen->FnHookWindow(Screen->HookWindow);
	
	} else {
	    /* UnMap() a sub-window */
	    W->Fn->Fn_Widget->UnMap((widget)W);
	    
	    if (W->MapUnMapHook)
		W->MapUnMapHook(W);
	}
    } else if (W->MapQueueMsg) {
	/* the window was still waiting to be mapped! */
	Delete(W->MapQueueMsg);
	W->MapQueueMsg = (msg)0;
    }
}

static byte InstallHookWindow(window Window, fn_hook Hook, fn_hook *WhereHook) {
    if (Window && !Window->Hook && !Window->WhereHook && Hook &&
	WhereHook && !WhereHook[0] && !WhereHook[1]) {
	
	Window->Hook = WhereHook[0] = Hook;
	Window->WhereHook = WhereHook;
	WhereHook[1] = (void *)Window;
	return TRUE;
    }
    return FALSE;
}

static void RemoveHookWindow(window Window, fn_hook Hook, fn_hook *WhereHook) {
    if (Window && Hook && Window->Hook == Hook &&
	WhereHook && Window->WhereHook == WhereHook &&
	WhereHook[0] == Hook && WhereHook[1] == (void *)Window) {
	
	Window->Hook = *WhereHook = (fn_hook)0;
	Window->WhereHook = (fn_hook *)0;
	WhereHook[1] = (void *)0;
    }
}


window FocusWindow(window W) {
    window oldW;
    if (W)
	oldW = Act(KbdFocus,W)(W);
    else
	oldW = Do(KbdFocus,Window)(W);
    
    if (W != oldW && (!W || W->Parent == (widget)All->FirstScreen)) {
	if (W) DrawBorderWindow(W, BORDER_ANY);
	if (oldW) DrawBorderWindow(oldW, BORDER_ANY);
	if (W || oldW) {
	    UpdateCursor();
	    if (!W || !(W->Attrib & WINDOW_MENU))
		Act(DrawMenu,All->FirstScreen)(All->FirstScreen, 0, MAXDAT);
	}
    }
    return oldW;
}

#if !defined(CONF_TERM)
window FakeKbdFocus(window W) {
    window oldW;
    widget P;
    screen Screen;
    
    if (W && (P = W->Parent) && IS_SCREEN(P)) {
	Screen = (screen)P;
	oldW = Screen->FocusWindow;
	Screen->FocusWindow = W;
    } else
	oldW = (window)0;
    
    return oldW;
}
#endif

#ifndef CONF_TERM
# ifdef CONF__MODULES
void FakeWriteAscii(window Window, ldat Len, CONST byte *Text) {
    if (DlLoad(TermSo) && Window->Fn->WriteAscii != FakeWriteAscii)
	Act(WriteAscii,Window)(Window, Len, Text);
}

void FakeWriteHWAttr(window Window, dat x, dat y, ldat Len, CONST hwattr *Attr) {
    if (DlLoad(TermSo) && Window->Fn->WriteHWAttr != FakeWriteHWAttr)
	Act(WriteHWAttr,Window)(Window, x, y, Len, Attr);
}

window FakeOpenTerm(CONST byte *arg0, byte * CONST *argv) {
    if (DlLoad(TermSo) && Ext(Term,Open) != FakeOpenTerm)
	return Ext(Term,Open)(arg0, argv);
    return NULL;
}
# else  /* CONF__MODULES */
#  define FakeWriteAscii  (void (*)(window, ldat, CONST byte *))NoOp
#  define FakeWriteHWAttr (void (*)(window, dat, dat, ldat, CONST hwattr *))NoOp
# endif /* CONF__MODULES */
#endif  /* CONF_TERM */


#ifdef CONF_WM
byte WMFindBorderWindow(window Window, dat u, dat v, byte Border, byte *PtrChar, hwcol *PtrColor);

#else
byte FakeFindBorderWindow(window W, dat u, dat v, byte Border, byte *PtrChar, hwcol *PtrColor) {
    byte Horiz, Vert;
    
    Horiz = u ? u+1 == W->XWidth ? (byte)2 : (byte)1 : (byte)0;
    Vert  = v ? v+1 == W->YWidth ? (byte)2 : (byte)1 : (byte)0;

    if (*PtrChar)
	*PtrChar = StdBorder[!!(All->SetUp->Flags & SETUP_ALTFONT)][Border][Vert*3+Horiz];
    if (*PtrColor)
	*PtrColor = W->ColBorder;
    
    return v ? POS_ROOT : POS_TITLE;
}
#endif /* CONF_WM */


static row FindRow(window Window, ldat Row) {
    row CurrRow, ElPossib[4];
    byte Index;
    ldat k, ElNumRows[4], ElDist[4];
    
    ElPossib[0] = Window->RowOne;
    ElPossib[1] = Window->RowSplit;
    ElPossib[2] = Window->FirstRow;
    ElPossib[3] = Window->LastRow;
    ElNumRows[0] = Window->NumRowOne;
    ElNumRows[1] = Window->NumRowSplit;
    ElNumRows[2] = (ldat)0;
    ElNumRows[3] = Window->MaxNumRow-(ldat)1;
    ElDist[0] = (ElNumRows[0] ? Abs(ElNumRows[0]-Row) : MAXULDAT);
    ElDist[1] = (ElNumRows[1] ? Abs(ElNumRows[1]-Row) : MAXULDAT);
    ElDist[2] = Row;
    ElDist[3] = Abs(ElNumRows[3]-Row);
    
    Index = Minimum((byte)4, ElDist);
    CurrRow=ElPossib[Index];
    k=ElNumRows[Index];
    
    if (CurrRow) {
	if (k<Row)
	    while (k<Row && (CurrRow=CurrRow->Next))
		k++;
	else if (k>Row)
	    while (k>Row && (CurrRow=CurrRow->Prev))
		k--;
    }
    return CurrRow;
}

static row FindRowByCode(window Window, udat Code, ldat *NumRow) {
    row Row;
    ldat Num=(ldat)0;
    
    if ((Row=Window->FirstRow))
	while (Row && Row->Code!=Code) {
	    Row=Row->Next;
	    Num++;
	}
    if (Row && NumRow)
	*NumRow=Num;

    return Row;
}


static struct s_fn_window _FnWindow = {
    window_magic, sizeof(struct s_window), (uldat)1,
    CreateWindow,
    (void *)InsertWidget,
    (void *)RemoveWidget,
    DeleteWindow,
    /* widget */
    &_FnObj,
    DrawSelfWindow,
    (void *)FindWidgetAt,
    (void *)FindGadgetByCode,
    SetXYWindow,
    (void *)SetFillWidget,
    MapWindow,
    UnMapWindow,
    (void *)OwnWidget,
    (void *)DisOwnWidget,
    (void *)RecursiveDeleteWidget,
    /* window */
    &_FnWidget,
#ifdef CONF_TERM
    TtyWriteAscii,
    TtyWriteHWAttr,
    TtyKbdFocus,
#else
    FakeWriteAscii,
    FakeWriteHWAttr,
    FakeKbdFocus,
#endif
    FocusWindow,
    GotoXYWindow,
    WriteRow,	/* exported by resize.c */
    SetColTextWindow,
    SetColorsWindow,
    ConfigureWindow,
    Create4MenuWindow,
    MapTopRealWindow,
#ifdef CONF_WM
    WMFindBorderWindow,
#else
    FakeFindBorderWindow,
#endif
    FindRow,
    FindRowByCode,
    InstallHookWindow,
    RemoveHookWindow
};




/* screen */

static screen CreateScreen(fn_screen Fn_Screen, dat LenTitle, CONST byte *Title,
			    dat BgWidth, dat BgHeight, CONST hwattr *Bg) {
    screen S = (screen)0;
    size_t size;
    
    if ((size=(size_t)BgWidth * BgHeight * sizeof(hwattr))) {
	
	if ((S=(screen)Fn_Screen->Fn_Widget->Create
	     ((fn_widget)Fn_Screen, Builtin_MsgPort, MAXDAT, MAXDAT, (hwattr)0, 0, 0))) {

	    if (!(S->Title=NULL, Title) || (S->Title=CloneStrL(Title, LenTitle))) {
		if ((S->Bg = AllocMem(size))) {

		    Fn_Screen->Fn_Widget->Used++;
		    
		    S->LenTitle = LenTitle;
		    S->FocusWindow = S->MenuWindow = S->ClickWindow = NULL;
		    S->HookWindow = NULL;
		    S->FnHookWindow = NULL;
		    S->Attrib = 0;
		    S->BgWidth = BgWidth;
		    S->BgHeight = BgHeight;
		    CopyMem(Bg, S->Bg, size);
		    S->All = (all)0;
		    
		    return S;
		}
		if (S->Title) FreeMem(S->Title);
	    }
	    (Fn_Screen->Fn_Widget->Delete)((widget)S);
	}
    }
    return NULL;
}

static screen CreateSimpleScreen(fn_screen Fn_Screen, dat LenTitle, CONST byte *Title, hwattr Bg) {
    return Fn_Screen->Create(Fn_Screen, LenTitle, Title, 1, 1, &Bg);
}

static void BgImageScreen(screen Screen, dat BgWidth, dat BgHeight, CONST hwattr *Bg) {
    size_t size;
    
    if (Screen && Bg && (size = (size_t)BgWidth * BgHeight * sizeof(hwattr)) &&
	(Screen->Bg = ReAllocMem(Screen->Bg, size))) {

	Screen->BgWidth = BgWidth;
	Screen->BgHeight = BgHeight;
	CopyMem(Bg, Screen->Bg, size);
	DrawArea2((screen)0, (widget)0, (widget)0, 0, Screen->YLimit+1, MAXDAT, MAXDAT, FALSE);
    }
}

static void InsertScreen(screen Screen, all Parent, screen Prev, screen Next) {
    if (!Screen->All && Parent) {
	InsertGeneric((obj)Screen, (obj_parent)&Parent->FirstScreen, (obj)Prev, (obj)Next, (ldat *)0);
	Screen->All = Parent;
    }
}

static void RemoveScreen(screen Screen) {
    if (Screen->All) {
	RemoveGeneric((obj)Screen, (obj_parent)&Screen->All->FirstScreen, (ldat *)0);
	Screen->All = (all)0;
    }
}

static void DeleteScreen(screen Screen) {
    fn_widget Fn_Widget = Screen->Fn->Fn_Widget;

    while (Screen->FirstW)
	Act(UnMap,Screen->FirstW)(Screen->FirstW);

    Remove(Screen);
    (Fn_Widget->Delete)((widget)Screen);
    if (!--Fn_Widget->Used)
	FreeMem(Fn_Widget);
}

static menu FindMenuScreen(screen Screen) {
    if (Screen) {
	if (Screen->MenuWindow)
	    /* menu activated from Screen->MenuWindow, return its menu */
	    return Screen->MenuWindow->Menu;

	/* no window activated the menu... either the menu is inactive
	 * or it is activated from the builtin menu */
	    
	if (Screen->FocusWindow && Screen->FocusWindow->Menu != All->CommonMenu) 
	    /* menu inactive... return the focus window's one */
	    return Screen->FocusWindow->Menu;
	    
	/* last case: menu activated from builtin menu */
	return All->BuiltinMenu;
    }
    return (menu)0;
}

static screen FindScreen(dat j) {
    screen CurrScreen;
    byte VirtScrFound = FALSE;
    
    CurrScreen = All->FirstScreen;
    while (CurrScreen &&
	  !(VirtScrFound = (j >= (dat)CurrScreen->YLimit)))
	CurrScreen = CurrScreen->Next;
    
    if (VirtScrFound)
	return CurrScreen;
    
    return (screen)0;
}

    

static void FocusScreen(screen tScreen) {
    screen Screen = All->FirstScreen;
    if (tScreen && Screen != tScreen) {
	MoveFirst(Screen, All, tScreen);
	DrawArea2((screen)0, (widget)0, (widget)0,
		 0, Min2(Screen->YLimit, tScreen->YLimit), MAXDAT, MAXDAT, FALSE);
	UpdateCursor();
    }
}

static void ActivateMenuScreen(screen Screen, menuitem Item, byte byMouse) {
    menu Menu;
    
    if ((All->State & STATE_ANY) != STATE_DEFAULT)
	return;
    
    if (Screen && Screen != All->FirstScreen)
	Act(Focus,Screen)(Screen);
    
    Menu = Act(FindMenu,Screen)(Screen);

    ChangeMenuFirstScreen(Item, byMouse, ACTIVATE_MENU_FLAG);
}

static void DrawMenuScreen(screen Screen, dat Xstart, dat Xend) {
    screen fScreen;
    menu Menu;
    menuitem Item;
    dat DWidth, DHeight, i, j, x;
    byte Color, Font, Select, State, MenuInfo;
    
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
    
    MenuInfo = State != STATE_MENU && (All->SetUp->Flags & SETUP_MENUINFO);
    
    Xstart = Max2(Xstart, 0);
    Xend   = Min2(Xend, DWidth-1);
    
    for (i=Xstart; i<=Xend; i++) {
	if (i+2>=DWidth) {
	    Color = State == STATE_SCREEN ? Menu->ColSelShtCut : Menu->ColShtCut;
	    if (XAND(Screen->Attrib, GADGET_BACK_SELECT|GADGET_PRESSED))
		Color = COL( COLBG(Color), COLFG(Color));
	    Font = Screen_Back[2-(DWidth-i)];
	}
	else if (DWidth-i<=(dat)3+lenTWDisplay) {
	    Color = State == STATE_SCREEN ? Menu->ColSelShtCut : Menu->ColShtCut;
	    Font = TWDisplay[3 + lenTWDisplay - (DWidth - i)];
	    if (!Font) Font = ' ';
	}
	else if (MenuInfo && FindInfo(Menu, i)) {
	    Select = State == STATE_SCREEN;
	    FindFontInfo(Menu, i, Select, &Font, &Color);
	}
	else {
	    if (!MenuInfo) {
		Item = Act(FindItem,Menu)(Menu, i);
		
		if (Item) {
		    /* check if Item is from All->CommonMenu */
		    if (Item->Menu == All->CommonMenu && Menu->LastMenuItem)
			x = Menu->LastMenuItem->Left + Menu->LastMenuItem->Len;
		    else
			x = 0;

		    Select = State == STATE_SCREEN ||
			(State == STATE_MENU && Item->Menu->MenuItemSelect == Item);
		    /*
		     * CHEAT: Item may be in CommonMenu, not in Menu...
		     * steal Item color from Menu.
		     */
		    FindFontMenuItem(Menu, Item, i - x, Select, &Font, &Color);
		}
	    }
	    if (MenuInfo || !Item) {
		Color = State == STATE_SCREEN ? Menu->ColSelect : Menu->ColItem;
		Font = ' ';
	    }
	}
	if (Screen != All->FirstScreen)
	    Color = Menu->ColDisabled;
	Video[i+j*DWidth]=HWATTR(Color, Font);
    }
    DirtyVideo(Xstart, j, Xend, j);    
}

static void DeActivateMenuScreen(screen Screen) {
    if (Screen == All->FirstScreen && (All->State & STATE_ANY) == STATE_MENU)
	ChangeMenuFirstScreen((menuitem)0, FALSE, DISABLE_MENU_FLAG);
}

static struct s_fn_screen _FnScreen = {
    screen_magic, sizeof(struct s_screen), (uldat)1,
    CreateScreen,
    InsertScreen,
    RemoveScreen,
    DeleteScreen,
    /* widget */
    &_FnObj,
    DrawSelfScreen,
    (void *)FindWidgetAt,
    (void *)FindGadgetByCode,
    (void *)SetFillWidget,
    (void *)NoOp, /* MapWidget */
    (void *)NoOp, /* UnMapWidget */
    (void *)OwnWidget,
    (void *)DisOwnWidget,
    (void *)RecursiveDeleteWidget,
    /* screen */
    &_FnWidget,
    FindMenuScreen,
    FindScreen,
    CreateSimpleScreen,
    BgImageScreen,
    FocusScreen,
    DrawMenuScreen,
    ActivateMenuScreen,
    DeActivateMenuScreen
};



/* group */

static group CreateGroup(fn_group Fn_Group, msgport MsgPort) {
    group Group = (group)0;

    if (MsgPort && (Group=(group)Fn_Group->Fn_Obj->Create((fn_obj)Fn_Group))) {
	Fn_Group->Fn_Obj->Used++;
	
	Group->FirstG = Group->LastG = Group->SelectG = (gadget)0;
	Group->MsgPort = (msgport)0;
	
	InsertLast(Group, Group, MsgPort);
    }
    return Group;
}
    
static void InsertGroup(group Group, msgport MsgPort, group Prev, group Next) {
    if (!Group->MsgPort && MsgPort) {
	InsertGeneric((obj)Group, (obj_parent)&MsgPort->FirstGroup, (obj)Prev, (obj)Next, (ldat *)0);
	Group->MsgPort = MsgPort;
    }
}

static void RemoveGroup(group Group) {
   if (Group->MsgPort) {
       RemoveGeneric((obj)Group, (obj_parent)&Group->MsgPort->FirstGroup, (ldat *)0);
       Group->MsgPort = (msgport)0;
   }
}

static void DeleteGroup(group Group) {
    fn_obj Fn_Obj = Group->Fn->Fn_Obj;
	
    Act(Remove,Group)(Group);
    while (Group->FirstG)
	Act(RemoveGadget, Group)(Group, Group->FirstG);

    (Fn_Obj->Delete)((obj)Group);
    if (!--Fn_Obj->Used)
	FreeMem(Fn_Obj);
}

static void InsertGadgetGroup(group Group, gadget G) {
    if (G && !G->Group) {
	InsertGeneric((obj)G, (obj_parent)&Group->FirstG, (obj)Group->LastG, (obj)0, (ldat *)0);
	G->Group = Group;
    }
}

static void RemoveGadgetGroup(group Group, gadget G) {
   if (G && G->Group == Group) {
       RemoveGeneric((obj)G, (obj_parent)&G->Group->FirstG, (ldat *)0);
       G->Group = (group)0;
   }
}


static gadget GetSelectedGadget(group Group) {
    return Group->SelectG;
}

static void SetSelectedGadget(group Group, gadget G) {
    if (!G || (G && G->Group == Group)) {
	if (Group->SelectG)
	    UnPressGadget(Group->SelectG, TRUE);
	if (G)
	    PressGadget(G);
    }
}

static struct s_fn_group _FnGroup = {
    group_magic, sizeof(struct s_group), (uldat)1,
    CreateGroup,
    InsertGroup,
    RemoveGroup,
    DeleteGroup,
    &_FnObj,
    InsertGadgetGroup,
    RemoveGadgetGroup,
    GetSelectedGadget,
    SetSelectedGadget
};


/* row */

static row CreateRow(fn_row Fn_Row, udat Code, byte Flags) {
    row Row = (row)0;
    
    if (Code < COD_RESERVED && (Row=(row)Fn_Row->Fn_Obj->Create((fn_obj)Fn_Row))) {
	
	Fn_Row->Fn_Obj->Used++;
	
	Row->Code = Code;
	Row->Flags = Flags;
	Row->Gap = Row->LenGap = Row->Len = Row->MaxLen = 0;
	Row->Text = Row->ColText = NULL;
    }
    return Row;
}

static void InsertRow(row Row, window Parent, row Prev, row Next) {
    if (!Row->Window && Parent) {
	InsertGeneric((obj)Row, (obj_parent)&Parent->FirstRow, (obj)Prev, (obj)Next, &Parent->MaxNumRow);
	Row->Window = Parent;
	if (!(Row->Window->Flags & WINFL_USEANY))
	    Row->Window->NumRowOne = Row->Window->NumRowSplit = (ldat)0;
    }
}

static void RemoveRow(row Row) {
    if (Row->Window) {
	if (!(Row->Window->Flags & WINFL_USEANY))
	    Row->Window->NumRowOne = Row->Window->NumRowSplit = (ldat)0;
	RemoveGeneric((obj)Row, (obj_parent)&Row->Window->FirstRow, &Row->Window->MaxNumRow);
	Row->Window = (window)0;
    }
}

static void DeleteRow(row Row) {    
    if (Row) {
	fn_obj Fn_Obj = Row->Fn->Fn_Obj;
	
	Remove(Row);
	if (Row->Text)
	    FreeMem(Row->Text);
	if (Row->ColText)
	    FreeMem(Row->ColText);
	
	(Fn_Obj->Delete)((obj)Row);
	if (!--Fn_Obj->Used)
	    FreeMem(Fn_Obj);
    }
}

static row Create4MenuRow(fn_row Fn_Row, window Window, udat Code, byte Flags, ldat Len, CONST byte *Text) {
    row Row = (row)0;

    if (Window && (Row=(Fn_Row->Create)(Fn_Row, Code, Flags)) &&
	(!Text || (Row->Text = CloneMem(Text, Len)))) {

	Row->Len = Row->MaxLen = Len;
	InsertLast(Row, Row, Window);

	if ((ldat)Window->XWidth<(Len=Max2((ldat)10, Len+(ldat)2)))
	    Window->XWidth=(udat)Min2((ldat)MAXDAT, Len);
    
	if ((ldat)Window->YWidth<(Len=Max2((ldat)3, Window->MaxNumRow+(ldat)2)))
	    Window->YWidth=(udat)Min2((ldat)MAXDAT, Len);
	
	
	return Row;
    }
    if (Row)
	Delete(Row);
    return (row)0;
}

byte FindInfo(menu Menu, dat i) {
    row Info;
    
    if (Menu && (Info=Menu->Info) && Info->Len>(udat)i)
	return TRUE;
    return FALSE;
}

static struct s_fn_row _FnRow = {
    row_magic, sizeof(struct s_row), (uldat)1,
    CreateRow,
    InsertRow,
    RemoveRow,
    DeleteRow,
    /* row */
    &_FnObj,
    Create4MenuRow
};



/* FIXME: here. */

/* menuitem */

static void SyncScreenMenus(menu Menu) {
    screen Screen;
    for (Screen = All->FirstScreen; Screen; Screen = Screen->Next) {
	if (Act(FindMenu,Screen)(Screen) == Menu)
	    Act(DrawMenu,Screen)(Screen, 0, MAXDAT);
    }
}

static menuitem CreateMenuItem(fn_menuitem Fn_MenuItem, menu Menu, window Window, byte FlagActive,
				dat Left, dat Len, dat ShortCut, CONST byte *Name) {
    menuitem MenuItem = (menuitem)0;
    byte *_Name = NULL;
    
    if (Window && Menu && Name && (_Name=CloneStrL(Name,Len)) &&
	(MenuItem=(menuitem)Fn_MenuItem->Fn_Obj->Create((fn_obj)Fn_MenuItem))) {	  
	
	Fn_MenuItem->Fn_Obj->Used++;
	
	MenuItem->FlagActive=FlagActive;
	MenuItem->Left=Left;
	MenuItem->Len=Len;
	MenuItem->ShortCut=ShortCut;
	MenuItem->Name=_Name;
	MenuItem->Window=Window;
	InsertLast(MenuItem, MenuItem, Menu);
	
	SyncScreenMenus(Menu);
	
	return MenuItem;
    }
    if (_Name)
	FreeMem(_Name);
    return MenuItem;
}

static void InsertMenuItem(menuitem MenuItem, menu Parent, menuitem Prev, menuitem Next) {
    if (!MenuItem->Menu && Parent) {
	InsertGeneric((obj)MenuItem, (obj_parent)&Parent->FirstMenuItem, (obj)Prev, (obj)Next, (ldat *)0);
	MenuItem->Menu = Parent;
    }
}

static void RemoveMenuItem(menuitem MenuItem) {
    if (MenuItem->Menu) {
	RemoveGeneric((obj)MenuItem, (obj_parent)&MenuItem->Menu->FirstMenuItem, (ldat *)0);
	MenuItem->Menu = (menu)0;
    }
}

static void DeleteMenuItem(menuitem MenuItem) {
    if (MenuItem) {
	fn_obj Fn_Obj = MenuItem->Fn->Fn_Obj;
	menu Menu = MenuItem->Menu;
	
	Remove(MenuItem);
	SyncScreenMenus(Menu);
	
	if (MenuItem->Window)
	    Delete(MenuItem->Window);
	if (MenuItem->Name)
	    FreeMem(MenuItem->Name);
	
	(Fn_Obj->Delete)((obj)MenuItem);
	if (!--Fn_Obj->Used)
	    FreeMem(Fn_Obj);
    }
}

menuitem Create4MenuMenuItem(fn_menuitem Fn_MenuItem, menu Menu, window Window, byte FlagActive,
			      dat Len, CONST byte *Name) {
    dat Left, ShortCut;
    
    if (!Menu || !Window)
	return (menuitem)0;
    
    if (Menu->LastMenuItem)
	Left=Menu->LastMenuItem->Left+Menu->LastMenuItem->Len;
    else
	Left=(dat)1;

    ShortCut=(dat)0;
    while (ShortCut<Len && Name[ShortCut]==' ')
	ShortCut++;
    
    if (Window)
	Window->Left=Left;
    
    return (Fn_MenuItem->Create)(Fn_MenuItem, Menu, Window, FlagActive, Left, Len, ShortCut, Name);
}

/* this returns non-zero for compatibility */
static uldat Create4MenuCommonMenuItem(fn_menuitem Fn_MenuItem, menu Menu) {
    if (Menu) {
	Menu->CommonItems = TRUE;
	SyncScreenMenus(Menu);
	return (uldat)1;
    }
    return (uldat)0;
}

static struct s_fn_menuitem _FnMenuItem = {
    menuitem_magic, sizeof(struct s_menuitem), (uldat)1,
    CreateMenuItem,
    InsertMenuItem,
    RemoveMenuItem,
    DeleteMenuItem,
    &_FnObj,
    Create4MenuMenuItem,
    Create4MenuCommonMenuItem
};

/* menu */

static menu CreateMenu(fn_menu Fn_Menu, msgport MsgPort, hwcol ColItem, hwcol ColSelect,
			hwcol ColDisabled, hwcol ColSelectDisabled, hwcol ColShtCut, hwcol ColSelShtCut,
			byte FlagDefColInfo) {
    menu Menu = (menu)0;
    
    if (MsgPort && (Menu=(menu)Fn_Menu->Fn_Obj->Create((fn_obj)Fn_Menu))) {
	Fn_Menu->Fn_Obj->Used++;
	
	Menu->ColItem=ColItem;
	Menu->ColSelect=ColSelect;
	Menu->ColDisabled=ColDisabled;
	Menu->ColSelectDisabled=ColSelectDisabled;
	Menu->ColShtCut=ColShtCut;
	Menu->ColSelShtCut=ColSelShtCut;
	Menu->FirstMenuItem=Menu->LastMenuItem=Menu->MenuItemSelect=(menuitem)0;
	Menu->CommonItems = FALSE;
	Menu->FlagDefColInfo=FlagDefColInfo;
	Menu->Info=(row)0;
	InsertLast(Menu, Menu, MsgPort);
    }
    return Menu;
}

static void InsertMenu(menu Menu, msgport MsgPort, menu Prev, menu Next) {
    if (!Menu->MsgPort && MsgPort) {
	InsertGeneric((obj)Menu, (obj_parent)&MsgPort->FirstMenu, (obj)Prev, (obj)Next, (ldat *)0);
	Menu->MsgPort = MsgPort;
    }
}

static void RemoveMenu(menu Menu) {
   if (Menu->MsgPort) {
       RemoveGeneric((obj)Menu, (obj_parent)&Menu->MsgPort->FirstMenu, (ldat *)0);
       Menu->MsgPort = (msgport)0;
   }
}

static void DeleteMenu(menu Menu) {
    uldat count = 30;
    
    if (Menu) {
	fn_obj Fn_Obj = Menu->Fn->Fn_Obj;
	msgport MsgPort = Menu->MsgPort;
	widget W, WNext;
	
	/*
	 * warning:
	 * the only way to get the list of windows that
	 * are using this menu is to scan the whole MsgPort.
	 * We must UnMap() all those windows
	 * as a window without menu cannot exist.
	 *
	 * optimization: if we are going to UnMap() a lot of windows,
	 * we set QueuedDrawArea2FullScreen = TRUE, so that the UnMap()
	 * calls do not have to redraw every time.
	 */
	if (!QueuedDrawArea2FullScreen) {
	    for (W = MsgPort->FirstW; W && count; W = WNext) {
		WNext = W->ONext;
		if (IS_WINDOW(W) && ((window)W)->Menu == Menu) {
		    if (W->Parent && IS_SCREEN(W->Parent)) {
			count--;
		    }
		}
	    }
	    if (!count)
		QueuedDrawArea2FullScreen = TRUE;
	}
	for (W = MsgPort->FirstW; W; W = WNext) {
	    WNext = W->ONext;
	    if (IS_WINDOW(W) && ((window)W)->Menu == Menu) {
		if (W->Parent && IS_SCREEN(W->Parent)) {
		    Act(UnMap,W)(W);
		((window)W)->Menu = (menu)0;
		}
	    }
	}
	Remove(Menu);
	DeleteList(Menu->FirstMenuItem);
	if (Menu->Info)
	    Delete(Menu->Info);
	
	(Fn_Obj->Delete)((obj)Menu);
	if (!--Fn_Obj->Used)
	    FreeMem(Fn_Obj);
    }
}

static row SetInfoMenu(menu Menu, byte Flags, ldat Len, CONST byte *Text, CONST hwcol *ColText) {
    row Row;
    if ((Row = Do(Create,Row)(FnRow, 0, Flags))) {
	if ((!Text || (Row->Text=CloneStrL(Text,Len))) &&
	    (!ColText || (Row->ColText=CloneMem(ColText, Len*sizeof(hwcol))))) {
	    Row->Len = Row->MaxLen = Len;
	    if (Menu->Info)
		Delete(Menu->Info);
	    return Menu->Info = Row;
	}
	Delete(Row);
	Row = (row)0;
    }
    return Row;
}

static menuitem FindItem(menu Menu, dat i) {
    menuitem Item = (menuitem)0;
    
    if (Menu) {
	for (Item = Menu->FirstMenuItem; Item; Item = Item->Next) {
	    if (i >= Item->Left && i < Item->Left + Item->Len)
		break;
	}
	
	if (!Item && Menu->CommonItems && All->CommonMenu) {

	    Item = Menu->LastMenuItem;
	    
	    if (!Item || i >= Item->Left + Item->Len) {
		/* search in All->CommonMenu */
		if (Item)
		    i -= Item->Left + Item->Len;
		for (Item = All->CommonMenu->FirstMenuItem; Item; Item = Item->Next) {
		    if (i >= Item->Left && i < Item->Left + Item->Len)
			break;
		}
	    } else
		Item = (menuitem)0;
	}
    }
    return Item;
}

static menuitem GetSelectItem(menu Menu) {
    if (Menu) {
	if (Menu->MenuItemSelect)
	    return Menu->MenuItemSelect;
	if (Menu->CommonItems && All->CommonMenu)
	    return All->CommonMenu->MenuItemSelect;
    }
    return (menuitem)0;
}

static void SetSelectItem(menu Menu, menuitem Item) {
    if (Menu) {
	if (Item) {
	    if (Item->Menu == Menu) {
		Menu->MenuItemSelect = Item;
		if (Menu->CommonItems && All->CommonMenu)
		    All->CommonMenu->MenuItemSelect = (menuitem)0;
		
	    } else if (Menu->CommonItems && Item->Menu == All->CommonMenu) {
		Menu->MenuItemSelect = (menuitem)0;
		All->CommonMenu->MenuItemSelect = Item;
	    }
	    /* else Item is not a meaningful one! */
	} else {
	    Menu->MenuItemSelect = Item;
	    if (Menu->CommonItems && All->CommonMenu)
		All->CommonMenu->MenuItemSelect = Item;
	}
    }
}

static struct s_fn_menu _FnMenu = {
    menu_magic, sizeof(struct s_menu), (uldat)1,
    CreateMenu,
    InsertMenu,
    RemoveMenu,
    DeleteMenu,
    /* menu */
    &_FnObj,
    SetInfoMenu,
    FindItem,
    GetSelectItem,
    SetSelectItem
};


/* msg */

#define Delta ((size_t)&(((msg)0)->Event))

static msg CreateMsg(fn_msg Fn_Msg, udat Type, udat EventLen) {
    msg Msg;
    
    if ((Msg = (msg)AllocMem(EventLen + Delta))) {
	if (AssignId((fn_obj)Fn_Msg, (obj)Msg)) {
	    (Msg->Fn = Fn_Msg)->Used++;
	    Fn_Msg->Fn_Obj->Used++;
	    
	    Msg->Prev=Msg->Next=(msg)0;
	    Msg->MsgPort=(msgport)0;
	    
	    Msg->Type = Type;
	    Msg->Len = EventLen;
	    return Msg;
	}
	FreeMem(Msg);
	Msg = (msg)0;
    }
    return Msg;
    
}

#undef Delta

static void InsertMsg(msg Msg, msgport Parent, msg Prev, msg Next) {
    if (!Msg->MsgPort && Parent) {
	/* if adding the first msg, move the msgport to the head
	 * of msgport list, so that the scheduler will run it */
	if (!Parent->FirstMsg && Parent->All)
	    MoveFirst(MsgPort, All, Parent);
	
	InsertGeneric((obj)Msg, (obj_parent)&Parent->FirstMsg, (obj)Prev, (obj)Next, (ldat *)0);
	Msg->MsgPort = Parent;
    }
}

static void RemoveMsg(msg Msg) {
    if (Msg->MsgPort) {
	RemoveGeneric((obj)Msg, (obj_parent)&Msg->MsgPort->FirstMsg, (ldat *)0);
	Msg->MsgPort = (msgport)0;
    }
}

static void DeleteMsg(msg Msg) {
    if (Msg) {
	fn_obj Fn_Obj = Msg->Fn->Fn_Obj;
	Remove(Msg);
	
	(Fn_Obj->Delete)((obj)Msg);
	if (!--Fn_Obj->Used)
	    FreeMem(Fn_Obj);
    }
}

static struct s_fn_msg _FnMsg = {
    msg_magic, sizeof(struct s_msg), (uldat)1,
    CreateMsg,
    InsertMsg,
    RemoveMsg,
    DeleteMsg,
    /* msg */
    &_FnObj
};

/* msgport */

static msgport CreateMsgPort(fn_msgport Fn_MsgPort, byte NameLen, CONST byte *ProgramName,
			      time_t PauseSec, frac_t PauseFraction,
			      byte WakeUp, void (*Handler)(msgport)) {
    msgport MsgPort = (msgport)0;
    byte *Name;
    
    if (Handler && (!ProgramName || (Name = CloneStrL(ProgramName, NameLen))) &&
	(MsgPort=(msgport)Fn_MsgPort->Fn_Obj->Create((fn_obj)Fn_MsgPort))) {
	
	Fn_MsgPort->Fn_Obj->Used++;
	
	MsgPort->WakeUp=WakeUp;
	MsgPort->NameLen=NameLen;
	MsgPort->ProgramName=Name;
	MsgPort->Handler=Handler;
	MsgPort->ShutDownHook=(void (*)(msgport))0;
	MsgPort->PauseDuration.Seconds=PauseSec;
	MsgPort->PauseDuration.Fraction=PauseFraction;
	(void)SumTime(&MsgPort->CallTime, &All->Now, &MsgPort->PauseDuration);
	MsgPort->RemoteData.Fd=NOFD;
	MsgPort->RemoteData.ChildPid=NOPID;
	MsgPort->RemoteData.FdSlot=NOSLOT;
	MsgPort->FirstMsg=MsgPort->LastMsg=(msg)0;
	MsgPort->FirstMenu=MsgPort->LastMenu=(menu)0;
	MsgPort->FirstW=MsgPort->LastW=(widget)0;
	MsgPort->FirstGroup=MsgPort->LastGroup=(group)0;
	MsgPort->FirstMutex=MsgPort->LastMutex=(mutex)0;
	MsgPort->AttachHW=(display_hw)0;
	InsertMiddle(MsgPort, MsgPort, All,
		     WakeUp ? (msgport)0 : All->LastMsgPort,
		     WakeUp ? All->FirstMsgPort : (msgport)0);
	SortMsgPortByCallTime(MsgPort);
    } else if (NameLen && Name)
	FreeMem(Name);
    return MsgPort;
}

static void InsertMsgPort(msgport MsgPort, all Parent, msgport Prev, msgport Next) {
    if (!MsgPort->All && Parent) {
	InsertGeneric((obj)MsgPort, (obj_parent)&Parent->FirstMsgPort, (obj)Prev, (obj)Next, (ldat *)0);
	MsgPort->All = Parent;
    }
}

static void RemoveMsgPort(msgport MsgPort) {
    if (MsgPort->All) {
	if (All->RunMsgPort == MsgPort)
	    All->RunMsgPort = MsgPort->Next;
	RemoveGeneric((obj)MsgPort, (obj_parent)&MsgPort->All->FirstMsgPort, (ldat *)0);
	MsgPort->All = (all)0;
    }
}

static void DeleteMsgPort(msgport MsgPort) {
    uldat count = 30;
    widget W;
    
    if (MsgPort) {
	fn_obj Fn_Obj = MsgPort->Fn->Fn_Obj;
	
	/*
	 * optimization: if we are going to UnMap() a lot of windows,
	 * we set QueuedDrawArea2FullScreen = TRUE, so that the UnMap()
	 * calls do not have to redraw every time.
	 */
	for (W = MsgPort->FirstW; W && count; W = W->ONext) {
	    if (IS_WINDOW(W) && W->Parent && IS_SCREEN(W->Parent)) {
		count--;
	    }
	}
	if (!count)
	    QueuedDrawArea2FullScreen = TRUE;
	
	if (MsgPort->ShutDownHook)
	    MsgPort->ShutDownHook(MsgPort);

	/*
	 * must delete the Menus first, as among widgets there are also
	 * menuitem windows, which cannot be deleted before deleting
	 * the corresponding menuitem.
	 */
	DeleteList(MsgPort->FirstMsg);
	DeleteList(MsgPort->FirstMenu);
	DeleteList(MsgPort->FirstW);
	DeleteList(MsgPort->FirstGroup);
	DeleteList(MsgPort->FirstMutex);
	
	Remove(MsgPort);
	if (MsgPort->ProgramName)
	    FreeMem(MsgPort->ProgramName);
	
	(Fn_Obj->Delete)((obj)MsgPort);
	if (!--Fn_Obj->Used)
	    FreeMem(Fn_Obj);
    }
}

static struct s_fn_msgport _FnMsgPort = {
    msgport_magic, sizeof(struct s_msgport), (uldat)1,
    CreateMsgPort,
    InsertMsgPort,
    RemoveMsgPort,
    DeleteMsgPort,
    /* msgport */
    &_FnObj
};
    
/* mutex */

mutex CreateMutex(fn_mutex Fn_Mutex, msgport Owner,
			   byte NameLen, CONST byte *Name, byte Perm) {
    byte Possible = PERM_WRITE;
    mutex CurrMutex, NewMutex = (mutex)0, AlreadyMutex = (mutex)0;
    
    if (!Perm || !Owner)
	return NewMutex;
    
    CurrMutex = All->FirstMutex;
    while (CurrMutex && Possible) {
	if (NameLen == CurrMutex->NameLen && !CmpMem(Name, CurrMutex->Name, NameLen)) {
	    if (CurrMutex->Owner==Owner) {
		AlreadyMutex = CurrMutex;
		continue;
	    }
	    Possible &= CurrMutex->Perm & PERM_WRITE ? PERM_NONE : CurrMutex->Perm & PERM_READ ? PERM_READ : (byte)~0;
	}
	CurrMutex = CurrMutex->Next;
    }
    
    if (Possible>=Perm) {
	if (AlreadyMutex) {
	    AlreadyMutex->Perm = Perm;
	    NewMutex=AlreadyMutex;
	}
	else if ((NewMutex = (mutex)Fn_Mutex->Fn_Obj->Create((fn_obj)Fn_Mutex))) {
	    Fn_Mutex->Fn_Obj->Used++;
	    
	    NewMutex->Perm = Perm;
	    InsertLast(Mutex, NewMutex, All);
	    NewMutex->Owner = (msgport)0;
	    Act(Own,NewMutex)(NewMutex, Owner);
	}
    }
    return NewMutex;
}

static void InsertMutex(mutex Mutex, all Parent, mutex Prev, mutex Next) {
    if (!Mutex->All && Parent) {
	InsertGeneric((obj)Mutex, (obj_parent)&Mutex->All->FirstMutex, (obj)Prev, (obj)Next, (ldat *)0);
	Mutex->All = Parent;
    }
}

static void RemoveMutex(mutex Mutex) {
    if (Mutex->All) {
	RemoveGeneric((obj)Mutex, (obj_parent)&Mutex->All->FirstMutex, (ldat *)0);
	Mutex->All = (all)0;
    }
}

static void DeleteMutex(mutex Mutex) {
    fn_obj Fn_Obj = Mutex->Fn->Fn_Obj;
	
    Act(DisOwn,Mutex)(Mutex);
    Remove(Mutex);
    (Fn_Obj->Delete)((obj)Mutex);
    if (!--Fn_Obj->Used)
	FreeMem(Fn_Obj);
}

static void OwnMutex(mutex Mutex, msgport Parent) {
    if (!Mutex->Owner && Parent) {

	if ((Mutex->OPrev = Parent->LastMutex))
	    Parent->LastMutex->ONext = Mutex;
	else
	    Parent->FirstMutex = Mutex;	
    
	Mutex->ONext = (mutex)0;
	Parent->LastMutex = Mutex;
	
	Mutex->Owner = Parent;
    }
}

static void DisOwnMutex(mutex Mutex) {
    msgport Parent;
    if ((Parent = Mutex->Owner)) {
	if (Mutex->OPrev)
	    Mutex->OPrev->ONext = Mutex->ONext;
	else if (Parent->FirstMutex == Mutex)
	    Parent->FirstMutex = Mutex->ONext;
    
	if (Mutex->ONext)
	    Mutex->ONext->OPrev = Mutex->OPrev;
	else if (Parent->LastMutex == Mutex)
	    Parent->LastMutex = Mutex->OPrev;
    
	Mutex->OPrev = Mutex->ONext = (mutex)0;
	
	Mutex->Owner = (msgport)0;
    }
}

static struct s_fn_mutex _FnMutex = {
    mutex_magic, sizeof(struct s_mutex), (uldat)1,
    CreateMutex,
    InsertMutex,
    RemoveMutex,
    DeleteMutex,
    /* mutex */
    &_FnObj,
    OwnMutex,
    DisOwnMutex
};


/* module */

static module CreateModule(fn_module Fn_Module, uldat NameLen, CONST byte *Name) {
    module Module = (module)0;
    byte *newName = NULL;
    
    if (Name && (newName = CloneStrL(Name, NameLen))) {
	if ((Module=(module)Fn_Module->Fn_Obj->Create((fn_obj)Fn_Module))) {
	
	    Fn_Module->Fn_Obj->Used++;
	
	    Module->NameLen = NameLen;
	    Module->Name = newName;
	    Module->Used = 0;
	    Module->Handle = Module->Private = NULL;
	
	    InsertLast(Module, Module, All);
	    return Module;
	}
	FreeMem(newName);
    }
    return Module;
}

static void InsertModule(module Module, all Parent, module Prev, module Next) {
    if (!Module->All && Parent) {
	InsertGeneric((obj)Module, (obj_parent)&Parent->FirstModule, (obj)Prev, (obj)Next, (ldat *)0);
	Module->All = Parent;
    }
}

static void RemoveModule(module Module) {
    if (Module->All) {
	RemoveGeneric((obj)Module, (obj_parent)&Module->All->FirstModule, (ldat *)0);
	Module->All = (all)0;
    }
}

static void DeleteModule(module Module) {
    if (!Module->Used) {
	fn_obj Fn_Obj = Module->Fn->Fn_Obj;

	Act(DlClose,Module)(Module);
	Remove(Module);
	if (Module->Name)
	    FreeMem(Module->Name);
	
	(Fn_Obj->Delete)((obj)Module);
	if (!--Fn_Obj->Used)
	    FreeMem(Fn_Obj);
    }
}


static struct s_fn_module _FnModule = {
    module_magic, sizeof(struct s_module), (uldat)1,
    CreateModule,
    InsertModule,
    RemoveModule,
    DeleteModule,
    /* module */
    &_FnObj,
#ifdef CONF__MODULES
    DlOpen,
    DlClose
#else
    (byte (*)(module))AlwaysFalse,
    (void (*)(module))NoOp
#endif
};



/* display_hw */

static display_hw CreateDisplayHW(fn_display_hw Fn_DisplayHW, uldat NameLen, CONST byte *Name) {
    display_hw DisplayHW = (display_hw)0;
    byte *newName = NULL;
    
    if (Name && (newName = CloneStrL(Name, NameLen))) {
	if ((DisplayHW=(display_hw)Fn_DisplayHW->Fn_Obj->Create((fn_obj)Fn_DisplayHW))) {
	
	    Fn_DisplayHW->Fn_Obj->Used++;
	
	    DisplayHW->NameLen = NameLen;
	    DisplayHW->Name = newName;
	    DisplayHW->Module = NULL;
	    DisplayHW->Quitted = TRUE;
	    DisplayHW->AttachSlot = NOSLOT;
	    /*
	     * ->Quitted will be set to FALSE only
	     * after DisplayHW->InitHW() has succeeded
	     */
	    InsertLast(DisplayHW, DisplayHW, All);
	    return DisplayHW;
	}
	FreeMem(newName);
    }
    return DisplayHW;
}

static void InsertDisplayHW(display_hw DisplayHW, all Parent, display_hw Prev, display_hw Next) {
    if (!DisplayHW->All && Parent) {
	InsertGeneric((obj)DisplayHW, (obj_parent)&Parent->FirstDisplayHW, (obj)Prev, (obj)Next, (ldat *)0);
	DisplayHW->All = Parent;
#if 0
	/*
	 * here we would call uninitialized DisplayHW routines like MoveToXY,
	 * put this after DisplayHW->InitHW()
	 */
	if (All->FnHookDisplayHW)
	    All->FnHookDisplayHW(All->HookDisplayHW);
#endif
    }
}

static void RemoveDisplayHW(display_hw DisplayHW) {
    if (DisplayHW->All) {
	RemoveGeneric((obj)DisplayHW, (obj_parent)&DisplayHW->All->FirstDisplayHW, (ldat *)0);
	DisplayHW->All = (all)0;

    	if (All->FnHookDisplayHW)
	    All->FnHookDisplayHW(All->HookDisplayHW);
    }
}

static void DeleteDisplayHW(display_hw DisplayHW) {
    fn_obj Fn_Obj = DisplayHW->Fn->Fn_Obj;
    byte isCTTY = DisplayHW->DisplayIsCTTY && DisplayHW == DisplayHWCTTY;
    byte Quitted = DisplayHW->Quitted;
	
    if (!Quitted)
	Act(Quit,DisplayHW)(DisplayHW);
	
    /* avoid getting stale pointers */
    if (All->MouseHW == DisplayHW)
	All->MouseHW = NULL;
    if (All->ExclusiveHW == DisplayHW)
	All->ExclusiveHW = NULL;
	
    Remove(DisplayHW);
    if (DisplayHW->NameLen && DisplayHW->Name)
	FreeMem(DisplayHW->Name);
    
    (Fn_Obj->Delete)((obj)DisplayHW);
    if (!--Fn_Obj->Used)
	FreeMem(Fn_Obj);
    
    if (!Quitted) {
	if (!All->FirstDisplayHW || isCTTY)
	    RunNoHW();
	else if (All->FirstDisplayHW && ResizeDisplay()) {
	    QueuedDrawArea2FullScreen = TRUE;
	}
    }
}


static struct s_fn_display_hw _FnDisplayHW = {
    display_hw_magic, sizeof(struct s_display_hw), (uldat)1,
    CreateDisplayHW,
    InsertDisplayHW,
    RemoveDisplayHW,
    DeleteDisplayHW,
    /* display_hw */
    &_FnObj,
    InitDisplayHW,
    QuitDisplayHW
};


fn Fn = {
    &_FnObj,
    &_FnWidget,
    &_FnGadget,
    &_FnWindow,
    &_FnScreen,
    &_FnGroup,
    &_FnRow,
    &_FnMenuItem,
    &_FnMenu,
    &_FnMsgPort,
    &_FnMutex,
    &_FnMsg,
    &_FnModule,
    &_FnDisplayHW
};

