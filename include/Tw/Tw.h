/*
 *  Tw.h  --  main include for all libTw data types, functions and macros
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TW_H
#define _TW_H

#ifdef __cplusplus
 extern "C" {
#endif
#if 0
 }
#endif

#include <Tw/Tw_defs.h>

#ifndef _TWIN_H
#define _TWIN_H

/* a cutdown version of twin.h */
#include <Tw/Twtypes.h>
#include <Tw/missing.h>

/* types for time intervals */
typedef struct s_timevalue  {
    time_t Seconds;
    frac_t Fraction;
} timevalue;

#endif /* _TWIN_H */

typedef tobj tgroup;     
typedef tobj twidget;
typedef tobj tgadget;
typedef tobj twindow;
typedef tobj tscreen;
typedef tobj trow;
typedef tobj tmenuitem;
typedef tobj tmenu;
typedef tobj tmsgport;
typedef tobj tmutex;
typedef tobj tall;




/* types for messages from server */
typedef struct s_tevent_common *tevent_common;
TW_TYPE_ATTR_PACKED struct s_tevent_common {
    twidget W;
    udat Code, pad;
};

typedef struct s_tevent_display *tevent_display;
TW_TYPE_ATTR_PACKED struct s_tevent_display {
    twidget W; /* not used here */
    udat Code, Len;
    dat X, Y;
    byte Data[sizeof(uldat)]; /* [len] bytes actually */
};
#define TW_SIZEOF_TEVENT_DISPLAY (sizeof(struct s_tevent_display) - sizeof(uldat))

/* Code */
#define TW_DPY_DrawHWAttr	((udat)0)
#define TW_DPY_FlushHW		((udat)1)
#define TW_DPY_KeyboardEvent	((udat)2)
#define TW_DPY_MouseEvent	((udat)3)
#define TW_DPY_SetCursorType	((udat)4)
#define TW_DPY_MoveToXY		((udat)5)
#define TW_DPY_Resize		((udat)6)

#define TW_DPY_SelectionExport	((udat)8)
#define TW_DPY_DragArea		((udat)9)
#define TW_DPY_Beep		((udat)10)
#define TW_DPY_Configure	((udat)11)
#define TW_DPY_SetPalette	((udat)12)
#define TW_DPY_ResetPalette	((udat)13)
#define TW_DPY_Helper		((udat)14)
#define TW_DPY_RedrawVideo	((udat)15)
#define TW_DPY_Quit		((udat)16)

typedef struct s_tevent_keyboard *tevent_keyboard;
TW_TYPE_ATTR_PACKED struct s_tevent_keyboard {
    twidget W;
    udat Code, ShiftFlags, SeqLen;
    byte pad, AsciiSeq[1];  /* actually (SeqLen+1) bytes, including final \0 */
};

typedef struct s_tevent_mouse *tevent_mouse;
TW_TYPE_ATTR_PACKED struct s_tevent_mouse {
    twidget W;
    udat Code, ShiftFlags;
    dat X, Y; /* these coords are relative to the widget top-left corner */
};

typedef struct s_tevent_widget *tevent_widget;
TW_TYPE_ATTR_PACKED struct s_tevent_widget {
    twidget W;
    udat Code, Flags;
    dat XWidth, YWidth;
    dat X, Y;
};
/* some TW_MSG_WIDGET_CHANGE codes */
#define TW_MSG_WIDGET_RESIZE	0
#define TW_MSG_WIDGET_EXPOSE	1

/* some TW_MSG_WIDGET_CHANGE flags */
#define TW_MSG_WIDGETFL_SHADED	1

typedef struct s_tevent_gadget *tevent_gadget;
TW_TYPE_ATTR_PACKED struct s_tevent_gadget {
    twidget W;
    udat Code, Flags; /* the Flags of the gadget that was (de)activated */
};

typedef struct s_tevent_menu *tevent_menu;
TW_TYPE_ATTR_PACKED struct s_tevent_menu {
    twindow W;
    udat Code, pad;
    tmenu Menu;
};

typedef struct s_tevent_selection *tevent_selection;
TW_TYPE_ATTR_PACKED struct s_tevent_selection {
    twidget W;
    udat Code, pad; /* unused */
    dat X, Y; /* these coords are absolute, to allow cross-window cut-n-paste */
};

#define TW_MAX_MIMELEN 64

typedef struct s_tevent_selectionnotify *tevent_selectionnotify;
TW_TYPE_ATTR_PACKED struct s_tevent_selectionnotify {
    twidget W;
    udat Code, pad; /* unused */
    uldat ReqPrivate;
    uldat Magic;
    byte MIME[TW_MAX_MIMELEN];
    uldat Len;
    byte Data[sizeof(uldat)]; /* Data[] is Len bytes actually */
};
/*SelectionNotify Magic*/
#define TW_SEL_TEXTMAGIC	((uldat)0x54657874)
#define TW_SEL_HWFONTMAGIC	((uldat)0x4877666E) /* it's unicode */
#define TW_SEL_FILEMAGIC	((uldat)0x46696c65)
#define TW_SEL_URLMAGIC		((uldat)0xAB1691BA)
#define TW_SEL_DATAMAGIC	((uldat)0xDA1AA1AD) /* check MIME if you get this */
#define TW_SEL_IDMAGIC		((uldat)0x49644964)

typedef struct s_tevent_selectionrequest *tevent_selectionrequest;
TW_TYPE_ATTR_PACKED struct s_tevent_selectionrequest {
    twidget W;
    udat Code, pad; /* unused */
    tmsgport Requestor;
    uldat ReqPrivate;
};

typedef struct s_tevent_control *tevent_control;
TW_TYPE_ATTR_PACKED struct s_tevent_control {
    twidget W;
    udat Code, Len;
    dat X, Y;
    byte Data[sizeof(uldat)]; /* [Len+1] bytes actually, Data[Len] == '\0' */
};
#define TW_SIZEOF_TEVENT_CONTROL (sizeof(struct s_tevent_control) - sizeof(uldat))

/* some TW_MSG_CONTROL codes */
#define TW_MSG_CONTROL_QUIT		((udat)0)
#define TW_MSG_CONTROL_RESTART		((udat)1)
#define TW_MSG_CONTROL_OPEN		((udat)2)
#define TW_MSG_CONTROL_DRAGNDROP	((udat)3)

typedef struct s_tevent_clientmsg *tevent_clientmsg;
TW_TYPE_ATTR_PACKED struct s_tevent_clientmsg {
    twidget W;
    udat Code, Format;
    uldat Len;
    union {
	byte b[sizeof(uldat)];
	udat d[sizeof(uldat)/sizeof(udat)];
	uldat l[1];
    } Data; /* [Len] bytes actually */
};
#define TW_SIZEOF_TEVENT_CLIENTMSG (sizeof(struct s_tevent_clientmsg) - sizeof(uldat))

typedef union s_tevent_any *tevent_any;
TW_TYPE_ATTR_PACKED union s_tevent_any {
    struct s_tevent_common    EventCommon;
    struct s_tevent_display   EventDisplay;
    struct s_tevent_keyboard  EventKeyboard;
    struct s_tevent_mouse     EventMouse;
    struct s_tevent_widget    EventWidget;
    struct s_tevent_gadget    EventGadget;
    struct s_tevent_menu      EventMenu;
    struct s_tevent_selection EventSelection;
    struct s_tevent_selectionnotify EventSelectionNotify;
    struct s_tevent_selectionrequest EventSelectionRequest;
    struct s_tevent_control   EventControl;
    struct s_tevent_clientmsg EventClientMsg;
};

typedef struct s_tmsg *tmsg;
TW_TYPE_ATTR_PACKED struct s_tmsg {
    uldat Len;		/* length of this struct, including `Len' field */
    uldat Magic;	/* == msg_magic (if user-created) or == MSG_MAGIC (if received) */
    uldat Type;		/* only (udat) part is significative */
    union s_tevent_any Event;
};
/* Msg Type : */
#define TW_MSG_DISPLAY		((udat)0x0FFF)

#define TW_MSG_SYSTEM_FIRST	((udat)0x1000)
#define TW_MSG_WIDGET_KEY	((udat)0x1000)
#define TW_MSG_WIDGET_MOUSE	((udat)0x1001)
#define TW_MSG_WIDGET_CHANGE	((udat)0x1002)
#define TW_MSG_WIDGET_GADGET	((udat)0x1003)
#define TW_MSG_MENU_ROW		((udat)0x1004)
#define TW_MSG_SELECTION	((udat)0x1005)
#define TW_MSG_SELECTIONNOTIFY	((udat)0x1006)
#define TW_MSG_SELECTIONREQUEST	((udat)0x1007)
#define TW_MSG_SELECTIONCLEAR	((udat)0x1008)

#define TW_MSG_USER_FIRST	((udat)0x2000)
#define TW_MSG_USER_CONTROL	((udat)0x2000)
#define TW_MSG_USER_CLIENTMSG	((udat)0x2100)



typedef struct s_tw_d *tdisplay;




typedef void (*tfn_listener)(tevent_any, void *);
typedef void (*tfn_default_listener)(tmsg, void *);

typedef struct s_tlistener *tlistener;


     

byte Tw_CheckMagic(TW_CONST byte id[]);

void Tw_ConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *));
		     
extern void *(*Tw_AllocMem)(size_t);
extern void *(*Tw_ReAllocMem)(void *, size_t);
extern void  (*Tw_FreeMem)(void *);
extern void *Tw_CloneMem(TW_CONST void *, size_t);
extern byte *Tw_CloneStr(TW_CONST byte *);
extern hwfont *Tw_CloneStr2HWFont(TW_CONST byte *, size_t);



#define Tw_LenStr(S) strlen(S)
#define Tw_CmpStr(S1, S2) strcmp(S1, S2)
#define Tw_CopyStr(From,To) strcpy(To, From)

#define Tw_CopyMem(From, To, Size)	memcpy(To, From, Size)
#define Tw_MoveMem(From, To, Size)	memmove(To, From, Size)
#define Tw_WriteMem(Mem, Char, Size)	memset(Mem, Char, Size)
#define Tw_CmpMem(m1, m2, Size)		memcmp(m1, m2, Size)


#define Tw_DropPrivileges() (setegid(getgid()), seteuid(getuid()))
#define Tw_GetPrivileges() seteuid(0)








uldat Tw_FindFunction(tdisplay TwD, byte Len, TW_CONST byte *Name, byte ProtoLen, TW_CONST byte *Proto);
byte  Tw_FindFunctions(tdisplay TwD, void *Function, ...);

byte Tw_ServerSizeof(tdisplay TwD, byte type);

byte Tw_EnableGzip(tdisplay TwD);
byte Tw_DisableGzip(tdisplay TwD);

void Tw_NeedResizeDisplay(tdisplay TwD);

void Tw_AttachHW(tdisplay TwD, uldat len, TW_CONST byte *name, byte flags);
#define TW_ATTACH_HW_REDIRECT 1
#define TW_ATTACH_HW_EXCLUSIVE 2
TW_CONST byte *Tw_AttachGetReply(tdisplay TwD, uldat *len);
void Tw_AttachConfirm(tdisplay TwD);

byte Tw_DetachHW(tdisplay TwD, uldat len, TW_CONST byte *name);

void Tw_SetFontTranslation(tdisplay TwD, TW_CONST byte trans[0x80]);
void Tw_SetUniFontTranslation(tdisplay TwD, TW_CONST hwfont trans[0x80]);

void	Tw_DeleteObj(tdisplay TwD, tobj Obj);

twidget Tw_CreateWidget(tdisplay TwD, dat XWidth, dat YWidth, uldat Attrib, uldat Flags,
			dat Left, dat Up, hwattr Bg);
#define Tw_DeleteWidget	Tw_DeleteObj
void	Tw_RecursiveDeleteWidget(tdisplay TwD, twidget Widget);
void	Tw_MapWidget(tdisplay TwD, twidget Widget, twidget Parent);
void	Tw_UnMapWidget(tdisplay TwD, twidget Widget);
void	Tw_SetXYWidget(tdisplay TwD, twidget Widget, dat X, dat Y);
void	Tw_ResizeWidget(tdisplay TwD, twidget Widget, dat XWidth, dat YWidth);
tmsgport Tw_GetOwnerWidget(tdisplay TwD, twidget Widget);
twidget Tw_FindWidgetAtWidget(tdisplay TwD, twidget Parent, dat i, dat j);

void    Tw_FocusSubWidget(tdisplay TwD, twidget W);
void    Tw_ExposeWidget(tdisplay TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, TW_CONST byte *Text, TW_CONST hwfont *Font, TW_CONST hwattr *Attr);
void    Tw_ExposeWidget2(tdisplay TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST byte *Text, TW_CONST hwfont *Font, TW_CONST hwattr *Attr);

void	Tw_ExposeTextWidget(tdisplay TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST byte *Text);
void	Tw_ExposeHWFontWidget(tdisplay TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST hwfont *Font);
void	Tw_ExposeHWAttrWidget(tdisplay TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST hwattr *Attr);

void	Tw_LowerWidget(tdisplay TwD, twidget W);
void	Tw_RaiseWidget(tdisplay TwD, twidget W);
void	Tw_RestackChildrenWidget(tdisplay TwD, twidget W, uldat N, TW_CONST twidget *arrayW);
#define Tw_RestackWidgets		Tw_RestackChildrenWidget
void	Tw_CirculateChildrenWidget(tdisplay TwD, twidget W, byte up_or_down);
#define Tw_CirculateWidgets		Tw_CirculateChildrenWidget
#define TW_CIRCULATE_RAISE_LAST 0
#define TW_CIRCULATE_LOWER_FIRST 1


void	Tw_LowerRow(tdisplay TwD, trow R);
void	Tw_RaiseRow(tdisplay TwD, trow R);
void	Tw_RestackChildrenRow(tdisplay TwD, tobj O, uldat N, TW_CONST trow *arrayR);
#define Tw_RestackRows			Tw_RestackChildrenRow
void	Tw_CirculateChildrenRow(tdisplay TwD, tobj Parent, byte up_or_down);
#define Tw_CirculateRows		Tw_CirculateChildrenRow

tgroup Tw_CreateGroup(tdisplay TwD);
void   Tw_InsertGadgetGroup(tdisplay TwD, tgroup, tgadget);
void   Tw_RemoveGadgetGroup(tdisplay TwD, tgroup, tgadget);
tgadget Tw_GetSelectedGadgetGroup(tdisplay TwD, tgroup);
void    Tw_SetSelectedGadgetGroup(tdisplay TwD, tgroup, tgadget);

/*
 * maximum allowed Code for Gadgets, Rows and MenuRows is (TW_COD_RESERVED - 1) i.e. 0xF7FF
 */
#define TW_COD_RESERVED 0xF800

tgadget Tw_CreateGadget
	 (tdisplay TwD, twidget Parent, dat XWidth, dat YWidth,
	  TW_CONST byte *TextNormal, uldat Attrib, uldat Flags, udat Code,
	  hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
	  dat Left, dat Up);
#define Tw_DeleteGadget			Tw_DeleteObj
#define Tw_RecursiveDeleteGadget	Tw_RecursiveDeleteWidget
#define Tw_MapGadget			Tw_MapWidget
#define Tw_UnMapGadget			Tw_UnMapWidget
#define Tw_SetXYGadget			Tw_SetXYWidget
#define Tw_ResizeGadget			Tw_ResizeWidget
#define Tw_GetOwnerGadget		Tw_GetOwnerWidget
#define Tw_ExposeTextGadget		Tw_ExposeTextWidget
#define Tw_ExposeHWFontGadget		Tw_ExposeHWFontWidget
#define Tw_ExposeHWAttrGadget		Tw_ExposeHWAttrWidget

#define Tw_LowerGadget			Tw_LowerWidget
#define Tw_RaiseGadget			Tw_RaiseWidget
#define Tw_RestackChildrenGadget	Tw_RestackChildrenWidget
#define Tw_RestackGadgets		Tw_RestackWidgets
#define Tw_CirculateChildrenGadget	Tw_CirculateChildrenWidget
#define Tw_CirculateGadgets		Tw_CirculateWidgets


tgadget Tw_CreateButtonGadget(tdisplay TwD, twidget Parent, dat XWidth, dat YWidth, TW_CONST byte *Text,
			      uldat Flags, udat Code, hwcol BgCol, hwcol Col, hwcol ColDisabled,
			      dat Left, dat Up);
void	Tw_SetPressedGadget(tdisplay TwD, tgadget Gadget, byte on);
byte    Tw_IsPressedGadget( tdisplay TwD, tgadget Gadget);
void	Tw_SetToggleGadget( tdisplay TwD, tgadget Gadget, byte on);
byte    Tw_IsToggleGadget(  tdisplay TwD, tgadget Gadget);
void    Tw_WriteTextGadget( tdisplay TwD, tgadget Gadget,              dat XWidth, dat YWidth, TW_CONST byte *Text, dat Left, dat Up);
void    Tw_WriteTextsGadget(tdisplay TwD, tgadget Gadget, byte bitmap, dat XWidth, dat YWidth, TW_CONST byte *Text, dat Left, dat Up);
void    Tw_SetTextGadget(   tdisplay TwD, tgadget Gadget,              dat XWidth, dat YWidth, TW_CONST byte *Text, dat Left, dat Up);
void    Tw_SetTextsGadget(  tdisplay TwD, tgadget Gadget, byte bitmap, dat XWidth, dat YWidth, TW_CONST byte *Text, dat Left, dat Up);

void    Tw_WriteHWFontGadget( tdisplay TwD, tgadget Gadget,              dat XWidth, dat YWidth, TW_CONST hwfont * HWFont, dat Left, dat Up);
void    Tw_WriteHWFontsGadget(tdisplay TwD, tgadget Gadget, byte bitmap, dat XWidth, dat YWidth, TW_CONST hwfont * HWFont, dat Left, dat Up);
void    Tw_SetHWFontGadget(   tdisplay TwD, tgadget Gadget,              dat XWidth, dat YWidth, TW_CONST hwfont * HWFont, dat Left, dat Up);
void    Tw_SetHWFontsGadget(  tdisplay TwD, tgadget Gadget, byte bitmap, dat XWidth, dat YWidth, TW_CONST hwfont * HWFont, dat Left, dat Up);

void	Tw_Create4MenuRow(tdisplay TwD, twindow Window, udat Code, byte Flags, uldat Len, TW_CONST byte *Text);
#define Tw_Row4Menu Tw_Create4MenuRow

twindow Tw_CreateWindow(tdisplay TwD, dat NameLen, TW_CONST byte *Name, TW_CONST hwcol *ColName, tmenu Menu,
		       hwcol ColText, uldat cursorType, uldat Attrib, uldat Flags,
		       dat XWidth, dat YWidth, dat ScrollBackLines);
#define Tw_DeleteWindow			Tw_DeleteObj
#define Tw_RecursiveDeleteWindow	Tw_RecursiveDeleteWidget
#define Tw_MapWindow			Tw_MapWidget
#define Tw_UnMapWindow			Tw_UnMapWidget
#define Tw_SetXYWindow			Tw_SetXYWidget
#define Tw_ResizeWindow			Tw_ResizeWidget
#define Tw_GetOwnerWindow		Tw_GetOwnerWidget
#define Tw_ExposeTextWindow		Tw_ExposeTextWidget
#define Tw_ExposeHWFontWindow		Tw_ExposeHWFontWidget
#define Tw_ExposeHWAttrWindow		Tw_ExposeHWAttrWidget

#define Tw_LowerWindow			Tw_LowerWidget
#define Tw_RaiseWindow			Tw_RaiseWidget
#define Tw_RestackChildrenWindow	Tw_RestackChildrenGadget
#define Tw_RestackWindows		Tw_RestackWidgets
#define Tw_CirculateChildrenWindow	Tw_CirculateChildrenWidget
#define Tw_CirculateWindows		Tw_CirculateWidgets

twindow Tw_Create4MenuWindow(tdisplay TwD, tmenu Menu);
#define Tw_Win4Menu			Tw_Create4MenuWindow
void	Tw_WriteAsciiWindow(tdisplay TwD, twindow Window, ldat Len, TW_CONST byte *Ascii);
void	Tw_WriteStringWindow(tdisplay TwD, twindow Window, ldat Len, TW_CONST byte *String);
void	Tw_WriteHWFontWindow(tdisplay TwD, twindow Window, ldat Len, TW_CONST hwfont *HWFont);
void    Tw_WriteHWAttrWindow(tdisplay TwD, twindow Window, dat x, dat y, ldat Len, TW_CONST hwattr *Attr);
/* backward compatibility. will be removed */
#define Tw_WriteRowWindow		Tw_WriteAsciiWindow
/* backward compatibility. will be removed */
#define TwWriteRowWindow		TwWriteAsciiWindow

void	Tw_SetColTextWindow(tdisplay TwD, twindow Window, hwcol ColText);
void	Tw_SetColorsWindow(tdisplay TwD, twindow Window, udat Bitmap, hwcol ColGadgets, hwcol ColArrows, hwcol ColBars, hwcol ColTabs,
			  hwcol ColBorder, hwcol ColText, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled);
void	Tw_ConfigureWindow(tdisplay TwD, twindow Window, byte Bitmap, dat Left, dat Up, dat MinXWidth, dat MinYWidth, dat MaxXWidth, dat MaxYWidth);
void	Tw_GotoXYWindow(tdisplay TwD, twindow Window, ldat X, ldat Y);
trow	Tw_FindRowByCodeWindow(tdisplay TwD, twindow Window, dat Code);


/* you cannot Create/Delete screens from a client */
#define Tw_LowerScreen			Tw_LowerWidget
#define Tw_RaiseScreen			Tw_RaiseWidget
#define Tw_RestackChildrenScreen		Tw_RestackChildrenWidget
#define Tw_RestackScreens		Tw_RestackWidgets
#define Tw_CirculateChildrenScreen	Tw_CirculateChildrenWidget
#define Tw_CirculateScreens		Tw_CirculateWidgets

void	Tw_BgImageScreen(tdisplay TwD, tscreen Screen, dat BgWidth, dat BgHeight, TW_CONST hwattr *BgImage);


tmenuitem Tw_Create4MenuMenuItem(tdisplay TwD, tobj Parent, twindow Window, byte Flags, dat Len, TW_CONST byte *Name);
#define   Tw_Item4Menu Tw_Create4MenuMenuItem
uldat     Tw_Create4MenuCommonMenuItem(tdisplay TwD, tmenu Menu);
#define   Tw_Item4MenuCommon Tw_Create4MenuCommonMenuItem
#define   Tw_DeleteMenuItem Tw_DeleteObj

#define	Tw_LowerMenuItem		Tw_LowerRow
#define Tw_RaiseMenuItem		Tw_RaiseRow
#define Tw_RestackChildrenMenuItem	Tw_RestackChildrenRow
#define Tw_RestackMenuItems		Tw_RestackRows
#define Tw_CirculateChildrenMenuItem	Tw_CirculateChildrenRow
#define Tw_CirculateMenuItems		Tw_CirculateRows


tmenu	Tw_CreateMenu(tdisplay TwD, hwcol ColItem, hwcol ColSelect, hwcol ColDisabled,
		     hwcol ColSelectDisabled, hwcol ColShtCut, hwcol ColSelShtCut, byte FlagDefColInfo);
void	Tw_SetInfoMenu(tdisplay TwD, tmenu Menu, byte Flags, ldat Len, TW_CONST byte *Text, TW_CONST hwcol *ColText);
#define Tw_Info4Menu  Tw_SetInfoMenu
#define Tw_DeleteMenu Tw_DeleteObj

tmsgport Tw_CreateMsgPort(tdisplay TwD, byte NameLen, TW_CONST byte *ProgramName, time_t PauseSec, frac_t PauseFraction, byte WakeUp);
/* WakeUp: */
#define TW_TIMER_ALWAYS	((byte)1)
#define TW_TIMER_ONCE	((byte)2)
#define Tw_DeleteMsgPort Tw_DeleteObj
tmsgport Tw_FindMsgPort(tdisplay TwD, tmsgport Prev, byte NameLen, TW_CONST byte *ProgramName);
     
tobj	Tw_PrevObj(tdisplay TwD, tobj Obj);
tobj	Tw_NextObj(tdisplay TwD, tobj Obj);
tobj	Tw_ParentObj(tdisplay TwD, tobj Obj);

tgadget	Tw_G_PrevGadget(tdisplay TwD, tgadget Gadget);
tgadget	Tw_G_NextGadget(tdisplay TwD, tgadget Gadget);
tgroup	Tw_GroupGadget(tdisplay TwD, tgadget Gadget);

twidget	Tw_O_PrevWidget(tdisplay TwD, twidget Widget);
twidget	Tw_O_NextWidget(tdisplay TwD, twidget Widget);
tmsgport Tw_OwnerWidget(tdisplay TwD, twidget Widget);

tscreen	Tw_FirstScreen(tdisplay TwD);
twidget	Tw_FirstWidget(tdisplay TwD, twidget W);
tmsgport Tw_FirstMsgPort(tdisplay TwD);
tmenu	Tw_FirstMenu(tdisplay TwD, tmsgport MsgPort);
twidget	Tw_FirstW(tdisplay TwD, tmsgport MsgPort);
tgroup	Tw_FirstGroup(tdisplay TwD, tmsgport MsgPort);
tmutex	Tw_FirstMutex(tdisplay TwD, tmsgport MsgPort);
tmenuitem Tw_FirstMenuItem(tdisplay TwD, tmenu Menu);
tgadget	Tw_FirstGadget(tdisplay TwD, tgroup Group);

dat  Tw_GetDisplayWidth(tdisplay TwD);
dat  Tw_GetDisplayHeight(tdisplay TwD);
tall Tw_GetAll(tdisplay TwD);

tmsg Tw_CreateMsg(tdisplay TwD, uldat Type, uldat Len);
void Tw_DeleteMsg(tdisplay TwD, tmsg Msg);
byte Tw_SendMsg(tdisplay TwD, tmsgport MsgPort, tmsg Msg);
void Tw_BlindSendMsg(tdisplay TwD, tmsgport MsgPort, tmsg Msg);

tobj Tw_GetOwnerSelection(tdisplay TwD);
void Tw_SetOwnerSelection(tdisplay TwD, time_t Time, frac_t Frac);

void Tw_RequestSelection(tdisplay TwD, tobj Owner, uldat ReqPrivate);
void Tw_NotifySelection(tdisplay TwD, tobj Requestor, uldat ReqPrivate,
			uldat Magic, TW_CONST byte MIME[TW_MAX_MIMELEN],
			uldat Len, TW_CONST byte *Data);
#define TW_SEL_CURRENTTIME ((time_t)0)


byte Tw_SetServerUid(tdisplay TwD, uldat uid, byte privileges);
#define TW_PRIV_NONE 0
#define TW_PRIV_SGIDTTY 1
#define TW_PRIV_SUIDROOT 2

tdisplay Tw_Open(TW_CONST byte *Tw_Display);
void	Tw_Close(tdisplay TwD);
int	Tw_ConnectionFd(tdisplay TwD);

uldat   Tw_LibraryVersion(tdisplay TwD);
uldat   Tw_ServerVersion(tdisplay TwD);

byte    Tw_InPanic(tdisplay TwD);

/* these return FALSE only after libTw has paniced */
byte    Tw_Flush(tdisplay TwD);
byte    Tw_Sync(tdisplay TwD);

/*
 * try to write to the underlying socket.
 * if not all data could be written, write as much as possible,
 * keep the rest queued, then return.
 * 
 * returns FALSE only after libTw has paniced,
 * returns TRUE+TRUE if not all data could be written.
 */
byte    Tw_TimidFlush(tdisplay TwD);

/*
 * This is the function you must call to check if there are pending Msgs,
 * i.e. already received from the socket.
 * Since Msgs can be received even during libTw calls, you cannot rely only
 * on select(Tw_ConnectionFd()) to detect incoming messages, as they could
 * be already queued in the library. So you must call either Tw_PendingMsg()
 * or Tw_PeekMsg() below to check for messages.
 * Returns the first pending Msg, without removing it from the queue,
 * ot (tmsg)0 if no Msgs are pending.
 */
tmsg Tw_PendingMsg(tdisplay TwD);

/*
 * This is the function you must call to check if there are Msgs available,
 * i.e. either already received or that can be received non-blocking.
 * Returns the first available Msg, without removing it from the queue,
 * ot (tmsg)0 if no Msgs are available.
 */
tmsg	Tw_PeekMsg(tdisplay TwD);

/*
 * This is the function you must call to get a Msg from the server.
 * If Wait is TRUE  and no Msg is available, it waits until a Msg arrives.
 * If Wait is FALSE and no Msg is available, it tries non-blocking
 *                  to receive more Msgs.
 * 
 * In both cases, if there is at least a Msgs available it is returned
 * without waiting.
 */
tmsg	Tw_ReadMsg(tdisplay TwD, byte Wait);

/*
 * this is just like Tw_ReadMsg(), but returns a Tw_AllocMem()ed copy
 * of the message, to avoid concurrency problems with other threads.
 * You must Tw_FreeMem() it when done!
 */
tmsg	Tw_CloneReadMsg(tdisplay TwD, byte Wait);



tlistener Tw_AddKeyboardListener(tdisplay TwD, twidget Widget, udat Key,  udat ShiftFlags, tfn_listener Listener, void *Arg);
tlistener Tw_AddMouseListener(   tdisplay TwD, twidget Widget, udat Code, udat ShiftFlags, tfn_listener Listener, void *Arg);

tlistener Tw_AddControlListener(  tdisplay TwD, twidget Widget, udat Code, tfn_listener Listener, void *Arg);
tlistener Tw_AddClientMsgListener(tdisplay TwD, twidget Widget, udat Code, tfn_listener Listener, void *Arg);
tlistener Tw_AddDisplayListener(  tdisplay TwD,                 udat Code, tfn_listener Listener, void *Arg);
tlistener Tw_AddWidgetListener(   tdisplay TwD, twidget Widget, udat Code, tfn_listener Listener, void *Arg);
tlistener Tw_AddGadgetListener(   tdisplay TwD, twidget Widget, udat Code, tfn_listener Listener, void *Arg);

tlistener Tw_AddMenuListener(tdisplay TwD, twidget Widget, tmenu Menu, udat Code, tfn_listener Listener, void *Arg);

tlistener Tw_AddSelectionListener(tdisplay TwD, twidget Widget, tfn_listener Listener, void *Arg);
tlistener Tw_AddSelectionNotifyListener( tdisplay TwD,          tfn_listener Listener, void *Arg);
tlistener Tw_AddSelectionRequestListener(tdisplay TwD,          tfn_listener Listener, void *Arg);

/* these five functions are used by Tw/Tw++.h */
void Tw_InsertListener(tdisplay TwD, tlistener L);
void Tw_RemoveListener(tdisplay TwD, tlistener L);
tlistener Tw_CreateListener(tdisplay TwD, udat Type, tevent_any E, tfn_listener Listener, void *Arg);
void Tw_SetTEListener(tdisplay TwD, tlistener L, udat type, tevent_any E);
void Tw_DeleteListener(tdisplay TwD, tlistener L);

void Tw_SetDefaultListener(tdisplay TwD, tfn_default_listener listener, void *arg);

byte  Tw_DispatchMsg(tdisplay TwD, tmsg Msg);
byte  Tw_MainLoop(tdisplay TwD);
void  Tw_ExitMainLoop(tdisplay TwD);





/*
 *   <*>  B I G    W A R N I N G  <*>
 * 
 * Tw_PendingMsg(), Tw_PeekMsg(), Tw_ReadMsg() and Tw_CloneReadMsg()
 * return a (tmsg) pointer to data in a static buffer.
 * the pointer becomes invalid after a call
 * to one of the following functions:
 * (but calling them with an arg pointing to data inside tmsg is legal,
 * even with this side effect)
 * 
 * Tw_Sync();
 * Tw_PeekMsg();
 * Tw_ReadMsg();
 * Tw_CloneReadMsg();
 * Tw_CreateGadget();
 * Tw_FindWidgetAtWidget();
 * Tw_CreateWindow();
 * Tw_Create4MenuWindow();
 * Tw_Create4MenuMenuItem();
 * Tw_CreateMsgPort();
 * Tw_CreateMenu();
 * Tw_FirstScreen();
 * Tw_FirstMsgPort();
 * [...]
 * 
 * and in general after a call to any Tw() function of libTw.h
 * with non-void return value, so that it sends something
 * to the server and waits for the server to send the return value.
 * 
 * If you are using threads, or just have a doubt that your code might
 * reference the message returned by Tw_ReadMsg()
 * after a call to one of the above functions, use Tw_CloneReadMsg() :
 * it's just like Tw_ReadMsg(), except that it returns a Tw_AllocMem() copy
 * of the message, that you can use as long as you wish. Just remember to
 * Tw_FreeMem() it when done!
 * 
 * Other important notes:
 * 1. Only ONE MsgPort can exists at max on each connection.
 * 2. Tw_DeleteMsgPort() calls Tw_DeleteMenu() on all menus created by the client
 *    and Tw_DeleteWidget[/Gadget/Window]() on all gadgets/widgets/windows created by the client.
 * 3. Tw_DeleteMenu() calls Tw_UnMapWindow() on all windows which use that menu.
 * 4. Tw_DeleteWidget[/Gadget/Window]() calls Tw_UnMapWidget() on all sub-gadgets/widgets/windows
 *    of that widget, does NOT call Tw_DeleteWidget[/Gadget/Window]() recursively on them!
 *    If you wish to delete a widget and all sub-widgets inside it,
 *    use Tw_RecursiveDeleteWidget[/Gadget/Window]()
 * 
 * also, if you exit() from your program without calling Tw_Flush(), Tw_Sync() or Tw_Close(),
 * pending data will *NOT* be sent to the server.
 */


#include <Tw/Tw_1.h>



#ifdef __cplusplus
}
#endif

#endif /* _TW_H */

