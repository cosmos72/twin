/*
 *  methods.c  --  functions for OO-style calls on twin data structures
 *                 (windows, menus, ...)
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
#include "methods.h"
#include "data.h"

#include "draw.h"
#include "resize.h"
#include "util.h"
#include "remote.h"
#include "wm.h"

#ifdef CONF_TERM
# include "tty.h"
#endif
#ifdef CONF_MODULES
# include "dl.h"
#endif

/* some object-oriented ones not included in fn_obj */

INLINE void *CloneMem(void *From, uldat Size) {
    void *temp;
    if (From && Size && (temp = AllocMem(Size)))
	return CopyMem(From, temp, Size);
    return NULL;
}

obj *Clone(obj *Obj) {
    obj *t1, *t2;
    
    if ((t1 = Do(Create,Obj)(Obj->Fn))) {
	/* fill the rest with zeros to be in a known state */
	WriteMem((byte *)t1 + FnObj->Size, '\0', Obj->Fn->Size - FnObj->Size);
	if ((t2 = Act(Copy,Obj)(Obj, t1)))
	    return t2;
	Act(Delete,t1)(t1);
	t1 = t2; /* i.e. NULL */
    }
    return t1;
}

INLINE void DeletePartialList(obj *Obj) {
    obj *Next;
    while (Obj) {
	Next = Obj->Next;
	Delete(Obj);
	Obj = Next;
    }
}

void CloneList(obj *From, obj **FirstTo, obj **LastTo) {
    obj *TmpObj, *Obj;
    
    
    if (!(*FirstTo=Obj=Clone(From))) {
	*LastTo = (obj *)0;
	return;
    }
    Obj->Prev=(obj *)0;
    From=From->Next;
    while (From) {
	Obj->Next=TmpObj=Clone(From);
	if (!TmpObj) {
	    DeletePartialList(*FirstTo);
	    return;
	}
	TmpObj->Prev=Obj;
	Obj=TmpObj;
	From=From->Next;
    }
    Obj->Next=(obj *)0;
    *LastTo=Obj;
}

INLINE void InsertGeneric(obj *Obj, obj_parent *Parent, obj *Prev, obj *Next, uldat *ObjCount) {
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

INLINE void RemoveGeneric(obj *Obj, obj_parent *Parent, uldat *ObjCount) {
    if (Obj->Prev)
	Obj->Prev->Next=Obj->Next;
    else if (Parent->First == Obj)
	Parent->First=Obj->Next;
    
    if (Obj->Next)
	Obj->Next->Prev=Obj->Prev;
    else if (Parent->Last == Obj)
	Parent->Last=Obj->Prev;
    
    Obj->Prev=Obj->Next=(obj *)0;
    if (ObjCount)
	(*ObjCount)--;
}

INLINE void OwnGeneric(obj *Obj, obj_parent *Parent) {
    if ((Obj->OPrev=Parent->Last))
	Parent->Last->ONext=Obj;
    else
	Parent->First=Obj;	
    
    Obj->ONext=(obj *)0;
    Parent->Last=Obj;
}

INLINE void DisOwnGeneric(obj *Obj, obj_parent *Parent) {
    if (Obj->OPrev)
	Obj->OPrev->ONext=Obj->ONext;
    else if (Parent->First == Obj)
	Parent->First=Obj->ONext;
    
    if (Obj->ONext)
	Obj->ONext->OPrev=Obj->OPrev;
    else if (Parent->Last == Obj)
	Parent->Last=Obj->OPrev;
    
    Obj->OPrev=Obj->ONext=(obj *)0;
}

/* fn_obj and others fn_XXX functions */

/* obj */

static obj *CreateObj(fn_obj *Fn_Obj) {
    obj *Obj;
    
    if ((Obj=(obj *)AllocMem(Fn_Obj->Size))) {
	if (AssignId(Fn_Obj, Obj)) {
	    (Obj->Fn=Fn_Obj)->Used++;
	    Obj->Prev=Obj->Next=(obj *)0;
	    Obj->Parent=(obj_parent *)0;
	}
	else
	    FreeMem(Obj), Obj = (obj *)0;
    }
    return Obj;
}

static obj *CopyObj(obj *From, obj *To) {
    obj *prev = To->Prev, *next = To->Next;
    uldat Id = To->Id;
    /* we allow From->Fn and To->Fn to be different,
     * as long as they share the same magic */
    if (From->Fn->Magic == To->Fn->Magic) {
	From->Fn->Used++;
	if (!--To->Fn->Used)
	    FreeMem(To->Fn);
	CopyMem(From, To, From->Fn->Size);
	To->Id = Id;
	To->Prev = prev;
	To->Next = next;
	return To;
    }
    return (obj *)0;
}

static void InsertObj(obj *Obj, obj_parent *Parent, obj *Prev, obj *Next) {
    if (!Obj->Parent && Parent) {
	InsertGeneric(Obj, Parent, Prev, Next, (uldat *)0);
	Obj->Parent = Parent;
    }
}

static void RemoveObj(obj *Obj) {
    if (Obj->Parent) {
	RemoveGeneric(Obj, Obj->Parent, (uldat *)0);
	Obj->Parent = (obj_parent *)0;
    }
}

static void DeleteObj(obj *Obj) {
    if (Obj) {
	/* not a good idea to RemoveObj(Obj) here */
	DropId(Obj);
	if (!--Obj->Fn->Used)
	    FreeMem(Obj->Fn);
	FreeMem(Obj);
    }
}

static fn_obj _FnObj = {
    obj_magic, (uldat)sizeof(obj), (uldat)1,
	CreateObj,
	CopyObj,
	InsertObj,
	RemoveObj,
	DeleteObj
};

/* area */

static area *CreateArea(fn_area *Fn_Area, area_parent *Parent, screen *FirstScreen, window *FirstWindow, window *OnlyThisWindow,
		 gadget *FirstGadget, gadget *OnlyThisGadget, dat Xstart, dat Ystart, dat Xend, dat Yend, byte Shaded) {
    area *Area;
    if ((Area=(area *)CreateObj((fn_obj *)Fn_Area))) {	
	Area->FirstScreen=FirstScreen;
	Area->FirstWindow=FirstWindow;
	Area->OnlyThisWindow=OnlyThisWindow;
	Area->FirstGadget=FirstGadget;
	Area->OnlyThisGadget=OnlyThisGadget;
	Area->Xstart=Xstart;
	Area->Ystart=Ystart;
	Area->Xend=Xend;
	Area->Yend=Yend;
	Area->Shaded=Shaded;
	if (Parent)
	    InsertLast(Area, Area, Parent);
    }
    return Area;
}

static void DeleteArea(area *Area) {
    if (Area) {
	Remove(Area);
	DeleteObj((obj *)Area);
    }
}

static fn_area _FnArea = {
    area_magic, (uldat)sizeof(area), (uldat)1,
	CreateArea,
	(area *(*)(area *, area *))CopyObj,
	(void (*)(area *, area_parent *, area *, area *))InsertObj,
        (void (*)(area *))RemoveObj,
	DeleteArea
};

/* area_win */

static area_win *CreateAreaWin(fn_area_win *Fn_AreaWin, area_win_parent *Parent, window *Window, gadget *FirstGadget, gadget *OnlyThisGadget,
			byte NoGadgets, uldat XLogic, uldat YLogic, dat Xstart, dat Ystart, dat Xend, dat Yend) {
    area_win *Area;
    if ((Area=(area_win *)CreateObj((fn_obj *)Fn_AreaWin))) {
	Area->Window=Window;
	Area->FirstGadget=FirstGadget;
	Area->OnlyThisGadget=OnlyThisGadget;
	Area->NoGadgets=NoGadgets;
	Area->XLogic=XLogic;
	Area->YLogic=YLogic;
	Area->Xstart=Xstart;
	Area->Ystart=Ystart;
	Area->Xend=Xend;
	Area->Yend=Yend;
	if (Parent)
	    InsertLast(AreaWin, Area, Parent);
    }
    return Area;
}

static void DeleteAreaWin(area_win *Area) {
    if (Area) {
	Remove(Area);
	DeleteObj((obj *)Area);
    }
}

static fn_area_win _FnAreaWin = {
    area_win_magic, (uldat)sizeof(area_win), (uldat)1,
	CreateAreaWin,
	(area_win *(*)(area_win *From, area_win *To))CopyObj,
	(void (*)(area_win *, area_win_parent *, area_win *, area_win *))InsertObj,
	(void (*)(area_win *))RemoveObj,
	DeleteAreaWin
};

/* row */

static row *CreateRow(fn_row *Fn_Row, udat Code, byte Flags) {
    row *Row;
    
    if ((Row=(row *)CreateObj((fn_obj *)Fn_Row))) {
	Row->Code = Code;
	Row->Flags = Flags;
	Row->Gap = Row->LenGap =
	Row->Len = Row->MaxLen = 0;
	Row->Text = Row->ColText = NULL;
    }
    return Row;
}

static row *CopyRow(row *From, row *To) {
    window *Window;
    
    if (!From || !To)
	return (row *)0;
    
    Window = To->Window;
    
    if (To->Text)
	FreeMem(To->Text), To->Text = NULL, To->Len = To->MaxLen = 0;
    if (To->ColText)
	FreeMem(To->ColText), To->ColText = NULL;
    
    if ((To = (row *)CopyObj((obj *)From, (obj *)To))) {
	To->Window = Window;
	if ((!From->Text || (To->Text = CloneMem(From->Text, From->MaxLen))) &&
	    (!From->ColText || (To->ColText = CloneMem(From->ColText, From->MaxLen))))
	    return To;
	    
	if (To->Text)
	    FreeMem(To->Text), To->Text = NULL;
	if (To->ColText)
	    FreeMem(To->ColText), To->ColText = NULL;
	To = (row *)0;
    }
    return To;
}

static void InsertRow(row *Row, window *Parent, row *Prev, row *Next) {
    if (!Row->Window && Parent) {
	InsertGeneric((obj *)Row, (obj_parent *)&Parent->FirstRow, (obj *)Prev, (obj *)Next, &Parent->MaxNumRow);
	Row->Window = Parent;
    }
}

static void RemoveRow(row *Row) {
    if (Row->Window) {
	RemoveGeneric((obj *)Row, (obj_parent *)&Row->Window->FirstRow, &Row->Window->MaxNumRow);
	Row->Window = (window *)0;
    }
}

static void DeleteRow(row *Row) {    
    if (Row) {
	Remove(Row);
	if (Row->Text)
	    FreeMem(Row->Text);
	if (Row->ColText)
	    FreeMem(Row->ColText);
	DeleteObj((obj *)Row);
    }
}

static row *SearchRow(window *Window, uldat Row) {
    row *CurrRow, *ElPossib[4];
    byte Index;
    uldat k, ElNumRows[4], ElDist[4];
    
    ElPossib[0] = Window->RowOne;
    ElPossib[1] = Window->RowSplit;
    ElPossib[2] = Window->FirstRow;
    ElPossib[3] = Window->LastRow;
    ElNumRows[0] = Window->NumRowOne;
    ElNumRows[1] = Window->NumRowSplit;
    ElNumRows[2] = (uldat)0;
    ElNumRows[3] = Window->MaxNumRow-(uldat)1;
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

static row *SearchRowCode(window *Window, udat Code, uldat *NumRow) {
    row *Row;
    uldat Num=(uldat)0;
    
    if ((Row=Window->FirstRow))
	while (Row && Row->Code!=Code) {
	    Row=Row->Next;
	    Num++;
	}
    if (Row && NumRow)
	*NumRow=Num;

    return Row;
}

static row *Create4MenuRow(fn_row *Fn_Row, window *Window, udat Code, byte Flags, uldat Len, byte *Text) {
    row *Row = (row *)0;

    if (Window && (Row=(Fn_Row->Create)(Fn_Row, Code, Flags)) &&
	(!Text || (Row->Text = CloneMem(Text, Len)))) {

	Row->Len = Row->MaxLen = Len;
	InsertLast(Row, Row, Window);

	if ((uldat)Window->XWidth<(Len=Max2((uldat)10, Len+(uldat)2)))
	    Window->XWidth=(udat)Min2((uldat)MAXUDAT, Len);
    
	if ((uldat)Window->YWidth<(Len=Max2((uldat)3, Window->MaxNumRow+(uldat)2)))
	    Window->YWidth=(udat)Min2((uldat)MAXUDAT, Len);
	
	
	return Row;
    }
    if (Row)
	Delete(Row);
    return (row *)0;
}

byte SearchInfo(menu *Menu, dat i) {
    row *Info;
    
    if (Menu && (Info=Menu->Info) && Info->Len>(udat)i)
	return TRUE;
    return FALSE;
}

static fn_row _FnRow = {
    row_magic, (uldat)sizeof(row), (uldat)1,
	CreateRow,
	CopyRow,
	InsertRow,
	RemoveRow,
	DeleteRow,
	Create4MenuRow
};

/* gadget */

static gadget *CreateGadget(fn_gadget *Fn_Gadget, window *Window,
			    hwcol ColText, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled,
			    udat Code, udat Flags, udat Left, udat Up, udat XWidth, udat YWidth, byte Bitmap,
			    byte *TextNormal, byte *TextSelect, byte *TextDisabled, byte *TextSelectDisabled,
			    hwcol *ColorNormal, hwcol *ColorSelect, hwcol *ColorDisabled, hwcol *ColorSelectDisabled) {
    gadget *Gadget = (gadget *)0;
    uldat Size = (uldat)XWidth*YWidth;

    if (Window && (Gadget=(gadget *)CreateObj((fn_obj *)Fn_Gadget))) {	
	Gadget->ColText=ColText;
	Gadget->ColSelect=ColSelect;
	Gadget->ColDisabled=ColDisabled;
	Gadget->ColSelectDisabled=ColSelectDisabled;
	Gadget->Code=Code;
	Gadget->Flags=Flags;
	Gadget->Left=Left;
	Gadget->Up=Up;
	Gadget->XWidth=XWidth;
	Gadget->YWidth=YWidth;

	Gadget->Contents[0] = CloneMem(TextNormal, Size);
		
	if (Bitmap & 0x2)
	    Gadget->Contents[1] = CloneMem(TextSelect, Size);
	else
	    Gadget->Contents[1] = NULL;
	
	if (Bitmap & 0x2)
	    Gadget->Contents[2] = CloneMem(TextDisabled, Size);
	else
	    Gadget->Contents[2] = NULL;
	
	if (Bitmap & 0x4)
	    Gadget->Contents[3] = CloneMem(TextSelectDisabled, Size);
	else
	    Gadget->Contents[3] = NULL;
	 
	if (Bitmap & 0x8)
	    Gadget->Contents[4] = CloneMem(ColorNormal, Size*sizeof(hwcol));
	else
	    Gadget->Contents[4] = NULL, Gadget->Flags |= GADGET_USE_DEFCOL;
	
	if (Bitmap & 0x10)
	    Gadget->Contents[5] = CloneMem(ColorSelect, Size*sizeof(hwcol));
	else
	    Gadget->Contents[5] = NULL;
	    
	if (Bitmap & 0x20)
	    Gadget->Contents[6] = CloneMem(ColorDisabled, Size*sizeof(hwcol));
	else
	    Gadget->Contents[6] = NULL;
	    
	if (Bitmap & 0x40)
	    Gadget->Contents[7] = CloneMem(ColorSelectDisabled, Size*sizeof(hwcol));
	else
	    Gadget->Contents[7] = NULL;
	
	InsertLast(Gadget, Gadget, Window);
    }
    return Gadget;
}

static gadget *CopyGadget(gadget *From, gadget *To) {
    window *Window;
    uldat Size;
    byte i;
    
    if (!From || !To)
	return (gadget *)0;
    
    Window = To->Window;
    
    for (i=0; i<8; i++)
	if (To->Contents[i])
	    FreeMem(To->Contents[i]), To->Contents[i] = NULL;
    
    if ((To = (gadget *)CopyObj((obj *)From, (obj *)To))) {
	To->Window = Window;
	
	Size=(uldat)From->XWidth*(uldat)From->YWidth;
    
	for (i=0; i<8; i++)
	    if (From->Contents[i] && !(To->Contents[i]=CloneMem(From->Contents[i], Size))) {
		for (i=0; From->Contents[i] && !To->Contents[i] && i<8; i++)
		    FreeMem(To->Contents[i]), To->Contents[i] = NULL;
		To = (gadget *)0;
		break;
	    }
    }
    return To;
}

static gadget *SearchGadgetCode(window *Parent, udat Code) {
    gadget *Gadget;
    
    if ((Gadget=Parent->FirstGadget))
	while (Gadget && Gadget->Code!=Code) {
	    Gadget=Gadget->Next;
	}
    return Gadget;
}

    
static void InsertGadget(gadget *Gadget, window *Parent, gadget *Prev, gadget *Next) {
    if (!Gadget->Window && Parent) {
	InsertGeneric((obj *)Gadget, (obj_parent *)&Parent->FirstGadget, (obj *)Prev, (obj *)Next, (uldat *)0);
	Gadget->Window = Parent;
    }
}

static void RemoveGadget(gadget *Gadget) {
    if (Gadget->Window) {
	RemoveGeneric((obj *)Gadget, (obj_parent *)&Gadget->Window->FirstGadget, (uldat *)0);
	Gadget->Window = (window *)0;
    }
}

static void DeleteGadget(gadget *Gadget) {
    byte i;
    if (Gadget) {
	Remove(Gadget);
	for (i=0; i<8; i++)
	    if (Gadget->Contents[i])
		FreeMem(Gadget->Contents[i]);
	DeleteObj((obj *)Gadget);
    }
}

static gadget *SearchGadget(window *Window, dat i, dat j) {
    ldat shUp, shLeft;
    udat Up, Left;
    screen *Screen;
    gadget *FirstGadget;
    
    if (!Window || !(Screen=Window->Screen))
	return (gadget *)0;
    
    shUp = (ldat)Window->Up - (ldat)Screen->Up*!(Window->Attrib & WINDOW_MENU)+(ldat)Screen->YLimit+(ldat)1;
    shLeft=(ldat)Window->Left-(ldat)Screen->Left*!(Window->Attrib & WINDOW_MENU)+(ldat)1;
    FirstGadget=Window->FirstGadget;
    
    if (!FirstGadget || (ldat)i<shLeft || (ldat)j<shUp ||
	(ldat)i>shLeft+(ldat)Window->XWidth-(ldat)3 ||
	(ldat)j>shUp+(ldat)Window->YWidth-(ldat)3)
	return (gadget *)0;
    
    if (Window->XLogic>(ldat)MAXUDAT ||
	Window->YLogic>(ldat)MAXUDAT)
	return (gadget *)0;
    
    shLeft=(ldat)i+Window->XLogic-shLeft;
    shUp=(ldat)j+Window->YLogic-shUp;
    
    while (FirstGadget) {
	Left=FirstGadget->Left;
	Up=FirstGadget->Up;
	
	if (Left>(udat)shLeft || Up>(udat)shUp ||
	    Left+FirstGadget->XWidth<=(udat)shLeft ||
	    Up+FirstGadget->YWidth<=(udat)shUp)
	    FirstGadget=FirstGadget->Next;
	else
	    break;
    }
    
    return FirstGadget;
}

static gadget *CreateButton(fn_gadget *Fn_Gadget, window *Window, udat XWidth, udat YWidth, hwcol BgCol) {
    gadget *Gadget;
    uldat Size;
    byte i;
    udat j, k;
    
    if ((Gadget=(gadget *)CreateObj((fn_obj *)Fn_Gadget))) {
	Gadget->XWidth=XWidth+(udat)1;
	Gadget->YWidth=YWidth+(udat)1;
	Size=(uldat)(XWidth+(udat)1)*(uldat)(YWidth+(udat)1);

	for (i=(byte)0; i<(byte)8; i++)
	    Gadget->Contents[i]=NULL;
	for (i=(byte)0; i<(byte)8; i++)
	    if (!(Gadget->Contents[i]=(byte *)AllocMem(Size))) {
		Act(Delete,Gadget)(Gadget);
		return (gadget *)0;
	    }
	
	Size=(uldat)XWidth*(uldat)YWidth;
	BgCol &= COL(0,MAXCOL);
	for (i=(byte)0; i<(byte)4; i++) {
	    for (j=k=(udat)0; j<YWidth; j++, k+=XWidth+(udat)1) {
		Gadget->Contents[i  ][k+(i & 1 ? 0 : XWidth)] = i & 1 ? ' ' : k ? 'Û' : 'Ü';
		Gadget->Contents[i+4][k+(i & 1 ? 0 : XWidth)] = BgCol;
	    }
	    Gadget->Contents[i][k]=(byte)' ';
	    WriteMem((void *)(Gadget->Contents[i]+k+1), i & 1 ? ' ' : 'ß', (uldat)XWidth);
	    WriteMem((void *)(Gadget->Contents[i+4]+k), BgCol, (ldat)(XWidth+(udat)1));
	}
	InsertLast(Gadget, Gadget, Window);
    }
    return Gadget;
}

void FillButton(gadget *Gadget, udat Code, udat Left, udat Up, udat Flags, byte *Text, hwcol Color, hwcol ColorDisabled) {
    byte **Contents;
    udat i, j, XWidth, YWidth;
    
    Gadget->Code=Code;
    Gadget->Left=Left;
    Gadget->Up=Up;
    Gadget->Flags=Flags;
    XWidth=Gadget->XWidth;
    YWidth=Gadget->YWidth;
    Contents=Gadget->Contents;
    
    for (j=(udat)0; j<(YWidth-(udat)1)*XWidth; j+=XWidth)
	for (i=(udat)0; i<XWidth-(udat)1; i++) {
	    Contents[0][i+j] =
		Contents[1][i+j+(udat)1] =
		Contents[2][i+j] =
		Contents[3][i+j+(udat)1] =
		*(Text++);
	    
	    Contents[4][i+j] = Contents[5][i+j+(udat)1] = Color;
	    Contents[6][i+j] = Contents[7][i+j+(udat)1] = ColorDisabled;
	}
}

gadget *CloneButton(gadget *SetUpGadget, udat Code, udat Left, udat Up, hwcol BgCol) {
    gadget *Gadget;
    udat i, XWidth, YWidth;
    byte **Contents;
    
    if (!(Gadget=(gadget *)Clone((obj *)SetUpGadget)))
	return (gadget *)0;
    
    BgCol &= COL(0,MAXCOL);
    Gadget->Code=Code;
    Gadget->Left=Left;
    Gadget->Up=Up;
    
    if (!(Gadget->Flags & WINFL_USE_DEFCOL)) {
	Contents=Gadget->Contents;
	XWidth=Gadget->XWidth;
	YWidth=Gadget->YWidth;
	for (i=(udat)0; i<(YWidth-(udat)1)*XWidth; i+=XWidth) {
	    Contents[4][i+XWidth-(udat)1] =
		Contents[5][i] =
		Contents[6][i+XWidth-(udat)1] =
		Contents[7][i] =
		BgCol;
	}
	for (i=(YWidth-(udat)1)*XWidth; i<XWidth*YWidth; i++) {
	    Contents[4][i] =
		Contents[5][i] =
		Contents[6][i] =
		Contents[7][i] =
		BgCol;
	}
    }
    return Gadget;
}

static fn_gadget _FnGadget = {
    gadget_magic, (uldat)sizeof(gadget), (uldat)1,
	CreateGadget,
	CopyGadget,
	InsertGadget,
	RemoveGadget,
	DeleteGadget,
	CreateButton,
	FillButton,
	CloneButton
};

/* ttydata */

static byte InitTtyData(window *Window) {
    ttydata *Data = Window->TtyData;
    uldat count = Window->MaxNumRow * Window->NumRowOne;
    hwattr *p;
    
    if (!Data) {
	if ((Data = AllocMem(sizeof(ttydata))))
	    Window->TtyData = Data;
	else
	    return FALSE;
    }
    if (!Window->Contents && !(Window->Contents = AllocMem(count * sizeof(hwattr))))
	return FALSE;
    
    p = Window->Contents;
    while (count--)
	*p++ = HWATTR( COL(WHITE,BLACK), ' ' );
    
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
    /*Window->Flags |= WINFL_CURSOR_ON;*/
    Window->ColText = Data->Color = Data->DefColor = Data->saveColor = COL(WHITE,BLACK);
    Data->Underline = COL(HIGH|WHITE,BLACK);
    Data->HalfInten = COL(HIGH|BLACK,BLACK);
    Data->TabStop[0] = 0x01010100;
    Data->TabStop[1] = Data->TabStop[2] = Data->TabStop[3] = Data->TabStop[4] = 0x01010101;
    Data->nPar = 0;
    
    return TRUE;
}

/* window */

static window *CreateWindow(fn_window *Fn_Window, udat LenTitle, byte *Title, hwcol *ColTitle, menu *Menu,
			    hwcol ColText, uldat CursorType, uldat Attrib, byte Flags,
			    udat XWidth, udat YWidth, udat ScrollBackLines) {

    window *Window = (window *)0;
    byte *_Title = NULL;
    hwcol *_ColTitle = NULL;
    
    if ((!Title || (_Title=CloneMem(Title, LenTitle))) &&
	(!ColTitle || (_ColTitle=CloneMem(ColTitle, LenTitle*sizeof(hwcol)))) &&
	(Window=(window *)CreateObj((fn_obj *)Fn_Window))) {
	
	Window->Menu=(menu *)0;
	Window->LenTitle=LenTitle;
	Window->Title=_Title;
	Window->ColTitle=_ColTitle;
	Window->TtyData=(ttydata *)0;
	Window->ShutDownHook=(void (*)(window *))0;
	Window->TransientHook=(void (*)(window *))NoOp;
	Window->MapQueueMsg=(msg *)0;
	Window->RemoteData.Fd = -1;
	Window->RemoteData.ChildPid = (pid_t)0;
	Window->RemoteData.FdSlot = NOSLOT;
	Window->XstSel=Window->YstSel=Window->XendSel=Window->YendSel=0;
        Window->ColGadgets=DEFAULT_ColGadgets;
	Window->ColArrows=DEFAULT_ColArrows;
	Window->ColBars=DEFAULT_ColBars;
	Window->ColTabs=DEFAULT_ColTabs;
	Window->ColBorder=DEFAULT_ColBorder;
	Window->ColText=ColText;
	Window->ColSelect=COL(COLBG(ColText),COLFG(ColText));
	Window->ColDisabled=DEFAULT_ColDisabled;
	Window->ColSelectDisabled=DEFAULT_ColSelectDisabled;
	Window->Attrib=Attrib;
	Window->CursorType=CursorType;
	Window->Flags=Flags;
	Window->Left=0;
	Window->Up=MAXUDAT;
	Window->XLogic=Window->YLogic=Window->CurX=Window->CurY=(uldat)0;
	Window->FirstRow=Window->LastRow=Window->RowOne=Window->RowSplit=(row *)0;
	Window->MaxNumRow=Window->NumRowOne=Window->NumRowSplit=(uldat)0;
	Window->FirstGadget=Window->LastGadget=Window->GadgetSelect=(gadget *)0;
	Window->Contents=(hwattr *)0;

	Window->MinXWidth=MIN_XWIN;
	Window->MinYWidth=MIN_YWIN;
	Window->XWidth = Max2(MIN_XWIN, XWidth);
	Window->YWidth = Max2(MIN_YWIN, YWidth);
	Window->MaxXWidth = MAXUDAT-1;
	Window->MaxYWidth = MAXUDAT-1;

	if (Flags & WINFL_USECONTENTS) {
	    Window->CurY = Window->YLogic = ScrollBackLines;
	    Window->MaxNumRow = ScrollBackLines + YWidth-2;
	    Window->NumRowOne = XWidth-2;
	    if (!InitTtyData(Window)) {
		Delete(Window);
		Window = (window *)0;
	    }
	}
	if (Window && Menu)
	    Act(Own,Window)(Window, Menu);
	return Window;
    }
    if (_Title)
	FreeMem(_Title);
    if (_ColTitle)
	FreeMem(_ColTitle);
    return Window;
}

static window *CopyWindow(window *From, window *To) {
    window *Prev, *Next;
    menu *Menu;
    screen *Screen;

    if (!From || !To)
	return (window *)0;

    Menu = To->Menu;
    Prev = To->OPrev;
    Next = To->ONext;
    
    if ((Screen = To->Screen))
	Act(UnMap,To)(To);

    if (To->Contents)
	FreeMem(To->Contents), To->Contents = (hwattr *)0;
    if (To->TtyData)
	FreeMem(To->TtyData), To->TtyData = (ttydata *)0;
    if (To->FirstRow)
	DeleteList(To->FirstRow);
    if (To->FirstGadget)
	DeleteList(To->FirstGadget);
    
    if ((To = (window *)CopyObj((obj *)From, (obj *)To))) {
	To->Menu = Menu;
	To->OPrev = Prev;
	To->ONext = Next;

	if (From->Contents)
	    To->Contents = CloneMem(From->Contents, From->MaxNumRow*From->NumRowOne*sizeof(hwattr));
	if (From->TtyData) {
	    To->TtyData = CloneMem(From->TtyData, sizeof(ttydata));
	    
	    if (To->Contents) {
		To->TtyData->Start = To->Contents + (From->TtyData->Start - From->Contents);
		To->TtyData->Split = To->Contents + (From->TtyData->Split - From->Contents);
		To->TtyData->Pos   = To->Contents + (From->TtyData->Pos   - From->Contents);
	    } else
		To->TtyData->Start = To->TtyData->Split = To->TtyData->Pos = (hwattr *)0;
	}

	To->RemoteData.Fd = NOFD;
	To->RemoteData.ChildPid = NOPID;
	To->RemoteData.FdSlot = NOSLOT;
	
	if ((!From->Contents || To->Contents) && (!From->TtyData || To->TtyData) &&
	    (!From->FirstRow || (CloneList((obj *)From->FirstRow, (obj **)&To->FirstRow,
					   (obj **)&To->LastRow), To->FirstRow)) &&
	    (!From->FirstGadget || (CloneList((obj *)From->FirstGadget, (obj **)&To->FirstGadget,
					      (obj **)&To->LastGadget), To->FirstGadget))) {
	    
	    if (Screen)
		Act(Map,To)(To, Screen);
	    return To;
	}
	
	if (To->Contents)
	    FreeMem(To->Contents), To->Contents = (hwattr *)0;
	if (To->TtyData)
	    FreeMem(To->TtyData), To->TtyData = (ttydata *)0;
	if (To->FirstRow)
	    DeleteList(To->FirstRow);
	if (To->FirstGadget)
	    DeleteList(To->FirstGadget);

	if (Screen)
	    Act(Map,To)(To, Screen);
    	To = (window *)0;
    }
    return To;
}

static void InsertWindow(window *Window, screen *Parent, window *Prev, window *Next) {
    InsertGeneric((obj *)Window, (obj_parent *)&Parent->FirstWindow, (obj *)Prev, (obj *)Next, (uldat *)0);
}

static void RemoveWindow(window *Window) {
    RemoveGeneric((obj *)Window, (obj_parent *)&Window->Screen->FirstWindow, (uldat *)0);
}

static void DeleteWindow(window *Window) {
    if (Window) {
	if (Window->ShutDownHook)
	    Window->ShutDownHook(Window);
	Act(UnMap,Window)(Window);
	Act(DisOwn,Window)(Window);
	if (Window->Title)
	    FreeMem(Window->Title);
	if (Window->ColTitle)
	    FreeMem(Window->ColTitle);
	if (Window->TtyData)
	    FreeMem(Window->TtyData);
	if (Window->Contents)
	    FreeMem(Window->Contents);
	DeleteList(Window->FirstRow);
	DeleteList(Window->FirstGadget);
	DeleteObj((obj *)Window);
    }
}

static window *SearchWindow(screen *Screen, dat i, dat j, byte *Shaded) {
    window *Window;
    byte Shade, WinFound=FALSE;
    ldat shUp, shLeft, NWinDiMenu;
    
    Shade=!!(All->SetUp->Flags & SETUP_DO_SHADE);
    
    Window = Screen->FirstWindow;
    while (!WinFound && Window) {
	NWinDiMenu = Window->Attrib & WINDOW_MENU ? 0 : (ldat)~(ldat)0;
	shUp = (ldat)Window->Up - ((ldat)Screen->Up & NWinDiMenu) + (ldat)Screen->YLimit;
	shLeft=(ldat)Window->Left-((ldat)Screen->Left & NWinDiMenu);
	
	if ((ldat)j<shUp || (ldat)i<shLeft ||
	    (ldat)j>=(ldat)Window->YWidth + shUp + (ldat)Shade ||
	    (ldat)i>=(ldat)Window->XWidth+shLeft+2*(ldat)Shade) {
	    
	    Window=Window->Next;
	    continue;
	}
	if ((ldat)j>=shUp+(ldat)Window->YWidth || (ldat)i>=shLeft+(ldat)Window->XWidth) {
	    if (((ldat)j>=shUp+(ldat)Window->YWidth && (ldat)i>shLeft+(ldat)1) || ((ldat)i>=shLeft+(ldat)Window->XWidth && (ldat)j>shUp))
		if (Shaded) *Shaded=TRUE;
	    
	    Window=Window->Next;
	    continue;
	}
	WinFound = TRUE;
    }
    
    if (WinFound)
	return Window;
    
    return (window *)0;
}


static void SetColTextWindow(window *Window, hwcol ColText) {
    if (Window)
	Window->ColText = ColText;
}

static void SetColorsWindow(window *Window, udat Bitmap,
			    hwcol ColGadgets, hwcol ColArrows, hwcol ColBars, hwcol ColTabs, hwcol ColBorder,
			    hwcol ColText, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled) {
    if (Window) {
	if (Bitmap & 1)
	    Window->ColGadgets = ColGadgets;
	if (Bitmap & 2)
	    Window->ColArrows = ColArrows;
	if (Bitmap & 4)
	    Window->ColBars = ColBars;
	if (Bitmap & 8)
	    Window->ColTabs = ColTabs;
	if (Bitmap & 0x10)
	    Window->ColBorder = ColBorder;
	if (Bitmap & 0x20)
	    Window->ColText = ColText;
	if (Bitmap & 0x40)
	    Window->ColSelect = ColSelect;
	if (Bitmap & 0x80)
	    Window->ColDisabled = ColDisabled;
	if (Bitmap & 0x100)
	    Window->ColSelectDisabled = ColSelectDisabled;
	if (Window->Screen)
	    DrawBorderWindow(Window, BORDER_ANY);
    }
}

static void ConfigureWindow(window * Window, byte Bitmap, dat Left, udat Up,
			    udat MinXWidth, udat MinYWidth, udat MaxXWidth, udat MaxYWidth) {
    window *Prev, *Next;
    if (Window) {
	if (Window->Screen) {
	    Prev = Window->Prev;
	    Next = Window->Next;
	    Remove(Window);
	    DrawAreaWindow(Window, FALSE);
	}
	
	if (Bitmap & 1)
	    Window->Left = Left;
	if (Bitmap & 2)
	    Window->Up = Up;
	if (Bitmap & 4) {
	    Window->MinXWidth = MinXWidth;
	    Window->XWidth = Max2(MinXWidth, Window->XWidth);
	}
	if (Bitmap & 8) {
	    Window->MinYWidth = MinYWidth;
	    Window->YWidth = Max2(MinYWidth, Window->YWidth);
	}
	if (Bitmap & 0x10) {
	    Window->MaxXWidth = Max2(Window->MinXWidth, MaxXWidth);
	    Window->XWidth = Min2(MaxXWidth, Window->XWidth);
	}
	if (Bitmap & 0x20) {
	    Window->MaxYWidth = Max2(Window->MinYWidth, MaxYWidth);
	    Window->YWidth = Min2(MaxYWidth, Window->YWidth);
	}
	if (Window->Screen) {
	    InsertMiddle(Window, Window, Window->Screen, Prev, Next);
	    DrawAreaWindow(Window, FALSE);
	}
    }
}

static void GotoXYWindow(window *Window, uldat X, uldat Y) {
    if (Window) {
	if (Window->Flags & WINFL_USECONTENTS) {
	    if (X >= Window->NumRowOne)
		X = Window->NumRowOne - 1;
	    if (Y >= Window->TtyData->SizeY)
		Y = Window->TtyData->SizeY - 1;
	    Window->TtyData->X = X;
	    Window->TtyData->Y = Y;
	    Window->TtyData->Pos = Window->TtyData->Start + X + (uldat)Y * Window->TtyData->SizeX;
	    if (Window->TtyData->Pos >= Window->TtyData->Split)
		Window->TtyData->Pos -= Window->TtyData->Split - Window->Contents;
	    Y += Window->MaxNumRow - Window->TtyData->SizeY;
	}
	Window->CurX = X;
	Window->CurY = Y;
	if (All->FirstScreen->FocusWindow == Window)
	    UpdateCursor();
    }
}

window *Create4MenuWindow(fn_window *Fn_Window, menu *Menu) {
    window *Window = (window *)0;
    if (Menu && (Window=Fn_Window->Create
		 (Fn_Window, 0, NULL, (hwcol *)0, Menu, COL(BLACK,WHITE),
		  NOCURSOR, WINDOW_MENU, WINFL_USE_DEFCOL|WINFL_SEL_ROWCURR,
		  MIN_XWIN, MIN_YWIN, 0))) {
	
	Act(SetColors,Window)(Window, 0x1FF, COL(0,0), COL(0,0), COL(0,0), COL(0,0), COL(HIGH|WHITE,WHITE),
			      COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK));
	Act(Configure,Window)(Window, 0x3F, 0, 0, MIN_XWIN, MIN_YWIN, MAXUDAT, MAXUDAT);
    }
    return Window;
}

static void MapWindow(window *Window, screen *Screen) {
    msg *Msg;
    if (WM_MsgPort && (Msg = Do(Create,Msg)(FnMsg, MSG_MAP, sizeof(event_map)))) {
	Msg->Event.EventMap.Window = Window;
	Msg->Event.EventMap.Code   = (udat)0;
	Msg->Event.EventMap.Screen = Screen;
	Window->MapQueueMsg = Msg;
	SendMsg(WM_MsgPort, Msg);
    } else
	Act(RealMap,Window)(Window,Screen);
}

static void RealMapWindow(window *Window, screen *Screen) {
    window *OldWindow;
    
    if (Screen && Window && !Window->Screen) {
	if (Window->MapQueueMsg)
	    /*
	     * let the upper layer do this:
	     * Delete(Window->MapQueueMsg);
	     */
	    Window->MapQueueMsg = (msg *)0;
	
	if (Window->Up == MAXUDAT)
	    Window->Up = !!Screen->YLimit + Screen->Up;
	
	InsertFirst(Window, Window, Screen);
	Window->Screen = Screen;
	/*
	 * warning: if Window is transient and All->TransientWindow
	 * was already non-zero, we are in trouble as we can't handle
	 * two or more TransientWindows
	 */
	if (Window->Attrib & WINDOW_TRANSIENT)
	    All->TransientWindow = Window;
	
	if (Screen == All->FirstScreen) {
	    OldWindow = Act(Focus,Window)(Window);
	    if (OldWindow)
		DrawBorderWindow(OldWindow, BORDER_ANY);
	    UpdateCursor();
	}
	DrawAreaWindow(Window, FALSE);
	if (!(Window->Attrib & WINDOW_MENU))
	    DrawMenuBar(Screen, MINDAT, MAXDAT);

	if (All->TransientWindow)
	    All->TransientWindow->TransientHook(All->TransientWindow);
    }
}

static void UnMapWindow(window *Window) {
    screen *Screen;
    window *NextWindow;
    byte wasFocus;
    
    if (!Window)
	return;
    
    if ((Screen = Window->Screen)) {
	if (Window == Screen->MenuWindow) {
	    /*
	     * ! DANGER ! 
	     * Trying to UnMap() a window while its menu is being used.
	     * UnMap() the menu window first!
	     */
	    Act(UnMap,Window)(Screen->FocusWindow);
	}
	    
	if ((wasFocus = Window == Screen->FocusWindow))
	    NextWindow = Window == Screen->FirstWindow ? Window->Next : Screen->FirstWindow;
	    
	Remove(Window);
	DrawAreaWindow(Window, FALSE);
	Window->Screen = (screen *)0;

	if (Window->Attrib & WINDOW_TRANSIENT && All->TransientWindow == Window)
	    All->TransientWindow = (window *)0;

	if (wasFocus) {
	    if (Screen == All->FirstScreen) {
		/* in case the user was dragging this window... */
		All->FlagsMove &= ~GLMOVE_1stWIN;
		if (NextWindow) {
		    (void)Act(Focus,NextWindow)(NextWindow);
		    DrawBorderWindow(NextWindow, BORDER_ANY);
		} else
		    Do(Focus,Window)((window *)0);
		if (!(Window->Attrib & WINDOW_MENU))
		    DrawMenuBar(Screen, MINDAT, MAXDAT);
		UpdateCursor();
	    } else
		Screen->FocusWindow = NextWindow;
	}
	
	if (All->TransientWindow)
	    All->TransientWindow->TransientHook(All->TransientWindow);
	
    } else if (Window->MapQueueMsg) {
	/* the window was still waiting to be mapped! */
	Delete(Window->MapQueueMsg);
	Window->MapQueueMsg = (msg *)0;
    }
}

static void OwnWindow(window *Window, menu *Menu) {
    if (!Window->Menu) {
	OwnGeneric((obj *)Window, (obj_parent *)&Menu->FirstWindow);
	Window->Menu = Menu;
    }
}

static void DisOwnWindow(window *Window) {
    if (Window->Menu) {
	DisOwnGeneric((obj *)Window, (obj_parent *)&Window->Menu->FirstWindow);
	Window->Menu = (menu *)0;
    }
}

/*
static window *SearchCoordScreen(dat x, dat y, uldat *ResX, uldat *ResY) {
    screen *Screen;
    window *Window;
    ldat shLeft, shUp;
    
    if (!(Screen=SearchScreen(y)) ||
	!(Window=SearchWindow(Screen, x, y, (byte *)0)))
	return (window *)0;
    
    if ((shLeft=(ldat)Window->Left-(ldat)Screen->Left*!(Window->Attrib & WINDOW_MENU)) < (ldat)x &&
	(shUp = (ldat)Window->Up - (ldat)Screen->Up*!(Window->Attrib & WINDOW_MENU)+(ldat)Screen->YLimit) < (ldat)y &&
	shLeft+(ldat)Window->XWidth-(ldat)1>(ldat)x &&
	shUp+(ldat)Window->YWidth-(ldat)1>(ldat)y) {
	if (ResX)
	    *ResX=(uldat)((ldat)x-shLeft-(ldat)1)+Window->XLogic;
	if (ResY)
	    *ResY=(uldat)((ldat)y-shUp-(ldat)1)+Window->YLogic;
	return Window;
    }
    return (window *)0;
}
*/

#if !defined(CONF_TERM)
window *SimpleFocus(window *Window) {
    window *oldWin = All->FirstScreen->FocusWindow;
    All->FirstScreen->FocusWindow = Window;
    return oldWin;
}
#endif

#if !defined(CONF_TERM) && defined(CONF_MODULES)
void FakeWriteAscii(window *Window, uldat Len, byte *Text) {
    if (DlLoad(TermSo) && Window->Fn->WriteAscii != FakeWriteAscii)
	Act(WriteAscii,Window)(Window, Len, Text);
}

void FakeWriteHWAttr(window *Window, udat x, udat y, uldat Len, hwattr *Attr) {
    if (DlLoad(TermSo) && Window->Fn->WriteHWAttr != FakeWriteHWAttr)
	Act(WriteHWAttr,Window)(Window, x, y, Len, Attr);
}
#endif

static fn_window _FnWindow = {
    window_magic, (uldat)sizeof(window), (uldat)1,
	CreateWindow,
	CopyWindow,
	InsertWindow,
	RemoveWindow,
	DeleteWindow,
	SetColTextWindow,
	SetColorsWindow,
	ConfigureWindow,
	GotoXYWindow,
	Create4MenuWindow,
	RealMapWindow,
	MapWindow,
	UnMapWindow,
	OwnWindow,
	DisOwnWindow,
#if defined(CONF_TERM)
	KbdFocus,
	WriteAscii,
	WriteHWAttr,
#elif defined(CONF_MODULES)
	SimpleFocus,
	FakeWriteAscii,
	FakeWriteHWAttr,
#else
	SimpleFocus,
	(void (*)(window *, uldat, byte *))NoOp,
	(void (*)(window *, uldat, hwattr *))NoOp,
#endif
	WriteRow,
	SearchRow,
	SearchRowCode,
	SearchGadget,
	SearchGadgetCode
};

/* menuitem */

static menuitem *CreateMenuItem(fn_menuitem *Fn_MenuItem, menu *Menu, window *Window, byte FlagActive,
				udat Left, udat Len, udat ShortCut, byte *Name) {
    menuitem *MenuItem = (menuitem *)0;
    byte *_Name = NULL;
    
    if (Window && Menu && Name && (_Name=CloneMem(Name,Len)) &&
	(MenuItem=(menuitem *)CreateObj((fn_obj *)Fn_MenuItem))) {	  
	MenuItem->FlagActive=FlagActive;
	MenuItem->Left=Left;
	MenuItem->Len=Len;
	MenuItem->ShortCut=ShortCut;
	MenuItem->Name=_Name;
	MenuItem->Window=Window;
	InsertLast(MenuItem, MenuItem, Menu);
	return MenuItem;
    }
    if (_Name)
	FreeMem(_Name);
    return MenuItem;
}

static menuitem *CopyMenuItem(menuitem *From, menuitem *To) {
    menu *Menu;
    
    if (!From || !To)
	return (menuitem *)0;
    
    Menu = To->Menu;
    
    if (To->Name)
	FreeMem(To->Name), To->Name = NULL;
    if (To->Window)
	Delete(To->Window), To->Window = (window *)0;

    if ((To = (menuitem *)CopyObj((obj *)From, (obj *)To))) {
	To->Menu = Menu;
    
	if ((!From->Name || (To->Name=CloneMem(From->Name, (uldat)From->Len))) &&
	    (!From->Window || (To->Window=(window *)Clone((obj *)From->Window))))
	    return To;
	    
	if (To->Name)
	    FreeMem(To->Name), To->Name = NULL, To->Len = 0;
	if (To->Window)
	    Delete(To->Window), To->Window = (window *)0;
	    
	To = (menuitem *)0;
    }    
    return To;
}

static void InsertMenuItem(menuitem *MenuItem, menu *Parent, menuitem *Prev, menuitem *Next) {
    if (!MenuItem->Menu && Parent) {
	InsertGeneric((obj *)MenuItem, (obj_parent *)&Parent->FirstMenuItem, (obj *)Prev, (obj *)Next, (uldat *)0);
	MenuItem->Menu = Parent;
    }
}

static void RemoveMenuItem(menuitem *MenuItem) {
    if (MenuItem->Menu) {
	RemoveGeneric((obj *)MenuItem, (obj_parent *)&MenuItem->Menu->FirstMenuItem, (uldat *)0);
	MenuItem->Menu = (menu *)0;
    }
}

static void DeleteMenuItem(menuitem *MenuItem) {
    if (MenuItem) {
	Remove(MenuItem);
	if (MenuItem->Window)
	    Delete(MenuItem->Window);
	if (MenuItem->Name)
	    FreeMem(MenuItem->Name);
	DeleteObj((obj *)MenuItem);
    }
}

static menuitem *SearchMenuItem(menu *Menu, dat i) {
    menuitem *MenuItem;
    byte MenuItemFound=FALSE;
    
    if (Menu && (MenuItem=Menu->FirstMenuItem)) {
	while (MenuItem) {
	    if (i<MenuItem->Left || i>=MenuItem->Left + MenuItem->Len) {
		MenuItem=MenuItem->Next;
		continue;
	    }
	    MenuItemFound = TRUE;
	    break;
	}
    
	if (MenuItemFound)
	    return MenuItem;
    }
    return (menuitem *)0;
}

menuitem *Create4MenuMenuItem(fn_menuitem *Fn_MenuItem, menu *Menu, window *Window, byte FlagActive,
			      udat Len, byte *Name) {
    udat Left, ShortCut;
    
    if (!Menu || !Window)
	return (menuitem *)0;
    
    if (Menu->LastMenuItem)
	Left=Menu->LastMenuItem->Left+Menu->LastMenuItem->Len;
    else
	Left=(udat)1;

    ShortCut=(udat)0;
    while (ShortCut<Len && Name[ShortCut]==' ')
	ShortCut++;
    
    if (Window)
	Window->Left=Left;
    
    return (Fn_MenuItem->Create)(Fn_MenuItem, Menu, Window, FlagActive, Left, Len, ShortCut, Name);
}

menuitem *Create4MenuCommonMenuItem(fn_menuitem *Fn_MenuItem, menu *Menu) {
    menuitem *Item;
    window *Window;
    
    if ((Window=Win4Menu(Menu)) &&
	Row4Menu(Window, COD_COMMON_RESIZE,   ROW_ACTIVE, 15," Move / Resize ") &&
	Row4Menu(Window, COD_COMMON_SCROLL,   ROW_ACTIVE, 15," Scroll        ") &&
	Row4Menu(Window, COD_COMMON_CENTER,   ROW_ACTIVE, 15," Center        ") &&
	Row4Menu(Window, COD_COMMON_ZOOM,     ROW_ACTIVE, 15," Zoom          ") &&
	Row4Menu(Window, COD_COMMON_MAXZOOM,  ROW_ACTIVE, 15," \x11ZOOM\x10        ") &&
	Row4Menu(Window, (udat)0,             ROW_IGNORE, 15,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ") &&
	Row4Menu(Window, COD_COMMON_HOTKEY,   ROW_ACTIVE, 15," Send HotKey   ") &&
	Row4Menu(Window, COD_COMMON_RAISELOWER,ROW_ACTIVE,15," Raise / Lower ") &&
	Row4Menu(Window, COD_COMMON_NEXT,     ROW_ACTIVE, 15," Next          ") &&
	Row4Menu(Window, COD_COMMON_WINLIST,  ROW_ACTIVE, 15," List...       ") &&
	Row4Menu(Window, (udat)0,             ROW_IGNORE, 15,"ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ") &&
	Row4Menu(Window, COD_COMMON_CLOSE,    ROW_ACTIVE, 15," Close         ")) {
	
	if ((Item = Fn_MenuItem->Create4Menu(Fn_MenuItem, Menu, Window, TRUE, 8, " Window ")))
	    return Item;
	
	Act(Delete,Window)(Window);
    }
    return (menuitem *)0;
}

static fn_menuitem _FnMenuItem = {
    menuitem_magic, (uldat)sizeof(menuitem), (uldat)1,
	CreateMenuItem,
	CopyMenuItem,
	InsertMenuItem,
	RemoveMenuItem,
	DeleteMenuItem,
	Create4MenuMenuItem,
	Create4MenuCommonMenuItem
};

/* menu */

static menu *CreateMenu(fn_menu *Fn_Menu, msgport *MsgPort, hwcol ColItem, hwcol ColSelect,
			hwcol ColDisabled, hwcol ColSelectDisabled, hwcol ColShtCut, hwcol ColSelShtCut,
			byte FlagDefColInfo) {
    menu *Menu = (menu *)0;
    
    if (MsgPort && (Menu=(menu *)CreateObj((fn_obj *)Fn_Menu))) {
	Menu->ColItem=ColItem;
	Menu->ColSelect=ColSelect;
	Menu->ColDisabled=ColDisabled;
	Menu->ColSelectDisabled=ColSelectDisabled;
	Menu->ColShtCut=ColShtCut;
	Menu->ColSelShtCut=ColSelShtCut;
	Menu->FirstMenuItem=Menu->LastMenuItem=Menu->MenuItemSelect=(menuitem *)0;
	Menu->FlagDefColInfo=FlagDefColInfo;
	Menu->Info=(row *)0;
	Menu->FirstWindow=Menu->LastWindow=(window *)0;
	InsertLast(Menu, Menu, MsgPort);
    }
    return Menu;
}

static menu *CopyMenu(menu *From, menu *To) {
    msgport *MsgPort;
    window *First, *Last;
    
    if (!From || !To)
	return (menu *)0;
    
    MsgPort = To->MsgPort;
    First = To->FirstWindow;
    Last = To->LastWindow;
    
    if (To->FirstMenuItem)
	DeleteList(To->FirstMenuItem);
    if (To->Info)
	Delete(To->Info), To->Info=(row *)0;
    
    if ((To = (menu *)CopyObj((obj *)From, (obj *)To))) {
	To->MsgPort = MsgPort;
	To->FirstWindow = First;
	To->LastWindow = Last;

	To->MenuItemSelect=(menuitem *)0;

	if ((!From->FirstMenuItem || (CloneList((obj *)From->FirstMenuItem, (obj **)&To->FirstMenuItem,
						(obj **)&To->LastMenuItem), To->FirstMenuItem)) &&
	    (!From->Info || (To->Info=(row *)Clone((obj *)From->Info))))
	  
	    return To;
	    
	if (To->FirstMenuItem)
	    DeleteList(To->FirstMenuItem);
	if (To->Info)
	    Delete(To->Info), To->Info=(row *)0;
	    
	To = (menu *)0;
    }
    return To;
}

static void InsertMenu(menu *Menu, msgport *Parent, menu *Prev, menu *Next) {
    if (!Menu->MsgPort && Parent) {
	InsertGeneric((obj *)Menu, (obj_parent *)&Parent->FirstMenu, (obj *)Prev, (obj *)Next, (uldat *)0);
	Menu->MsgPort = Parent;
    }
}

static void RemoveMenu(menu *Menu) {
   if (Menu->MsgPort) {
       RemoveGeneric((obj *)Menu, (obj_parent *)&Menu->MsgPort->FirstMenu, (uldat *)0);
       Menu->MsgPort = (msgport *)0;
   }
}

static void DeleteMenu(menu *Menu) {
    if (Menu) {
	Remove(Menu);
	/*
	 * problem:
	 * there's currently no way to write in each menuitem that its window
	 * was Delete()d by DeleteList(Menu->FirstWindow),
	 * while we can trivially do the opposite way: Delete(MenuItem)
	 * correctly updates the window list in the Menu.
	 * 
	 * So we _FIRST_ DeleteList(Menu->FirstMenuItem) 
	 * _THEN_ DeleteList(Menu->FirstWindow).
	 */
	DeleteList(Menu->FirstMenuItem);
	DeleteList(Menu->FirstWindow);
	if (Menu->Info)
	    Delete(Menu->Info);
	DeleteObj((obj *)Menu);
    }
}

static row *SetInfoMenu(menu *Menu, byte Flags, uldat Len, byte *Text, hwcol *ColText) {
    row *Row;
    if ((Row = Do(Create,Row)(FnRow, (udat)0, Flags))) {
	if ((!Text || (Row->Text=CloneMem(Text,Len))) &&
	    (!ColText || (Row->ColText=CloneMem(ColText, Len*sizeof(hwcol))))) {
	    Row->Len = Row->MaxLen = Len;
	    if (Menu->Info)
		Delete(Menu->Info);
	    return Menu->Info = Row;
	}
	Delete(Row);
	Row = (row *)0;
    }
    return Row;
}


static fn_menu _FnMenu = {
    menu_magic, (uldat)sizeof(menu), (uldat)1,
	CreateMenu,
	CopyMenu,
	InsertMenu,
	RemoveMenu,
	DeleteMenu,
	SetInfoMenu,
	SearchMenuItem
};

/* screen */

static screen *CreateScreen(fn_screen *Fn_Screen, udat BgWidth, udat BgHeight, hwattr *Bg) {
    screen *Screen = (screen *)0;
    size_t size;
    
    if ((size=(size_t)BgWidth * BgHeight * sizeof(hwattr)) &&
	(Screen=(screen *)CreateObj((fn_obj *)Fn_Screen)) &&
	(Screen->Bg = AllocMem(size))) {

	Screen->FirstWindow=Screen->LastWindow=
	    Screen->FocusWindow=Screen->MenuWindow=(window *)0;
	Screen->ScreenWidth=Screen->ScreenHeight=Screen->YLimit=Screen->Up=1;
	Screen->Attrib=Screen->Left=0;
	Screen->BgWidth=BgWidth;
	Screen->BgHeight=BgHeight;
	CopyMem(Bg, Screen->Bg, size);
    } else if (Screen) {
	if (Screen->Bg)
	    FreeMem(Screen->Bg);
	DeleteObj((obj *)Screen);
	Screen=(screen *)0;
    }
    return Screen;
}

static screen *CreateSimpleScreen(fn_screen *Fn_Screen, hwattr Bg) {
    hwattr bg[1];
    bg[0] = Bg;
    return Fn_Screen->Create(Fn_Screen, 1, 1, bg);
}

static void BgImageScreen(screen *Screen, udat BgWidth, udat BgHeight, hwattr *Bg) {
    size_t size;
    
    if (Screen && Bg && (size=(size_t)BgWidth * BgHeight * sizeof(hwattr)) &&
	(Screen->Bg = ReAllocMem(Screen->Bg, size))) {

	Screen->BgWidth=BgWidth;
	Screen->BgHeight=BgHeight;
	CopyMem(Bg, Screen->Bg, size);
	DrawArea((screen *)0, (window *)0, (window *)0, (gadget *)0, (gadget *)0, 0, Screen->YLimit, Screen->ScreenWidth-1, Screen->ScreenHeight-1, FALSE);
    }
}

static screen *CopyScreen(screen *From, screen *To) {
    all *_All;
    window *First, *Last, *Focus;
    
    if (!From || !To)
	return (screen *)0;
    
    First = To->FirstWindow;
    Last = To->LastWindow;
    Focus = To->FocusWindow;
    _All = To->All;
    
    if ((To = (screen *)CopyObj((obj *)From, (obj *)To))) {
	To->FirstWindow = First;
	To->LastWindow = Last;
	To->FocusWindow = Focus;
	if ((To->All = _All))
	    DrawScreen(To);
    }
    return To;
}

static void InsertScreen(screen *Screen, all *Parent, screen *Prev, screen *Next) {
    if (!Screen->All && Parent) {
	InsertGeneric((obj *)Screen, (obj_parent *)&Parent->FirstScreen, (obj *)Prev, (obj *)Next, (uldat *)0);
	Screen->All = Parent;
    }
}

static void RemoveScreen(screen *Screen) {
    if (Screen->All) {
	RemoveGeneric((obj *)Screen, (obj_parent *)&Screen->All->FirstScreen, (uldat *)0);
	Screen->All = (all *)0;
    }
}

static void DeleteScreen(screen *Screen) {
    if (Screen) {
	while (Screen->FirstWindow)
	    Act(UnMap,Screen->FirstWindow)(Screen->FirstWindow);
	Remove(Screen);
	DeleteObj((obj *)Screen);
    }
}

static menu *SearchMenu(screen *Screen) {
    if (Screen) {
	if (Screen->FocusWindow)
	    return Screen->FocusWindow->Menu;
	return All->BuiltinMenu;
    }
    return (menu *)0;
}

static screen *SearchScreen(dat j) {
    screen *CurrScreen;
    byte VirtScrFound=FALSE;
    
    CurrScreen = All->FirstScreen;
    while (CurrScreen &&
	  !(VirtScrFound = (j+1 >= (dat)CurrScreen->YLimit)))
	CurrScreen=CurrScreen->Next;
    
    if (VirtScrFound)
	return CurrScreen;
    
    return (screen *)0;
}

static fn_screen _FnScreen = {
    screen_magic, (uldat)sizeof(screen), (uldat)1,
	CreateScreen,
	CopyScreen,
	InsertScreen,
	RemoveScreen,
	DeleteScreen,
	SearchWindow,
	SearchMenu,
	SearchScreen,
	CreateSimpleScreen,
	BgImageScreen
};

/* msg */

#define Delta ((size_t)&(((msg *)0)->Event))

static msg *CreateMsg(fn_msg *Fn_Msg, udat Type, udat EventLen) {
    msg *Msg;
    
    if ((Msg = (msg *)AllocMem(EventLen + Delta))) {
	if (AssignId((fn_obj *)Fn_Msg, (obj *)Msg)) {
	    (Msg->Fn=Fn_Msg)->Used++;
	    Msg->Prev=Msg->Next=(msg *)0;
	    Msg->MsgPort=(msgport *)0;
	    Msg->Type = Type;
	    Msg->Len = EventLen;
	    return Msg;
	} else
	    FreeMem(Msg), Msg = (msg *)0;
    }
    return Msg;
    
}

static msg *CopyMsg(msg *From, msg *To) {
    uldat Id = To->Id;
    /* we allow From->Fn and To->Fn to be different,
     * as long as they share the same magic */
    if (From->Fn->Magic == To->Fn->Magic) {
	From->Fn->Used++;
	if (!--To->Fn->Used)
	    FreeMem(To->Fn);
	CopyMem(From, To, From->Len + Delta);
	To->Id = Id;
	return To;
    }
    return (msg *)0;
}

#undef Delta

static void InsertMsg(msg *Msg, msgport *Parent, msg *Prev, msg *Next) {
    if (!Msg->MsgPort && Parent) {
	/* if adding the first msg, move the msgport to the head
	 * of msgport list, so that the scheduler will run it */
	if (!Parent->FirstMsg && Parent->All)
	    MoveFirst(MsgPort, All, Parent);
	
	InsertGeneric((obj *)Msg, (obj_parent *)&Parent->FirstMsg, (obj *)Prev, (obj *)Next, (uldat *)0);
	Msg->MsgPort = Parent;
    }
}

static void RemoveMsg(msg *Msg) {
    if (Msg->MsgPort) {
	RemoveGeneric((obj *)Msg, (obj_parent *)&Msg->MsgPort->FirstMsg, (uldat *)0);
	Msg->MsgPort = (msgport *)0;
    }
}

static void DeleteMsg(msg *Msg) {
    if (Msg) {
	Remove(Msg);
	DeleteObj((obj *)Msg);
    }
}

static fn_msg _FnMsg = {
    msg_magic, (uldat)sizeof(msg), (uldat)1,
	CreateMsg,
	CopyMsg,
	InsertMsg,
	RemoveMsg,
	DeleteMsg
};

/* msgport */

static msgport *CreateMsgPort(fn_msgport *Fn_MsgPort, byte NameLen, byte *ProgramName,
			      time_t PauseSec, frac_t PauseFraction,
			      byte WakeUp, void (*Handler)(msgport *)) {
    msgport *MsgPort = (msgport *)0;
    byte *Name;
    
    if (Handler && (!ProgramName || (Name = CloneMem(ProgramName, NameLen))) &&
	(MsgPort=(msgport *)CreateObj((fn_obj *)Fn_MsgPort))) {
	
	MsgPort->WakeUp=WakeUp;
	MsgPort->NameLen=NameLen;
	MsgPort->ProgramName=Name;
	MsgPort->Handler=Handler;
	MsgPort->ShutDownHook=(void (*)(msgport *))0;
	MsgPort->PauseDuration.Seconds=PauseSec;
	MsgPort->PauseDuration.Fraction=PauseFraction;
	(void)SumTime(&MsgPort->CallTime, &All->Now, &MsgPort->PauseDuration);
	MsgPort->RemoteData.Fd=NOFD;
	MsgPort->RemoteData.ChildPid=NOPID;
	MsgPort->RemoteData.FdSlot=NOSLOT;
	MsgPort->FirstMsg=MsgPort->LastMsg=(msg *)0;
	MsgPort->FirstMenu=MsgPort->LastMenu=(menu *)0;
	MsgPort->FirstMutex=MsgPort->LastMutex=(mutex *)0;
	InsertMiddle(MsgPort, MsgPort, All,
		     WakeUp ? (msgport *)0 : All->LastMsgPort,
		     WakeUp ? All->FirstMsgPort : (msgport *)0);
	SortMsgPortByCallTime(MsgPort);
    } else if (NameLen && Name)
	FreeMem(Name);
    return MsgPort;
}

static msgport *CopyMsgPort(msgport *From, msgport *To) {
    /* quite meaningless */
    return (msgport *)0;
}

static void InsertMsgPort(msgport *MsgPort, all *Parent, msgport *Prev, msgport *Next) {
    if (!MsgPort->All && Parent) {
	InsertGeneric((obj *)MsgPort, (obj_parent *)&Parent->FirstMsgPort, (obj *)Prev, (obj *)Next, (uldat *)0);
	MsgPort->All = Parent;
    }
}

static void RemoveMsgPort(msgport *MsgPort) {
    if (MsgPort->All) {
	if (All->RunMsgPort == MsgPort)
	    All->RunMsgPort = MsgPort->Next;
	RemoveGeneric((obj *)MsgPort, (obj_parent *)&MsgPort->All->FirstMsgPort, (uldat *)0);
	MsgPort->All = (all *)0;
    }
}

static void DeleteMsgPort(msgport *MsgPort) {
    if (MsgPort) {
	if (MsgPort->ShutDownHook)
	    MsgPort->ShutDownHook(MsgPort);
	
	DeleteList(MsgPort->FirstMsg);
	DeleteList(MsgPort->FirstMutex);
	DeleteList(MsgPort->FirstMenu);
	
	Remove(MsgPort);
	if (MsgPort->ProgramName)
	    FreeMem(MsgPort->ProgramName);
	DeleteObj((obj *)MsgPort);
    }
}

static fn_msgport _FnMsgPort = {
    msgport_magic, (uldat)sizeof(msgport), (uldat)1,
	CreateMsgPort,
	CopyMsgPort,
	InsertMsgPort,
	RemoveMsgPort,
	DeleteMsgPort,
};
    
/* mutex */

static fn_mutex _FnMutex;

mutex *CreateMutex(fn_mutex *Fn_Mutex, msgport *MsgPort,
			   byte NameLen, byte *Name, byte Perm) {
    byte Possible = PERM_WRITE;
    mutex *CurrMutex, *NewMutex = (mutex *)0, *AlreadyMutex = (mutex *)0;
    
    if (!Perm || !MsgPort)
	return NewMutex;
    
    CurrMutex = All->FirstMutex;
    while (CurrMutex && Possible) {
	if (NameLen == CurrMutex->NameLen && !CmpMem(Name, CurrMutex->Name, NameLen)) {
	    if (CurrMutex->MsgPort==MsgPort) {
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
	else if ((NewMutex = (mutex *)CreateObj((fn_obj *)Fn_Mutex))) {
	    NewMutex->Perm = Perm;
	    InsertLast(Mutex, NewMutex, All);
	    NewMutex->MsgPort = (msgport *)0;
	    Act(Own,NewMutex)(NewMutex, MsgPort);
	}
    }
    return NewMutex;
}

/* you can't Copy a mutex */
static mutex *CopyMutex(mutex *From, mutex *To) {
    return (mutex *)0;
}

static void InsertMutex(mutex *Mutex, all *Parent, mutex *Prev, mutex *Next) {
    if (!Mutex->All && Parent) {
	InsertGeneric((obj *)Mutex, (obj_parent *)&Mutex->All->FirstMutex, (obj *)Prev, (obj *)Next, (uldat *)0);
	Mutex->All = Parent;
    }
}

static void RemoveMutex(mutex *Mutex) {
    if (Mutex->All) {
	RemoveGeneric((obj *)Mutex, (obj_parent *)&Mutex->All->FirstMutex, (uldat *)0);
	Mutex->All = (all *)0;
    }
}

static void DeleteMutex(mutex *Mutex) {
    if (Mutex) {
	Act(DisOwn,Mutex)(Mutex);
	Remove(Mutex);
	DeleteObj((obj *)Mutex);
    }
}

static void OwnMutex(mutex *Mutex, msgport *Parent) {
    if (!Mutex->MsgPort && Parent) {
	OwnGeneric((obj *)Mutex, (obj_parent *)&Parent->FirstMutex);
	Mutex->MsgPort = Parent;
    }
}

static void DisOwnMutex(mutex *Mutex) {
    if (Mutex->MsgPort) {
	DisOwnGeneric((obj *)Mutex, (obj_parent *)&Mutex->MsgPort->FirstMutex);
	Mutex->MsgPort = (msgport *)0;
    }
}

static fn_mutex _FnMutex = {
    mutex_magic, (uldat)sizeof(mutex), (uldat)1,
	CreateMutex,
	CopyMutex,
	InsertMutex,
	RemoveMutex,
	DeleteMutex,
	OwnMutex,
	DisOwnMutex
};


/* module */

static module *CreateModule(fn_module *Fn_Module, uldat NameLen, byte *Name) {
    module *Module = (module *)0;
    byte *newName = NULL;
    
    if (Name && (newName = CloneMem(Name, NameLen+1)) &&
	(Module=(module *)CreateObj((fn_obj *)Fn_Module))) {
	
	newName[NameLen] = '\0';
	Module->NameLen = NameLen;
	Module->Name = newName;
	Module->Handle = Module->Private = NULL;
	InsertLast(Module, Module, All);
    } else if (newName)
	FreeMem(newName);
    return Module;
}

static module *CopyModule(module *From, module *To) {
    /* what for ? */
    return (module *)0;
}

static void InsertModule(module *Module, all *Parent, module *Prev, module *Next) {
    if (!Module->All && Parent) {
	InsertGeneric((obj *)Module, (obj_parent *)&Parent->FirstModule, (obj *)Prev, (obj *)Next, (uldat *)0);
	Module->All = Parent;
    }
}

static void RemoveModule(module *Module) {
    if (Module->All) {
	RemoveGeneric((obj *)Module, (obj_parent *)&Module->All->FirstModule, (uldat *)0);
	Module->All = (all *)0;
    }
}

static void DeleteModule(module *Module) {
    if (Module) {
	Act(DlClose,Module)(Module);
	Remove(Module);
	if (Module->Name)
	    FreeMem(Module->Name);
	DeleteObj((obj *)Module);
    }
}

static fn_module _FnModule = {
    module_magic, (uldat)sizeof(module), (uldat)1,
	CreateModule,
	CopyModule,
	InsertModule,
	RemoveModule,
	DeleteModule,
#ifdef CONF_MODULES
	DlOpen,
	DlClose
#else
	(byte (*)(module *))AlwaysFalse,
	(void (*)(module *))NoOp
#endif
};


fn Fn = {
    &_FnObj,
	&_FnArea,
	&_FnAreaWin,
	&_FnRow,
	&_FnGadget,
	&_FnWindow,
	&_FnMenuItem,
	&_FnMenu,
	&_FnScreen,
	&_FnMsg,
	&_FnMsgPort,
	&_FnMutex,
	&_FnModule
};

