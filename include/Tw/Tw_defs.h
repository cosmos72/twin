/*
 *  Tw_defs.h  --  #defines for libTw macros
 *
 *  Copyright (C) 1999-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TW_DEFS_H
#define _TW_DEFS_H

/* "Twin" in native byte-order */
#define TWIN_MAGIC ((uldat)0x6E697754ul)
/* "Go!!" in native byte-order */
#define TW_GO_MAGIC   ((uldat)0x21216F47ul)
/* "Wait" in native byte-order */
#define TW_WAIT_MAGIC ((uldat)0x74696157ul)

#define TW_INET_PORT	7754

#define TW_SMALLBUFF	256
#define TW_BIGBUFF	4096


#ifndef _TWIN_H


/* return from signal macros */

#define TW_RETFROMSIGNAL(value)		return TW_ARG_IFNOTVOID(value, TW_RETSIGTYPE)
#define TW_ARG_IFNOTVOID(value, type)	TW_CAT(TW_ARG_IFNOTVOID_,type)(value)
#define TW_ARG_IFNOTVOID_void(value)
#define TW_ARG_IFNOTVOID_int(value)	(value)



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
#define HWATTR16(col,ascii) (((byte16)(byte)(col) << 8) | (byte16)(byte)(ascii))
#define HWATTR_COLMASK16(attr) ((attr) & 0xFF00)
#define HWATTR_FONTMASK16(attr) ((attr) & 0xFF)
#define HWCOL16(attr) ((hwcol)((attr) >> 8))
#define HWFONT16(attr) ((byte)(attr))


/* if sizeof(hwattr) == 4, bytes are { 'ascii_low', 'col', 'ascii_high', 'unused' } */

/* hwattr <-> hwcol+hwfont conversion */
#define HWATTR32(col,ascii) (((byte32)(byte)(col) << 8) | (((byte32)(ascii) & 0xFF00) << 8) | (byte32)(byte)(ascii))
#define HWATTR_COLMASK32(attr) ((attr) & 0xFF00)
#define HWATTR_FONTMASK32(attr) ((attr) & 0xFF00FF)
#define HWCOL32(attr) ((hwcol)((attr) >> 8))
#define HWFONT32(attr) ((byte16)(((attr) & 0xFF) | (((attr) >> 8) & 0xFF00)))







/*
 * Notes about the timevalue struct:
 * 
 * it is used to represent both time intervals and absolute times;
 * the ->Seconds is a ttany numeric field.
 * DON'T assume ttany is 32 bit (or any other arbitrary size)
 * since in 19 Jan 2038 at 04:14:08 any signed, 32 bit ttany will overflow.
 * So use sizeof(ttany) if you really need.
 * 
 * the ->Fraction is a ttany numeric field.
 * As above, DON'T assume ttany is 32 bit (or any other arbitrary size)
 * since in the future we may want a finer granularity than the nanosecond one
 * possible with a 32 bit ttany.
 * So :
 * 1) use sizeof(ttany) if you really need
 * 2) don't assume (ttany)1 is a nanosecond (or any other arbitrary time),
 *    but always use the form '1 NanoSECs', '250 MilliSECs + 7 MicroSECs', etc.
 * 3) if you _absolutely_ need to know to what time (ttany)1 corresponds,
 *    use this: '1 FullSECs' is the number of (ttany)1 intervals in a second.
 * 4) for the moment, the only defined fractions of a second are:
 *    FullSECs, MilliSECs, MicroSECs, NanoSECs.
 *    Others may be added in the future (PicoSECs, FemtoSECs, AttoSECs, ...)
 */
#define THOUSAND	((ttany)1000)

#define NanoSEC		1 /* i.e. (ttany)1 is a nanosecond */
#define MicroSEC	(THOUSAND * NanoSEC)
#define MilliSEC	(THOUSAND * MicroSEC)
#define FullSEC		(THOUSAND * MilliSEC)


#define NanoSECs	* NanoSEC
#define MicroSECs	* MicroSEC
#define MilliSECs	* MilliSEC
#define FullSECs	* FullSEC


/**********************************/



#define msg_magic	((uldat)0xA3a61ce4ul)


#endif /* _TWIN_H */



/* keyboard events ShiftFlags */
#define TW_KBD_SHIFT_FL		0x1
#define TW_KBD_CTRL_FL		0x2
#define TW_KBD_ALT_FL		0x4
#define TW_KBD_CAPS_LOCK	0x8
#define TW_KBD_NUM_LOCK		0x10


/* Widget->Attrib */
/*
 * ask the server to send events even for mouse motion without any pressed button.
 * works only if WIDGET_WANT_MOUSE is set too.
 */
#define TW_WIDGET_WANT_MOUSE_MOTION	0x0001
#define TW_WIDGET_WANT_KEYS	0x0002
#define TW_WIDGET_WANT_MOUSE	0x0004
#define TW_WIDGET_WANT_CHANGES	0x0008
#define TW_WIDGET_AUTO_FOCUS	0x0010

/* Widget->Flags */
#define TW_WIDGETFL_USEEXPOSE	0x02
#define TW_WIDGETFL_USEFILL	0x03
#define TW_WIDGETFL_USEANY	0x07 /* mask of all above ones */

#define TW_WIDGETFL_NOTVISIBLE	0x8000





/* Gadget->Attrib */
#define TW_GADGET_WANT_MOUSE_MOTION	TW_WIDGET_WANT_MOUSE_MOTION /* 0x0001 */
#define TW_GADGET_WANT_KEYS	TW_WIDGET_WANT_KEYS	/* 0x0002 */
#define TW_GADGET_WANT_MOUSE	TW_WIDGET_WANT_MOUSE	/* 0x0004 */
#define TW_GADGET_WANT_CHANGES	TW_WIDGET_WANT_CHANGES	/* 0x0008 */
#define TW_GADGET_AUTO_FOCUS	TW_WIDGET_AUTO_FOCUS	/* 0x0010 */


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

#define TW_GADGETFL_NOTVISIBLE	TW_WIDGETFL_NOTVISIBLE




/* Window->Attrib */
#define TW_WINDOW_WANT_MOUSE_MOTION	TW_WIDGET_WANT_MOUSE_MOTION /* 0x0001 */
#define TW_WINDOW_WANT_KEYS	TW_WIDGET_WANT_KEYS	/* 0x0002 */
#define TW_WINDOW_WANT_MOUSE	TW_WIDGET_WANT_MOUSE	/* 0x0004 */
#define TW_WINDOW_WANT_CHANGES	TW_WIDGET_WANT_CHANGES	/* 0x0008 */
#define TW_WINDOW_AUTO_FOCUS	TW_WIDGET_AUTO_FOCUS	/* 0x0010 */
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

#define TW_WINDOWFL_NOTVISIBLE	TW_WIDGETFL_NOTVISIBLE





/* Screen->Flags */
#define TW_SCREENFL_USEBG	0x00 /* it's the default */
#define TW_SCREENFL_USEEXPOSE	TW_WIDGETFL_USEEXPOSE	/* 0x02 */
#define TW_SCREENFL_USEFILL	TW_WIDGETFL_USEFILL	/* 0x03 */
#define TW_SCREENFL_USEANY	TW_WIDGETFL_USEANY	/* 0x07 */

#define TW_SCREENFL_NOTVISIBLE	TW_WIDGETFL_NOTVISIBLE






/* Window->CursorType : */
/* These come from linux/drivers/char/console.c */
#define TW_DEFAULTCURSOR 0
#define TW_NOCURSOR	 1
#define TW_LINECURSOR	 2
#define TW_SOLIDCURSOR	 8




/* Row->Flags : */
#define TW_ROW_INACTIVE	((byte)0x00)
#define TW_ROW_ACTIVE	((byte)0x01)
#define TW_ROW_IGNORE	((byte)0x02)
#define TW_ROW_DEFCOL	((byte)0x04)






#endif /* _TW_DEFS_H */

