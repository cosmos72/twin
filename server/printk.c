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

#include <stdarg.h>

#include "twin.h"
#include "remote.h"
#include "methods.h"
#include "builtin.h"
#include "draw.h"

static byte buf[SMALLBUFF*4]; /* hope it's enough */
static int log_fd = NOFD;

int printk(CONST byte *format, ...) {
    int i = 0;
#if defined(HAVE_VSNPRINTF) || defined(HAVE_VSPRINTF)
    int left, chunk;
    byte *s = buf;
    va_list ap;
    
    va_start(ap, format);
# ifdef HAVE_VSNPRINTF
    i = vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);
# else
    i = vsprintf(buf, format, ap); /* hopefully i < sizeof(buf) */
    va_end(ap);
    
    if (i > sizeof(buf)) {
	fputs("twin: internal error: printk() overflow! \033[1mQUIT NOW !\033[0m\n", stderr);
	return sizeof(buf);
    }
#endif /* HAVE_VSNPRINTF */

#ifdef CONF_PRINTK
    if (MessagesWin->HLogic > SMALLBUFF) {
	while (MessagesWin->HLogic > SMALLBUFF) {
	    Delete(MessagesWin->USE.R.FirstRow);
	    MessagesWin->CurY--;
	}
	if (MessagesWin->Parent)
	    DrawFullWindow2(MessagesWin);
    }
    Act(RowWriteAscii,MessagesWin)(MessagesWin, i, buf);
#endif /* CONF_PRINTK */

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
#endif /* defined(HAVE_VSNPRINTF) || defined(HAVE_VPRINTF) */
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
    log_fd = NOFD;
}

