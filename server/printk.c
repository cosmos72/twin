/*
 *  printk.c -- report messages from twin core.
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "twin.h"
#include "remote.h"
#include "methods.h"
#include "builtin.h"
#include "draw.h"

static byte buf[SMALLBUFF*4]; /* hope it's enough */
static int log_fd = NOFD;

int printk(CONST byte *format, ...) {
    va_list ap;
    byte *s = buf;
    int i, left, chunk;
    
    va_start(ap, format);
    i = vsprintf(buf, format, ap); /* hopefully i < sizeof(buf) */
    va_end(ap);
    
    if (i >= SMALLBUFF*4) {
	fputs("twin: internal error: printk() overflow! \033[1mQUIT NOW !\033[0m\n", stderr);
	return SMALLBUFF*4;
    }

#ifdef CONF_PRINTK
    if (MessagesWin->HLogic > SMALLBUFF) {
	while (MessagesWin->HLogic > SMALLBUFF) {
	    Delete(MessagesWin->USE.R.FirstRow);
	    MessagesWin->CurY--;
	}
	if (MessagesWin->Parent)
	    DrawFullWindow2(MessagesWin);
    }
    Act(WriteRow,MessagesWin)(MessagesWin, i, buf);
#endif

    if (log_fd == NOFD)
	fputs(buf, stderr);
    else {
	s = buf;
	left = i;
	do {
	    do {
		chunk = write(log_fd, s, left);
	    } while (chunk < 0 && errno == EINTR);
	    if (chunk > 0) {
		s += chunk;
		left -= chunk;
	    }
	} while (left > 0 && chunk > 0);
    }
    return i;
}

int flushk(void) {
    return fflush(stderr);
}

byte RegisterPrintk(int fd) {
    if (log_fd == NOFD) {
	log_fd = fd;
	return TRUE;
    }
    return FALSE;
}

void UnRegisterPrintk(void) {
    log_fd = NOSLOT;
}

