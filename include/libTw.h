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
    udat Code, FullShiftFlags;
    udat SeqLen;
    byte pad, AsciiSeq[1];  /* including final \0 */
};

typedef struct tevent_mouse *tevent_mouse;
struct tevent_mouse {
    twindow Window;
    udat Code, FullShiftFlags;
    dat X, Y; /* these coords are relative to the window top-left corner */
};

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

typedef struct tevent_clipboard *tevent_clipboard;
struct tevent_clipboard {
    twindow Window;
    udat Code, FullShiftFlags; /* Code is unused */
    dat X, Y; /* these coords are absolute, to allow cross-window cut-n-paste */
    uldat Magic;
    uldat Len;
    byte Data[1];
};
/*ClipBoard Magic*/
#define TW_CLIP_TEXTMAGIC	((uldat)0x54657874)
#define TW_CLIP_FILEMAGIC	((uldat)0x46696c65)
#define TW_CLIP_IDMAGIC		((uldat)0x49644964)

typedef union tevent_any *tevent_any;
union tevent_any {
    struct tevent_common EventCommon;
    struct tevent_keyboard EventKeyboard;
    struct tevent_mouse EventMouse;
    struct tevent_window EventWindow;
    struct tevent_gadget EventGadget;
    struct tevent_menu EventMenu;
    struct tevent_clipboard EventClipBoard;
};

typedef struct tmsg *tmsg;
struct tmsg {
    uldat Len;		/* length of this struct, including `Len' field */
    uldat Magic;	/* == msg_magic */
    uldat Type;
    union tevent_any Event;
};
/* Msg Type : */
#define TW_MSG_WINDOW_KEY	((udat)0x1000)
#define TW_MSG_WINDOW_MOUSE	((udat)0x1001)
#define TW_MSG_WINDOW_CHANGE	((udat)0x1002)
#define TW_MSG_WINDOW_GADGET	((udat)0x1003)
#define TW_MSG_MENU_ROW		((udat)0x1004)
#define TW_MSG_CLIPBOARD	((udat)0x1005)



















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







#define TwLenStr(S) strlen(S)
#define TwCmpStr(S1, S2) strcmp((S1), (S2))
#define TwCopyStr(From,To) strcpy((To),(From))
#define TwCloneStr(S) strdup(S)

#define TwAllocMem(Size)		malloc(Size)
#define TwReAllocMem(Mem, Size)		realloc((Mem), (Size))
#define TwCopyMem(From, To, Size)	memcpy((To), (From), (size_t)(Size))
#define TwMoveMem(From, To, Size)	memmove((To), (From), (size_t)(Size))
#define TwWriteMem(Mem, Char, Size)	memset((Mem), (int)(Char), (size_t)(Size))
#define TwCmpMem(m1, m2, Size)		memcmp((m1), (m2), (size_t)(Size))
#define TwFreeMem(Mem)			free(Mem)

#define TwDropPrivileges() (setegid(getgid()), seteuid(getuid()))
#define TwGetPrivileges() seteuid(0)








uldat TwFindFunction(byte Len, byte *Name);

byte TwEnableGzip(void);
byte TwDisableGzip(void);

void TwNeedResizeDisplay(void);

void TwAttachHW(byte len, byte *name, byte redirect);
byte *TwAttachGetReply(uldat *len);

void TwDetachHW(void);

tgadget TwCreateGadget(twindow Window,
		       hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
		       udat Code, udat Flags, udat Left, udat Up, udat XWidth, udat YWidth, byte Bitmap,
		       byte *TextNormal, byte *TextSelect, byte *TextDisabled, byte *TextSelectDisabled,
		       hwcol *ColNormal, hwcol *ColSelect, hwcol *ColDisabled, hwcol *ColSelectDisabled);
void    TwCopyGadget(tgadget From, tgadget To);
void	TwDeleteGadget(tgadget Gadget);
tgadget TwSearchGadget(twindow Window, dat i, dat j);

void	TwCreate4MenuRow(twindow Window, udat Code, byte FlagActive, uldat TextLen, byte *Text);
#define TwRow4Menu TwCreate4MenuRow

twindow TwCreateWindow(udat TitleLen, byte *Title, hwcol *ColTitle, tmenu Menu,
		       hwcol ColText, uldat CursorType, uldat Attrib, byte Flags,
		       udat XWidth, udat YWidth, udat ScrollBackLines);
void	TwDeleteWindow(twindow Window);
twindow TwCreate4MenuWindow(tmenu Menu);
#define TwWin4Menu TwCreate4MenuWindow
void	TwMapWindow(twindow Window, tscreen Screen);
void	TwUnMapWindow(twindow Window);
void	TwWriteAsciiWindow(twindow Window, uldat AsciiLen, byte *AsciiSeq);
void    TwWriteHWAttrWindow(twindow Window, udat x, udat y, uldat Len, hwattr *Attr);
void	TwWriteRowWindow(twindow Window, uldat Len, byte *Text);
void	TwSetColTextWindow(twindow Window, hwcol ColText);
void	TwSetColorsWindow(twindow Window, udat Bitmap, hwcol ColGadgets, hwcol ColArrows, hwcol ColBars, hwcol ColTabs,
			  hwcol ColBorder, hwcol ColText, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled);
void	TwConfigureWindow(twindow Window, byte Bitmap, dat Left, udat Up, udat MinXWidth, udat MinYWidth, udat MaxXWidth, udat MaxYWidth);
void	TwGotoXYWindow(twindow Window, uldat X, uldat Y);
tgadget TwSearchGadgetWindow(twindow Window, dat X, dat Y);

tmenuitem TwCreate4MenuMenuItem(tmenu Menu, twindow Window, byte Flags, udat NameLen, byte *Name);
#define   TwItem4Menu TwCreate4MenuMenuItem
tmenuitem TwCreate4MenuCommonMenuItem(tmenu Menu);
#define   TwItem4MenuCommon TwCreate4MenuCommonMenuItem
void	TwDeleteMenuItem(tmenuitem MenuItem);

tmenu	TwCreateMenu(tmsgport MsgPort, hwcol ColItem, hwcol ColSelect, hwcol ColDisabled,
		     hwcol ColSelectDisabled, hwcol ColShtCut, hwcol ColSelShtCut, byte FlagDefColInfo);
void	TwSetInfoMenu(tmenu Menu, byte Flags, uldat Len, byte *Text, hwcol *ColText);
#define TwInfo4Menu TwSetInfoMenu
void	TwDeleteMenu(tmenu Menu);

tmsgport TwCreateMsgPort(byte NameLen, byte *ProgramName, time_t PauseSec, frac_t PauseFraction, byte WakeUp);
/* WakeUp: */
#define TW_TIMER_ALWAYS	((byte)1)
#define TW_TIMER_ONCE	((byte)2)
void	TwDeleteMsgPort(tmsgport MsgPort);

tscreen	TwFirstScreen(void);
twindow	TwFirstWindow(tscreen Screen);
tgadget TwFirstGadget(twindow Window);
tmsgport TwFirstMsgPort(void);
tmenu    TwFirstMenu(tmsgport MsgPort);
tmenuitem TwFirstMenuItem(tmenu Menu);

tobj TwPrevObj(tobj Obj);
tobj TwNextObj(tobj Obj);
tobj TwParentObj(tobj Obj);

byte    TwOpen(byte *TwDisplay);
void    TwClose(void);
int     TwConnectionFd(void);

/* these return FALSE only after libTw has paniced */
byte    TwFlush(void);
byte    TwSync(void);

byte    TwInPanic(void);

/*
 * This is the function you must call to get a Msg from the server.
 * If Wait is TRUE  and no Msg is available, it waits until a Msg arrives.
 * If Wait is FALSE and no Msg is available, it tries non-blocking
 *                  to receive more Msgs.
 * 
 * In both cases, if there are already received Msgs they are returned
 * without waiting.
 */
tmsg	TwReadMsg(byte Wait);


/*
 * This is the function you must call to check if there are pending Msgs.
 * Don't rely on (Fd = TwConnectionFd()) to be set after a select() !
 * Look at the twterm sources for an example.
 * 
 * It returns the first pending Msg, or (tmsg)0 if none is available
 * and none can be received non-blocking.
 */
tmsg	TwPeekMsg(void);

/*
 *   <*>  B I G    W A R N I N G  <*>
 * 
 * TwPeekMsg() and TwReadMsg() return a (tmsg) pointer to data
 * in a static buffer.
 * the pointer becomes invalid after a call
 * to one of the following functions:
 * (but calling them with an arg pointing to data inside tmsg is legal,
 * even with this side effect)
 * 
 * TwSync();
 * TwPeekMsg();
 * TwReadMsg();
 * TwCreateGadget();
 * TwSearchGadget();
 * TwCreateWindow();
 * TwCreate4MenuWindow();
 * TwCreate4MenuMenuItem();
 * TwCreateMsgPort();
 * TwCreateMenu();
 * 
 * and in general after a call to any Tw() function of libTw.h
 * with non-void return value, so that it sends something
 * to the server and waits for the server to send the return value.
 * 
 * Other important notes:
 * 1. a client can have only ONE MsgPort at time.
 * 2. TwDeleteMsgPort() calls TwDeleteMenu() on all menus created by the client.
 * 3. TwDeleteMenu() calls TwDeleteWindow() on all windows which use that menu.
 * 4. TwDeleteWindow() calls TwDeleteGadget() on all gadgets of that window.
 * 
 * also, if you exit() from your program without calling TwFlush(), TwSync() or TwClose(),
 * pending data will *NOT* be sent to the server.
 */

#define TwGrab		TwCreateMutex
#define TwInfo4Menu	TwSetInfoMenu
#define TwWin4Menu	TwCreate4MenuWindow
#define TwRow4Menu	TwCreate4MenuRow
#define TwItem4Menu	TwCreate4MenuMenuItem


#include "libTwerrno.h"


#endif /* _LIB_TW_H */

