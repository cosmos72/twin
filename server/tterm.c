/*
 *  tterm.c  --  create and manage multiple terminal emulator windows on twin
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "data.h"
#include "methods.h"
#include "extreg.h"

#include "remote.h"
#include "pty.h"
#include "util.h"
#include "common.h"

#define COD_QUIT      (udat)1
#define COD_SPAWN     (udat)3

static menu Term_Menu;

static byte *default_args[3];
static byte *default_title = "Twin Term";

static msgport Term_MsgPort;


static void TwinTermH(msgport MsgPort);
static void TwinTermIO(int Fd, window Window);

static void termShutDown(widget W) {
    window Window;
    if (IS_WINDOW(W)) {
	Window = (window)W;
	if (Window->RemoteData.Fd != NOFD)
	    close(Window->RemoteData.Fd);
	UnRegisterWindowFdIO(Window);
    }
}

static window newTermWindow(byte *title) {
    window Window;

    Window = Do(Create,Window)
	(FnWindow, Term_MsgPort, LenStr(title), title, NULL,
	 Term_Menu, COL(WHITE,BLACK), LINECURSOR,
	 WINDOW_WANT_KEYS|WINDOW_DRAG|WINDOW_RESIZE|WINDOW_Y_BAR|WINDOW_CLOSE,
	 WINDOWFL_CURSOR_ON|WINDOWFL_USECONTENTS,
	 80, 25, 200);
    
    if (Window) {
	Act(SetColors,Window)
	    (Window, 0x1FF, COL(HIGH|YELLOW,CYAN), COL(HIGH|GREEN,HIGH|BLUE),
	     COL(WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE),
	     COL(WHITE,BLACK), COL(HIGH|BLACK,HIGH|WHITE), COL(HIGH|BLACK,BLACK), COL(BLACK,HIGH|BLACK));
    
	Act(Configure,Window)(Window, (1<<2)|(1<<3), 0, 0, 7, 3, 0, 0);
    }
    return Window;
}

static window OpenTerm(CONST byte *arg0, byte * CONST *argv) {
    window Window;
    byte *title;
    
    /* if {arg0, argv} is {NULL, ...} or {"", ... } then start user's shell */
    if (arg0 && *arg0 && argv && argv[0]) {
	if ((title = strrchr(argv[0], '/')))
	    title++;
	else
	    title = argv[0];
    } else {
	arg0 = default_args[0];
	argv = default_args+1;
	
	title = default_title;
    }
    
    if ((Window = newTermWindow(title))) {
        if (SpawnInWindow(Window, arg0, argv)) {
	    if (RegisterWindowFdIO(Window, TwinTermIO)) {
		Window->ShutDownHook = termShutDown;
		Act(Map,Window)(Window, (widget)All->FirstScreen);
		return Window;
	    }
	    close(Window->RemoteData.Fd);
	}
	Delete(Window);
    }
    return NULL;
}

#ifdef CONF__UNICODE
static void TermWriteHWFontWindow(window W, uldat len, CONST hwfont *hwData) {
    hwfont (*inv_charset)(hwfont) = W->USE.C.TtyData->InvCharset;
    byte *Data, *sData;
    uldat n;
    
    if ((Data = sData = AllocMem(n = len))) {
	while (n--)
	    *Data++ = inv_charset(*hwData++);
	
	RemoteWindowWriteQueue(W, len, sData);
	FreeMem(sData);
    }
}
#endif

static void TwinTermH(msgport MsgPort) {
    msg Msg;
    event_any *Event;
    udat Code/*, Repeat*/;
    window Win;
    
    while ((Msg=Term_MsgPort->FirstMsg)) {
	Remove(Msg);
	
	Event=&Msg->Event;
	Win = (window)Event->EventSelection.W;
	if (Win && !IS_WINDOW(Win))
	    Win = NULL;
	
	if (Msg->Type==MSG_WIDGET_KEY) {
	    Code=Event->EventKeyboard.Code;
	    /* send keypresses */
	    if (Win)
		(void)RemoteWindowWriteQueue(Win, Event->EventKeyboard.SeqLen,
					     Event->EventKeyboard.AsciiSeq);
	} else if (Msg->Type==MSG_SELECTION) {
	    
		TwinSelectionRequest((obj)Term_MsgPort, Win->Id, TwinSelectionGetOwner());

	} else if (Msg->Type==MSG_SELECTIONNOTIFY) {
	    
	    if ((Win = (window)Id2Obj(window_magic_id,
				      Event->EventSelectionNotify.ReqPrivate))) {
#ifdef CONF__UNICODE
		if (Event->EventSelectionNotify.Magic == SEL_HWFONTMAGIC)
		    TermWriteHWFontWindow(Win, Event->EventSelectionNotify.Len / sizeof(hwfont),
					    (hwfont *)Event->EventSelectionNotify.Data);
		else
#endif
		    (void)RemoteWindowWriteQueue(Win, Event->EventSelectionNotify.Len,
						 Event->EventSelectionNotify.Data);
	    }
	} else if (Msg->Type==MSG_WIDGET_MOUSE) {
	    if (Win) {
		byte buf[10];
		byte len = CreateXTermMouseEvent(&Event->EventMouse, 10, buf);
    
		/* send mouse movements using xterm mouse protocol */
		if (len)
		    (void)RemoteWindowWriteQueue(Win, len, buf);
	    }
	} else if (Msg->Type==MSG_WIDGET_GADGET) {
	    if (Win && Event->EventGadget.Code == 0 /* Close Code */ )
		Delete(Win);
	} else if (Msg->Type==MSG_MENU_ROW) {
	    if (Event->EventMenu.Menu==Term_Menu) {
		Code=Event->EventMenu.Code;
		switch (Code) {
		  case COD_SPAWN:
		    OpenTerm(NULL, NULL);
		    break;
		  default:
		    break;
		}
	    }
	} else if (Msg->Type==MSG_USER_CONTROL) {
	    /* this duplicates the same functionality of builtin.c */
	    if (Event->EventControl.Code == MSG_CONTROL_OPEN) {
		byte **cmd = TokenizeStringVec(Event->EventControl.Len, Event->EventControl.Data);
		if (cmd) {
		    OpenTerm(cmd[0], cmd);
		    FreeStringVec(cmd);
		} else
		    OpenTerm(NULL, NULL);
	    }
	}
	Delete(Msg);
    }
}

static void TwinTermIO(int Fd, window Window) {
    static byte buf[BIGBUFF];
    uldat got = 0, chunk = 0;
    
    do {
	/*
	 * BIGBUFF - 1 to avoid silly windows...
	 * linux ttys buffer up to 4095 bytes.
	 */
	chunk = read(Fd, buf + got, BIGBUFF - 1 - got);
    } while (chunk && chunk != (uldat)-1 && (got += chunk) < BIGBUFF - 1);
    
    if (got)
	Act(TtyWriteAscii,Window)(Window, got, buf);
    else if (chunk == (uldat)-1 && errno != EINTR && errno != EWOULDBLOCK)
	/* something bad happened to our child :( */
	Delete(Window);
}

#ifdef THIS_MODULE

#include "tty.h"

static void OverrideMethods(byte enter) {
    if (enter) {
	OverrideMethod(Widget,KbdFocus,      FakeKbdFocus,    TtyKbdFocus);
	OverrideMethod(Gadget,KbdFocus,      FakeKbdFocus,    TtyKbdFocus);
	OverrideMethod(Window,KbdFocus,      FakeKbdFocus,    TtyKbdFocus);
	OverrideMethod(Window,TtyWriteAscii, FakeWriteAscii,  TtyWriteAscii);
	OverrideMethod(Window,TtyWriteString,FakeWriteString, TtyWriteString);
	OverrideMethod(Window,TtyWriteHWFont,FakeWriteHWFont, TtyWriteHWFont);
	OverrideMethod(Window,TtyWriteHWAttr,FakeWriteHWAttr, TtyWriteHWAttr);
	ForceKbdFocus();
    } else {
	OverrideMethod(Window,TtyWriteHWAttr,TtyWriteHWAttr,  FakeWriteHWAttr);
	OverrideMethod(Window,TtyWriteHWFont,TtyWriteHWFont,  FakeWriteHWFont);
	OverrideMethod(Window,TtyWriteString,TtyWriteString,  FakeWriteString);
	OverrideMethod(Window,TtyWriteAscii, TtyWriteAscii,   FakeWriteAscii);
	OverrideMethod(Window,KbdFocus,      TtyKbdFocus,     FakeKbdFocus);
	OverrideMethod(Gadget,KbdFocus,      TtyKbdFocus,     FakeKbdFocus);
	OverrideMethod(Widget,KbdFocus,      TtyKbdFocus,     FakeKbdFocus);
    }
}


byte InitModule(module Module)
#else
byte InitTerm(void)
#endif
{
    window Window;
    byte *shellpath, *shell;
    
    if (((shellpath = getenv("SHELL")) || (shellpath = "/bin/sh")) && 
	(default_args[0] = CloneStr(shellpath)) &&
	(default_args[1] = (shell = strrchr(shellpath, '/'))
	 ? CloneStr(shell) : CloneStr(shellpath)) &&
    
	(Term_MsgPort=Do(Create,MsgPort)
	 (FnMsgPort, 14, "builtin twterm", (uldat)0, (udat)0, (byte)0, TwinTermH)) &&
	(Term_Menu=Do(Create,Menu)
	 (FnMenu, Term_MsgPort,
	  COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0)) &&
	Info4Menu(Term_Menu, ROW_ACTIVE, (uldat)19, " Builtin Twin Term ", "ptppppppptpppptpppp") &&

	(Window=Win4Menu(Term_Menu)) &&
	Row4Menu(Window, COD_SPAWN, ROW_ACTIVE, 10, " New Term ") &&
	Row4Menu(Window, COD_QUIT,  FALSE,       6, " Exit ") &&
	Item4Menu(Term_Menu, Window, TRUE, 6, " File ") &&
	
	Item4MenuCommon(Term_Menu)) {

	RegisterExt(Term,Open,OpenTerm);
#ifdef THIS_MODULE
	OverrideMethods(TRUE);
#endif

	if (default_args[1][0] == '/')
	    default_args[1][0] = '-';
	return TRUE;
    }
    if (shellpath)
	printk("twin: InitTerm(): %."STR(SMALLBUFF)"s\n", ErrStr);
    else
	printk("twin: environment variable $SHELL not set!\n");
    return FALSE;
}

#ifdef THIS_MODULE
void QuitModule(module Module) {
    UnRegisterExt(Term,Open,OpenTerm);
    OverrideMethods(FALSE);
    if (Term_MsgPort)
	Delete(Term_MsgPort);
}
#endif /* THIS_MODULE */
