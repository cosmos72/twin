/*
 *  edit.c  --  a wannabe editor for twin. Currently just echoes keyboard.
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>

#include "libTw.h"
#include "libTwerrno.h"

#define COD_QUIT      (udat)1

static tmsgport Clip_MsgPort;
static tmenu Clip_Menu;
static twindow Clip_Win;

static byte InitClip(void) {
    twindow Window;

    if (!TwOpen(NULL))
	return FALSE;
    
    if ((Clip_MsgPort=TwCreateMsgPort
	 (11, "twclipboard", (time_t)0, (frac_t)0, (byte)0)) &&
	(Clip_Menu=TwCreateMenu
	 (Clip_MsgPort, COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0))) {
	
	TwInfo4Menu(Clip_Menu, TW_ROW_ACTIVE, 16, " Twin Clipboard ", "ptpppptppppppppp");
    } else
	return FALSE;
    
    if ((Clip_Win=TwCreateWindow
	 (14, "Twin Clipboard", NULL,
	  Clip_Menu, COL(HIGH|WHITE,HIGH|BLACK),
	  TW_LINECURSOR,
	  TW_WINDOW_WANT_KEYS|TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_X_BAR|TW_WINDOW_Y_BAR|TW_WINDOW_CLOSE,
	  TW_WINFL_CURSOR_ON|TW_WINFL_USE_DEFCOL,
	  40, 20, 0)) &&
	(Window=TwWin4Menu(Clip_Menu)) &&
	(TwRow4Menu(Window, COD_QUIT, TW_ROW_INACTIVE, 17, " Quit      Alt-X ") ,
	 TwItem4Menu(Clip_Menu, Window, TRUE, 6, " File "))) {

	TwSetColorsWindow(Clip_Win, 0x1FF,
			  COL(HIGH|GREEN,WHITE), COL(CYAN,BLUE), COL(HIGH|BLUE,BLACK), COL(HIGH|WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE),
			  COL(HIGH|WHITE,HIGH|BLACK), COL(HIGH|BLACK,WHITE), COL(BLACK,HIGH|BLACK), COL(BLACK,WHITE));
	TwConfigureWindow(Clip_Win, 0xF<<2, 0, 0, 7, 3, MAXDAT, MAXDAT);

    } else
	return FALSE;
    
    
    if ((Window=TwWin4Menu(Clip_Menu)) &&
	TwItem4Menu(Clip_Menu, Window, FALSE, 6, " Clip ")) {
		
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
    
    if ((Window=TwWin4Menu(Clip_Menu)) &&
	TwItem4Menu(Clip_Menu, Window, TRUE, 8, " Search ")) {

	TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE,  9, " Find    ");
	TwRow4Menu(Window, (udat)0, TW_ROW_ACTIVE,  9, " Replace ");
    } else
	return FALSE;
    
    if (!TwItem4MenuCommon(Clip_Menu))
	return FALSE;

    TwMapWindow(Clip_Win, TwFirstScreen());
    TwFlush();

    return TRUE;
}

int main(int argc, char *argv[]) {
    tmsg Msg;
    udat Code;
    uldat err, WinN = 1;
    
    if (InitClip()) while ((Msg=TwReadMsg(TRUE))) {
	if (Msg->Type==TW_MSG_WINDOW_KEY) {
	    
	    tevent_keyboard EventK = &Msg->Event.EventKeyboard;
	    Code=EventK->Code;
	    (void)TwWriteRowWindow(EventK->Window, EventK->SeqLen, EventK->AsciiSeq);
	    
	} else if (Msg->Type==TW_MSG_SELECTION) {
	    /*
	     * send Msg->Event.EventSelection.Window as ReqPrivate field,
	     * so that we will get it back in TW_MSG_SELECTIONNOTIFY message
	     * without having to store it manually
	     */
	    TwRequestSelection(TwGetOwnerSelection(), Msg->Event.EventSelection.Window);
	    
	} else if (Msg->Type==TW_MSG_SELECTIONNOTIFY) {
	    
	    tevent_selectionnotify EventN = &Msg->Event.EventSelectionNotify;
	    if (EventN->Magic == TW_SEL_TEXTMAGIC)
		(void)TwWriteRowWindow(EventN->ReqPrivate, EventN->Len, EventN->Data);
	    
	} else if (Msg->Type==TW_MSG_SELECTIONCLEAR) {
	    ;
	} else if (Msg->Type==TW_MSG_WINDOW_GADGET) {
	    
	    tevent_gadget EventG = &Msg->Event.EventGadget;
	    if (EventG->Code == 0 && EventG->Window == Clip_Win) {
		TwUnMapWindow(Clip_Win);
		TwDeleteWindow(Clip_Win);
		if (!--WinN)
		    break;
	    }
	}
    }
    if ((err = TwErrno))
	fprintf(stderr, "%s: libTw error: %s%s\n", argv[0],
		TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
    
    if (!TwInPanic()) {
	/* these are not strictly necessary, as the server would cleanup by itself... */
	TwDeleteMenu(Clip_Menu);
	TwDeleteMsgPort(Clip_MsgPort);
	TwClose();
	return 0;
    }
    return 1;
}

