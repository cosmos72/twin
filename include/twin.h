/*
 *  twin.h  --  main include for all twin-related data types, functions and macros
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_H
#define _TWIN_H

#ifndef RETSIGTYPE
# include "autoconf.h"
#endif

/* pull in "obj" field in <Tw/stattypes.h> */
#define obj obj
typedef struct s_obj *obj;

#include "compiler.h"
#include "version.h"
#include "osincludes.h"
#include <Tw/compiler.h>
#include <Tw/datatypes.h>
#include <Tw/datasizes.h>
#include <Tw/stattypes.h>
#include <Tw/endianity.h>
#include <Tw/uni_types.h>
#include <Tw/version.h>
#include <Tw/missing.h>
#include <Tw/mouse.h>


#define Abs(x) ((x)>0 ? (x) : -(x))
#define Swap(a, b, tmp) ((tmp)=(a), (a)=(b), (b)=(tmp))
#define Min2(x, y) ((x)<(y) ? (x) : (y))
#define Max2(x, y) ((x)>(y) ? (x) : (y))
#define Sign(n) ((n)>0 ? 1 : (n)<0 ? -1 : 0)

#define Max3(x, y, z) ((x)>(y) ? Max2(x,z) : Max2(y,z))
#define Min3(x, y, z) ((x)<(y) ? Min2(x,z) : Min2(y,z))

/***************/

#define NOFD   (-1)
#define specFD   (-2)	/* use for every FD that needs a special RemoteFlush()
			 * instead of a plain write() and set PrivateFlush as needed */

#ifndef NOPID		/* some OSes (Solaris for one) already define NOPID */
# define NOPID  ((pid_t)0)
#endif
#define NOSLOT MAXULDAT

#ifndef FALSE
# define FALSE	((byte)0)
# define TRUE	(!FALSE)
#endif

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



/* return from signal macros */

#define RETFROMSIGNAL(value)		return ARG_IFNOTVOID(value, RETSIGTYPE)
#define ARG_IFNOTVOID(value, type)	CAT(ARG_IFNOTVOID_,type)(value)
#define ARG_IFNOTVOID_void(value)
#define ARG_IFNOTVOID_int(value)	(value)



/* maximum number of arguments of a libTw function */
#define TW_MAX_ARGS_N	20


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

#define MAXHWCOL	COL(MAXCOL,MAXCOL)

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


/* if sizeof(hwattr) == 4, bytes are { 'ascii_low', 'col', 'ascii_high', 'extra' } */

/* hwattr <-> hwcol+hwfont conversion */
# define HWATTR32(col,ascii) (((byte32)(byte)(col) << 8) | (((byte32)(ascii) & 0xFF00) << 8) | (byte32)(byte)(ascii))
# define HWATTR_COLMASK32(attr) ((attr) & 0xFF00)
# define HWATTR_FONTMASK32(attr) ((attr) & 0xFF00FF)
# define HWCOL32(attr) ((hwcol)((attr) >> 8))
# define HWFONT32(attr) ((byte16)(((attr) & 0xFF) | (((attr) >> 8) & 0xFF00)))

# define HWATTR_EXTRA32(attr,extra) (((byte32)(byte)(extra) << 24) | ((byte32)(attr) & 0xFFFFFF))
# define HWATTR_EXTRAMASK32(attr) ((attr) & 0xFF000000)
# define HWEXTRA32(attr) ((byte)((attr) >> 24))

/*
 * Notes about the timevalue struct:
 * 
 * it is used to represent both time intervals and absolute times;
 * the ->Seconds is a tany numeric field.
 * DON'T assume time_t is 32 bit (or any other arbitrary size)
 * since in 19 Jan 2038 at 04:14:08 any signed, 32 bit tany will overflow.
 * So use sizeof(tany) if you really need.
 * 
 * the ->Fraction is a tany numeric field (tany is unsigned).
 * As above, DON'T assume tany is 32 bit (or any other arbitrary size)
 * since in the future we may want a finer granularity than the nanosecond one
 * possible with a 32 bit tany.
 * So :
 * 1) use sizeof(tany) if you really need
 * 2) don't assume (tany)1 is a nanosecond (or any other arbitrary time),
 *    but always use the form '1 NanoSECs', '250 MilliSECs + 7 MicroSECs', etc.
 * 3) if you _absolutely_ need to know to what time (tany)1 corresponds,
 *    use this: '1 FullSECs' is the number of (tany)1 intervals in a second.
 * 4) for the moment, the only defined fractions of a second are:
 *    FullSECs, MilliSECs, MicroSECs, NanoSECs.
 *    Others may be added in the future (PicoSECs, FemtoSECs, AttoSECs, ...)
 */

typedef struct s_timevalue  {
    tany Seconds;
    tany Fraction;
} timevalue;

#define THOUSAND	((tany)1000)

#define NanoSECs	* 1 /* i.e. (frac_t)1 is a nanosecond */
#define MicroSECs	* (THOUSAND NanoSECs)
#define MilliSECs	* (THOUSAND MicroSECs)
#define FullSECs	* (THOUSAND MilliSECs)

#define ABS(x) ((x)>0 ? (x) : -(x))

#define XAND(Bits, Mask)  (( (Bits) & (Mask) ) == (Mask) )

typedef struct s_font {
    byte AsciiCode;
    byte *Bitmap;
} font;

typedef struct s_palette {
    byte Red, Green, Blue;
} palette;

typedef struct s_mouse_state {
    dat x, y;
    dat delta_x, delta_y;
    byte keys;
} mouse_state;

typedef struct s_ttydata ttydata;
typedef struct s_remotedata remotedata;
typedef struct s_draw_ctx draw_ctx;

/*typedef struct s_obj *obj;*/
typedef struct s_fn_obj *fn_obj;
typedef struct s_obj_parent *obj_parent;
typedef struct s_widget *widget;
typedef struct s_fn_widget *fn_widget;
typedef struct s_gadget *gadget;
typedef struct s_fn_gadget *fn_gadget;
typedef struct s_window *window;
typedef struct s_fn_window *fn_window;
typedef struct s_screen *screen;
typedef struct s_fn_screen *fn_screen;
typedef struct s_group *group;
typedef struct s_fn_group *fn_group;
typedef struct s_row *row;
typedef struct s_fn_row *fn_row;
typedef struct s_menuitem *menuitem;
typedef struct s_fn_menuitem *fn_menuitem;
typedef struct s_menu *menu;
typedef struct s_fn_menu *fn_menu;
typedef struct s_msg *msg;
typedef struct s_fn_msg *fn_msg;
typedef struct s_msgport *msgport;
typedef struct s_fn_msgport *fn_msgport;
typedef struct s_mutex *mutex;
typedef struct s_fn_mutex *fn_mutex;
typedef struct s_module *module;
typedef struct s_fn_module *fn_module;
typedef struct s_extension *extension;
typedef struct s_fn_extension *fn_extension;
typedef struct s_display_hw *display_hw;
typedef struct s_fn_display_hw *fn_display_hw;

typedef struct s_fn fn;
typedef struct s_setup setup;
typedef struct s_all *all;

typedef void (*fn_hook)(widget);


/* ttydata->Flags */
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
#define TTY_RELORIG	((udat)0x0400)
#define TTY_SETMETA	((udat)0x0800)
#define TTY_UPDATECURSOR ((udat)0x1000)
#define TTY_REPORTMOUSE	 ((udat)0x2000)
#define TTY_REPORTMOUSE2 ((udat)0x4000)
#define TTY_NEEDREFOCUS	((udat)0x8000)

/* ttydata->Effects */
#define EFF_INTENSITY	((udat)0x0001)
#define EFF_HALFINTENS	((udat)0x0002)
#define EFF_UNDERLINE	((udat)0x0004)
#define EFF_BLINK	((udat)0x0008)
#define EFF_REVERSE	((udat)0x0010)

/* ttydata->nPar */
#define NPAR		16

/* ttydata->*G? */
#define GRAF_MAP	0 /*GRAF_MAP: in the range 0x80 - 0xFF it's identical to LAT1_MAP*/
#define LAT1_MAP	0
#define IBMPC_MAP	1
#define USER_MAP	2

typedef enum ttystate {
    ESnormal = 0, ESesc, ESsquare, ESgetpars, ESgotpars, ESfunckey,
      EShash, ESsetG0, ESsetG1, ESpercent, ESignore, ESnonstd,
      ESpalette, ESxterm_1_, ESxterm_1, ESxterm_2_, ESxterm_2, ESany = 0xFF, ESques = 0x100
} ttystate;

struct s_ttydata {
    ttystate State;
    udat Flags;
    udat Effects;
    dat ScrollBack;	/* Number of scrollback lines */
    dat SizeX, SizeY;	/* Terminal size */
    dat Top, Bottom;	/* Y scrolling region. default 0...SizeY-1 */
    dat X, Y;		/* Cursor position in visible buffer */
    dat saveX, saveY;
    hwattr *Start, *Split;/* Start and Split of visible buffer */
    			  /* AfterSplit is just Window->Contents */
    hwattr *Pos;	  /* Pointer to cursor position in buffer */
    hwcol Color, DefColor, saveColor, Underline, HalfInten;
    uldat TabStop[5];
    uldat nPar, Par[NPAR];
    
    byte currG, G, G0, G1, saveG, saveG0, saveG1;
    byte utf, utf_count;
    hwfont utf_char;
    void *InvCharset;	/* pointer to hwfont -> byte translation function */
    
    dat newLen, newMax;
    byte *newName;	/* buffer for xterm set window title escape seq */
};



struct s_remotedata {
    int Fd;
    pid_t ChildPid;
    uldat FdSlot; /* index in the FdList array (remote.c) */
};

struct s_draw_ctx {
    draw_ctx *Next;
    screen Screen;
    widget TopW;
    widget W;
    widget OnlyW;
    ldat Left, Up, Rgt, Dwn; /* widget corners position on Screen */
    ldat X1, Y1, X2, Y2;     /* screen area to draw */
    dat DWidth;
    dat DHeight;
    byte NoChildren;
    byte BorderDone;
    byte Shaded;
};




struct s_obj {
    uldat Id;
    fn_obj Fn;
    obj Prev, Next, Parent;
};

struct s_obj_parent {
    obj First, Last;
};

struct s_fn_obj {
    uldat Magic, Size, Used;
    obj (*Create)(fn_obj);
    void (*Insert)(obj Obj, obj, obj Prev, obj Next);
    void (*Remove)(obj);
    void (*Delete)(obj);
    void (*ChangeField)(obj, udat field, uldat CLEARMask, uldat XORMask);
};


struct s_wE {		/* for WIDGET_USEEXPOSE widgets */
    union {
	CONST byte *Text;
	CONST hwfont *HWFont;
	CONST hwattr *HWAttr;
    } E;
    dat Flags, Pitch;
    ldat X1, Y1, X2, Y2;
};

/* WIDGET_USEEXPOSE  USE.E.Flags: */
#define WIDGET_USEEXPOSE_TEXT 1
#define WIDGET_USEEXPOSE_HWFONT 2
#define WIDGET_USEEXPOSE_HWATTR 4

struct s_widget {
    uldat Id;
    fn_widget Fn;
    widget Prev, Next;/* list in the same parent */
    widget Parent;	/* where this widget sits */
    /* widget */
    widget FirstW, LastW; /* list of children */
    widget SelectW;	    /* selected child */
    dat Left, Up, XWidth, YWidth;
    uldat Attrib;
    uldat Flags;
    ldat XLogic, YLogic;
    widget O_Prev, O_Next; /* list with the same msgport (owner) */
    msgport Owner;
    fn_hook ShutDownHook; /* hooks for this widget */
    fn_hook Hook, *WhereHook;
    fn_hook MapUnMapHook;
    msg MapQueueMsg;
    hwattr USE_Fill;
    union {
	struct s_wE E;
    } USE;
};

struct s_fn_widget {
    uldat Magic, Size, Used;
    widget (*Create)(fn_widget, msgport Owner, dat XWidth, dat YWidth, uldat Attrib, uldat Flags, dat Left, dat Up, hwattr USE_Fill);
    void (*Insert)(widget, widget Parent, widget Prev, widget Next);
    void (*Remove)(widget);
    void (*Delete)(widget);
    void (*ChangeField)(widget, udat field, uldat CLEARMask, uldat XORMask);
    /* widget */
    fn_obj Fn_Obj; /* backup of overloaded functions */
    void (*DrawSelf)(draw_ctx *D);
    widget (*FindWidgetAt)(widget Parent, dat X, dat Y);
    gadget (*FindGadgetByCode)(widget Parent, udat Code);
    void (*SetXY)(widget, dat X, dat Y);
    void (*SetFill)(widget, hwattr Fill);
    widget (*Focus)(widget);
    widget (*KbdFocus)(widget);
    void (*Map)(widget, widget Parent);
    void (*UnMap)(widget);
    void (*MapTopReal)(widget, screen);
    void (*Raise)(widget);
    void (*Lower)(widget);
    void (*Own)(widget, msgport);
    void (*DisOwn)(widget);    
    void (*RecursiveDelete)(widget, msgport);
    void (*Expose)(widget, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, CONST byte *, CONST hwfont *, CONST hwattr *);
    byte (*InstallHook)(widget, fn_hook, fn_hook *Where);
    void (*RemoveHook)(widget, fn_hook, fn_hook *Where);
};

/* Widget->Attrib */
/*
 * ask the server to send events even for mouse motion without any pressed button.
 * works only if WIDGET_WANT_MOUSE is set too.
 */
#define WIDGET_WANT_MOUSE_MOTION	0x0001
#define WIDGET_WANT_KEYS	0x0002
#define WIDGET_WANT_MOUSE	0x0004
#define WIDGET_WANT_CHANGES	0x0008
#define WIDGET_AUTO_FOCUS	0x0010


/* Widget->Flags */
#define WIDGETFL_USEEXPOSE	0x02
#define WIDGETFL_USEFILL	0x03
#define WIDGETFL_USEANY		0x07 /* mask of all above ones */

#define WIDGETFL_NOTVISIBLE	0x8000

#define w_USE(w, USExxx)	(((w)->Flags & WIDGETFL_USEANY) == CAT(WIDGETFL_,USExxx))


struct s_gT {		/* for GADGETFL_USETEXT gadgets */
    hwfont *Text[4];
    hwcol *Color[4];
};

struct s_gadget {
    uldat Id;
    fn_gadget Fn;
    widget Prev, Next;
    widget Parent;
    /* widget */
    widget FirstW, LastW;/* list of children */
    widget SelectW;	 /* selected child */
    dat Left, Up, XWidth, YWidth;
    uldat Attrib;
    uldat Flags;
    ldat XLogic, YLogic;
    widget O_Prev, O_Next; /* list in the same msgport (owner) */
    msgport Owner;
    fn_hook ShutDownHook; /* hooks for this widget */
    fn_hook Hook, *WhereHook;
    fn_hook MapUnMapHook;
    msg MapQueueMsg;
    hwattr USE_Fill;
    union {
	struct s_gT T;
	struct s_wE E;
    } USE;
    /* gadget */
    hwcol ColText, ColSelect, ColDisabled, ColSelectDisabled;
    udat Code;
    gadget G_Prev, G_Next; /* list in the same group */
    group Group;
};

struct s_fn_gadget {
    uldat Magic, Size, Used;
    gadget (*Create)
	(fn_gadget, msgport Owner, widget Parent, dat XWidth, dat YWidth,
	 CONST byte *TextNormal, uldat Attrib, uldat Flags, udat Code,
	 hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
	 dat Left, dat Up);
    void (*Insert)(gadget, widget Parent, widget Prev, widget Next);
    void (*Remove)(gadget);
    void (*Delete)(gadget);
    void (*ChangeField)(gadget, udat field, uldat CLEARMask, uldat XORMask);
    /* widget */
    fn_obj Fn_Obj;
    void (*DrawSelf)(draw_ctx *D);
    widget (*FindWidgetAt)(gadget Parent, dat X, dat Y);    
    gadget (*FindGadgetByCode)(gadget Parent, udat Code);
    void (*SetXY)(gadget, dat X, dat Y);
    void (*SetFill)(widget, hwattr Fill);
    widget (*Focus)(gadget);
    widget (*KbdFocus)(gadget);
    void (*Map)(gadget, widget Parent);
    void (*UnMap)(gadget);
    void (*MapTopReal)(gadget, screen);
    void (*Raise)(gadget);
    void (*Lower)(gadget);
    void (*Own)(gadget, msgport);
    void (*DisOwn)(gadget);    
    void (*RecursiveDelete)(gadget, msgport);
    void (*Expose)(gadget, dat XWidth, dat YWidth, dat Left, dat Up, CONST byte *, CONST hwfont *, CONST hwattr *);
    byte (*InstallHook)(gadget, fn_hook, fn_hook *Where);
    void (*RemoveHook)(gadget, fn_hook, fn_hook *Where);
    /* gadget */
    fn_widget Fn_Widget;
    gadget (*CreateEmptyButton)(fn_gadget Fn_Gadget, msgport Owner, dat XWidth, dat YWidth, hwcol BgCol);
    byte (*FillButton)(gadget Gadget, widget Parent, udat Code, dat Left, dat Up,
		       udat Flags, CONST byte *Text, hwcol Color, hwcol ColorDisabled);
    gadget (*CreateButton)(fn_gadget Fn_Gadget, widget Parent, dat XWidth, dat YWidth, CONST byte *Text,
			      uldat Flags, udat Code, hwcol BgCol, hwcol Col, hwcol ColDisabled,
			      dat Left, dat Up);
    void (*WriteTexts)(gadget Gadget, byte bitmap, dat XWidth, dat YWidth, CONST byte *Text, dat Left, dat Up);
    void (*WriteHWFonts)(gadget Gadget, byte bitmap, dat XWidth, dat YWidth, CONST hwfont *HWFont, dat Left, dat Up);
};

/* Gadget->Attrib */
#define GADGET_WANT_MOUSE_MOTION	WIDGET_WANT_MOUSE_MOTION /* 0x0001 */
#define GADGET_WANT_KEYS	WIDGET_WANT_KEYS	/* 0x0002 */
#define GADGET_WANT_MOUSE	WIDGET_WANT_MOUSE	/* 0x0004 */
#define GADGET_WANT_CHANGES	WIDGET_WANT_CHANGES	/* 0x0008 */
#define GADGET_AUTO_FOCUS	WIDGET_AUTO_FOCUS	/* 0x0010 */


/* Gadget->Flags */
#define GADGETFL_USETEXT	0x00   /* it's the default */
#define GADGETFL_USEEXPOSE	WIDGETFL_USEEXPOSE /* 0x02 */
#define GADGETFL_USEFILL	WIDGETFL_USEFILL   /* 0x03 */
#define GADGETFL_USEANY		WIDGETFL_USEANY    /* 0x07 */

#define G_USE(g, USExxx)	(((g)->Flags & GADGETFL_USEANY) == CAT(GADGETFL_,USExxx))

/* remember this gadget is a button, so that SetText() does not ruin the shadow */
#define GADGETFL_BUTTON		0x0010
#define GADGETFL_DISABLED	0x0020
#define GADGETFL_TEXT_DEFCOL	0x0040
/* this makes the gadget 'checkable' : can be in 'checked' or 'unchecked' state.
 * also necessary to put the gadget in a group */
#define GADGETFL_TOGGLE		0x0080
#define GADGETFL_PRESSED	0x0100

#define GADGETFL_NOTVISIBLE	0x8000


/*              NOTE :
 * 
 * the USE.T fields (Contents) of a gadget are used as follows:
 * Text[0]==TextNormal;      (mandatory)
 * Text[1]==TextSelect;        (if not present, use ...[0])
 * Text[2]==TextDisabled;      (if not present, use ...[0])
 * Text[3]==TextSelectDisabled; (if not present, use ...[1]; if ...[1] not present too, use ...[0])
 * 
 * Color[0]==ColorNormal;      (mandatory unless GADGET_USE_DEFCOL is set)
 * Color[1]==ColorSelect;        (if not present, use ...[0])
 * Color[2]==ColorDisabled;      (if not present, use ...[0])
 * Color[3]==ColorSelectDisabled;(if not present, use ...[1]; if ...[1] not present too, use ...[0])
 * 
 * If GADGET_USE_DEFCOL is set,
 * Color[0], [1], [2] and [3] are ignored.
 * 
 */


struct s_WR {		/* for WINDOWFL_USEROWS windows */
    row FirstRow, LastRow;
    row RowOne, RowSplit;	/*RESERVED: used to optimize the drawing on screen */
    ldat NumRowOne, NumRowSplit;/*RESERVED: updated automatically by WriteRow. To insert */
    /*or remove manually rows, you must zero out NumRowOne */
    /*and NumRowSplit forcing twin to recalculate them */
};

struct s_WC {		/* for WINDOWFL_USECONTENTS windows */
    hwattr *Contents;
    ttydata *TtyData;
    ldat HSplit;
};

struct s_window {
    uldat Id;
    fn_window Fn;
    widget Prev, Next;/* list in the same parent */
    widget Parent;	/* where this window sits */
    /* widget */
    widget FirstW, LastW; /* list of children */
    widget SelectW;	    /* selected child */
    dat Left, Up, XWidth, YWidth;
    uldat Attrib;
    uldat Flags;
    ldat XLogic, YLogic;
    widget O_Prev, O_Next; /* list with the same msgport (owner) */
    msgport Owner;
    fn_hook ShutDownHook; /* hooks for this widget */
    fn_hook Hook, *WhereHook;
    fn_hook MapUnMapHook;
    msg MapQueueMsg;
    hwattr USE_Fill;
    union {
	struct s_WR R;
	struct s_WC C;
	struct s_wE E;
    } USE;
    /* window */
    menu Menu;
    menuitem MenuItem; /* from which the window depends */
    dat NameLen;
    byte *Name; hwcol *ColName;
    hwfont *BorderPattern[2];
    remotedata RemoteData;
    ldat CurX, CurY;
    ldat XstSel, YstSel, XendSel, YendSel;
    hwcol ColGadgets, ColArrows, ColBars, ColTabs, ColBorder,
    	ColText, ColSelect, ColDisabled, ColSelectDisabled;
    uldat State;
    uldat CursorType;
    dat MinXWidth, MinYWidth;
    dat MaxXWidth, MaxYWidth;
    ldat WLogic, HLogic;	/* window interior logic size */
    hwfont CONST * Charset;	/* the byte -> hwfont translation to use */
};

struct s_fn_window {
    uldat Magic, Size, Used;
    window (*Create)(fn_window, msgport Owner, dat NameLen, CONST byte *Name, CONST hwcol *ColName,
		     menu Menu, hwcol ColText, uldat CursorType, uldat Attrib, uldat Flags,
		     dat XWidth, dat YWidth, dat ScrollBackLines);
    void (*Insert)(window, widget Parent, widget Prev, widget Next);
    void (*Remove)(window);
    void (*Delete)(window);
    void (*ChangeField)(window, udat field, uldat CLEARMask, uldat XORMask);
    /* widget */
    fn_obj Fn_Obj;
    void (*DrawSelf)(draw_ctx *D);
    widget (*FindWidgetAt)(window Parent, dat X, dat Y);
    gadget (*FindGadgetByCode)(window Parent, udat Code);
    void (*SetXY)(window, dat X, dat Y);
    void (*SetFill)(window, hwattr Fill);
    widget (*Focus)(window);
    widget (*KbdFocus)(window);
    void (*Map)(window, widget Parent);
    void (*UnMap)(window);
    void (*MapTopReal)(window, screen);
    void (*Raise)(window);
    void (*Lower)(window);
    void (*Own)(window, msgport);
    void (*DisOwn)(window);    
    void (*RecursiveDelete)(window, msgport);
    void (*Expose)(window, dat XWidth, dat YWidth, dat Left, dat Up, CONST byte *, CONST hwfont *, CONST hwattr *);
    byte (*InstallHook)(window, fn_hook, fn_hook *Where);
    void (*RemoveHook)(window, fn_hook, fn_hook *Where);
    /* window */
    fn_widget Fn_Widget;
    void (*TtyWriteAscii)(window, ldat Len, CONST byte *Ascii);
    void (*TtyWriteString)(window, ldat Len, CONST byte *String);
    void (*TtyWriteHWFont)(window, ldat Len, CONST hwfont *HWFont);
    void (*TtyWriteHWAttr)(window, dat x, dat y, ldat Len, CONST hwattr *Attr);

    byte (*RowWriteAscii)(window, ldat Len, CONST byte *Ascii);
    byte (*RowWriteString)(window, ldat Len, CONST byte *String);
    byte (*RowWriteHWFont)(window, ldat Len, CONST hwfont *HWFont);
    byte (*RowWriteHWAttr)(window, dat x, dat y, ldat Len, CONST hwattr *Attr);

    void (*GotoXY)(window, ldat X, ldat Y);
    void (*SetTitle)(window, dat titlelen, byte *title);
    void (*SetColText)(window, hwcol ColText);
    void (*SetColors)(window, udat Bitmap,
		      hwcol ColGadgets, hwcol ColArrows, hwcol ColBars, hwcol ColTabs, hwcol ColBorder,
		      hwcol ColText, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled);
    void (*Configure)(window, byte Bitmap, dat Left, dat Up, dat MinXWidth, dat MinYWidth,
		      dat MaxXWidth, dat MaxYWidth);
    window (*Create4Menu)(fn_window, menu);
    byte (*FindBorder)(window, dat u, dat v, byte Border, hwattr *PtrAttr);
    row (*FindRow)(window, ldat RowN);
    row (*FindRowByCode)(window, udat Code, ldat *NumRow);
};


/* Window->Attrib */
#define WINDOW_WANT_MOUSE_MOTION	WIDGET_WANT_MOUSE_MOTION /* 0x0001 */
#define WINDOW_WANT_KEYS	WIDGET_WANT_KEYS	/* 0x0002 */
#define WINDOW_WANT_MOUSE	WIDGET_WANT_MOUSE	/* 0x0004 */
#define WINDOW_WANT_CHANGES	WIDGET_WANT_CHANGES	/* 0x0008 */
#define WINDOW_AUTO_FOCUS	WIDGET_AUTO_FOCUS	/* 0x0010 */
#define WINDOW_DRAG		0x0100
#define WINDOW_RESIZE		0x0200
#define WINDOW_CLOSE		0x0400
#define WINDOW_ROLLED_UP	0x0800
#define WINDOW_X_BAR		0x1000
#define WINDOW_Y_BAR		0x2000
#define WINDOW_AUTO_KEYS	0x4000



/* Window->Flags */
#define WINDOWFL_USEROWS	0x00 /* it's the default */
#define WINDOWFL_USECONTENTS	0x01
#define WINDOWFL_USEEXPOSE	WIDGETFL_USEEXPOSE /* 0x02 */
#define WINDOWFL_USEFILL	WIDGETFL_USEFILL   /* 0x03 */
#define WINDOWFL_USEANY		WIDGETFL_USEANY    /* 0x07 */

#define W_USE(W, USExxx)	(((W)->Flags & WINDOWFL_USEANY) == CAT(WINDOWFL_,USExxx))

#define	WINDOWFL_CURSOR_ON	0x10
#define WINDOWFL_MENU		0x20
#define WINDOWFL_DISABLED	0x40
#define WINDOWFL_BORDERLESS	0x80
#define WINDOWFL_ROWS_INSERT	0x0100
#define WINDOWFL_ROWS_DEFCOL	0x0200
#define WINDOWFL_ROWS_SELCURRENT	0x0400
#define WINDOWFL_NOTVISIBLE	0x8000


/* Window->State */
#define X_BAR_SELECT		0x0001
#define Y_BAR_SELECT		0x0002
#define XY_BAR_SELECT		(X_BAR_SELECT | Y_BAR_SELECT)
#define TAB_SELECT		0x0004
#define PAGE_BACK_SELECT	0x0008
#define PAGE_FWD_SELECT		0x0010
#define ARROW_BACK_SELECT	0x0020
#define ARROW_FWD_SELECT	0x0040
#define SCROLL_ANY_SELECT  	(ARROW_BACK_SELECT | ARROW_FWD_SELECT | PAGE_BACK_SELECT | PAGE_FWD_SELECT | TAB_SELECT)

#define WINDOW_FWDSEL		0x0080
#define WINDOW_REVSEL		0x0100
#define WINDOW_ANYSEL		(WINDOW_FWDSEL|WINDOW_REVSEL)
#define WINDOW_DO_SEL		0x0200
#define WINDOW_GADGET_PRESSED	0x0400

#define BUTTON_FIRST_SELECT	0x00400000
#define BUTTON_LAST_SELECT	0x80000000
#define BUTTON_ANY_SELECT	0xFFC00000

/*#define BUTTON_FIRST		0 */
/*#define BUTTON_CLOSE		0 */
/*#define BUTTON_LAST		9 */

#define BUTTON_MAX		10



/* Window->CursorType */
/* These come from linux/drivers/char/console.c */
#define NOCURSOR	1
#define LINECURSOR	2
#define SOLIDCURSOR	8

/* window size limits */
#define MIN_XWIN	5
#define MIN_YWIN	2

struct s_sB {	/* for SCREENFL_USEBG screens */
    dat BgWidth, BgHeight;
    hwattr *Bg;
};

struct s_screen {
    uldat Id;
    fn_screen Fn;
    screen Prev, Next;/* list in the same All */
    widget dummyParent;/* NULL */
    /* widget */
    widget FirstW, LastW; /* list of children */
    widget FocusW;	    /* same as SelectW : focused child */
    dat dummyLeft, YLimit, dummyXWidth, dummyYWidth;
    uldat Attrib;
    uldat Flags;
    ldat XLogic, YLogic;
    widget O_Prev, O_Next; /* list with the same msgport (owner) */
    msgport Owner;
    fn_hook ShutDownHook; /* hooks for this widget */
    fn_hook Hook, *WhereHook;
    fn_hook MapUnMapHook;
    msg MapQueueMsg;
    hwattr USE_Fill;
    union {
	struct s_sB B;
	struct s_wE E;
    } USE;
    /* screen */
    dat NameLen;
    byte *Name;
    window MenuWindow, ClickWindow;
    all All;
    fn_hook FnHookW;/* allow hooks on children Map()/UnMap() inside this widget */
    widget HookW;
};
struct s_fn_screen {
    uldat Magic, Size, Used;
    screen (*Create)(fn_screen, dat NameLen, CONST byte *Name,
		     dat BgWidth, dat BgHeight, CONST hwattr *Bg);
    void (*Insert)(screen, all, screen Prev, screen Next);
    void (*Remove)(screen);
    void (*Delete)(screen);
    void (*ChangeField)(screen, udat field, uldat CLEARMask, uldat XORMask);
    /* widget */
    fn_obj Fn_Obj;
    void (*DrawSelf)(draw_ctx *D);
    widget (*FindWidgetAt)(screen Parent, dat X, dat Y);
    gadget (*FindGadgetByCode)(screen Parent, udat Code);
    void (*SetXY)(screen, dat X, dat Y);
    void (*SetFill)(screen, hwattr Fill);
    widget (*Focus)(screen);
    widget (*KbdFocus)(screen);
    void (*Map)(screen, widget Parent);
    void (*UnMap)(screen);
    void (*MapTopReal)(screen, screen);
    void (*Raise)(screen);
    void (*Lower)(screen);
    void (*Own)(screen, msgport);
    void (*DisOwn)(screen);    
    void (*RecursiveDelete)(screen, msgport);
    void (*Expose)(screen, dat XWidth, dat YWidth, dat Left, dat Up, CONST byte *, CONST hwfont *, CONST hwattr *);
    byte (*InstallHook)(screen, fn_hook, fn_hook *Where);
    void (*RemoveHook)(screen, fn_hook, fn_hook *Where);
    /* screen */
    fn_widget Fn_Widget;
    menu (*FindMenu)(screen);
    screen (*Find)(dat j);
    screen (*CreateSimple)(fn_screen, dat NameLen, CONST byte *Name, hwattr Bg);
    void (*BgImage)(screen, dat BgWidth, dat BgHeight, CONST hwattr *Bg);
    void (*DrawMenu)(screen, dat Xstart, dat Xend);
    void (*ActivateMenu)(screen, menuitem, byte ByMouse);
    void (*DeActivateMenu)(screen);
};


/* Screen->Attrib */
/* not used */


/* Screen->Flags */
#define SCREENFL_USEBG		0x00 /* it's the default */
#define SCREENFL_USEEXPOSE	WIDGETFL_USEEXPOSE	/* 0x02 */
#define SCREENFL_USEFILL	WIDGETFL_USEFILL	/* 0x03 */
#define SCREENFL_USEANY		WIDGETFL_USEANY		/* 0x07 */

#define S_USE(s, USExxx)	(((s)->Flags & SCREENFL_USEANY) == CAT(SCREENFL_,USExxx))

#define SCREENFL_BACK_SELECT	0x0010
#define SCREENFL_BACK_PRESSED	0x0020
#define SCREENFL_NOTVISIBLE	0x8000


/* group -- gadget group */


struct s_group {
    uldat Id;
    fn_group Fn;
    group Prev, Next;/* list in the same msgport */
    msgport MsgPort;
    /* group */
    gadget FirstG, LastG; /* list in this group */
    gadget SelectG;
};

struct s_fn_group {
    uldat Magic, Size, Used;
    group (*Create)(fn_group, msgport Parent);
    void (*Insert)(group, msgport MsgPort, group Prev, group Next);
    void (*Remove)(group);
    void (*Delete)(group);
    void (*ChangeField)(group, udat field, uldat CLEARMask, uldat XORMask);
    /* group */
    fn_obj Fn_Obj; /* backup of overloaded functions */
    void (*InsertGadget)(group, gadget);
    void (*RemoveGadget)(group, gadget);
    gadget (*GetSelectedGadget)(group);
    void   (*SetSelectedGadget)(group, gadget);
};



/* row */


struct s_row {
    uldat Id;
    fn_row Fn;
    row Prev, Next;
    window Window;
    /* row */
    udat Code;
    byte Flags;
    uldat Len, MaxLen;
    uldat Gap, LenGap;
    hwfont *Text;
    hwcol *ColText;
};

struct s_fn_row {
    uldat Magic, Size, Used;
    row (*Create)(fn_row, udat Code, byte Flags);
    void (*Insert)(row, window, row Prev, row Next);
    void (*Remove)(row);
    void (*Delete)(row);
    void (*ChangeField)(row, udat field, uldat CLEARMask, uldat XORMask);
    /* row */
    fn_obj Fn_Obj;
    byte (*SetText)(row, ldat Len, CONST byte *Text, byte DefaultCol);
    byte (*SetHWFont)(row, ldat Len, CONST hwfont *HWFont, byte DefaultCol);
    void (*Raise)(row);
    void (*Lower)(row);
};

/*Flags : */
#define ROW_INACTIVE	((byte)0x00)
#define ROW_ACTIVE	((byte)0x01)
#define ROW_IGNORE	((byte)0x02)
#define ROW_DEFCOL	((byte)0x04)




struct s_menuitem {
    uldat Id;
    fn_menuitem Fn;
    menuitem Prev, Next;
    obj Parent;
    /* row */
    udat Code;
    byte Flags;
    uldat Len, MaxLen;
    uldat Gap, LenGap;
    hwfont *Text;
    hwcol *ColText;
    /* menuitem */
    window Window;
    dat Left, ShortCut;
    ldat WCurY; 
};
struct s_fn_menuitem {
    uldat Magic, Size, Used;
    menuitem (*Create)(fn_menuitem, obj Parent, window Window, udat Code, byte Flags,
		       dat Left, ldat Len, dat ShortCut, CONST byte *Name);
    void (*Insert)(menuitem, obj, menuitem Prev, menuitem Next);
    void (*Remove)(menuitem);
    void (*Delete)(menuitem);
    void (*ChangeField)(menuitem, udat field, uldat CLEARMask, uldat XORMask);
    /* row */
    fn_obj Fn_Obj;
    byte (*SetText)(row, ldat Len, CONST byte *Text, byte DefaultCol);
    byte (*SetHWFont)(row, ldat Len, CONST hwfont *HWFont, byte DefaultCol);
    /* menuitem */	
    void (*Raise)(menuitem);
    void (*Lower)(menuitem);
    fn_row Fn_Row;
    menuitem (*Create4Menu)(fn_menuitem, obj Parent, window Window, udat Code,
			    byte Flags, ldat Len, CONST byte *Name);
    uldat (*Create4MenuCommon)(fn_menuitem, menu);
    /* for compatibility this must return a non-zero value. */
};


struct s_menu {
    uldat Id;
    fn_menu Fn;
    menu Prev, Next; /* in the same msgport */
    msgport MsgPort;
    /* menu */
    hwcol ColItem, ColSelect, ColDisabled, ColSelectDisabled, ColShtCut, ColSelShtCut;
    byte CommonItems;
    byte FlagDefColInfo;
    row Info;
    menuitem FirstI, LastI, SelectI;
};
struct s_fn_menu {
    uldat Magic, Size, Used;
    menu (*Create)(fn_menu, msgport MsgPort, hwcol ColItem, hwcol ColSelect, hwcol ColDisabled,
		   hwcol ColSelectDisabled, hwcol ColShtCut, hwcol ColSelShtCut, byte FlagDefColInfo);
    void (*Insert)(menu, msgport, menu Prev, menu Next);
    void (*Remove)(menu);
    void (*Delete)(menu);
    void (*ChangeField)(menu, udat field, uldat CLEARMask, uldat XORMask);
    /* menu */
    fn_obj Fn_Obj;
    row (*SetInfo)(menu, byte Flags, ldat Len, CONST byte *Text, CONST hwcol *ColText);
    menuitem (*FindItem)(menu, dat i);
    menuitem (*GetSelectedItem)(menu);
    menuitem (*RecursiveGetSelectedItem)(menu, dat *depth);
    void (*SetSelectedItem)(menu, menuitem);
};


#define MSG_KEY			((udat)0)
#define MSG_MOUSE		((udat)1)
#define MSG_MAP			((udat)2)
#define MSG_CONTROL		((udat)3)

#define MSG_DISPLAY		((udat)0x0FFF)

#define MSG_SYSTEM_FIRST	((udat)0x1000)
#define MSG_WIDGET_KEY		((udat)0x1000)
#define MSG_WIDGET_MOUSE	((udat)0x1001)
#define MSG_WIDGET_CHANGE	((udat)0x1002)
#define MSG_WIDGET_GADGET	((udat)0x1003)
#define MSG_MENU_ROW		((udat)0x1004)
#define MSG_SELECTION		((udat)0x1005)
#define MSG_SELECTIONNOTIFY	((udat)0x1006)
#define MSG_SELECTIONREQUEST	((udat)0x1007)
#define MSG_SELECTIONCLEAR	((udat)0x1008)


#define MSG_USER_FIRST		((udat)0x2000)
#define MSG_USER_CONTROL	((udat)0x2000)
#define MSG_USER_CONTROL_REPLY	((udat)0x2001)
#define MSG_USER_CLIENTMSG	((udat)0x2100)

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
 * MSG_WIDGET_KEY	use ...EventKeyboard
 * MSG_WIDGET_MOUSE	use ...EventMouse
 * MSG_WIDGET_CHANGE	use ...EventWindow
 * MSG_WIDGET_GADGET	use ...EventGadget
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


typedef struct s_event_common event_common;
struct s_event_common {
    widget W;
    udat Code, pad;
};

typedef struct s_event_map event_map;
struct s_event_map {
    widget W;
    udat Code, pad;	/* unused */
    screen Screen;
};

typedef struct s_event_keyboard event_keyboard;
struct s_event_keyboard {
    widget W;
    udat Code, ShiftFlags, SeqLen;
    byte pad, AsciiSeq[1];  /* AsciiSeq[SeqLen] == '\0' */
};

typedef struct s_event_mouse event_mouse;
struct s_event_mouse {
    widget W;
    udat Code, ShiftFlags;
    dat X, Y;
};

typedef struct s_event_control event_control;
struct s_event_control {
    widget W;
    udat Code, Len;
    dat X, Y;
    byte Data[sizeof(uldat)]; /* [Len] bytes actually */
};

/* some MSG_CONTROL codes */
#define MSG_CONTROL_QUIT	((udat)0)
#define MSG_CONTROL_RESTART	((udat)1)
#define MSG_CONTROL_OPEN	((udat)2)
#define MSG_CONTROL_DRAGNDROP	((udat)3)

/* use for free-format messages between clients */
typedef struct s_event_clientmsg event_clientmsg;
struct s_event_clientmsg {
    widget W;
    udat Code, Format;
    uldat Len;
    union {
	byte b[sizeof(uldat)];
	udat d[sizeof(uldat)/sizeof(udat)];
	uldat l[1];
    } Data; /* [Len] bytes actually */
};

typedef struct s_event_display event_display;
struct s_event_display {
    widget W; /* not used here */
    udat Code, Len;
    dat X, Y;
    byte *Data; /* [Len] bytes actually */
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

typedef struct s_event_widget event_widget;
struct s_event_widget {
    widget W;
    udat Code, Flags;
    dat XWidth, YWidth;
    dat X, Y;
};

/* some MSG_WIDGET_CHANGE codes */
#define MSG_WIDGET_RESIZE 0
#define MSG_WIDGET_EXPOSE 1

/* some MSG_WIDGET_CHANGE flags */
#define MSG_WIDGETFL_SHADED	1

typedef struct s_event_gadget event_gadget;
struct s_event_gadget {
    widget W;
    udat Code, Flags; /* the Flags of the gadget */
};

typedef struct s_event_menu event_menu;
struct s_event_menu {
    window W;
    udat Code, pad;
    menu Menu;
    row Row;
};

typedef struct s_event_selection event_selection;
struct s_event_selection {
    widget W;
    udat Code, pad; /* unused */
    dat X, Y;
};

#define MAX_MIMELEN 64

typedef struct s_event_selectionnotify event_selectionnotify;
struct s_event_selectionnotify {
    widget W;
    udat Code, pad; /* unused */
    uldat ReqPrivate;
    uldat Magic;
    byte MIME[MAX_MIMELEN];
    uldat Len;
    byte Data[sizeof(uldat)]; /* Data[] is Len bytes actually */
};
/*SelectionNotify Magic*/
#define SEL_APPEND	0x00000000
#define SEL_TEXTMAGIC	0x54657874
#define SEL_HWFONTMAGIC 0x4877666E /* it's unicode */
#define SEL_FILEMAGIC	0x46696c65
#define SEL_URLMAGIC	0xAB1691BA
#define SEL_DATAMAGIC	0xDA1AA1AD /* check MIME if you get this */
#define SEL_IDMAGIC	0x49644964

typedef struct s_event_selectionrequest event_selectionrequest;
struct s_event_selectionrequest {
    widget W;
    udat Code, pad; /* unused */
    obj Requestor;
    uldat ReqPrivate;
};

typedef union event_any event_any;
union event_any {
    event_common EventCommon;
    event_keyboard EventKeyboard;
    event_mouse EventMouse;
    event_control EventControl;
    event_clientmsg EventClientMsg;
    event_display EventDisplay;
    event_map EventMap;
    event_widget EventWidget;
    event_gadget EventGadget;
    event_menu EventMenu;
    event_selection EventSelection;
    event_selectionnotify EventSelectionNotify;
    event_selectionrequest EventSelectionRequest;
};

struct s_msg {
    uldat Id;
    fn_msg Fn;
    msg Prev, Next;
    msgport MsgPort;
    /* msg */
    udat Type;		/* See above notes */
    udat Len;		/* length of Event */
    event_any Event;
};
struct s_fn_msg {
    uldat Magic, Size, Used;
    msg (*Create)(fn_msg, udat Type, udat EventLen);
    void (*Insert)(msg, msgport, msg Prev, msg Next);
    void (*Remove)(msg);
    void (*Delete)(msg);
    void (*ChangeField)(msg, udat field, uldat CLEARMask, uldat XORMask);
    /* msg */
    fn_obj Fn_Obj;
};



struct s_msgport {
    uldat Id;
    fn_msgport Fn;
    msgport Prev, Next; /* list in the same All */
    all All;
    /* msgport */
    byte WakeUp, NameLen, *Name;
    /* Note : a MsgPort is always woken up if it has pending messages. */
    void (*Handler)(msgport);
    void (*ShutDownHook)(msgport);
    timevalue CallTime, PauseDuration;
    remotedata RemoteData;
    msg FirstMsg, LastMsg;
    menu FirstMenu, LastMenu;	/* menus created by this MsgPort */
    widget FirstW, LastW;	/* widgets owned by this MsgPort */
    group FirstGroup, LastGroup; /* groups done by this MsgPort */
    mutex FirstMutex, LastMutex;/* mutexes owned by this MsgPort */
    uldat CountE, SizeE;        /* number of extensions used by this MsgPort */
    extension *Es;              /* extensions used by this MsgPort */
    display_hw AttachHW;	/* that was attached as told by MsgPort */
};
struct s_fn_msgport {
    uldat Magic, Size, Used;
    msgport (*Create)(fn_msgport, byte NameLen, CONST byte *Name,
		      tany PauseSec, tany PauseFraction,
		      byte WakeUp, void (*Handler)(msgport));
    void (*Insert)(msgport, all, msgport Prev, msgport Next);
    void (*Remove)(msgport);
    void (*Delete)(msgport);
    void (*ChangeField)(msgport, udat field, uldat CLEARMask, uldat XORMask);
    /* msgport */
    fn_obj Fn_Obj;
    void (*UseExtension)(msgport, extension);
    void (*UnuseExtension)(msgport, extension);
};
/* MsgPort->WakeUp: */
#define TIMER_ALWAYS	((byte)1)
#define TIMER_ONCE	((byte)2)


struct s_mutex {
    uldat Id;
    fn_mutex Fn;
    mutex Prev, Next; /* in the same All */
    all All;
    /* mutex */    
    mutex O_Prev, O_Next; /* owned by the same MsgPort */
    msgport Owner;
    byte Perm, NameLen;
    byte *Name;
};
struct s_fn_mutex {
    uldat Magic, Size, Used;
    mutex (*Create)(fn_mutex, msgport Owner, byte NameLen, CONST byte *Name, byte Perm);
    void (*Insert)(mutex, all, mutex Prev, mutex Next);
    void (*Remove)(mutex);
    void (*Delete)(mutex);
    void (*ChangeField)(mutex, udat field, uldat CLEARMask, uldat XORMask);
    /* mutex */
    fn_obj Fn_Obj;
    void (*Own)(mutex, msgport);
    void (*DisOwn)(mutex);
};
#define PERM_NONE	((byte)0)
#define PERM_READ	((byte)1)
#define PERM_WRITE	((byte)2)


/* module */

struct s_module {
    uldat Id;
    fn_module Fn;
    module Prev, Next; /* in the same All */
    all All;
    /* module */
    uldat NameLen, Used;
    byte *Name;
    void *Handle, *Private;
};
struct s_fn_module {
    uldat Magic, Size, Used;
    module (*Create)(fn_module, uldat NameLen, CONST byte *Name);
    void (*Insert)(module, all, module Prev, module Next);
    void (*Remove)(module);
    void (*Delete)(module);
    void (*ChangeField)(module, udat field, uldat CLEARMask, uldat XORMask);
    /* module */    
    fn_obj Fn_Obj;
    byte (*DlOpen)(module);
    void (*DlClose)(module);
};



struct s_extension {
    uldat Id;
    fn_extension Fn;
    extension Prev, Next; /* in the same All */
    all All;
    /* module */
    uldat NameLen, Used;
    byte *Name;
    void *Handle, *Private;
    /* extension */
    tany (*CallB)(extension, topaque len, CONST byte *data, void *return_type); /* call extension-specific functions */
    void (*Quit)(extension); /* how to quit this extension if it is not dlopen()ed */
};
struct s_fn_extension {
    uldat Magic, Size, Used;
    extension (*Create)(fn_extension, uldat NameLen, CONST byte *Name);
    void (*Insert)(extension, all, extension Prev, extension Next);
    void (*Remove)(extension);
    void (*Delete)(extension);
    void (*ChangeField)(extension, udat field, uldat CLEARMask, uldat XORMask);
    /* module */    
    fn_obj Fn_Obj;
    byte (*DlOpen)(extension);
    void (*DlClose)(extension);
    /* extension */
    fn_module Fn_Module;
    extension (*Query)(byte namelen, CONST byte *name);
};



struct s_display_hw {
    uldat Id;
    fn_display_hw Fn;
    display_hw Prev, Next; /* in the same All */
    all All;
    
    /* display_hw */
    uldat NameLen;
    byte *Name;
    module Module;

    void *Private;	/* used to store HW-specific data */
    
    void (*FlushVideo)(void);
    void (*FlushHW)(void);

    void (*KeyboardEvent)(int fd, display_hw hw);
    void (*MouseEvent)(int fd, display_hw hw);
    
    void (*ShowMouse)(void);
    void (*HideMouse)(void);
    void (*UpdateMouseAndCursor)(void);

    /* just detect size */
    void (*DetectSize)(dat *x, dat *y);

    /* check if size (x,y) is possible. if not, decrease (x,y) to the nearest possible size */
    void (*CheckResize)(dat *x, dat *y);
    
    /* unconditionally resize to (x,y). it is guaranteed that CheckResize returned this (x,y) */
    void (*Resize)(dat x, dat y);
    
    byte (*HWSelectionImport)(void);
    void (*HWSelectionExport)(void);
    void (*HWSelectionRequest)(obj Requestor, uldat ReqPrivate);
    void (*HWSelectionNotify)(uldat ReqPrivate, uldat Magic,
			      CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data);
    tany HWSelectionPrivate;
	
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
    void (*ConfigureKeyboard)(udat resource, byte todefault, udat value);
    void (*ConfigureMouse)(udat resource, byte todefault, udat value);
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
     *				  a little before flushing, hoping to receive more data
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
    dat RedrawLeft, RedrawUp, RedrawRight, RedrawDown;
    /*
     * the corrupted area that needs to be redrawn.
     * 
     * the upper layer (i.e. hw.c) automagically updates
     * ChangedVideoFlag and ChangedVideo[] to include this area
     * for your display.
     */
    
    uldat keyboard_slot, mouse_slot;

    mouse_state MouseState;

    dat X, Y;
    /*
     * real display size, in character cells.
     */
    
    dat usedX, usedY;
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
    
    dat XY[2];  /* hw-dependent cursor position */
    uldat TT;   /* hw-dependent cursor type */
};

struct s_fn_display_hw {
    uldat Magic, Size, Used;
    display_hw (*Create)(fn_display_hw, uldat NameLen, CONST byte *Name);
    void (*Insert)(display_hw, all, display_hw Prev, display_hw Next);
    void (*Remove)(display_hw);
    void (*Delete)(display_hw);
    void (*ChangeField)(display_hw, udat field, uldat CLEARMask, uldat XORMask);
    /* display_hw */
    fn_obj Fn_Obj;
    byte (*Init)(display_hw);
    void (*Quit)(display_hw);
};

/* DisplayHW->FlagsHW */
#define FlHWSoftMouse		((byte)0x01)
#define FlHWChangedMouseFlag	((byte)0x02)
#define FlHWNeedOldVideo	((byte)0x04)
#define FlHWExpensiveFlushVideo	((byte)0x08)
#define FlHWNoInput		((byte)0x10)

/* DisplayHW->NeedHW */
#define NEEDFlushStdout		((byte)0x01)
#define NEEDFlushHW		((byte)0x02)
#define NEEDResizeDisplay	((byte)0x04)
#define NEEDSelectionExport	((byte)0x08)
#define NEEDPanicHW		((byte)0x10)
#define NEEDPersistentSlot	((byte)0x20)
#define NEEDFromPreviousFlushHW	((byte)0x40)
#define NEEDBeepHW		((byte)0x80)

    
/* errors */
#define NOMEMORY	((udat)1)
#define NOTABLES	((udat)2)
#define DLERROR		((udat)3)
#define SYSCALLERROR	((udat)4)
#define USERERROR	((udat)5)

/* IDs */
#define NOID		((uldat)0)
#define BADID		((uldat)-1)

#define MAXID		((uldat)0x0FFFFFFFul)
#define magic_mask	((uldat)0xF0000000ul)
#define magic_shift	28

#define obj_magic_id		0
#define widget_magic_id		1
#define gadget_magic_id		2
#define window_magic_id		3
#define screen_magic_id		4
#define group_magic_id		5
#define row_magic_id		6
#define menuitem_magic_id	7
#define menu_magic_id		8
#define msgport_magic_id	9
#define msg_magic_id		0xA
#define mutex_magic_id		0xB
#define module_magic_id		0xC
#define extension_magic_id	0xD
#define display_hw_magic_id	0xE
#define all_magic_id		0xF

/*
 * These must have consecutive values, but obj_magic_STR can be changed
 * as long as it has the same value as obj_magic_CHR.
 * To avoid troubles with strlen(), you should not use '\0' or "\0"
 * for any of the values below.
 */
#define base_magic_CHR		'\x30'
#define obj_magic_STR		"\x30"
#define widget_magic_STR	"\x31"
#define gadget_magic_STR	"\x32"
#define window_magic_STR	"\x33"
#define screen_magic_STR	"\x34"
#define group_magic_STR		"\x35"
#define row_magic_STR		"\x36"
#define menuitem_magic_STR	"\x37"
#define menu_magic_STR		"\x38"
#define msgport_magic_STR	"\x39"
#define msg_magic_STR		"\x3A"
#define mutex_magic_STR		"\x3B"
#define module_magic_STR	"\x3C"
#define extension_magic_STR	"\x3D"
#define display_hw_magic_STR	"\x3E"
#define all_magic_STR		"\x3F"

#define obj_magic	((uldat)0x0dead0b1ul)
#define widget_magic	((uldat)0x161d9743ul)
#define gadget_magic	((uldat)0x29867551ul)
#define window_magic	((uldat)0x31357531ul)
#define screen_magic	((uldat)0x42659871ul)
#define group_magic	((uldat)0x5741f326ul)
#define row_magic	((uldat)0x68074ffaul)
#define menuitem_magic	((uldat)0x7abc8fdeul)
#define menu_magic	((uldat)0x8bad0bedul)
#define msgport_magic	((uldat)0x90981437ul)
#define msg_magic	((uldat)0xA3a61ce4ul) /* this gets compiled in libTw ! */
#define mutex_magic	((uldat)0xB0faded0ul)
#define module_magic	((uldat)0xCb0f1278ul)
#define extension_magic ((uldat)0xDe81ec51ul)
#define display_hw_magic ((uldat)0xEdbcc609ul)
#define all_magic	((uldat)0xFa11Fa11ul)


#define magic_n		16 /* max top hex digit of the above ones + 1 */

/*
 *   B I G   F A T   WARNING:
 * 
 * msg_magic is the magic number for user-created (tmsg) structures,
 * while MSG_MAGIC (defined in sockproto.h) is the serial number reserved by
 * libTw to receive server messages (which are still (tmsg) structures).
 */


#define IS_OBJ(type,O)	(((O)->Id & magic_mask) == (type##_magic & magic_mask))
#define IS_WIDGET(O)	(IS_OBJ(widget,O) || IS_OBJ(gadget,O) || IS_OBJ(window,O) || IS_OBJ(screen,O))
#define IS_GADGET(O)	IS_OBJ(gadget,O)
#define IS_WINDOW(O)	IS_OBJ(window,O)
#define IS_SCREEN(O)	IS_OBJ(screen,O)
#define IS_ROW(O)	(IS_OBJ(row,O) || IS_OBJ(menuitem,O))
#define IS_MENUITEM(O)	IS_OBJ(menuitem,O)
#define IS_MENU(O)	IS_OBJ(menu,O)
#define IS_MSGPORT(O)	IS_OBJ(msgport,O)
#define IS_MUTEX(O)	IS_OBJ(mutex,O)
#define IS_MSG(O)	IS_OBJ(msg,O)
#define IS_MODULE(O)	(IS_OBJ(module,O) || IS_OBJ(extension,O))
#define IS_EXTENSION(O)	IS_OBJ(extension,O)
#define IS_DISPLAY_HW(O) IS_OBJ(display_hw,O)
#define IS_ALL(O)	IS_OBJ(all,O)

/* in the same order as the #defines above ! */
struct s_fn {
    fn_obj f_obj;
    fn_widget f_widget;
    fn_gadget f_gadget;
    fn_window f_window;
    fn_screen f_screen;
    fn_group f_group;
    fn_row f_row;
    fn_menuitem f_menuitem;
    fn_menu f_menu;
    fn_msgport f_msgport;
    fn_mutex f_mutex;
    fn_msg f_msg;
    fn_module f_module;
    fn_extension f_extension;
    fn_display_hw f_display_hw;
    fn_obj f_all;
};

struct s_setup {
    dat MaxMouseSnap;
    udat MinAllocSize;
    byte Flags;
    byte ButtonSelection, ButtonPaste;
    byte DeltaXShade, DeltaYShade;
};
/* All->Setup->Flags */
#define SETUP_SHADOWS		0x01
#define SETUP_BLINK		0x02
#define SETUP_CURSOR_ALWAYS	0x04
#define SETUP_MENU_HIDE		0x08
#define SETUP_MENU_INFO		0x10
#define SETUP_MENU_RELAX	0x20
#define SETUP_SCREEN_SCROLL	0x40


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
#define STATE_DRAG		((byte)10)
#define STATE_RESIZE		((byte)15)
#define STATE_SCROLL		((byte)16)
#define STATE_GADGET		((byte)26)
#define STATE_MENU		((byte)27)
#define STATE_SCREEN		((byte)28)
#define STATE_BUTTON_SCREEN	((byte)29)
#define STATE_ROOT		((byte)30)
#define STATE_DEFAULT		((byte)31)

/* mask for all the above */
#define STATE_ANY		((byte)0x1F)

/* furher All->State flags */
#define STATE_FL_BYMOUSE	((byte)0x40)


/* values returned by FnWindow->FindBorder (modeled after STATE_*) */
/*#define POS_BUTTON(n)		((byte)(n))*/
#define POS_TITLE		((byte)10)
#define POS_SIDE_LEFT		((byte)11)
#define POS_SIDE_UP		((byte)12)
#define POS_SIDE_RIGHT		((byte)13)
#define POS_SIDE_DOWN		((byte)14)
#define POS_BUTTON_RESIZE	((byte)15)

#define POS_X_BAR_BACK		((byte)16)
#define POS_X_BAR_FWD		((byte)17)
#define POS_X_TAB		((byte)18)
#define POS_X_ARROW_BACK	((byte)19)
#define POS_X_ARROW_FWD		((byte)20)

#define POS_Y_BAR_BACK		((byte)21)
#define POS_Y_BAR_FWD		((byte)22)
#define POS_Y_TAB		((byte)23)
#define POS_Y_ARROW_BACK	((byte)24)
#define POS_Y_ARROW_FWD		((byte)25)

#define POS_INSIDE		((byte)26)
#define POS_MENU		((byte)27)
#define POS_BUTTON_SCREEN	((byte)29)
#define POS_ROOT		((byte)30)

typedef struct s_selection {
    timevalue Time;
    msgport Owner;
    display_hw OwnerOnce;
    uldat Magic;
    byte MIME[MAX_MIMELEN];
    uldat Len, Max;
    byte *Data;
} selection;

typedef struct s_button_vec {
    hwfont shape[2];
    sbyte pos;
    byte exists;
    byte changed;
} button_vec;


struct s_all {
    /* for compatibility with s_obj */
    uldat Id;
    fn_obj Fn;
    obj Prev, Next;
    obj Parent;
    
    screen FirstScreen, LastScreen;
    msgport FirstMsgPort, LastMsgPort, RunMsgPort;
    mutex FirstMutex, LastMutex;
    
    module FirstModule, LastModule;
    fn_hook FnHookModule; widget HookModule;
    
    display_hw FirstDisplayHW, LastDisplayHW, MouseHW, ExclusiveHW;
    fn_hook FnHookDisplayHW; widget HookDisplayHW;
    
    dat DisplayWidth, DisplayHeight;
    byte State;
    timevalue Now;
    selection *Selection;
    setup *SetUp;
    void (*AtQuit)(void);

    menu BuiltinMenu, CommonMenu;
    row BuiltinRow;
    uldat MouseMotionN;/* number of mapped windows wanting mouse motion events */
    
    
    button_vec ButtonVec[BUTTON_MAX + 1]; /* +1 for window corner */
    
    hwfont *Gtranslations[USER_MAP+1];
};


/************** Keys **************/

#define KBD_SHIFT_FL	(byte)0x1
#define KBD_CTRL_FL	(byte)0x2
#define KBD_ALT_FL	(byte)0x4
#define KBD_CAPS_LOCK	(byte)0x8
#define KBD_NUM_LOCK	(byte)0x10


#define ENTER      	((udat)'\r')
#define ESCAPE     	((udat)'\033')




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
   * with the current MkDep, DEBUG_MALLOC gets defined only if doing
   * `make DEBUG_MALLOC=y ...' and the C file that includes twin.h actually
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
#else /* !CONF__ALLOC */

void *AllocMem(size_t Size);

# ifdef DEBUG_MALLOC
INLINE void FreeMem(void *Mem) {
    if (!FAIL(Mem))
	free(Mem);
}
# else /* !DEBUG_MALLOC */

#  define FreeMem(Mem)		free(Mem)

# endif /* DEBUG_MALLOC */

# ifdef USE_MY_REALLOC
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
# else /* !USE_MY_REALLOC */

#  define ReAllocMem(Mem, Size)	realloc(Mem, Size)

# endif /* USE_MY_REALLOC */

#endif /* CONF__ALLOC */

# define LenStr(S) strlen(S)
# define CmpStr(S1, S2) strcmp(S1, S2)
# define CopyStr(From,To) strcpy(To, From)

# define CopyMem(From, To, Size)	memcpy(To, From, Size)
# define MoveMem(From, To, Size)	memmove(To, From, Size)
# define WriteMem(Mem, Char, Size)	memset(Mem, Char, Size)
# define CmpMem(m1, m2, Size)		memcmp(m1, m2, Size)


typedef enum { none, sgidtty, suidroot } e_privilege;

# define DropPrivileges() (setegid(getgid()), seteuid(getuid()))
# define GainRootPrivileges() seteuid(0)
# define GainGroupPrivileges(g) setegid(g)

byte *CloneStr(CONST byte *s);

#endif /* _TWIN_H */

