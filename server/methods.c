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
#include "extreg.h"

#include "draw.h"
#include "printk.h"
#include "resize.h"
#include "util.h"
#include "hw.h"
#include "hw_multi.h"

#include <Tw/Tw.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
# include <Tutf/Tutf_defs.h>
#endif
#ifdef CONF_TERM
# include "tty.h"
#endif
#ifdef CONF__MODULES
# include "dl.h"
#endif
#ifdef CONF_WM
# include "wm.h"
#endif
#ifdef CONF_EXT
# include "extensions/ext_query.h"
#endif


#ifdef CONF__UNICODE
extern hwattr extra_POS_INSIDE;
#else
# define extra_POS_INSIDE 0
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
	    Obj->Parent = (obj)0;
	} else {
	    FreeMem(Obj);
	    Obj = (obj)0;
	}
    }
    return Obj;
}

static void InsertObj(obj Obj, obj Parent, obj Prev, obj Next) {
    printk("twin: internal error: virtual function InsertObj() called!\n");
#if 0
    if (!Obj->Parent && Parent) {
	InsertGeneric(Obj, Parent, Prev, Next, (ldat *)0);
	Obj->Parent = (obj)Parent;
    }
#endif
}

static void RemoveObj(obj Obj) {
    printk("twin: internal error: virtual function RemoveObj() called!\n");
#if 0
    if (Obj->Parent) {
	RemoveGeneric(Obj, (obj_parent)Obj->Parent, (ldat *)0);
	Obj->Parent = (obj)0;
    }
#endif
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
    DeleteObj,
    (void *)NoOp
};

/* widget */


static widget CreateWidget(fn_widget Fn_Widget, msgport Owner, dat XWidth, dat YWidth, uldat Attrib, uldat Flags, dat Left, dat Up, hwattr Fill) {
    widget W = (widget)0;

    if (Owner && (W=(widget)Fn_Widget->Fn_Obj->Create((fn_obj)Fn_Widget))) {
	Fn_Widget->Fn_Obj->Used++;
	
	W->FirstW = W->LastW = W->SelectW = (widget)0;
	W->Left = Left;
	W->Up   = Up;
	W->XWidth = XWidth;
	W->YWidth = YWidth;
	W->Attrib = Attrib;
	W->Flags = Flags;
	W->XLogic = W->YLogic = 0;
	W->O_Prev = W->O_Next = (widget)0;
	W->Owner = (msgport)0;
	
	W->ShutDownHook = (fn_hook)0;
	W->Hook = (fn_hook)0;
	W->WhereHook = (fn_hook *)0;
	W->MapUnMapHook = (fn_hook)0;
	W->MapQueueMsg = (msg)0;

	W->USE_Fill = Fill;
	if (w_USE(W, USEEXPOSE))
	    WriteMem(&W->USE.E, '\0', sizeof(W->USE.E));

	Act(Own,W)(W, Owner);
    }
    return W;
}
    
static void InsertWidget(widget W, widget Parent, widget Prev, widget Next) {
    if (Parent)
	/*
	 * don't check W->Parent here, as Raise() and Lower() call
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
    if (W->Hook)
	Act(RemoveHook,W)(W, W->Hook, W->WhereHook);
    if (W->ShutDownHook)
	W->ShutDownHook(W);
    Act(DisOwn,W)(W);
    while (W->FirstW)
	Act(UnMap,W->FirstW)(W->FirstW);

    (Fn_Obj->Delete)((obj)W);
    if (!--Fn_Obj->Used)
	FreeMem(Fn_Obj);
}

static void SetFillWidget(widget W, hwattr Fill) {
    if (W->USE_Fill != Fill) {
	W->USE_Fill = Fill;
	if (W->Parent)
	    DrawAreaWidget(W);
    }
}

static void ChangeFieldWidget(widget W, udat field, uldat CLEARMask, uldat XORMask) {
    uldat i;
    
    if (W) switch (field) {
      case TWS_widget_Left:
      case TWS_widget_Up:
      case TWS_widget_Width:
      case TWS_widget_Height:
	break;
      case TWS_widget_USE_Fill:
	i = (hwattr)((W->USE_Fill & ~CLEARMask) ^ XORMask);
	SetFillWidget(W, i);
	break;
      case TWS_widget_XLogic:
      case TWS_widget_YLogic:
	break;
      default:
	W->Fn->Fn_Obj->ChangeField((obj)W, field, CLEARMask, XORMask);
	break;
    }
}

widget FocusWidget(widget W) {
    widget oldW;
    if (W)
	oldW = Act(KbdFocus,W)(W);
    else
	oldW = Do(KbdFocus,Widget)(W);
    
    if (W != oldW && (!W || W->Parent == (widget)All->FirstScreen)) {
	if (W && IS_WINDOW(W)) DrawBorderWindow((window)W, BORDER_ANY);
	if (oldW && IS_WINDOW(oldW)) DrawBorderWindow((window)oldW, BORDER_ANY);
	if ((W && IS_WINDOW(W)) || (oldW && IS_WINDOW(oldW))) {
	    UpdateCursor();
	    if (!W || !IS_WINDOW(W) || !(((window)W)->Flags & WINDOWFL_MENU))
		Act(DrawMenu,All->FirstScreen)(All->FirstScreen, 0, MAXDAT);
	}
    }
    return oldW;
}

#ifndef CONF_TERM
# define TtyKbdFocus FakeKbdFocus
widget FakeKbdFocus(widget W) {
    widget oldW;
    widget P;
    screen Screen = W && (P=W->Parent) && IS_SCREEN(P) ? (screen)P : All->FirstScreen;
    
    if (Screen) {
	oldW = Screen->FocusW;
	Screen->FocusW = W;
    } else
	oldW = (widget)0;
	    
    return oldW;
}
#endif

static gadget FindGadgetByCode(widget Parent, udat Code) {
    widget W;
    
    for (W = Parent->FirstW; W; W = W->Next) {
	if (IS_GADGET(W) && ((gadget)W)->Code == Code)
	    return (gadget)W;
    }
    return (gadget)0;
}


static void IncMouseMotionN(void) {
    if (!All->MouseMotionN++)
	EnableMouseMotionEvents(TRUE);
}

static void DecMouseMotionN(void) {
    if (All->MouseMotionN && !--All->MouseMotionN)
	EnableMouseMotionEvents(FALSE);
}


static void MapWidget(widget W, widget Parent) {
    msg Msg;
    
    if (W && !W->Parent && !W->MapQueueMsg && Parent) {
	if (IS_SCREEN(Parent)) {
	    if (Ext(WM,MsgPort) && (Msg = Do(Create,Msg)(FnMsg, MSG_MAP, 0))) {
		Msg->Event.EventMap.W = W;
		Msg->Event.EventMap.Code   = 0;
		Msg->Event.EventMap.Screen = (screen)Parent;
		W->MapQueueMsg = Msg;
		SendMsg(Ext(WM,MsgPort), Msg);
	    } else
		Act(MapTopReal,W)(W, (screen)Parent);
	} else if (IS_WIDGET(Parent)) {
	    if (W->Up == MAXDAT) {
		W->Left = Parent->XLogic;
		W->Up = Parent->YLogic;
	    }
	    InsertFirst(W, W, Parent);
	    W->Parent = Parent;
	
	    DrawAreaWidget(W);
	    
	    if (W->Attrib & (WIDGET_WANT_MOUSE_MOTION|WIDGET_AUTO_FOCUS))
		IncMouseMotionN();
	    
	    if (W->MapUnMapHook)
		W->MapUnMapHook(W);
	}
    }
}

static void MapTopRealWidget(widget W, screen Screen) {
    widget OldW;
    
    if (Screen && !W->Parent && (!IS_WINDOW(W) || ((window)W)->Menu)) {
	if (W->MapQueueMsg)
	    /*
	     * let the upper layer do this:
	     * Delete(W->MapQueueMsg);
	     */
	    W->MapQueueMsg = (msg)0;
	
	if (W->Up == MAXDAT) {
	    W->Left = Screen->XLogic;
	    W->Up = Max2(Screen->YLimit+1, 0) + Screen->YLogic;
	} else {
	    W->Left += Screen->XLogic;
	    W->Up += Screen->YLogic;
	}
	
	InsertFirst(W, W, (widget)Screen);
	W->Parent = (widget)Screen;

	/* top-level widgets must be visible */
	W->Flags &= ~WINDOWFL_NOTVISIBLE;

	if (W->Attrib & (WIDGET_WANT_MOUSE_MOTION|WIDGET_AUTO_FOCUS))
	    IncMouseMotionN();
	
	if (Screen == All->FirstScreen) {
	    OldW = Act(KbdFocus,W)(W);
	    if (OldW && IS_WINDOW(OldW))
		DrawBorderWindow((window)OldW, BORDER_ANY);
	    UpdateCursor();
	}
	if (IS_WINDOW(W))
	    DrawAreaWindow2((window)W);
	else
	    DrawAreaWidget(W);
	if (!(W->Flags & WINDOWFL_MENU))
	    Act(DrawMenu,Screen)(Screen, 0, MAXDAT);

	if (W->MapUnMapHook)
	    W->MapUnMapHook(W);
	
	if (Screen->FnHookW)
	    Screen->FnHookW(Screen->HookW);
    }
}

static void UnMapWidget(widget W) {
    widget Parent;
    window Next;
    screen Screen;
    byte wasFocus;
    
    if (W && (Parent = W->Parent)) {
	if (IS_SCREEN(Parent)) {
	    if ((Screen = (screen)Parent) == All->FirstScreen &&
		W == (widget)Screen->MenuWindow) {
		/*
		 * ! DANGER ! 
		 * Trying to UnMap() the menu owner.
		 * shutdown the menu first!
		 */
		CloseMenu();
	    }

	    if (W->Attrib & (WIDGET_WANT_MOUSE_MOTION|WIDGET_AUTO_FOCUS))
		DecMouseMotionN();

	    if (Screen->ClickWindow == (window)W)
		Screen->ClickWindow = NULL;
	
	    if ((wasFocus = W == Screen->FocusW)) {
		if (W->Flags & WINDOWFL_MENU)
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
	    if (IS_WINDOW(W))
		DrawAreaWindow2((window)W);
	    else
		DrawAreaWidget(W);
	    
	    if (IS_SCREEN(Parent)) {
		W->Left = 0;
		W->Up = MAXDAT;
	    }
	    W->Parent = (widget)0;

	    if (wasFocus) {
		if (Screen == All->FirstScreen) {
		    /*
		     * in case the user was dragging this window...
		     */
		    if ((All->State & STATE_ANY) < STATE_MENU)
			All->State &= ~STATE_ANY;
		    
		    if (Next) {
			(void)Act(KbdFocus,Next)(Next);
			DrawBorderWindow(Next, BORDER_ANY);
		    } else
			Do(KbdFocus,Window)((window)0);
		    if (!(W->Flags & WINDOWFL_MENU))
			Act(DrawMenu,Screen)(Screen, 0, MAXDAT);
		    UpdateCursor();
		} else
		    Screen->FocusW = (widget)Next;
	    }

	    if (W->MapUnMapHook)
		W->MapUnMapHook(W);

	    if (Screen->FnHookW)
		Screen->FnHookW(Screen->HookW);
	
	} else {
	    /* UnMap() a sub-window */
	    if (W == Parent->SelectW)
		Parent->SelectW = (widget)0;
	
	    Remove(W);
	    DrawAreaWidget(W);
	    W->Parent = (widget)0;
	    
	    if (W->Attrib & (WIDGET_WANT_MOUSE_MOTION|WIDGET_AUTO_FOCUS))
		DecMouseMotionN();
	    
	    if (W->MapUnMapHook)
		W->MapUnMapHook(W);
	}
    } else if (W->MapQueueMsg) {
	/* the window was still waiting to be mapped! */
	Delete(W->MapQueueMsg);
	W->MapQueueMsg = (msg)0;
    }
}

static void RaiseW(widget W) {
    RaiseWidget(W, FALSE);
}

static void LowerW(widget W) {
    LowerWidget(W, FALSE);
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
	if ((Widget->O_Next = Owner->FirstW))
	    Owner->FirstW->O_Prev = Widget;
	else
	    Owner->LastW=Widget;	
    
	Widget->O_Prev = (widget)0;
	Owner->FirstW = Widget;
	Widget->Owner = Owner;
    }
}

static void DisOwnWidget(widget W) {
    msgport Owner;
    if ((Owner = W->Owner)) {
	if (W->O_Prev)
	    W->O_Prev->O_Next = W->O_Next;
	else if (Owner->FirstW == W)
	    Owner->FirstW = W->O_Next;
	
	if (W->O_Next)
	    W->O_Next->O_Prev = W->O_Prev;
	else if (Owner->LastW == W)
	    Owner->LastW = W->O_Prev;
	
	W->O_Prev = W->O_Next = (widget)0;
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

static byte InstallHookWidget(widget W, fn_hook Hook, fn_hook *WhereHook) {
    if (W && !W->Hook && !W->WhereHook && Hook &&
	WhereHook && !WhereHook[0] && !WhereHook[1]) {
	
	W->Hook = WhereHook[0] = Hook;
	W->WhereHook = WhereHook;
	WhereHook[1] = (void *)W;
	return TRUE;
    }
    return FALSE;
}

static void RemoveHookWidget(widget W, fn_hook Hook, fn_hook *WhereHook) {
    if (W && Hook && W->Hook == Hook &&
	WhereHook && W->WhereHook == WhereHook &&
	WhereHook[0] == Hook && WhereHook[1] == (void *)W) {
	
	W->Hook = *WhereHook = (fn_hook)0;
	W->WhereHook = (fn_hook *)0;
	WhereHook[1] = (void *)0;
    }
}

static struct s_fn_widget _FnWidget = {
    widget_magic, sizeof(struct s_widget), (uldat)1,
    CreateWidget,
    InsertWidget,
    RemoveWidget,
    DeleteWidget,
    ChangeFieldWidget,
    &_FnObj,
    DrawSelfWidget, /* exported by draw.c */
    FindWidgetAt,   /* exported by draw.c */
    FindGadgetByCode,
    SetXYWidget,
    SetFillWidget,
    FocusWidget,
    TtyKbdFocus,
    MapWidget,
    UnMapWidget,
    MapTopRealWidget,
    RaiseW,
    LowerW,
    OwnWidget,
    DisOwnWidget,
    RecursiveDeleteWidget,
    ExposeWidget2,    /* exported by resize.c */
    InstallHookWidget,
    RemoveHookWidget,
};


/* gadget */

static gadget CreateGadget
    (fn_gadget Fn_Gadget, msgport Owner, widget Parent, dat XWidth, dat YWidth,
     CONST byte *TextNormal, uldat Attrib, uldat Flags, udat Code, 
     hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
     dat Left, dat Up)
{
    gadget G = (gadget)0;
    ldat Size;

    if (Owner && Code < COD_RESERVED && XWidth > 0 && YWidth > 0 && 
	(G=(gadget)Fn_Gadget->Fn_Widget->Create
	 ((fn_widget)Fn_Gadget, Owner, XWidth, YWidth, Attrib, Flags, Left, Up, HWATTR(ColText, ' ')))) {
	
	Fn_Gadget->Fn_Widget->Used++;

	G->ColText=ColText;
	G->ColSelect=ColTextSelect;
	G->ColDisabled=ColTextDisabled;
	G->ColSelectDisabled=ColTextSelectDisabled;
	G->Code=Code;

	G->G_Prev = G->G_Next = (gadget)0;
	G->Group = (group)0;
	
	if (G_USE(G, USETEXT)) {
	    Size = (ldat)XWidth*YWidth;
	    if (TextNormal)
		G->USE.T.Text[0] = CloneStr2HWFont(TextNormal, Size);
	    else
		G->USE.T.Text[0] = NULL;
	    
	    G->USE.T.Text[1] = G->USE.T.Text[2] = G->USE.T.Text[3] = NULL;
	    G->USE.T.Color[0] = G->USE.T.Color[1] = G->USE.T.Color[2] = G->USE.T.Color[3] = NULL;
	}
	
	/* G->Flags |= GADGETFL_TEXT_DEFCOL; */
	
	if (Parent)
	    Act(Map,G)(G, Parent);
    }
    return G;
}

static void DeleteGadget(gadget G) {
    fn_widget Fn_Widget = G->Fn->Fn_Widget;
    byte i;
    
    Act(UnMap,G)(G);
    if (G_USE(G, USETEXT)) {
	for (i=0; i<4; i++) {
	    if (G->USE.T.Text[i])
		FreeMem(G->USE.T.Text[i]);
	    if (G->USE.T.Color[i])
		FreeMem(G->USE.T.Color[i]);
	}
    }
    (Fn_Widget->Delete)((widget)G);
    if (!--Fn_Widget->Used)
	FreeMem(Fn_Widget);
}

static void ChangeFieldGadget(gadget G, udat field, uldat CLEARMask, uldat XORMask) {
    uldat i, mask;
    
    if (G) switch (field) {
      case TWS_gadget_ColText:
      case TWS_gadget_ColSelect:
      case TWS_gadget_ColDisabled:
      case TWS_gadget_ColSelectDisabled:
      case TWS_gadget_Code:
	break;
      case TWS_gadget_Flags:
	mask = GADGETFL_DISABLED|GADGETFL_TEXT_DEFCOL|GADGETFL_PRESSED|GADGETFL_TOGGLE;
	CLEARMask &= mask;
	XORMask &= mask;
	i = (G->Flags & ~CLEARMask) ^ XORMask;
	if ((i & mask) != (G->Flags & mask)) {
	    if ((i & GADGETFL_PRESSED) != (G->Flags & GADGETFL_PRESSED)) {
		if (i & GADGETFL_PRESSED)
		    PressGadget(G);
		else
		    UnPressGadget(G, TRUE);
	    }
	    mask = GADGETFL_DISABLED|GADGETFL_TEXT_DEFCOL;
	    if ((i & mask) != (G->Flags & mask)) {
		G->Flags = i;
		DrawAreaWidget((widget)G);
	    } else
		G->Flags = i;
	}
	break;
      default:
	G->Fn->Fn_Widget->ChangeField((widget)G, field, CLEARMask, XORMask);
	break;
    }
}


static gadget CreateEmptyButton(fn_gadget Fn_Gadget, msgport Owner, dat XWidth, dat YWidth, hwcol BgCol) {
    gadget G;
    ldat Size;
    byte i;
    dat j, k;
#ifdef CONF__UNICODE
# define _FULL  T_UTF_16_FULL_BLOCK
# define _LOWER T_UTF_16_LOWER_HALF_BLOCK
# define _UPPER T_UTF_16_UPPER_HALF_BLOCK
#else
# define _FULL  ((byte)'Û')
# define _LOWER ((byte)'Ü')
# define _UPPER ((byte)'ß')
#endif
    if ((G=(gadget)Fn_Gadget->Fn_Widget->Create
	 ((fn_widget)Fn_Gadget, Owner, ++XWidth, ++YWidth, 0, GADGETFL_USETEXT|GADGETFL_BUTTON, 0, 0, (hwattr)0))) {
	
	Size = (ldat)XWidth * YWidth;
	
	for (i=0; i<4; i++)
	    G->USE.T.Text[i] = NULL, G->USE.T.Color[i] = NULL;
	
	for (i=0; i<4; i++)
	    if (!(G->USE.T.Text[i]=(hwfont *)AllocMem(Size * sizeof(hwfont))) ||
		!(G->USE.T.Color[i]=(hwcol *)AllocMem(Size * sizeof(hwcol)))) {
		
		Act(Delete,G)(G);
		return (gadget)0;
	    }

	Fn_Gadget->Fn_Widget->Used++;
	
	Size = (ldat)--XWidth * --YWidth;
	BgCol &= COL(0,MAXCOL);
	
	for (i=0; i<4; i++) {
	    for (j=k=(dat)0; j<YWidth; j++, k+=XWidth+1) {
		G->USE.T.Text [i][k+(i & 1 ? 0 : XWidth)] = i & 1 ? ' ' : k ? _FULL : _LOWER;
		G->USE.T.Color[i][k+(i & 1 ? 0 : XWidth)] = BgCol;
	    }
	    G->USE.T.Text[i][k] = ' ';
#if TW_SIZEOFHWFONT == 1
	    WriteMem((void *)(G->USE.T.Text [i]+k+1), i & 1 ? ' ' : _UPPER, XWidth);
#else
	    for (j=(dat)0; j<XWidth; j++)
		G->USE.T.Text[i][k+1+j] = i & 1 ? ' ' : _UPPER;
#endif
#if TW_SIZEOFHWCOL == 1
	    WriteMem((void *)(G->USE.T.Color[i]+k), BgCol, XWidth+1);
#else
	    for (j=(dat)0; j<=XWidth; j++)
		G->USE.T.Color[i][k+j] = BgCol;
#endif
	}
	
	G->G_Prev = G->G_Next = (gadget)0;
	G->Group = (group)0;
    }
    return G;
#undef _FULL
#undef _UPPER
#undef _LOWER
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
    G->Flags = (Flags & ~GADGETFL_USEANY) | GADGETFL_USETEXT | GADGETFL_BUTTON;
    XWidth = G->XWidth;
    YWidth = G->YWidth;
    
    T = Text;
    for (j=(dat)0; j<(YWidth-(dat)1)*XWidth; j+=XWidth) {
	for (i=(dat)0; i<XWidth-(dat)1; i++) {
	    G->USE.T.Text[0][i+j] = G->USE.T.Text[1][i+j+1] =
		G->USE.T.Text[2][i+j] = G->USE.T.Text[3][i+j+1] =
#ifdef CONF__UNICODE
		Tutf_CP437_to_UTF_16[*(T++)];
#else
		*(T++);
#endif
	    
	    G->USE.T.Color[0][i+j] = G->USE.T.Color[1][i+j+1] = Color;
	    G->USE.T.Color[2][i+j] = G->USE.T.Color[3][i+j+1] = ColorDisabled;
	}
    }
    if (Parent)
	Act(Map,G)(G, Parent);
    
    return TRUE;
}


static gadget CreateButton(fn_gadget Fn_Gadget, widget Parent, dat XWidth, dat YWidth, CONST byte *Text,
			    uldat Flags, udat Code, hwcol BgCol, hwcol Col, hwcol ColDisabled,
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
    ChangeFieldGadget,
    /* widget */
    &_FnObj,
    DrawSelfGadget,       /* exported by draw.c */
    (void *)FindWidgetAt, /* exported by draw.c */
    (void *)FindGadgetByCode,
    (void *)SetXYWidget,
    (void *)SetFillWidget,
    (void *)FocusWidget,
    (void *)TtyKbdFocus,
    (void *)MapWidget,
    (void *)UnMapWidget,
    (void *)MapTopRealWidget,
    (void *)RaiseW,
    (void *)LowerW,
    (void *)OwnWidget,
    (void *)DisOwnWidget,
    (void *)RecursiveDeleteWidget,
    (void *)ExposeWidget2,	/* exported by resize.c */
    (void *)InstallHookWidget,
    (void *)RemoveHookWidget,
    /* gadget */
    &_FnWidget,
    CreateEmptyButton,
    FillButton,
    CreateButton,
    WriteTextsGadget,	/* exported by resize.c */
    WriteHWFontsGadget,	/* exported by resize.c */
};

/* ttydata */

static byte InitTtyData(window Window, dat ScrollBackLines) {
    ttydata *Data = Window->USE.C.TtyData;
    ldat count = Window->WLogic * Window->HLogic;
    hwattr *p = Window->USE.C.Contents, h;
    
    if (!Data && !(Window->USE.C.TtyData = Data = AllocMem(sizeof(ttydata))))
	return FALSE;

    if (!p && !(Window->USE.C.Contents = p = AllocMem(count * sizeof(hwattr))))
	return FALSE;
    
    h = HWATTR( COL(WHITE,BLACK), ' ') | extra_POS_INSIDE;
    while (count--)
	*p++ = h;
    
    /*
     * this is a superset of reset_tty(),
     * but we don't want to call it from here
     */
    Data->State = ESnormal;
    Data->Flags = TTY_AUTOWRAP;
    Data->Effects = 0;
    Window->YLogic = Window->CurY = Data->ScrollBack = ScrollBackLines;
    Window->USE.C.HSplit = 0;
    Data->SizeX = Window->WLogic;
    Data->SizeY = Window->HLogic - ScrollBackLines;
    Data->Top = 0;
    Data->Bottom = Data->SizeY;
    Data->saveX = Data->X = Window->CurX = 0;
    Data->saveY = Data->Y = 0;
    Data->Pos = Data->Start = Window->USE.C.Contents + Data->ScrollBack * Window->WLogic;
    Data->Split = Window->USE.C.Contents + Window->WLogic * Window->HLogic;
    
    Window->CursorType = LINECURSOR;
    /* respect the WINDOWFL_CURSOR_ON set by the client and don't force it on */
#if 0
    Window->Flags |= WINDOWFL_CURSOR_ON;
#endif
    Window->ColText = Data->Color = Data->DefColor = Data->saveColor = COL(WHITE,BLACK);
    Data->Underline = COL(HIGH|WHITE,BLACK);
    Data->HalfInten = COL(HIGH|BLACK,BLACK);
    Data->TabStop[0] = 0x01010100;
    Data->TabStop[1] = Data->TabStop[2] = Data->TabStop[3] = Data->TabStop[4] = 0x01010101;
    Data->nPar = 0;

    Data->G = Data->saveG = 0;
    /*
     * default to latin1 charset if CONF__UNICODE is enabled
     */
#ifdef CONF__UNICODE
    Data->currG = Data->G0 = Data->saveG0 = LAT1_MAP;
#else
    Data->currG = Data->G0 = Data->saveG0 = IBMPC_MAP;
#endif	
    Data->G1 = Data->saveG1 = GRAF_MAP;

    Data->utf = Data->utf_count = Data->utf_char = 0;
#ifdef CONF__UNICODE
    Data->InvCharset = Tutf_UTF_16_to_ISO_8859_1;
#endif
    Data->newLen = Data->newMax = 0;
    Data->newName = NULL;
    
    return TRUE;
}

/* window */

static window CreateWindow(fn_window Fn_Window, msgport Owner,
			   dat TitleLen, CONST byte *Title, CONST hwcol *ColTitle, menu Menu,
			   hwcol ColText, uldat CursorType, uldat Attrib, uldat Flags,
			   dat XWidth, dat YWidth, dat ScrollBackLines) {

    window Window = (window)0;
    byte *_Title = NULL;
    hwcol *_ColTitle = NULL;
    byte HasBorder = 2 * !(Flags & WINDOWFL_BORDERLESS);
    
    /* overflow safety */
    if (HasBorder) {
	if ((dat)(XWidth + HasBorder) > 0)
	    XWidth += HasBorder;
	if ((dat)(YWidth + HasBorder) > 0)
	    YWidth += HasBorder;
    }
    
    if ((!Title || (_Title=CloneStrL(Title, TitleLen))) &&
	(!ColTitle || (_ColTitle=CloneMem(ColTitle, TitleLen*sizeof(hwcol)))) &&
	Owner && (Window=(window)Fn_Window->Fn_Widget->Create
		  ((fn_widget)Fn_Window, Owner, XWidth, YWidth,
		   Attrib, Flags, 0, MAXDAT, HWATTR(ColText, ' ')))) {
	Window->Menu = Menu;
	Window->MenuItem = (menuitem)0;
	Window->NameLen = TitleLen;
	Window->Name = _Title;
	Window->ColName = _ColTitle;
	Window->BorderPattern[0] = Window->BorderPattern[1] = (void *)0;
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
	if (Window->Flags & WINDOWFL_BORDERLESS)
	    Window->Attrib &= ~WINDOW_ROLLED_UP;
	if (Window->Attrib & WINDOW_WANT_KEYS)
	    Window->Attrib &= ~WINDOW_AUTO_KEYS;
	
	Window->State = (uldat)0;
	Window->CursorType = CursorType;

	Window->MinXWidth=MIN_XWIN;
	Window->MinYWidth=MIN_YWIN;
	Window->XWidth = XWidth = Max2(MIN_XWIN, XWidth);
	Window->YWidth = YWidth = Max2(MIN_YWIN, YWidth);
	Window->MaxXWidth = MAXDAT;
	Window->MaxYWidth = MAXDAT;

#ifdef CONF__UNICODE
	Window->Charset = Tutf_CP437_to_UTF_16;
#endif
	WriteMem(&Window->USE, '\0', sizeof(Window->USE));

	if (W_USE(Window, USECONTENTS)) {
	    if (MAXDAT - ScrollBackLines < YWidth - HasBorder)
		ScrollBackLines = MAXDAT - YWidth + HasBorder;
	    Window->CurY = Window->YLogic = ScrollBackLines;
	    Window->WLogic = XWidth - HasBorder;
	    Window->HLogic = ScrollBackLines + YWidth - HasBorder;
	    if (!InitTtyData(Window, ScrollBackLines)) {
		Act(Delete,Window)(Window);
		Window = (window)0;
	    }
	} else if (W_USE(Window, USEROWS)) {
	    Window->WLogic = 1024; /* just an arbitrary value... */
	    Window->HLogic = 0;    /* no rows */
	} else {
	    Window->WLogic = XWidth - HasBorder;
	    Window->HLogic = ScrollBackLines + YWidth - HasBorder;
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
    if (W->Name)
	FreeMem(W->Name);
    if (W->ColName)
	FreeMem(W->ColName);
    if (W_USE(W, USECONTENTS)) {
	if (W->USE.C.TtyData)
	    FreeMem(W->USE.C.TtyData);
	if (W->USE.C.Contents)
	    FreeMem(W->USE.C.Contents);
    } else if (W_USE(W, USEROWS))
	DeleteList(W->USE.R.FirstRow);
	
    (Fn_Widget->Delete)((widget)W);
    if (!--Fn_Widget->Used)
	FreeMem(Fn_Widget);
}


static void ChangeFieldWindow(window W, udat field, uldat CLEARMask, uldat XORMask) {
    uldat i, mask;
    
    if (W) switch (field) {
      case TWS_window_CurX:
      case TWS_window_CurY:
      case TWS_window_XstSel:
      case TWS_window_YstSel:
      case TWS_window_XendSel:
      case TWS_window_YendSel:
	/* FIXME: finish this */
	break;
      case TWS_window_ColGadgets:
      case TWS_window_ColArrows:
      case TWS_window_ColBars:
      case TWS_window_ColTabs:
      case TWS_window_ColBorder:
      case TWS_window_ColText:
      case TWS_window_ColSelect:
      case TWS_window_ColDisabled:
      case TWS_window_ColSelectDisabled:
	{
	    hwcol *C = NULL;
	    switch (field) {
	      case TWS_window_ColGadgets:	C = &W->ColGadgets; break;
	      case TWS_window_ColArrows:	C = &W->ColArrows; break;
	      case TWS_window_ColBars:		C = &W->ColBars; break;
	      case TWS_window_ColTabs:		C = &W->ColTabs; break;
	      case TWS_window_ColBorder:	C = &W->ColBorder; break;
	      case TWS_window_ColText:		C = &W->ColText; break;
	      case TWS_window_ColSelect:	C = &W->ColSelect; break;
	      case TWS_window_ColDisabled:	C = &W->ColDisabled; break;
	      case TWS_window_ColSelectDisabled:C = &W->ColSelectDisabled; break;
	      default: break;
	    }
	    i = (*C & ~CLEARMask) ^ XORMask;
	    if (i != *C) {
		*C = i;
		/* FIXME: this is an overkill */
		DrawAreaWidget((widget)W);
	    }
	}
	break;
      case TWS_window_Flags:
	mask = WINDOWFL_CURSOR_ON;
	CLEARMask &= mask;
	XORMask &= mask;
	i = (W->Flags & ~CLEARMask) ^ XORMask;
	if ((i & mask) != (W->Flags & mask)) {
	    W->Flags = i;
	    if (ContainsCursor((widget)W))
		UpdateCursor();
	}
	break;
      case TWS_window_Attrib:
	mask = WINDOW_WANT_KEYS|WINDOW_WANT_MOUSE|WINDOW_WANT_CHANGES|WINDOW_AUTO_FOCUS|
	    WINDOW_DRAG|WINDOW_RESIZE|WINDOW_CLOSE|WINDOW_ROLLED_UP|WINDOW_X_BAR|
	    WINDOW_Y_BAR|WINDOW_AUTO_KEYS|WINDOW_WANT_MOUSE_MOTION;
	CLEARMask &= mask;
	XORMask &= mask;
	i = (W->Attrib & ~CLEARMask) ^ XORMask;
	if ((i & mask) != (W->Attrib & mask)) {
	    if ((i & WINDOW_ROLLED_UP) != (W->Attrib & WINDOW_ROLLED_UP))
		RollUpWindow(W, !!(i & WINDOW_ROLLED_UP));
	    if ((i & (WINDOW_WANT_MOUSE_MOTION|WIDGET_AUTO_FOCUS)) != (W->Attrib & (WINDOW_WANT_MOUSE_MOTION|WIDGET_AUTO_FOCUS)) && W->Parent) {
		if (i & (WINDOW_WANT_MOUSE_MOTION|WIDGET_AUTO_FOCUS))
		    IncMouseMotionN();
		else
		    DecMouseMotionN();
	    }
	    mask = WINDOW_RESIZE|WINDOW_CLOSE|WINDOW_X_BAR|WINDOW_Y_BAR;
	    if ((i & mask) != (W->Attrib & mask) && W->Parent) {
		W->Attrib = i;
		DrawBorderWindow(W, BORDER_ANY);
	    } else
		W->Attrib = i;
	}
	break;
      case TWS_window_State:
      case TWS_window_CursorType:
      case TWS_window_MinXWidth:
      case TWS_window_MinYWidth:
      case TWS_window_MaxXWidth:
      case TWS_window_MaxYWidth:
      case TWS_window_WLogic:
      case TWS_window_HLogic:
	break;
      default:
	W->Fn->Fn_Widget->ChangeField((widget)W, field, CLEARMask, XORMask);
	break;
    }
}

static void SetTitleWindow(window W, dat titlelen, byte *title) {
    widget P;
    
    if (W->Name)
	FreeMem(W->Name);
    
    W->NameLen = titlelen;
    W->Name = title;
    
#if 1
    /*
     * do not allow changing window borders just because
     * some untrusted application set a new title
     */
    DrawBorderWindow(W, BORDER_UP);
#else
    /* user may have title-dependent borders in ~/.twinrc, honour them: */
    Win->BorderPattern[0] = Win->BorderPattern[1] = NULL;
    DrawBorderWindow(W, BORDER_ANY);
#endif
    
    if ((P = W->Parent) && IS_SCREEN(P)) {
	/* need to update window list with new name ? */
	if (((screen)P)->FnHookW)
	    ((screen)P)->FnHookW(((screen)P)->HookW);
    }
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
    if (Bitmap & 0x20) {
	W->ColText = ColText;
	if (W_USE(W, USECONTENTS))
	    W->USE.C.TtyData->Color = ColText;
    }
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
	if (IS_SCREEN(W->Parent)) {
	    W->Left += W->Parent->XLogic;
	    W->Up += W->Parent->YLogic;
	}
	InsertMiddle(W, W, W->Parent, Prev, Next);
	DrawAreaWindow2(W);
    }
}

static void ConfigureWindow(window W, byte Bitmap, dat Left, dat Up,
			    dat MinXWidth, dat MinYWidth, dat MaxXWidth, dat MaxYWidth) {
    widget Prev, Next;
    dat HasBorder = 2 * !(W->Flags & WINDOWFL_BORDERLESS);

    if (W->Parent) {
	Prev = W->Prev;
	Next = W->Next;
	Remove(W);
	DrawAreaWindow2(W);
    }
	
    if (Bitmap & 1) {
	W->Left = Left;
	if (W->Parent && IS_SCREEN(W->Parent))
	    W->Left += W->Parent->XLogic;
    }
    if (Bitmap & 2) {
	W->Up = Up;
	if (W->Parent && IS_SCREEN(W->Parent))
	    W->Up += W->Parent->YLogic;
    }

    if (Bitmap & 4) {
	if (MinXWidth <= MAXDAT - HasBorder)
	    MinXWidth = Max2(MinXWidth, MinXWidth + HasBorder);
	W->MinXWidth = MinXWidth;
	W->XWidth = Max2(MinXWidth, W->XWidth);
    }
    if (Bitmap & 8) {
	if (MinYWidth <= MAXDAT - HasBorder)
	    MinYWidth = Max2(MinYWidth, MinYWidth + HasBorder);
	W->MinYWidth = MinYWidth;
	W->YWidth = Max2(MinYWidth, W->YWidth);
    }
    if (Bitmap & 0x10) {
	if (MaxXWidth <= MAXDAT - HasBorder)
	    MaxXWidth = Max2(MaxXWidth, MaxXWidth + HasBorder);
	W->MaxXWidth = Max2(W->MinXWidth, MaxXWidth);
	W->XWidth = Min2(MaxXWidth, W->XWidth);
    }
    if (Bitmap & 0x20) {
	if (MaxYWidth <= MAXDAT - HasBorder)
	    MaxYWidth = Max2(MaxYWidth, MaxYWidth + HasBorder);
	W->MaxYWidth = Max2(W->MinYWidth, MaxYWidth);
	W->YWidth = Min2(MaxYWidth, W->YWidth);
    }
    if (W->Parent) {
	InsertMiddle(W, W, W->Parent, Prev, Next);
	DrawAreaWindow2(W);
    }
}

static void GotoXYWindow(window Window, ldat X, ldat Y) {
    if (W_USE(Window, USECONTENTS)) {
	ttydata *TT = Window->USE.C.TtyData;
	
	X = Max2(X, 0);
	Y = Max2(Y, 0);
	if (X >= TT->SizeX)
	    X = TT->SizeX - 1;
	if (Y >= TT->SizeY)
	    Y = TT->SizeY - 1;
	TT->X = X;
	TT->Y = Y;
	TT->Pos = TT->Start + X + (ldat)Y * TT->SizeX;
	if (TT->Pos >= TT->Split)
	    TT->Pos -= TT->Split - Window->USE.C.Contents;
	Y += Window->HLogic - TT->SizeY;
    }
    Window->CurX = X;
    Window->CurY = Y;
    if (ContainsCursor((widget)Window))
	UpdateCursor();
}

window Create4MenuWindow(fn_window Fn_Window, menu Menu) {
    window Window = (window)0;
    if (Menu && (Window=Fn_Window->Create
		 (Fn_Window, Menu->MsgPort, 0, NULL, (hwcol *)0, Menu, COL(BLACK,WHITE),
		  NOCURSOR, WINDOW_AUTO_KEYS,
		  WINDOWFL_MENU|WINDOWFL_USEROWS|WINDOWFL_ROWS_DEFCOL|WINDOWFL_ROWS_SELCURRENT,
		  MIN_XWIN, MIN_YWIN, 0))) {
	
	Act(SetColors,Window)(Window, 0x1FF, COL(0,0), COL(0,0), COL(0,0), COL(0,0), COL(HIGH|WHITE,WHITE),
			      COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK));
	Act(Configure,Window)(Window, 0x3F, 0, 1, MIN_XWIN, MIN_YWIN, MAXDAT, MAXDAT);
    }
    return Window;
}

#ifndef CONF_TERM
# ifdef CONF__MODULES
void FakeWriteAscii(window Window, ldat Len, CONST byte *Ascii) {
    if (DlLoad(TermSo) && Window->Fn->TtyWriteAscii != FakeWriteAscii)
	Act(TtyWriteAscii,Window)(Window, Len, Ascii);
}

void FakeWriteString(window Window, ldat Len, CONST byte *String) {
    if (DlLoad(TermSo) && Window->Fn->TtyWriteString != FakeWriteString)
	Act(TtyWriteString,Window)(Window, Len, String);
}

void FakeWriteHWFont(window Window, ldat Len, CONST hwfont *HWFont) {
    if (DlLoad(TermSo) && Window->Fn->TtyWriteHWFont != FakeWriteHWFont)
	Act(TtyWriteHWFont,Window)(Window, Len, HWFont);
}

void FakeWriteHWAttr(window Window, dat x, dat y, ldat Len, CONST hwattr *Attr) {
    if (DlLoad(TermSo) && Window->Fn->TtyWriteHWAttr != FakeWriteHWAttr)
	Act(TtyWriteHWAttr,Window)(Window, x, y, Len, Attr);
}

window FakeOpenTerm(CONST byte *arg0, byte * CONST *argv) {
    if (DlLoad(TermSo) && Ext(Term,Open) != FakeOpenTerm)
	return Ext(Term,Open)(arg0, argv);
    return NULL;
}
# else  /* CONF__MODULES */
#  define FakeWriteAscii  (void (*)(window, ldat, CONST byte *))NoOp
#  define FakeWriteString   (void (*)(window, ldat, CONST byte *))NoOp
#  define FakeWriteHWFont (void (*)(window, ldat, CONST hwfont *))NoOp
#  define FakeWriteHWAttr (void (*)(window, dat, dat, ldat, CONST hwattr *))NoOp
# endif /* CONF__MODULES */
#endif  /* CONF_TERM */


#ifdef CONF_WM
byte WMFindBorderWindow(window Window, dat u, dat v, byte Border, hwattr *PtrAttr);

#else
byte FakeFindBorderWindow(window W, dat u, dat v, byte Border, hwattr *PtrAttr) {
    byte Horiz, Vert;
    
    Horiz = u ? u+1 == W->XWidth ? (byte)2 : (byte)1 : (byte)0;
    Vert  = v ? v+1 == W->YWidth ? (byte)2 : (byte)1 : (byte)0;

    if (*PtrAttr)
	*PtrAttr = HWATTR(W->ColBorder, StdBorder[Border][Vert*3+Horiz]);
    
    return v ? POS_ROOT : POS_TITLE;
}
#endif /* CONF_WM */


static row FindRow(window Window, ldat Row) {
    row CurrRow, ElPossib[4];
    byte Index;
    ldat k, ElNumRows[4], ElDist[4];
    
    ElPossib[0] = Window->USE.R.RowOne;
    ElPossib[1] = Window->USE.R.RowSplit;
    ElPossib[2] = Window->USE.R.FirstRow;
    ElPossib[3] = Window->USE.R.LastRow;
    ElNumRows[0] = Window->USE.R.NumRowOne;
    ElNumRows[1] = Window->USE.R.NumRowSplit;
    ElNumRows[2] = (ldat)0;
    ElNumRows[3] = Window->HLogic-(ldat)1;
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
    if (CurrRow && IS_MENUITEM(CurrRow))
	((menuitem)CurrRow)->WCurY = Row;
    return CurrRow;
}

static row FindRowByCode(window Window, udat Code, ldat *NumRow) {
    row Row;
    ldat Num=(ldat)0;
    
    if ((Row=Window->USE.R.FirstRow))
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
    ChangeFieldWindow,
    /* widget */
    &_FnObj,
    DrawSelfWindow,
    (void *)FindWidgetAt,
    (void *)FindGadgetByCode,
    SetXYWindow,
    (void *)SetFillWidget,
    (void *)FocusWidget,
    (void *)TtyKbdFocus,
    (void *)MapWidget,
    (void *)UnMapWidget,
    (void *)MapTopRealWidget,
    (void *)RaiseW,
    (void *)LowerW,
    (void *)OwnWidget,
    (void *)DisOwnWidget,
    (void *)RecursiveDeleteWidget,
    (void *)ExposeWindow2,	/* exported by resize.c */
    (void *)InstallHookWidget,
    (void *)RemoveHookWidget,
    /* window */
    &_FnWidget,
#ifdef CONF_TERM
    TtyWriteAscii,
    TtyWriteString,
    TtyWriteHWFont,
    TtyWriteHWAttr,
#else
    FakeWriteAscii,
    FakeWriteString,
    FakeWriteHWFont,
    FakeWriteHWAttr,
#endif
    RowWriteAscii,		/* exported by resize.c */
    RowWriteAscii,
    RowWriteHWFont,
    (void *)AlwaysFalse,
	
    GotoXYWindow,
    SetTitleWindow,
    SetColTextWindow,
    SetColorsWindow,
    ConfigureWindow,
    Create4MenuWindow,
#ifdef CONF_WM
    WMFindBorderWindow,
#else
    FakeFindBorderWindow,
#endif
    FindRow,
    FindRowByCode,
};




/* screen */

static screen CreateScreen(fn_screen Fn_Screen, dat NameLen, CONST byte *Name,
			    dat BgWidth, dat BgHeight, CONST hwattr *Bg) {
    screen S = (screen)0;
    size_t size;
    
    if ((size=(size_t)BgWidth * BgHeight * sizeof(hwattr))) {
	
	if ((S=(screen)Fn_Screen->Fn_Widget->Create
	     ((fn_widget)Fn_Screen, Builtin_MsgPort, MAXDAT, MAXDAT, 0, SCREENFL_USEBG, 0, 0, Bg[0]))) {

	    if (!(S->Name=NULL, Name) || (S->Name=CloneStrL(Name, NameLen))) {
		if ((S->USE.B.Bg = AllocMem(size))) {

		    Fn_Screen->Fn_Widget->Used++;
		    
		    S->NameLen = NameLen;
		    S->MenuWindow = S->ClickWindow = NULL;
		    S->HookW = NULL;
		    S->FnHookW = NULL;
		    S->USE.B.BgWidth = BgWidth;
		    S->USE.B.BgHeight = BgHeight;
		    CopyMem(Bg, S->USE.B.Bg, size);
		    S->All = (all)0;
		    
		    return S;
		}
		if (S->Name) FreeMem(S->Name);
	    }
	    (Fn_Screen->Fn_Widget->Delete)((widget)S);
	}
    }
    return NULL;
}

static screen CreateSimpleScreen(fn_screen Fn_Screen, dat NameLen, CONST byte *Name, hwattr Bg) {
    return Fn_Screen->Create(Fn_Screen, NameLen, Name, 1, 1, &Bg);
}

static void BgImageScreen(screen Screen, dat BgWidth, dat BgHeight, CONST hwattr *Bg) {
    size_t size;
    
    if (Screen && S_USE(Screen, USEBG) && Bg &&
	(size = (size_t)BgWidth * BgHeight * sizeof(hwattr)) &&
	(Screen->USE.B.Bg = ReAllocMem(Screen->USE.B.Bg, size))) {

	Screen->USE.B.BgWidth = BgWidth;
	Screen->USE.B.BgHeight = BgHeight;
	CopyMem(Bg, Screen->USE.B.Bg, size);
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
    
    if (S_USE(Screen, USEBG) && Screen->USE.B.Bg) {
	FreeMem(Screen->USE.B.Bg);
	Screen->USE.B.Bg = NULL;
    }
    
    (Fn_Widget->Delete)((widget)Screen);
    if (!--Fn_Widget->Used)
	FreeMem(Fn_Widget);
}

static void ChangeFieldScreen(screen S, udat field, uldat CLEARMask, uldat XORMask) {
    if (S) switch (field) {
      default:
	S->Fn->Fn_Widget->ChangeField((widget)S, field, CLEARMask, XORMask);
	break;
    }
}

static void SetXYScreen(screen Screen, dat X, dat Y) {
    if (Screen == All->FirstScreen) {
	Y = Max2(Y, -1);
	Y = Min2(Y, All->DisplayHeight - 1);
	ResizeFirstScreen(Y - Screen->YLimit);
    }
}

static menu FindMenuScreen(screen Screen) {
    if (Screen) {
	if (Screen->MenuWindow && IS_WINDOW(Screen->MenuWindow))
	    /* menu activated from Screen->MenuWindow, return its menu */
	    return Screen->MenuWindow->Menu;

	/* no window activated the menu... either the menu is inactive
	 * or it is activated from the builtin menu */
	    
	if (Screen->FocusW && IS_WINDOW(Screen->FocusW) &&
	    ((window)Screen->FocusW)->Menu != All->CommonMenu) 
	    /* menu inactive... return the focus window's one */
	    return ((window)Screen->FocusW)->Menu;
	    
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

    

static widget FocusScreen(screen tScreen) {
    screen Screen = All->FirstScreen;
    if (tScreen && Screen != tScreen) {
	MoveFirst(Screen, All, tScreen);
	DrawArea2((screen)0, (widget)0, (widget)0,
		 0, Min2(Screen->YLimit, tScreen->YLimit), MAXDAT, MAXDAT, FALSE);
	UpdateCursor();
    }
    return (widget)Screen;
}

static void ActivateMenuScreen(screen Screen, menuitem Item, byte ByMouse) {
    
    if ((All->State & STATE_ANY) != STATE_DEFAULT)
	return;
    
    if (Screen && Screen != All->FirstScreen)
	Act(Focus,Screen)(Screen);

    SetMenuState(Item, ByMouse);
}

static void DeActivateMenuScreen(screen Screen) {
    if (Screen == All->FirstScreen && (All->State & STATE_ANY) == STATE_MENU)
	CloseMenu();
}

static struct s_fn_screen _FnScreen = {
    screen_magic, sizeof(struct s_screen), (uldat)1,
    CreateScreen,
    InsertScreen,
    RemoveScreen,
    DeleteScreen,
    ChangeFieldScreen,
    /* widget */
    &_FnObj,
    DrawSelfScreen,
    (void *)FindWidgetAt,
    (void *)FindGadgetByCode,
    SetXYScreen,
    (void *)SetFillWidget,
    FocusScreen,
    (void *)NoOp, /* KbdFocusWidget */
    (void *)NoOp, /* MapWidget */
    (void *)NoOp, /* UnMapWidget */
    (void *)NoOp, /* MapTopRealWidget */
    (void *)RaiseW,
    (void *)LowerW,
    (void *)OwnWidget,
    (void *)DisOwnWidget,
    (void *)RecursiveDeleteWidget,
    (void *)ExposeWidget2,	/* exported by resize.c */
    (void *)InstallHookWidget,
    (void *)RemoveHookWidget,
    /* screen */
    &_FnWidget,
    FindMenuScreen,
    FindScreen,
    CreateSimpleScreen,
    BgImageScreen,
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
    if (G && !G->Group && !G->G_Prev && !G->G_Next) {
	if ((G->G_Next = Group->FirstG))
	    Group->FirstG->G_Prev = G;
	else
	    Group->LastG = G;
    
	Group->FirstG = G;
	G->Group = Group;
    }
}

static void RemoveGadgetGroup(group Group, gadget G) {
   if (G && G->Group == Group) {
       if (G->G_Prev)
	   G->G_Prev->G_Next = G->G_Next;
       else if (Group->FirstG == G)
	   Group->FirstG = G->G_Next;
    
       if (G->G_Next)
	   G->G_Next->G_Prev = G->G_Prev;
       else if (Group->LastG == G)
	   Group->LastG = G->G_Prev;
    
       G->G_Prev = G->G_Next = (gadget)0;
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
    (void *)NoOp,
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
	Row->Text = NULL;
	Row->ColText = NULL;
    }
    return Row;
}

static void InsertRow(row Row, window Parent, row Prev, row Next) {
    if (!Row->Window && Parent && W_USE(Parent, USEROWS)) {
	InsertGeneric((obj)Row, (obj_parent)&Parent->USE.R.FirstRow, (obj)Prev, (obj)Next, &Parent->HLogic);
	Row->Window = Parent;
	Parent->USE.R.NumRowOne = Parent->USE.R.NumRowSplit = (ldat)0;
    }
}

static void RemoveRow(row Row) {
    if (Row->Window && W_USE(Row->Window, USEROWS)) {
	Row->Window->USE.R.NumRowOne = Row->Window->USE.R.NumRowSplit = (ldat)0;
	RemoveGeneric((obj)Row, (obj_parent)&Row->Window->USE.R.FirstRow, &Row->Window->HLogic);
	Row->Window = (window)0;
    }
}

static void DeleteRow(row Row) {    
    if (Row) {
	fn_obj Fn_Obj = Row->Fn->Fn_Obj;
	window W = Row->Window;
	
	Remove(Row);
	if (Row->Text)
	    FreeMem(Row->Text);
	if (Row->ColText)
	    FreeMem(Row->ColText);
	
	(Fn_Obj->Delete)((obj)Row);
	if (!--Fn_Obj->Used)
	    FreeMem(Fn_Obj);
	
	if (W && W->Parent && (W->Flags & WINDOWFL_MENU))
	    ResizeRelWindow(W, 0, -1);
    }
}

static byte SetTextRow(row Row, ldat Len, CONST byte *Text, byte DefaultCol) {
    if (EnsureLenRow(Row, Len, DefaultCol)) {
	if (Len) {

#if TW_SIZEOFHWFONT != 1
	    hwfont *RowText = Row->Text;
	    ldat i = Len;
	    while (i-- > 0) {
# ifdef CONF__UNICODE
		*RowText++ = Tutf_CP437_to_UTF_16[*Text++];
# else /* !CONF__UNICODE */
		*RowText++ = *Text++;
# endif /* CONF__UNICODE */
	    }
#else /* TW_SIZEOFHWFONT == 1 */
	    CopyMem(Text, Row->Text, Len);
#endif /* TW_SIZEOFHWFONT != 1 */
	    if (!(Row->Flags & ROW_DEFCOL) && !DefaultCol)
		/* will not work correctly if sizeof(hwcol) != 1 */
		WriteMem(Row->ColText, COL(WHITE,BLACK), Len * sizeof(hwcol));
	}
	Row->Len = Len;
	Row->Gap = Row->LenGap = 0;
	return TRUE;
    }
    return FALSE;
}

static byte SetHWFontRow(row Row, ldat Len, CONST hwfont *HWFont, byte DefaultCol) {
    if (EnsureLenRow(Row, Len, DefaultCol)) {
	if (Len) {
	    CopyMem(HWFont, Row->Text, Len * sizeof(hwfont));
	    if (!(Row->Flags & ROW_DEFCOL) && !DefaultCol)
		/* will not work correctly if sizeof(hwcol) != 1 */
		WriteMem(Row->ColText, COL(WHITE,BLACK), Len * sizeof(hwcol));
	}
	Row->Len = Len;
	Row->Gap = Row->LenGap = 0;
	return TRUE;
    }
    return FALSE;
}


/*
 * this is used also for plain rows.
 * be careful to only access fields that even rows have
 */
static void RaiseMenuItem(menuitem M) {
    obj Parent;
    menuitem Next;
    
    if (M && (Parent = (obj)M->Parent)) {
	if (IS_MENU(Parent))
	    Next = ((menu)Parent)->FirstI;
	else if (IS_WINDOW(Parent) && W_USE((window)Parent, USEROWS))
	    Next = (menuitem)((window)Parent)->USE.R.FirstRow;
	else
	    return;
	
	Act(Remove,M)(M);
	Act(Insert,M)(M, Parent, (menuitem)0, Next);
	
	if (IS_MENU(Parent))
	    SyncMenu((menu)Parent);
	else
	    DrawAreaWidget((widget)Parent);
    }
}

/*
 * this is used also for plain rows.
 * be careful to only access fields that even rows have
 */
static void LowerMenuItem(menuitem M) {
    obj Parent;
    menuitem Prev;
    
    if (M && (Parent = (obj)M->Parent)) {
	if (IS_MENU(Parent))
	    Prev = ((menu)Parent)->LastI;
	else if (IS_WINDOW(Parent) && W_USE((window)Parent, USEROWS))
	    Prev = (menuitem)((window)Parent)->USE.R.LastRow;
	else
	    return;
	
	Act(Remove,M)(M);
	Act(Insert,M)(M, Parent, Prev, (menuitem)0);
	
	if (IS_MENU(Parent))
	    SyncMenu((menu)Parent);
	else if (Parent->Parent)
	    DrawAreaWidget((widget)Parent);
    }
}


static struct s_fn_row _FnRow = {
    row_magic, sizeof(struct s_row), (uldat)1,
    CreateRow,
    InsertRow,
    RemoveRow,
    DeleteRow,
    (void *)NoOp,
    /* row */
    &_FnObj,
    SetTextRow,
    SetHWFontRow,
    (void *)RaiseMenuItem,
    (void *)LowerMenuItem
};


byte FindInfo(menu Menu, dat i) {
    row Info;
    
    if (Menu && (Info=Menu->Info) && Info->Len>(udat)i)
	return TRUE;
    return FALSE;
}

/* menuitem */

static menuitem CreateMenuItem(fn_menuitem Fn_MenuItem, obj Parent, window Window, udat Code,
			       byte Flags, dat Left, ldat Len, dat ShortCut, CONST byte *Name) {
    menuitem MenuItem = (menuitem)0;
    hwfont *_Name = NULL;
    
    if (Parent && (IS_MENU(Parent) || (IS_WINDOW(Parent) && W_USE((window)Parent, USEROWS))) &&
	(!Window || IS_WINDOW(Window)) &&
	Name && (_Name=CloneStr2HWFont(Name,Len)) &&
	(MenuItem=(menuitem)Fn_MenuItem->Fn_Row->Create((fn_row)Fn_MenuItem, Code, Flags))) {
	
	Fn_MenuItem->Fn_Row->Used++;
	
	MenuItem->Len=Len;
	MenuItem->Text=_Name;
	MenuItem->Window=Window;
	MenuItem->Left=Left;
	MenuItem->ShortCut=ShortCut;
	MenuItem->WCurY=MAXLDAT;
	
	if (Window)
	    Window->MenuItem = MenuItem;
	
	if (IS_WINDOW(Parent)) {
	    Window = (window)Parent;
	    
	    if ((ldat)Window->XWidth<(Len=Max2((ldat)10, Len+(ldat)2)))
		Window->XWidth = Len;
    
	    if ((ldat)Window->YWidth<(Len=Min2(MAXDAT, Window->HLogic+(ldat)3)))
		Window->YWidth = Len;

	    Act(Insert, MenuItem)(MenuItem, (obj)Window, (menuitem)Window->USE.R.LastRow, NULL);
	} else {
	    Act(Insert, MenuItem)(MenuItem, Parent, ((menu)Parent)->LastI, NULL);
	    SyncMenu((menu)Parent);
	}
	return MenuItem;
    }
    if (_Name)
	FreeMem(_Name);
    return MenuItem;
}

static void InsertMenuItem(menuitem MenuItem, obj Parent, menuitem Prev, menuitem Next) {
    if (!MenuItem->Parent && Parent) {
	if (IS_MENU(Parent)) {
	    InsertGeneric((obj)MenuItem, (obj_parent)&((menu)Parent)->FirstI, (obj)Prev, (obj)Next, (ldat *)0);
	    MenuItem->Parent = Parent;
	} else if (IS_WINDOW(Parent)) {
	    (MenuItem->Fn->Fn_Row->Insert)((row)MenuItem, (window)Parent, (row)Prev, (row)Next);
	}
    }
}

static void RemoveMenuItem(menuitem MenuItem) {
    if (MenuItem->Parent) {
	if (IS_MENU(MenuItem->Parent)) {
	    RemoveGeneric((obj)MenuItem, (obj_parent)&((menu)MenuItem->Parent)->FirstI, (ldat *)0);
	    MenuItem->Parent = (obj)0;
	} else
	    (MenuItem->Fn->Fn_Row->Remove)((row)MenuItem);
    }
}

static void DeleteMenuItem(menuitem MenuItem) {
    if (MenuItem) {
	fn_row Fn_Row = MenuItem->Fn->Fn_Row;
	obj Parent = MenuItem->Parent;
	
	Remove(MenuItem);
	if (IS_MENU(Parent))
	    SyncMenu((menu)Parent);
	
	if (MenuItem->Window)
	    Delete(MenuItem->Window);
	
	(Fn_Row->Delete)((row)MenuItem);
	if (!--Fn_Row->Used)
	    FreeMem(Fn_Row);
    }
}

menuitem Create4MenuMenuItem(fn_menuitem Fn_MenuItem, obj Parent, window Window,
			     udat Code, byte Flags, ldat Len, CONST byte *Name) {
    dat Left, ShortCut;
    
    if (!Parent)
	return (menuitem)0;

    if (IS_MENU(Parent) && ((menu)Parent)->LastI)
	Left=((menu)Parent)->LastI->Left + ((menu)Parent)->LastI->Len;
    else
	Left=(dat)1;
    
    ShortCut=(dat)0;
    while (ShortCut<Len && Name[ShortCut]==' ')
	ShortCut++;
    
    if (Window)
	Window->Left=Left;
    
    return (Fn_MenuItem->Create)(Fn_MenuItem, Parent, Window, Code, Flags, Left, Len, ShortCut, Name);
}

/* this returns non-zero for compatibility */
static uldat Create4MenuCommonMenuItem(fn_menuitem Fn_MenuItem, menu Menu) {
    if (Menu) {
	Menu->CommonItems = TRUE;
	SyncMenu(Menu);
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
    (void *)NoOp,
    &_FnObj,
    SetTextRow,
    (void *)SetHWFontRow,
    RaiseMenuItem,
    LowerMenuItem,
    &_FnRow,
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
	Menu->FirstI=Menu->LastI=Menu->SelectI=(menuitem)0;
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
	 * as a window without menu cannot be displayed.
	 *
	 * optimization: if we are going to UnMap() a lot of windows,
	 * we set QueuedDrawArea2FullScreen = TRUE, so that the UnMap()
	 * calls do not have to redraw every time.
	 */
	if (!QueuedDrawArea2FullScreen) {
	    for (W = MsgPort->FirstW; W && count; W = WNext) {
		WNext = W->O_Next;
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
	    WNext = W->O_Next;
	    if (IS_WINDOW(W) && ((window)W)->Menu == Menu) {
		if (W->Parent && IS_SCREEN(W->Parent)) {
		    Act(UnMap,W)(W);
		    ((window)W)->Menu = (menu)0;
		}
	    }
	}
	Remove(Menu);
	DeleteList(Menu->FirstI);
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
	if ((!Text || (Row->Text=CloneStr2HWFont(Text,Len))) &&
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
	for (Item = Menu->FirstI; Item; Item = Item->Next) {
	    if (i >= Item->Left && i < Item->Left + Item->Len)
		break;
	}
	
	if (!Item && Menu->CommonItems && All->CommonMenu) {

	    Item = Menu->LastI;
	    
	    if (!Item || i >= Item->Left + Item->Len) {
		/* search in All->CommonMenu */
		if (Item)
		    i -= Item->Left + Item->Len;
		for (Item = All->CommonMenu->FirstI; Item; Item = Item->Next) {
		    if (i >= Item->Left && i < Item->Left + Item->Len)
			break;
		}
	    } else
		Item = (menuitem)0;
	}
    }
    return Item;
}

static menuitem GetSelectedItem(menu Menu) {
    if (Menu) {
	if (Menu->SelectI)
	    return Menu->SelectI;
	if (Menu->CommonItems && All->CommonMenu)
	    return All->CommonMenu->SelectI;
    }
    return (menuitem)0;
}

static menuitem RecursiveGetSelectedItem(menu Menu, dat *depth) {
    menuitem _I = Act(GetSelectedItem,Menu)(Menu), I = (menuitem)0;
    window W = (window)0, FW = (window)All->FirstScreen->FocusW;
    dat d = -1;
    
    while (_I && IS_MENUITEM(_I)) {
	I = _I;
	d++;
	if (W == FW)
	    break;
	if ((W = I->Window) && W->Parent)
	    _I = (menuitem)Act(FindRow,W)(W, W->CurY);
	else
	    break;
    }
    if (depth) *depth = d > 0 ? d : 0;
    return I;
}

static void SetSelectedItem(menu Menu, menuitem Item) {
    menuitem S;
    
    if (Menu) {
	if (Item) {
	    if (Item->Parent == (obj)Menu) {
		S = Menu->SelectI;
		Menu->SelectI = Item;
		if (Menu->CommonItems && All->CommonMenu)
		    All->CommonMenu->SelectI = (menuitem)0;
	    } else if (Menu->CommonItems && Item->Parent == (obj)All->CommonMenu) {
		Menu->SelectI = (menuitem)0;
		All->CommonMenu->SelectI = Item;
	    }
	    /* else Item is not a meaningful one! */
	} else {
	    Menu->SelectI = Item;
	    if (Menu->CommonItems && All->CommonMenu)
		All->CommonMenu->SelectI = Item;
	}

	Act(DrawMenu,All->FirstScreen)(All->FirstScreen, 0, MAXDAT);
    }
}

static struct s_fn_menu _FnMenu = {
    menu_magic, sizeof(struct s_menu), (uldat)1,
    CreateMenu,
    InsertMenu,
    RemoveMenu,
    DeleteMenu,
    (void *)NoOp,
    /* menu */
    &_FnObj,
    SetInfoMenu,
    FindItem,
    GetSelectedItem,
    RecursiveGetSelectedItem,
    SetSelectedItem
};


/* msg */

#define Delta ((size_t)&(((msg)0)->Event))

static msg CreateMsg(fn_msg Fn_Msg, udat Type, udat EventLen) {
    msg Msg;
    
    switch (Type) {
      case MSG_MAP:		EventLen += sizeof(event_map); break;
      case MSG_DISPLAY:		EventLen += sizeof(event_display); break;
      case MSG_KEY:
      case MSG_WIDGET_KEY:	EventLen += sizeof(event_keyboard); break;
      case MSG_WIDGET_MOUSE:
      case MSG_MOUSE:		EventLen += sizeof(event_mouse); break;
      case MSG_WIDGET_CHANGE:	EventLen += sizeof(event_widget); break;
      case MSG_WIDGET_GADGET:	EventLen += sizeof(event_gadget); break;
      case MSG_MENU_ROW:	EventLen += sizeof(event_menu); break;
      case MSG_SELECTION:	EventLen += sizeof(event_selection); break;
      case MSG_SELECTIONNOTIFY:	EventLen += sizeof(event_selectionnotify) - sizeof(uldat); break;
      case MSG_SELECTIONREQUEST:EventLen += sizeof(event_selectionrequest); break;
      case MSG_CONTROL:
      case MSG_USER_CONTROL:	EventLen += sizeof(event_control) - sizeof(uldat); break;
      case MSG_USER_CLIENTMSG:	EventLen += sizeof(event_clientmsg) - sizeof(uldat); break;

      case MSG_SELECTIONCLEAR:	EventLen += sizeof(event_common); break;
      default:
	printk("twin: CreateMsg(): internal error: unknown Msg->Type 0x%04x(%d)\n", (int)Type, (int)Type);
	return (msg)0;
    }
    
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
    (void *)NoOp,
    /* msg */
    &_FnObj
};

/* msgport */

static msgport CreateMsgPort(fn_msgport Fn_MsgPort, byte NameLen, CONST byte *Name,
			      tany PauseSec, tany PauseFraction,
			      byte WakeUp, void (*Handler)(msgport)) {
    msgport MsgPort = (msgport)0;
    byte *_Name;
    
    if (Handler && (!Name || (_Name = CloneStrL(Name, NameLen))) &&
	(MsgPort=(msgport)Fn_MsgPort->Fn_Obj->Create((fn_obj)Fn_MsgPort))) {
	
	Fn_MsgPort->Fn_Obj->Used++;
	
	MsgPort->WakeUp = WakeUp;
	MsgPort->NameLen = NameLen;
	MsgPort->Name = _Name;
	MsgPort->Handler = Handler;
	MsgPort->ShutDownHook=(void (*)(msgport))0;
	MsgPort->PauseDuration.Seconds = PauseSec;
	MsgPort->PauseDuration.Fraction = PauseFraction;
	(void)SumTime(&MsgPort->CallTime, &All->Now, &MsgPort->PauseDuration);
	MsgPort->RemoteData.Fd = NOFD;
	MsgPort->RemoteData.ChildPid = NOPID;
	MsgPort->RemoteData.FdSlot = NOSLOT;
	MsgPort->FirstMsg=MsgPort->LastMsg = (msg)0;
	MsgPort->FirstMenu=MsgPort->LastMenu = (menu)0;
	MsgPort->FirstW=MsgPort->LastW = (widget)0;
	MsgPort->FirstGroup=MsgPort->LastGroup = (group)0;
	MsgPort->FirstMutex=MsgPort->LastMutex = (mutex)0;
	MsgPort->CountE=MsgPort->SizeE = (uldat)0;
	MsgPort->Es=(extension *)0;
	MsgPort->AttachHW = (display_hw)0;
	InsertMiddle(MsgPort, MsgPort, All,
		     WakeUp ? (msgport)0 : All->LastMsgPort,
		     WakeUp ? All->FirstMsgPort : (msgport)0);
	SortMsgPortByCallTime(MsgPort);
    } else if (NameLen && _Name)
	FreeMem(_Name);
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
    extension *Es;
    
    if (MsgPort) {
	fn_obj Fn_Obj = MsgPort->Fn->Fn_Obj;
	
	/*
	 * optimization: if we are going to UnMap() a lot of windows,
	 * we set QueuedDrawArea2FullScreen = TRUE, so that the UnMap()
	 * calls do not have to redraw every time.
	 */
	for (W = MsgPort->FirstW; W && count; W = W->O_Next) {
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
	
	for (count = MsgPort->CountE, Es = MsgPort->Es; count; count--, Es++)
	    Act(UnuseExtension,MsgPort)(MsgPort, *Es);
	    
	Remove(MsgPort);
	if (MsgPort->Name)
	    FreeMem(MsgPort->Name);
	
	(Fn_Obj->Delete)((obj)MsgPort);
	if (!--Fn_Obj->Used)
	    FreeMem(Fn_Obj);
    }
}

static byte GrowExtensionMsgPort(msgport M) {
    uldat oldsize, size;
    extension *newEs;
    
    oldsize = M->SizeE;

    size = oldsize < 2 ? 4 : oldsize + (oldsize>>1);
    if (size > MAXID)
	size = MAXID;
    
    if (!(newEs = (extension *)ReAllocMem(M->Es, size*sizeof(extension))))
	return FALSE;
    
    M->Es = newEs;
    return TRUE;
}

static void UseExtensionMsgPort(msgport M, extension E) {
    uldat count;
    extension *Es;
    
    if (M && E) {
	
	for (count = M->CountE, Es = M->Es; count; count--, Es++) {
	    if (*Es == E)
		/* already in use */
		return;
	}
	
	if (M->CountE >= M->SizeE && !GrowExtensionMsgPort(M))
	    return;

	M->Es[M->CountE++] = E;
	E->Used++;
    }
}

static void UnuseExtensionMsgPort(msgport M, extension E) {
    uldat count;
    extension *Es;
    
    if (M && E) for (count = M->CountE, Es = M->Es; count; count--, Es++) {
	if (*Es == E) {
	    if (count > 1)
		*Es = Es[count - 1];
	    M->CountE--;
	    
	    if (!--E->Used)
		Delete(E);
	}
    }
}

static struct s_fn_msgport _FnMsgPort = {
    msgport_magic, sizeof(struct s_msgport), (uldat)1,
    CreateMsgPort,
    InsertMsgPort,
    RemoveMsgPort,
    DeleteMsgPort,
    (void *)NoOp,
    /* msgport */
    &_FnObj,
    UseExtensionMsgPort,
    UnuseExtensionMsgPort
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

	if ((Mutex->O_Prev = Parent->LastMutex))
	    Parent->LastMutex->O_Next = Mutex;
	else
	    Parent->FirstMutex = Mutex;	
    
	Mutex->O_Next = (mutex)0;
	Parent->LastMutex = Mutex;
	
	Mutex->Owner = Parent;
    }
}

static void DisOwnMutex(mutex Mutex) {
    msgport Parent;
    if ((Parent = Mutex->Owner)) {
	if (Mutex->O_Prev)
	    Mutex->O_Prev->O_Next = Mutex->O_Next;
	else if (Parent->FirstMutex == Mutex)
	    Parent->FirstMutex = Mutex->O_Next;
    
	if (Mutex->O_Next)
	    Mutex->O_Next->O_Prev = Mutex->O_Prev;
	else if (Parent->LastMutex == Mutex)
	    Parent->LastMutex = Mutex->O_Prev;
    
	Mutex->O_Prev = Mutex->O_Next = (mutex)0;
	
	Mutex->Owner = (msgport)0;
    }
}

static struct s_fn_mutex _FnMutex = {
    mutex_magic, sizeof(struct s_mutex), (uldat)1,
    CreateMutex,
    InsertMutex,
    RemoveMutex,
    DeleteMutex,
    (void *)NoOp,
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
    (void *)NoOp,
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


/* extension */

static void DeleteExtension(extension E) {
    fn_module Fn_Module;
    msgport M;
    extension *Es;
    uldat i;
    
    /* search for MsgPorts using this extension */
    for (M = All->FirstMsgPort; M; M = M->Next) {
	for (i = M->CountE, Es = M->Es; i; i--, Es++) {
	    if (E == *Es) {
		Act(UnuseExtension,M)(M, E);
		break;
	    }
	}
    }
    
    /* used if the extension is NOT DlOpen()ed */
    if (E->Quit)
	E->Quit(E);
    
    Fn_Module = E->Fn->Fn_Module;
    (Fn_Module->Delete)((module)E);
    if (!--Fn_Module->Used)
	FreeMem(Fn_Module);
}

static struct s_fn_extension _FnExtension = {
    extension_magic, sizeof(struct s_extension), (uldat)1,
    (void *)CreateModule,
    (void *)InsertModule,
    (void *)RemoveModule,
    DeleteExtension,
    (void *)NoOp,
    /* module */
    &_FnObj,
#ifdef CONF__MODULES
    (void *)DlOpen,
    (void *)DlClose,
#else
    (byte (*)(extension))AlwaysFalse,
    (void (*)(extension))NoOp,
#endif
    &_FnModule,
#ifdef CONF_EXT
    QueryExtension, /* exported by extensions/ext_query.c */
#else
    (void *)AlwaysNull,
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
	    RunNoHW(FALSE);
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
    (void *)NoOp,
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
    &_FnExtension,
    &_FnDisplayHW
};

