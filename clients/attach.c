/*
 *  attach.c  --  connect to a running twin and tell it to attach/detach
 *                from a given display
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

#include "libTw.h"

void usage(byte *name) {
    fprintf(stderr, "Usage: %s [-a|-d] [-v|-s] [-twin@<TWDISPLAY>] -hw=<display>[,options]\n"
	    "Currently known display methods: \n"
	    "\tX[@<XDISPLAY>]\n"
	    "\ttwin[@<TWDISPLAY>]\n"
	    "\ttty[@<tty device>]\n", name);
}

byte gotSignalWinch;

void SignalWinch(int n) {
    signal(SIGWINCH, SignalWinch);
    gotSignalWinch = TRUE;
}
    
int main(int argc, char *argv[]) {
    byte detach = 0, redirect = 1;
    byte *dpy = NULL, *arg = NULL, *tty = ttyname(0), *name = argv[0];
    byte ret = 0, mustwait = 0;
    byte *s, *buff;
    uldat chunk;
    
    if (strstr(argv[0], "detach"))
	detach = 1;

    while (*++argv) {
	if (!strcmp(*argv, "-d"))
	    detach = 1;
	else if (!strcmp(*argv, "-a"))
	    detach = 0;
	else if (!strcmp(*argv, "-v"))
	    redirect = 2;
	else if (!strcmp(*argv, "-s"))
	    redirect = 0;
	else if (!strncmp(*argv, "-twin@", 6))
	    dpy = *argv + 6;
	else if (!strncmp(*argv, "-hw=", 4)) {
	    if (!strncmp(*argv+4, "tty", 3)) {
		buff = *argv + 7;
		s = strchr(buff, ',');
		if (s) *s = '\0';
		
		if (!*buff || (*buff == '@' && (!buff[1] || buff[1] == '-' || !strcmp(buff+1, tty))))
		    /* attach twin to our tty */
		    mustwait = 1;
		
		if (s) *s = ',';
		else s = "";
		
		if (mustwait) {
		    if (redirect == 1)
			redirect = 0;
			
		    buff = getenv("TERM");
		    if (!buff) buff = "";
		    
		    arg = malloc(strlen(tty) + 9 + (s ? strlen(s) : 0) + (buff ? 6 + strlen(buff) : 0));
		    
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
    
    if (detach == 0 && !arg) {
	usage(name);
	return 1;
    }
    
    if (TwOpen(dpy)) do {
	if (detach) {
	    TwDetachHW(); /* cannot fail */
	    TwFlush();
	    return 0;
	}

	TwAttachHW(arg ? strlen(arg) : 0, arg, redirect);
	TwFlush();
	
	signal(SIGWINCH, SignalWinch);
	    
	if (redirect)
	    printf("twin reported:\n");
	
	for (;;) {
	    buff = TwAttachGetReply(&chunk);
	    if (buff == (byte *)FALSE || buff == (byte *)TRUE) {
		ret = (byte)(size_t)buff;
		break;
	    } else if (buff == (byte *)-1)
		/* libTw panic */
		break;
	    
	    printf("%.*s", (int)chunk, buff);

	    if (mustwait && gotSignalWinch)
		TwNeedResizeDisplay(), TwFlush(), gotSignalWinch = FALSE;
	}

	if (TwInPanic())
	    break;
	
	if (ret && mustwait) {
	    int fd = TwConnectionFd();
	    fd_set fds;
	    FD_ZERO(&fds);
	    
	    while (!TwInPanic()) {
		while (TwReadMsg(FALSE))
		    ;
		FD_SET(fd, &fds);
		select(fd+1, &fds, NULL, NULL, NULL);
		if (gotSignalWinch)
		    TwNeedResizeDisplay(), TwFlush(), gotSignalWinch = FALSE;
	    }
	} else if (redirect) {
	    if (ret)
		printf("ok... twin successfully attached.\n");
	    else
		printf("ach... twin failed to attach.\n");
	}
	return !ret;
    } while (0);
    
    fprintf(stderr, "%s: libTw error: %s\n", name, TwStrError(TwErrno));
    return 1;
}

