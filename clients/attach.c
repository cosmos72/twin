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

#include "Tw/Tw.h"
#include "Tw/Twerrno.h"
#include "version.h"

static char *MYname;

static void Usage(byte detach) {
    fprintf(stderr, "Usage: %s [OPTIONS] %s\n"
	    "Currently known options: \n"
	    " -h, -help               display this help and exit\n"
	    " -V, -version            output version information and exit\n"
	    " -a                      attach display%s\n"
	    " -d                      detach display%s\n"
	    " -s, -share              allow multiple simultaneous displays (default)\n"
	    " -x, -excl               request exclusive display - detach all others\n"
	    " -v                      verbose output (default)\n"
	    " -q                      quiet - don't report messages from twin server\n"
	    " -f                      force running even with wrong protocol version\n"
	    " -twin@<TWDISPLAY>       specify server to contact instead of $TWDISPLAY\n"
	    " -hw=<display>[,options] start the given display\n"
	    "Currently known display methods: \n"
	    "\tX[@<XDISPLAY>]\n"
	    "\ttwin[@<TWDISPLAY>]\n"
	    "\ttty[@<tty device>]\n"
	    "\tggi[@<ggi display>]\n",
	    MYname, detach ? "" : "-hw=<display> [...]",
	    detach ? "" : " (default)", detach ? " (default)" : "");
}

static byte gotSignalWinch;

static void SignalWinch(int n) {
    signal(SIGWINCH, SignalWinch);
    gotSignalWinch = TRUE;
}

static void ShowVersion(void) {
    printf("%s " TWIN_VERSION_STR " with socket protocol "
	  TW_PROTOCOL_VERSION_STR "\n", MYname);
}

static byte VersionsMatch(byte force) {
    uldat cv = TW_PROTOCOL_VERSION, lv = TwLibraryVersion(), sv = TwServerVersion();
	
    if (lv != sv || lv != cv) {
	fprintf(stderr, "%s: %s: socket protocol version mismatch!%s\n"
		"          client is %d.%d.%d, library is %d.%d.%d, server is %d.%d.%d\n",
		MYname, (force ? "warning" : "fatal"), (force ? " (ignored)" : ""),
		TWVER_MAJOR(cv), TWVER_MINOR(cv), TWVER_PATCH(cv),
		TWVER_MAJOR(lv), TWVER_MINOR(lv), TWVER_PATCH(lv),
		TWVER_MAJOR(sv), TWVER_MINOR(sv), TWVER_PATCH(sv));
	return FALSE;
    }
    return TRUE;
}

TW_DECL_MAGIC(attach_magic);

int main(int argc, char *argv[]) {
    byte detach = 0, redirect, force = 0, flags = TW_ATTACH_HW_REDIRECT;
    byte *dpy = NULL, *arg = NULL, *tty = ttyname(0);
    byte ret = 0, ourtty = 0, servtty = 0;
    byte *s;
    TW_CONST byte *buff;
    uldat chunk;
    
    MYname = argv[0];
    
    if (strstr(argv[0], "detach"))
	detach = 1;

    while (*++argv) {
	if (!strcmp(*argv, "-V") || !strcmp(*argv, "-version")) {
	    ShowVersion();
	    return 0;
	} else if (!strcmp(*argv, "-h") || !strcmp(*argv, "-help")) {
	    Usage(detach);
	    return 0;
	} else if (!strcmp(*argv, "-x") || !strcmp(*argv, "-excl"))
	    flags |= TW_ATTACH_HW_EXCLUSIVE;
	else if (!strcmp(*argv, "-s") || !strcmp(*argv, "-share"))
	    flags &= ~TW_ATTACH_HW_EXCLUSIVE;
	else if (!strcmp(*argv, "-a"))
	    detach = 0;
	else if (!strcmp(*argv, "-d"))
	    detach = 1;
	else if (!strcmp(*argv, "-v"))
	    flags |= TW_ATTACH_HW_REDIRECT;
	else if (!strcmp(*argv, "-q"))
	    flags &= ~TW_ATTACH_HW_REDIRECT;
	else if (!strcmp(*argv, "-f"))
	    force = 1;
	else if (!strncmp(*argv, "-twin@", 6))
	    dpy = *argv + 6;
	else if (!strncmp(*argv, "-hw=", 4)) {
	    if (!strncmp(*argv+4, "tty", 3)) {
		buff = *argv + 7;
		s = strchr(buff, ',');
		if (s) *s = '\0';
		
		if (!*buff)
		    /* attach twin to our tty */
		    ourtty = 1;
		else if (*buff == '@' && buff[1]) {
		    if (buff[1] == '-') {
			/* tell twin to attach to its tty */
			servtty = 1;
		    } else if (tty) {
			if (!strcmp(buff+1, tty))
			    /* attach twin to our tty */
			    ourtty = 1;
		    } else {
			fprintf(stderr, "%s: ttyname() failed, cannot find controlling tty!\n", MYname);
			return 1;
		    }
		} else {
		    fprintf(stderr, "%s: malformed display hw `%s'\n", MYname, *argv);
		    return 1;
		}
		
		if (s) *s = ',';
		else s = "";
		
		if (ourtty) {
		    buff = getenv("TERM");
		    if (!buff) buff = "";
		    
		    arg = malloc(strlen(tty) + 9 + strlen(s) + (buff ? 6 + strlen(buff) : 0));
		    
		    sprintf(arg, "-hw=tty@%s%s%s%s", tty, (buff ? (byte *)",TERM=" : buff), buff, s);
		} else if (servtty) {
		    arg = malloc(8 + strlen(s));
		    sprintf(arg, "-hw=tty%s", s);
		} else
		    arg = *argv;
	    } else if ((*argv)[4])
		arg = *argv;
	    else {
		Usage(detach);
		return 1;
	    }
	} else {
	    Usage(detach);
	    return 1;
	}
    }
    
    if (detach == 0 && !arg) {
	Usage(detach);
	return 1;
    }
    
    redirect = flags & TW_ATTACH_HW_REDIRECT;

    if (TwCheckMagic(attach_magic) && TwOpen(dpy) && TwCreateMsgPort(8, "Twattach", 0, 0, 0)) do {
	
	if (!VersionsMatch(force)) {
	    if (!force) {
		fprintf(stderr, "%s: Aborting. Use option `-f' to ignore versions check.\n", MYname);
		TwClose();
		return 1;
	    }
	}

	if (detach) {
	    return !TwDetachHW(arg ? strlen(arg) : 0, arg);
	}

	TwAttachHW(arg ? strlen(arg) : 0, arg, flags);
	TwFlush();
	
	signal(SIGWINCH, SignalWinch);
	    
	if (redirect)
	    fprintf(stderr, "reported messages...\n");
	
	for (;;) {
	    buff = TwAttachGetReply(&chunk);
	    if (buff <= (byte *)2) {
		ret = (byte)(size_t)buff;
		break;
	    } else if (buff == (byte *)-1)
		/* libTw panic */
		break;

	    fprintf(stderr, "%.*s", (int)chunk, buff);
	}
	fflush(stderr);
	
	if (TwInPanic())
	    break;

	if (ourtty) {
	    fputs("\033[2J", stdout);
	    fflush(stdout);
	}
	
	/*
	 * twin waits this before grabbing the display...
	 * so we can fflush(stdout) to show all messages
	 * *BEFORE* twin draws on (eventually) the same tty
	 */
	TwAttachConfirm();
    
	if (ret == 2) {
	    /*
	     * twin told us to stay and sit on the display
	     * until it is quitted.
	     */
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
		fprintf(stderr, "... ok, twin successfully attached.\n");
	    else
		fprintf(stderr, "... ach, twin failed to attach.\n");
	}
	return !ret;
    } while (0);
    
    chunk = TwErrno;
    fprintf(stderr, "%s: libTw error: %s%s\n", MYname,
	    TwStrError(chunk), TwStrErrorDetail(chunk, TwErrnoDetail));
    return 1;
}

