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

#include "libTw.h"

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
	
	TwInfo4Menu(Event_Menu, TW_ROW_ACTIVE, (uldat)14, " Event Tester ", "ptppppptpppppp");
    } else
	return FALSE;
    
    if ((Event_Win=TwCreateWindow
	 (14, " Event Tester ", "\x9F\x9F\x9F\x9F\x9F\x9F\x9F\x9F\x9F\x9F\x9F\x9F\x9F\x9F",
	  Event_Menu, COL(WHITE,BLACK), TW_NOCURSOR,
	  TW_WINDOW_WANT_KEYS|TW_WINDOW_WANT_MOUSE|TW_WINDOW_WANT_CHANGES|TW_WINDOW_DRAG|TW_WINDOW_RESIZE|TW_WINDOW_CLOSE,
	  0, (udat)20, (udat)10, (udat)0))) {
	
	TwSetColorsWindow(Event_Win, 0x1FF,
			  COL(HIGH|YELLOW,CYAN), COL(HIGH|GREEN,HIGH|BLUE), COL(WHITE,HIGH|BLUE),
			  COL(HIGH|WHITE,HIGH|BLUE), COL(HIGH|WHITE,HIGH|BLUE),
			  COL(WHITE,BLACK), COL(WHITE,HIGH|BLACK), COL(HIGH|BLACK,BLACK), COL(BLACK,HIGH|BLACK));
	TwConfigureWindow(Event_Win, 0xF<<2, 0, 0, (udat)10, (udat)5, (udat)30, (udat)15);
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
    tevent_keyboard EventK;
    tevent_gadget EventG;
    tevent_clipboard EventC;
    tevent_mouse EventM;
    tevent_window EventW;
    udat Code, origCode;
    
    if (InitEvent()) while ((Msg=TwReadMsg(TRUE))) {
	if (Msg->Type==TW_MSG_WINDOW_KEY) {
	    EventK=&Msg->Event.EventKeyboard;
	    printf("Key: Code %d (0x%04x), ASCII ", (int)EventK->Code, (int)EventK->Code);
	    human_print(EventK->SeqLen, EventK->AsciiSeq);
	    putchar('\n');
	} else if (Msg->Type==TW_MSG_CLIPBOARD) {
	    EventC=&Msg->Event.EventClipBoard;
	    printf("Clipboard Paste: Magic %d (0x%08x), ASCII ", (unsigned)EventC->Magic, (unsigned)EventC->Magic);
	    human_print(EventC->Len, EventC->Data);
	    putchar('\n');
	} else if (Msg->Type==TW_MSG_WINDOW_MOUSE) {
	    byte *s1, *s2, *s3, *s4, *s5, *s6, *s7;
	    
	    EventM=&Msg->Event.EventMouse;
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
	    EventG=&Msg->Event.EventGadget;
	    printf("Window Gadget: Code %d (0x%04x)\n", (unsigned)EventG->Code, (unsigned)EventG->Code);
	    if (EventG->Code == 0 && EventG->Window == Event_Win) {
		break;
	    }
	} else if (Msg->Type==TW_MSG_WINDOW_CHANGE) {
	    EventW=&Msg->Event.EventWindow;
	    printf("Window Change: new size x=%d y=%d\n",
		   EventW->XWidth - 2, EventW->YWidth - 2);
	}
    }
    
    if (TwErrno)
	printf("%s: libTw error: %s\n", argv[0], TwStrError(TwErrno));
    
    TwClose();
    return 0;
}

