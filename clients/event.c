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

#include <libTw.h>

static tmsgport Event_MsgPort;
static tmenu Event_Menu;
static twindow Event_Win;

static byte InitEvent(void) {
    if (!TwOpen(NULL))
	return FALSE;
    
    if ((Event_MsgPort=TwCreateMsgPort
	 (12, "Event Tester", (time_t)0, (frac_t)0, (byte)0)) &&
	(Event_Menu=TwCreateMenu
	 (Event_MsgPort, COL(BLACK,WHITE), COL(BLACK,GREEN), COL(HIGH|BLACK,WHITE), COL(HIGH|BLACK,BLACK),
	  COL(RED,WHITE), COL(RED,GREEN), (byte)0)) &&
	TwItem4MenuCommon(Event_Menu)) {
	
	TwInfo4Menu(Event_Menu, TW_ROW_ACTIVE, 14, " Event Tester ", "ptppppptpppppp");
    } else
	return FALSE;
    
    if ((Event_Win=TwCreateWindow
	 (12, "Event Tester", NULL,
	  Event_Menu, COL(WHITE,BLACK), TW_NOCURSOR,
	  TW_WINDOW_WANT_KEYS|TW_WINDOW_WANT_MOUSE|TW_WINDOW_WANT_CHANGES|TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_CLOSE,
	  0, 20, 10, 0))) {
	
	TwSetColorsWindow(Event_Win, 0x1FF,
			  COL(HIGH|YELLOW,CYAN), COL(HIGH|GREEN,HIGH|BLUE), COL(WHITE,HIGH|BLUE),
			  COL(HIGH|WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE),
			  COL(WHITE,BLACK), COL(WHITE,HIGH|BLACK), COL(HIGH|BLACK,BLACK), COL(BLACK,HIGH|BLACK));
	TwConfigureWindow(Event_Win, 0xF<<2, 0, 0, 10, 5, 30, 15);
    } else
	return FALSE;
    
    
    TwMapWindow(Event_Win, TwFirstScreen());
    TwFlush();

    return TRUE;
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

int main(int argc, char *argv[]) {
    tmsg Msg;
    udat Code, origCode;
    
    if (InitEvent()) while ((Msg=TwReadMsg(TRUE))) {
	if (Msg->Type==TW_MSG_WINDOW_KEY) {
	    tevent_keyboard EventK = &Msg->Event.EventKeyboard;
	    
	    printf("Key: Code %u (0x%04x), ShiftFlags 0x%04x, ASCII ",
		   (unsigned)EventK->Code, (unsigned)EventK->Code,
		   (unsigned)EventK->ShiftFlags);
	    human_print(EventK->SeqLen, EventK->AsciiSeq);
	    putchar('\n');
	} else if (Msg->Type==TW_MSG_SELECTION) {
	    uldat Owner = TwGetOwnerSelection();
	    printf("Selection Paste: Owner %d (0x%08x), requesting data notify...\n",
		   Owner, Owner);
	    TwRequestSelection(Owner, NOID);
	} else if (Msg->Type==TW_MSG_SELECTIONNOTIFY) {
	    tevent_selectionnotify EventN = &Msg->Event.EventSelectionNotify;

	    printf("Selection Notify: Magic %u (0x%08x), Len %u, ASCII ",
		   (unsigned)EventN->Magic, (unsigned)EventN->Magic, (unsigned)EventN->Len);
	    human_print(EventN->Len, EventN->Data);
	    putchar('\n');
	} else if (Msg->Type==TW_MSG_WINDOW_MOUSE) {
	    byte *s1, *s2, *s3, *s4, *s5, *s6, *s7;
	    tevent_mouse EventM = &Msg->Event.EventMouse;
	    
	    origCode=Code=EventM->Code;
	    
	    if (isPRESS(Code)) {
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
	    
	} else if (Msg->Type==TW_MSG_WINDOW_GADGET) {
	    tevent_gadget EventG = &Msg->Event.EventGadget;
	    printf("Window Gadget: Code %d (0x%04x)\n", (unsigned)EventG->Code, (unsigned)EventG->Code);
	    if (EventG->Code == 0 && EventG->Window == Event_Win) {
		break;
	    }
	} else if (Msg->Type==TW_MSG_WINDOW_CHANGE) {
	    tevent_window EventW = &Msg->Event.EventWindow;
	    printf("Window Change: new size x=%d y=%d\n",
		   EventW->XWidth - 2, EventW->YWidth - 2);
	}
    }
    
    if (TwErrno)
	printf("%s: libTw error: %s\n", argv[0], TwStrError(TwErrno));
    
    TwClose();
    return 0;
}

