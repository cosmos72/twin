/*
 *  edit.c  --  a wannabe editor for twin. Currently just echoes keyboard.
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>

#include "libTw.h"

#define COD_QUIT      (udat)1

static tmsgport Edit_MsgPort;
static tmenu Edit_Menu;
static twindow Edit_Win;

static byte InitEdit(void) {
    twindow Window;

    if (!TwOpen(NULL))
	return FALSE;
    
    if ((Edit_MsgPort=TwCreateMsgPort
	 (11, "Twin Editor", (time_t)0, (frac_t)0, (byte)0)) &&
	(Edit_Menu=TwCreateMenu
	 (Edit_MsgPort, COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0))) {
	
	TwInfo4Menu(Edit_Menu, TW_ROW_ACTIVE, (uldat)13, " Twin Editor ", "ptpppptpppppp");
    } else
	return FALSE;
    
    if ((Edit_Win=TwCreateWindow
	 (13, " Twin Editor ", "\x8C\x84\x84\x84\x84\x8C\x84\x84\x84\x84\x84\x84\x84",
	  Edit_Menu, COL(HIGH|WHITE,HIGH|BLACK),
	  TW_LINECURSOR,
	  TW_WINDOW_WANT_KEYS|TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_X_BAR|TW_WINDOW_Y_BAR|TW_WINDOW_CLOSE,
	  TW_WINFL_CURSOR_ON|TW_WINFL_USE_DEFCOL,
	  (udat)40, (udat)20, (udat)0)) &&
	(Window=TwWin4Menu(Edit_Menu)) &&
	(TwRow4Menu(Window, COD_QUIT, TW_ROW_INACTIVE, 17, " Quit      Alt-X ") ,
	 TwItem4Menu(Edit_Menu, Window, TRUE, 6, " File "))) {

	TwSetColorsWindow(Edit_Win, 0x1FF,
			  COL(HIGH|GREEN,WHITE), COL(CYAN,BLUE), COL(HIGH|BLUE,BLACK), COL(HIGH|WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE),
			  COL(HIGH|WHITE,HIGH|BLACK), COL(HIGH|BLACK,WHITE), COL(BLACK,HIGH|BLACK), COL(BLACK,WHITE));
	TwConfigureWindow(Edit_Win, 0x3F, (dat)2, (udat)15,
			  (udat)7, (udat)3, MAXDAT, MAXDAT);

    } else
	return FALSE;
    
    
    if ((Window=TwWin4Menu(Edit_Menu)) &&
	TwItem4Menu(Edit_Menu, Window, FALSE, 6, " Edit ")) {
		
	TwRow4Menu(Window, (udat)0, TW_ROW_INACTIVE,16, " Undo           ");
	TwRow4Menu(Window, (udat)0, TW_ROW_INACTIVE,16, " Redo           ");
	TwRow4Menu(Window, (udat)0, TW_ROW_IGNORE,  16, "컴컴컴컴컴컴컴컴");
	TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE,  16, " Cut            ");
	TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE,  16, " Copy           ");
	TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE,  16, " Paste          ");
	TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE,  16, " Clear          ");
	TwRow4Menu(Window, (udat)0, TW_ROW_IGNORE,  16, "컴컴컴컴컴컴컴컴");
	TwRow4Menu(Window, (udat)0, TW_ROW_INACTIVE,16, " Show Clipboard ");
    } else
	return FALSE;
    
    if ((Window=TwWin4Menu(Edit_Menu)) &&
	TwItem4Menu(Edit_Menu, Window, TRUE, 8, " Search ")) {

	TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE,  9, " Find    ");
	TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE,  9, " Replace ");
    } else
	return FALSE;
    
    if (!TwItem4MenuCommon(Edit_Menu))
	return FALSE;

    TwMapWindow(Edit_Win, TwFirstScreen());
    TwFlush();

    return TRUE;
}

int main(int argc, char *argv[]) {
    tmsg Msg;
    tevent_keyboard EventK;
    tevent_gadget EventG;
    tevent_clipboard EventC;
    udat Code;
    uldat WinN = 1;
    
    if (InitEdit()) while ((Msg=TwReadMsg(TRUE))) {
	if (Msg->Type==TW_MSG_WINDOW_KEY) {
	    EventK=&Msg->Event.EventKeyboard;
	    Code=EventK->Code;
	    (void)TwWriteRowWindow(EventK->Window, EventK->SeqLen, EventK->AsciiSeq);
	} else if (Msg->Type==TW_MSG_CLIPBOARD) {
	    EventC=&Msg->Event.EventClipBoard;
	    if (EventC->Magic == TW_CLIP_TEXTMAGIC)
		(void)TwWriteRowWindow(EventC->Window, EventC->Len, EventC->Data);
	} else if (Msg->Type==TW_MSG_WINDOW_GADGET) {
	    EventG=&Msg->Event.EventGadget;
	    if (EventG->Code == 0 && EventG->Window == Edit_Win) {
		TwUnMapWindow(Edit_Win);
		TwDeleteWindow(Edit_Win);
		if (!--WinN)
		    break;
	    }
	}
    }
    if (TwErrno)
	printf("%s: libTw error: %s\n", argv[0], TwStrError(TwErrno));
    
    if (!TwInPanic()) {
	/* these are not strictly necessary, as the server would cleanup by itself... */
	TwDeleteMenu(Edit_Menu);
	TwDeleteMsgPort(Edit_MsgPort);
	TwClose();
	return 0;
    }
    return 1;
}

