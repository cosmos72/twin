/*
 *  display.c  -- connect to a running twin and register as a display
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
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#include "twin.h"
#include "hw.h"
#include "hw_private.h"

#include "libTw.h"

static void usage(byte *name) {
    fprintf(stderr, "Usage: %s [-v|-s] [-twin@<TWDISPLAY>] -hw=<display>[,options]\n"
	    "Currently known display methods: \n"
	    "\tX[@<XDISPLAY>]\n"
	    "\ttwin[@<TWDISPLAY>]\n"
	    "\ttty[@<tty device>]\n"
	    "\tggi[@<ggi display>]\n", name);
}

static display_hw *HW;

int main(int argc, char *argv[]) {
    byte redirect = 1;
    byte *dpy = NULL, *arg = NULL, *tty = ttyname(0), *name = argv[0];
    byte ret = 0, ourtty = 0;
    byte *s, *buff;
    uldat chunk;
    tmsg Msg;
    
    while (*++argv) {
	if (!strcmp(*argv, "-v"))
	    redirect = 1;
	else if (!strcmp(*argv, "-s"))
	    redirect = 0;
	else if (!strncmp(*argv, "-twin@", 6))
	    dpy = *argv + 6;
	else if (!strncmp(*argv, "-hw=", 4)) {
	    if (!strncmp(*argv+4, "display", 7)) {
		printf("%s: argument `-hw=display' is for internal use only.\n", name);
		usage(name);
		return 1;
	    }
	    if (!strncmp(*argv+4, "tty", 3)) {
		buff = *argv + 7;
		s = strchr(buff, ',');
		if (s) *s = '\0';
		
		if (!*buff || (*buff == '@' && (!buff[1] || !strcmp(buff+1, tty))))
		    /* attach twin to our tty */
		    ourtty = 1;
		/*
		 * using server tty makes no sense for twdisplay
		 */
		if (s) *s = ',';
		else s = "";
		
		if (ourtty) {
		    buff = getenv("TERM");
		    if (!buff) buff = "";
		    
		    arg = malloc(strlen(tty) + 9 + strlen(s) + (buff ? 6 + strlen(buff) : 0));
		    
		    sprintf(arg, "-hw=tty@%s%s%s%s", tty, (buff ? (byte *)",TERM=" : buff), buff, s);
		} else
		    arg = *argv;
	    } else if ((*argv)[4])
		arg = *argv;
	    else {
		usage(name);
		return 1;
	    }
	} else {
	    usage(name);
	    return 1;
	}
    }
    
    if (!arg) {
	usage(name);
	return 1;
    }

    if (!(HW = AttachDisplayHW(strlen(arg), arg, 0)))
	return 1;
			 
    if (TwOpen(dpy) && TwCreateMsgPort(9, "twdisplay", (uldat)0, (udat)0, (byte)0)) do {
	byte buf[] = "-hw=display\0resize,drag,slow";
	
	sprintf(buf+11, "%s%s%s",
		myHW->CanResize ? ",resize" : "",
		myHW->CanDragArea ? ",drag" : "",
		myHW->ExpensiveFlushVideo ? ",slow" : "");
	
	TwAttachHW(strlen(buf), buf, redirect);
	TwFlush();
	
	if (redirect)
	    printf("reported messages ...\n");
	
	for (;;) {
	    buff = TwAttachGetReply(&chunk);
	    if (buff <= (byte *)2) {
		ret = (byte)(size_t)buff;
		break;
	    } else if (buff == (byte *)-1)
		/* libTw panic */
		break;

	    printf("%.*s", (int)chunk, buff);
	}
	fflush(stdout);
	
	if (TwInPanic())
	    break;

	TwAttachConfirm();
	/*
	 * twin waits this before grabbing the display...
	 * so we can fflush(stdout) to show all messages
	 * *BEFORE* twin draws on (eventually) the same tty
	 */
    
	if (ret == 2) {
	    /*
	     * twin told us to stay and sit on the display
	     * until it is quitted.
	     */
	    int fd = TwConnectionFd();
	    fd_set fds;
	    FD_ZERO(&fds);

	    while (!TwInPanic()) {
		while ((Msg = TwReadMsg(FALSE)))
		    HandleMsg(Msg);
		
		if (NeedHW & NEEDResizeDisplay)
		    ResizeDisplay();
	    
		if (NeedHW & NEEDExportClipBoard)
		    ExportClipBoard();
	    
		if (NeedHW & NEEDPanicHW)
		    Panic();
	    
		HW->FlushVideo();
		if (HW->NeedHW & NEEDFlushHW)
		    HW->FlushHW();

		FD_SET(fd, &fds);
		select(fd+1, &fds, NULL, NULL, NULL);
		if (gotSignalWinch)
		    
	    }
	} else {
	    if (redirect) {
		if (ret)
		    printf("... ok, twin successfully attached.\n");
		else
		    printf("... ach, twin failed to attach.\n");
	    }
	    printf("%s: twin said we can quit... strange!\n", name);
	}
	return !ret;
    } while (0);
    
    fprintf(stderr, "%s: libTw error: %s\n", name, TwStrError(TwErrno));
    return 1;
}

