/*
 *  clutter.c  --  Useless client that spams twin with windows. Used for benchmarks ;-)
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

static tmsgport Clutter_MsgPort;
static tmenu Clutter_Menu;
static tscreen Clutter_Screen;

dat X, Y;

#define MAXLRAND48 0x7FFFFFFFl

static byte NewClutterWindow(void) {
    twindow Window;
    if ((Window = TwCreateWindow
	 (7, "Clutter", NULL,
	  Clutter_Menu, COL(BLACK,HIGH|BLACK), TW_LINECURSOR,
	  TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_X_BAR|TW_WINDOW_Y_BAR|TW_WINDOW_CLOSE,
	  TW_WINDOWFL_ROWS_DEFCOL,
	  11, 3, 0))) {
	
	TwConfigureWindow(Window, 0x3, lrand48() / (MAXLRAND48 / X),
			  lrand48() / (MAXLRAND48 / Y), 0, 0, 0, 0);
	TwMapWindow(Window, Clutter_Screen);
	return TRUE;
    }
    return FALSE;
}

TW_DECL_MAGIC(clutter_magic);

static byte InitClutter(void) {

    if (!TwCheckMagic(clutter_magic) || !TwOpen(NULL))
	return FALSE;
    
    srand48(time(NULL));
    Clutter_Screen = TwFirstScreen();
    X = TwGetDisplayWidth();
    Y = TwGetDisplayHeight();

    if ((Clutter_MsgPort=TwCreateMsgPort(9, "twclutter")) &&
	(Clutter_Menu=TwCreateMenu(
	  COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0)) &&
	TwItem4MenuCommon(Clutter_Menu)) {
	
	TwInfo4Menu(Clutter_Menu, TW_ROW_ACTIVE, 17, " Useless Clutter ", NULL);

	return TRUE;
    }
    return FALSE;
}

int main(int argc, char *argv[]) {
    tmsg Msg;
    uldat err;
    
    if (!InitClutter()) {
	err = TwErrno;
	fprintf(stderr, "%s: libTw error: %s%s\n", argv[0],
		TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
	return 1;
    }
    
    while (NewClutterWindow()) {
	while ((Msg=TwReadMsg(FALSE))) {
	    if (Msg->Type==TW_MSG_WIDGET_GADGET) {
		if (Msg->Event.EventGadget.Code == 0) {
		    return 0;
		}
	    }
	}
    }
    if ((err = TwErrno)) {
	fprintf(stderr, "%s: libTw error: %s%s\n", argv[0],
		TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
	return 1;
    }
    return 0;
}

