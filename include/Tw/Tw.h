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

#ifndef _TWIN_H
#define _TWIN_H

/* a cutdown version of twin.h */

#ifndef TW_RETSIGTYPE
# include <Tw/Twautoconf.h>
#endif

#include <Tw/compiler.h>
#include <Tw/version.h>
#include <Tw/osincludes.h>
#include <Tw/missing.h>
#include <Tw/datatypes.h>
#include <Tw/datasizes.h>
#include <Tw/uni_types.h>

#define TW_NOID		((uldat)0)
#define TW_ERRID	((uldat)-1)
#define TW_NOFD		(-1)
#define TW_NOSLOT	MAXULDAT

#ifndef FALSE
# define FALSE	((byte)0)
# define TRUE	(!FALSE)
#endif

/* "Twin" in native byte-order */
#define TWIN_MAGIC ((uldat)0x6E697754ul)
/* "Go!!" in native byte-order */
#define TW_GO_MAGIC   ((uldat)0x21216F47ul)
/* "Wait" in native byte-order */
#define TW_WAIT_MAGIC ((uldat)0x74696157ul)

#define TW_INET_PORT	7754

#define TW_SMALLBUFF	256
#define TW_BIGBUFF	4096


/* Macros for HW VGA (not ANSI!) colors */
#define BLACK	((hwcol)0)
#define BLUE	((hwcol)1)
#define GREEN	((hwcol)2)
#define CYAN	(BLUE|GREEN)
#define RED	((hwcol)4)
#define MAGENTA	(BLUE|RED)
#define YELLOW  (GREEN|RED)
#define WHITE	(BLUE|GREEN|RED)
#define HIGH	((hwcol)8)
#define MAXCOL	((hwcol)0xF)

#define ANSI2VGA(col) (((col) & 0x1 ? RED   : 0) | \
		       ((col) & 0x2 ? GREEN : 0) | \
		       ((col) & 0x4 ? BLUE  : 0))
#define VGA2ANSI(col) ANSI2VGA(col)

/* foreground / background colors handling */
/*
 * NOTE: draw.c:DoShadowColor() assumes that
 * COL(fg1, bg1) | COL(fg2, bg2) == COL(fg1|fg2, bg1|bg2)
 * and
 * COL(fg1, bg1) & COL(fg2, bg2) == COL(fg1&fg2, bg1&bg2)
 */
#define FG(col)	(col)
#define BG(col)	((col)<<4)
#define COL(fg,bg) (FG(fg)|BG(bg))
#define COLBG(col) ((col) >> 4)
#define COLFG(col) ((col) & 0x0F)


/* if sizeof(hwattr) == 2, bytes are { 'ascii', 'col' } */

/* hwattr <-> hwcol+hwfont conversion */
# define HWATTR16(col,ascii) (((byte16)(byte)(col) << 8) | (byte16)(byte)(ascii))
# define HWATTR_COLMASK16(attr) ((attr) & 0xFF00)
# define HWATTR_FONTMASK16(attr) ((attr) & 0xFF)
# define HWCOL16(attr) ((hwcol)((attr) >> 8))
# define HWFONT16(attr) ((byte)(attr))


/* if sizeof(hwattr) == 4, bytes are { 'ascii_low', 'col', 'ascii_high', 'unused' } */

/* hwattr <-> hwcol+hwfont conversion */
# define HWATTR32(col,ascii) (((byte32)(byte)(col) << 8) | (((byte32)(ascii) & 0xFF00) << 8) | (byte32)(byte)(ascii))
# define HWATTR_COLMASK32(attr) ((attr) & 0xFF00)
# define HWATTR_FONTMASK32(attr) ((attr) & 0xFF00FF)
# define HWCOL32(attr) ((hwcol)((attr) >> 8))
# define HWFONT32(attr) ((byte16)(((attr) & 0xFF) | (((attr) >> 8) & 0xFF00)))




/* types for time intervals */
typedef struct s_timevalue  {
    time_t Seconds;
    frac_t Fraction;
} timevalue;

/*
 * Notes about the timevalue struct:
 * 
 * it is used to represent both time intervals and absolute times;
 * the ->Seconds is a time_t numeric field.
 * DON'T assume time_t is 32 bit (or any other arbitrary size)
 * since in 19 Jan 2038 at 04:14:08 any signed, 32 bit time_t will overflow.
 * So use sizeof(time_t) if you really need.
 * 
 * the ->Fraction is a frac_t numeric field.
 * As above, DON'T assume frac_t is 32 bit (or any other arbitrary size)
 * since in the future we may want a finer granularity than the nanosecond one
 * possible with a 32 bit frac_t.
 * So :
 * 1) use sizeof(frac_t) if you really need
 * 2) don't assume (frac_t)1 is a nanosecond (or any other arbitrary time),
 *    but always use the form '1 NanoSECs', '250 MilliSECs + 7 MicroSECs', etc.
 * 3) if you _absolutely_ need to know to what time (frac_t)1 corresponds,
 *    use this: '1 FullSECs' is the number of (frac_t)1 intervals in a second.
 * 4) for the moment, the only defined fractions of a second are:
 *    FullSECs, MilliSECs, MicroSECs, NanoSECs.
 *    Others may be added in the future (PicoSECs, FemtoSECs, AttoSECs, ...)
 */
#define THOUSAND	((frac_t)1000)

#define NanoSECs	* 1 /* i.e. (frac_t)1 is a nanosecond */
#define MicroSECs	* (THOUSAND NanoSECs)
#define MilliSECs	* (THOUSAND MicroSECs)
#define FullSECs	* (THOUSAND MilliSECs)



/* keyboard events ShiftFlags */
#define TW_KBD_SHIFT_FL		0x1
#define TW_KBD_CTRL_FL		0x2
#define TW_KBD_ALT_FL		0x4
#define TW_KBD_CAPS_LOCK	0x8
#define TW_KBD_NUM_LOCK		0x10

/* mouse events stuff */
#define HOLD_LEFT	1
#define HOLD_MIDDLE	2
#define HOLD_RIGHT	4
#define HOLD_ANY	(HOLD_LEFT|HOLD_MIDDLE|HOLD_RIGHT)

#define PRESS_LEFT	0x08
#define PRESS_MIDDLE	0x18
#define PRESS_RIGHT	0x28
#define PRESS_ANY	0x38

#define DOWN_LEFT	(HOLD_LEFT|PRESS_LEFT)
#define DOWN_MIDDLE	(HOLD_MIDDLE|PRESS_MIDDLE)
#define DOWN_RIGHT	(HOLD_RIGHT|PRESS_RIGHT)
#define DOWN_ANY	(HOLD_ANY|PRESS_ANY)

#define RELEASE_LEFT	0x10
#define RELEASE_MIDDLE	0x20
#define RELEASE_RIGHT	0x30
#define RELEASE_ANY	0x30

#define DRAG_MOUSE	0x40

#define MOTION_MOUSE	0x00

#define ANY_ACTION_MOUSE	(PRESS_ANY | RELEASE_ANY | DRAG_MOUSE | MOTION_MOUSE)

#define MAX_ACTION_MOUSE	0x48

#define isPRESS(code)	((code) & 0x08)
#define isDRAG(code)	((code) & DRAG_MOUSE)
#define isRELEASE(code)	((code) & ANY_ACTION_MOUSE && !isPRESS(code) && !isDRAG(code))
#define isMOTION(code)	(!(code))

#define isSINGLE_PRESS(code) (isPRESS(code) && ((code) == DOWN_LEFT || (code) == DOWN_MIDDLE || (code) == DOWN_RIGHT))
#define isSINGLE_DRAG(code) (isDRAG(code) && ((code) == (DRAG_MOUSE|HOLD_LEFT) || (code) == (DRAG_MOUSE|HOLD_MIDDLE) || (code) == (DRAG_MOUSE|HOLD_RIGHT)))
#define isSINGLE_RELEASE(code) (isRELEASE(code) && !((code) & HOLD_ANY))



#define msg_magic	((uldat)0xA3a61ce4ul)


#define TWVER_MAJOR(n) ((n)>>16)
#define TWVER_MINOR(n) (((n)>>8) & 0xFF)
#define TWVER_PATCH(n) ((n) & 0xFF)
#define TWVER_BUILD(M, m, p) (((M)<<16)|((m)<<8)|(p))


#endif /* _TWIN_H */



typedef uldat tobj;
typedef uldat tgroup;     
typedef uldat twidget;
typedef uldat tgadget;
typedef uldat twindow;
typedef uldat tscreen;
typedef uldat trow;
typedef uldat tmenuitem;
typedef uldat tmenu;
typedef uldat tmsgport;
typedef uldat tmutex;




/* types for messages from server */
typedef struct s_tevent_common *tevent_common;
struct s_tevent_common {
    twidget W;
    udat Code, pad;
};

typedef struct s_tevent_display *tevent_display;
struct s_tevent_display {
    twidget W; /* not used here */
    udat Code, Len;
    dat X, Y;
    byte Data[sizeof(uldat)]; /* [len] bytes actually */
};
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
struct s_tevent_keyboard {
    twidget W;
    udat Code, ShiftFlags, SeqLen;
    byte pad, AsciiSeq[1];  /* actually (SeqLen+1) bytes, including final \0 */
};

typedef struct s_tevent_mouse *tevent_mouse;
struct s_tevent_mouse {
    twidget W;
    udat Code, ShiftFlags;
    dat X, Y; /* these coords are relative to the widget top-left corner */
};

typedef struct s_tevent_widget *tevent_widget;
struct s_tevent_widget {
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
struct s_tevent_gadget {
    twidget W;
    udat Code, Flags; /* the Flags of the gadget that was (de)activated */
};

typedef struct s_tevent_menu *tevent_menu;
struct s_tevent_menu {
    twindow W;
    udat Code, pad;
    tmenu Menu;
};

typedef struct s_tevent_selection *tevent_selection;
struct s_tevent_selection {
    twidget W;
    udat Code, pad; /* unused */
    dat X, Y; /* these coords are absolute, to allow cross-window cut-n-paste */
};

#define TW_MAX_MIMELEN 64

typedef struct s_tevent_selectionnotify *tevent_selectionnotify;
struct s_tevent_selectionnotify {
    twidget W;
    udat Code, pad; /* unused */
    uldat ReqPrivate;
    uldat Magic;
    byte MIME[TW_MAX_MIMELEN];
    uldat Len;
    byte Data[1]; /* Data[] is Len bytes actually */
};
/*SelectionNotify Magic*/
#define TW_SEL_TEXTMAGIC	((uldat)0x54657874)
#define TW_SEL_HWFONTMAGIC	((uldat)0x4877666E)
#define TW_SEL_FILEMAGIC	((uldat)0x46696c65)
#define TW_SEL_URLMAGIC		((uldat)0xAB1691BA)
#define TW_SEL_DATAMAGIC	((uldat)0xDA1AA1AD) /* check MIME if you get this */
#define TW_SEL_IDMAGIC		((uldat)0x49644964)

typedef struct s_tevent_selectionrequest *tevent_selectionrequest;
struct s_tevent_selectionrequest {
    twidget W;
    udat Code, pad; /* unused */
    tmsgport Requestor;
    uldat ReqPrivate;
};

typedef struct s_tevent_control *tevent_control;
struct s_tevent_control {
    twidget W;
    udat Code, Len;
    dat X, Y;
    byte Data[sizeof(uldat)]; /* [Len+1] bytes actually, Data[Len] == '\0' */
};

/* some TW_MSG_CONTROL codes */
#define TW_MSG_CONTROL_QUIT		((udat)0)
#define TW_MSG_CONTROL_RESTART		((udat)1)
#define TW_MSG_CONTROL_OPEN		((udat)2)
#define TW_MSG_CONTROL_DRAGNDROP	((udat)3)

typedef struct s_tevent_clientmsg *tevent_clientmsg;
struct s_tevent_clientmsg {
    twidget W;
    udat Code, Len;
    byte Data[sizeof(uldat)]; /* [Len+1] bytes actually, Data[Len] == '\0' */
};

typedef union s_tevent_any *tevent_any;
union s_tevent_any {
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
struct s_tmsg {
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


     


     
     










/* Widget->Attrib */
/*
 * ask the server to send events even for mouse motion without any pressed button.
 * works only if WIDGET_WANT_MOUSE is set too.
 */
#define TW_WIDGET_WANT_MOUSE_MOTION	0x0001
#define TW_WIDGET_WANT_KEYS	0x0002
#define TW_WIDGET_WANT_MOUSE	0x0004
#define TW_WIDGET_WANT_CHANGES	0x0008

/* Widget->Flags */
#define TW_WIDGETFL_USEEXPOSE	0x02
#define TW_WIDGETFL_USEFILL	0x03
#define TW_WIDGETFL_USEANY	0x07 /* mask of all above ones */






/* Gadget->Attrib */
#define TW_GADGET_WANT_MOUSE_MOTION	TW_WIDGET_WANT_MOUSE_MOTION /* 0x0001 */
#define TW_GADGET_WANT_KEYS	TW_WIDGET_WANT_KEYS	/* 0x0002 */
#define TW_GADGET_WANT_MOUSE	TW_WIDGET_WANT_MOUSE	/* 0x0004 */
#define TW_GADGET_WANT_CHANGES	TW_WIDGET_WANT_CHANGES	/* 0x0008 */


/* Gadget->Flags */
#define TW_GADGETFL_USETEXT	0x00   /* it's the default */
#define TW_GADGETFL_USEEXPOSE	TW_WIDGETFL_USEEXPOSE	/* 0x02 */
#define TW_GADGETFL_USEFILL	TW_WIDGETFL_USEFILL	/* 0x03 */
#define TW_GADGETFL_USEANY	TW_WIDGETFL_USEANY	/* 0x07 */

#define TW_GADGETFL_DISABLED	0x0020
#define TW_GADGETFL_TEXT_DEFCOL	0x0040
/* this makes the gadget 'checkable' : can be in 'checked' or 'unchecked' state.
 * also necessary to put the gadget in a group */
#define TW_GADGETFL_TOGGLE	0x0080
#define TW_GADGETFL_PRESSED	0x0100






/* Window->Attrib */
#define TW_WINDOW_WANT_MOUSE_MOTION	TW_WIDGET_WANT_MOUSE_MOTION /* 0x0001 */
#define TW_WINDOW_WANT_KEYS	TW_WIDGET_WANT_KEYS	/* 0x0002 */
#define TW_WINDOW_WANT_MOUSE	TW_WIDGET_WANT_MOUSE	/* 0x0004 */
#define TW_WINDOW_WANT_CHANGES	TW_WIDGET_WANT_CHANGES	/* 0x0008 */
#define TW_WINDOW_DRAG		0x0100
#define TW_WINDOW_RESIZE	0x0200
#define TW_WINDOW_CLOSE		0x0400
#define TW_WINDOW_ROLLED_UP	0x0800
#define TW_WINDOW_X_BAR		0x1000
#define TW_WINDOW_Y_BAR		0x2000
#define TW_WINDOW_AUTO_KEYS	0x4000



/* Window->Flags */
#define TW_WINDOWFL_USEROWS	0x00 /* it's the default */
#define TW_WINDOWFL_USECONTENTS	0x01
#define TW_WINDOWFL_USEEXPOSE	TW_WIDGETFL_USEEXPOSE /* 0x02 */
#define TW_WINDOWFL_USEFILL	TW_WIDGETFL_USEFILL   /* 0x03 */
#define TW_WINDOWFL_USEANY	TW_WIDGETFL_USEANY	/* 0x07 */

#define	TW_WINDOWFL_CURSOR_ON	0x10
#define TW_WINDOWFL_MENU	0x20
#define TW_WINDOWFL_DISABLED	0x40
#define TW_WINDOWFL_BORDERLESS	0x80
#define TW_WINDOWFL_ROWS_INSERT	0x0100
#define TW_WINDOWFL_ROWS_DEFCOL	0x0200
#define TW_WINDOWFL_ROWS_SELCURRENT	0x0400





/* Screen->Flags */
#define TW_SCREENFL_USEBG	0x00 /* it's the default */
#define TW_SCREENFL_USEEXPOSE	TW_WIDGETFL_USEEXPOSE	/* 0x02 */
#define TW_SCREENFL_USEFILL	TW_WIDGETFL_USEFILL	/* 0x03 */
#define TW_SCREENFL_USEANY	TW_WIDGETFL_USEANY	/* 0x07 */







/* Window CursorType : */
/* These come from linux/drivers/char/console.c */
#define TW_DEFAULTCURSOR 0
#define TW_NOCURSOR	 1
#define TW_LINECURSOR	 2
#define TW_SOLIDCURSOR	 8




/* Row Flags : */
#define TW_ROW_INACTIVE	((byte)0x00)
#define TW_ROW_ACTIVE	((byte)0x01)
#define TW_ROW_IGNORE	((byte)0x02)
#define TW_ROW_DEFCOL	((byte)0x04)



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
tmsgport Tw_GetOwnerWidget(tdisplay TwD, twidget Widget);
twidget Tw_FindWidgetAtWidget(tdisplay TwD, twidget Parent, dat i, dat j);

void    Tw_FocusSubWidget(tdisplay TwD, twidget W);
void    Tw_ExposeWidget(tdisplay TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, TW_CONST byte *Text, TW_CONST hwfont *Font, TW_CONST hwattr *Attr);
void    Tw_ExposeWidget2(tdisplay TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST byte *Text, TW_CONST hwfont *Font, TW_CONST hwattr *Attr);

void	Tw_ExposeTextWidget(tdisplay TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST byte *Text);
void	Tw_ExposeHWFontWidget(tdisplay TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST hwfont *Font);
void	Tw_ExposeHWAttrWidget(tdisplay TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST hwattr *Attr);



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
#define Tw_GetOwnerGadget		Tw_GetOwnerWidget
#define Tw_ExposeTextGadget		Tw_ExposeTextWidget
#define Tw_ExposeHWFontGadget		Tw_ExposeHWFontWidget
#define Tw_ExposeHWAttrGadget		Tw_ExposeHWAttrWidget

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
#define Tw_GetOwnerWindow		Tw_GetOwnerWidget
#define Tw_ExposeTextWindow		Tw_ExposeTextWidget
#define Tw_ExposeHWFontWindow		Tw_ExposeHWFontWidget
#define Tw_ExposeHWAttrWindow		Tw_ExposeHWAttrWidget

twindow Tw_Create4MenuWindow(tdisplay TwD, tmenu Menu);
#define Tw_Win4Menu			Tw_Create4MenuWindow
void	Tw_WriteAsciiWindow(tdisplay TwD, twindow Window, ldat AsciiLen, TW_CONST byte *AsciiSeq);
void	Tw_WriteStringWindow(tdisplay TwD, twindow Window, ldat AsciiLen, TW_CONST byte *String);
void	Tw_WriteHWFontWindow(tdisplay TwD, twindow Window, ldat AsciiLen, TW_CONST hwfont *HWFont);
void    Tw_WriteHWAttrWindow(tdisplay TwD, twindow Window, dat x, dat y, ldat Len, TW_CONST hwattr *Attr);
void	Tw_WriteRowWindow(tdisplay TwD, twindow Window, ldat Len, TW_CONST byte *Text);

void	Tw_SetColTextWindow(tdisplay TwD, twindow Window, hwcol ColText);
void	Tw_SetColorsWindow(tdisplay TwD, twindow Window, udat Bitmap, hwcol ColGadgets, hwcol ColArrows, hwcol ColBars, hwcol ColTabs,
			  hwcol ColBorder, hwcol ColText, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled);
void	Tw_ConfigureWindow(tdisplay TwD, twindow Window, byte Bitmap, dat Left, dat Up, dat MinXWidth, dat MinYWidth, dat MaxXWidth, dat MaxYWidth);
void	Tw_ResizeWindow(tdisplay TwD, twindow Window, dat XWidth, dat YWidth);
void	Tw_GotoXYWindow(tdisplay TwD, twindow Window, ldat X, ldat Y);

tmenuitem Tw_Create4MenuMenuItem(tdisplay TwD, tobj Parent, twindow Window, byte Flags, dat Len, TW_CONST byte *Name);
#define   Tw_Item4Menu Tw_Create4MenuMenuItem
uldat     Tw_Create4MenuCommonMenuItem(tdisplay TwD, tmenu Menu);
#define   Tw_Item4MenuCommon Tw_Create4MenuCommonMenuItem
#define   Tw_DeleteMenuItem Tw_DeleteObj

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
     
void	Tw_BgImageScreen(tdisplay TwD, tscreen Screen, dat BgWidth, dat BgHeight, TW_CONST hwattr *BgImage);

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





/*
 * libTw.so.1 --- single head compatibility. these are too handy to drop them.
 */
extern tdisplay Tw_DefaultD;

#define TwCheckMagic(id)		Tw_CheckMagic(id)

#define TwConfigMalloc(my_malloc, my_realloc, my_free) Tw_ConfigMalloc(my_malloc, my_realloc, my_free)

#define TwLenStr(S)			Tw_LenStr(S)
#define TwCmpStr(S1, S2)		Tw_CmpStr(S1, S2)
#define TwCopyStr(From,To)		Tw_CopyStr(From, To)

#define TwAllocMem(Size)		Tw_AllocMem(Size)
#define TwReAllocMem(Mem, Size)		Tw_ReAllocMem(Mem, Size)
#define TwFreeMem(Mem)			Tw_FreeMem(Mem)
#define TwCloneMem(Mem,Size)		Tw_CloneMem(Mem,Size)
#define TwCloneStr(S)			Tw_CloneStr(S)
#define TwCloneStr2HWFont(Mem, Size)	Tw_CloneMem2HWFont(Mem, Size)

#define TwCopyMem(From, To, Size)	Tw_CopyMem(From, To, Size)
#define TwMoveMem(From, To, Size)	Tw_MoveMem(From, To, Size)
#define TwWriteMem(Mem, Char, Size)	Tw_WriteMem(Mem, Char, Size)
#define TwCmpMem(m1, m2, Size)		Tw_CmpMem(m1, m2, Size)

#define TwDropPrivileges()		Tw_DropPrivileges()
#define TwGetPrivileges()		Tw_GetPrivileges()

#define TwFindFunction(Len, Name, ProtoLen, Proto)	Tw_FindFunction(Tw_DefaultD, Len, Name, ProtoLen, Proto)
#define TwServerSizeof(Type)		Tw_ServerSizeof(Tw_DefaultD, Type)
#define TwEnableGzip()			Tw_EnableGzip(Tw_DefaultD)
#define TwDisableGzip()			Tw_DisableGzip(Tw_DefaultD)
#define TwNeedResizeDisplay()		Tw_NeedResizeDisplay(Tw_DefaultD)
#define TwAttachHW(len, name, flags)	Tw_AttachHW(Tw_DefaultD, len, name, flags)
#define TwAttachGetReply(len)		Tw_AttachGetReply(Tw_DefaultD, len)
#define TwAttachConfirm()		Tw_AttachConfirm(Tw_DefaultD)
#define TwDetachHW(len, name)		Tw_DetachHW(Tw_DefaultD, len, name)
#define TwSetFontTranslation(trans)	Tw_SetFontTranslation(Tw_DefaultD, trans)
#define TwSetUniFontTranslation(trans)	Tw_SetUniFontTranslation(Tw_DefaultD, trans)

#define TwDeleteObj(O)			Tw_DeleteObj(Tw_DefaultD, O)

#define TwCreateWidget(XWidth, YWidth, Attrib, Flags, Left, Up, Fill) \
					Tw_CreateWidget(Tw_DefaultD, XWidth, YWidth, Attrib, Flags, Left, Up, Fill)
#define TwDeleteWidget(W)		Tw_DeleteWidget(Tw_DefaultD, W)
#define TwRecursiveDeleteWidget(W)	Tw_RecursiveDeleteWidget(Tw_DefaultD, W)
#define TwMapWidget(W, Parent)		Tw_MapWidget(Tw_DefaultD, W, Parent)
#define TwUnMapWidget(W)		Tw_UnMapWidget(Tw_DefaultD, W)
#define TwSetXYWidget(W, x, y)		Tw_SetXYWidget(Tw_DefaultD, W, x, y)
#define TwGetOwnerWidget(W)		Tw_GetOwnerWidget(Tw_DefaultD, W)
#define TwFindWidgetAtWidget(W, i, j)	Tw_FindWidgetAtWidget(Tw_DefaultD, W, i, j)
#define TwExposeTextWidget(Window, XWidth, YWidth, Left, Up, Pitch, Text) \
		Tw_ExposeTextWidget(Tw_DefaultD, Window, XWidth, YWidth, Left, Up, Pitch, Text)
#define TwExposeHWFontWidget(Window, XWidth, YWidth, Left, Up, Pitch, Font) \
		Tw_ExposeHWFontWidget(Tw_DefaultD, Window, XWidth, YWidth, Left, Up, Pitch, Font)
#define TwExposeHWAttrWidget(Window, XWidth, YWidth, Left, Up, Pitch, Attr) \
		Tw_ExposeHWAttrWidget(Tw_DefaultD, Window, XWidth, YWidth, Left, Up, Pitch, Attr)


#define TwFocusSubWidget(W)		Tw_FocusSubWidget(Tw_DefaultD, W)

#define TwCreateGroup()			Tw_CreateGroup(Tw_DefaultD)
#define TwInsertGadgetGroup(Group, G)	Tw_InsertGadgetGroup(Tw_DefaultD, Group, G)
#define TwRemoveGadgetGroup(Group, G)	Tw_RemoveGadgetGroup(Tw_DefaultD, Group, G)
#define TwGetSelectedGadgetGroup(Group) Tw_GetSelectedGadgetGroup(Tw_DefaultD, Group)
#define TwSetSelectedGadgetGroup(Group, G) Tw_SetSelectedGadgetGroup(Tw_DefaultD, Group, G)

#define TwCreateGadget( Parent, XWidth, YWidth, TextNormal, Attrib, Flags, Code, \
     ColText, ColTextSelect, ColTextDisabled, ColTextSelectDisabled, Left, Up ) \
  Tw_CreateGadget(Tw_DefaultD, Parent, XWidth, YWidth, TextNormal, Attrib, Flags, Code, \
     ColText, ColTextSelect, ColTextDisabled, ColTextSelectDisabled, Left, Up )
#define TwDeleteGadget			TwDeleteObj
#define TwRecursiveDeleteGadget		TwRecursiveDeleteWidget
#define TwMapGadget			TwMapWidget
#define TwUnMapGadget			TwUnMapWidget
#define TwSetXYGadget			TwSetXYWidget
#define TwGetOwnerGadget		TwGetOwnerWidget
#define TwExposeTextGadget		TwExposeTextWidget
#define TwExposeHWFontGadget		TwExposeHWFontWidget
#define TwExposeHWAttrGadget		TwExposeHWAttrWidget
#define TwCreateButtonGadget(Window, BgCol, Col, ColDisabled, Code, Flags, \
				 Left, Up, XWidth, YWidth, Text) \
		Tw_CreateButtonGadget(Tw_DefaultD, \
			     Window, BgCol, Col, ColDisabled, Code, Flags, \
			     Left, Up, XWidth, YWidth, Text)

#define TwSetPressedGadget(Gadget, on)	Tw_SetPressedGadget(Tw_DefaultD, Gadget, on)
#define TwIsPressedGadget(Gadget)	Tw_IsPressedGadget(Tw_DefaultD, Gadget)
#define TwSetToggleGadget(Gadget, on)	Tw_SetToggleGadget(Tw_DefaultD, Gadget, on)
#define TwIsToggleGadget(Gadget)	Tw_IsToggleGadget(Tw_DefaultD, Gadget)
#define TwWriteTextGadget(Gadget, X, Y, Text, Left, Up) \
					Tw_WriteTextGadget(Tw_DefaultD, Gadget, X, Y, Text, Left, Up)
#define TwWriteTextsGadget(Gadget, bitmap, X, Y, Text, Left, Up) \
					Tw_WriteTextsGadget(Tw_DefaultD, Gadget, bitmap, X, Y, Text, Left, Up)
#define TwSetTextGadget(Gadget, X, Y, Text, Left, Up) \
					Tw_SetTextGadget(Tw_DefaultD, Gadget, X, Y, Text, Left, Up)
#define TwSetTextsGadget(Gadget, bitmap, X, Y, Text, Left, Up) \
					Tw_SetTextsGadget(Tw_DefaultD, Gadget, bitmap, X, Y, Text, Left, Up)

#define TwWriteHWFontGadget(Gadget, X, Y, HWFont, Left, Up) \
					Tw_WriteHWFontGadget(Tw_DefaultD, Gadget, X, Y, HWFont, Left, Up)
#define TwWriteHWFontsGadget(Gadget, bitmap, X, Y, HWFont, Left, Up) \
					Tw_WriteHWFontsGadget(Tw_DefaultD, Gadget, bitmap, X, Y, HWFont, Left, Up)
#define TwSetHWFontGadget(Gadget, X, Y, HWFont, Left, Up) \
					Tw_SetHWFontGadget(Tw_DefaultD, Gadget, X, Y, HWFont, Left, Up)
#define TwSetHWFontsGadget(Gadget, bitmap, X, Y, HWFont, Left, Up) \
					Tw_SetHWFontsGadget(Tw_DefaultD, Gadget, bitmap, X, Y, HWFont, Left, Up)


#define TwCreate4MenuRow(Window, Code, FlagActive, TextLen, Text) \
		Tw_Create4MenuRow(Tw_DefaultD, Window, Code, FlagActive, TextLen, Text)
#define TwRow4Menu			TwCreate4MenuRow
#define TwCreateWindow(NameLen, Name, ColName, Menu, ColText, cursorType, \
		       Attrib, Flags, XWidth, YWidth, ScrollBackLines) \
		Tw_CreateWindow(Tw_DefaultD, NameLen, Name, ColName, Menu, ColText, cursorType, \
		       Attrib, Flags, XWidth, YWidth, ScrollBackLines)

#define TwDeleteWindow			TwDeleteObj
#define TwRecursiveDeleteWindow		TwRecursiveDeleteWidget
#define TwMapWindow			TwMapWidget
#define TwUnMapWindow			TwUnMapWidget
#define TwSetXYWindow			TwSetXYWidget
#define TwGetOwnerWindow		TwGetOwnerWidget
#define TwExposeTextWindow		TwExposeTextWidget
#define TwExposeHWFontWindow		TwExposeHWFontWidget
#define TwExposeHWAttrWindow		TwExposeHWAttrWidget

#define TwCreate4MenuWindow(Menu)	Tw_Create4MenuWindow(Tw_DefaultD, Menu)
#define TwWin4Menu			TwCreate4MenuWindow
#define TwWriteAsciiWindow(Window, Len, AsciiSeq) \
		Tw_WriteAsciiWindow(Tw_DefaultD, Window, Len, AsciiSeq)
#define TwWriteStringWindow(Window, Len, String) \
		Tw_WriteStringWindow(Tw_DefaultD, Window, Len, String)
#define TwWriteHWFontWindow(Window, Len, HWFont) \
		Tw_WriteHWFontWindow(Tw_DefaultD, Window, Len, HWFont)
#define TwWriteHWAttrWindow(Window, x, y, Len, Attr) \
		Tw_WriteHWAttrWindow(Tw_DefaultD, Window, x, y, Len, Attr)
#define TwWriteRowWindow(Window, Len, Text) Tw_WriteRowWindow(Tw_DefaultD, Window, Len, Text)

#define TwSetColTextWindow(Window, ColText) Tw_SetColTextWindow(Tw_DefaultD, Window, ColText)
#define TwSetColorsWindow(Window, Bitmap, ColGadgets, ColArrows, ColBars, ColTabs, \
			  ColBorder, ColText, ColSelect, ColDisabled, ColSelectDisabled) \
		Tw_SetColorsWindow(Tw_DefaultD, Window, Bitmap, ColGadgets, ColArrows, ColBars, ColTabs, \
			  ColBorder, ColText, ColSelect, ColDisabled, ColSelectDisabled)
#define TwConfigureWindow(Window, Bitmap, Left, Up, MinXWidth, MinYWidth, MaxXWidth, MaxYWidth) \
	Tw_ConfigureWindow(Tw_DefaultD, Window, Bitmap, Left, Up, MinXWidth, MinYWidth, MaxXWidth, MaxYWidth)
#define TwResizeWindow(Window, X, Y)	Tw_ResizeWindow(Tw_DefaultD, Window, X, Y)
#define TwGotoXYWindow(Window, X, Y)	Tw_GotoXYWindow(Tw_DefaultD, Window, X, Y)

#define TwCreate4MenuMenuItem(Parent, Window, Flags, NameLen, Name) \
		Tw_Create4MenuMenuItem(Tw_DefaultD, Parent, Window, Flags, NameLen, Name)
#define TwItem4Menu			TwCreate4MenuMenuItem
#define TwCreate4MenuCommonMenuItem(Menu) Tw_Create4MenuCommonMenuItem(Tw_DefaultD, Menu)
#define TwItem4MenuCommon		TwCreate4MenuCommonMenuItem
#define TwDeleteMenuItem(MenuItem)	Tw_DeleteMenuItem(Tw_DefaultD, MenuItem)
#define TwCreateMenu(ColItem, ColSelect, ColDisabled, \
		     ColSelectDisabled, ColShtCut, ColSelShtCut, FlagDefColInfo) \
		Tw_CreateMenu(Tw_DefaultD, ColItem, ColSelect, ColDisabled, \
		     ColSelectDisabled, ColShtCut, ColSelShtCut, FlagDefColInfo)

#define TwSetInfoMenu(Menu, Flags, Len, Text, ColText) \
		Tw_SetInfoMenu(Tw_DefaultD, Menu, Flags, Len, Text, ColText)
#define TwInfo4Menu			TwSetInfoMenu
#define TwDeleteMenu(Menu)		Tw_DeleteMenu(Tw_DefaultD, Menu)
     
#define TwCreateMsgPort(NameLen, ProgramName, PauseSec, PauseFraction, WakeUp) \
		Tw_CreateMsgPort(Tw_DefaultD, NameLen, ProgramName, PauseSec, PauseFraction, WakeUp)
#define TwDeleteMsgPort(MsgPort)	Tw_DeleteMsgPort(Tw_DefaultD, MsgPort)
#define TwFindMsgPort(Prev, NameLen, ProgramName) \
		Tw_FindMsgPort(Tw_DefaultD, Prev, NameLen, ProgramName)
     
#define TwBgImageScreen(Screen, BgWidth, BgHeight, BgImage) \
		Tw_BgImageScreen(Tw_DefaultD, Screen, BgWidth, BgHeight, BgImage)

#define TwPrevObj(Obj)			Tw_PrevObj(Tw_DefaultD, Obj)
#define TwNextObj(Obj)			Tw_NextObj(Tw_DefaultD, Obj)
#define TwParentObj(Obj)		Tw_ParentObj(Tw_DefaultD, Obj)
#define TwO_PrevWidget(Widget)		Tw_O_PrevWidget(Tw_DefaultD, Widget)
#define TwO_NextWidget(Widget)		Tw_O_NextWidget(Tw_DefaultD, Widget)
#define TwOwnerWidget(Widget)		Tw_OwnerWidget(Tw_DefaultD, Widget)
#define TwG_PrevGadget(Gadget)		Tw_G_PrevGadget(Tw_DefaultD, Gadget)
#define TwG_NextGadget(Gadget)		Tw_G_NextGadget(Tw_DefaultD, Gadget)
#define TwGroupGadget(Gadget)		Tw_GroupGadget(Tw_DefaultD, Gadget)

#define TwFirstScreen()			Tw_FirstScreen(Tw_DefaultD)
#define TwFirstWidget(Parent)		Tw_FirstWidget(Tw_DefaultD, Parent)
#define TwFirstMsgPort()		Tw_FirstMsgPort(Tw_DefaultD)
#define TwFirstMenu(MsgPort)		Tw_FirstMenu(Tw_DefaultD, MsgPort)
#define TwFirstW(MsgPort)		Tw_FirstW(Tw_DefaultD, MsgPort)
#define TwFirstGroup(MsgPort)		Tw_FirstGroup(Tw_DefaultD, MsgPort)
#define TwFirstMutex(MsgPort)		Tw_FirstMutex(Tw_DefaultD, MsgPort)
#define TwFirstMenuItem(Menu)		Tw_FirstMenuItem(Tw_DefaultD, Menu)
#define TwFirstGadget(Group)		Tw_FirstMenu(Tw_DefaultD, Group)

#define TwGetDisplayWidth()		Tw_GetDisplayWidth(Tw_DefaultD)
#define TwGetDisplayHeight()		Tw_GetDisplayHeight(Tw_DefaultD)

#define TwCreateMsg(Type, Len)		Tw_CreateMsg(Tw_DefaultD, Type, Len)
#define TwDeleteMsg(Msg)		Tw_DeleteMsg(Tw_DefaultD, Msg)
#define TwSendMsg(MsgPort, Msg)		Tw_SendMsg(Tw_DefaultD, MsgPort, Msg)
#define TwBlindSendMsg(MsgPort, Msg)	Tw_BlindSendMsg(Tw_DefaultD, MsgPort, Msg)

#define TwGetOwnerSelection()		Tw_GetOwnerSelection(Tw_DefaultD)
#define TwSetOwnerSelection(Time, Frac) Tw_SetOwnerSelection(Tw_DefaultD, Time, Frac)
#define TwRequestSelection(Owner, ReqPrivate)	Tw_RequestSelection(Tw_DefaultD, Owner, ReqPrivate)
#define TwNotifySelection(Requestor, ReqPrivate, Magic, MIME, Len, Data) \
					Tw_NotifySelection(Tw_DefaultD, Requestor, ReqPrivate, \
							   Magic, MIME, Len, Data)

#define TwSetServerUid(uid, privileges)	Tw_SetServerUid(Tw_DefaultD, uid, privileges)

#define TwOpen(Display)			(!!(Tw_DefaultD = Tw_Open(Display)))
#define TwClose()			(Tw_Close(Tw_DefaultD), Tw_DefaultD = (tdisplay)0)
#define TwConnectionFd()		Tw_ConnectionFd(Tw_DefaultD)
#define TwLibraryVersion()		Tw_LibraryVersion(Tw_DefaultD)
#define TwServerVersion()		Tw_ServerVersion(Tw_DefaultD)
#define TwFlush()			Tw_Flush(Tw_DefaultD)
#define TwTimidFlush()			Tw_TimidFlush(Tw_DefaultD)
#define TwSync()			Tw_Sync(Tw_DefaultD)
#define TwInPanic()			Tw_InPanic(Tw_DefaultD)
#define TwPendingMsg()			Tw_PendingMsg(Tw_DefaultD)
#define TwPeekMsg()			Tw_PeekMsg(Tw_DefaultD)
#define TwReadMsg(Wait)			Tw_ReadMsg(Tw_DefaultD, Wait)
#define TwCloneReadMsg(Wait)		Tw_CloneReadMsg(Tw_DefaultD, Wait)

#define TwAddKeyboardListener(               Widget, Key,  ShiftFlags, Listener, Arg) \
       Tw_AddKeyboardListener(Tw_DefaultD,   Widget, Key,  ShiftFlags, Listener, Arg)

#define TwAddMouseListener(             Widget, Code, ShiftFlags, Listener, Arg) \
       Tw_AddMouseListener(Tw_DefaultD, Widget, Code, ShiftFlags, Listener, Arg)

#define TwAddControlListener(             Widget, Code, Listener, Arg) \
       Tw_AddControlListener(Tw_DefaultD, Widget, Code, Listener, Arg)

#define TwAddClientMsgListener(             Widget, Code, Listener, Arg) \
       Tw_AddClientMsgListener(Tw_DefaultD, Widget, Code, Listener, Arg)

#define TwAddDisplayListener(             Code, Listener, Arg) \
       Tw_AddDisplayListener(Tw_DefaultD, Code, Listener, Arg)

#define TwAddWidgetListener(             Widget, Code, Listener, Arg) \
       Tw_AddWidgetListener(Tw_DefaultD, Widget, Code, Listener, Arg)

#define TwAddGadgetListener(             Widget, Code, Listener, Arg) \
       Tw_AddGadgetListener(Tw_DefaultD, Widget, Code, Listener, Arg)

#define TwAddMenuListener(             Widget, Menu, Code, Listener, Arg) \
       Tw_AddMenuListener(Tw_DefaultD, Widget, Menu, Code, Listener, Arg)

#define TwAddSelectionListener(             Widget, Listener, Arg) \
       Tw_AddSelectionListener(Tw_DefaultD, Widget, Listener, Arg)
       
#define TwAddSelectionNotifyListener(             Listener, Arg) \
       Tw_AddSelectionNotifyListener(Tw_DefaultD, Listener, Arg)
       
#define TwAddSelectionRequestListener(             Listener, Arg) \
       Tw_AddSelectionRequestListener(Tw_DefaultD, Listener, Arg)

#define TwInsertListener(L)		Tw_InsertListener(Tw_DefaultD, L)
#define TwRemoveListener(L)		Tw_RemoveListener(Tw_DefaultD, L)
#define TwCreateListener(Type, E, L, Arg) Tw_CreateListener(Tw_DefaultD, Type, E, L, Arg)
#define TwSetTEListener(L, Type, E)	Tw_SetTEListener(Tw_DefaultD, L, Type, E)
#define TwDeleteListener(L)		Tw_DeleteListener(Tw_DefaultD, L)

#define TwSetDefaultListener(L, arg)	Tw_SetDefaultListener(Tw_DefaultD, L, arg)

#define TwDispatchMsg(Msg)		Tw_DispatchMsg(Tw_DefaultD, Msg)
#define TwMainLoop()			Tw_MainLoop(Tw_DefaultD)

#ifdef __cplusplus
 }
#endif

#endif /* _TW_H */

