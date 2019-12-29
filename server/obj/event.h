/*
 *  menu.h  --  declare server class s_menu
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_EVENT_H
#define _TWIN_EVENT_H

#include "obj/fwd.h"

#define MSG_KEY ((udat)0)
#define MSG_MOUSE ((udat)1)
#define MSG_MAP ((udat)2)
#define MSG_CONTROL ((udat)3)

#define MSG_DISPLAY ((udat)0x0FFF)

#define MSG_SYSTEM_FIRST ((udat)0x1000)
#define MSG_WIDGET_KEY ((udat)0x1000)
#define MSG_WIDGET_MOUSE ((udat)0x1001)
#define MSG_WIDGET_CHANGE ((udat)0x1002)
#define MSG_WIDGET_GADGET ((udat)0x1003)
#define MSG_MENU_ROW ((udat)0x1004)
#define MSG_SELECTION ((udat)0x1005)
#define MSG_SELECTIONNOTIFY ((udat)0x1006)
#define MSG_SELECTIONREQUEST ((udat)0x1007)
#define MSG_SELECTIONCLEAR ((udat)0x1008)

#define MSG_USER_FIRST ((udat)0x2000)
#define MSG_USER_CONTROL ((udat)0x2000)
#define MSG_USER_CONTROL_REPLY ((udat)0x2001)
#define MSG_USER_CLIENTMSG ((udat)0x2100)

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
  udat Code, pad; /* unused */
  screen Screen;
};

typedef struct s_event_keyboard event_keyboard;
struct s_event_keyboard {
  widget W;
  udat Code, ShiftFlags, SeqLen;
  byte pad;
  char AsciiSeq[1]; /* AsciiSeq[SeqLen] == '\0' */
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
  char Data[sizeof(uldat)]; /* [Len] bytes actually */
};

/* some MSG_CONTROL codes */
#define MSG_CONTROL_QUIT ((udat)0)
#define MSG_CONTROL_RESTART ((udat)1)
#define MSG_CONTROL_OPEN ((udat)2)
#define MSG_CONTROL_DRAGNDROP ((udat)3)

/* use for free-format messages between clients */
typedef struct s_event_clientmsg event_clientmsg;
struct s_event_clientmsg {
  widget W;
  udat Code, Format;
  uldat Len;
  union {
    byte b[sizeof(uldat)];
    udat d[sizeof(uldat) / sizeof(udat)];
    uldat l[1];
  } Data; /* [Len] bytes actually */
};

typedef struct s_event_display event_display;
struct s_event_display {
  widget W; /* not used here */
  udat Code, Len;
  dat X, Y;
  void *Data; /* [Len] bytes actually */
};

#define DPY_DrawHWAttr ((udat)0)
#define DPY_FlushHW ((udat)1)
#define DPY_KeyboardEvent ((udat)2)
#define DPY_MouseEvent ((udat)3)
#define DPY_SetCursorType ((udat)4)
#define DPY_MoveToXY ((udat)5)
#define DPY_Resize ((udat)6)

#define DPY_SelectionExport ((udat)8)
#define DPY_DragArea ((udat)9)
#define DPY_Beep ((udat)10)
#define DPY_Configure ((udat)11)
#define DPY_SetPalette ((udat)12)
#define DPY_ResetPalette ((udat)13)
#define DPY_Helper ((udat)14)
#define DPY_RedrawVideo ((udat)15)
#define DPY_Quit ((udat)16)

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
#define MSG_WIDGETFL_SHADED 1

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
  char MIME[MAX_MIMELEN];
  uldat Len;
  char Data[sizeof(uldat)]; /* Data[] is Len bytes actually */
};
/*SelectionNotify Magic*/
#define SEL_APPEND 0x00000000
#define SEL_TEXTMAGIC 0x54657874
#define SEL_HWFONTMAGIC 0x4877666E /* it's unicode */
#define SEL_FILEMAGIC 0x46696c65
#define SEL_URLMAGIC 0xAB1691BA
#define SEL_DATAMAGIC 0xDA1AA1AD /* check MIME if you get this */
#define SEL_IDMAGIC 0x49644964

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

#endif /* _TWIN_EVENT_H */
