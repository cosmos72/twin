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





/*
 *   <*>  B I G    F A T   W A R N I N G  <*>
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
 * Tw_Create4Menu2Row();
 * Tw_Create4MenuWindow();
 * Tw_Create4MenuMenuItem();
 * Tw_CreateMsgPort();
 * Tw_CreateMenu();
 * Tw_FirstScreen();
 * Tw_FirstMsgPort();
 * [...]
 * 
 * and in general after a call to any Tw_*() function of Tw.h
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











#ifndef _TW_H
#define _TW_H

#ifdef __cplusplus
 extern "C" {
#endif
#if 0
 }
#endif

#include <Tw/Tw_defs.h>
#include <Tw/Twtypes.h>
#include <Tw/missing.h>
#include <Tw/mouse.h>

#ifndef _TWIN_H
#define _TWIN_H

/* a cutdown version of twin.h */


/** a structure containing high resolution time */
typedef struct s_timevalue  {
    tany Seconds;
    tany Fraction;
} timevalue;

#endif /* _TWIN_H */

/** type for groups of radio buttons */
typedef tobj tgroup;     
/** type for generic widgets */
typedef tobj twidget;
/** type for buttons */
typedef tobj tgadget;
/** type for windows */
typedef tobj twindow;
/** type for screens */
typedef tobj tscreen;
/** type for rows */
typedef tobj trow;
/** type for menu items */
typedef tobj tmenuitem;
/** type for menu bars */
typedef tobj tmenu;
/** type for client message ports */
typedef tobj tmsgport;
/** type for server-side mutexes (used for inter-client synchronization) */
typedef tobj tmutex;
/** type for server extensions */
typedef tobj textension;
/** type for server topmost object */
typedef tobj tall;




/* types for messages from server */
typedef struct s_tevent_common *tevent_common;
/** type for common part of the various events a client can receive inside a tmsg */
struct s_tevent_common {
    twidget W;
    udat Code, pad;
} TW_TYPE_ATTR_PACKED;

typedef struct s_tevent_display *tevent_display;
/** type for display-driver related events (not used by normal clients) */
struct s_tevent_display {
    twidget W; /* not used here */
    udat Code, Len;
    dat X, Y;
    byte Data[sizeof(uldat)]; /* [len] bytes actually */
} TW_TYPE_ATTR_PACKED;
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
/** type for keypress events */
struct s_tevent_keyboard {
    twidget W;
    udat Code, ShiftFlags, SeqLen;
    byte pad, AsciiSeq[1];  /* actually (SeqLen+1) bytes, including final \0 */
} TW_TYPE_ATTR_PACKED;

typedef struct s_tevent_mouse *tevent_mouse;
/** type for mouse events */
struct s_tevent_mouse {
    twidget W;
    udat Code, ShiftFlags;
    dat X, Y; /* these coords are relative to the widget top-left corner */
} TW_TYPE_ATTR_PACKED;

typedef struct s_tevent_widget *tevent_widget;
/** type for widget-related events (resize,expose,...) */
struct s_tevent_widget {
    twidget W;
    udat Code, Flags;
    dat XWidth, YWidth;
    dat X, Y;
} TW_TYPE_ATTR_PACKED;

/* some TW_MSG_WIDGET_CHANGE codes */
#define TW_MSG_WIDGET_RESIZE	0
#define TW_MSG_WIDGET_EXPOSE	1

/* some TW_MSG_WIDGET_CHANGE flags */
#define TW_MSG_WIDGETFL_SHADED	1

typedef struct s_tevent_gadget *tevent_gadget;
/** type for gadget-related events (user click on buttons) */
struct s_tevent_gadget {
    twidget W;
    udat Code, Flags; /* the Flags of the gadget that was (de)activated */
} TW_TYPE_ATTR_PACKED;

typedef struct s_tevent_menu *tevent_menu;
/** type for menu-related events (user activation of a menu row) */
struct s_tevent_menu {
    twindow W;
    udat Code, pad;
    tmenu Menu;
    trow Row;
} TW_TYPE_ATTR_PACKED;

typedef struct s_tevent_selection *tevent_selection;
/** type for 'user wants to paste selection' event */
struct s_tevent_selection {
    twidget W;
    udat Code, pad; /* unused */
    dat X, Y; /* these coords are absolute, to allow cross-window cut-n-paste */
} TW_TYPE_ATTR_PACKED;

#define TW_MAX_MIMELEN 64

typedef struct s_tevent_selectionnotify *tevent_selectionnotify;
/** type for selection notification event (selection owner sends clipboard to who asks for it) */
struct s_tevent_selectionnotify {
    twidget W;
    udat Code, pad; /* unused */
    uldat ReqPrivate;
    uldat Magic;
    byte MIME[TW_MAX_MIMELEN];
    uldat Len;
    byte Data[sizeof(uldat)]; /* Data[] is Len bytes actually */
} TW_TYPE_ATTR_PACKED;

/*SelectionNotify Magic*/
#define TW_SEL_TEXTMAGIC	((uldat)0x54657874)
#define TW_SEL_HWFONTMAGIC	((uldat)0x4877666E) /* it's unicode */
#define TW_SEL_FILEMAGIC	((uldat)0x46696c65)
#define TW_SEL_URLMAGIC		((uldat)0xAB1691BA)
#define TW_SEL_DATAMAGIC	((uldat)0xDA1AA1AD) /* check MIME if you get this */
#define TW_SEL_IDMAGIC		((uldat)0x49644964)

typedef struct s_tevent_selectionrequest *tevent_selectionrequest;
/** type to request selection to owner */
struct s_tevent_selectionrequest {
    twidget W;
    udat Code, pad; /* unused */
    tmsgport Requestor;
    uldat ReqPrivate;
} TW_TYPE_ATTR_PACKED;

typedef struct s_tevent_control *tevent_control;
/** type for inter-client control messages (quit,restart,open,dragndrop,...) */
struct s_tevent_control {
    twidget W;
    udat Code, Len;
    dat X, Y;
    byte Data[sizeof(uldat)]; /* [Len+1] bytes actually, Data[Len] == '\0' */
} TW_TYPE_ATTR_PACKED;

#define TW_SIZEOF_TEVENT_CONTROL (sizeof(struct s_tevent_control) - sizeof(uldat))

/* some TW_MSG_CONTROL codes */
#define TW_MSG_CONTROL_QUIT		((udat)0)
#define TW_MSG_CONTROL_RESTART		((udat)1)
#define TW_MSG_CONTROL_OPEN		((udat)2)
#define TW_MSG_CONTROL_DRAGNDROP	((udat)3)

typedef struct s_tevent_clientmsg *tevent_clientmsg;
/** type for inter-client arbitrary messages */
struct s_tevent_clientmsg {
    twidget W;
    udat Code, Format;
    uldat Len;
    union {
	byte b[sizeof(uldat)];
	udat d[sizeof(uldat)/sizeof(udat)];
	uldat l[1];
    } Data; /* [Len] bytes actually */
} TW_TYPE_ATTR_PACKED;

#define TW_SIZEOF_TEVENT_CLIENTMSG (sizeof(struct s_tevent_clientmsg) - sizeof(uldat))

typedef union s_tevent_any *tevent_any;
/** union of all event types */
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
} TW_TYPE_ATTR_PACKED;

typedef struct s_tmsg *tmsg;
/** type to send/receive events; contains union s_tevent_any */
struct s_tmsg {
    uldat Len;		/* length of this struct, including `Len' field */
    uldat Magic;	/* == msg_magic (if user-created) or == MSG_MAGIC (if received) */
    uldat Type;		/* only (udat) part is significative */
    union s_tevent_any Event;
} TW_TYPE_ATTR_PACKED;

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
#define TW_MSG_USER_CONTROL_REPLY ((udat)0x2101)
#define TW_MSG_USER_CLIENTMSG	((udat)0x2100)


/** type containing information about server connection (it's an opaque pointer) */
typedef struct s_tw_d *tdisplay;



/** type of function pointers passed to libTw as listeners (i.e. tmsg handlers) */
typedef void (*tfn_listener)(tevent_any, void *);
/** type of function pointer passed to libTw as default listener (i.e. tmsg handler) */
typedef void (*tfn_default_listener)(tmsg, void *);

/** type containing information about a listener (it's an opaque pointer) */
typedef struct s_tlistener *tlistener;






tdisplay Tw_Open(TW_CONST byte *Tw_Display);
void Tw_Close(tdisplay TwD);


/** Tw_AttachHW() flags: */
#define TW_ATTACH_HW_REDIRECT 1
#define TW_ATTACH_HW_EXCLUSIVE 2



/** Tw_CirculateWidgets() and Tw_CirculateRows() flags: */
#define TW_CIRCULATE_RAISE_LAST 0
#define TW_CIRCULATE_LOWER_FIRST 1


/*
 * maximum allowed Code for Gadgets, Rows and MenuRows is (TW_COD_RESERVED - 1) i.e. 0xF7FF
 */
#define TW_COD_RESERVED 0xF800


/* Tw_CreateMsgPort() flags */
#define TW_TIMER_ALWAYS	((byte)1)
#define TW_TIMER_ONCE	((byte)2)

     
/* Tw_SetOwnerSelection() Time useful value: */
#define TW_SEL_CURRENTTIME ((time_t)0)


/** Tw_SetServerUid() privileges: */
#define TW_PRIV_NONE 0
#define TW_PRIV_SGIDTTY 1
#define TW_PRIV_SUIDROOT 2



/** return 1 if server supports all given libTw functions */
byte  Tw_FindLFunction(tdisplay TwD, ...);
/**
 * call given server extension (opened with Tw_OpenExtension()).
 * Caller must cast all optional args to (ttany),
 * including strings and arrays.
 * 
 * The same cast rules apply to arguments implicitly passed to
 * Tw_CallVExtension().
 */
tany  Tw_CallLExtension(tdisplay TwD, textension id, TW_CONST byte *proto, topaque args_n, ...);




#include <Tw/common_m4.h>
#include <Tw/proto_m4.h>
#include <Tw/alias_m4.h>


#include <Tw/Tw1.h>

#ifdef __cplusplus
}
#endif

#endif /* _TW_H */

