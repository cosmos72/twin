/*
 *  twin.h  --  main include for all twin-related data types, functions and macros
 *
 *  Copyright (C) 1993-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_H
#define _TWIN_H

#include "twinsys.h"



#if !defined(CONST)
# if defined(__STDC__)
#  define CONST const
# else
#  define CONST
# endif
#endif



#define Abs(x) ((x)>0 ? (x) : -(x))
#define Swap(a, b, tmp) ((tmp)=(a), (a)=(b), (b)=(tmp))
#define Min2(x, y) ((x)<(y) ? (x) : (y))
#define Max2(x, y) ((x)>(y) ? (x) : (y))
#define Sign(n) ((n)>0 ? 1 : (n)<0 ? -1 : 0)

#define Max3(x, y, z) ((x)>(y) ? Max2(x,z) : Max2(y,z))
#define Min3(x, y, z) ((x)<(y) ? Min2(x,z) : Min2(y,z))


/***************/

typedef   signed char	 num;
typedef unsigned char	byte;
typedef   signed short	 dat;
typedef unsigned short	udat;
typedef   signed int	ldat;
typedef unsigned int   uldat;

#define MAXU(t)		((t)~(t)0)
#define MINS(t)		((t)((t)1<<(8*sizeof(t)-1)))
#define MAXS(t)		((t)~MINS(t))

#define MINNUM		MINS(num)
#define MAXNUM		MAXS(num)
#define MAXBYTE		MAXU(byte)
#define MINDAT		MINS(dat)
#define MAXDAT		MAXS(dat)
#define MAXUDAT		MAXU(udat)
#define MINLDAT		MINS(ldat)
#define MAXLDAT		MAXS(ldat)
#define MAXULDAT	MAXU(uldat)

#define NOFD   (-1)
#define specFD   (-2)	/* use for every FD that needs a special RemoteFlush()
			 * instead of a plain write() and set PrivateFlush as needed */

#define NOPID  ((pid_t)0)
#define NOSLOT MAXULDAT

#define FALSE	((byte)0)
#define TRUE	(!FALSE)

/* "Twin" in native byte-order */
#define TWIN_MAGIC ((uldat)0x6E697754ul)
/* "Go!!" in native byte-order */
#define GO_MAGIC   ((uldat)0x21216F47ul)
/* "Wait" in native byte-order */
#define WAIT_MAGIC ((uldat)0x74696157ul)
/* "Stop" in native byte-order */
#define STOP_MAGIC ((uldat)0x706F7453ul)

#define TW_INET_PORT	7754

#define SMALLBUFF	256
#define BIGBUFF		4096
#define HUGEBUFF	131072


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

/*
 * Notes about the timevalue struct:
 * 
 * it is used to represent both time intervals and absolute times;
 * the ->Seconds is a time_t numeric field.
 * DON'T assume time_t is 32 bit (or any other arbitrary size)
 * since in 19 Jan 2038 at 04:14:08 any signed, 32 bit time_t will overflow.
 * So use sizeof(time_t) if you really need.
 * 
 * the ->Fraction is a frac_t numeric field (frac_t is signed).
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
typedef ldat frac_t;

typedef struct timevalue  {
    time_t Seconds;
    frac_t Fraction;
} timevalue;

#define THOUSAND	((frac_t)1000)

#define NanoSECs	* 1 /* i.e. (frac_t)1 is a nanosecond */
#define MicroSECs	* (THOUSAND NanoSECs)
#define MilliSECs	* (THOUSAND MicroSECs)
#define FullSECs	* (THOUSAND MilliSECs)

/* this is tricky... we don't know signedness nor sizeof(time_t) during preprocess */
#define MAXTIME_T	( (time_t)-1 > (time_t)0 ? MAXU(time_t) : MAXS(time_t) )
#define MINTIME_T	( (time_t)-1 > (time_t)0 ? (time_t)0 : MINS(time_t) )
#define MAXFRAC_T	MAXS(frac_t)
#define MINFRAC_T	MINS(frac_t)

#define ABS(x) ((x)>0 ? (x) : -(x))

#define XAND(Bits, Mask)  (( (Bits) & (Mask) ) == (Mask) )

typedef struct font {
    byte AsciiCode;
    byte *Bitmap;
} font;

typedef struct palette {
    byte Red, Green, Blue;
} palette;

typedef struct mouse_state {
    dat x, y;
    dat delta_x, delta_y;
    byte keys;
} mouse_state;

typedef struct fn_obj fn_obj;
typedef struct fn_area fn_area;
typedef struct obj obj;
typedef struct obj_parent obj_parent;
typedef struct area area;
typedef struct area_parent area_parent;
typedef struct area_win area_win;
typedef struct area_win_parent area_win_parent;
typedef struct fn_area_win fn_area_win;
typedef struct gadget gadget;
typedef struct fn_gadget fn_gadget;
typedef struct row row;
typedef struct fn_row fn_row;
typedef struct ttydata ttydata;
typedef struct remotedata remotedata;
typedef struct window window;
typedef struct fn_window fn_window;
typedef struct menuitem menuitem;
typedef struct fn_menuitem fn_menuitem;
typedef struct menu menu;
typedef struct fn_menu fn_menu;
typedef struct screen screen;
typedef struct fn_screen fn_screen;
typedef struct msg msg;
typedef struct fn_msg fn_msg;
typedef struct msgport msgport;
typedef struct fn_msgport fn_msgport;
typedef struct mutex mutex;
typedef struct fn_mutex fn_mutex;
typedef struct module module;
typedef struct fn_module fn_module;
typedef struct display_hw display_hw;
typedef struct fn_display_hw fn_display_hw;
typedef struct fn fn;
typedef struct setup setup;
typedef struct all all;

typedef void (*fn_hook)(window *);


typedef struct fdlist fdlist;	/* for compressed sockets, two fdlist's are used: */
struct fdlist {			/* the uncompressed one has                       */
    int Fd;                     /* Fd == GZFD and pairSlot == the compressed one; */
    uldat pairSlot;		/* the compressed has                             */
    void *HandlerData;		/* Fd == real fd and pairSlot == the uncompressed;*/
    void (*HandlerIO)(int Fd, size_t any);
    msgport *MsgPort;		/* other sockets just have                        */
    byte *WQueue;		/* Fd == real fd and pairSlot == NOSLOT;          */
    byte *RQueue;
    uldat WQlen, WQmax;
    uldat RQstart, RQlen, RQmax;
    void (*PrivateAfterFlush)(uldat Slot); /* private enable/disable compression routine.
					    * it gets called after RemoteFlush() and it must
					    * remove itself if needed (e.g. call-once routines)
					    */
    byte (*PrivateFlush)(uldat Slot); /* private flush (compression) routine */
    void *PrivateData;		/* used by (un)compression routines to hold private data */
    byte extern_couldntwrite;
};

struct obj {
    uldat Id;
    fn_obj *Fn;
    obj *Prev, *Next;
    obj_parent *Parent;
    obj *OPrev, *ONext;
};

struct obj_parent {
    obj *First, *Last;
};

struct fn_obj {
    uldat Magic, Size, Used;
    obj *(*Create)(fn_obj *);
    obj *(*Copy)(obj *From, obj *To);
    void (*Insert)(obj *Obj, obj_parent *, obj *Prev, obj *Next);
    void (*Remove)(obj *);
    void (*Delete)(obj *);
};

struct area {
    uldat Id;
    fn_area *Fn;
    area *Prev, *Next;
    area_parent *Parent;
    screen *FirstScreen;
    window *FirstWindow, *OnlyThisWindow;
    gadget *FirstGadget, *OnlyThisGadget;
    dat Xstart, Ystart, Xend, Yend;
    byte Shaded;
};

struct area_parent {
    area *FirstArea, *LastArea;
};

struct fn_area {
    uldat Magic, Size, Used;
    area *(*Create)(fn_area *, area_parent *, screen *, window *, window *OnlyThisWindow,
		    gadget *FirstGadget, gadget *OnlyThisGadget, dat Xstart, dat Ystart,
		    dat Xend, dat Yend, byte Shaded);
    area *(*Copy)(area *From, area *To);
    void (*Insert)(area *, area_parent *, area *Prev, area *Next);
    void (*Remove)(area *);
    void (*Delete)(area *);
};

struct area_win {
    uldat Id;
    fn_area_win *Fn;
    area_win *Prev, *Next;
    area_win_parent *Parent;
    window *Window;
    gadget *FirstGadget, *OnlyThisGadget;
    byte NoGadgets;
    uldat XLogic, YLogic;
    dat Xstart, Ystart, Xend, Yend;
};

struct area_win_parent {
    area_win *FirstAreaWin, *LastAreaWin;
};

struct fn_area_win {
    uldat Magic, Size, Used;
    area_win *(*Create)(fn_area_win *, area_win_parent *, window *, gadget *FirstGadget, gadget *OnlyThisGadget,
			byte NoGadgets, uldat XLogic, uldat YLogic, dat Xstart, dat Ystart, dat Xend, dat Yend);
    area_win *(*Copy)(area_win *From, area_win *To);
    void (*Insert)(area_win *, area_win_parent *, area_win *Prev, area_win *Next);
    void (*Remove)(area_win *);
    void (*Delete)(area_win *);
};

struct gadget {
    uldat Id;
    fn_gadget *Fn;
    gadget *Prev, *Next;
    window *Window;
    hwcol ColText, ColSelect, ColDisabled, ColSelectDisabled;
    udat Code, Flags;
    udat Left, Up, XWidth, YWidth;
    byte *Contents[8];
};

struct fn_gadget {
    uldat Magic, Size, Used;
    gadget *(*Create)(fn_gadget *, window *Window,
		      hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
		      udat Code, udat Flags, udat Left, udat Up, udat XWidth, udat YWidth,
		      CONST byte *TextNormal, CONST byte *TextSelect, CONST byte *TextDisabled, CONST byte *TextSelectDisabled,
		      CONST hwcol *ColNormal, CONST hwcol *ColSelect, CONST hwcol *ColDisabled, CONST hwcol *ColSelectDisabled);
    gadget *(*Copy)(gadget *From, gadget *To);
    void (*Insert)(gadget *, window *, gadget *Prev, gadget *Next);
    void (*Remove)(gadget *);
    void (*Delete)(gadget *);
    gadget *(*CreateEmptyButton)(fn_gadget *Fn_Gadget, window *Window, udat XWidth, udat YWidth, hwcol BgCol);
    byte (*FillButton)(gadget *Gadget, udat Code, udat Left, udat Up, udat Flags, CONST byte *Text, hwcol Color, hwcol ColorDisabled);
    gadget *(*CreateButton)(fn_gadget *Fn_Gadget, window *Window, hwcol BgCol, hwcol Col, hwcol ColDisabled,
			    udat Code, udat Flags, udat Left, udat Up, udat XWidth, udat YWidth, CONST byte *Text);
    gadget *(*CloneButton)(gadget *SetUpGadget, udat Code, udat Left, udat Up, hwcol BgCol);

};

/*Flags : */
#define GADGET_USE_DEFCOL	WINFL_USE_DEFCOL
#define GADGET_DISABLED		((udat)0x02)
/*GADGET_PRESSED==0x4000      */

/*              NOTE :              */
/*
 the "Contents" of a gadget is structured as follows:
 Contents[0]==TextNormal;      (mandatory)
 Contents[1]==TextSelect;        (if not present, use ...[0])
 Contents[2]==TextDisabled;      (if not present, use ...[0])
 Contents[3]==TextSelectDisabled; (if not present, use ...[1]; if ...[1] not present too, use ...[0])
 * 
 Contents[4]==ColorNormal;      (mandatory unless WINFL_USE_DEFCOL is set)
 Contents[5]==ColorSelect;        (if not present, use ...[4])
 Contents[6]==ColorDisabled;      (if not present, use ...[4])
 Contents[7]==ColorSelectDisabled;(if not present, use ...[5]; if ...[5] not present too, use ...[4])
 * 
 If WINFL_USE_DEFCOL is active,
 ...[4], ...[5], ...[6] and ...[7] are ignored.
 * 
 */

struct row {
    uldat Id;
    fn_row *Fn;
    row *Prev, *Next;
    window *Window;
    udat Code;
    byte Flags;
    uldat Len, MaxLen;
    uldat Gap, LenGap;
    byte *Text, *ColText;
};
struct fn_row {
    uldat Magic, Size, Used;
    row *(*Create)(fn_row *, udat Code, byte Flags);
    row *(*Copy)(row *From, row *To);
    void (*Insert)(row *, window *, row *Prev, row *Next);
    void (*Remove)(row *);
    void (*Delete)(row *);
    row *(*Create4Menu)(fn_row *Fn_Row, window *Window, udat Code, byte FlagActive, uldat Len, CONST byte *Text);
};
/*Flags : */
#define ROW_INACTIVE	((byte)0x00)
#define ROW_ACTIVE	((byte)0x01)
#define ROW_IGNORE	((byte)0x02)

typedef enum ttystate {
    ESnormal = 0, ESesc, ESsquare, ESgetpars, ESgotpars, ESfunckey,
      EShash, ESsetG0, ESsetG1, ESpercent, ESignore, ESnonstd,
      ESpalette, ESany = 0xFF, ESques = 0x100
} ttystate;

#define TTY_STOPPED	((udat)0x0001)
#define TTY_AUTOWRAP	((udat)0x0002)
#define TTY_NEEDWRAP	((udat)0x0004)
#define TTY_DISPCTRL	((udat)0x0008)
#define TTY_CRLF	((udat)0x0010)
#define TTY_KBDAPPLIC	((udat)0x0020)
#define TTY_INSERT	((udat)0x0040)
#define TTY_REVERSECOL	((udat)0x0080)
#define TTY_INVERTSCR	((udat)0x0100)
#define TTY_ALTCURSKEYS	((udat)0x0200)
#define TTY_ABSORIG	((udat)0x0400)
#define TTY_SETMETA	((udat)0x0800)
#define TTY_UPDATECURSOR ((udat)0x1000)
#define TTY_REPORTMOUSE	 ((udat)0x2000)
#define TTY_REPORTMOUSE2 ((udat)0x4000)
#define TTY_NEEDREFOCUS	((udat)0x8000)

#define EFF_INTENSITY	((udat)0x0001)
#define EFF_HALFINTENS	((udat)0x0002)
#define EFF_UNDERLINE	((udat)0x0004)
#define EFF_BLINK	((udat)0x0008)
#define EFF_REVERSE	((udat)0x0010)

#define NPAR		16

#define GRAF_MAP	0
#define LAT1_MAP	1
#define USER_MAP	2
#define IBMPC_MAP	3

struct ttydata {
    ttystate State;
    udat Flags;
    udat Effects;
    udat ScrollBack;	/* Number of scrollback lines */
    udat SizeX, SizeY;	/* Terminal size */
    udat Top, Bottom;	/* Y scrolling region. default 0...SizeY-1 */
    udat X, Y;		/* Cursor position in visible buffer */
    udat saveX, saveY;
    hwattr *Start, *Split;/* Start and Split of visible buffer */
    			  /* AfterSplit is just Window->Contents */
    hwattr *Pos;	  /* Pointer to cursor position in buffer */
    hwcol Color, DefColor, saveColor, Underline, HalfInten;
    uldat TabStop[5];
    uldat nPar, Par[NPAR];
    
    byte currG, G, G0, G1, saveG, saveG0, saveG1;
};

struct remotedata {
    int Fd;
    pid_t ChildPid;
    uldat FdSlot; /* index in the FdList array (remote.c) */
};

struct window {
    uldat Id;
    fn_window *Fn;
    window *Prev, *Next; /* list in the same screen */
    screen *Screen;      /* in which window sits */
    window *OPrev, *ONext; /* list with the same menu */
    menu *Menu;		 /* from which the window eventually depends */
    udat LenTitle;
    byte *Title, *ColTitle;
    byte *BorderPattern[2];
    ttydata *TtyData;
    fn_hook ShutDownHook;
    fn_hook Hook, *WhereHook;
    fn_hook MapUnMapHook;
    msg *MapQueueMsg;
    remotedata RemoteData;
    uldat XLogic, YLogic, CurX, CurY;
    uldat XstSel, YstSel, XendSel, YendSel;
    hwcol ColGadgets, ColArrows, ColBars, ColTabs, ColBorder, ColText, ColSelect, ColDisabled, ColSelectDisabled;
    byte Flags;
    uldat Attrib;
    uldat CursorType;
    dat Left; udat Up;
    udat XWidth, YWidth;
    udat MinXWidth, MinYWidth;
    udat MaxXWidth, MaxYWidth;
    hwattr *Contents;
    uldat MaxNumRow;
    row *FirstRow, *LastRow;
    row *RowOne, *RowSplit;	/*RESERVED: used to optimize the drawing on screen */
    uldat NumRowOne, NumRowSplit;/*RESERVED: updated automatically by WriteRow. To insert */
    				/*or remove manually rows, you must zero out NumRowOne */
				/*and NumRowSplit forcing twin to recalculate them */
    gadget *FirstGadget, *LastGadget, *GadgetSelect;
};
struct fn_window {
    uldat Magic, Size, Used;
    window *(*Create)(fn_window *, udat LenTitle, CONST byte *Title, CONST hwcol *ColTitle, menu *Menu,
		      hwcol ColText, uldat CursorType, uldat Attrib, byte Flags,
		      udat XWidth, udat YWidth, udat ScrollBackLines);
    window *(*Copy)(window *From, window *To);
    void (*Insert)(window *, screen *, window *Prev, window *Next);
    void (*Remove)(window *);
    void (*Delete)(window *);
    byte (*FindBorder)(window *, udat u, udat v, byte Border, byte *PtrChar, byte *PtrColor);
    void (*SetColText)(window *, hwcol ColText);
    void (*SetColors)(window *, udat Bitmap,
		      hwcol ColGadgets, hwcol ColArrows, hwcol ColBars, hwcol ColTabs, hwcol ColBorder,
		      hwcol ColText, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled);
    void (*Configure)(window *, byte Bitmap, dat Left, udat Up, udat MinXWidth, udat MinYWidth,
		      udat MaxXWidth, udat MaxYWidth);
    void (*GotoXY)(window *, uldat X, uldat Y);
    window *(*Create4Menu)(fn_window *, menu *);
    void (*RealMap)(window *, screen *);
    void (*Map)(window *, screen *);
    void (*UnMap)(window *);
    void (*Own)(window *, menu *);
    void (*DisOwn)(window *);    
    window *(*Focus)(window *);
    window *(*KbdFocus)(window *);
    void (*WriteAscii)(window *, uldat Len, CONST byte *Text);
    void (*WriteHWAttr)(window *, udat x, udat y, uldat Len, CONST hwattr *Attr);
    byte (*WriteRow)(window *, uldat Len, CONST byte *Text);
    row *(*SearchRow)(window *, uldat RowN);
    row *(*SearchRowCode)(window *, udat Code, uldat *NumRow);
    gadget *(*SearchGadget)(window *, dat i, dat j);
    gadget *(*SearchGadgetCode)(window *, udat Code);
    byte (*InstallHook)(window *, fn_hook, fn_hook *Where);
    void (*RemoveHook)(window *, fn_hook, fn_hook *Where);
};

/* Window->Attrib */
#define WINDOW_MENU		((uldat)0x0001)
#define WINDOW_WANT_KEYS	((uldat)0x0002)
#define WINDOW_WANT_MOUSE	((uldat)0x0004)
#define WINDOW_WANT_CHANGE	((uldat)0x0008)
#define WINDOW_DRAG		((uldat)0x0010)
#define WINDOW_RESIZE		((uldat)0x0020)
#define WINDOW_CLOSE		((uldat)0x0040)
#define WINDOW_ROLLED_UP	((uldat)0x0080)
#define WINDOW_X_BAR		((uldat)0x0100)
#define WINDOW_Y_BAR		((uldat)0x0200)

/* #define WINDOW_UNUSED	((uldat)0x0400) */

/* this must fit in `udat' since it is shared with gadget.Flags */
#define GADGET_PRESSED		((uldat)0x0800)

#define X_BAR_SELECT		((uldat)0x1000)
#define Y_BAR_SELECT		((uldat)0x2000)
#define XY_BAR_SELECT		(X_BAR_SELECT | Y_BAR_SELECT)
#define TAB_SELECT		((uldat)0x4000)
#define PAGE_BACK_SELECT	((uldat)0x8000)
#define PAGE_FWD_SELECT		((uldat)0x00010000lu)
#define ARROW_BACK_SELECT	((uldat)0x00020000lu)
#define ARROW_FWD_SELECT	((uldat)0x00040000lu)
#define SCROLL_ANY_SELECT  	(ARROW_BACK_SELECT | ARROW_FWD_SELECT | PAGE_BACK_SELECT | PAGE_FWD_SELECT | TAB_SELECT)

#define WINDOW_FWDSEL		((uldat)0x00080000lu)
#define WINDOW_REVSEL		((uldat)0x00100000lu)
#define WINDOW_ANYSEL		(WINDOW_FWDSEL|WINDOW_REVSEL)
#define WINDOW_DO_SEL		((uldat)0x00200000lu)

#define BUTTON_FIRST_SELECT	((uldat)0x00400000lu)
#define BUTTON_LAST_SELECT	((uldat)0x80000000lu)
#define BUTTON_ANY_SELECT	((uldat)0xFFE00000lu)

/*#define BUTTON_FIRST		((byte)0) */
/*#define BUTTON_CLOSE		((byte)0) */
/*#define BUTTON_LAST		((byte)9) */

#define BUTTON_MAX		((byte)10)




/* Window->Flags */
/* #define WINFL_USEROWS	((byte)0x00) it's the default */
#define WINFL_USECONTENTS	((byte)0x01)
#define WINFL_BYUSER		((byte)0x02)
#define WINFL_USEANY		((byte)0x03)

#define WINFL_USE_DEFCOL	((byte)0x04)
#define	WINFL_CURSOR_ON		((byte)0x08)
#define WINFL_INSERT		((byte)0x10)
#define WINFL_SEL_ROWCURR	((byte)0x20)
#define WINFL_DISABLED		((byte)0x40)

/* CursorType: */
/* These come from linux/drivers/char/console.c */
#define NOCURSOR	1
#define LINECURSOR	2
#define SOLIDCURSOR	8

/* window size limits */
#define MIN_XWIN	4
#define MIN_YWIN	2

struct menuitem {
    uldat Id;
    fn_menuitem *Fn;
    menuitem *Prev, *Next;
    menu *Menu;
    window *Window;
    byte FlagActive;
    udat Left, Len, ShortCut;
    byte *Name;
};
struct fn_menuitem {
    uldat Magic, Size, Used;
    menuitem *(*Create)(fn_menuitem *, menu *Menu, window *Window, byte FlagActive,
			udat Left, udat Len, udat ShortCut, CONST byte *Name);
    menuitem *(*Copy)(menuitem *From, menuitem *To);
    void (*Insert)(menuitem *, menu *, menuitem *Prev, menuitem *Next);
    void (*Remove)(menuitem *);
    void (*Delete)(menuitem *);
    menuitem *(*Create4Menu)(fn_menuitem *, menu *, window *, byte FlagActive,
			     udat Len, CONST byte *Name);
    uldat (*Create4MenuCommon)(fn_menuitem *, menu *);
    /* for compatibility this must return an (uldat) != 0 for success. yuch */
};

struct menu {
    uldat Id;
    fn_menu *Fn;
    menu *Prev, *Next; /* with the same msgport */
    msgport *MsgPort;
    hwcol ColItem, ColSelect, ColDisabled, ColSelectDisabled, ColShtCut, ColSelShtCut;
    byte CommonItems;
    byte FlagDefColInfo;
    row *Info;
    menuitem *FirstMenuItem, *LastMenuItem, *MenuItemSelect;
    window *FirstWindow, *LastWindow; /* that have this as menu */
};
struct fn_menu {
    uldat Magic, Size, Used;
    menu *(*Create)(fn_menu *, msgport *MsgPort, hwcol ColItem, hwcol ColSelect, hwcol ColDisabled,
		    hwcol ColSelectDisabled, hwcol ColShtCut, hwcol ColSelShtCut, byte FlagDefColInfo);
    menu *(*Copy)(menu *From, menu *To);
    void (*Insert)(menu *, msgport *, menu *Prev, menu *Next);
    void (*Remove)(menu *);
    void (*Delete)(menu *);
    row *(*SetInfo)(menu *, byte Flags, uldat Len, CONST byte *Text, CONST hwcol *ColText);
    menuitem *(*SearchItem)(menu *, dat i);
    menuitem *(*GetSelectItem)(menu *);
    void (*SetSelectItem)(menu *, menuitem *);
};



struct screen {
    uldat Id;
    fn_screen *Fn;
    screen *Prev, *Next;
    all *All;
    udat LenTitle;
    byte *Title;
    dat ScreenWidth, ScreenHeight;
    window *FirstWindow, *LastWindow, *FocusWindow, *MenuWindow;
    fn_hook FnHookWindow; window *HookWindow;
    udat Attrib;
    dat Left; udat Up; udat YLimit;
    udat BgWidth, BgHeight;
    hwattr *Bg;
};
struct fn_screen {
    uldat Magic, Size, Used;
    screen *(*Create)(fn_screen *, udat LenTitle, CONST byte *Title,
		      udat BgWidth, udat BgHeight, CONST hwattr *Bg);
    screen *(*Copy)(screen *From, screen *To);
    void (*Insert)(screen *, all *, screen *Prev, screen *Next);
    void (*Remove)(screen *);
    void (*Delete)(screen *);
    window *(*SearchWindow)(screen *, dat i, dat j);
    menu *(*SearchMenu)(screen *);
    screen *(*Search)(dat j);
    screen *(*CreateSimple)(fn_screen *, udat LenTitle, CONST byte *Title, hwattr Bg);
    void (*BgImage)(screen *, udat BgWidth, udat BgHeight, CONST hwattr *Bg);
    void (*Focus)(screen *);
    void (*DrawMenu)(screen *, dat Xstart, dat Xend);
    void (*ActivateMenu)(screen *, menuitem *, byte byMouse);
    void (*DeActivateMenu)(screen *);
};
/*Attrib : */
#define GADGET_BACK_SELECT 0x8000
/*GADGET_PRESSED==0x4000      */


#define MSG_KEY			((udat)0)
#define MSG_MOUSE		((udat)1)
#define MSG_MAP			((udat)2)
#define MSG_CONTROL		((udat)3)

#define MSG_DISPLAY		((udat)0x0FFF)

#define MSG_SYSTEM_FIRST	((udat)0x1000)
#define MSG_WINDOW_KEY		((udat)0x1000)
#define MSG_WINDOW_MOUSE	((udat)0x1001)
#define MSG_WINDOW_CHANGE	((udat)0x1002)
#define MSG_WINDOW_GADGET	((udat)0x1003)
#define MSG_MENU_ROW		((udat)0x1004)
#define MSG_SELECTION		((udat)0x1005)
#define MSG_SELECTIONNOTIFY	((udat)0x1006)
#define MSG_SELECTIONREQUEST	((udat)0x1007)
#define MSG_SELECTIONCLEAR	((udat)0x1008)


#define MSG_USER_FIRST		((udat)0x2000)
#define MSG_USER_CONTROL	((udat)0x2000)

/*
 * Notes about MsgType :
 *
 * 0x0000 ... 0x0FFF : Messages from Twin to the WM or another special task;
 * currently defined are:
 * MSG_KEY		use Msg->Event.EventKeyboard to get the event
 * MSG_MOUSE		use ...Event.EventMouse
 * MSG_CONTROL		use ...Event.EventControl
 * 
 * MSG_DISPLAY		use ...Event.EventDisplay
 *
 * 0x1000 ... 0x1FFF : Messages from the WM to a generick task
 * currently defined are:
 * MSG_WINDOW_KEY	use ...EventKeyboard
 * MSG_WINDOW_MOUSE	use ...EventMouse
 * MSG_WINDOW_CHANGE	use ...EventWindow
 * MSG_WINDOW_GADGET	use ...EventGadget
 * MSG_MENU_ROW		use ...EventMenu
 * MSG_SELECTION	use ...EventSelection
 * MSG_SELECTIONNOTIFY	use ...EventSelectionNotify
 * MSG_SELECTIONREQUEST	use ...EventSelectionRequest
 * MSG_SELECTIONCLEAR	use ...EventCommon
 * If you don't want to get messages from gadgets or menuitem rows,
 * just set to 0 (zero) their Code.
 * 
 * 0x2000 ... 0xFFFF : Messages from a generic task to another.
 * they are completely user-defined. As guideline, the following
 * are defined:
 * 
 * MSG_USER_CONTROL	use ...Event.EventControl
 */


typedef struct event_common event_common;
struct event_common {
    window *Window;
    udat Code, pad;
};

typedef struct event_map event_map;
struct event_map {
    window *Window;
    udat Code, pad;	/* unused */
    screen *Screen;
};

typedef struct event_keyboard event_keyboard;
struct event_keyboard {
    window *Window;
    udat Code, ShiftFlags, SeqLen;
    byte pad, AsciiSeq[1];  /* AsciiSeq[SeqLen] == '\0' */
};

typedef struct event_mouse event_mouse;
struct event_mouse {
    window *Window;
    udat Code, ShiftFlags;
    dat X, Y;
};

typedef struct event_control event_control;
struct event_control {
    window *Window;
    udat Code, Len;
    byte Data[1]; /* Data[Len] == '\0' */
};

/* some MSG_CONTROL codes */
#define MSG_CTRL_QUIT		((udat)0)
#define MSG_CTRL_RESTART	((udat)1)
#define MSG_CTRL_OPEN		((udat)2)

typedef struct event_display event_display;
struct event_display {
    window *Window; /* not used here */
    udat Code, Len;
    dat X, Y;
    byte *Data; /* [len] bytes actually */
};

#define DPY_DrawHWAttr		((udat)0)
#define DPY_FlushHW		((udat)1)
#define DPY_KeyboardEvent	((udat)2)
#define DPY_MouseEvent		((udat)3)
#define DPY_SetCursorType	((udat)4)
#define DPY_MoveToXY		((udat)5)
#define DPY_Resize		((udat)6)

#define DPY_SelectionExport	((udat)8)
#define DPY_DragArea		((udat)9)
#define DPY_Beep		((udat)10)
#define DPY_Configure		((udat)11)
#define DPY_SetPalette		((udat)12)
#define DPY_ResetPalette	((udat)13)
#define DPY_Helper		((udat)14)
#define DPY_RedrawVideo		((udat)15)
#define DPY_Quit		((udat)16)

typedef struct event_window event_window;
struct event_window {
    window *Window;
    udat Code, pad; /* not used */
    dat XWidth, YWidth;
};

typedef struct event_gadget event_gadget;
struct event_gadget {
    window *Window;
    udat Code, pad;
};

typedef struct event_menu event_menu;
struct event_menu {
    window *Window;
    udat Code, pad;
    menu *Menu;
};

typedef struct event_selection event_selection;
struct event_selection {
    window *Window;
    udat Code, pad; /* unused */
    dat X, Y;
};

#define MAX_MIMELEN 64

typedef struct event_selectionnotify event_selectionnotify;
struct event_selectionnotify {
    window *Window;
    udat Code, pad; /* unused */
    uldat ReqPrivate;
    uldat Magic;
    byte MIME[MAX_MIMELEN];
    uldat Len;
    byte Data[1]; /* Data[] is Len bytes actually */
};
/*SelectionNotify Magic*/
#define SEL_APPEND	((uldat)0x00000000)
#define SEL_TEXTMAGIC	((uldat)0x54657874)
#define SEL_FILEMAGIC	((uldat)0x46696c65)
#define SEL_URLMAGIC	((uldat)0xAB1691BA)
#define SEL_DATAMAGIC	((uldat)0xDA1AA1AD) /* check MIME if you get this */
#define SEL_IDMAGIC	((uldat)0x49644964)

typedef struct event_selectionrequest event_selectionrequest;
struct event_selectionrequest {
    window *Window;
    udat Code, pad; /* unused */
    obj *Requestor;
    uldat ReqPrivate;
};

typedef union event_any event_any;
union event_any {
    event_common EventCommon;
    event_keyboard EventKeyboard;
    event_mouse EventMouse;
    event_control EventControl;
    event_display EventDisplay;
    event_map EventMap;
    event_window EventWindow;
    event_gadget EventGadget;
    event_menu EventMenu;
    event_selection EventSelection;
    event_selectionnotify EventSelectionNotify;
    event_selectionrequest EventSelectionRequest;
};

struct msg {
    uldat Id;
    fn_msg *Fn;
    msg *Prev, *Next;
    msgport *MsgPort;
    udat Type;		/* See above notes */
    udat Len;		/* length of Event */
    event_any Event;
};
struct fn_msg {
    uldat Magic, Size, Used;
    msg *(*Create)(fn_msg *, udat Type, udat EventLen);
    msg *(*Copy)(msg *From, msg *To);
    void (*Insert)(msg *, msgport *, msg *Prev, msg *Next);
    void (*Remove)(msg *);
    void (*Delete)(msg *);
};



struct msgport {
    uldat Id;
    fn_msgport *Fn;
    msgport *Prev, *Next; /* list in the same All */
    all *All;
    byte WakeUp, NameLen, *ProgramName;
    			/*Note : A Task is always woken up if it has pending messages. */
    void (*Handler)(msgport *);
    void (*ShutDownHook)(msgport *);
    timevalue CallTime, PauseDuration;
    remotedata RemoteData;
    msg *FirstMsg, *LastMsg;
    menu *FirstMenu, *LastMenu; /* that have this as MsgPort */
    mutex *FirstMutex, *LastMutex; /* that have this as MsgPort */
    display_hw *AttachHW;	/* that was attached as told by MsgPort */
};
struct fn_msgport {
    uldat Magic, Size, Used;
    msgport *(*Create)(fn_msgport *, byte NameLen, CONST byte *ProgramName,
		       time_t PauseSec, frac_t PauseFraction,
		       byte WakeUp, void (*Handler)(msgport *));
    msgport *(*Copy)(msgport *From, msgport *To);
    void (*Insert)(msgport *, all *, msgport *Prev, msgport *Next);
    void (*Remove)(msgport *);
    void (*Delete)(msgport *);
};
/* WakeUp: */
#define TIMER_ALWAYS	((byte)1)
#define TIMER_ONCE	((byte)2)

struct mutex {
    uldat Id;
    fn_mutex *Fn;
    mutex *Prev, *Next; /* in the same All */
    all *All;
    mutex *OPrev, *ONext; /* with the same MsgPort */
    msgport *MsgPort;
    byte Perm, NameLen, *Name;
};
struct fn_mutex {
    uldat Magic, Size, Used;
    mutex *(*Create)(fn_mutex *, msgport *MsgPort, byte NameLen, CONST byte *Name, byte Perm);
    mutex *(*Copy)(mutex *From, mutex *To);
    void (*Insert)(mutex *, all *, mutex *Prev, mutex *Next);
    void (*Remove)(mutex *);
    void (*Delete)(mutex *);
    void (*Own)(mutex *, msgport *);
    void (*DisOwn)(mutex *);
};
#define PERM_NONE	((byte)0)
#define PERM_READ	((byte)1)
#define PERM_WRITE	((byte)2)


/* module */

struct module {
    uldat Id;
    fn_module *Fn;
    module *Prev, *Next; /* in the same All */
    all *All;
    uldat NameLen, Used;
    byte *Name;
    void *Handle, *Private;
};
struct fn_module {
    uldat Magic, Size, Used;
    module *(*Create)(fn_module *, uldat NameLen, CONST byte *Name);
    module *(*Copy)(module *From, module *To);
    void (*Insert)(module *, all *, module *Prev, module *Next);
    void (*Remove)(module *);
    void (*Delete)(module *);
    byte (*DlOpen)(module *);
    void (*DlClose)(module *);
};




struct display_hw {
    uldat Id;
    fn_display_hw *Fn;
    display_hw *Prev, *Next; /* in the same All */
    all *All;
    uldat NameLen;
    byte *Name;
    module *Module;

    void *Private;	/* used to store HW-specific data */
    
    void (*FlushVideo)(void);
    void (*FlushHW)(void);

    void (*KeyboardEvent)(int fd, display_hw *hw);
    void (*MouseEvent)(int fd, display_hw *hw);
    
    void (*ShowMouse)(void);
    void (*HideMouse)(void);
    void (*UpdateMouseAndCursor)(void);

    /* just detect size */
    void (*DetectSize)(udat *x, udat *y);

    /* check if size (x,y) is possible. if not, decrease (x,y) to the nearest possible size */
    void (*CheckResize)(udat *x, udat *y);
    
    /* unconditionally resize to (x,y). it is guaranteed that CheckResize returned this (x,y) */
    void (*Resize)(udat x, udat y);
    
    byte (*HWSelectionImport)(void);
    void (*HWSelectionExport)(void);
    void (*HWSelectionRequest)(obj *Requestor, uldat ReqPrivate);
    void (*HWSelectionNotify)(uldat ReqPrivate, uldat Magic,
			      CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data);
    void *HWSelectionPrivate;
	
    byte (*CanDragArea)(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);
    void (*DragArea)(dat Xstart, dat Ystart, dat Xend, dat Yend, dat DstXstart, dat DstYstart);
    /*
     * if the display HW is capable of doing BLiTs (BLock image Transfers) under
     * certain conditions (tipically X11's XCopyArea()), set canDragArea to a
     * function that checks if the HW can do the BLiT with the given coordinates,
     * and set DragArea to a function that unconditionally does the BLiT
     * (it will very probably need to FlushVideo() first)
     */

    void (*Beep)(void);
    void (*Configure)(udat resource, byte todefault, udat value);
    void (*SetPalette)(udat N, udat R, udat G, udat B);
    void (*ResetPalette)(void);

    void (*QuitHW)(void);
    void (*QuitKeyboard)(void);
    void (*QuitMouse)(void);
    void (*QuitVideo)(void);

    byte DisplayIsCTTY;
    /*
     * set to TRUE if display is the controlling terminal
     */

    byte Quitted;
    /*
     * used internally... is set to TRUE before InitHW() and after QuitHW()
     */
    
    byte FlagsHW;
    /*
     * various display HW flags:
     * 
     * FlHWSoftMouse		: set if display HW has to manually hide/show the mouse pointer
     * FlHWChangedMouseFlag	: set after a mouse event that requires redrawing mouse pointer
     * FlHWNeedOldVideo		: set if FlushVideo() is a bit expensive, and it's better to cache
     *				  the actual display contents in OldVideo[] and send only
     *				  what effectively changed, instead of all the dirty areas.
     * FlHWExpensiveFlushVideo	: set if FlushVideo() is SO expensive that it's better to sleep
     *				  a little before flushing,hoping to receive more data
     *				  in the meantime, in order to merge the flush operations.
     * FlHWNoInput		: set if the display HW should be used as view-only,
     * 				  ignoring all input from it.
     */
    
    byte NeedHW;
    /*
     * various runtime flags
     */

    byte CanResize;
    /*
     * set to TRUE if the display can actually resize itself (example: X11)
     * set to FALSE if it can only live with the externally set size (example: ttys)
     */
    
    byte RedrawVideo;
    /*
     * set to TRUE if the display was corrupted by some external event
     * example: hw_X11.c sets this when its window gets Expose events
     */
    udat RedrawLeft, RedrawUp, RedrawRight, RedrawDown;
    /*
     * the corrupted area that needs to be redrawn.
     * 
     * the upper layer (i.e. hw.c) automagically updates
     * ChangedVideoFlag and ChangedVideo[] to include this area
     * for your display.
     */
    
    uldat keyboard_slot, mouse_slot;

    mouse_state MouseState;

    udat X, Y;
    /*
     * real display size, in character cells.
     */
    
    udat usedX, usedY;
    /*
     * used display size (i.e. ScreenWidth, ScreenHeight)
     */
    
    dat Last_x, Last_y;
    /*
     * position of last mouse event
     */

    uldat merge_Threshold;
    /*
     * if sending many small draw commands to the HW is more expensive
     * than sending fewer, bigger ones even considering you will also send
     * intermediate, clean areas, set merge_Threshold to a reasonable value
     * for merge: dirty areas less far than this will be considered as merged
     * by Threshold_isDirtyVideo().
     * Anyway, it's up to your FlushVideo() to actually merge them.
     * 
     * Otherwise, set this to zero.
     */
    
    uldat AttachSlot; /* slot of client that told us to attach to this display */
    
    udat XY[2]; /* hw-dependent cursor position */
    uldat TT;   /* hw-dependent cursor type */
    
};
struct fn_display_hw {
    uldat Magic, Size, Used;
    display_hw *(*Create)(fn_display_hw *, uldat NameLen, CONST byte *Name);
    display_hw *(*Copy)(display_hw *From, display_hw *To);
    void (*Insert)(display_hw *, all *, display_hw *Prev, display_hw *Next);
    void (*Remove)(display_hw *);
    void (*Delete)(display_hw *);
    byte (*Init)(display_hw *);
    void (*Quit)(display_hw *);
};

/* FlagsHW */
#define FlHWSoftMouse		((byte)0x01)
#define FlHWChangedMouseFlag	((byte)0x02)
#define FlHWNeedOldVideo	((byte)0x04)
#define FlHWExpensiveFlushVideo	((byte)0x08)
#define FlHWNoInput		((byte)0x10)

/* NEEDOpHW */
#define NEEDFlushStdout		((byte)0x01)
#define NEEDFlushHW		((byte)0x02)
#define NEEDResizeDisplay	((byte)0x04)
#define NEEDSelectionExport	((byte)0x08)
#define NEEDPanicHW		((byte)0x10)
#define NEEDPersistentSlot	((byte)0x20)
#define NEEDFromPreviousFlushHW	((byte)0x40)
#define NEEDBeepHW		((byte)0x80)

    
    
#define NOMEMORY	((udat)1)
#define DLERROR		((udat)2)
#define SYSCALLERROR	((udat)3)
#define USERERROR	((udat)4)

#define NOID		((uldat)0)
#define MAXID		((uldat)0x0FFFFFFFul)
#define magic_mask	((uldat)0xF0000000ul)
#define magic_shift	28

#define obj_magic	((uldat)0x1dead0b1ul)
#define area_magic	((uldat)0x2feed976ul)
#define area_win_magic	((uldat)0x3a5ea507ul)
#define row_magic	((uldat)0x48074ffaul)
#define gadget_magic	((uldat)0x59867551ul)
#define window_magic	((uldat)0x61357531ul)
#define menuitem_magic	((uldat)0x7abc8fdeul)
#define menu_magic	((uldat)0x8bad0bedul)
#define screen_magic	((uldat)0x92659871ul)
#define msg_magic	((uldat)0xA3a61ce4ul)
#define msgport_magic	((uldat)0xB0981437ul)
#define mutex_magic	((uldat)0xC0faded0ul)
#define module_magic	((uldat)0xDb0f1278ul)
#define display_hw_magic	((uldat)0xEdbcc609ul)

#define magic_n		15 /* # of the above ones + 1 */

/* in the same order as the #defines above ! */
struct fn {
    fn_obj *f_obj;
    fn_area *f_area;
    fn_area_win *f_area_win;
    fn_row *f_row;
    fn_gadget *f_gadget;
    fn_window *f_window;
    fn_menuitem *f_menuitem;
    fn_menu *f_menu;
    fn_screen *f_screen;
    fn_msg *f_msg;
    fn_msgport *f_msgport;
    fn_mutex *f_mutex;
    fn_module *f_module;
    fn_display_hw *f_display_hw;
};

struct setup {
    dat MaxMouseSnap;
    udat MinAllocSize;
    byte Flags;
    byte SelectionButton, PasteButton;
    byte DeltaXShade, DeltaYShade;
};
/* All->Setup->Flags */
#define SETUP_SHADOWS		0x01
#define SETUP_BLINK		0x02
#define SETUP_ALWAYSCURSOR	0x04
#define SETUP_HIDEMENU		0x08
#define SETUP_MENUINFO		0x10
#define SETUP_EDGESCROLL	0x20
#define SETUP_ALTFONT		0x40


#define MAX_XSHADE	9
#define MAX_YSHADE	9

/*
 * values of All->State.
 * This order is hardcoded in methods.c:UnMapWindow(),
 * in resize.c:ExecScrollFocusWindow(),
 * in scroller.c:ScrollerH(),
 * do not change it!
 */
/*#define STATE_BUTTON(n)	((byte)(n)) */
#define STATE_RESIZE		((byte)10)
#define STATE_DRAG		((byte)11)
#define STATE_SCROLL		((byte)12)
#define STATE_GADGET		((byte)13)
#define STATE_MENU		((byte)14)
#define STATE_SCREEN		((byte)15)
#define STATE_SCREENBUTTON	((byte)16)
#define STATE_ROOT		((byte)17)
#define STATE_DEFAULT		((byte)18)

/* mask for all the above */
#define STATE_ANY		((byte)0x1f)

/* furher All->State flags */
#define STATE_FL_BYMOUSE	((byte)0x40)
#define STATE_FL_FREEZE		((byte)0x80)


/* values returned by FnWindow->FindBorder (modeled after STATE_*) */
/*#define POS_BUTTON(n)		((byte)(n))*/
#define POS_TITLE		((byte)10)
#define POS_CORNER		((byte)11)
#define POS_SIDE		((byte)12)

#define POS_BAR_BACK		((byte)13)
#define POS_BAR_FWD		((byte)14)
#define POS_TAB			((byte)15)
#define POS_ARROW_BACK		((byte)16)
#define POS_ARROW_FWD		((byte)17)

#define POS_INSIDE		((byte)18)
#define POS_MENU		((byte)19)
#define POS_SCREENBUTTON	((byte)21)
#define POS_ROOT		((byte)22)

typedef struct selection {
    timevalue Time;
    msgport *Owner;
    display_hw *OwnerOnce;
    uldat Magic;
    byte MIME[MAX_MIMELEN];
    uldat Len, Max;
    byte *Data;
} selection;

typedef struct button_vec {
    byte shape[2];
    num pos;
    byte exists;
    byte changed;
} button_vec;


struct all {
    screen *FirstScreen, *LastScreen;
    msgport *FirstMsgPort, *LastMsgPort, *RunMsgPort;
    mutex *FirstMutex, *LastMutex;
    module *FirstModule, *LastModule;
    fn_hook FnHookModule; window *HookModule;
    display_hw *FirstDisplayHW, *LastDisplayHW, *MouseHW, *ExclusiveHW;
    fn_hook FnHookDisplayHW; window *HookDisplayHW;
    byte State;
    timevalue Now;
    selection *Selection;
    setup *SetUp;
    void (*AtQuit)(void);

    menu *BuiltinMenu, *CommonMenu;
    
    button_vec ButtonVec[BUTTON_MAX];
    
    byte *Gtranslations[IBMPC_MAP];
};


/*
 La struttura di GlobalKeyCodes[i] e GlobalMouseCodes[i]
 (un puntatore ciascuno per ogni stato di movimento)
 e' esattamente la stessa presente
 nel campo KeyCodes di menu.
 */

/************** Keys **************/

#define NUM_LOCK	(byte)0x1
#define CAPS_LOCK	(byte)0x2
#define RIGHT_SHIFT	(byte)0x4
#define LEFT_SHIFT	(byte)0x8

#define RIGHT_CTRL	(byte)0xC
#define	LEFT_CTRL	(byte)0xD

#define RIGHT_ALT	(byte)0xE
#define LEFT_ALT	(byte)0xF

#define MAX_SHIFT_FLAG	(byte)0x10


#define ENTER      	((udat)'\r')
#define ESCAPE     	((udat)'\033')

#define HOLD		((udat)1)
#define HOLD_LEFT	((udat)1)
#define HOLD_MIDDLE	((udat)2)
#define HOLD_RIGHT	((udat)4)
#define HOLD_ANY	(HOLD_LEFT|HOLD_MIDDLE|HOLD_RIGHT)
#define HOLD_CODE(n)	(HOLD << (n)) /* n is 0,1,2 */

#define PRESS_LEFT	((udat)0x08)
#define PRESS_MIDDLE	((udat)0x18)
#define PRESS_RIGHT	((udat)0x28)
#define PRESS_ANY	((udat)0x38)
#define PRESS_CODE(n)	((udat)0x08 | ((udat)(n) << 4)) /* n is 0,1,2 */

#define DOWN_LEFT	(HOLD_LEFT|PRESS_LEFT)
#define DOWN_MIDDLE	(HOLD_MIDDLE|PRESS_MIDDLE)
#define DOWN_RIGHT	(HOLD_RIGHT|PRESS_RIGHT)
#define DOWN_ANY	(HOLD_ANY|PRESS_ANY)
#define DOWN_CODE(n)	(HOLD_CODE(n)|PRESS_CODE(n)) /* n is 0,1,2 */

#define RELEASE_LEFT	((udat)0x10)
#define RELEASE_MIDDLE	((udat)0x20)
#define RELEASE_RIGHT	((udat)0x30)
#define RELEASE_ANY	((udat)0x30)
#define RELEASE_CODE(n)	((udat)0x10 + ((udat)(n) << 4)) /* n is 0,1,2 */

#define DRAG_MOUSE	((udat)0x40)

#define ANY_ACTION_MOUSE	(PRESS_ANY | RELEASE_ANY | DRAG_MOUSE)

#define MAX_MOUSE_CODE	(udat)0x48

#define isPRESS(code)	((code) & 0x08)
#define isDRAG(code)	((code) & DRAG_MOUSE)
#define isRELEASE(code)	((code) & ANY_ACTION_MOUSE && !((code) & (DRAG_MOUSE|(udat)0x08)))

#define isSINGLE_PRESS(code) (isPRESS(code) && ((code) == DOWN_LEFT || (code) == DOWN_MIDDLE || (code) == DOWN_RIGHT))
#define isSINGLE_DRAG(code) (isDRAG(code) && ((code) == (DRAG_MOUSE|HOLD_LEFT) || (code) == (DRAG_MOUSE|HOLD_MIDDLE) || (code) == (DRAG_MOUSE|HOLD_RIGHT)))
#define isSINGLE_RELEASE(code) (isRELEASE(code) && !((code) & HOLD_ANY))

/*
 * These macros can be used only for proper mouse codes.
 * The button numbers are: 0 (Left), 1 (Middle), 2 (Right)
 */

/* if (Code & HOLD_ANY) then HOLD_N(n) returns the lowest button # pressed in Code */
#define HOLD_N(Code)	(ffs(Code) - 1)

/* if (isSINGLE_PRESS(Code)) then PRESS_N(n) return the button # pressed in Code */
#define PRESS_N(Code)	((Code) >> 4)

/* if (isSINGLE_DRAG(Code)) then DRAG_N(n) return the button # pressed in Code */
#define DRAG_N(Code)	HOLD_N(Code)

/* if (isSINGLE_RELEASE(Code)) then RELEASE_N(n) return the button # released in Code */
#define RELEASE_N(Code) (((Code) >> 4) - 1)


/**********************************/

/* Some common menu rows codes: */

#define COD_CANCEL		0xF700
#define COD_OK			0xF701
#define COD_HELP		0xF702
#define COD_OPEN		0xF703
#define COD_DELETE		0xF704
#define COD_NEW			0xF705

/* don't use codes above or equal to this one! */
#define COD_RESERVED		0xF800

/* INLINE/define stuff: */

#ifdef DEBUG_MALLOC
  /*
   * with the current (0.3.5) MkDep, DEBUG_MALLOC gets defined only if doing
   * `make DEBUG_MALLOC=1 ...' and the C file that includes twin.h actually
   * checks for DEBUG_MALLOC. Anyway, this is acceptable :-)
   */
  extern byte *S;
  extern byte *E;
  void panic_free(void *v);
# define FAIL(v) ((v) && ((byte *)(v) < S || (byte *)(v) > E) ? (panic_free(v), TRUE) : FALSE)
#endif /* DEBUG_MALLOC */

#ifdef CONF__ALLOC
  byte InitAlloc(void);
  void *AllocStatHighest(void);
  void *AllocMem(size_t Size);
  void FreeMem(void *Mem);
  void *ReAllocMem(void *Mem, size_t Size);
#else /* CONF__ALLOC */

# define AllocMem(Size)		malloc(Size)
# ifdef DEBUG_MALLOC
   void FreeMem(void *Mem);
# else
#  define FreeMem(Mem)		free(Mem)
# endif

# ifndef USE_MY_REALLOC
#  define ReAllocMem(Mem, Size)	realloc((Mem), (Size))
# else /* USE_MY_REALLOC */
INLINE void *ReAllocMem(void *Mem, uldat Size) {
    void *res = (void *)0;
    if (Size) {
	if (Mem) {
	    if ((res = realloc(Mem, Size)))
		return res;
	    if ((res = AllocMem(Size))) {
		CopyMem(Mem, res, Size);
		FreeMem(Mem);
		return res;
	    }
	    return res;
	}
	FreeMem(Mem);
	return res;
    }
    if (Size)
	return malloc(Size);
    return res;
}
# endif /* USE_MY_REALLOC */
#endif /* CONF__ALLOC */

# define LenStr(S) strlen(S)
# define CmpStr(S1, S2) strcmp((S1), (S2))
# define CopyStr(From,To) strcpy((To),(From))

# define CopyMem(From, To, Size)	memcpy((To), (From), (size_t)(Size))
# define MoveMem(From, To, Size)	memmove((To), (From), (size_t)(Size))
# define WriteMem(Mem, Char, Size)	memset((Mem), (int)(Char), (size_t)(Size))
# define CmpMem(m1, m2, Size)		memcmp((m1), (m2), (size_t)(Size))


# define DropPrivileges() (setegid(getgid()), seteuid(getuid()))
# define GetRootPrivileges() seteuid(0)
# define GetGroupPrivileges(g) setegid(g)

byte *CloneStr(CONST byte *s);

#endif /* _TWIN_H */

