/*
 *  Tw_defs.h  --  #defines for libtw macros
 *
 *  Copyright (C) 1999-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef TW_DEFS_H
#define TW_DEFS_H

enum tw_magic {
  /* "Twin" in native byte-order */
  TWIN_MAGIC = 0x6E697754ul,
  /* "Go!!" in native byte-order */
  TW_GO_MAGIC = 0x21216F47ul,
  /* "Wait" in native byte-order */
  TW_WAIT_MAGIC = 0x74696157ul,
  /* "Stop" in native byte-order */
  TW_STOP_MAGIC = 0x706F7453ul,
};

enum {
  TW_INET_PORT = 7754,
};

/* these must be #defines, they are stringified with # preprocessor macro */
#define TW_SMALLBUFF 256
#define TW_BIGBUFF 4096
#define TW_HUGEBUFF 16777216

/* return from signal macros */

#define TW_RETFROMSIGNAL(value) return TW_ARG_IFNOTVOID(value, void)
#define TW_ARG_IFNOTVOID(value, type) TW_CAT(TW_ARG_IFNOTVOID_, type)(value)
#define TW_ARG_IFNOTVOID_void(value)
#define TW_ARG_IFNOTVOID_int(value) (value)

/* maximum number of arguments of a libtw function */
#define TW_MAX_ARGS_N 20

/* enum for VGA (not ANSI!) colors */
enum tcolor_e {
  tblack = 0,
  tblue = 1,
  tgreen = 2,
  tcyan = tblue | tgreen,
  tred = 4,
  tmagenta = tblue | tred,
  tyellow = tgreen | tred,
  twhite = tblue | tgreen | tred,
  thigh = 8,
  tmaxcol = 0xF,
};

#define TANSI2VGA(col) (((col)&0x1 ? tred : 0) | ((col)&0x2 ? tgreen : 0) | ((col)&0x4 ? tblue : 0))
#define TVGA2ANSI(col) TANSI2VGA(col)

/* foreground / background colors handling */
/*
 * NOTE: draw.c:DoShadowColor() assumes that
 * TCOL(fg1, bg1) | TCOL(fg2, bg2) == TCOL(fg1|fg2, bg1|bg2)
 * and
 * TCOL(fg1, bg1) & TCOL(fg2, bg2) == TCOL(fg1&fg2, bg1&bg2)
 */
#define TFG(col) (col)
#define TBG(col) ((col) << 4)
#define TCOL(fg, bg) (TFG(fg) | TBG(bg))
#define TCOLBG(col) ((col) >> 4)
#define TCOLFG(col) ((col)&0x0F)

/**********************************/

enum {
  tmsg_magic = 0xA3A61CE4ul, /* this must match msg_magic in server/obj/magic.h */
};

/* keyboard events ShiftFlags */
enum tw_kbd_shift_flag {
  TW_KBD_SHIFT_FL = 0x1,
  TW_KBD_CTRL_FL = 0x2,
  TW_KBD_ALT_FL = 0x4,
  TW_KBD_CAPS_LOCK = 0x8,
  TW_KBD_NUM_LOCK = 0x10,
};

/* Widget->Attr */
/*
 * ask the server to send events even for mouse motion without any pressed button.
 * works only if WIDGET_WANT_MOUSE is set too.
 */
enum tw_widget_attrib {
  TW_WIDGET_WANT_MOUSE_MOTION = 0x0001,
  TW_WIDGET_WANT_KEYS = 0x0002,
  TW_WIDGET_WANT_MOUSE = 0x0004,
  TW_WIDGET_WANT_CHANGES = 0x0008,
  TW_WIDGET_AUTO_FOCUS = 0x0010,
};

/* Widget->Flags */
enum tw_widget_flag {
  TW_WIDGETFL_USEEXPOSE = 0x02,
  TW_WIDGETFL_USEFILL = 0x03,
  TW_WIDGETFL_USEANY = 0x07, /* mask of all above ones */

  TW_WIDGETFL_NOTVISIBLE = 0x8000,
};

/* Gadget->Attr */
enum tw_gadget_attrib {
  TW_GADGET_WANT_MOUSE_MOTION = TW_WIDGET_WANT_MOUSE_MOTION, /* 0x0001 */
  TW_GADGET_WANT_KEYS = TW_WIDGET_WANT_KEYS,                 /* 0x0002 */
  TW_GADGET_WANT_MOUSE = TW_WIDGET_WANT_MOUSE,               /* 0x0004 */
  TW_GADGET_WANT_CHANGES = TW_WIDGET_WANT_CHANGES,           /* 0x0008 */
  TW_GADGET_AUTO_FOCUS = TW_WIDGET_AUTO_FOCUS,               /* 0x0010 */
};

/* Gadget->Flags */
enum tw_gadget_flag {
  TW_GADGETFL_USETEXT = 0x00,                    /* it's the default */
  TW_GADGETFL_USEEXPOSE = TW_WIDGETFL_USEEXPOSE, /* 0x02 */
  TW_GADGETFL_USEFILL = TW_WIDGETFL_USEFILL,     /* 0x03 */
  TW_GADGETFL_USEANY = TW_WIDGETFL_USEANY,       /* 0x07 */

  TW_GADGETFL_DISABLED = 0x0020,
  TW_GADGETFL_TEXT_DEFCOL = 0x0040,
  /* this makes the gadget 'checkable' : can be in 'checked' or 'unchecked' state.
   * also necessary to put the gadget in a group */
  TW_GADGETFL_TOGGLE = 0x0080,
  TW_GADGETFL_PRESSED = 0x0100,

  TW_GADGETFL_NOTVISIBLE = TW_WIDGETFL_NOTVISIBLE,
};

/* Window->Attr */
enum tw_window_attrib {
  TW_WINDOW_WANT_MOUSE_MOTION = TW_WIDGET_WANT_MOUSE_MOTION, /* 0x0001 */
  TW_WINDOW_WANT_KEYS = TW_WIDGET_WANT_KEYS,                 /* 0x0002 */
  TW_WINDOW_WANT_MOUSE = TW_WIDGET_WANT_MOUSE,               /* 0x0004 */
  TW_WINDOW_WANT_CHANGES = TW_WIDGET_WANT_CHANGES,           /* 0x0008 */
  TW_WINDOW_AUTO_FOCUS = TW_WIDGET_AUTO_FOCUS,               /* 0x0010 */
  TW_WINDOW_DRAG = 0x0100,
  TW_WINDOW_RESIZE = 0x0200,
  TW_WINDOW_CLOSE = 0x0400,
  TW_WINDOW_ROLLED_UP = 0x0800,
  TW_WINDOW_X_BAR = 0x1000,
  TW_WINDOW_Y_BAR = 0x2000,
  TW_WINDOW_AUTO_KEYS = 0x4000,
};

/* Window->Flags */
enum tw_window_flag {
  TW_WINDOWFL_USEROWS = 0x00, /* it's the default */
  TW_WINDOWFL_USECONTENTS = 0x01,
  TW_WINDOWFL_USEEXPOSE = TW_WIDGETFL_USEEXPOSE, /* 0x02 */
  TW_WINDOWFL_USEFILL = TW_WIDGETFL_USEFILL,     /* 0x03 */
  TW_WINDOWFL_USEANY = TW_WIDGETFL_USEANY,       /* 0x07 */

  TW_WINDOWFL_CURSOR_ON = 0x10,
  TW_WINDOWFL_MENU = 0x20,
  TW_WINDOWFL_DISABLED = 0x40,
  TW_WINDOWFL_BORDERLESS = 0x80,
  TW_WINDOWFL_ROWS_INSERT = 0x0100,
  TW_WINDOWFL_ROWS_DEFCOL = 0x0200,
  TW_WINDOWFL_ROWS_SELCURRENT = 0x0400,

  TW_WINDOWFL_NOTVISIBLE = TW_WIDGETFL_NOTVISIBLE,
};

/* Screen->Flags */
enum tw_screen_flag {
  TW_SCREENFL_USEBG = 0x00,                      /* it's the default */
  TW_SCREENFL_USEEXPOSE = TW_WIDGETFL_USEEXPOSE, /* 0x02 */
  TW_SCREENFL_USEFILL = TW_WIDGETFL_USEFILL,     /* 0x03 */
  TW_SCREENFL_USEANY = TW_WIDGETFL_USEANY,       /* 0x07 */

  TW_SCREENFL_NOTVISIBLE = TW_WIDGETFL_NOTVISIBLE,
};

/* Window->CursorType : */
/* These come from linux/drivers/char/console.c */
enum tw_window_cursor {
  TW_DEFAULTCURSOR = 0,
  TW_NOCURSOR = 1,
  TW_LINECURSOR = 2,
  TW_SOLIDCURSOR = 8,
};

/* Row->Flags : */
enum tw_row_flag {
  TW_ROW_INACTIVE = 0x00,
  TW_ROW_ACTIVE = 0x01,
  TW_ROW_IGNORE = 0x02,
  TW_ROW_DEFCOL = 0x04,
};

#endif /* TW_DEFS_H */
