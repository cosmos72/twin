/*
 *  event.c  --  print libTw events in human readable form
 *
 *  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <stdio.h>

#include "Tw/Tw.h"
#include "Tw/Twerrno.h"
#include "Tw/Twkeys.h"

static tmsgport Event_MsgPort;
static tmenu Event_Menu;
static twindow Event_Win, Event_SubWin;

TW_DECL_MAGIC(event_magic);

static byte InitEvent(void) {
    if (!TwCheckMagic(event_magic) || !TwOpen(NULL))
	return FALSE;
    
    if ((Event_MsgPort=TwCreateMsgPort
	 (12, "Event Tester", (time_t)0, (frac_t)0, (byte)0)) &&
	(Event_Menu=TwCreateMenu(
	  COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0)) &&
	TwItem4MenuCommon(Event_Menu)) {
	
	TwInfo4Menu(Event_Menu, TW_ROW_ACTIVE, 14, " Event Tester ", "ptppppptpppppp");
    
	if ((Event_Win=TwCreateWindow
	     (12, "Event Tester", NULL,
	      Event_Menu, COL(WHITE,BLACK), TW_NOCURSOR,
	      TW_WINDOW_WANT_KEYS|TW_WINDOW_WANT_MOUSE|TW_WINDOW_WANT_CHANGES|TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_CLOSE,
	      TW_WINDOWFL_USEEXPOSE, 18, 8, 0))) {
	
	    TwSetColorsWindow(Event_Win, 0x1FF,
			      COL(HIGH|YELLOW,CYAN), COL(HIGH|GREEN,HIGH|BLUE), COL(WHITE,HIGH|BLUE),
			      COL(HIGH|WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE),
			      COL(WHITE,BLACK), COL(WHITE,HIGH|BLACK), COL(HIGH|BLACK,BLACK), COL(BLACK,HIGH|BLACK));
	    TwConfigureWindow(Event_Win, 0xF<<2, 0, 0, 10, 5, 30, 15);
	    TwMapWindow(Event_Win, TwFirstScreen());

	    if ((Event_SubWin=TwCreateWindow
		 (9, "SubWindow", NULL,
		  Event_Menu, COL(WHITE,BLUE), TW_NOCURSOR,
		  TW_WINDOW_WANT_KEYS|TW_WINDOW_WANT_MOUSE|TW_WINDOW_WANT_MOUSE_MOTION|TW_WINDOW_WANT_CHANGES,
		  TW_WINDOWFL_BORDERLESS, 7, 3, 0))) {

		TwSetXYWindow(Event_SubWin, 5, 2);
		TwMapWindow(Event_SubWin, Event_Win);

		return TwFlush();
	    }
	}
    }
    return FALSE;
}

void human_print(uldat len, byte *s) {
    byte c;
    
    putchar('`');
    
    while (len--) {
	c = *s++;
	
	if (c >= ' ' && c <= '~')
	    putchar(c);
	else
	    printf("\\x%02x", (int)c);
    }
    
    putchar('\'');
}

#define IS(name) case TW_##name: return #name
    
byte *twkeyname(udat Code) {
    static byte buf[2];
    
    switch (Code) {
	IS(Tab);
	IS(Linefeed);
	IS(Return);
	IS(Escape);
	IS(space);
	IS(BackSpace);
	IS(Home);
	IS(Left);
	IS(Up);
	IS(Right);
	IS(Down);
	IS(Prior);
	IS(Next);
	IS(End);
	IS(Insert);
	IS(Delete);
	IS(Pause);
	IS(Scroll_Lock);
	IS(Sys_Req);
	IS(Clear);
	IS(Select);
	IS(Print);
	IS(Execute);
	IS(Undo);
	IS(Redo);
	IS(Menu);
	IS(Find);
	IS(Cancel);
	IS(Help);
	IS(Break);
	IS(Num_Lock);
	IS(KP_Home);
	IS(KP_Left);
	IS(KP_Up);
	IS(KP_Right);
	IS(KP_Down);
	IS(KP_Prior);
	IS(KP_Next);
	IS(KP_End);
	IS(KP_Insert);
	IS(KP_Delete);
	IS(KP_Multiply);
	IS(KP_Add);
	IS(KP_Separator);
	IS(KP_Subtract);
	IS(KP_Decimal);
	IS(KP_Divide);
	IS(KP_Equal);
	IS(KP_F1);
	IS(KP_F2);
	IS(KP_F3);
	IS(KP_F4);
	IS(KP_Space);
	IS(KP_Tab);
	IS(KP_Enter);
	IS(KP_0);
	IS(KP_1);
	IS(KP_2);
	IS(KP_3);
	IS(KP_4);
	IS(KP_5);
	IS(KP_6);
	IS(KP_7);
	IS(KP_8);
	IS(KP_9);
	IS(F1);
	IS(F2);
	IS(F3);
	IS(F4);
	IS(F5);
	IS(F6);
	IS(F7);
	IS(F8);
	IS(F9);
	IS(F10);
	IS(F11);
	IS(F12);
	IS(F13);
	IS(F14);
	IS(F15);
	IS(F16);
	IS(F17);
	IS(F18);
	IS(F19);
	IS(F20);
	IS(EcuSign);
	IS(ColonSign);
	IS(CruzeiroSign);
	IS(FFrancSign);
	IS(LiraSign);
	IS(MillSign);
	IS(NairaSign);
	IS(PesetaSign);
	IS(RupeeSign);
	IS(WonSign);
	IS(NewSheqelSign);
	IS(DongSign);
	IS(EuroSign);
	IS(XTermMouse);
	IS(Null);
      default:
	if (Code > (udat)' ' && Code <= (udat)'~') {
	    buf[0] = (byte)Code;
	    buf[1] = '\0';
	    return buf;
	}
	break;
    }
    return "unknown";
}

byte *twcontrolname(udat Code) {
    switch(Code) {
      case TW_MSG_CONTROL_QUIT:
	return "quit";
      case TW_MSG_CONTROL_RESTART:
	return "restart";
      case TW_MSG_CONTROL_OPEN:
	return "open";
      case TW_MSG_CONTROL_DRAGNDROP:
	return "drag-and-drop";
      default:
	return "unknown";
    }
}

int main(int argc, char *argv[]) {
    tmsg Msg;
    uldat err;
    udat Code, origCode;
    
    if (InitEvent()) while ((Msg=TwReadMsg(TRUE))) {
	if (Msg->Event.EventCommon.W != Event_Win)
	    printf("SubWindow: ");
	    
	if (Msg->Type==TW_MSG_WIDGET_KEY) {
	    tevent_keyboard EventK = &Msg->Event.EventKeyboard;
	    
	    printf("Key: Code %.3u (0x%04x, %s),\tShiftFlags 0x%04x, ASCII ",
		   (unsigned)EventK->Code, (unsigned)EventK->Code, twkeyname(EventK->Code),
		   (unsigned)EventK->ShiftFlags);
	    human_print(EventK->SeqLen, EventK->AsciiSeq);
	    putchar('\n');
	} else if (Msg->Type==TW_MSG_SELECTION) {
	    uldat Owner = TwGetOwnerSelection();
	    printf("Selection Paste: Owner %d (0x%08x), requesting data notify...\n",
		   Owner, Owner);
	    TwRequestSelection(Owner, TW_NOID);
	} else if (Msg->Type==TW_MSG_SELECTIONNOTIFY) {
	    tevent_selectionnotify EventN = &Msg->Event.EventSelectionNotify;

	    printf("Selection Notify: Magic %u (0x%08x), Len %u, ASCII ",
		   (unsigned)EventN->Magic, (unsigned)EventN->Magic, (unsigned)EventN->Len);
	    human_print(EventN->Len, EventN->Data);
	    putchar('\n');
	} else if (Msg->Type==TW_MSG_WIDGET_MOUSE) {
	    byte *s1, *s2, *s3, *s4, *s5, *s6, *s7;
	    tevent_mouse EventM = &Msg->Event.EventMouse;
	    
	    origCode=Code=EventM->Code;
	    
	    if (isMOTION(Code)) {
		s1 = "Motion";
		s2 = "";
	    } else if (isPRESS(Code)) {
		s1 = "Press";
		switch (Code & PRESS_ANY) {
		  case PRESS_LEFT:   s2 = " Left";   Code &= ~HOLD_LEFT; break;
		  case PRESS_MIDDLE: s2 = " Middle"; Code &= ~HOLD_MIDDLE; break;
		  case PRESS_RIGHT:  s2 = " Right";  Code &= ~HOLD_RIGHT; break;
		  default: s2 = "Unknown Button "; break;
		}
	    } else if (isRELEASE(Code)) {
		s1 = "Release";
		switch (Code & RELEASE_ANY) {
		  case RELEASE_LEFT:   s2 = " Left";   break;
		  case RELEASE_MIDDLE: s2 = " Middle"; break;
		  case RELEASE_RIGHT:  s2 = " Right";  break;
		  default: s2 = "Unknown Button "; break;
		}
	    } else if (isDRAG(Code)) {
		s1 = "Drag";
		s2 = "";
	    } else {
		s1 = "Unknown Event ";
		s2 = "";
	    }
	    
	    if ((Code &= HOLD_ANY)) {
		s3 = "while ";
		s4 = Code & HOLD_LEFT ? "Left " : "";
		s5 = Code & HOLD_MIDDLE ? "Middle " : "";
		s6 = Code & HOLD_RIGHT ? "Right " : "";
		s7 = "held, ";
	    } else
		s3 = s4 = s5 = s6 = s7 = "";
	    
	    printf("Mouse: Code 0x%04x: %s%s, %s%s%s%s%sx=%d, y=%d\n",
		   (int)origCode, s1, s2, s3, s4, s5, s6, s7, EventM->X, EventM->Y);
	    
	} else if (Msg->Type==TW_MSG_WIDGET_GADGET) {
	    tevent_gadget EventG = &Msg->Event.EventGadget;
	    printf("Gadget: Code %d (0x%04x)\n", (unsigned)EventG->Code, (unsigned)EventG->Code);
	    if (EventG->Code == 0 && EventG->W == Event_Win) {
		break;
	    }
	} else if (Msg->Type==TW_MSG_WIDGET_CHANGE) {
	    tevent_widget EventW = &Msg->Event.EventWidget;
	    if (EventW->Code == TW_MSG_WIDGET_RESIZE)
		printf("Widget Change: new size x=%d y=%d\n",
		       EventW->XWidth, EventW->YWidth);
	    else if (EventW->Code == TW_MSG_WIDGET_EXPOSE) {
		printf("Widget Change: expose x=%d, y=%d, width=%d, height=%d%s\n",
		       EventW->X, EventW->Y, EventW->XWidth, EventW->YWidth,
		       EventW->Flags & TW_MSG_WIDGETFL_SHADED ? ", shaded" : "");
		TwExposeTextWindow(EventW->W, EventW->XWidth, EventW->YWidth,
				   EventW->X, EventW->Y, 0, NULL);
	    }
	} else if (Msg->Type==TW_MSG_USER_CONTROL) {
	    tevent_control EventC = &Msg->Event.EventControl;
	    printf("User Control Message: Code %d (%s), x=%d y=%d, ASCII ",
		   EventC->Code, twcontrolname(EventC->Code), EventC->X, EventC->Y);
	    human_print(EventC->Len, EventC->Data);
	    putchar('\n');
	} else if (Msg->Type==TW_MSG_USER_CLIENTMSG) {
	    tevent_clientmsg EventC = &Msg->Event.EventClientMsg;
	    printf("User Client Message: Code %d, ASCII ", EventC->Code);
	    human_print(EventC->Len, EventC->Data);
	    putchar('\n');
	}
    }
    
    if ((err = TwErrno))
	printf("%s: libTw error: %s%s\n", argv[0],
	       TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
    
    TwClose();
    return 0;
}

