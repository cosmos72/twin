/*
 *  libTw.h  --  main include for all libTw data types, functions and macros
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _LIB_TW_H
#define _LIB_TW_H

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
typedef struct timevalue  {
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




/* mouse events stuff */
#define HOLD_LEFT	((udat)1)
#define HOLD_MIDDLE	((udat)2)
#define HOLD_RIGHT	((udat)4)
#define HOLD_ANY	(HOLD_LEFT|HOLD_MIDDLE|HOLD_RIGHT)

#define PRESS_LEFT	((udat)0x08)
#define PRESS_RIGHT	((udat)0x18)
#define PRESS_MIDDLE	((udat)0x28)
#define PRESS_ANY	((udat)0x38)

#define DOWN_LEFT	(HOLD_LEFT|PRESS_LEFT)
#define DOWN_RIGHT	(HOLD_RIGHT|PRESS_RIGHT)
#define DOWN_MIDDLE	(HOLD_MIDDLE|PRESS_MIDDLE)
#define DOWN_ANY	(HOLD_ANY|PRESS_ANY)

#define RELEASE_LEFT	((udat)0x10)
#define RELEASE_RIGHT	((udat)0x20)
#define RELEASE_MIDDLE	((udat)0x30)
#define RELEASE_ANY	((udat)0x30)

#define DRAG_MOUSE	((udat)0x40)

#define ANY_ACTION_MOUSE	(PRESS_ANY | RELEASE_ANY | DRAG_MOUSE)

#define MAX_ESEGUI_MOUSE	(udat)0x48

#define isPRESS(code)	((code) & 0x08)
#define isDRAG(code)	((code) & DRAG_MOUSE)
#define isRELEASE(code)	((code) & ANY_ACTION_MOUSE && !isPRESS(code) && !isDRAG(code))






#define gadget_magic	((uldat)0x59867551ul)
#define window_magic	((uldat)0x61357531ul)
#define menuitem_magic	((uldat)0x7abc8fdeul)
#define menu_magic	((uldat)0x8bad0bedul)
#define screen_magic	((uldat)0x92659871ul)
#define msg_magic	((uldat)0xA3a61ce4ul)
#define msgport_magic	((uldat)0xB0981437ul)
#define mutex_magic	((uldat)0xC0faded0ul)


#endif /* _TWIN_H */




typedef uldat tobj;
typedef uldat tgadget;
typedef uldat trow;
typedef uldat twindow;
typedef uldat tmenuitem;
typedef uldat tmenu;
typedef uldat tscreen;
typedef uldat tmsgport;




/* types for messages from server */
typedef struct tevent_common *tevent_common;
struct tevent_common {
    twindow Window;
    udat Code, pad;
};

typedef struct tevent_keyboard *tevent_keyboard;
struct tevent_keyboard {
    twindow Window;
    udat Code, ShiftFlags, SeqLen;
    byte pad, AsciiSeq[1];  /* including final \0 */
};

typedef struct tevent_mouse *tevent_mouse;
struct tevent_mouse {
    twindow Window;
    udat Code, ShiftFlags;
    dat X, Y; /* these coords are relative to the window top-left corner */
};

typedef struct tevent_display *tevent_display;
struct tevent_display {
    twindow Window; /* not used here */
    udat Code, Len;
    dat X, Y;
    byte Data[1]; /* [len] bytes actually */
};

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

typedef struct tevent_window *tevent_window;
struct tevent_window {
    twindow Window;
    udat Code, pad;
    udat XWidth, YWidth;    
};

typedef struct tevent_gadget *tevent_gadget;
struct tevent_gadget {
    twindow Window;
    udat Code, pad;
};

typedef struct tevent_menu *tevent_menu;
struct tevent_menu {
    twindow Window;
    udat Code, pad;
    tmenu Menu;
};

typedef struct tevent_selection *tevent_selection;
struct tevent_selection {
    twindow Window;
    udat Code, pad; /* unused */
    dat X, Y; /* these coords are absolute, to allow cross-window cut-n-paste */
};

#define TW_MAX_MIMELEN 64

typedef struct tevent_selectionnotify *tevent_selectionnotify;
struct tevent_selectionnotify {
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

typedef struct tevent_selectionrequest *tevent_selectionrequest;
struct tevent_selectionrequest {
    twindow Window;
    udat Code, pad; /* unused */
    tmsgport Requestor;
    uldat ReqPrivate;
};

typedef union tevent_any *tevent_any;
union tevent_any {
    struct tevent_common    EventCommon;
    struct tevent_keyboard  EventKeyboard;
    struct tevent_mouse     EventMouse;
    struct tevent_display   EventDisplay;
    struct tevent_window    EventWindow;
    struct tevent_gadget    EventGadget;
    struct tevent_menu      EventMenu;
    struct tevent_selection EventSelection;
    struct tevent_selectionnotify EventSelectionNotify;
    struct tevent_selectionrequest EventSelectionRequest;
};

typedef struct tmsg *tmsg;
struct tmsg {
    uldat Len;		/* length of this struct, including `Len' field */
    uldat Magic;	/* == msg_magic */
    uldat Type;
    union tevent_any Event;
};
/* Msg Type : */
#define TW_MSG_DISPLAY		((udat)0x0FFF)
#define TW_MSG_WINDOW_KEY	((udat)0x1000)
#define TW_MSG_WINDOW_MOUSE	((udat)0x1001)
#define TW_MSG_WINDOW_CHANGE	((udat)0x1002)
#define TW_MSG_WINDOW_GADGET	((udat)0x1003)
#define TW_MSG_MENU_ROW		((udat)0x1004)
#define TW_MSG_SELECTION	((udat)0x1005)
#define TW_MSG_SELECTIONNOTIFY	((udat)0x1006)
#define TW_MSG_SELECTIONREQUEST	((udat)0x1007)
#define TW_MSG_SELECTIONCLEAR	((udat)0x1008)


typedef struct s_tw_d *tdisplay;
















/* Gadget Flags : */
#define TW_GADGET_DISABLED	((udat)0x02)

/* Row Flags : */
#define TW_ROW_INACTIVE	((byte)0x00)
#define TW_ROW_ACTIVE	((byte)0x01)
#define TW_ROW_IGNORE	((byte)0x02)

/* Window Attrib : */
#define TW_WINDOW_MENU		((uldat)0x0001)
#define TW_WINDOW_WANT_KEYS	((uldat)0x0002)
#define TW_WINDOW_WANT_MOUSE	((uldat)0x0004)
#define TW_WINDOW_WANT_CHANGES	((uldat)0x0008)
#define TW_WINDOW_DRAG		((uldat)0x0010)
#define TW_WINDOW_RESIZE	((uldat)0x0020)
#define TW_WINDOW_CLOSE		((uldat)0x0040)
#define TW_WINDOW_STAYONTOP	((uldat)0x0080)
#define TW_WINDOW_X_BAR		((uldat)0x0100)
#define TW_WINDOW_Y_BAR		((uldat)0x0200)

/* Window Flags: */
#define TW_WINFL_USEROWS	((byte)0x00) /* it's the default */
#define TW_WINFL_USECONTENTS	((byte)0x01)
#define TW_WINFL_BYUSER		((byte)0x02)

#define TW_WINFL_USE_DEFCOL	((byte)0x04)
#define	TW_WINFL_CURSOR_ON	((byte)0x08)
#define TW_WINFL_INSERT		((byte)0x10)
#define TW_WINFL_SEL_ROWCURR	((byte)0x20)
#define TW_WINFL_DISABLED	((byte)0x40)

/* Window CursorType : */
/* These come from linux/drivers/char/console.c */
#define TW_NOCURSOR	1
#define TW_LINECURSOR	2
#define TW_SOLIDCURSOR	8




void Tw_ConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *));
		     
extern void *(*Tw_AllocMem)(size_t);
extern void *(*Tw_ReAllocMem)(void *, size_t);
extern void  (*Tw_FreeMem)(void *);
extern byte *(*Tw_CloneStr)(byte *);


#define Tw_LenStr(S) strlen(S)
#define Tw_CmpStr(S1, S2) strcmp((S1), (S2))
#define Tw_CopyStr(From,To) strcpy((To),(From))

#define Tw_CopyMem(From, To, Size)	memcpy((To), (From), (size_t)(Size))
#define Tw_MoveMem(From, To, Size)	memmove((To), (From), (size_t)(Size))
#define Tw_WriteMem(Mem, Char, Size)	memset((Mem), (int)(Char), (size_t)(Size))
#define Tw_CmpMem(m1, m2, Size)		memcmp((m1), (m2), (size_t)(Size))


#define Tw_DropPrivileges() (setegid(getgid()), seteuid(getuid()))
#define Tw_GetPrivileges() seteuid(0)








uldat Tw_FindFunction(tdisplay TwD, byte Len, byte *Name);
byte  Tw_FindFunctions(tdisplay TwD, void *Function, ...);

byte Tw_EnableGzip(tdisplay TwD);
byte Tw_DisableGzip(tdisplay TwD);

void Tw_NeedResizeDisplay(tdisplay TwD);

void Tw_AttachHW(tdisplay TwD, uldat len, byte *name, byte redirect);
byte *Tw_AttachGetReply(tdisplay TwD, uldat *len);
void Tw_AttachConfirm(tdisplay TwD);

byte Tw_DetachHW(tdisplay TwD, uldat len, byte *name);

void Tw_SetFontTranslation(tdisplay TwD, byte trans[0x80]);

tgadget Tw_CreateGadget(tdisplay TwD, twindow Window,
		       hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
		       udat Code, udat Flags, udat Left, udat Up, udat XWidth, udat YWidth,
		       byte *TextNormal, byte *TextSelect, byte *TextDisabled, byte *TextSelectDisabled,
		       hwcol *ColNormal, hwcol *ColSelect, hwcol *ColDisabled, hwcol *ColSelectDisabled);
void    Tw_CopyGadget(tdisplay TwD, tgadget From, tgadget To);
void	Tw_DeleteGadget(tdisplay TwD, tgadget Gadget);
tgadget Tw_SearchGadget(tdisplay TwD, twindow Window, dat i, dat j);
tgadget Tw_CreateButtonGadget(tdisplay TwD, twindow Window,
			      hwcol BgCol, hwcol Col, hwcol ColDisabled,
			      udat Code, udat Flags, udat Left, udat Up, udat XWidth, udat YWidth,
			      byte *Text);

void	Tw_Create4MenuRow(tdisplay TwD, twindow Window, udat Code, byte FlagActive, uldat TextLen, byte *Text);
#define Tw_Row4Menu Tw_Create4MenuRow

twindow Tw_CreateWindow(tdisplay TwD, udat TitleLen, byte *Title, hwcol *ColTitle, tmenu Menu,
		       hwcol ColText, uldat cursorType, uldat Attrib, byte Flags,
		       udat XWidth, udat YWidth, udat ScrollBackLines);
void	Tw_DeleteWindow(tdisplay TwD, twindow Window);
twindow Tw_Create4MenuWindow(tdisplay TwD, tmenu Menu);
#define Tw_Win4Menu Tw_Create4MenuWindow
void	Tw_MapWindow(tdisplay TwD, twindow Window, tscreen Screen);
void	Tw_UnMapWindow(tdisplay TwD, twindow Window);
void	Tw_WriteAsciiWindow(tdisplay TwD, twindow Window, uldat AsciiLen, byte *AsciiSeq);
void    Tw_WriteHWAttrWindow(tdisplay TwD, twindow Window, udat x, udat y, uldat Len, hwattr *Attr);
void	Tw_WriteRowWindow(tdisplay TwD, twindow Window, uldat Len, byte *Text);
void	Tw_SetColTextWindow(tdisplay TwD, twindow Window, hwcol ColText);
void	Tw_SetColorsWindow(tdisplay TwD, twindow Window, udat Bitmap, hwcol ColGadgets, hwcol ColArrows, hwcol ColBars, hwcol ColTabs,
			  hwcol ColBorder, hwcol ColText, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled);
void	Tw_ConfigureWindow(tdisplay TwD, twindow Window, byte Bitmap, dat Left, udat Up, udat MinXWidth, udat MinYWidth, udat MaxXWidth, udat MaxYWidth);
void	Tw_ResizeWindow(tdisplay TwD, twindow Window, udat XWidth, udat YWidth);
void	Tw_GotoXYWindow(tdisplay TwD, twindow Window, uldat X, uldat Y);
tgadget Tw_SearchGadgetWindow(tdisplay TwD, twindow Window, dat X, dat Y);

tmenuitem Tw_Create4MenuMenuItem(tdisplay TwD, tmenu Menu, twindow Window, byte Flags, udat NameLen, byte *Name);
#define   Tw_Item4Menu Tw_Create4MenuMenuItem
tmenuitem Tw_Create4MenuCommonMenuItem(tdisplay TwD, tmenu Menu);
#define   Tw_Item4MenuCommon Tw_Create4MenuCommonMenuItem
void	Tw_DeleteMenuItem(tdisplay TwD, tmenuitem MenuItem);

tmenu	Tw_CreateMenu(tdisplay TwD, tmsgport MsgPort, hwcol ColItem, hwcol ColSelect, hwcol ColDisabled,
		     hwcol ColSelectDisabled, hwcol ColShtCut, hwcol ColSelShtCut, byte FlagDefColInfo);
#define Tw_Grab	Tw_CreateMutex
void	Tw_SetInfoMenu(tdisplay TwD, tmenu Menu, byte Flags, uldat Len, byte *Text, hwcol *ColText);
#define Tw_Info4Menu Tw_SetInfoMenu
void	Tw_DeleteMenu(tdisplay TwD, tmenu Menu);

tmsgport Tw_CreateMsgPort(tdisplay TwD, byte NameLen, byte *ProgramName, time_t PauseSec, frac_t PauseFraction, byte WakeUp);
/* WakeUp: */
#define TW_TIMER_ALWAYS	((byte)1)
#define TW_TIMER_ONCE	((byte)2)
void	Tw_DeleteMsgPort(tdisplay TwD, tmsgport MsgPort);

void	Tw_BgImageScreen(tdisplay TwD, tscreen Screen, udat BgWidth, udat BgHeight, hwattr *BgImage);

tscreen	Tw_FirstScreen(tdisplay TwD);
twindow	Tw_FirstWindow(tdisplay TwD, tscreen Screen);
tgadget Tw_FirstGadget(tdisplay TwD, twindow Window);
tmsgport Tw_FirstMsgPort(tdisplay TwD);
tmenu    Tw_FirstMenu(tdisplay TwD, tmsgport MsgPort);
tmenuitem Tw_FirstMenuItem(tdisplay TwD, tmenu Menu);

tobj Tw_PrevObj(tdisplay TwD, tobj Obj);
tobj Tw_NextObj(tdisplay TwD, tobj Obj);
tobj Tw_ParentObj(tdisplay TwD, tobj Obj);

udat Tw_GetDisplayWidth(tdisplay TwD);
udat Tw_GetDisplayHeight(tdisplay TwD);

tmsg Tw_CreateMsg(tdisplay TwD, udat Type, udat Len);
void Tw_DeleteMsg(tdisplay TwD, tmsg Msg);
byte Tw_SendMsg(tdisplay TwD, tmsgport MsgPort, tmsg Msg);
void Tw_BlindSendMsg(tdisplay TwD, tmsgport MsgPort, tmsg Msg);

tobj Tw_GetOwnerSelection(tdisplay TwD);
/* this actually has a 'tmsgport Owner' parameter in sockproto.h */
void Tw_SetOwnerSelection(tdisplay TwD, tobj Owner, time_t Time, frac_t Frac);

void Tw_RequestSelection(tdisplay TwD, tobj Owner, uldat ReqPrivate);
void Tw_NotifySelection(tdisplay TwD, tobj Requestor, uldat ReqPrivate,
			uldat Magic, byte MIME[TW_MAX_MIMELEN], uldat Len, byte *Data);
#define TW_SEL_CURRENTTIME ((time_t)0)



tdisplay Tw_Open(byte *Tw_Display);
void	Tw_Close(tdisplay TwD);
int	Tw_ConnectionFd(tdisplay TwD);

/* these return FALSE only after libTw has paniced */
byte    Tw_Flush(tdisplay TwD);
byte    Tw_Sync(tdisplay TwD);

byte    Tw_InPanic(tdisplay TwD);

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
 * This is the function you must call to get a Msg from the server.
 * If Wait is TRUE  and no Msg is available, it waits until a Msg arrives.
 * If Wait is FALSE and no Msg is available, it tries non-blocking
 *                  to receive more Msgs.
 * 
 * In both cases, if there are already received Msgs they are returned
 * without waiting.
 */
tmsg	Tw_ReadMsg(tdisplay TwD, byte Wait);


/*
 * This is the function you must call to check if there are pending Msgs.
 * Don't rely on (Fd = Tw_ConnectionFd()) to be set after a select() !
 * Look at the twterm sources for an example.
 * 
 * It returns the first pending Msg, or (tmsg)0 if none is available
 * and none can be received non-blocking.
 */
tmsg	Tw_PeekMsg(tdisplay TwD);

/*
 *   <*>  B I G    W A R N I N G  <*>
 * 
 * Tw_PeekMsg() and Tw_ReadMsg() return a (tmsg) pointer to data
 * in a static buffer.
 * the pointer becomes invalid after a call
 * to one of the following functions:
 * (but calling them with an arg pointing to data inside tmsg is legal,
 * even with this side effect)
 * 
 * Tw_Sync();
 * Tw_PeekMsg();
 * Tw_ReadMsg();
 * Tw_CreateGadget();
 * Tw_SearchGadget();
 * Tw_CreateWindow();
 * Tw_Create4MenuWindow();
 * Tw_Create4MenuMenuItem();
 * Tw_CreateMsgPort();
 * Tw_CreateMenu();
 * 
 * and in general after a call to any Tw() function of libTw.h
 * with non-void return value, so that it sends something
 * to the server and waits for the server to send the return value.
 * 
 * Other important notes:
 * 1. a client can have only ONE MsgPort at time.
 * 2. Tw_DeleteMsgPort() calls Tw_DeleteMenu() on all menus created by the client.
 * 3. Tw_DeleteMenu() calls Tw_DeleteWindow() on all windows which use that menu.
 * 4. Tw_DeleteWindow() calls Tw_DeleteGadget() on all gadgets of that window.
 * 
 * also, if you exit() from your program without calling Tw_Flush(), Tw_Sync() or Tw_Close(),
 * pending data will *NOT* be sent to the server.
 */





/*
 * libTw.so.1 --- single head compatibility. these are too handy to drop them.
 */
extern tdisplay Tw_DefaultD;

#define TwConfigMalloc(my_malloc, my_realloc, my_free) Tw_ConfigMalloc(my_malloc, my_realloc, my_free)

#define TwLenStr(S)			strlen(S)
#define TwCmpStr(S1, S2)		strcmp((S1), (S2))
#define TwCopyStr(From,To)		strcpy((To),(From))

#define TwAllocMem(Size)		Tw_AllocMem(Size)
#define TwReAllocMem(Mem, Size)		Tw_ReAllocMem((Mem), (Size))
#define TwFreeMem(Mem)			Tw_FreeMem(Mem)
#define TwCloneStr(S)			Tw_CloneStr(S)

#define TwCopyMem(From, To, Size)	memcpy((To), (From), (size_t)(Size))
#define TwMoveMem(From, To, Size)	memmove((To), (From), (size_t)(Size))
#define TwWriteMem(Mem, Char, Size)	memset((Mem), (int)(Char), (size_t)(Size))
#define TwCmpMem(m1, m2, Size)		memcmp((m1), (m2), (size_t)(Size))

#define TwDropPrivileges()		(setegid(getgid()), seteuid(getuid()))
#define TwGetPrivileges()		seteuid(0)

#define TwFindFunction(Len, Name)	Tw_FindFunction(Tw_DefaultD, Len, Name)
#define TwEnableGzip()			Tw_EnableGzip(Tw_DefaultD)
#define TwDisableGzip()			Tw_DisableGzip(Tw_DefaultD)
#define TwNeedResizeDisplay()		Tw_NeedResizeDisplay(Tw_DefaultD)
#define TwAttachHW(len, name, redirect)	Tw_AttachHW(Tw_DefaultD, len, name, redirect)
#define TwAttachGetReply(len)		Tw_AttachGetReply(Tw_DefaultD, len)
#define TwAttachConfirm()		Tw_AttachConfirm(Tw_DefaultD)
#define TwDetachHW(len, name)		Tw_DetachHW(Tw_DefaultD, len, name)
#define TwSetFontTranslation(trans)	Tw_SetFontTranslation(Tw_DefaultD, trans)
#define TwCreateGadget(Window, \
		       ColText, ColTextSelect, ColTextDisabled, ColTextSelectDisabled, \
		       Code, Flags, Left, Up, XWidth, YWidth, Bitmap, \
		       TextNormal, TextSelect, TextDisabled, TextSelectDisabled, \
		       ColNormal, ColSelect, ColDisabled, ColSelectDisabled) \
	Tw_CreateGadget(Tw_DefaultD, Window, \
		       ColText, ColTextSelect, ColTextDisabled, ColTextSelectDisabled, \
		       Code, Flags, Left, Up, XWidth, YWidth, Bitmap, \
		       TextNormal, TextSelect, TextDisabled, TextSelectDisabled, \
		       ColNormal, ColSelect, ColDisabled, ColSelectDisabled)
#define TwCopyGadget(From, To)		Tw_CopyGadget(Tw_DefaultD, From, To)
#define TwDeleteGadget(Gadget)		Tw_DeleteGadget(Tw_DefaultD, Gadget)
#define TwSearchGadget(Window, i, j)	Tw_SearchGadget(Tw_DefaultD, Window, i, j)
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
#define TwMapWindow(Window, Screen)	Tw_MapWindow(Tw_DefaultD, Window, Screen)
#define TwUnMapWindow(Window)		Tw_UnMapWindow(Tw_DefaultD, Window)
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
#define TwGotoXYWindow(Window, X, Y)	Tw_GotoXYWindow(Tw_DefaultD, Window, X, Y)
#define TwSearchGadgetWindow(Window, X, Y) Tw_SearchGadgetWindow(Tw_DefaultD, Window, X, Y)
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
#define TwBgImageScreen(Screen, BgWidth, BgHeight, BgImage) \
	Tw_BgImageScreen(Tw_DefaultD, Screen, BgWidth, BgHeight, BgImage)
#define TwFirstScreen()			Tw_FirstScreen(Tw_DefaultD)
#define TwFirstWindow(Screen)		Tw_FirstWindow(Tw_DefaultD, Screen)
#define TwFirstGadget(Window)		Tw_FirstGadget(Tw_DefaultD, Window)
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
#define TwSetOwnerSelection(Owner, Time, Frac) \
					Tw_SetOwnerSelection(Tw_DefaultD, Owner, Time, Frac)
#define TwRequestSelection(Owner, ReqPrivate)	Tw_RequestSelection(Tw_DefaultD, Owner, ReqPrivate)
#define TwNotifySelection(Requestor, ReqPrivate, Magic, MIME, Len, Data) \
					Tw_NotifySelection(Tw_DefaultD, Requestor, ReqPrivate, \
							   Magic, MIME, Len, Data)


#define TwOpen(Tw_Display)		(!!(Tw_DefaultD = Tw_Open(Tw_Display)))
#define TwClose()			Tw_Close(Tw_DefaultD)
#define TwConnectionFd()		Tw_ConnectionFd(Tw_DefaultD)
#define TwFlush()			Tw_Flush(Tw_DefaultD)
#define TwTimidFlush()			Tw_TimidFlush(Tw_DefaultD)
#define TwSync()			Tw_Sync(Tw_DefaultD)
#define TwInPanic()			Tw_InPanic(Tw_DefaultD)
#define TwReadMsg(Wait)			Tw_ReadMsg(Tw_DefaultD, Wait)
#define TwPeekMsg()			Tw_PeekMsg(Tw_DefaultD)





#include "libTwerrno.h"


#endif /* _LIB_TW_H */

