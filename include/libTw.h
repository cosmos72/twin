/*
 *  libTw.h  --  main include for all libTw data types, functions and macros
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _LIB_TW_H
#define _LIB_TW_H

#ifdef __cplusplus
 extern "C" {
#endif
#if 0
 }
#endif

#if !defined(TW_CONST)
# if defined(CONST)
#  define TW_CONST CONST
# else
#  define TW_CONST const
# endif
#endif

#if !defined(TW_VOLATILE)
# if defined(VOLATILE)
#  define TW_VOLATILE VOLATILE
# else
#  define TW_VOLATILE volatile
# endif
#endif

#if !defined(TW_FNATTR_CONST)
# if defined(FNATTR_CONST)
#  define TW_FNATTR_CONST FNATTR_CONST
# elif defined(__GNUC__)
#  define TW_FNATTR_CONST __attribute__((const))
# else
#  define TW_FNATTR_CONST
# endif
#endif


#ifndef _TWIN_H
#define _TWIN_H

/* a cutdown version of twin.h */

#include <time.h>
#include <limits.h>
#include <stdlib.h>

#ifdef _AIX
# include <sys/select.h>
#endif

typedef   signed char	 num;
typedef unsigned char	byte;
typedef   signed short	 dat;
typedef unsigned short	udat;
typedef   signed int	ldat;
typedef unsigned int   uldat;

#define MAXU(t)		((t)~(t)0)
#define MINS(t)		((t)((t)1<<(8*sizeof(t)-1)))
#define MAXS(t)		((t)~MINS(t))

#define MINDAT		MINS(dat)
#define MAXDAT		MAXS(dat)
#define MAXUDAT		MAXU(udat)
#define MINLDAT		MINS(ldat)
#define MAXLDAT		MAXS(ldat)
#define MAXULDAT	MAXU(uldat)

#define NOID	((uldat)0)
#define ERRID	((uldat)-1)
#define NOFD	(-1)
#define NOPID	((pid_t)0)
#define NOSLOT	MAXULDAT

#define FALSE	((byte)0)
#define TRUE	(!FALSE)

/* "Twin" in native byte-order */
#define TWIN_MAGIC ((uldat)0x6E697754ul)
/* "Go!!" in native byte-order */
#define TW_GO_MAGIC   ((uldat)0x21216F47ul)
/* "Wait" in native byte-order */
#define TW_WAIT_MAGIC ((uldat)0x74696157ul)

#define TW_INET_PORT	7754

#define SMALLBUFF	256
#define BIGBUFF		4096


/* types for (hardware) text mode data */
typedef unsigned short hwattr;
typedef unsigned char  hwcol;
typedef unsigned char  hwfont;

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


/* hwattr <-> hwcol+hwfont conversion */
#define HWATTR(col,ascii) (((hwattr)(col)<<8) | (hwattr)(hwfont)(ascii))
#define HWCOL(attr) ((hwcol)(attr>>8))
#define HWFONT(attr) ((byte)(hwfont)(attr))






/* types for time intervals */
typedef unsigned int frac_t;
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

/* this is tricky... we don't know signedness nor sizeof(time_t) during preprocess */
#define MAX_TIME_T	( (time_t)-1 > (time_t)0 ? (time_t)-1 : \
			  (time_t)~((time_t)1 << (sizeof(time_t) * 8 - 1)) )


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

#define ANY_ACTION_MOUSE	(PRESS_ANY | RELEASE_ANY | DRAG_MOUSE)

#define MAX_ACTION_MOUSE	0x48

#define isPRESS(code)	((code) & 0x08)
#define isDRAG(code)	((code) & DRAG_MOUSE)
#define isRELEASE(code)	((code) & ANY_ACTION_MOUSE && !isPRESS(code) && !isDRAG(code))



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




/* types for messages from server */
typedef struct s_tevent_common *tevent_common;
struct s_tevent_common {
    twindow Window;
    udat Code, pad;
};

typedef struct s_tevent_keyboard *tevent_keyboard;
struct s_tevent_keyboard {
    twindow Window;
    udat Code, ShiftFlags, SeqLen;
    byte pad, AsciiSeq[1];  /* including final \0 */
};

typedef struct s_tevent_mouse *tevent_mouse;
struct s_tevent_mouse {
    twindow Window;
    udat Code, ShiftFlags;
    dat X, Y; /* these coords are relative to the window top-left corner */
};

typedef struct s_tevent_control *tevent_control;
struct s_tevent_control {
    twindow Window;
    udat Code, Len;
    dat X, Y;
    byte Data[sizeof(uldat)]; /* Data[Len] == '\0' */
};

/* some TW_MSG_CONTROL codes */
#define TW_MSG_CONTROL_QUIT		((udat)0)
#define TW_MSG_CONTROL_RESTART		((udat)1)
#define TW_MSG_CONTROL_OPEN		((udat)2)
#define TW_MSG_CONTROL_DRAGNDROP	((udat)3)

typedef struct s_tevent_clientmsg *tevent_clientmsg;
struct s_tevent_clientmsg {
    twindow Window;
    udat Code, Len;
    byte Data[sizeof(uldat)]; /* [len] bytes actually */
};

typedef struct s_tevent_display *tevent_display;
struct s_tevent_display {
    twindow Window; /* not used here */
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

typedef struct s_tevent_window *tevent_window;
struct s_tevent_window {
    twindow Window;
    udat Code, pad;
    dat XWidth, YWidth;    
};

typedef struct s_tevent_gadget *tevent_gadget;
struct s_tevent_gadget {
    twindow Window;
    udat Code, Flags; /* the Flags of the gadget that was (de)activated */
};

typedef struct s_tevent_menu *tevent_menu;
struct s_tevent_menu {
    twindow Window;
    udat Code, pad;
    tmenu Menu;
};

typedef struct s_tevent_selection *tevent_selection;
struct s_tevent_selection {
    twindow Window;
    udat Code, pad; /* unused */
    dat X, Y; /* these coords are absolute, to allow cross-window cut-n-paste */
};

#define TW_MAX_MIMELEN 64

typedef struct s_tevent_selectionnotify *tevent_selectionnotify;
struct s_tevent_selectionnotify {
    twindow Window;
    udat Code, pad; /* unused */
    uldat ReqPrivate;
    uldat Magic;
    byte MIME[TW_MAX_MIMELEN];
    uldat Len;
    byte Data[1]; /* Data[] is Len bytes actually */
};
/*SelectionNotify Magic*/
#define TW_SEL_TEXTMAGIC	((uldat)0x54657874)
#define TW_SEL_FILEMAGIC	((uldat)0x46696c65)
#define TW_SEL_URLMAGIC		((uldat)0xAB1691BA)
#define TW_SEL_DATAMAGIC	((uldat)0xDA1AA1AD) /* check MIME if you get this */
#define TW_SEL_IDMAGIC		((uldat)0x49644964)

typedef struct s_tevent_selectionrequest *tevent_selectionrequest;
struct s_tevent_selectionrequest {
    twindow Window;
    udat Code, pad; /* unused */
    tmsgport Requestor;
    uldat ReqPrivate;
};

typedef union s_tevent_any *tevent_any;
union s_tevent_any {
    struct s_tevent_common    EventCommon;
    struct s_tevent_keyboard  EventKeyboard;
    struct s_tevent_mouse     EventMouse;
    struct s_tevent_control   EventControl;
    struct s_tevent_clientmsg EventClientMsg;
    struct s_tevent_display   EventDisplay;
    struct s_tevent_window    EventWindow;
    struct s_tevent_gadget    EventGadget;
    struct s_tevent_menu      EventMenu;
    struct s_tevent_selection EventSelection;
    struct s_tevent_selectionnotify EventSelectionNotify;
    struct s_tevent_selectionrequest EventSelectionRequest;
};

typedef struct s_tmsg *tmsg;
struct s_tmsg {
    uldat Len;		/* length of this struct, including `Len' field */
    uldat Magic;	/* == msg_magic (if user-created) or == MSG_MAGIC (if received) */
    uldat Type;
    union s_tevent_any Event;
};
/* Msg Type : */
#define TW_MSG_DISPLAY		((udat)0x0FFF)

#define TW_MSG_SYSTEM_FIRST	((udat)0x1000)
#define TW_MSG_WINDOW_KEY	((udat)0x1000)
#define TW_MSG_WINDOW_MOUSE	((udat)0x1001)
#define TW_MSG_WINDOW_CHANGE	((udat)0x1002)
#define TW_MSG_WINDOW_GADGET	((udat)0x1003)
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
struct s_tlistener {
    uldat Type;
    tlistener Prev, Next;
    void *Arg;
    tfn_listener Listener;
    tevent_any Event;
    tdisplay TwD;
};


     


     
     











/* Gadget Flags : */
#define TW_GADGET_DISABLED	((udat)0x02)
#define TW_GADGET_USE_DEFCOL	TW_WINFL_USE_DEFCOL
#define TW_GADGET_TOGGLE	((udat)0x08)
#define TW_GADGET_PRESSED	((udat)0x0800)

/* Row Flags : */
#define TW_ROW_INACTIVE	((byte)0x00)
#define TW_ROW_ACTIVE	((byte)0x01)
#define TW_ROW_IGNORE	((byte)0x02)

/* Window Attrib : */
#define TW_WINDOW_WANT_KEYS	((uldat)0x0002)
#define TW_WINDOW_WANT_MOUSE	((uldat)0x0004)
#define TW_WINDOW_WANT_CHANGES	((uldat)0x0008)
#define TW_WINDOW_DRAG		((uldat)0x0010)
#define TW_WINDOW_RESIZE	((uldat)0x0020)
#define TW_WINDOW_CLOSE		((uldat)0x0040)
#define TW_WINDOW_ROLLED_UP	((uldat)0x0080)
#define TW_WINDOW_X_BAR		((uldat)0x0100)
#define TW_WINDOW_Y_BAR		((uldat)0x0200)

/* Window Flags: */
#define TW_WINFL_USEROWS	((byte)0x00) /* not needed, it's the default */
#define TW_WINFL_USECONTENTS	((byte)0x01)
#define TW_WINFL_BYUSER		((byte)0x02) /* not yet supported */

#define TW_WINFL_USE_DEFCOL	((byte)0x04)
#define	TW_WINFL_CURSOR_ON	((byte)0x08)
#define TW_WINFL_INSERT		((byte)0x10)
#define TW_WINFL_SEL_ROWCURR	((byte)0x20)
#define TW_WINFL_DISABLED	((byte)0x40)
#define TW_WINFL_BORDERLESS	((byte)0x80)

/* Window CursorType : */
/* These come from linux/drivers/char/console.c */
#define TW_DEFAULTCURSOR 0
#define TW_NOCURSOR	 1
#define TW_LINECURSOR	 2
#define TW_SOLIDCURSOR	 8





void Tw_ConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *));
		     
extern void *(*Tw_AllocMem)(size_t);
extern void *(*Tw_ReAllocMem)(void *, size_t);
extern void  (*Tw_FreeMem)(void *);
extern byte *(*Tw_CloneStr)(TW_CONST byte *);


#define Tw_LenStr(S) strlen(S)
#define Tw_CmpStr(S1, S2) strcmp((S1), (S2))
#define Tw_CopyStr(From,To) strcpy((To),(From))

#define Tw_CopyMem(From, To, Size)	memcpy((To), (From), (size_t)(Size))
#define Tw_MoveMem(From, To, Size)	memmove((To), (From), (size_t)(Size))
#define Tw_WriteMem(Mem, Char, Size)	memset((Mem), (int)(Char), (size_t)(Size))
#define Tw_CmpMem(m1, m2, Size)		memcmp((m1), (m2), (size_t)(Size))


#define Tw_DropPrivileges() (setegid(getgid()), seteuid(getuid()))
#define Tw_GetPrivileges() seteuid(0)








uldat Tw_FindFunction(tdisplay TwD, byte Len, TW_CONST byte *Name);
byte  Tw_FindFunctions(tdisplay TwD, void *Function, ...);

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

void	Tw_DeleteObj(tdisplay TwD, tobj Obj);

twidget Tw_CreateWidget(tdisplay TwD, tmsgport Owner, dat XWidth, dat YWidth,
			hwattr Bg, dat Left, dat Up);
void	Tw_DeleteWidget(tdisplay TwD, twidget Widget);
void	Tw_RecursiveDeleteWidget(tdisplay TwD, twidget Widget);
void	Tw_MapWidget(tdisplay TwD, twidget Widget, twidget Parent);
void	Tw_UnMapWidget(tdisplay TwD, twidget Widget);
void	Tw_SetXYWidget(tdisplay TwD, twidget Widget, dat X, dat Y);
tmsgport Tw_GetOwnerWidget(tdisplay TwD, twidget Widget);
twidget Tw_FindWidgetAtWidget(tdisplay TwD, twidget Parent, dat i, dat j);

tgroup Tw_CreateGroup(tdisplay TwD, tmsgport);
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
	  TW_CONST byte *TextNormal, udat Code, udat Flags,
	  hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
	  dat Left, dat Up, 
	  TW_CONST byte *TextSelect, TW_CONST byte *TextDisabled, TW_CONST byte *TextSelectDisabled,
	  TW_CONST hwcol *ColNormal,
	  TW_CONST hwcol *ColSelect, TW_CONST hwcol *ColDisabled, TW_CONST hwcol *ColSelectDisabled);
void	Tw_DeleteGadget(tdisplay TwD, tgadget Gadget);
void	Tw_RecursiveDeleteGadget(tdisplay TwD, tgadget Gadget);
void	Tw_MapGadget(tdisplay TwD, tgadget Gadget, twidget Parent);
void	Tw_UnMapGadget(tdisplay TwD, tgadget Gadget);
void	Tw_SetXYGadget(tdisplay TwD, tgadget Gadget, dat X, dat Y);
tmsgport Tw_GetOwnerGadget(tdisplay TwD, tgadget Gadget);
tgadget Tw_CreateButtonGadget(tdisplay TwD, twidget Parent, dat XWidth, dat YWidth, TW_CONST byte *Text,
			      udat Code, udat Flags, hwcol BgCol, hwcol Col, hwcol ColDisabled,
			      dat Left, dat Up);
void	Tw_SetPressedGadget(tdisplay TwD, tgadget Gadget, byte on);
byte    Tw_IsPressedGadget(tdisplay TwD, tgadget Gadget);
void	Tw_SetToggleGadget(tdisplay TwD, tgadget Gadget, byte on);
byte    Tw_IsToggleGadget(tdisplay TwD, tgadget Gadget);
void    Tw_WriteTextGadget(tdisplay TwD, tgadget Gadget, dat XWidth, dat YWidth, TW_CONST byte *Text, dat Left, dat Up);
void    Tw_SetTextGadget(tdisplay TwD, tgadget Gadget, dat XWidth, dat YWidth, TW_CONST byte *Text, dat Left, dat Up);

void	Tw_Create4MenuRow(tdisplay TwD, twindow Window, udat Code, byte FlagActive, ldat TextLen, TW_CONST byte *Text);
#define Tw_Row4Menu Tw_Create4MenuRow

twindow Tw_CreateWindow(tdisplay TwD, dat TitleLen, TW_CONST byte *Title, TW_CONST hwcol *ColTitle, tmenu Menu,
		       hwcol ColText, uldat cursorType, uldat Attrib, byte Flags,
		       dat XWidth, dat YWidth, dat ScrollBackLines);
void	Tw_DeleteWindow(tdisplay TwD, twindow Window);
void	Tw_RecursiveDeleteWindow(tdisplay TwD, twindow Window);
twindow Tw_Create4MenuWindow(tdisplay TwD, tmenu Menu);
#define Tw_Win4Menu Tw_Create4MenuWindow
void	Tw_MapWindow(tdisplay TwD, twindow Window, twidget Parent);
void	Tw_UnMapWindow(tdisplay TwD, twindow Window);
void	Tw_SetXYWindow(tdisplay TwD, twindow Window, dat X, dat Y);
tmsgport Tw_GetOwnerWindow(tdisplay TwD, twindow Window);
void	Tw_WriteAsciiWindow(tdisplay TwD, twindow Window, ldat AsciiLen, TW_CONST byte *AsciiSeq);
void    Tw_WriteHWAttrWindow(tdisplay TwD, twindow Window, dat x, dat y, ldat Len, TW_CONST hwattr *Attr);
void	Tw_WriteRowWindow(tdisplay TwD, twindow Window, ldat Len, TW_CONST byte *Text);
void	Tw_SetColTextWindow(tdisplay TwD, twindow Window, hwcol ColText);
void	Tw_SetColorsWindow(tdisplay TwD, twindow Window, udat Bitmap, hwcol ColGadgets, hwcol ColArrows, hwcol ColBars, hwcol ColTabs,
			  hwcol ColBorder, hwcol ColText, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled);
void	Tw_ConfigureWindow(tdisplay TwD, twindow Window, byte Bitmap, dat Left, dat Up, dat MinXWidth, dat MinYWidth, dat MaxXWidth, dat MaxYWidth);
void	Tw_ResizeWindow(tdisplay TwD, twindow Window, dat XWidth, dat YWidth);
void	Tw_GotoXYWindow(tdisplay TwD, twindow Window, ldat X, ldat Y);

tmenuitem Tw_Create4MenuMenuItem(tdisplay TwD, tmenu Menu, twindow Window, byte Flags, dat NameLen, TW_CONST byte *Name);
#define   Tw_Item4Menu Tw_Create4MenuMenuItem
uldat     Tw_Create4MenuCommonMenuItem(tdisplay TwD, tmenu Menu);
#define   Tw_Item4MenuCommon Tw_Create4MenuCommonMenuItem
void	Tw_DeleteMenuItem(tdisplay TwD, tmenuitem MenuItem);

tmenu	Tw_CreateMenu(tdisplay TwD, tmsgport MsgPort, hwcol ColItem, hwcol ColSelect, hwcol ColDisabled,
		     hwcol ColSelectDisabled, hwcol ColShtCut, hwcol ColSelShtCut, byte FlagDefColInfo);
void	Tw_SetInfoMenu(tdisplay TwD, tmenu Menu, byte Flags, ldat Len, TW_CONST byte *Text, TW_CONST hwcol *ColText);
#define Tw_Info4Menu Tw_SetInfoMenu
void	Tw_DeleteMenu(tdisplay TwD, tmenu Menu);

tmsgport Tw_CreateMsgPort(tdisplay TwD, byte NameLen, TW_CONST byte *ProgramName, time_t PauseSec, frac_t PauseFraction, byte WakeUp);
/* WakeUp: */
#define TW_TIMER_ALWAYS	((byte)1)
#define TW_TIMER_ONCE	((byte)2)
void	Tw_DeleteMsgPort(tdisplay TwD, tmsgport MsgPort);
tmsgport Tw_FindMsgPort(tdisplay TwD, tmsgport Prev, byte NameLen, TW_CONST byte *ProgramName);
     
void	Tw_BgImageScreen(tdisplay TwD, tscreen Screen, dat BgWidth, dat BgHeight, TW_CONST hwattr *BgImage);

tscreen	Tw_FirstScreen(tdisplay TwD);
twidget	Tw_FirstWidget(tdisplay TwD, twidget Parent);
tmsgport Tw_FirstMsgPort(tdisplay TwD);
tmenu    Tw_FirstMenu(tdisplay TwD, tmsgport MsgPort);
tmenuitem Tw_FirstMenuItem(tdisplay TwD, tmenu Menu);

tobj Tw_PrevObj(tdisplay TwD, tobj Obj);
tobj Tw_NextObj(tdisplay TwD, tobj Obj);
tobj Tw_ParentObj(tdisplay TwD, tobj Obj);

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



tlistener Tw_AddKeyboardListener(tdisplay TwD, twindow Window, udat Key,  udat ShiftFlags, tfn_listener Listener, void *Arg);
tlistener Tw_AddMouseListener(   tdisplay TwD, twindow Window, udat Code, udat ShiftFlags, tfn_listener Listener, void *Arg);

tlistener Tw_AddControlListener(  tdisplay TwD, twindow Window, udat Code, tfn_listener Listener, void *Arg);
tlistener Tw_AddClientMsgListener(tdisplay TwD, twindow Window, udat Code, tfn_listener Listener, void *Arg);
tlistener Tw_AddDisplayListener(  tdisplay TwD,                 udat Code, tfn_listener Listener, void *Arg);
tlistener Tw_AddWindowListener(   tdisplay TwD, twindow Window, udat Code, tfn_listener Listener, void *Arg);
tlistener Tw_AddGadgetListener(   tdisplay TwD, twindow Window, udat Code, tfn_listener Listener, void *Arg);

tlistener Tw_AddMenuListener(tdisplay TwD, twindow Window, tmenu Menu, udat Code, tfn_listener Listener, void *Arg);

tlistener Tw_AddSelectionListener(tdisplay TwD, twindow Window, tfn_listener Listener, void *Arg);
tlistener Tw_AddSelectionNotifyListener( tdisplay TwD,          tfn_listener Listener, void *Arg);
tlistener Tw_AddSelectionRequestListener(tdisplay TwD,          tfn_listener Listener, void *Arg);

void Tw_InsertListener(tdisplay TwD, tlistener L);
void Tw_RemoveListener(tdisplay TwD, tlistener L);
void Tw_DeleteListener(tdisplay TwD, tlistener L);

void Tw_SetDefaultListener(tdisplay TwD, tfn_default_listener listener, void *arg);

byte  Tw_DispatchMsg(tdisplay TwD, tmsg Msg);
uldat Tw_MainLoop(tdisplay TwD);





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

#define TwConfigMalloc(my_malloc, my_realloc, my_free) Tw_ConfigMalloc(my_malloc, my_realloc, my_free)

#define TwLenStr(S)			Tw_LenStr(S)
#define TwCmpStr(S1, S2)		Tw_CmpStr(S1, S2)
#define TwCopyStr(From,To)		Tw_CopyStr(From, To)

#define TwAllocMem(Size)		Tw_AllocMem(Size)
#define TwReAllocMem(Mem, Size)		Tw_ReAllocMem(Mem, Size)
#define TwFreeMem(Mem)			Tw_FreeMem(Mem)
#define TwCloneStr(S)			Tw_CloneStr(S)

#define TwCopyMem(From, To, Size)	Tw_CopyMem(From, To, Size)
#define TwMoveMem(From, To, Size)	Tw_MoveMem(From, To, Size)
#define TwWriteMem(Mem, Char, Size)	Tw_WriteMem(Mem, Char, Size)
#define TwCmpMem(m1, m2, Size)		Tw_CmpMem(m1, m2, Size)

#define TwDropPrivileges()		Tw_DropPrivileges()
#define TwGetPrivileges()		Tw_GetPrivileges()

#define TwFindFunction(Len, Name)	Tw_FindFunction(Tw_DefaultD, Len, Name)
#define TwEnableGzip()			Tw_EnableGzip(Tw_DefaultD)
#define TwDisableGzip()			Tw_DisableGzip(Tw_DefaultD)
#define TwNeedResizeDisplay()		Tw_NeedResizeDisplay(Tw_DefaultD)
#define TwAttachHW(len, name, flags)	Tw_AttachHW(Tw_DefaultD, len, name, flags)
#define TwAttachGetReply(len)		Tw_AttachGetReply(Tw_DefaultD, len)
#define TwAttachConfirm()		Tw_AttachConfirm(Tw_DefaultD)
#define TwDetachHW(len, name)		Tw_DetachHW(Tw_DefaultD, len, name)
#define TwSetFontTranslation(trans)	Tw_SetFontTranslation(Tw_DefaultD, trans)

#define TwDeleteObj(O)			Tw_DeleteObj(Tw_DefaultD, O)

#define TwCreateWidget(Owner, XWidth, YWidth, Bg, Left, Up) \
					Tw_CreateWidget(Tw_DefaultD, Owner, XWidth, YWidth, Bg, Left, Up)
#define TwDeleteWidget(W)		Tw_DeleteWidget(Tw_DefaultD, W)
#define TwRecursiveDeleteWidget(W)	Tw_RecursiveDeleteWidget(Tw_DefaultD, W)
#define TwMapWidget(W, Parent)		Tw_MapWidget(Tw_DefaultD, W, Parent)
#define TwUnMapWidget(W)		Tw_UnMapWidget(Tw_DefaultD, W)
#define TwSetXYWidget(W, x, y)		Tw_SetXYWidget(Tw_DefaultD, W, x, y)
#define TwGetOwnerWidget(W)		Tw_GetOwnerWidget(Tw_DefaultD, W)
#define TwFindWidgetAtWidget(W, i, j)	Tw_FindWidgetAtWidget(Tw_DefaultD, W, i, j)

#define TwCreateGroup(MsgPort)		Tw_CreateGroup(Tw_DefaultD, MsgPort)
#define TwInsertGadgetGroup(Group, G)	Tw_InsertGadgetGroup(Tw_DefaultD, Group, G)
#define TwRemoveGadgetGroup(Group, G)	Tw_RemoveGadgetGroup(Tw_DefaultD, Group, G)
#define TwGetSelectedGadgetGroup(Group) Tw_GetSelectedGadgetGroup(Tw_DefaultD, Group)
#define TwSetSelectedGadgetGroup(Group, G) Tw_SetSelectedGadgetGroup(Tw_DefaultD, Group, G)

#define TwCreateGadget( Parent, XWidth, YWidth, TextNormal, Code, Flags, \
     ColText, ColTextSelect, ColTextDisabled, ColTextSelectDisabled, \
     Left, Up, TextSelect, TextDisabled, TextSelectDisabled, \
     ColNormal, ColSelect, ColDisabled, ColSelectDisabled) \
  Tw_CreateGadget(Tw_DefaultD, Parent, XWidth, YWidth, TextNormal, Code, Flags, \
     ColText, ColTextSelect, ColTextDisabled, ColTextSelectDisabled, \
     Left, Up, TextSelect, TextDisabled, TextSelectDisabled, \
     ColNormal, ColSelect, ColDisabled, ColSelectDisabled)
#define TwDeleteGadget(Gadget)		Tw_DeleteGadget(Tw_DefaultD, Gadget)
#define TwMapGadget(G, Parent)		Tw_MapGadget(Tw_DefaultD, G, Parent)
#define TwUnMapGadget(G)		Tw_UnMapGadget(Tw_DefaultD, G)
#define TwSetXYGadget(W, x, y)		Tw_SetXYGadget(Tw_DefaultD, W, x, y)
#define TwGetOwnerGadget(G)		Tw_GetOwnerGadget(Tw_DefaultD, G)

#define TwCreateButtonGadget(Window, BgCol, Col, ColDisabled, Code, Flags, \
				 Left, Up, XWidth, Width, Text) \
		Tw_CreateButtonGadget(Tw_DefaultD, \
			     Window, BgCol, Col, ColDisabled, Code, Flags, \
			     Left, Up, XWidth, YWidth, Text)

#define TwSetPressedGadget(Gadget, on)	Tw_SetPressedGadget(Tw_DefaultD, Gadget, on)
#define TwIsPressedGadget(Gadget)	Tw_IsPressedGadget(Tw_DefaultD, Gadget)
#define TwSetToggleGadget(Gadget, on)	Tw_SetToggleGadget(Tw_DefaultD, Gadget, on)
#define TwIsToggleGadget(Gadget)	Tw_IsToggleGadget(Tw_DefaultD, Gadget)
#define TwWriteTextGadget(Gadget, X, Y, Text, Left, Up) \
					Tw_WriteTextGadget(Tw_DefaultD, Gadget, X, Y, Text, Left, Up)
#define TwSetTextGadget(Gadget, X, Y, Text, Left, Up) \
					Tw_SetTextGadget(Tw_DefaultD, Gadget, X, Y, Text, Left, Up)


#define TwCreate4MenuRow(Window, Code, FlagActive, TextLen, Text) \
		Tw_Create4MenuRow(Tw_DefaultD, Window, Code, FlagActive, TextLen, Text)
#define TwRow4Menu			TwCreate4MenuRow
#define TwCreateWindow(TitleLen, Title, ColTitle, Menu, ColText, cursorType, \
		       Attrib, Flags, XWidth, YWidth, ScrollBackLines) \
		Tw_CreateWindow(Tw_DefaultD, TitleLen, Title, ColTitle, Menu, ColText, cursorType, \
		       Attrib, Flags, XWidth, YWidth, ScrollBackLines)
#define TwDeleteWindow(Window)		Tw_DeleteWindow(Tw_DefaultD, Window)
#define TwCreate4MenuWindow(Menu)	Tw_Create4MenuWindow(Tw_DefaultD, Menu)
#define TwWin4Menu			TwCreate4MenuWindow
#define TwMapWindow(Window, Parent)	Tw_MapWindow(Tw_DefaultD, Window, Parent)
#define TwUnMapWindow(Window)		Tw_UnMapWindow(Tw_DefaultD, Window)
#define TwSetXYWindow(W, x, y)		Tw_SetXYWindow(Tw_DefaultD, W, x, y)
#define TwGetOwnerWindow(W)		Tw_GetOwnerWindow(Tw_DefaultD, W)
#define TwWriteAsciiWindow(Window, AsciiLen, AsciiSeq) \
		Tw_WriteAsciiWindow(Tw_DefaultD, Window, AsciiLen, AsciiSeq)
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

#define TwFindWidgetAtWidget(Widget, X, Y) Tw_FindWidgetAtWidget(Tw_DefaultD, Widget, X, Y)

#define TwCreate4MenuMenuItem(Menu, Window, Flags, NameLen, Name) \
		Tw_Create4MenuMenuItem(Tw_DefaultD, Menu, Window, Flags, NameLen, Name)
#define TwItem4Menu			TwCreate4MenuMenuItem
#define TwCreate4MenuCommonMenuItem(Menu) Tw_Create4MenuCommonMenuItem(Tw_DefaultD, Menu)
#define TwItem4MenuCommon		TwCreate4MenuCommonMenuItem
#define TwDeleteMenuItem(MenuItem)	Tw_DeleteMenuItem(Tw_DefaultD, MenuItem)
#define TwCreateMenu(MsgPort, ColItem, ColSelect, ColDisabled, \
		     ColSelectDisabled, ColShtCut, ColSelShtCut, FlagDefColInfo) \
		Tw_CreateMenu(Tw_DefaultD, MsgPort, ColItem, ColSelect, ColDisabled, \
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
#define TwFirstScreen()			Tw_FirstScreen(Tw_DefaultD)
#define TwFirstWidget(Parent)		Tw_FirstWidget(Tw_DefaultD, Parent)
#define TwFirstMsgPort()		Tw_FirstMsgPort(Tw_DefaultD)
#define TwFirstMenu(MsgPort)		Tw_FirstMenu(Tw_DefaultD, MsgPort)
#define TwFirstMenuItem(Menu)		Tw_FirstMenuItem(Tw_DefaultD, Menu)
#define TwPrevObj(Obj)			Tw_PrevObj(Tw_DefaultD, Obj)
#define TwNextObj(Obj)			Tw_NextObj(Tw_DefaultD, Obj)
#define TwParentObj(Obj)		Tw_ParentObj(Tw_DefaultD, Obj)
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

#define TwAddKeyboardListener(               Window, Key,  ShiftFlags, Listener, Arg) \
       Tw_AddKeyboardListener(Tw_DefaultD,   Window, Key,  ShiftFlags, Listener, Arg)

#define TwAddMouseListener(             Window, Code, ShiftFlags, Listener, Arg) \
       Tw_AddMouseListener(Tw_DefaultD, Window, Code, ShiftFlags, Listener, Arg)

#define TwAddControlListener(             Window, Code, Listener, Arg) \
       Tw_AddControlListener(Tw_DefaultD, Window, Code, Listener, Arg)

#define TwAddClientMsgListener(             Window, Code, Listener, Arg) \
       Tw_AddClientMsgListener(Tw_DefaultD, Window, Code, Listener, Arg)

#define TwAddDisplayListener(             Code, Listener, Arg) \
       Tw_AddDisplayListener(Tw_DefaultD, Code, Listener, Arg)

#define TwAddWindowListener(             Window, Code, Listener, Arg) \
       Tw_AddWindowListener(Tw_DefaultD, Window, Code, Listener, Arg)

#define TwAddGadgetListener(             Window, Code, Listener, Arg) \
       Tw_AddGadgetListener(Tw_DefaultD, Window, Code, Listener, Arg)

#define TwAddMenuListener(             Window, Menu, Code, Listener, Arg) \
       Tw_AddMenuListener(Tw_DefaultD, Window, Menu, Code, Listener, Arg)

#define TwAddSelectionListener(             Window, Listener, Arg) \
       Tw_AddSelectionListener(Tw_DefaultD, Window, Listener, Arg)
       
#define TwAddSelectionNotifyListener(             Listener, Arg) \
       Tw_AddSelectionNotifyListener(Tw_DefaultD, Listener, Arg)
       
#define TwAddSelectionRequestListener(             Listener, Arg) \
       Tw_AddSelectionRequestListener(Tw_DefaultD, Listener, Arg)

#define TwInsertListener(L)		Tw_InsertListener(Tw_DefaultD, L)
#define TwRemoveListener(L)		Tw_RemoveListener(Tw_DefaultD, L)
#define TwDeleteListener(L)		Tw_DeleteListener(Tw_DefaultD, L)

#define TwSetDefaultListener(L, arg)	Tw_SetDefaultListener(Tw_DefaultD, L, arg)

#define TwDispatchMsg(Msg)		Tw_DispatchMsg(Tw_DefaultD, Msg)
#define TwMainLoop()			Tw_MainLoop(Tw_DefaultD)

#ifdef __cplusplus
 }
#endif

#endif /* _LIB_TW_H */

